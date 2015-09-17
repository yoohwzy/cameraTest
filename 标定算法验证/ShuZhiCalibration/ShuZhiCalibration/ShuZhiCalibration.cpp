// ShuZhiCalibration.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include "Class\BlocksDetector.h"
#include <thread>

BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

int _tmain(int argc, _TCHAR* argv[])
{
	mdi.colorType = mdi.RGB;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//采集多少帧图像
	s = BufferStorage(mdi);


	cv::Mat img = cv::imread("E14杂质二值化_x3.jpg", 1);
	s.NowBufferImg = img;

	double t = 0;
	//瓷砖边缘检测
	BlocksDetector bd = BlocksDetector(&s, &mdi);
	t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "边缘检测时间：" << t << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif

	//记录一行x=i的位置上的点，应该竖直平移多少
	std::vector<int> row_adjust;
	int max = 0;
	for (int i = 0; i < mdi.width; i++)
	{
		//uchar* linehead = img.ptr<uchar>(bd.UpLine.k*(i - bd.UpLine.x0) + bd.UpLine.y0);//每行的起始地址
		//if (linehead[i * 3 + 2] != 255)
		//	linehead[i * 3 + 2] = 255;
		//else
		//{
		//	linehead[i * 3 + 1] = 0;
		//	linehead[i * 3 + 0] = 0;
		//}
		int v = bd.UpLine.k*(i - bd.UpLine.x0);
		if (v > max)
			max = v;
		row_adjust.push_back(v);
	}
	for (int i = 0; i < mdi.width; i++)
	{
		row_adjust[i] -= max;
	}

	img = cv::imread("E14杂质_x3.jpg");
	cv::Mat newimg = img.clone();
	for (int i = 0; i < mdi.MaxPics; i++)
	{
		uchar* linehead = newimg.ptr<uchar>(i);//每行的起始地址
		for (int j = 0; j < mdi.width; j++)
		{
			if ((i + row_adjust[j]) < 0 || (i + row_adjust[j]) >= mdi.MaxPics)
				continue;
			uchar* linehead_tmp = img.ptr<uchar>(i + row_adjust[j]);//每行的起始地址

			linehead[j * 3 + 0] = linehead_tmp[j * 3 + 0];
			linehead[j * 3 + 1] = linehead_tmp[j * 3 + 1];
			linehead[j * 3 + 2] = linehead_tmp[j * 3 + 2];
		}
	}
 
	return 0;
	cv::imwrite("竖直矫正.jpg", newimg);
}

