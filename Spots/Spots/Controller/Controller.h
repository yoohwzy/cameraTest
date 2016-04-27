#pragma once


#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h>
#include <Class/IOCard/PCI1761/PCI1761.h>
#include <Class/Statistics/Statistics.h>

#include <View/SpotsMainView.h>

#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include <Model\Worker.h>
#include <Model\LogImgGenerator.h>


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
	bool IsRealModel = 1;

	void init();

	void StartWatch();
	void StopWatch();


	//�����������ʾ������
	//���ͼ��
	//��Ʒ���ͣ�1A 2B 3C 4Rejcet
	void ShowWorkResult(cv::Mat image,int type)
	{
		ui_lock.lock();
		switch (type)
		{
		case 1:Statistics::AddTodayA(); break;
		case 2:Statistics::AddTodayB(); break;
		case 3:Statistics::AddTodayC(); break;
		case 4:Statistics::AddTodayRejected(); break;
		default:break;
		}

		spotsMainView->ShowBigImg(image);
		logImg.AddItem(image,"test");
		spotsMainView->ShowLogImg(logImg.DrawingBoard);
		spotsMainView->UpdateStatistics();
		ui_lock.unlock();
	}
	void ImageGetCallBack(cv::Mat);//��ͼ��ɺ�ص������ڽ�ͼƬ������Ӳ��

	// �������е�����ϵͳ�����󣬵��ñ��ӿڽ������������㷨�С�
	void ResetParameter();
	void LoadParameterFromIni();

	/*****************�������ģʽ����*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************�������ģʽ���� End*****************/



	/**********************************/
	/*                                */
	/*                ����            */
	/*                                */
	/**********************************/

	int Real_WidthMM = 600;//ͼ���д�ש�ĺ᳤
	int Real_LengthMM = 300;//ͼ���д�ש���ݳ�
	int Worker_WaitTimeMSIn = 100;
	int Worker_WaitTimeMSOut = 250;
	int Worker_FrameTimeOut = 2000;


	/******************��Եȱ�ݲ���***************/
	double BlockLocalizer_THRESHOD = 10;			//��Ե���Ҷ�ֵ����ֵ
	double BlockLocalizer_ContinuePointCount = 30;	//�������ٸ������ж�Ϊ��Ե

	int  BlockEdgeDetector_Enable = 1;
	double BlockEdgeDetector_DIFF_THRESHOLD = 0.4;	//��ͼ֮��������ֵ����Ϊ������ȱ��
	double BlockEdgeDetector_FAULTS_SPAN = 4;		//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	double BlockEdgeDetector_FAULTS_COUNT = 5;		//�������������DIFF_THRESHOLD���ж�Ϊȱ��

	int BlockEdgeLineDetector_Enable = 1;			//
	int BlockEdgeLineDetector_BINARY_THRESHOD = 5;	//��ֵ����ֵ
	int BlockEdgeLineDetector_LENGTH_THRESHOD = 5;	//����������Ȳ���Ϊ��ȱ�ݣ�pix��
	int BlockEdgeLineDetector_DEEP_THRESHOD = 5;	//���������Ȳ���Ϊ��ȱ�ݣ�pix��

	bool SAVE_IMG = false;


	/**************�ּ�����*************/

	//�߲���
	double Classify_EDGE_SINGLE_LENGTH_A = -1;
	double Classify_EDGE_SINGLE_LENGTH_B = -1;
	double Classify_EDGE_SINGLE_LENGTH_C = -1;
	double Classify_EDGE_SINGLE_LENGTH_ACCEPT = -1;

	double Classify_EDGE_TOTAL_LENGTH_A = -1;
	double Classify_EDGE_TOTAL_LENGTH_B = -1;
	double Classify_EDGE_TOTAL_LENGTH_C = -1;
	double Classify_EDGE_TOTAL_LENGTH_ACCEPT = -1;

	double Classify_EDGE_SINGLE_DEEP_A = -1;
	double Classify_EDGE_SINGLE_DEEP_B = -1;
	double Classify_EDGE_SINGLE_DEEP_C = -1;
	double Classify_EDGE_SINGLE_DEEP_ACCEPT = -1;

	double Classify_EDGE_TOTAL_DEEP_A = -1;
	double Classify_EDGE_TOTAL_DEEP_B = -1;
	double Classify_EDGE_TOTAL_DEEP_C = -1;
	double Classify_EDGE_TOTAL_DEEP_ACCEPT = -1;

	//���Ӳ���
	double Classify_HOAL_DIAMETER_A = -1;
	double Classify_HOAL_DIAMETER_B = -1;
	double Classify_HOAL_DIAMETER_C = -1;
	double Classify_HOAL_DIAMETER_ACCEPT = -1;

	double Classify_HOAL_COUNT_A = -1;
	double Classify_HOAL_COUNT_B = -1;
	double Classify_HOAL_COUNT_C = -1;
	double Classify_HOAL_COUNT_ACCEPT = -1;


	//����
	double Classify_SCRATCH_SINGLE_LENGTH_A = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_B = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_C = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = -1;

	double Classify_SCRATCH_TOTAL_LENGTH_A = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_B = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_C = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = -1;


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
	std::mutex watcher_lock;
	std::mutex image_write_lock;

	bool exitFlag = false;//triggerWatcher ������־
	void triggerWatcher();
	//void jobWatcher();	
	//void workerCallBack();	//�㷨������ص�����ʾ������
	void imageSave(cv::Mat img);
};