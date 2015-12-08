#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	Init(virtualImg);

	/*读取定标图片 定标*/
	//BlocksDetector bd_Standard = BlocksDetector(cv::imread("A9划痕凹点_x3二值化.jpg"));
	//bd_Standard.Start();
	//bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	//bd_Standard.StartUP_DOWN(BlocksDetector::Down);
	//bd_Standard.ABCD();
	//Measurer m = Measurer(&bd_Standard, &globle_var::mdi(), 300, 600);		 //定标图片初始化
	//printf_globle("定标完成\n");
}
TiggerWatcherAndGrabber::~TiggerWatcherAndGrabber()
{
	//if (!USING_VIRTUAL_CAMERA)
	//{
	//	MicroDisplayInit::Release(globle_var::_mdi);
	//}
	//printf_globle("TiggerWatcherAndGrabber unload!\n");
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

	/**********************初始化采集卡***********************/
	if (mc100_open(0) >= 0)
	{
		printf_globle("打开采集卡0成功！\n");
		mc100_write_port(0, MC100_PORTA, 0x00);
	}
	else
	{
		printf_globle("打开采集卡0失败！\n");
	}
	MicroDisplay::TestCam();
	return true;
}




void TiggerWatcherAndGrabber::StartWatch()
{
	if (!IsWatching)
	{
		IsWatching = true;
		std::thread t_watcher(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		t_watcher.detach();
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
}
bool TiggerWatcherAndGrabber::ManualTigger()
{
	if (IsGrabbing || !IsWatching)
	{
		printf_globle("IsGrabbing = 1\n");
		return false;
	}
	if (IsGrabbing || !IsWatching)
	{
		printf_globle("IsWatching = 0\n");
		return false;
	}

	BeManualTiggered = true;
	return true;
}
void TiggerWatcherAndGrabber::watcherThread()
{
	while (IsWatching)
	{
		if (!IsGrabbing && (BeManualTiggered || BeAutoTiggered()))
		{
			if (BeManualTiggered)
			{
				printf_globle("手动触发\n");
				BeManualTiggered = false;
			}
			else
				printf_globle("触发器触发\n");

			GrabbingIndex += 1;

			if (gb != NULL)
				delete gb;
			gb = new GrabbingBuffer(globle_var::FrameCount, globle_var::Width);

			if (GrabbingIndex > 10000000) GrabbingIndex = 1;

			stringstream ss;
			ss << GrabbingIndex << " " << "producer: Start  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");

			IsGrabbing = true;


			double t = (double)cv::getTickCount();


			ss << GrabbingIndex << " " << "TiggerWaitTimeMS: " << globle_var::TiggerWaitTimeMS << endl;
			printf_globle(ss.str());
			ss.str("");
			//触发后，等待砖进入拍摄区。
			Sleep(globle_var::TiggerWaitTimeMS);

			//std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
			//std::thread t_capture(std::mem_fn(&TiggerWatcherAndGrabber::capture), this);
			//t_3in1.join();
			//t_capture.join();

			capture(); 
			threeInOne();

			//图片获取完成，复制到全局变量，并删除s
			//OriginalImage.release();
			//Image.release();
			OriginalImage = gb->OriginalImage.clone();
			Image = gb->Image.clone();

			delete gb;
			gb = NULL;

			if (!IsCalibration)
			{
				if (hwnd != NULL)
				{
					PostMessage(hwnd, WM_USER + 100, 0, 0);
				}
				//标记生产者工作结束
				IsGrabbing = false;
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
			Sleep(10);
		}
	}
}
void TiggerWatcherAndGrabber::capture()
{
	double t = (double)cv::getTickCount();
	printf_globle("capture(); START \r\n");
	if (!USING_VIRTUAL_CAMERA)
	{
		MicroDisplay *md = new MicroDisplay(gb, globle_var::FrameCount, globle_var::Width);
		md->Capture();
		delete md;
	}
	else
	{
		VirtualCamera *vc = new VirtualCamera(gb, globle_var::FrameCount, globle_var::Width, _virtualImg);
		vc->Capture();
		delete vc;
	}
	//采样计时
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	stringstream ss;
	ss << GrabbingIndex << " " << "capture();  END"<<endl << globle_var::Width << "x" << globle_var::FrameCount << "：" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
}
//图片合成
void TiggerWatcherAndGrabber::threeInOne()
{
	stringstream ss;
	ss << GrabbingIndex << " " << "ThreeInOne：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	double t = (double)cv::getTickCount();
	for (gb->ReadIndex = 0; gb->ReadIndex < globle_var::FrameCount; gb->ReadIndex++)
	{
		//尚未写入缓存，等待
		while (gb->ReadIndex + gb->NinOne >= gb->WriteIndex && gb->WriteIndex < globle_var::FrameCount)
		{
			Sleep(10);
		}
		gb->ThreeInOne(gb->ReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
}