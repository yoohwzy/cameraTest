// pretreatmentTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include "Pretreatment.h"
#include <sstream>
#include <iostream>
#include <string>

using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	for (size_t i = 1; i <56; i++)
	{
		stringstream ss;
		ss << "D://����//2015.12.30//2//" << i << "_oԭͼ.jpg";
		cv::Mat img = cv::imread(ss.str(), 0);
		if (img.rows > 0)
		{
			Pretreatment::Run(img);
			//cv::namedWindow("1", 0);
			//cv::imshow("1", img);
			//cv::waitKey(0);
			cv::imwrite(ss.str(), img);
		}
	}
	return 0;
}

