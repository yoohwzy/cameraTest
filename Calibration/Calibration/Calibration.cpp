// Calibration.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"

#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include "Class\BlocksDetector.h"
#include "Class\Measurer.h"

#include <thread>


BufferStorage s_Standard;
BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

int _tmain(int argc, _TCHAR* argv[])
{
	mdi.colorType = mdi.RGB;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//�ɼ�����֡ͼ��
	s_Standard = BufferStorage(mdi);


	int tileCalibrationWidth = 300;
	int tileCalibrationHeight = 600;
	cv::Mat pincushionCalibrationImg = cv::imread("PincushionDistortion.png");
	cv::Mat tileThreshold = cv::imread("E14���ʶ�ֵ��_x3.jpg");
	s_Standard.NowBufferImg = tileThreshold;

	double t = 0;
	//��ש��Ե���
	BlocksDetector bd_Standard = BlocksDetector(&s_Standard, &mdi);
	t = (double)cv::getTickCount();
	bd_Standard.Start();
	bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	bd_Standard.StartUP_DOWN(BlocksDetector::Down);

	bd_Standard.ABCD();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "��Ե���ʱ�䣺" << t << endl;


//#ifdef OUTPUT_DEBUG_INFO
//	if (OUTPUT_DEBUG_INFO)
//	{
//		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd_Standard.drowDebugDetectLR);
//		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd_Standard.drowDebugDetectUD);
//		cv::imwrite("samples/02drowDebugResult.jpg", bd_Standard.drowDebugResult);
//	}
//#endif



	Measurer m = Measurer(&bd_Standard, &mdi, tileCalibrationWidth, tileCalibrationHeight);
	//pincushionCalibrationImg = cv::imread("PincushionDistortionʵ��.jpg");
	//m.PincushionCalibration(pincushionCalibrationImg);
	//cv::Mat img2 = pincushionCalibrationImg.clone();
	//m.PincushionImgAdjust(img2);
	m.ObserveCalibration();


	s.NowBufferImg = cv::imread("E31����_x3��ֵ��.jpg");
	BlocksDetector bd2 = BlocksDetector(&s, &mdi);
	bd2.Start();
	bd2.StartUP_DOWN(BlocksDetector::Up);
	bd2.StartUP_DOWN(BlocksDetector::Down);
	bd2.ABCD();
	m.CaculteSize(&bd2);
	return 0;
}

