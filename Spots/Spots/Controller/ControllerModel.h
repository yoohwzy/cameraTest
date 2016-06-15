#pragma once
#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCameraModel.h>//必须放在这，否则会报错  XX定义不明确
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

//控制器的模型
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

	//处理结束后显示处理结果
	//结果图像
	//产品类型，0 BL未找到砖 1A 2B 3C 4Rejcet
	void ImgProcessOverCallBack(int SN, cv::Mat image, Block b, int type)
	{
		ui_lock.lock();
		block = b;
		spotsMainView->ShowBigImg(image);
		if (type != 0)
		{
			if (type != 4)
			{
				//判断色调是否正确
				vector<int>::iterator it;
				for (it = MainHueErrorSNs.begin(); it != MainHueErrorSNs.end(); it++)
				{
					if (*it == SN)//如果该SN在MainHueErrorSNs有记录，说明色调有错
					{
						type = 4;
						MFCConsole::Output("因色调错误而不合格\r\n");
						MainHueErrorSNs.erase(it);
					}
					else if (*it + 10 < SN)//若该SN远大于记录中的数值，则擦除这一记录
						MainHueErrorSNs.erase(it);
				}
			}
			//再根据
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
				arm.AddAction(0, 500);//0号output口在500ms后产生一个阶跃信号
				break;
			default:break;
			}

			spotsMainView->ShowLogImg(logImg.DrawingBoard);
			spotsMainView->UpdateStatistics();
			stringstream ss;
			ss << "尺寸为：\n对角线1=" << b.ac_mm << "\n对角线2=" << b.bd_mm << "\n差=" << abs(b.ac_mm - b.bd_mm);
			spotsMainView->UpdateSizeInfo(ss.str());
		}
		ui_lock.unlock(); 
	}
	//采图完成后回调，用于将图片保存至硬盘
	void ImageGetCallBack(cv::Mat img)
	{
		if (SAVE_IMG)//是否要保存图片
		{
			//异步保存
			std::thread t_tiggerThread(std::mem_fn(&ControllerModel::imageSave), this, img);
			t_tiggerThread.detach();
			////同步保存
			//imageSave(img);
		}
		else
			MFCConsole::Output("当前为不保存图片模式\r\n");
	};
	//主色调错误后的回调函数
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


	// 在设置中调整了系统参数后，调用本方法将参数更新至算法中。
	void ResetParameter();
	/*****************虚拟相机模式方法*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();
	/*****************虚拟相机模式方法 End*****************/


protected:
	//通用的初始化代码
	//初始化PCI1761、数据库、根据参数将系统切换到真实/虚拟相机模式
	//真实模式下调用TiggerStartWatch，开启监视线程
	void baseInit();
	void imageSave(cv::Mat img);


	/**********************************/
	/*                                */
	/*           属性与对象           */
	/*                                */
	/**********************************/
public:
	MainHueScanner *P_mainHueScanner = NULL;
	//是否为虚拟相机模式，本属性通过外部配置，在启动exe时若附加了virtual命令，则IsRealModel=0；
	bool IsRealModel = 1;
	bool ExitFlag = false;//triggerWatcher 结束标志
	bool PauseFlag = false;//triggerWatcher 暂停标志
protected:
	SpotsMainView *spotsMainView = NULL;
	PCI1761 pci1761;
	LogImgGenerator logImg;
	Arm arm;
	//同时只允许两个工人工作，即只能两张图（两块砖）
	//工人1
	Worker *worker1 = NULL;
	//工人2
	Worker *worker2 = NULL;
	int workerindex = 0;//用于判断使用哪一个工人进行处理

	std::mutex ui_lock;
	std::mutex watcher_lock;
	std::mutex image_write_lock;

	bool e2vInited = true;
	bool pci1761Inited = true;
private:



	/**********************************/
	/*                                */
	/*           虚函数接口           */
	/*                                */
	/**********************************/
public:
	//初始化
	virtual void Init() = 0;
	//析构
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
	//开始监控触发器线程
	virtual void TiggerStartWatch() = 0;
	//停止监控触发器线程
	virtual void TiggerStopWatch() = 0;
	//用以存储色调错误的SN
	vector<int> MainHueErrorSNs;
protected:
	virtual void triggerWatcherThread() = 0;//触发器监视线程
	virtual void captureAndProcessThread(int sn=0) = 0;//采图与工作委托






public:
	/**********************************/
	/*                                */
	/*            检测参数            */
	/*                                */
	/**********************************/
	bool SAVE_IMG = false;//是否保存图片到硬盘

	int Capture_WaitTimeMSIn = 100;//等待瓷砖进入拍摄区的时间
	int Capture_WaitTimeMSOut = 250;//等待瓷砖离开拍摄区的时间
	int Capture_FrameTimeOut = 2000;//拍摄超时时间



	/**************内部参数****************/
	int Pretreatment_Enable = 1;


	/**********************************/
	/*                                */
	/*            面阵参数            */
	/*                                */
	/**********************************/




	/**********************************/
	/*                                */
	/*            分级参数            */
	/*                                */
	/**********************************/
	//边参数
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

	//凹坑参数
	double Classify_HOAL_DIAMETER_A = -1;
	double Classify_HOAL_DIAMETER_B = -1;
	double Classify_HOAL_DIAMETER_C = -1;
	double Classify_HOAL_DIAMETER_ACCEPT = -1;

	double Classify_HOAL_COUNT_A = -1;
	double Classify_HOAL_COUNT_B = -1;
	double Classify_HOAL_COUNT_C = -1;
	double Classify_HOAL_COUNT_ACCEPT = -1;


	//划痕
	double Classify_SCRATCH_SINGLE_LENGTH_A = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_B = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_C = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = -1;

	double Classify_SCRATCH_TOTAL_LENGTH_A = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_B = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_C = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = -1;


};