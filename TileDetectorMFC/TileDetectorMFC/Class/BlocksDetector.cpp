#include "BlocksDetector.h"



BlocksDetector::BlocksDetector(cv::Mat& Img)
{
	img = Img;

	//初始化左边缘检测参数
	leftY = 0;//在第几行检测
	leftX = ORANGE_MARGIN_ROW;//检测的中点
	leftRnage = ORANGE_RANGE_ROW;//在中点周围多少像素内检测
	leftNoneCount = 0;//连续没有找到边缘的数量。
	leftNeedReFind = false;//对该行是否需要扩大range重新搜索

	rightY = 0;//在第几行检测
	rightX = img.cols - ORANGE_MARGIN_ROW;;//检测的中点
	rightRnage = ORANGE_RANGE_ROW;//在中点周围多少像素内检测
	rightNoneCount = 0;//连续没有找到边缘的数量。
	rightNeedReFind = false;//对该行是否需要扩大range重新搜索


#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
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
	}
#endif
}
BlocksDetector::~BlocksDetector()
{
}

bool BlocksDetector::Start()
{
	A = B = C = D = cv::Point(0, 0);

	if (1 == 1)//用IF隔离局部代码
	{
		for (; (leftY + 3) < img.rows; leftY += ROW_SPAN)
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
		if (LeftBorder.size() < 2)
			return false;

		//拟合直线，判断可疑点
		cv::Vec4f leftLine;
		cv::fitLine(cv::Mat(LeftBorder), leftLine, CV_DIST_L2, 0, 0.01, 0.01);

		LeftLine.x0 = leftLine[2];
		LeftLine.y0 = leftLine[3];
		LeftLine.dx = leftLine[0];
		LeftLine.dy = leftLine[1];
		LeftLine.k = LeftLine.dy / LeftLine.dx;
		for (size_t i = 0; i < tmpLeftList.size(); i++)
		{
			int x = tmpLeftList[i].x;
			int y = tmpLeftList[i].y;
			if (abs((y - LeftLine.y0) / LeftLine.k + LeftLine.x0 - x) < 10)
			{
				LeftBorder.push_back(tmpLeftList[i]);
			}
		}
		std::sort(LeftBorder.begin(), LeftBorder.end(), ORDER_BY_Y_ASC);



		///******************逼近顶点******************/
		//LeftUp.push_back(LeftBorder[0]);
		//LeftDown.push_back(LeftBorder[LeftBorder.size() - 1]);
		//int leftFirstLine = LeftBorder[0].y, leftFirstX = LeftBorder[0].x;
		//int leftLastLine = LeftBorder[LeftBorder.size() - 1].y, leftLastX = LeftBorder[LeftBorder.size() - 1].x;
		//cv::Point upstart = cv::Point(-1, (leftFirstLine > ROW_SPAN) ? (leftFirstLine - ROW_SPAN) : 0);
		//cv::Point upend = cv::Point(leftFirstX, leftFirstLine);
		//GetEdgeVerticalApproach(upstart, upend, RANGE_MINI, BorderType::Left, upstart);
		//cv::Point downstart = cv::Point(leftLastX, leftLastLine);
		//cv::Point downend = cv::Point(-1, (leftLastLine + ROW_SPAN) > img.rows ? (img.rows - 1) : (leftLastLine + ROW_SPAN));
		//GetEdgeVerticalApproach(downstart, downend, RANGE_MINI, BorderType::Left, downend);
		//if (LeftUp.size() > 0)
		//{
		//	std::sort(LeftUp.begin(), LeftUp.end(), ORDER_BY_Y_ASC);
		//	DetectedA.y = LeftUp[0].y;
		//}
		//if (LeftUp.size() > 0)
		//{
		//	std::sort(LeftDown.begin(), LeftDown.end(), ORDER_BY_Y_DESC);
		//	DetectedD.y = LeftDown[0].y;
		//}
		///******************逼近顶点结束******************/



#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			//debug绘图
			for (size_t i = 0; i < LeftBorder.size(); i++)
				cv::circle(drowDebugDetectLR, LeftBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpLeftList.size(); i++)
				cv::circle(drowDebugDetectLR, tmpLeftList[i], 20, cv::Scalar(70, 255, 255), 2);
			for (size_t i = 0; i < RightBorder.size(); i++)
				cv::circle(drowDebugDetectLR, RightBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpRightList.size(); i++)
				cv::circle(drowDebugDetectLR, tmpRightList[i], 20, cv::Scalar(70, 255, 255), 2);


			//
			for (size_t i = 0; i < LeftBorder.size(); i++)
				cv::circle(drowDebugResult, LeftBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpLeftList.size(); i++)
				cv::circle(drowDebugResult, tmpLeftList[i], 20, cv::Scalar(70, 255, 255), 2);
			for (size_t i = 0; i < LeftUp.size(); i++)
				cv::circle(drowDebugResult, LeftUp[i], 30, cv::Scalar(100, 255, 30), 1);
			for (size_t i = 0; i < LeftDown.size(); i++)
				cv::circle(drowDebugResult, LeftDown[i], 30, cv::Scalar(100, 255, 30), 1);


			cv::line(drowDebugResult, cv::Point((img.rows - LeftLine.y0) / LeftLine.k + LeftLine.x0, img.rows), cv::Point((0 - LeftLine.y0) / LeftLine.k + LeftLine.x0, 0), cv::Scalar(180, 100, 255), 1);
			//cv::line(drowDebugResult, cv::Point(x0, y0), cv::Point(leftLine[2] + 2000 * leftLine[0], leftLine[3] + 2000 * leftLine[1]), cv::Scalar(255, 255, 255), 3);

		}
#endif
	}
	if (2 == 2)
	{
		for (; (rightY + 3) < img.rows; rightY += ROW_SPAN)
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
		if (RightBorder.size() < 2)
			return false;

		//拟合直线，判断可疑点
		cv::Vec4f rightLine;
		cv::fitLine(cv::Mat(RightBorder), rightLine, CV_DIST_L2, 0, 0.01, 0.01);

		RightLine.x0 = rightLine[2];
		RightLine.y0 = rightLine[3];
		RightLine.dx = rightLine[0];
		RightLine.dy = rightLine[1];
		RightLine.k = RightLine.dy / RightLine.dx;
		for (size_t i = 0; i < tmpRightList.size(); i++)
		{
			int x = tmpRightList[i].x;
			int y = tmpRightList[i].y;
			if (abs((y - RightLine.y0) / RightLine.k + RightLine.x0 - x) < 10)
			{
				RightBorder.push_back(tmpRightList[i]);
			}
		} 
		std::sort(RightBorder.begin(), RightBorder.end(), ORDER_BY_Y_ASC);


		///******************逼近顶点******************/
		//RightUp.push_back(RightBorder[RightBorder.size() - 1]);
		//RightDown.push_back(RightBorder[0]);
		//int rightFirstLine = RightBorder[0].y, rightFirstX = RightBorder[0].x;
		//int rightLastLine = RightBorder[RightBorder.size() - 1].y, rightLastX = RightBorder[RightBorder.size() - 1].x;
		//cv::Point upstart = cv::Point(-1, (rightFirstLine > ROW_SPAN) ? (rightFirstLine - ROW_SPAN) : 0);
		//cv::Point upend = cv::Point(rightFirstX, rightFirstLine);
		//GetEdgeVerticalApproach(upstart, upend, RANGE_MINI, BorderType::Right, upstart);
		//cv::Point downstart = cv::Point(rightLastX, rightLastLine);
		//cv::Point downend = cv::Point(-1, (rightLastLine + ROW_SPAN) > img.rows ? (img.rows - 1) : (rightLastLine + ROW_SPAN));
		//GetEdgeVerticalApproach(downstart, downend, RANGE_MINI, BorderType::Right, downstart);

		//if (RightUp.size() > 0)
		//{
		//	std::sort(RightUp.begin(), RightUp.end(), ORDER_BY_Y_ASC);
		//	DetectedB.y = RightUp[0].y;
		//}
		//if (RightDown.size() > 0)
		//{
		//	std::sort(RightDown.begin(), RightDown.end(), ORDER_BY_Y_DESC);
		//	DetectedC.y = RightDown[0].y;
		//}
		///******************逼近顶点结束******************/

#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			for (size_t i = 0; i < RightBorder.size(); i++)
				cv::circle(drowDebugDetectLR, RightBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpRightList.size(); i++)
				cv::circle(drowDebugDetectLR, tmpRightList[i], 6, cv::Scalar(128, 128, 0), 1);


			for (size_t i = 0; i < RightBorder.size(); i++)
				cv::circle(drowDebugResult, RightBorder[i], 5, cv::Scalar(0, 0, 255), 2);
			for (size_t i = 0; i < tmpRightList.size(); i++)
				cv::circle(drowDebugResult, tmpRightList[i], 6, cv::Scalar(128, 128, 0), 1);
			for (size_t i = 0; i < RightUp.size(); i++)
				cv::circle(drowDebugResult, RightUp[i], 30, cv::Scalar(100, 255, 30), 1);
			for (size_t i = 0; i < RightDown.size(); i++)
				cv::circle(drowDebugResult, RightDown[i], 30, cv::Scalar(100, 255, 30), 1);

			cv::line(drowDebugResult, cv::Point((img.rows - RightLine.y0) / RightLine.k + RightLine.x0, img.rows), cv::Point((0 - RightLine.y0) / RightLine.k + RightLine.x0, 0), cv::Scalar(180, 100, 255), 1);

		}
#endif
	}
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

	return true;
}
bool BlocksDetector::StartUP_DOWN(BorderType bt)
{
	int centerY = 0;
	if (allLeftList.size() == 0 || allRightList.size() == 0)
		return false;
	if (bt == BlocksDetector::Up && allLeftList[0].y == 0 && allRightList[0].y == 0)
		return false;
	else if (bt == BlocksDetector::Down && !(allLeftList[allLeftList.size() - 1].y > 0 && allLeftList[allLeftList.size() - 1].y < (img.rows - SUM_COUNT) && allRightList[allRightList.size() - 1].y>0 && allRightList[allRightList.size() - 1].y < (img.rows - SUM_COUNT)))
		return false;
	vector<cv::Point> *targetBorder;
	vector<cv::Point> *tmptargetList;
	vector<cv::Point> *alltargetList;
	Line *line;
	if (bt == BlocksDetector::Up)
	{
		targetBorder = &UpBorder;
		tmptargetList = &tmpUpList;
		alltargetList = &allUpList;
		line = &UpLine;
		centerY = allLeftList[0].y;
	}
	else
	{
		targetBorder = &DownBorder;
		tmptargetList = &tmpDownList;
		alltargetList = &allDownList;
		line = &DownLine;
		centerY = allLeftList[allLeftList.size() - 1].y;
	}
	int range = ORANGE_RANGE_COL;
	//截取ROI
	//ROI的起点y值，同时也是计算最终y坐标的OFFSET
	//int offsetY = LeftBorder[0].y - ORANGE_RANGE_COL;
	//if (offsetY < 0) offsetY = 0;
	//ROI中边界查找的起点行高
	bool needReFind = false;//对该行是否需要扩大range重新搜索
	for (size_t x = 0; (x + 3) < img.cols; x += COL_SPAN)
	{
		//相对于整幅图像的Y值
		int y1 = GetEdgeX3(cv::Point(x, centerY), range, bt);
		if (y1 >= 0)
		{
			(*alltargetList).push_back(cv::Point(x, y1));
			if (range == RANGE_MINI || (*targetBorder).size() < 3)
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

			if (range > RANGE_MINI) range -= RANGE_REDUCE_BY_COL;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//是否要重新扫描改行
			{
				range = ORANGE_RANGE_COL;
				x -= COL_SPAN;
				needReFind = false;
			}
		}
	}


	//拟合直线，判断可疑点
	cv::Vec4f line4f;
	cv::fitLine(cv::Mat((*targetBorder)), line4f, CV_DIST_L2, 0, 0.01, 0.01);

	(*line).x0 = line4f[2];
	(*line).y0 = line4f[3];
	(*line).dx = line4f[0];
	(*line).dy = line4f[1];
	(*line).k = (*line).dy / (*line).dx;

	for (size_t i = 0; i < (*tmptargetList).size(); i++)
	{
		int x = (*tmptargetList)[i].x;
		int y = (*tmptargetList)[i].y;
		if (abs((y - (*line).y0) / (*line).k + (*line).x0 - x) < 10)
		{
			(*targetBorder).push_back((*tmptargetList)[i]);
		}
	}
	std::sort((*targetBorder).begin(), (*targetBorder).end(), ORDER_BY_X_ASC);

	if (bt == BorderType::Up)
	{
		UpLeft.push_back((*targetBorder)[0]);
		UpRight.push_back((*targetBorder)[(*targetBorder).size() - 1]);
	}
	else
	{
		DownLeft.push_back((*targetBorder)[0]);
		DownRight.push_back((*targetBorder)[(*targetBorder).size() - 1]);
	}

	///******************逼近顶点******************/
	//int upFirstCol = (*targetBorder)[0].x, upFirstRow = (*targetBorder)[0].y + ((*targetBorder)[0].y - (*targetBorder)[1].y) / 2;
	//int upLastCol = (*targetBorder)[(*targetBorder).size() - 1].x, upLastRow = (*targetBorder)[(*targetBorder).size() - 1].y;
	//cv::Point leftstart = cv::Point((upFirstCol > COL_SPAN) ? (upFirstCol - COL_SPAN) : 0, -1);
	//cv::Point leftend = cv::Point(upFirstCol, upFirstRow);
	//GetEdgeHorizontalApproach(leftstart, leftend, RANGE_MINI, bt, leftstart);
	//cv::Point rightstart = cv::Point(upLastCol, upLastRow);
	//cv::Point rightend = cv::Point((upLastCol + COL_SPAN) > img.cols ? (img.cols - 1) : (upLastCol + COL_SPAN), -1);
	//GetEdgeHorizontalApproach(rightstart, rightend, RANGE_MINI, bt, rightend);

	//////排序整理
	//if (bt == BorderType::Up && UpLeft.size() > 0)
	//{
	//	std::sort(UpLeft.begin(), UpLeft.end(), ORDER_BY_X_ASC);
	//	std::sort(UpRight.begin(), UpRight.end(), ORDER_BY_X_DESC);
	//	DetectedA.x = UpLeft[0].x;
	//	DetectedB.x = UpRight[0].x;
	//}
	//else if (DownRight.size() > 0)
	//{
	//	std::sort(DownLeft.begin(), DownLeft.end(), ORDER_BY_X_ASC);
	//	std::sort(DownRight.begin(), DownRight.end(), ORDER_BY_X_DESC);
	//	DetectedD.x = DownLeft[0].x;
	//	DetectedC.x = DownRight[0].x;
	//}
	///******************逼近顶点结束******************/

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
		cv::line(drowDebugResult, cv::Point(0, (*line).k * (0 - (*line).x0) + (*line).y0), cv::Point(img.cols, (*line).k * (img.cols - (*line).x0) + (*line).y0), cv::Scalar(180, 255, 100), 1);
		//cv::line(drowDebugResult, cv::Point((*line).x0, (*line).y0), cv::Point((*line).x0 + 2000 * (*line).dx, (*line).y0 + 2000 * (*line).dy), cv::Scalar(255, 0, 255), 3);

		//for (size_t i = 0; i < LeftUp.size(); i++)
		//	cv::circle(drowDebugResult, LeftUp[i], 1, cv::Scalar(0, 255, 0), 1);
		//for (size_t i = 0; i < LeftDown.size(); i++)
		//	cv::circle(drowDebugResult, LeftDown[i], 1, cv::Scalar(0, 255, 0), 1);
	}
#endif


	return true;
}
bool BlocksDetector::ABCD()
{
	//A
	if (1 == 1)
	{
		double k1 = UpLine.k;
		double x1 = UpLine.x0;
		double y1 = UpLine.y0;
		double k2 = LeftLine.k;
		double x2 = LeftLine.x0;
		double y2 = LeftLine.y0;
		A.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		A.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}
	//B
	if (1 == 1)
	{
		double k1 = UpLine.k;
		double x1 = UpLine.x0;
		double y1 = UpLine.y0;
		double k2 = RightLine.k;
		double x2 = RightLine.x0;
		double y2 = RightLine.y0;
		B.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		B.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}
	//C
	if (1 == 1)
	{
		double k1 = DownLine.k;
		double x1 = DownLine.x0;
		double y1 = DownLine.y0;
		double k2 = RightLine.k;
		double x2 = RightLine.x0;
		double y2 = RightLine.y0;
		C.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		C.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}
	//D
	if (1 == 1)
	{
		double k1 = DownLine.k;
		double x1 = DownLine.x0;
		double y1 = DownLine.y0;
		double k2 = LeftLine.k;
		double x2 = LeftLine.x0;
		double y2 = LeftLine.y0;
		D.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		D.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}

	if (A.x <= 0 || A.x >= (img.cols - 1) || A.y <= 0 || A.y >= (img.rows - 1))
		return false;
	if (B.x <= 0 || B.x >= (img.cols - 1) || B.y <= 0 || B.y >= (img.rows - 1))
		return false;
	if (C.x <= 0 || C.x >= (img.cols - 1) || C.y <= 0 || C.y >= (img.rows - 1))
		return false;
	if (D.x <= 0 || D.x >= (img.cols - 1) || D.y <= 0 || D.y >= (img.rows - 1))
		return false;
	return true;
}

int BlocksDetector::DetectOneLineLeft(int y)
{
	if (y == -1)
		y = leftY;
	//判断是否越界
	if (y < 0 || (y + 3 + 1) >img.rows)
		return -1;
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		//绘制检测中心点
		//cv::circle(drowDebugDetectLR, cv::Point(leftX, y), 5, cv::Scalar(0, 255, 255), 3);
	}
#endif
	int x1 = GetEdgeX3(cv::Point(leftX, y), leftRnage, BlocksDetector::Left);
	if (x1 >= 0)
	{
		allLeftList.push_back(cv::Point(x1, y));
		//如果在预测范围之内找到了边界，就认为找到了真正的点，否则认为存疑，存入临时vector。
		if (leftRnage == RANGE_MINI)
		{
			LeftBorder.push_back(cv::Point(x1, y));
		}
		else
		{
			tmpLeftList.push_back(cv::Point(x1, y));
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
		if (leftRnage > RANGE_MINI) leftRnage -= RANGE_REDUCE_BY_ROW;
		if (leftRnage < RANGE_MINI) leftRnage = RANGE_MINI;
	}
	else if (LeftBorder.size() > 0)
		leftNoneCount++;
	return x1;
}
int BlocksDetector::DetectOneLineRight(int y)
{
	//判断是否越界
	if (y < 0 || (y + 3 + 1) >img.rows)
		return -1;
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		//绘制检测中心点
		//cv::circle(drowDebugDetectLR, cv::Point(rightX, y), 5, cv::Scalar(0, 255, 255), 3);
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
		if (rightRnage > RANGE_MINI) rightRnage -= RANGE_REDUCE_BY_ROW;
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
	Line *line;

	vector<cv::Point> *collection;
	if (bt == BlocksDetector::Left)
	{
		line = &LeftLine;
		if (start.x == -1)
			collection = &LeftUp;
		else
			collection = &LeftDown;
	}
	else
	{
		line = &RightLine;
		if (start.x == -1)
			collection = &RightUp;
		else
			collection = &RightDown;
	}

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
		if (middleLine == Target.y && middleLine != 0 && middleLine < (img.rows - 2))
		{
			Target = cv::Point(-1, (Target.y > ROW_SPAN) ? (Target.y - ROW_SPAN) : 0);
			return GetEdgeVerticalApproach(Target, end, range, bt, Target);
		}
		if (middleLine == startline || middleLine == endline)
			return end.x;
		x1 = GetEdgeX3(cv::Point(end.x, middleLine), range, bt);
		if (x1 > 0)//  && IsPointNearline(*line, cv::Point(x1, middleLine))
		{
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				cv::circle(drowDebugResult, cv::Point(x1, middleLine), 3, cv::Scalar(0, 255, 0), 1);
			}
#endif
			(*collection).push_back(cv::Point(x1, middleLine));
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
			Target = cv::Point(-1, (Target.y + ROW_SPAN) > img.rows ? (img.rows - 1) : (Target.y + ROW_SPAN));
			return GetEdgeVerticalApproach(start, Target, range, bt, Target);
		}
		if (middleLine == startline || middleLine == endline)
			return start.x;
		x1 = GetEdgeX3(cv::Point(start.x, middleLine), range, bt);
		if (x1 > 0)// && IsPointNearline(*line, cv::Point(x1, middleLine))
		{
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				cv::circle(drowDebugResult, cv::Point(x1, middleLine), 3, cv::Scalar(0, 255, 0), 1);
			}
#endif
			(*collection).push_back(cv::Point(x1, middleLine));
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
	if ((width + xstart) >img.cols - 1)
		width = img.cols - 1 - xstart;
	if (img.channels() == 1)
		oneLineGray = img(cv::Rect(xstart, start.y, width, 1));
	else
		cv::cvtColor(img(cv::Rect(xstart, start.y, width, 1)), oneLineGray, CV_BGR2GRAY);

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


#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			lineheadO[i * 3 + 1] = 255;//填充绿色，表示扫描范围
			lineheadO[i * 3 + 0] = 0;//填充绿色，表示扫描范围
			lineheadO[i * 3 + 2] = 0;//填充绿色，表示扫描范围
		}
#endif
		if (c > diff)
		{
			diff = c;
			ret = i;
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				lineheadO[i * 3] = 255;//填充蓝色表示diff超过阈值
				lineheadO[i * 3 + 1] = 0;
				lineheadO[i * 3 + 2] = 0;
			}
#endif
		}
	}
	if (ret >= 0)
	{
#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			lineheadO[ret * 3 + 2] = 255;//填充红色
			lineheadO[ret * 3 + 1] = 0;
			lineheadO[ret * 3 + 0] = 0;
		}
#endif
		ret += xstart;
	}
	return ret;
}
int BlocksDetector::GetEdgeHorizontalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target)
{
	vector<cv::Point> *targetBorder;
	Line *line;

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
			line = &UpLine;
		}
		else
		{
			targetBorder = &DownLeft;
			line = &DownLine;
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
		if (y1 > 0)// && IsPointNearline((*line), cv::Point(middleCol, y1))
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
			line = &UpLine;
		}
		else
		{
			targetBorder = &DownRight;
			line = &DownLine;
		}
		//若找到的点与最初目标点重合，扩大区域继续搜索
		if (middleCol == Target.x)
		{
			Target = cv::Point((Target.x + COL_SPAN + 1) > img.cols ? (img.cols - 1) : Target.x + COL_SPAN, -1);
			return GetEdgeHorizontalApproach(start, Target, range, bt, Target);
		}
		if (middleCol == startCol || middleCol == endCol)
			return start.y;
		y1 = GetEdgeX3(cv::Point(middleCol, start.y), range, bt);
		if (y1 > 0)//&& IsPointNearline((*line), cv::Point(middleCol, y1))
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
	if ((height + ystart) >(img.rows - 1))
		height = (img.rows - 1) - ystart;

	if (img.channels() == 1)
		oneLineGray = img(cv::Rect(start.x, ystart, 1, height));
	else
		cv::cvtColor(img(cv::Rect(start.x, ystart, 1, height)), oneLineGray, CV_BGR2GRAY);
	

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
			upsum += oneLineGray.ptr<uchar>(yy)[0] > 5 ? 255 : 0;//TODO:确定要这么干嘛？
		}
		int downsum = 0;
		for (size_t yy = y + 1; yy <= y + sumcount; yy++)
		{
			downsum += oneLineGray.ptr<uchar>(yy)[0] > 5 ? 255 : 0;
		}
		int c = 0;
		if (isUp)
			c = (downsum - upsum);
		else
			c = (upsum - downsum);

#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			drowROI.ptr<uchar>(y)[1] = 255;//填充绿色
			drowROI.ptr<uchar>(y)[0] = 0;//填充绿色
			drowROI.ptr<uchar>(y)[2] = 0;//填充绿色
		}
#endif
		if (c > diff)
		{
			diff = c;
			ret = y;
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				drowROI.ptr<uchar>(y)[0] = 255;//填充蓝色
				drowROI.ptr<uchar>(y)[1] = 0;//填充蓝色
				drowROI.ptr<uchar>(y)[2] = 0;//填充蓝色
			}
#endif
		}
	}
	if (ret >= 0)
	{
#ifdef OUTPUT_DEBUG_INFO
		if (OUTPUT_DEBUG_INFO)
		{
			drowROI.ptr<uchar>(ret)[2] = 255;//填充红色
			drowROI.ptr<uchar>(ret)[1] = 0;//填充红色
			drowROI.ptr<uchar>(ret)[0] = 0;//填充红色
		}
#endif
		ret += ystart;
	}
	return ret;
}
bool BlocksDetector::IsPointNearline(Line line, cv::Point point)
{
	const int threshold = 10;
	int x = point.x;
	int y = point.y;
	//竖直边缘
	if (abs(line.k) > 1)
	{
		if (abs((y - line.y0) / line.k + line.x0 - x) < threshold)//x = (y-y0)/k + x0
		{
			return true;
		}
	}
	//水平边缘
	else
	{
		if (abs(line.k*(x - line.x0) + line.y0) < threshold)//y = k(x-x0)+y0
		{
			return true;
		}
	}
	return false;
}