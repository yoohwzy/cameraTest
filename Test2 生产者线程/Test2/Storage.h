#pragma once

#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

using namespace std;

class Storage
{
public: 
	Storage(){};
	Storage(int width,int length);
	~Storage() ;

	int BufferWriteIndex = 0;
	int BufferReadIndex = 0;

	cv::Mat Buffer0;
	cv::Mat Buffer0Gray;
	cv::Mat Buffer0Img;
	//cv::Mat Buffer0Flag;//缓存区锁，表示某一帧状态，初始为0，写入后为1，读取后为2

	cv::Mat Buffer1;
	cv::Mat Buffer1Gray;
	cv::Mat Buffer1Img;
	//cv::Mat Buffer1Flag;//缓存区锁，表示某一帧状态，初始为0，写入后为1，读取后为2

	cv::Mat NowBuffer;//指向当前待处理的缓冲区Buffer0 或 Buffer1
	cv::Mat NowBufferGray;
	cv::Mat NowBufferImg;

	int WIDTH;//线阵相机分辨率
	int LENGTH;//缓存采集长度
	bool EndFlag = false;


	void Start();//开始新一轮采集，设置Read/Write Index = 0;
	bool AddFrame(cv::Mat& frame);//从摄像头采入一帧,返回是否停止采集
	int GetFrame(cv::Mat& frame);//从缓存读出一帧，赋值给frame,返回 0 到底,1 继续,-1下一帧尚未写入完成
private:
	unsigned int bufferIndex = 0;//判断当前正在使用哪一个buffer，每执行一次Start()更换一个Buffer


	//enum StorageChoose
	//{
	//	Storage1,
	//	Sotrage2
	//};
};


