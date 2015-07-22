#pragma once
#include "globle.h"


#define CONFIG_FILENAME "anOriginalConfigFileName.mcf"

class MicroDisplayInit
{
private:

public:
	//const char *dllName = "DualLineGray16.dll";
	const char *dllName = "DualLineRGB30.dll";

	const int nr_of_buffer = 8;			// Number of memory buffer
	const int nBoard = 0;			// Board Number
	const int nCamPort = PORT_A;		// Port (PORT_A / PORT_B / PORT_C / PORT_D)
	const int MaxPics = 10000;		// Number of images to grab
	int status = 0;

	// Setting the image size
	const int width = 4096;
	const int height = 100;


	static int Init(Fg_Struct **fg, dma_mem **pMem0);
	static int InitLoad(Fg_Struct **fg, dma_mem **pMem0);//加载配置文件初始化
	static int InitParameter(Fg_Struct **fg, dma_mem **pMem0);//参数初始化
	static int CreateDiplay(Fg_Struct **fg, dma_mem **pMem0);//参数初始化


	static int getNoOfBitsFromImageFormat(const int format);
};

