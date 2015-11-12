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
		//cv::line(src, Point(x1, y1), Point(x2, y2), Scalar(R, G, B));
	}
	else
	{
		int y1 = 0;
		int x1 = FitLine[2] - FitLine[0] / FitLine[1] * FitLine[3];
		int y2 = 11000;
		int x2 = (y2 - FitLine[3])*FitLine[0] / FitLine[1] + FitLine[2];
		//line(src, Point(x1, y1), Point(x2, y2), Scalar(R, G, B));
	}
}



void EdgeDetector::DistanceDetector_set(Vector<Point> Boundary_Point_temp, Vec4f FitLine, Vector<float> &Distance)
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

EdgeDetector::EdgeDetector(Mat img, Block *_block)
{
	block = _block;

	src = img;


	A = block->A;

	B = block->B;

	C = block->C;

	D = block->D;

	xleft = (A.x - abs(A.x - D.x) - 100>0 ? (A.x - abs(A.x - D.x) - 100) : 0), yleft = A.y - 100, left_height = D.y - A.y + 200, left_width = 2 * abs(A.x - D.x) + 200;
	xright = B.x - abs(B.x - C.x) - 100, yright = B.y - 100, right_height = C.y - B.y + 200, right_width = min(2 * abs(B.x - C.x) + 200, 4096 - B.x + abs(B.x - C.x));
	xup = A.x - 100, yup = (A.y - abs(A.y - B.y) - 100>0 ? (A.y - abs(A.y - B.y) - 100) : 0), up_width = B.x - A.x + 200, up_height = 2 * abs(A.y - B.y) + 200;
	xdown = D.x - 100, ydown = D.y - abs(D.y - C.y) - 100, down_width = C.x - D.x + 200, down_height = min(2 * abs(D.y - C.y) + 200, 11000 - D.y + abs(D.y - C.y));

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


	Mat leftROI1, rightROI1, upROI1, downROI1;
	leftROI.copyTo(leftROI1);
	rightROI.copyTo(rightROI1);
	upROI.copyTo(upROI1);
	downROI.copyTo(downROI1);
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

	ROI1.push_back(leftROI1);
	ROI1.push_back(downROI1);
	ROI1.push_back(rightROI1);
	ROI1.push_back(upROI1);


}

EdgeDetector::~EdgeDetector()
{

}

void EdgeDetector::start()
{

	Processor ps;


	//double	t = (double)cv::getTickCount();
	//if (src.channels() == 3)
	//for (int i = 0; i < ROI.size(); i++)
	//	cvtColor(ROI[i], ROI[i], CV_BGR2GRAY);


	//if (src.channels() == 3)
	//	cvtColor(src, src, CV_BGR2GRAY);

	// ROI二值化
	for (int i = 0; i < ROI.size(); i++)
	{
		ps.Binaryzation(ROI[i]);
		//Canny(ROI[i], ROI[i], 40, 80);
	}

	//if (src.channels() == 3)
	//for (int i = 0; i < ROI1.size(); i++)
	//	cvtColor(ROI1[i], ROI1[i], CV_BGR2GRAY);
	/*t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	cout << t;*/
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
		/*temp.clear();*/
	}
	//ROI.clear();


	// 得到每个边界轮廓的有效边界
	for (int i = 0; i < contourstemp.size(); i++)
	{
		int t = 0, t1 = 0, t2 = 0, xp = 0, yp = 0;
		switch (i)
		{
		case 0: t = left_width - 2; xp = xleft; yp = yleft - 1; break;
		case 1: t = 1; xp = xdown; yp = ydown; break;
		case 2:	t = 1; xp = xright; yp = yright - 1; break;
		case 3: t = up_height - 2; xp = xup; yp = yup; break;
		}
		if (i % 2 == 0)
		{
			for (int j = 0; j < contourstemp[i].size(); j++)
			if (contourstemp[i][j].x != t)
			{
				Point ptemp;
				ptemp.x = contourstemp[i][j].x/* + xp*/;
				ptemp.y = contourstemp[i][j].y/* + yp*/;
				p1.push_back(ptemp);
			}

		}
		else
		{
			for (int j = 0; j < contourstemp[i].size(); j++)
			if (contourstemp[i][j].y != t)
			{
				Point ptemp;
				ptemp.x = contourstemp[i][j].x/* + xp*/;
				ptemp.y = contourstemp[i][j].y/* + yp*/;
				p1.push_back(ptemp);
			}
		}
		contours.push_back(p1);
		p1.clear();
	}
	/*contourstemp.clear();*/

	vector<vector<Point>> Fit_contours;

	for (int i = 0; i < contours.size(); i++)
	{
		vector<Point> tm;
		for (int j = 300; j < contours[i].size() - 300; j = j + 70)
		{
			tm.push_back(contours[i][j]);
		}
		Fit_contours.push_back(tm);
	}

	vector<Vec4f> line_;
	vector<Vec4f> line_1;
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
			line_1.push_back(Fit_Line);
			DrawLine(i, ROI1[i], Fit_Line, 200, 200, 100);
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
			line_.push_back(Fit_Line);
		}
	}
	/*Fit_contours.clear();*/

	// 求出四个交点A\B\C\D
	Vector<Point> Point_of_Intersection;
	PointOfIntersection(line_, Point_of_Intersection);
	block->A = Point_of_Intersection[3];

	block->B = Point_of_Intersection[2];

	block->C = Point_of_Intersection[1];

	block->D = Point_of_Intersection[0];

	/*line_.clear();
	Point_of_Intersection.clear();*/

	// 左边
	int sy = 0, ey = 0;
	int ylA = (block->A).y - yleft, ylD = (block->D).y - yleft;
	for (int i = ylA; i < ylD - 10; i += 10)
	{
		int thred = 0;
		ey = 0; sy = 0;
		int j = line_1[0][0] / line_1[0][1] * (i - line_1[0][3]) + line_1[0][2];
		/*ROI1[0].at<uchar>(i, j) = 255;*/
		if (i < ylA + 50)
			thred = 10;
		else
			thred = 15;
		if (ROI1[0].at<uchar>(i, j + 6) < thred)
		{
			/*		ROI1[0].at<uchar>(i, j + 6) = 255;*/

			if (i < ylA + 20)
				sy = ylA;
			else
				sy = i - 20;
			for (int q = i + 1; q < ylD - 10; q += 10)
			{
				int p = line_1[0][0] / line_1[0][1] * (q - line_1[0][3]) + line_1[0][2];
				if (ROI1[0].at<uchar>(q, p + 6) > thred)
				{
					ey = q + 20;
					i = q + 20;
					if (i > ylD - 9)
						i = ylD - 9;
					break;
				}
				if (ey == 0 && (ylD - q <= 20))
				{
					ey = ylD;
					i = ylD - 10;
				}
			}
			if (ey - sy > 51)
			{
				int xx = min((line_1[0][0] / line_1[0][1] * (sy - line_1[0][3]) + line_1[0][2]),
					(line_1[0][0] / line_1[0][1] * (ey - line_1[0][3]) + line_1[0][2]));
				ps.Binaryzation(ROI1[0](Rect(xx, sy, min(100, ROI1[0].cols - xx), ey - sy)));
				//	Canny(src(Rect(xx, sy, 100, ey - sy)), src(Rect(xx, sy, 100, ey - sy)), 100, 250);
				/*ps.Binaryzation_otsu(src(Rect(xx, sy, 100, ey - sy)));*/
				int flag = 0;
				for (int a = sy; a < ey; a++)
				{
					int h = line_1[0][0] / line_1[0][1] * (a - line_1[0][3]) + line_1[0][2];
					for (int b = h + 2; b < xx + 100; b++)
					{
						if (b > h + 10)
						{
							flag++;
							break;
						}

						if (ROI1[0].at<uchar>(a, b) != 0)
						{
							//	src.at<uchar>(a, b)=0;
							int k = DistanceDetector(Point(b, a), line_1[0]);
							if (DistanceDetector(Point(b, a), line_1[0])>5)
								flag++;
							break;
						}

					}
				}
				if (flag > 7)
				{
					Point3f temp;
					temp.x = xx + xleft;
					temp.y = sy + (ey - sy) / 2 + yleft;
					temp.z = (ey - sy) / 2;
					Defects.push_back(temp);
					//circle(src, Point(xx + xleft, sy + (ey - sy) / 2 + yleft), (ey - sy) / 2, Scalar(255, 255, 255));
				}


			}
		}
		//src.at<uchar>(i, j + 5) = 255;
	}

	// 下边
	int sx = 0, ex = 0;
	int xdD = (block->D).x - xdown, xdC = (block->C).x - xdown;
	for (int i = xdD; i < xdC; i += 10)
	{
		sx = 0; ex = 0;
		int j = line_1[1][1] / line_1[1][0] * (i - line_1[1][2]) + line_1[1][3];
		//	ROI1[1].at<uchar>(j, i) = 250;
		if (ROI1[1].at<uchar>(j - 6, i) < 10)
		{
			//	src.at<uchar>(i, j + 5) = 255;
			if (i < xdD + 10)
				sx = xdD;
			else
				sx = i - 10;
			for (int q = i + 1; q < xdC; q += 10)
			{
				int p = line_1[1][1] / line_1[1][0] * (q - line_1[1][2]) + line_1[1][3];
				if (ROI1[1].at<uchar>(p - 6, q) >= 10)
				{
					ex = q + 10;
					i = q + 10;
					if (i > xdC)
						i = xdC;
					break;
				}
				if (ex == 0 && (xdC - q <= 10))
				{
					ex = xdC;
					i = xdC;
				}
			}
			if (ex - sx > 30)
			{
				int yy = max((line_1[1][1] / line_1[1][0] * (sx - line_1[1][2]) + line_1[1][3]),
					(line_1[1][1] / line_1[1][0] * (ex - line_1[1][2]) + line_1[1][3]));

				ps.Binaryzation(ROI1[1](Rect(sx, yy - min(100, yy), ex - sx, min(100, yy))));
				//	Canny(src(Rect(sx, yy - 100, ex - sx, 100)), src(Rect(sx, yy - 100, ex - sx, 100)), 100, 250);
				/*ps.Binaryzation_otsu(src(Rect(sx, yy - 100, ex - sx, 100)));*/

				int flag = 0;
				for (int a = sx; a < ex; a++)
				{
					int h = line_1[1][1] / line_1[1][0] * (a - line_1[1][2]) + line_1[1][3];
					for (int b = h - 2; b > yy - 100; b--)
					{
						if (b == yy - 99)
							flag++;
						if (ROI1[1].at<uchar>(b, a) != 0)
						{
							//	src.at<uchar>(a, b)=0;
							if (DistanceDetector(Point(a, b), line_1[1])>5)
								flag++;
							break;
						}

					}
				}
				if (flag > 7)
				{
					Point3f temp;
					temp.x = sx + (ex - sx) / 2 + xdown;
					temp.y = yy + ydown;
					temp.z = (ex - sx) / 2;
					Defects.push_back(temp);
					//circle(src, Point(sx + (ex - sx) / 2 + xdown, yy + ydown), (ex - sx) / 2, Scalar(255, 255, 255));
				}

				ex = 0; sx = 0;
			}
		}
		//src.at<uchar>(i, j + 5) = 255;
	}

	// 右边
	int yrB = (block->B).y - yright, yrC = (block->C).y - yright;
	for (int i = yrB; i < yrC - 20; i += 10)
	{
		int thred = 0;
		sy = 0, ey = 0;
		int j = line_1[2][0] / line_1[2][1] * (i - line_1[2][3]) + line_1[2][2];
		if (i < yrB + 50)
			thred = 10;
		else
			thred = 15;
		if (ROI1[2].at<uchar>(i, j - 6) < thred)
		{
			//	src.at<uchar>(i, j + 5) = 255;
			if (i < yrB + 20)
				sy = yrB;
			else
				sy = i - 20;
			for (int q = i + 1; q < yrC - 10; q += 10)
			{
				int p = line_1[2][0] / line_1[2][1] * (q - line_1[2][3]) + line_1[2][2];
				if (ROI1[2].at<uchar>(q, p - 6) > thred)
				{
					ey = q + 20;
					i = q + 20;
					if (i > yrC - 9)
						i = yrC - 9;
					break;
				}
				if (ey == 0 && (yrC - q <= 20))
				{
					ey = yrC;
					i = yrC - 10;
				}
			}
			if (ey - sy > 50)
			{
				int xx = max((line_1[2][0] / line_1[2][1] * (sy - line_1[2][3]) + line_1[2][2]),
					(line_1[2][0] / line_1[2][1] * (ey - line_1[2][3]) + line_1[2][2]));
				ps.Binaryzation(ROI1[2](Rect(xx - min(100, xx), sy, min(100, xx), ey - sy)));
				//	Canny(src(Rect(xx - 100, sy, 100, ey - sy)), src(Rect(xx - 100, sy, 100, ey - sy)), 100, 250);
				/*ps.Binaryzation_otsu(src(Rect(xx - 100, sy, 100, ey - sy)));*/
				int flag = 0;
				for (int a = sy; a < ey; a++)
				{
					int h = line_1[2][0] / line_1[2][1] * (a - line_1[2][3]) + line_1[2][2];
					for (int b = h - 2; b > xx - 100; b--)
					{
						if (b == xx - 99)
							flag++;
						if (ROI1[2].at<uchar>(a, b) != 0)
						{
							//	src.at<uchar>(a, b)=0;
							if (DistanceDetector(Point(b, a), line_1[2])>5)
								flag++;
							break;
						}

					}
				}
				if (flag > 7)
				{
					Point3f temp;
					temp.x = xx + xright;
					temp.y = sy + (ey - sy) / 2 + yright;
					temp.z = (ey - sy) / 2;
					Defects.push_back(temp);
					//circle(src, Point(xx + xright, sy + (ey - sy) / 2 + yright), (ey - sy) / 2, Scalar(255, 255, 255));
				}

				ey = 0; sy = 0;
			}
		}
		//src.at<uchar>(i, j + 5) = 255;
	}

	// 上边
	int xuA = (block->A).x - xup, xuB = (block->B).x - xup;
	for (int i = xuA + 3200; i < xuB - 20; i += 10)
	{
		sx = 0; ex = 0;
		int j = line_1[3][1] / line_1[3][0] * (i - line_1[3][2]) + line_1[3][3];
		/*ROI1[3].at<uchar>(j, i) = 255;*/
		if (ROI1[3].at<uchar>(j + 6, i) < 10)
		{
			//	src.at<uchar>(i, j + 5) = 255;
			if (i < xuA + 10)
				sx = xuA;
			else
				sx = i - 10;
			for (int q = i + 1; q < xuB; q += 10)
			{
				int p = line_1[3][1] / line_1[3][0] * (q - line_1[3][2]) + line_1[3][3];
				if (ROI1[3].at<uchar>(p + 6, q) > 10)
				{
					ex = q + 10;
					i = q + 10;
					if (i > xuB)
						i = xuB;
					break;
				}
				if (ex == 0 && (xuB - q <= 10))
				{
					ex = xuB;
					i = xuB;
				}
			}
			if (ex - sx > 30)
			{
				int yy = min((line_1[3][1] / line_1[3][0] * (sx - line_1[3][2]) + line_1[3][3]),
					(line_1[3][1] / line_1[3][0] * (ex - line_1[3][2]) + line_1[3][3]));
				ps.Binaryzation(ROI1[3](Rect(sx, yy, ex - sx, min(100, ROI1[3].rows - yy))));
				//	Canny(src(Rect(sx, yy, ex - sx, 100)), src(Rect(sx, yy, ex - sx, 100)), 100, 250);
				/*ps.Binaryzation_otsu(src(Rect(sx, yy, ex - sx, 100)));*/
				int flag = 0;
				for (int a = sx; a < ex; a++)
				{
					int h = (line_1[3][1] / line_1[3][0] * (a - line_1[3][2]) + line_1[3][3]);
					for (int b = h + 2; b < h + 15; b++)
					{
						if (b == h + 14)
						{
							flag++;
							break;
						}

						if (ROI1[3].at<uchar>(b, a) != 0)
						{
							//	src.at<uchar>(a, b)=0;
							if (DistanceDetector(Point(a, b), line_1[3])>5)
								flag++;
							break;
						}

					}
				}
				if (flag > 7)
				{
					/*stringstream ss;
					static int cc = 1;
					ss << "roi_" << cc++ << ".jpg";
					imwrite(ss.str, src1(Rect(sx, yy, ex - sx, 100)));*/
					Point3f temp;
					temp.x = sx + (ex - sx) / 2 + xup;
					temp.y = yy + yup;
					temp.z = (ex - sx) / 2;
					Defects.push_back(temp);
					//circle(src, Point(sx + (ex - sx) / 2 + xup, yy + yup), (ex - sx) / 2, Scalar(255, 255, 255));
				}

				ex = 0; sx = 0;
			}
		}
	}
	/*line_1.clear();
	contours.clear();*/
}