// ����ͼ������.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"
#include <time.h>
#define random(x) (rand()%x)

int _tmain(int argc, _TCHAR* argv[])
{
	srand((int)time(0));



	////�󰼵�����
	//int minR = 25;
	//int maxR = 60;

	////С���� ˮ������
	//int minR = 15;
	//int maxR = 30;

	////��������
	//int minR = 30;
	//int maxR = 160;

	//ָ������
	int minR = 60;
	int maxR = 200;

	int deflection = 50;//���ĵ����ƫ�����

	//cv::Mat src = cv::imread("A9���۰���_x3.jpg");
	//cv::Point center = cv::Point(962, 4394);//�󰼵�
	//string path = "samples\\�󰼵�A_";
	//cv::Point center = cv::Point(1398, 4473);//С����
	//string path = "samples\\С����A_";
	//cv::Point center = cv::Point(882, 6244);
	//string path = "samples\\ˮ��A4_";
	//cv::Point center = cv::Point(1677, 3400);
	//string path = "samples\\����A2_";
	//cv::Point center = cv::Point(2110, 9000);//С����
	//string path = "samples\\����A2_";



	//cv::Mat src = cv::imread("D9���۰���_x3.jpg");
	//cv::Point center = cv::Point(852, 5638);//�󰼵�
	//string path = "samples\\�󰼵�D_";
	//cv::Point center = cv::Point(1284, 5708);//С����
	//string path = "samples\\С����D_";
	//cv::Point center = cv::Point(1832, 9057);//С����
	//string path = "samples\\����D2_";
	//cv::Point center = cv::Point(992, 10057);//С����
	//string path = "samples\\����D2_";



	//cv::Mat src = cv::imread("A14����_x3.jpg");
	//cv::Point center = cv::Point(1300, 2050);
	//string path = "samples\\ָ��A2_";
	//cv::Point center = cv::Point(2130, 7226);
	//string path = "samples\\����A_";


	//cv::Mat src = cv::imread("D14����_x3.jpg");
	//cv::Point center = cv::Point(1200, 9400);
	//string path = "samples\\ָ��D2_";
	//cv::Point center = cv::Point(2029, 7711);
	//string path = "samples\\����D_";


	//cv::Mat src = cv::imread("A31����_x3.jpg");
	//cv::Point center = cv::Point(2600,7900);
	//string path = "samples\\ˮ��A4_";


	cv::Mat src = cv::imread("D31����_x3.jpg");
	cv::Point center = cv::Point(1800, 3100);
	string path = "samples\\ˮ��D4_";




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

