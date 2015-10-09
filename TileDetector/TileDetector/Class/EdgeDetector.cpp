#include "EdgeDetector.h"

#include"Processor.h"





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













EdgeDetector::EdgeDetector(Mat src, BlocksDetector *_bd)
{
	
	Processor ps;
	bd = _bd;
	if (src.channels()==3)
		cvtColor(src, src, CV_BGR2GRAY);

	//	Mat Contours(src_gray.size(), src_gray.type(), Scalar(0, 0, 0));


	Point ptemp;

	xleft = (*bd).A.x - abs((*bd).A.x - (*bd).B.x) - 100, yleft = (*bd).A.y - 100, left_height = (*bd).B.y - (*bd).A.y + 200, left_width = 2 * abs((*bd).A.x - (*bd).B.x) + 200;
	xright = (*bd).C.x - abs((*bd).C.x - (*bd).D.x) - 100, yright = (*bd).C.y - 100, right_height = (*bd).D.y - (*bd).C.y + 200, right_width = 2 * abs((*bd).C.x - (*bd).D.x) + 200;
	xup = xleft + left_width - 6, yup = (*bd).A.y - abs((*bd).A.y - (*bd).C.y) - 100, up_width = xright - xleft - left_width + 12, up_height = 2 * abs((*bd).A.y - (*bd).C.y) + 200;
	xdown = xleft + left_width - 6, ydown = (*bd).B.y - abs((*bd).B.y - (*bd).D.y) - 100, down_width = xright - xleft - left_width + 12, down_height = 2 * abs((*bd).B.y - (*bd).D.y) + 200;

	src(Rect(xleft, yleft, left_width, left_height)).copyTo(leftROI);
	src(Rect(xright, yright, right_width, right_height)).copyTo(rightROI);
	src(Rect(xup, yup, up_width, up_height)).copyTo(upROI);
	src(Rect(xdown, ydown, down_width, down_height)).copyTo(downROI);

	vector<Mat> ROI;
	ROI.push_back(leftROI);
	ROI.push_back(downROI);
	ROI.push_back(rightROI);
	ROI.push_back(upROI);

	// ROI二值化
	for (int i = 0; i < ROI.size(); i++)
	{
		ps.Binaryzation(ROI[i]);
	}

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

	vector<vector<Point>> contours1;


	// 所有边界排列
	int startval = 0;
	for (int i = 0; i < contours.size(); i++)
	{

		switch (i % 2)
		{
		case 0:
		{
				  int a, b;
				  for (int j = 0; j < contours[i].size() - 1; j++)
				  {
					  a = contours[i][j].y;
					  b = contours[i][j + 1].y;
					  if ((abs(a - b))>100)
					  {
						  startval = j + 1;

						  /*  for (int k = startval - 5; k < startval + 5;k++)
						  cout << contours[i][k] << "\n";*/

						  break;
					  }
					  else startval = 0;
				  }
				  for (int q = startval; q < contours[i].size(); q++)
				  {
					  Point ptemp;
					  ptemp.x = contours[i][q].x;
					  ptemp.y = contours[i][q].y;
					  p.push_back(ptemp);
				  }
				  for (int q = 0; q < startval; q++)
				  {
					  Point ptemp;
					  ptemp.x = contours[i][q].x;
					  ptemp.y = contours[i][q].y;
					  p.push_back(ptemp);
				  }
				  contours1.push_back(p);
				  p.clear();
				  break;
		}
		case 1:
		{
				  if (abs(contours[i][0].x - contours[i][contours[i].size() - 1].x) < 2)
				  {
					  for (int j = 0; j < contours[i].size() - 1; j++)
					  {
						  if (abs(contours[i][j].x - contours[i][j + 1].x)>100)
						  {
							  startval = j + 1;
							  break;
						  }
					  }
				  }
				  else startval = 0;

				  for (int q = startval; q < contours[i].size(); q++)
				  {
					  Point ptemp;
					  ptemp.x = contours[i][q].x;
					  ptemp.y = contours[i][q].y;
					  p.push_back(ptemp);
				  }
				  for (int q = 0; q < startval; q++)
				  {
					  Point ptemp;
					  ptemp.x = contours[i][q].x;
					  ptemp.y = contours[i][q].y;
					  p.push_back(ptemp);
				  }
				  contours1.push_back(p);
				  p.clear();
				  break;
		}

		}
	}

	int lstart = 0, lend = contours1[0].size() - 1;
	int rstart = 0, rend = contours1[2].size() - 1;
	int flag = 1, tolerate = 0;

	// 左边边界点获取分割
	for (int i = 0; i < contours1[0].size() - 1; i++)
	{
		if (contours1[0][i + 1].y - contours1[0][i].y == 1)
		{
			lstart = i + 1;
			for (int q = lstart; q < lstart + 50; q++)
			{
				if (contours1[0][q + 1].y - contours1[0][q].y == 1)
				{
					flag++;
					if (flag>30)
						break;
				}
				else
				{
					i = q + 1;
					break;
				}

			}
		}
		if (flag > 30)
			break;
	}
	for (int i = contours1[0].size() - 1; i > 1; i--)
	{
		tolerate = 0;
		flag = 1;
		if (contours1[0][i].y - contours1[0][i - 1].y == 1)
		{
			lend = i - 1;
			for (int q = lend; q > lend - 50; q--)
			{
				if (contours1[0][q].y - contours1[0][q - 1].y == 1)
				{
					flag++;
					if (flag >30)
						break;
				}
				else
				{
					i = q + 1;
					break;
				}

			}
		}
		if (flag > 30)
			break;
	}

	// 右边边界点获取分割
	for (int i = 0; i < contours1[2].size() - 1; i++)
	{
		tolerate = 0;
		flag = 1;
		if (contours1[2][i].y - contours1[2][i + 1].y == 1)
		{
			rstart = i + 1;
			for (int q = rstart; q < rstart + 50; q++)
			{
				if (contours1[2][q].y - contours1[2][q + 1].y == 1)
				{
					flag++;
					if (flag>30)
						break;
				}
				else
				{
					i = q + 1;
					break;

				}
			}
		}
		if (flag > 30)
			break;
	}

	for (int i = contours1[2].size() - 1; i > 1; i--)
	{
		tolerate = 0;
		flag = 1;
		if (contours1[2][i - 1].y - contours1[2][i].y == 1)
		{
			rend = i - 1;
			for (int q = rend; q > rend - 50; q--)
			{
				if (contours1[2][q - 1].y - contours1[2][q].y == 1)
				{
					flag++;
					if (flag > 30)
						break;
				}
				else
				{
					i = q + 1;
					break;
				}

			}
		}
		if (flag > 30)
			break;
	}

	// 各个边界完整边界点获取
	vector<Point> nn;
	contours.clear();
	// 左
	for (int j = lstart; j < lend; j++)
	{
		Point s;
		s.x = contours1[0][j].x;
		s.y = contours1[0][j].y;
		nn.push_back(s);
	}
	contours.push_back(nn);
	nn.clear();
	// 下
	for (int j = lend; j < contours1[0].size(); j++)
	{
		Point s;
		s.x = contours1[0][j].x;
		s.y = contours1[0][j].y;
		nn.push_back(s);
	}
	for (int j = 0; j < contours1[1].size(); j++)
	{
		Point s;
		s.x = contours1[1][j].x;
		s.y = contours1[1][j].y;
		nn.push_back(s);
	}
	for (int j = 0; j < rstart; j++)
	{
		Point s;
		s.x = contours1[2][j].x;
		s.y = contours1[2][j].y;
		nn.push_back(s);
	}
	contours.push_back(nn);
	nn.clear();
	// 右
	for (int j = rstart; j < rend; j++)
	{
		Point s;
		s.x = contours1[2][j].x;
		s.y = contours1[2][j].y;
		nn.push_back(s);
	}
	contours.push_back(nn);
	nn.clear();
	// 上
	for (int j = rend; j < contours1[2].size(); j++)
	{
		Point s;
		s.x = contours1[2][j].x;
		s.y = contours1[2][j].y;
		nn.push_back(s);
	}
	for (int j = 0; j < contours1[3].size(); j++)
	{
		Point s;
		s.x = contours1[3][j].x;
		s.y = contours1[3][j].y;
		nn.push_back(s);
	}
	for (int j = 0; j < lstart; j++)
	{
		Point s;
		s.x = contours1[0][j].x;
		s.y = contours1[0][j].y;
		nn.push_back(s);
	}
	contours.push_back(nn);
	nn.clear();

	// 各个完整边界点抽样保存在fitcontours中
	vector<Point> mm;
	for (int m = 0; m < contours.size(); m++)
	{
		mm.clear();
		for (int h = 0; h < contours[m].size(); h = h + 100)
		{
			mm.push_back(contours[m][h]);
		}
		fitcontours.push_back(mm);

	}

	// 各边界抽样点拟合直线
	Vec4f _Fitline;
	vector<Vec4f> Fitline;
	Vector<float> Distance1;
	for (int i = 0; i < fitcontours.size(); i++)
	{
		// 样点过于特殊剔除
		for (int j = 0; j<10; j++)
		{
			Distance1.clear();
			fitLine(Mat(fitcontours[i]), _Fitline, CV_DIST_L2, 0, 1, 1);
			DistanceDetector(fitcontours[i], _Fitline, Distance1);

			int Tabel = Distamce_MaxTabel(Distance1);
			if (Distance1[Tabel]>5)
			{
				//
				//				circle(src_contours, fitcontours[i][Tabel], 20, CV_RGB(160, 0, 50 + i * 50), 1);
				fitcontours[i].erase(fitcontours[i].begin() + Tabel); // 擦除奇异点

			}
			else
				break;
		}
		//
		//		ed.DrawLine(i, src_contours, _Fitline, 0, 200, 100);
		Fitline.push_back(_Fitline);
	}

	//  拟合直线交点
	Vector<Point>Point_of_Intersection;
	PointOfIntersection(Fitline, Point_of_Intersection);
	(*bd).A = Point_of_Intersection[3];
	(*bd).B = Point_of_Intersection[0];
	(*bd).C = Point_of_Intersection[1];
	(*bd).D = Point_of_Intersection[2];
	

	// 崩边判定

	Vector<Point> Singalar;
	Vector<float> Distance;
	Vector<int> R;

	for (int k = 0; k < contours.size(); k++)
	{
		Singalar.clear();
		Distance.clear();
		for (int i = 0; i < contours[k].size(); i++)
		{
			// 计算每个点到拟合直线的距离
			float distance = 0;

			distance = (abs((contours[k][i].x - Fitline[k][2])*Fitline[k][1] / Fitline[k][0] + Fitline[k][3] - contours[k][i].y))
				/ (sqrt(1 + (Fitline[k][1] / Fitline[k][0])*(Fitline[k][1] / Fitline[k][0])));

			if (distance > distance_threld)
			{
				// 如果距离大于阈值则计入异常点
				Distance.push_back(distance);
				Singalar.push_back(contours[k][i]);
			}

		}

		int flag = 1;
		int label = 0;
		int tolerate = 0;
		for (int i = 0; i < Singalar.size(); i++)
		{
			if (i == Singalar.size() - 1)
				break;
			float temp = 0;
			temp = abs((Singalar[i + 1].x - Singalar[i].x)) + abs((Singalar[i + 1].y - Singalar[i].y));	// 计算相邻异常点的距离
			if (temp<3) // 距离小于3则认为是连续的异常点
			{
				flag++; // 距离小于3则认为是连续的异常点，标志位加1
				label = i;
				if ((flag > Edge_threld) && (i == Singalar.size() - 2)) // 异常点算完且标志位大于判定崩边阈值
				{
					//
					//					circle(src, Singalar[label - flag / 2], flag, CV_RGB(255, 0, 0), 1);
					Center.push_back(Singalar[label - flag / 2]);
					R.push_back(flag / 2);
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
									//									circle(src, Singalar[book], 200, CV_RGB(255, 0, 0), 1);
									Center.push_back(Singalar[book]);
									R.push_back(flag / 2);
								}
							}
							//
							else
							{
								//
								//								circle(src, Singalar[label - flag / 2], flag, CV_RGB(255, 0, 0), 1);
								Center.push_back(Singalar[label - flag / 2]);
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
						//						circle(src, Singalar[label - flag / 2], flag, CV_RGB(255, 0, 0), 1);
						Center.push_back(Singalar[label - flag / 2]);
						R.push_back(flag / 2);
						flag = 1;
					}
				}
			}


		}
	}

	/*for (int j = 0; j < Center.size(); j++)
	cout << Center[j] << "\n";*/



	//  检测崩边位置，若有崩边的中心距离很近，合并崩边
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
			for (int j = i + 1; j < Center.size(); j++)
			{
				d = (Center[i].x - Center[j].x)*(Center[i].x - Center[j].x) + (Center[i].y - Center[j].y)*(Center[i].y - Center[j].y);
				if ((d<=R[i] * R[i]) || (d<=R[j] * R[j]))
				{
					Center1.push_back(Center[i]);
					w.push_back(j);
				}

			}
			Point3f tt;
			if (Center1.size()>1)
			{
				int m = 0, n = 0, g = 0;
				for (int q = 0; q < Center1.size(); q++)
				{
					m += Center1[q].x;
					n += Center1[q].y;
				}
				for (int q = 0; q < w.size(); q++)
					g += R[w[q]];
				tt.x = m / Center1.size();
				tt.y = n / Center1.size();
				tt.z = g;

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
//
	
}
