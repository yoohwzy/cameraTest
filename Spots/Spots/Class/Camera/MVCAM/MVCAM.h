#pragma once

#include <opencv2\opencv.hpp>
#include "./include/CameraApi.h"

class MVCAM
{
	//属性配置
public:
	//是否初始化
	bool HasInited;
	//颜色模式，CV_8U CV_8UC3
	int ColorType = CV_8UC3;
	//曝光时间
	double ExposureTimeMS = 50;
	//模拟增益
	double AnalogGain = 2;
public:
	MVCAM();
	~MVCAM();
	bool Init();
	void Release()
	{
		CameraStop(m_hCamera);
		//在退出程序前关闭相机(反初始化，非常重要，如果直接关闭程序而不
		//反初始化相机，程序有可能会报内存错误)
		CameraUnInit(m_hCamera);
		CameraAlignFree(m_pFrameBuffer);
	};


	//开始采集，调用后立即返回
	void StartCapture(){ CameraPlay(m_hCamera); };
	//停止采集
	void StopCapture(){ CameraPause(m_hCamera); };
	//取得帧
	void GetFrame(cv::Mat& img);
	cv::Mat Grub() { cv::Mat m; GetFrame(m); return m; }
private:
	UINT            m_threadID;		//图像抓取线程的ID
	HANDLE          m_hDispThread;	//图像抓取线程的句柄
	CameraHandle    m_hCamera;		//相机句柄，多个相机同时使用时，可以用数组代替	
	BYTE*           m_pFrameBuffer; //用于将原始图像数据转换为RGB的缓冲区
};

