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
		while (y == -1 && (startY + 200) < img.rows /2)
		{
			y = getYOnLine(cv::Point(firstPoint.x, startY), RANGE_DEFAULT);
			startY += 200;
		}
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
			centerY = lastY + lastY - y;
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
			centerY = lastY + lastY - y;
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
	cv::Point firstPoint(uppoints[0].x, uppoints[0].y + 50);

	//先查找第一个点
	if (1 == 1)
	{
		int x = -1;
		int startX = 0;
		while (x == -1 && (startX + RANGE_DEFAULT/2) < uppoints[0].x + RANGE_DEFAULT)
		{
			x = getXOnRow(cv::Point(startX, firstPoint.y), RANGE_DEFAULT);
			startX += 200;
		}
		firstPoint.x = x;
	}
	leftpoints.push_back(firstPoint);

	int centerX = firstPoint.x;
	int lastX = firstPoint.x;
	int range = RANGE_DEFAULT;
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	//扫描其他点，左往右
	for (int y = firstPoint.y + ROW_SPAN; (y + ROW_SPAN) < img.rows; y += ROW_SPAN)
	{
		int	x = getXOnRow(cv::Point(centerX, y), range);
		if (x >= 0)
		{
			leftpoints.push_back(cv::Point(x, y));
			//匀速模型预测下一个点的y坐标
			centerX = lastX + lastX - x;
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
	}

#ifdef BD_OUTPUT_DEBUG_INFO
	//debug绘图
	for (size_t i = 0; i < leftpoints.size(); i++)
	{
		cv::circle(drowDebugDetectLR, leftpoints[i], 1, cv::Scalar(255, 255, 0), -1);
	}
#endif
	needReFind = 0;
}



int BlockLocalizer::getYOnLine(cv::Point startPoint, int range)
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
		cv::rectangle(drowDebugDetectUD, roiRect, cv::Scalar(0, 0, 255));
#endif

		//选取roi
		cv::Mat roi = img(roiRect).clone();
		cv::GaussianBlur(roi, roi, cv::Size(3, 3), 0, 0);
		//水平投影
		cv::Mat roirow;
		cv::reduce(roi, roirow, 1, CV_REDUCE_AVG);
		int count = 0;
		for (int j = 0; j < roirow.rows; j++)
		{
			for (int i = j; i < roirow.rows; i++)
			{
				if (roirow.ptr<uchar>(i)[0] > THRESHOD)
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
	return -1;
}

int BlockLocalizer::getXOnRow(cv::Point startPoint, int range)
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
		cv::rectangle(drowDebugDetectLR, roiRect, cv::Scalar(0, 0, 255));
#endif

		//选取roi
		cv::Mat roi = img(roiRect).clone();
		cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0, 0);
		//竖直投影
		cv::Mat roirow;
		cv::reduce(roi, roirow, 0, CV_REDUCE_AVG);
		int count = 0;
		for (int j = 0; j < roirow.cols; j++)
		{
			for (int i = j; i < roirow.cols; i++)
			{
				if (roirow.ptr<uchar>(0)[i] > THRESHOD)
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
	return -1;
}