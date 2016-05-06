#include "Arm.h"


Arm::Arm()
{
	for (size_t i = 0; i < armCount; i++)
	{
		queue<int> q;
		TickCount.push_back(q);
	}
	if (pci1761.init())
	{
		pci1761.SetR(0x00);

		t_watch = std::thread(std::mem_fn(&Arm::tickTick), this);
		t_watch.detach();
	}
}
//���һ�λ�е��ִ�У�����false��ʾ�û�е�۲�����
//1.��е�۱��0-7
//2.����ʱ���ƫ����������500��ʾ500����󴥷�
bool Arm::AddAction(int ArmNum, int timeOffSetMS)
{
	//int t = cv::getTickCount() + timeOffSetMS * 1000 * cv::getTickFrequency();
	int t = 0;
	if (ArmNum < 0 || ArmNum >= armCount)
		return false;
	TickCount[ArmNum].push(t);

	return true;
}
void Arm::tickTick()
{
	////bool emptyFlag = true; //��Ǵ�����Ϊ��
	//bool workFlag = false; //�����һ�ֹ���״̬
	//while (stopFlag)
	//{
	//	//emptyFlag = true;
	//	workFlag = false;
	//	for (size_t i = 0; i < armCount; i++)
	//	{
	//		if (TickCount[i].size() > 0)
	//		{
	//			//emptyFlag = false;

	//			int st1 = cv::getTickCount();
	//			while (TickCount[i].size() > 0)
	//			{
	//				int st2 = TickCount[i].front();
	//				double timespanMS = double(st1 - st2) / (cvGetTickFrequency() * 1000);
	//				//ʱ��δ��������
	//				if (timespanMS < 0)
	//					break;
	//				//����Ѿ������˴�����ʱ��
	//				else if (timespanMS > 1000)
	//				{
	//					TickCount[i].pop();//��ջ��������ʱ���
	//				}
	//				//������е��
	//				else
	//				{
	//					TickCount[i].pop();//��ջ����ʾ�ôλ�е����ִ�С�
	//					workFlag = true;
	//					pci1761.SetR(i, true);
	//					std::this_thread::sleep_for(chrono::milliseconds(200));
	//					pci1761.SetR(i, false);
	//				}
	//			}
	//		}
	//		else
	//		{
	//			continue;
	//		}
	//	}
	//	std::this_thread::sleep_for(chrono::milliseconds(100));
	//}
}
