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

class ControllerCycleBuffer :public ControllerModel
{
public:
	ControllerCycleBuffer(SpotsMainView* _spotsMainView) : ControllerModel(_spotsMainView)
	{
	}
	~ControllerCycleBuffer()
	{
		ExitFlag = true;
		Sleep(100);
		Release();
	}

	void Init() override;
	void Release() override;
	void TiggerStartWatch() override;
	void TiggerStopWatch() override;
	void MainHueErrorCallBack(int SN, cv::Mat img) override;

private:
	E2VCameraCycleBuffer *p_e2v = NULL;
	void triggerWatcherThread() override;//�����������̣߳�����IsGrabbing2=1ʱ����������߳̿�ʼ��ͼ
	void captureAndProcessThread(int sn = 0) override;//��ͼ�빤��ί��

	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VCycleBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			oldFrame -= E2VCycleBuffer::BufferLength;
	}

	bool IsGrabbing = false;//���ڿ�����ͼ�߳�
	bool IsGrabbing2 = false;//���ڲ�ͼ
};