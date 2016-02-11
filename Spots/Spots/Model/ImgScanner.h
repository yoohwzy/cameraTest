#pragma once

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h"

#include <thread>

using namespace std;


//本类开启两个线程，其一是e2v相机sdk中的线程，其二是程序的取图线程。
class ImgScanner
{
public:
	ImgScanner(E2VBuffer *_e2vbuffer);
	~ImgScanner();

	bool StartFlag = true;

	static const int FrameTimeUS = 200;//每隔200微秒取一帧
private:
	E2VBuffer *p_e2vbuffer = NULL;
	E2VCamera *p_e2v = NULL;

	void release();
	void freerun();
};

