#pragma once

#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\BufferStorage.h"

//@description	�˴�����ȫ�ֱ�����ȫ���࣬�Լ���غ���
//@author VShawn
//@last modify date 2015��10��31�� By VShawn
class globle_var
{
public:
	globle_var();
	~globle_var();
	//��ʼ��ϵͳ����
	//����true:��ini�ļ���ȡ������false:ʹ��Ĭ�ϲ���
	static bool InitSetting(bool isload = false);
	//���òɼ��������޸ĺ����Ӧ���³�ʼ��
	static bool SetGrabSetting(string colorType, int maxPics, int width);
	//��������Ϣ������inf
	static void SaveSetting();




	//������������ ��ʱ���ٺ��뿪ʼ�ɼ�
	static int TiggerWaitTimeMS;
	//���������Ƭ�ļ�����Ϊ""ʱʹ����ʵ���
	static string VirtualCameraFileName;
	static BufferStorage s;
	static MicroDisplayInit mdi(){ return _mdi; };//mdiдΪֻ��
private:
	static MicroDisplayInit _mdi;
};

