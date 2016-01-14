#include "Worker.h"

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
	MyStatus = WorkerStatus::Busy;

	int startFrame = p_e2vbuffer->GetWriteIndex();
	//������ʱʱ��
	frameIndexAdd(startFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
	if (startFrame >= E2VBuffer::BufferLength)
		startFrame -= E2VBuffer::BufferLength;


	//������ȴ�һ��ʱ�䣬ש�ߵ���������
	image = getPhoto(startFrame, 0);

	MyStatus = WorkerStatus::Done;
	return;
}









//length < 12000
//ȡͼ��Χ startFrame �� endFrame������startFrame��endFrame����
cv::Mat Worker::getPhoto(int startFrame, int length)
{
	//TODO::����һû����

	bool staticLengthModel = length > 0;
	if (!staticLengthModel)//�ɼ��̶�����
		length = Worker::frameCountsOut;

	int width = p_e2vbuffer->Buffer.cols;
	int endFrameAbso = startFrame;//�������һ֡��������һ֡���ܴ������Ƿ����½��ض�ֹͣ�ɼ���whileѭ��break��
	frameIndexAdd(endFrameAbso, length - 1);
	int endFrame = endFrameAbso;

	GetPhotoOn = true;
	//wait capture end
	Sleep(100);
	while (GetPhotoOn)
	{
		//�ж��Ƿ������ô����
		int now = p_e2vbuffer->GetWriteIndex();
		//��ʱ���˳�
		if (
			(endFrameAbso > startFrame  && now >= endFrameAbso) ||
			(startFrame > endFrameAbso && now >= endFrameAbso && now < length)
			)
		{
			break;
		}
		Sleep(2);
	}
	if (GetPhotoOn == false)
	{
		//����˴�˵�����ⲿ��GetPhotoOn��0�����������½����źŽ����˲ɼ�
		//�˴����¼���endFrame
		endFrame = p_e2vbuffer->GetWriteIndex();
		frameIndexAdd(endFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
		//TODO::�ж�ÿһ֡�Ƿ��ǽ�������һ��whileѭ�����ж�ÿһ֡�Ƿ�Ϊȫ�ڣ�ȫ��˵���ɼ�Ӧ�ý����ˡ�		
	}
	while (true)
	{
		//�ж��Ƿ������ô����
		int now = p_e2vbuffer->GetWriteIndex();
		//������ɶ��˳�
		if (
			(endFrame > startFrame  && now >= endFrame) ||
			(startFrame > endFrame && now >= endFrame && now < length)
			)
		{
			GetPhotoOn = false;
			break;
		}
		Sleep(2);
	}



	//��ȡͼ��
	if (endFrame > startFrame)
	{
		length = endFrame - startFrame + 1;
		cv::Mat result = p_e2vbuffer->Buffer(cv::Rect(0, startFrame, width, length)).clone();
		result *= 3;
		return result;
	}
	else
	{
		length = (E2VBuffer::BufferLength - startFrame) + (endFrame + 1);
		cv::Mat result(length, width, p_e2vbuffer->GetColorType(), cv::Scalar(0, 0, 0));
		cv::Mat roi1 = result(cv::Rect(0, 0, width, E2VBuffer::BufferLength - startFrame));
		cv::Mat roi2 = result(cv::Rect(0, E2VBuffer::BufferLength - startFrame, width, endFrame + 1));

		p_e2vbuffer->Buffer(cv::Rect(0, startFrame, width, E2VBuffer::BufferLength - startFrame)).copyTo(roi1);	//debug 2΢�� release 1.5΢��
		p_e2vbuffer->Buffer(cv::Rect(0, 0, width, endFrame + 1)).copyTo(roi2);


		result *= 3;
		return result;
	}
}
