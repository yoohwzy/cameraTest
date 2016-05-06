#include "E2VBuffer.h"
#include <Class/Debug/MFCConsole.h>


E2VBuffer::E2VBuffer(int width, bool isGray)
{
	colorType = isGray ? CV_8U : CV_8UC3;
	if (isGray)
		Buffer = cv::Mat(BufferLength, width, colorType, cv::Scalar(0, 0, 0));
	else
		Buffer = cv::Mat(BufferLength, width, colorType, cv::Scalar(0, 0, 0));
}

void E2VBuffer::WriteData(cv::Mat oneline)
{
	//double t1 = (double)cv::getTickCount();//²ÉÍ¼ÓÃÊ± Î¢Ãë
	cv::Mat roi = Buffer(cv::Rect(0, WriteIndex, Buffer.cols, 1));

	//µ½µ×ÈçºÎ²Å¿ì£¿
	//cv::addWeighted(roi, 0, oneline, 1.0, 0, roi);  //debug 48Î¢Ãë release 5.28Î¢Ãë
	oneline.copyTo(roi);	//debug 2Î¢Ãë release 1.5Î¢Ãë


	++WriteIndex;
	if (WriteIndex >= 20000)
		WriteIndex = 0;

	//t1 = ((double)cv::getTickCount() - t1) * 1000000 / cv::getTickFrequency();
	//std::cout << t1 << endl;
}

cv::Mat E2VBuffer::GetImage(int startLine, int endLine)
{
	double t = cv::getTickCount();
	//Ô½½ç·µ»Ø¿Õ
	if (startLine >= E2VBuffer::BufferLength ||
		startLine < 0 ||
		endLine >= E2VBuffer::BufferLength ||
		endLine < 0
		)
	{
		return cv::Mat(0, 0, colorType);
	}


	if (endLine >= startLine)
	{
		int length = endLine - startLine + 1;
		cv::Mat result = Buffer(cv::Rect(0, startLine, Buffer.cols, length)).clone();

		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		stringstream ss;
		ss << "GetImage :" << t << "ms " << result.rows << " * " << result.cols << endl;
		MFCConsole::Output(ss.str());
		return result;
	}
	else
	{
		int length1 = (E2VBuffer::BufferLength - startLine);
		int length2 = (endLine + 1);
		cv::Mat result(length1 + length2, Buffer.cols, colorType, cv::Scalar(0, 0, 0));
		cv::Mat roi1 = result(cv::Rect(0, 0, Buffer.cols, length1));
		cv::Mat roi2 = result(cv::Rect(0, length1, Buffer.cols, length2));

		Buffer(cv::Rect(0, startLine, Buffer.cols, length1)).copyTo(roi1);	//debug 2Î¢Ãë release 1.5Î¢Ãë
		Buffer(cv::Rect(0, 0, Buffer.cols, length2)).copyTo(roi2);

		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		stringstream ss;
		ss << "GetImage :" << t << "ms " << result.rows << " * " << result.cols << endl;
		MFCConsole::Output(ss.str());
		return result;
	}
}