#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "highgui.h"
#include "web_server.h"
#include "otPipeline.h"
#include "otModule.h"
#include "otFactory.h"

static otPipeline *pipeline = NULL;
static bool running = true;

void web_status() {
	printf("Content-type: text/plain\r\n\r\n");
	printf("Running OTDaemon.\r\n");
};

otModule *module_search(const std::string &id) {
	otModule *module;
	for ( int i = 0; i < pipeline->size(); i++ ) {
		module = pipeline->getModule(i);
		if ( module->property("id").asString() == id )
			return module;
	}
	return NULL;
}

void web_pipeline_create() {
	otModule *module;
	printf("Content-type: text/plain\r\n\r\n");

	if ( strlen(ClientInfo->Query("objectname")) == 0 ) {
		printf("error:no objectname !\r\n");
		return;
	}

	module = otFactory::getInstance()->create(ClientInfo->Query("objectname"));
	if ( module == NULL ) {
		printf("error:invalid objectname\r\n");
		return;
	}

	pipeline->addElement(module);

	printf("%s\r\n", module->property("id").asString().c_str());
}

void web_pipeline_start() {
	pipeline->start();
	printf("Content-type: text/plain\r\n\r\n");
	printf("ok\r\n");
}

void web_pipeline_stop() {
	pipeline->stop();
	printf("Content-type: text/plain\r\n\r\n");
	printf("ok\r\n");
}

void web_pipeline_status() {
	printf("Content-type: text/plain\r\n\r\n");
	printf("%s\r\n", pipeline->isStarted() ? "running" : "stopped");
}

void web_pipeline_connect() {
	otModule *in, *out;
	int inidx = 0, outidx = 0;
	printf("Content-type: text/plain\r\n\r\n");

	if ( strlen(ClientInfo->Query("out")) == 0 ) {
		printf("error:no out property !\r\n");
		return;
	}

	if ( strlen(ClientInfo->Query("in")) == 0 ) {
		printf("error:no in property !\r\n");
		return;
	}

	if ( strlen(ClientInfo->Query("outidx")) != 0 )
		outidx = atoi(ClientInfo->Query("outidx"));
	if ( strlen(ClientInfo->Query("inidx")) != 0 )
		inidx = atoi(ClientInfo->Query("inidx"));

	in = module_search(ClientInfo->Query("in"));
	out = module_search(ClientInfo->Query("out"));

	if ( in == NULL ) {
		printf("error:unable to found in object\r\n");
		return;
	}

	if ( out == NULL ) {
		printf("error:unable to found out object\r\n");
		return;
	}

	in->setInput(out->getOutput(outidx), inidx);
	printf("ok\r\n");
}

void web_pipeline_quit() {
	printf("Content-type: text/plain\r\n\r\n");
	printf("bye\r\n\r\n");
	running = false;
}

void web_pipeline_get() {
	otModule *module;

	if ( strlen(ClientInfo->Query("objectname")) == 0 ) {
		printf("error:no objectname !\r\n");
		return;
	}

	if ( strlen(ClientInfo->Query("name")) == 0 ) {
		printf("error:no name !\r\n");
		return;
	}

	module = module_search(ClientInfo->Query("objectname"));
	if ( module == NULL ) {
		printf("error:unable to found out object\r\n");
		return;
	}

	printf("%s\r\n", module->property(ClientInfo->Query("name")).asString().c_str());
}

void web_pipeline_set() {
	otModule *module;

	if ( strlen(ClientInfo->Query("objectname")) == 0 ) {
		printf("error:no objectname !\r\n");
		return;
	}

	if ( strlen(ClientInfo->Query("name")) == 0 ) {
		printf("error:no name !\r\n");
		return;
	}

	if ( strlen(ClientInfo->Query("value")) == 0 ) {
		printf("error:no value !\r\n");
		return;
	}

	module = module_search(ClientInfo->Query("objectname"));
	if ( module == NULL ) {
		printf("error:unable to found out object\r\n");
		return;
	}

	module->property(ClientInfo->Query("name")).set(ClientInfo->Query("value"));
	printf("ok\r\n");
}

int main(int argc, char **argv) {
	struct web_server server = {0};

	otFactory::init();
	pipeline = new otPipeline();

	web_server_init(&server, 7500, NULL, 0);
	web_server_addhandler(&server, "* /pipeline/create", web_pipeline_create, 0);
	web_server_addhandler(&server, "* /pipeline/start", web_pipeline_start, 0);
	web_server_addhandler(&server, "* /pipeline/stop", web_pipeline_stop, 0);
	web_server_addhandler(&server, "* /pipeline/status", web_pipeline_status, 0);
	web_server_addhandler(&server, "* /pipeline/connect", web_pipeline_connect, 0);
	web_server_addhandler(&server, "* /pipeline/set", web_pipeline_set, 0);
	web_server_addhandler(&server, "* /pipeline/get", web_pipeline_get, 0);
	web_server_addhandler(&server, "* /pipeline/quit", web_pipeline_quit, 0);

	while ( running ) {
		cvWaitKey(2);
		if ( pipeline->isStarted() )
			pipeline->update();
		web_server_run(&server);
	}

	delete pipeline;
}
