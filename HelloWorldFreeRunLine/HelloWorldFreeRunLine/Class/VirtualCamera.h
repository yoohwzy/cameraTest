#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h"
#include "BufferStorage.h"

class VirtualCamera
{
public:
	VirtualCamera();
	VirtualCamera(MicroDisplayInit& mdi);
	VirtualCamera(MicroDisplayInit& mdi,string imgname);
	~VirtualCamera();
	//采样停止标志
	bool EndFlag = false;
	//循环采图 直到采够为止
	int FreeRunning(MicroDisplayInit& mdi, BufferStorage& s);
private:
	int WIDTH = 0;
	int BufferLength;//缓冲长度
	cv::Mat buffer;
	cv::Mat GetNext();
	int BufferIndex = 0;//当前传输到第几线
};