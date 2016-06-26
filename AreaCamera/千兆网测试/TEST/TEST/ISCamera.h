#pragma once
#ifndef SIMULATE

#include <opencv2\opencv.hpp>
#include <ic/tisudshl.h>
#include <memory>

class ISCamera
{
public:
	ISCamera();
	~ISCamera();
	bool Init();
public:
	//��ʼ�ɼ������ú���������
	virtual void StartCapture();
	//ֹͣ�ɼ�
	virtual void StopCapture();
	//ȡ��֡
	virtual void GetFrame(cv::Mat& img);
public:
	//�Ƿ��ڲɼ���
	virtual bool IsRunning() const;
private:
	bool setupDeviceFromFile(const std::string& devStateFilename = "lastSelectedDeviceState.xml");
private:
	std::unique_ptr<DShowLib::Grabber> grabber;
	smart_ptr<DShowLib::FrameHandlerSink> pHandlerSink;
};
#endif
