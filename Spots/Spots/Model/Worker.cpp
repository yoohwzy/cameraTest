#include "Worker.h"
#include <Controller\Controller.h>

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
		//������ʱʱ��
		frameIndexAdd(startFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
		if (startFrame >= E2VBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			startFrame -= E2VBuffer::BufferLength;
		//������ȴ�һ��ʱ�䣬ש�ߵ�����������ٻ�ȡͼ��
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
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);

	//�����̱߳���ͼƬ
	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//��ʼͼ����
	Synthesizer s = Synthesizer(SN);
	Synthesizer::Status status = s.Run(grayImg);
	
	if (status != Synthesizer::Status::NotFound)
	{
		//����ȱ��ͼ��
		drawFaults(grayImg, s.faults);

		//��ʾ���
		P_Controller->ShowWorkResult(grayImg, 1);
	}

	MyStatus = WorkerStatus::Done;
	return;
}





//length < 12000
//ȡͼ��Χ startFrame �� endFrame������startFrame��endFrame����
cv::Mat Worker::getPhoto(int startFrame, int length)
{
	//�ȴ��������������Ҫ�ȴ�������
	int waitLength = WaitTimeMS * 1000 / ImgScanner::FrameTimeUS;


	if (length == 0)//�ɼ��̶�����
		length = Worker::frameCountsOut;

	int endFrameAbso = startFrame + length + waitLength;//�������һ֡��������һ֡���ܴ������Ƿ����½��ض�ֹͣ�ɼ���whileѭ��break��
	int endFrame = endFrameAbso;


	GetPhotoOn = true;
	//wait capture end
	Sleep(100);
	while (GetPhotoOn)
	{
		//�ж��Ƿ������ô����
		int now = p_e2vbuffer->GetWriteIndex();
		if (now < startFrame)
			now += E2VBuffer::BufferLength;

		//��ʱ���˳�
		if (now >= endFrameAbso)
		{
			break;
		}
		Sleep(2);
	}
	if (GetPhotoOn == false)
	{
		//����˴�˵�����ⲿ��GetPhotoOn��0�����������½����źŽ����˲ɼ�
		//�˴����¼���endFrame
		endFrame = p_e2vbuffer->GetWriteIndex() + waitLength;
		//TODO::�ж�ÿһ֡�Ƿ��ǽ�������һ��whileѭ�����ж�ÿһ֡�Ƿ�Ϊȫ�ڣ�ȫ��˵���ɼ�Ӧ�ý����ˡ�

		while (true)
		{
			//�ж��Ƿ������ô����
			int now = p_e2vbuffer->GetWriteIndex();
			if (now < startFrame)
				now += E2VBuffer::BufferLength;

			//������ɶ��˳�
			if (now >= endFrameAbso)
			{
				GetPhotoOn = false;
				break;
			}
			Sleep(2);
		}
	}

	startFrame += waitLength;

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