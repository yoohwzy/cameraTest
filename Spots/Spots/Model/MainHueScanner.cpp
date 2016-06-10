#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include <Controller/ControllerModel.h>
#include "MainHueScanner.h"
#include <Class/Helper/StringHelper.h>
#include <Class/Debug/MFCConsole.h>

int MainHueScanner::Standard_H = 0;
int MainHueScanner::Standard_S = 0;
int MainHueScanner::Standard_V = 0;

bool MainHueScanner::Enabled = false;
bool MainHueScanner::SAVE_IMG = false;
int MainHueScanner::WaitTimeMSIn = 100;//等待瓷砖进入拍摄区的时间
int MainHueScanner::WaitTimeMSOut = 100;

//主色调分析类
//用于判断釉下文字缺陷，面阵相机拍照后，连续计算主色调。
//实例化后直接开启面阵相机采图线程
MainHueScanner::MainHueScanner(ControllerModel *pController)
{
	if (Enabled)
	{
		mvcam.ColorType = CV_8UC3;
		mvcam.ExposureTimeMS = 50;
		mvcam.AnalogGain = 2;
		mvcam.Init();
		endFlag = true;
		if (mvcam.HasInited)
		{
			HasInited = 1;

			p_Controller = pController;
			mvcam.StartCapture();
			sn = 0;
			std::thread t_run(std::mem_fn(&MainHueScanner::scanImg), this);
			auto tn = t_run.native_handle();
			SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
			t_run.detach();
		}
		else
		{
			HasInited = 0;
		}
	}
}


MainHueScanner::~MainHueScanner()
{
	this->Stop();
	while (!endFlag)
	{
		//Sleep(50);
		std::this_thread::sleep_for(chrono::milliseconds(50));
	}
	p_Controller = NULL;
}

void MainHueScanner::scanImg()
{
	int lastsn = 0;
	endFlag = 0;
	while (!stopFlag && Enabled)
	{
		if (sn == 0)//暂停
		{
			if (lastsn != 0)
			{
				cv::destroyWindow("MainHue");
				lastsn = 0;
			}
			Sleep(10);
		}
		else
		{
			if (lastsn == 0)
			{
				if (MainHueScanner::WaitTimeMSIn > 0)Sleep(MainHueScanner::WaitTimeMSIn);
				lastsn = sn;
			}
			cv::Mat img = mvcam.Grub();
			cv::namedWindow("MainHue");
			cv::imshow("MainHue", img);
			cv::waitKey(5);
			if (analysis(img) != 0)
			{
				//sn = 0;
			}
			Sleep(30);
		}
	}
	endFlag = 1;
	cv::destroyWindow("MainHue");
	if (!Enabled)
		mvcam.Release();
}
//分析是否有缺陷，无缺陷返回0
int MainHueScanner::analysis(cv::Mat img)
{
	Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV);
	/// 对hue通道使用180个bin,对saturatoin通道使用256个bin
	int h_bins = 180; int s_bins = 256; int v_bins = 256;
	int histSize_H[] = { h_bins };
	int histSize_S[] = { s_bins };
	int histSize_V[] = { v_bins };

	// hue的取值范围从0到180, saturation取值范围从0到255
	float h_ranges[] = { 0, 180 };
	const float* Hranges[] = { h_ranges };
	float s_ranges[] = { 0, 255 };
	const float* Sranges[] = { h_ranges };
	float v_ranges[] = { 0, 255 };
	const float* Vranges[] = { h_ranges };

	// 使用通道
	int channels_H = 0;
	int channels_S = 1;
	int channels_V = 2;

	//检测
	cv::Mat imgThresholded;
	//检查数组元素是否在两个数量之间 
	//原数组
	//lowerb 包括进的下边界
	//upperb 不包括进的上边界
	//dst 输出数组必须是 8u 或 8s 类型. 
	//inRange(img_hsv, cv::Scalar(0, 0, 50), cv::Scalar(40, 65, 140), imgThresholded); //Threshold the image  
	inRange(img_hsv, cv::Scalar(0, 0, 50), cv::Scalar(Standard_H + 20, Standard_S + 20, Standard_V + 20), imgThresholded); //Threshold the image  
	cv::Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));
	//去除噪点
	morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
	//连接连通域 
	morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
	if (countNonZero(imgThresholded) < 0.9*imgThresholded.cols*imgThresholded.rows)
	{
		p_Controller->MainHueErrorCallBack(sn, img);
		return 1;//有标记
	}
	else
		return 0;//无标记
}
//定标，计算出标准HSV并存储
void MainHueScanner::DingBiao(cv::Mat img)
{
	Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV);
	/// 对hue通道使用180个bin,对saturatoin通道使用256个bin
	int h_bins = 180; int s_bins = 256; int v_bins = 256;
	int histSize_H[] = { h_bins };
	int histSize_S[] = { s_bins };
	int histSize_V[] = { v_bins };

	// hue的取值范围从0到180, saturation取值范围从0到255
	float h_ranges[] = { 0, 180 };
	const float* Hranges[] = { h_ranges };
	float s_ranges[] = { 0, 255 };
	const float* Sranges[] = { h_ranges };
	float v_ranges[] = { 0, 255 };
	const float* Vranges[] = { h_ranges };

	// 使用通道
	int channels_H = 0;
	int channels_S = 1;
	int channels_V = 2;



	//定标
	vector<int>standard_value(3);//存储hsv标准值
	standard_value[0] = 20;
	standard_value[1] = 45;
	standard_value[2] = 126;

	/// 直方图
	MatND hist_H, hist_S, hist_V;
	// 分别计算HSV图像的直方图
	calcHist(&img_hsv, 1, &channels_H, cv::Mat(), hist_H, 1, histSize_H, Hranges, true, false);
	calcHist(&img_hsv, 1, &channels_S, cv::Mat(), hist_S, 1, histSize_S, Sranges, true, false);
	calcHist(&img_hsv, 1, &channels_V, cv::Mat(), hist_V, 1, histSize_V, Vranges, true, false);

	cv::Point maxlocation;
	minMaxLoc(hist_H, NULL, NULL, 0, &maxlocation);
	standard_value[0] = maxlocation.y;
	Standard_H = maxlocation.y;

	minMaxLoc(hist_S, NULL, NULL, 0, &maxlocation);
	standard_value[1] = maxlocation.y;
	Standard_S = maxlocation.y;

	minMaxLoc(hist_V, NULL, NULL, 0, &maxlocation);
	standard_value[2] = maxlocation.y;
	Standard_V = maxlocation.y;
}
void MainHueScanner::SetStandardHSV(string str)
{
	vector<string> strs;
	string delim = ";";
	StringHelper::split(str, delim, &strs);
	if (strs.size() > 0)
	{
		vector<string> hsv;
		delim = ",";
		StringHelper::split(strs[0], delim, &hsv);
		if (hsv.size() != 3)
			return;
		else
		{
			MainHueScanner::Standard_H = StringHelper::string2int(hsv[0]);
			MainHueScanner::Standard_S = StringHelper::string2int(hsv[1]);
			MainHueScanner::Standard_V = StringHelper::string2int(hsv[2]);
		}
	}
};