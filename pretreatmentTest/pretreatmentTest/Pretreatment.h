#pragma once

//‘§¥¶¿Ì¿‡£¨æ≤Ã¨
#include <opencv2\opencv.hpp>

class Pretreatment
{
public:
	Pretreatment();
	~Pretreatment();
	static void Run(cv::Mat& img);
};

