#pragma once

//opencv �� pci1761ͷ�ļ�λ�ò���Ū��
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
	//���һ�λ�е��ִ�У�����false��ʾ�û�е�۲�����
	//1.��е�۱��0-7
	//2.����ʱ���ƫ����������500��ʾ500����󴥷�
	bool AddAction(int ArmNum, int timeOffSetMS = 0);

	//�洢��Ҫִ�л�е��1������ʱ�����ÿ��ִ���꽫������ջ
	vector<queue<int>> TickCount;
	PCI1761 pci1761;
private:
	const int armCount = 1;//�м�����е��
	thread t_watch;
	bool stopFlag = false;
	//ʱ�䲻�ϵ��ߣ����ߵ��д�����ʱ�̼���������ջ
	void tickTick();

};

