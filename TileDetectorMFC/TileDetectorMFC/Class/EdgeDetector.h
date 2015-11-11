#pragma once

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "BlocksDetector.h"

#include "Base/Block.h"

/*2015��10��10��*/

using namespace cv;

class EdgeDetector
{
	
public:
	
	EdgeDetector(Mat, Block *_block);
	~EdgeDetector();
	void Defect_Detector(vector < vector<Point> >contours_, vector<Vec4f> line_, vector<Point3f> &defects);
	void Defect_Detector(Mat src, vector<Vec4f> line_, vector<Point3f> &defects);
	void classifying(vector < vector<Point> > contours, vector<Vec4f> line_, vector < vector<Point> > &contours_);
	void start();
	void DistanceDetector_set(Vector<Point> Boundary_Point_temp, Vec4f FitLine, Vector<float> &Distance); // ��Ե�㵽���ֱ�ߵľ��룬�����������Distance��
	int DistanceDetector(Point Boundary_Point_temp, Vec4f FitLine); // ��Ե�㵽���ֱ�ߵľ���
	int Distamce_MaxTabel(Vector<float> Distance);  // ��Ե�㵽���ֱ�ߵ������룬���ؼ����о�������ı��
	void DrawLine(int EdgeIndex, Mat src, Vec4f FitLine, int R, int G, int B); // ���ߺ�����EdgeIndex�������ж������±�Ե���߻������ұ�Ե����
	void PointOfIntersection(Vector<Vec4f>FitLine_Aggregate, Vector<Point> &Point_of_Intersection);  // FitLine_Aggregate���������ֱ�ߵĲ��������ϡ��ҡ��£���Point_of_Intersection�洢����ֱ�ߵĽ��㣨���ϡ����ϡ����¡����£�
	vector<Point3f> Defects; // ����ȱ�ݵ㣨x��y���뾶��


	


private:
	Block *block;
	Mat src;
	vector<Mat> ROI, ROI1;
	Mat leftROI, upROI, rightROI, downROI; // ��ԵROI
	Point A, B, C, D;
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

