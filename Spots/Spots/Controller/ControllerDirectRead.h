#pragma once
#include "ControllerModel.h"

#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraDirectRead.h>//��������⣬����ᱨ��  XX���岻��ȷ

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
private:
	E2VCameraDirectRead *p_e2v = NULL;
	void triggerWatcherThread() override;//�����������̣߳�����IsGrabbing2=1ʱ����������߳̿�ʼ��ͼ
	void captureAndProcessThread(int sn = 0) override;//��ͼ�빤��ί��

	//������ڲ�ͼ��־
	bool captureIsRunning = false;
};