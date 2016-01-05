#pragma once

#include <opencv2/opencv.hpp>

class E2VBuffer
{
public:
	E2VBuffer(int width,bool isGray = false);
	~E2VBuffer()
	{

	};
	//�Ҷ�ģʽ����true����ɫģʽ����false
	bool ColorModelIsGray(){ return colorIsGray; };
	void WriteBuffer(cv::Mat oneline);
	cv::Mat Buffer;
private:
	int writeIndex = 0;
	bool colorIsGray = false;
};


