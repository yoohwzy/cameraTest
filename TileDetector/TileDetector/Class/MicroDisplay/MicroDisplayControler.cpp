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
int MicroDisplayControler::FreeRunning(MicroDisplayInit& mdi, BufferStorage& s)
{
	//采集mdi.MaxPics行图像到缓存
	if (MicroDisplayControler::StartGrabbing(mdi) < 0)
	{
		ErrorMessageWait(mdi.fg);
		return -1;
	}
	frameindex_t lastPicNr = 0;
	cv::Mat OriginalImage;
	do{
		lastPicNr = Fg_getLastPicNumberBlockingEx(mdi.fg, lastPicNr + 1, mdi.nCamPort, 100, mdi.pMem0);
		if (lastPicNr < 0){
			int status = 0;
			status = ErrorMessageWait(mdi.fg);
			Fg_stopAcquireEx(mdi.fg, mdi.nCamPort, mdi.pMem0, 0);
			Fg_FreeMemEx(mdi.fg, mdi.pMem0);
			Fg_FreeGrabber(mdi.fg);
			CloseDisplay(mdi.nId);
			return 0;
		}
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(mdi.fg, lastPicNr, 0, mdi.pMem0);
		if (mdi.nId != -1)
			::DrawBuffer(mdi.nId, Fg_getImagePtrEx(mdi.fg, lastPicNr, 0, mdi.pMem0), (int)lastPicNr, "");
		if (mdi.colorType == mdi.GRAY)
			OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8U, bytePtr);
		else
			OriginalImage = cv::Mat(mdi.height, mdi.width, CV_8UC3, bytePtr);
	} while (!s.AddFrame(OriginalImage));
	MicroDisplayControler::EndGrabbing(mdi);
	//功能主循环END
	return 0;
}