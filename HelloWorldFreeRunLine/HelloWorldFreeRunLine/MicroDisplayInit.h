#pragma once
#include "globle.h"


#define CONFIG_FILENAME "anOriginalConfigFileName.mcf"

// 相机参数初始化类，存储相机设定参数，并初始化采集卡与相机
class MicroDisplayInit
{
private:
	static int getNrOfBoards();
	static int getBoardInfo();
public:
	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	// Number of memory buffer
	int nr_of_buffer = 8;
	// Board Number
	int nBoard = 0;
	// Port (PORT_A / PORT_B / PORT_C / PORT_D)
	int nCamPort = PORT_A;
	// Number of images to grab
	int MaxPics = 10000;
	int colorType = 0;
	// 看图窗口ID
	int nId = -1;

	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
	// Setting the image size
	int width = 4096;
	int height = 1;



	static int Init(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	//加载配置文件初始化
	static int InitLoad(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	//参数初始化
	static int InitParameter(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	//创建内存  不弄窗口
	static void CreateBufferWithOutDiplay(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	//创建内存与可视化窗口
	static void CreateBufferWithDiplay(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	//开始采集
	static int StartGrabbing(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	//结束采集
	static void EndGrabbing(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);


	static int getNoOfBitsFromImageFormat(const int format);
};

