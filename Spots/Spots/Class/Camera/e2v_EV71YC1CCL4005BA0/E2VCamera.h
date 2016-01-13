#pragma once


#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include <opencv2/opencv.hpp>

#include "E2VBuffer.h"
using namespace std;

class E2VCamera
{
public:
	//��ʼ���ɼ�����
	//1.����ָ�룬ΪNULLʱ�Զ������ڴ�
	//2.�ɼ����
	//3.ÿ֡ʱ����΢�룩Ҫ�����30
	//4.ɫ��ģʽ ö�� RGB OR GRAY
	//5.�ɼ������Logical number of the board.��Ĭ��Ϊ0��
	//6.�ɼ���Port�ںţ�PORT_A(Ĭ��) OR PORT_B
	E2VCamera(E2VBuffer *_e2vbuffer, int width, int frameTimeUS, int colorType = RGB, int boardID = 0, int Camport = PORT_A);
	~E2VCamera();

	//��ʼ��ͼ
	void FreeRun();


	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};

	bool StartFlag = true;
private:
	E2VBuffer *p_e2vbuffer = NULL;
	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer










	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//ÿ֡���
	const int _frameHeight = 1;		//ÿ֡�߶�
	int _frameTimeUS = 0;			//ÿ֡ʱ����΢�룩
	int _nBoard = 0;				//�ɼ������
	int _camPort = PORT_A;			//�ɼ���Port
	int _colorType = 1;				//��ͼ��ɫģʽ


	void test();

	// �ڿ���̨��ʾ���ɼ�����Ϣ
	int getNrOfBoards();
	// get board and camera-link serial port information
	// using silicon-software runtime library fglib5
	int getBoardInfo();


	//��ʼ��fg������ErrCode��0Ϊ�޴���
	//static int init_fg(Fg_Struct *fg, const char *dllName, int nBoard, int camPort);
	void init_fg();

	//������ʾ���ڣ����ش���id;
	int createDiplayBuffer();

	//�����ڴ棬����ErrCode
	void memoryAllocation();

	//�ͷ��ڴ�
	void release();

	//���MD�ɼ����Ƿ��д���
	//����->�˳�
	void errorMessageWait();
};