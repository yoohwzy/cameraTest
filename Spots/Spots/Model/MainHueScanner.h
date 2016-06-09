#pragma once
#include <Controller/ControllerModel.h>
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
	void Run(int SN){ sn = SN; };
	void Pause(){ sn = 0; };
	void Stop(){ stopFlag = true; };
	bool HasInited = false;
	//定标，计算出标准HSV并存储
	static void DingBiao(cv::Mat);
	//设置标准值参数，格式如string = "H1,S1,V1;H2,S2,V2;"表示有两组
	static void SetStandardHSV(string str);
	static int Standard_H;
	static int Standard_S;
	static int Standard_V;
private:
	ControllerModel *p_Controller = NULL;
	int sn = 0;//图像序列号，为0时暂停
	bool stopFlag = false;
	MVCAM mvcam;
	void scanImg();
	//分析是否有缺陷
	int analysis(cv::Mat);
};

