#pragma once
#include "ControllerModel.h"

#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraDirectRead.h>//必须放在这，否则会报错  XX定义不明确

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

	//拍摄的长度，多少行
	int PhotoHeight = 5000;
private:
	E2VCameraDirectRead *p_e2v = NULL;
	void triggerWatcherThread() override;//触发器监视线程，发出IsGrabbing2=1时，相机监视线程开始采图
	void captureAndProcessThread(int sn = 0) override;//采图与工作委托

	//相机正在采图标志
	bool captureIsRunning = false;
};