#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

//#define BED_OUTPUT_DEBUG_INFO 1

class BlockEdgeDetector
{
public:
	BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeDetector();

#ifdef BED_OUTPUT_DEBUG_INFO
	cv::Mat drowDebugResult;
	vector<cv::Mat> debug_ups;
	vector<cv::Mat> debug_downs;
	vector<cv::Mat> debug_lefts;
	vector<cv::Mat> debug_rights;
#endif

private:
	const int JUDGEMENT_THRESHOLD = 8;//���������ֵ����Ϊ��ȱ��
	int FAULTS_SPAN = 4;//��������
	int FAULTS_COUNT = 3;

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//�������ƫ��ֵ

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	int processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points);
};

