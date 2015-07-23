// HelloWorldFreeRunLine.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"
#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include <thread>


BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

<<<<<<< HEAD
const bool USING_VIRTUAL_CAMERA = false;//是否使用虚拟摄像头 1使用 0用E2V
=======
const int USING_VIRTUAL_CAMERA = 1;//是否使用虚拟摄像头
>>>>>>> bf7d178fc64e70f6838b90186d71fbcb666d6cc4

int main()
{
	mdi.colorType = mdi.GRAY;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//采集多少帧图像
	s = BufferStorage(mdi.width, mdi.MaxPics);

	if (!USING_VIRTUAL_CAMERA)
<<<<<<< HEAD
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
		vc = VirtualCamera(mdi);
	}
=======
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
		vc = VirtualCamera(mdi);
	}
>>>>>>> bf7d178fc64e70f6838b90186d71fbcb666d6cc4




	int grabbingIndex = 0;
	//主循环
	while (true)
	{
		grabbingIndex += 1;
		if (grabbingIndex > 1000) grabbingIndex = 1;

		char input;
		std::cout << "输入1开始采图，q退出：";
		do{
			input = getchar();
			if (input == 'q')
				return 0;
			Sleep(10);
		} while (input != '1');

		//触发

		//初始化缓存
		s.Start();


		double t = (double)cv::getTickCount();

		if (!USING_VIRTUAL_CAMERA)
		{
			if (MicroDisplayControler::FreeRunning(mdi, s) < 0)
			{
				ErrorMessageWait(mdi.fg);
				return -1;
			}
		}
		else
		{
			vc.FreeRunning(mdi, s);
		}
<<<<<<< HEAD
		//采样计时
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cout << mdi.width << "x" << mdi.MaxPics << "：" << t << endl;
		//重新开始计时
		t = (double)cv::getTickCount();
=======
>>>>>>> bf7d178fc64e70f6838b90186d71fbcb666d6cc4

		//处理算法

		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cout << "处理用时：" << t << endl;

		string p1;
		stringstream ss1;
		ss1 << "samples/result" << grabbingIndex << "_o.jpg";
		ss1 >> p1;
		string p2;
		stringstream ss2;
		ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
		ss2 >> p2;
		cv::imwrite(p1, s.NowBuffer);
		//cv::imwrite("result1.jpg", s.NowBufferGray);
		cv::imwrite(p2, s.NowBufferImg);
	}
	MicroDisplayInit::Release(mdi);
	return 0;
}

