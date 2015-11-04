#include "Contoller.h"

Contoller::Contoller(string virtualImg)
{
	sofeState = SofeState::SS_BEFORE_INIT;
	//配置参数
	globle_var::initMDI();
	globle_var::initBufferStorage();

	/*读取定标图片 定标*/
	//BlocksDetector bd_Standard = BlocksDetector(cv::imread("A9划痕凹点_x3二值化.jpg"));
	//bd_Standard.Start();
	//bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	//bd_Standard.StartUP_DOWN(BlocksDetector::Down);
	//bd_Standard.ABCD();
	//Measurer m = Measurer(&bd_Standard, &globle_var::mdi, 300, 600);		 //定标图片初始化
	//printf_globle("定标完成\r\n");


	if (virtualImg == "")
		initForE2V();
	else
		initForVC(virtualImg);

	//启动  启动器  进程
	std::thread t_runner(std::mem_fn(&Contoller::RunnerThread), this);
	t_runner.detach();

	sofeState = SofeState::SS_WATCHING;
}
void Contoller::initForE2V()
{
	USING_VIRTUAL_CAMERA = 0;//是否使用虚拟摄像头 1使用 0用E2V
	//初始化采集卡
	status = MicroDisplayInit::InitParameter(globle_var::mdi);
	//status = MicroDisplayInit::InitLoad(globle_var::mdi, "4096RGB1LineX1.mcf");
	if (status < 0)
	{
		MD_ErrorMessageWait(globle_var::mdi.fg);
		return;
	}
	MicroDisplayInit::CreateBufferWithOutDiplay(globle_var::mdi);
	//MicroDisplayInit::CreateBufferWithDiplay(globle_var::mdi);
	Fg_saveConfig(globle_var::mdi.fg, "save.mcf");
	//没有报错说明初始化完成，有错会自动报错并退出。
	printf_globle("相机&采集卡初始化完成！\r\n");


	/**********************初始化采集卡***********************/
	if (mc100_open(0) >= 0)
	{
		printf_globle("打开采集卡0成功！\r\n");
		//开始监听采集卡
		std::thread t_watcher(std::mem_fn(&Contoller::IoCardWatcherThread), this);
		t_watcher.detach();
		mc100_write_port(0, MC100_PORTA, 0x00);
		IoCardOK = 1;
	}
	else
	{
		printf_globle("打开采集卡0失败！\r\n");
	}
}
void Contoller::initForVC(string virtualImg)
{
	USING_VIRTUAL_CAMERA = 1;
	vc = VirtualCamera(globle_var::mdi, virtualImg);//初始化虚拟相机
	printf_globle("相机&采集卡初始化完成！\r\n");
}










/******************************进程方法**********************************/

//监听采集卡状态进程
void Contoller::IoCardWatcherThread()
{
	sofeState = SofeState::SS_WATCHING;
	do{
		if (State_CanBeTiggered() &&	mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sofeState = SofeState::SS_ACTIVATED;
			//producerState != ProducerState::GRABBING
			printf_globle("触发器触发\r\n");
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
	do{
		//生产者并未在采图
		if (producerState != ProducerState::PS_GRABBING)
		{
			//触发器触发未响应
			if (sofeState == SofeState::SS_ACTIVATED)
			{
				producerState = ProducerState::PS_GRABBING;

				printf_globle("开始采图\r\n");
				Sleep(DelayTime);

				grabbingIndex += 1;
				if (grabbingIndex > 10000000) grabbingIndex = 1;


				//初始化缓存
				globle_var::s.Start();
				//启动生产者
				std::thread t1(std::mem_fn(&Contoller::producerThread), this);
				t1.detach();


				//等待采集处理完毕
				while (producerState == ProducerState::PS_GRABBING)
				{
					Sleep(10);
				}

				//若采图完成时，消费者正在运行算法，表示上一轮的处理尚未完成，算法速度不够。
				if (customerState == CustomerState::CS_PROCESSING)
					ExitWithError("算法速度过慢！上一轮处理尚未完成！");

				//启动消费者
				std::thread t2(std::mem_fn(&Contoller::customerThread), this);
				t2.detach();
				Sleep(10);

				//启动处理进程
				customerState = CustomerState::CS_PROCESSING;
				//while (customerState == CustomerState::CS_PROCESSING)//加入等待则变为顺序结构，即处理完成后才能下一次触发
				//{
				//	Sleep(10);
				//}

				//#ifdef OUTPUT_DEBUG_INFO
				//			if (OUTPUT_DEBUG_INFO)
				//			{
				//				string p1;
				//				stringstream ss1;
				//				ss1 << "samples/result" << grabbingIndex << "_o原图.jpg";
				//				ss1 >> p1;
				//				string p2;
				//				stringstream ss2;
				//				ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
				//				ss2 >> p2;
				//				cv::imwrite(p1, globle_var::s.NowBuffer);
				//				//cv::imwrite(p2, globle_var::s.NowBufferImg);
				//			}
				//#endif

				//设置程序状态回到WATCHING，表示触发信号已响应
				sofeState = SofeState::SS_WATCHING;
			}
			else
			{
				Sleep(10);
			}
		}
		else if (sofeState == SofeState::SS_ACTIVATED)
			//设置程序状态回到WATCHING，表示触发信号已响应
			sofeState = SofeState::SS_WATCHING;

	} while (sofeState != SofeState::SS_EXIT);
}


//生产者
void Contoller::producerThread()
{
	double t = (double)cv::getTickCount();
	if (!USING_VIRTUAL_CAMERA)
	{
		MicroDisplayControler::FreeRunning(globle_var::mdi, globle_var::s);
	}
	else
	{
		vc.FreeRunning(globle_var::mdi, globle_var::s);
	}
	//采样计时
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

	stringstream ss;
	ss << grabbingIndex << " " << "producer:" << globle_var::mdi.width << "x" << globle_var::mdi.MaxPics << "：" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	//标记生产者工作结束
	producerState = ProducerState::PS_GRABBING_END;
}
//消费者
void Contoller::customerThread()
{
	stringstream ss;
	ss << grabbingIndex << " " << "customer：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	threeInOne();

	//图片合成完成后才标准开始处理
	customerState = CustomerState::CS_PROCESSING;
	//开始计时
	double t = (double)cv::getTickCount();

	cv::Mat DetectedImg = globle_var::s.NowBufferImg.clone();
	if (DetectedImg.channels() > 0)
	{
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cv::cvtColor(DetectedImg, DetectedImg, CV_BGR2GRAY);
		ss << grabbingIndex << " " << "customer-cvtColor：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");

		t = (double)cv::getTickCount();
	}
	////获取二值化图像
	//Processor::Binaryzation(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << grabbingIndex << " " << "Binaryzation：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();

	//Processor::Gaussian_Blur(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << grabbingIndex << " " << "Gaussian_Blur：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();


	//瓷砖边缘检测
//	BlocksDetector bd = BlocksDetector(DetectedImg);
//	t = (double)cv::getTickCount();
//	bd.Start();
//	bd.StartUP_DOWN(BlocksDetector::Up);
//	bd.StartUP_DOWN(BlocksDetector::Down);
//	bd.ABCD();
//	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
//	ss << grabbingIndex << " " << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");

//#ifdef OUTPUT_DEBUG_INFO
//	if (OUTPUT_DEBUG_INFO)
//	{
//		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
//		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
//		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
//	}
//#endif
//
//	//瓷砖边缘缺陷检测
//	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
//	if (ed.Defects.size() > 0)
//	{
//		ss << grabbingIndex << " " << "边缘有缺陷" << endl;
	//printf_globle(ss.str());
	//ss.str("");
//	}
//	else
//	{
//		Measurer m = Measurer();
//		m.CaculteSize(&bd);
//	}
//	BlockHoughDetector bhd = BlockHoughDetector(globle_var::s.NowBufferImg);

	//标记消费者工作结束
	ss << grabbingIndex << " " << "customer：End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	customerState = CustomerState::CS_WAITING;
}
//图片合成
void Contoller::threeInOne()
{
	stringstream ss;
	ss << grabbingIndex << " " << "customer-ThreeInOne：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	double t = (double)cv::getTickCount();
	//同步进行光照矫正与叠加
	for (globle_var::s.BufferReadIndex = 0; globle_var::s.BufferReadIndex < globle_var::mdi.MaxPics; globle_var::s.BufferReadIndex++)
	{
		//尚未写入缓存，等待
		while (globle_var::s.BufferReadIndex + globle_var::s.NinOne >= globle_var::s.BufferWriteIndex && globle_var::s.BufferWriteIndex != globle_var::mdi.MaxPics)
		{
			Sleep(1);
		}
		globle_var::s.ThreeInOne(globle_var::s.BufferReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << grabbingIndex << " " << "customer-ThreeInOne：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
}