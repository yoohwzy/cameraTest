#pragma once
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"

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
	//从缓存读出一帧，赋值给frame,返回 0 到底,1 继续,-1下一帧尚未写入完成
	int GetFrame(cv::Mat& frame);
	//将三行叠加一行。
	void ThreeInOne(int lineIndex);
private:

	int MaxPics = 0;
	int width = 0;
	//cv::Mat SamplesRGB;//一行用于光照矫正
};