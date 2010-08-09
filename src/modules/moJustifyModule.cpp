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


#include <assert.h>
#include "../moLog.h"
#include "../moModule.h"
#include "../moDataGenericContainer.h"
#include "../moDataStream.h"
#include "cv.h"
#include "moJustifyModule.h"

MODULE_DECLARE(Justify, "native", "Justify the x/y data: (X/Y) = ((ma,mb)/(mc,md)) * (x/y) + (dx/dy)");

moJustifyModule::moJustifyModule() : moModule(MO_MODULE_INPUT|MO_MODULE_OUTPUT) {

	MODULE_INIT();

	this->properties["dx"] = new moProperty(0.0);
	this->properties["dx"] = new moProperty(0.0);
	this->properties["ma"] = new moProperty(1.0);
	this->properties["mb"] = new moProperty(0.0);
	this->properties["mc"] = new moProperty(0.0);
	this->properties["md"] = new moProperty(1.0);

	this->input = NULL;
	this->declareInput(0, &this->input, new moDataStreamInfo(
			"data", "blob", "Data stream with type of blob"));

	this->output = new moDataStream("GenericTouch");
	this->declareOutput(0, &this->output, new moDataStreamInfo(
			"data", "blob", "Result of the justify"));
}

moJustifyModule::~moJustifyModule() {
	if (this->output != NULL)
		delete this->output;
}

void moJustifyModule::notifyData(moDataStream *input) {
	assert(input != NULL);
	assert(input == this->input);

	std::string format = input->getFormat();
	
	this->input->lock();
	moDataGenericList::iterator it;
	for (it = this->blobs.begin(); it != this->blobs.end(); it++)
                delete (*it);
        this->blobs.clear();

	moDataGenericList *list = (moDataGenericList *)this->input->getData();
	for ( it = list->begin(); it != list->end(); it++ ) {
		// FIXME !
		if (format == "GenericFiducial") {
			assert((*it)->properties["type"]->asString() == "fiducial");
		} else if (format == "GenericTouch") {
			assert((*it)->properties["type"]->asString() == "touch");
		}
		float x = (float)(*it)->properties["x"]->asDouble();
		float y = (float)(*it)->properties["y"]->asDouble();
		float ma = this->property("ma").asDouble();
		float mb = this->property("mb").asDouble();
		float mc = this->property("mc").asDouble();
		float md = this->property("md").asDouble();
		float dx = this->property("dx").asDouble();
		float dy = this->property("dy").asDouble();

		LOGM(MO_INFO, "id=" << (*it)->properties["id"]->asInteger() << " x=" << x << " y=" << y);
		moDataGenericContainer *touch = new moDataGenericContainer();
		touch->properties["type"] = new moProperty((*it)->properties["type"]->asString());
		touch->properties["id"] = new moProperty((*it)->properties["id"]->asInteger());
		touch->properties["x"] = new moProperty(ma*x + mb*y + dx);
		touch->properties["y"] = new moProperty(mc*x + md*y + dy);
		if (format == "GenericFiducial") {
			touch->properties["angle"] = new moProperty((*it)->properties["angle"]->asDouble());
			touch->properties["leaf_size"] = new moProperty((*it)->properties["leaf_size"]->asDouble());
			touch->properties["root_size"] = new moProperty((*it)->properties["root_size"]->asDouble());
		}
		if (format == "GenericTouch") {
			touch->properties["w"] = new moProperty((*it)->properties["w"]->asDouble());
			touch->properties["h"] = new moProperty((*it)->properties["h"]->asDouble());
		}
		this->blobs.push_back(touch);
	}
	this->output->push(&this->blobs);

	this->input->unlock();
}

void moJustifyModule::update() {
	if ((this->input == NULL) || (this->output == NULL))
		return;

}

