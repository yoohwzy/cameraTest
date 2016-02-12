#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "Controller.h"


void Controller::init(){

	release();

	//����ͼ�񻺴�
	p_e2vbuffer = new E2VBuffer(4096, true);
	p_imgscanner = new ImgScanner(p_e2vbuffer);
	bool e2vInitFlag = true;
	//��ʼ��e2v_EV71YC1CCL4005BA0ʧ��
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
		//��ʼ������
		worker1 = new Worker(p_e2vbuffer);
		worker2 = new Worker(p_e2vbuffer);

		worker1->P_Controller = this;
		worker2->P_Controller = this;
		//��ʼ��ش���
		std::thread t_tiggerThread(std::mem_fn(&Controller::triggerWatcher), this);
		t_tiggerThread.detach();

		spotsMainView->SwitchModel2Virtual(false);

		MFCConsole::Output("���л�����ʵ���ģʽ��\r\n");
	}
	else
	{
		if (!e2vInitFlag && !pci1761InitFlag)
			AfxMessageBox(L"�������&pci1761��ʼ��ʧ�ܣ�");
		else if (!pci1761InitFlag)
			AfxMessageBox(L"pci1761��ʼ��ʧ�ܣ�");
		else if (!e2vInitFlag)
			AfxMessageBox(L"���������ʼ��ʧ�ܣ�");


		//�����������
		cv::Mat virtualImg;
		worker1 = new Worker(NULL);
		worker1->image = virtualImg;
		worker1->P_Controller = this;

		spotsMainView->SwitchModel2Virtual(true);

		MFCConsole::Output("���л����������ģʽ��\r\n");
	}

	// ͳ�����ݳ�ʼ��
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
		if (pci1761.GetRisingEdgeIDI(7))//�����ؿ�ʼ��ͼ
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
		else if (pci1761.GetTrailingEdgeIDI(7))//�½��ؽ�����ͼ
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



/*********************  ����ģʽ����  ***********************/

void Controller::VirtualSelectImg(cv::Mat image)
{
	if (image.cols == 0)
		MessageBox(0, L"ͼƬ��ȡʧ�ܣ�", L"����", 0);
	else
	{
		worker1->image = image;
	}
}
void Controller::VirtualWorkerStart()
{
	if (worker1->image.cols == 0)
	{
		MessageBox(0, L"���ȼ��������Ƭ��", L"����", 0);
		return;
	}
	if (worker1->MyStatus == Worker::Done || worker1->MyStatus == Worker::Free)
	{
		worker1->StartWork();
	}
	else
	{
		MessageBox(0, L"��һ�ִ�����δ������", L"����", 0);
	}
}
/*********************����ģʽ��������***********************/
