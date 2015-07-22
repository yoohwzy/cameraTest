// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "MicroDisplayInit.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#define CONFIG_FILENAME "anOriginalConfigFileName.mcf"

int ErrorMessage(Fg_Struct *fg)
{
	int			error = Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	fprintf(stderr, "Error: %d : %s\n", error, err_str);
	return error;
}
int ErrorMessageWait(Fg_Struct *fg)
{
	int			error = ErrorMessage(fg);
	printf(" ... press ENTER to continue\n");
	getchar();
	return error;
}

static int getNrOfBoards()
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
int getBoardInfo()
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
					printf("Following serial ports are available:\n");
				for (int j = 0; j < iPortNrOnBoard; j++){
					iPortCount++;
					printf("%d. Board_%u %s (%x) Port_%d\n", iPortCount - 1, i, boardName, boardType, j);
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

int main()
{

	getBoardInfo();
	cv::Mat OriginalImage;
	int status = 0;
	int MaxPics = 10000;		// Number of images to grab

	Fg_Struct *fg = 0;
	dma_mem *pMem0 = 0;
	MicroDisplayInit mdi;
	status = MicroDisplayInit::InitParameter(&fg, &pMem0);
	if (status < 0)
	{
		ErrorMessageWait(fg);
		return -1;
	}
	int nId = MicroDisplayInit::CreateDiplay(&fg, &pMem0);

	// ====================================================
	// MAIN LOOP
	frameindex_t lastPicNr = 0;
	while ((lastPicNr = Fg_getLastPicNumberBlockingEx(fg, lastPicNr + 1, mdi.nCamPort, 10, pMem0))< MaxPics) {
		if (lastPicNr <0){
			status = ErrorMessageWait(fg);
			Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
			Fg_FreeMemEx(fg, pMem0);
			Fg_FreeGrabber(fg);
			CloseDisplay(nId);
			return status;
		}
		::DrawBuffer(nId, Fg_getImagePtrEx(fg, lastPicNr, 0, pMem0), (int)lastPicNr, "");
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, lastPicNr, 0,pMem0);
		OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8UC3, bytePtr);
		cv::namedWindow("image");
		cv::imshow("image", OriginalImage);
		cv::waitKey(5);
	}

	// ====================================================
	// Freeing the grabber resource
	Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
	Fg_FreeMemEx(fg, pMem0);
	Fg_FreeGrabber(fg);
	CloseDisplay(nId);
	return 0;
}

