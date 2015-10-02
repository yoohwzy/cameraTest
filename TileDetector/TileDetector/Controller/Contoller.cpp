#include "Contoller.h"


Contoller::Contoller(string virtualImg)
{
	sofeState = SofeState::SS_BEFORE_INIT;
	//配置参数
	mdi.colorType = MicroDisplayInit::RGB;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 11000;//采集多少帧图像
	s = BufferStorage(mdi);

	if (virtualImg == "")
		initForE2V();
	else
		initForVC(virtualImg);

	//启动  启动器  进程
	std::thread t_runner(std::mem_fn(&Contoller::RunnerThread), this);
	t_runner.detach();



	sofeState = SofeState::SS_INITED;
}
void Contoller::initForE2V()
{
	USING_VIRTUAL_CAMERA = 0;//是否使用虚拟摄像头 1使用 0用E2V

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
	//没有报错说明初始化完成，有错会自动报错并退出。
	OutPutDebugInfo("相机&采集卡初始化完成！");


	/**********************初始化采集卡***********************/
	if (mc100_open(0) >= 0)
	{
		OutPutDebugInfo("打开采集卡0成功！");
		//开始监听采集卡
		std::thread t_watcher(std::mem_fn(&Contoller::IoCardWatcherThread), this);
		t_watcher.detach();
		mc100_write_port(0, MC100_PORTA, 0x00);
		IoCardOK = 1;
	}
	else
	{
		OutPutDebugInfo("打开采集卡0失败！");
	}
}
void Contoller::initForVC(string virtualImg)
{
	USING_VIRTUAL_CAMERA = 1;
	vc = VirtualCamera(mdi, virtualImg);//初始化虚拟相机
	OutPutDebugInfo("相机&采集卡初始化完成！");
}










/******************************进程方法**********************************/

//监听采集卡状态进程
void Contoller::IoCardWatcherThread()
{
	sofeState = SofeState::SS_WATCHING;
	do{
		if (sofeState == SofeState::SS_WATCHING &&
			producerState != ProducerState::PS_GRABBING &&
			mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sofeState = SofeState::SS_ACTIVATED;
			//producerState != ProducerState::GRABBING
			OutPutDebugInfo("触发器触发");
			Sleep(10);
			//等待采集处理完毕
			while (producerState == ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}
		}
		else
		{
			Sleep(1);
		}
	} while (sofeState != SofeState::SS_EXIT);
}

void Contoller::RunnerThread()
{
	int grabbingIndex = 0;
	do{
		//触发器触发未响应，且程序并未在采图
		if (sofeState == SofeState::SS_ACTIVATED &&
			producerState != ProducerState::PS_GRABBING)
		{

			producerState = ProducerState::PS_GRABBING;
			//设置程序状态回到WATCHING，表示触发信号已响应
			sofeState = SofeState::SS_WATCHING;

			OutPutDebugInfo("开始采图");
			Sleep(DelayTime);

			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;


			//初始化缓存
			s.Start();
			//启动生产者
			std::thread t1(std::mem_fn(&Contoller::producerThread), this);
			t1.detach();

			//启动消费者
			std::thread t2(std::mem_fn(&Contoller::customerThread), this);
			t2.detach();
			Sleep(10);
			customerState = CustomerState::CS_PROCESSING;


			//等待采集处理完毕
			while (producerState == ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}
			while (customerState == CustomerState::CS_PROCESSING)
			{
				Sleep(10);
			}

			////若采图完成时，消费者正在运行算法，表示上一轮的处理尚未完成，算法速度不够。
			//if (customerState == CustomerState::CS_PROCESSING)
			//	ExitWithError("算法速度过慢！上一轮处理尚未完成！");



#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				string p1;
				stringstream ss1;
				ss1 << "samples/result" << grabbingIndex << "_o原图.jpg";
				ss1 >> p1;
				string p2;
				stringstream ss2;
				ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
				ss2 >> p2;
				cv::imwrite(p1, s.NowBuffer);
				//cv::imwrite(p2, s.NowBufferImg);
			}
#endif
		}
		else
		{
			Sleep(10);
		}
	} while (sofeState != SofeState::SS_EXIT);
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
		vc.FreeRunning(mdi, s);
	}
	//采样计时
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "producer:" << mdi.width << "x" << mdi.MaxPics << "：" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;


	//t = (double)cv::getTickCount();
	////同步进行光照矫正与叠加
	//for (s.BufferReadIndex = 0; s.BufferReadIndex < mdi.MaxPics; s.BufferReadIndex++)
	//{
	//	//尚未写入缓存，等待
	//	while (s.BufferReadIndex + s.NinOne >= s.BufferWriteIndex && s.BufferWriteIndex != mdi.MaxPics)
	//	{
	//		Sleep(1);
	//	}
	//	s.ThreeInOne(s.BufferReadIndex);
	//}
	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//std::cout << "ThreeInOne：" << t << endl;
	//标记生产者工作结束
	producerState = ProducerState::PS_GRABBING_END;
}
//消费者
void Contoller::customerThread()
{
	threeInOne();

	std::cout << "customer：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//图片合成完成后才标准开始处理
	customerState = CustomerState::CS_PROCESSING;
	//开始计时
	double t = (double)cv::getTickCount();

	cv::Mat DetectedImg = s.NowBufferImg.clone();
	if (DetectedImg.channels() > 0)
		cv::cvtColor(DetectedImg, DetectedImg, CV_BGR2GRAY);
	//获取二值化图像
	Processor::Binaryzation(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Binaryzation：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	t = (double)cv::getTickCount();

	Processor::Gaussian_Blur(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Gaussian_Blur：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	t = (double)cv::getTickCount();


	//瓷砖边缘检测
	BlocksDetector bd = BlocksDetector(DetectedImg);
	t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif
	//标记消费者工作结束
	customerState = CustomerState::CS_WAITING;

	std::cout << "customer：End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
}
//图片合成
void Contoller::threeInOne()
{
	std::cout << "ThreeInOne：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	double t = (double)cv::getTickCount();
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
	std::cout << "ThreeInOne：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
}