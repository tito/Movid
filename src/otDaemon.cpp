#include "otDaemon.h"

#include "otFactory.h"
#include "otCamera.h"
#include "otImageDisplayModule.h"

static otDaemon *instance = NULL;

otDaemon::otDaemon() {
}

otDaemon::~otDaemon() {
}

otDaemon *otDaemon::getInstance() {
	if ( instance == NULL )
		instance = new otDaemon();
	return instance;
}

void otDaemon::init() {
}

void otDaemon::cleanup() {
}

