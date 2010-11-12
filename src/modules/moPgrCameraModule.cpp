/***********************************************************************
 ** Copyright (C) 2010 Movid Authors.and Aras (arasbm@gmail.com) All rights reserved.
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
#include "../moDataStream.h"
#include "moPgrCameraModule.h"

using namespace FlyCapture2;

MODULE_DECLARE(Pgr, "not native", "Fetch PGR camera stream");

moPgrCameraModule::moPgrCameraModule() : moModule(MO_MODULE_OUTPUT)
{

	MODULE_INIT();

	//this->camera = NULL;
	this->stream = new moDataStream("IplImage");
	this->monochrome = new moDataStream("IplImage8");

	// declare outputs
	this->declareOutput(0, &this->stream, new moDataStreamInfo("camera", "IplImage", "Image stream of the camera"));
	this->declareOutput(1, &this->monochrome, new moDataStreamInfo("camera", "IplImage8", "Image stream of the camera"));

	// declare properties
	this->properties["index"] = new moProperty(0);
	BusManager busMgr;
	unsigned int numCam;
	Error pgrErr = busMgr.GetNumOfCameras(&numCam);
	if (pgrErr != PGRERROR_OK)
	{
		LOGM(MO_ERROR, "Could not get number of PGR cameras");
	}
	else if (numCam < 1)
	{
		LOGM(MO_ERROR, "No PGR camera is present");
	}
	else
	{
		//Grab the first camera for now (TODO: make it possible to choose which camera to run)
		//use this->property("index").asInteger() to store the index
		pgrErr = busMgr.GetCameraFromIndex(0, &this->pgrGuid);
		if (pgrErr != PGRERROR_OK)
		{
			LOGM(MO_ERROR, "ERORR in initializing the camera");
		}
	}
}

moPgrCameraModule::~moPgrCameraModule()
{
}

void moPgrCameraModule::start()
{
	//assert( this->camera == NULL );
	LOGM(MO_TRACE, "starting a PGR camera");
	Error pgrErr = this->camera.Connect(&this->pgrGuid);
	if (pgrErr != PGRERROR_OK)
	{
		LOGM(MO_ERROR, "could not load camera: " << this->property("index").asInteger());
		this->setError("Unable to open camera");
	}

	//Now start the cam
	pgrErr = this->camera.StartCapture();
	if (pgrErr != PGRERROR_OK)
	{
		LOGM(MO_ERROR, "Error in starting the camera");
	}

	LOGM(MO_TRACE, "successfully started a PGR camera");
	moModule::start();
}

void moPgrCameraModule::stop()
{
	moModule::stop();
	//if ( this->camera != NULL ) {
	Error pgrErr = this->camera.StopCapture();
	if (pgrErr != PGRERROR_OK)
	{
		LOGM(MO_ERROR, "ERORR in stopping the PGR camera");
	}
	else
	{
		LOGM(MO_TRACE, "successfully releaseed the camera");
		//this->camera = NULL;
	}
	//}
}

void moPgrCameraModule::update()
{
	//if ( this->camera != NULL ) {
	// push a new image on the stream
	LOGM(MO_TRACE, "push a new image on the stream");
	// Get an Image from PGR
	Image rawImage;
	Error pgrErr = this->camera.RetrieveBuffer( &rawImage );
	if (pgrErr != PGRERROR_OK)
	{
		LOGM(MO_ERROR, "Error in getting image from buffer");
	}

	//Convert the PGR image to OpenCV IplImage
	IplImage *img = this->ConvertImageToOpenCV(&rawImage);
	// TODO: support color image: this->stream->push(img);
	this->monochrome->push(img);
	this->notifyUpdate();
	//}
}

void moPgrCameraModule::poll()
{
	this->notifyUpdate();
	moModule::poll();
}

IplImage* moPgrCameraModule::ConvertImageToOpenCV(Image* pImage)
{
	IplImage* cvImage = NULL;
	bool bColor = true;
	CvSize mySize;
	mySize.height = pImage->GetRows();
	mySize.width = pImage->GetCols();

	switch ( pImage->GetPixelFormat() )
	{
	case PIXEL_FORMAT_MONO8:
		cvImage = cvCreateImageHeader(mySize, 8, 1 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 1;
		bColor = false;
		break;
	case PIXEL_FORMAT_411YUV8:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_422YUV8:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_444YUV8:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_RGB8:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_MONO16:
		cvImage = cvCreateImageHeader(mySize, 16, 1 );
		cvImage->depth = IPL_DEPTH_16U;
		cvImage->nChannels = 1;
		bColor = false;
		break;
	case PIXEL_FORMAT_RGB16:
		cvImage = cvCreateImageHeader(mySize, 16, 3 );
		cvImage->depth = IPL_DEPTH_16U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_S_MONO16:
		cvImage = cvCreateImageHeader(mySize, 16, 1 );
		cvImage->depth = IPL_DEPTH_16U;
		cvImage->nChannels = 1;
		bColor = false;
		break;
	case PIXEL_FORMAT_S_RGB16:
		cvImage = cvCreateImageHeader(mySize, 16, 3 );
		cvImage->depth = IPL_DEPTH_16U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_RAW8:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_RAW16:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_MONO12:
		//"Image format is not supported by OpenCV"
		bColor = false;
		break;
	case PIXEL_FORMAT_RAW12:
		//"Image format is not supported by OpenCV
		break;
	case PIXEL_FORMAT_BGR:
		cvImage = cvCreateImageHeader(mySize, 8, 3 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 3;
		break;
	case PIXEL_FORMAT_BGRU:
		cvImage = cvCreateImageHeader(mySize, 8, 4 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 4;
		break;
	case PIXEL_FORMAT_RGBU:
		cvImage = cvCreateImageHeader(mySize, 8, 4 );
		cvImage->depth = IPL_DEPTH_8U;
		cvImage->nChannels = 4;
		break;
	default:
		//ERROR in detecting image format
		return NULL;
	}

	if(bColor)
	{
		Image colorImage; //new image to be referenced by cvImage
		colorImage.SetData(new unsigned char[pImage->GetCols() * pImage->GetRows()*3], pImage->GetCols() * pImage->GetRows() * 3);
		pImage->Convert(PIXEL_FORMAT_BGR, &colorImage); //needs to be as BGR to be saved
		cvImage->width = colorImage.GetCols();
		cvImage->height = colorImage.GetCols();
		cvImage->widthStep = colorImage.GetStride();
		cvImage->origin = 0; //interleaved color channels
		cvImage->imageDataOrigin = (char*)colorImage.GetData(); //DataOrigin and Data same pointer, no ROI
		cvImage->imageData         = (char*)(colorImage.GetData());
		cvImage->widthStep              = colorImage.GetStride();
		cvImage->nSize = sizeof (IplImage);
		cvImage->imageSize = cvImage->height * cvImage->widthStep;
	}
	else
	{
		cvImage->imageDataOrigin = (char*)(pImage->GetData());
		cvImage->imageData         = (char*)(pImage->GetData());
		cvImage->widthStep         = pImage->GetStride();
		cvImage->nSize             = sizeof (IplImage);
		cvImage->imageSize         = cvImage->height * cvImage->widthStep;
		//at this point cvImage contains a valid IplImage
	}
	return cvImage;
}

