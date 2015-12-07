#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	vc = NULL;
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

	if (vc == NULL)
		delete vc;
	printf_globle("TiggerWatcherAndGrabber unload!\n");
}


void TiggerWatcherAndGrabber::Init(string virtualImg)
{
	if (virtualImg == "")
	{
		printf_globle("TiggerWatcherAndGrabber load! 当前模式为真实相机。\n");
		Switch2Real(true);
	}
	else
	{
		printf_globle("TiggerWatcherAndGrabber load! 当前模式为虚拟相机。\n");
		Switch2Virtual(virtualImg, true);
	}
}
void TiggerWatcherAndGrabber::Switch2Virtual(string virtualImg, bool refresh)
{
	IsGrabbing = false;

	if (USING_VIRTUAL_CAMERA && !refresh)
		return;
	USING_VIRTUAL_CAMERA = true;
	if (globle_var::_mdi.fg != NULL)
		MicroDisplayInit::Release(globle_var::_mdi);




	vc = new VirtualCamera(globle_var::mdi(), virtualImg);//初始化虚拟相机
	printf_globle("虚拟相机初始化完成！\n");
}
void TiggerWatcherAndGrabber::Switch2Real(bool refresh)
{
	IsGrabbing = false;

	if (!USING_VIRTUAL_CAMERA && !refresh)
		return;
	USING_VIRTUAL_CAMERA = false;
	if (globle_var::_mdi.fg != NULL)
		MicroDisplayInit::Release(globle_var::_mdi);


	//初始化采集卡
	int status = MicroDisplayInit::InitParameter(globle_var::_mdi);
	//status = MicroDisplayInit::InitLoad(globle_var::_mdi, "4096RGB1LineX1.mcf");
	if (status < 0)
	{
		MD_ErrorMessageWait(globle_var::mdi().fg);
		return;
	}
	MicroDisplayInit::CreateBufferWithOutDiplay(globle_var::_mdi);
	//MicroDisplayInit::CreateBufferWithDiplay(globle_var::_mdi);
	Fg_saveConfig(globle_var::mdi().fg, "save.mcf");
	//没有报错说明初始化完成，有错会自动报错并退出。
	printf_globle("相机&采集卡初始化完成！\n");


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
			if (GrabbingIndex > 10000000) GrabbingIndex = 1;

			stringstream ss;
			ss << GrabbingIndex << " " << "producer: Start  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");

			IsGrabbing = true;
			globle_var::s.Start();

			double t = (double)cv::getTickCount();


			ss << GrabbingIndex << " " << "TiggerWaitTimeMS: " << globle_var::TiggerWaitTimeMS << endl;
			printf_globle(ss.str());
			ss.str("");
			//触发后，等待砖进入拍摄区。
			Sleep(globle_var::TiggerWaitTimeMS);

			//std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
			//t_3in1.detach();


			if (!USING_VIRTUAL_CAMERA)
			{
				MicroDisplayControler::FreeRunning(globle_var::_mdi, globle_var::s);
			}
			else
			{
				if (vc->buffer.cols == 0 || vc->buffer.rows == 0)
				{
					printf_globle("虚拟相机底片不存在。");
					IsGrabbing = false;
					if (hwnd != NULL)
					{
						PostMessage(hwnd, WM_USER + 100, 0, 0);
					}
					return;
				}
				vc->FreeRunning(globle_var::_mdi, globle_var::s);
			}
			//采样计时
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

			ss << GrabbingIndex << " " << "producer:" << globle_var::mdi().width << "x" << globle_var::mdi().MaxPics << "：" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");


			threeInOne();



			//等待3合1
			while (IsThreeInOne)
			{
				Sleep(1);
				//printf_globle("Sleep(1)\n");
			};
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

//图片合成
void TiggerWatcherAndGrabber::threeInOne()
{
	IsThreeInOne = true;
	stringstream ss;
	ss << GrabbingIndex << " " << "ThreeInOne：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	double t = (double)cv::getTickCount();
	//同步进行光照矫正与叠加
	for (globle_var::s.BufferReadIndex = 0; globle_var::s.BufferReadIndex < globle_var::mdi().MaxPics; globle_var::s.BufferReadIndex++)
	{
		//尚未写入缓存，等待
		while (globle_var::s.BufferReadIndex + globle_var::s.NinOne >= globle_var::s.BufferWriteIndex && globle_var::s.BufferWriteIndex < globle_var::mdi().MaxPics)
		{
			Sleep(1);
		}
		globle_var::s.ThreeInOne(globle_var::s.BufferReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	IsThreeInOne = false;
}