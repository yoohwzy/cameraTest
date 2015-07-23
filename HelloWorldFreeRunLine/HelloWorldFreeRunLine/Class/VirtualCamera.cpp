#include "VirtualCamera.h"


VirtualCamera::VirtualCamera()
{
}
VirtualCamera::VirtualCamera(MicroDisplayInit& mdi)
{
	BufferLength = mdi.MaxPics;
	WIDTH = mdi.width;
	buffer = cv::imread("virtualcameras/1.jpg");
	if (buffer.cols != WIDTH || buffer.rows < BufferLength)
	{
		cv::resize(buffer, buffer, cv::Size(WIDTH, BufferLength));
	}
	cv::Mat t = buffer;
}
VirtualCamera::~VirtualCamera()
{
	buffer.release();
}

cv::Mat VirtualCamera::GetNext()
{
	int i = BufferIndex;
	if (++BufferIndex >= BufferLength)
	{
		EndFlag = true;
		i = 0;
	}
	cv::Mat ROI = buffer(cv::Rect(0, i, WIDTH, 1));
	cv::Mat ret = ROI.clone();
	return ret;
}
int VirtualCamera::FreeRunning(MicroDisplayInit& mdi, BufferStorage& s)
{
	BufferIndex = 0;
	cv::Mat OriginalImage;
	do{
	} while (!s.AddFrame(GetNext()));
	return 0;
}