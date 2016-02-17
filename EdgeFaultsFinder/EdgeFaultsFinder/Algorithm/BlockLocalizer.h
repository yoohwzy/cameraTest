#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
#include "Base\Block.h"

#define BD_OUTPUT_DEBUG_INFO 1

using namespace std;
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
	void FindLeft();
private:
	cv::Mat img;
	//在水平边缘（上、下）上获得y坐标值
	int getYOnLine(cv::Point start, int range);
	//在竖直边缘（左、右）上获得x坐标值
	int getXOnRow(cv::Point start, int range);

	vector<cv::Point> uppoints;
	vector<cv::Point> downpoints;
	vector<cv::Point> leftpoints;
	vector<cv::Point> rightpoints;


	void rectFix(cv::Rect& rect)
	{
		if (rect.x < 0)
			rect.x = 0;
		if (rect.x >= img.cols)
			rect.x = img.cols - 1;
		if (rect.x + rect.width > img.cols)
			rect.width = img.cols - rect.x;

		if (rect.y < 0)
			rect.y = 0;
		if (rect.y >= img.rows)
			rect.y = img.rows - 1;
		if (rect.y + rect.height > img.rows)
			rect.height = img.rows - rect.y;
	}
	//对LIST<POINT>排序
	static bool ORDER_BY_Y_ASC(cv::Point i, cv::Point j) { return i.y < j.y; }
	static bool ORDER_BY_Y_DESC(cv::Point i, cv::Point j) { return i.y > j.y; }
	static bool ORDER_BY_X_ASC(cv::Point i, cv::Point j) { return i.x < j.x; }
	static bool ORDER_BY_X_DESC(cv::Point i, cv::Point j) { return i.x > j.x; }
};

