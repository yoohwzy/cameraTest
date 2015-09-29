#include "stdafx.h"
#include "VirtualCamera.h"


VirtualCamera::VirtualCamera()
{
	cv::Mat virtualImg = cv::imread("lj0407bz01.jpg");
	int width = virtualImg.cols;//µ¥Ö¡Í¼Ïñ¿í¶È
	WIDTH = width;
	int index = 0;
	for (size_t i = 0; i < bufferLength; i++) 
	{
		cv::Mat ROI = virtualImg(cv::Rect(0, index, width, 1));
		++index;
		if (index == virtualImg.rows)
			index = 0;
		cv::Mat t = cv::Mat(ROI);
		buffer.push_back(t);
	}
}
VirtualCamera::~VirtualCamera()
{
}

cv::Mat VirtualCamera::GetNext()
{
	int i = BufferIndex;
	if (++BufferIndex >= bufferLength)
	{
		EndFlag = true;
		--BufferIndex;
	}
	return buffer[i];
}