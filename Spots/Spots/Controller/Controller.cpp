#include "Controller.h"


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
				worker1->StartWork();
				lastestWorker = worker1;
			}
			else
			{
				printf_globle("worker2 Start Work\r\n");
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