
#pragma once
#include "../stdafx.h"

#ifndef CV_BGR2GRAY
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#endif

//���ڴ�ż�⵽��ȱ�ݣ���������������
class Faults
{
	//ȱ��
	struct BrokenEdge
	{
		cv::Point position;//ȱ��λ��
		double deep;//������ȣ����ף�
		double length;//ȱ�߳������ף�
	};
	//ȱ��
	struct BrokenCorner
	{
		cv::Point position;//ȱ��λ��
		double deep;//������ȣ����ף�
		double length;//ȱ�ǳ�y�ᣨ���ף�
		double width;//ȱ�ǿ�x�ᣨ���ף�
	};
	//����
	struct Crazing
	{
		cv::Point position;//ȱ��λ��
		double diameter;//ֱ�������ף�
	};
	//����
	struct Hole
	{
		cv::Point position;//����λ��
		double diameter;//ֱ�������ף�
	};
	//����
	struct Scratch
	{
		cv::Point position;//λ��
		double length;//ȱ�ǳ�y�ᣨ���ף�
		double angel;//ֱ�������ף�
	};



public:


	/************************����************************/

	//�����һ�ֵĴ�����Ϣ
	void Clear(){
		BrokenEdges.clear();//ȱ��
		BrokenCorners.clear();//ȱ��
		Crazings.clear();//����
		Holes.clear();//����
		Scratchs.clear();//����
	};

	//���ò��������룺x��ÿ���ض�Ӧ���ٺ��ף�y��ÿ���ض�Ӧ���ٺ��ף��Խ���ÿ���ض�Ӧ���ٺ���
	void SetParameter(double MilliMeterPerPix_X, double MilliMeterPerPix_Y, double MilliMeterPerPix_Diagonal)
	{
		milliMeterPerPix_X = MilliMeterPerPix_X;
		milliMeterPerPix_Y = MilliMeterPerPix_Y;
		milliMeterPerPix_Diagonal = MilliMeterPerPix_Diagonal;
	};

	//��ȡx��ÿ���ض�Ӧ���ٺ���
	double GetMilliMeterPerPix_X(){ return milliMeterPerPix_X; };
	//��ȡy��ÿ���ض�Ӧ���ٺ���
	double GetMilliMeterPerPix_Y(){ return milliMeterPerPix_Y; };
	//��ȡ�Խ���ÿ���ض�Ӧ���ٺ���
	double GetMilliMeterPerPix_Diagonal(){ return milliMeterPerPix_Diagonal; };





	/**************************����********************************/
	//�Խ�����������
	double DiagonalDiff_mm;

	vector<BrokenEdge> BrokenEdges;//ȱ��
	vector<BrokenCorner> BrokenCorners;//ȱ��
	vector<Crazing> Crazings;//����
	vector<Hole> Holes;//����
	vector<Scratch> Scratchs;//����
private:
	double milliMeterPerPix_X = 0;//x��ÿ���ض�Ӧ���ٺ���
	double milliMeterPerPix_Y = 0;//y��ÿ���ض�Ӧ���ٺ���
	double milliMeterPerPix_Diagonal = 0;//�Խ���ÿ���ض�Ӧ���ٺ���
};