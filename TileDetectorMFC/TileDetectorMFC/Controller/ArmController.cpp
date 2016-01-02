#include "ArmController.h"


ArmController::ArmController()
{
	for (size_t i = 0; i < armCount; i++)
	{
		queue<SYSTEMTIME> q;
		TriggerTimestamps.push_back(q);
	}
	if (pci1761.init())
	{
		pci1761.SetR(0x00);

		t_watch = std::thread(std::mem_fn(&ArmController::tickTick), this);
		t_watch.detach();
	}
}
bool ArmController::AddAction(int ArmNum, SYSTEMTIME ctime)
{
	if (ArmNum < 0 || ArmNum >= armCount)
		return false;
	TriggerTimestamps[ArmNum].push(ctime);

	return true;
}
void ArmController::tickTick()
{
	//bool emptyFlag = true; //��Ǵ�����Ϊ��
	bool workFlag = false; //�����һ�ֹ���״̬
	while (stopFlag)
	{
		//emptyFlag = true;
		workFlag = false;
		for (size_t i = 0; i < armCount; i++)
		{
			if (TriggerTimestamps[i].size() > 0)
			{
				//emptyFlag = false;

				SYSTEMTIME st1 = TimeHelper::GetTimeNow();
				while (TriggerTimestamps[i].size() > 0)
				{
					SYSTEMTIME st2 = TriggerTimestamps[i].front();
					int timespan = TimeHelper::GetTimeSpanMilliseconds(st1, st2);
					//ʱ��δ��������
					if (timespan < 0)
						break;
					//����Ѿ������˴�����ʱ��
					else if (timespan > 1000)
					{
						TriggerTimestamps[i].pop();//��ջ��������ʱ���
					}
					//������е��
					else
					{
						TriggerTimestamps[i].pop();//��ջ����ʾ�ôλ�е����ִ�С�
						workFlag = true;

						pci1761.SetR(i, true);
						std::this_thread::sleep_for(chrono::milliseconds(500));
						pci1761.SetR(i, false);
					}
				}
			}
			else
			{
				continue;
			}
		}
		std::this_thread::sleep_for(chrono::milliseconds(100));
	}
}
