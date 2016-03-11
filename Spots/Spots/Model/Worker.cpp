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
		if (startFrame >= E2VBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			startFrame -= E2VBuffer::BufferLength;
		//触发后等待一段时间，砖走到拍摄区域后再获取图像
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

	//开启线程保存图片
	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//开始图像处理
	Synthesizer s = Synthesizer(SN);
	Synthesizer::Status status = s.Run(grayImg);
	
	if (status != Synthesizer::Status::NotFound)
	{
		//绘制缺陷图像
		drawFaults(grayImg, s.faults);

		//显示结果
		P_Controller->ShowWorkResult(grayImg, 1);
	}

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

	//三合一没有做 转入算法模块做预处理
	return p_e2vbuffer->GetImage(startFrame, endFrame);
}


void Worker::drawFaults(cv::Mat& img, Faults& faults)
{
	if (img.channels() == 1)
		cv::cvtColor(img, img, CV_GRAY2RGB);

	if (faults.BrokenEdges.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << SN << " 存在 " << faults.BrokenEdges.size() << " 处崩边缺陷，红色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.BrokenEdges.size(); i++)
		{
			cv::circle(img, faults.BrokenEdges[i].position, faults.BrokenEdges[i].length + 50, cv::Scalar(0, 0, 255), 10);
		}
		//arm.AddAction(0, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
	}
	if (faults.BrokenCorners.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << SN << " 存在 " << faults.BrokenCorners.size() << " 处崩角缺陷，洋红标出。" << endl;
			MFCConsole::Output(ss.str());
		}

		for (size_t i = 0; i < faults.BrokenCorners.size(); i++)
		{
			cv::circle(img, faults.BrokenCorners[i].position, faults.BrokenCorners[i].length + 50, cv::Scalar(127, 0, 228), 5);
		}
		//arm.AddAction(1, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
	}
	//if (faults.SomethingBigs.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << SN << " 存在 " << faults.BrokenEdges.size() << " 处崩角缺陷，洋红标出。" << endl;
	//		MFCConsole::Output( << ss.str();
	//	}

	//	CString str;
	//	str.Format(_T("%d 存在%d处EID缺陷，蓝色标出。\r\n"), SN, faults.SomethingBigs.size());
	//	m_Info += str;
	//	clog += str;
	//	for (size_t i = 0; i < faults.SomethingBigs.size(); i++)
	//	{
	//		cv::circle(img, faults.SomethingBigs[i].position, faults.SomethingBigs[i].diameter, cv::Scalar(255, 0, 0), 5);
	//	}
	//	//arm.AddAction(2, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
	//}
	if (faults.Scratchs.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << SN << " 存在 " << faults.Scratchs.size() << " 处划痕缺陷，绿色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.Scratchs.size(); i++)
		{
			cv::circle(img, faults.Scratchs[i].position, faults.Scratchs[i].length, cv::Scalar(0, 255, 0), 5);
		}
		//arm.AddAction(3, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
	}
	if (faults.Holes.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << SN << " 存在 " << faults.Holes.size() << " 处凹点缺陷，黄色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.Holes.size(); i++)
		{
			cv::circle(img, faults.Holes[i].position, faults.Holes[i].diameter, cv::Scalar(0, 255, 255), 5);
		}
	}
	if (faults.MarkPens.size() > 0)
	{

		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << SN << " 存在 " << faults.MarkPens.size() << " 处人工标记，橙色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.MarkPens.size(); i++)
		{
			cv::rectangle(img, faults.MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
		}
	}
}