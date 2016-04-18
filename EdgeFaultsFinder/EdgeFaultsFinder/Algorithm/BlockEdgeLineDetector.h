#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define BED_OUTPUT_DEBUG_INFO 1


//崩边检测
class BlockEdgeLineDetector
{
public:
	BlockEdgeLineDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeLineDetector();
	void Run();


#ifdef BED_OUTPUT_DEBUG_INFO
	cv::Mat drowDebugResult;
	vector<cv::Mat> debug_ups;
	vector<cv::Mat> debug_downs;
	vector<cv::Mat> debug_lefts;
	vector<cv::Mat> debug_rights;
#endif

	int THRESHOD = 10;//二值化阈值
	//parameter
	double DIFF_THRESHOLD = 7;	//两点差值百分比超过这个阈值则认为可能有缺陷
	int FAULTS_SPAN = 4;	//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	int FAULTS_COUNT = 5;	//连续几个点大于DIFF_THRESHOLD则判定为缺陷
	int DEEP_THRESHOD;//崩边超过这个深度
private:

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//返回最大偏差值

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	int getDeepUp(cv::Point p);
	//获得某点到左边的距离
	//返回距离（pix）
	int getDeepLeft(int x);

	void processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points);
	void processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points);
};

