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
	int GetWriteIndex()
	{ 
		return WriteIndex; 
	};
	//根据起始行索引，获取一副图像
	//例，输入startLine = 0,endLine = 2，将获得一副0行开始，长度为3像素的图像
	//输入startLine = 19000,endLine = 0，将获得一副19000行开始，长度为1001像素的图像
	cv::Mat GetImage(int startLine, int endLine);


	//将一行写入循环缓存
	void WriteData(cv::Mat oneline);
	//从循环缓存中读取图片，从起始索引到结束索引
	void ReadData(int startIndex,int endIndex);
	cv::Mat Buffer;
private:
	int colorType = CV_8U;
	int WriteIndex = 0;
};


