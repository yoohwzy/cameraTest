// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "MicroDisplayInit.h"
#include "MicroDisplayStorage.h"


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


MicroDisplayStorage s;
int main()
{
	int status = 0;

	Fg_Struct *fg = 0;
	dma_mem *pMem0 = 0;
	MicroDisplayInit mdi;
	mdi.colorType = mdi.GRAY;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//采集多少帧图像
	
	s = MicroDisplayStorage(mdi.width, mdi.MaxPics);
	s.Start();

	status = MicroDisplayInit::InitParameter(mdi, &fg, &pMem0);
	if (status < 0)
	{
		ErrorMessageWait(fg);
		return -1;
	}
	int nId = MicroDisplayInit::CreateDiplay(mdi, &fg, &pMem0);

	double t = (double)cv::getTickCount();
	// ====================================================
	// MAIN LOOP
	frameindex_t lastPicNr = 0;
	while ((lastPicNr = Fg_getLastPicNumberBlockingEx(fg, lastPicNr + 1, mdi.nCamPort, 10, pMem0)) < mdi.MaxPics) {
		if (lastPicNr < 0){
			status = ErrorMessageWait(fg);
			Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
			Fg_FreeMemEx(fg, pMem0);
			Fg_FreeGrabber(fg);
			CloseDisplay(nId);
			return status;
		}
		::DrawBuffer(nId, Fg_getImagePtrEx(fg, lastPicNr, 0, pMem0), (int)lastPicNr, "");
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, lastPicNr, 0, pMem0);

		//cv::Mat OriginalImage;
		//if (mdi.colorType == mdi.GRAY)
		//	OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8U, bytePtr);
		//else
		//	OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8UC3, bytePtr);
		//s.AddFrame(OriginalImage, lastPicNr);
	}
	// ====================================================
	// Freeing the grabber resource

	for (size_t i = 0; i < mdi.MaxPics; i++)
	{
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, i, 0, pMem0);
		cv::Mat OriginalImage;
		if (mdi.colorType == mdi.GRAY)
			OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8U, bytePtr);
		else
			OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8UC3, bytePtr);
		s.AddFrame(OriginalImage, lastPicNr);
	}


	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << mdi.width << "x" << mdi.MaxPics << "：" << t << endl;


	Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
	Fg_FreeMemEx(fg, pMem0);
	Fg_FreeGrabber(fg);
	CloseDisplay(nId);

	cv::imwrite("result.jpg", s.NowBuffer);
	//cv::imwrite("result1.jpg", s.NowBufferGray);
	cv::imwrite("result2.jpg", s.NowBufferImg);
	return 0;
}

