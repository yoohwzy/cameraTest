// MainHueTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;

#include "MainHueAnalysis.h"



int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat image = cv::imread("b14����222.jpg");
	//cv::Mat image = cv::imread("b14����_x3.jpg");
	//cv::Mat image = cv::imread("�½�λͼͼ��.bmp");

	MainHueAnalysis m(image);
	return 0;
}

