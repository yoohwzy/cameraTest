#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h"

class BufferStorage
{
public:
	BufferStorage(){};
	BufferStorage(MicroDisplayInit& mditmp);
	~BufferStorage();

	int BufferWriteIndex = 0;
	int BufferReadIndex = 0;

	//原始像素
	cv::Mat Buffer0;
	//灰度像素
	//cv::Mat Buffer0Gray;
	//三合一图像
	cv::Mat Buffer0Img;
	//cv::Mat Buffer0Flag;//缓存区锁，表示某一帧状态，初始为0，写入后为1，读取后为2

	//原始像素
	cv::Mat Buffer1;
	//灰度像素
	//cv::Mat Buffer1Gray;
	//三合一图像
	cv::Mat Buffer1Img;

	//原始像素
	cv::Mat NowBuffer;//指向当前待处理的缓冲区Buffer0 或 Buffer1
	//灰度像素
	//cv::Mat NowBufferGray;
	//三合一图像
	cv::Mat NowBufferImg;

	bool EndReadFlag = false;
	//为true时表示拍摄完成
	bool EndWriteFlag = false;

	//开始新一轮采集，设置Read/Write Index = 0;
	void Start();
	//从摄像头采入一帧,返回是否停止采集
	bool AddFrame(cv::Mat& frame);
	//从缓存读出一帧，赋值给frame,返回 0 到底,1 继续,-1下一帧尚未写入完成
	int GetFrame(cv::Mat& frame);
private:
	MicroDisplayInit mdi;
	int bufferIndex = 0;//判断当前正在使用哪一个buffer，每执行一次Start()更换一个Buffer
	int WIDTH = 4096;//线阵相机分辨率
	int LENGTH = 1000;//缓存采集长度
};

