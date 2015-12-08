#pragma once


#include "../../stdafx.h"
#include "../../globle_debug.h"
#include "../BufferStorage.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include <opencv2/opencv.hpp>

class MicroDisplay
{
public:
	//初始化采集参数
	//1.采集行数
	//2.采集宽度
	//3.色彩模式 枚举 RGB OR GRAY
	//4.采集卡编号Logical number of the board.（默认为0）
	//5.采集卡Port口号，PORT_A(默认) OR PORT_B
	MicroDisplay(int height, int width,int colorType = RGB,int boardID = 0, int Camport = PORT_A);
	void Capture();

	//测试采集卡-相机功能是否正常。
	static int TestCam();

	~MicroDisplay();
private:
	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//每帧宽度
	const int _frameHeight = 1;		//每帧高度
	int _frameCount = 0;			//总共帧数

	int nBoard = 0;					//采集卡编号
	int camPort = PORT_A;			//采集卡Port

	//frame grabber
	Fg_Struct *fg = NULL;
	//Memory buffer
	dma_mem *memHandle = NULL;

	BufferStorage *s;


	// 采图颜色模式
	int _colorType = 0;
	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};






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