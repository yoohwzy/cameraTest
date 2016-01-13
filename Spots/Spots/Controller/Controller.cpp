#include "Controller.h"


Controller::Controller(E2VBuffer *_e2vbuffer)
{
	p_e2vbuffer = _e2vbuffer;
	pci1761.init();
}


Controller::~Controller()
{
	exitFlag = true;
	Sleep(10);
	p_e2vbuffer = NULL;
}

void Controller::triggerWatcher()
{
	while (!exitFlag)
	{
		if (pci1761.GetRisingEdgeIDI(7))
		{
			Worker w = Worker(p_e2vbuffer);
			workers.push(w);
		}
		else if (pci1761.GetTrailingEdgeIDI(7))
		{
			workers.front().GetPhotoOn = false;
		}
	}
}
void workerCallBack()
{

}