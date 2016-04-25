#pragma once

#ifndef FAULTS
#define FAULTS

#ifndef CV_BGR2GRAY
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#endif


using namespace std;

//���ڴ�ż�⵽��ȱ�ݣ���������������
class Faults
{
public:
	//Faults();
	//~Faults(){
	//	Clear();
	//};

	//ȱ��
	struct BrokenEdge
	{
		cv::Point position;//ȱ��λ��
		double deep;//������ȣ�pix��
		double length;//ȱ�߳���pix��
		double deep_mm;//������ȣ�mm��
		double length_mm;//ȱ�߳���mm��
	};
	//ȱ��
	struct BrokenCorner
	{
		cv::Point position;//ȱ��λ��
		double deep;//������ȣ�pix��
		double length;//ȱ�ǳ�y�ᣨpix��
		double width;//ȱ�ǿ�x�ᣨpix��

		double deep_mm;//������ȣ�mm��
		double length_mm;//ȱ�ǳ�y�ᣨmm��
		double width_mm;//ȱ�ǿ�x�ᣨmm��
	};
	//����
	struct Crazing
	{
		cv::Point position;//����λ��
		double diameter;//ֱ����pix��
		double diameter_mm;//ֱ����mm��
	};
	//����
	struct Hole
	{
		cv::Point position;//����λ��
		double diameter;//ֱ����pix��
		double diameter_mm;//ֱ����mm��
	};
	//����
	struct Scratch
	{
		cv::Point position;//λ��
		double length;//���۳��ȣ�pix��
		double length_mm;//���۳��ȣ�pix��
		double angel;//�Ƕȡ�
	};
	////�޴�ȱ�ݣ�һ�������ж�Ϊ���ϸ�Ʒ
	//struct SomethingBig
	//{
	//	cv::Point position;//λ��
	//	double diameter;//ֱ����pix��
	//};
	//�˹����
	struct MarkPen
	{
		cv::Rect markposition;//λ��
	};




	/************************����************************/

	//�����һ�ֵĴ�����Ϣ
	void Clear(){
		BrokenEdges.clear();//ȱ��
		BrokenCorners.clear();//ȱ��
		Crazings.clear();//����
		Holes.clear();//����
		Scratchs.clear();//����
		//SomethingBigs.clear();
		MarkPens.clear();
	};

	//���ò��������룺x��ÿ���ض�Ӧ����pix��y��ÿ���ض�Ӧ����pix���Խ���ÿ���ض�Ӧ����pix
	void SetParameter(double MilliMeterPermm_X, double MilliMeterPermm_Y, double MilliMeterPermm_Diagonal)
	{
		milliMeterPermm_X = MilliMeterPermm_X;
		milliMeterPermm_Y = MilliMeterPermm_Y;
		milliMeterPermm_Diagonal = MilliMeterPermm_Diagonal;
	};

	//��ȡx��ÿ���ض�Ӧ����pix
	double GetMilliMeterPermm_X(){ return milliMeterPermm_X; };
	//��ȡy��ÿ���ض�Ӧ����pix
	double GetMilliMeterPermm_Y(){ return milliMeterPermm_Y; };
	//��ȡ�Խ���ÿ���ض�Ӧ����pix
	double GetMilliMeterPermm_Diagonal(){ return milliMeterPermm_Diagonal; };





	/**************************����********************************/
	//�Խ������pix��
	double DiagonalDiff_mm;

	vector<BrokenEdge> BrokenEdges;//ȱ��
	vector<BrokenCorner> BrokenCorners;//ȱ��
	vector<Crazing> Crazings;//����
	vector<Hole> Holes;//����
	vector<Scratch> Scratchs;//����
	//vector<SomethingBig> SomethingBigs;//�޴�ȱ�ݣ�һ�������ж�Ϊ���ϸ�Ʒ
	vector<MarkPen> MarkPens;//�˹����
private:
	double milliMeterPermm_X = 0;//x��ÿ���ض�Ӧ����pix
	double milliMeterPermm_Y = 0;//y��ÿ���ض�Ӧ����pix
	double milliMeterPermm_Diagonal = 0;//�Խ���ÿ���ض�Ӧ����pix
};

#endif