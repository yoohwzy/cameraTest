#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define EFLD_OUTPUT_DEBUG_INFO 1


//��������ȱ��  ��ɨ ���
class EdgeFaultLineDetector
{
public:
	EdgeFaultLineDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~EdgeFaultLineDetector();
	void Run();


#ifdef EFLD_OUTPUT_DEBUG_INFO
	cv::Mat drowDebugResult;
	vector<cv::Mat> debug_ups;
	vector<cv::Mat> debug_downs;
	vector<cv::Mat> debug_lefts;
	vector<cv::Mat> debug_rights;
#endif
	//ͳ�����ݣ���Ϊ����ʹ�á�

	//ͳ�Ƴ���������
	int maxDeepPix = 0;
	//ͳ�Ƴ�����󳤶�
	int maxLengthPix = 0;

	//ͳ�Ƴ�����С���
	int minDeepPix = 9999;
	//ͳ�Ƴ�����С����
	int minLengthPix = 9999;


	//parameter
	int BINARY_THRESHOD = 25;//�߽�ȷ����ֵ
	int LENGTH_THRESHOD = 10;//���߳���Ҫ�������
	int DEEP_THRESHOD = 10;//�������Ҫ�������
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
	int getDeepDown(cv::Point p);
	int getDeepLeft(cv::Point p);
	int getDeepRight(cv::Point p);
	
	//���ĳ�㵽��ߵľ���
	//���ؾ��루pix��
	int getDeepLeft(int x);

	void processVBS(vector<Faults::BrokenEdge> vbs, bool isUpDown);
};
