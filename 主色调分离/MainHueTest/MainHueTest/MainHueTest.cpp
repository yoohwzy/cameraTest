// MainHueTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;

#include "MainHueAnalysis.h"



int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat image = cv::imread("b14杂质222.jpg");
	//cv::Mat image = cv::imread("b14杂质_x3.jpg");
	//cv::Mat image = cv::imread("新建位图图像.bmp");

	MainHueAnalysis m(image);
	return 0;
}

