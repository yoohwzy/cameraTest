#include "MicroDisplayInit.h"

// 初始化采集卡与摄像机
// 读取mcf初始化
// 返回Status;
int MicroDisplayInit::InitLoad(MicroDisplayInit& mdi, char * mcfName)
{
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		getBoardInfo();
	}
#endif
	//初始化fg
	int status = MicroDisplayInit::initFG(mdi);
	if ((status = Fg_loadConfig(mdi.fg, mcfName)) < 0) {
		MD_ErrorMessageWait(mdi.fg);
		return -1;
	}
	printf_globle("Load Config File ok\n");
	return MicroDisplayInit::memoryAllocation(mdi);
	return status;
}
// 初始化采集卡与摄像机
// 设置参数初始化
// 返回Status;
int MicroDisplayInit::InitParameter(MicroDisplayInit& mdi)
{
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		getBoardInfo();
	}
#endif

	//初始化fg
	int status = MicroDisplayInit::initFG(mdi);
	char debugInfo[256];
	if (Fg_setParameter(mdi.fg, FG_WIDTH, &mdi.width, mdi.nCamPort) < 0) {
		return status;
	}
	if (Fg_setParameter(mdi.fg, FG_HEIGHT, &mdi.height, mdi.nCamPort) < 0) {
		return status;
	}
	int bitAlignment = FG_LEFT_ALIGNED;
	if (Fg_setParameter(mdi.fg, FG_BITALIGNMENT, &bitAlignment, mdi.nCamPort) < 0) {
		return status;
	}
	//Gain值
	//int _FG_KNEE_LUT_SCALE = 10;
	//if (Fg_setParameter(mdi.fg, FG_KNEE_LUT_SCALE, &_FG_KNEE_LUT_SCALE, mdi.nCamPort) < 0) {
	//	return status;
	//}
	sprintf(debugInfo, "Set Image Size on port %d (w: %d,h: %d) ok\n", mdi.nCamPort, mdi.width, mdi.height);
	printf_globle(debugInfo);

	return MicroDisplayInit::memoryAllocation(mdi);
}

void MicroDisplayInit::CreateBufferWithDiplay(MicroDisplayInit& mdi)
{
	char debugInfo[256];

	int format = 0;
	Fg_getParameter(mdi.fg, FG_FORMAT, &format, mdi.nCamPort);
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

	mdi.nId = ::CreateDisplay(Bits, mdi.width, mdi.height);
	SetBufferWidth(mdi.nId, mdi.width, mdi.height);
}
void MicroDisplayInit::CreateBufferWithOutDiplay(MicroDisplayInit& mdi)
{
	MicroDisplayInit::CreateBufferWithDiplay(mdi);
	CloseDisplay(mdi.nId);
	mdi.nId = -1;
}
void MicroDisplayInit::Release(MicroDisplayInit& mdi)
{
	Fg_FreeMemEx(mdi.fg, mdi.pMem0);
	Fg_FreeGrabber(mdi.fg);
	CloseDisplay(mdi.nId);
}







/*****************PRIVATE****************/
int MicroDisplayInit::initFG(MicroDisplayInit& mdi)
{
	int status = 0;
	// Initialization of the microEnable frame grabber
	if (mdi.colorType == GRAY)
	{
		if ((mdi.fg = Fg_Init(mdi.dllNameGRAY, mdi.nBoard)) == NULL) {
			return status;
		}
		//设置传输模式，设置后才为4k
		int _FG_CAMERA_LINK_CAMTYP = FG_CL_DUALTAP_8_BIT;
		if (Fg_setParameter(mdi.fg, FG_CAMERA_LINK_CAMTYP, &_FG_CAMERA_LINK_CAMTYP, mdi.nCamPort) < 0) {
			return status;
		}
	}
	else
	{
		if ((mdi.fg = Fg_Init(mdi.dllNameRGB, mdi.nBoard)) == NULL) {
			return status;
		}
		//传输模式的调整
		int _FG_CAMERA_LINK_CAMTYP = FG_CL_RGB;
		if (Fg_setParameter(mdi.fg, FG_CAMERA_LINK_CAMTYP, &_FG_CAMERA_LINK_CAMTYP, mdi.nCamPort) < 0) {
			return status;
		}
	}
	printf_globle("Init FG ok\n");
	return status;
}
int MicroDisplayInit::memoryAllocation(MicroDisplayInit& mdi)
{
	int status = 0;
	char debugInfo[256];

	// Memory allocation
	int format = 0;
	Fg_getParameter(mdi.fg, FG_FORMAT, &format, mdi.nCamPort);
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
	if ((mdi.pMem0 = Fg_AllocMemEx(mdi.fg, totalBufSize, mdi.nr_of_buffer)) == NULL){
		return status;
	}
	else {
		sprintf(debugInfo, "%d framebuffer allocated for port %d ok\n", mdi.nr_of_buffer, mdi.nCamPort);
		printf_globle(debugInfo);
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












#pragma region 调试用函数
/***************调试用函数**************/
// 在控制台显示出采集卡信息
int MicroDisplayInit::getNrOfBoards()
{
	int nrOfBoards = 0;
	char buffer[256];
	unsigned int buflen = 256;
	buffer[0] = 0;

	// availability : starting with RT 5.2
	if (Fg_getSystemInformation(NULL, INFO_NR_OF_BOARDS, PROP_ID_VALUE, 0, buffer, &buflen) == FG_OK){
		nrOfBoards = atoi(buffer);
	}
	return nrOfBoards;

}

// get board and camera-link serial port information
// using silicon-software runtime library fglib5
int MicroDisplayInit::getBoardInfo()
{
	int boardType;
	int i = 0;

	int maxNrOfboards = 10;// use a constant no. of boards to query, when evaluations versions minor to RT 5.2
	int nrOfBoardsFound = 0;
	int maxBoardIndex = -1;
	int iPortCount = 0;
	int nrOfBoardsPresent = 0;

	// detect all boards
	nrOfBoardsPresent = getNrOfBoards();
	if (!nrOfBoardsPresent)
		return 0;

	for (i = 0; i < maxNrOfboards; i++) {
		int iPortNrOnBoard = 0;
		const char * boardName;
		bool skipIndex = false;
		boardType = Fg_getBoardType(i);
		switch (boardType) {
		case PN_MICROENABLE4AS1CL:
			boardName = "MicroEnable IV AS1-CL";
			iPortNrOnBoard = 1;
			break;
		case PN_MICROENABLE4AD1CL:
			boardName = "MicroEnable IV AD1-CL";
			iPortNrOnBoard = 1;
			break;
		case PN_MICROENABLE4VD1CL:
			boardName = "MicroEnable IV VD1-CL";
			iPortNrOnBoard = 2;
			break;
		case PN_MICROENABLE4AD4CL:
			boardName = "MicroEnable IV AD4-CL";
			iPortNrOnBoard = 2;
			break;
		case PN_MICROENABLE4VD4CL:
			boardName = "MicroEnable IV VD4-CL";
			iPortNrOnBoard = 2;
			break;
			// ignore the non-cameralink boards
		case PN_MICROENABLE3I:
		case PN_MICROENABLE3IXXL:
		case PN_MICROENABLE4AQ4GE:
		case PN_MICROENABLE4VQ4GE:
		default:
			boardName = "Unknown / Unsupported Board";
			skipIndex = true;
		}
		if (!skipIndex){
			nrOfBoardsFound++;
			maxBoardIndex = i;

			if (iPortNrOnBoard > 0){
				if (i == 0)
					printf_globle("Following serial ports are available:\n");
				for (int j = 0; j < iPortNrOnBoard; j++){
					iPortCount++;
					char debugInfo[256];
					sprintf(debugInfo, "%d. Board_%u %s (%x) Port_%d\n", iPortCount - 1, i, boardName, boardType, j);
					printf_globle(debugInfo);
				}
			}
		}
		else{
		}
		if (nrOfBoardsFound >= nrOfBoardsPresent){
			break;// all boards are scanned
		}
	}
	return iPortCount;
}
#pragma endregion