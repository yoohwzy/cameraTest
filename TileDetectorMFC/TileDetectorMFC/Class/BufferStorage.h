#pragma once
#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"

class BufferStorage
{
public:
	BufferStorage(){};
	BufferStorage(int _MaxPics, int _width);
	~BufferStorage();

	//���кϳ�һ�У�
	int NinOne = 3;
	int BufferWriteIndex = 0;
	int BufferReadIndex = 0;

	//ԭʼ����
	cv::Mat Buffer;
	//�Ҷ�����
	//cv::Mat Buffer1Gray;
	//����һͼ��
	cv::Mat BufferImg;

	bool EndReadFlag = false;
	//Ϊtrueʱ��ʾ�������
	bool EndWriteFlag = false;

	//��ʼ��һ�ֲɼ�������Read/Write Index = 0;
	void Start();
	//������ͷ����һ֡,�����Ƿ�ֹͣ�ɼ�
	bool AddFrame(cv::Mat& frame);
	//�ӻ������һ֡����ֵ��frame,���� 0 ����,1 ����,-1��һ֡��δд�����
	int GetFrame(cv::Mat& frame);
	//�����е���һ�С�
	void ThreeInOne(int lineIndex);
private:

	int MaxPics = 0;
	int width = 0;
	//cv::Mat SamplesRGB;//һ�����ڹ��ս���
};