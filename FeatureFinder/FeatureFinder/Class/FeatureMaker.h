#pragma once
#include "../globle.h"

class FeatureMaker
{
public:
	FeatureMaker();
	~FeatureMaker();
	//��ȡ�Ҷ�ֱ��ͼ
	//1ԭͼ��
	//2�Ҷ�ֱ��ͼ
	//3bin��Ŀ���Ҷȼ���
	//���1*grayHist�е�Mat FLOAT32��
	static void GetGrayHist(cv::Mat& srt, cv::Mat& grayHist, int histSize = 255);
	//����ͼ���ƽ���Ҷȣ�����
	static void GetGrayAvgStdDev(cv::Mat& src, double& avg, double& stddev);

};

