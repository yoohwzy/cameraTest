#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h";
#include "BlocksDetector.h"

class Measurer
{
private:
	MicroDisplayInit *mdi;
	BlocksDetector *bd_Standard;

	//�궨ש ������ ʵ�ʵ��ĸ�����
	cv::Point A_Standard;
	cv::Point B_Standard;
	cv::Point C_Standard;
	cv::Point D_Standard;

public:
	//�ϱ�Ե��׼���
	double tileStandardWidthUp_pix = 0;
	//�±�Ե��׼���
	double tileStandardWidthDown_pix = 0;
	//���Ե��׼�߶�
	double tileStandardHeightLeft_pix = 0;
	//�ұ�Ե��׼�߶�
	double tileStandardHeightRight_pix = 0;
	//�Խ���1����AC��
	double tileStandardDiagonalAC_pix = 0;
	//�Խ���2����BD��
	double tileStandardDiagonalBD_pix = 0;


	//��ש��׼���
	double tileStandardWidth_mm = 0;
	//��ש��׼�߶�
	double tileStandardHeight_mm = 0;



	//�����ʼ�� ��ש��ȵ�λΪmm
	Measurer(BlocksDetector *b, MicroDisplayInit *mdii, int TileStandardWidthmm, int TileStandardHeightmm);
	//��ȡ���ݳ�ʼ�� ����������ʾһ�����ر�ʾ���ٺ���
	Measurer(BlocksDetector *b, MicroDisplayInit *mdii, double MilliMeterPerPix_Width, double MilliMeterPerPix_Height);
	~Measurer();


	/*//Ͱ��ʧ��
	void BarrelDistortion(cv::Mat calibrationImg);*/

	//���λ���궨������һ��ÿ�����ص�����Ӧ��ʵ�����꣬����Ϊ�궨�õĺڰ׸���ͼ��
	void PincushionCalibration(cv::Mat calibrationImg);
	void PincushionImgAdjust(cv::Mat& Img);
	//��Ƭӳ�䵽ʵ�� ����4096����ʾ��ƬX���ϣ�ÿ���㾭�궨��Ӧ��λ���ĸ�λ�á�
	vector<double> PincushionMap;
	//ʵ��ӳ�䵽��Ƭ������ȷ��ʵ��Xλ�ô������ص���ɣ�
	vector<double> PincushionMap_Anti;


	//���߱궨 ��ñ�׼�߶ȣ���ȣ�1pix�������mm
	void ObserveCalibration();
	//���ݱ궨����
	void ObserveImgAdjust(cv::Mat& Img);
	vector<int> RowAdjust;


	void CaculteSize(BlocksDetector *bd);


	double MilliMeterPerPix_Width = 0;
	double MilliMeterPerPix_Height = 0;
	double MilliMeterPerPix_Diagonal = 0;
};

