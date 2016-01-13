#include "Worker.h"


Worker::Worker(E2VBuffer *_e2vbuffer)
{
	p_e2vbuffer = _e2vbuffer;

	//std::thread t_workThread(std::mem_fn(&Worker::work), this);
	//t_workThread.detach();
	work();
}


Worker::~Worker()
{
}


void Worker::work()
{
	int startFrame = p_e2vbuffer->GetWriteIndex();
	//������ȴ�һ��ʱ�䣬ש�ߵ���������
	image = getPhoto(startFrame, 0);
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
	int endFrame = startFrame + length - 1;//�������һ֡��������һ֡���ܴ������Ƿ����½��ض�ֹͣ�ɼ���


	// ���ѭ�������ҳ��
	if (endFrame >= E2VBuffer::BufferLength)
	{
		endFrame = endFrame - E2VBuffer::BufferLength;
	}

	GetPhotoOn = true;

	//wait capture end
	while (GetPhotoOn)
	{
		//�ж��Ƿ������ô����
		int now = p_e2vbuffer->GetWriteIndex();
		if (endFrame > startFrame  && now >= endFrame)
			break;
		else if (startFrame > endFrame && now >= endFrame && now < length)
			break;
		Sleep(1);
	}

	//GetPhotoOn == flase ��ʾ�Ǵ��������½��ؽ����˲ɼ�
	if (GetPhotoOn == false)
	{
		//TODO::�ж�ÿһ֡�Ƿ��ǽ�������һ��whileѭ�����ж�ÿһ֡�Ƿ�Ϊȫ�ڣ�ȫ��˵���ɼ�Ӧ�ý����ˡ�
		
		endFrame = p_e2vbuffer->GetWriteIndex();
	}
	GetPhotoOn = false;


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
