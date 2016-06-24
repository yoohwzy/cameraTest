#pragma once

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


//���ڴ洢��ש��ͼ���е�λ����Ϣ
class Block
{
public:
	Block(int _imageWidth, int _imageHeight);
	~Block()
	{
		
	};
	struct Line
	{
		int x0;
		int y0;
		double k;
		double dx;
		double dy;
		bool isAllOutOfImg = false;//�Ƿ���λ��ͼƬ����
	};

	cv::Point A, B, C, D;

	Line LeftLine;
	Line RightLine;
	Line UpLine;
	Line DownLine;

	//�����ĸ������ABCD�ĸ��� ���ĸ���λ�ò��ԣ�����false
	bool Lines2ABCD();
	//����ABCD�ĸ�������ĸ���
	bool ABCD2Lines();

	cv::Point GetPonintByX(int x, Line *l);
	cv::Point GetPonintByY(int y, Line *l);

	double ab_mm;
	double bc_mm;
	double cd_mm;
	double da_mm;
	double ac_mm;
	double bd_mm;

	double ab_pix;
	double bc_pix;
	double cd_pix;
	double da_pix;
	double ac_pix;
	double bd_pix;
private:
	//ͼ���С
	int imageWidth = 0;
	int imageHeight = 0;
public:
	static int Standard_Width_mm;//��׼ש�᳤mm
	static int Standard_Length_mm;//��׼ש�ݳ�mm
	static double X_mmPerPix;//x�᷽����ÿ���ش�����ٺ���
	static double Y_mmPerPix;//y�᷽����ÿ���ش�����ٺ���

	//ֱ�Ӽ���Խ��ߵ����س���
	static void CalculateDuiJiaoXian(Block *p_b);
	//���ݶ������ݣ������ש�ı߳���
	static void Calculate(Block *p_b);
	//��֤��ֱ��һ���Ǵ�ֱʱ����
	static void BiaoDing(Block *p_b);
};

