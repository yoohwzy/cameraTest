#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	Init(virtualImg);


	gb = new GrabbingBuffer(globle_var::FrameCount, globle_var::Width);

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
TiggerWatcherAndGrabber::~TiggerWatcherAndGrabber()
{
	if (gb != NULL)
	{
		delete gb;
		gb = NULL;
	}
	if (e2v != NULL)
	{
		delete e2v;
		e2v = NULL;
	}

	printf_globle("TiggerWatcherAndGrabber unload!\n");
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
	_virtualImg = virtualImg;
	IsGrabbing = false;
	USING_VIRTUAL_CAMERA = true;
	printf_globle("配置为【虚拟相机】模式！\n");
	return VirtualCamera::TestCam(virtualImg);
}
bool TiggerWatcherAndGrabber::Switch2Real()
{
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
	if (e2v != NULL)
		delete e2v;
	e2v = new E2VCamera(globle_var::FrameCount, globle_var::Width);
	return true;
}




void TiggerWatcherAndGrabber::StartWatch()
{
	if (!IsWatching && t_watcher == NULL)
	{
		IsWatching = true;
		//std::thread t_watcher(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		t_watcher = new thread(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		t_watcher->detach();
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
	t_watcher = NULL;
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
		if (!IsGrabbing && (BeManualTiggered || pci1761.GetRisingEdgeIDI(7) ))
		{
			IsGrabbing = true;

#ifdef OUTPUT_TO_CONSOLE
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

			GrabbingIndex += 1;
			if (GrabbingIndex > 1000000) GrabbingIndex = 1;

			t_span = ((double)cv::getTickCount() - t_span) / cv::getTickFrequency();
			ss << endl << endl << "第 " << GrabbingIndex << "次触发：与上次触发间隔为" << t_span << "秒" << endl;
			printf_globle(ss.str());
			LogHelper::Log(ss.str());
			ss.str("");
			t_span = (double)cv::getTickCount();//用于计算两次触发间隔

#ifdef OUTPUT_TO_CONSOLE

			ss << GrabbingIndex << " " << "producer: Start" << endl;
			printf_globle(ss.str());
			ss.str("");

			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << GrabbingIndex << " " << "twag Inint(ms):" << t << endl;
			printf_globle(ss.str());
			LogHelper::Log(ss.str());
			ss.str("");

#endif

			if (globle_var::TiggerWaitTimeMS > 0)
			{
				//触发后，等待砖进入拍摄区。
				Sleep(globle_var::TiggerWaitTimeMS);

#ifdef OUTPUT_TO_CONSOLE
				t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << GrabbingIndex << " " << "Sleep real(ms):" << t << endl;
				printf_globle(ss.str());
#endif
			}


			std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
			std::thread t_capture(std::mem_fn(&TiggerWatcherAndGrabber::capture), this);
			t_3in1.join();
			t_capture.join();

			//capture();
			//threeInOne();

			//图片获取完成，复制到全局变量，并删除s
			//OriginalImage.release();
			//Image.release();
			OriginalImage = gb->OriginalImage.clone();
			Image = gb->Image.clone();



			IsGrabbing = false;

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
		else
		{
			Sleep(5);
		}
	}
}
void TiggerWatcherAndGrabber::capture()
{

#ifdef OUTPUT_TO_CONSOLE
	double t = (double)cv::getTickCount();
	printf_globle("  capture(); START \r\n");
#endif

	gb->Start();
	if (!USING_VIRTUAL_CAMERA)
	{
		e2v->Capture(gb);
	}
	else
	{
		VirtualCamera *vc = new VirtualCamera(gb, globle_var::FrameCount, globle_var::Width, _virtualImg);
		vc->Capture();
		delete vc;
	}


#ifdef OUTPUT_TO_CONSOLE
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
#ifdef OUTPUT_TO_CONSOLE
	double t = (double)cv::getTickCount();
#endif


	for (gb->ReadIndex = 0; gb->ReadIndex < globle_var::FrameCount; gb->ReadIndex++)
	{
		//尚未写入缓存，等待
		while (gb->ReadIndex + gb->NinOne >= gb->WriteIndex && gb->WriteIndex < globle_var::FrameCount)
		{
			Sleep(10);
		}
		gb->ThreeInOne(gb->ReadIndex);
	}



#ifdef OUTPUT_TO_CONSOLE
	stringstream ss;
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
#endif
}