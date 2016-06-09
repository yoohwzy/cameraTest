// AreaCameraClass.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "MVCAM.h"
#include <thread>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	double t = (double)cv::getTickCount();
	MVCAM mvcam;
	mvcam.Init();
	mvcam.StartCapture();
	t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
	std::cout << "mvcam.Init() ms:" << t << endl;
	t = (double)cv::getTickCount();

	//while (true)
	//{
	//	cv::Mat img = mvcam.Grub();
	//	t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
	//	std::cout << "mvcam.Grub() ms:" << t << endl;
	//	cv::imshow("123", img);
	//	cv::waitKey(5);
	//}
	mvcam.Release();
	return 0;
}


