// BrightnessAdjust.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat img = cv::imread("A白纸_o原图.jpg", 0);
	cv::Mat pic = cv::imread("A9划痕凹点_x3.jpg");


	cv::Mat onerow = img(cv::Rect(0, 0, img.cols, 1)).clone();
	cv::Mat row32f(onerow.size(), CV_32FC1, cv::Scalar(0));
	onerow.convertTo(row32f, CV_32FC1);
	for (int i = 1; i < 5000; i++)
	{
		cv::Mat roi = img(cv::Rect(0, i, img.cols, 1)).clone();
		cv::Mat roi32f(roi.size(), CV_32FC1, cv::Scalar(0));
		roi.convertTo(roi32f, CV_32FC1);
		row32f = (roi32f) / (double)(i + 1) + row32f*(i / (double)(i + 1));
	}
	cv::Mat row_coefficient_float(onerow.size(), CV_32FC1, cv::Scalar(0));

	float* lineheadH = row32f.ptr<float>(0);//每行的起始地址
	float max_Gray = 0;
	for (int i = 0; i < row32f.cols; i++)
	{
		float Gray = lineheadH[i];
		if (Gray > max_Gray)
			max_Gray = Gray;
	}
	float* linehead_row_coefficient_float = row_coefficient_float.ptr<float>(0);//计算float系数
	for (int i = 0; i < row32f.cols; i++)
	{
		linehead_row_coefficient_float[i] = max_Gray / lineheadH[i];
	}
	cv::Mat row_coefficient_int(onerow.size(), CV_8UC1, cv::Scalar(0));
	uchar* linehead_row_coefficient_int = row_coefficient_int.ptr<uchar>(0);//计算int系数
	for (int i = 0; i < row32f.cols; i++)
	{
		linehead_row_coefficient_int[i] = (int)(linehead_row_coefficient_float[i] + 0.5);
	}


	for (int i = 1; i < 5000; i++)
	{
		cv::Mat roi = pic(cv::Rect(0, i, img.cols, 1));
		uchar* linehead_pic = pic.ptr<uchar>(i);//计算int系数
		for (int j = 0; j < pic.cols; j++)
		{
			linehead_pic[j * 3 + 0] = linehead_pic[j * 3 + 0] * linehead_row_coefficient_float[j];
			linehead_pic[j * 3 + 1] = linehead_pic[j * 3 + 1] * linehead_row_coefficient_float[j];
			linehead_pic[j * 3 + 2] = linehead_pic[j * 3 + 2] * linehead_row_coefficient_float[j];
		}
	}
	return 0;
}