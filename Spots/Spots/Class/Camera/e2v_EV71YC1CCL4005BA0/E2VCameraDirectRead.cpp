#include "E2VCameraDirectRead.h"
#include <Class/Debug/MFCConsole.h>
#include <thread>


E2VCameraDirectRead::E2VCameraDirectRead(int width, int height, int colorType, int boardID, int Camport)
	: E2VCameraModel(width, height, colorType, boardID, Camport)
{
}
E2VCameraDirectRead::~E2VCameraDirectRead()
{

}
cv::Mat E2VCameraDirectRead::GetImage()
{
	if (HasBeenInited)
	{
		//acquire one image per subbuffer
		//1.fg
		//2.采集卡口
		//3.采集帧数
		//4.？
		//5.缓存地址
		if ((Fg_AcquireEx(E2VCameraModel::fg, _camPort, 1, ACQ_STANDARD, memHandle)) < 0) {
			fprintf(stderr, "Fg_AcquireEx() failed: %s\n", Fg_getLastErrorDescription(fg));
			errorMessageWait(); 
			return cv::Mat();
		}

		frameindex_t fcount = 0;
		frameindex_t last_pic_nr = 0;
		frameindex_t cur_pic_nr;

		cv::Mat OriginalImage;

		cur_pic_nr = Fg_getLastPicNumberBlockingEx(fg, last_pic_nr + 1, _camPort, 100, memHandle);
		if (cur_pic_nr < 0)
		{
			Fg_stopAcquire(fg, _camPort);
			errorMessageWait(); return cv::Mat();
		}
		unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, cur_pic_nr, 0, memHandle);
		//if (nId != -1)
		//	::DrawBuffer(nId, Fg_getImagePtrEx(fg, lastPicNr, 0, memHandle), (int)lastPicNr, "");

		OriginalImage = cv::Mat(_frameHeight, _width, CV_8UC3, bytePtr).clone();
		if (_colorType == GRAY)
		{
			cv::cvtColor(OriginalImage, OriginalImage, CV_BGR2GRAY);
		}

		fcount++;


		Fg_stopAcquireEx(fg, _camPort, memHandle, 0);
		return OriginalImage;
	}
	else
		return cv::Mat();
}