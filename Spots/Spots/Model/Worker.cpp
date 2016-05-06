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
	//��ͼ
	cv::Mat grayImg;
	//��ʵ���
	if (p_e2vbuffer != NULL)
	{
		MyStatus = WorkerStatus::Busy;

		int startFrame = p_e2vbuffer->GetWriteIndex();
		int firstFrame = startFrame;//��¼����ʱ��д�к�
#ifdef WORKER_DEBUG
		stringstream ss;
		ss << "startFrame =" << startFrame << endl;
#endif
		//������ʱʱ��
		//������ȴ�һ��ʱ�䣬ש�ߵ�����������ٻ�ȡͼ��
		frameIndexAdd(startFrame, WaitTimeMSIn * 1000 / ImgScanner::FrameTimeUS);
#ifdef WORKER_DEBUG
		ss << "startFrame(after frameIndexAdd()) =" << startFrame << endl;
		MFCConsole::Output(ss.str());
#endif

		//��һ��forѭ����Ϊ�˴���ѭ��ָ��Խ����������startFrame = 19500 + 600 = 100����ʱGetWriteIndex = 19800 > 100 ��Ҫ�ȴ�GetWriteIndexԽ��
		for (size_t i = 0; i < 10; i++)
			if (p_e2vbuffer->GetWriteIndex() > startFrame)
				Sleep(10);
			else
				break;
		//�ڶ���ѭ�� �ȴ�������㵽��ʼ�У�����д��ԭ���Ǽ�������ʱʱ�䣬���ܴ��������δ���㵽startFrame�������
		int nowFrame = p_e2vbuffer->GetWriteIndex();
		while (
			!(
			(startFrame >= firstFrame && (nowFrame > startFrame || nowFrame < firstFrame)) ||//��һ�������startFrameδ��ҳ���������ֿ��ܣ�1.nowFrameҲû��ҳ��Ҫ��nowFrame>startFrame��2.��nowFrame��ҳ��(��nowFrame<firstFrame)�������ж�Ϊͨ����
			(startFrame < firstFrame && (nowFrame > startFrame))//�ڶ��������startFrame��ҳ�ˣ����ж�nowFrame > startFrame����
			)
			)
		{
			Sleep(10);
			nowFrame = p_e2vbuffer->GetWriteIndex();
		}


		image = getPhoto(startFrame, 0);
		grayImg = image;
	}
	//�������
	else if (image.cols > 0)
	{
		MFCConsole::Output("�������ģʽ����\r\n");
		grayImg = image.clone();
	}
	else
	{
		MFCConsole::Output("Ӳ����ʼ��ʧ�ܣ�����������޵�Ƭ�������ú����ԡ�\r\n");
		MessageBox(0, L"Ӳ����ʼ��ʧ�ܣ������ú����ԡ�", L"����", 0);
		return;
	}

	//�����̱߳���ͼƬ��Ӳ��
	P_Controller->ImageGetCallBack(grayImg);


	/********************************************/
	//��ʼͼ����
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);



	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//��ʼͼ����
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
		//����ȱ��ͼ��
		drawFaults(grayImg, s.faults);
		cv::line(grayImg, cv::Point(0, s.p_block->UpLine.k * (0 - s.p_block->UpLine.x0) + s.p_block->UpLine.y0), cv::Point(grayImg.cols, s.p_block->UpLine.k * (grayImg.cols - s.p_block->UpLine.x0) + s.p_block->UpLine.y0), cv::Scalar(0, 0, 255), 1);
		cv::line(grayImg, cv::Point(0, s.p_block->DownLine.k * (0 - s.p_block->DownLine.x0) + s.p_block->DownLine.y0), cv::Point(grayImg.cols, s.p_block->DownLine.k * (grayImg.cols - s.p_block->DownLine.x0) + s.p_block->DownLine.y0), cv::Scalar(0, 255, 255), 1);
		cv::line(grayImg, cv::Point((grayImg.rows - s.p_block->LeftLine.y0) / s.p_block->LeftLine.k + s.p_block->LeftLine.x0, grayImg.rows), cv::Point((0 - s.p_block->LeftLine.y0) / s.p_block->LeftLine.k + s.p_block->LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
		cv::line(grayImg, cv::Point((grayImg.rows - s.p_block->RightLine.y0) / s.p_block->RightLine.k + s.p_block->RightLine.x0, grayImg.rows), cv::Point((0 - s.p_block->RightLine.y0) / s.p_block->RightLine.k + s.p_block->RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);


		int type = 1;//��Ʒ����  1 A�� 2 B�� 3 C�� 4 ���ϸ�
		//ֱ���ж�Ϊ���ϸ�������������type=4

		//��Ʒ�ּ�
		if (type != 4)//���߷ּ�
		{
			double _EDGE_TOTAL_LENGTH = 0;//�����ܳ���
			double _EDGE_TOTAL_DEEP = 0;//���������
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
			ss << "�����ܳ�=" << _EDGE_TOTAL_LENGTH << "mm" << endl;
			ss << "��������=" << _EDGE_TOTAL_DEEP << "mm" << endl;
			MFCConsole::Output(ss.str());
		}
		if (type != 4)//�ڲ�ȱ�ݷּ�
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


	//��ʾ���
	MyStatus = WorkerStatus::Done;
	return;
}





//length < 12000
//ȡͼ��Χ startFrame �� endFrame������startFrame��endFrame����
cv::Mat Worker::getPhoto(int startFrame, int length)
{
	int waitLength = WaitTimeMSOut * 1000 / ImgScanner::FrameTimeUS;//����½��غ󣬵ȴ���ש�뿪��������֡��
	if (length == 0)//�ɼ��̶�����
		length = FrameTimeOut * 1000 / ImgScanner::FrameTimeUS;

	int endFrameAbso = startFrame;
	frameIndexAdd(endFrameAbso, length);//�������һ֡��������һ֡���ܴ������Ƿ����½��ض�ֹͣ�ɼ���whileѭ��break��
	int endFrame = endFrameAbso;


	GetPhotoOn = true;
	bool overtimeflag = false;//��ʱ���
	//wait capture end
	Sleep(100);
	//ѭ���ȴ��½��ػ��ͼ��ʱ
	while (GetPhotoOn)
	{
		int now = p_e2vbuffer->GetWriteIndex();
		//��ʱ���˳�
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
			overtimeflag = true;//һֱû�л���½����źţ���ǳ�ʱ
			break;
		}
		Sleep(2);
	}
	//�ж��Ƿ��ȡ�����½���
	if (GetPhotoOn == false && overtimeflag == false)
	{
		//����˴�˵�����ⲿ��GetPhotoOn��0�����������½����źŽ����˲ɼ�
		//�˴����¼���endFrame
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


		//TODO::�ж�ÿһ֡�Ƿ��ǽ�������һ��whileѭ�����ж�ÿһ֡�Ƿ�Ϊȫ�ڣ�ȫ��˵���ɼ�Ӧ�ý����ˡ�

		while (true)
		{
			//�ж��Ƿ������ô����
			int now = p_e2vbuffer->GetWriteIndex();
			//������ɶ��˳�
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

	//����һû���� ת���㷨ģ����Ԥ����
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
			ss << SN << " ���� " << faults.BrokenEdges.size() << " ������ȱ�ݣ���ɫ�����" << endl;
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
			ss << SN << " ���� " << faults.BrokenCorners.size() << " ������ȱ�ݣ��������" << endl;
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
	//		ss << SN << " ���� " << faults.BrokenEdges.size() << " ������ȱ�ݣ��������" << endl;
	//		MFCConsole::Output( << ss.str();
	//	}

	//	CString str;
	//	str.Format(_T("%d ����%d��EIDȱ�ݣ���ɫ�����\r\n"), SN, faults.SomethingBigs.size());
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
			ss << SN << " ���� " << faults.Scratchs.size() << " ������ȱ�ݣ���ɫ�����" << endl;
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
			ss << SN << " ���� " << faults.Holes.size() << " ������ȱ�ݣ���ɫ�����" << endl;
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
			ss << SN << " ���� " << faults.MarkPens.size() << " ���˹���ǣ���ɫ�����" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.MarkPens.size(); i++)
		{
			cv::rectangle(img, faults.MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
		}
	}
}