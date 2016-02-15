#pragma once
#include <opencv2\opencv.hpp>
#include "Base\Block.h"

#define BD_OUTPUT_DEBUG_INFO 1


//@description 瓷砖位置快速定位，传入预处理后的图像
// 首先查找上边缘，取小区域高斯模糊，二值化
//@author VShawn
//@last modify date 2015-9-29 22:39:36 By VShawn
class BlockLocalizer
{
public:
	BlockLocalizer(cv::Mat& Img);
	~BlockLocalizer();

#ifdef BD_OUTPUT_DEBUG_INFO
	//检测过程中的图片
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif

	void FindUp();
private:
	cv::Mat img;
};

