// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"

#include "Controller\Contoller.h"

#include <thread>
#include "mc100.h"


int main()
{
	////真实相机
	//Contoller c = Contoller();
	//虚拟相机
	Contoller *c = new Contoller("D31崩角_o原图.jpg");
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
		}
		else
			Sleep(10);
	} while (input != "q");
	
	c->sofeState = SofeState::SS_EXIT;
	Sleep(500);
	delete c;
	return 0;
}
