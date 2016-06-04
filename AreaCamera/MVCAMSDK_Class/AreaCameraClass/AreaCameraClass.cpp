// AreaCameraClass.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "MVCAM.h"
#include <thread>


int _tmain(int argc, _TCHAR* argv[])
{
	MVCAM mvcam;
	mvcam.Init();
	mvcam.StartCapture();
	//while (true)
	//{
	//	cv::Mat img = mvcam.Grub();
	//	cv::imshow("123", img);
	//	cv::waitKey(5);
	//}
	cv::Mat img = mvcam.Grub();
	cv::imshow("123", img);
	cv::waitKey(0);
	return 0;
}


