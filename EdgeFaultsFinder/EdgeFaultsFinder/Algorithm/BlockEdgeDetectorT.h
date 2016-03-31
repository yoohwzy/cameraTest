#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define BED_OUTPUT_DEBUG_INFO 1
//���ڸߵ���ֵ�ı�Եȱ�ݲ���

class BlockEdgeDetectorT
{
public:
	BlockEdgeDetectorT(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeDetectorT();
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
	int maxdiff_X = 0;
	//��ֱ�������±ߣ���ֵ�����ֵ
	int maxdiff_Y = 0;


	//parameter
	int DIFF_THRESHOLD = 7;	//��ͼ֮��������ֵ����Ϊ������ȱ��
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


	void getContoursUpDown(cv::Mat binaryImage, vector<cv::Point2f>& contours);
	void getContoursLeftRight(cv::Mat binaryImage, vector<cv::Point2f>& contours);

	//�����Ե������
	//����1��
	void process(vector<vector<cv::Point>> contours);
	void process(vector<cv::Point2f> contours);

	void testag(vector<cv::Point2f> contour);//ʵ���㷨���ֶ���ϣ��ҳ��������б���йŹֵ�����ϸ�·�����

	void processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points);
	void processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points);
};

