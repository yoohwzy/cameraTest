#include "VirtualCamera.h"


VirtualCamera::VirtualCamera()
{
}
VirtualCamera::VirtualCamera(MicroDisplayInit& mdi, string imgname)
{
	BufferLength = mdi.MaxPics;
	WIDTH = mdi.Width;
	stringstream ss;

	if (imgname.find(":\\", 0) == std::string::npos)
	{
		ss << "virtualcameras/" << imgname;
		ss >> imgname;
	}
	else
	{
		ss << imgname;
	}
	if (mdi.colorType == MicroDisplayInit::GRAY)
		buffer = cv::imread(imgname, 0);
	else
		buffer = cv::imread(imgname, 1);
	if (buffer.cols == 0 || buffer.rows == 0)
	{
		stringstream sss;
		sss << imgname << "   do not exist!" << endl;
		printf_globle(sss.str());
		//ExitWithError(sss.str());
	}
	else if (buffer.cols != WIDTH || buffer.rows < BufferLength)
	{
		cv::resize(buffer, buffer, cv::Size(WIDTH, BufferLength));
	}
}
VirtualCamera::~VirtualCamera()
{
	buffer.release();
}

cv::Mat VirtualCamera::GetNext()
{
	int i = BufferIndex;
	if (!EndFlag && ++BufferIndex > BufferLength)
	{
		EndFlag = true;
		i = 0;
	}
	cv::Mat ROI = buffer(cv::Rect(0, i, WIDTH, 1));
	cv::Mat ret = ROI.clone();
	return ret;
}
int VirtualCamera::FreeRunning(MicroDisplayInit& mdi, BufferStorage *s)
{
	EndFlag = false;
	BufferIndex = 0;
	//cv::Mat OriginalImage;
	double t = (double)cv::getTickCount();
	do{
		Sleep(0.1);
	} while (!s->AddFrame(GetNext()));
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

	t *= 1000;
	int sleep = mdi.MaxPics/10000*1000 - (int)t;
	Sleep(sleep);
	return 0;
}