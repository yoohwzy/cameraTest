#include "Arm.h"
#include "MFCConsole.h"


Arm::Arm()
{

	//if (pci1761.init())
	//{
	//	pci1761.SetR(0x00);

	//	t_watch = std::thread(std::mem_fn(&Arm::tickTick), this);
	//	t_watch.detach();
	//}

	t_watch = std::thread(std::mem_fn(&Arm::tickTick), this);
	t_watch.detach();
	MFCConsole::Output("Arm inited!\r\n");
}
void Arm::tickTick()
{
	while (!stopFlag)
	{
		//MFCConsole::Output("1\r\n");
		for (map<int, int64>::iterator it = actionTime.begin(); it != actionTime.end();)
		{
			//MFCConsole::Output("2\r\n");
			int sn = it->first;
			long long tick = it->second;

			map<int, int>::iterator itActionPort = actionPort.find(sn);
			//cout << "key: " << it->first << " value: " << it->second << endl;

			double timespanMS = double(cv::getTickCount() - tick) / (cvGetTickFrequency() * 1000);
			//时间未到，跳出
			if (timespanMS < 0)
			{
				++it;
				break;
			}
			//如果已经超过了触发的时机
			else if (timespanMS > 1000)
			{
				erasePortByActionTime(it);
				actionTime.erase(it++);
			}
			//触发机械臂
			else
			{
				if (itActionPort != actionPort.end() && itActionPort->second >= 0)
				{
					int port = itActionPort->second;
					//pci1761.SetR(port, true);
					std::this_thread::sleep_for(chrono::milliseconds(200));
					//pci1761.SetR(port, false);
					stringstream ss;
					ss << "触发 key: " << it->first << " value: " << it->second << " prot:" << port << endl;
					MFCConsole::Output(ss.str());
				}
				else
				{
					stringstream ss;
					ss << "无port 不触发 key: " << it->first << " value: " << it->second << endl;
					MFCConsole::Output(ss.str());
				}
				erasePortByActionTime(it);//出栈，表示该次机械臂已执行。
				actionTime.erase(it++);

			}
		}
		std::this_thread::sleep_for(chrono::milliseconds(100));
	}
}
