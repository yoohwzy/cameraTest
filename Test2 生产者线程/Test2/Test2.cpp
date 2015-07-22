// Test2.cpp : 定义控制台应用程序的入口点。
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



//生产者
void producer()
{
	do{
		cv::Mat f = vc.GetNext();
		s.AddFrame(f);
	} while (!vc.EndFlag);
}


int _tmain(int argc, _TCHAR* argv[])
{	
	s = Storage(vc.WIDTH, vc.bufferLength);
	s.Start();

	double t = (double)cv::getTickCount(); 
	producer();
	
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << vc.WIDTH << "x" << vc.bufferLength << "：" << t << endl;

	cv::imwrite("result.jpg", s.NowBuffer);
	cv::imwrite("result1.jpg", s.NowBufferGray);
	cv::imwrite("result2.jpg", s.NowBufferImg);

	return 0;
}

