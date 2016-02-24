// EdgeFaultsFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

#include "Algorithm\Base\Faults.h"
#include "Algorithm\Base\Block.h"

#include "Algorithm\BlocksDetector.h"
#include "Algorithm\BlockLocalizer.h"

using namespace std;

void main(int argc, _TCHAR* argv[])
{
	cv::Mat img = cv::imread("D://16_o原图4.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图2.jpg", 0);

	Faults faults;
	Block block = Block(img.cols, img.rows);
	BlockLocalizer bl = BlockLocalizer(img, &block, &faults);
	if (bl.NotFoundBlockFlag == true)
	{
		return;
	}
	if (bl.BrokenEdgeFlag == true)
	{
		return;
	}
}

