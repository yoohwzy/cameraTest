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

	// 为true时采集图像，转为false时结束采集
	// 仅当length = 0时有效，用于确定采集何时结束
	bool GetPhotoOn = false;

	int WaitTimeMS = 50;//等待瓷砖进入拍摄区的时间
	cv::Mat image;
private:
	E2VBuffer *p_e2vbuffer = NULL;


	// 直接读取N张图片
	void work();
	// 从循环缓存中取出图片
	// 开始采集的行，采集行数，为0时等待下降沿或采集行数超过frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);
	static const int frameCountsOut = 12000;
	
	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VBuffer::BufferLength)
			oldFrame -= E2VBuffer::BufferLength;
	}
};