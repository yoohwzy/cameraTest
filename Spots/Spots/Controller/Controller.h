#pragma once

#include "../Class/IOCard/PCI1761/PCI1761.h"
#include "Worker.h"

#include <queue>
using namespace std;

class Controller
{
public:
	Controller(E2VBuffer *_e2vbuffer);
	~Controller();
private:
	E2VBuffer *p_e2vbuffer = NULL;
	PCI1761 pci1761;
	queue<Worker> workers;
	bool exitFlag = false;
	void triggerWatcher();
	void workerCallBack();
};