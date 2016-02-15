#pragma once
#include <opencv2\opencv.hpp>
#include "Base\Block.h"

#define BD_OUTPUT_DEBUG_INFO 1


//@description ��שλ�ÿ��ٶ�λ������Ԥ������ͼ��
// ���Ȳ����ϱ�Ե��ȡС�����˹ģ������ֵ��
//@author VShawn
//@last modify date 2015-9-29 22:39:36 By VShawn
class BlockLocalizer
{
public:
	BlockLocalizer(cv::Mat& Img);
	~BlockLocalizer();

#ifdef BD_OUTPUT_DEBUG_INFO
	//�������е�ͼƬ
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif

	void FindUp();
private:
	cv::Mat img;
};

