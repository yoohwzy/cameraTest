#pragma once

#include "../../stdafx.h"
#include "../../globle_debug.h"
#include "../BufferStorage.h"
#include <opencv2/opencv.hpp>

//@description 虚拟相机类，用于读取一张图片作为底片，在程序脱机测试时使用。
//输入文件名只需"XXX.jpg"，程序会自动在virtualcameras文件夹下读取XXX.jpg。
//@author VShawn
//@last modify date 2015-9-30 16:55:05 By VShawn
class VirtualCamera
{
public:
	//初始化虚拟相机，若图片文件不存在，报错并退出。
	VirtualCamera(GrabbingBuffer *gb, int frameCount, int width, string imgname, int colorType = RGB);
	~VirtualCamera()
	{
		buffer.release();
		p_gb = NULL;
	};


	void Capture();

	//测试采集卡-相机功能是否正常。
	static bool TestCam(string imgname);

	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:
	cv::Mat buffer;
	GrabbingBuffer *p_gb;

	// 采图颜色模式
	int _colorType = 1;
	int _width = 0;					//每帧宽度
	const int _frameHeight = 1;		//每帧高度
	int _frameCount = 0;			//总共帧数


	
};