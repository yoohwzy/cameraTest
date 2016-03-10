#pragma once


#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h>
#include <Class/IOCard/PCI1761/PCI1761.h>

#include <View/SpotsMainView.h>

#include <opencv2\opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include <Model\Worker.h>
#include <Model\LogImgGenerator.h>

#include <Class\Debug\MFCConsole.h>
#include <Controller\StatisticsController.h>

using namespace std;

class Controller
{
protected:
	SpotsMainView*  spotsMainView = NULL;

public:
	Controller(SpotsMainView* _spotsMainView) :	spotsMainView(_spotsMainView)
	{
		//ATLASSERT(spotsMainView);
		spotsMainView->AddController(this);

		//init();
	}
	~Controller()
	{
		exitFlag = true;
		Sleep(10);

		release();
	}

	void init();

	//�����������ʾ������
	//���ͼ��
	//��Ʒ���ͣ�1A 2B 3C 4Rejcet
	void ShowWorkResult(cv::Mat image,int type)
	{
		ui_lock.lock();
		switch (type)
		{
		case 1:StatisticsController::AddTodayA(); break;
		case 2:StatisticsController::AddTodayB(); break;
		case 3:StatisticsController::AddTodayC(); break;
		case 4:StatisticsController::AddTodayRejected(); break;
		default:break;
		}

		spotsMainView->ShowBigImg(image);
		logImg.AddItem(image,"test");
		spotsMainView->ShowLogImg(logImg.DrawingBoard);
		spotsMainView->UpdateStatistics();
		ui_lock.unlock();
	}

	/*****************�������ģʽ����*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************�������ģʽ���� End*****************/

private:
	void release();
	//HWND handle_mainWindow;

	E2VBuffer *p_e2vbuffer = NULL;
	ImgScanner *p_imgscanner = NULL;

	PCI1761 pci1761;
	LogImgGenerator logImg;

	//ͬʱֻ�����������˹�������ֻ������ͼ������ש��

	//���һ�����ٻ��Ĺ���
	Worker *lastestWorker = NULL;
	//����1
	Worker *worker1 = NULL;
	//����2
	Worker *worker2 = NULL;


	std::mutex ui_lock;

	bool exitFlag = false;
	void triggerWatcher();
	void jobWatcher();
	void workerCallBack();
};