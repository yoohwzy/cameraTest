// MainHueTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;

#include "MainHueAnalysis.h"



int _tmain(int argc, _TCHAR* argv[])
{
	//cv::Mat image = cv::imread("red_autumn_2-wallpaper-1920x1080.jpg");
	cv::Mat image = cv::imread("�½�λͼͼ��.bmp");

	MainHueAnalysis m(image);
	return 0;
}

