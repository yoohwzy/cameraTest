#include "Contoller.h"


Contoller::Contoller()
{
	sm.SetState(StateMachine::State::BEFORE_INIT);
	//配置参数
	mdi.colorType = MicroDisplayInit::RGB;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 11000;//采集多少帧图像
	s = BufferStorage(mdi);

	if (!USING_VIRTUAL_CAMERA)
	{
		//初始化采集卡
		status = MicroDisplayInit::InitParameter(mdi);
		//status = MicroDisplayInit::InitLoad(mdi, "4096RGB1LineX1.mcf");
		if (status < 0)
		{
			MD_ErrorMessageWait(mdi.fg);
			return;
		}
		MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
		//MicroDisplayInit::CreateBufferWithDiplay(mdi);
		Fg_saveConfig(mdi.fg, "save.mcf");
	}
	else
	{
		vc = new VirtualCamera(mdi, "D31崩角_o原图.jpg");//初始化虚拟相机
	}
	OutPutDebugInfo("相机&采集卡初始化完成！");


	/**********************初始化采集卡***********************/
	if (mc100_open(0) >= 0)
	{
		OutPutDebugInfo("打开采集卡0成功！");
		//开始监听采集卡
		//std::thread t_watcher(watcher);
		//t_watcher.detach();
		mc100_write_port(0, MC100_PORTA, 0x00);
	}
	else
	{
		OutPutDebugInfo("打开采集卡0失败！");
	}
	/*
	std::thread t_runner(runner);
	t_runner.detach();
	*/

	sm.SetState(StateMachine::State::INITED);
}

//监听采集卡状态进程
void Contoller::IoCardWatcherThread()
{
	do{
		// 读取PA0引脚的电平为低,且程序并未在采图时
		if (sm.GetState != StateMachine::State::GRABBING && mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sm.SetState(StateMachine::State::GRABBING);
			OutPutDebugInfo("触发器触发");
			Sleep(10);
			//等待采集处理完毕
			while (!producerEndFlag)
			{
				Sleep(10);
			}
		}
		else
		{
			Sleep(1);
		}
	} while (!exitFlag);
}