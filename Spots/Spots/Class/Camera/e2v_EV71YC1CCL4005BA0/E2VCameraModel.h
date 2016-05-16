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


using namespace std;


//E2V����Ļ��࣬�Դ����������ֲɼ�ģʽ
//@author VShawn
class E2VCameraModel
{
public:
	enum ColorType
	{
		GRAY = CV_8U,
		RGB = CV_8UC3
	};

	//��ʼ���ɼ�����
	//1.�ɼ�ͼ����
	//2.�ɼ�ͼ��߶�
	//3.ɫ��ģʽ ö�� RGB OR GRAY
	//4.�ɼ������Logical number of the board.��Ĭ��Ϊ0��
	//5.�ɼ���Port�ںţ�PORT_A(Ĭ��) OR PORT_B
	E2VCameraModel(int width, int height = 0, int colorType = RGB, int boardID = 0, int Camport = PORT_A);
	~E2VCameraModel();

	//�麯����������ʵ��
	virtual cv::Mat GetImage(){ return cv::Mat(); };

	bool ReInitFg();//�������³�ʼ��
	bool HasBeenInited = false;		//�Ƿ�����ɳ�ʼ��

protected:
	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer




	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//ͼ����
	int _height = 0;				//ͼ��߶�
	const int _frameHeight = 1;		//ÿ֡�߶�
	int _nBoard = 0;				//�ɼ������
	int _camPort = PORT_A;			//�ɼ���Port
	int _colorType = 1;				//��ͼ��ɫģʽ

	//���MD�ɼ����Ƿ��д���
	//����->�˳�
	void errorMessageWait();

private:
	//��ʼ��fg������ErrCode��0Ϊ�޴���
	bool init_fg();

	// �ڿ���̨��ʾ���ɼ�����Ϣ
	int getNrOfBoards();
	// get board and camera-link serial port information
	// using silicon-software runtime library fglib5
	int getBoardInfo();

	//������ʾ���ڣ����ش���id;
	int createDiplayBuffer();

	//�����ڴ棬����ErrCode
	bool memoryAllocation();

	//�ͷ��ڴ�
	void release();
};