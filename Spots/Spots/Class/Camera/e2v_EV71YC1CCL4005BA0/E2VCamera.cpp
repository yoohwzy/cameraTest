#include "E2VCamera.h"


E2VCamera::E2VCamera(E2VBuffer *_e2vbuffer, int width, int frameTimeUS, int colorType, int boardID, int Camport)
{
	p_e2vbuffer = _e2vbuffer;
	_width = width;
	_frameTimeUS = frameTimeUS;
	_colorType = colorType;
	_nBoard = boardID;
	_camPort = Camport;
	
	init_fg();

	//if (_e2vbuffer == NULL)
	//	p_e2vbuffer = new E2VBuffer(width, colorType == GRAY);

	//caculateForTimes();
}
E2VCamera::~E2VCamera()
{
	release();
}
void E2VCamera::FreeRun()
{
	//acquire one image per subbuffer
	//1.fg
	//2.采集卡口
	//3.采集帧数
	//4.？
	//5.缓存地址
	if ((Fg_AcquireEx(fg, _camPort, GRAB_INFINITE, ACQ_STANDARD, memHandle)) < 0) {
		fprintf(stderr, "Fg_AcquireEx() failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}

	frameindex_t fcount = 0;
	frameindex_t last_pic_nr = 0;
	frameindex_t cur_pic_nr;

	cv::Mat OriginalImage;

	//连续运行
	while (StartFlag)
	{
		double t1 = (double)cv::getTickCount();//采图用时 微秒
		cur_pic_nr = Fg_getLastPicNumberBlockingEx(fg, last_pic_nr + 1, _camPort, 100, memHandle);
		if (cur_pic_nr < 0)
		{
			Fg_stopAcquire(fg, _camPort);
			errorMessageWait();
		}
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, cur_pic_nr, 0, memHandle);
		//if (nId != -1)
		//	::DrawBuffer(nId, Fg_getImagePtrEx(fg, lastPicNr, 0, memHandle), (int)lastPicNr, "");

		OriginalImage = cv::Mat(_frameHeight, _width, CV_8UC3, bytePtr);
		if (_colorType == GRAY)
		{
			cv::cvtColor(OriginalImage, OriginalImage, CV_BGR2GRAY);
		}
		p_e2vbuffer->WriteData(OriginalImage);
		t1 = ((double)cv::getTickCount() - t1) * 1000000 / cv::getTickFrequency();


		//等待满_frameTimeUS
		if (t1 < _frameTimeUS)
		{
			double t2 = (double)cv::getTickCount();
			double tickCountSpan = (_frameTimeUS - t1) * cv::getTickFrequency() / 1000000;
			while (((double)cv::getTickCount() - t2) < tickCountSpan)
			{
				//stringstream ss;
				//ss << ((double)cv::getTickCount() - t2) << " " << tickCountSpan << endl;
				//printf_globle(ss.str());
			}
		}
		fcount++;
	}

	Fg_stopAcquireEx(fg, _camPort, memHandle, 0);
}





/**********************私有*********************/
void E2VCamera::test()
{
	init_fg();
	release();
}

void E2VCamera::init_fg()
{
	// 初始化FG

	if ((fg = Fg_Init(dllNameRGB, _nBoard)) == NULL) {
		fprintf(stderr, "Fg_Init() failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}
	//传输模式的调整
	int _FG_CAMERA_LINK_CAMTYP = FG_CL_RGB;
	if (Fg_setParameter(fg, FG_CAMERA_LINK_CAMTYP, &_FG_CAMERA_LINK_CAMTYP, _camPort) < 0) {
		fprintf(stderr, "Fg_setParameter(FG_CAMERA_LINK_CAMTYP) failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}




	// 设置参数
	if (Fg_setParameter(fg, FG_WIDTH, &_width, _camPort) < 0) {
		fprintf(stderr, "Fg_setParameter(FG_WIDTH) failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}
	int h = _frameHeight;//每帧采集图像高度
	if (Fg_setParameter(fg, FG_HEIGHT, &h, _camPort) < 0) {
		fprintf(stderr, "Fg_setParameter(FG_HEIGHT) failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}
	int bitAlignment = FG_LEFT_ALIGNED;
	if (Fg_setParameter(fg, FG_BITALIGNMENT, &bitAlignment, _camPort) < 0) {
		fprintf(stderr, "Fg_setParameter(FG_BITALIGNMENT) failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}

	//创建DMA缓存
	memoryAllocation();

	//显示或不显示E2V自带的显示框
	//createDiplayBuffer();
	//CloseDisplay(createDiplayBuffer());
}


int E2VCamera::createDiplayBuffer()
{
	int format = 0;
	Fg_getParameter(fg, FG_FORMAT, &format, _camPort);
	size_t bytesPerPixel = 1;
	switch (format){
	case FG_GRAY:	bytesPerPixel = 1; break;
	case FG_GRAY16:	bytesPerPixel = 2; break;
	case FG_COL24:	bytesPerPixel = 3; break;
	case FG_COL32:	bytesPerPixel = 4; break;
	case FG_COL30:	bytesPerPixel = 5; break;
	case FG_COL48:	bytesPerPixel = 6; break;
	}
	// returns the bit with according to the selected image format
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
	int dispId0 = ::CreateDisplay(Bits, _width, _frameHeight);
	SetBufferWidth(dispId0, _width, _frameHeight);
	return dispId0;
}

void E2VCamera::memoryAllocation()
{
	// Memory allocation
	int format = 0;
	Fg_getParameter(fg, FG_FORMAT, &format, _camPort);
	size_t bytesPerPixel = 1;
	switch (format){
	case FG_GRAY:	bytesPerPixel = 1; break;
	case FG_GRAY16:	bytesPerPixel = 2; break;
	case FG_COL24:	bytesPerPixel = 3; break;
	case FG_COL32:	bytesPerPixel = 4; break;
	case FG_COL30:	bytesPerPixel = 5; break;
	case FG_COL48:	bytesPerPixel = 6; break;
	}
	int nr_of_buffer = 8;
	size_t totalBufSize = _width*_frameHeight*nr_of_buffer*bytesPerPixel;
	if ((memHandle = Fg_AllocMemEx(fg, totalBufSize, nr_of_buffer)) == NULL){
		fprintf(stderr, "Fg_AllocMemEx() failed: %s\n", Fg_getLastErrorDescription(fg));
		errorMessageWait();
	}
}

void E2VCamera::release()
{
	if (memHandle != NULL)
	{
		Fg_FreeMemEx(fg, memHandle);
		memHandle = NULL;
	}
	if (fg != NULL)
	{
		Fg_FreeGrabber(fg);
		fg = NULL;
	}
}

//报错->退出
void E2VCamera::errorMessageWait()
{
	int error = Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	release();


	printf(err_str);
#ifdef afx_msg
	int dwLen = strlen(err_str) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, err_str, dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpwstr = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, err_str, dwLen, lpwstr, nwLen);
	MessageBox(NULL, lpwstr, L"Error", 0);
#endif
	exit(1);
}


#pragma region 调试用函数
int E2VCamera::getNrOfBoards()
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
int E2VCamera::getBoardInfo()
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
		int iPortNrO_nBoard = 0;
		const char * boardName;
		bool skipIndex = false;
		boardType = Fg_getBoardType(i);
		switch (boardType) {
		case PN_MICROENABLE4AS1CL:
			boardName = "MicroEnable IV AS1-CL";
			iPortNrO_nBoard = 1;
			break;
		case PN_MICROENABLE4AD1CL:
			boardName = "MicroEnable IV AD1-CL";
			iPortNrO_nBoard = 1;
			break;
		case PN_MICROENABLE4VD1CL:
			boardName = "MicroEnable IV VD1-CL";
			iPortNrO_nBoard = 2;
			break;
		case PN_MICROENABLE4AD4CL:
			boardName = "MicroEnable IV AD4-CL";
			iPortNrO_nBoard = 2;
			break;
		case PN_MICROENABLE4VD4CL:
			boardName = "MicroEnable IV VD4-CL";
			iPortNrO_nBoard = 2;
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

			if (iPortNrO_nBoard > 0){
				if (i == 0)
					printf("Following serial ports are available:\n");
				for (int j = 0; j < iPortNrO_nBoard; j++){
					iPortCount++;
					char debugInfo[256];
					sprintf(debugInfo, "%d. Board_%u %s (%x) Port_%d\n", iPortCount - 1, i, boardName, boardType, j);
					printf(debugInfo);
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