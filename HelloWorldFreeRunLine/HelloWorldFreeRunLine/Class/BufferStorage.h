#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h"

class BufferStorage
{
public:
	BufferStorage(){};
	BufferStorage(MicroDisplayInit& mditmp);
	~BufferStorage();

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
private:
	MicroDisplayInit mdi;
	int bufferIndex = 0;//�жϵ�ǰ����ʹ����һ��buffer��ÿִ��һ��Start()����һ��Buffer
	int WIDTH = 4096;//��������ֱ���
	int LENGTH = 1000;//����ɼ�����
};

