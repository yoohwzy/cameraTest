#include "MicroDisplayInit.h"

// 初始化采集卡与摄像机
// 首先尝试读取mcf初始化，失败后手动配置参数初始化
// 返回Status;
int MicroDisplayInit::Init(Fg_Struct **fg, dma_mem **pMem0)
{
	int status = 0;



	return status;
}
// 初始化采集卡与摄像机
// 读取mcf初始化，参数文件定义在头文件 CONFIG_FILENAME
// 返回Status;
int MicroDisplayInit::InitLoad(Fg_Struct **fg, dma_mem **pMem0)
{
	int status = 0;



	return status;
}
// 初始化采集卡与摄像机
// 设置参数初始化
// 返回Status;
int MicroDisplayInit::InitParameter(Fg_Struct **fg, dma_mem **pMem0)
{
	int status = 0;
	MicroDisplayInit mdi;

	char debugInfo[256];


	// Initialization of the microEnable frame grabber
	if (((*fg) = Fg_Init(mdi.dllName, mdi.nBoard)) == NULL) {
		return status;
	}
	OutPutDebugInfo("Init Grabber ok");

	//设置参数
	if (Fg_setParameter((*fg), FG_WIDTH, &mdi.width, mdi.nCamPort) < 0) {
		return status;
	}
	if (Fg_setParameter((*fg), FG_HEIGHT, &mdi.height, mdi.nCamPort) < 0) {
		return status;
	}
	int bitAlignment = FG_LEFT_ALIGNED;
	if (Fg_setParameter((*fg), FG_BITALIGNMENT, &bitAlignment, mdi.nCamPort) < 0) {
		return status;
	}
	sprintf(debugInfo, "Set Image Size on port %d (w: %d,h: %d) ok", mdi.nCamPort, mdi.width, mdi.height);
	OutPutDebugInfo(debugInfo);


	// Memory allocation
	int format = 0;
	Fg_getParameter((*fg), FG_FORMAT, &format, mdi.nCamPort);
	size_t bytesPerPixel = 1;
	switch (format){
	case FG_GRAY:	bytesPerPixel = 1; break;
	case FG_GRAY16:	bytesPerPixel = 2; break;
	case FG_COL24:	bytesPerPixel = 3; break;
	case FG_COL32:	bytesPerPixel = 4; break;
	case FG_COL30:	bytesPerPixel = 5; break;
	case FG_COL48:	bytesPerPixel = 6; break;
	}
	size_t totalBufSize = mdi.width*mdi.height*mdi.nr_of_buffer*bytesPerPixel;
	if (((*pMem0) = Fg_AllocMemEx((*fg), totalBufSize, mdi.nr_of_buffer)) == NULL){
		return status;
	}
	else {
		sprintf(debugInfo, "%d framebuffer allocated for port %d ok", mdi.nr_of_buffer, mdi.nCamPort);
		OutPutDebugInfo(debugInfo);
	}
	return status;
}

// Creating a display window for image output
int MicroDisplayInit::CreateDiplay(Fg_Struct **fg, dma_mem **pMem0)
{
	int status = 0;
	MicroDisplayInit mdi;

	char debugInfo[256];



	int format = 0;
	Fg_getParameter((*fg), FG_FORMAT, &format, mdi.nCamPort);
	size_t bytesPerPixel = 1;
	switch (format){
	case FG_GRAY:	bytesPerPixel = 1; break;
	case FG_GRAY16:	bytesPerPixel = 2; break;
	case FG_COL24:	bytesPerPixel = 3; break;
	case FG_COL32:	bytesPerPixel = 4; break;
	case FG_COL30:	bytesPerPixel = 5; break;
	case FG_COL48:	bytesPerPixel = 6; break;
	}
	// Creating a display window for image output
	int Bits = getNoOfBitsFromImageFormat(format);

	int nId = ::CreateDisplay(Bits, mdi.width, mdi.height);
	SetBufferWidth(nId, mdi.width, mdi.height);

	if ((Fg_AcquireEx((*fg), mdi.nCamPort, GRAB_INFINITE, ACQ_STANDARD, (*pMem0))) < 0){
		return status;
	}
	return status;
}

// returns the bit with according to the selected image format
int MicroDisplayInit::getNoOfBitsFromImageFormat(const int format)
{
	int Bits = 0;
	switch (format){
	case FG_GRAY:
		Bits = 8;
		break;
	case FG_GRAY16:
		Bits = 16;
		break;
	case FG_COL24:
	case FG_COL48:
		Bits = 24;
		break;
	default:
		Bits = 8;
		break;
	};
	return Bits;
}