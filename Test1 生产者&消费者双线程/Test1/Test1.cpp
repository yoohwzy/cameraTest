// Test1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

#include "VirtualCamera.h"
#include "Storage.h"
#include <thread>
#include <iostream>
#include <windows.h>

using namespace std;



VirtualCamera vc;
Storage s;
cv::Mat NowBuffer;
cv::Mat NowBufferGray;
cv::Mat NowBufferImg;

//生产者
void producer()
{
	do{
		cv::Mat f = vc.GetNext();
		s.AddFrame(f);
	} while (!vc.EndFlag);
}


//消费者
void customer()
{
	int i = 0;
	int flag = 0;
	do{
		cv::Mat f;
		flag = s.GetFrame(f);
		if (flag == -1)
		{
			Sleep(1);
			continue;
		}
		if (flag == 0)
			break;

		NowBuffer(cv::Rect(0, i, s.WIDTH, 1)) += f;

		NowBufferImg(cv::Rect(0, i, s.WIDTH, 1)) += f;
		NowBufferImg(cv::Rect(0, i + 1, s.WIDTH, 1)) += f;
		NowBufferImg(cv::Rect(0, i + 2, s.WIDTH, 1)) += f;

		cv::Mat frameGray;
		cv::cvtColor(f, frameGray, CV_BGR2GRAY);
		//NowBufferGray(cv::Rect(0, i, s.WIDTH, 1)) += frameGray;





		//表面检测与拼接
		i++;

	} while (flag != 0);

	//  + 各种算法
}
int _tmain(int argc, _TCHAR* argv[])
{
	s = Storage(vc.WIDTH, vc.bufferLength);
	NowBuffer = cv::Mat(s.LENGTH, s.WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
	NowBufferGray = cv::Mat(s.LENGTH, s.WIDTH, CV_8UC3, cv::Scalar(0));
	NowBufferImg = cv::Mat(s.LENGTH + 10, s.WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
	s.Start();

	double t = (double)cv::getTickCount();

	std::thread t1(producer);
	t1.join();

	std::thread t2(customer);
	t2.join();


	//等待消费者处理完毕
	while (!s.EndFlag)
	{
		Sleep(10);
	}

	//while (!vc.EndFlag)
	//{
	//	Sleep(10);
	//}

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();


	cv::imwrite("result1.jpg", NowBuffer);
	cv::imwrite("result2.jpg", NowBufferGray);
	cv::imwrite("result3.jpg", NowBufferImg);
	//cv::namedWindow("1");
	//cv::imshow("1", Pic);
	//cv::waitKey(0);


	cout << s.WIDTH << "x" << s.LENGTH << "：" << t << endl;
	return 0;
}