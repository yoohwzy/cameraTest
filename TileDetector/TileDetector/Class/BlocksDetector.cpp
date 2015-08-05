#include "BlocksDetector.h"



BlocksDetector::BlocksDetector(BufferStorage *ss, MicroDisplayInit *mdii)
{
	s = ss;
	mdi = mdii;

	//初始化左边缘检测参数
	leftY = 0;//在第几行检测
	leftX = ORANGE_MARGIN_ROW;//检测的中点
	leftRnage = ORANGE_RANGE_ROW;//在中点周围多少像素内检测
	leftNoneCount = 0;//连续没有找到边缘的数量。
	leftNeedReFind = false;//对该行是否需要扩大range重新搜索

	rightY = 0;//在第几行检测
	rightX = (*mdi).width - ORANGE_MARGIN_ROW;;//检测的中点
	rightRnage = ORANGE_RANGE_ROW;//在中点周围多少像素内检测
	rightNoneCount = 0;//连续没有找到边缘的数量。
	rightNeedReFind = false;//对该行是否需要扩大range重新搜索


#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		drowDebugDetectLR = (*s).NowBufferImg.clone();
		drowDebugDetectUD = (*s).NowBufferImg.clone();
		drowDebugResult = (*s).NowBufferImg.clone();
	}
#endif
}
BlocksDetector::~BlocksDetector()
{
}
void BlocksDetector::Start()
{
	if (1 == 1)
	{
		for (; (leftY + 3) < (*mdi).MaxPics; leftY += ROW_SPAN)
		{
			if (leftNoneCount > 3)//连续N个点没有找到边界后，跳出
				break;
			if (DetectOneLineLeft(leftY) < 0)
				if (leftNeedReFind)//是否要重新扫描该行
				{
					leftRnage = ORANGE_RANGE_ROW;
					leftY -= ROW_SPAN;
					leftNeedReFind = false;
				}
		}
		//若没有找到边缘，停止程序。
		if (LeftBorder.size() == 0)
			return;
		//逼近顶点
		int leftFirstLine = LeftBorder[0].y, leftFirstX = LeftBorder[0].x;
		int leftLastLine = LeftBorder[LeftBorder.size() - 1].y, leftLastX = LeftBorder[LeftBorder.size() - 1].x;
		cv::Point upstart = cv::Point(-1, (leftFirstLine > ROW_SPAN) ? (leftFirstLine - ROW_SPAN) : 0);
		cv::Point upend = cv::Point(leftFirstX, leftFirstLine);
		GetEdgeVerticalApproach(upstart, upend, RANGE_MINI, BorderType::Left, upstart);
		cv::Point downstart = cv::Point(leftLastX, leftLastLine);
		cv::Point downend = cv::Point(-1, (leftLastLine + ROW_SPAN) > (*mdi).MaxPics ? ((*mdi).MaxPics - 1) : (leftLastLine + ROW_SPAN));
		GetEdgeVerticalApproach(downstart, downend, RANGE_MINI, BorderType::Left, downend);


#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			//debug绘图
			for (size_t i = 0; i < LeftBorder.size(); i++)
				cv::circle(drowDebugResult, LeftBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpLeftList.size(); i++)
				cv::circle(drowDebugResult, tmpLeftList[i], 6, cv::Scalar(128, 128, 0), 1);
			for (size_t i = 0; i < LeftUp.size(); i++)
				cv::circle(drowDebugResult, LeftUp[i], 1, cv::Scalar(0, 255, 0), 1);
			for (size_t i = 0; i < LeftDown.size(); i++)
				cv::circle(drowDebugResult, LeftDown[i], 1, cv::Scalar(0, 255, 0), 1);
		}
#endif
		LeftBorder.insert(LeftBorder.end(), LeftUp.begin(), LeftUp.end());
		LeftBorder.insert(LeftBorder.end(), LeftDown.begin(), LeftDown.end());

	}
	if (2 == 2)
	{
		for (; (rightY + 3) < (*mdi).MaxPics; rightY += ROW_SPAN)
		{
			if (rightNoneCount > 3)//连续N个点没有找到边界后，跳出
				break;
			if (DetectOneLineRight(rightY) < 0)
				if (rightNeedReFind)//是否要重新扫描改行
				{
					rightRnage = ORANGE_RANGE_ROW;
					rightY -= ROW_SPAN;
					rightNeedReFind = false;
				}
		}
		//若没有找到边缘，停止程序。
		if (RightBorder.size() == 0)
			return;
		//逼近顶点
		int rightFirstLine = RightBorder[0].y, rightFirstX = RightBorder[0].x;
		int rightLastLine = RightBorder[RightBorder.size() - 1].y, rightLastX = RightBorder[RightBorder.size() - 1].x;
		cv::Point upstart = cv::Point(-1, (rightFirstLine > ROW_SPAN) ? (rightFirstLine - ROW_SPAN) : 0);
		cv::Point upend = cv::Point(rightFirstX, rightFirstLine);
		GetEdgeVerticalApproach(upstart, upend, RANGE_MINI, BorderType::Right, upstart);
		cv::Point downstart = cv::Point(rightLastX, rightLastLine);
		cv::Point downend = cv::Point(-1, (rightLastLine + ROW_SPAN) > (*mdi).MaxPics ? ((*mdi).MaxPics - 1) : (rightLastLine + ROW_SPAN));
		GetEdgeVerticalApproach(downstart, downend, RANGE_MINI, BorderType::Right, downstart);

#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			for (size_t i = 0; i < RightBorder.size(); i++)
				cv::circle(drowDebugResult, RightBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpRightList.size(); i++)
				cv::circle(drowDebugResult, tmpRightList[i], 6, cv::Scalar(128, 128, 0), 1);
			for (size_t i = 0; i < RightUp.size(); i++)
				cv::circle(drowDebugResult, RightUp[i], 1, cv::Scalar(0, 255, 0), 1);
			for (size_t i = 0; i < RightDown.size(); i++)
				cv::circle(drowDebugResult, RightDown[i], 1, cv::Scalar(0, 255, 0), 1);
		}
#endif
		RightBorder.insert(RightBorder.end(), RightUp.begin(), RightUp.end());
		RightBorder.insert(RightBorder.end(), RightDown.begin(), RightDown.end());
	}

	std::sort(LeftBorder.begin(), LeftBorder.end(), ORDER_BY_Y_ASC);
	std::sort(RightBorder.begin(), RightBorder.end(), ORDER_BY_Y_ASC);

	std::sort(allLeftList.begin(), allLeftList.end(), ORDER_BY_Y_ASC);
	std::sort(allRightList.begin(), allRightList.end(), ORDER_BY_Y_ASC);
	//return;


	//
	//	A = LeftBorder[0];
	//	B = RightBorder[0];
	//	C = RightBorder[RightBorder.size() - 1];
	//	D = LeftBorder[LeftBorder.size() - 1];
	//
	//#ifdef OUTPUT_DEBUG_INFO
	//	if (OUTPUT_DEBUG_INFO)
	//	{
	//		cv::putText(drowDebugResult, "A", A, CV_FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0));
	//		cv::putText(drowDebugResult, "B", B, CV_FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0));
	//		cv::putText(drowDebugResult, "C", C, CV_FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0));
	//		cv::putText(drowDebugResult, "D", D, CV_FONT_HERSHEY_COMPLEX, 2, cv::Scalar(255, 0, 0));
	//	}
	//#endif
}
void BlocksDetector::StartUP_DOWN(BorderType bt)
{
	int centerY = 0;

	if (bt == BlocksDetector::Up && allLeftList[0].y == 0 && allRightList[0].y == 0)
		return;
	else if (bt == BlocksDetector::Down && !(allLeftList[allLeftList.size() - 1].y > 0 && allLeftList[allLeftList.size() - 1].y < ((*mdi).MaxPics - SUM_COUNT) && allRightList[allRightList.size() - 1].y>0 && allRightList[allRightList.size() - 1].y < ((*mdi).MaxPics - SUM_COUNT)))
		return;

	vector<cv::Point> *targetBorder;
	vector<cv::Point> *tmptargetList;
	vector<cv::Point> *alltargetList;
	if (bt == BlocksDetector::Up)
	{
		targetBorder = &UpBorder;
		tmptargetList = &tmpUpList;
		alltargetList = &allUpList;
		centerY = allLeftList[0].y;
	}
	else
	{
		targetBorder = &DownBorder;
		tmptargetList = &tmpDownList;
		alltargetList = &allDownList;
		centerY = allLeftList[allLeftList.size() - 1].y;
	}
	int range = ORANGE_RANGE_COL;
	//截取ROI
	//ROI的起点y值，同时也是计算最终y坐标的OFFSET
	//int offsetY = LeftBorder[0].y - ORANGE_RANGE_COL;
	//if (offsetY < 0) offsetY = 0;
	//ROI中边界查找的起点行高
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	for (size_t x = 0; (x + 3) < (*mdi).width; x += COL_SPAN)
	{
#ifdef OUTPUT_DEBUG_INFO
		cv::Mat _drowDebugDetectUD = drowDebugDetectUD;
		if (OUTPUT_DEBUG_INFO)
		{
			cv::circle(drowDebugDetectUD, cv::Point(x, centerY), 5, cv::Scalar(0, 255, 255), 3);
		}
#endif
		//相对于整幅图像的Y值
		int y1 = GetEdgeX3(cv::Point(x, centerY), range, bt);
		if (y1 >= 0)
		{
			(*alltargetList).push_back(cv::Point(x, y1));
			if (range == RANGE_MINI)
				(*targetBorder).push_back(cv::Point(x, y1));
			else
				(*tmptargetList).push_back(cv::Point(x, y1));
			//匀速模型预测下一个点的y坐标
			if ((*alltargetList).size() > 2)
			{
				int last_1 = (*alltargetList)[(*alltargetList).size() - 1].y;
				int last_2 = (*alltargetList)[(*alltargetList).size() - 2].y;
				centerY = last_1 + last_1 - last_2;
			}
			else
				centerY = y1;

			needReFind = true;

			if (range > RANGE_MINI) range -= RANGE_REDUCE_BY;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//是否要重新扫描改行
			{
				range = ORANGE_RANGE_ROW;
				x -= COL_SPAN;
				needReFind = false;
			}
		}
	}
	if ((*targetBorder).size() > 2)
	{
		int upFirstCol = (*targetBorder)[0].x, upFirstRow = (*targetBorder)[0].y + ((*targetBorder)[0].y - (*targetBorder)[1].y) / 2;
		int upLastCol = (*targetBorder)[(*targetBorder).size() - 1].x, upLastRow = (*targetBorder)[(*targetBorder).size() - 1].y;
		cv::Point leftstart = cv::Point((upFirstCol > COL_SPAN) ? (upFirstCol - COL_SPAN) : 0, -1);
		cv::Point leftend = cv::Point(upFirstCol, upFirstRow);
		GetEdgeHorizontalApproach(leftstart, leftend, RANGE_MINI, bt, leftstart);
		cv::Point rightstart = cv::Point(upLastCol, upLastRow);
		cv::Point rightend = cv::Point((upLastCol + COL_SPAN) > (*mdi).width ? ((*mdi).width - 1) : (upLastCol + COL_SPAN), -1);
		GetEdgeHorizontalApproach(rightstart, rightend, RANGE_MINI, bt, rightend);

		//排序整理
		std::sort((*targetBorder).begin(), (*targetBorder).end(), ORDER_BY_X_ASC);
	}


#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		//debug绘图
		for (size_t i = 0; i < (*targetBorder).size(); i++)
		{
			cv::circle(drowDebugResult, (*targetBorder)[i], 5, cv::Scalar(0, 255, 255), 1);
			cv::circle(drowDebugResult, (*targetBorder)[i], 1, cv::Scalar(255, 255, 0), -1);
		}
		for (size_t i = 0; i < (*tmptargetList).size(); i++)
			cv::circle(drowDebugResult, (*tmptargetList)[i], 8, cv::Scalar(0, 128, 120), 2);
		//for (size_t i = 0; i < LeftUp.size(); i++)
		//	cv::circle(drowDebugResult, LeftUp[i], 1, cv::Scalar(0, 255, 0), 1);
		//for (size_t i = 0; i < LeftDown.size(); i++)
		//	cv::circle(drowDebugResult, LeftDown[i], 1, cv::Scalar(0, 255, 0), 1);
	}
#endif
}

int BlocksDetector::DetectOneLineLeft(int y)
{
	if (y == -1)
		y = leftY;
	//判断是否越界
	if (y < 0 || (y + 3 + 1) >(*mdi).MaxPics)
		return -1;
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::circle(drowDebugDetectLR, cv::Point(leftX, y), 5, cv::Scalar(0, 255, 255), 3);
	}
#endif
	int x1 = GetEdgeX3(cv::Point(leftX, y), leftRnage, BlocksDetector::Left);
	if (x1 >= 0)
	{
		//如果在预测范围之内找到了边界，就认为找到了真正的点，否则认为存疑。
		if (leftRnage == RANGE_MINI)
		{
			if (LeftBorder.size() == 0)
			{
				LeftBorder.insert(LeftBorder.end(), tmpLeftList.begin(), tmpLeftList.end());
				tmpLeftList.clear();
			}
			LeftBorder.push_back(cv::Point(x1, y));
			allLeftList.push_back(cv::Point(x1, y));
		}
		else
		{
			tmpLeftList.push_back(cv::Point(x1, y));
			allLeftList.push_back(cv::Point(x1, y));
		}
		leftNoneCount = 0;
		leftNeedReFind = true;

		//匀速模型预测下一个点的x坐标
		if (allLeftList.size() > 2)
		{
			int last_1 = allLeftList[allLeftList.size() - 1].x;
			int last_2 = allLeftList[allLeftList.size() - 2].x;
			leftX = last_1 + last_1 - last_2;
		}
		else
			leftX = x1;
		if (leftRnage > RANGE_MINI) leftRnage -= RANGE_REDUCE_BY;
		if (leftRnage < RANGE_MINI) leftRnage = RANGE_MINI;
	}
	else if (LeftBorder.size() > 0)
		leftNoneCount++;
	return x1;
}
int BlocksDetector::DetectOneLineRight(int y)
{
	//判断是否越界
	if (y < 0 || (y + 3 + 1) >(*mdi).MaxPics)
		return -1;
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::circle(drowDebugDetectLR, cv::Point(rightX, y), 5, cv::Scalar(0, 255, 255), 3);
	}
#endif
	int x1 = GetEdgeX3(cv::Point(rightX, y), rightRnage, BlocksDetector::Right);
	if (x1 >= 0)
	{
		//如果在预测范围之内找到了边界，就认为找到了真正的点，否则认为存疑。
		if (rightRnage == RANGE_MINI)
		{
			if (RightBorder.size() == 0)
			{
				RightBorder.insert(RightBorder.end(), tmpRightList.begin(), tmpRightList.end());
				tmpRightList.clear();
			}
			RightBorder.push_back(cv::Point(x1, y));
			allRightList.push_back(cv::Point(x1, y));
		}
		else
		{
			tmpRightList.push_back(cv::Point(x1, y));
			allRightList.push_back(cv::Point(x1, y));
		}

		rightNoneCount = 0;
		rightNeedReFind = true;

		//匀速模型预测下一个点的x坐标
		if (allRightList.size() > 2)
		{
			int last_1 = allRightList[allRightList.size() - 1].x;
			int last_2 = allRightList[allRightList.size() - 2].x;
			rightX = last_1 + last_1 - last_2;
		}
		else
			rightX = x1;
		if (rightRnage > RANGE_MINI) rightRnage -= RANGE_REDUCE_BY;
		if (rightRnage < RANGE_MINI) rightRnage = RANGE_MINI;
	}
	else if (RightBorder.size() > 0)
		rightNoneCount++;
	return x1;
}

int BlocksDetector::GetEdgeX3(cv::Point start, int range, BlocksDetector::BorderType bt)
{
	int a = 0, b = 100, c = 1000;
	switch (bt)
	{
	case BlocksDetector::Left:
		a = GetEdgeVertical(start, range, true);
		b = GetEdgeVertical(cv::Point(start.x, start.y + 1), range, true);
		c = GetEdgeVertical(cv::Point(start.x, start.y + 2), range, true);
		break;
	case BlocksDetector::Up:
		a = GetEdgeHorizontal(start, range, true);
		b = GetEdgeHorizontal(cv::Point(start.x + 1, start.y), range, true);
		c = GetEdgeHorizontal(cv::Point(start.x + 2, start.y), range, true);
		break;
	case BlocksDetector::Right:
		a = GetEdgeVertical(start, range, false);
		b = GetEdgeVertical(cv::Point(start.x, start.y + 1), range, false);
		c = GetEdgeVertical(cv::Point(start.x, start.y + 2), range, false);
		break;
	case BlocksDetector::Down:
		a = GetEdgeHorizontal(start, range, false);
		b = GetEdgeHorizontal(cv::Point(start.x + 1, start.y), range, false);
		c = GetEdgeHorizontal(cv::Point(start.x + 2, start.y), range, false);
		break;
	default:
		break;
	}
	if ((a + b + c) > 0 && abs((a + c - b - b)) < 5)
		return ((a + b + c) / 3);
	return -1;
}

int BlocksDetector::GetEdgeVerticalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target)
{
	int x1 = -1;
	int startline = start.y;
	int endline = end.y;
	int middleLine = (endline - startline) / 2 + startline;
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::circle(drowDebugDetectLR, cv::Point(end.x, middleLine), 3, cv::Scalar(255, 0, 0), 2);
	}
#endif
	//二分法，求startline与endline的中间是否有边界
	//递归直至startline = endline
	//找上边缘顶点
	if (start.x == -1)
	{
		//若找到的点与最初目标点重合，扩大区域继续搜索
		if (middleLine == Target.y && middleLine != 0 && middleLine < ((*mdi).MaxPics - 2))
		{
			Target = cv::Point(-1, (Target.y > ROW_SPAN) ? (Target.y - ROW_SPAN) : 0);
			return GetEdgeVerticalApproach(Target, end, range, bt, Target);
		}
		if (middleLine == startline || middleLine == endline)
			return end.x;
		x1 = GetEdgeX3(cv::Point(end.x, middleLine), range, bt);
		if (x1 > 0)
		{
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				cv::circle(drowDebugResult, cv::Point(x1, middleLine), 3, cv::Scalar(0, 255, 0), 1);
			}
#endif
			if (bt == BlocksDetector::Left)
				LeftUp.push_back(cv::Point(x1, middleLine));
			else
				RightUp.push_back(cv::Point(x1, middleLine));
			x1 = GetEdgeVerticalApproach(start, cv::Point(x1, middleLine), range, bt, Target);
		}
		else
			x1 = GetEdgeVerticalApproach(cv::Point(x1, middleLine), end, range, bt, Target);
	}
	//找下边缘顶点
	else
	{
		//若找到的点与最初目标点重合，扩大区域继续搜索
		if (middleLine == Target.y)
		{
			Target = cv::Point(-1, (Target.y + ROW_SPAN) > (*mdi).MaxPics ? ((*mdi).MaxPics - 1) : (Target.y + ROW_SPAN));
			return GetEdgeVerticalApproach(start, Target, range, bt, Target);
		}
		if (middleLine == startline || middleLine == endline)
			return start.x;
		x1 = GetEdgeX3(cv::Point(start.x, middleLine), range, bt);
		if (x1 > 0)
		{
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				cv::circle(drowDebugResult, cv::Point(x1, middleLine), 3, cv::Scalar(0, 255, 0), 1);
			}
#endif
			if (bt == BlocksDetector::Left)
				LeftDown.push_back(cv::Point(x1, middleLine));
			else
				RightDown.push_back(cv::Point(x1, middleLine));
			x1 = GetEdgeVerticalApproach(cv::Point(x1, middleLine), end, range, bt, Target);
		}
		else
			x1 = GetEdgeVerticalApproach(start, cv::Point(-1, middleLine), range, bt, Target);
	}

	return x1;
}
int BlocksDetector::GetEdgeVertical(cv::Point start, int range, bool isLeft)
{
	const int sumcount = SUM_COUNT;
	//检测算法
	cv::Mat oneLineGray;
	int xstart = start.x - range;
	if (xstart < 0)
		xstart = 0;
	int width = range + range;
	if ((width + xstart) >(*mdi).width - 1)
		width = (*mdi).width - 1 - xstart;
	cv::cvtColor((*s).NowBufferImg(cv::Rect(xstart, start.y, width, 1)), oneLineGray, CV_BGR2GRAY);

	int elementCount = oneLineGray.cols;//每行元素数
	uchar* linehead = oneLineGray.ptr<uchar>(0);//每行的起始地址
	int ret = -1;
	int diff = SUM_THRESHOD;
	uchar* lineheadO;


#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		lineheadO = drowDebugDetectLR(cv::Rect(xstart, start.y, width, 1)).ptr<uchar>(0);//每行的起始地址
	}
#endif

	for (size_t i = sumcount; i < oneLineGray.cols - 1 - sumcount; i++)
	{
		int leftsum = 0;
		for (size_t ii = i; ii > i - sumcount; ii--)
		{
			leftsum += linehead[ii];
		}
		int rightsum = 0;
		for (size_t ii = i + 1; ii <= i + sumcount; ii++)
		{
			rightsum += linehead[ii];
		}
		int c = 0;
		if (isLeft)
			c = rightsum - leftsum;
		else
			c = leftsum - rightsum;
		if (c > diff)
		{
			diff = c;
			ret = i;
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				lineheadO[i * 3] = 255;//填充蓝色
			}
#endif
		}
#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			lineheadO[i * 3 + 1] = 255;//填充绿色
		}
#endif
	}
	if (ret >= 0)
	{
#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			lineheadO[ret * 3 + 2] = 255;//填充红色
		}
#endif
		ret += xstart;
	}
	return ret;
}
int BlocksDetector::GetEdgeHorizontalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target)
{
	vector<cv::Point> *targetBorder;


	int y1 = -1;
	int startCol = start.x;
	int endCol = end.x;
	int middleCol = (endCol - startCol) / 2 + startCol;
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::circle(drowDebugDetectUD, cv::Point(middleCol, end.y == -1 ? start.y : end.y), 3, cv::Scalar(255, 0, 0), 1);
	}
#endif
	//逼近左边，此时end为已找到的点
	if (start.y == -1)
	{
		if (bt == BlocksDetector::Up)
		{
			targetBorder = &UpLeft;
		}
		else
		{
			targetBorder = &DownLeft;
		}
		//若找到的点与最初目标点重合，扩大区域继续搜索
		if (middleCol == Target.x)
		{
			Target = cv::Point((Target.x - COL_SPAN) > 0 ? (Target.x - COL_SPAN) : 0, -1);
			return GetEdgeHorizontalApproach(Target, end, range, bt, Target);
		}
		if (middleCol == startCol || middleCol == endCol)
			return end.y;
		y1 = GetEdgeX3(cv::Point(middleCol, end.y), range, bt);
		if (y1 > 0)
		{
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				cv::circle(drowDebugResult, cv::Point(middleCol, y1), 3, cv::Scalar(255, 255, 0), 1);
				cv::circle(drowDebugResult, cv::Point(middleCol, y1), 1, cv::Scalar(255, 0, 255), -1);
			}
#endif
			targetBorder->push_back(cv::Point(middleCol, y1));
			y1 = GetEdgeHorizontalApproach(start, cv::Point(middleCol, y1), range, bt, Target);
		}
		else
			y1 = GetEdgeHorizontalApproach(cv::Point(middleCol, y1), end, range, bt, Target);
	}
	//逼近右边，此时start为已经找到的点
	else
	{
		if (bt == BlocksDetector::Up)
		{
			targetBorder = &UpRight;
		}
		else
		{
			targetBorder = &DownRight;
		}
		//若找到的点与最初目标点重合，扩大区域继续搜索
		if (middleCol == Target.x)
		{
			Target = cv::Point((Target.x + COL_SPAN + 1) > (*mdi).width ? ((*mdi).width - 1) : Target.x + COL_SPAN, -1);
			return GetEdgeHorizontalApproach(start, Target, range, bt, Target);
		}
		if (middleCol == startCol || middleCol == endCol)
			return start.y;
		y1 = GetEdgeX3(cv::Point(middleCol, start.y), range, bt);
		if (y1 > 0)
		{
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				cv::circle(drowDebugResult, cv::Point(middleCol, y1), 3, cv::Scalar(255, 255, 0), 1);
				cv::circle(drowDebugResult, cv::Point(middleCol, y1), 1, cv::Scalar(255, 0, 255), -1);
			}
#endif
			targetBorder->push_back(cv::Point(middleCol, y1));
			y1 = GetEdgeHorizontalApproach(cv::Point(middleCol, y1), end, range, bt, Target);
		}
		else
			y1 = GetEdgeHorizontalApproach(start, cv::Point(middleCol, y1), range, bt, Target);
	}
	return y1;
}
int BlocksDetector::GetEdgeHorizontal(cv::Point start, int range, bool isUp)
{
	cv::Mat oneLineGray;
	const int sumcount = SUM_COUNT;
	if (range <= sumcount)
		range = sumcount + 1;
	//确定ROI范围
	int ystart = start.y - range;
	if (ystart < 0)
		ystart = 0;
	int height = range + range;
	if ((height + ystart) >((*s).NowBufferImg.rows - 1))
		height = ((*s).NowBufferImg.rows - 1) - ystart;

	cv::cvtColor((*s).NowBufferImg(cv::Rect(start.x, ystart, 1, height)), oneLineGray, CV_BGR2GRAY);

	int ret = -1;
	int diff = SUM_THRESHOD;
#ifdef OUTPUT_DEBUG_INFO
	cv::Mat drowROI;
	cv::Mat _drowDebugDetectUD = drowDebugDetectUD;
	if (OUTPUT_DEBUG_INFO)
	{
		drowROI = drowDebugDetectUD(cv::Rect(start.x, ystart, 1, height));
	}
#endif
	for (size_t y = sumcount; y < height - sumcount; y++)
	{
		int upsum = 0;
		for (size_t yy = y; yy > y - sumcount; yy--)
		{
			upsum += oneLineGray.ptr<uchar>(yy)[0];
		}
		int downsum = 0;
		for (size_t yy = y + 1; yy <= y + sumcount; yy++)
		{
			downsum += oneLineGray.ptr<uchar>(yy)[0];
		}
		int c = 0;
		if (isUp)
			c = (downsum - upsum);
		else
			c = (upsum - downsum);
		if (c > diff)
		{
			diff = c;
			ret = y;
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				drowROI.ptr<uchar>(y)[0] = 255;//填充蓝色
			}
#endif
		}
#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			drowROI.ptr<uchar>(y)[1] = 255;//填充绿色
		}
#endif
	}
	if (ret >= 0)
	{
#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			drowROI.ptr<uchar>(ret)[2] = 255;//填充红色
		}
#endif
		ret += ystart;
	}
	return ret;
}