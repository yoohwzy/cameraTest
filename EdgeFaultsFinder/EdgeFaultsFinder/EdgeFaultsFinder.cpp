// EdgeFaultsFinder.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

#include "Algorithm\BlocksDetector.h"
#include "Algorithm\BlockLocalizer.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat img = cv::imread("D://16_oԭͼ.jpg", 0);
	//cv::Mat img = cv::imread("D://16_oԭͼ2.jpg", 0);

	BlockLocalizer bl = BlockLocalizer(img);
	bl.FindUp();
	bl.FindLeft();

	//cv::threshold(img, img, 10,255,CV_THRESH_BINARY);

	//BlocksDetector bd = BlocksDetector(img);

	//bool left_right = bd.Start();
	//bool up = bd.StartUP_DOWN(BlocksDetector::Up);
	//bool down = bd.StartUP_DOWN(BlocksDetector::Down);
	//if (!left_right ||
	//	!up ||
	//	!down)
	//{
	//	if (!left_right)
	//		cout << ("����δ�ҵ�\r\n");
	//	else if (!up)
	//		cout << ("��δ�ҵ�\r\n");
	//	else if (!down)
	//		cout << ("��δ�ҵ�\r\n");
	//	return false;
	//}
	//return 0;
}

