/****************************************************************************
*
* BgsParams.hpp
*
* Purpose: Base class for BGS parameters. Any parameters common to all BGS
*					 algorithms should be specified directly in this class.
*
* Author: Donovan Parks, May 2008
*
******************************************************************************/

#ifndef BGS_PARAMS_H_
#define BGS_PARAMS_H_

namespace Algorithms
{
namespace BackgroundSubtraction
{

class BgsParams
{
public:
	virtual ~BgsParams() {}

	virtual void SetFrameSize(unsigned int width, unsigned int height)
	{
		m_width = width;
		m_height = height;
		m_size = width*height;
	}

	unsigned int &Width() { return m_width; }
	unsigned int &Height() { return m_height; }
	unsigned int &Size() { return m_size; }

protected:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_size;
};

};
};

#endif