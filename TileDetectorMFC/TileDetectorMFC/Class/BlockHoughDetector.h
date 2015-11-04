#pragma once
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"

//@description	��שλ�ÿ��ٶ�λ�������ֵ�����ͼ�񣬼����ABCD�ĸ�������λ��
//				��֮ǰȡ����ϱ�Եֱ�߲�ͬ�������ȱ�Ե�Ĵ��·�Χ ȡROI ��Ե���� hough
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

