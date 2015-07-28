#pragma once

#include "../globle.h"
#include "BufferStorage.h";
#include "MicroDisplay\MicroDisplayInit.h";
class BlocksDetector
{
public:
	BlocksDetector(BufferStorage *ss, MicroDisplayInit *mdii);
	~BlocksDetector();

	void Start();

	//检测过程中的图片
	cv::Mat drowDebugDetect;
	cv::Mat drowDebugResult;

	cv::Point A;
	cv::Point B;
	cv::Point C;
	cv::Point D;

	vector < cv::Point > LeftBorder;
	vector < cv::Point > RightBorder;
	vector < cv::Point > UpBorder;
	vector < cv::Point > DownBorder;


private:
	BufferStorage *s;
	MicroDisplayInit *mdi;

	//逼近法求出左上角，已知endline的x列为边界
	int GetEdgeLeftx3(cv::Point start, int range = 200);
	int GetEdgeLeftApproach(cv::Point start, cv::Point end, int range = 200);
	int GetEdgeLeft(cv::Point start, int range = 200);

	int GetEdgeRightx3(cv::Point start, int range = 200);
	int GetEdgeRightApproach(cv::Point start, cv::Point end, int range = 200);
	int GetEdgeRight(cv::Point start, int range = 200);

	int GetEdgeUpx3(cv::Point start, int range = 200);
	int GetEdgeUpApproach(cv::Point start, cv::Point end, int range = 200);
	int GetEdgeUP(cv::Mat& ROI, int offsetY, cv::Point start, int range = 200);


	//检测边缘时，累加多少个
	const int SUM_COUNT = 20;
	//平均每个点之间要差SUM_AVG_THRESHOD才算作是边界，真正的阈值为SUM_COUNT*SUM_AVG_THRESHOD;
	const int SUM_AVG_THRESHOD = 5;
	const int SUM_THRESHOD = SUM_COUNT * SUM_AVG_THRESHOD;


	//隔几行采样一次
	const int ROW_SPAN = 100;
	//默认启动扫描的中心点，左边为ORANGE_MARGIN_LINE，右边为width-ORANGE_MARGIN_LINE
	const int ORANGE_MARGIN_ROW = 300;
	//默认的，对一个左右多少范围内进行扫描
	const int ORANGE_RANGE_ROW = 400;


	//隔几列采样一次
	const int COL_SPAN = 100;
	//默认的，对一个上下多少范围内进行扫描
	const int ORANGE_RANGE_COL = 400;

	//对LIST<POINT>排序

	static bool ORDER_BY_Y_ASC(cv::Point i, cv::Point j) { return i.y < j.y; }
	static bool ORDER_BY_Y_DESC(cv::Point i, cv::Point j) { return i.y > j.y; }
	static bool ORDER_BY_X_ASC(cv::Point i, cv::Point j) { return i.x < j.x; }
	static bool ORDER_BY_X_DESC(cv::Point i, cv::Point j) { return i.x > j.x; }
};

