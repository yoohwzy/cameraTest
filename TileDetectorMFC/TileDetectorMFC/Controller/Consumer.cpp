#include "TiggerWatcherAndGrabber.h"

#include "Consumer.h"

bool Consumer::StartNewProces(cv::Mat img)
{
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
	ss << GrabbingIndex << " " << "customer£ºStart at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	////»ñÈ¡¶þÖµ»¯Í¼Ïñ
	//Processor::Binaryzation(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << GrabbingIndex << " " << "Binaryzation£º" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();

	//Processor::Gaussian_Blur(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << GrabbingIndex << " " << "Gaussian_Blur£º" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();


	//´É×©±ßÔµ¼ì²â
	cv::Mat DetectedImg = originalImg.clone();
	BlocksDetector bd = BlocksDetector(DetectedImg);
	double t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	bd.ABCD();
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "BlocksDetector£º" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	t = (double)cv::getTickCount();

	//TODO:BlocksDetector¼ÓÈëÅÐ¶ÏÊÇ·ñ¼ì²âµ½ÍêÕû´É×©




	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
	ed.start();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "EdgeDetector£º" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	t = (double)cv::getTickCount();
	if (ed.Defects.size() > 0)
	{
		ss << GrabbingIndex << " " << "±ßÔµÓÐÈ±ÏÝ£¬ÊýÁ¿£º" << ed.Defects.size() << endl;
		printf_globle(ss.str());
		ss.str("");
	}


	//#ifdef OUTPUT_DEBUG_INFO
	//	if (OUTPUT_DEBUG_INFO)
	//	{
	//		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
	//		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
	//		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
	//	}
	//#endif
	//
	//	//´É×©±ßÔµÈ±ÏÝ¼ì²â
	//	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
	//	if (ed.Defects.size() > 0)
	//	{
	//		ss << GrabbingIndex << " " << "±ßÔµÓÐÈ±ÏÝ" << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//	}
	//	else
	//	{
	//		Measurer m = Measurer();
	//		m.CaculteSize(&bd);
	//	}
	//	BlockHoughDetector bhd = BlockHoughDetector(globle_var::s.NowBufferImg);


	ss << GrabbingIndex << " " << "customer£ºEnd at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");


	isProcessing = false;
	if (hwnd != NULL)
	{
		PostMessage(hwnd, WM_USER + 101, 0, 0);
	}
}