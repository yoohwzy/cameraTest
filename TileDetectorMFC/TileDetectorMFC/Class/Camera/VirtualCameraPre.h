#pragma once

#include "../../stdafx.h"
#include "../../globle_debug.h"
#include "../BufferStorage.h"
#include <opencv2/opencv.hpp>

//@description ��������࣬���ڶ�ȡһ��ͼƬ��Ϊ��Ƭ���ڳ����ѻ�����ʱʹ�á�
//��VirtualCamera�಻ͬ���ǣ�������Ԥ�ȳ�ʼ������Ԥ�������������Ƭ
//�����ļ���ֻ��"XXX.jpg"��������Զ���virtualcameras�ļ����¶�ȡXXX.jpg��
//@author VShawn
//@last modify date 2015-9-30 16:55:05 By VShawn
class VirtualCameraPre
{
public:
	//��ʼ�������������ͼƬ�ļ������ڣ������˳���
	VirtualCameraPre(int frameCount, int width, string imgname, int colorType = RGB);
	~VirtualCameraPre()
	{
		buffer.release();
	};


	void Capture(GrabbingBuffer *gb);

	//���Բɼ���-��������Ƿ�������
	static bool TestCam(string imgname);

	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:
	cv::Mat buffer;

	// ��ͼ��ɫģʽ
	int _colorType = 1;
	int _width = 0;					//ÿ֡���
	const int _frameHeight = 1;		//ÿ֡�߶�
	int _frameCount = 0;			//�ܹ�֡��


	
};