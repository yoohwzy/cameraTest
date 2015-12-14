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
#include "../Class/Camera/VirtualCameraPre.h"
//#include "../Class/Camera/MicroDisplay.h"
//#include "../Class/Camera/VirtualCamera.h"
#include "../Class/BufferStorage.h"
#include "../Class/Base/Block.h"

#include "../Class/IOCard/mc100USBCard.h"
#include "../Class/IOCard/PCI1761.h"

#include <thread>

class TiggerWatcherAndGrabber
{
public:
	//����1 MFC���ھ�� ����2 ���������Ƭ����
	TiggerWatcherAndGrabber(HWND _hwnd = NULL,string virtualImg = "");
	~TiggerWatcherAndGrabber()
	{
		if (p_gb != NULL)
		{
			delete p_gb;
			p_gb = NULL;
		}
		if (p_e2v != NULL)
		{
			delete p_e2v;
			p_e2v = NULL;
		}
		if (p_vc != NULL)
		{
			delete p_vc;
			p_vc = NULL;
		}
		printf_globle("TiggerWatcherAndGrabber unload!\n");
	};

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
	GrabbingBuffer *p_gb = NULL;
private:
	HWND hwnd;
	E2VCamera *p_e2v = NULL;
	VirtualCameraPre *p_vc = NULL;

	std::thread *p_t_watcher = NULL;

	bool USING_VIRTUAL_CAMERA = false;
	//�ֶ�������־
	bool BeManualTiggered = false;
	//mc100USBCard mc100;
	PCI1761 pci1761;
	bool IsCalibration = false;
	bool IsWatching = false;
	bool IsGrabbing = false;
	

	void watcherThread();
	//��ͼ����
	void capture();
	//���ӽ���
	void threeInOne();

};

