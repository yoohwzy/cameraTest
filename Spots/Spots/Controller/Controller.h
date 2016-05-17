#pragma once
#include "ControllerModel.h"

#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraCycleBuffer.h>//��������⣬����ᱨ��  XX���岻��ȷ
#include <Class/IOCard/PCI1761/PCI1761.h>
#include <Class/Statistics/Statistics.h>

#include <View/SpotsMainView.h>

#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include <Model\Worker.h>
#include <Model\LogImgGenerator.h>
#include <Model\Arm.h>


using namespace std;

//#define Controller_DEBUG

class Controller :public ControllerModel
{
public:
	Controller(SpotsMainView* _spotsMainView) : ControllerModel(_spotsMainView)
	{
	}
	~Controller()
	{
		ExitFlag = true;
		Sleep(100);

		release();
	}
	//�Ƿ�Ϊ�������ģʽ��������ͨ���ⲿ���ã�������exeʱ��������virtual�����IsRealModel=0��
	bool IsRealModel = 1;


	void init() override;
	void release() override;
	void TiggerStartWatch() override;
	void TiggerStopWatch() override;

	/*****************�������ģʽ����*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************�������ģʽ���� End*****************/
private:
	E2VCameraCycleBuffer *p_e2v = NULL;

	void triggerWatcherThread();//�����������̣߳�����IsGrabbing2=1ʱ����������߳̿�ʼ��ͼ
	void captureAndassembleThread();//��ͼ�빤��ί��
	int index = 0;
	int workerindex = 0;


	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VCycleBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			oldFrame -= E2VCycleBuffer::BufferLength;
	}




	bool IsGrabbing = false;//���ڿ�����ͼ�߳�
	bool IsGrabbing2 = false;//���ڲ�ͼ
};