#pragma once

#include <opencv2\opencv.hpp>
#include "./include/CameraApi.h"

class MVCAM
{
public:
	MVCAM();
	~MVCAM();
	static bool HasInited;
	bool Init();
	bool Release()
	{
		//���˳�����ǰ�ر����(����ʼ�����ǳ���Ҫ�����ֱ�ӹرճ������
		//����ʼ������������п��ܻᱨ�ڴ����)
		CameraUnInit(m_hCamera);
		CameraAlignFree(m_pFrameBuffer);
	};


	//��ʼ�ɼ������ú���������
	void StartCapture();
	//ֹͣ�ɼ�
	void StopCapture();
	//ȡ��֡
	void GetFrame(cv::Mat& img);
	cv::Mat Grub() { cv::Mat m; GetFrame(m); return m; }
private:
	UINT            m_threadID;		//ͼ��ץȡ�̵߳�ID
	HANDLE          m_hDispThread;	//ͼ��ץȡ�̵߳ľ��
	CameraHandle    m_hCamera;		//��������������ͬʱʹ��ʱ���������������	
	BYTE*           m_pFrameBuffer; //���ڽ�ԭʼͼ������ת��ΪRGB�Ļ�����
	tSdkFrameHead   m_sFrInfo;		//���ڱ��浱ǰͼ��֡��֡ͷ��Ϣ

};

