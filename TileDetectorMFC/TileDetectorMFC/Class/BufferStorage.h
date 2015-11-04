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
	cv::Mat Buffer0;
	//�Ҷ�����
	//cv::Mat Buffer0Gray;
	//����һͼ��
	cv::Mat Buffer0Img;
	//cv::Mat Buffer0Flag;//������������ʾĳһ֡״̬����ʼΪ0��д���Ϊ1����ȡ��Ϊ2

	//ԭʼ����
	cv::Mat Buffer1;
	//�Ҷ�����
	//cv::Mat Buffer1Gray;
	//����һͼ��
	cv::Mat Buffer1Img;

	//ԭʼ����
	cv::Mat NowBuffer;//ָ��ǰ������Ļ�����Buffer0 �� Buffer1
	//�Ҷ�����
	//cv::Mat NowBufferGray;
	//����һͼ��
	cv::Mat NowBufferImg;

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

	int bufferIndex = 0;//�жϵ�ǰ����ʹ����һ��buffer��ÿִ��һ��Start()����һ��Buffer
};