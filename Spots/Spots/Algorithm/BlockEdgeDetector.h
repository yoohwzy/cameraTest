#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define BED_OUTPUT_DEBUG_INFO 1

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

	//ͳ�����ݣ���Ϊ����ʹ�á�

	//ˮƽ�������ұߣ�ͳ�Ƴ���������
	int maxDeep_X = 0;
	//��ֱ�������±ߣ�ͳ�Ƴ���������
	int maxDeep_Y = 0;

	//ˮƽ�������ұߣ���ֵ�����ֵ
	int maxdiff_X = 0;
	//��ֱ�������±ߣ���ֵ�����ֵ
	int maxdiff_Y = 0;
private:
	const int JUDGEMENT_THRESHOLD = 7;//���������ֵ����Ϊ������ȱ��
	int FAULTS_SPAN = 4;//�������� 
	int FAULTS_COUNT = 5;

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//�������ƫ��ֵ

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	void processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points);
	void processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points);
};

