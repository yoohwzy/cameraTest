#include "ArmController.h"


ArmController::ArmController()
{
	pci1761.init();
	for (size_t i = 0; i < armCount; i++)
	{
		queue<int> q;
		TriggerTimestamps.push_back(q);
	}

	t_watch = std::thread(std::mem_fn(&ArmController::tickTick), this);
	t_watch.detach();
}
//void ArmController::StartWork()
//{
//	t_watch = std::thread(std::mem_fn(&ArmController::tickTick), this);
//	t_watch.detach();
//}

void ArmController::tickTick()
{
	bool emptyFlag = true; //标记待触发为空
	bool workFlag = false; //标记上一轮工作状态
	while (true)
	{
		emptyFlag = true;
		workFlag = false;
		for (size_t i = 0; i < armCount; i++)
		{
			if (TriggerTimestamps[i].size() > 0)
			{
				emptyFlag = false;
				while((TimeHelper::GetTimestamp() - TriggerTimestamps[i].front()) > 1000)//如果已经超过了触发的时机
				{
					TriggerTimestamps[i].pop();//出栈，舍弃该时间戳
				}
				if (TriggerTimestamps[i].front() < TimeHelper::GetTimestamp())
				{
					TriggerTimestamps[i].pop();//出栈，表示该次机械臂已执行。
					workFlag = true;

					pci1761.SetR(i, true);
					std::this_thread::sleep_for(chrono::milliseconds(50));
					pci1761.SetR(i, false);
				}
			}
			else
			{
				continue;
			}
		}
		if (emptyFlag)
		{
			std::this_thread::sleep_for(chrono::milliseconds(100));
		}
		else
		{

		}
	}
}
