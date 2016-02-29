#include "BlockLocalizer.h"


BlockLocalizer::BlockLocalizer(cv::Mat& _img, Block* _block, Faults* _faults)
{
	img = _img;
	block = _block;
	faults = _faults;

	NotFoundBlockFlag = true;//未找到瓷砖标记
	BrokenEdgeFlag = false;//有崩边缺陷标记

#ifdef BD_OUTPUT_DEBUG_INFO
	bool test = 1;
	if (img.channels() == 1)
	{
		cvtColor(img, drowDebugDetectLR, CV_GRAY2BGR);
		drowDebugDetectUD = drowDebugDetectLR.clone();
		drowDebugResult = drowDebugDetectLR.clone();
	}
	else
	{
		drowDebugDetectLR = img.clone();
		drowDebugDetectUD = img.clone();
		drowDebugResult = img.clone();
	}
#endif


	FindUp();
	if (uppoints.size() < 5)
	{
		return;
	}
	thread t1 = thread(std::mem_fn(&BlockLocalizer::FindLeft), this);
	thread t2 = thread(std::mem_fn(&BlockLocalizer::FindRight), this);
	t1.join();
	t2.join();
	//FindLeft();
	//FindRight();
	if (leftpoints.size() < 5 || rightpoints.size() < 5)
	{
		return;
	}

	FindDown();
	Judgement();
	block->Lines2ABCD();
	NotFoundBlockFlag = false;


#ifdef BD_OUTPUT_DEBUG_INFO
	cv::line(drowDebugResult, cv::Point(0, (*block).UpLine.k * (0 - (*block).UpLine.x0) + (*block).UpLine.y0), cv::Point(drowDebugResult.cols, (*block).UpLine.k * (drowDebugResult.cols - (*block).UpLine.x0) + (*block).UpLine.y0), cv::Scalar(0, 0, 255), 1);
	cv::line(drowDebugResult, cv::Point(0, (*block).DownLine.k * (0 - (*block).DownLine.x0) + (*block).DownLine.y0), cv::Point(drowDebugResult.cols, (*block).DownLine.k * (drowDebugResult.cols - (*block).DownLine.x0) + (*block).DownLine.y0), cv::Scalar(0, 255, 255), 1);
	cv::line(drowDebugResult, cv::Point((drowDebugResult.rows - (*block).LeftLine.y0) / (*block).LeftLine.k + (*block).LeftLine.x0, drowDebugResult.rows), cv::Point((0 - (*block).LeftLine.y0) / (*block).LeftLine.k + (*block).LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
	cv::line(drowDebugResult, cv::Point((drowDebugResult.rows - (*block).RightLine.y0) / (*block).RightLine.k + (*block).RightLine.x0, drowDebugResult.rows), cv::Point((0 - (*block).RightLine.y0) / (*block).RightLine.k + (*block).RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);

	if (faults->BrokenEdges.size() > 0)
	{
		for (size_t i = 0; i < faults->BrokenEdges.size(); i++)
		{
			cv::circle(drowDebugResult, faults->BrokenEdges[i].position, faults->BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
		}
	}
#endif
}


BlockLocalizer::~BlockLocalizer()
{
	faults = NULL;
	block = NULL;
}

void BlockLocalizer::FindUp()
{
	const int COL_SPAN = 150;
	const int RANGE_DEFAULT = 400;
	const int RANGE_MINI = 100;

	cv::Point firstPoint(2048,0);
	//先查找第一个点
	if (1 == 1)
	{
		int y = -1;
		int startY = 0;
		while (y == -1 && startY < img.rows /2)
		{
			y = getYOnLine(cv::Point(firstPoint.x, startY), RANGE_DEFAULT);
			startY += RANGE_DEFAULT / 2;
		}
		if (y == -1)
			return;
		else
			firstPoint.y = y;
	}


	uppoints.push_back(firstPoint);
	int centerY = firstPoint.y;
	int lastY = firstPoint.y;
	int range = RANGE_DEFAULT;
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	//扫描其他点，左往右
	for (int x = 2048 + COL_SPAN; (x + COL_SPAN) < img.cols; x += COL_SPAN)
	{
		int	y = getYOnLine(cv::Point(x, centerY), range);
		if (y >= 0)
		{
			uppoints.push_back(cv::Point(x, y));
			//匀速模型预测下一个点的y坐标
			centerY = y + y - lastY;
			lastY = y;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) /3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//是否要重新扫描改行
			{
				range = RANGE_DEFAULT;
				x -= COL_SPAN;
				needReFind = false;
			}
		}
	}
	centerY = firstPoint.y;
	lastY = firstPoint.y;
	range = RANGE_DEFAULT;
	needReFind = false;
	//扫描其他点，右往左
	for (int x = 2048 - COL_SPAN; (x - COL_SPAN) > 0; x -= COL_SPAN)
	{
		int	y = getYOnLine(cv::Point(x, centerY), range);
		if (y >= 0)
		{
			uppoints.push_back(cv::Point(x, y));
			//匀速模型预测下一个点的y坐标
			centerY = y + y - lastY;
			lastY = y;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//是否要重新扫描改行
			{
				range = RANGE_DEFAULT;
				x += COL_SPAN;
				needReFind = false;
			}
		}
	}
	std::sort(uppoints.begin(), uppoints.end(), ORDER_BY_X_ASC);
	std::unique(uppoints.begin(), uppoints.end());
	



#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < uppoints.size(); i++)
	{
		cv::circle(drowDebugDetectUD, uppoints[i], 4, cv::Scalar(255, 255, 0), -1);
	}
#endif

	needReFind = 0;
}

void BlockLocalizer::FindLeft()
{
	const int ROW_SPAN = 150;
	const int RANGE_DEFAULT = 400;
	const int RANGE_MINI = 100;
	cv::Point firstPoint(uppoints[0].x, uppoints[0].y + 100);

	//先查找第一个点
	if (1 == 1)
	{
		int x = -1;
		int startX = 0;
		while (x == -1 && startX < firstPoint.x + RANGE_DEFAULT)
		{
			x = getXOnRow(cv::Point(startX, firstPoint.y), RANGE_DEFAULT);
			startX += RANGE_DEFAULT / 2;
		}
		firstPoint.x = x;
		if (x == -1)
			return;
		else
			firstPoint.x = x;
	}
	leftpoints.push_back(firstPoint);

	int centerX = firstPoint.x;
	int lastX = firstPoint.x;
	int range = RANGE_DEFAULT;
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	//扫描其他点，左往右
	for (int y = firstPoint.y + ROW_SPAN; y < img.rows; y += ROW_SPAN)
	{
		int	x = getXOnRow(cv::Point(centerX, y), range);
		if (x >= 0)
		{
			leftpoints.push_back(cv::Point(x, y));
			//匀速模型预测下一个点的y坐标
			centerX = x + x - lastX;
			lastX = x;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//是否要重新扫描改行
			{
				range = RANGE_DEFAULT;
				y -= ROW_SPAN;
				needReFind = false;
			}
		}
		if ((y + ROW_SPAN) >= img.rows && y != (img.rows - 1))
			y = img.rows - ROW_SPAN - 1;
	}
	std::sort(leftpoints.begin(), leftpoints.end(), ORDER_BY_Y_ASC);
	std::unique(leftpoints.begin(), leftpoints.end());


#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < leftpoints.size(); i++)
	{
		cv::circle(drowDebugDetectLR, leftpoints[i], 1, cv::Scalar(255, 255, 0), -1);
	}
#endif
	//needReFind = 0;
}


void BlockLocalizer::FindRight()
{
	const int ROW_SPAN = 150;
	const int RANGE_DEFAULT = 400;
	const int RANGE_MINI = 100;
	cv::Point firstPoint(uppoints[uppoints.size() - 1].x, uppoints[uppoints.size() - 1].y + 100);

	//先查找第一个点
	if (1 == 1)
	{
		int x = -1;
		int startX = img.cols - 1;
		while (x == -1 && startX > firstPoint.x - RANGE_DEFAULT)
		{
			x = getXOnRow(cv::Point(startX, firstPoint.y), RANGE_DEFAULT, false);
			startX -= RANGE_DEFAULT / 2;
		}
		if (x == -1)
			return;
		else
			firstPoint.x = x;
	}
	rightpoints.push_back(firstPoint);

	int centerX = firstPoint.x;
	int lastX = firstPoint.x;
	int range = RANGE_DEFAULT;
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	//扫描其他点，左往右
	for (int y = firstPoint.y + ROW_SPAN; (y) < img.rows; y += ROW_SPAN)
	{
		int	x = getXOnRow(cv::Point(centerX, y), range, false);
		if (x >= 0)
		{
			rightpoints.push_back(cv::Point(x, y));
			//匀速模型预测下一个点的y坐标
			centerX = x + x - lastX;
			lastX = x;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else if (needReFind)//是否要重新扫描改行
		{
			range = RANGE_DEFAULT;
			y -= ROW_SPAN;
			needReFind = false;
		}
		if ((y + ROW_SPAN) >= img.rows && y != (img.rows - 1))
			y = img.rows - ROW_SPAN - 1;
	}
	std::sort(rightpoints.begin(), rightpoints.end(), ORDER_BY_Y_ASC);
	std::unique(rightpoints.begin(), rightpoints.end());


#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < rightpoints.size(); i++)
	{
		cv::circle(drowDebugDetectLR, rightpoints[i], 1, cv::Scalar(255, 255, 0), -1);
	}
#endif
	//needReFind = 0;
}
void BlockLocalizer::FindDown()
{
	const int COL_SPAN = 150;
	const int RANGE_DEFAULT = 400;
	const int RANGE_MINI = 100;

	cv::Point firstPoint(leftpoints[leftpoints.size() - 1].x + 100, leftpoints[leftpoints.size() - 1].y);
	//先查找第一个点
	if (1 == 1)
	{
		int y = -1;
		int startY = img.rows - 1;
		while (y == -1 && startY > firstPoint.y - RANGE_DEFAULT)
		{
			y = getYOnLine(cv::Point(firstPoint.x, startY), RANGE_DEFAULT, false);
			startY -= RANGE_DEFAULT / 2;
		}
		if (y == -1)
			return;
		else
			firstPoint.y = y;
	}
	downpoints.push_back(firstPoint);
	int centerY = firstPoint.y;
	int lastY = firstPoint.y;
	int range = RANGE_DEFAULT;
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	//扫描其他点，左往右
	for (int x = firstPoint.x; (x + COL_SPAN) < img.cols; x += COL_SPAN)
	{
		int	y = getYOnLine(cv::Point(x, centerY), range, false);
		if (y >= 0)
		{
			downpoints.push_back(cv::Point(x, y));
			//匀速模型预测下一个点的y坐标
			centerY = y + y - lastY;
			lastY = y;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else if (needReFind)//是否要重新扫描改行
		{
			range = RANGE_DEFAULT;
			x -= COL_SPAN;
			needReFind = false;
		}
	}
	std::sort(downpoints.begin(), downpoints.end(), ORDER_BY_X_ASC);
	std::unique(downpoints.begin(), downpoints.end());

#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < downpoints.size(); i++)
	{
		cv::circle(drowDebugDetectUD, downpoints[i], 4, cv::Scalar(255, 255, 0), -1);
	}
#endif

	//needReFind = 0;
}

int BlockLocalizer::getYOnLine(cv::Point startPoint, int range, bool scanUp2Down)
{
	const int THRESHOD = 10;

	const int continuePointCount = 50;//连续多少个点则判断为边缘
	//const int roiHeight = continuePointCount * 2;//ROI高度
	const int roiWidth = 11;//所取ROI宽度，一定为奇数，输入的x值为ROI的中心

	//创建roi范围，并防止越界。
	cv::Rect roiRect(0, 0, roiWidth, range);
	roiRect.x = startPoint.x - roiWidth / 2;
	roiRect.y = startPoint.y - range / 2;

	rectFix(roiRect);

	if (roiRect.height > continuePointCount && roiRect.width > 2)
	{
#ifdef BD_OUTPUT_DEBUG_INFO
		if (range == 400)
			cv::rectangle(drowDebugDetectUD, roiRect, cv::Scalar(0, 0, 255));
		else
			cv::rectangle(drowDebugDetectUD, roiRect, cv::Scalar(0, 255, 255));
#endif

		//选取roi
		cv::Mat roi = img(roiRect).clone();
		cv::GaussianBlur(roi, roi, cv::Size(3, 3), 0, 0);
		//水平投影
		cv::Mat roirow;
		cv::reduce(roi, roirow, 1, CV_REDUCE_AVG);
		int count = 0;
		if (scanUp2Down)
		{
			for (int j = 0; j < roirow.rows; j++)
			{
				for (int i = j; i < roirow.rows; i++)
				{
					if (roirow.ptr<uchar>(i)[0] >= THRESHOD)
						count++;
					else if (i != j)
					{
						j = i - 1;
						break;
					}
					else
						break;

					if (count > continuePointCount)
					{
						return (j + roiRect.y);
					}
				}
			}
		}
		else
		{
			for (int j = roirow.rows - 1; j >= 0; j--)
			{
				for (int i = j; i >= 0; i--)
				{
					if (roirow.ptr<uchar>(i)[0] >= THRESHOD)
						count++;
					else if (i != j)
					{
						j = i + 1;
						break;
					}
					else
						break;

					if (count > continuePointCount)
					{
						return (j + roiRect.y);
					}
				}
			}
		}
	}
	return -1;
}
int BlockLocalizer::getXOnRow(cv::Point startPoint, int range, bool scanLeft2right)
{
	const int THRESHOD = 10;
	const int continuePointCount = 20;//连续多少个点则判断为边缘
	const int roiHeight = 11;//所取ROI宽度，一定为奇数，输入的x值为ROI的中心

	//创建roi范围，并防止越界。
	cv::Rect roiRect(0, 0, range, roiHeight);
	roiRect.x = startPoint.x - range / 2;
	roiRect.y = startPoint.y - roiHeight / 2;

	rectFix(roiRect);


	if (roiRect.width > continuePointCount && roiRect.height > 2)
	{
#ifdef BD_OUTPUT_DEBUG_INFO
		if (range == 400)
			cv::rectangle(drowDebugDetectLR, roiRect, cv::Scalar(0, 0, 255));
		else
			cv::rectangle(drowDebugDetectLR, roiRect, cv::Scalar(0, 255, 255));
#endif

		//选取roi
		cv::Mat roi = img(roiRect).clone();
		cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0, 0);
		//竖直投影
		cv::Mat roirow;
		cv::reduce(roi, roirow, 0, CV_REDUCE_AVG);
		int count = 0;
		if (scanLeft2right)
		{
			for (int j = 0; j < roirow.cols; j++)
			{
				for (int i = j; i < roirow.cols; i++)
				{
					if (roirow.ptr<uchar>(0)[i] >= THRESHOD)
						count++;
					else if (i != j)
					{
						j = i - 1;
						break;
					}
					else
						break;

					if (count > continuePointCount)
					{
						return (j + roiRect.x);
					}
				}
			}
		}
		else
		{
			for (int j = roirow.cols - 1; j >= 0; j--)
			{
				for (int i = j; i >= 0; i--)
				{
					if (roirow.ptr<uchar>(0)[i] >= THRESHOD)
						count++;
					else if (i != j)
					{
						j = i + 1;
						break;
					}
					else
						break;

					if (count > continuePointCount)
					{
						return (j + roiRect.x);
					}
				}
			}
		}
	}
	return -1;
}


void BlockLocalizer::Judgement()
{
	judgementForOneLine(uppoints, true, (*block).UpLine);
	judgementForOneLine(downpoints, true, (*block).DownLine);
	judgementForOneLine(leftpoints, false, (*block).LeftLine);
	judgementForOneLine(rightpoints, false, (*block).RightLine);
}
void BlockLocalizer::judgementForOneLine(vector<cv::Point>& points, bool updown, Block::Line& line)
{
	if (fixLineOnBorder(points, line))
	{
		//判断是否崩边
		judgemanBrokenLine(points, updown);

		//拟合直线
		cv::Vec4f line4f;
		cv::fitLine(cv::Mat(points), line4f, CV_DIST_L2, 0, 0.01, 0.01);

		line.x0 = line4f[2];
		line.y0 = line4f[3];
		line.dx = line4f[0];
		line.dy = line4f[1];
		line.k = line.dy / line.dx;
	}
}
void BlockLocalizer::judgemanBrokenLine(vector<cv::Point>& points, bool updown)
{
	const int PIX_OFFSET = 20;//误差超过这个值，认为崩边。
	int count = points.size() - 2;

	//确定每个点间的间隔
	int spanX = 0;
	int spanY = 0;
	int iii = 0;
	//之所以使用循环，是考虑到了第一个点有重复的情况，如0 1号点相同，那么spanY=0
	while (spanY == 0 && iii < count)
	{
		spanY = abs(points[iii].y - points[iii + 1].y);
		iii++;
	}
	iii = 0;
	while (spanX == 0 && iii < count)
	{
		spanX = abs(points[iii].x - points[iii + 1].x);
		iii++;
	}

	//循环检查相邻点之间的像素增量
	//比较相邻增量间的差异是否大于阈值，若大于则认为该点为转折点，diffs将push进一个新的像素增量
	vector<float> diffs;//记录像素差
	vector<int> diffsCount;//对应像素差计数
	vector<int> errorPointIndex;//记录转折点索引
	if (updown)
	{
		vector<int> dy;
		for (size_t i = 0; i < count; i++)
		{
			int xss = abs(points[i].x - points[i + 1].x);
			if (abs(points[i].x - points[i + 1].x) == spanX)
				dy.push_back(points[i].y - points[i + 1].y);
		}
		diffs.push_back(dy[0]);
		diffsCount.push_back(1);

		for (size_t i = 0; i < dy.size(); i++)
		{
			float lastdiff = diffs[diffs.size() - 1];
			if (abs(lastdiff - dy[i]) < PIX_OFFSET)
			{
				diffs[diffs.size() - 1] = lastdiff * 0.7 + dy[i] * 0.3;
				diffsCount[diffsCount.size() - 1] += 1;
			}
			else
			{
				diffs.push_back(dy[i]);
				diffsCount.push_back(1);
				errorPointIndex.push_back(i);

			}
		}
	}
	else
	{
		vector<int> dx;
		for (size_t i = 0; i < count; i++)
		{
			int yxxs = abs(points[i].y - points[i + 1].y);
			if (abs(points[i].y - points[i + 1].y) == spanY)
				dx.push_back(points[i].x - points[i + 1].x);
		}
		diffs.push_back(dx[0]);
		diffsCount.push_back(1);

		for (size_t i = 0; i < dx.size(); i++)
		{
			float lastdiff = diffs[diffs.size() - 1];
			if (abs(lastdiff - dx[i]) < PIX_OFFSET)
			{
				diffs[diffs.size() - 1] = lastdiff * 0.7 + dx[i] * 0.3;
				diffsCount[diffsCount.size() - 1] += 1;
			}
			else
			{
				diffs.push_back(dx[i]);
				diffsCount.push_back(1);
				errorPointIndex.push_back(i);
			}
		}
	}
	//斜率全部一致
	if (errorPointIndex.size() == 0)
	{
		return;
	}
	//斜率只有一个点的波动
	if (errorPointIndex.size() <= 2)
	{
		int smallcount = 0;
		for (size_t i = 0; i < diffs.size(); i++)
			if (diffs[i] == 1)
				smallcount++;
		if ((smallcount + 1) == diffs.size())
			return;
	}
	


	//添加缺陷信息
	Faults::BrokenEdge be;

	//int dx = 0;
	//int dy = 0;

	//计算各种情况下的length 与  deep值

	int l1 = updown ? abs(points[0].x - points[errorPointIndex[errorPointIndex.size() - 1]].x) : abs(points[0].y - points[errorPointIndex[errorPointIndex.size() - 1]].y);
	int d1 = !updown ? abs(points[0].x - points[errorPointIndex[errorPointIndex.size() - 1]].x) : abs(points[0].y - points[errorPointIndex[errorPointIndex.size() - 1]].y);
	int l2 = updown ? abs(points[points.size() - 1].x - points[errorPointIndex[0]].x) : abs(points[points.size() - 1].y - points[errorPointIndex[0]].y);
	int d2 = !updown ? abs(points[points.size() - 1].x - points[errorPointIndex[0]].x) : abs(points[points.size() - 1].y - points[errorPointIndex[0]].y);

	int indexOffset = l1 < l2 ? abs(errorPointIndex[errorPointIndex.size() - 1]) : abs((int)points.size() - 1 - errorPointIndex[0]);
	indexOffset /= 2;

	//根据水平还是竖直方向分配length 与  deep
	be.length = l1 < l2 ? l1 : l2;
	be.deep = l1 < l2 ? d1 : d2;

	be.position.x = l1 < l2 ? abs(points[0].x + points[errorPointIndex[errorPointIndex.size() - 1]].x) / 2 : abs(points[points.size() - 1].x + points[errorPointIndex[0]].x) / 2;
	be.position.y = l1 < l2 ? abs(points[0].y + points[errorPointIndex[errorPointIndex.size() - 1]].y) / 2 : abs(points[points.size() - 1].y + points[errorPointIndex[0]].y) / 2;
	//be.position.x += updown ? be.length / 2 : be.deep;
	//be.position.y += updown ? be.deep / 2 : be.length;
	faults->BrokenEdges.push_back(be);

#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < errorPointIndex.size(); i++)
	{
		cv::circle(drowDebugResult, points[errorPointIndex[i]], 10, cv::Scalar(255, 255, 0), -1);
	}
	cv::circle(drowDebugResult, be.position, 10, cv::Scalar(123, 255, 255), -1);
#endif

	BrokenEdgeFlag = true;
}
bool BlockLocalizer::fixLineOnBorder(vector<cv::Point>& points, Block::Line& line)
{
	int borderX = img.cols - 1;
	int borderY = img.rows - 1;
	if ((points[0].x == 0 && points[1].x == 0 && points[points.size() - 1].x == 0 && points[points.size() - 2].x == 0) ||
		(points[0].x == borderX && points[1].x == borderX && points[points.size() - 1].x == borderX && points[points.size() - 2].x == borderX))
	{
		line.k = 999999;
		line.x0 = points[0].x;
		line.y0 = points[0].y;
		return false;
	}
	if ((points[0].y == 0 && points[1].y == 0 && points[points.size() - 1].y == 0 && points[points.size() - 2].y == 0) ||
		(points[0].y == borderY && points[1].y == borderY && points[points.size() - 1].y == borderY && points[points.size() - 2].y == borderY)
		)
	{
		line.k = 0;
		line.x0 = points[0].x;
		line.y0 = points[0].y;
		return false;
	}
	vector<cv::Point> ::iterator pIter = points.begin();
	cv::Point tmp = 0;
	for (; pIter != points.end();) // 遍历
	{
		if (pIter->x == 0 || pIter->x == borderX || pIter->y == 0 || pIter->y == borderY)
		{
			tmp.x = pIter->x;
			tmp.y = pIter->y;
			pIter = points.erase(pIter);
			continue;
		}
		pIter++;
	}

	//若删除点后，剩余点数过少，则将线认为边缘线
	if (points.size() < 5)
	{
		line.x0 = tmp.x;
		line.y0 = tmp.y;
		if (tmp.x == 0 || tmp.x == borderX)
		{
			line.k = 0;
		}
		if (tmp.y == 0 || tmp.y == borderY)
		{
			line.k = 999999;
		}
		return false;
	}
	return true;
}
