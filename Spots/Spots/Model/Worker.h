#pragma once
#include "ImgScanner.h"
#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h>
#include <thread>

//�����㷨�ۺ���
#include <Algorithm\Synthesizer.h>
#include <Class\Debug\MFCConsole.h>

extern class Controller;
class Worker
{
public:
	//��ʼ��ʱ������ָ��ΪNULL�����ʾʹ�����������
	Worker(E2VBuffer *_e2vbuffer = NULL);
	~Worker();

	enum WorkerStatus{
		Free,
		Busy,
		Done
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;

	// Ϊtrueʱ�ɼ�ͼ��תΪfalseʱ�����ɼ�
	// ����length = 0ʱ��Ч������ȷ���ɼ���ʱ����
	bool GetPhotoOn = false;
	cv::Mat image;

	Controller *P_Controller = NULL;



	//��Ҫ���õĲ���
	int Real_WidthMM = 600;//ͼ���д�ש�ĺ᳤
	int Real_LengthMM = 300;//ͼ���д�ש���ݳ�
	int WaitTimeMSIn = 50;//�ȴ���ש������������ʱ��
	int WaitTimeMSOut = 50;//�ȴ���ש�뿪��������ʱ��
	int FrameTimeOut = 2000;//���㳬ʱʱ��
	// ��ש���к�
	int SN = 0;

	//edge ����

	double BlockLocalizer_THRESHOD = 10;			//��Ե���Ҷ�ֵ����ֵ
	double BlockLocalizer_ContinuePointCount = 30;	//�������ٸ������ж�Ϊ��Ե

	int  BlockEdgeDetector_Enable = 1;
	double BlockEdgeDetector_DIFF_THRESHOLD = 7;	//��ͼ֮��������ֵ����Ϊ������ȱ��
	double BlockEdgeDetector_FAULTS_SPAN = 4;		//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	double BlockEdgeDetector_FAULTS_COUNT = 5;		//�������������DIFF_THRESHOLD���ж�Ϊȱ��

	int BlockEdgeLineDetector_Enable = 1;

	/**************�ּ�����*************/

	//�߲���
	double Classify_EDGE_SINGLE_LENGTH_A = 0;
	double Classify_EDGE_SINGLE_LENGTH_B = 0;
	double Classify_EDGE_SINGLE_LENGTH_C = 0;
	double Classify_EDGE_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_EDGE_TOTAL_LENGTH_A = 0;
	double Classify_EDGE_TOTAL_LENGTH_B = 0;
	double Classify_EDGE_TOTAL_LENGTH_C = 0;
	double Classify_EDGE_TOTAL_LENGTH_ACCEPT = 0;

	double Classify_EDGE_SINGLE_DEEP_A = 0;
	double Classify_EDGE_SINGLE_DEEP_B = 0;
	double Classify_EDGE_SINGLE_DEEP_C = 0;
	double Classify_EDGE_SINGLE_DEEP_ACCEPT = 0;

	double Classify_EDGE_TOTAL_DEEP_A = 0;
	double Classify_EDGE_TOTAL_DEEP_B = 0;
	double Classify_EDGE_TOTAL_DEEP_C = 0;
	double Classify_EDGE_TOTAL_DEEP_ACCEPT = 0;

	//���Ӳ���
	double Classify_HOAL_DIAMETER_A = 0;
	double Classify_HOAL_DIAMETER_B = 0;
	double Classify_HOAL_DIAMETER_C = 0;
	double Classify_HOAL_DIAMETER_ACCEPT = 0;

	double Classify_HOAL_COUNT_A = 0;
	double Classify_HOAL_COUNT_B = 0;
	double Classify_HOAL_COUNT_C = 0;
	double Classify_HOAL_COUNT_ACCEPT = 0;


	//����
	double Classify_SCRATCH_SINGLE_LENGTH_A = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_B = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_C = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_SCRATCH_TOTAL_LENGTH_A = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_B = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_C = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = 0;


private:
	//double axis_x_mmPerPix = 1;//x�᷽����ÿ���ش�����ٺ���
	//double axis_y_mmPerPix = 1;//y�᷽����ÿ���ش�����ٺ���

	E2VBuffer *p_e2vbuffer = NULL;
	// ֱ�Ӷ�ȡN��ͼƬ
	void work();
	// ��ѭ��������ȡ��ͼƬ
	// ��ʼ�ɼ����У��ɼ�������Ϊ0ʱ�ȴ��½��ػ�ɼ���������frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// ����ȱ��
	void drawFaults(cv::Mat&, Faults& faults);

	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			oldFrame -= E2VBuffer::BufferLength;
	}
};