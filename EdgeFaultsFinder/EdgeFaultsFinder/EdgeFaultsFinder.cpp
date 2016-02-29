// EdgeFaultsFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

#include "Algorithm\Base\Faults.h"
#include "Algorithm\Base\Block.h"

#include "Algorithm\BlockLocalizer.h"

using namespace std;

void main(int argc, _TCHAR* argv[])
{
	//cv::Mat img = cv::imread("D://出差//2015.12.28//baocuo//2_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://出差//2015.12.31//1 边上//1_o原图.jpg", 0);
	cv::Mat img = cv::imread("D://出差//2015.12.31//1 边上//6_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图2.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图3.jpg", 0);

	cv::normalize(img, img, 0, 100, cv::NORM_MINMAX);

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
	
	cv::cvtColor(img, img, CV_GRAY2BGR);

	cv::line(img, cv::Point(0, block.UpLine.k * (0 - block.UpLine.x0) + block.UpLine.y0), cv::Point(img.cols, block.UpLine.k * (img.cols - block.UpLine.x0) + block.UpLine.y0), cv::Scalar(0, 0, 255), 1);
	cv::line(img, cv::Point(0, block.DownLine.k * (0 - block.DownLine.x0) + block.DownLine.y0), cv::Point(img.cols, block.DownLine.k * (img.cols - block.DownLine.x0) + block.DownLine.y0), cv::Scalar(0, 255, 255), 1);
	cv::line(img, cv::Point((img.rows - block.LeftLine.y0) / block.LeftLine.k + block.LeftLine.x0, img.rows), cv::Point((0 - block.LeftLine.y0) / block.LeftLine.k + block.LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
	cv::line(img, cv::Point((img.rows - block.RightLine.y0) / block.RightLine.k + block.RightLine.x0, img.rows), cv::Point((0 - block.RightLine.y0) / block.RightLine.k + block.RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);

	cv::namedWindow("2", 0);
	cv::imshow("2", img);
	cv::waitKey(0);
}