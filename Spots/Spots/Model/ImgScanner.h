#pragma once

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h"

#include <thread>

using namespace std;


//本类开启两个线程，其一是e2v相机sdk中的线程，其二是程序的取图线程。
class ImgScanner
{
public:
	//初始化线阵相机，将线阵相机与采图缓存类关联，设置相机行频
	//输入行频不应大于相机的物理行频（即e2v相机管理面板上设置的行频）
	ImgScanner(E2VBuffer *_e2vbuffer, int FrameCountPerSecond = 5000);
	~ImgScanner();

	static int FrameTimeUS;//每隔多少微秒取一帧 = 10^6/FrameCountPerSecond
	bool StartFlag = true;

private:
	E2VBuffer *p_e2vbuffer = NULL;
	E2VCamera *p_e2v = NULL;

	void release();
	void freerun();
};

