#pragma once

//opencv �� pci1761ͷ�ļ�λ�ò���Ū��
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
	//���һ�λ�е��ִ��ʱ�䣬һ���ٴ��������������
	//1.��שSN��
	//2.����ʱ���ƫ����������500��ʾ500����󴥷�
	void AddAction(int sn, int timeOffSetMS = 0)
	{
		//t = ((double)getTickCount() - t) / getTickFrequency();
		int64 t = cv::getTickCount();
		int64 tick = cv::getTickCount() + timeOffSetMS / 1000 * cv::getTickFrequency();
		actionTime.insert(map<int, int64>::value_type(sn, tick));
	}
	//���ִ�е�����ڱ�ţ�һ���ڼ���㷨���������
	//1.��שSN��
	//2.���ż̵��������-1��ʾ�����
	void AddPort(int sn, int Port = -1)
	{
		actionPort.insert(map<int, int>::value_type(sn, Port));
	}


	////�洢��Ҫִ�л�е��X������ʱ�����ÿ��ִ���꽫������ջ
	//vector<queue<int>> TickCount;
	////�����Ƿ�Ҫ�����ôδ�������Action[0][0] = 0 ��ʾTickCount[0][0]��һ�δ�����ִ�У���֮Ϊ1��ִ��
	//vector<queue<bool>> Action;
	//PCI1761 pci1761;
private:
	thread t_watch;
	bool stopFlag = false;
	//ʱ�䲻�ϵ��ߣ����ߵ��д�����ʱ�̼���������ջ
	void tickTick();
	void erasePortByActionTime(map<int, int64>::iterator& it)
	{
		int key = it->first;
		map<int, int>::iterator itActionPort = actionPort.find(key);
		//ɾ��actionPort
		while (itActionPort != actionPort.end())
		{
			actionPort.erase(itActionPort);
			itActionPort = actionPort.find(key);
		}
		//ɾ��actionTime
	}


	//�洢��Ҫִ�л�е��0������ʱ���cv::getTickCount()��ÿ��ִ���꽫������ջ
	map<int, int64> actionTime;
	//�����Ƿ�Ҫ�����ôδ�������actionPort[i] = -1 ��ʾactionTime0[i]��һ�δ�����ִ��
	//Ϊ0�� O0��ִ�У�Ϊ1��O1��ִ��
	map<int, int> actionPort;

};