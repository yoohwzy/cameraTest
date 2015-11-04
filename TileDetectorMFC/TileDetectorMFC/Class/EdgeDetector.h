#pragma once

#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "BlocksDetector.h"



using namespace cv;

class EdgeDetector
{
	
public:
	
	EdgeDetector(Mat, BlocksDetector *bd); // ���߼��
	void DistanceDetector(Vector<Point> Boundary_Point_temp, Vec4f FitLine, Vector<float> &Distance); // ��Ե�㵽���ֱ�ߵľ��룬�����������Distance��
	int Distamce_MaxTabel(Vector<float> Distance);  // ��Ե�㵽���ֱ�ߵ������룬���ؼ����о�������ı��
	void DrawLine(int EdgeIndex, Mat src, Vec4f FitLine, int R, int G, int B); // ���ߺ�����EdgeIndex�������ж������±�Ե���߻������ұ�Ե����
	void PointOfIntersection(Vector<Vec4f>FitLine_Aggregate, Vector<Point> &Point_of_Intersection);  // FitLine_Aggregate���������ֱ�ߵĲ��������ϡ��ҡ��£���Point_of_Intersection�洢����ֱ�ߵĽ��㣨���ϡ����ϡ����¡����£�
	vector<Point3f> Defects; // ����ȱ�ݵ㣨x��y���뾶��

	


private:
	BlocksDetector *bd;

	Mat leftROI, upROI, rightROI, downROI; // ��ԵROI
	int xleft = 0, yleft = 0, left_height = 0, left_width = 0; // ���ԵROI������ROI���x�����y��������
	int xright = 0, yright = 0, right_height = 0, right_width = 0; // �ұ�ROI����
	int xup = 0, yup = 0, up_height = 0, up_width = 0;  // �ϱ�ROI����
	int xdown = 0, ydown = 0, down_height = 0, down_width = 0; // �±�ROI����

	int distance_threld = 5; // �㵽���ֱ�ߵľ�����ֵ
	int Edge_threld = 16; // �ж������������������ֵ�����ڸ�ֵ��Ϊ�������ĵ��쳣������Ϊ����
	int tolerate_threld = 2; // ���̶���ֵ


	float distance = 0;
	int t1 = 0, t2 = 0;
	int Tabel = 0;
};

