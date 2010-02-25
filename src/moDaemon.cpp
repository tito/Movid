#include <iostream>

#include "moDaemon.h"

#include "moFactory.h"

static moDaemon *instance = NULL;

moDaemon::moDaemon() {
}

moDaemon::~moDaemon() {
}

moDaemon *moDaemon::getInstance() {
	if ( instance == NULL )
		instance = new moDaemon();
	return instance;
}

void moDaemon::init() {
	moFactory::init();
}

void moDaemon::cleanup() {
}

