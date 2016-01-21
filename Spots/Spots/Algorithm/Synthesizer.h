#pragma once

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

#include <Class\Debug\MFCConsole.h>

using namespace std;
//�㷨�ۺ���������ͳ�������㷨����
class Synthesizer
{
public:
	enum Status
	{
		NoImage = 0,//��ͼ
		NotFound = 1,//δ�ҵ���ש
		TypeA = 2,//A��
		TypeB = 3,//B��
		TypeC = 4,//C��
		Rejected = 5//���ϸ�
	};
	Synthesizer(int _SN);
	~Synthesizer();
	//�����������������Ƭ������3����
	Status Run(cv::Mat linerGrayImg);

	//���
	int SN = 0;
	//��ֵ������ֵ
	int ConsumerThreshodLow = 5;
	//��ֵ������ֵ
	int ConsumerThreshodHight = 9;

	int ConsumerLedStartX = 0;
	int ConsumerLedEndX = 4095;
private:
	Block *p_block = NULL;
	Faults faults;

	//��ש��λ�������Ƿ��ҵ���ש
	bool positioning(cv::Mat grayImg);
	//��Եȱ�ݼ��
	void detectEdge(cv::Mat grayImg);
	//�ڲ�ȱ�ݼ��
	void detectInner(cv::Mat grayImg);
};