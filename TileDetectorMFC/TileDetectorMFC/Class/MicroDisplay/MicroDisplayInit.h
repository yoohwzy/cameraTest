#pragma once
#include "../../stdafx.h"
#include "../../globle_head.h"
#include "../../globle_debug.h"


// ���������ʼ���࣬�洢����趨����������ʼ���ɼ��������
class MicroDisplayInit
{
private:
	static int getNrOfBoards();
	static int getBoardInfo();
	static int memoryAllocation(MicroDisplayInit& mdi);
	static int initFG(MicroDisplayInit& mdi);
public:
	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	Fg_Struct *fg = 0;
	dma_mem *pMem0 = 0;
	// Number of memory buffer
	int nr_of_buffer = 8;
	// Board Number
	int nBoard = 0;
	// Port (PORT_A / PORT_B / PORT_C / PORT_D)
	int nCamPort = PORT_A;
	// Number of images to grab
	int MaxPics = 10002;
	// ��ͼ����ID
	int nId = -1;
	// ��ͼ��ɫģʽ
	int colorType = 0;
	enum ColorType 
	{
		GRAY = 0,
		RGB = 1
	};
	// Setting the image size
	int Width = 4096;
	int Height = 10;

	cv::Mat SamplesGray;
	cv::Mat SamplesRGB;

	//���������ļ���ʼ��
	static int InitLoad(MicroDisplayInit& mdi, char * mcfName = "4096grayline.mcf");
	//������ʼ��
	//���Ρ����ÿ�ȡ����ø߶ȡ������ڴ�memoryAllocation
	static int InitParameter(MicroDisplayInit& mdi);
	//�����ڴ�  ��Ū����
	static void CreateBufferWithOutDiplay(MicroDisplayInit& mdi);
	//�����ڴ�����ӻ�����
	static void CreateBufferWithDiplay(MicroDisplayInit& mdi);
	//�������ͷ��ڴ棬��������
	static void Release(MicroDisplayInit& mdi);

	static int getNoOfBitsFromImageFormat(const int format);
};

