#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "Controller.h"


void Controller::init(){

	release();

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
	}

	bool pci1761InitFlag = true;
	if (!pci1761.init())
	{
		pci1761InitFlag = false;
	}





	if (e2vInitFlag && pci1761InitFlag)
	{
		//初始化工人
		worker1 = new Worker(p_e2vbuffer);
		worker2 = new Worker(p_e2vbuffer);

		worker1->P_Controller = this;
		worker2->P_Controller = this;
		//开始监控触发
		std::thread t_tiggerThread(std::mem_fn(&Controller::triggerWatcher), this);
		t_tiggerThread.detach();

		spotsMainView->SwitchModel2Virtual(false);

		MFCConsole::Output("已切换到真实相机模式。\r\n");
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
		worker1->P_Controller = this;

		spotsMainView->SwitchModel2Virtual(true);

		MFCConsole::Output("已切换到虚拟相机模式。\r\n");
	}

	// 统计数据初始化
	StatisticsController::InitDate();
	spotsMainView->UpdateStatistics();
	cv::Mat white(2, 2, CV_8U, cv::Scalar(255));
	spotsMainView->ShowBigImg(white);
}
void Controller::release()
{
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

void Controller::triggerWatcher()
{
	short int tiggerindex = 0;
	double t = cv::getTickCount();
	while (!exitFlag)
	{
		if (pci1761.GetRisingEdgeIDI(7))//上升沿开始采图
		{
			t = cv::getTickCount();

			tiggerindex++;
			if (tiggerindex >= 100)
				tiggerindex = 0;

			if (tiggerindex % 2 == 1)
			{
				MFCConsole::Output("worker1 Start Work\r\n");
				worker1->StartWork();
				lastestWorker = worker1;
			}
			else
			{
				MFCConsole::Output("worker2 Start Work\r\n");  
				 
				worker2->StartWork();
				lastestWorker = worker2;
			}
			Sleep(100);
		}
		else if (pci1761.GetTrailingEdgeIDI(7))//下降沿结束采图
		{
			MFCConsole::Output("Stop Work\r\n");
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			stringstream ss;
			ss << "Timespan:" << t << "ms" << endl;
			MFCConsole::Output(ss.str());

			lastestWorker->GetPhotoOn = false;
		}
		else
			this_thread::sleep_for(chrono::microseconds(1));
	}
}



/*********************  虚拟模式方法  ***********************/

void Controller::VirtualSelectImg(cv::Mat image)
{
	if (image.cols == 0)
		MessageBox(0, L"图片读取失败！", L"错误", 0);
	else
	{
		worker1->image = image;
	}
}
void Controller::VirtualWorkerStart()
{
	if (worker1->image.cols == 0)
	{
		MessageBox(0, L"请先加载虚拟底片！", L"错误", 0);
		return;
	}
	if (worker1->MyStatus == Worker::Done || worker1->MyStatus == Worker::Free)
	{
		worker1->StartWork();
	}
	else
	{
		MessageBox(0, L"上一轮处理尚未结束！", L"警告", 0);
	}
}
/*********************虚拟模式方法结束***********************/
