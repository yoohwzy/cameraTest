#pragma once

//opencv 与 pci1761头文件位置不能弄反
#include <opencv2/opencv.hpp>
#include <Class/IOCard/PCI1761/PCI1761.h>
#include <queue>
#include <thread>
#include <mutex>

using namespace std;

class Arm
{
public:
	Arm();
	~Arm()
	{
		stopFlag = true;
		//auto h = t_watch.native_handle();
		Sleep(150);
		//ExitThread(h);
		//CloseHandle(h);
	}
	//添加一次机械臂执行，返回false表示该机械臂不存在
	//1.机械臂编号0-7
	//2.触发时间的偏移量，输入500表示500毫秒后触发
	bool AddAction(int ArmNum, int timeOffSetMS = 0);

	//存储需要执行机械臂1动作的时间戳，每次执行完将该条出栈
	vector<queue<int>> TickCount;
	PCI1761 pci1761;
private:
	const int armCount = 1;//有几条机械臂
	thread t_watch;
	bool stopFlag = false;
	//时间不断地走，当走到有触发的时刻即触发并出栈
	void tickTick();

};

