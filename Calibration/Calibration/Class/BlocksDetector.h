#pragma once

#include "../globle.h"
#include "BufferStorage.h";
#include "MicroDisplay\MicroDisplayInit.h";
class BlocksDetector
{

public:
	BlocksDetector(BufferStorage *ss, MicroDisplayInit *mdii);
	~BlocksDetector();
	struct Line
	{
		int x0;
		int y0;
		double k;
		double dx;
		double dy;
	};
	enum BorderType{
		Left,
		Up,
		Right,
		Down
	};


	void Start();
	// 寻找上下边缘
	void StartUP_DOWN(BorderType bt);

	//检测某一行的左侧边缘，返回边缘的x坐标，-1表示未找到
	//传入参数为在第几行检测，不传入参数则检测leftY行
	int DetectOneLineLeft(int y = -1);
	int DetectOneLineRight(int y = -1);

	//计算出四个顶点(图像上的四个顶点)
	void ABCD();


#ifdef OUTPUT_DEBUG_INFO
	//检测过程中的图片
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif

	cv::Point A;
	cv::Point B;
	cv::Point C;
	cv::Point D;

	vector < cv::Point > LeftBorder;
	Line LeftLine;
	vector < cv::Point > RightBorder;
	Line RightLine;
	vector < cv::Point > UpBorder;
	Line UpLine;
	vector < cv::Point > DownBorder;
	Line DownLine;

	//存储边界顶点，临时存放，最后要进行分类
	vector < cv::Point > LeftUp;
	vector < cv::Point > LeftDown;
	vector < cv::Point > RightUp;
	vector < cv::Point > RightDown;

	vector < cv::Point > UpLeft;
	vector < cv::Point > DownLeft;
	vector < cv::Point > UpRight;
	vector < cv::Point > DownRight;
private:
	BufferStorage *s;
	MicroDisplayInit *mdi;

	vector<cv::Point> tmpLeftList;
	vector<cv::Point> allLeftList;//记录所有找到的点，预测用
	vector<cv::Point> tmpRightList;
	vector<cv::Point> allRightList;

	vector<cv::Point> tmpUpList;
	vector<cv::Point> allUpList;
	vector<cv::Point> tmpDownList;
	vector<cv::Point> allDownList;


	int leftY;//在第几行检测
	int leftX;//检测的中点
	int leftRnage;//在中点周围多少像素内检测
	int leftNoneCount = 0;//连续没有找到边缘的数量。
	bool leftNeedReFind = false;//对该行是否需要扩大range重新搜索

	int rightY;
	int rightX;
	int rightRnage;
	int rightNoneCount = 0;//连续没有找到边缘的数量。
	bool rightNeedReFind = false;//对该行是否需要扩大range重新搜索



	int GetEdgeX3(cv::Point start, int range, BorderType bt);
	//逼近真正的竖直边缘顶点
	//start：开始点  end：结束点 range：搜索范围  bt：边缘类型  Target：到哪个点结束（递归用）
	int GetEdgeVerticalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target);
	int GetEdgeVertical(cv::Point start, int range, bool isLeft);
	//isLeft true：逼近左顶点，false：逼近右顶点
	int GetEdgeHorizontalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target);
	int GetEdgeHorizontal(cv::Point start, int range, bool isUp);
	//判断某点是否在某条线上。
	bool IsPointNearline(Line line, cv::Point point);









	 




	 

	//检测边缘时，累加多少个像素点
	const int SUM_COUNT = 20;
	//平均每个点之间要差SUM_AVG_THRESHOD才算作是边界，真正的阈值为SUM_COUNT*SUM_AVG_THRESHOD;
	const int SUM_AVG_THRESHOD = 5;
	const int SUM_THRESHOD = SUM_COUNT * SUM_AVG_THRESHOD;


	//隔几行采样一次
	const int ROW_SPAN = 111;
	//默认启动扫描的中心点，左边为ORANGE_MARGIN_LINE，右边为width-ORANGE_MARGIN_LINE
	const int ORANGE_MARGIN_ROW = 300;
	//默认的，对一个左右多少范围内进行扫描
	const int ORANGE_RANGE_ROW = 200;

	//隔几列采样一次
	const int COL_SPAN = 100;
	//默认的，对一个上下多少范围内进行扫描
	const int ORANGE_RANGE_COL = 200;

	//每次搜索的正负范围
	const int RANGE_MINI = 35;		//参考值50
	//每次范围减少多少
	const int RANGE_REDUCE_BY = 50;


	//对LIST<POINT>排序
	static bool ORDER_BY_Y_ASC(cv::Point i, cv::Point j) { return i.y < j.y; }
	static bool ORDER_BY_Y_DESC(cv::Point i, cv::Point j) { return i.y > j.y; }
	static bool ORDER_BY_X_ASC(cv::Point i, cv::Point j) { return i.x < j.x; }
	static bool ORDER_BY_X_DESC(cv::Point i, cv::Point j) { return i.x > j.x; }
};

