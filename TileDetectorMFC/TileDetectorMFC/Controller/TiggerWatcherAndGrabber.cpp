#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	Init(virtualImg);

	IsGrabbing2 = false;
	p_gb = new GrabbingBuffer(globle_var::FrameCount, globle_var::Width);

	/*读取定标图片 定标*/
	//BlocksDetector bd_Standard = BlocksDetector(cv::imread("A9划痕凹点_x3二值化.jpg"));
	//bd_Standard.Start();
	//bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	//bd_Standard.StartUP_DOWN(BlocksDetector::Down);
	//bd_Standard.ABCD();
	//Measurer m = Measurer(&bd_Standard, &globle_var::mdi(), 300, 600);		 //定标图片初始化
	//printf_globle("定标完成\n");
	printf_globle("TiggerWatcherAndGrabber loaded!\n");
}

void TiggerWatcherAndGrabber::Init(string virtualImg)
{
	if (virtualImg == "")
	{
		printf_globle("TiggerWatcherAndGrabber load! 当前模式为真实相机。\n");
		Switch2Real();
	}
	else
	{
		printf_globle("TiggerWatcherAndGrabber load! 当前模式为虚拟相机。\n");
		Switch2Virtual(virtualImg);
	}
}
bool TiggerWatcherAndGrabber::Switch2Virtual(string virtualImg)
{
	if (p_e2v != NULL)
	{
		delete p_e2v;
		p_e2v = NULL;
	}
	if (p_vc != NULL)
	{
		delete p_vc;
		p_vc = NULL;
	}


	IsGrabbing = false;
	USING_VIRTUAL_CAMERA = true;
	printf_globle("配置为【虚拟相机】模式！\n");
	p_vc = new VirtualCameraPre(globle_var::FrameCount, globle_var::Width, virtualImg);
	return VirtualCameraPre::TestCam(virtualImg);
}
bool TiggerWatcherAndGrabber::Switch2Real()
{
	if (p_e2v != NULL)
	{
		delete p_e2v;
		p_e2v = NULL;
	}
	if (p_vc != NULL)
	{
		delete p_vc;
		p_vc = NULL;
	}

	IsGrabbing = false;

	USING_VIRTUAL_CAMERA = false;
	printf_globle("配置为【相机&采集卡】模式！\n");

	/**********************初始化IO卡***********************/
	//if (mc100.init())
	//	printf_globle("打开mc100成功！\n");
	//else
	//	printf_globle("打开mc100失败！\n");
	if (pci1761.init())
		printf_globle("打开pci1761成功！\n");
	else
		printf_globle("打开pci1761失败！\n");

	/**********************初始化采集卡*********************/
	p_e2v = new E2VCamera(globle_var::FrameCount, globle_var::Width, globle_var::FrameTimeUS);
	return true;
}




void TiggerWatcherAndGrabber::StartWatch()
{
	if (!IsWatching && p_t_watcher == NULL)
	{
		IsWatching = true;
		p_t_watcher = new thread(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		p_t_watcher->detach();
		printf_globle("StartWatch：开始监控触发信号\n");

	}
	else{
		printf_globle("StartWatch：正在监控，不能重复监控操作。\n");
	}
}
void TiggerWatcherAndGrabber::StartWatchWithCalibration()
{
	IsCalibration = true;
	StartWatch();
}


void TiggerWatcherAndGrabber::StopWatch()
{
	printf_globle("停止监控触发信号\n");
	IsWatching = false;
	Sleep(50);
	p_t_watcher = NULL;
}
bool TiggerWatcherAndGrabber::ManualTigger()
{
	if (IsGrabbing)
	{
		printf_globle("IsGrabbing = 1\n");
		return false;
	}
	if (!IsWatching)
	{
		printf_globle("IsWatching = 0\n");
		return false;
	}

	BeManualTiggered = true;
	return true;
}


void TiggerWatcherAndGrabber::watcherThread()
{
	double t_span = (double)cv::getTickCount();//用于计算两次触发间隔

	while (IsWatching)
	{
		if ((BeManualTiggered || pci1761.GetRisingEdgeIDI(7)))
		{
			if (IsGrabbing)
			{
				printf_globle("IsGrabbing = 1 触发后不响应\r\n");
				continue;
			}


			IsGrabbing = true;
#ifdef TWAG_OUTPUT_TO_CONSOLE
			double t = (double)cv::getTickCount();
			stringstream ss;
			if (BeManualTiggered)
			{
				printf_globle("手动触发\n");
			}
			else
				printf_globle("触发器触发\n");
#endif

			if (BeManualTiggered)
				BeManualTiggered = false;

#ifdef TWAG_OUTPUT_TO_CONSOLE

			ss << _grabbingIndex + 1 << " " << "producer: Start" << endl;
			printf_globle(ss.str());
			ss.str("");

			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << _grabbingIndex + 1 << " " << "twag Init(ms):" << t << endl;
			printf_globle(ss.str());
			LogHelper::Log(ss.str());
			ss.str("");
#endif




			if (globle_var::TiggerCaptureWaitTimeMS > 0)
			{
#ifdef TWAG_OUTPUT_TO_CONSOLE
				double tSleep = (double)cv::getTickCount();
#endif
				//触发后，等待砖进入拍摄区。
				//Sleep(globle_var::TiggerCaptureWaitTimeMS);
				std::this_thread::sleep_for(chrono::milliseconds(globle_var::TiggerCaptureWaitTimeMS));

#ifdef TWAG_OUTPUT_TO_CONSOLE
				tSleep = ((double)cv::getTickCount() - tSleep) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << _grabbingIndex + 1 << " " << "Sleep real(ms):" << tSleep << endl;
				printf_globle(ss.str());
#endif
			}



			if (!IsGrabbing2)
			{
				_grabbingIndex += 1;
				if (_grabbingIndex > 1000000) _grabbingIndex = 1;

#ifdef TWAG_OUTPUT_TO_CONSOLE

			t_span = ((double)cv::getTickCount() - t_span) / cv::getTickFrequency();
			ss << endl << endl << "第 " << _grabbingIndex << "次触发：与上次触发间隔为" << t_span << "秒" << endl;
			printf_globle(ss.str());
			LogHelper::Log(ss.str());
			ss.str("");
			t_span = (double)cv::getTickCount();//用于计算两次触发间隔


#endif



				std::thread t_run(std::mem_fn(&TiggerWatcherAndGrabber::runThread), this);
				//auto tn = t_run.native_handle();
				//SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);
				t_run.detach();
			}
			else
			{
				ss << _grabbingIndex + 1 << " " << "        触发后不响应  IsGrabbing2 = 1 ,still running capture();" << endl;
				printf_globle(ss.str());
				LogHelper::Log(ss.str());
			}
			IsGrabbing = false;
		}
		else
		{
			//Sleep(10);
			std::this_thread::sleep_for(chrono::milliseconds(10));
		}
	}
}

void TiggerWatcherAndGrabber::runThread(){



	IsGrabbing2 = true;
	GrabbingIndex = _grabbingIndex;


	std::thread t_capture(std::mem_fn(&TiggerWatcherAndGrabber::capture), this);
	auto tn = t_capture.native_handle();
	SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);
	if (DoThreeInOne)
	{
		std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
		t_3in1.join();
	}
	t_capture.join();

	if (DoThreeInOne)
	{
		//图片获取完成，复制到全局变量，并删除s
		//OriginalImage.release();
		//Image.release();
		OriginalImage = p_gb->OriginalImage.clone();
		Image = p_gb->Image.clone();
	}
	else
	{
		OriginalImage = p_gb->OriginalImage.clone();
		Image = OriginalImage;
	}




	IsGrabbing2 = false;

	if (!IsCalibration)
	{
		if (hwnd != NULL)
		{
			PostMessage(hwnd, WM_USER + 100, 0, 0);
		}
		//标记生产者工作结束
	}
	else//若是标定环境，则停止监控
	{
		IsCalibration = false;
		StopWatch();
		PostMessage(hwnd, WM_USER + 102, 0, 0);//定标、采集结束处理程序
	}
}


void TiggerWatcherAndGrabber::capture()
{

#ifdef TWAG_OUTPUT_TO_CONSOLE
	double t = (double)cv::getTickCount();
	printf_globle("  capture(); START \r\n");
#endif






	p_gb->Start();
	if (!USING_VIRTUAL_CAMERA)
	{
		p_e2v->Capture(p_gb);
	}
	else
	{
		p_vc->Capture(p_gb);
	}


#ifdef TWAG_OUTPUT_TO_CONSOLE
	//采样计时
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	stringstream ss;
	ss << GrabbingIndex << " " << "capture();  END" << endl << globle_var::Width << "x" << globle_var::FrameCount << "：" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
#endif
}
//图片合成
void TiggerWatcherAndGrabber::threeInOne()
{
#ifdef TWAG_OUTPUT_TO_CONSOLE
	double t = (double)cv::getTickCount();
#endif


	for (p_gb->ReadIndex = 0; p_gb->ReadIndex < globle_var::FrameCount; p_gb->ReadIndex++)
	{
		//尚未写入缓存，等待
		while (p_gb->ReadIndex + p_gb->NinOne >= p_gb->WriteIndex && p_gb->WriteIndex < globle_var::FrameCount)
		{
			//Sleep(10);
			std::this_thread::sleep_for(chrono::milliseconds(10));
		}
		p_gb->ThreeInOne(p_gb->ReadIndex);
	}
#ifdef TWAG_OUTPUT_TO_CONSOLE
	stringstream ss;
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
#endif
}