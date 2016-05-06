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
//添加一次机械臂执行，返回false表示该机械臂不存在
//1.机械臂编号0-7
//2.触发时间的偏移量，输入500表示500毫秒后触发
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
	////bool emptyFlag = true; //标记待触发为空
	//bool workFlag = false; //标记上一轮工作状态
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
	//				//时间未到，跳出
	//				if (timespanMS < 0)
	//					break;
	//				//如果已经超过了触发的时机
	//				else if (timespanMS > 1000)
	//				{
	//					TickCount[i].pop();//出栈，舍弃该时间戳
	//				}
	//				//触发机械臂
	//				else
	//				{
	//					TickCount[i].pop();//出栈，表示该次机械臂已执行。
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
