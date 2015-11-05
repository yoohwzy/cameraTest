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
	originalImg = img.clone();

	std::thread t_processingThread(std::mem_fn(&Consumer::processingThread), this);
	t_processingThread.detach();

	return true;
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


	//瓷砖位置检测
	cv::Mat DetectedImg = originalImg.clone();
	BlocksDetector bd = BlocksDetector(DetectedImg);
	double t = (double)cv::getTickCount();

	//BlocksDetector加入判断是否检测到完整瓷砖
	if(bd.Start() && bd.StartUP_DOWN(BlocksDetector::Up) &&	bd.StartUP_DOWN(BlocksDetector::Down))
	{
	}
	else
	{
		printf_globle("未检测到瓷砖\r");
		return;
	}
	if (!bd.ABCD())
	{
		printf_globle("未检测到完整的瓷砖\r");
		return;
	}


	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	t = (double)cv::getTickCount();

	







	//瓷砖崩边检测
	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
	ed.start();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "EdgeDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	t = (double)cv::getTickCount();

	edgeFaults.clear();
	if (ed.Defects.size() > 0)
	{
		for (size_t i = 0; i < ed.Defects.size(); i++)
		{
			edgeFaults.push_back(ed.Defects[i]);
		}
		ss << GrabbingIndex << " " << "边缘有缺陷，数量：" << ed.Defects.size() << endl;
		printf_globle(ss.str());
		ss.str("");
	}


	//瓷砖内部缺陷检测











	#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			//std::thread t_write1(WriteImg, "samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
			//t_write1.detach();
			//std::thread t_write2(WriteImg, "samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
			//t_write2.detach();
			//std::thread t_write3(WriteImg, "samples/02drowDebugResult.jpg", bd.drowDebugResult);
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