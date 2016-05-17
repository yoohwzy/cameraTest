#pragma once
#include "ControllerModel.h"

#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraCycleBuffer.h>//必须放在这，否则会报错  XX定义不明确
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
	//是否为虚拟相机模式，本属性通过外部配置，在启动exe时若附加了virtual命令，则IsRealModel=0；
	bool IsRealModel = 1;


	void init() override;
	void release() override;
	void TiggerStartWatch() override;
	void TiggerStopWatch() override;

	/*****************虚拟相机模式方法*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************虚拟相机模式方法 End*****************/
private:
	E2VCameraCycleBuffer *p_e2v = NULL;

	void triggerWatcherThread();//触发器监视线程，发出IsGrabbing2=1时，相机监视线程开始采图
	void captureAndassembleThread();//采图与工作委托
	int index = 0;
	int workerindex = 0;


	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VCycleBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			oldFrame -= E2VCycleBuffer::BufferLength;
	}




	bool IsGrabbing = false;//正在开启采图线程
	bool IsGrabbing2 = false;//正在采图
};