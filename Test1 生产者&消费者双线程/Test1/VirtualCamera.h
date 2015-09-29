#pragma once

#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

using namespace std;

class VirtualCamera
{
public:
	VirtualCamera();
	~VirtualCamera();
	cv::Mat GetNext();
	int BufferIndex = 0;//当前传输到第几线
	bool EndFlag = false;
	int WIDTH = 0;
	const int bufferLength = 20000;//缓冲长度
private:
	int cameraResolution = 0;
	std::vector<cv::Mat> buffer;
};