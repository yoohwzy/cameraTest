#pragma once


#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h"
#include "../Class/IOCard/PCI1761/PCI1761.h"

#include "../View/SpotsMainView.h"

#include <opencv2\opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include "../globle_debug.h"
#include "Worker.h"

using namespace std;

class Controller
{
protected:
	SpotsMainView*  spotsMainView = NULL;

public:
	Controller(SpotsMainView* _spotsMainView) :
		spotsMainView(_spotsMainView)
	{
		//ATLASSERT(spotsMainView);
		spotsMainView->AddController(this);

		//init();
	}
	~Controller()
	{
		release();
	}

	void init();

	//处理结束后显示处理结果
	void ShowWorkResult(cv::Mat image)
	{
		ui_lock.lock();
		spotsMainView->ShowBigImg(image);
		ui_lock.unlock();
	}

	/*****************虚拟相机模式方法*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************虚拟相机模式方法 End*****************/

private:
	void release();
	//HWND handle_mainWindow;

	E2VBuffer *p_e2vbuffer = NULL;
	ImgScanner *p_imgscanner = NULL;

	PCI1761 pci1761;

	//同时只允许两个工人工作，即只能两张图（两块砖）

	//最后一个被召唤的工人
	Worker *lastestWorker = NULL;
	//工人1
	Worker *worker1 = NULL;
	//工人2
	Worker *worker2 = NULL;


	std::mutex ui_lock;

	bool exitFlag = false;
	void triggerWatcher();
	void jobWatcher();
	void workerCallBack();
};