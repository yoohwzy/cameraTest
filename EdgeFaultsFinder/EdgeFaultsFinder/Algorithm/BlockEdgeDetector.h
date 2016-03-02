#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

class BlockEdgeDetector
{
public:
	BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeDetector();

private:
	const int JUDGEMENT_THRESHOLD = 8;//���������ֵ����Ϊ��ȱ��
	int FAULTS_SPAN = 5;//��������
	int FAULTS_COUNT = 5;

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	//�������ƫ��ֵ

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	int processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point3f> points);
};

