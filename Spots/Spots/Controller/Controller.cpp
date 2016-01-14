#include "Controller.h"


Controller::Controller()
{
	//handle_mainWindow = _handle_mainWindow;
	p_e2vbuffer = new E2VBuffer(4096, true);
	p_imgscanner = new ImgScanner(p_e2vbuffer);


	pci1761.init();
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
	while (!exitFlag)
	{
		if (pci1761.GetRisingEdgeIDI(7))//上升沿开始采图
		{
			tiggerindex++;
			if (tiggerindex >= 100)
				tiggerindex = 0;

			if (tiggerindex % 2 == 1)
			{
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