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


#include <stdio.h>
#include <assert.h>

#include "../moLog.h"
#include "../moModule.h"
#include "../moDataStream.h"
#include "moKinectNetworkModule.h"
#include "highgui.h"

#include <math.h>

#include <arpa/inet.h>

MODULE_DECLARE(KinectNetwork, "native", "Fetch Kinect network stream");

#define DEPTH_LEN		(640 * 480)
#define DEPTH_SZ		DEPTH_LEN * sizeof(uint16_t)
#define RGB_LEN			(640 * 480 * 3)
#define RGB_SZ			RGB_LEN * sizeof(uint8_t)

uint16_t t_gamma[2048];

moKinectNetworkModule::moKinectNetworkModule() : moModule(MO_MODULE_OUTPUT) {

	MODULE_INIT();

	this->s = -1;
	this->output_buffer = NULL;
	this->stream = new moDataStream("IplImage");
	this->monochrome = new moDataStream("IplImage8");

	// declare outputs
	this->declareOutput(0, &this->stream, new moDataStreamInfo(
			"camera", "IplImage", "Image stream of the camera"));
	this->declareOutput(1, &this->monochrome, new moDataStreamInfo(
			"camera", "IplImage8", "Image stream of the camera"));

	// declare properties
	this->properties["index"] = new moProperty(0);
	this->properties["port"] = new moProperty(6001);

	// reinit t_gamma
	int i;
	for (i=0; i<2048; i++) {
		float v = i/2048.0;
		v = powf(v, 3)* 6;
		t_gamma[i] = v*6*256;
	}
}

moKinectNetworkModule::~moKinectNetworkModule() {
}

void moKinectNetworkModule::start() {
    struct sockaddr_in saddr;

	LOGM(MO_TRACE, "start camera");

	this->s = socket(AF_INET, SOCK_STREAM, 0);
	if ( this->s < 0 ) {
		LOGM(MO_ERROR, "could not init socket");
		this->setError("Unable to init socket");
	}

	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	saddr.sin_port = htons(this->properties["port"]->asInteger());

	if ( connect(this->s, (sockaddr *)&saddr, sizeof(saddr)) < 0 ) {
		LOGM(MO_ERROR, "unable to connect to server");
		this->setError("Unable to connect to server");
		this->s = -1;
	}

	// Allocate image
	CvSize size;
	size.width = 640;
	size.height = 480;
	this->output_buffer = cvCreateImage(size, IPL_DEPTH_8U, 1);

	moModule::start();
}

void moKinectNetworkModule::stop() {
	moModule::stop();
	if ( this->output_buffer != NULL )
		cvReleaseImage(&this->output_buffer);
}

void moKinectNetworkModule::update() {
	int n, sz;
	uint8_t buf8[DEPTH_SZ];

	if ( this->s == -1 )
		return;

	sz = DEPTH_SZ;
	while ( sz ) {
		n = read(this->s, &buf8[DEPTH_SZ - sz], sz);
		if ( n == -1 ) {
			LOGM(MO_ERROR, "failed to read a full image");
			return;
		}
		sz -= n;
	}

	// Our current module work on 8 bits depth, not 16. Convert.
	uint16_t buf16[DEPTH_LEN];
	uint8_t buf8n[DEPTH_LEN];
	memset(buf8n, 0, DEPTH_LEN);
	memcpy((char *)(void *)buf16, buf8, DEPTH_SZ);
	for ( int i = 0; i < DEPTH_LEN; i++ ) {
		/**
		int pval = t_gamma[buf16[i]];
		int lb = pval & 0xff;
		if ( (pval >> 8) == 0 )
			buf8n[i] = lb;
		**/
		buf8n[i] = (buf16[i] >> 1) & 0xff;
	}


	// Copy
	memcpy(this->output_buffer->imageData, buf8n, DEPTH_LEN);

	// Push
	this->monochrome->push(this->output_buffer);

	/** Import algo from glview
	uint8_t gl_depth_back[640*480*4];
	uint16_t buf16[DEPTH_LEN];
	memcpy((char *)(void *)buf16, buf8, DEPTH_SZ);
	for (int i=0; i<640*480; i++) {
		int pval = t_gamma[buf16[i]];
		int lb = pval & 0xff;
		switch (pval>>8) {
			case 0:
				gl_depth_back[3*i+0] = 255;
				gl_depth_back[3*i+1] = 255-lb;
				gl_depth_back[3*i+2] = 255-lb;
				break;
			case 1:
				gl_depth_back[3*i+0] = 255;
				gl_depth_back[3*i+1] = lb;
				gl_depth_back[3*i+2] = 0;
				break;
			case 2:
				gl_depth_back[3*i+0] = 255-lb;
				gl_depth_back[3*i+1] = 255;
				gl_depth_back[3*i+2] = 0;
				break;
			case 3:
				gl_depth_back[3*i+0] = 0;
				gl_depth_back[3*i+1] = 255;
				gl_depth_back[3*i+2] = lb;
				break;
			case 4:
				gl_depth_back[3*i+0] = 0;
				gl_depth_back[3*i+1] = 255-lb;
				gl_depth_back[3*i+2] = 255;
				break;
			case 5:
				gl_depth_back[3*i+0] = 0;
				gl_depth_back[3*i+1] = 0;
				gl_depth_back[3*i+2] = 255-lb;
				break;
			default:
				gl_depth_back[3*i+0] = 0;
				gl_depth_back[3*i+1] = 0;
				gl_depth_back[3*i+2] = 0;
				break;
		}
	}

	// Convert to an Iplimage
	IplImage *img;
	CvSize size;
	size.width = 640;
	size.height = 480;
	img = cvCreateImage(size, IPL_DEPTH_8U, 3);
	memcpy(img->imageData, gl_depth_back, DEPTH_LEN * 3);

	// Push
	this->stream->push(img);
	**/

	this->notifyUpdate();
}

void moKinectNetworkModule::poll() {
	this->notifyUpdate();
	moModule::poll();
}

