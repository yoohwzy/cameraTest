#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define SAVE_IMG
#define BED_OUTPUT_DEBUG_INFO 1

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
	double DIFF_THRESHOLD = 0.4;	//两点差值百分比超过这个阈值则认为可能有缺陷
	int FAULTS_SPAN = 4;	//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	int FAULTS_COUNT = 5;	//连续几个点大于DIFF_THRESHOLD则判定为缺陷

private:

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//返回最大偏差值

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	void process(vector<cv::Mat> reduceList, vector<cv::Point> points,string info);
};

