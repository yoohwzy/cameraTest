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
		printf_globle("�����ͼƬ������\r\n");
		return false;
	}

	//���ɼ�����ͼ�񱣴�һ�ݸ��������ҶȻ�
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
		printf_globle("�����ͼƬ������\r\n");
		return false;
	}
	//if (IsProcessing) return false;

	//���ɼ�����ͼ�񱣴�һ�ݸ��������ҶȻ�
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
	ss << GrabbingIndex << " " << "customer��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");


	////��ȡ��ֵ��ͼ��
	if (grayImg.cols == 0)
	{
		printf_globle("Img Empty!\r\n");
	}
	cv::Mat threshodImg;
	cv::threshold(grayImg, threshodImg, ConsumerThreshodLow, 255, 0);
	cv::Mat threshodImgHight;
	cv::threshold(grayImg, threshodImgHight, ConsumerThreshodHight, 255, 0);


	double t = 0;




	//��û�����ʼ������
	//if (!IsCalibration && p_measurer == NULL)
	//{
	//	p_measurer = new Measurer(p_block, 300, 600);
	//	p_measurer->ObserveCalibration();
	//}
	






	//��ש��ȷ��λ  &&  ���߼��
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

		ed.distance_threld = distance_threld;
		ed.Edge_threld = Edge_threld;
		ed.start();

		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "EdgeDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		t = (double)cv::getTickCount();
		if (faults.BrokenEdges.size() > 0)
		{
			ss << GrabbingIndex << " " << "��Ե��ȱ�ݣ�������" << faults.BrokenEdges.size() << endl;
		}
		printf_globle(ss.str());
		ss.str("");
	}



	//�����֧
	if (IsCalibration)
	{
		IsProcessing = false;
		IsCalibration = false;

		t = (double)cv::getTickCount();
		p_measurer = new Measurer(p_block, 300, 600);
		p_measurer->ObserveCalibration();
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "�궨��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
	}
	else//ȱ�ݼ���֧
	{
		//t = (double)cv::getTickCount();
		//EdgeInnerDetctor eid = EdgeInnerDetctor(grayImg, p_block, &faults);
		//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

		//if (faults.SomethingBigs.size() > 0)
		//{
		//	ss << GrabbingIndex << " " << "���±��ڲ���ȱ�ݣ�������" << faults.SomethingBigs.size() << endl;
		//	printf_globle(ss.str());
		//	ss.str("");
		//}

		//ss << GrabbingIndex << " " << "EdgeInnerDetctor��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		//printf_globle(ss.str());
		//ss.str("");



		//��ש�ڲ�ȱ�ݼ��
		if (3 == 3)
		{
			t = (double)cv::getTickCount();
			ss << "��ש�ڲ�ȱ�ݼ�� ��ʼ" << endl;
			Pretreatment p;
			p.pretreatment(grayImg, p_block, &faults);

			ss << GrabbingIndex << " " << "�ڲ��л��ۣ�" << faults.Scratchs.size() << endl;
			ss << GrabbingIndex << " " << "�ڲ��а��㣺" << faults.Holes.size() << endl;
			ss << "��ש�ڲ�ȱ�ݼ�� ����" << endl;

			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			ss << GrabbingIndex << " " << "�ڲ�ȱ�ݼ�⣺" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");
		}
	}

	t_consumer = ((double)cv::getTickCount() - t_consumer) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "customer��" << t_consumer << " End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
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
