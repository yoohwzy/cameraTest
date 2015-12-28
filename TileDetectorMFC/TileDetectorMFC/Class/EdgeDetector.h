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
	//vector<Point3f> Defects; // ����ȱ�ݵ㣨x��y���뾶��

	cv::Mat grayImg;
	cv::Mat ThreshodImgHigh;
	cv::Mat ThreshodImgLow;
	int ThreshodHigh;
	int ThreshodLow;

private:
	Block *p_block = NULL;
	Faults *p_faults = NULL;
	// ��Ե�㼯�ϵ����ֱ�ߵľ��룬�����������Distance��
	void DistanceDetector_set(vector<Point> Boundary_Point_temp, Vec4f FitLine, vector<float> &Distance);
	// ��Ե�㵽���ֱ�ߵľ���
	int DistanceDetector(Point Boundary_Point_temp, Vec4f FitLine);
	// ��Ե�㵽���ֱ�ߵ������룬���ؼ����о�������ı��
	int Distamce_MaxTabel(vector<float> Distance);
	// ���ߺ�����EdgeIndex�������ж������±�Ե���߻������ұ�Ե����
	void DrawLine(int EdgeIndex, Mat src, Vec4f FitLine, int R, int G, int B);
	// �����ֱ�ߵĽ���
	void PointOfIntersection(vector<Vec4f>FitLine_Aggregate, vector<Point> &Point_of_Intersection);
	// Ԥ����ROI����ȡ��̬��Χ
	void Dynamic_range(Mat &image);
	// Ѱ�ұ�Ե�㣬ContoursΪԭͼ��Ե��ROI_ContoursΪROI��Ե
	void Find_contours(vector<Mat> image, vector<vector<Point>> &Contours, vector<vector<Point>> &ROI_Contours);
	// ���ٽ��ĵ�ϲ�
	void Merge_Defects(vector<Point3f> &Defects);
	// ��Ե�㸽����ȱ�ݼ��,�쳣�����Blocks�У��쳣���������local_��
	void Blocks_Defects(vector<Mat> roi, vector<Vec4f> line, vector<Mat> &Blocks, vector<Point3f> &local_);
	// ֱ�����
	void FitLine(vector<vector<Point>> &Fit_contours, vector<Vec4f> &line_, vector<Vec4f> &line_roi, Vec4f &Fit_Line);

	Mat src;
	vector<Mat> ROI;
	//vector<Mat> ROI1;
	// ��ԵROI
	Mat leftROI, upROI, rightROI, downROI;
	Point A, B, C, D;
	// ���ԵROI������ROI���x�����y��������
	int xleft = 0, yleft = 0, left_height = 0, left_width = 0;
	// �ұ�ROI����
	int xright = 0, yright = 0, right_height = 0, right_width = 0;
	// �ϱ�ROI����
	int xup = 0, yup = 0, up_height = 0, up_width = 0;
	// �±�ROI����
	int xdown = 0, ydown = 0, down_height = 0, down_width = 0;

	// �㵽���ֱ�ߵľ�����ֵ
	int distance_threld = 12;
	// �ж������������������ֵ�����ڸ�ֵ��Ϊ�������ĵ��쳣������Ϊ����
	int Edge_threld = 8;
	// ���̶���ֵ
	//int seat = 6;
	int simple = 5;
	int Pixel_threld = 15;

	int contours_threld = 31;
	int Dynamic_range_min = 5;
	float Dynamic_range_max = 0.9;

	float distance = 0;
	int t1 = 0, t2 = 0;
	int Tabel = 0;

	//����ROI����Χ����ֹԽ��
	void RectAdjust(cv::Mat img,int& x, int& y, int& width, int& height);


	bool finalJudge(cv::Point center, int width, int height);
};

