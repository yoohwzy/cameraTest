#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#include "tbb/tbb.h"  
#include "tbb/task.h" 
#include "tbb/tbb_stddef.h" 
#pragma comment(lib, "tbb.lib")  
//#define SAVE_IMG
//#define BED_OUTPUT_DEBUG_INFO 1

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
	static bool Enabled;
	static double DIFF_THRESHOLD;	//�����ֵ�ٷֱȳ��������ֵ����Ϊ������ȱ��
	static double FAULTS_SPAN;	//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	static double FAULTS_COUNT;	//�������������DIFF_THRESHOLD���ж�Ϊȱ��

private:

	cv::Mat image;
	Faults *p_faults = NULL;
	Block *p_block = NULL;


	const int ROI_WIDTH_UPDOWN = 50;
	int ROI_HEIGHT_UP = 30;
	int ROI_HEIGHT_DOWN = 30;
	const int ROI_WIDTH_INC = 25;

	int ROI_WIDTH_LEFTRIGHT = 40;
	const int ROI_HEIGHT_LEFTRIGHT = 70;
	const int ROI_HEIGHT_INC = 70;//(float)(endY - startY) / 60 + 0.5;//��Χ����


	void doUp();
	//��ȡ�ϱ߽������
	int getDeepUp(cv::Point);
	void doDown();
	//��ȡ�±߽������
	int getDeepDown(cv::Point);
	void doLeft();
	void doRight();

	void process(vector<cv::Mat> reduceList, vector<cv::Point> points,string info);
	double getFrechetDistance(vector<double> lineA, vector<double> lineB);
	double getFrechetDistance(cv::Mat lineA, cv::Mat lineB);
};

