#include <sys/queue.h>
#include <signal.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <map>

// jpeg !
#include <jpeglib.h>

// opencv (for cvWaitKey)
#include "cv.h"
#include "highgui.h"

// JSON
#include "cJSON.h"

// opentracker
#include "otPipeline.h"
#include "otModule.h"
#include "otFactory.h"
#include "otProperty.h"
#include "otDataStream.h"

// libevent
#include "event.h"
#include "evhttp.h"

#define OT_DAEMON "daemon"

static otPipeline *pipeline = NULL;
static bool running = true;
static struct event_base *base = NULL;
static bool config_httpserver = true;
static std::string config_pipelinefn = "";
static struct evhttp *server = NULL;


class otStreamModule : public otModule {
public:
	otStreamModule() : otModule(OT_MODULE_INPUT, 1, 0) {
		this->input = new otDataStream("stream");
		this->output_buffer = NULL;
		this->need_update = false;
		this->properties["id"] = new otProperty(otModule::createId("WebStream"));
		this->properties["scale"] = new otProperty(1);
	}

	void stop() {
		if ( this->output_buffer != NULL ) {
			cvReleaseImage(&this->output_buffer);
			this->output_buffer = NULL;
		}
		otModule::stop();
	}

	void notifyData(otDataStream *source) {
		IplImage* src = (IplImage*)(this->input->getData());
		if ( src == NULL )
			return;
		if ( this->output_buffer == NULL ) {
			CvSize size = cvGetSize(src);
			size.width /= this->property("scale").asInteger();
			size.height /= this->property("scale").asInteger();
			this->output_buffer = cvCreateImage(size, src->depth, src->nChannels);
		}
		this->need_update = true;
	}

	void setInput(otDataStream* stream, int n=0) {
		if ( this->input != NULL )
			this->input->removeObserver(this);
		this->input = stream;
		if ( this->input != NULL )
			this->input->addObserver(this);
	}

	virtual otDataStream *getInput(int n=0) {
		return this->input;
	}

	virtual otDataStream *getOutput(int n=0) {
		return NULL;
	}

	void copy() {
		if ( this->output_buffer == NULL || this->input == NULL )
			return;
		this->input->lock();
		IplImage* src = (IplImage*)(this->input->getData());
		if ( src == NULL || src->imageData == NULL )
			return;
		if ( this->property("scale").asInteger() == 1 )
			cvCopy(src, this->output_buffer);
		else
			cvResize(src, this->output_buffer);
		this->input->unlock();
	}

	virtual void update() {}
	virtual std::string getName() { return "Stream"; }
	virtual std::string getDescription() { return ""; }
	virtual std::string getAuthor() { return ""; }
	bool need_update;

	otDataStream *input;
	IplImage* output_buffer;
};



bool ipl2jpeg(IplImage *frame, unsigned char **outbuffer, long unsigned int *outlen) {
	unsigned char *outdata = (uchar *) frame->imageData;
	struct jpeg_compress_struct cinfo = {0};
	struct jpeg_error_mgr jerr;
	JSAMPROW row_ptr[1];
	int row_stride;

	*outbuffer = NULL;
	*outlen = 0;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_mem_dest(&cinfo, outbuffer, outlen);

	cinfo.image_width = frame->width;
	cinfo.image_height = frame->height;
	cinfo.input_components = frame->nChannels;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = frame->width * frame->nChannels;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_ptr[0] = &outdata[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_ptr, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	return true;

}


//
// WEB CALLBACKS
//

void web_json(struct evhttp_request *req, cJSON *root) {
	struct evbuffer *evb = evbuffer_new();
	char *out;

	out = cJSON_Print(root);
	cJSON_Delete(root);

	evbuffer_add(evb, out, strlen(out));
	evhttp_add_header(req->output_headers, "Content-Type", "application/json");
	evhttp_send_reply(req, HTTP_OK, "Everything is fine", evb);
	evbuffer_free(evb);

	free(out);
}

void web_message(struct evhttp_request *req, const char *message, int success=1) {
	cJSON *root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", success);
	cJSON_AddStringToObject(root, "message", message);
	web_json(req, root);
}

void web_error(struct evhttp_request *req, const char* message) {
	return web_message(req, message, 0);
}

void web_status(struct evhttp_request *req, void *arg) {
	web_message(req, "ok");
}

otModule *module_search(const std::string &id, otPipeline *pipeline) {
	otModule *module;
	for ( unsigned int i = 0; i < pipeline->size(); i++ ) {
		module = pipeline->getModule(i);
		if ( module->property("id").asString() == id )
			return module;
	}
	return NULL;
}


struct chunk_req_state {
	struct evhttp_request *req;
	otStreamModule *stream;
	int i;
	bool closed;
};

static void web_pipeline_stream_close(struct evhttp_connection *conn, void *arg) {
	struct chunk_req_state *state = static_cast<chunk_req_state*>(arg);
	state->closed = true;
}

static void web_pipeline_stream_trickle(int fd, short events, void *arg)
{
	struct evbuffer *evb = NULL;
	struct chunk_req_state *state = static_cast<chunk_req_state*>(arg);
	struct timeval when = { 0, 20 };
	long unsigned int outlen;
	unsigned char *outbuf;

	if ( state->closed ) {
		// free !
		state->stream->setInput(NULL);
		delete state->stream;
		free(state);
		return;
	}

	if ( state->stream->need_update == false ) {
		event_once(-1, EV_TIMEOUT, web_pipeline_stream_trickle, state, &when);
		return;
	}

	state->stream->copy();
	state->stream->need_update = false;
	cvCvtColor(state->stream->output_buffer, state->stream->output_buffer, CV_BGR2RGB);

	ipl2jpeg(state->stream->output_buffer, &outbuf, &outlen);

	evb = evbuffer_new();
	evbuffer_add_printf(evb, "--mjpegstream\r\n");
	evbuffer_add_printf(evb, "Content-Type: image/jpeg\r\n");
	evbuffer_add_printf(evb, "Content-Length: %lu\r\n\r\n", outlen);
	evbuffer_add(evb, outbuf, outlen);
	evhttp_send_reply_chunk(state->req, evb);
	evbuffer_free(evb);

	free(outbuf);

	event_once(-1, EV_TIMEOUT, web_pipeline_stream_trickle, state, &when);
	/**
		evhttp_send_reply_end(state->req);
		free(state);
	**/
}

void web_pipeline_stream(struct evhttp_request *req, void *arg) {
	struct timeval when = { 0, 20 };
	struct evkeyvalq headers;
	const char *uri;
	int	idx = 0;
	otModule *module = NULL;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = module_search(evhttp_find_header(&headers, "objectname"), pipeline);
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	if ( evhttp_find_header(&headers, "index") != NULL )
		idx = atoi(evhttp_find_header(&headers, "index"));

	struct chunk_req_state *state = (struct chunk_req_state*)malloc(sizeof(struct chunk_req_state));

	memset(state, 0, sizeof(struct chunk_req_state));
	state->req = req;
	state->closed = false;
	state->stream = new otStreamModule();

	if ( evhttp_find_header(&headers, "scale") != NULL )
		state->stream->property("scale").set(evhttp_find_header(&headers, "scale"));


	state->stream->setInput(module->getOutput(idx));

	evhttp_add_header(req->output_headers, "Content-Type", "multipart/x-mixed-replace; boundary=mjpegstream");
	evhttp_send_reply_start(req, HTTP_OK, "Everything is fine");

	evhttp_connection_set_closecb(req->evcon, web_pipeline_stream_close, state);

	event_once(-1, EV_TIMEOUT, web_pipeline_stream_trickle, state, &when);

}

void web_pipeline_create(struct evhttp_request *req, void *arg) {
	otModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = otFactory::getInstance()->create(evhttp_find_header(&headers, "objectname"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "invalid objectname");
	}

	pipeline->addElement(module);

	evhttp_clear_headers(&headers);
	web_message(req, module->property("id").asString().c_str());
}

void web_pipeline_status(struct evhttp_request *req, void *arg) {
	std::map<std::string, otProperty*>::iterator it;
	char buffer[64];
	cJSON *root, *data, *modules, *mod, *properties, *io, *observers, *array;
	otDataStream *ds;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "status", data=cJSON_CreateObject());
	cJSON_AddNumberToObject(data, "size", pipeline->size());
	cJSON_AddNumberToObject(data, "running", pipeline->isStarted() ? 1 : 0);
	cJSON_AddItemToObject(data, "modules", modules=cJSON_CreateObject());

	for ( unsigned int i = 0; i < pipeline->size(); i++ ) {
		otModule *module = pipeline->getModule(i);
		assert( module != NULL );

		cJSON_AddItemToObject(modules,
			module->property("id").asString().c_str(),
			mod=cJSON_CreateObject());

		cJSON_AddStringToObject(mod, "name", module->getName().c_str());
		cJSON_AddStringToObject(mod, "description", module->getDescription().c_str());
		cJSON_AddStringToObject(mod, "author", module->getAuthor().c_str());
		cJSON_AddNumberToObject(mod, "running", module->isStarted() ? 1 : 0);
		cJSON_AddItemToObject(mod, "properties", properties=cJSON_CreateObject());

		for ( it = module->getProperties().begin(); it != module->getProperties().end(); it++ ) {
			cJSON_AddStringToObject(properties, it->first.c_str(),
					it->second->asString().c_str());
		}

		if ( module->getInputCount() ) {
			cJSON_AddItemToObject(mod, "inputs", array=cJSON_CreateArray());
			for ( int i = 0; i < module->getInputCount(); i++ ) {
				ds = module->getInput(i);
				cJSON_AddItemToArray(array, io=cJSON_CreateObject());
				cJSON_AddNumberToObject(io, "index", i);
				cJSON_AddStringToObject(io, "name", module->getInputInfos(i)->getName().c_str());
				cJSON_AddStringToObject(io, "type", module->getInputInfos(i)->getType().c_str());
				cJSON_AddNumberToObject(io, "used", ds == NULL ? 0 : 1);
			}
		}

		if ( module->getOutputCount() ) {
			cJSON_AddItemToObject(mod, "outputs", array=cJSON_CreateArray());
			for ( int i = 0; i < module->getOutputCount(); i++ ) {
				ds = module->getOutput(i);
				cJSON_AddItemToArray(array, io=cJSON_CreateObject());
				cJSON_AddNumberToObject(io, "index", i);
				cJSON_AddStringToObject(io, "name", module->getOutputInfos(i)->getName().c_str());
				cJSON_AddStringToObject(io, "type", module->getOutputInfos(i)->getType().c_str());
				cJSON_AddNumberToObject(io, "used", ds == NULL ? 0 : 1);
				cJSON_AddItemToObject(io, "observers", observers=cJSON_CreateObject());
				if ( ds != NULL ) {
					for ( unsigned int j = 0; j < ds->getObserverCount(); j++ ) {
						snprintf(buffer, sizeof(buffer), "%d", j);
						cJSON_AddStringToObject(observers, buffer,
							ds->getObserver(j)->property("id").asString().c_str());
					}
				}
			}
		}
	}

	web_json(req, root);
}

void web_factory_list(struct evhttp_request *req, void *arg) {
	std::vector<std::string>::iterator it;
	std::vector<std::string> list = otFactory::getInstance()->list();
	cJSON *root, *data;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "list", data=cJSON_CreateArray());

	for ( it = list.begin(); it != list.end(); it++ ) {
		cJSON_AddItemToArray(data, cJSON_CreateString(it->c_str()));
	}

	web_json(req, root);
}

void web_factory_desribe(struct evhttp_request *req, void *arg) {
	std::map<std::string, otProperty*>::iterator it;
	cJSON *root, *mod, *properties, *io, *array;
	otDataStream *ds;
	otModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "name") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing name");
	}

	module = otFactory::getInstance()->create(evhttp_find_header(&headers, "name"));
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "invalid name");
	}

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "describe", mod=cJSON_CreateObject());

	cJSON_AddStringToObject(mod, "name", module->getName().c_str());
	cJSON_AddStringToObject(mod, "description", module->getDescription().c_str());
	cJSON_AddStringToObject(mod, "author", module->getAuthor().c_str());
	cJSON_AddNumberToObject(mod, "running", module->isStarted() ? 1 : 0);
	cJSON_AddItemToObject(mod, "properties", properties=cJSON_CreateObject());

	for ( it = module->getProperties().begin(); it != module->getProperties().end(); it++ ) {
		cJSON_AddStringToObject(properties, it->first.c_str(),
				it->second->asString().c_str());
	}

	if ( module->getInputCount() ) {
		cJSON_AddItemToObject(mod, "inputs", array=cJSON_CreateArray());
		for ( int i = 0; i < module->getInputCount(); i++ ) {
			ds = module->getInput(i);
			cJSON_AddItemToArray(array, io=cJSON_CreateObject());
			cJSON_AddNumberToObject(io, "index", i);
			cJSON_AddStringToObject(io, "name", module->getInputInfos(i)->getName().c_str());
			cJSON_AddStringToObject(io, "type", module->getInputInfos(i)->getType().c_str());
		}
	}

	if ( module->getOutputCount() ) {
		cJSON_AddItemToObject(mod, "outputs", array=cJSON_CreateArray());
		for ( int i = 0; i < module->getOutputCount(); i++ ) {
			ds = module->getOutput(i);
			cJSON_AddItemToArray(array, io=cJSON_CreateObject());
			cJSON_AddNumberToObject(io, "index", i);
			cJSON_AddStringToObject(io, "name", module->getOutputInfos(i)->getName().c_str());
			cJSON_AddStringToObject(io, "type", module->getOutputInfos(i)->getType().c_str());
		}
	}

	delete module;

	evhttp_clear_headers(&headers);
	web_json(req, root);
}

void web_pipeline_connect(struct evhttp_request *req, void *arg) {
	otModule *in, *out;
	int inidx = 0, outidx = 0;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "out") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing out");
	}

	if ( evhttp_find_header(&headers, "in") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing in");
	}

	if ( evhttp_find_header(&headers, "outidx") != NULL )
		outidx = atoi(evhttp_find_header(&headers, "outidx"));
	if ( evhttp_find_header(&headers, "inidx") != NULL )
		inidx = atoi(evhttp_find_header(&headers, "inidx"));

	in = module_search(evhttp_find_header(&headers, "in"), pipeline);
	out = module_search(evhttp_find_header(&headers, "out"), pipeline);

	if ( in == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "in object not found");
	}

	if ( out == NULL && strcmp(evhttp_find_header(&headers, "out"), "NULL") != 0 ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "out object not found");
	}

	if ( strcmp(evhttp_find_header(&headers, "out"), "NULL") == 0 )
		in->setInput(NULL, inidx);
	else
		in->setInput(out->getOutput(outidx), inidx);

	evhttp_clear_headers(&headers);
	web_message(req, "ok");
}

void web_pipeline_get(struct evhttp_request *req, void *arg) {
	otModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	if ( evhttp_find_header(&headers, "name") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing name");
	}

	module = module_search(evhttp_find_header(&headers, "objectname"), pipeline);
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	web_message(req, module->property(evhttp_find_header(&headers, "name")).asString().c_str());
	evhttp_clear_headers(&headers);
}

void web_pipeline_set(struct evhttp_request *req, void *arg) {
	otModule *module;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	if ( evhttp_find_header(&headers, "name") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing name");
	}

	if ( evhttp_find_header(&headers, "value") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing value");
	}

	module = module_search(evhttp_find_header(&headers, "objectname"), pipeline);
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	module->property(evhttp_find_header(&headers, "name")).set(
		evhttp_find_header(&headers, "value"));

	evhttp_clear_headers(&headers);
	web_message(req, "ok");
}

void web_pipeline_remove(struct evhttp_request *req, void *arg) {
	otModule *module;
	otDataStream *ds;
	struct evkeyvalq headers;
	const char *uri;

	uri = evhttp_request_uri(req);
	evhttp_parse_query(uri, &headers);

	if ( evhttp_find_header(&headers, "objectname") == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "missing objectname");
	}

	module = module_search(evhttp_find_header(&headers, "objectname"), pipeline);
	if ( module == NULL ) {
		evhttp_clear_headers(&headers);
		return web_error(req, "object not found");
	}

	pipeline->stop();
	module->stop();

	// disconnect inputs
	if ( module->getInputCount() ) {
		for ( int i = 0; i < module->getInputCount(); i++ ) {
			ds = module->getInput(i);
			if ( ds == NULL )
				continue;
			ds->removeObserver(module);
		}
	}

	// disconnect output
	if ( module->getOutputCount() ) {
		for ( int i = 0; i < module->getOutputCount(); i++ ) {
			ds = module->getOutput(i);
			if ( ds == NULL )
				continue;
			ds->removeObservers();
		}
	}

	// remove element from pipeline
	pipeline->removeElement(module);

	delete module;

	web_message(req, "ok");
	evhttp_clear_headers(&headers);
}

void web_pipeline_start(struct evhttp_request *req, void *arg) {
	pipeline->start();
	web_message(req, "ok");
}

void web_pipeline_stop(struct evhttp_request *req, void *arg) {
	pipeline->stop();
	web_message(req, "ok");
}

void web_pipeline_quit(struct evhttp_request *req, void *arg) {
	web_message(req, "bye");
	running = false;
}

void web_file(struct evhttp_request *req, void *arg) {
	assert( arg != NULL );
	long filesize = 0;
	struct evbuffer *evb;
	char *filename = (char *)arg,
		 *buf;
	FILE *fd;

	fd = fopen(filename, "r");
	if ( fd == NULL ) {
		web_error(req, "gui file not found");
		return;
	}

	fseek(fd, 0, SEEK_END);
	filesize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	buf = (char*)malloc(filesize);
	if ( buf == NULL ) {
		fclose(fd);
		web_error(req, "memory error");
		return;
	}

	fread(buf, filesize, 1, fd);
	fclose(fd);

	if ( strncmp(filename + strlen(filename) - 2, "js", 2) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "application/javascript");
	else if ( strncmp(filename + strlen(filename) - 3, "css", 3) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "text/css");
	else if ( strncmp(filename + strlen(filename) - 3, "png", 3) == 0 )
		evhttp_add_header(req->output_headers, "Content-Type", "image/png");
	else
		evhttp_add_header(req->output_headers, "Content-Type", "text/html");

	evb = evbuffer_new();
	evbuffer_add(evb, buf, filesize);

	evhttp_send_reply(req, HTTP_OK, "Everything is fine", evb);
	evbuffer_free(evb);
	free(buf);
}

// pipeline create objectname id
// pipeline set id key value
// pipeline connect out_id out_idx in_id in_idx
otPipeline *pipeline_parse_file(const std::string &filename) {
	otPipeline *pipeline = NULL;
	otModule *module1, *module2;
	std::string line;
	int ln = 0;
	int inidx, outidx;
	std::ifstream f(filename.c_str());

	if ( !f.is_open() )
		return NULL;

	pipeline = new otPipeline();

	while ( !f.eof() )
	{
		ln ++;
		getline(f, line);
		if ( line == "" )
			continue;
		if ( line[0] == '#' )
			continue;

		std::istringstream iss(line);
		std::vector<std::string> tokens;

		std::copy(std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>(),
				std::back_inserter<std::vector<std::string> >(tokens));

		std::cout << "LINE: tokens=" << tokens.size() << ", line=<" << line << ">" << std::endl;

		if ( tokens.size() <= 1 ) {
			std::cerr << "Error at line " << ln << ": invalid line command" << std::endl;
			goto parse_error;
		}
		if ( tokens[0] == "pipeline" ) {
			if ( tokens.size() < 2 ) {
				std::cerr << "Error at line " << ln << ": not enough parameters" << std::endl;
				goto parse_error;
			}

			if ( tokens[1] == "create" ) {
				if ( tokens.size() != 4 ) {
					std::cerr << "Error at line " << ln << ": not enough parameters" << std::endl;
					goto parse_error;
				}

				module1 = module_search(tokens[3], pipeline);
				if ( module1 != NULL ) {
					std::cerr << "Error at line " << ln << ": id already used" << std::endl;
					goto parse_error;
				}

				module1 = otFactory::getInstance()->create(tokens[2]);
				if ( module1 == NULL ) {
					std::cerr << "Error at line " << ln << ": unknown module " << tokens[2] << std::endl;
					goto parse_error;
				}

				module1->property("id").set(tokens[3]);

				pipeline->addElement(module1);

			} else if ( tokens[1] == "set" ) {
				if ( tokens.size() != 5 ) {
					std::cerr << "Error at line " << ln << ": not enough parameters" << std::endl;
					goto parse_error;
				}

				module1 = module_search(tokens[2], pipeline);
				if ( module1 == NULL ) {
					std::cerr << "Error at line " << ln << ": unable to found module with id " << tokens[2] << std::endl;
					goto parse_error;
				}

				module1->property(tokens[3]).set(tokens[4]);

			} else if ( tokens[1] == "connect" ) {
				if ( tokens.size() != 6 ) {
					std::cerr << "Error at line " << ln << ": not enough parameters" << std::endl;
					goto parse_error;
				}

				module1 = module_search(tokens[2], pipeline);
				if ( module1 == NULL ) {
					std::cerr << "Error at line " << ln << ": unable to found module with id " << tokens[2] << std::endl;
					goto parse_error;
				}

				module2 = module_search(tokens[4], pipeline);
				if ( module2 == NULL ) {
					std::cerr << "Error at line " << ln << ": unable to found module with id " << tokens[4] << std::endl;
					goto parse_error;
				}

				outidx = atoi(tokens[3].c_str());
				inidx = atoi(tokens[5].c_str());

				module2->setInput(module1->getOutput(outidx), inidx);

			} else {
				std::cerr << "Error at line " << ln << ": unknown pipeline subcommand: " << tokens[1] << std::endl;
				goto parse_error;
			}
		} else {
			std::cerr << "Error at line " << ln << ": unknown keyword: " << tokens[0] << std::endl;
			goto parse_error;
		}
	}

	pipeline->start();
	return pipeline;

parse_error:;
	delete pipeline;
	return NULL;
}

void usage(void) {
	printf("Usage: %s [options...]                                \n" \
		   "                                                      \n" \
		   "  -n                     No webserver                 \n" \
		   "  -l <filename>          Read a pipeline from filename\n",
		   OT_DAEMON
	);
}

int parse_options(int *argc, char ***argv) {
	int ch;
	while ( (ch = getopt(*argc, *argv, "l:n")) != -1 ) {
		switch ( ch ) {
			case 'n':
				config_httpserver = false;
				break;
			case 'l':
				config_pipelinefn = std::string(optarg);
				break;
			case '?':
			default:
				usage();
				break;
		}
	}

	(*argc) -= optind;
	(*argv) -= optind;

	return 0;
}

int main(int argc, char **argv) {

	if ( parse_options(&argc, &argv) < 0 )
		return 1;

	otFactory::init();

	if ( config_pipelinefn != "" ) {
		pipeline = pipeline_parse_file(config_pipelinefn);
		if ( pipeline == NULL ) {
			return 2;
		}
	} else if ( config_httpserver == false ) {
		std::cerr << "ERROR : no pipeline or webserver to start !" << std::endl;
		return 3;
	}

	// no default pipeline ? create one !
	if ( pipeline == NULL )
		pipeline = new otPipeline();

	if ( config_httpserver ) {

		signal(SIGPIPE, SIG_IGN);

		base = event_init();
		server = evhttp_new(NULL);

		evhttp_bind_socket(server, "127.0.0.1", 7500);

		evhttp_set_cb(server, "/factory/list", web_factory_list, NULL);
		evhttp_set_cb(server, "/factory/describe", web_factory_desribe, NULL);
		evhttp_set_cb(server, "/pipeline/create", web_pipeline_create, NULL);
		evhttp_set_cb(server, "/pipeline/remove", web_pipeline_remove, NULL);
		evhttp_set_cb(server, "/pipeline/status", web_pipeline_status, NULL);
		evhttp_set_cb(server, "/pipeline/connect", web_pipeline_connect, NULL);
		evhttp_set_cb(server, "/pipeline/set", web_pipeline_set, NULL);
		evhttp_set_cb(server, "/pipeline/get", web_pipeline_get, NULL);
		evhttp_set_cb(server, "/pipeline/stream", web_pipeline_stream, NULL);
		evhttp_set_cb(server, "/pipeline/start", web_pipeline_start, NULL);
		evhttp_set_cb(server, "/pipeline/stop", web_pipeline_stop, NULL);
		evhttp_set_cb(server, "/pipeline/quit", web_pipeline_quit, NULL);

		evhttp_set_cb(server, "/gui/index.html", web_file, (void*)"gui/html/index.html");
		evhttp_set_cb(server, "/gui/jquery.js", web_file, (void*)"gui/html/jquery.js");
		evhttp_set_cb(server, "/gui/ot.js", web_file, (void*)"gui/html/ot.js");
		evhttp_set_cb(server, "/gui/gui.css", web_file, (void*)"gui/html/gui.css");
		evhttp_set_cb(server, "/gui/nostream.png", web_file, (void*)"gui/html/nostream.png");
	}

	while ( running ) {
		// FIXME remove this hack !!!
		cvWaitKey(5);

		// update pipeline
		if ( pipeline->isStarted() )
			pipeline->update();

		// got a server, update
		if ( server != NULL )
			event_base_loop(base, EVLOOP_ONCE|EVLOOP_NONBLOCK);
	}

	delete pipeline;
}
