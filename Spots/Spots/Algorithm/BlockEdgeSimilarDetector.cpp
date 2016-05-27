#include "BlockEdgeSimilarDetector.h"
#include <Class\Debug\MFCConsole.h>

#include <string>
#include <iostream>
#include <fstream>

BlockEdgeSimilarDetector::BlockEdgeSimilarDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	if (image.channels() == 3)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	p_block = _block;
	p_faults = _faults;
}
void BlockEdgeSimilarDetector::Run()
{
#ifdef BED_OUTPUT_DEBUG_INFO
	drowDebugResult = image.clone();
	if (drowDebugResult.channels() == 1)
		cv::cvtColor(drowDebugResult, drowDebugResult, CV_GRAY2BGR);
	doUp();
	doDown(); 
	doLeft();
	doRight();
#else
	thread t1 = thread(std::mem_fn(&BlockEdgeSimilarDetector::doUp), this);
	thread t2 = thread(std::mem_fn(&BlockEdgeSimilarDetector::doDown), this);
	thread t3 = thread(std::mem_fn(&BlockEdgeSimilarDetector::doLeft), this);
	thread t4 = thread(std::mem_fn(&BlockEdgeSimilarDetector::doRight), this);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
#endif


	double maxdiff = (maxdiff_X > maxdiff_Y) ? maxdiff_X : maxdiff_Y;
	int maxDeep = (maxDeep_X > maxDeep_Y) ? maxDeep_X : maxDeep_Y;
	stringstream ss;
	ss << "BED定标消息：最大不相似度=" << maxdiff << "，缺陷最大深度 = " << maxDeep << "pix" << endl;
	MFCConsole::Output(ss.str());
}

BlockEdgeSimilarDetector::~BlockEdgeSimilarDetector()
{
	p_faults = NULL;
	p_block = NULL;
}
void BlockEdgeSimilarDetector::doUp()
{
	// 如果左边界直接在图像边缘上，则不进行检测。
	if (p_block->A.y < 2 && p_block->B.y < 2)
		return;


	//计算出上边缘的倒角深度，以该深度取ROI比较相似度

	int deep1 = getDeepUp(p_block->GetPonintByX((p_block->A.x + p_block->B.x) / 2, &p_block->UpLine));//中点深度
	int deep2 = getDeepUp(p_block->GetPonintByX((2 * p_block->A.x + p_block->B.x) / 3, &p_block->UpLine));//1/3点深度
	int deep3 = getDeepUp(p_block->GetPonintByX((p_block->A.x + 2 * p_block->B.x) / 3, &p_block->UpLine));//2/3点深度

	ROI_HEIGHT_UP = (deep1 + deep2 + deep3) / 3;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;

	//上边界
	int startX = p_block->A.x + 55;
	if (startX < 0)
		startX = 0;
	int endX = p_block->B.x - 55;
	if (endX >= image.cols)
		endX = image.cols - 1;

	for (int x = startX; x < endX && x < image.cols; x += ROI_WIDTH_INC, index++)
	{
		int x1 = x;
		if (x < 0 || x >= image.cols)
			continue;
		if ((x + ROI_WIDTH_UPDOWN) >= endX)
			x1 = endX - ROI_WIDTH_UPDOWN - 1;
		if ((x + ROI_WIDTH_UPDOWN) >= image.cols)
			x1 = image.cols - ROI_WIDTH_UPDOWN - 1;

		int y = p_block->GetPonintByX(x1, &p_block->UpLine).y;
		if (y < 0 || y >= image.rows)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x1, y, ROI_WIDTH_UPDOWN, ROI_HEIGHT_UP)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_ups.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(3, 3), 0);
		//cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "BlockEdgeSimilarDetector\\up\\上_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\up\\上_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
		cv::Mat hist(100, reduceImg.rows, CV_8U, cv::Scalar(0));
		for (size_t y = 0; y < reduceImg.rows; y++)
		{
			int count = reduceImg.ptr<float>(y)[0];
			int x = y;
			for (int i = 99; i >= 0 && count > 0; i--)
			{
				count--;
				hist.ptr<uchar>(i)[x] = 255;
			}
		}
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\up\\上_" << strnum << "hist.jpg";
		cv::imwrite(ss.str(), hist);
#endif

		tmpROI.release();
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point(x, p_block->GetPonintByX(x, &p_block->UpLine).y));
	}
	process(reduceList, points, "up");
}
int BlockEdgeSimilarDetector::getDeepUp(cv::Point p)
{
	int deep = 20;

	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 0 || point_y >= image.rows)
		return 0;

	cv::Mat roi = image(cv::Rect(point_x - 30, point_y, 60, 100));
	cv::Mat reduceImg(1, image.cols, CV_32S);
	cv::reduce(roi, reduceImg, 1, CV_REDUCE_SUM, CV_32S);
	int maxDiff = 0;
	for (int i = 1; i < reduceImg.rows; i++)
	{
		int diff = abs(reduceImg.ptr<int>(i)[0] - reduceImg.ptr<int>(i - 1)[0]);
		if (maxDiff < diff)
		{
			maxDiff = diff;
			if (deep < i)
				deep = i;
		}
	}
	return deep + 5;
}
void BlockEdgeSimilarDetector::doDown()
{
	// 如果边界直接在图像边缘上，则不进行检测。
	if (p_block->C.y >= image.rows - 2 && p_block->D.y >= image.rows - 2)
		return;

	//计算出下边缘的倒角深度，以该深度取ROI比较相似度

	int deep1 = getDeepUp(p_block->GetPonintByX((p_block->A.x + p_block->B.x) / 2, &p_block->UpLine));//中点深度
	int deep2 = getDeepUp(p_block->GetPonintByX((2 * p_block->A.x + p_block->B.x) / 3, &p_block->UpLine));//1/3点深度
	int deep3 = getDeepUp(p_block->GetPonintByX((p_block->A.x + 2 * p_block->B.x) / 3, &p_block->UpLine));//2/3点深度

	ROI_HEIGHT_DOWN = (deep1 + deep2 + deep3) / 3;



	int inc = 25;//(float)(endX - startX) / 30 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//下边界
	int startX = p_block->D.x + 55;
	if (startX < 0)
		startX = 0;
	int endX = p_block->C.x - 55;
	if (endX >= image.cols)
		endX = image.cols - 1;

	for (int x = startX; x < endX && x < image.cols; x += inc, index++)
	{
		int x1 = x;
		if (x < 0 || x >= image.cols)
			continue;
		if ((x + ROI_WIDTH_UPDOWN) >= endX)
			x1 = endX - ROI_WIDTH_UPDOWN - 1;
		if ((x + ROI_WIDTH_UPDOWN) >= image.cols)
			x1 = image.cols - ROI_WIDTH_UPDOWN - 1;


		int y = p_block->GetPonintByX(x1, &p_block->DownLine).y;
		if (y < 0 || y >= image.rows)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x1, y - ROI_HEIGHT_DOWN, ROI_WIDTH_UPDOWN, ROI_HEIGHT_DOWN)).clone();

		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_downs.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(3, 3), 0);
		//cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "BlockEdgeSimilarDetector\\down\\下_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\down\\下_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);

		cv::Mat hist(100, reduceImg.rows, CV_8U, cv::Scalar(0));
		for (size_t y = 0; y < reduceImg.rows; y++)
		{
			int count = reduceImg.ptr<float>(y)[0];
			int x = y;
			for (int i = 99; i >= 0 && count > 0; i--)
			{
				count--;
				hist.ptr<uchar>(i)[x] = 255;
			}
		}
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\down\\下_" << strnum << "hist.jpg";
		cv::imwrite(ss.str(), hist);
#endif

		tmpROI.release();

		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByX(x, &p_block->DownLine));
	}
	process(reduceList, points, "down");
}
int BlockEdgeSimilarDetector::getDeepDown(cv::Point p)
{
	int deep = 20;

	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 0 || point_y >= image.rows)
		return 0;

	cv::Mat roi = image(cv::Rect(point_x - 30, point_y - 100, 60, 100));
	cv::Mat reduceImg(1, image.cols, CV_32S);
	cv::reduce(roi, reduceImg, 1, CV_REDUCE_SUM, CV_32S);
	int maxDiff = 0;
	for (int i = reduceImg.rows - 1; i >= 1; i++)
	{
		int diff = abs(reduceImg.ptr<int>(i)[0] - reduceImg.ptr<int>(i - 1)[0]);
		if (maxDiff < diff)
		{
			maxDiff = diff;
			if (deep < (reduceImg.rows - i))
				deep = reduceImg.rows - i;
		}
	}
	return deep + 5;
}
void BlockEdgeSimilarDetector::doLeft()
{
	// 如果边界直接在图像边缘上，则不进行检测。
	if (p_block->A.x < 2 && p_block->D.x < 2)
		return;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//左边界
	int startY = p_block->A.y + 69;
	if (startY < 0)
		startY = 0;
	int endY = p_block->D.y - 69;
	if (endY >= image.rows)
		endY = image.rows - 1;
	for (int y = startY; y < endY && y < image.rows; y += ROI_HEIGHT_INC, index++)
	{
		int y1 = y;
		if (y1 < 0 || y1 >= image.rows)
			continue;
		if ((y1 + ROI_HEIGHT_LEFTRIGHT) >= endY)
			y1 = endY - ROI_HEIGHT_LEFTRIGHT - 1;
		if ((y1 + ROI_HEIGHT_LEFTRIGHT) >= image.rows)
			y1 = image.rows - ROI_HEIGHT_LEFTRIGHT - 1;

		int x = p_block->GetPonintByY(y1, &p_block->LeftLine).x;
		if (x < 0 || x >= image.cols)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x, y1, ROI_WIDTH_LEFTRIGHT, ROI_HEIGHT_LEFTRIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(9, 9), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_lefts.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(3, 3), 0);
		//cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半
		reduceImg = reduceImg.t();

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "BlockEdgeSimilarDetector\\left\\左_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\left\\左_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
		cv::Mat hist(100, reduceImg.rows, CV_8U, cv::Scalar(0));
		for (size_t y = 0; y < reduceImg.rows; y++)
		{
			int count = reduceImg.ptr<float>(y)[0];
			int x = y;
			for (int i = 99; i >= 0 && count > 0; i--)
			{
				count--;
				hist.ptr<uchar>(i)[x] = 255;
			}
		}
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\left\\左_" << strnum << "hist.jpg";
		cv::imwrite(ss.str(), hist);
#endif

		//tmpROI.release();

		//reduceImg.convertTo(reduceImg, CV_32F);
		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByY(y, &p_block->LeftLine));

	}
	process(reduceList, points, "left");
	//processAndSaveData(reduceList, points, "L\\左");
}
void BlockEdgeSimilarDetector::doRight()
{
	// 如果边界直接在图像边缘上，则不进行检测。
	if (p_block->B.y >= image.cols - 2 && p_block->C.y >= image.cols - 2)
		return;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//右边界
	int startY = p_block->B.y + 69;
	if (startY < 0)
		startY = 0;
	int endY = p_block->D.y - 69;
	if (endY >= image.rows)
		endY = image.rows - 1;
	for (int y = startY; y < endY && y < image.rows; y += ROI_HEIGHT_INC, index++)
	{	
		int y1 = y;
		if (y1 < 0 || y1 >= image.rows)
			continue;
		if ((y1 + ROI_HEIGHT_LEFTRIGHT) >= endY)
			y1 = endY - ROI_HEIGHT_LEFTRIGHT - 1;
		if ((y1 + ROI_HEIGHT_LEFTRIGHT) >= image.rows)
			y1 = image.rows - ROI_HEIGHT_LEFTRIGHT - 1;

		int x = p_block->GetPonintByY(y1, &p_block->RightLine).x;
		if (x < 0 || x >= image.cols)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x - ROI_WIDTH_LEFTRIGHT, y1, ROI_WIDTH_LEFTRIGHT, ROI_HEIGHT_LEFTRIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_rights.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(3, 3), 0);
		//cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半

		reduceImg = reduceImg.t();
#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "BlockEdgeSimilarDetector\\right\\右_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\right\\右_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
		cv::Mat hist(100, reduceImg.rows, CV_8U, cv::Scalar(0));
		for (size_t y = 0; y < reduceImg.rows; y++)
		{
			int count = reduceImg.ptr<float>(y)[0];
			int x = y;
			for (int i = 99; i >= 0 && count > 0; i--)
			{
				count--;
				hist.ptr<uchar>(i)[x] = 255;
			}
		}
		ss.str("");
		ss << "BlockEdgeSimilarDetector\\right\\右_" << strnum << "hist.jpg";
		cv::imwrite(ss.str(), hist);
#endif

		tmpROI.release();
		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByY(y, &p_block->RightLine));
	}
	process(reduceList, points, "right");
}


void BlockEdgeSimilarDetector::process(vector<cv::Mat> reduceList, vector<cv::Point> points, string info)
{
	if (points.size() != reduceList.size())
		return;

	vector<bool> pointFlag;

#ifdef BED_OUTPUT_DEBUG_INFO
	vector<double> diffs;
#endif
	for (size_t i = 0; i < points.size(); i++)
	{
		pointFlag.push_back(0);
	}

#ifdef BED_OUTPUT_DEBUG_INFO
	for (size_t i = 0; i < points.size(); i++)
	{
		stringstream ss;
		ss << i;
		cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0));
		cv::circle(drowDebugResult, points[i], 2, cv::Scalar(255, 0, 255), -1);
	}
#endif

	//vector<int> errorPointsIndex;
	if (reduceList.size() > 2)
	{
		const int span = 2;//相似度计算间隔
		for (int i = 1; i < reduceList.size() - 1 - span; i++)
		{
			cv::Mat hist1 = reduceList[i];
			cv::Mat hist2 = reduceList[i + span];
			double diff = getFrechetDistance(hist1, hist2); //越大越像

#ifdef BED_OUTPUT_DEBUG_INFO
			diffs.push_back(diff);

			double fdis = getFrechetDistance(hist1, hist2);
			double diff1 = cv::compareHist(hist1, hist2, CV_COMP_CORREL); //越大越像
			double diff2 = cv::compareHist(hist1, hist2, CV_COMP_CHISQR); //越小越像
			double diff3 = cv::compareHist(hist1, hist2, CV_COMP_INTERSECT); //越大越像
			double diff4 = cv::compareHist(hist1, hist2, CV_COMP_BHATTACHARYYA); //越小越像
#endif

			if (diff > 18)
			//if (diff < DIFF_THRESHOLD)
			{
				pointFlag[i] = 1;
				pointFlag[i + span] = 1;
				//errorPointsIndex.push_back(i);
#ifdef BED_OUTPUT_DEBUG_INFO
				stringstream ss;
				ss << i;
				cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
				ss.str("");
				ss << i + span;
				cv::putText(drowDebugResult, ss.str(), points[i + span], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
				//cv::circle(drowDebugResult, points[i], 2, cv::Scalar(0, 255, 255), -1);
#endif
			}
			//else if (pointFlag[i])
			//{
			//	pointFlag[i] = 0;
			//}
		}
	}
	//不合并缺陷
	if (points.size() > 2)
	{
		//int pointSpanX = abs(points[1].x - points[0].x);
		//int pointSpanY = abs(points[1].y - points[0].y);
		//int length = pointSpanY > pointSpanX ? pointSpanY : pointSpanX;
		//length = length + length;
		for (int i = 0; i < pointFlag.size(); i++)
		{
			if (pointFlag[i])
			{
				Faults::EdgeFault ef;

				if (info == "up")
				{
					ef.width = ROI_WIDTH_UPDOWN;
					ef.height = ROI_HEIGHT_UP;
				}
				else if (info == "down")
				{
					ef.width = ROI_WIDTH_UPDOWN;
					ef.height = ROI_HEIGHT_DOWN;
				}
				else
				{
					ef.width = ROI_WIDTH_LEFTRIGHT;
					ef.height = ROI_HEIGHT_LEFTRIGHT;
				}
				ef.position = points[i];
				p_faults->EdgeFaults.push_back(ef);
			}
		}
	}



#ifdef BED_OUTPUT_DEBUG_INFO
	if (1 == 1)
	{
		stringstream ss;
		ss << "BlockEdgeSimilarDetector\\" << info << ".txt";
		ofstream of(ss.str(), ios::out);
		for (size_t i = 0; i < diffs.size(); i++)
		{
			of << diffs[i] << ",";
		}
	}
#endif
}

double BlockEdgeSimilarDetector::getFrechetDistance(vector<double> lineA, vector<double> lineB)
{
	int lineASize = lineA.size();
	int lineBSize = lineB.size();

	double dist = 100000;
	if (lineBSize > lineASize)
		return dist;

	vector<double> tmpDists;
	//计算初始距离
	double tmpDist = -1;
	//左移、右移N次距离
	for (int offset = -2; offset < 2; offset++)
	{
		tmpDist = -1;
		int x2 = offset;
		for (int x1 = 0; x1 < lineASize; x1++)
		{
			if (x2 >(lineBSize - 1))
				break;
			if (x2 < 0)
			{
				x2++;
				continue;
			}
			double y1 = (double)lineA[x1];
			double y2 = (double)lineB[x2];
			double thisTurnDist = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
			if (tmpDist < thisTurnDist)//保存每轮的最大值
				tmpDist = thisTurnDist;
			x2++;
		}
		if (tmpDist != -1)
			tmpDists.push_back(tmpDist);
		else if (tmpDist == 0)
		{
			return 0;
		}
	}
	for (int i = 0; i < tmpDists.size(); i++)//返回最大值数组中的最小值即是距离
	{
		if (dist > tmpDists[i])
			dist = tmpDists[i];
	}
	return dist;
}
double BlockEdgeSimilarDetector::getFrechetDistance(cv::Mat lineA, cv::Mat lineB)
{
	int lineASize = lineA.rows;
	int lineBSize = lineB.rows;
	if (lineA.rows == 1 && lineB.rows == 1)
	{
		lineASize = lineA.cols;
		lineBSize = lineB.cols;
	}
	else if (lineA.cols == 1 && lineB.cols == 1)
	{

	}
	else
	{
		return 100000;
	}

	double dist = 100000;
	if (lineBSize > lineASize)
		return dist;

	vector<double> tmpDists;
	//计算初始距离
	double tmpDist = -1;
	//左移、右移N次距离
	for (int offset = 0; offset < 10; offset++)
	{
		tmpDist = -1;
		int x2 = offset;
		for (int x1 = 0; x1 < lineASize; x1++)
		{
			if (x2 >(lineBSize - 1))
				break;
			if (x2 < 0)
			{
				x2++;
				continue;
			}
			double y1 = 0;//(double)lineA.ptr<double>(x1)[0];
			double y2 = 0;//(double)lineB.ptr<double>(x2)[0];
			if (lineA.rows == 1 && lineB.rows == 1)
			{
				y1 = (float)lineA.ptr<float>(0)[x1];
				y2 = (float)lineB.ptr<float>(0)[x2];
			}
			else
			{
				y1 = (float)lineA.ptr<float>(x1)[0];
				y2 = (float)lineB.ptr<float>(x2)[0];
			}
			double thisTurnDist = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
			if (tmpDist < thisTurnDist)//保存每轮的最大值
				tmpDist = thisTurnDist;
			x2++;
		}
		if (tmpDist != -1)
			tmpDists.push_back(tmpDist);
		else if (tmpDist == 0)
		{
			return 0;
		}
	}
	for (int i = 0; i < tmpDists.size(); i++)//返回最大值数组中的最小值即是距离
	{
		if (dist > tmpDists[i])
			dist = tmpDists[i];
	}
	return dist;
}