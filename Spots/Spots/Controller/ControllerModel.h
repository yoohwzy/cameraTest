#pragma once
#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraModel.h>//��������⣬����ᱨ��  XX���岻��ȷ
#include <Class/IOCard/PCI1761/PCI1761.h>
#include <Class/Statistics/Statistics.h>

#include <View/SpotsMainView.h>

#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include <Model\Worker.h>
#include <Model\LogImgGenerator.h>
#include <Model\Arm.h>
#include <Model\MainHueScanner.h>

using namespace std;

//#define ControllerModel_DEBUG

//��������ģ��
class ControllerModel
{
public:
	Block block = Block(0, 0);
	ControllerModel(SpotsMainView* _spotsMainView) : spotsMainView(_spotsMainView)
	{
		spotsMainView->AddController(this);
	}
	~ControllerModel()
	{
		Release();
		spotsMainView = NULL;
	}

	//�����������ʾ������
	//���ͼ��
	//��Ʒ���ͣ�0 BLδ�ҵ�ש 1A 2B 3C 4Rejcet
	void ImgProcessOverCallBack(int SN, cv::Mat image, Block b, int type)
	{
		ui_lock.lock();
		block = b;
		spotsMainView->ShowBigImg(image);
		if (type != 0)
		{
			if (type != 4)
			{
				//�ж�ɫ���Ƿ���ȷ
				vector<int>::iterator it;
				for (it = MainHueErrorSNs.begin(); it != MainHueErrorSNs.end(); it++)
				{
					if (*it == SN)//�����SN��MainHueErrorSNs�м�¼��˵��ɫ���д�
					{
						type = 4;
						MFCConsole::Output("��ɫ����������ϸ�\r\n");
						MainHueErrorSNs.erase(it);
					}
					else if (*it + 10 < SN)//����SNԶ���ڼ�¼�е���ֵ���������һ��¼
						MainHueErrorSNs.erase(it);
				}
			}
			//�ٸ���
			switch (type)
			{
			case 1:Statistics::AddTodayA();
				logImg.AddItem(image, "A");
				break;
			case 2:Statistics::AddTodayB();
				logImg.AddItem(image, "B");
				break;
			case 3:Statistics::AddTodayC();
				logImg.AddItem(image, "C");
				break;
			case 4:
				Statistics::AddTodayRejected();
				logImg.AddItem(image, "D");
				arm.AddAction(0, 500);//0��output����500ms�����һ����Ծ�ź�
				break;
			default:break;
			}

			spotsMainView->ShowLogImg(logImg.DrawingBoard);
			spotsMainView->UpdateStatistics();
			stringstream ss;
			ss << "�ߴ�Ϊ��\n�Խ���1=" << b.ac_mm << "\n�Խ���2=" << b.bd_mm << "\n��=" << abs(b.ac_mm - b.bd_mm);
			spotsMainView->UpdateSizeInfo(ss.str());
		}
		ui_lock.unlock(); 
	}
	//��ͼ��ɺ�ص������ڽ�ͼƬ������Ӳ��
	void ImageGetCallBack(cv::Mat img)
	{
		if (SAVE_IMG)//�Ƿ�Ҫ����ͼƬ
		{
			//�첽����
			std::thread t_tiggerThread(std::mem_fn(&ControllerModel::imageSave), this, img);
			t_tiggerThread.detach();
			////ͬ������
			//imageSave(img);
		}
		else
			MFCConsole::Output("��ǰΪ������ͼƬģʽ\r\n");
	};
	//��ɫ�������Ļص�����
	virtual void MainHueErrorCallBack(int SN, cv::Mat img){
		ui_lock.lock();
		bool flag = true;
		for (int i = 0; i < MainHueErrorSNs.size(); i++)
			if (MainHueErrorSNs[i] == SN)
			{
				flag = false;
				break;
			}
		MainHueErrorSNs.push_back(SN);
		spotsMainView->ShowBigImg(img);
		ui_lock.unlock();
	};


	// �������е�����ϵͳ�����󣬵��ñ������������������㷨�С�
	void ResetParameter();
	/*****************�������ģʽ����*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();
	/*****************�������ģʽ���� End*****************/


protected:
	//ͨ�õĳ�ʼ������
	//��ʼ��PCI1761�����ݿ⡢���ݲ�����ϵͳ�л�����ʵ/�������ģʽ
	//��ʵģʽ�µ���TiggerStartWatch�����������߳�
	void baseInit();
	void imageSave(cv::Mat img);


	/**********************************/
	/*                                */
	/*           ���������           */
	/*                                */
	/**********************************/
public:
	MainHueScanner *P_mainHueScanner = NULL;
	//�Ƿ�Ϊ�������ģʽ��������ͨ���ⲿ���ã�������exeʱ��������virtual�����IsRealModel=0��
	bool IsRealModel = 1;
	bool ExitFlag = false;//triggerWatcher ������־
	bool PauseFlag = false;//triggerWatcher ��ͣ��־
protected:
	SpotsMainView *spotsMainView = NULL;
	PCI1761 pci1761;
	LogImgGenerator logImg;
	Arm arm;
	//ͬʱֻ�����������˹�������ֻ������ͼ������ש��
	//����1
	Worker *worker1 = NULL;
	//����2
	Worker *worker2 = NULL;
	int workerindex = 0;//�����ж�ʹ����һ�����˽��д���

	std::mutex ui_lock;
	std::mutex watcher_lock;
	std::mutex image_write_lock;

	bool e2vInited = true;
	bool pci1761Inited = true;
private:



	/**********************************/
	/*                                */
	/*           �麯���ӿ�           */
	/*                                */
	/**********************************/
public:
	//��ʼ��
	virtual void Init() = 0;
	//����
	virtual void Release() 
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
		if (P_mainHueScanner != NULL)
		{
			delete P_mainHueScanner;
			P_mainHueScanner = NULL;
		}
	};
	//��ʼ��ش������߳�
	virtual void TiggerStartWatch() = 0;
	//ֹͣ��ش������߳�
	virtual void TiggerStopWatch() = 0;
	//���Դ洢ɫ�������SN
	vector<int> MainHueErrorSNs;
protected:
	virtual void triggerWatcherThread() = 0;//�����������߳�
	virtual void captureAndProcessThread(int sn=0) = 0;//��ͼ�빤��ί��






public:
	/**********************************/
	/*                                */
	/*            ������            */
	/*                                */
	/**********************************/
	bool SAVE_IMG = false;//�Ƿ񱣴�ͼƬ��Ӳ��

	int Capture_WaitTimeMSIn = 100;//�ȴ���ש������������ʱ��
	int Capture_WaitTimeMSOut = 250;//�ȴ���ש�뿪��������ʱ��
	int Capture_FrameTimeOut = 2000;//���㳬ʱʱ��



	/**************�ڲ�����****************/
	int Pretreatment_Enable = 1;


	/**********************************/
	/*                                */
	/*            �������            */
	/*                                */
	/**********************************/




	/**********************************/
	/*                                */
	/*            �ּ�����            */
	/*                                */
	/**********************************/
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


};