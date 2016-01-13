#pragma once


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include <opencv2/opencv.hpp>

#include "E2VBuffer.h"
using namespace std;

class E2VCamera
{
public:
	//初始化采集参数
	//1.缓存指针，为NULL时自动开辟内存
	//2.采集宽度
	//3.每帧时长（微秒）要求大于30
	//4.色彩模式 枚举 RGB OR GRAY
	//5.采集卡编号Logical number of the board.（默认为0）
	//6.采集卡Port口号，PORT_A(默认) OR PORT_B
	E2VCamera(E2VBuffer *_e2vbuffer, int width, int frameTimeUS, int colorType = RGB, int boardID = 0, int Camport = PORT_A);
	~E2VCamera();

	//开始采图
	void FreeRun();


	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};

	bool StartFlag = true;
private:
	E2VBuffer *p_e2vbuffer = NULL;
	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer










	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//每帧宽度
	const int _frameHeight = 1;		//每帧高度
	int _frameTimeUS = 0;			//每帧时长（微秒）
	int _nBoard = 0;				//采集卡编号
	int _camPort = PORT_A;			//采集卡Port
	int _colorType = 1;				//采图颜色模式


	void test();

	// 在控制台显示出采集卡信息
	int getNrOfBoards();
	// get board and camera-link serial port information
	// using silicon-software runtime library fglib5
	int getBoardInfo();


	//初始化fg，返回ErrCode，0为无错误
	//static int init_fg(Fg_Struct *fg, const char *dllName, int nBoard, int camPort);
	void init_fg();

	//创建显示窗口，返回窗口id;
	int createDiplayBuffer();

	//分配内存，返回ErrCode
	void memoryAllocation();

	//释放内存
	void release();

	//检测MD采集卡是否有错误
	//报错->退出
	void errorMessageWait();
};