#include "BlockLocalizer.h"


BlockLocalizer::BlockLocalizer(cv::Mat& Img)
{
	img = Img;

#ifdef BD_OUTPUT_DEBUG_INFO
	if (BD_OUTPUT_DEBUG_INFO)
	{
		if (img.channels() == 1)
		{
			cvtColor(img, drowDebugDetectLR, CV_GRAY2BGR);
			drowDebugDetectUD = drowDebugDetectLR.clone();
			drowDebugResult = drowDebugDetectLR.clone();
		}
		else
		{
			drowDebugDetectLR = img.clone();
			drowDebugDetectUD = img.clone();
			drowDebugResult = img.clone();
		}
	}
#endif
}


BlockLocalizer::~BlockLocalizer()
{
}

void BlockLocalizer::FindUp()
{
	//先查找第一个点


}