#pragma once

#include "../Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h"
#include "../Class/IOCard/PCI1761/PCI1761.h"
#include "Worker.h"

#include <opencv2\opencv.hpp>
#include <queue>
#include <thread>
using namespace std;

class Controller
{
public:
	Controller();
	~Controller();

private:
	//HWND handle_mainWindow;

	E2VBuffer *p_e2vbuffer = NULL;
	ImgScanner *p_imgscanner = NULL;

	PCI1761 pci1761;

	//ͬʱֻ�����������˹�������ֻ������ͼ������ש��

	//���һ�����ٻ��Ĺ���
	Worker *lastestWorker = NULL;
	//����1
	Worker *worker1 = NULL;
	//����2
	Worker *worker2 = NULL;

	bool exitFlag = false;
	void triggerWatcher();
	void jobWatcher();
	void workerCallBack();
};