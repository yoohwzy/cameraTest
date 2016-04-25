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

	int Real_WidthMM = 600;//ͼ���д�ש�ĺ᳤
	int Real_LengthMM = 300;//ͼ���д�ש���ݳ�
	double Axis_X_mmPerPix = 1;//x�᷽����ÿ���ش�����ٺ���
	double Axis_Y_mmPerPix = 1;//y�᷽����ÿ���ش�����ٺ���

	//�����ĸ������ABCD�ĸ��� ���ĸ���λ�ò��ԣ�����false
	bool Lines2ABCD();
	//����ABCD�ĸ�������ĸ���
	bool ABCD2Lines();

	void ABCDAdjust();

	cv::Point GetPonintByX(int x, Line *l);
	cv::Point GetPonintByY(int y, Line *l);

	//ͼ���С
	int imageWidth = 0;
	int imageHeight = 0;
};

