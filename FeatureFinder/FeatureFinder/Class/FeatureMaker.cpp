#include "FeatureMaker.h"


FeatureMaker::FeatureMaker()
{
}


FeatureMaker::~FeatureMaker()
{
}

void FeatureMaker::GetGrayHist(cv::Mat& src, cv::Mat& grayHist, int histSize)
{
	float range[] = { 0, 255 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	cv::calcHist(&src, 1, 0, cv::Mat(), grayHist, 1, &histSize, &histRange, uniform, accumulate);
}
void FeatureMaker::GetGrayAvgStdDev(cv::Mat& src, double& avg, double &stddev)
{
	cv::Mat img;
	if (src.channels() == 3)
		cv::cvtColor(src, img, CV_BGR2GRAY);
	else
		img = src;
	cv::mean(src);
	cv::Mat mean;
	cv::Mat stdDev;
	cv::meanStdDev(img, mean, stdDev);

	avg = mean.ptr<double>(0)[0];
	stddev = stdDev.ptr<double>(0)[0];
}