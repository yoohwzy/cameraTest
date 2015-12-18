#pragma once

#include "../../stdafx.h"
#include "../../globle_debug.h"
#include "../BufferStorage.h"
#include <opencv2/opencv.hpp>

//@description ��������࣬���ڶ�ȡһ��ͼƬ��Ϊ��Ƭ���ڳ����ѻ�����ʱʹ�á�
//�����ļ���ֻ��"XXX.jpg"��������Զ���virtualcameras�ļ����¶�ȡXXX.jpg��
//@author VShawn
//@last modify date 2015-9-30 16:55:05 By VShawn
class VirtualCamera
{
public:
	//��ʼ�������������ͼƬ�ļ������ڣ������˳���
	VirtualCamera(GrabbingBuffer *gb, int frameCount, int width, string imgname, int colorType = RGB);
	~VirtualCamera()
	{
		buffer.release();
		p_gb = NULL;
	};


	void Capture();

	//���Բɼ���-��������Ƿ�������
	static bool TestCam(string imgname);

	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:
	cv::Mat buffer;
	GrabbingBuffer *p_gb;

	// ��ͼ��ɫģʽ
	int _colorType = 1;
	int _width = 0;					//ÿ֡���
	const int _frameHeight = 1;		//ÿ֡�߶�
	int _frameCount = 0;			//�ܹ�֡��


	
};