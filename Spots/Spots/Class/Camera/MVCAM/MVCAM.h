#pragma once

#include <opencv2\opencv.hpp>
#include "./include/CameraApi.h"

class MVCAM
{
	//��������
public:
	//�Ƿ��ʼ��
	bool HasInited;
	//��ɫģʽ��CV_8U CV_8UC3
	int ColorType = CV_8UC3;
	//�ع�ʱ��
	double ExposureTimeMS = 50;
	//ģ������
	double AnalogGain = 2;
public:
	MVCAM();
	~MVCAM();
	bool Init();
	void Release()
	{
		CameraStop(m_hCamera);
		//���˳�����ǰ�ر����(����ʼ�����ǳ���Ҫ�����ֱ�ӹرճ������
		//����ʼ������������п��ܻᱨ�ڴ����)
		CameraUnInit(m_hCamera);
		CameraAlignFree(m_pFrameBuffer);
	};


	//��ʼ�ɼ������ú���������
	void StartCapture(){ CameraPlay(m_hCamera); };
	//ֹͣ�ɼ�
	void StopCapture(){ CameraPause(m_hCamera); };
	//ȡ��֡
	void GetFrame(cv::Mat& img);
	cv::Mat Grub() { cv::Mat m; GetFrame(m); return m; }
private:
	UINT            m_threadID;		//ͼ��ץȡ�̵߳�ID
	HANDLE          m_hDispThread;	//ͼ��ץȡ�̵߳ľ��
	CameraHandle    m_hCamera;		//��������������ͬʱʹ��ʱ���������������	
	BYTE*           m_pFrameBuffer; //���ڽ�ԭʼͼ������ת��ΪRGB�Ļ�����
};

