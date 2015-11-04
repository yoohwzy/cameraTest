#pragma once

#include "../stdafx.h"
#include "../globle_head.h"
#include "../globle_debug.h"
#include "BlocksDetector.h"



using namespace cv;

class EdgeDetector
{
	
public:
	
	EdgeDetector(Mat, BlocksDetector *bd); // 崩边检测
	void DistanceDetector(Vector<Point> Boundary_Point_temp, Vec4f FitLine, Vector<float> &Distance); // 边缘点到拟合直线的距离，距离存在向量Distance中
	int Distamce_MaxTabel(Vector<float> Distance);  // 边缘点到拟合直线的最大距离，返回集合中距离最大点的标号
	void DrawLine(int EdgeIndex, Mat src, Vec4f FitLine, int R, int G, int B); // 划线函数，EdgeIndex是用来判断是上下边缘划线还是左右边缘划线
	void PointOfIntersection(Vector<Vec4f>FitLine_Aggregate, Vector<Point> &Point_of_Intersection);  // FitLine_Aggregate是四条拟合直线的参数（左、上、右、下），Point_of_Intersection存储四条直线的交点（左上、右上、右下、左下）
	vector<Point3f> Defects; // 保存缺陷点（x，y，半径）

	


private:
	BlocksDetector *bd;

	Mat leftROI, upROI, rightROI, downROI; // 边缘ROI
	int xleft = 0, yleft = 0, left_height = 0, left_width = 0; // 左边缘ROI参数：ROI起点x、起点y、长、宽
	int xright = 0, yright = 0, right_height = 0, right_width = 0; // 右边ROI参数
	int xup = 0, yup = 0, up_height = 0, up_width = 0;  // 上边ROI参数
	int xdown = 0, ydown = 0, down_height = 0, down_width = 0; // 下边ROI参数

	int distance_threld = 5; // 点到拟合直线的距离阈值
	int Edge_threld = 16; // 判定崩边连续点点数量阈值，大于该值认为有连续的点异常，则认为崩边
	int tolerate_threld = 2; // 容忍度阈值


	float distance = 0;
	int t1 = 0, t2 = 0;
	int Tabel = 0;
};

