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

	int nr_of_buffer = 8;			// Number of memory buffer
	int nBoard = 0;			// Board Number
	int nCamPort = PORT_A;		// Port (PORT_A / PORT_B / PORT_C / PORT_D)
	int MaxPics = 10000;		// Number of images to grab
	int colorType = 0;
	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
	// Setting the image size
	int width = 4096;
	int height = 1;



	static int Init(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);
	static int InitLoad(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);//加载配置文件初始化
	static int InitParameter(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);//参数初始化
	static int CreateDiplay(MicroDisplayInit& mdi, Fg_Struct **fg, dma_mem **pMem0);//参数初始化


	static int getNoOfBitsFromImageFormat(const int format);
};

