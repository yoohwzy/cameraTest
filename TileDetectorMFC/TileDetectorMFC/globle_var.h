#pragma once

//@description	�˴�����ȫ�ֱ�����ȫ���࣬�Լ���غ���
//@author VShawn
//@last modify date 2015��10��31�� By VShawn

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

	//������������ ��ʱ���ٺ��뿪ʼ�ɼ�
	static int TiggerWaitTimeMS;
	//���������Ƭ�ļ�����Ϊ""ʱʹ����ʵ���
	static string VirtualCameraFileName;


	static MicroDisplayInit mdi;
	static BufferStorage s;
};

