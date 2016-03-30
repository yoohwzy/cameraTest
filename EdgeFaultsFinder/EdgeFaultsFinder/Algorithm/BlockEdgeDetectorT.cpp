#include "BlockEdgeDetectorT.h"
#include <math.h>

#include <iostream>
#include <fstream>

BlockEdgeDetectorT::BlockEdgeDetectorT(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	if (image.channels() == 3)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	p_block = _block;
	p_faults = _faults;
}
void BlockEdgeDetectorT::Run()
{
#ifdef BED_OUTPUT_DEBUG_INFO
	drowDebugResult = image.clone();
	if (drowDebugResult.channels() == 1)
		cv::cvtColor(drowDebugResult, drowDebugResult, CV_GRAY2BGR);
	//doUp();
	doDown();
	//doLeft();
	//doRight();
#else
	thread t1 = thread(std::mem_fn(&BlockEdgeDetectorT::doUp), this);
	thread t2 = thread(std::mem_fn(&BlockEdgeDetectorT::doDown), this);
	thread t3 = thread(std::mem_fn(&BlockEdgeDetectorT::doLeft), this);
	thread t4 = thread(std::mem_fn(&BlockEdgeDetectorT::doRight), this);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
#endif
}

BlockEdgeDetectorT::~BlockEdgeDetectorT()
{
	p_faults = NULL;
	p_block = NULL;
}


void BlockEdgeDetectorT::process(vector<cv::Point2f> contour)
{
	vector<double> angles;

	const int _SPAN = 1;
	for (int j = _SPAN; j < contour.size() - _SPAN; j++)
	{
		if (contour[j - _SPAN].y == contour[j + _SPAN].y)
			angles.push_back(0);
		else if (contour[j - _SPAN].x == contour[j + _SPAN].x)
			angles.push_back(90);
		else
		{
			double k = (-1) * (double)(contour[j + _SPAN].y - contour[j - _SPAN].y) / (double)(contour[j + _SPAN].x - contour[j - _SPAN].x);
			double  angle = atan(k) / CV_PI * 180;
			angles.push_back(angle);
		}
	}



	ofstream ofs("up.txt", ios::out);
	ofs << "L2R,";
	for (int i = 0; i < angles.size(); i++)
	{
		ofs << angles[i] << ",";
	}



	vector<int> updown;
	for (int j = 1; j < contour.size(); j++)
	{
		if (contour[j].y == contour[j - 1].y)
			updown.push_back(0);
		else
			updown.push_back(contour[j].y - contour[j - 1].y);
	}
	for (int i = 0; i < updown.size(); i++)
	{
		if (i < 5)
			updown[i] = 0;
		else
		{

		}
	}


	ofstream ofs2("up_updown.txt", ios::out);
	for (int i = 0; i < updown.size(); i++)
	{
		ofs2 << updown[i] << ",";
	}
}


//void BlockEdgeDetectorT::process(vector<vector<cv::Point>> contours)
//{
//	vector<double> angles;
//	for (int i = 0; i < contours.size(); i++)
//	{
//		if (contours[i].size() < 500)
//			continue;
//		else
//		{
//			const int _SPAN = 5;
//
//			int startj = -1;
//			for (int j = 0; j < contours[i].size() - _SPAN; j++)
//				if (contours[i][j].x != 1)
//					continue;
//				else
//				{
//					startj = j;
//					break;
//				}
//
//			if (startj != -1)
//				for (int j = startj + _SPAN; j < contours[i].size() - _SPAN; j++)
//				{
//					//找到折返点
//					if (contours[i][j - 1] == contours[i][j + 1])
//						break;
//					if (contours[i][j - _SPAN].y == contours[i][j + _SPAN].y)
//						angles.push_back(0);
//					else if (contours[i][j - _SPAN].x == contours[i][j + _SPAN].x)
//						angles.push_back(90);
//					else
//					{
//						double k = (-1) * (double)(contours[i][j + _SPAN].y - contours[i][j - _SPAN].y) / (double)(contours[i][j + _SPAN].x - contours[i][j - _SPAN].x);
//						double  angle = atan(k) / CV_PI * 180;
//						angles.push_back(angle);
//					}
//				}
//		}
//	}
//
//
//	ofstream ofs("up.txt", ios::out);
//	ofs << "L2R,";
//	for (int i = 0; i < angles.size(); i++)
//	{
//		ofs << angles[i] << ",";
//	}
//}

void BlockEdgeDetectorT::testag(vector<cv::Point2f> contour)
{
	vector<double> partk;
	const int partspan = 100;
	partk.push_back(p_block->UpLine.k);
	for (int i = 0; i + partspan < contour.size(); i += partspan)
	{
		vector<cv::Point> points;
		for (int j = i; j < i + partspan; j+= 10)
		{
			points.push_back(contour[j]);
		}
		cv::Vec4f line4f;
		fitLine(cv::Mat(points), line4f, CV_DIST_L2, 0, 0.01, 0.01);

		double dx = line4f[0];
		double dy = line4f[1];
		partk.push_back(dy / dx);
	}

	for (int i = 0; i < partk.size(); i++)
	{

	}
}
void BlockEdgeDetectorT::getContoursUpDown(cv::Mat binaryImage, vector<cv::Point2f>& contour)
{
	cv::Mat img = binaryImage;
	int imgwidth = img.cols;
	int imgheight = img.rows;
	//边界追踪
	cv::Point startpoint;
	//寻找第一个点
	for (int i = 0; i < imgwidth; i++)//列
	{
		bool flag = 0;
		//循环查找边界
		for (int j = 0; j < imgheight; j++)//行
		{
			if (img.ptr<uchar>(j)[i] > 0)
			{
				startpoint.y = j;
				startpoint.x = i;
				flag = 1;
				break;
			}
		}
		if (flag)
			break;
	}

	contour.clear();
	contour.push_back(startpoint);
	cv::Point v[8];
	v[0] = cv::Point(-1, -1);//左上
	v[1] = cv::Point(0, -1);//上
	v[2] = cv::Point(1, -1);//右上
	v[3] = cv::Point(1, 0);//右
	v[4] = cv::Point(1, 1);//右下
	v[5] = cv::Point(0, 1);//下
	v[6] = cv::Point(-1, 1);//左下
	v[7] = cv::Point(-1, 0);//左
	while (true)
	{
		cv::Point lastpoint(contour[contour.size() - 1]);
		cv::Point lastlastpoint = lastpoint;
		if (contour.size() > 1)
			lastlastpoint = contour[contour.size() - 2];

		//追到左边界或者右边界时退出
		if (lastpoint.x == (imgwidth - 1) || lastpoint.y == (imgheight - 1))
			break;

		//寻找上一点的右边那一点（八邻域范围）
		int start = 0;
		for (int i = 0; i < 8; i++)
		{
			if ((lastpoint + v[i]) == lastlastpoint)
			{
				if (i == 7)
					start = 0;
				else
					start = i + 1;
				break;
			}
		}

		int count = 0;
		bool flag = 0;//当取到v[7]时，表示边界追踪开始往左，则退出。
		while (true)
		{
			int i = count + start;
			if (i >= 8) i -= 8;
			cv::Point nextpoint = lastpoint + v[i];
			if (//nextpoint != lastlastpoint &&
				nextpoint.x >= 0 && nextpoint.y >= 0 &&
				nextpoint.x < imgwidth && nextpoint.y < imgheight &&
				img.ptr<uchar>(nextpoint.y)[nextpoint.x] > 0)
			{
				if (i == 7)
					flag = 1;
				contour.push_back(nextpoint);
				break;
			}
			count++;
			if (count >= 8)
				break;
		}

		if (flag)
			break;
	}
	//平滑滤波
	//for (size_t i = 1; i < contour.size() - 1; i++)
	//{
	//	contour[i].x = (contour[i - 1].x + contour[i + 1].x) / 2;
	//	contour[i].y = (contour[i - 1].y + contour[i + 1].y) / 2;
	//}
	//for (size_t i = 1; i < contour.size() - 1; i++)
	//{
	//	contour[i].x = (contour[i - 1].x + contour[i + 1].x) / 2;
	//	contour[i].y = (contour[i - 1].y + contour[i + 1].y) / 2;
	//}



	//for (int i = 0; i < contour.size(); i++)
	//{
	//	contour[i].y = 30 + contour[i].y - ((int)(p_block->UpLine.k*(i - 0)) + contour[0].y);
	//}

#ifdef BED_OUTPUT_DEBUG_INFO
	//验证绘图
	cv::Mat draw(imgheight,imgwidth,CV_8U, cv::Scalar(0));
	for (int i = 0; i < contour.size(); i++)
	{
		draw.ptr<uchar>((int)contour[i].y)[(int)contour[i].x] = 255;
	}
#endif

	int s = contour.size();
}

void BlockEdgeDetectorT::doUp()
{
	const int ROI_WIDTH = 50;
	const int ROI_HEIGHT = 100;
	int inc = ROI_WIDTH - 1;

	int index = 0;
	vector<cv::Point> points;
	vector<cv::Point> pointsL;//低阈值边界点
	vector<cv::Point> pointsH;//高阈值边界点

	int x1, y1, x2, y2;
	y1 = p_block->A.y < p_block->B.y ? p_block->A.y : p_block->B.y;
	x1 = p_block->A.x + 100;
	y2 = p_block->A.y > p_block->B.y ? p_block->A.y : p_block->B.y;
	y2 += ROI_HEIGHT / 2;
	x2 = p_block->B.x - 100;

	if (x1 <0 || x1 > image.rows ||
		y1 <0 || y1 > image.cols ||
		x2 <0 || x2 > image.rows ||
		y2 <0 || y2 > image.cols)
	{
		return;
	}


	cv::Mat roi = image(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2))).clone();
	cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0);
	//cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0);
	cv::Mat lowTI, highTI;
	cv::threshold(roi, lowTI, 10, 255, CV_THRESH_BINARY);
	cv::threshold(roi, highTI, 30, 255, CV_THRESH_BINARY);

	vector<cv::Point2f> tmpcontourLow;
	getContoursUpDown(lowTI, tmpcontourLow);
	//process(tmpcontourLow);
	testag(tmpcontourLow);
	vector<cv::Point2f> tmpcontourHeight;
	getContoursUpDown(highTI, tmpcontourHeight);
	process(tmpcontourHeight);


	//cv::Canny(highTI, highTI, 125, 125);
	//cv::Canny(lowTI, lowTI, 125, 125);
	//vector<vector<cv::Point>> tmpcontoursL;
	//cv::findContours(lowTI.clone(), tmpcontoursL, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	//process(tmpcontoursL);
	//vector<vector<cv::Point>> tmpcontoursH;
	//cv::findContours(highTI.clone(), tmpcontoursH, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	//process(tmpcontoursH);




#ifdef BED_OUTPUT_DEBUG_INFO
	for (size_t i = 0; i < points.size(); i++)
	{
		stringstream ss;
		ss << i;
		cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0));
		cv::circle(drowDebugResult, points[i], 2, cv::Scalar(255, 0, 255), -1);
	}
#endif
}
void BlockEdgeDetectorT::doDown()
{
	const int ROI_WIDTH = 50;
	const int ROI_HEIGHT = 100;
	int inc = ROI_WIDTH - 1;

	int index = 0;
	vector<cv::Point> points;
	vector<cv::Point> pointsL;//低阈值边界点
	vector<cv::Point> pointsH;//高阈值边界点


	//下边界
	int x1, y1, x2, y2;
	y1 = p_block->D.y < p_block->C.y ? p_block->D.y : p_block->C.y;
	x1 = p_block->D.x + 100;
	y1 -= ROI_HEIGHT / 2;
	y2 = p_block->D.y > p_block->C.y ? p_block->D.y : p_block->C.y;
	y2 += ROI_HEIGHT / 2;
	x2 = p_block->C.x - 100;

	if (x1 <0 || x1 > image.cols ||
		y1 <0 || y1 > image.rows || 
		x2 <0 || x2 > image.cols ||
		y2 <0 || y2 > image.rows)
	{
		return;
	}

	cv::Mat roi = image(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2))).clone();
	cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0);
	//cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0);
	cv::Mat lowTI, highTI;
	cv::threshold(roi, lowTI, 10, 255, CV_THRESH_BINARY_INV);
	cv::threshold(roi, highTI, 30, 255, CV_THRESH_BINARY_INV);

	vector<cv::Point2f> tmpcontourLow;
	getContoursUpDown(lowTI, tmpcontourLow);
	//process(tmpcontourLow);
	testag(tmpcontourLow);
	vector<cv::Point2f> tmpcontourHeight;
	getContoursUpDown(highTI, tmpcontourHeight);
	process(tmpcontourHeight);
}
void BlockEdgeDetectorT::doLeft()
{
	const int ROI_WIDTH = 66;
	const int ROI_HEIGHT = 50;
	int inc = 25;//(float)(endY - startY) / 60 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//左边界
	int startY = p_block->A.y + 150;
	int endY = p_block->D.y - 150;
	for (int y = startY; y < endY && y < image.rows; y += inc, index++)
	{
		int y1 = y;
		if (y1 < 0 || y1 >= image.rows)
			continue;
		if ((y1 + ROI_HEIGHT) >= endY)
			y1 = endY - ROI_HEIGHT - 1;
		if ((y1 + ROI_HEIGHT) >= image.rows)
			y1 = image.rows - ROI_HEIGHT - 1;

		int x = p_block->GetPonintByY(y1, &p_block->LeftLine).x;
		if (x < 0 || x >= image.cols)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x, y1, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(9, 9), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_lefts.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\L\\左_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\L\\左_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		//tmpROI.release();

		//reduceImg.convertTo(reduceImg, CV_32F);
		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByY(y, &p_block->LeftLine));

	}
	processLeftRight(reduceList, points);
	//processAndSaveData(reduceList, points, "L\\左");
}
void BlockEdgeDetectorT::doRight()
{
	const int ROI_WIDTH = 66;
	const int ROI_HEIGHT = 50;
	int inc = 25;//(float)(endY - startY) / 60 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//右边界
	int startY = p_block->B.y + 150;
	int endY = p_block->D.y - 150;
	for (int y = startY; y < endY && y < image.rows; y += inc, index++)
	{
		int y1 = y;
		if (y1 < 0 || y1 >= image.rows)
			continue;
		if ((y1 + ROI_HEIGHT) >= endY)
			y1 = endY - ROI_HEIGHT - 1;
		if ((y1 + ROI_HEIGHT) >= image.rows)
			y1 = image.rows - ROI_HEIGHT - 1;

		int x = p_block->GetPonintByY(y1, &p_block->RightLine).x;
		if (x < 0 || x >= image.cols)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x - ROI_WIDTH, y1, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_rights.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\R\\右_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\R\\右_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		//reduceImg.convertTo(reduceImg, CV_32F);
		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByY(y, &p_block->RightLine));
	}
	processLeftRight(reduceList, points);
}



void BlockEdgeDetectorT::processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points)
{
#ifdef BED_OUTPUT_DEBUG_INFO
	for (size_t i = 0; i < points.size(); i++)
	{
		stringstream ss;
		ss << i;
		cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0));
		cv::circle(drowDebugResult, points[i], 2, cv::Scalar(125, 125, 255), -1);
	}
#endif

	//筛选出错误点
	vector<int> errorPointsIndex;
	vector<int> errorPointsDeep;
	if (reduceList.size() > 2)
	{
		for (int i = 0; i < reduceList.size() - 2; i++)
		{
//#ifdef BED_OUTPUT_DEBUG_INFO
//			cv::Mat img1 = debug_lefts[i];
//			cv::Mat img2 = debug_lefts[i + 1];
//			cv::Mat r1 = reduceList[i];
//			cv::Mat r2 = reduceList[i + 1];
//#endif
			cv::Mat diffresult;
			cv::absdiff(reduceList[i], reduceList[i + 1], diffresult);
			double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
			cv::Point maxLoc;
			minMaxLoc(diffresult, NULL, &maxVal, NULL, &maxLoc);
			if (maxdiff_X < maxVal) maxdiff_X = maxVal;//定标用统计
			if (maxVal > DIFF_THRESHOLD)
			{
				int count = 0;
				int x = maxLoc.x;
				for (int j = maxLoc.x; j < diffresult.cols; j++)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - x) > FAULTS_SPAN)
						break;
					//判断值是否大于阈值
					if (diffresult.ptr<float>(0)[j] > DIFF_THRESHOLD)
					{
						count++;
						x = j;
					}
				}
				x = maxLoc.x;
				for (int j = maxLoc.x; j >= 0; j--)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - x) > FAULTS_SPAN)
						break;
					//判断值是否大于阈值
					if (diffresult.ptr<float>(0)[j] > DIFF_THRESHOLD && abs((float)j - x) <= FAULTS_SPAN)
					{
						count++;
						x = j;
					}
				}
				if (count > FAULTS_COUNT)
				{
					errorPointsIndex.push_back(i);//记录下是第几个reduceList
					errorPointsDeep.push_back(count);//记录连续错误几个点
#ifdef BED_OUTPUT_DEBUG_INFO
						stringstream ss;
						ss << i;
						cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
						cv::circle(drowDebugResult, points[i], 2, cv::Scalar(0, 255, 255), -1);
#endif
				}
				if (maxDeep_X < count) maxDeep_X = count;//定标用统计
			}
			continue;
		}
	}

	//根据错误点生成BrokenEdge实体
	if (errorPointsIndex.size() > 0)
	{
		int absolutDeep = 0;
		int startIndex = errorPointsIndex[0];
		int endIndex = errorPointsIndex[0];
		for (int i = 0; i < errorPointsIndex.size(); i++)
		{
			if (errorPointsIndex[i] - startIndex < 3)
			{
				endIndex = errorPointsIndex[i];
				absolutDeep = errorPointsDeep[i] > absolutDeep ? errorPointsDeep[i] : absolutDeep;
			}
			if (errorPointsIndex[i] - startIndex >= 3 || i == errorPointsIndex.size() - 1)
			{
				if (startIndex == endIndex)
				{
					if (startIndex == (points.size() - 1))
						startIndex = (points.size() - 2);
					else if (endIndex == 0)
						endIndex = 1;
					else
					{
						startIndex--;
						endIndex++;
					}
				}

				int x = (points[startIndex].x + points[endIndex].x) / 2;
				int y = (points[startIndex].y + points[endIndex].y) / 2;
				Faults::BrokenEdge be;
				be.position = cv::Point(x, y);
				be.deep = absolutDeep;
				be.length = abs(points[endIndex].y - points[startIndex].y);
				p_faults->BrokenEdges.push_back(be);

				startIndex = errorPointsIndex[i];
				endIndex = errorPointsIndex[i];
			}
		}
	}
}
void BlockEdgeDetectorT::processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points)
{
	vector<int> errorPointsIndex;
	vector<int> errorPointsDeep;
	if (reduceList.size() > 2)
	{
		for (int i = 0; i < reduceList.size() - 2; i++)
		{
			cv::Mat diffresult;
			cv::absdiff(reduceList[i], reduceList[i + 1], diffresult);
			double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
			cv::Point maxLoc;
			minMaxLoc(diffresult, NULL, &maxVal, NULL, &maxLoc);
			if (maxdiff_Y < maxVal) maxdiff_Y = maxVal;//定标用统计
			if (maxVal > DIFF_THRESHOLD)
			{
				int count = 0;
				int y = maxLoc.y;
				for (int j = maxLoc.y; j < diffresult.rows; j++)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - y) > FAULTS_SPAN) break;
					//判断值是否大于阈值
					if (diffresult.ptr<float>(j)[0] > DIFF_THRESHOLD)
					{
						count++;
						y = j;
					}
				}
				y = maxLoc.y;
				for (int j = maxLoc.y; j >= 0; j--)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - y) > FAULTS_SPAN) break;
					//判断值是否大于阈值
					if (diffresult.ptr<float>(j)[0] > DIFF_THRESHOLD && abs((float)j - j) <= FAULTS_SPAN)
					{
						count++;
						y = j;
					}
				}
				if (count > FAULTS_COUNT)
				{
					errorPointsIndex.push_back(i);
					errorPointsDeep.push_back(count);
#ifdef BED_OUTPUT_DEBUG_INFO
					stringstream ss;
					ss << i;
					cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
					cv::circle(drowDebugResult, points[i], 2, cv::Scalar(0, 255, 255), -1);
#endif
				}
				if (maxDeep_Y < count) maxDeep_Y = count;//定标用统计
			}
			continue;
		}
	}


	if (errorPointsIndex.size() > 0)
	{
		int absolutDeep = 0;
		int startIndex = errorPointsIndex[0];
		int endIndex = errorPointsIndex[0];
		for (int i = 0; i < errorPointsIndex.size(); i++)
		{
			if (errorPointsIndex[i] - startIndex < 3)
			{
				endIndex = errorPointsIndex[i];
				absolutDeep = errorPointsDeep[i] > absolutDeep ? errorPointsDeep[i] : absolutDeep;
			}
			if (errorPointsIndex[i] - startIndex >= 3 || i == errorPointsIndex.size() - 1)
			{
				if (startIndex == endIndex)
				{
					if (startIndex == (points.size() - 1))
						startIndex = (points.size() - 2);
					else if (endIndex == 0)
						endIndex = 1;
					else
					{
						startIndex--;
						endIndex++;
					}
				}

				int x = (points[startIndex].x + points[endIndex].x) / 2;
				int y = (points[startIndex].y + points[endIndex].y) / 2;
				Faults::BrokenEdge be;
				be.position = cv::Point(x, y);
				be.deep = absolutDeep;
				be.length = abs(points[endIndex].y - points[startIndex].y);
				p_faults->BrokenEdges.push_back(be);

				startIndex = errorPointsIndex[i];
			}
		}
	}
}

//void ImageBinarization(IplImage *src)
//{
//	int i, j, width, height, step, chanel, threshold;
//
//	float size, avg, va, maxVa, p, a, s;
//	unsigned char *dataSrc;
//	float histogram[256];
//
//	width = src->width;
//	height = src->height;
//	dataSrc = (unsigned char *)src->imageData;
//	step = src->widthStep / sizeof(char);
//	chanel = src->nChannels;
//
//	for (i = 0; i<256; i++)
//		histogram[i] = 0;
//	for (i = 0; i<height; i++)
//		for (j = 0; j<width*chanel; j++)
//		{
//			histogram[dataSrc[i*step + j] - '0' + 48]++;
//		}
//	size = width * height;
//	for (i = 0; i<256; i++)
//		histogram[i] /= size;
//
//	avg = 0;
//	for (i = 0; i<256; i++)
//		avg += i*histogram[i];
//	va = 0;
//	for (i = 0; i<256; i++)
//		va += fabs(i*i*histogram[i] - avg*avg);
//
//	threshold = 20;
//	maxVa = 0;
//	p = a = s = 0;
//	for (i = 0; i<256; i++)
//	{
//		p += histogram[i];
//		a += i*histogram[i];
//		s = (avg*p - a)*(avg*p - a) / p / (1 - p);
//		if (s > maxVa)
//		{
//			threshold = i;
//			maxVa = s;
//		}
//	}
//
//	for (i = 0; i<height; i++)
//		for (j = 0; j<width*chanel; j++)
//		{
//			if (dataSrc[i*step + j] > threshold)
//				dataSrc[i*step + j] = 255;
//			else
//				dataSrc[i*step + j] = 0;
//		}
//}