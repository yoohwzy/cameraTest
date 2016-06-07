#include "MainHueScanner.h"
#include <Class/Debug/MFCConsole.h>

//��ɫ��������
//�����ж���������ȱ�ݣ�����������պ�����������ɫ����
//ʵ������ֱ�ӿ������������ͼ�߳�
MainHueScanner::MainHueScanner(ControllerModel *pController)
{
	p_Controller = pController;
	mvcam.ColorType = CV_8U;
	mvcam.ExposureTimeMS = 50;
	mvcam.AnalogGain = 2;
	mvcam.Init();
	mvcam.StartCapture();

	sn = 0;
	std::thread t_run(std::mem_fn(&MainHueScanner::scanImg), this);
	auto tn = t_run.native_handle();
	SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
	t_run.detach();
}


MainHueScanner::~MainHueScanner()
{
	stopFlag = true;
	Sleep(50);
	p_Controller = NULL;
}

void MainHueScanner::scanImg()
{
	while (!stopFlag)
	{
		if (sn == 0)//��ͣ
			Sleep(10); 
		else
		{
			cv::Mat img = mvcam.Grub();
			analysis(img);
			Sleep(20);
		}
	}
}
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

	/// ֱ��ͼ
	MatND hist_H, hist_S, hist_V;
	// �ֱ����HSVͼ���ֱ��ͼ
	calcHist(&img_hsv, 1, &channels_H, Mat(), hist_H, 1, histSize_H, Hranges, true, false);
	calcHist(&img_hsv, 1, &channels_S, Mat(), hist_S, 1, histSize_S, Sranges, true, false);
	calcHist(&img_hsv, 1, &channels_V, Mat(), hist_V, 1, histSize_V, Vranges, true, false);


	//����
	vector<int>standard_value(3);//�洢hsv��׼ֵ
	standard_value[0] = 20;
	standard_value[1] = 45;
	standard_value[2] = 126;
	if (false)
	{
		Point maxlocation;
		minMaxLoc(hist_H, NULL, NULL, 0, &maxlocation);
		standard_value[0] = maxlocation.y;
		minMaxLoc(hist_S, NULL, NULL, 0, &maxlocation);
		standard_value[1] = maxlocation.y;
		minMaxLoc(hist_V, NULL, NULL, 0, &maxlocation);
		standard_value[2] = maxlocation.y;
	}
	//���
	else
	{
		Mat imgThresholded;
		inRange(img_hsv, Scalar(0, 0, 50), Scalar(40, 65, 140), imgThresholded); //Threshold the image  
		Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
		//ȥ�����
		morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
		//������ͨ�� 
		morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
		if (countNonZero(imgThresholded) < 0.9*imgThresholded.cols*imgThresholded.rows)
		{
			p_Controller->MainHueErrorCallBack(sn, img);
			return true;//�б��
		}
		else
			return false;//�ޱ��
	}

	return 0;
}