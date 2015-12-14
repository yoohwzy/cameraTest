#pragma once


#include "../../stdafx.h"
#include "../../globle_debug.h"
#include "../BufferStorage.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include <opencv2/opencv.hpp>


class E2VCamera
{
public:
	//初始化采集参数
	//1.采集行数
	//2.采集宽度
	//3.色彩模式 枚举 RGB OR GRAY
	//4.采集卡编号Logical number of the board.（默认为0）
	//5.采集卡Port口号，PORT_A(默认) OR PORT_B
	E2VCamera(int frameCount, int width, int colorType = RGB, int boardID = 0, int Camport = PORT_A);
	~E2VCamera();

	//开始采图
	void Capture(GrabbingBuffer *gb);


	//测试采集卡-相机功能是否正常，若有错直接报错退出！
	static bool TestCam();


	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:

	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//每帧宽度
	const int _frameHeight = 1;		//每帧高度
	int _frameCount = 0;			//总共帧数

	int nBoard = 0;					//采集卡编号
	int camPort = PORT_A;			//采集卡Port

	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer
	
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






	//10000帧图像应多少微秒采集
	const int _GrubbingTimeMicroSecondPer10000f = 1000000;
	//1帧图像采集用时
	const int _GrubbingTimeMicroSecondPer1f = 100;
};