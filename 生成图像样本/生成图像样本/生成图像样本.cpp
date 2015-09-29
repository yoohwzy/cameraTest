// 生成图像样本.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include <time.h>
#define random(x) (rand()%x)

int _tmain(int argc, _TCHAR* argv[])
{
	srand((int)time(0));



	////大凹点配置
	//int minR = 25;
	//int maxR = 60;

	////小凹点 水渍配置
	//int minR = 15;
	//int maxR = 30;

	////划痕配置
	//int minR = 30;
	//int maxR = 160;

	//指纹配置
	int minR = 60;
	int maxR = 200;

	int deflection = 50;//中心点随机偏离距离

	//cv::Mat src = cv::imread("A9划痕凹点_x3.jpg");
	//cv::Point center = cv::Point(962, 4394);//大凹点
	//string path = "samples\\大凹点A_";
	//cv::Point center = cv::Point(1398, 4473);//小凹点
	//string path = "samples\\小凹点A_";
	//cv::Point center = cv::Point(882, 6244);
	//string path = "samples\\水渍A4_";
	//cv::Point center = cv::Point(1677, 3400);
	//string path = "samples\\划痕A2_";
	//cv::Point center = cv::Point(2110, 9000);//小凹点
	//string path = "samples\\釉面A2_";



	//cv::Mat src = cv::imread("D9划痕凹点_x3.jpg");
	//cv::Point center = cv::Point(852, 5638);//大凹点
	//string path = "samples\\大凹点D_";
	//cv::Point center = cv::Point(1284, 5708);//小凹点
	//string path = "samples\\小凹点D_";
	//cv::Point center = cv::Point(1832, 9057);//小凹点
	//string path = "samples\\划痕D2_";
	//cv::Point center = cv::Point(992, 10057);//小凹点
	//string path = "samples\\釉面D2_";



	//cv::Mat src = cv::imread("A14杂质_x3.jpg");
	//cv::Point center = cv::Point(1300, 2050);
	//string path = "samples\\指纹A2_";
	//cv::Point center = cv::Point(2130, 7226);
	//string path = "samples\\杂质A_";


	//cv::Mat src = cv::imread("D14杂质_x3.jpg");
	//cv::Point center = cv::Point(1200, 9400);
	//string path = "samples\\指纹D2_";
	//cv::Point center = cv::Point(2029, 7711);
	//string path = "samples\\杂质D_";


	//cv::Mat src = cv::imread("A31崩角_x3.jpg");
	//cv::Point center = cv::Point(2600,7900);
	//string path = "samples\\水纹A4_";


	cv::Mat src = cv::imread("D31崩角_x3.jpg");
	cv::Point center = cv::Point(1800, 3100);
	string path = "samples\\水纹D4_";




	int r = 400;
	cv::Mat ROI = src(cv::Rect(center.x - r, center.y - r, r + r, r + r));




	for (size_t i = 1; i <= 10; i++)
	{
		double angle = random(361);
		cv::Mat rotateMat = cv::getRotationMatrix2D(cv::Point(r, r), angle, 1);
		cv::Mat rotateImg;
		cv::warpAffine(ROI, rotateImg, rotateMat, ROI.size());
		
		int offsetX = random(deflection) - deflection/2;
		int offsetY = random(deflection) - deflection/2;

		int range = abs(random(maxR - minR)) + minR;
		cv::Mat ROI2 = rotateImg(cv::Rect(r - offsetX - range, r - offsetY - range, range + range, range + range));

		char num[4];
		sprintf(num, "%03d", i);
		stringstream ss;
		ss << path << num << ".jpg";
		cv::imwrite(ss.str(), ROI2);

		//cv::imshow("1", ROI2);
		//cv::waitKey(0);
	}

	return 0;
}

