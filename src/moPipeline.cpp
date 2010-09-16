/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.  All rights reserved.
 **
 ** This file is part of the Movid Software.
 **
 ** This file may be distributed under the terms of the Q Public License
 ** as defined by Trolltech AS of Norway and appearing in the file
 ** LICENSE included in the packaging of this file.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** Contact info@movid.org if any conditions of this licensing are
 ** not clear to you.
 **
 **********************************************************************/


//
// moPipeline.cpp
//
// Handle a group of object (pipeline)
//

#include <ctime>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <valarray>
#include <list>
#include "moPipeline.h"
#include "moFactory.h"
#include "moLog.h"

LOG_DECLARE("Pipeline");

MODULE_DECLARE_EX(Pipeline,, "native", "Handle object list");

// TODO: move to another file
extern int g_config_delay;

moPipeline::moPipeline() : moModule(MO_MODULE_NONE) {
	MODULE_INIT();
}

moPipeline::~moPipeline() {
	std::vector<moModule *>::iterator it = this->modules.begin();
	while ( it != this->modules.end() ) {
		delete *it;
		this->modules.erase(it);
	}
}

moModule *moPipeline::firstModule() {
	assert( this->modules.size() > 0 );
	return this->modules[0];
}

moModule *moPipeline::lastModule() {
	assert( this->modules.size() > 0 );
	return this->modules[this->modules.size() - 1];
}

void moPipeline::addElement(moModule *module) {
	assert( module != NULL );
	LOG(MO_TRACE, "add <" << module->property("id").asString() << "> to <" \
		<< this->property("id").asString() << ">");
	module->owner = this;
	this->modules.push_back(module);
}

void moPipeline::removeElement(moModule *module) {
	std::vector<moModule *>::iterator it;
	LOG(MO_TRACE, "remove <" << module->property("id").asString() << "> from <" \
		<< this->property("id").asString() << ">");
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( *it == module ) {
			this->modules.erase(it);
			break;
		}
	}
}

void moPipeline::setInput(moDataStream* stream, int n) {
	this->firstModule()->setInput(stream, n);
}

moDataStream* moPipeline::getInput(int n) {
	return this->lastModule()->getInput(n);
}

moDataStream* moPipeline::getOutput(int n) {
	return this->lastModule()->getOutput(n);
}

int moPipeline::getInputCount() {
	return this->firstModule()->getInputCount();
}

int moPipeline::getOutputCount() {
	return this->lastModule()->getOutputCount();
}

moDataStreamInfo *moPipeline::getInputInfos(int n) {
	return this->firstModule()->getInputInfos(n);
}

moDataStreamInfo *moPipeline::getOutputInfos(int n) {
	return this->lastModule()->getOutputInfos(n);
}

void moPipeline::start() {
	std::vector<moModule *>::iterator it;

	moModule::start();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->start();
	}
}

void moPipeline::stop() {
	std::vector<moModule *>::iterator it;

	moModule::stop();

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->stop();
	}
}

void moPipeline::update() {
	// nothing done in pipeline
	return;
}

void moPipeline::poll() {
	std::vector<moModule *>::iterator it;

	LOGM(MO_TRACE, "poll");

	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		(*it)->poll();
	}
}

unsigned int moPipeline::size() {
	return this->modules.size();
}

moModule *moPipeline::getModule(unsigned int index) {
	assert( index >= 0 );
	assert( index < this->size() );

	return this->modules[index];
}

moModule *moPipeline::getModuleById(const std::string& id) {
	moModule *module;
	for ( unsigned int i = 0; i < this->size(); i++ ) {
		module = this->getModule(i);
		if ( module->property("id").asString() == id )
			return module;
	}
	return NULL;
}

void moPipeline::setGroup(bool group) {
	this->is_group = group;
}

bool moPipeline::isGroup() {
	return this->is_group;
}

bool moPipeline::isPipeline() {
	return true;
}

bool moPipeline::haveError() {
	std::vector<moModule *>::iterator it;
	if ( last_internal_error != "" )
		return true;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( (*it)->haveError() )
			return true;
	}
	return false;
}

std::string moPipeline::getLastError() {
	std::vector<moModule *>::iterator it;
	if ( last_internal_error != "" )
		return last_internal_error;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ ) {
		if ( (*it)->haveError() )
			return (*it)->getLastError();
	}
	return "";
}

// pipeline create objectname id
// pipeline set id key value
// pipeline connect out_id out_idx in_id in_idx

#define PIPELINE_PARSE_ERROR(x) do { \
	LOG(MO_ERROR, __LINE__ << "] Error at line " << line_idx << ": " << x); \
	return false; \
} while(0);

bool moPipeline::parse(const std::string& filename) {
	moModule *module1, *module2;
	std::string line;
	int line_idx = 0;
	int inidx, outidx;
	std::ifstream f(filename.c_str());

	// ensure that the file is open
	if ( !f.is_open() ) {
		LOG(MO_ERROR, "unable to open file <" << filename << ">");
		this->last_internal_error = "unable to open file";
		return false;
	}

	bool datamode = false;
	std::vector< std::valarray<std::string> > data_lateset;
	std::vector< std::valarray<std::string> >::iterator data_lateset_iterator;
	std::string data_id;
	std::ostringstream oss;
	while ( !f.eof() ) {
		line_idx++;
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

		if ( tokens.size() <= 1 && datamode == false )
			PIPELINE_PARSE_ERROR("invalid line command");

		if ( tokens[0] == PIPELINE_BOUNDARY ) {
			// already in data mode, but get another boundary
			// push the current data in a id
			if ( datamode == true ) {
				datamode = false;

				// search each property to set
				bool have_set = false;
				do {
					have_set = false;
					for ( data_lateset_iterator = data_lateset.begin();
						  data_lateset_iterator != data_lateset.end();
						  data_lateset_iterator++ ) {

						if ( (*data_lateset_iterator)[2] == data_id ) {

							module1 = this->getModuleById((*data_lateset_iterator)[0]);
							if ( module1 == NULL )
								PIPELINE_PARSE_ERROR("unable to find module with id " << tokens[2]);
							module1->property((*data_lateset_iterator)[1]).set(oss.str());
							module1->property((*data_lateset_iterator)[1]).setText(true);

							if ( module1->haveError() )
								PIPELINE_PARSE_ERROR("module error:" << module1->getLastError());

							// erase current var
							have_set = true;
							data_lateset.erase(data_lateset_iterator);
							break;
						}
					}
				} while ( have_set );

				oss.str("");
			}

			// if we have more than one token, restart data mode
			if ( tokens.size() == 2 ) {
				datamode = true;
				data_id = tokens[1];
			}
			continue;
		}

		if ( datamode == true ) {
			// we are in data mode, and we don't have boundary, push data.
			oss << line << "\n";
			continue;
		}

		if ( tokens[0] == "config" ) {
			if ( tokens.size() < 3 )
				PIPELINE_PARSE_ERROR("not enough parameters");
			if ( tokens[1] == "delay" )
				g_config_delay = atoi(tokens[2].c_str());
		} else if ( tokens[0] == "pipeline" ) {
			if ( tokens.size() < 2 )
				PIPELINE_PARSE_ERROR("not enough parameters");

			if ( tokens[1] == "create" ) {
				if ( tokens.size() != 4 )
					PIPELINE_PARSE_ERROR("not enough parameters");

				module1 = this->getModuleById(tokens[3]);
				if ( module1 != NULL )
					PIPELINE_PARSE_ERROR("id already used");

				module1 = moFactory::getInstance()->create(tokens[2]);
				if ( module1 == NULL )
					PIPELINE_PARSE_ERROR("unknown module " << tokens[2]);

				if ( module1->haveError() )
					PIPELINE_PARSE_ERROR("module error:" << module1->getLastError());

				module1->property("id").set(tokens[3]);
				module1->property("id").setReadOnly(true);

				if ( module1->haveError() )
					PIPELINE_PARSE_ERROR("module error:" << module1->getLastError());

				this->addElement(module1);

			} else if ( tokens[1] == "set" ) {
				if ( tokens.size() < 4 || tokens.size() > 5 )
					PIPELINE_PARSE_ERROR("not enough parameters");

				module1 = this->getModuleById(tokens[2]);
				if ( module1 == NULL )
					PIPELINE_PARSE_ERROR("unable to find module with id " << tokens[2]);

				if ( tokens.size() == 5 )
					module1->property(tokens[3]).set(tokens[4]);
				else
					module1->property(tokens[3]).set("");

				if ( module1->haveError() )
					PIPELINE_PARSE_ERROR("module error:" << module1->getLastError());

			} else if ( tokens[1] == "settext" ) {
				if ( tokens.size() != 5 )
					PIPELINE_PARSE_ERROR("not enough parameters");

				std::string vals[3];
				vals[0] = tokens[2];
				vals[1] = tokens[3];
				vals[2] = tokens[4];
				std::valarray<std::string> value(vals, 3);
				data_lateset.push_back(value);

			} else if ( tokens[1] == "connect" ) {
				if ( tokens.size() != 6 )
					PIPELINE_PARSE_ERROR("not enough parameters");

				module1 = this->getModuleById(tokens[2]);
				if ( module1 == NULL )
					PIPELINE_PARSE_ERROR("unable to find module with id " << tokens[2]);

				module2 = this->getModuleById(tokens[4]);
				if ( module2 == NULL )
					PIPELINE_PARSE_ERROR("unable to find module with id " << tokens[4]);

				outidx = atoi(tokens[3].c_str());
				inidx = atoi(tokens[5].c_str());

				module2->setInput(module1->getOutput(outidx), inidx);

				if ( module1->haveError() )
					PIPELINE_PARSE_ERROR("module error:" << module1->getLastError());

				if ( module2->haveError() )
					PIPELINE_PARSE_ERROR("module error:" << module2->getLastError());

			} else
				PIPELINE_PARSE_ERROR("unknown pipeline subcommand: " << tokens[1]);
		} else
			PIPELINE_PARSE_ERROR("unknown command: " << tokens[0]);
	}
	return true;
}

std::string moPipeline::serializeCreation() {
	std::ostringstream oss;
	char buf[128];
	time_t tt = time(NULL);
	struct tm* tm = localtime(&tt);
	strftime(buf, sizeof(buf)-1, "%x %X", tm);

	oss << "# ================================================================" << std::endl;
	oss << "# PIPELINE AUTOGENERATED on " << buf << std::endl;
	oss << "# ================================================================" << std::endl;
	oss << "" << std::endl;

	// export modules and their properties
	std::vector<moModule *>::iterator it;
	for ( it = this->modules.begin(); it != this->modules.end(); it++ )
		(*it)->serializeCreation(oss, false);

	// now do connections, once all modules have been created
	std::vector<moModule *>::iterator mod;
	for ( mod = this->modules.begin(); mod != this->modules.end(); mod++ )
		(*mod)->serializeConnections(oss);

	// export data from module (big properties)
	for ( it = this->modules.begin(); it != this->modules.end(); it++ )
		(*it)->serializeCreation(oss, true);

	return oss.str();
}
