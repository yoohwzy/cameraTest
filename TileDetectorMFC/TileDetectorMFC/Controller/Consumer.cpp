#include "TiggerWatcherAndGrabber.h"

#include "Consumer.h"


Consumer::Consumer(HWND _hwnd){
	hwnd = _hwnd;
	_hwnd = NULL;
};
bool Consumer::StartNewProces(cv::Mat img)
{
	if (img.rows == 0 || img.cols == 0)
	{
		printf_globle("�����ͼƬ������\r\n");
		return false;
	}
	if (IsProcessing) return false;

	//���ɼ�����ͼ�񱣴�һ�ݸ��������ҶȻ�
	originalImg.release();
	originalImg = img.clone();
	if (originalImg.channels() == 3)
		cv::cvtColor(originalImg, grayImg, CV_BGR2GRAY);


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
	if (IsProcessing) return false;

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



	//���
	faults.Clear();
	IsProcessing = true;
 
	//IsProcessing = false;
	//sendMsg(0, 0);
	//return;

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


	double t = 0;

	//��ש�ֶ�λ
	if (1 == 1)//ʹ��if�����ֲ�����
	{
		BlocksDetector bd = BlocksDetector(grayImg);

		t = (double)cv::getTickCount();
		//BlocksDetector�����ж��Ƿ��⵽������ש
		if (!bd.Start() ||
			!bd.StartUP_DOWN(BlocksDetector::Up) || 
			!bd.StartUP_DOWN(BlocksDetector::Down))
		{
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


		//�жϸ��������Ƿ�С��0����ڿ��
		if (!p_block->Lines2ABCD())
		{
			line(originalImg, p_block->A, p_block->B, cv::Scalar(0, 255, 0), 5);
			line(originalImg, p_block->A, p_block->D, cv::Scalar(255, 0, 0), 5);
			line(originalImg, p_block->C, p_block->B, cv::Scalar(255, 255, 0), 5);
			line(originalImg, p_block->C, p_block->D, cv::Scalar(0, 255, 255), 5);
			IsProcessing = false;
			sendMsg(0, 2);
			printf_globle(Consumer::GetErrorDescription(2));
			return;
		}

		//ss << "p_block->ABCD()" << endl;
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << GrabbingIndex << " " << "BlocksDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");
		t = (double)cv::getTickCount();
	}



	//��û�����ʼ������
	//if (!IsCalibration && p_measurer == NULL)
	//{
	//	p_measurer = new Measurer(p_block, 300, 600);
	//	p_measurer->ObserveCalibration();
	//}
	






	//��ש��ȷ��λ  &&  ���߼��
	//if (2 == 2)
	//{
	//	EdgeDetector ed = EdgeDetector(grayImg, p_block, &faults);
	//	ed.start();

	//	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//	ss << GrabbingIndex << " " << "EdgeDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//	t = (double)cv::getTickCount();
	//	if (faults.BrokenEdges.size() > 0)
	//	{
	//		ss << GrabbingIndex << " " << "��Ե��ȱ�ݣ�������" << faults.BrokenEdges.size() << endl;
	//	}
	//	printf_globle(ss.str());
	//	ss.str("");
	//}

	//IsProcessing = false;
	//sendMsg(0, 0);
	//return;


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
