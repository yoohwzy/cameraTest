#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerDirectRead.h"
#include <Class/Setting/SettingHelper.h>
#include <Class/Helper/StringHelper.h>
#include <Class/Debug/MFCConsole.h>
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //文件目录lib 如果没有这行，会出现link错误

void ControllerDirectRead::Init()
{
	Release();
	stringstream ss;

	//ss.str("");
	//ss << "IsRealModel = " << IsRealModel << endl;
	//MFCConsole::Output(ss.str());

	if (IsRealModel)
	{
		int virtualCamEnable = 1;
		if (SettingHelper::GetKeyInt("E2V", "Virtual_Cam_Enable", virtualCamEnable))
			IsRealModel = virtualCamEnable == 0;
	}

	//ss.str("");
	//ss << "IsRealModel = " << IsRealModel << endl;
	//MFCConsole::Output(ss.str());

	//初始化E2V相机
	if (IsRealModel)
	{
		int Cam_FrameCount_PerSecond = 5000;
		SettingHelper::GetKeyInt("E2V", "Cam_FrameCount_PerSecond", Cam_FrameCount_PerSecond);
		int COLOR_TYPE_IS_GRAY = 1;
		SettingHelper::GetKeyInt("E2V", "COLOR_TYPE_IS_GRAY", COLOR_TYPE_IS_GRAY);
		if (COLOR_TYPE_IS_GRAY != 0)
			p_e2v = new	E2VCameraDirectRead(4096, PhotoHeight, CV_8U, 0, PORT_A);
		else
			p_e2v = new	E2VCameraDirectRead(4096, PhotoHeight, CV_8UC3, 0, PORT_A);


		if (!p_e2v->HasBeenInited)
		{
			//若初始化e2v_EV71YC1CCL4005BA0失败
			//则删除已实例化的对象，切换为虚拟相机模式
			if (p_e2v != NULL)
			{
				delete p_e2v;
				p_e2v = NULL;
			}
			e2vInited = false;
		}

	}
	baseInit();
}
void ControllerDirectRead::Release()
{
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

	if (p_e2v != NULL)
	{
		delete p_e2v;
		p_e2v = NULL;
	}
}

void ControllerDirectRead::TiggerStartWatch()
{
	if (IsRealModel)
	{
		watcher_lock.lock();

		ExitFlag = false;
		//开始监控触发
		std::thread t_tiggerThread(std::mem_fn(&ControllerDirectRead::triggerWatcherThread), this);
		auto tn = t_tiggerThread.native_handle();
		SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
		t_tiggerThread.detach();

		watcher_lock.unlock();
	}
}

void ControllerDirectRead::TiggerStopWatch()
{
	if (IsRealModel)
	{
		watcher_lock.lock();

		ExitFlag = true;

		watcher_lock.unlock();
	}
}

//private:



void ControllerDirectRead::triggerWatcherThread()
{
	double t = cv::getTickCount();
	double tiggerTimeSpan = cv::getTickCount();
	while (!ExitFlag)
	{
		if (PauseFlag)//暂停标志
		{
			Sleep(50);
			continue;
		}
		//if (pci1761.GetRisingEdgeIDI(7))//上升沿开始采图
		if (pci1761.GetTrailingEdgeIDI(7))//开始采图
		{
			if (1 == 1)
			{
				t = cv::getTickCount();
				tiggerTimeSpan = (cv::getTickCount() - tiggerTimeSpan) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << "\r\n\r\n与上次触发间隔Timespan:" << tiggerTimeSpan << "ms" << endl;
				tiggerTimeSpan = cv::getTickCount();
				MFCConsole::Output(ss.str());
			}

			//触发后等待时间
			if (Capture_WaitTimeMSIn > 0)
			{
#ifdef Controller_DEBUG
				stringstream ss;
				double tSleep = (double)cv::getTickCount();
#endif
				//触发后，等待砖进入拍摄区。
				//Sleep(globle_var::TiggerCaptureWaitTimeMS);
				std::this_thread::sleep_for(chrono::milliseconds(Capture_WaitTimeMSIn));

#ifdef Controller_DEBUG
				tSleep = ((double)cv::getTickCount() - tSleep) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << "Capture_WaitTimeMSIn Sleep real(ms):" << tSleep << endl;
				printf_globle(ss.str());
#endif
			}

			//开始采图
			if (!captureIsRunning)
			{
				std::thread t_run(std::mem_fn(&ControllerDirectRead::captureAndProcessThread), this);
				//auto tn = t_run.native_handle();
				//SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
				t_run.detach();
			}
			else
			{
				stringstream ss;
				ss << "        触发后不响应，采图正在进行中  captureIsRunning = 1 ,still running capture();" << endl;
				MFCConsole::Output(ss.str());
			}
			//PauseFlag == 1;
		}
		Sleep(10);
	}
}



void ControllerDirectRead::captureAndProcessThread()
{
	captureIsRunning = true;
	captureIndex++;
	if (captureIndex > 60000)
		captureIndex = 1;
	if (MFCConsole::IsOpened)
	{
		stringstream ss;
		ss << "                                         captureIndex :" << captureIndex << endl;
		MFCConsole::Output(ss.str());
	}

	//取图
	cv::Mat image = p_e2v->GetImage();
	captureIsRunning = false;
	

	//第四部分 分配工作
	workerindex++;
	if (workerindex > 1) workerindex = 0;
	//分配任务给工人
	stringstream ss;
	if (workerindex == 0)
	{
		if (worker1->MyStatus == Worker::Free)
		{
			worker1->SN = captureIndex;
			
			worker1->image = image;
			ss << "-------------------------Worker1 Start Work\r\n" << worker1->SN << endl;
			MFCConsole::Output(ss.str());
			worker1->StartWork();
		}
		else
		{
			MFCConsole::Output(ss.str());
			MFCConsole::Output("!!!!!!!!!!!!!!!!!!!!!!!!!!Worker1 Is busy\r\n");
		}

	}
	else if (workerindex == 1)
	{
		if (worker2->MyStatus == Worker::Free)
		{
			worker2->SN = captureIndex;
			worker2->image = image;
			ss << "-------------------------Worker2 Start Work\r\n" << worker2->SN << endl;
			MFCConsole::Output(ss.str());
			worker2->StartWork();
		}
		else
		{
			MFCConsole::Output(ss.str());
			MFCConsole::Output("!!!!!!!!!!!!!!!!!!!!!!!!!!Worker2 Is busy\r\n");
		}
	}
}