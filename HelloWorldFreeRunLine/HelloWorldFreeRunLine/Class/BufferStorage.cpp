#include "BufferStorage.h"


BufferStorage::BufferStorage(int width, int length)
{
	WIDTH = width;
	LENGTH = length;
}
BufferStorage::~BufferStorage()
{
	Buffer0.release();
	Buffer0Gray.release();
	Buffer0Img.release();

	Buffer1.release();
	Buffer1Gray.release();
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
		Buffer0Gray.release();
		Buffer0Gray = cv::Mat(LENGTH, WIDTH, CV_8U, cv::Scalar(0));
		Buffer0Img.release();
		Buffer0Img = cv::Mat(LENGTH + 10, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		NowBuffer = Buffer0;
		NowBufferGray = Buffer0Gray;
		NowBufferImg = Buffer0Img;
	}
	else
	{
		Buffer1.release();
		Buffer1 = cv::Mat(LENGTH, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		Buffer1Gray.release();
		Buffer1Gray = cv::Mat(LENGTH, WIDTH, CV_8U, cv::Scalar(0));
		Buffer1Img.release();
		Buffer1Img = cv::Mat(LENGTH + 10, WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
		NowBuffer = Buffer1;
		NowBufferGray = Buffer1Gray;
		NowBufferImg = Buffer1Img;
	}
	bufferIndex  = 1 + bufferIndex;
}
bool BufferStorage::AddFrame(cv::Mat& frame)
{
	if (BufferWriteIndex >= LENGTH)
		return true;

	if (frame.channels() == 1)
		cv::cvtColor(frame, frame, CV_GRAY2BGR);

	cv::Mat Now = NowBuffer;
	cv::Mat oneFrame = NowBuffer(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	oneFrame += frame;

	cv::Mat frameGray;
	cv::cvtColor(frame, frameGray, CV_BGR2GRAY);
	oneFrame = NowBufferGray(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	oneFrame += frameGray;

	//double speed = 0.68;
	//int temp = int(speed*(BufferWriteIndex - 1) + 0.5);
	//int BufferWriteIndex2 = int(speed*BufferWriteIndex + 0.5);
	//if (BufferWriteIndex2 != temp)
	//{
	//	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex2, WIDTH, 1));
	//	oneFrame += frame;
	//}

	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex, WIDTH, 1));
	oneFrame += frame;
	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex + 1, WIDTH, 1));
	oneFrame += frame;
	oneFrame = NowBufferImg(cv::Rect(0, BufferWriteIndex + 2, WIDTH, 1));
	oneFrame += frame;

	//if (BufferWriteIndex >= 3)
	//{
	//	cv::Mat DetectImg = NowBufferImg(cv::Rect(0, BufferWriteIndex, WIDTH, 3));
	//	int ddepth = CV_16S;
	//	int scale = 1;
	//	int delta = 0;
	//	double MinNum, MaxNum;
	//	cv::Mat grad, DetectImg_gray;
	//	cv::Mat grad_x, grad_y;
	//	cv::Mat abs_grad_x, abs_grad_y;
	//	cvtColor(DetectImg, DetectImg_gray, CV_RGB2GRAY);

	//	cv::Sobel(DetectImg_gray, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT);
	//	convertScaleAbs(grad_x, abs_grad_x);

	//	cv::Sobel(DetectImg_gray, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT);
	//	convertScaleAbs(grad_y, abs_grad_y);

	//	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	//	cv::minMaxIdx(grad, &MinNum, &MaxNum);
	//	cv::Scalar average = cv::mean(grad);
	//	if (MaxNum > (average[0] + 25))
	//	{
	//		std::cout << "发现缺陷，行数" << BufferWriteIndex;
	//	}
	//	//imwrite("Gradient.jpg", grad);
	//}


	//指向下一条缓冲
	++BufferWriteIndex;
	if (BufferWriteIndex >= LENGTH)
	{
		EndWriteFlag = true;
		return true;
	}
	return false;
}
bool BufferStorage::AddFrame(cv::Mat& frame,int i)
{
	if (i >= LENGTH)
		return true;

	if (frame.channels() == 1)
		cv::cvtColor(frame, frame, CV_GRAY2BGR);

	cv::Mat Now = NowBuffer;
	cv::Mat oneFrame = NowBuffer(cv::Rect(0, i, WIDTH, 1));
	oneFrame += frame;

	cv::Mat frameGray;
	cv::cvtColor(frame, frameGray, CV_BGR2GRAY);
	oneFrame = NowBufferGray(cv::Rect(0, i, WIDTH, 1));
	oneFrame += frameGray;

	oneFrame = NowBufferImg(cv::Rect(0, i, WIDTH, 1));
	oneFrame += frame;
	oneFrame = NowBufferImg(cv::Rect(0, i + 1, WIDTH, 1));
	oneFrame += frame;
	oneFrame = NowBufferImg(cv::Rect(0, i + 2, WIDTH, 1));
	oneFrame += frame;


	//指向下一条缓冲
	++i;
	if (i >= LENGTH)
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