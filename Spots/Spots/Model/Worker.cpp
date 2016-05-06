#include "Worker.h"
#include <Controller\Controller.h>
#include <Class/Debug/MFCConsole.h>

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
		int firstFrame = startFrame;//记录触发时所写行号
#ifdef WORKER_DEBUG
		stringstream ss;
		ss << "startFrame =" << startFrame << endl;
#endif
		//加入延时时间
		//触发后等待一段时间，砖走到拍摄区域后再获取图像
		frameIndexAdd(startFrame, WaitTimeMSIn * 1000 / ImgScanner::FrameTimeUS);
#ifdef WORKER_DEBUG
		ss << "startFrame(after frameIndexAdd()) =" << startFrame << endl;
		MFCConsole::Output(ss.str());
#endif

		//第一个for循环是为了处理循环指针越界的情况，如startFrame = 19500 + 600 = 100，此时GetWriteIndex = 19800 > 100 需要等待GetWriteIndex越界
		for (size_t i = 0; i < 10; i++)
			if (p_e2vbuffer->GetWriteIndex() > startFrame)
				Sleep(10);
			else
				break;
		//第二个循环 等待相机拍摄到起始行（这样写的原因是加入了延时时间，可能存在相机还未拍摄到startFrame的情况）
		int nowFrame = p_e2vbuffer->GetWriteIndex();
		while (
			!(
			(startFrame >= firstFrame && (nowFrame > startFrame || nowFrame < firstFrame)) ||//第一种情况，startFrame未换页，则有两种可能，1.nowFrame也没换页则要求nowFrame>startFrame，2.或nowFrame换页了(即nowFrame<firstFrame)则立即判定为通过。
			(startFrame < firstFrame && (nowFrame > startFrame))//第二种情况，startFrame换页了，则判定nowFrame > startFrame即可
			)
			)
		{
			Sleep(10);
			nowFrame = p_e2vbuffer->GetWriteIndex();
		}


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

	//开启线程保存图片至硬盘
	P_Controller->ImageGetCallBack(grayImg);


	/********************************************/
	//开始图像处理
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);



	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//开始图像处理
	Synthesizer s = Synthesizer(SN, Real_WidthMM, Real_LengthMM);
	s.BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
	s.BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;

	s.BlockEdgeLineDetector_Enable = BlockEdgeLineDetector_Enable;
	s.BlockEdgeLineDetector_BINARY_THRESHOD = BlockEdgeLineDetector_BINARY_THRESHOD;
	s.BlockEdgeLineDetector_LENGTH_THRESHOD = BlockEdgeLineDetector_LENGTH_THRESHOD;
	s.BlockEdgeLineDetector_DEEP_THRESHOD = BlockEdgeLineDetector_DEEP_THRESHOD;

	s.BlockEdgeDetector_Enable = BlockEdgeDetector_Enable;
	s.BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
	s.BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
	s.BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;

	s.BlockEdgeLineDetector_Enable = BlockEdgeLineDetector_Enable;

	Synthesizer::Status status = s.Run(grayImg);
	
	if (status != Synthesizer::Status::NotFound)
	{
		//绘制缺陷图像
		drawFaults(grayImg, s.faults);
		cv::line(grayImg, cv::Point(0, s.p_block->UpLine.k * (0 - s.p_block->UpLine.x0) + s.p_block->UpLine.y0), cv::Point(grayImg.cols, s.p_block->UpLine.k * (grayImg.cols - s.p_block->UpLine.x0) + s.p_block->UpLine.y0), cv::Scalar(0, 0, 255), 1);
		cv::line(grayImg, cv::Point(0, s.p_block->DownLine.k * (0 - s.p_block->DownLine.x0) + s.p_block->DownLine.y0), cv::Point(grayImg.cols, s.p_block->DownLine.k * (grayImg.cols - s.p_block->DownLine.x0) + s.p_block->DownLine.y0), cv::Scalar(0, 255, 255), 1);
		cv::line(grayImg, cv::Point((grayImg.rows - s.p_block->LeftLine.y0) / s.p_block->LeftLine.k + s.p_block->LeftLine.x0, grayImg.rows), cv::Point((0 - s.p_block->LeftLine.y0) / s.p_block->LeftLine.k + s.p_block->LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
		cv::line(grayImg, cv::Point((grayImg.rows - s.p_block->RightLine.y0) / s.p_block->RightLine.k + s.p_block->RightLine.x0, grayImg.rows), cv::Point((0 - s.p_block->RightLine.y0) / s.p_block->RightLine.k + s.p_block->RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);


		int type = 1;//产品级别  1 A级 2 B级 3 C级 4 不合格
		//直接判断为不合格的情况，若有则type=4

		//产品分级
		if (type != 4)//崩边分级
		{
			double _EDGE_TOTAL_LENGTH = 0;//崩边总长度
			double _EDGE_TOTAL_DEEP = 0;//崩边总深度
			for (int i = 0; i < s.faults.BrokenEdges.size(); i++)
			{
				_EDGE_TOTAL_LENGTH += s.faults.BrokenEdges[i].length_mm;
				_EDGE_TOTAL_DEEP += s.faults.BrokenEdges[i].deep_mm;
				if (s.faults.BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_ACCEPT || s.faults.BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_ACCEPT)
				{
					type = 4;
					break;
				}
				if (type < 3 && (s.faults.BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_C || s.faults.BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_C))
				{
					type = 3;
				}
				if (type < 2 && (s.faults.BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_B || s.faults.BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_B))
				{
					type = 2;
				}
			}
			if (_EDGE_TOTAL_LENGTH > Classify_EDGE_SINGLE_LENGTH_ACCEPT || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_ACCEPT)
				type = 4;
			if (type < 3 && (_EDGE_TOTAL_LENGTH > Classify_EDGE_SINGLE_LENGTH_C || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_C))
				type = 3;
			if (type < 2 && (_EDGE_TOTAL_LENGTH > Classify_EDGE_SINGLE_LENGTH_B || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_B))
				type = 2;

			stringstream ss;
			ss << "崩边总长=" << _EDGE_TOTAL_LENGTH << "mm" << endl;
			ss << "崩边总深=" << _EDGE_TOTAL_DEEP << "mm" << endl;
			MFCConsole::Output(ss.str());
		}
		if (type != 4)//内部缺陷分级
		{
			for (size_t i = 0; i < s.faults.Crazings.size(); i++)
			{

			}
		}

		P_Controller->ImgProcessOverCallBack(grayImg, type);
	}
	else
	{

	}


	//显示结果
	MyStatus = WorkerStatus::Done;
	return;
}





//length < 12000
//取图范围 startFrame 到 endFrame，包括startFrame与endFrame两行
cv::Mat Worker::getPhoto(int startFrame, int length)
{
	int waitLength = WaitTimeMSOut * 1000 / ImgScanner::FrameTimeUS;//获得下降沿后，等待瓷砖离开拍摄区域帧长
	if (length == 0)//采集固定长度
		length = FrameTimeOut * 1000 / ImgScanner::FrameTimeUS;

	int endFrameAbso = startFrame;
	frameIndexAdd(endFrameAbso, length);//绝对最后一帧，到了这一帧不管触发器是否有下降沿都停止采集。while循环break。
	int endFrame = endFrameAbso;


	GetPhotoOn = true;
	bool overtimeflag = false;//超时标记
	//wait capture end
	Sleep(100);
	//循环等待下降沿或采图超时
	while (GetPhotoOn)
	{
		int now = p_e2vbuffer->GetWriteIndex();
		//因超时而退出
		if (
			(endFrameAbso >= startFrame && now >= endFrameAbso) ||
			(endFrameAbso < startFrame && now >= endFrameAbso && now < startFrame)
			)
		{
#ifdef WORKER_DEBUG
			if (1 == 1)
			{
				stringstream ss;
				ss << "worker get endFrameAbso,endFrameAbso =" << endFrameAbso << endl;
				MFCConsole::Output(ss.str());
			}
#endif
			overtimeflag = true;//一直没有获得下降沿信号，标记超时
			break;
		}
		Sleep(2);
	}
	//判断是否获取到了下降沿
	if (GetPhotoOn == false && overtimeflag == false)
	{
		//进入此处说明是外部将GetPhotoOn置0，即触发器下降沿信号结束了采集
		//此处重新计算endFrame
		endFrame = p_e2vbuffer->GetWriteIndex();
#ifdef WORKER_DEBUG
		if (1 == 1)
		{
			stringstream ss;
			ss << "endFrame =" << endFrame << endl;
			MFCConsole::Output(ss.str());
		}
#endif
		frameIndexAdd(endFrame, waitLength);
#ifdef WORKER_DEBUG
		if (1 == 1)
		{
			stringstream ss;
			ss << "endFrame(after frameIndexAdd()) =" << endFrame << endl;
			MFCConsole::Output(ss.str());
		}
#endif


		//TODO::判断每一帧是否是结束。加一个while循环，判断每一帧是否为全黑，全黑说明采集应该结束了。

		while (true)
		{
			//判断是否读够那么多行
			int now = p_e2vbuffer->GetWriteIndex();
			//计数完成而退出
			if (
				endFrame >= startFrame && now >= endFrame ||
				(endFrame < startFrame && now >= endFrame && now < startFrame)
				)
			{
				GetPhotoOn = false;
				break;
			}
			Sleep(2);
		}
	}

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