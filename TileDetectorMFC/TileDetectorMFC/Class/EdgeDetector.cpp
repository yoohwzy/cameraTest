#include "EdgeDetector.h"
#include "Processor.h"


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



void EdgeDetector::DistanceDetector(Vector<Point> Boundary_Point_temp, Vec4f FitLine, Vector<float> &Distance)
{
	//float distance = 0;
	for (int Boundary_Point_tempIndex = 0; Boundary_Point_tempIndex < Boundary_Point_temp.size(); Boundary_Point_tempIndex++)
	{
		distance = (abs((Boundary_Point_temp[Boundary_Point_tempIndex].x - FitLine[2])*FitLine[1] / FitLine[0] + FitLine[3] - Boundary_Point_temp[Boundary_Point_tempIndex].y))
			/ (sqrt(1 + (FitLine[1] / FitLine[0])*(FitLine[1] / FitLine[0])));
		Distance.push_back(distance);
	}
}



void EdgeDetector::PointOfIntersection(Vector<Vec4f>FitLine_Aggregate, Vector<Point> &Point_of_Intersection)
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


int EdgeDetector::Distamce_MaxTabel(Vector<float> Distance)
{
	float Temp = 0;
	for (int DistanceIndex = 0; DistanceIndex < Distance.size(); DistanceIndex++)
	{
		if (Temp < Distance[DistanceIndex])
		{
			Temp = Distance[DistanceIndex];
			Tabel = DistanceIndex;
		}
	}
	return Tabel;
}

void  EdgeDetector::Defect_Detector(vector < vector<Point> >contours_, vector<Vec4f> line_, vector<Point3f> &defects)
{
	vector < vector<Point> >SingularPoints;
	vector<Point>SingularPoints_temp;
	for (int i = 0; i < contours_.size(); i++)
	{
		SingularPoints_temp.clear();
		int  ttt = 0;
		for (int j = 0; j < contours_[i].size(); j++)
		{
			ttt = (abs((contours_[i][j].x - line_[i][2])*line_[i][1] / line_[i][0] + line_[i][3] - contours_[i][j].y))
				/ (sqrt(1 + (line_[i][1] / line_[i][0])*(line_[i][1] / line_[i][0])));
			if (ttt>5)
			{
				SingularPoints_temp.push_back(contours_[i][j]);
				//					circle(image1, contours_[i][j], 3, CV_RGB(255, 0, 0), -1, 8);
			}
		}
		SingularPoints.push_back(SingularPoints_temp);
	}

	int flag = 1;

	int tolerate = 0;
	Vector<float> Distance;
	Vector<int> R;
	Vector<Point> Center;
	for (int j = 0; j < SingularPoints.size(); j++)
	{
		int label = 0;
		flag = 1;
		for (int i = 0; i < SingularPoints[j].size(); i++)
		{
			if (i == SingularPoints[j].size() - 1)
				break;
			float temp = 0;
			temp = abs((SingularPoints[j][i + 1].x - SingularPoints[j][i].x)) + abs((SingularPoints[j][i + 1].y - SingularPoints[j][i].y));	// 计算相邻异常点的距离
			if (temp<3) // 距离小于3则认为是连续的异常点
			{
				flag++; // 距离小于3则认为是连续的异常点，标志位加1
				label = i;
				if ((flag > Edge_threld) && (i == SingularPoints[j].size() - 2)) // 异常点算完且标志位大于判定崩边阈值
				{
					//
					//					circle(src, SingularPoints[j][label - flag / 2], flag, CV_RGB(255, 0, 0), 1);
					Center.push_back(SingularPoints[j][label - flag / 2]);
					R.push_back(flag / 2 + 10);
				}

			}
			else	// 距离大于3
			{
				if (temp<100) // 若距离小于一个阈值，认为在该处出现了一个例外，允许其继续叠加后续异常点数量
				{
					tolerate++; // 容忍度加1
					if (tolerate > tolerate_threld) // 当容忍度大于阈值时，认为不再连续
					{
						tolerate = 0;
						if (flag > Edge_threld)
						{
							if (flag > 200)
							{
								int m = 0;
								int book = 0;
								for (int n = 0; n < Distance.size(); n++)
								{
									if (m < Distance.size())
									{
										book = i;
									}
									//
									//									circle(src, SingularPoints[j][book], 200, CV_RGB(255, 0, 0), 1);
									Center.push_back(SingularPoints[j][book]);
									R.push_back(flag / 2);
								}
							}
							//
							else
							{
								//
								//								circle(src, SingularPoints[j][label - flag / 2], flag, CV_RGB(255, 0, 0), 1);
								flag += temp;
								Center.push_back(SingularPoints[j][label - flag / 2]);
								R.push_back(flag / 2);
							}
							flag = 1;
						}
					}
				}
				else // 距离太大，异常点不再连续
				{
					if (flag > Edge_threld)
					{
						tolerate = 0;
						//
						//						circle(src, SingularPoints[j][label - flag / 2], flag, CV_RGB(255, 0, 0), 1);
						Center.push_back(SingularPoints[j][label - flag / 2]);
						R.push_back(flag / 2 + 15);
						flag = 1;
					}
				}
			}
		}
	}

	//Point3f ttt;
	//for (int i = 0; i < Center.size(); i++)
	//{
	//	ttt.x = Center[i].x;
	//	ttt.y = Center[i].y;
	//	ttt.z = R[i];
	//	Defects.push_back(ttt);
	//}


	/*double t = (double)cv::getTickCount();*/
	if (Center.size() > 0)
	{
		Vector<int>w;
		for (int i = 0; i < Center.size(); i++)
		{
			int h = 1;
			// 判定崩边位置是否已经合并
			for (int p = 0; p < w.size(); p++)
			{
				if (i == w[p])
				{
					h = 0;
					break;
				}

			}
			if (h)
			{
				// 没有合并的位置
				w.clear();
				float d = 0;
				Vector<Point> Center1;
				Center1.push_back(Center[i]);
				int g = R[i];
				for (int j = i + 1; j < Center.size(); j++)
				{
					d = abs(Center[i].x - Center[j].x) + abs(Center[i].y - Center[j].y);
					if (d <= (abs(R[i] + R[j])))
					{
						Center1.push_back(Center[i]);
						w.push_back(j);
					}
				}
				Point3f tt;
				if (Center1.size()>1)
				{
					int m = 0, n = 0;
					for (int q = 0; q < Center1.size(); q++)
					{
						m += Center1[q].x;
						n += Center1[q].y;
					}
					for (int q = 0; q < w.size(); q++)
						g += R[w[q]];
					tt.x = m / Center1.size();
					tt.y = n / Center1.size();
					tt.z = g + 20;

				}
				else
				{
					tt.x = Center1[0].x;
					tt.y = Center1[0].y;
					tt.z = R[i];
				}
				Defects.push_back(tt);
			}
		}
	}
}

EdgeDetector::EdgeDetector(Mat img, BlocksDetector *_bd)
{
	bd = _bd;
	src = img;

	
	A = bd->A;
	B = bd->B;
	C = bd->C;
	D = bd->D;

	xleft = A.x - abs(A.x - D.x) - 100, yleft = A.y - 100, left_height = D.y - A.y + 200, left_width = 2 * abs(A.x - D.x) + 200;
	xright = B.x - abs(B.x - C.x) - 100, yright = B.y - 100, right_height = C.y - B.y + 200, right_width = 2 * abs(B.x - C.x) + 200;
	xup = xleft + left_width - 6, yup = A.y - abs(A.y - B.y) - 100, up_width = xright - xleft - left_width + 12, up_height = 2 * abs(A.y - B.y) + 200;
	xdown = xleft + left_width - 6, ydown = D.y - abs(D.y - C.y) - 100, down_width = xright - xleft - left_width + 12, down_height = 2 * abs(D.y - C.y) + 200;

	src(Rect(xleft, yleft, left_width, left_height)).copyTo(leftROI);  
	src(Rect(xright, yright, right_width, right_height)).copyTo(rightROI); 
	src(Rect(xup, yup, up_width, up_height)).copyTo(upROI); 
	src(Rect(xdown, ydown, down_width, down_height)).copyTo(downROI); 
	
	//imwrite("C:\\Users\\John\\Desktop\\leftroi.jpg", leftROI);
	//imwrite("C:\\Users\\John\\Desktop\\rightroi.jpg", rightROI);
	//imwrite("C:\\Users\\John\\Desktop\\uproi.jpg", upROI);
	//imwrite("C:\\Users\\John\\Desktop\\downroi.jpg", downROI);

	ROI.push_back(leftROI);
	ROI.push_back(downROI);
	ROI.push_back(rightROI);
	ROI.push_back(upROI);

}

EdgeDetector::~EdgeDetector()
{

}

void EdgeDetector::start()
{
	
	Processor ps;
		
	//double t = (double)cv::getTickCount();
	if (src.channels() == 3)
	for (int i = 0; i < ROI.size(); i++)
		cvtColor(ROI[i], ROI[i], CV_BGR2GRAY);


	// ROI二值化
	for (int i = 0; i < ROI.size(); i++)
	{
		ps.Binaryzation(ROI[i]);
		//Canny(ROI[i], ROI[i], 40, 80);
	}

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//std::cout << "EdgeDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;


	vector<Point> p, p1;
	Vector<Point> Center;
	vector<vector<Point>> contourstemp; // 保存各个边界的最大轮廓边界点
	vector<vector<Point>> temp;			// 保存一个边界的所有轮廓点集合
	vector<vector<Point>> contours, fitcontours;		// 保存每个边界的有效边界点
	// 搜索ROI边界轮廓
	for (int roi_index = 0; roi_index < ROI.size(); roi_index++)
	{

		findContours(ROI[roi_index], temp, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
		int maxtemp = 0, maxt = 0;
		for (int i = 0; i < temp.size(); i++)
		{
			if (maxt < temp[i].size())
			{
				maxt = temp[i].size();
				maxtemp = i;
			}

		}
		contourstemp.push_back(temp[maxtemp]);
	}



	// 得到每个边界轮廓的有效边界
	for (int i = 0; i < contourstemp.size(); i++)
	{
		int t = 0, t1 = 0, t2 = 0, xp = 0, yp = 0;
		switch (i)
		{
		case 0: t = left_width - 2; xp = xleft; yp = yleft - 1; break;
		case 1: t = 1, t1 = 1; t2 = down_width - 2; xp = xdown; yp = ydown; break;
		case 2:	t = 1; xp = xright; yp = yright - 1; break;
		case 3: t = up_height - 2, t1 = 1; t2 = up_width - 2; xp = xup; yp = yup; break;
		}
		if (i % 2 == 0)
		{
			for (int j = 0; j < contourstemp[i].size(); j++)
			if (contourstemp[i][j].x != t)
			{
				Point ptemp;
				ptemp.x = contourstemp[i][j].x + xp;
				ptemp.y = contourstemp[i][j].y + yp;
				p1.push_back(ptemp);
			}

		}
		else
		{
			for (int j = 0; j < contourstemp[i].size(); j++)
			if (contourstemp[i][j].y != t && contourstemp[i][j].x != t1 && contourstemp[i][j].x != t2)
			{
				Point ptemp;
				ptemp.x = contourstemp[i][j].x + xp;
				ptemp.y = contourstemp[i][j].y + yp;
				p1.push_back(ptemp);
			}
		}
		contours.push_back(p1);
		p1.clear();
	}

	

	// 每边取样点用于拟合
	vector<vector<Point>> Fit_contours;

	for (int i = 0; i < contours.size(); i++)
	{
		vector<Point> tm;
		for (int j = 0; j < contours[i].size(); j = j + 100)
		{
			tm.push_back(contours[i][j]);
		}
		Fit_contours.push_back(tm);
	}

//	
	/*Mat image(src.size(), src.type(), Scalar(0));
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			image.at<Vec3b>(contours[i][j])[0] = 50*i;
			image.at<Vec3b>(contours[i][j])[1] = 60*i-40;
			image.at<Vec3b>(contours[i][j])[2] = 30*i;
		}
	}*/




	// 拟合直线
	vector<Vec4f> line_;
	Vec4f Fit_Line;
	for (int i = 0; i < Fit_contours.size(); i++)
	{
		if (!Fit_contours[i].empty())
		{
			for (int j = 0; j < 10; j++)
			{
				Vector<float> Distance;
				fitLine(Mat(Fit_contours[i]), Fit_Line, CV_DIST_L2, 0, 1, 1);
				//
				//	ed.DrawLine(i, image, Fit_Line, 0, 200, 100 + j * 30);
				DistanceDetector(Fit_contours[i], Fit_Line, Distance);
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
			line_.push_back(Fit_Line);
		}
	}

	// 求出四个交点A\B\C\D
	Vector<Point> Point_of_Intersection;
	PointOfIntersection(line_, Point_of_Intersection);
	bd->A = Point_of_Intersection[3];
	bd->B = Point_of_Intersection[2];
	bd->C = Point_of_Intersection[1];
	bd->D = Point_of_Intersection[0];
	//for (int i = 0; i < Point_of_Intersection.size(); i++)
	//	cout << Point_of_Intersection[i] << "\n";

	// 左右边界分类
	vector<Point> D_start, U_end, U_start, D_end;
	float Distance_0, Distance_1;
	vector<Point>  left_, right_;
	vector<int> label;
	int tp1 = abs(Point_of_Intersection[3].x - Point_of_Intersection[0].x);
	int tp2 = abs(Point_of_Intersection[2].x - Point_of_Intersection[1].x);
	int tp3 = abs(Point_of_Intersection[3].y - Point_of_Intersection[2].y);
	int tp4 = abs(Point_of_Intersection[0].y - Point_of_Intersection[1].y);

	// 左边界
	for (int i = 0; i < contours[0].size(); i++)
	{

		int ttt = (abs((contours[0][i].x - line_[0][2])*line_[0][1] / line_[0][0] + line_[0][3] - contours[0][i].y))
			/ (sqrt(1 + (line_[0][1] / line_[0][0])*(line_[0][1] / line_[0][0])));
		
		if (ttt>5)
		{
			

			// 到上边距离
			if (abs(Point_of_Intersection[3].y - contours[0][i].y)< tp3+50)
			{
				U_end.push_back(contours[0][i]);
				label.push_back(i);
			}

			else
			// 到下边距离
			if (abs(Point_of_Intersection[0].y - contours[0][i].y)< tp4+50)
			{
				D_start.push_back(contours[0][i]);
				label.push_back(i);
			}
		}

	}



	for (int i = 0; i < U_end.size() - 2; i++)
	{
		int t = abs(U_end[i].x - U_end[i + 1].x);
		if (t>1000)
		{
			vector<Point> U_end_t(U_end);
			U_end.clear();
			for (int j = i + 1; j < U_end_t.size(); j++)
				U_end.push_back(U_end_t[j]);
			for (int j = 0; j < i + 1; j++)
				U_end.push_back(U_end_t[j]);
			break;
		}
	}

	for (int i = 0; i < D_start.size() - 2; i++)
	{
		int t = abs(D_start[i].x - D_start[i + 1].x);
		if (t>1000)
		{
			vector<Point> D_start_t(D_start);
			D_start.clear();
			for (int j = i + 1; j < D_start_t.size(); j++)
				D_start.push_back(D_start_t[j]);
			for (int j = 0; j < i + 1; j++)
				D_start.push_back(D_start_t[j]);
			break;
		}
	}

	int b = 0, e = 0;
	sort(label.begin(), label.end());
	for (int i = 0; i < label.size() - 1; i++)
	{
		int j = i + 1;
		if (label[j] - label[i]>100)
		{
			b = label[i] + 1;
			e = label[j];
			break;
		}
	}

	if (b == 0 && e == 0)
	{
		b = label[label.size()] + 1;
		e = contours[0].size();
	}

	label.clear();
	for (int i = b; i < e; i++)
		left_.push_back(contours[0][i]);

	// 右边界
	for (int i = 0; i < contours[2].size(); i++)
	{
		int ttt = (abs((contours[2][i].x - line_[2][2])*line_[2][1] / line_[2][0] + line_[2][3] - contours[2][i].y))
			/ (sqrt(1 + (line_[2][1] / line_[2][0])*(line_[2][1] / line_[2][0])));

		if (ttt>5)
		{
			if (abs(Point_of_Intersection[3].y - contours[2][i].y)< tp3 + 50)
			{
				U_start.push_back(contours[2][i]);
				label.push_back(i);
			}
		
			else
			if (abs(Point_of_Intersection[0].y - contours[2][i].y)< tp4 + 50)
			{
				D_end.push_back(contours[2][i]);
				label.push_back(i);
			}
		}
		
		
	}

	for (int i = 0; i < U_start.size() - 2; i++)
	{
		int t = abs(U_start[i].x - U_start[i + 1].x);
		if (t>1000)
		{
			vector<Point> U_start_t(U_start);
			U_start.clear();
			for (int j = i + 1; j < U_start_t.size(); j++)
				U_start.push_back(U_start_t[j]);
			for (int j = 0; j < i + 1; j++)
				U_start.push_back(U_start_t[j]);
		}
	}

	for (int i = 0; i < D_end.size() - 2; i++)
	{
		int t = abs(D_end[i].x - D_end[i + 1].x);
		if (t>1000)
		{
			vector<Point> D_end_t(D_end);
			D_end.clear();
			for (int j = i + 1; j < D_end_t.size(); j++)
				D_end.push_back(D_end_t[j]);
			for (int j = 0; j < i + 1; j++)
				D_end.push_back(D_end_t[j]);
		}
	}
	b = 0, e = 0;
	sort(label.begin(), label.end());
	for (int i = 0; i < label.size() - 2; i++)
	{
		int j = i + 1;
		if (label[j] - label[i]>100)
		{
			b = label[i] + 1;
			e = label[j];
		}
	}
	if (b == 0 && e == 0)
	{
		b = label[label.size()-1] + 1;
		e = contours[2].size();
	}
	label.clear();
	for (int i = b; i < e; i++)
		right_.push_back(contours[2][i]);


	// 合并上下边界
	vector<Point>  down_, up_;
	vector<vector<Point>> contours_;
	// 下
	for (int i = 0; i < D_start.size(); i++)
	{
		down_.push_back(D_start[i]);
	}
	for (int i = 0; i < contours[1].size(); i++)
	{
		down_.push_back(contours[1][i]);
	}
	for (int i = 0; i < D_end.size(); i++)
	{
		down_.push_back(D_end[i]);
	}

	// 上
	for (int i = 0; i < U_start.size(); i++)
	{
		up_.push_back(U_start[i]);
	}
	for (int i = 0; i < contours[3].size(); i++)
	{
		up_.push_back(contours[3][i]);
	}
	for (int i = 0; i < U_end.size(); i++)
	{
		up_.push_back(U_end[i]);
	}

	contours_.push_back(left_);
	contours_.push_back(down_);
	contours_.push_back(right_);
	contours_.push_back(up_);

	//
	//Mat image1(src.size(), src.type(), Scalar(0));
	//for (int i = 0; i < contours_.size(); i++)
	//{
	//	int m = 0,n=0,k=0,m1=0;
	//	if (i == 0)  m = 1;
	//	if (i == 1) n = 1;
	//	if (i == 2) k = 1;
	//	if (i == 3) m1 = 200;
	//	//DrawLine(i, image1, line_[i], 0, 200, 100 + i * 30);
	//	for (int j = 0; j < contours_[i].size(); j++)
	//	{
	//		image1.at<Vec3b>(contours_[i][j])[0] = 200*m+m1;
	//		image1.at<Vec3b>(contours_[i][j])[1] = 200 * n+m1 ;
	//		image1.at<Vec3b>(contours_[i][j])[2] = 200 * k+m1;
	//	}
	//}

	// 检测崩边
	Defect_Detector(contours_, line_, Defects);

	for (int i = 0; i < Defects.size(); i++)
		circle(src, Point(Defects[i].x, Defects[i].y), Defects[i].z, CV_RGB(0, 255, 255));
}
