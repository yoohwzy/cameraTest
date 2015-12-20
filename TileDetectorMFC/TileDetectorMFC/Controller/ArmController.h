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
	//�洢��Ҫִ�л�е��1������ʱ�����ÿ��ִ���꽫������ջ
	vector<queue<int>> TriggerTimestamps;
private:
	const int armCount = 1;//�м�����е��
	thread t_watch;
	PCI1761 pci1761;

	//ʱ�䲻�ϵ��ߣ����ߵ��д�����ʱ�̼���������ջ
	void tickTick();

};

