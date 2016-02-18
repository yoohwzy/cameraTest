#include "BlockLocalizer.h"


BlockLocalizer::BlockLocalizer(cv::Mat& Img)
{
	img = Img;

#ifdef BD_OUTPUT_DEBUG_INFO
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
	if (uppoints.size() == 0)
	{
		return;
	}
	thread t1 = thread(std::mem_fn(&BlockLocalizer::FindLeft), this);
	thread t2 = thread(std::mem_fn(&BlockLocalizer::FindRight), this);
	t1.join();
	t2.join();
	//FindLeft();
	//FindRight();
	if (leftpoints.size() == 0 || rightpoints.size() == 0)
	{
		return;
	}

	FindDown();

	Judgement();
}


BlockLocalizer::~BlockLocalizer()
{
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
			startX += RANGE_DEFAULT / 2;
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

#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < downpoints.size(); i++)
	{
		cv::circle(drowDebugDetectUD, downpoints[i], 4, cv::Scalar(255, 255, 0), -1);
	}
#endif

	needReFind = 0;
}

int BlockLocalizer::getYOnLine(cv::Point startPoint, int range, bool scanUp2Down)
{
	const int THRESHOD = 12;

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
	if (uppoints.size() < 5)
	{
		return;
	}
	judgemanBrokenLine(uppoints);
	judgemanBrokenLine(downpoints);
	judgemanBrokenLine(leftpoints);
	judgemanBrokenLine(rightpoints);
}

bool BlockLocalizer::judgemanBrokenLine(vector<cv::Point>& points)
{

	//cv::Point start2 = uppoints[1];
	//cv::Point center = uppoints[uppoints.size() / 2];
	//cv::Point start_2 = uppoints[uppoints.size() - 2];
	//double k1 = (center.y - start2.y) / (double)(center.x - start2.x);
	//double k2 = (start_2.y - center.y) / (double)(start_2.x - center.x);
	//double diff = k1 - k2;

	/*	此处的center点不一定就是两个点的中间点，因此不能简单的计算y坐标是否满足 center*2 = start2 + start_2
	可以计算start2 与 center 的斜率k1
	center 与 start_2 的斜率k2
	比较k1，k2

	或者计算多组斜率，求出标准差
	*/



	int count = points.size() - 3;
	cv::Mat k(1, count, CV_32F, cv::Scalar(0));
	for (size_t i = 0; i < count; i++)
	{
		//float k1 = (points[i].y - points[i + 1].y) / (float)(points[i].x - points[i + 2].x);
		float k1 = (float)(points[i].x - points[i + 2].x);
		//float k1 = (points[i].y - points[i + 1].y);
		k.ptr<float>(0)[i] = k1;
	}

	cv::Mat tmp_m, tmp_sd;
	cv::meanStdDev(k, tmp_m, tmp_sd);

	double stdDev = tmp_sd.ptr<double>(0)[0];
	if (stdDev < 0.005)//TODO:将该敏感度参数设置入数据库
		return true;
	else
		return false;
}