#pragma once
#include <thread>
#include "../Class/IOCard/PCI1761.h"
#include "../Class/Helper/TimeHelper.h"
#include <vector>
#include <queue>

using namespace std;
class ArmController
{
public:
	ArmController();
	~ArmController()
	{
		auto h = t_watch.native_handle();
		//ExitThread(h);
		CloseHandle(h);
	}
	////
	//void StartWork();
	//存储需要执行机械臂1动作的时间戳，每次执行完将该条出栈
	vector<queue<int>> TriggerTimestamps;
private:
	const int armCount = 1;//有几条机械臂
	thread t_watch;
	PCI1761 pci1761;

	//时间不断地走，当走到有触发的时刻即触发并出栈
	void tickTick();

};

