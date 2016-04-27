#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define BELD_OUTPUT_DEBUG_INFO 1


//崩边检测
class BlockEdgeLineDetector
{
public:
	BlockEdgeLineDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeLineDetector();
	void Run();


#ifdef BELD_OUTPUT_DEBUG_INFO
	cv::Mat drowDebugResult;
	vector<cv::Mat> debug_ups;
	vector<cv::Mat> debug_downs;
	vector<cv::Mat> debug_lefts;
	vector<cv::Mat> debug_rights;
#endif

	//parameter
	int BINARY_THRESHOD = 5;//边界确定阈值
	int LENGTH_THRESHOD = 10;//崩边长度要超过这个
	int DEEP_THRESHOD = 5;//崩边深度要超过这个
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
	int getDeepDown(cv::Point p);
	int getDeepLeft(cv::Point p);
	int getDeepRight(cv::Point p);
	
	//获得某点到左边的距离
	//返回距离（pix）
	int getDeepLeft(int x);

	void processVBS(vector<Faults::BrokenEdge> vbs, bool isUpDown);
};

