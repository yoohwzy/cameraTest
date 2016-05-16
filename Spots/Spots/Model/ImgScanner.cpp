#include "ImgScanner.h"
#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h>


ImgScanner::ImgScanner(E2VBuffer *_e2vbuffer, int FrameCountPerSecond)
{
	FrameTimeUS = 1000000 / FrameCountPerSecond;
	if (FrameTimeUS < 10)
		FrameTimeUS = 10;
	p_e2vbuffer = _e2vbuffer;
	p_e2v = new E2VCamera(p_e2vbuffer, 4096, ImgScanner::FrameTimeUS, p_e2vbuffer->GetColorType() == CV_8U ? E2VCamera::GRAY : E2VCamera::RGB, 0, PORT_A);
	if (p_e2v->StartFlag)
	{
		StartFlag = true;
		std::thread t_scanThread(std::mem_fn(&ImgScanner::freerun), this);
		auto tn = t_scanThread.native_handle();
		SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);
		t_scanThread.detach();
	}
	else
	{
		StartFlag = false;
		release();
	}
}


ImgScanner::~ImgScanner()
{
	release();
}

void ImgScanner::freerun()
{
	p_e2v->FreeRun();
}

void ImgScanner::release()
{
	if (p_e2v != NULL)
	{
		p_e2v->StartFlag = false;
		Sleep(10);
		delete p_e2v;
		p_e2v = NULL;
	}
}
int ImgScanner::FrameTimeUS = 200;