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

#include "../Class/IOCard/mc100USBCard.h"
#include "../Class/IOCard/PCI1761.h"

#include <thread>

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
	GrabbingBuffer *gb = NULL;
private:
	HWND hwnd;
	E2VCamera *e2v = NULL;
	std::thread *t_watcher = NULL;

	bool USING_VIRTUAL_CAMERA = false;
	//�ֶ�������־
	bool BeManualTiggered = false;
	//mc100USBCard mc100;
	PCI1761 pci1761;
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

