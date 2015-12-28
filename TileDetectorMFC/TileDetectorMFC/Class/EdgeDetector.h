#pragma once

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "BlocksDetector.h"

#include "Base/Block.h"
#include "Base\Faults.h"


using namespace cv;

class EdgeDetector
{
	
public:

	EdgeDetector(Mat&, Block *_block,Faults *_faults);
	~EdgeDetector()
	{
		p_block = NULL;
		p_faults = NULL;
	};
	void start();
	//vector<Point3f> Defects; // 保存缺陷点（x，y，半径）

	cv::Mat grayImg;
	cv::Mat ThreshodImgHigh;
	cv::Mat ThreshodImgLow;
	int ThreshodHigh;
	int ThreshodLow;

private:
	Block *p_block = NULL;
	Faults *p_faults = NULL;
	// 边缘点集合到拟合直线的距离，距离存在向量Distance中
	void DistanceDetector_set(vector<Point> Boundary_Point_temp, Vec4f FitLine, vector<float> &Distance);
	// 边缘点到拟合直线的距离
	int DistanceDetector(Point Boundary_Point_temp, Vec4f FitLine);
	// 边缘点到拟合直线的最大距离，返回集合中距离最大点的标号
	int Distamce_MaxTabel(vector<float> Distance);
	// 划线函数，EdgeIndex是用来判断是上下边缘划线还是左右边缘划线
	void DrawLine(int EdgeIndex, Mat src, Vec4f FitLine, int R, int G, int B);
	// 求拟合直线的交点
	void PointOfIntersection(vector<Vec4f>FitLine_Aggregate, vector<Point> &Point_of_Intersection);
	// 预处理ROI，获取动态范围
	void Dynamic_range(Mat &image);
	// 寻找边缘点，Contours为原图边缘，ROI_Contours为ROI边缘
	void Find_contours(vector<Mat> image, vector<vector<Point>> &Contours, vector<vector<Point>> &ROI_Contours);
	// 将临近的点合并
	void Merge_Defects(vector<Point3f> &Defects);
	// 边缘点附近的缺陷检测,异常块存入Blocks中，异常块坐标存入local_中
	void Blocks_Defects(vector<Mat> roi, vector<Vec4f> line, vector<Mat> &Blocks, vector<Point3f> &local_);
	// 直线拟合
	void FitLine(vector<vector<Point>> &Fit_contours, vector<Vec4f> &line_, vector<Vec4f> &line_roi, Vec4f &Fit_Line);

	Mat src;
	vector<Mat> ROI;
	//vector<Mat> ROI1;
	// 边缘ROI
	Mat leftROI, upROI, rightROI, downROI;
	Point A, B, C, D;
	// 左边缘ROI参数：ROI起点x、起点y、长、宽
	int xleft = 0, yleft = 0, left_height = 0, left_width = 0;
	// 右边ROI参数
	int xright = 0, yright = 0, right_height = 0, right_width = 0;
	// 上边ROI参数
	int xup = 0, yup = 0, up_height = 0, up_width = 0;
	// 下边ROI参数
	int xdown = 0, ydown = 0, down_height = 0, down_width = 0;

	// 点到拟合直线的距离阈值
	int distance_threld = 12;
	// 判定崩边连续点点数量阈值，大于该值认为有连续的点异常，则认为崩边
	int Edge_threld = 8;
	// 容忍度阈值
	//int seat = 6;
	int simple = 5;
	int Pixel_threld = 15;

	int contours_threld = 31;
	int Dynamic_range_min = 5;
	float Dynamic_range_max = 0.9;

	float distance = 0;
	int t1 = 0, t2 = 0;
	int Tabel = 0;

	//调整ROI区域范围，防止越界
	void RectAdjust(cv::Mat img,int& x, int& y, int& width, int& height);


	bool finalJudge(cv::Point center, int width, int height);
};

