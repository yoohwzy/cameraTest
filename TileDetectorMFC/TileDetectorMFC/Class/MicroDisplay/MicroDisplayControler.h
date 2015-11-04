#pragma once
#include "../../stdafx.h"
#include "../../globle_head.h"
#include "../../globle_debug.h"
#include "MicroDisplayInit.h"
#include "../BufferStorage.h"

//#define CONFIG_FILENAME "anOriginalConfigFileName.mcf"

// �����ɼ��������ࣨ��̬�������Ʋɼ��Ŀ�ʼ���������ɼ�ѭ��
class MicroDisplayControler
{
private:
public:
	//��ʼ�ɼ�
	static int StartGrabbing(MicroDisplayInit& mdi);
	//�����ɼ�
	static void EndGrabbing(MicroDisplayInit& mdi);
	//ѭ����ͼ ֱ���ɹ�Ϊֹ
	static int FreeRunning(MicroDisplayInit& mdi, BufferStorage& s);
	//�Խ���
	static int FreeRunningFocusing(MicroDisplayInit& mdi);
};