#pragma once
//@description	�������ڼ��Ӵ�����״̬��������ʼ��ͼ���ܾ�����ֱ����ͼ����
//				����->��ʼ��->��ʼ���Ӻ���->����->��ʼ�����->��ʼ��ͼ����ֹ����->�ͷ����->������ͼ������->ֹͣ����
//@author VShawn
//@last modify date 2015��10��31�� By VShawn
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "../globle_var.h"


#include "../Class/Camera/E2VCamera.h"
//#include "../Class/Camera/MicroDisplay.h"
#include "../Class/Camera/VirtualCamera.h"
#include "../Class/BufferStorage.h"
#include "../Class/Base/Block.h"

#include <thread>
#include "../mc100.h"

class TiggerWatcherAndGrabber
{
public:
	//����1 MFC���ھ�� ����2 ���������Ƭ����
	TiggerWatcherAndGrabber(HWND _hwnd = NULL,string virtualImg = "");
	~TiggerWatcherAndGrabber();

	void StartWatch();
	//��ʼһ�ζ��꣬��ɺ��Զ�ֹͣ
	void StartWatchWithCalibration();
	void StopWatch();

	void Init(string virtualImg="");
	bool Switch2Real();
	bool Switch2Virtual(string virtualImg);


	//�ֶ����������޷���������false
	bool ManualTigger();
	int GrabbingIndex = 0;

	//ԭʼͼ��
	cv::Mat OriginalImage;
	//���е��Ӻ��ͼ��
	cv::Mat Image;
private:
	HWND hwnd;
	GrabbingBuffer *gb = NULL;
	E2VCamera *e2v = NULL;
	std::thread *t_watcher = NULL;

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
	bool IsCalibration = false;
	bool IsWatching = false;
	bool IsGrabbing = false;
	
	string _virtualImg = "";//��������ļ���

	void watcherThread();
	//��ͼ����
	void capture();
	//���ӽ���
	void threeInOne();

};

