#pragma once

#include <opencv2/opencv.hpp>   
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>  

using namespace std;

class Storage
{
public: 
	Storage(){};
	Storage(int width,int length);
	~Storage() ;

	int BufferWriteIndex = 0;
	int BufferReadIndex = 0;

	cv::Mat Buffer0;
	cv::Mat Buffer0Gray;
	cv::Mat Buffer0Img;
	//cv::Mat Buffer0Flag;//������������ʾĳһ֡״̬����ʼΪ0��д���Ϊ1����ȡ��Ϊ2

	cv::Mat Buffer1;
	cv::Mat Buffer1Gray;
	cv::Mat Buffer1Img;
	//cv::Mat Buffer1Flag;//������������ʾĳһ֡״̬����ʼΪ0��д���Ϊ1����ȡ��Ϊ2

	cv::Mat NowBuffer;//ָ��ǰ������Ļ�����Buffer0 �� Buffer1
	cv::Mat NowBufferGray;
	cv::Mat NowBufferImg;

	int WIDTH;//��������ֱ���
	int LENGTH;//����ɼ�����
	bool EndFlag = false;


	void Start();//��ʼ��һ�ֲɼ�������Read/Write Index = 0;
	bool AddFrame(cv::Mat& frame);//������ͷ����һ֡,�����Ƿ�ֹͣ�ɼ�
	int GetFrame(cv::Mat& frame);//�ӻ������һ֡����ֵ��frame,���� 0 ����,1 ����,-1��һ֡��δд�����
private:
	unsigned int bufferIndex = 0;//�жϵ�ǰ����ʹ����һ��buffer��ÿִ��һ��Start()����һ��Buffer


	//enum StorageChoose
	//{
	//	Storage1,
	//	Sotrage2
	//};
};


