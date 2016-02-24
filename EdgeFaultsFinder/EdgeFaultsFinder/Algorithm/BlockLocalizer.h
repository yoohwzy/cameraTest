#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

//#define BD_OUTPUT_DEBUG_INFO 1

using namespace std;
//@description 瓷砖位置快速定位，传入预处理后的图像
// 首先查找上边缘，取小区域高斯模糊，二值化
//@author VShawn
//@last modify date 2015-9-29 22:39:36 By VShawn
class BlockLocalizer
{
public:
	BlockLocalizer(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockLocalizer();

	bool NotFoundBlockFlag = true;//未找到瓷砖标记
	bool BrokenEdgeFlag = false;//有崩边缺陷标记


#ifdef BD_OUTPUT_DEBUG_INFO
	//检测过程中的图片
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif

private:
	cv::Mat img;
	//在水平边缘（上、下）上获得y坐标值
	//start搜索的中点
	//在start上下共range高的范围内搜索
	//scanUp2Down搜索方向为上到下？
	int getYOnLine(cv::Point start, int range, bool scanUp2Down = true);
	//在竖直边缘（左、右）上获得x坐标值
	//start搜索的中点
	//在start左右共range宽的范围内搜索
	//scanLeft2right 搜索方向
	int getXOnRow(cv::Point start, int range, bool scanLeft2right = true);

	vector<cv::Point> uppoints;
	vector<cv::Point> downpoints;
	vector<cv::Point> leftpoints;
	vector<cv::Point> rightpoints;

	Faults *faults = NULL;
	Block *block = NULL;

	void FindUp();
	void FindLeft();
	void FindRight();
	void FindDown();


	//判断各线是否在边界上
	//是否有较大的崩边
	//最后给出各边方程
	void Judgement();
	void judgementForOneLine(vector<cv::Point>& points, bool updown, Block::Line& line);
	//判断一组点是否是一根直线的，若有折线则BrokenEdgeFlag标记true
	//言下之意即为判断瓷砖是否有崩边，崩边则BrokenEdgeFlag标记true
	void judgemanBrokenLine(vector<cv::Point>& points,bool updown);
	//判断一组点是否在边界上，分为以下几种情况
	//1.开始点与结束点都在边界上：该直线为边界线，返回false
	//2.一般点在边界上，一半不在，删除在边界上的点，以便进行拟合，返回true
	bool fixLineOnBorder(vector<cv::Point>& points, Block::Line& line);



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

