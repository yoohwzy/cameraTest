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
	originalImg.release();
	originalImg = img.clone();

	std::thread t_processingThread(std::mem_fn(&Consumer::processingThread), this);
	t_processingThread.detach();

	block = new Block(globle_var::mdi.width, globle_var::mdi.MaxPics);
	return true;
}
void Consumer::Process4Calibraion()
{
	IsCalibration = true;
	processingThread();
}

void Consumer::processingThread()
{
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
	//瓷砖位置检测
	if (1 == 1)//使用if隔绝局部变量
	{
		printf_globle("cv::Mat DetectedImg = originalImg.clone()\r");
		BlocksDetector *bd = new BlocksDetector(DetectedImg);
		printf_globle("BlocksDetector bd = BlocksDetector(DetectedImg)\r");

		t = (double)cv::getTickCount();
		//BlocksDetector加入判断是否检测到完整瓷砖
		if (!bd->Start() || !bd->StartUP_DOWN(BlocksDetector::Up) || !bd->StartUP_DOWN(BlocksDetector::Down))
		{
			printf_globle("未检测到瓷砖\r");
			return;
		}
		printf_globle("bd->Start() && bd->StartUP_DOWN(BlocksDetector::Up) &&	bd->StartUP_DOWN(BlocksDetector::Down)\r");


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

		printf_globle("block->ABCD()\r");
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
		t = (double)cv::getTickCount();
	}




	//瓷砖崩边检测
	if (2 == 2)
	{
		EdgeDetector *ed = new EdgeDetector(DetectedImg, block);
		ed->start();

		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "EdgeDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
		t = (double)cv::getTickCount();

		EdgeFaults.clear();
		if (ed->Defects.size() > 0)
		{
			for (size_t i = 0; i < ed->Defects.size(); i++)
			{
				EdgeFaults.push_back(ed->Defects[i]);
			}
			ss << GrabbingIndex << " " << "边缘有缺陷，数量：" << ed->Defects.size() << endl;
			printf_globle(ss.str());
			ss.str("");
		}

		delete ed;
	}

	//定标分支
	if (IsCalibration)
	{
		isProcessing = false;
		IsCalibration = false;

		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		m = new Measurer(block, 300, 600);
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "标定：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
	}
	else//内部缺陷检测分支
	{

		//瓷砖内部缺陷检测
		//if (3 == 3)
		//{
		//	printf_globle("瓷砖内部缺陷检测 开始\r");
		//	Pretreatment *p = new Pretreatment();
		//	vector<cv::Point> lp = p->pretreatment(DetectedImg);
		//	if (lp.size() > 0)
		//	{
		//		ss << GrabbingIndex << " " << "内部有缺陷，数量：" << lp.size() << endl;
		//		printf_globle(ss.str());
		//		ss.str("");
		//	}
		//	printf_globle("瓷砖内部缺陷检测 结束\r");
		//	delete p;
		//}

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