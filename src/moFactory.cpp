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


#include <string>

#include "moLog.h"
#include "moFactory.h"

#define REGISTER_MODULE(name) \
	extern moModule* factory_create_mo##name(); \
	LOG(MO_DEBUG, "register <" #name "> module"); \
	moFactory::getInstance()->registerModule(#name, factory_create_mo##name);

LOG_DECLARE("Factory");

static moFactory *instance = NULL;

void moFactory::init() {
	REGISTER_MODULE(Amplify);
	REGISTER_MODULE(BackgroundSubtract);
	REGISTER_MODULE(BlobTracker)
	REGISTER_MODULE(BlobFinder)
	REGISTER_MODULE(Camera);
	REGISTER_MODULE(Combine);
	REGISTER_MODULE(Dump);
	REGISTER_MODULE(FiducialTracker);
	REGISTER_MODULE(GrayScale);
	REGISTER_MODULE(GreedyBlobTracker);
	REGISTER_MODULE(Highpass);
	REGISTER_MODULE(Image);
	REGISTER_MODULE(ImageDisplay);
	REGISTER_MODULE(Invert);
	REGISTER_MODULE(Justify);
	REGISTER_MODULE(Mask);
	REGISTER_MODULE(MirrorImage);
	REGISTER_MODULE(Smooth);
	REGISTER_MODULE(Roi);
	REGISTER_MODULE(Threshold);
	REGISTER_MODULE(Tuio);
	REGISTER_MODULE(Video);
	REGISTER_MODULE(Erode);
	REGISTER_MODULE(Dilate);
	REGISTER_MODULE(Canny);
	REGISTER_MODULE(Calibration);
	REGISTER_MODULE(Hsv);
	REGISTER_MODULE(DistanceTransform);
	REGISTER_MODULE(PeakFinder);
	REGISTER_MODULE(FingerTipFinder);
	REGISTER_MODULE(YCrCbThreshold);
	//DoNotRemoveThisComment
	LOG(MO_INFO, "register " << moFactory::getInstance()->list().size() << " modules");
}

void moFactory::cleanup() {
	delete instance;
}

moFactory::moFactory() {
}

moFactory::~moFactory() {
}

moFactory *moFactory::getInstance() {
	if ( instance == NULL )
		instance = new moFactory();
	return instance;
}

void moFactory::registerModule(const std::string &name, moFactoryCreateCallback callback) {
	this->database[name] = callback;
}

moModule *moFactory::create(const std::string &name) {
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	if ( sname.size() > 2 && sname.substr(0, 2) == "ot" )
		sname =	sname.substr(2, sname.length() - 2);
	if ( sname.size() > 6 && sname.substr(sname.length() - 6, 6) == "Module" )
		sname = sname.substr(0, sname.length() - 6);

	std::map<std::string, moFactoryCreateCallback>::iterator it;
	it = this->database.find(sname);
	if ( it == this->database.end() )
		return NULL;
	return (it->second)();
}

std::vector<std::string> moFactory::list() {
	std::vector<std::string> l;
	std::map<std::string, moFactoryCreateCallback>::iterator it;
	for ( it = this->database.begin(); it != this->database.end(); it++ )
		l.push_back(it->first);
	return l;
}

