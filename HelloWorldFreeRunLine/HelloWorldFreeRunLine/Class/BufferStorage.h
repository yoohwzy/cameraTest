#pragma once
#include "../globle.h"

class BufferStorage
{
public:
	BufferStorage(){};
	BufferStorage(int width, int length);
	~BufferStorage();


	int BufferWriteIndex = 0;
	int BufferReadIndex = 0;

	//ԭʼ����
	cv::Mat Buffer0;
	//�Ҷ�����
	cv::Mat Buffer0Gray;
	//����һͼ��
	cv::Mat Buffer0Img;
	//cv::Mat Buffer0Flag;//������������ʾĳһ֡״̬����ʼΪ0��д���Ϊ1����ȡ��Ϊ2

	//ԭʼ����
	cv::Mat Buffer1;
	//�Ҷ�����
	cv::Mat Buffer1Gray;
	//����һͼ��
	cv::Mat Buffer1Img;

	//ԭʼ����
	cv::Mat NowBuffer;//ָ��ǰ�������Ļ�����Buffer0 �� Buffer1
	//�Ҷ�����
	cv::Mat NowBufferGray;
	//����һͼ��
	cv::Mat NowBufferImg;



	bool EndReadFlag = false;
	bool EndWriteFlag = false;


	void Start();//��ʼ��һ�ֲɼ�������Read/Write Index = 0;
	bool AddFrame(cv::Mat& frame);//������ͷ����һ֡,�����Ƿ�ֹͣ�ɼ�
	bool AddFrame(cv::Mat& frame,int);//������ͷ����һ֡,�����Ƿ�ֹͣ�ɼ�
	int GetFrame(cv::Mat& frame);//�ӻ������һ֡����ֵ��frame,���� 0 ����,1 ����,-1��һ֡��δд�����
private:
	int bufferIndex = 0;//�жϵ�ǰ����ʹ����һ��buffer��ÿִ��һ��Start()����һ��Buffer
	int WIDTH = 4096;//��������ֱ���
	int LENGTH = 1000;//����ɼ�����
};
