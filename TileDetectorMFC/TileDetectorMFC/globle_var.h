#pragma once

//#include "Class\BufferStorage.h"
#include "Class\Base\SettingHelper.h"


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

	static int Width;					//ÿ֡���
	static const int FrameHeight = 1;		//ÿ֡�߶�
	static int FrameCount;			//�ܹ�֡��
	static int ColorType;				//��ͼ��ɫģʽ












	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:
};

