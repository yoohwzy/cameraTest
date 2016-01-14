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
	//加入延时时间
	frameIndexAdd(startFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
	if (startFrame >= E2VBuffer::BufferLength)
		startFrame -= E2VBuffer::BufferLength;


	//触发后等待一段时间，砖走到拍摄区域
	image = getPhoto(startFrame, 0);

	MyStatus = WorkerStatus::Done;
	return;
}









//length < 12000
//取图范围 startFrame 到 endFrame，包括startFrame与endFrame两行
cv::Mat Worker::getPhoto(int startFrame, int length)
{
	//TODO::三合一没有做

	bool staticLengthModel = length > 0;
	if (!staticLengthModel)//采集固定长度
		length = Worker::frameCountsOut;

	int width = p_e2vbuffer->Buffer.cols;
	int endFrameAbso = startFrame;//绝对最后一帧，到了这一帧不管触发器是否有下降沿都停止采集。while循环break。
	frameIndexAdd(endFrameAbso, length - 1);
	int endFrame = endFrameAbso;

	GetPhotoOn = true;
	//wait capture end
	Sleep(100);
	while (GetPhotoOn)
	{
		//判断是否读够那么多行
		int now = p_e2vbuffer->GetWriteIndex();
		//因超时而退出
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
		//进入此处说明是外部将GetPhotoOn置0，即触发器下降沿信号结束了采集
		//此处重新计算endFrame
		endFrame = p_e2vbuffer->GetWriteIndex();
		frameIndexAdd(endFrame, WaitTimeMS * 1000 / ImgScanner::FrameTimeUS);
		//TODO::判断每一帧是否是结束。加一个while循环，判断每一帧是否为全黑，全黑说明采集应该结束了。		
	}
	while (true)
	{
		//判断是否读够那么多行
		int now = p_e2vbuffer->GetWriteIndex();
		//计数完成而退出
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



	//提取图像
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

		p_e2vbuffer->Buffer(cv::Rect(0, startFrame, width, E2VBuffer::BufferLength - startFrame)).copyTo(roi1);	//debug 2微秒 release 1.5微秒
		p_e2vbuffer->Buffer(cv::Rect(0, 0, width, endFrame + 1)).copyTo(roi2);


		result *= 3;
		return result;
	}
}
