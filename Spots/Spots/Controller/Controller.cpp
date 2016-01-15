#include "Controller.h"


Controller::Controller()
{
	//handle_mainWindow = _handle_mainWindow;
	p_e2vbuffer = new E2VBuffer(4096, true);
	p_imgscanner = new ImgScanner(p_e2vbuffer);

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
		AfxMessageBox(L"初始化e2v_EV71YC1CCL4005BA0失败！");
	}
	if (!pci1761.init())
		AfxMessageBox(L"初始化PCI-1761失败！");
	
	worker1 = new Worker(p_e2vbuffer);
	worker2 = new Worker(p_e2vbuffer);
	std::thread t_tiggerThread(std::mem_fn(&Controller::triggerWatcher), this);
	t_tiggerThread.detach();
	//std::thread t_jobThread(std::mem_fn(&Controller::jobWatcher), this);
	//t_jobThread.detach();
}


Controller::~Controller()
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
				printf_globle("worker1 Start Work\r\n");
				//if (worker1->MyStatus != Worker::WorkerStatus::Free)
				//{
				//	delete worker1;
				//	worker1 = new Worker(p_e2vbuffer);
				//}
				worker1->StartWork();
				lastestWorker = worker1;
			}
			else
			{
				printf_globle("worker2 Start Work\r\n");
				//if (worker2->MyStatus != Worker::WorkerStatus::Free)
				//{
				//	delete worker2;
				//	worker2 = new Worker(p_e2vbuffer);
				//}
				worker2->StartWork();
				lastestWorker = worker2;
			}
			Sleep(100);
		}
		else if (pci1761.GetTrailingEdgeIDI(7))//下降沿结束采图
		{
			printf_globle("Stop Work\r\n");
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			stringstream ss;
			ss << "Timespan:" << t << "ms" << endl;
			printf_globle(ss.str());

			lastestWorker->GetPhotoOn = false;
		}
		else
			this_thread::sleep_for(chrono::microseconds(1));
	}
}

void Controller::jobWatcher()
{

	while (!exitFlag)
	{
		if (worker1->MyStatus == Worker::WorkerStatus::Done)
		{

		}
		else if (worker2->MyStatus == Worker::WorkerStatus::Done)
		{

		}
		else
			this_thread::sleep_for(chrono::microseconds(10));
	}
}