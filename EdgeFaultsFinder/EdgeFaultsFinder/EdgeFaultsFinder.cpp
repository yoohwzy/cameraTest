// EdgeFaultsFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2\opencv.hpp>

#include "Algorithm\Base\Faults.h"
#include "Algorithm\Base\Block.h"

#include "Algorithm\BlockLocalizer.h"
#include "Algorithm\BlockEdgeDetector.h"
#include "Algorithm\BlockEdgeDetectorT.h"

using namespace std;

void main(int argc, _TCHAR* argv[])
{
	//cv::Mat img = cv::imread("D://出差//11错_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://出差//2015.12.28//baocuo//2_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://出差//2015.12.30//1//12_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://出差//2015.12.31//1 边上//1_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://出差//2015.12.31//1 边上//5_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://出差//2015.12.31//1 边上//6_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图2.jpg", 0);
	//cv::Mat img = cv::imread("D://16_o原图3.jpg", 0);

	for (size_t index = 12; index < 52; index++)
	{
		stringstream ss;
		ss << "D://出差//2015.12.29//2//" << index << "_o原图.jpg";
		//ss << "D://出差//2015.12.30//2//" << index << "_o原图.jpg";

		//cv::Mat img = cv::imread(ss.str(), 0);
		//cv::Mat img = cv::imread("D://16_o原图6.jpg", 0);
		cv::Mat img = cv::imread("D://出差//2015.12.31//1 边上//2_o原图.jpg", 0);

		if (img.rows == 0)
		{
			cout << index << "图片未加载！" << endl;
			continue;
		}
		cout << index << endl;

		cv::normalize(img, img, 0, 100, cv::NORM_MINMAX);

		double t = cv::getTickCount();
		Faults faults;
		Block block = Block(img.cols, img.rows);
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << t << endl;
		t = cv::getTickCount();


		BlockLocalizer bl = BlockLocalizer(img, &block, &faults);
		bl.THRESHOD = 10;
		bl.ContinuePointCount = 30;
		bl.Run();


		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << t << endl;
		t = cv::getTickCount();

		if (bl.NotFoundBlockFlag == true)
		{
			cv::namedWindow("0", 0);
			cv::imshow("0", img);
			cv::waitKey(0);
			continue;
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
			continue;
		}

		BlockEdgeDetector bed = BlockEdgeDetector(img, &block, &faults);
		bed.DIFF_THRESHOLD = 0.4;
		bed.FAULTS_SPAN = 4;
		bed.FAULTS_COUNT = 5;
		bed.Run();

		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << t << endl;
		t = cv::getTickCount();


		cv::cvtColor(img, img, CV_GRAY2BGR);

		cv::line(img, cv::Point(0, block.UpLine.k * (0 - block.UpLine.x0) + block.UpLine.y0), cv::Point(img.cols, block.UpLine.k * (img.cols - block.UpLine.x0) + block.UpLine.y0), cv::Scalar(0, 0, 255), 1);
		cv::line(img, cv::Point(0, block.DownLine.k * (0 - block.DownLine.x0) + block.DownLine.y0), cv::Point(img.cols, block.DownLine.k * (img.cols - block.DownLine.x0) + block.DownLine.y0), cv::Scalar(0, 255, 255), 1);
		cv::line(img, cv::Point((img.rows - block.LeftLine.y0) / block.LeftLine.k + block.LeftLine.x0, img.rows), cv::Point((0 - block.LeftLine.y0) / block.LeftLine.k + block.LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
		cv::line(img, cv::Point((img.rows - block.RightLine.y0) / block.RightLine.k + block.RightLine.x0, img.rows), cv::Point((0 - block.RightLine.y0) / block.RightLine.k + block.RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);

		if (faults.BrokenEdges.size() > 0)
		{
			for (size_t i = 0; i < faults.BrokenEdges.size(); i++)
			{
				cv::circle(img, faults.BrokenEdges[i].position, faults.BrokenEdges[i].length + 50, cv::Scalar(0, 0, 255), 10);
			}
			//arm.AddAction(0, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
		}


		cv::namedWindow("2", 0);
		cv::imshow("2", img);
		cv::waitKey(0);
	}
}