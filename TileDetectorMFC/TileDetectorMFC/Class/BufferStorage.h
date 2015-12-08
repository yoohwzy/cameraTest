#pragma once
#include "../stdafx.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
using namespace std;
class BufferStorage
{
public:
	BufferStorage(){};
	BufferStorage(int _MaxPics, int _width);
	~BufferStorage();

	//几行合成一行？
	int NinOne = 3;
	int BufferWriteIndex = 0;
	int BufferReadIndex = 0;

	//原始像素
	cv::Mat Buffer;
	//灰度像素
	//cv::Mat Buffer1Gray;
	//三合一图像
	cv::Mat BufferImg;

	bool EndReadFlag = false;
	//为true时表示拍摄完成
	bool EndWriteFlag = false;

	//开始新一轮采集，设置Read/Write Index = 0;
	void Start();
	//从摄像头采入一帧,返回是否停止采集
	bool AddFrame(cv::Mat& frame);
	//将三行叠加一行。
	void ThreeInOne(int lineIndex);
private:

	int MaxPics = 0;
	int width = 0;
};