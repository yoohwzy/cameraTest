#pragma once
#include "../../stdafx.h"
#include "../../globle_head.h"
#include "../../globle_debug.h"
#include "MicroDisplayInit.h"
#include "../BufferStorage.h"

//#define CONFIG_FILENAME "anOriginalConfigFileName.mcf"

// 相机与采集卡控制类（静态），控制采集的开始，结束，采集循环
class MicroDisplayControler
{
private:
public:
	//开始采集
	static int StartGrabbing(MicroDisplayInit& mdi);
	//结束采集
	static void EndGrabbing(MicroDisplayInit& mdi);
	//循环采图 直到采够为止
	static int FreeRunning(MicroDisplayInit& mdi, BufferStorage *s);
	//对焦用
	static int FreeRunningFocusing(MicroDisplayInit& mdi);

	//测试采集卡是否正常
	static int Test(MicroDisplayInit& mdi);
};