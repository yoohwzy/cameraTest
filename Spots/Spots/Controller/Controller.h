#pragma once

#include <afxwin.h>

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h"
#include "../Class/IOCard/PCI1761/PCI1761.h"
#include "Worker.h"

#include "../View/SpotsMainView.h"

#include <opencv2\opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include "../globle_debug.h"


using namespace std;

class Controller
{
protected:
	SpotsMainView*  spotsMainView = NULL;

public:
	Controller(SpotsMainView* _spotsMainView) :
		spotsMainView(_spotsMainView)
	{
		ATLASSERT(spotsMainView);
		spotsMainView->AddController(this);

		init();
	}
	~Controller()
	{
		release();
	}

	void init(){ 
		//开启图像缓存
		p_e2vbuffer = new E2VBuffer(4096, true);
		p_imgscanner = new ImgScanner(p_e2vbuffer);
		bool e2vInitFlag = true;
		//初始化e2v_EV71YC1CCL4005BA0失败
		if (!p_imgscanner->StartFlag)
		{
			if (p_imgscanner != NULL)
			{
				delete p_imgscanner;
				p_imgscanner = NULL;
			}
			if (p_e2vbuffer != NULL)
			{
				delete p_e2vbuffer;
				p_e2vbuffer = NULL;
			}
			e2vInitFlag = false;
			//AfxMessageBox(L"初始化e2v_EV71YC1CCL4005BA0失败！");
		}

		bool pci1761InitFlag = true;
		if (!pci1761.init())
		{
			pci1761InitFlag = false;
			//AfxMessageBox(L"初始化PCI-1761失败！");
		}





		if (e2vInitFlag && pci1761InitFlag)
		{
			//初始化工人
			worker1 = new Worker(p_e2vbuffer);
			worker2 = new Worker(p_e2vbuffer);

			//开始监控触发
			std::thread t_tiggerThread(std::mem_fn(&Controller::triggerWatcher), this);
			t_tiggerThread.detach();
			//std::thread t_jobThread(std::mem_fn(&Controller::jobWatcher), this);
			//t_jobThread.detach();
		}
		else
		{
			if (!e2vInitFlag && !pci1761InitFlag)
				AfxMessageBox(L"线阵相机&pci1761初始化失败！");
			else if (!pci1761InitFlag)
				AfxMessageBox(L"pci1761初始化失败！");
			else if (!e2vInitFlag)
				AfxMessageBox(L"线阵相机初始化失败！");


			//开启虚拟相机
			cv::Mat virtualImg;
			worker1 = new Worker(NULL);
			worker1->image = virtualImg;
		}
	}
	void release()
	{
		exitFlag = true;
		Sleep(10);
		p_e2vbuffer = NULL;
		if (worker1 != NULL)
		{
			delete worker1;
			worker1 = NULL;
		}
		if (worker2 != NULL)
		{
			delete worker2;
			worker2 = NULL;
		}

		if (p_imgscanner != NULL)
		{
			delete p_imgscanner;
			p_imgscanner = NULL;
		}
		if (p_e2vbuffer != NULL)
		{
			delete p_e2vbuffer;
			p_e2vbuffer = NULL;
		}
	}



	void Start()
	{
		cv::Mat img(100, 100, CV_8UC3, cv::Scalar(0,255,0));
		ui_lock.lock();
		spotsMainView->ShowBigImg(img);
		ui_lock.unlock();
	}





private:
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