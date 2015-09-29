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
	int BufferIndex = 0;//��ǰ���䵽�ڼ���
	bool EndFlag = false;
	int WIDTH = 0;
	const int bufferLength = 20000;//���峤��
private:
	int cameraResolution = 0;
	std::vector<cv::Mat> buffer;
};