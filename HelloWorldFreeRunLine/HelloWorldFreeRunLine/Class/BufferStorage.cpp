#include "BufferStorage.h"



BufferStorage::BufferStorage(MicroDisplayInit& mditmp)
{
	mdi = mditmp;
	WIDTH = mdi.width;
	LENGTH = mdi.MaxPics;
}
BufferStorage::~BufferStorage()
{
	Buffer0.release();
//	Buffer0Gray.release();
	Buffer0Img.release();

	Buffer1.release();
//	Buffer1Gray.release();
	Buffer1Img.release();
}
void BufferStorage::Start()
{
	EndReadFlag = false;
	EndWriteFlag = false;
	BufferWriteIndex = 0;
	BufferReadIndex = 0;
	if (bufferIndex % 2 == 0)
	{
		Buffer0.release();
		Buffer0 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		//Buffer0Gray.release();
		//Buffer0Gray = cv::Mat(LENGTH, WIDTH, CV_8U, cv::Scalar(0));
		Buffer0Img.release();
		Buffer0Img = cv::Mat(LENGTH + 10, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		NowBuffer = Buffer0;
		//NowBufferGray = Buffer0Gray;
		NowBufferImg = Buffer0Img;
	}
	else
	{
		Buffer1.release();
		Buffer1 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		//Buffer1Gray.release();
		//Buffer1Gray = cv::Mat(LENGTH, WIDTH, CV_8U, cv::Scalar(0));
		Buffer1Img.release();
		Buffer1Img = cv::Mat(LENGTH + 10, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		NowBuffer = Buffer1;
		//NowBufferGray = Buffer1Gray;
		NowBufferImg = Buffer1Img;
	}
	bufferIndex = 1 + bufferIndex;
}
bool BufferStorage::AddFrame(cv::Mat& frame)
{
	if (BufferWriteIndex >= LENGTH)
		return true;

	if (frame.channels() == 1)
	{
		if (mdi.SamplesGray.cols != 0)
			frame = frame + mdi.SamplesGray;
		cv::cvtColor(frame, frame, CV_GRAY2BGR);
	}
	else
	{
		if (mdi.SamplesRGB.cols = 0)
			frame = frame + mdi.SamplesRGB;
	}

	cv::Mat Now = NowBuffer;
	cv::Mat oneFrame = NowBuffer(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	oneFrame += frame;

	//cv::Mat frameGray;
	//cv::cvtColor(frame, frameGray, CV_BGR2GRAY);
	//oneFrame = NowBufferGray(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	//oneFrame += frameGray;

	//N张图像叠加
	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	oneFrame += frame;
	if (BufferWriteIndex == 0)//补偿第一第二行没得到叠加的情况
	{
		oneFrame += frame;
		oneFrame += frame;
	}
	if (BufferWriteIndex == 1)//补偿第一第二行没得到叠加的情况
	{
		oneFrame += frame;
	}
	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex + 1, WIDTH, 1));
	oneFrame += frame;
	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex + 2, WIDTH, 1));
	oneFrame += frame;


	//指向下一条缓冲
	++BufferWriteIndex;
	if (BufferWriteIndex >= LENGTH)
	{
		EndWriteFlag = true;
		return true;
	}
	return false;
}

int BufferStorage::GetFrame(cv::Mat& frame)
{
	//-1下一帧尚未写入完成
	if (BufferReadIndex == BufferWriteIndex && BufferReadIndex != LENGTH)
		return -1;

	if (BufferReadIndex >= LENGTH)
	{
		EndReadFlag = true;
		return 0;
	}

	++BufferReadIndex;

	if (BufferReadIndex >= LENGTH)
	{
		EndReadFlag = true;
		return 0;
	}

	cv::Mat Now = NowBuffer;
	cv::Mat oneFrame = NowBuffer(cv::Rect(0, BufferReadIndex, WIDTH, 1));
	oneFrame.copyTo(frame);
	return 1;
}