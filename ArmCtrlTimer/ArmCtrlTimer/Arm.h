#pragma once

//opencv 与 pci1761头文件位置不能弄反
#include <opencv2/opencv.hpp>
//#include <Class/IOCard/PCI1761/PCI1761.h>
#include <thread>
#include <mutex>

#include <map>

using namespace std;

class Arm
{
public:
	Arm();
	~Arm()
	{
		stopFlag = true;
		std::this_thread::sleep_for(chrono::milliseconds(150));
	}
	//添加一次机械臂执行时间，一般再触发器触发后调用
	//1.瓷砖SN号
	//2.触发时间的偏移量，输入500表示500毫秒后触发
	void AddAction(int sn, int timeOffSetMS = 0)
	{
		//t = ((double)getTickCount() - t) / getTickFrequency();
		int64 t = cv::getTickCount();
		int64 tick = cv::getTickCount() + timeOffSetMS / 1000 * cv::getTickFrequency();
		actionTime.insert(map<int, int64>::value_type(sn, tick));
	}
	//添加执行的输出口编号，一般在检测算法结束后调用
	//1.瓷砖SN号
	//2.几号继电器输出，-1表示不输出
	void AddPort(int sn, int Port = -1)
	{
		actionPort.insert(map<int, int>::value_type(sn, Port));
	}


	////存储需要执行机械臂X动作的时间戳，每次执行完将该条出栈
	//vector<queue<int>> TickCount;
	////设置是否要启动该次触发，若Action[0][0] = 0 表示TickCount[0][0]这一次触发不执行，反之为1则执行
	//vector<queue<bool>> Action;
	//PCI1761 pci1761;
private:
	thread t_watch;
	bool stopFlag = false;
	//时间不断地走，当走到有触发的时刻即触发并出栈
	void tickTick();
	void erasePortByActionTime(map<int, int64>::iterator& it)
	{
		int key = it->first;
		map<int, int>::iterator itActionPort = actionPort.find(key);
		//删除actionPort
		while (itActionPort != actionPort.end())
		{
			actionPort.erase(itActionPort);
			itActionPort = actionPort.find(key);
		}
		//删除actionTime
	}


	//存储需要执行机械臂0动作的时间戳cv::getTickCount()，每次执行完将该条出栈
	map<int, int64> actionTime;
	//设置是否要启动该次触发，若actionPort[i] = -1 表示actionTime0[i]这一次触发不执行
	//为0则 O0口执行，为1则O1口执行
	map<int, int> actionPort;

};