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
		if (startFrame >= E2VBuffer::BufferLength)
			startFrame -= E2VBuffer::BufferLength;
		//������ȴ�һ��ʱ�䣬ש�ߵ���������
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


	//��ʼͼ����
	//Synthesizer s = Synthesizer(0);
	//Synthesizer::Status status  = s.Run(image);
	
	//��ʾ���
	P_Controller->ShowWorkResult(image);
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

	//TODO::����һû����
	return p_e2vbuffer->GetImage(startFrame, endFrame);
}




////length < 12000
////ȡͼ��Χ startFrame �� endFrame������startFrame��endFrame����
//cv::Mat Worker::getPhoto(int startFrame, int length)
//{
//	if (length == 0)//�ɼ��̶�����
//		length = Worker::frameCountsOut;
//
//	int endFrameAbso = startFrame;//�������һ֡��������һ֡���ܴ������Ƿ����½��ض�ֹͣ�ɼ���whileѭ��break��
//	frameIndexAdd(endFrameAbso, length - 1);
//	int endFrame = endFrameAbso;
//
//	GetPhotoOn = true;
//	//wait capture end
//	Sleep(100);
//	while (GetPhotoOn)
//	{
//		//�ж��Ƿ������ô����
//		int now = p_e2vbuffer->GetWriteIndex();
//		//��ʱ���˳�
//		if (
//			(endFrameAbso > startFrame  && now >= endFrameAbso) ||
//			(startFrame > endFrameAbso && now >= endFrameAbso && now < length)
//			)
//		{
//			break;
//		}
//		Sleep(2);
//	}
//	if (GetPhotoOn == false)
//	{
//		//����˴�˵�����ⲿ��GetPhotoOn��0�����������½����źŽ����˲ɼ�
//		//�˴����¼���endFrame
//		endFrame = p_e2vbuffer->GetWriteIndex();
//		frameIndexAdd(endFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
//		//TODO::�ж�ÿһ֡�Ƿ��ǽ�������һ��whileѭ�����ж�ÿһ֡�Ƿ�Ϊȫ�ڣ�ȫ��˵���ɼ�Ӧ�ý����ˡ�		
//	}
//	while (true)
//	{
//		//�ж��Ƿ������ô����
//		int now = p_e2vbuffer->GetWriteIndex();
//		//������ɶ��˳�
//		if (
//			(endFrame > startFrame  && now >= endFrame) ||
//			(startFrame > endFrame && now >= endFrame && now < length)
//			)
//		{
//			GetPhotoOn = false;
//			break;
//		}
//		Sleep(2);
//	}
//
//
//
//	//TODO::����һû����
//	return p_e2vbuffer->GetImage(startFrame, endFrame);
//}
