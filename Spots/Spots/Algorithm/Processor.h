#pragma once
#include <opencv.hpp>
using namespace cv;


//@description 图像预处理类
//@author 黄瑾
//@last modify date 2015-9-29 22:39:30 By VShawn
class Processor
{
public:
	static Mat Binaryzation(Mat &img);
	static Mat Binaryzation_otsu(Mat &img);
	static Mat Gaussian_Blur(Mat &img);
private:	
	static int otsuThreshold(Mat img);   // 大津阈值法
	static int peak_valleyThreshold(Mat img);
};

