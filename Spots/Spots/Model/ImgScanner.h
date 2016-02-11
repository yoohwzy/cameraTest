#pragma once

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h"

#include <thread>

using namespace std;


//���࿪�������̣߳���һ��e2v���sdk�е��̣߳�����ǳ����ȡͼ�̡߳�
class ImgScanner
{
public:
	ImgScanner(E2VBuffer *_e2vbuffer);
	~ImgScanner();

	bool StartFlag = true;

	static const int FrameTimeUS = 200;//ÿ��200΢��ȡһ֡
private:
	E2VBuffer *p_e2vbuffer = NULL;
	E2VCamera *p_e2v = NULL;

	void release();
	void freerun();
};

