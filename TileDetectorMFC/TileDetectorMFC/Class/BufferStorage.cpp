#include "BufferStorage.h"



BufferStorage::BufferStorage(int _MaxPics, int _width)
{
	MaxPics = _MaxPics;
	width = _width;
}
BufferStorage::~BufferStorage()
{
	Buffer.release();
	BufferImg.release();
}
void BufferStorage::Start()
{
	EndReadFlag = false;
	EndWriteFlag = false;
	BufferWriteIndex = 0;
	BufferReadIndex = 0;

	Buffer.release();
	Buffer = cv::Mat(MaxPics, width, CV_8UC3, cv::Scalar(0, 0, 0));
	//BufferGray.release();
	//BufferGray = cv::Mat(MaxPics, width, CV_8U, cv::Scalar(0));
	BufferImg.release();
	BufferImg = cv::Mat(MaxPics + 0, width, CV_8UC3, cv::Scalar(0, 0, 0));
}
bool BufferStorage::AddFrame(cv::Mat& frame)
{
	if (BufferWriteIndex >= MaxPics)
		return true;

	if (frame.channels() == 1)
	{
		cv::cvtColor(frame, frame, CV_GRAY2BGR);
	}
	cv::Mat oneFrame = Buffer(cv::Rect(0, BufferWriteIndex, width, 1));
	oneFrame += frame;

	//指向下一行
	++BufferWriteIndex;
	if (BufferWriteIndex >= MaxPics)
	{
		EndWriteFlag = true;
		return true;
	}
	return false;
}
void BufferStorage::ThreeInOne(int lineIndex)
{
	cv::Mat frame = Buffer(cv::Rect(0, lineIndex, width, 1));

	//N张图像叠加
	cv::Mat oneFrame = BufferImg(cv::Rect(0, lineIndex, width, 1));
	oneFrame += frame;
	if (lineIndex == 0)//补偿第一第二行没得到叠加的情况
	{
		oneFrame += frame;
		oneFrame += frame;
	}
	if (lineIndex == 1)//补偿第一第二行没得到叠加的情况
	{
		oneFrame += frame;
	}
	if (lineIndex + 1 < MaxPics)
	{
		oneFrame = BufferImg(cv::Rect(0, lineIndex + 1, width, 1));
		oneFrame += frame;
	}
	if (lineIndex + 2 < MaxPics)
	{
		oneFrame = BufferImg(cv::Rect(0, lineIndex + 2, width, 1));
		oneFrame += frame;
	}
}