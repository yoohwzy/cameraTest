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


//E2V相机的基类，以此派生出三种采集模式
//@author VShawn
class E2VCamera
{
public:
	//初始化采集参数
	//1.缓存指针，为NULL时自动开辟内存
	//2.采集图像宽度
	//3.采集图像高度
	//4.每帧时长（微秒）要求大于30，默认为0
	//5.色彩模式 枚举 RGB OR GRAY
	//6.采集卡编号Logical number of the board.（默认为0）
	//7.采集卡Port口号，PORT_A(默认) OR PORT_B
	E2VCamera(E2VBuffer *_e2vbuffer, int width, int height = 0, int colorType = RGB, int frameTimeUS = 0, int boardID = 0, int Camport = PORT_A);
	~E2VCamera();

	//开始采图
	virtual bool FreeRun();


	enum ColorType
	{
		GRAY = CV_8U,
		RGB = CV_8UC3
	};

	bool StartFlag = true;

private:
	E2VBuffer *p_e2vbuffer = NULL;
	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer










	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//图像宽度
	int _height = 0;				//图像高度
	const int _frameHeight = 1;		//每帧高度
	int _frameTimeUS = 0;			//每帧时长（微秒）
	int _nBoard = 0;				//采集卡编号
	int _camPort = PORT_A;			//采集卡Port
	int _colorType = 1;				//采图颜色模式



	//初始化fg，返回ErrCode，0为无错误
	//static int init_fg(Fg_Struct *fg, const char *dllName, int nBoard, int camPort);
	bool init_fg();


	// 在控制台显示出采集卡信息
	int getNrOfBoards();
	// get board and camera-link serial port information
	// using silicon-software runtime library fglib5
	int getBoardInfo();

	//创建显示窗口，返回窗口id;
	int createDiplayBuffer();

	//分配内存，返回ErrCode
	bool memoryAllocation();

	//释放内存
	void release();

	//检测MD采集卡是否有错误
	//报错->退出
	void errorMessageWait();
};