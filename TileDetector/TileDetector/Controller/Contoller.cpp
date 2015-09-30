#include "Contoller.h"


Contoller::Contoller()
{
	sofeState.SetState(StateMachine::SofeState::SS_BEFORE_INIT);
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

	sofeState.SetState(StateMachine::SofeState::SS_INITED);
}

//监听采集卡状态进程
void Contoller::IoCardWatcherThread()
{
	sofeState.SetState(StateMachine::SofeState::SS_WATCHING);
	do{
		if (sofeState.GetState == StateMachine::SofeState::SS_WATCHING &&
			producerState.GetState != StateMachine::ProducerState::PS_GRABBING &&
			mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sofeState.SetState(StateMachine::SofeState::SS_ACTIVATED);
			//producerState.GetState != StateMachine::ProducerState::GRABBING
			OutPutDebugInfo("触发器触发");
			Sleep(10);
			//等待采集处理完毕
			while (producerState.GetState == StateMachine::ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}
		}
		else
		{
			Sleep(1);
		}
	} while (sofeState.GetState != StateMachine::SofeState::SS_EXIT);
}

void Contoller::RunnerThread()
{
	int grabbingIndex = 0;
	do{
		//触发器触发未响应，且程序并未在采图
		if (sofeState.GetState == StateMachine::SofeState::SS_ACTIVATED &&
			producerState.GetState != StateMachine::ProducerState::PS_GRABBING)
		{
			//设置程序状态回到WATCHING，表示触发信号已响应
			sofeState.SetState(StateMachine::SofeState::SS_WATCHING);

			OutPutDebugInfo("开始采图");
			Sleep(DelayTime);

			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;

			//初始化缓存
			s.Start();
			producerState.SetState(StateMachine::ProducerState::PS_GRABBING);
			std::thread t1(std::mem_fn(Contoller::producerThread));
			t1.detach();

			//等待采集处理完毕
			while (producerState.GetState == StateMachine::ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}

			//若采图完成时，消费者正在运行算法，表示上一轮的处理尚未完成，算法速度不够。
			if (customerState.GetState == StateMachine::CustomerState::CS_PROCESSING)
				ExitWithError("算法速度过慢！上一轮处理尚未完成！");
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				//string p1;
				//stringstream ss1;
				//ss1 << "samples/result" << grabbingIndex << "_o原图.jpg";
				//ss1 >> p1;
				//string p2;
				//stringstream ss2;
				//ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
				//ss2 >> p2;
				//cv::imwrite(p1, s.NowBuffer);
				//cv::imwrite(p2, s.NowBufferImg);
			}
#endif
		}
		else
		{
			Sleep(10);
		}
	} while (sofeState.GetState != StateMachine::SofeState::SS_EXIT);
}





//生产者
void Contoller::producerThread()
{
	double t = (double)cv::getTickCount();

	if (!USING_VIRTUAL_CAMERA)
	{
		MicroDisplayControler::FreeRunning(mdi, s);
	}
	else
	{
		vc->FreeRunning(mdi, s);
	}
	//采样计时
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << mdi.width << "x" << mdi.MaxPics << "：" << t << endl;


	t = (double)cv::getTickCount();
	//同步进行光照矫正与叠加
	for (s.BufferReadIndex = 0; s.BufferReadIndex < mdi.MaxPics; s.BufferReadIndex++)
	{
		//尚未写入缓存，等待
		while (s.BufferReadIndex + s.NinOne >= s.BufferWriteIndex && s.BufferWriteIndex != mdi.MaxPics)
		{
			Sleep(1);
		}
		s.ThreeInOne(s.BufferReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "ThreeInOne：" << t << endl;
	//标记生产者工作结束
	producerState.SetState(StateMachine::ProducerState::PS_GRABBING_END);
}
