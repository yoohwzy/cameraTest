#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define BED_OUTPUT_DEBUG_INFO 1


//���߼��
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

	int THRESHOD = 10;//��ֵ����ֵ
	//parameter
	double DIFF_THRESHOLD = 7;	//�����ֵ�ٷֱȳ��������ֵ����Ϊ������ȱ��
	int FAULTS_SPAN = 4;	//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	int FAULTS_COUNT = 5;	//�������������DIFF_THRESHOLD���ж�Ϊȱ��
	int DEEP_THRESHOD;//���߳���������
private:

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//�������ƫ��ֵ

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	int getDeepUp(cv::Point p);
	//���ĳ�㵽��ߵľ���
	//���ؾ��루pix��
	int getDeepLeft(int x);

	void processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points);
	void processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points);
};

