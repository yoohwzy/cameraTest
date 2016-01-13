#pragma once

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h"

#include <thread>

using namespace std;

class ImgScanner
{
public:
	ImgScanner(E2VBuffer *_e2vbuffer);
	~ImgScanner();
private:
	E2VBuffer *p_e2vbuffer = NULL;
	E2VCamera *p_e2v = NULL;

	void freerun();
};

