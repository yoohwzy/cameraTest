#pragma once
#include <thread>

//引入算法综合器
class ControllerModel;// 解决类互相包含


#include <Class\Camera\MVCAM\MVCAM.h>
//主色调分析类
//用于判断釉下文字缺陷，面阵相机拍照后，连续计算主色调。
//实例化后直接开启面阵相机采图线程
class MainHueScanner
{
public:
	MainHueScanner(ControllerModel *pController);
	~MainHueScanner();
	int GetSN(){ return sn; };
	void Run(int SN){ sn = SN; };
	void Pause()
	{
		thread t = thread(std::mem_fn(&MainHueScanner::waitPause), this);
		t.detach();
	};
	void Stop(){ this->sn = 0; stopFlag = true; };
	bool HasInited = false;
	//定标，计算出标准HSV并存储
	static void DingBiao(cv::Mat);
	//设置标准值参数，格式如string = "H1,S1,V1;H2,S2,V2;"表示有两组
	static void SetStandardHSV(string str);
	static int Standard_H;
	static int Standard_S;
	static int Standard_V;


	static bool Enabled;
	static bool SAVE_IMG;
	static int WaitTimeMSIn;//等待瓷砖进入拍摄区的时间
	static int WaitTimeMSOut;//等待瓷砖离开拍摄区的时间
private:
	void waitPause()
	{
		if (MainHueScanner::WaitTimeMSOut > 0)
			Sleep(MainHueScanner::WaitTimeMSOut);
		this->sn = 0;
	}
	ControllerModel *p_Controller = NULL;
	int sn = 0;//图像序列号，为0时暂停
	bool stopFlag = false;
	bool endFlag = true;//进程结束标志
	MVCAM mvcam;
	void scanImg();
	//分析是否有缺陷，无缺陷返回0
	int analysis(cv::Mat);

	static inline bool SortBysize(vector<cv::Point>const &v1, vector<cv::Point>const &v2)	
	{
		return v1.size()>v2.size();
		//降序排列 
	};
	//输入图片 与图片编号
	void imageSave(cv::Mat img, int _sn);
};

