#include <string>

#include "otFactory.h"

#include "otCameraModule.h"
#include "otGaussianBlurModule.h"
#include "otImageDisplayModule.h"

otModule *otFactory::create(const char *name) {
	// XXX plug dynamic resolve here too
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	if ( sname.substr(0, 2) == "ot" )
		sname =	sname.substr(2, sname.length() - 2);
	if ( sname.substr(sname.length() - 6, 6) == "Module" )
		sname = sname.substr(0, sname.length() - 6);

	if ( sname == "Camera" )
		return new otCamera();
	else if ( sname == "ImageDisplay" )
		return new otImageDisplayModule();
	else if ( sname == "GaussianBlur" )
		return new otGaussianBlurModule();

	return NULL;
}

