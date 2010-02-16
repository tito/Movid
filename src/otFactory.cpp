#include <string>

#include "otFactory.h"

#include "modules/otCameraModule.h"
#include "modules/otGaussianBlurModule.h"
#include "modules/otImageDisplayModule.h"
#include "modules/otInvertModule.h"
#include "modules/otVideoModule.h"


otModule *otFactory::create(const char *name) {
	// XXX plug dynamic resolve here too
	std::string sname = std::string(name);

	// for easier creation, remove the ot at start
	if ( sname.size() > 2 && sname.substr(0, 2) == "ot" )
		sname =	sname.substr(2, sname.length() - 2);
	if ( sname.size() > 6 && sname.substr(sname.length() - 6, 6) == "Module" )
		sname = sname.substr(0, sname.length() - 6);

	if ( sname == "Camera" )
		return new otCameraModule();
	else if ( sname == "ImageDisplay" )
		return new otImageDisplayModule();
	else if ( sname == "GaussianBlur" )
		return new otGaussianBlurModule();
	else if ( sname == "Invert" )
		return new otInvertModule();
	else if ( sname == "Video" )
		return new otVideoModule();

	return NULL;
}

