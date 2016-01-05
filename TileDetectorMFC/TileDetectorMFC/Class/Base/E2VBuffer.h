#pragma once

#include <opencv2/opencv.hpp>

class E2VBuffer
{
public:
	E2VBuffer(int width,bool isGray = false);
	~E2VBuffer()
	{

	};
	//灰度模式返回true，彩色模式返回false
	bool ColorModelIsGray(){ return colorIsGray; };
	void WriteBuffer(cv::Mat oneline);
	cv::Mat Buffer;
private:
	int writeIndex = 0;
	bool colorIsGray = false;
};


