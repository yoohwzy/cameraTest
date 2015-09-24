// Calibration.cpp : 定义控制台应用程序的入口点。
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
	mdi.MaxPics = 11000;//采集多少帧图像
	s_Standard = BufferStorage(mdi);


	int tileCalibrationWidth = 300;
	int tileCalibrationHeight = 600;
	//cv::Mat pincushionCalibrationImg = cv::imread("PincushionDistortion.png");

	//s_Standard.NowBufferImg = cv::imread("E14杂质二值化_x3.jpg");
	//s.NowBufferImg = cv::imread("E9 A_x3二值化.jpg");

	s_Standard.NowBufferImg = cv::imread("A9划痕凹点_x3二值化.jpg");
	s.NowBufferImg = cv::imread("A14杂质_x3二值化.jpg");

	double t = 0;
	//瓷砖边缘检测
	BlocksDetector bd_Standard = BlocksDetector(&s_Standard);
	t = (double)cv::getTickCount();
	bd_Standard.Start();
	bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	bd_Standard.StartUP_DOWN(BlocksDetector::Down);

	bd_Standard.ABCD();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "标准砖边缘检测时间：" << t << endl;


//#ifdef OUTPUT_DEBUG_INFO
//	if (OUTPUT_DEBUG_INFO)
//	{
//		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd_Standard.drowDebugDetectLR);
//		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd_Standard.drowDebugDetectUD);
//		cv::imwrite("samples/02drowDebugResult.jpg", bd_Standard.drowDebugResult);
//	}
//#endif


	t = (double)cv::getTickCount();

	Measurer m = Measurer(&bd_Standard, &mdi, tileCalibrationWidth, tileCalibrationHeight);
	//pincushionCalibrationImg = cv::imread("PincushionDistortion实拍.jpg");
	//m.PincushionCalibration(pincushionCalibrationImg);
	//cv::Mat img2 = pincushionCalibrationImg.clone();
	//m.PincushionImgAdjust(img2);
	m.ObserveCalibration();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "初始化Measurer时间：" << t << endl;
	std::cout << "标准砖标定完成" << t << endl;
	t = (double)cv::getTickCount();

	BlocksDetector bd2 = BlocksDetector(&s);
	bd2.Start();
	bd2.StartUP_DOWN(BlocksDetector::Up);
	bd2.StartUP_DOWN(BlocksDetector::Down);
	bd2.ABCD();
	#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			cv::imwrite("samples/00drowDebugDetectLR.jpg", bd2.drowDebugDetectLR);
			cv::imwrite("samples/01drowDebugDetectUD.jpg", bd2.drowDebugDetectUD);
			cv::imwrite("samples/02drowDebugResult.jpg", bd2.drowDebugResult);
		}
	#endif

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "待测量砖边缘检测时间：" << t << endl;
	t = (double)cv::getTickCount();

	m.CaculteSize(&bd2);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "待测量砖尺寸测量时间：" << t << endl << endl << endl;


	printf("瓷砖AB边长：%fpix，合计%f mm\r\n", m.AB_Len, m.AB_mm);
	printf("瓷砖BC边长：%fpix，合计%f mm\r\n", m.BC_Len, m.BC_mm);
	printf("瓷砖CD边长：%fpix，合计%f mm\r\n", m.CD_Len, m.CD_mm);
	printf("瓷砖DA边长：%fpix，合计%f mm\r\n \r\n", m.DA_Len, m.DA_mm);

	printf("瓷砖对角线AC：%fpix\r\n", m.AC_Len);
	printf("瓷砖对角线BD：%fpix\r\n\r\n", m.BD_Len);

	printf("∠A=%f°,∠B=%f°,∠C=%f°,∠D=%f°；\r\n\r\n", m.angleA, m.angleB, m.angleC, m.angleD);

	return 0;
}

