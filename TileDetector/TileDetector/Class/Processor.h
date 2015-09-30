#pragma once
#include <opencv.hpp>
using namespace cv;


//@description Í¼ÏñÔ¤´¦ÀíÀà
//@author »Æèª
//@last modify date 2015-9-29 22:39:30 By VShawn
class Processor
{
public:
	static Mat Binaryzation(Mat &img);
	static Mat Gaussian_Blur(Mat &img);
};

