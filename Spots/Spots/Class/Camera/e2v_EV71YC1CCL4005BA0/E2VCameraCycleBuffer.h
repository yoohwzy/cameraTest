#pragma once

#include "E2VCameraModel.h"


using namespace std;

//E2V�����ѭ������
class E2VCycleBuffer
{
public:
	//�����������ѭ������
	//������ͼ���ȣ�4k�����Ƿ�Ϊ�Ҷ�ͼ��bool��
	E2VCycleBuffer(int width, bool isGray = false);
	//E2VCycleBuffer(int width, int length = 20000, bool isGray = false);
	~E2VCycleBuffer()
	{
	};
	static const int BufferLength = 20000;

	//���OPENCV��ɫ��ģʽ��
	int GetColorType(){ return colorType; };
	//��ȡ��ǰ��дָ��
	int GetWriteIndex()
	{
		return WriteIndex;
	};
	//������ʼ����������ȡһ��ͼ��
	//��������startLine = 0,endLine = 2�������һ��0�п�ʼ������Ϊ3���ص�ͼ��
	//����startLine = 19000,endLine = 0�������һ��19000�п�ʼ������Ϊ1001���ص�ͼ��
	cv::Mat GetImage(int startLine, int endLine);


	//��һ��д��ѭ������
	void WriteData(cv::Mat oneline);
	////��ѭ�������ж�ȡͼƬ������ʼ��������������
	//void ReadData(int startIndex, int endIndex);
	cv::Mat Buffer;
private:
	int colorType = CV_8U;
	int WriteIndex = 0;
};
























//E2V����Ĳ����࣬Ϊѭ�������ͼ�汾
//@author VShawn
class E2VCameraCycleBuffer :public E2VCameraModel
{
public:
	//��ʼ���ɼ�����
	//1.�ɼ�ͼ����
	//2.ɫ��ģʽ ö�� RGB OR GRAY
	//3.ÿ֡ʱ����΢�룩Ҫ�����30��Ĭ��Ϊ0
	//4.�ɼ������Logical number of the board.��Ĭ��Ϊ0��
	//5.�ɼ���Port�ںţ�PORT_A(Ĭ��) OR PORT_B
	E2VCameraCycleBuffer(int width, int colorType = RGB, int frameTimeUS = 0, int boardID = 0, int Camport = PORT_A);
	~E2VCameraCycleBuffer();

	E2VCycleBuffer *P_CycleBuffer = NULL;
	//ÿ֡ʱ����΢�룩Ҫ�����30��Ĭ��Ϊ0
	int FrameTimeUS = 0;
private:

	bool StartFlag = true;

	//��ʼѭ����ͼ
	bool freeRun();
};