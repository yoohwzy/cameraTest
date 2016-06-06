#pragma once
#include "ControllerModel.h"

#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraDirectRead.h>//��������⣬����ᱨ��  XX���岻��ȷ
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

class ControllerDirectRead :public ControllerModel
{
public:
	ControllerDirectRead(SpotsMainView* _spotsMainView) : ControllerModel(_spotsMainView)
	{
	}
	~ControllerDirectRead()
	{
		ExitFlag = true;
		Sleep(100);
		Release();
	}

	void Init() override;
	void Release() override;
	void TiggerStartWatch() override;
	void TiggerStopWatch() override;

	//����ĳ��ȣ�������
	int PhotoHeight = 5000;
	const int FrameRate = 5000;//��ǰ���������Ƶ
private:
	E2VCameraDirectRead *p_e2v = NULL;
	MainHueScanner *p_mainHueScanner = NULL;
	void triggerWatcherThread() override;//�����������̣߳�����IsGrabbing2=1ʱ����������߳̿�ʼ��ͼ
	void captureAndProcessThread() override;//��ͼ�빤��ί��

	//������ڲ�ͼ��־
	bool captureIsRunning = false;
	long captureIndex = 0;
};