#pragma once

//@description	此处声明全局变量，全局类，以及相关函数
//@author VShawn
//@last modify date 2015年10月31日 By VShawn

#pragma once

#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\BufferStorage.h"


class globle_var
{
public:
	globle_var();
	~globle_var();
	static void InitSetting();
	static void InitSetting(string colorType,int maxPics,int width);

	//触发器触发后 延时多少毫秒开始采集
	static int TiggerWaitTimeMS;
	//虚拟相机底片文件名，为""时使用真实相机
	static string VirtualCameraFileName;


	static MicroDisplayInit mdi;
	static BufferStorage s;
};

