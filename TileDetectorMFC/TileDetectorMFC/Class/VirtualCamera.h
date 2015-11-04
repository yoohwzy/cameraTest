#pragma once

#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "MicroDisplay\MicroDisplayInit.h"
#include "BufferStorage.h"

//@description ��������࣬���ڶ�ȡһ��ͼƬ��Ϊ��Ƭ���ڳ����ѻ�����ʱʹ�á�
//@author VShawn
//@last modify date 2015-9-30 16:55:05 By VShawn
class VirtualCamera
{
public:
	VirtualCamera();
	VirtualCamera(MicroDisplayInit& mdi);
	//��ʼ�������������ͼƬ�ļ������ڣ������˳���
	VirtualCamera(MicroDisplayInit& mdi,string imgname);
	~VirtualCamera();
	//����ֹͣ��־
	bool EndFlag = false;
	//ѭ����ͼ ֱ���ɹ�Ϊֹ
	int FreeRunning(MicroDisplayInit& mdi, BufferStorage& s);
	cv::Mat buffer;
private:
	int WIDTH = 0;
	int BufferLength;//���峤��
	cv::Mat GetNext();
	int BufferIndex = 0;//��ǰ���䵽�ڼ���
};