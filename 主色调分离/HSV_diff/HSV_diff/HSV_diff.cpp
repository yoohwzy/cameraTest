// HSV_diff.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <opencv2\opencv.hpp>
#include <thread>
using namespace std;
using namespace cv;

#include "MVCAM\MVCAM.h"
static inline bool SortBysize(vector<cv::Point>const &v1, vector<cv::Point>const &v2)
{
	return v1.size()>v2.size();
	//�������� 
};

//�����Ƿ���ȱ�ݣ���ȱ�ݷ���0
int analysis(cv::Mat img)
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
	Mat imgThresholded, imgThresholdback;
	inRange(img_hsv, Scalar(0, 0, 50), Scalar(40, 65, 140), imgThresholded); //Threshold the image

	inRange(img_hsv, Scalar(0, 0, 0), Scalar(180, 255, 40), imgThresholdback); //��������
	dilate(imgThresholdback, imgThresholdback, Mat());
	vector<vector<cv::Point>>backcontours;
	findContours(imgThresholdback, backcontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	bool backflag = false;//�������
	if (backcontours.size() >= 1)
	{
		sort(backcontours.begin(), backcontours.end(), SortBysize);
		Rect backrect = boundingRect(backcontours[0]);
		if (backrect.width > imgThresholdback.cols - 5)
		{
			backflag = true;
		}
	}

	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	//ȥ�����
	morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
	//������ͨ�� 
	morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
	if (countNonZero(imgThresholded) < 0.9*imgThresholded.cols*imgThresholded.rows&& !backflag)
	{
		return 1;//�б��
	}
	else
		return 0;//�ޱ��
}


int _tmain(int argc, _TCHAR* argv[])
{
	MVCAM mvcam;
	mvcam.ColorType = CV_8UC3;
	mvcam.ExposureTimeMS = 50;
	mvcam.AnalogGain = 2;
	mvcam.Init();
	mvcam.StartCapture();

	unsigned long long i = 0;
	while (true)
	{
		i++;
		cv::Mat img = mvcam.Grub();
		cv::namedWindow("MainHue");
		cv::imshow("MainHue", img);
		cv::waitKey(5);
		if (analysis(img) != 0)
		{
			cout << i << "  0" << endl;
		}
		else
		{
			cout << i << "  1" << endl;
		}
		Sleep(30);
	}
	return 0;
}

