#include "BlockLocalizer.h"


BlockLocalizer::BlockLocalizer(cv::Mat& _img, Block* _block, Faults* _faults)
{
	img = _img;
	p_block = _block;
	p_faults = _faults;

	NotFoundBlockFlag = true;//δ�ҵ���ש���
	BrokenEdgeFlag = false;//�б���ȱ�ݱ��

#ifdef BL_OUTPUT_DEBUG_INFO
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
}

void BlockLocalizer::Run()
{
	FindUp();
	if (uppoints.size() < 5)
	{
		return;
	}
#ifdef BL_OUTPUT_DEBUG_INFO
	FindLeft();
	FindRight();
#else
	thread t1 = thread(std::mem_fn(&BlockLocalizer::FindLeft), this);
	thread t2 = thread(std::mem_fn(&BlockLocalizer::FindRight), this);
	t1.join();
	t2.join();
#endif
	if (leftpoints.size() < 5 || rightpoints.size() < 5)
	{
		return;
	}
	FindDown();
	if (downpoints.size() < 5)
	{
		return;
	}
	Judgement();
	p_block->Lines2ABCD();
	NotFoundBlockFlag = false;

	p_block->Axis_X_mmPerPix = p_block->Real_WidthMM * 2 / (double)(abs(p_block->B.x - p_block->A.x) + abs(p_block->C.x - p_block->D.x));
	p_block->Axis_Y_mmPerPix = p_block->Real_LengthMM * 2 / (double)(abs(p_block->D.y - p_block->A.y) + abs(p_block->C.y - p_block->B.y));
#ifdef BL_OUTPUT_DEBUG_INFO
	cv::line(drowDebugResult, cv::Point(0, (*p_block).UpLine.k * (0 - (*p_block).UpLine.x0) + (*p_block).UpLine.y0), cv::Point(drowDebugResult.cols, (*p_block).UpLine.k * (drowDebugResult.cols - (*p_block).UpLine.x0) + (*p_block).UpLine.y0), cv::Scalar(0, 0, 255), 1);
	cv::line(drowDebugResult, cv::Point(0, (*p_block).DownLine.k * (0 - (*p_block).DownLine.x0) + (*p_block).DownLine.y0), cv::Point(drowDebugResult.cols, (*p_block).DownLine.k * (drowDebugResult.cols - (*p_block).DownLine.x0) + (*p_block).DownLine.y0), cv::Scalar(0, 255, 255), 1);
	cv::line(drowDebugResult, cv::Point((drowDebugResult.rows - (*p_block).LeftLine.y0) / (*p_block).LeftLine.k + (*p_block).LeftLine.x0, drowDebugResult.rows), cv::Point((0 - (*p_block).LeftLine.y0) / (*p_block).LeftLine.k + (*p_block).LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
	cv::line(drowDebugResult, cv::Point((drowDebugResult.rows - (*p_block).RightLine.y0) / (*p_block).RightLine.k + (*p_block).RightLine.x0, drowDebugResult.rows), cv::Point((0 - (*p_block).RightLine.y0) / (*p_block).RightLine.k + (*p_block).RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);

	if (p_faults->BrokenEdges.size() > 0)
	{
		for (int i = 0; i < p_faults->BrokenEdges.size(); i++)
		{
			cv::circle(drowDebugResult, p_faults->BrokenEdges[i].position, p_faults->BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
		}
	}
#endif
}

BlockLocalizer::~BlockLocalizer()
{
	p_faults = NULL;
	p_block = NULL;
}

void BlockLocalizer::FindUp()
{
	cv::Point firstPoint(2048,0);
	//�Ȳ��ҵ�һ����
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
	bool needReFind = false;//�Ը����Ƿ���Ҫ����range��������
	//ɨ�������㣬������
	for (int x = 2048 + COL_SPAN; x < img.cols; x += COL_SPAN)
	{
		int	y = getYOnLine(cv::Point(x, centerY), range);
		if (y >= 0)
		{
			uppoints.push_back(cv::Point(x, y));
			//����ģ��Ԥ����һ�����y����
			centerY = y + y - lastY;
			if (centerY < 0) centerY = 0;
			if (centerY >= img.rows) centerY = img.rows - 1;
			lastY = y;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) /3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//�Ƿ�Ҫ����ɨ�����
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
	//ɨ�������㣬������
	for (int x = 2048 - COL_SPAN; x > 0; x -= COL_SPAN)
	{
		int	y = getYOnLine(cv::Point(x, centerY), range);
		if (y >= 0)
		{
			uppoints.push_back(cv::Point(x, y));
			//����ģ��Ԥ����һ�����y����
			centerY = y + y - lastY;
			lastY = y;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//�Ƿ�Ҫ����ɨ�����
			{
				range = RANGE_DEFAULT;
				x += COL_SPAN;
				needReFind = false;
			}
		}
	}
	std::sort(uppoints.begin(), uppoints.end(), ORDER_BY_X_ASC);
	std::unique(uppoints.begin(), uppoints.end());
	



#ifdef BL_OUTPUT_DEBUG_INFO
	//debug��ͼ
	for (int i = 0; i < uppoints.size(); i++)
	{
		cv::circle(drowDebugDetectUD, uppoints[i], 4, cv::Scalar(255, 255, 0), -1);
	}
#endif

	needReFind = 0;
}
void BlockLocalizer::FindLeft()
{
	cv::Point firstPoint(uppoints[0].x, uppoints[0].y + 100);

	//�Ȳ��ҵ�һ����
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
	bool needReFind = false;//�Ը����Ƿ���Ҫ����range��������
	//ɨ�������㣬������
	for (int y = firstPoint.y + ROW_SPAN; y < img.rows; y += ROW_SPAN)
	{
		int	x = getXOnRow(cv::Point(centerX, y), range);
		if (x >= 0)
		{
			leftpoints.push_back(cv::Point(x, y));
			//����ģ��Ԥ����һ�����y����
			centerX = x + x - lastX;
			lastX = x;
			if (lastX < 0) lastX = 0;
			if (lastX >= img.cols) lastX = img.cols - 1;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else
		{
			if (needReFind)//�Ƿ�Ҫ����ɨ�����
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


#ifdef BL_OUTPUT_DEBUG_INFO
	//debug��ͼ
	for (int i = 0; i < leftpoints.size(); i++)
	{
		cv::circle(drowDebugDetectLR, leftpoints[i], 1, cv::Scalar(255, 255, 0), -1);
	}
#endif
	//needReFind = 0;
}
void BlockLocalizer::FindRight()
{
	cv::Point firstPoint(uppoints[uppoints.size() - 1].x, uppoints[uppoints.size() - 1].y + 100);

	//�Ȳ��ҵ�һ����
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
	bool needReFind = false;//�Ը����Ƿ���Ҫ����range��������
	//ɨ�������㣬������
	for (int y = firstPoint.y + ROW_SPAN; (y) < img.rows; y += ROW_SPAN)
	{
		int	x = getXOnRow(cv::Point(centerX, y), range, false);
		if (x >= 0)
		{
			rightpoints.push_back(cv::Point(x, y));
			//����ģ��Ԥ����һ�����y����
			centerX = x + x - lastX;
			if (lastX < 0) lastX = 0;
			if (lastX >= img.cols) lastX = img.cols - 1;
			lastX = x;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else if (needReFind)//�Ƿ�Ҫ����ɨ�����
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


#ifdef BL_OUTPUT_DEBUG_INFO
	//debug��ͼ
	for (int i = 0; i < rightpoints.size(); i++)
	{
		cv::circle(drowDebugDetectLR, rightpoints[i], 1, cv::Scalar(255, 255, 0), -1);
	}
#endif
	//needReFind = 0;
}
void BlockLocalizer::FindDown()
{
	cv::Point firstPoint(leftpoints[leftpoints.size() - 1].x + 100, leftpoints[leftpoints.size() - 1].y);
	//�Ȳ��ҵ�һ����
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
	bool needReFind = false;//�Ը����Ƿ���Ҫ����range��������
	//ɨ�������㣬������
	for (int x = firstPoint.x; (x + COL_SPAN) < img.cols; x += COL_SPAN)
	{
		int	y = getYOnLine(cv::Point(x, centerY), range, false);
		if (y >= 0)
		{
			downpoints.push_back(cv::Point(x, y));
			//����ģ��Ԥ����һ�����y����
			centerY = y + y - lastY;
			if (centerY < 0) centerY = 0;
			if (centerY >= img.rows) centerY = img.rows - 1;
			lastY = y;
			if (!needReFind)
				needReFind = true;

			if (range > RANGE_MINI) range -= (RANGE_DEFAULT - RANGE_MINI) / 3;
			if (range < RANGE_MINI) range = RANGE_MINI;
		}
		else if (needReFind)//�Ƿ�Ҫ����ɨ�����
		{
			range = RANGE_DEFAULT;
			x -= COL_SPAN;
			needReFind = false;
		}
	}
	std::sort(downpoints.begin(), downpoints.end(), ORDER_BY_X_ASC);
	std::unique(downpoints.begin(), downpoints.end());

#ifdef BL_OUTPUT_DEBUG_INFO
	//debug��ͼ
	for (int i = 0; i < downpoints.size(); i++)
	{
		cv::circle(drowDebugDetectUD, downpoints[i], 4, cv::Scalar(255, 255, 0), -1);
	}
#endif
}

int BlockLocalizer::getYOnLine(cv::Point startPoint, int range, bool scanUp2Down)
{
	const int roiWidth = 11;//��ȡROI��ȣ�һ��Ϊ�����������xֵΪROI������

	//����roi��Χ������ֹԽ�硣
	cv::Rect roiRect(0, 0, roiWidth, range);
	roiRect.x = startPoint.x - roiWidth / 2;
	roiRect.y = startPoint.y - range / 2;

	rectFix(roiRect);

	if (roiRect.height > ContinuePointCount && roiRect.width > 2)
	{
#ifdef BL_OUTPUT_DEBUG_INFO
		if (range == 400)
			cv::rectangle(drowDebugDetectUD, roiRect, cv::Scalar(0, 0, 255));
		else
			cv::rectangle(drowDebugDetectUD, roiRect, cv::Scalar(0, 255, 255));
#endif

		//ѡȡroi
		cv::Mat roi = img(roiRect).clone();
		cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0, 0);
		cv::Mat canny;
		cv::Canny(roi, canny, 7, 10);
		//ˮƽͶӰ
		cv::Mat roirow;
		cv::reduce(roi, roirow, 1, CV_REDUCE_AVG);
		if (scanUp2Down)
		{
			for (int j = 0; j < roirow.rows; j++)
			{
				int count = 0;
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

					if (count > ContinuePointCount)
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
				int count = 0;
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

					if (count > ContinuePointCount)
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
	const int roiHeight = 11;//��ȡROI��ȣ�һ��Ϊ�����������xֵΪROI������

	//����roi��Χ������ֹԽ�硣
	cv::Rect roiRect(0, 0, range, roiHeight);
	roiRect.x = startPoint.x - range / 2;
	roiRect.y = startPoint.y - roiHeight / 2;

	rectFix(roiRect);


	if (roiRect.width > ContinuePointCount && roiRect.height > 2)
	{
#ifdef BL_OUTPUT_DEBUG_INFO
		if (range == 400)
			cv::rectangle(drowDebugDetectLR, roiRect, cv::Scalar(0, 0, 255));
		else
			cv::rectangle(drowDebugDetectLR, roiRect, cv::Scalar(0, 255, 255));
#endif

		//ѡȡroi
		cv::Mat roi = img(roiRect).clone();
		cv::GaussianBlur(roi, roi, cv::Size(5, 5), 0, 0);
		//��ֱͶӰ
		cv::Mat roirow;
		cv::reduce(roi, roirow, 0, CV_REDUCE_AVG);
		if (scanLeft2right)
		{
			for (int j = 0; j < roirow.cols; j++)
			{
				int count = 0;
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

					if (count > ContinuePointCount)
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
				int count = 0;
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

					if (count > ContinuePointCount)
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
	judgementForOneLine(uppoints, true, (*p_block).UpLine);
	judgementForOneLine(downpoints, true, (*p_block).DownLine);
	judgementForOneLine(leftpoints, false, (*p_block).LeftLine);
	judgementForOneLine(rightpoints, false, (*p_block).RightLine);
}
void BlockLocalizer::judgementForOneLine(vector<cv::Point>& points, bool updown, Block::Line& line)
{
	if (fixLineOnBorder(points, line) && points.size() >= 5)//�޸��߽�㣬��߽���ͼ���⣬��һ����ͼ��һ������ͼ��������
	{
		//�ж��Ƿ����
		judgemanBrokenLine(points, updown);

		//���ֱ��
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
	const int PIX_OFFSET = 20;//�������ֵ����Ϊ���ߡ�
	int count = points.size() - 2;//�������

	//ȷ��ÿ�����ļ��
	int spanX = 0;
	int spanY = 0;
	int iii = 0;
	//֮����ʹ��ѭ�����ǿ��ǵ��˵�һ�������ظ����������0 1�ŵ���ͬ����ôspanY=0
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

	//ѭ��������ڵ�֮�����������
	//�Ƚ�����������Ĳ����Ƿ������ֵPIX_OFFSET������������Ϊ�õ�Ϊת�۵㣬diffs��push��һ���µ���������
	vector<float> diffs;//��¼���ز�ж��ٸ�ת�۵��Ӧ���ж��ٸ����ݡ�/*����һ��ʼ�Ĳ�ֵһֱ��20��ͻȻת�۲�ֵ��30�����¼Ϊ20,30*/
	vector<int> diffsCount;//��Ӧ���ز����
	vector<int> errorPointIndex;//��¼ת�۵�����
	if (updown)
	{
		//������ڵ������λ�òdy
		vector<int> dy;
		for (int i = 0; i < count; i++)
		{
			if (abs(points[i].x - points[i + 1].x) == spanX)//���������ΪspanX
				dy.push_back(points[i].y - points[i + 1].y);//�������y���귽���ϵ�λ�ò�
		}
		diffs.push_back(dy[0]);
		diffsCount.push_back(1);

		for (int i = 0; i < dy.size(); i++)
		{
			float lastdiff = diffs[diffs.size() - 1];
			if (abs(lastdiff - dy[i]) < PIX_OFFSET)//��׼��ֵ��ȥ������ֵС����ֵ
			{
				diffs[diffs.size() - 1] = lastdiff * 0.7 + dy[i] * 0.3;//ʹ��һ��������ϵ�������±�׼��ֵ
				diffsCount[diffsCount.size() - 1] += 1;
			}
			else
			{
				//�ж��Ƿ��ǹ�����ȱ�ݵ㣬����ǹ�������continue������ż�¼Ϊ�����
				if (i > 5 && i < count - 6)//����λ��ש���м���������Ĵ���������Ǵ�ı���
				{
					if (abs(dy[i - 2] - dy[i + 2]) < PIX_OFFSET)//����õ�ǰ�������������ģ��������õ㡣
					{
						vector<cv::Point>::iterator it = points.begin() + i;
						points.erase(it);
						continue;
					}
				}

				diffs.push_back(dy[i]);
				diffsCount.push_back(1);
				errorPointIndex.push_back(i);
			}
		}
	}
	else if (!updown)
	{
		//�����һ���㵽�ڶ���������ؾ�����dx
		vector<int> dx;
		for (int i = 0; i < count; i++)
		{
			if (abs(points[i].y - points[i + 1].y) == spanY)
				dx.push_back(points[i].x - points[i + 1].x);
		}
		diffs.push_back(dx[0]);
		diffsCount.push_back(1);

		//diffsCount��¼ĳһdx������������
		//diffs[diffs.size() - 1]��¼��ǰ��dx
		for (int i = 0; i < dx.size(); i++)
		{
			float lastdiff = diffs[diffs.size() - 1];
			if (abs(lastdiff - dx[i]) < PIX_OFFSET)
			{
				diffs[diffs.size() - 1] = lastdiff * 0.7 + dx[i] * 0.3;//����Ȩ��
				diffsCount[diffsCount.size() - 1] += 1;
			}
			else
			{
				//�ж��Ƿ��ǹ�����ȱ�ݵ㣬����ǹ�������continue������ż�¼Ϊ�����
				if (i > 5 && i < count - 6)//����λ��ש���м���������Ĵ���������Ǵ�ı���
				{
					if (abs(dx[i - 2] - dx[i + 2]) < PIX_OFFSET)//����õ�ǰ�������������ģ��������õ㡣
					{
						vector<cv::Point>::iterator it = points.begin() + i;
						points.erase(it);
						continue;
					}
				}

				diffs.push_back(dx[i]);
				diffsCount.push_back(1);
				errorPointIndex.push_back(i);//��¼ת�۵�
			}
		}
	}
	//б��ȫ��һ��
	if (errorPointIndex.size() == 0)
	{
		return;
	}

	//���ȱ����Ϣ
	//�����������µ�length ��  deepֵ
	//errorPoint�ڱ�Ե���������
	if (errorPointIndex[0] < (points.size() / 3) || errorPointIndex[errorPointIndex.size() - 1] > (points.size() / 3 * 2))
	{
		Faults::BrokenEdge be;
		int l1 = updown ? abs(points[0].x - points[errorPointIndex[errorPointIndex.size() - 1]].x) : abs(points[0].y - points[errorPointIndex[errorPointIndex.size() - 1]].y);
		int d1 = !updown ? abs(points[0].x - points[errorPointIndex[errorPointIndex.size() - 1]].x) : abs(points[0].y - points[errorPointIndex[errorPointIndex.size() - 1]].y);
		int l2 = updown ? abs(points[points.size() - 1].x - points[errorPointIndex[0]].x) : abs(points[points.size() - 1].y - points[errorPointIndex[0]].y);
		int d2 = !updown ? abs(points[points.size() - 1].x - points[errorPointIndex[0]].x) : abs(points[points.size() - 1].y - points[errorPointIndex[0]].y);

		int indexOffset = l1 < l2 ? abs(errorPointIndex[errorPointIndex.size() - 1]) : abs((int)points.size() - 1 - errorPointIndex[0]);
		indexOffset /= 2;

		//����ˮƽ������ֱ�������length ��  deep
		be.length = l1 < l2 ? l1 : l2;
		be.deep = l1 < l2 ? d1 : d2;
		if (updown)
		{
			be.length_mm = be.length*p_block->Axis_X_mmPerPix;
			be.deep_mm = be.deep*p_block->Axis_Y_mmPerPix;
		}
		else
		{
			be.length_mm = be.length*p_block->Axis_Y_mmPerPix;
			be.deep_mm = be.deep*p_block->Axis_X_mmPerPix;
		}


		be.position.x = l1 < l2 ? abs(points[0].x + points[errorPointIndex[errorPointIndex.size() - 1]].x) / 2 : abs(points[points.size() - 1].x + points[errorPointIndex[0]].x) / 2;
		be.position.y = l1 < l2 ? abs(points[0].y + points[errorPointIndex[errorPointIndex.size() - 1]].y) / 2 : abs(points[points.size() - 1].y + points[errorPointIndex[0]].y) / 2;
		//be.position.x += updown ? be.length / 2 : be.deep;
		//be.position.y += updown ? be.deep / 2 : be.length;
		p_faults->BrokenEdges.push_back(be);

#ifdef BL_OUTPUT_DEBUG_INFO
		//debug��ͼ
		for (int i = 0; i < errorPointIndex.size(); i++)
		{
			cv::circle(drowDebugResult, points[errorPointIndex[i]], 10, cv::Scalar(255, 255, 0), -1);
		}
		cv::circle(drowDebugResult, be.position, 10, cv::Scalar(123, 255, 255), -1);
#endif
	}
	else
	{
		for (size_t i = 0; i < errorPointIndex.size(); i++)
		{
			Faults::BrokenEdge be;
			be.position = points[errorPointIndex[i]];
			be.deep = PIX_OFFSET;
			be.length = ROW_SPAN;
			p_faults->BrokenEdges.push_back(be);
#ifdef BL_OUTPUT_DEBUG_INFO
			//debug��ͼ
			cv::circle(drowDebugResult, be.position, 10, cv::Scalar(123, 255, 255), -1);
#endif
		}
	}
	BrokenEdgeFlag = true;
}
bool BlockLocalizer::fixLineOnBorder(vector<cv::Point>& points, Block::Line& line)
{
	//����ɾ����һ�������һ�㣬�ų�����
	points.pop_back();
	points.erase(points.begin());
	int borderX = img.cols - 1;
	int borderY = img.rows - 1;

	//��һ��������ҵ��ı߽��ȫ��ͼ��߽��ϵģ����жϱ߽���ͼ���⡣
	if ((points[0].x == 0 && points[1].x == 0 && points[points.size() - 1].x == 0 && points[points.size() - 2].x == 0) ||
		(points[0].x == borderX && points[1].x == borderX && points[points.size() - 1].x == borderX && points[points.size() - 2].x == borderX))
	{
		line.k = 999999;
		line.x0 = points[0].x;
		line.y0 = points[0].y;
		line.isAllOutOfImg = true;
		return false;
	}
	//��һ��������ҵ��ı߽��ȫ��ͼ��߽��ϵģ����жϱ߽���ͼ���⡣
	if ((points[0].y == 0 && points[1].y == 0 && points[points.size() - 1].y == 0 && points[points.size() - 2].y == 0) ||
		(points[0].y == borderY && points[1].y == borderY && points[points.size() - 1].y == borderY && points[points.size() - 2].y == borderY)
		)
	{
		line.k = 0;
		line.x0 = points[0].x;
		line.y0 = points[0].y;
		line.isAllOutOfImg = true;
		return false;
	}

	//�ڶ��������һ�����ͼ�ڣ�һ�����ͼ��߽硣��ȥ���߽��ϵĵ㡣
	vector<cv::Point> ::iterator pIter = points.begin();
	for (; pIter != points.end();) // ����
	{
		if (pIter->x == 0 || pIter->x == borderX || pIter->y == 0 || pIter->y == borderY)
		{
			pIter = points.erase(pIter);
			continue;
		}
		pIter++;
	}

	//��ɾ�����ʣ��������٣�������Ϊ��Ե��
	if (points.size() < 5)
	{
		line.x0 = points[0].x;
		line.y0 = points[0].y;
		if (line.x0 == 0 || line.x0 == borderX)
		{
			line.k = 0;
		} 
		if (line.y0 == 0 || line.y0 == borderY)
		{
			line.k = 999999;
		}
		return false;
		line.isAllOutOfImg = true;
	}
	return true;
}
