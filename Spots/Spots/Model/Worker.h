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
	//��Ҫ���õĲ���
	int WaitTimeMS = 50;//�ȴ���ש������������ʱ��

	// ��ש���к�
	int SN = 0;


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

	//edge ����
	int BlockLocalizer_THRESHOD = 10;			//��Ե���Ҷ�ֵ����ֵ
	int BlockLocalizer_ContinuePointCount = 30;	//�������ٸ������ж�Ϊ��Ե

	int BlockEdgeDetector_DIFF_THRESHOLD = 7;	//��ͼ֮��������ֵ����Ϊ������ȱ��
	int BlockEdgeDetector_FAULTS_SPAN = 4;		//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	int BlockEdgeDetector_FAULTS_COUNT = 5;		//�������������DIFF_THRESHOLD���ж�Ϊȱ��

private:
	E2VBuffer *p_e2vbuffer = NULL;
	// ֱ�Ӷ�ȡN��ͼƬ
	void work();
	// ��ѭ��������ȡ��ͼƬ
	// ��ʼ�ɼ����У��ɼ�������Ϊ0ʱ�ȴ��½��ػ�ɼ���������frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// ����ȱ��
	void drawFaults(cv::Mat&, Faults& faults);

	// �����ó���ֱ���˳�
	static const int frameCountsOut = 12000;
	
	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VBuffer::BufferLength)
			oldFrame -= E2VBuffer::BufferLength;
	}
};