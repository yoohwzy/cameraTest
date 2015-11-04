#pragma once
//@description	�������ڼ��Ӵ�����״̬��������ʼ��ͼ���ܾ�����ֱ����ͼ����
//				����->��ʼ��->��ʼ���Ӻ���->����->��ʼ�����->��ʼ��ͼ����ֹ����->�ͷ����->������ͼ������->ֹͣ����
//@author VShawn
//@last modify date 2015��10��31�� By VShawn
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "../globle_var.h"


#include "../Class/MicroDisplay/MicroDisplayControler.h"
#include "../Class/VirtualCamera.h"


#include <thread>
#include "../mc100.h"

class TiggerWatcherAndGrabber
{
public:
	//����1 MFC���ھ�� ����2 ���������Ƭ����
	TiggerWatcherAndGrabber(HWND _hwnd = NULL,string virtualImg = "");
	~TiggerWatcherAndGrabber();

	void StartWatch();
	void StopWatch();
	//�ֶ����������޷���������false
	bool ManualTigger();

	int GrabbingIndex = 0;

private:
	VirtualCamera vc;
	HWND hwnd;
	bool USING_VIRTUAL_CAMERA = false;
	//�ֶ�������־
	bool BeManualTiggered = false;
	bool BeAutoTiggered()
	{ 
		if (mc100_open(0) >= 0)
			return mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1;
		else
			return false;
	};
	bool IsWatching = false;
	bool IsGrabbing = false;


	//ʹ����ʵ�����ʼ��
	void initForE2V();
	//ʹ�����������ʼ��
	void initForVC(string virtualImg);


	void watcherThread();

	void threeInOne();

};

