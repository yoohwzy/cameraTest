#pragma once
#include <opencv.hpp>
using namespace cv;


//@description ͼ��Ԥ������
//@author ���
//@last modify date 2015-9-29 22:39:30 By VShawn
class Processor
{
public:
	static Mat Binaryzation(Mat &img);
	static Mat Gaussian_Blur(Mat &img);
};

