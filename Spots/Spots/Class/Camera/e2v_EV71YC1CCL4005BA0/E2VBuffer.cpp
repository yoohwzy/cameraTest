#include "E2VBuffer.h"


E2VBuffer::E2VBuffer(int width, bool isGray)
{
	colorType = isGray ? CV_8U : CV_8UC3;
	if (isGray)
		Buffer = cv::Mat(BufferLength, width, colorType, cv::Scalar(0, 0, 0));
	else
		Buffer = cv::Mat(BufferLength, width, colorType, cv::Scalar(0, 0, 0));
}
//E2VBuffer::E2VBuffer(int width, int length, bool isGray)
//{
//	colorIsGray = isGray;
//	if (isGray)
//		Buffer = cv::Mat(length, width, CV_8U, cv::Scalar(0, 0, 0));
//	else
//		Buffer = cv::Mat(length, width, CV_8UC3, cv::Scalar(0, 0, 0));
//}


void E2VBuffer::WriteData(cv::Mat oneline)
{
	//double t1 = (double)cv::getTickCount();//采图用时 微秒
	cv::Mat roi = Buffer(cv::Rect(0, WriteIndex, Buffer.cols, 1));

	//到底如何才快？
	//cv::addWeighted(roi, 0, oneline, 1.0, 0, roi);  //debug 48微秒 release 5.28微秒
	oneline.copyTo(roi);	//debug 2微秒 release 1.5微秒


	++WriteIndex;
	if (WriteIndex >= 20000)
		WriteIndex = 0;

	//t1 = ((double)cv::getTickCount() - t1) * 1000000 / cv::getTickFrequency();
	//std::cout << t1 << endl;
}
