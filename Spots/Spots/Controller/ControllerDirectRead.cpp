#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerDirectRead.h"
#include <Class/Setting/SettingHelper.h>
#include <Class/Helper/StringHelper.h>
#include <Class/Debug/MFCConsole.h>
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //�ļ�Ŀ¼lib ���û�����У������link����

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

	//��ʼ��E2V���
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
			//����ʼ��e2v_EV71YC1CCL4005BA0ʧ��
			//��ɾ����ʵ�����Ķ����л�Ϊ�������ģʽ
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
		//��ʼ��ش���
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
		if (PauseFlag)//��ͣ��־
		{
			Sleep(50);
			continue;
		}
		//if (pci1761.GetRisingEdgeIDI(7))//�����ؿ�ʼ��ͼ
		if (pci1761.GetTrailingEdgeIDI(7))//��ʼ��ͼ
		{
			if (1 == 1)
			{
				t = cv::getTickCount();
				tiggerTimeSpan = (cv::getTickCount() - tiggerTimeSpan) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << "\r\n\r\n���ϴδ������Timespan:" << tiggerTimeSpan << "ms" << endl;
				tiggerTimeSpan = cv::getTickCount();
				MFCConsole::Output(ss.str());
			}

			//������ȴ�ʱ��
			if (Capture_WaitTimeMSIn > 0)
			{
#ifdef Controller_DEBUG
				stringstream ss;
				double tSleep = (double)cv::getTickCount();
#endif
				//�����󣬵ȴ�ש������������
				//Sleep(globle_var::TiggerCaptureWaitTimeMS);
				std::this_thread::sleep_for(chrono::milliseconds(Capture_WaitTimeMSIn));

#ifdef Controller_DEBUG
				tSleep = ((double)cv::getTickCount() - tSleep) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << "Capture_WaitTimeMSIn Sleep real(ms):" << tSleep << endl;
				printf_globle(ss.str());
#endif
			}

			//��ʼ��ͼ
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
				ss << "        ��������Ӧ����ͼ���ڽ�����  captureIsRunning = 1 ,still running capture();" << endl;
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

	//ȡͼ
	cv::Mat image = p_e2v->GetImage();
	captureIsRunning = false;
	

	//���Ĳ��� ���乤��
	workerindex++;
	if (workerindex > 1) workerindex = 0;
	//�������������
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