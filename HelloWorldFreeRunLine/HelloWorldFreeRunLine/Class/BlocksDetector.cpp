#include "BlocksDetector.h"



BlocksDetector::BlocksDetector(BufferStorage *ss, MicroDisplayInit *mdii)
{
	s = ss;
	mdi = mdii;
	drow = (*s).NowBufferImg.clone();
}
BlocksDetector::~BlocksDetector()
{
}

void BlocksDetector::Start()
{
	//vector<int> leftEdgeXTmp;
	if (1 == 1)//使用IF隔绝局部变量
	{
		int range = 300;
		int leftX = 500;
		int lastFoundLine = 0;//上一次找到边缘是在第几行，用于界定上方与下方的判断
		int noneCount = 0;//连续没有找到边缘的数量。
		bool needReFind = false;//对该行是否需要扩大range重新搜索
		for (size_t i = 0; (i + 3) < (*mdi).MaxPics; i += linespan)
		{
			cv::circle(drow, cv::Point(leftX, i), 5, cv::Scalar(0, 255, 255), 3);
			int x1 = GetEdgeLeftx3(cv::Point(leftX, i), range);
			if (x1 >= 0)
			{
				cv::Mat oneLine = (*s).NowBufferImg(cv::Rect(0, i, (*mdi).width, 1));
				uchar* lineheadRGB = oneLine.ptr<uchar>(0);//每行的起始地址

				lineheadRGB[x1 * 3 + 0] = 0;
				lineheadRGB[x1 * 3 + 1] = 0;
				lineheadRGB[x1 * 3 + 2] = 255;
				cv::circle((*s).NowBufferImg, cv::Point(x1, i), 5, cv::Scalar(0, 0, 255), 2);


				//leftEdgeLine.push_back(i);
				leftBorder.push_back(cv::Point(x1, i));
				//leftEdgeXTmp.push_back(x1);

				noneCount = 0;
				lastFoundLine = i;
				needReFind = true;

				//匀速模型预测下一个点的x坐标
				if (leftBorder.size() > 2)
				{
					int last_1 = leftBorder[leftBorder.size() - 1].x;
					int last_2 = leftBorder[leftBorder.size() - 2].x;
					leftX = last_1 + last_1 - last_2;
				}
				else
					leftX = x1;
				if (range > 50) range -= 50;
			}
			else
			{
				if (lastFoundLine > 0)
					noneCount++;
				if (noneCount > 3)//连续N个点没有找到边界后，跳出
					break;
				if (needReFind)//是否要重新扫描改行
				{
					range = 400;
					i -= linespan;
					needReFind = false;
				}
			}
		}
	}
	//vector<int> rightEdgeXTmp;
	if (1 == 1)//使用IF隔绝局部变量
	{
		int range = 300;
		int rightX = (*mdi).width - 400;
		int lastFoundLine = 0;//上一次找到边缘是在第几行，用于界定上方与下方的判断
		int noneCount = 0;//连续没有找到边缘的数量。
		bool needReFind = false;//对该行是否需要扩大range重新搜索
		for (size_t i = 0; (i + 3) < (*mdi).MaxPics; i += linespan)
		{
			cv::circle(drow, cv::Point(rightX, i), 5, cv::Scalar(0, 255, 255), 3);
			int x1 = GetEdgeRightx3(cv::Point(rightX, i), range);
			if (x1 >= 0)
			{
				cv::Mat oneLine = (*s).NowBufferImg(cv::Rect(0, i, (*mdi).width, 1));
				uchar* lineheadRGB = oneLine.ptr<uchar>(0);//每行的起始地址

				lineheadRGB[x1 * 3 + 0] = 0;
				lineheadRGB[x1 * 3 + 1] = 0;
				lineheadRGB[x1 * 3 + 2] = 255;
				cv::circle((*s).NowBufferImg, cv::Point(x1, i), 5, cv::Scalar(0, 0, 255), 2);

				rightBorder.push_back(cv::Point(x1, i));
				//rightEdgeLine.push_back(i);
				//rightEdgeXTmp.push_back(x1);
				//rightEdgeX.insert({ i, x1 });

				noneCount = 0;
				lastFoundLine = i;
				needReFind = true;

				//匀速模型预测下一个点的x坐标
				if (rightBorder.size() > 2)
				{
					int last_1 = rightBorder[rightBorder.size() - 1].x;
					int last_2 = rightBorder[rightBorder.size() - 2].x;
					rightX = last_1 + last_1 - last_2;
				}
				else
					rightX = x1;
				if (range > 50) range -= 50;
			}
			else
			{
				if (lastFoundLine > 0)
					noneCount++;
				if (noneCount > 3)//连续N个点没有找到边界后，跳出
					break;
				if (needReFind)//是否要重新扫描改行
				{
					range = 400;
					i -= linespan;
					needReFind = false;
				}
			}
		}
	}
	int leftFirstLine = leftBorder[0].y, leftFirstX = leftBorder[0].x;
	int leftLastLine = leftBorder[leftBorder.size() - 1].y, leftLastX = leftBorder[leftBorder.size() - 1].x;
	GetEdgeLeftApproach(cv::Point(-1, (leftFirstLine > linespan) ? (leftFirstLine - linespan) : 0), cv::Point(leftFirstX, leftFirstLine));
	GetEdgeLeftApproach(cv::Point(leftLastX, leftLastLine), cv::Point(-1, (leftLastLine + linespan) > (*mdi).MaxPics ? ((*mdi).MaxPics - 1) : (leftLastLine + linespan)));

	int rightFirstLine = rightBorder[0].y, rightFirstX = rightBorder[0].x;
	int rightLastLine = rightBorder[rightBorder.size() - 1].y, rightLastX = rightBorder[rightBorder.size() - 1].x;
	GetEdgeRightApproach(cv::Point(-1, (rightFirstLine > linespan) ? (rightFirstLine - linespan) : 0), cv::Point(rightFirstX, rightFirstLine));
	GetEdgeRightApproach(cv::Point(rightLastX, rightLastLine), cv::Point(-1, (rightLastLine + linespan) > (*mdi).MaxPics ? ((*mdi).MaxPics - 1) : (rightLastLine + linespan)));

	std::sort(leftBorder.begin(), leftBorder.end(), comp);
	std::sort(rightBorder.begin(), rightBorder.end(), comp);
}

int BlocksDetector::GetEdgeLeftx3(cv::Point start, int range)
{
	int a = BlocksDetector::GetEdgeLeft(start, range);
	int b = BlocksDetector::GetEdgeLeft(cv::Point(start.x, start.y + 1), range);
	int c = BlocksDetector::GetEdgeLeft(cv::Point(start.x, start.y + 1), range);
	if ((a + b + c) > 0 && abs((a + c - b - b)) < 5)
		return ((a + b + c) / 3);
	return -1;
}

int BlocksDetector::GetEdgeLeftApproach(cv::Point start, cv::Point end, int range)
{
	int x1 = -1;
	int startline = start.y;
	int endline = end.y;
	int middleLine = (endline - startline) / 2 + startline;
	//二分法，求startline与endline的中间是否有边界
	//递归直至startline = endline
	cv::circle(drow, cv::Point(end.x, middleLine), 3, cv::Scalar(255, 0, 0), 2);
	if (start.x == -1)
	{
		if (middleLine == startline || middleLine == endline)
			return end.x;
		x1 = GetEdgeLeftx3(cv::Point(end.x, middleLine), range);
		if (x1 > 0)
		{
			leftBorder.push_back(cv::Point(x1, middleLine));
			//leftEdgeLine.push_back(middleLine);
			//leftEdgeX.insert({ middleLine, x1 });
			x1 = GetEdgeLeftApproach(start, cv::Point(x1, middleLine));
		}
		else
			x1 = GetEdgeLeftApproach(cv::Point(x1, middleLine), end);
	}
	else
	{
		if (middleLine == startline || middleLine == endline)
			return start.x;
		x1 = GetEdgeLeftx3(cv::Point(start.x, middleLine), range);
		if (x1 > 0)
		{
			leftBorder.push_back(cv::Point(x1, middleLine));
			//leftEdgeLine.push_back(middleLine);
			//leftEdgeX.insert({ middleLine, x1 });
			x1 = GetEdgeLeftApproach(cv::Point(x1, middleLine), end);
		}
		else
			x1 = GetEdgeLeftApproach(start, cv::Point(-1, middleLine));
	}

	if (x1 > 0)
	{
		cv::Mat oneLine = (*s).NowBufferImg(cv::Rect(0, middleLine, (*mdi).width, 1));
		int elementCount = (*mdi).width;//每行元素数
		uchar* lineheadRGB = oneLine.ptr<uchar>(0);//每行的起始地址
		lineheadRGB[x1 * 3 + 0] = 0;
		lineheadRGB[x1 * 3 + 1] = 0;
		lineheadRGB[x1 * 3 + 2] = 255;
		cv::circle((*s).NowBufferImg, cv::Point(x1, middleLine), 3, cv::Scalar(0, 255, 0), 1);
	}
	return x1;
}

// 在:Point start坐标附近，查找是否有边缘，返回坐标，没有则返回-1
int BlocksDetector::GetEdgeLeft(cv::Point start, int range)
{
	const int sumcount = 20;
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
	int diff = sumcount * 10;

	uchar* lineheadO = drow(cv::Rect(xstart, start.y, width, 1)).ptr<uchar>(0);//每行的起始地址


	for (size_t i = sumcount; i < oneLineGray.cols - 1 - sumcount; i++)
	{
		int leftsum = 0;
		for (size_t ii = i; ii > i - 20; ii--)
		{
			leftsum += linehead[ii];
		}
		int rightsum = 0;
		for (size_t ii = i + 1; ii <= i + 20; ii++)
		{
			rightsum += linehead[ii];
		}
		int c = (rightsum - leftsum);
		if (c > diff)
		{
			diff = c;
			ret = i;
			lineheadO[i * 3] = 255;//填充蓝色
		}
		lineheadO[i * 3 + 1] = 255;//填充绿色
	}
	if (ret >= 0)
	{
		lineheadO[ret * 3 + 2] = 255;//填充红色
		ret += xstart;
	}
	return ret;
}
int BlocksDetector::GetEdgeRightx3(cv::Point start, int range)
{
	int a = BlocksDetector::GetEdgeRight(start, range);
	int b = BlocksDetector::GetEdgeRight(cv::Point(start.x, start.y + 1), range);
	int c = BlocksDetector::GetEdgeRight(cv::Point(start.x, start.y + 1), range);
	if ((a + b + c) > 0 && abs((a + c - b - b)) < 5)
		return ((a + b + c) / 3);
	return -1;
}
int BlocksDetector::GetEdgeRightApproach(cv::Point start, cv::Point end, int range)
{
	int x1 = -1;
	int startline = start.y;
	int endline = end.y;
	int middleLine = (endline - startline) / 2 + startline;
	//二分法，求startline与endline的中间是否有边界
	//递归直至startline = endline
	//if (startline )
	cv::circle(drow, cv::Point(end.x, middleLine), 3, cv::Scalar(255, 0, 0), 2);

	if (start.x == -1)
	{
		if (middleLine == startline || middleLine == endline)
			return end.x;
		x1 = GetEdgeRightx3(cv::Point(end.x, middleLine), range);
		if (x1 > 0)
		{
			rightBorder.push_back(cv::Point(x1, middleLine));
			//rightEdgeLine.push_back(middleLine);
			//rightEdgeX.insert({ middleLine, x1 });
			x1 = GetEdgeRightApproach(start, cv::Point(x1, middleLine));
		}
		else
			x1 = GetEdgeRightApproach(cv::Point(x1, middleLine), end);
	}
	else
	{
		if (middleLine == startline || middleLine == endline)
			return start.x;
		x1 = GetEdgeRightx3(cv::Point(start.x, middleLine), range);
		if (x1 > 0)
		{
			rightBorder.push_back(cv::Point(x1, middleLine));
			//rightEdgeLine.push_back(middleLine);
			//rightEdgeX.insert({ middleLine, x1 });
			x1 = GetEdgeRightApproach(cv::Point(x1, middleLine), end);
		}
		else
			x1 = GetEdgeRightApproach(start, cv::Point(-1, middleLine));
	}

	if (x1 > 0)
	{
		cv::Mat oneLine = (*s).NowBufferImg(cv::Rect(0, middleLine, (*mdi).width, 1));
		int elementCount = (*mdi).width;//每行元素数
		uchar* lineheadRGB = oneLine.ptr<uchar>(0);//每行的起始地址
		lineheadRGB[x1 * 3 + 0] = 0;
		lineheadRGB[x1 * 3 + 1] = 0;
		lineheadRGB[x1 * 3 + 2] = 255;
		cv::circle((*s).NowBufferImg, cv::Point(x1, middleLine), 3, cv::Scalar(0, 255, 0), 1);
	}

	return x1;
}
int BlocksDetector::GetEdgeRight(cv::Point start, int range)
{
	const int sumcount = 20;
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
	int diff = sumcount * 10;

	uchar* lineheadO = drow(cv::Rect(xstart, start.y, width, 1)).ptr<uchar>(0);//每行的起始地址

	//for (size_t i = elementCount - sumcount; i > sumcount - 1; i--)
	for (size_t i = sumcount; i < oneLineGray.cols - 1 - sumcount; i++)
	{
		int leftsum = 0;
		for (size_t ii = i; ii > i - 20; ii--)
		{
			leftsum += linehead[ii];
		}
		int rightsum = 0;
		for (size_t ii = i + 1; ii <= i + 20; ii++)
		{
			rightsum += linehead[ii];
		}
		int c = (leftsum - rightsum);
		if (c > diff)
		{
			diff = c;
			ret = i;
			lineheadO[i * 3] = 255;//填充蓝色
		}
		lineheadO[i * 3 + 1] = 255;//填充绿色
	}
	linehead[oneLineGray.cols - 1] = 0;
	if (ret >= 0)
	{
		lineheadO[ret * 3 + 2] = 255;//填充红色
		ret += xstart;
	}
	return ret;

	for (size_t i = sumcount; i < oneLineGray.cols - 1 - sumcount; i++)
	{
		int leftsum = 0;
		for (size_t ii = i; ii > i - 20; ii--)
		{
			leftsum += linehead[ii];
		}
		int rightsum = 0;
		for (size_t ii = i + 1; ii <= i + 20; ii++)
		{
			rightsum += linehead[ii];
		}
		int c = (rightsum - leftsum);
		if (c > diff)
		{
			diff = c;
			ret = i;
			lineheadO[i * 3] = 255;//填充蓝色
		}
		lineheadO[i * 3 + 1] = 255;//填充绿色
	}
	if (ret >= 0)
	{
		lineheadO[ret * 3 + 2] = 255;//填充红色
		ret += xstart;
	}
}