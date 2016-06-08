#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerCycleBuffer.h"
#include <Class/Setting/SettingHelper.h>
#include <Class\Helper\StringHelper.h>
#include <shlwapi.h>
#include <Class/Debug/MFCConsole.h>
#pragma comment(lib,"Shlwapi.lib") //�ļ�Ŀ¼lib ���û�����У������link����

void ControllerCycleBuffer::Init()
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
			p_e2v = new	E2VCameraCycleBuffer(4096, CV_8U, 1000000 / Cam_FrameCount_PerSecond, 0, PORT_A);
		else
			p_e2v = new	E2VCameraCycleBuffer(4096, CV_8UC3, 1000000 / Cam_FrameCount_PerSecond, 0, PORT_A);


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
void ControllerCycleBuffer::Release()
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

void ControllerCycleBuffer::TiggerStartWatch()
{
	if (IsRealModel)
	{
		watcher_lock.lock();

		ExitFlag = false;
		//��ʼ��ش���
		std::thread t_tiggerThread(std::mem_fn(&ControllerCycleBuffer::triggerWatcherThread), this);
		auto tn = t_tiggerThread.native_handle();
		SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
		t_tiggerThread.detach();

		watcher_lock.unlock();
	}
}

void ControllerCycleBuffer::TiggerStopWatch()
{
	if (IsRealModel)
	{
		watcher_lock.lock();

		ExitFlag = true;

		watcher_lock.unlock();
	}
}

//private:



void ControllerCycleBuffer::triggerWatcherThread()
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
			if (IsGrabbing)
			{
				MFCConsole::Output("IsGrabbing = 1 ��������Ӧ\r\n");
				continue;
			}

			IsGrabbing = true;
			t = cv::getTickCount();
			tiggerTimeSpan = (cv::getTickCount() - tiggerTimeSpan) * 1000 / cv::getTickFrequency();
			stringstream ss;
			ss << "\r\n\r\n���ϴδ������Timespan:" << tiggerTimeSpan << "ms" << endl;
			tiggerTimeSpan = cv::getTickCount();

			if (!IsGrabbing2)
			{
				std::thread t_run(std::mem_fn(&ControllerCycleBuffer::captureAndProcessThread), this, 0);
				//auto tn = t_run.native_handle();
				//SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
				t_run.detach();
			}
			else
			{
				stringstream ss;
				ss << "        ��������Ӧ  IsGrabbing2 = 1 ,still running capture();" << endl;
				MFCConsole::Output(ss.str());
			}
			//PauseFlag == 1;
			IsGrabbing = false;
		}
		Sleep(10);
	}
}



void ControllerCycleBuffer::captureAndProcessThread(int sn)
{
	IsGrabbing2 = true;

	if (MFCConsole::IsOpened)
	{
		stringstream ss;
		ss << "                                         capture " << endl;
		MFCConsole::Output(ss.str());
	}
	//ȡͼ
	int length = 0;//��ͼ����

	//��һ���֣�ȷ����ʼ�в��ȴ����д������
	
	int startFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
	int firstFrame = startFrame;//��¼����ʱ��д�к�
	//������ʱʱ��
	//������ȴ�һ��ʱ�䣬ש�ߵ�����������ٻ�ȡͼ��
	int addFrameCountIn = Capture_WaitTimeMSIn * 1000 / p_e2v->FrameTimeUS;
	frameIndexAdd(startFrame, addFrameCountIn);
	//��һ��forѭ����Ϊ�˴���ѭ��ָ��Խ����������startFrame = 19500 + 600 = 100����ʱGetWriteIndex = 19800 > 100 ��Ҫ�ȴ�GetWriteIndexԽ��
	for (size_t i = 0; i < 10; i++)
		if (p_e2v->P_CycleBuffer->GetWriteIndex() > startFrame)
			Sleep(10);
		else
			break;
	//�ڶ���ѭ�� �ȴ�������㵽��ʼ�У�����д��ԭ���Ǽ�������ʱʱ�䣬���ܴ��������δ���㵽startFrame�������
	int nowFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
	while (
		!(
		(startFrame >= firstFrame && (nowFrame > startFrame || nowFrame < firstFrame)) ||//��һ�������startFrameδ��ҳ���������ֿ��ܣ�1.nowFrameҲû��ҳ��Ҫ��nowFrame>startFrame��2.��nowFrame��ҳ��(��nowFrame<firstFrame)�������ж�Ϊͨ����
		(startFrame < firstFrame && nowFrame > startFrame && nowFrame < firstFrame)//�ڶ��������startFrame��ҳ�ˣ����ж�nowFrame > startFrame����
		)
		)
	{
		Sleep(10);
		nowFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
	}
#ifdef Controller_DEBUG
	if (1 == 1)
	{
		stringstream ss;
		ss << "capture _ startFrame =" << firstFrame << " + " << addFrameCountIn << " = " << startFrame << endl;
		ss << "������ʼ�У�" << nowFrame << endl;
		MFCConsole::Output(ss.str());
	}
#endif
	IsGrabbing2 = false;


	//�ڶ����֣�ȷ�������в��ȴ����д������

	int addFrameCountOut = Capture_WaitTimeMSOut * 1000 / p_e2v->FrameTimeUS;//����½��غ󣬵ȴ���ש�뿪��������֡��
	if (length == 0)//�ɼ��̶�����
		length = Capture_WaitTimeMSOut * 1000 / p_e2v->FrameTimeUS;

	int endFrameAbso = startFrame;
	frameIndexAdd(endFrameAbso, length);//�������һ֡��������һ֡���ܴ������Ƿ����½��ض�ֹͣ�ɼ���whileѭ��break��
	int endFrame = endFrameAbso;
#ifdef Controller_DEBUG
	if (1 == 1)
	{
		stringstream ss;
		ss << "capture _ endFrameAbso = " << startFrame << " + " << length << "=" << endFrameAbso << endl;
		MFCConsole::Output(ss.str());
	}
#endif

	bool overtimeflag = false;//��ʱ���
	//wait capture end
	Sleep(100);
	//ѭ���ȴ��½��ػ��ͼ��ʱ
	//while (pci1761.GetTrailingEdgeIDI(7))
	//while (pci1761.GetRisingEdgeIDI(7))
	while (true)
	{
		int now = p_e2v->P_CycleBuffer->GetWriteIndex();
		//��ʱ���˳�
		if (
			(endFrameAbso >= startFrame && now >= endFrameAbso) ||
			(endFrameAbso < startFrame && now >= endFrameAbso && now < startFrame)
			)
		{
#ifdef Controller_DEBUG
			if (1 == 1)
			{
				stringstream ss;
				ss << "capture _ get endFrameAbso,endFrameAbso =" << endFrameAbso << endl;
				MFCConsole::Output(ss.str());
			}
#endif
			overtimeflag = true;//һֱû�л���½����źţ���ǳ�ʱ
			break;
		}
		Sleep(2);
	}
	if (overtimeflag == false)//����ǻ���½��ؽ��������һ���������ʱ���к�
	{
		//����˴�˵�����ⲿ��GetPhotoOn��0�����������½����źŽ����˲ɼ�
		//�˴����¼���endFrame
		endFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
		frameIndexAdd(endFrame, addFrameCountOut);
#ifdef Controller_DEBUG
		if (1 == 1)
		{
			stringstream ss;
			ss << "capture _ endFrame = ???? + " << addFrameCountOut << " = " << endFrame << endl;
			MFCConsole::Output(ss.str());
		}
#endif


		//TODO::�ж�ÿһ֡�Ƿ��ǽ�������һ��whileѭ�����ж�ÿһ֡�Ƿ�Ϊȫ�ڣ�ȫ��˵���ɼ�Ӧ�ý����ˡ�

		while (true)
		{
			//�ж��Ƿ������ô����
			int now = p_e2v->P_CycleBuffer->GetWriteIndex();
			//������ɶ��˳�
			if (
				endFrame > startFrame && now >= endFrame ||
				(endFrame < startFrame && now >= endFrame && now < startFrame)
				)
			{
				break;
			}
			Sleep(2);
		}
	}

	//�������֣�ȡͼ
	cv::Mat image = p_e2v->P_CycleBuffer->GetImage(startFrame, endFrame).clone();
	
	//�����̱߳���ͼƬ��Ӳ��
	this->ImageGetCallBack(image);

	//���Ĳ��� ���乤��
	workerindex++;
	if (workerindex > 1) workerindex = 0;
	//�������������
	stringstream ss;
	if (workerindex == 0)
	{
		if (worker1->MyStatus == Worker::Free)
		{
			worker1->SN = index++;
			
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
			worker2->SN = index++;
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