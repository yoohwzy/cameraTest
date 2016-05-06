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
	//���һ�λ�е��ִ�У�����false��ʾ�û�е�۲�����
	//1.��е�۱��0-7
	//2.�ں�ʱ������ʱ�����
	bool AddAction(int ArmNum, SYSTEMTIME ct);

	//�洢��Ҫִ�л�е��1������ʱ�����ÿ��ִ���꽫������ջ
	vector<queue<SYSTEMTIME>> TriggerTimestamps;
	PCI1761 pci1761;
private:
	const int armCount = 1;//�м�����е��
	thread t_watch;
	bool stopFlag = false;
	//ʱ�䲻�ϵ��ߣ����ߵ��д�����ʱ�̼���������ջ
	void tickTick();

};

