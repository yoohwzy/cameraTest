#pragma once
#include "../globle.h"

class FeatureMaker
{
public:
	FeatureMaker();
	~FeatureMaker();
	//获取灰度直方图
	//1原图像
	//2灰度直方图
	//3bin数目（灰度级别）
	//输出1*grayHist行的Mat FLOAT32型
	static void GetGrayHist(cv::Mat& srt, cv::Mat& grayHist, int histSize = 255);
	//计算图像的平均灰度，方差
	static void GetGrayAvgStdDev(cv::Mat& src, double& avg, double& stddev);

};

