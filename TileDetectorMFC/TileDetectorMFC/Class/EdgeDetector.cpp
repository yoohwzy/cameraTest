#include "EdgeDetector.h"
#include "Processor.h"

EdgeDetector::EdgeDetector(Mat& img, Block *_block, Faults *_faults)
{
	p_block = _block;
	p_faults = _faults;
	_block = NULL;
	_faults = NULL;
	src = img;

	
	A = p_block->A;

	B = p_block->B;

	C = p_block->C;

	D = p_block->D;

	xleft = (A.x - abs(A.x - D.x) - 100 > 0 ? (A.x - abs(A.x - D.x) - 100) : 0);
	yleft = A.y - 100 < 0 ? 0 : A.y - 100;
	left_height = D.y - A.y + 200;
	left_width = 2 * abs(A.x - D.x) + 200;
	RectAdjust(img, xleft, yleft, left_width, left_height);

	xright = B.x - abs(B.x - C.x) - 100;
	yright = B.y - 100;
	right_height = C.y - B.y + 200;
	right_width = min(2 * abs(B.x - C.x) + 200, 4096 - B.x + abs(B.x - C.x));
	RectAdjust(img, xright, yright, right_width, right_height);

	xup = A.x - 100;
	yup = (A.y - abs(A.y - B.y) - 100 > 0 ? (A.y - abs(A.y - B.y) - 100) : 0);
	up_width = B.x - A.x + 200;
	up_height = 2 * abs(A.y - B.y) + 200;
	RectAdjust(img, xup, yup, up_width, up_height);

	xdown = D.x - 100;
	ydown = D.y - abs(D.y - C.y) - 100;
	down_width = C.x - D.x + 200;
	down_height = min(2 * abs(D.y - C.y) + 200, 11000 - D.y + abs(D.y - C.y));
	RectAdjust(img, xdown, ydown, down_width, down_height);

	src(Rect(xleft, yleft, left_width, left_height)).copyTo(leftROI);
	if (leftROI.channels() == 3)
		cvtColor(leftROI, leftROI, CV_BGR2GRAY);
	src(Rect(xright, yright, right_width, right_height)).copyTo(rightROI);
	if (rightROI.channels() == 3)
		cvtColor(rightROI, rightROI, CV_BGR2GRAY);
	src(Rect(xup, yup, up_width, up_height)).copyTo(upROI);
	if (upROI.channels() == 3)
		cvtColor(upROI, upROI, CV_BGR2GRAY);
	src(Rect(xdown, ydown, down_width, down_height)).copyTo(downROI);
	if (downROI.channels() == 3)
		cvtColor(downROI, downROI, CV_BGR2GRAY);


	//Mat leftROI1, rightROI1, upROI1, downROI1;
	//leftROI.copyTo(leftROI1);
	//rightROI.copyTo(rightROI1);
	//upROI.copyTo(upROI1);
	//downROI.copyTo(downROI1);
	/*leftROI=src(Rect(xleft, yleft, left_width, left_height));
	rightROI=src(Rect(xright, yright, right_width, right_height));
	upROI=src(Rect(xup, yup, up_width, up_height));
	downROI=src(Rect(xdown, ydown, down_width, down_height));*/

	/*imwrite("C:\\Users\\43597_000\\Desktop\\leftroi.jpg", leftROI);
	imwrite("C:\\Users\\43597_000\\Desktop\\rightroi.jpg", rightROI);
	imwrite("C:\\Users\\43597_000\\Desktop\\uproi.jpg", upROI);
	imwrite("C:\\Users\\43597_000\\Desktop\\downroi.jpg", downROI);*/

	ROI.push_back(leftROI);
	ROI.push_back(downROI);
	ROI.push_back(rightROI);
	ROI.push_back(upROI);

	//ROI1.push_back(leftROI1);
	//ROI1.push_back(downROI1);
	//ROI1.push_back(rightROI1);
	//ROI1.push_back(upROI1);


}

void EdgeDetector::start()
{
	/*double t2 = (double)cv::getTickCount();

	double t = (double)cv::getTickCount();*/
	// 预处理
	for (int i = 0; i < ROI.size(); i++)
	{
		Dynamic_range(ROI[i]);
	}
	/*t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << "预处理time=" << t << "\t";

	t = (double)cv::getTickCount();*/
	// 获取边缘
	vector<vector<Point>> Contours;// 原图的边缘坐标
	vector<vector<Point>> ROI_Contours; // ROI上的边缘坐标
	Find_contours(ROI, Contours, ROI_Contours); // 找到的边缘对应于原图和ROI分别保存
	/*t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << "获取边缘time=" << t << "\t";


	t = (double)cv::getTickCount();*/
	// 取边缘样点用于拟合
	vector<vector<Point>> Fit_contours; // 存储样点用于拟合

	for (int i = 0; i < ROI_Contours.size(); i++)
	{
		vector<Point> tm;
		for (int j = 50; j < ROI_Contours[i].size() - 50; j = j + 50) // 每隔50个点取一个
		{
			tm.push_back(ROI_Contours[i][j]);
		}
		Fit_contours.push_back(tm);
	}
	/*t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << "取边缘样点用于拟合time=" << t << "\t";


	t = (double)cv::getTickCount();*/
	// 边缘直线拟合
	vector<Vec4f> line_;   // 拟合后原图上的直线
	vector<Vec4f> line_roi; // 拟合后roi上的直线
	Vec4f Fit_Line;
	FitLine(Fit_contours, line_, line_roi, Fit_Line);	 // 直线拟合
	/*t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << "边缘直线拟合time=" << t << "\t";

	t = (double)cv::getTickCount();*/
	// 求出四个交点A\B\C\D
	vector<Point> Point_of_Intersection;
	PointOfIntersection(line_, Point_of_Intersection);
	p_block->A = Point_of_Intersection[3];
	p_block->B = Point_of_Intersection[2];
	p_block->C = Point_of_Intersection[1];
	p_block->D = Point_of_Intersection[0];

	vector<Point> corner;
	corner.push_back(p_block->A);
	corner.push_back(p_block->D);
	corner.push_back(p_block->C);
	corner.push_back(p_block->B);

	/*t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << "求出四个交点time=" << t << "\t";*/

	/*Mat image1(src.size(), src.type(),Scalar(0));
	for (int i = 0; i < Contours.size(); i++)
	{
	for (int j = 0; j < Contours[i].size(); j++)
	{
	image1.at<uchar>(Contours[i][j]) = 255;
	}
	}*/


	/*t = (double)cv::getTickCount();*/
	// 崩边检测
	int starter = 0, ender = 0; // 一个边缘点集合的起始点和结束点
	int aa = 0, bb = 0, cc = 0, dd = 0;
	int aa1 = 0, bb1 = 0, cc1 = 0, dd1 = 0;

	// 瓷砖向右歪斜
	if ((p_block->A).y > (p_block->B).y)
	{
		aa1 = ROI_Contours[0].size();	bb1 = ROI_Contours[1].size();	cc = 0;	dd = 0;
		for (int i = 0; i < 50; i++)
		{
			int t0 = abs(Contours[0][i].x - Contours[0][i + 1].x);
			int t1 = abs(Contours[0][i].x - (p_block->A).x);
			if (t1 < 30){
				aa = i;		break;
			}
			else
				if (i == 49)	aa = 49;
		}

		for (int i = 0; i < 50; i++)
		{
			int t0 = abs(Contours[1][i].y - Contours[1][i + 1].y);
			int t1 = abs(Contours[1][i].y - (p_block->D).y);
			if (t1 < 30){
				bb = i;	break;
			}
			else
				if (i == 49)	bb = 49;
		}

		for (int i = ROI_Contours[2].size() - 2; i > ROI_Contours[2].size() - 50; i--)
		{
			int t0 = abs(Contours[2][i].x - Contours[2][i + 1].x);
			int t1 = abs(Contours[2][i].x - (p_block->C).x);
			if (abs(Contours[2][i].x - (p_block->C).x) < 30)	{
				cc1 = i;		break;
			}
			else
				if (i == ROI_Contours[2].size() - 49)
					cc1 = ROI_Contours[2].size() - 49;
		}


		for (int i = ROI_Contours[3].size() - 2; i > ROI_Contours[3].size() - 50; i--)
		{
			int t0 = abs(Contours[3][i].y - Contours[3][i + 1].y);
			int t1 = abs(Contours[3][i].y - (p_block->B).y);
			if (t1 < 30){
				dd1 = i;	break;
			}
			else
				if (i == ROI_Contours[3].size() - 49)
					dd1 = ROI_Contours[3].size() - 49;
		}
	}

	else  // 瓷砖向左歪斜
	{
		aa = 0; bb = 0;	cc1 = ROI_Contours[2].size();	dd1 = ROI_Contours[3].size();
		for (int i = ROI_Contours[0].size() - 2; i > ROI_Contours[0].size() - 50; i--)
		{
			if (abs(Contours[0][i].x - (p_block->D).x) < 30){
				aa1 = i;		break;
			}
			else
				if (i == ROI_Contours[0].size() - 49)	aa1 = ROI_Contours[0].size() - 49;
		}

		for (int i = ROI_Contours[1].size() - 2; i > ROI_Contours[1].size() - 50; i--)
		{
			if (abs(Contours[1][i].y - (p_block->C).y) < 30){ bb1 = i;		break; }
			else
				if (i == ROI_Contours[1].size() - 49)	bb1 = ROI_Contours[1].size() - 49;
		}

		for (int i = 0; i <50; i++)
		{
			if (abs(Contours[2][i].x - (p_block->B).x) < 30)	{
				cc = i;		break;
			}
			else
				if (i == 49)
					cc = 49;
		}


		for (int i = 0; i <50; i++)
		{
			if (abs(Contours[3][i].y - (p_block->A).y)<30)	{
				dd = i;		break;
			}
			else
				if (i == 49)
					dd = 49;
		}
	}


	// 崩边检测
	for (int index = 0; index < ROI_Contours.size(); index++)
	{
		int xp = 0, yp = 0;
		int st = 0, et = ROI_Contours[index].size();;
		switch (index)
		{
		case 0:	st = aa;	 et = aa1;	xp = xleft;	yp = yleft;	break;
		case 1:	st = bb;	 et = bb1;	xp = xdown;	yp = ydown;	break;
		case 2:	st = cc;     et = cc1;	xp = xright;	yp = yright;	break;
		case 3:	st = dd;	 et = dd1;	xp = xup;	yp = yup;	break;
		}



		for (int i = st; i < et; i += simple)  // 间隔simple个点采样检测
		{
			int tt = ROI_Contours[index][i].x;
			int tt1 = ROI_Contours[index][i].y;
			starter = st;	ender = 0;
			int dist = DistanceDetector(ROI_Contours[index][i], line_roi[index]);
			if (dist >= distance_threld)
			{
				if (i < st + 20)
					starter = 0;
				else
					starter = i - 19;
				if (starter > ROI_Contours[index].size() - 30)
					ender = ROI_Contours[index].size() - 1;
				else
					for (int q = i + 5; q < ROI_Contours[index].size(); q += 5)
					{
						int dist = DistanceDetector(ROI_Contours[index][q], line_roi[index]);
						if (dist < distance_threld)
						{
							ender = q + 20;
							i = q + 20;
							if (i>ROI_Contours[index].size() - 7)
							{
								i = ROI_Contours[index].size() - 1;
								ender = ROI_Contours[index].size() - 1;
							}
							break;
						}
					}
				if ((ender - starter > 39 + distance_threld) || (starter == st) || (ender == ROI_Contours[index].size() - 1))
				{
					/////////////////////////////////////////////////////////////////
					//cv::circle(ROI[index], ROI_Contours[index][starter + (ender - starter) / 2], (ender - starter) / 2, Scalar(255));
					/////////////////////////////////////////////////////////////////
					int flag = 0;
					int edge_deep = 0;
					for (int a = starter; a < ender; a++)
					{
						int dist = DistanceDetector(ROI_Contours[index][a], line_roi[index]);
						if (edge_deep < dist)
							edge_deep = dist;
						if (dist >= distance_threld)
						{
							int tt = ROI_Contours[index][a].x;
							int tt1 = ROI_Contours[index][a].y;
							flag++;
						}
					}

					if (flag > Edge_threld)
					{
						flag = 0;
						// 边角判别
						Point location;
						location.x = ROI_Contours[index][starter + (ender - starter) / 2].x + xp;
						location.y = ROI_Contours[index][starter + (ender - starter) / 2].y + yp;
						int length = (ender - starter);

						int current = index;
						int next = index + 1;
						if (next == 4) next = 0;
						double corner_distance_fir = sqrt(pow((location.x - corner[index].x), 2) + pow((location.y - corner[index].y), 2)) - length / 2;
						double corner_distance_sec = sqrt(pow((location.x - corner[next].x), 2) + pow((location.y - corner[next].y), 2)) - length / 2;
						if (corner_distance_fir < 10 || corner_distance_sec < 10)
						{
							Faults::BrokenCorner bc;
							bc.position = location;
							if (current % 2)
							{
								bc.width = length;
								bc.length = edge_deep;
							}
							else
							{
								bc.length = length;
								bc.width = edge_deep;
							}
							bc.deep = bc.length*bc.width / sqrt(pow(bc.length, 2) + pow(bc.width, 2));
							p_faults->BrokenCorners.push_back(bc);
							cv::circle(src, Point(bc.position.x, bc.position.y), bc.length, Scalar(255, 255, 250));
							break;
						}
						else
						{
							Faults::BrokenEdge be;
							/*Faults fs;*/
							be.position.x = ROI_Contours[index][starter + (ender - starter) / 2].x + xp;
							be.position.y = ROI_Contours[index][starter + (ender - starter) / 2].y + yp;
							if (index % 2)
							{
								be.length = (ender - starter)/**(double)(fs.GetMilliMeterPerPix_X())*/;
								be.deep = edge_deep/**(double)(fs.GetMilliMeterPerPix_Y())*/;
							}
							else
							{
								be.length = (ender - starter)/**(double)(fs.GetMilliMeterPerPix_Y())*/;
								be.deep = edge_deep/**(double)(fs.GetMilliMeterPerPix_X())*/;
							}

							p_faults->BrokenEdges.push_back(be);
							cv::circle(src, Point(be.position.x, be.position.y), be.length, Scalar(255, 255, 250));
							break;
						}
					}
				}
			}
		}
	}
	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//cout << "崩边检测time=" << t << endl;
	//cout << "start time= " << ((double)cv::getTickCount() - t2) / cv::getTickFrequency() << endl;
}
void EdgeDetector::DrawLine(int EdgeIndex, Mat src, Vec4f FitLine, int R, int G, int B)
{
	if (EdgeIndex % 2)
	{
		int x1 = 0;
		int y1 = FitLine[3] - FitLine[1] * FitLine[2] / FitLine[0];
		int x2 = 4096;
		int y2 = FitLine[1] * x2 / FitLine[0] + FitLine[3] - FitLine[1] * FitLine[2] / FitLine[0];
		cv::line(src, Point(x1, y1), Point(x2, y2), Scalar(R, G, B));
	}
	else
	{
		int y1 = 0;
		int x1 = FitLine[2] - FitLine[0] / FitLine[1] * FitLine[3];
		int y2 = 11000;
		int x2 = (y2 - FitLine[3])*FitLine[0] / FitLine[1] + FitLine[2];
		line(src, Point(x1, y1), Point(x2, y2), Scalar(R, G, B));
	}
}

void EdgeDetector::DistanceDetector_set(vector<Point> Boundary_Point_temp, Vec4f FitLine, vector<float> &Distance)
{
	//float distance = 0;
	for (int Boundary_Point_tempIndex = 0; Boundary_Point_tempIndex < Boundary_Point_temp.size(); Boundary_Point_tempIndex++)
	{
		distance = (abs((Boundary_Point_temp[Boundary_Point_tempIndex].x - FitLine[2])*FitLine[1] / FitLine[0] + FitLine[3] - Boundary_Point_temp[Boundary_Point_tempIndex].y))
			/ (sqrt(1 + (FitLine[1] / FitLine[0])*(FitLine[1] / FitLine[0])));
		Distance.push_back(distance);
	}
}

int EdgeDetector::DistanceDetector(Point Boundary_Point_temp, Vec4f FitLine)
{
	float Distance = 0;
	Distance = (abs((Boundary_Point_temp.x - FitLine[2])*FitLine[1] / FitLine[0] + FitLine[3] - Boundary_Point_temp.y))
		/ (sqrt(1 + (FitLine[1] / FitLine[0])*(FitLine[1] / FitLine[0])));
	return Distance;
}

void EdgeDetector::PointOfIntersection(vector<Vec4f>FitLine_Aggregate, vector<Point> &Point_of_Intersection)
{
	for (int FitLine_Aggregate_Index = 0; FitLine_Aggregate_Index < FitLine_Aggregate.size(); FitLine_Aggregate_Index++)
	{
		t1 = FitLine_Aggregate_Index;
		t2 = FitLine_Aggregate_Index + 1;
		Point point;
		if (t2 == 4) t2 = 0;
		float xcols = 0, yrows = 0;
		if (FitLine_Aggregate[t1][0] == 0)
		{
			yrows = FitLine_Aggregate[t1][3];
			xcols = (yrows - FitLine_Aggregate[t2][3]) *FitLine_Aggregate[t2][0] / FitLine_Aggregate[t2][1] + FitLine_Aggregate[t2][2];
		}
		else
		{
			xcols = (FitLine_Aggregate[t2][3] - FitLine_Aggregate[t2][1] * FitLine_Aggregate[t2][2] / FitLine_Aggregate[t2][0] -
				FitLine_Aggregate[t1][3] + FitLine_Aggregate[t1][1] * FitLine_Aggregate[t1][2] / FitLine_Aggregate[t1][0]) /
				(FitLine_Aggregate[t1][1] / FitLine_Aggregate[t1][0] - FitLine_Aggregate[t2][1] / FitLine_Aggregate[t2][0]);
			yrows = FitLine_Aggregate[t1][1] * xcols / FitLine_Aggregate[t1][0] + FitLine_Aggregate[t1][3]
				- FitLine_Aggregate[t1][1] * FitLine_Aggregate[t1][2] / FitLine_Aggregate[t1][0];
		}
		point.x = xcols;
		point.y = yrows;
		Point_of_Intersection.push_back(point);
	}
}

int EdgeDetector::Distamce_MaxTabel(vector<float> Distance)
{
	float Temp = -1;
	for (int DistanceIndex = 0; DistanceIndex < Distance.size(); DistanceIndex++)
	{
		if (Temp <= Distance[DistanceIndex])
		{
			Temp = Distance[DistanceIndex];
			Tabel = DistanceIndex;
		}
	}
	return Tabel;
}

void EdgeDetector::Dynamic_range(Mat &image)
{
	int graymax = 0;
	for (int j = 0; j < image.rows; j++)
	{
		uchar *data = image.ptr(j);
		for (int k = 0; k < image.cols; k++)
		{
			if (data[k]>graymax)
				graymax = data[k];
		}
	}

	for (int j = 0; j <image.rows; j++)
	{
		uchar *data = image.ptr<uchar>(j);
		for (int k = 0; k < image.cols; k++)
		{
			if (data[k] < Dynamic_range_min)
				data[k] = 0;
			else
			{
				if (data[k]>(graymax*Dynamic_range_max))
					data[k] = 255;
				else
				{
					int a = 255 / (graymax*Dynamic_range_max - Dynamic_range_min);
					int b = (-1)*Dynamic_range_min * a;
					data[k] = data[k] * a + b;
				}
			}
		}
	}
	//dilate(ROI[i], ROI[i], Mat()); // 膨胀
}

void EdgeDetector::Find_contours(vector<Mat> image, vector<vector<Point>> &Contours, vector<vector<Point>> &ROI_Contours)
{
	for (int index = 0; index < image.size(); index++)
	{
		vector<Point> contours_temp;
		vector<Point> contours_temp_roi;
		int flag = 1;
		int t1 = 0, t2 = 0;
		int xp = 0, yp = 0;

		if (index % 2 == 0) // 左右ROI
		{

			if (index == 0) // 左边ROI
			{
				t1 = 0;	t2 = image[index].cols;	xp = xleft; yp = yleft;
			}
			else // 右边ROI
			{
				t1 = -1 * image[index].cols + 1;	t2 = 0;	xp = xright;	yp = yright;
			}

			for (int i = 0; i < image[index].rows; i++) 
			{
				flag = 1;
				const uchar *data = image[index].ptr<uchar>(i);
				for (int j = t1; j < t2; j++)
				{
					if (data[abs(j)] >= Pixel_threld)
					{
						flag = 1;
						for (int p = j + 1; p < j + contours_threld; p++)
						{
							if (data[abs(p)] < Pixel_threld)
							{
								j = p + 1;
								break;
							}
							else
								flag++;
						}
						if (flag == contours_threld)
						{
							Point temp;
							temp.y = i;
							temp.x = abs(j);
							contours_temp_roi.push_back(temp);
							temp.y = temp.y + yp;
							temp.x = temp.x + xp;
							contours_temp.push_back(temp);
							break;
						}

					}
				}

			}

		}

		else
		{
			if (index == 1)
			{
				t1 = -1 * image[index].rows + 1;	t2 = 0;	xp = xdown;	yp = ydown;
			}
			else
			{
				t1 = 0;	t2 = image[index].rows - 1;	xp = xup;	yp = yup;
			}
			for (int j = 0; j < image[index].cols; j++)
			{

				for (int i = t1; i < t2; i++)
				{
					flag = 1;
					const uchar *data = image[index].ptr<uchar>(abs(i));

					if (data[j] >= Pixel_threld)
					{
						flag = 1;

						for (int p = i + 1; p < i + contours_threld; p++)
						{
							if (p == abs(t2))
								break;
							const uchar *data = image[index].ptr<uchar>(abs(p));
							if (data[j] < Pixel_threld)
							{
								i = p + 1;
								break;
							}
							else
								flag++;
						}
					}
					if (flag == contours_threld)
					{
						Point temp;
						temp.y = abs(i);
						temp.x = j;
						contours_temp_roi.push_back(temp);
						temp.y = abs(i) + yp;
						temp.x = j + xp;
						contours_temp.push_back(temp);
						break;
					}

				}
			}
		}



		Contours.push_back(contours_temp);
		ROI_Contours.push_back(contours_temp_roi);
		contours_temp.clear();
		contours_temp_roi.clear();
	}
}

void EdgeDetector::FitLine(vector<vector<Point>> &Fit_contours, vector<Vec4f> &line_, vector<Vec4f> &line_roi, Vec4f &Fit_Line)
{
	for (int i = 0; i < Fit_contours.size(); i++)
	{
		if (!Fit_contours[i].empty())
		{
			for (int j = 0; j < 10; j++)
			{
				vector<float> Distance;
				fitLine(Mat(Fit_contours[i]), Fit_Line, CV_DIST_L2, 0, 1, 1);
				//

				DistanceDetector_set(Fit_contours[i], Fit_Line, Distance);
				int Tabel = Distamce_MaxTabel(Distance);
				if (Distance[Tabel]>5)
				{
					//
					//					circle(image, Fit_contours[i][Tabel], 20, CV_RGB(160, 0, 50 + i * 50), 1);
					Fit_contours[i].erase(Fit_contours[i].begin() + Tabel); // 擦除奇异点
				}
				else
					break;
			}
			line_roi.push_back(Fit_Line);
			/////////////////////////////////////////////////////
			/*DrawLine(i, ROI[i], Fit_Line, 200, 200, 100);*/
			/////////////////////////////////////////////////////
			int t1 = 0, t2 = 0;
			switch (i)
			{
			case 0: t1 = xleft; t2 = yleft; break;
			case 1: t1 = xdown; t2 = ydown; break;
			case 2: t1 = xright; t2 = yright; break;
			case 3: t1 = xup; t2 = yup; break;
			}
			Fit_Line[2] += t1;
			Fit_Line[3] += t2;
			DrawLine(i, src, Fit_Line, 200, 200, 100);
			/*DrawLine(i, image1, Fit_Line, 200, 200, 100);*/
			line_.push_back(Fit_Line);
		}
	}
}

void EdgeDetector::Merge_Defects(vector<Point3f> &Defects)
{
	if (Defects.size() > 1)
	{
		for (int i = 0; i < Defects.size(); i++)
		{
			for (int j = i + 1; j < Defects.size(); j++)
			{
				int distan = sqrt(pow((Defects[i].x - Defects[j].x), 2) + pow((Defects[i].y - Defects[j].y), 2));
				if (distan <= (Defects[i].z + Defects[j].z + 100))
				{
					Defects[i].x = (Defects[i].x + Defects[j].x) / 2;
					Defects[i].y = (Defects[i].y + Defects[j].y) / 2;
					Defects[i].z = Defects[i].z + Defects[j].z + distan / 2;
					Defects.erase(Defects.begin() + j);
					j--;
				}
			}
		}
	}
}

void EdgeDetector::Blocks_Defects(vector<Mat> roi, vector<Vec4f> line_1, vector<Mat> &Blocks, vector<Point3f> &local_)
{
	int ylA = (p_block->A).y - yleft, ylD = (p_block->D).y - yleft;
	int xdD = (p_block->D).x - xdown, xdC = (p_block->C).x - xdown;
	int yrB = (p_block->B).y - yright, yrC = (p_block->C).y - yright;
	int xuA = (p_block->A).x - xup, xuB = (p_block->B).x - xup;
	int st = 0, st1 = 0;
	int fir = 0, las = 0, p = 0;
	int xp = 0, yp = 0;
	int Pixel_t = 255;
	for (int index = 0; index < roi.size(); index++)
	{
		switch (index)
		{
		case 0:	fir = ylA + 100;	las = ylD - 100;	p = 15;		Pixel_t = 40;	 xp = xleft;	 yp = yleft;	 break;
		case 1:	fir = xdD + 100;	las = xdC - 100;	p = -15;	Pixel_t = 15;	 xp = xdown;	 yp = ydown;	 break;
		case 2:	fir = yrB + 100;	las = yrC - 100;	p = -15;	Pixel_t = 40;	 xp = xright;	 yp = yright;	 break;
		case 3:	fir = xuA + 100;	las = xuB - 100;	p = 15;		Pixel_t = 15;	 xp = xup;		 yp = yup;		 break;
		}
		for (int i = fir; i < las; i += 5)
		{
			int j = 0;
			Point tm;
			Point tm1;
			if (index % 2 == 0)
			{
				j = line_1[index][0] / line_1[index][1] * (i - line_1[index][3]) + line_1[index][2];
				int j1 = line_1[index][0] / line_1[index][1] * (i + 5 - line_1[index][3]) + line_1[index][2];
				tm1.x = j1 + p;
				tm1.y = i + 5;
				tm.x = j + p;
				tm.y = i;
			}

			else
			{
				j = line_1[index][1] / line_1[index][0] * (i - line_1[index][2]) + line_1[index][3];
				int j0 = line_1[index][1] / line_1[index][0] * (i + 5 - line_1[index][2]) + line_1[index][3];
				tm1.x = i + 5;
				tm1.y = j0 + p;
				tm.x = i;
				tm.y = j + p;
			}


			Point temp;
			Point3f temp1;
			int tt = 0, tt1 = 0;
			if (abs(ROI[index].at<uchar>(tm)-ROI[index].at<uchar>(tm1)) > Pixel_t)
			{
				st = i - 30;
				if (index % 2 == 0)
				{
					st1 = line_1[index][0] / line_1[index][1] * (st - line_1[index][3]) + line_1[index][2];
					tt = min(50, ROI[index].cols - st1);
					if (index == 0)
					{
						temp.x = st1;
						temp.y = st;
					}
					else
					{
						temp.x = st1 - tt;
						temp.y = st;
					}
					temp1.x = st1 + xp;
					temp1.y = st + 45 + yp;
					temp1.z = tt;
					tt1 = 90;
				}
				else
				{
					st1 = line_1[index][1] / line_1[index][0] * (st - line_1[index][2]) + line_1[index][3];
					tt = 90;
					tt1 = min(50, ROI[index].rows - st1);
					if (index == 1)
					{
						temp.x = st;
						temp.y = st1 - tt1;
					}
					else
					{
						temp.x = st;
						temp.y = st1;
					}
					temp1.x = st + 45 + xp;
					temp1.y = st1 + yp;
					temp1.z = tt1;
				}
				i += 65;
				local_.push_back(temp1);
				Blocks.push_back(ROI[index](Rect(temp.x, temp.y, tt, tt1)));
			}

		}
	}

}



void EdgeDetector::RectAdjust(cv::Mat img,int &x, int& y, int& width, int& height)
{
	int imgwidth = img.size().width;
	int imgheight = img.size().height;
	if (x < 0)
	{
		x = 0;
	}
	else if (x >= imgwidth)
	{
		x = imgwidth - width - 1;
	}

	if (y < 0)
	{
		y = 0;
	}
	else if (y >= imgheight)
	{
		y = imgheight - height - 1;
	}
	if (width > imgwidth)
	{
		x = 0;
		width = imgwidth;
	}
	else if (x + width > imgwidth)
	{
		x = (imgwidth - width);
	}
	if (height > imgheight)
	{
		y = 0;
		height = imgheight;
	}
	else if (y + height > imgheight)
	{
		y = (imgheight - height);
	}
}





