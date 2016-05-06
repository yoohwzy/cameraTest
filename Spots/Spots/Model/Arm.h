#pragma once
#include <afxwin.h>

#include <thread>
#include <Class\IOCard\PCI1761\PCI1761.h>
#include <Class\Helper\TimeHelper.h>
#include <vector>
#include <queue>

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
	//2.在何时触发（时间戳）
	bool AddAction(int ArmNum, SYSTEMTIME ct);

	//存储需要执行机械臂1动作的时间戳，每次执行完将该条出栈
	vector<queue<SYSTEMTIME>> TriggerTimestamps;
	PCI1761 pci1761;
private:
	const int armCount = 1;//有几条机械臂
	thread t_watch;
	bool stopFlag = false;
	//时间不断地走，当走到有触发的时刻即触发并出栈
	void tickTick();

};

