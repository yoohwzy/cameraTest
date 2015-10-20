// HelloWorldFreeRunLine.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"

#include "Controller\Contoller.h"

#include <thread>
#include "mc100.h"

#include "Class\Processor.h"
int main()
{
	////��ʵ���
	//Contoller c = Contoller();
	//�������
	Contoller *c = new Contoller("result11_oԭͼ.jpg");
	//��ѭ��
	string input;
	do{
		std::cout << "����1��ʼ��ͼ��ִ�������߳���3�Խ���4������ս������ţ�q�˳���"<<endl;
		std::cin >> input;
		if (input == "1")
		{
			if (c->sofeState == SofeState::SS_ACTIVATED || c->producerState == ProducerState::PS_GRABBING)
				continue;
			c->sofeState = SofeState::SS_ACTIVATED;

			//�ȴ��ɼ��������
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

			//��ȡ��ֵ��ͼ��
			Processor::Binaryzation(DetectedImg);

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			std::cout << "Binaryzation��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			t = (double)cv::getTickCount();

			Processor::Gaussian_Blur(DetectedImg);

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			std::cout << "Gaussian_Blur��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
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
