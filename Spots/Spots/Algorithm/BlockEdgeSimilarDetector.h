#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define SAVE_IMG
#define BED_OUTPUT_DEBUG_INFO 1

//�������ƶȼ����Ե�Ƿ���ȱ��
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
	//ͳ�����ݣ���Ϊ����ʹ�á�

	//ˮƽ�������ұߣ�ͳ�Ƴ���������
	int maxDeep_X = 0;
	//��ֱ�������±ߣ�ͳ�Ƴ���������
	int maxDeep_Y = 0;

	//ˮƽ�������ұߣ���ֵ�����ֵ
	double maxdiff_X = 0;
	//��ֱ�������±ߣ���ֵ�����ֵ
	double maxdiff_Y = 0;


	//parameter
	double DIFF_THRESHOLD = 0.4;	//�����ֵ�ٷֱȳ��������ֵ����Ϊ������ȱ��
	int FAULTS_SPAN = 4;	//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	int FAULTS_COUNT = 5;	//�������������DIFF_THRESHOLD���ж�Ϊȱ��

private:

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//�������ƫ��ֵ

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	void process(vector<cv::Mat> reduceList, vector<cv::Point> points,string info);
};

