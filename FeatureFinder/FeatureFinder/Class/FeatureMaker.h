#pragma once
<<<<<<< HEAD

#include "../globle.h"
=======
>>>>>>> 59f70f38b725ef6302e1c042cda95869edc56f42
class FeatureMaker
{
public:
	FeatureMaker();
	~FeatureMaker();
<<<<<<< HEAD
	//��ȡ�Ҷ�ֱ��ͼ
	//1ԭͼ��
	//2�Ҷ�ֱ��ͼ
	//3bin��Ŀ���Ҷȼ���
	//���1*grayHist�е�Mat FLOAT32��
	static void GetGrayHist(cv::Mat& srt, cv::Mat& grayHist, int histSize = 255);
	//����ͼ���ƽ���Ҷȣ�����
	static void GetGrayAvgStdDev(cv::Mat& src, double& avg, double& stddev);

=======
>>>>>>> 59f70f38b725ef6302e1c042cda95869edc56f42
};

