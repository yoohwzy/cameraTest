// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"

#include "Controller\Contoller.h"

#include <thread>
#include "mc100.h"

#include "Class\Processor.h"
int main()
{
	////真实相机
	//Contoller c = Contoller();
	//虚拟相机
	Contoller *c = new Contoller("result11_o原图.jpg");
	//主循环
	string input;
	do{
		std::cout << "输入1开始采图并执行消费者程序，3对焦，4拍摄光照矫正样张，q退出："<<endl;
		std::cin >> input;
		if (input == "1")
		{
			if (c->sofeState == SofeState::SS_ACTIVATED || c->producerState == ProducerState::PS_GRABBING)
				continue;
			c->sofeState = SofeState::SS_ACTIVATED;

			//等待采集处理完毕
			while (c->sofeState == SofeState::SS_ACTIVATED  || c->producerState != ProducerState::PS_GRABBING_END)
			{
				Sleep(10);
			}
		}
		else if (input == "4")
		{
		}
		else if (input == "3")
		{
			Processor p;
			cv::Mat DetectedImg = cv::imread("result1_x3.jpg");
			double t = (double)cv::getTickCount();

			//获取二值化图像
			Processor::Binaryzation(DetectedImg);

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			std::cout << "Binaryzation：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			t = (double)cv::getTickCount();

			Processor::Gaussian_Blur(DetectedImg);

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			std::cout << "Gaussian_Blur：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			t = (double)cv::getTickCount();
		}
		else
			Sleep(10);
	} while (input != "q");
	
	c->sofeState = SofeState::SS_EXIT;
	Sleep(500);
	delete c;
	return 0;
}
