/****************************************************************************
*
* Image.hpp
*
* Purpose:  C++ wrapper for OpenCV IplImage which supports simple and 
*						efficient access to the image data
*
* Author: Donovan Parks, September 2007
*
* Based on code from: 
*  http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.hpptml
******************************************************************************/

#include "Image.hpp"

ImageBase::~ImageBase()
{ 
	if(imgp != NULL && m_bReleaseMemory)
		cvReleaseImage(&imgp);
	imgp = NULL;	
}

void DensityFilter(BwImage& image, BwImage& filtered, int minDensity, unsigned char fgValue)
{
	for(int r = 1; r < image.Ptr()->height-1; ++r)
	{
		for(int c = 1; c < image.Ptr()->width-1; ++c)
		{
			int count = 0;
			if(image(r,c) == fgValue)
			{
				if(image(r-1,c-1) == fgValue)
					count++;
				if(image(r-1,c) == fgValue)
					count++;
				if(image(r-1,c+1) == fgValue)
					count++;
				if(image(r,c-1) == fgValue)
					count++;
				if(image(r,c+1) == fgValue)
					count++;
				if(image(r+1,c-1) == fgValue)
					count++;
				if(image(r+1,c) == fgValue)
					count++;
				if(image(r+1,c+1) == fgValue)
					count++;

				if(count < minDensity)
					filtered(r,c) = 0;
				else
					filtered(r,c) = fgValue;
			}
			else
			{
				filtered(r,c) = 0;
			}
		}
	}
}