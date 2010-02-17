#include <string>

#include "otFactory.h"

#include "modules/otCameraModule.h"
#include "modules/otVideoModule.h"
#include "modules/otSmoothModule.h"
#include "modules/otImageDisplayModule.h"
#include "modules/otInvertModule.h"
#include "modules/otBackgroundSubtractModule.h"
#include "modules/otGrayScaleModule.h"
#include "modules/otThresholdModule.h"
#include "modules/otAmplifyModule.h"


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
	if ( sname == "Video" )
		return new otVideoModule();
	if ( sname == "ImageDisplay" )
		return new otImageDisplayModule();
	if ( sname == "Smooth" )
		return new otSmoothModule();
	if ( sname == "Invert" )
		return new otInvertModule();
	if ( sname == "BackgroundSubtract" )
		return new otBackgroundSubtractModule();
	if ( sname == "GrayScale" )
		return new otGrayScaleModule();
	if ( sname == "Threshold" )
		return new otThresholdModule();
	if ( sname == "Amplify" )
		return new otAmplifyModule();

	return NULL;
}

