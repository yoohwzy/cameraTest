#pragma once
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"

//@description	瓷砖位置快速定位，传入二值化后的图像，计算出ABCD四个角理论位置
//				与之前取点拟合边缘直线不同，这里先边缘的大致范围 取ROI 边缘算子 hough
//@author VShawn
//@last modify date 2015-10-25 10:55:28 By VShawn
class BlockHoughDetector
{
public:
	BlockHoughDetector(cv::Mat);
	~BlockHoughDetector();
private:
	cv::Mat Caculte(cv::Mat& img, cv::Mat& _operator, cv::Point operatorCenter);
	void InitMat(cv::Mat& m, char* num);
};

