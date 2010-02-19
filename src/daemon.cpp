#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <map>

#include "highgui.h"
#include "web_server.h"
#include "otPipeline.h"
#include "otModule.h"
#include "otFactory.h"
#include "otProperty.h"
#include "otDataStream.h"
#include "cJSON.h"

static otPipeline *pipeline = NULL;
static bool running = true;

void web_message(const char *message, int success=1) {
	char *out;
	cJSON *root;

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", success);
	cJSON_AddStringToObject(root, "message", message);
	out = cJSON_Print(root);
	cJSON_Delete(root);

	printf("Content-type: application/json\r\n\r\n");
	printf("%s\r\n", out);

	free(out);
}

void web_error(const char* message) {
	return web_message(message, 0);
}

otModule *module_search(const std::string &id) {
	otModule *module;
	for ( unsigned int i = 0; i < pipeline->size(); i++ ) {
		module = pipeline->getModule(i);
		if ( module->property("id").asString() == id )
			return module;
	}
	return NULL;
}

void web_status() {
	web_message("ok");
}

void web_pipeline_create() {
	otModule *module;

	if ( strlen(ClientInfo->Query((char*)"objectname")) == 0 )
		return web_error("missing objectname");

	module = otFactory::getInstance()->create(ClientInfo->Query((char*)"objectname"));
	if ( module == NULL )
		return web_error("invalid objectname");

	pipeline->addElement(module);

	web_message(module->property("id").asString().c_str());
}

void web_pipeline_status() {
	std::map<std::string, otProperty*>::iterator it;
	char *out, buffer[64];
	cJSON *root, *data, *modules, *mod, *properties, *io, *observers;
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

		for ( it = module->properties.begin(); it != module->properties.end(); it++ ) {
			cJSON_AddStringToObject(properties, it->first.c_str(),
					it->second->asString().c_str());
		}

		if ( module->getInputCount() ) {
			cJSON_AddItemToObject(mod, "inputs", io=cJSON_CreateObject());
			for ( int i = 0; i < module->getInputCount(); i++ ) {
				ds = module->getInput(i);
				cJSON_AddNumberToObject(io, "index", i);
				cJSON_AddStringToObject(io, "name", module->getInputName(i).c_str());
				cJSON_AddStringToObject(io, "type", module->getInputType(i).c_str());
				cJSON_AddNumberToObject(io, "used", ds == NULL ? 0 : 1);
			}
		}

		if ( module->getOutputCount() ) {
			cJSON_AddItemToObject(mod, "outputs", io=cJSON_CreateObject());
			for ( int i = 0; i < module->getOutputCount(); i++ ) {
				ds = module->getOutput(i);
				cJSON_AddNumberToObject(io, "index", i);
				cJSON_AddStringToObject(io, "name", module->getOutputName(i).c_str());
				cJSON_AddStringToObject(io, "type", module->getOutputType(i).c_str());
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

	out = cJSON_Print(root);
	cJSON_Delete(root);

	printf("Content-type: application/json\r\n\r\n");
	printf("%s\r\n", out);
	free(out);
}

void web_factory_list() {
	char *out;
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

	out = cJSON_Print(root);
	cJSON_Delete(root);

	printf("Content-type: application/json\r\n\r\n");
	printf("%s\r\n", out);
	free(out);
}

void web_factory_desribe() {
	std::map<std::string, otProperty*>::iterator it;
	char *out;
	cJSON *root, *mod, *properties, *io;
	otDataStream *ds;
	otModule *module;

	if ( strlen(ClientInfo->Query((char*)"name")) == 0 )
		return web_error("missing name");

	module = otFactory::getInstance()->create(ClientInfo->Query((char*)"name"));
	if ( module == NULL )
		return web_error("invalid name");

	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "success", 1);
	cJSON_AddStringToObject(root, "message", "ok");
	cJSON_AddItemToObject(root, "describe", mod=cJSON_CreateObject());

	cJSON_AddStringToObject(mod, "name", module->getName().c_str());
	cJSON_AddStringToObject(mod, "description", module->getDescription().c_str());
	cJSON_AddStringToObject(mod, "author", module->getAuthor().c_str());
	cJSON_AddNumberToObject(mod, "running", module->isStarted() ? 1 : 0);
	cJSON_AddItemToObject(mod, "properties", properties=cJSON_CreateObject());

	for ( it = module->properties.begin(); it != module->properties.end(); it++ ) {
		cJSON_AddStringToObject(properties, it->first.c_str(),
				it->second->asString().c_str());
	}

	if ( module->getInputCount() ) {
		cJSON_AddItemToObject(mod, "inputs", io=cJSON_CreateObject());
		for ( int i = 0; i < module->getInputCount(); i++ ) {
			ds = module->getInput(i);
			cJSON_AddNumberToObject(io, "index", i);
			cJSON_AddStringToObject(io, "name", module->getInputName(i).c_str());
			cJSON_AddStringToObject(io, "type", module->getInputType(i).c_str());
		}
	}

	if ( module->getOutputCount() ) {
		cJSON_AddItemToObject(mod, "outputs", io=cJSON_CreateObject());
		for ( int i = 0; i < module->getOutputCount(); i++ ) {
			ds = module->getOutput(i);
			cJSON_AddNumberToObject(io, "index", i);
			cJSON_AddStringToObject(io, "name", module->getOutputName(i).c_str());
			cJSON_AddStringToObject(io, "type", module->getOutputType(i).c_str());
		}
	}

	delete module;

	out = cJSON_Print(root);
	cJSON_Delete(root);

	printf("Content-type: application/json\r\n\r\n");
	printf("%s\r\n", out);
	free(out);
}

void web_pipeline_connect() {
	otModule *in, *out;
	int inidx = 0, outidx = 0;

	if ( strlen(ClientInfo->Query((char*)"out")) == 0 )
		return web_error("missing out");

	if ( strlen(ClientInfo->Query((char*)"in")) == 0 )
		return web_error("missing in");

	if ( strlen(ClientInfo->Query((char*)"outidx")) != 0 )
		outidx = atoi(ClientInfo->Query((char*)"outidx"));
	if ( strlen(ClientInfo->Query((char*)"inidx")) != 0 )
		inidx = atoi(ClientInfo->Query((char*)"inidx"));

	in = module_search(ClientInfo->Query((char*)"in"));
	out = module_search(ClientInfo->Query((char*)"out"));

	if ( in == NULL )
		return web_error("in object not found");

	if ( out == NULL )
		return web_error("out object not found");

	in->setInput(out->getOutput(outidx), inidx);

	web_message("ok");
}

void web_pipeline_get() {
	otModule *module;

	if ( strlen(ClientInfo->Query((char*)"objectname")) == 0 )
		return web_error("missing objectname");

	if ( strlen(ClientInfo->Query((char*)"name")) == 0 )
		return web_error("missing name");

	module = module_search(ClientInfo->Query((char*)"objectname"));
	if ( module == NULL )
		return web_error("object not found");

	web_message(module->property(ClientInfo->Query((char*)"name")).asString().c_str());
}

void web_pipeline_set() {
	otModule *module;

	if ( strlen(ClientInfo->Query((char*)"objectname")) == 0 )
		return web_error("missing objectname");

	if ( strlen(ClientInfo->Query((char*)"name")) == 0 )
		return web_error("missing name");

	if ( strlen(ClientInfo->Query((char*)"value")) == 0 )
		return web_error("missing value");

	module = module_search(ClientInfo->Query((char*)"objectname"));
	if ( module == NULL )
		return web_error("object not found");

	module->property(ClientInfo->Query((char*)"name")).set(ClientInfo->Query((char*)"value"));

	web_message("ok");
}

void web_pipeline_start() {
	pipeline->start();
	web_message("ok");
}

void web_pipeline_stop() {
	pipeline->stop();
	web_message("ok");
}

void web_pipeline_quit() {
	web_message("bye");
	running = false;
}

int main(int argc, char **argv) {
	struct web_server server = {0};

	otFactory::init();
	pipeline = new otPipeline();

	web_server_init(&server, 7500, NULL, 0);
	web_server_addhandler(&server, "* /factory/list", web_factory_list, 0);
	web_server_addhandler(&server, "* /factory/describe", web_factory_desribe, 0);
	web_server_addhandler(&server, "* /pipeline/create", web_pipeline_create, 0);
	web_server_addhandler(&server, "* /pipeline/status", web_pipeline_status, 0);
	web_server_addhandler(&server, "* /pipeline/connect", web_pipeline_connect, 0);
	web_server_addhandler(&server, "* /pipeline/set", web_pipeline_set, 0);
	web_server_addhandler(&server, "* /pipeline/get", web_pipeline_get, 0);
	web_server_addhandler(&server, "* /pipeline/start", web_pipeline_start, 0);
	web_server_addhandler(&server, "* /pipeline/stop", web_pipeline_stop, 0);
	web_server_addhandler(&server, "* /pipeline/quit", web_pipeline_quit, 0);

	while ( running ) {
		cvWaitKey(2);
		if ( pipeline->isStarted() )
			pipeline->update();
		web_server_run(&server);
	}

	delete pipeline;
}
