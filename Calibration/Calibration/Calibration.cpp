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
	mdi.MaxPics = 11000;//�ɼ�����֡ͼ��
	s_Standard = BufferStorage(mdi);


	int tileCalibrationWidth = 300;
	int tileCalibrationHeight = 600;
	//cv::Mat pincushionCalibrationImg = cv::imread("PincushionDistortion.png");

	//s_Standard.NowBufferImg = cv::imread("E14���ʶ�ֵ��_x3.jpg");
	//s.NowBufferImg = cv::imread("E9 A_x3��ֵ��.jpg");

	s_Standard.NowBufferImg = cv::imread("A9���۰���_x3��ֵ��.jpg");
	s.NowBufferImg = cv::imread("A14����_x3��ֵ��.jpg");

	double t = 0;
	//��ש��Ե���
	BlocksDetector bd_Standard = BlocksDetector(&s_Standard);
	t = (double)cv::getTickCount();
	bd_Standard.Start();
	bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	bd_Standard.StartUP_DOWN(BlocksDetector::Down);

	bd_Standard.ABCD();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "��׼ש��Ե���ʱ�䣺" << t << endl;


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
	//pincushionCalibrationImg = cv::imread("PincushionDistortionʵ��.jpg");
	//m.PincushionCalibration(pincushionCalibrationImg);
	//cv::Mat img2 = pincushionCalibrationImg.clone();
	//m.PincushionImgAdjust(img2);
	m.ObserveCalibration();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "��ʼ��Measurerʱ�䣺" << t << endl;
	std::cout << "��׼ש�궨���" << t << endl;
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
	std::cout << "������ש��Ե���ʱ�䣺" << t << endl;
	t = (double)cv::getTickCount();

	m.CaculteSize(&bd2);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "������ש�ߴ����ʱ�䣺" << t << endl << endl << endl;


	printf("��שAB�߳���%fpix���ϼ�%f mm\r\n", m.AB_Len, m.AB_mm);
	printf("��שBC�߳���%fpix���ϼ�%f mm\r\n", m.BC_Len, m.BC_mm);
	printf("��שCD�߳���%fpix���ϼ�%f mm\r\n", m.CD_Len, m.CD_mm);
	printf("��שDA�߳���%fpix���ϼ�%f mm\r\n \r\n", m.DA_Len, m.DA_mm);

	printf("��ש�Խ���AC��%fpix\r\n", m.AC_Len);
	printf("��ש�Խ���BD��%fpix\r\n\r\n", m.BD_Len);

	printf("��A=%f��,��B=%f��,��C=%f��,��D=%f�㣻\r\n\r\n", m.angleA, m.angleB, m.angleC, m.angleD);

	return 0;
}

