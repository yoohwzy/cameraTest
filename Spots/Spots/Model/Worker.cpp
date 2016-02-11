#include "Worker.h"
#include <Controller\Controller.h>

Worker::Worker(E2VBuffer *_e2vbuffer)
{
	p_e2vbuffer = _e2vbuffer;
}


Worker::~Worker()
{
}

void Worker::StartWork()
{
	std::thread t_workThread(std::mem_fn(&Worker::work), this);
	t_workThread.detach();
	//work();
}
void Worker::work()
{
	//采图
	cv::Mat grayImg;
	//真实相机
	if (p_e2vbuffer != NULL)
	{
		MyStatus = WorkerStatus::Busy;

		int startFrame = p_e2vbuffer->GetWriteIndex();
		//加入延时时间
		frameIndexAdd(startFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
		if (startFrame >= E2VBuffer::BufferLength)
			startFrame -= E2VBuffer::BufferLength;
		//触发后等待一段时间，砖走到拍摄区域
		image = getPhoto(startFrame, 0);
		grayImg = image;
	}
	//虚拟相机
	else if (image.cols > 0)
	{
		MFCConsole::Output("虚拟相机模式运行\r\n");
		grayImg = image.clone();
	}
	else
	{
		MFCConsole::Output("硬件初始化失败，且虚拟相机无底片，请设置好再试。\r\n");
		MessageBox(0, L"硬件初始化失败，请设置好再试。", L"警告", 0);
		return;
	}
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//开始图像处理
	//Synthesizer s = Synthesizer(0);
	//Synthesizer::Status status  = s.Run(image);
	
	//显示结果
	P_Controller->ShowWorkResult(image);
	MyStatus = WorkerStatus::Done;
	return;
}





//length < 12000
//取图范围 startFrame 到 endFrame，包括startFrame与endFrame两行
cv::Mat Worker::getPhoto(int startFrame, int length)
{
	//等待相机进入拍摄区要等待的行数
	int waitLength = WaitTimeMS * 1000 / ImgScanner::FrameTimeUS;


	if (length == 0)//采集固定长度
		length = Worker::frameCountsOut;

	int endFrameAbso = startFrame + length + waitLength;//绝对最后一帧，到了这一帧不管触发器是否有下降沿都停止采集。while循环break。
	int endFrame = endFrameAbso;


	GetPhotoOn = true;
	//wait capture end
	Sleep(100);
	while (GetPhotoOn)
	{
		//判断是否读够那么多行
		int now = p_e2vbuffer->GetWriteIndex();
		if (now < startFrame)
			now += E2VBuffer::BufferLength;

		//因超时而退出
		if (now >= endFrameAbso)
		{
			break;
		}
		Sleep(2);
	}
	if (GetPhotoOn == false)
	{
		//进入此处说明是外部将GetPhotoOn置0，即触发器下降沿信号结束了采集
		//此处重新计算endFrame
		endFrame = p_e2vbuffer->GetWriteIndex() + waitLength;
		//TODO::判断每一帧是否是结束。加一个while循环，判断每一帧是否为全黑，全黑说明采集应该结束了。

		while (true)
		{
			//判断是否读够那么多行
			int now = p_e2vbuffer->GetWriteIndex();
			if (now < startFrame)
				now += E2VBuffer::BufferLength;

			//计数完成而退出
			if (now >= endFrameAbso)
			{
				GetPhotoOn = false;
				break;
			}
			Sleep(2);
		}
	}

	startFrame += waitLength;

	//TODO::三合一没有做
	return p_e2vbuffer->GetImage(startFrame, endFrame);
}




////length < 12000
////取图范围 startFrame 到 endFrame，包括startFrame与endFrame两行
//cv::Mat Worker::getPhoto(int startFrame, int length)
//{
//	if (length == 0)//采集固定长度
//		length = Worker::frameCountsOut;
//
//	int endFrameAbso = startFrame;//绝对最后一帧，到了这一帧不管触发器是否有下降沿都停止采集。while循环break。
//	frameIndexAdd(endFrameAbso, length - 1);
//	int endFrame = endFrameAbso;
//
//	GetPhotoOn = true;
//	//wait capture end
//	Sleep(100);
//	while (GetPhotoOn)
//	{
//		//判断是否读够那么多行
//		int now = p_e2vbuffer->GetWriteIndex();
//		//因超时而退出
//		if (
//			(endFrameAbso > startFrame  && now >= endFrameAbso) ||
//			(startFrame > endFrameAbso && now >= endFrameAbso && now < length)
//			)
//		{
//			break;
//		}
//		Sleep(2);
//	}
//	if (GetPhotoOn == false)
//	{
//		//进入此处说明是外部将GetPhotoOn置0，即触发器下降沿信号结束了采集
//		//此处重新计算endFrame
//		endFrame = p_e2vbuffer->GetWriteIndex();
//		frameIndexAdd(endFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
//		//TODO::判断每一帧是否是结束。加一个while循环，判断每一帧是否为全黑，全黑说明采集应该结束了。		
//	}
//	while (true)
//	{
//		//判断是否读够那么多行
//		int now = p_e2vbuffer->GetWriteIndex();
//		//计数完成而退出
//		if (
//			(endFrame > startFrame  && now >= endFrame) ||
//			(startFrame > endFrame && now >= endFrame && now < length)
//			)
//		{
//			GetPhotoOn = false;
//			break;
//		}
//		Sleep(2);
//	}
//
//
//
//	//TODO::三合一没有做
//	return p_e2vbuffer->GetImage(startFrame, endFrame);
//}
