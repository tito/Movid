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


#ifndef MO_PIPELINE_H
#define MO_PIPELINE_H

#include <vector>
#include <string>
#include "moModule.h"

class moPipeline : public moModule {
public:

	moPipeline();
	virtual ~moPipeline();

	virtual void setInput(moDataStream* stream, int n=0);
	virtual moDataStream *getInput(int n=0);
	virtual moDataStream *getOutput(int n=0);

	virtual void addElement(moModule *module);
	virtual void removeElement(moModule *module);

	virtual int getInputCount();
	virtual int getOutputCount();
	virtual moDataStreamInfo *getInputInfos(int n=0);
	virtual moDataStreamInfo *getOutputInfos(int n=0);

	virtual void start();
	virtual void stop();
	virtual void update();
	virtual void poll();

	moModule *firstModule();
	moModule *lastModule();
	moModule *getModule(unsigned int index);
	moModule *getModuleById(const std::string &id);
	unsigned int size();

	virtual void setGroup(bool group=true);
	virtual bool isGroup();
	virtual bool isPipeline();

	/*! \brief Get last error message, once called, it will reset the error state
	 */
	virtual std::string getLastError();

	/*! \brief Indicate if the module have an error
	 */
	virtual bool haveError();

	/*! \brief Dump the pipeline into a file
	 */
	virtual std::string serializeCreation();

	/*! \brief Parse a file, and inject to the current pipeline
	 */
	bool parse(const std::string& filename);

private:
	std::vector<moModule *> modules;
	bool is_group;
	std::string last_internal_error;

	MODULE_INTERNALS();
};

#endif

