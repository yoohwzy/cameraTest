#include "MicroDisplayControler.h"

int MicroDisplayControler::StartGrabbing(MicroDisplayInit& mdi)
{
	int status = Fg_AcquireEx(mdi.fg, mdi.nCamPort, GRAB_INFINITE, ACQ_STANDARD, mdi.pMem0);
	if (status < 0){
		return status;
	}
	return 0;
}
void MicroDisplayControler::EndGrabbing(MicroDisplayInit& mdi)
{
	Fg_stopAcquireEx(mdi.fg, mdi.nCamPort, mdi.pMem0, 0);
}
int MicroDisplayControler::FreeRunning(MicroDisplayInit& mdi, BufferStorage *s)
{
	int status = 0;
	status = MicroDisplayInit::InitParameter(mdi);
	if (status < 0)
	{
		MD_ErrorMessageWait(mdi.fg);
		return -1;
	}

	//采集mdi.MaxPics行图像到缓存
	status = MicroDisplayControler::StartGrabbing(mdi);
	if (status < 0)
	{
		MD_ErrorMessageWait(mdi.fg);
		return -1;
	}


	frameindex_t lastPicNr = 0;
	cv::Mat OriginalImage;
	do{
		lastPicNr = Fg_getLastPicNumberBlockingEx(mdi.fg, lastPicNr + 1, mdi.nCamPort, 100, mdi.pMem0);
		if (lastPicNr < 0){
			int status = 0;
			MD_ErrorMessageWait(mdi.fg);
			Fg_stopAcquireEx(mdi.fg, mdi.nCamPort, mdi.pMem0, 0);
			Fg_FreeMemEx(mdi.fg, mdi.pMem0);
			Fg_FreeGrabber(mdi.fg);
			CloseDisplay(mdi.nId);
			return -1;
		}
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(mdi.fg, lastPicNr, 0, mdi.pMem0);
		if (mdi.nId != -1)
			::DrawBuffer(mdi.nId, Fg_getImagePtrEx(mdi.fg, lastPicNr, 0, mdi.pMem0), (int)lastPicNr, "");
		if (mdi.colorType == mdi.GRAY)
			OriginalImage = cv::Mat(mdi.Height, mdi.Width, CV_8U, bytePtr);
		else
			OriginalImage = cv::Mat(mdi.Height, mdi.Width, CV_8UC3, bytePtr);
	} while (!s->AddFrame(OriginalImage));
	MicroDisplayControler::EndGrabbing(mdi);
	MicroDisplayInit::Release(mdi);
	//功能主循环END
	return 0;
}
int MicroDisplayControler::Test(MicroDisplayInit& mdi)
{
	int status = MicroDisplayInit::InitParameter(mdi);
	if (status >= 0)
	{
		MicroDisplayInit::Release(mdi);
	}
	return status;
}
int MicroDisplayControler::FreeRunningFocusing(MicroDisplayInit& mdi)
{
	printf_globle("开始对焦，按1放大，按2缩小，按3退出。\n");
	//采集mdi.MaxPics行图像到缓存
	if (MicroDisplayControler::StartGrabbing(mdi) < 0)
	{
		MD_ErrorMessageWait(mdi.fg);
		return -1;
	}
	cv::Mat OriginalImage;
	int resize = mdi.Width;
	frameindex_t lastPicNr = 0;
	cv::namedWindow("SHOW", CV_WINDOW_AUTOSIZE);
	int pos = 0;
	int realPos = 0;
	const int maxpos = 80;
	cv::createTrackbar("OFF SET / %", "SHOW", &pos, maxpos);
	do{
		lastPicNr = Fg_getLastPicNumberBlockingEx(mdi.fg, lastPicNr + 1, mdi.nCamPort, 100, mdi.pMem0);
		if (lastPicNr < 0){
			int status = 0;
			MD_ErrorMessageWait(mdi.fg);
			Fg_stopAcquireEx(mdi.fg, mdi.nCamPort, mdi.pMem0, 0);
			Fg_FreeMemEx(mdi.fg, mdi.pMem0);
			Fg_FreeGrabber(mdi.fg);
			CloseDisplay(mdi.nId);

			return 0;
		}
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(mdi.fg, lastPicNr, 0, mdi.pMem0);
		//if (mdi.nId != -1)
		//	::DrawBuffer(mdi.nId, Fg_getImagePtrEx(mdi.fg, lastPicNr, 0, mdi.pMem0), (int)lastPicNr, "");
		if (mdi.colorType == mdi.GRAY)
			OriginalImage = cv::Mat(mdi.Height, mdi.Width, CV_8U, bytePtr);
		else
			OriginalImage = cv::Mat(mdi.Height, mdi.Width, CV_8UC3, bytePtr);
		if (resize != mdi.Width)
			cv::resize(OriginalImage, OriginalImage, cv::Size(resize, mdi.Height));
		realPos = resize / 100 * pos;
		cv::Mat ROI = OriginalImage(cv::Rect(realPos, 0, resize - realPos, mdi.Height));
		cv::imshow("SHOW", ROI);
		char t = cv::waitKey(20);
		if (t == '3')
			break;
		if (t == '2' && resize > 800)
			resize /= 2;
		if (t == '1' && resize <  mdi.Width * 3)
			resize *= 2;

	} while (true);
	MicroDisplayControler::EndGrabbing(mdi);
	cv::destroyAllWindows();
	//功能主循环END
	return 0;
}