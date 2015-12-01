#include "TiggerWatcherAndGrabber.h"

#include "Consumer.h"

bool Consumer::StartNewProces(cv::Mat img)
{
	if (img.rows == 0 || img.cols == 0)
	{
		printf_globle("拍摄的图片不存在\r\n");
		return false;
	}
	if (isProcessing) return false;

	//将采集到的图像保存一份副本，并灰度化
	originalImg.release();
	originalImg = img.clone();
	if (originalImg.channels() == 3)
		cv::cvtColor(originalImg, grayImg, CV_BGR2GRAY);


	std::thread t_processingThread(std::mem_fn(&Consumer::processingThread), this);
	t_processingThread.detach();

	block = new Block(globle_var::mdi.width, globle_var::mdi.MaxPics);
	return true;
}
bool Consumer::StartNewProces4Calibraion(cv::Mat img)
{
	if (img.rows == 0 || img.cols == 0)
	{
		printf_globle("拍摄的图片不存在\r\n");
		return false;
	}
	if (isProcessing) return false;

	//将采集到的图像保存一份副本，并灰度化
	originalImg.release();
	originalImg = img.clone();
	if (originalImg.channels() == 3)
		cv::cvtColor(originalImg, grayImg, CV_BGR2GRAY);

	block = new Block(globle_var::mdi.width, globle_var::mdi.MaxPics);
	IsCalibration = true;
	processingThread();
	return true;
}

void Consumer::processingThread()
{
	//清空
	faults.Clear();
	isProcessing = true;
 


	stringstream ss;
	ss << GrabbingIndex << " " << "customer：Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	////获取二值化图像
	//Processor::Binaryzation(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << GrabbingIndex << " " << "Binaryzation：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();

	//Processor::Gaussian_Blur(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << GrabbingIndex << " " << "Gaussian_Blur：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();


	double t = 0;
	cv::Mat DetectedImg = originalImg;


	//瓷砖粗定位
	if (1 == 1)//使用if隔绝局部变量
	{
		BlocksDetector *bd = new BlocksDetector(DetectedImg);

		t = (double)cv::getTickCount();
		//BlocksDetector加入判断是否检测到完整瓷砖
		if (!bd->Start() || !bd->StartUP_DOWN(BlocksDetector::Up) || !bd->StartUP_DOWN(BlocksDetector::Down))
		{
			isProcessing = false;
			printf_globle("未检测到瓷砖\r");
			return;
		}
		ss << "bd->Start() && bd->StartUP_DOWN(BlocksDetector::Up) &&	bd->StartUP_DOWN(BlocksDetector::Down)" << endl;


		block->UpLine = bd->UpLine;
		block->DownLine = bd->DownLine;
		block->LeftLine = bd->LeftLine;
		block->RightLine = bd->RightLine;

		delete bd;

		if (!block->Lines2ABCD())
		{
			printf_globle("未检测到完整的瓷砖\r");
			return;
		}

		ss << "block->ABCD()" << endl;
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
		t = (double)cv::getTickCount();
	}
	//若没有初始化定标
	if (!IsCalibration && m == NULL)
	{
		m = new Measurer(block, 300, 600);
		m->ObserveCalibration();
	}
	


	//瓷砖精确定位  &&  崩边检测
	if (2 == 2)
	{
		EdgeDetector *ed = new EdgeDetector(grayImg, block,&faults);
		ed->start();

		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "EdgeDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		t = (double)cv::getTickCount();
		if (faults.BrokenEdges.size() > 0)
		{
			ss << GrabbingIndex << " " << "边缘有缺陷，数量：" << faults.BrokenEdges.size() << endl;
		}
		printf_globle(ss.str());
		ss.str("");
		delete ed;
	}



	//定标分支
	if (IsCalibration)
	{
		isProcessing = false;
		IsCalibration = false;

		t = (double)cv::getTickCount();
		m = new Measurer(block, 300, 600);
		m->ObserveCalibration();
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "标定：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
	}
	else//缺陷检测分支
	{
		t = (double)cv::getTickCount();
		EdgeInnerDetctor eid = EdgeInnerDetctor(grayImg, block, &faults);
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

		if (faults.SomethingBigs.size() > 0)
		{
			ss << GrabbingIndex << " " << "上下边内部有缺陷，数量：" << faults.SomethingBigs.size() << endl;
			printf_globle(ss.str());
			ss.str("");
		}

		ss << GrabbingIndex << " " << "EdgeInnerDetctor：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");



		//瓷砖内部缺陷检测
		if (3 == 3)
		{
			t = (double)cv::getTickCount();
			ss << "瓷砖内部缺陷检测 开始" << endl;
			Pretreatment p;
			p.pretreatment(grayImg, block, &faults);

			ss << GrabbingIndex << " " << "内部有划痕：" << faults.Scratchs.size() << endl;
			ss << GrabbingIndex << " " << "内部有凹点：" << faults.Holes.size() << endl;
			ss << "瓷砖内部缺陷检测 结束" << endl;

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			ss << GrabbingIndex << " " << "内部缺陷检测：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");
		}
	}



	#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			//std::thread t_write1(WriteImg, "samples/00drowDebugDetectLR.jpg", bd->drowDebugDetectLR);
			//t_write1.detach();
			//std::thread t_write2(WriteImg, "samples/01drowDebugDetectUD.jpg", bd->drowDebugDetectUD);
			//t_write2.detach();
			//std::thread t_write3(WriteImg, "samples/02drowDebugResult.jpg", bd->drowDebugResult);
			//t_write3.detach();
		}
	#endif


	ss << GrabbingIndex << " " << "customer：End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");


	isProcessing = false;
	if (hwnd != NULL)
	{
		PostMessage(hwnd, WM_USER + 101, 0, 0);
	}
}