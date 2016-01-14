#include "TiggerWatcherAndGrabber.h"

#include "Consumer.h"


Consumer::Consumer(HWND _hwnd){
	hwnd = _hwnd;
	//_hwnd = NULL;
};
bool Consumer::StartNewProces(cv::Mat img)
{
	printf_globle("Consumer StartNewProces\r\n");
	if (img.rows == 0 || img.cols == 0)
	{
		printf_globle("拍摄的图片不存在\r\n");
		return false;
	}

	//将采集到的图像保存一份副本，并灰度化
	//originalImg.release();
	originalImg = img.clone();
	if (originalImg.channels() == 3)
	{
		cv::cvtColor(originalImg, grayImg, CV_BGR2GRAY);
	}


	p_block = new Block(globle_var::Width, globle_var::FrameCount);
	std::thread t_processingThread(std::mem_fn(&Consumer::processingThread), this);
	t_processingThread.detach();

	return true;
}
bool Consumer::StartNewProces4Calibraion(cv::Mat img)
{
	if (img.rows == 0 || img.cols == 0)
	{
		printf_globle("拍摄的图片不存在\r\n");
		return false;
	}
	//if (IsProcessing) return false;

	//将采集到的图像保存一份副本，并灰度化
	originalImg.release();
	originalImg = img.clone();
	if (originalImg.channels() == 3)
		cv::cvtColor(originalImg, grayImg, CV_BGR2GRAY);

	p_block = new Block(globle_var::Width, globle_var::FrameCount);
	IsCalibration = true;
	processingThread();
	return true;
}

void Consumer::processingThread()
{
	double t_consumer = (double)cv::getTickCount();



	//faults.Clear();
	IsProcessing = true;
 
	//IsProcessing = false;
	//sendMsg(0, 0);
	//return;

	stringstream ss;
	ss << GrabbingIndex << " " << "customer：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");


	////获取二值化图像
	if (grayImg.cols == 0)
	{
		printf_globle("Img Empty!\r\n");
	}
	cv::Mat threshodImg;
	cv::threshold(grayImg, threshodImg, ConsumerThreshodLow, 255, 0);
	cv::Mat threshodImgHight;
	cv::threshold(grayImg, threshodImgHight, ConsumerThreshodHight, 255, 0);


	double t = 0;

	//瓷砖粗定位
	if (1 == 1)//使用if隔绝局部变量
	{
		BlocksDetector bd = BlocksDetector(threshodImg, ConsumerLedStartX, ConsumerLedEndX);

		t = (double)cv::getTickCount();
		//BlocksDetector加入判断是否检测到完整瓷砖
		bool left_right = bd.Start();
		bool up = bd.StartUP_DOWN(BlocksDetector::Up);
		bool down = bd.StartUP_DOWN(BlocksDetector::Down);
		if (!left_right ||
			!up ||
			!down)
		{
			if (!left_right)
				printf_globle("左右未找到\r\n");
			else if (!up)
				printf_globle("上未找到\r\n");
			else if (!down)
				printf_globle("下未找到\r\n");
			IsProcessing = false;
			sendMsg(0, 1);
			printf_globle(Consumer::GetErrorDescription(1));
			return;
		}
		//ss << "bd.Start() && bd.StartUP_DOWN(BlocksDetector::Up) &&	bd.StartUP_DOWN(BlocksDetector::Down)" << endl;


		p_block->UpLine = bd.UpLine;
		p_block->DownLine = bd.DownLine;
		p_block->LeftLine = bd.LeftLine;
		p_block->RightLine = bd.RightLine;


		//判断各点坐标是否小于0或大于宽度
		if (!p_block->Lines2ABCD())
		{
			printf_globle("瓷砖有边角位于图片外\r\n");
			p_block->ABCDAdjust();
		}

		//ss << "p_block->ABCD()" << endl;
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
		t = (double)cv::getTickCount();
	}



	//若没有则初始化定标
	//if (!IsCalibration && p_measurer == NULL)
	//{
	//	p_measurer = new Measurer(p_block, 300, 600);
	//	p_measurer->ObserveCalibration();
	//}
	






	//瓷砖精确定位  &&  崩边检测
	if (2 == 2)
	{
		Block tmpb = Block(p_block->imageWidth, p_block->imageHeight);
		tmpb.LeftLine = p_block->LeftLine;
		tmpb.RightLine = p_block->RightLine;
		tmpb.UpLine = p_block->UpLine;
		tmpb.DownLine = p_block->DownLine;
		tmpb.Lines2ABCD();
		EdgeDetector ed = EdgeDetector(grayImg, &tmpb, &faults);
		//EdgeDetector ed = EdgeDetector(grayImg, p_block, &faults);
		//ed.ThreshodImgHigh = threshodImgHight;
		//ed.ThreshodImgLow = threshodImg;
		//ed.ThreshodHigh = ConsumerThreshodHight;
		//ed.ThreshodLow = ConsumerThreshodLow;
		//ed.grayImg = grayImg;
		ed.start();

		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "EdgeDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		t = (double)cv::getTickCount();
		if (faults.BrokenEdges.size() > 0)
		{
			ss << GrabbingIndex << " " << "边缘有缺陷，数量：" << faults.BrokenEdges.size() << endl;
		}
		printf_globle(ss.str());
		ss.str("");
	}



	//定标分支
	if (IsCalibration)
	{
		IsProcessing = false;
		IsCalibration = false;

		t = (double)cv::getTickCount();
		p_measurer = new Measurer(p_block, 300, 600);
		p_measurer->ObserveCalibration();
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "标定：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
	}
	else//缺陷检测分支
	{
		//t = (double)cv::getTickCount();
		//EdgeInnerDetctor eid = EdgeInnerDetctor(grayImg, p_block, &faults);
		//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

		//if (faults.SomethingBigs.size() > 0)
		//{
		//	ss << GrabbingIndex << " " << "上下边内部有缺陷，数量：" << faults.SomethingBigs.size() << endl;
		//	printf_globle(ss.str());
		//	ss.str("");
		//}

		//ss << GrabbingIndex << " " << "EdgeInnerDetctor：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		//printf_globle(ss.str());
		//ss.str("");



		//瓷砖内部缺陷检测
		if (3 == 3)
		{
			t = (double)cv::getTickCount();
			ss << "瓷砖内部缺陷检测 开始" << endl;
			Calibration c;
			c.C_pretreatment(grayImg, p_block, &scales);
			Pretreatment p;
			p.pretreatment(grayImg, p_block, &faults, &scales);

			ss << GrabbingIndex << " " << "内部有划痕：" << faults.Scratchs.size() << endl;
			ss << GrabbingIndex << " " << "内部有凹点：" << faults.Holes.size() << endl;
			ss << "瓷砖内部缺陷检测 结束" << endl;

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			ss << GrabbingIndex << " " << "内部缺陷检测：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");
		}
	}

	t_consumer = ((double)cv::getTickCount() - t_consumer) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "customer：" << t_consumer << " End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");



	IsProcessing = false;
	sendMsg(0, 0);
}
void Consumer::sendMsg(int type, int subtype)
{
	if (hwnd != NULL)
	{
		PostMessage(hwnd, WM_USER + 101, type, subtype);
	}
}
