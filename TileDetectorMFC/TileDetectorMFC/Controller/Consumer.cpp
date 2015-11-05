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
	ss << GrabbingIndex << " " << "customer��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	////��ȡ��ֵ��ͼ��
	//Processor::Binaryzation(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << GrabbingIndex << " " << "Binaryzation��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();

	//Processor::Gaussian_Blur(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << GrabbingIndex << " " << "Gaussian_Blur��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();


	//��ש��Ե���
	cv::Mat DetectedImg = originalImg.clone();
	BlocksDetector bd = BlocksDetector(DetectedImg);
	double t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	bd.ABCD();
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "BlocksDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	t = (double)cv::getTickCount();

	//TODO:BlocksDetector�����ж��Ƿ��⵽������ש




	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
	ed.start();

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "EdgeDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
	t = (double)cv::getTickCount();
	if (ed.Defects.size() > 0)
	{
		ss << GrabbingIndex << " " << "��Ե��ȱ�ݣ�������" << ed.Defects.size() << endl;
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
	//	//��ש��Եȱ�ݼ��
	//	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
	//	if (ed.Defects.size() > 0)
	//	{
	//		ss << GrabbingIndex << " " << "��Ե��ȱ��" << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//	}
	//	else
	//	{
	//		Measurer m = Measurer();
	//		m.CaculteSize(&bd);
	//	}
	//	BlockHoughDetector bhd = BlockHoughDetector(globle_var::s.NowBufferImg);


	ss << GrabbingIndex << " " << "customer��End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");


	isProcessing = false;
	if (hwnd != NULL)
	{
		PostMessage(hwnd, WM_USER + 101, 0, 0);
	}
}