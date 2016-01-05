#include "E2VBuffer.h"


E2VBuffer::E2VBuffer(int width, bool isGray)
{
	colorIsGray = isGray;
	if (isGray)
		Buffer = cv::Mat(20000, width, CV_8U, cv::Scalar(0, 0, 0));
	else
		Buffer = cv::Mat(20000, width, CV_8UC3, cv::Scalar(0, 0, 0));
}


void E2VBuffer::WriteBuffer(cv::Mat oneline)
{
	cv::Mat roi = Buffer(cv::Rect(0, writeIndex, Buffer.cols, 1));

	//到底如何才快？
	cv::addWeighted(roi, 0, oneline, 1.0, 0, roi);
	++writeIndex;
}
