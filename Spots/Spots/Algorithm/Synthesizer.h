#pragma once

//#include "../globle_debug.h"
#include <opencv2\opencv.hpp>
//�����㷨
#include <Algorithm/Base/Block.h>
#include <Algorithm/Base/Faults.h>

#include <Algorithm/BlocksDetector.h>
#include <Algorithm/EdgeDetector.h>
#include <Algorithm/Processor.h>
#include <Algorithm/EdgeDetector.h>
#include <Algorithm/EdgeInnerDetctor.h>
#include <Algorithm/Pretreatment.h>

using namespace std;
//�㷨�ۺ���������ͳ�������㷨����
class Synthesizer
{
public:
	Synthesizer();
	~Synthesizer();
	//�����������������Ƭ������3����
	void Run(cv::Mat linerGrayImg);

	//���
	int SN = 0;
	//��ֵ������ֵ
	int ConsumerThreshodLow = 5;
	//��ֵ������ֵ
	int ConsumerThreshodHight = 9;

	int ConsumerLedStartX = 0;
	int ConsumerLedEndX = 4095;
private:
	Block *block = NULL;
	Faults faults;

	//��ש��λ
	void positioning();
	//��Եȱ�ݼ��
	void detectEdge();
	//�ڲ�ȱ�ݼ��
	void detectInner();
};