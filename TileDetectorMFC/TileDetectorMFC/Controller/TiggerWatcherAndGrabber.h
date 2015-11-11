#pragma once
//@description	本类用于监视触发器状态，触发后开始采图并拒绝触发直至采图结束
//				流程->初始化->开始监视函数->触发->初始化相机->开始采图并禁止触发->释放相机->结束采图允许触发->停止监视
//@author VShawn
//@last modify date 2015年10月31日 By VShawn
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "../globle_var.h"


#include "../Class/MicroDisplay/MicroDisplayControler.h"
#include "../Class/Base/VirtualCamera.h"

#include "../Class/Base/Block.h"

#include <thread>
#include "../mc100.h"

class TiggerWatcherAndGrabber
{
public:
	//参数1 MFC窗口句柄 参数2 虚拟相机照片名称
	TiggerWatcherAndGrabber(HWND _hwnd = NULL,string virtualImg = "");
	~TiggerWatcherAndGrabber();

	void StartWatch();
	//开始一次定标，完成后自动停止
	void StartWatchWithCalibration();
	void StopWatch();

	void Init(string virtualImg="");
	void Switch2Real(bool refresh = false);
	void Switch2Virtual(string virtualImg, bool refresh = false);


	//手动触发，若无法触发返回false
	bool ManualTigger();
	int GrabbingIndex = 0;

	VirtualCamera *vc;

private:
	HWND hwnd;
	bool USING_VIRTUAL_CAMERA = false;
	//手动触发标志
	bool BeManualTiggered = false;
	bool BeAutoTiggered()
	{ 
		if (mc100_open(0) >= 0)
			return mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1;
		else
			return false;
	};
	bool IsCalibration = false;
	bool IsWatching = false;
	bool IsGrabbing = false;
	bool IsThreeInOne = false;



	void watcherThread();

	void threeInOne();

};

