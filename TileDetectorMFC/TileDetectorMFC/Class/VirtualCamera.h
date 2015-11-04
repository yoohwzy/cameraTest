#pragma once

#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "MicroDisplay\MicroDisplayInit.h"
#include "BufferStorage.h"

//@description 虚拟相机类，用于读取一张图片作为底片，在程序脱机测试时使用。
//@author VShawn
//@last modify date 2015-9-30 16:55:05 By VShawn
class VirtualCamera
{
public:
	VirtualCamera();
	VirtualCamera(MicroDisplayInit& mdi);
	//初始化虚拟相机，若图片文件不存在，报错并退出。
	VirtualCamera(MicroDisplayInit& mdi,string imgname);
	~VirtualCamera();
	//采样停止标志
	bool EndFlag = false;
	//循环采图 直到采够为止
	int FreeRunning(MicroDisplayInit& mdi, BufferStorage& s);
	cv::Mat buffer;
private:
	int WIDTH = 0;
	int BufferLength;//缓冲长度
	cv::Mat GetNext();
	int BufferIndex = 0;//当前传输到第几线
};