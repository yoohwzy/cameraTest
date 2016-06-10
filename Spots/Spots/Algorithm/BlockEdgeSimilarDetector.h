#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#include "tbb/tbb.h"  
#include "tbb/task.h" 
#include "tbb/tbb_stddef.h" 
#pragma comment(lib, "tbb.lib")  
//#define SAVE_IMG
//#define BED_OUTPUT_DEBUG_INFO 1

//根据相似度计算边缘是否有缺陷
class BlockEdgeSimilarDetector
{
public:
	BlockEdgeSimilarDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeSimilarDetector();
	void Run();


#ifdef BED_OUTPUT_DEBUG_INFO
	cv::Mat drowDebugResult;
	vector<cv::Mat> debug_ups;
	vector<cv::Mat> debug_downs;
	vector<cv::Mat> debug_lefts;
	vector<cv::Mat> debug_rights;
#endif
	//统计数据，作为定标使用。

	//水平方向（左右边）统计出的最大深度
	int maxDeep_X = 0;
	//竖直方向（上下边）统计出的最大深度
	int maxDeep_Y = 0;

	//水平方向（左右边）差值的最大值
	double maxdiff_X = 0;
	//竖直方向（上下边）差值的最大值
	double maxdiff_Y = 0;


	//parameter
	static bool Enabled;
	static double DIFF_THRESHOLD;	//两点差值百分比超过这个阈值则认为可能有缺陷
	static double FAULTS_SPAN;	//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	static double FAULTS_COUNT;	//连续几个点大于DIFF_THRESHOLD则判定为缺陷

private:

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	const int ROI_WIDTH_UPDOWN = 50;
	int ROI_HEIGHT_UP = 30;
	int ROI_HEIGHT_DOWN = 30;
	const int ROI_WIDTH_INC = 25;

	int ROI_WIDTH_LEFTRIGHT = 40;
	const int ROI_HEIGHT_LEFTRIGHT = 70;
	const int ROI_HEIGHT_INC = 70;//(float)(endY - startY) / 60 + 0.5;//范围增量


	void doUp();
	//获取上边进边深度
	int getDeepUp(cv::Point);
	void doDown();
	//获取下边进边深度
	int getDeepDown(cv::Point);
	void doLeft();
	void doRight();

	void process(vector<cv::Mat> reduceList, vector<cv::Point> points,string info);
	double getFrechetDistance(vector<double> lineA, vector<double> lineB);
	double getFrechetDistance(cv::Mat lineA, cv::Mat lineB);
};

