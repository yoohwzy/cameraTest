#pragma once


#include <opencv2/opencv.hpp>

using namespace std;

class E2VBuffer
{
public:
	E2VBuffer(int width, bool isGray = false);
	//E2VBuffer(int width, int length = 20000, bool isGray = false);
	~E2VBuffer()
	{

	};
	static const int BufferLength = 20000;


	//获得OPENCV的色彩模式，
	int GetColorType(){ return colorType; };
	//获取当前的写指针
	int GetWriteIndex(){ return WriteIndex; };

	//将一行写入循环缓存
	void WriteData(cv::Mat oneline);
	//从循环缓存中读取图片，从起始索引到结束索引
	void ReadData(int startIndex,int endIndex);
	cv::Mat Buffer;
private:
	int colorType = CV_8U;
	int WriteIndex = 0;
};


