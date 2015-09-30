// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include "Class\BlocksDetector.h"
#include "Class\Processor.h"


#include <thread>
#include "mc100.h"

BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

const bool USING_VIRTUAL_CAMERA = 1;//是否使用虚拟摄像头 1使用 0用E2V

//采集到第几张
int grabbingIndex = 0;

//开始采集，采集中标志
bool startFlag = false, grabbingFlag = false;
//生产者完成标志，消费者完成标志
bool producerEndFlag = false, customerEndFlag = false;

bool exitFlag = false;

//生产者
void producer()
{
	producerEndFlag = false;

	double t = (double)cv::getTickCount();

	if (!USING_VIRTUAL_CAMERA)
	{
		if (MicroDisplayControler::FreeRunning(mdi, s) < 0)
		{
			MD_ErrorMessageWait(mdi.fg);
			return;
		}
	}
	else
	{
		vc.FreeRunning(mdi, s);
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
	producerEndFlag = true;
}
//消费者
void customer()
{
	customerEndFlag = false;

	//开始计时
	double t = (double)cv::getTickCount();

	cv::Mat DetectedImg = s.NowBufferImg.clone();
	if (DetectedImg.channels() > 0)
		cv::cvtColor(DetectedImg, DetectedImg, CV_BGR2GRAY);
	//获取二值化图像
	Processor::Binaryzation(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Binaryzation：" << t << endl;
	t = (double)cv::getTickCount();

	Processor::Gaussian_Blur(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Gaussian_Blur：" << t << endl;
	t = (double)cv::getTickCount();


	//瓷砖边缘检测
	BlocksDetector bd = BlocksDetector(DetectedImg);
	t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "BlocksDetector：" << t << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif
	//标记消费者工作结束
	customerEndFlag = true;
}
int init()
{
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
			return -1;
		}
		MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
		Fg_saveConfig(mdi.fg, "save.mcf");
		//MicroDisplayInit::CreateBufferWithDiplay(mdi);//使用彩色相机时，运行速度会很慢
	}
	else
	{
		//初始化虚拟相机
		vc = VirtualCamera(mdi, "D31崩角_o原图.jpg");
	}
}
//监听采集卡状态
void watcher()
{
	do{
		// 读取PA0引脚的电平为低,且程序并未在采图时
		if (grabbingFlag == false && mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			startFlag = true;
			std::cout << "触发器触发" << endl;
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
//采图进程启动者
void runner()
{
	do{
		// 读取PA0引脚的电平为低,且程序并未在采图时
		if (startFlag == true && grabbingFlag == false)
		{
			std::cout << "开始采图" << endl;
			grabbingFlag = true;
			startFlag = false;

			Sleep(410);

			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;

			//初始化缓存
			s.Start();

			std::thread t1(producer);
			t1.detach();

			//等待采集处理完毕
			while (!producerEndFlag)
			{
				Sleep(10);
			}

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

			grabbingFlag = false;
		}
		else
		{
			Sleep(10);
		}
	} while (!exitFlag);
}
int main()
{
	ExitWithError("123");
	init();
	printf("init end！\r\n");

	if (mc100_open(0) >= 0)
	{
		cout << "打开采集卡0成功！" << endl;
		//开始监听采集卡
		std::thread t_watcher(watcher);
		t_watcher.detach();
	}
	else
	{
		cout << "打开采集卡0失败！" << endl;
	}

	std::thread t_runner(runner);
	t_runner.detach();

	//主循环
	string input;
	mc100_write_port(0, MC100_PORTA, 0x00);
	do{
		std::cout << "输入1开始采图，2执行消费者程序，3对焦，4拍摄光照矫正样张，q退出：";
		std::cin >> input;
		if (input == "1")
		{
			startFlag = true;
			Sleep(10);

			while (grabbingFlag == true)
			{
				Sleep(10);
			}
		}

		else if (input == "2")
		{
			std::thread t2(customer);
			t2.detach();
			Sleep(10);

			while (!customerEndFlag)
			{
				Sleep(10);
			}
		}
		else if (input == "4")
		{
			//采样矫正图像
			int MaxPics = mdi.MaxPics;
			mdi.MaxPics = 10;
			BufferStorage stmp = BufferStorage(mdi);
			stmp.Start();
			if (!USING_VIRTUAL_CAMERA)
			{
				if (MicroDisplayControler::FreeRunning(mdi, stmp) < 0)
				{
					MD_ErrorMessageWait(mdi.fg);
					return 0;
				}
			}
			else
			{
				VirtualCamera vctmp = VirtualCamera(mdi, "光照校正_o.jpg");
				vctmp.FreeRunning(mdi, stmp);
			}
			//cv::Mat img = stmp.NowBufferImg;

			//X - 样本，矫正时直接相加即可
			cv::Mat SamplesRGBTMP = stmp.NowBufferImg(cv::Rect(0, 2, mdi.width, 1)).clone() / 3;
			cv::imwrite("samples/光照校正样本.jpg", SamplesRGBTMP);
			//RGB
			cv::Mat SamplesRGB = cv::Mat(mdi.height, mdi.width, CV_8UC3, cv::Scalar(50, 50, 50));
			SamplesRGB = SamplesRGB - SamplesRGBTMP;
			SamplesRGB.copyTo(mdi.SamplesRGB);
			//Gray
			cv::cvtColor(SamplesRGB, mdi.SamplesGray, CV_BGR2GRAY);
			cv::Mat SamplesGray = mdi.SamplesGray;
			std::cout << "OK\r\n";

			//重新初始化缓存
			mdi.MaxPics = MaxPics;
			s = BufferStorage(mdi);
		}
		else if (input == "3")
		{
			MicroDisplayInit::Release(mdi);
			mdi.height = 200;
			status = MicroDisplayInit::InitParameter(mdi);
			if (status < 0)
			{
				return -1;
			}
			MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
			//MicroDisplayInit::CreateBufferWithDiplay(mdi);
			MicroDisplayControler::FreeRunningFocusing(mdi);
			MicroDisplayInit::Release(mdi);
			init();
		}
		else
			Sleep(10);
	} while (input != "q");
	exitFlag = true;
	MicroDisplayInit::Release(mdi);
	Sleep(500);
	return 0;
}
