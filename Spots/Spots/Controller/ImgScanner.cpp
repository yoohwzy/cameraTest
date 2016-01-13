#include "ImgScanner.h"



ImgScanner::ImgScanner(E2VBuffer *_e2vbuffer)
{
	p_e2vbuffer = _e2vbuffer;
	p_e2v = new E2VCamera(p_e2vbuffer, 4096, 200, p_e2vbuffer->GetColorType() == CV_8U ? E2VCamera::GRAY : E2VCamera::RGB, 0, PORT_A);
	std::thread t_scanThread(std::mem_fn(&ImgScanner::freerun), this);
	t_scanThread.detach();
}


ImgScanner::~ImgScanner()
{
	if (p_e2v != NULL)
	{
		p_e2v->StartFlag = false;
		Sleep(10);
		delete p_e2v;
		p_e2v = NULL;
	}
}

void ImgScanner::freerun()
{
	p_e2v->FreeRun();
}