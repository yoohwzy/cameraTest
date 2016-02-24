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
	cv::Mat img = cv::imread("D://16_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图2.jpg", 0);

	Faults faults;
	Block block = Block(img.cols, img.rows);
	BlockLocalizer bl = BlockLocalizer(img, &block, &faults);
	if (bl.NotFoundBlockFlag == true)
	{
		cv::namedWindow("0", 0);
		cv::imshow("0", img);
		cv::waitKey(0);
		return;
	}
	if (bl.BrokenEdgeFlag == true)
	{
		cv::cvtColor(img, img, CV_GRAY2BGR);
		for (size_t i = 0; i < faults.BrokenEdges.size(); i++)
		{
			cv::circle(img, faults.BrokenEdges[i].position, faults.BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
		}
		cv::namedWindow("1", 0);
		cv::imshow("1", img);
		cv::waitKey(0);
		return;
	}
}

