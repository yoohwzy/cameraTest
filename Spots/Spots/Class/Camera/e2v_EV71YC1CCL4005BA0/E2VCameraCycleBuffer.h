#pragma once

#include "E2VCameraModel.h"


using namespace std;

//E2V相机的循环缓存
class E2VCycleBuffer
{
public:
	//创建线阵相机循环缓存
	//参数：图像宽度（4k），是否为灰度图像（bool）
	E2VCycleBuffer(int width, bool isGray = false);
	//E2VCycleBuffer(int width, int length = 20000, bool isGray = false);
	~E2VCycleBuffer()
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
	////从循环缓存中读取图片，从起始索引到结束索引
	//void ReadData(int startIndex, int endIndex);
	cv::Mat Buffer;
private:
	int colorType = CV_8U;
	int WriteIndex = 0;
};
























//E2V相机的操作类，为循环缓存采图版本
//@author VShawn
class E2VCameraCycleBuffer :public E2VCameraModel
{
public:
	//初始化采集参数
	//1.采集图像宽度
	//2.色彩模式 枚举 RGB OR GRAY
	//3.每帧时长（微秒）要求大于30，默认为0
	//4.采集卡编号Logical number of the board.（默认为0）
	//5.采集卡Port口号，PORT_A(默认) OR PORT_B
	E2VCameraCycleBuffer(int width, int colorType = RGB, int frameTimeUS = 0, int boardID = 0, int Camport = PORT_A);
	~E2VCameraCycleBuffer();

	E2VCycleBuffer *P_CycleBuffer = NULL;
	//每帧时长（微秒）要求大于30，默认为0
	int FrameTimeUS = 0;
private:

	bool StartFlag = true;

	//开始循环采图
	bool freeRun();
};