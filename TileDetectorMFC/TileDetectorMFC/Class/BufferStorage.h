#pragma once
#include "../stdafx.h"
#include <opencv2/opencv.hpp>
using namespace std;


////图像存储类
//class BufferStorage
//{
//public:
//	BufferStorage();
//	~BufferStorage();
//
//	//原始图像
//	cv::Mat OriginalImage;
//	//三合一图像
//	cv::Mat Image;
//};
//








//线阵相机采集时图片写入缓存类
//@author Shawn
class GrabbingBuffer
{
public:
	//1.线阵相机采集行数
	//2.采集宽度
	GrabbingBuffer(int frameCount, int width);
	~GrabbingBuffer();

	//几行合成一行？
	int NinOne = 3;
	int WriteIndex = 0;
	int ReadIndex = 0;

	//原始图像
	cv::Mat OriginalImage;

	//三合一图像
	cv::Mat Image;

	void Start(){
		WriteIndex = 0;
		ReadIndex = 0;
	};
	//从摄像头采入一帧,返回是否停止采集
	bool AddFrame(cv::Mat& frame);
	//将三行叠加一行。
	void ThreeInOne(int lineIndex);
private:

	int _frameCount = 0;
	int _width = 0;
};

