#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h"
#include "BufferStorage.h"

class VirtualCamera
{
public:
	VirtualCamera();
	VirtualCamera(MicroDisplayInit& mdi);
	VirtualCamera(MicroDisplayInit& mdi,string imgname);
	~VirtualCamera();
	//����ֹͣ��־
	bool EndFlag = false;
	//ѭ����ͼ ֱ���ɹ�Ϊֹ
	int FreeRunning(MicroDisplayInit& mdi, BufferStorage& s);
private:
	int WIDTH = 0;
	int BufferLength;//���峤��
	cv::Mat buffer;
	cv::Mat GetNext();
	int BufferIndex = 0;//��ǰ���䵽�ڼ���
};