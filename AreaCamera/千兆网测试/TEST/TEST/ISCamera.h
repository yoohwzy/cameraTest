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
	//开始采集，调用后立即返回
	virtual void StartCapture();
	//停止采集
	virtual void StopCapture();
	//取得帧
	virtual void GetFrame(cv::Mat& img);
public:
	//是否在采集中
	virtual bool IsRunning() const;
private:
	bool setupDeviceFromFile(const std::string& devStateFilename = "lastSelectedDeviceState.xml");
private:
	std::unique_ptr<DShowLib::Grabber> grabber;
	smart_ptr<DShowLib::FrameHandlerSink> pHandlerSink;
};
#endif
