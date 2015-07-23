// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "MicroDisplayInit.h"
#include "MicroDisplayStorage.h"
#include <thread>


MicroDisplayStorage s;
int status = 0;

Fg_Struct *fg = 0;
dma_mem *pMem0 = 0;
MicroDisplayInit mdi;


int ErrorMessage(Fg_Struct *fg)
{
	int error = Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	fprintf(stderr, "Error: %d : %s\n", error, err_str);
	return error;
}
int ErrorMessageWait(Fg_Struct *fg)
{
	int error = ErrorMessage(fg);
	printf(" ... press ENTER to continue\n");
	getchar();
	return error;
}

//生产者
void producer()
{
	//MicroDisplayInit::CreateBufferWithDiplay(mdi, &fg, &pMem0);
	//frameindex_t lastPicNr = 0;
	//cv::Mat OriginalImage;
	//do{
	//	lastPicNr = Fg_getLastPicNumberBlockingEx(fg, lastPicNr + 1, mdi.nCamPort, 10, pMem0);
	//	if (lastPicNr < 0){
	//		status = ErrorMessageWait(fg);
	//		Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
	//		Fg_FreeMemEx(fg, pMem0);
	//		Fg_FreeGrabber(fg);
	//		CloseDisplay(nId);
	//		return;
	//	}
	//	unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, lastPicNr, 0, pMem0);

	//	if (mdi.colorType == mdi.GRAY)
	//		OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8U, bytePtr);
	//	else
	//		OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8UC3, bytePtr);
	//} while (!s.AddFrame(OriginalImage));
	//Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
	//Fg_FreeMemEx(fg, pMem0);
	//Fg_FreeGrabber(fg);
	//CloseDisplay(nId);
}





int main()
{
	mdi.colorType = mdi.GRAY;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//采集多少帧图像
	
	s = MicroDisplayStorage(mdi.width, mdi.MaxPics);
	//初始化采集卡
	status = MicroDisplayInit::InitParameter(mdi, &fg, &pMem0);
	if (status < 0)
	{
		ErrorMessageWait(fg);
		return -1;
	}
	MicroDisplayInit::CreateBufferWithOutDiplay(mdi, &fg, &pMem0);
	//MicroDisplayInit::CreateBufferWithDiplay(mdi, &fg, &pMem0);

	int grabbingIndex = 0;
	//主循环
	while (true)
	{
		grabbingIndex += 1;
		char input;
		std::cout << "输入1开始采图，q退出：";
		do{
			input = getchar();
			if (input == 'q')
				return 0;
			Sleep(10);
		} while (input != '1');

		//触发

		//初始化缓存
		s.Start();
		if (MicroDisplayInit::StartGrabbing(mdi, &fg, &pMem0) < 0)
		{
			ErrorMessageWait(fg);
			return -1;
		}


		double t = (double)cv::getTickCount();

		frameindex_t lastPicNr = 0;
		cv::Mat OriginalImage;
		do{
			lastPicNr = Fg_getLastPicNumberBlockingEx(fg, lastPicNr + 1, mdi.nCamPort, 100, pMem0);
			if (lastPicNr < 0){
				status = ErrorMessageWait(fg);
				Fg_stopAcquireEx(fg, mdi.nCamPort, pMem0, 0);
				Fg_FreeMemEx(fg, pMem0);
				Fg_FreeGrabber(fg);
				CloseDisplay(mdi.nId);
				return 0;
			}
			unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, lastPicNr, 0, pMem0);
			if (mdi.nId != -1)
				::DrawBuffer(mdi.nId, Fg_getImagePtrEx(fg, lastPicNr, 0, pMem0), (int)lastPicNr, "");
			if (mdi.colorType == mdi.GRAY)
				OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8U, bytePtr);
			else
				OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8UC3, bytePtr);
		} while (!s.AddFrame(OriginalImage));
		MicroDisplayInit::EndGrabbing(mdi, &fg, &pMem0);



		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cout << mdi.width << "x" << mdi.MaxPics << "：" << t << endl;

		string p1;
		stringstream ss1;
		ss1 << "samples/result" << grabbingIndex << "_o.jpg";
		ss1 >> p1;
		string p2;
		stringstream ss2;
		ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
		ss2 >> p2;
		cv::imwrite(p1, s.NowBuffer);
		//cv::imwrite("result1.jpg", s.NowBufferGray);
		cv::imwrite(p2, s.NowBufferImg);
	}
	Fg_FreeMemEx(fg, pMem0);
	Fg_FreeGrabber(fg);
	CloseDisplay(mdi.nId);
	return 0;
}

