#pragma once


#include "../../stdafx.h"
#include "../../globle_debug.h"
#include "../BufferStorage.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include <opencv2/opencv.hpp>


class MicroDisplay
{
public:
/*	//��ʼ���ɼ�����
	//1.�ɼ�����
	//2.�ɼ����
	//3.�ɼ������Logical number of the board.��Ĭ��Ϊ0��
	//4.�ɼ���Port�ںţ�PORT_A(Ĭ��) OR PORT_B
	MicroDisplay(GrabbingBuffer *gb, int frameCount, int width, int boardID = 0, int Camport = PORT_A);*/

	//��ʼ���ɼ�����
	//1.�ɼ�����
	//2.�ɼ����
	//3.ɫ��ģʽ ö�� RGB OR GRAY
	//4.�ɼ������Logical number of the board.��Ĭ��Ϊ0��
	//5.�ɼ���Port�ںţ�PORT_A(Ĭ��) OR PORT_B
	MicroDisplay(GrabbingBuffer *gb, int frameCount, int width, int colorType = RGB, int boardID = 0, int Camport = PORT_A);
	~MicroDisplay();

	

	void Capture();

	//���Բɼ���-��������Ƿ����������д�ֱ�ӱ����˳���
	static bool TestCam();


	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:
	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//ÿ֡���
	const int _frameHeight = 1;		//ÿ֡�߶�
	int _frameCount = 0;			//�ܹ�֡��

	int nBoard = 0;					//�ɼ������
	int camPort = PORT_A;			//�ɼ���Port

	
	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer

	GrabbingBuffer *_gb;

	
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