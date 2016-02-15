// EdgeFaultsFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

#include "Algorithm\BlocksDetector.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat img = cv::imread("D://16_o原图.jpg", 0);

	cv::threshold(img, img, 10,255,CV_THRESH_BINARY);

	BlocksDetector bd = BlocksDetector(img);

	bool left_right = bd.Start();
	bool up = bd.StartUP_DOWN(BlocksDetector::Up);
	bool down = bd.StartUP_DOWN(BlocksDetector::Down);
	if (!left_right ||
		!up ||
		!down)
	{
		if (!left_right)
			cout << ("左右未找到\r\n");
		else if (!up)
			cout << ("上未找到\r\n");
		else if (!down)
			cout << ("下未找到\r\n");
		return false;
	}
	return 0;
}

