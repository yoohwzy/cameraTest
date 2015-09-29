#include "stdafx.h"
#include "Storage.h"


Storage::Storage(int width, int length)
{
	WIDTH = width;
	LENGTH = length;
	//Buffer0 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
	//Buffer0Flag = cv::Mat(LENGTH, 1, CV_8U, cv::Scalar(0));
	//Buffer1 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
	//Buffer1Flag = cv::Mat(LENGTH, 1, CV_8U, cv::Scalar(0));
}
Storage::~Storage()
{
	Buffer0.release();
	Buffer1.release();
}
void Storage::Start()
{
	EndFlag = false;
	BufferWriteIndex = 0;
	BufferReadIndex = 0;
	if (bufferIndex % 2 == 0)
	{
		Buffer0.release();
		Buffer0 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		Buffer0Flag = cv::Mat(LENGTH, 1, CV_8U, cv::Scalar(0));
		NowBuffer = Buffer0;
	}
	else
	{
		Buffer1.release();
		Buffer1 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		Buffer1Flag = cv::Mat(LENGTH, 1, CV_8U, cv::Scalar(0));
		bufferIndex = 0;
		NowBuffer = Buffer1;
	}
	++bufferIndex;
}

bool Storage::AddFrame(cv::Mat& frame)
{
	if (BufferWriteIndex >= LENGTH)
		return true;

	if (frame.channels() == 1)
		cv::cvtColor(frame, frame, CV_GRAY2BGR);

	cv::Mat Now = NowBuffer;
	cv::Mat oneFrame = NowBuffer(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	oneFrame += frame;

	//指向下一条缓冲
	++BufferWriteIndex;
	if (BufferWriteIndex >= LENGTH)
		return true;
	return false;
}

int Storage::GetFrame(cv::Mat& frame)
{
	//-1下一帧尚未写入完成
	if (BufferReadIndex == BufferWriteIndex && BufferReadIndex != LENGTH)
		return -1;

	if (BufferReadIndex >= LENGTH)
	{
		EndFlag = true;
		return 0;
	}

	++BufferReadIndex;

	if (BufferReadIndex >= LENGTH)
	{
		EndFlag = true;
		return 0;
	}

	cv::Mat Now = NowBuffer;
	cv::Mat oneFrame = NowBuffer(cv::Rect(0, BufferReadIndex, WIDTH, 1));
	oneFrame.copyTo(frame);
	return 1;
}