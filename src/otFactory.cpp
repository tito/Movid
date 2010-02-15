#include <string>

#include "otFactory.h"

#include "otCamera.h"
#include "otGaussianBlurModule.h"
#include "otImageDisplayModule.h"

otModule *otFactory::create(const char *name) {
	// XXX plug dynamic resolve here too
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	//if ( sname.substr(0, 2) == "ot" )
	//	sname =	sname.substr(2, sname.length() - 2);

	if ( sname == "otCamera" )
		return new otCamera();
	else if ( sname == "otImageDisplayModule" )
		return new otImageDisplayModule();
	else if ( sname == "otGaussianBlurModule" )
		return new otGaussianBlurModule();

	return NULL;
}

