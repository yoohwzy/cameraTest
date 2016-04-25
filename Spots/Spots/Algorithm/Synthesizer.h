#pragma once

#include <opencv2\opencv.hpp>
//�����㷨
#include <Algorithm/Base/Block.h>
#include <Algorithm/Base/Faults.h>


//#include <Algorithm/Pretreatment.h>

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
		Rejected = 5,//���ϸ�
	};
	Synthesizer(int _SN, int Real_WidthMM, int Real_LengthMM);
	~Synthesizer();
	//�����������������Ƭ������3����
	Status Run(cv::Mat TileImg);


	//���
	int SN = 0;
	Faults faults;
	Block *p_block = NULL;

	//parameter
	int Real_WidthMM = 600;//ͼ���д�ש�ĺ᳤
	int Real_LengthMM = 300;//ͼ���д�ש���ݳ�
	double BlockLocalizer_THRESHOD = 10;			//��Ե���Ҷ�ֵ����ֵ
	double BlockLocalizer_ContinuePointCount = 30;	//�������ٸ������ж�Ϊ��Ե

	int  BlockEdgeDetector_Enable = 1;
	double BlockEdgeDetector_DIFF_THRESHOLD = 7;	//��ͼ֮��������ֵ����Ϊ������ȱ��
	double BlockEdgeDetector_FAULTS_SPAN = 4;		//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	double BlockEdgeDetector_FAULTS_COUNT = 5;		//�������������DIFF_THRESHOLD���ж�Ϊȱ��

	int BlockEdgeLineDetector_Enable = 1;
private:


	enum _Status
	{
		_NotFound,
		_Edge_Broken,//����
		_NEXT//����
	};


	//��ש��λ�������Ƿ��ҵ���ש
	_Status positioning(cv::Mat grayImg);
	//��Եȱ�ݼ��
	_Status detectEdge(cv::Mat grayImg);
	//�ڲ�ȱ�ݼ��
	_Status detectInner(cv::Mat grayImg);
};