#include "BufferStorage.h"



BufferStorage::BufferStorage(int _MaxPics, int _width)
{
	MaxPics = _MaxPics;
	width = _width;
}
BufferStorage::~BufferStorage()
{
	Buffer.release();
//	BufferGray.release();
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
	cv::Mat Now = Buffer;
	cv::Mat oneFrame = Buffer(cv::Rect(0, BufferWriteIndex, width, 1));
	oneFrame += frame;

	//���źϳ�һ��
	//ThreeInOne(BufferWriteIndex);
	//ָ����һ������
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

	////���ս���
	//if (SamplesRGB.cols != 0)
	//	frame = frame + SamplesRGB;


	//N��ͼ�����
	cv::Mat oneFrame = BufferImg(cv::Rect(0, lineIndex, width, 1));
	oneFrame += frame;
	if (lineIndex == 0)//������һ�ڶ���û�õ����ӵ����
	{
		oneFrame += frame;
		oneFrame += frame;
	}
	if (lineIndex == 1)//������һ�ڶ���û�õ����ӵ����
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
//int BufferStorage::GetFrame(cv::Mat& frame)
//{
//	//-1��һ֡��δд�����
//	if (BufferReadIndex == BufferWriteIndex && BufferReadIndex != MaxPics)
//		return -1;
//
//	if (BufferReadIndex >= MaxPics)
//	{
//		EndReadFlag = true;
//		return 0;
//	}
//
//	++BufferReadIndex;
//
//	if (BufferReadIndex >= MaxPics)
//	{
//		EndReadFlag = true;
//		return 0;
//	}
//
//	cv::Mat Now = Buffer;
//	cv::Mat oneFrame = Buffer(cv::Rect(0, BufferReadIndex, width, 1));
//	oneFrame.copyTo(frame);
//	return 1;
//}