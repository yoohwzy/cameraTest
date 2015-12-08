#pragma once
#include "../../stdafx.h"
#include "../../globle_head.h"
#include "../../globle_debug.h"


// 相机参数初始化类，存储相机设定参数，并初始化采集卡与相机
class MicroDisplayInit
{
private:
	static int getNrOfBoards();
	static int getBoardInfo();
	static int memoryAllocation(MicroDisplayInit& mdi);
	static int initFG(MicroDisplayInit& mdi);
public:
	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	Fg_Struct *fg = 0;
	dma_mem *pMem0 = 0;
	// Number of memory buffer
	int nr_of_buffer = 8;
	// Board Number
	int nBoard = 0;
	// Port (PORT_A / PORT_B / PORT_C / PORT_D)
	int nCamPort = PORT_A;
	// Number of images to grab
	int MaxPics = 10002;
	// 看图窗口ID
	int nId = -1;
	// 采图颜色模式
	int colorType = 0;
	enum ColorType 
	{
		GRAY = 0,
		RGB = 1
	};
	// Setting the image size
	int Width = 4096;
	int Height = 10;

	cv::Mat SamplesGray;
	cv::Mat SamplesRGB;

	//加载配置文件初始化
	static int InitLoad(MicroDisplayInit& mdi, char * mcfName = "4096grayline.mcf");
	//参数初始化
	//依次、设置宽度、设置高度、申请内存memoryAllocation
	static int InitParameter(MicroDisplayInit& mdi);
	//创建内存  不弄窗口
	static void CreateBufferWithOutDiplay(MicroDisplayInit& mdi);
	//创建内存与可视化窗口
	static void CreateBufferWithDiplay(MicroDisplayInit& mdi);
	//析构，释放内存，结束程序
	static void Release(MicroDisplayInit& mdi);

	static int getNoOfBitsFromImageFormat(const int format);
};

