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
	//bool emptyFlag = true; //标记待触发为空
	bool workFlag = false; //标记上一轮工作状态
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
					//时间未到，跳出
					if (timespan < 0)
						break;
					//如果已经超过了触发的时机
					else if (timespan > 1000)
					{
						TriggerTimestamps[i].pop();//出栈，舍弃该时间戳
					}
					//触发机械臂
					else
					{
						TriggerTimestamps[i].pop();//出栈，表示该次机械臂已执行。
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
