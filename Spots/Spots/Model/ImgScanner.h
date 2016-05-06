#pragma once

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h"

#include <thread>

using namespace std;


//���࿪�������̣߳���һ��e2v���sdk�е��̣߳�����ǳ����ȡͼ�̡߳�
class ImgScanner
{
public:
	//��ʼ�����������������������ͼ��������������������Ƶ
	//������Ƶ��Ӧ���������������Ƶ����e2v���������������õ���Ƶ��
	ImgScanner(E2VBuffer *_e2vbuffer, int FrameCountPerSecond = 5000);
	~ImgScanner();

	static int FrameTimeUS;//ÿ������΢��ȡһ֡ = 10^6/FrameCountPerSecond
	bool StartFlag = true;

private:
	E2VBuffer *p_e2vbuffer = NULL;
	E2VCamera *p_e2v = NULL;

	void release();
	void freerun();
};

