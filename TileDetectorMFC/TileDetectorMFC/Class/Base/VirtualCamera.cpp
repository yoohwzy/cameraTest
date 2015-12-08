#include "VirtualCamera.h"


VirtualCamera::VirtualCamera(GrabbingBuffer *gb, int frameCount, int width, string imgname, int colorType)
{
	_width = width;
	_frameCount = frameCount;
	_colorType = colorType;
	_gb = gb;

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
	if (_colorType == GRAY)
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
	else if (buffer.cols != _width || buffer.rows < _frameCount)
	{
		cv::resize(buffer, buffer, cv::Size(_width, _frameCount));
	}
}
VirtualCamera::~VirtualCamera()
{
	buffer.release();
}
void VirtualCamera::Capture()
{
	//cv::Mat OriginalImage;
	double t = (double)cv::getTickCount();

	unsigned int fcount = 0;
	unsigned int last_pic_nr = 0;
	unsigned int cur_pic_nr;


	cv::Mat OriginalImage;
	while (fcount < _frameCount)
	{
		OriginalImage = buffer(cv::Rect(0, fcount, _width, _frameHeight));
		_gb->AddFrame(OriginalImage);
		fcount++;
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	t *= 1000;
	//int sleep = _frameCount / 10000 * 1000 - (int)t;
	//if (sleep > 10)
	//	Sleep(sleep);
}


bool VirtualCamera::TestCam(string imgname)
{
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
	cv::Mat buffer = cv::imread(imgname, 0);
	if (buffer.cols == 0 || buffer.rows == 0)
	{
		stringstream sss;
		sss << imgname << "   do not exist!" << endl;
		printf_globle(sss.str());
		return false;
	}
	return true;
}