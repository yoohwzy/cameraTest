#pragma once
#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h"
#include "ImgScanner.h"
#include <thread>

class Worker
{
public:
	Worker(E2VBuffer *_e2vbuffer);
	~Worker();

	enum WorkerStatus{
		Free,
		Busy,
		Done
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;

	// Ϊtrueʱ�ɼ�ͼ��תΪfalseʱ�����ɼ�
	// ����length = 0ʱ��Ч������ȷ���ɼ���ʱ����
	bool GetPhotoOn = false;

	int WaitTimeMS = 50;//�ȴ���ש������������ʱ��
	cv::Mat image;
private:
	E2VBuffer *p_e2vbuffer = NULL;


	// ֱ�Ӷ�ȡN��ͼƬ
	void work();
	// ��ѭ��������ȡ��ͼƬ
	// ��ʼ�ɼ����У��ɼ�������Ϊ0ʱ�ȴ��½��ػ�ɼ���������frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);
	static const int frameCountsOut = 12000;
	
	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VBuffer::BufferLength)
			oldFrame -= E2VBuffer::BufferLength;
	}
};