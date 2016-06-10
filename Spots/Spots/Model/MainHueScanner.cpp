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
int MainHueScanner::WaitTimeMSIn = 100;//�ȴ���ש������������ʱ��
int MainHueScanner::WaitTimeMSOut = 100;

//��ɫ��������
//�����ж���������ȱ�ݣ�����������պ�����������ɫ����
//ʵ������ֱ�ӿ������������ͼ�߳�
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
		if (sn == 0)//��ͣ
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
//�����Ƿ���ȱ�ݣ���ȱ�ݷ���0
int MainHueScanner::analysis(cv::Mat img)
{
	Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV);
	/// ��hueͨ��ʹ��180��bin,��saturatoinͨ��ʹ��256��bin
	int h_bins = 180; int s_bins = 256; int v_bins = 256;
	int histSize_H[] = { h_bins };
	int histSize_S[] = { s_bins };
	int histSize_V[] = { v_bins };

	// hue��ȡֵ��Χ��0��180, saturationȡֵ��Χ��0��255
	float h_ranges[] = { 0, 180 };
	const float* Hranges[] = { h_ranges };
	float s_ranges[] = { 0, 255 };
	const float* Sranges[] = { h_ranges };
	float v_ranges[] = { 0, 255 };
	const float* Vranges[] = { h_ranges };

	// ʹ��ͨ��
	int channels_H = 0;
	int channels_S = 1;
	int channels_V = 2;

	//���
	cv::Mat imgThresholded;
	//�������Ԫ���Ƿ�����������֮�� 
	//ԭ����
	//lowerb ���������±߽�
	//upperb �����������ϱ߽�
	//dst ������������ 8u �� 8s ����. 
	//inRange(img_hsv, cv::Scalar(0, 0, 50), cv::Scalar(40, 65, 140), imgThresholded); //Threshold the image  
	inRange(img_hsv, cv::Scalar(0, 0, 50), cv::Scalar(Standard_H + 20, Standard_S + 20, Standard_V + 20), imgThresholded); //Threshold the image  
	cv::Mat element = getStructuringElement(MORPH_RECT, cv::Size(5, 5));
	//ȥ�����
	morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
	//������ͨ�� 
	morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
	if (countNonZero(imgThresholded) < 0.9*imgThresholded.cols*imgThresholded.rows)
	{
		p_Controller->MainHueErrorCallBack(sn, img);
		return 1;//�б��
	}
	else
		return 0;//�ޱ��
}
//���꣬�������׼HSV���洢
void MainHueScanner::DingBiao(cv::Mat img)
{
	Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV);
	/// ��hueͨ��ʹ��180��bin,��saturatoinͨ��ʹ��256��bin
	int h_bins = 180; int s_bins = 256; int v_bins = 256;
	int histSize_H[] = { h_bins };
	int histSize_S[] = { s_bins };
	int histSize_V[] = { v_bins };

	// hue��ȡֵ��Χ��0��180, saturationȡֵ��Χ��0��255
	float h_ranges[] = { 0, 180 };
	const float* Hranges[] = { h_ranges };
	float s_ranges[] = { 0, 255 };
	const float* Sranges[] = { h_ranges };
	float v_ranges[] = { 0, 255 };
	const float* Vranges[] = { h_ranges };

	// ʹ��ͨ��
	int channels_H = 0;
	int channels_S = 1;
	int channels_V = 2;



	//����
	vector<int>standard_value(3);//�洢hsv��׼ֵ
	standard_value[0] = 20;
	standard_value[1] = 45;
	standard_value[2] = 126;

	/// ֱ��ͼ
	MatND hist_H, hist_S, hist_V;
	// �ֱ����HSVͼ���ֱ��ͼ
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