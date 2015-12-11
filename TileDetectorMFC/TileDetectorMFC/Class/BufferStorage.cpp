#include "BufferStorage.h"



GrabbingBuffer::GrabbingBuffer(int frameCount, int width)
{
	_frameCount = frameCount;
	_width = width;


	WriteIndex = 0;
	ReadIndex = 0;

	OriginalImage.release();
	Image.release();

	OriginalImage = cv::Mat(_frameCount, _width, CV_8UC3, cv::Scalar(0, 0, 0));
	Image = cv::Mat(_frameCount, _width, CV_8UC3, cv::Scalar(0, 0, 0));
}
GrabbingBuffer::~GrabbingBuffer()
{
	OriginalImage.release();
	Image.release();
}
bool GrabbingBuffer::AddFrame(cv::Mat& frame)
{
	if (WriteIndex >= _frameCount)
		return true;

	if (frame.channels() == 1)
	{
		cv::cvtColor(frame, frame, CV_GRAY2BGR);
	}
	cv::Mat oneFrame = OriginalImage(cv::Rect(0, WriteIndex, _width, 1));
	oneFrame = frame;
	//指向下一行
	++WriteIndex;
	if (WriteIndex >= _frameCount)
	{
		return true;
	}
	return false;
}
void GrabbingBuffer::ThreeInOne(int lineIndex)
{
	//防止越界
	if (lineIndex < 0 || lineIndex >= _frameCount)
		return;

	cv::Mat frame = OriginalImage(cv::Rect(0, lineIndex, _width, 1));
	//N张图像叠加
	cv::Mat oneFrame = Image(cv::Rect(0, lineIndex, _width, 1));
	oneFrame += frame;
	if (lineIndex == 0)//补偿第一第二行没得到叠加的情况
	{
		oneFrame += frame;
	}
	if (lineIndex == 1)//补偿第一第二行没得到叠加的情况
	{
		oneFrame += frame;
	}
	if (lineIndex + 1 < _frameCount)
	{
		oneFrame = Image(cv::Rect(0, lineIndex + 1, _width, 1));
		oneFrame += frame;
	}
	if (lineIndex + 2 < _frameCount)
	{
		oneFrame = Image(cv::Rect(0, lineIndex + 2, _width, 1));
		oneFrame += frame;
	}
	frame.release();
}