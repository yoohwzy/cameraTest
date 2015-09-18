#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h";
#include "BlocksDetector.h"
#define RADIAN(a)  (a*CV_PI/180.0) //���ݽǶȻ�û���
#define ANGLE(r)	(180.0*r/CV_PI)//���ݻ��Ȼ�ýǶ�

class Measurer
{
private:
	MicroDisplayInit *mdi;
	BlocksDetector *bd_Standard;



	/**************************��׼����***********************/

	//�궨ש ������ ʵ�ʵ��ĸ�����
	cv::Point A_Standard;
	cv::Point B_Standard;
	cv::Point C_Standard;
	cv::Point D_Standard;

	//�ϱ�Ե��׼���
	double tileStandardWidth_AB_pix = 0;
	//�±�Ե��׼���
	double tileStandardWidth_CD_pix = 0;
	//���Ե��׼�߶�
	double tileStandardWidth_AD_pix = 0;
	//�ұ�Ե��׼�߶�
	double tileStandardWidth_BC_pix = 0;
	//�Խ���1����AC��
	double tileStandardDiagonalAC_pix = 0;
	//�Խ���2����BD��
	double tileStandardDiagonalBD_pix = 0;


	//��ש��׼���
	double tileStandardWidth_mm = 300;
	//��ש��׼�߶�
	double tileStandardHeight_mm = 600;

	//һ�����ر�ʾ����mm
	double MilliMeterPerPix_Width = 0;
	double MilliMeterPerPix_Height = 0;
	double MilliMeterPerPix_Diagonal = 0;


	//����������

	//��Ƭӳ�䵽ʵ�� ����4096����ʾ��ƬX���ϣ�ÿ���㾭�궨��Ӧ��λ���ĸ�λ�á�
	vector<double> PincushionMap;
	//ʵ��ӳ�䵽��Ƭ������ȷ��ʵ��Xλ�ô������ص���ɣ�
	vector<double> PincushionMap_Anti;
	//��ֱ�궨��ƫ��ֵ������ֱ�����ϵĵ㣬ʵ��y����ӦΪy-RowAdjust[x]�������ĳ��x,y������Ӧ��(x,y+RowAdjust[x])��Ѱ��
	vector<int> RowAdjust;

	/**************************��׼����END***********************/

public:


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

	//���߱궨 ��ñ�׼�߶ȣ���ȣ�1pix�������mm
	void ObserveCalibration();
	//���ݱ궨����
	void ObserveImgAdjust(cv::Mat& Img);
	//�������ש�ߴ磬�ĸ��ǵĽǶ�
	void CaculteSize(BlocksDetector *bd);


	/**********************��������**********************/
	double AB_Len = 0;
	double CD_Len = 0;
	double AD_Len = 0;
	double BC_Len = 0;

	double& BA_Len = AB_Len;
	double& CB_Len = BC_Len;
	double& DA_Len = AD_Len;
	double& DC_Len = CD_Len;

	//�Խ��ߣ�pix��
	double AC_Len = 0;
	double BD_Len = 0;

	double& CA_Len = AC_Len;
	double& DB_Len = BD_Len;

	double AB_mm = 0;
	double CD_mm = 0;
	double AD_mm = 0;
	double BC_mm = 0;

	double& BA_mm = AB_mm;
	double& DC_mm = CD_mm;
	double& DA_mm = AD_mm;
	double& CB_mm = BC_mm;

	double angleA = 0;
	double angleB = 0;
	double angleC = 0;
	double angleD = 0;
};

