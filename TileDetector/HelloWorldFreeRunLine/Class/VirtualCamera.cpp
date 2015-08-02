#include "VirtualCamera.h"


VirtualCamera::VirtualCamera()
{
}
VirtualCamera::VirtualCamera(MicroDisplayInit& mdi, string imgname)
{
	BufferLength = mdi.MaxPics;
	WIDTH = mdi.width;
	stringstream ss;
	ss << "virtualcameras/" << imgname;
	ss >> imgname;
	if (mdi.colorType == MicroDisplayInit::GRAY)
		buffer = cv::imread(imgname, 0);
	else
		buffer = cv::imread(imgname, 1);
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
	if (!EndFlag && ++BufferIndex >= BufferLength)
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
	EndFlag = false;
	BufferIndex = 0;
	cv::Mat OriginalImage;
	do{
		Sleep(0.1);
	} while (!s.AddFrame(GetNext()));
	return 0;
}