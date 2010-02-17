#include <assert.h>
#include "otImageFormatModule.h"
#include "cv.h"

MODULE_DECLARE(ImageFormatModule, "native", "Format the image according to properties e.g(width=500,height=500, format=[rgb|gray])");

//TODO
// formats:
// 
// size
// format/nChannles ?  rgb, gray, others ?
// depth
//
//
// or instead just ResizeModule, GrayScaleModule, etc..? (could make this by just calling different constructors on FormatModule?)
//     



