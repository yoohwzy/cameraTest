// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include "Class\BlocksDetector.h"
#include <thread>

BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

const bool USING_VIRTUAL_CAMERA = true;//是否使用虚拟摄像头 1使用 0用E2V
bool producerEndFlag = false, customerEndFlag = false;


//生产者
void producer()
{
	double t = (double)cv::getTickCount();

	if (!USING_VIRTUAL_CAMERA)
	{
		if (MicroDisplayControler::FreeRunning(mdi, s) < 0)
		{
			ErrorMessageWait(mdi.fg);
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

	//标记生产者工作结束
	producerEndFlag = true;
}
//消费者
void customer()
{
	BlocksDetector bd = BlocksDetector(&s, &mdi);

	//开始计时
	double t1 = (double)cv::getTickCount();

	//处理算法

	//到了第几行
	int i = 0;
	//状态标记0表示结束，-1表示需要等待下一帧写入
	int flag = 0;
	do{
		if (s.EndWriteFlag)
			break;
		cv::Mat f;
		flag = s.GetFrame(f);
		if (flag == -1)
		{
			Sleep(1);
			continue;
		}
		if (flag == 0)
			break;
		//检测算法
		cv::Mat oneLine = s.NowBufferImg(cv::Rect(0, i, mdi.width, 1));
		int elementCount = mdi.width;//每行元素数
		uchar* lineheadRGB = oneLine.ptr<uchar>(0);//每行的起始地址
		i++;
	} while (flag != 0);
	t1 = ((double)cv::getTickCount() - t1) / cv::getTickFrequency();
	std::cout << "并行处理用时：" << t1 << endl;

	while (!s.EndWriteFlag)
	{
		Sleep(10);
	}

	double t = (double)cv::getTickCount();
	bd.Start();
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "非并行处理用时：" << t << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif
	//标记消费者工作结束
	customerEndFlag = true;
}

int main()
{
	mdi.colorType = mdi.GRAY;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//采集多少帧图像
	s = BufferStorage(mdi);

	if (!USING_VIRTUAL_CAMERA)
	{
		//初始化采集卡
		status = MicroDisplayInit::InitParameter(mdi);
		if (status < 0)
		{
			ErrorMessageWait(mdi.fg);
			return -1;
		}
		//MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
		MicroDisplayInit::CreateBufferWithDiplay(mdi);
	}
	else
	{
		//初始化虚拟相机
		//vc = VirtualCamera(mdi, "瓷砖崩边上边.jpg");
		//vc = VirtualCamera(mdi, "样品2_o.jpg");
		//vc = VirtualCamera(mdi, "瓷砖缺陷2_o.jpg");
		vc = VirtualCamera(mdi, "瓷砖崩边1_o.jpg");
	}




	int grabbingIndex = 0;
	//主循环
	string input;
	do{
		std::cout << "输入1开始采图，2拍摄光照矫正样张，q退出：";
		std::cin >> input;
		
		if (input == "1")
		{
			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;

			//触发

			//初始化缓存
			s.Start();

			std::thread t1(producer);
			t1.join();

			std::thread t2(customer);
			t2.join();

			//等待两个线程处理完毕
			while (!customerEndFlag || !producerEndFlag)
			{
				Sleep(10);
			}

			string p1;
			stringstream ss1;
			ss1 << "samples/result" << grabbingIndex << "_o.jpg";
			ss1 >> p1;
			string p2;
			stringstream ss2;
			ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
			ss2 >> p2;
			//cv::imwrite(p1, s.NowBuffer);
			//cv::imwrite("result1.jpg", s.NowBufferGray);
			//cv::imwrite(p2, s.NowBufferImg);
		}
		if (input == "2")
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
					ErrorMessageWait(mdi.fg);
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
		else
			Sleep(10);
	} while (input != "q");

	MicroDisplayInit::Release(mdi);
	return 0;
}