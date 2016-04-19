#include "BlockEdgeLineDetector.h"


BlockEdgeLineDetector::BlockEdgeLineDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	if (image.channels() == 3)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	p_block = _block;
	p_faults = _faults;
}
void BlockEdgeLineDetector::Run()
{
#ifdef BED_OUTPUT_DEBUG_INFO
	drowDebugResult = image.clone();
	if (drowDebugResult.channels() == 1)
		cv::cvtColor(drowDebugResult, drowDebugResult, CV_GRAY2BGR);
	doUp();
	//doDown(); 
	//doLeft();
	//doRight();
#else
	thread t1 = thread(std::mem_fn(&BlockEdgeLineDetector::doUp), this);
	thread t2 = thread(std::mem_fn(&BlockEdgeLineDetector::doDown), this);
	thread t3 = thread(std::mem_fn(&BlockEdgeLineDetector::doLeft), this);
	thread t4 = thread(std::mem_fn(&BlockEdgeLineDetector::doRight), this);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
#endif
}

BlockEdgeLineDetector::~BlockEdgeLineDetector()
{
	p_faults = NULL;
	p_block = NULL;
}
void BlockEdgeLineDetector::doUp()
{
	int lengthmm = 600;//边长（毫米）
	Block::Line line = p_block->UpLine;
	vector<cv::Point> dubiousPoints;
	int startX = p_block->A.x + 5;
	if (startX < 0)
		startX = 0;
	int endX = p_block->B.x - 5;
	if (endX >= image.cols)
		endX = image.cols - 1;
	int inc = (float)(endX - startX) / lengthmm + 0.5;//范围增量

	//粗略查找崩边
	for (int point_x = startX; point_x < endX && point_x < image.cols; point_x += inc)
	{
		if (point_x < 0 || point_x >= image.cols)
			continue;

		int point_y = p_block->GetPonintByX(point_x, &line).y;
		if (point_y < 0 || point_y >= image.rows)
			continue;
		int deep = getDeepUp(cv::Point(point_x, point_y));
		if (deep > DEEP_THRESHOD)
			dubiousPoints.push_back(cv::Point(point_x, point_y));
	}

	//细找崩边，统计崩边长度
	int lastend = 0;//上一轮循环的终点
	int brokenEdgeIndex = -1;//用于表示是否正在一个崩边中，为-1时表示不再，否则为 p_faults->BrokenEdges的索引
	for (int i = 0; i < dubiousPoints.size(); i++)
	{
		int start = dubiousPoints[i].x - inc - inc + 1;

		if (start < lastend) 
			start = lastend + 1;
		else//如果崩边检测没有接上上一次的，则标记brokenEdgeIndex = -1
			brokenEdgeIndex = -1;

		if (start < startX)start = startX;

		int end = dubiousPoints[i].x + inc + inc - 1;
		if (end > endX)end = endX;
		lastend = end;

		for (int point_x = start; point_x <= end; point_x ++)
		{
			int point_y = p_block->GetPonintByX(point_x, &line).y;
			if (point_y < 0 || point_y >= image.rows)
				continue;
			int deep = getDeepUp(cv::Point(point_x, point_y));
			if (deep > DEEP_THRESHOD)
			{
#ifdef BED_OUTPUT_DEBUG_INFO
				drowDebugResult.ptr<uchar>(point_y)[point_x * 3 + 2] = 255;
#endif
				if (brokenEdgeIndex == -1)
				{
					Faults::BrokenEdge b;
					b.deep = deep;
					b.length = 1;
					b.position = cv::Point(point_x, point_y);
					p_faults->BrokenEdges.push_back(b);
					brokenEdgeIndex = p_faults->BrokenEdges.size() - 1;
				}
				else
				{
					if (p_faults->BrokenEdges[brokenEdgeIndex].deep < deep)
						p_faults->BrokenEdges[brokenEdgeIndex].deep = deep;
					p_faults->BrokenEdges[brokenEdgeIndex].length++;
					p_faults->BrokenEdges[brokenEdgeIndex].position = cv::Point(point_x - p_faults->BrokenEdges[brokenEdgeIndex].length / 2, point_y);
				}
			}
			else
			{
				//一个崩边缺陷的结束
				brokenEdgeIndex = -1;
			}
		}
	}
}
int BlockEdgeLineDetector::getDeepUp(cv::Point p)
{
	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 0 || point_y >= image.rows)
		return 0;

	int deep = 0;
	for (; deep < 50; deep++)
	{
		if (point_y + deep + 4 >= image.cols)
			break;
		if (image.ptr<uchar>(point_y + deep)[point_x] >= THRESHOD &&
			image.ptr<uchar>(point_y + deep + 1)[point_x] >= THRESHOD &&
			image.ptr<uchar>(point_y + deep + 2)[point_x] >= THRESHOD &&
			image.ptr<uchar>(point_y + deep + 3)[point_x] >= THRESHOD &&
			image.ptr<uchar>(point_y + deep + 4)[point_x] >= THRESHOD
			)
			break;
	}
	return deep;
}
void BlockEdgeLineDetector::doDown()
{
	const int ROI_WIDTH = 50;
	const int ROI_HEIGHT = 66;
	int inc = 25;//(float)(endX - startX) / 30 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//下边界
	int startX = p_block->D.x + 50;
	int endX = p_block->C.x - 50;
	for (int x = startX; x < endX && x < image.cols; x += inc, index++)
	{
		int x1 = x;
		if (x < 0 || x >= image.cols)
			continue;
		if ((x + ROI_WIDTH) >= endX)
			x1 = endX - ROI_WIDTH - 1;
		if ((x + ROI_WIDTH) >= image.cols)
			x1 = image.cols - ROI_WIDTH - 1;


		int y = p_block->GetPonintByX(x1, &p_block->DownLine).y;
		if (y < 0 || y >= image.rows)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x1, y - ROI_HEIGHT, ROI_WIDTH, ROI_HEIGHT)).clone();

		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_downs.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半

		reduceImg = reduceImg.t();
#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\D\\下_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\D\\下_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		tmpROI.release();

		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByX(x, &p_block->DownLine));
	}
	processUpDown(reduceList, points);
}
void BlockEdgeLineDetector::doLeft()
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
void BlockEdgeLineDetector::doRight()
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



void BlockEdgeLineDetector::processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points)
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

			cv::Mat percent = diffresult / reduceList[i];

			double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
			cv::Point maxLoc;
			minMaxLoc(percent, NULL, &maxVal, NULL, &maxLoc);
			if (maxVal > DIFF_THRESHOLD)
			{
				int count = 0;
				int x = maxLoc.x;
				//往右数
				for (int j = maxLoc.x; j < percent.cols; j++)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - x) > FAULTS_SPAN)
						break;
					//判断值是否大于阈值
					if (percent.ptr<float>(0)[j] > DIFF_THRESHOLD)
					{
						count++;
						x = j;
					}
				}
				x = maxLoc.x;
				//往左数
				for (int j = maxLoc.x; j >= 0; j--)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - x) > FAULTS_SPAN) break;
					//判断值是否大于阈值
					if (percent.ptr<float>(0)[j] > DIFF_THRESHOLD && abs((float)j - x) <= FAULTS_SPAN)
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
void BlockEdgeLineDetector::processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points)
{
#ifdef BED_OUTPUT_DEBUG_INFO
	for (size_t i = 0; i < points.size(); i++)
	{
		stringstream ss;
		ss << i;
		cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0));
		cv::circle(drowDebugResult, points[i], 2, cv::Scalar(255, 0, 255), -1);
	}
#endif

	vector<int> errorPointsIndex;
	vector<int> errorPointsDeep;
	if (reduceList.size() > 2)
	{
		for (int i = 0; i < reduceList.size() - 2; i++)
		{
			cv::Mat diffresult;
			cv::absdiff(reduceList[i], reduceList[i + 1], diffresult);
			cv::Mat percent = diffresult / reduceList[i];

			double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
			cv::Point maxLoc;
			minMaxLoc(percent, NULL, &maxVal, NULL, &maxLoc);
			if (maxVal > DIFF_THRESHOLD)
			{
				int count = 0;
				int y = maxLoc.y;
				for (int j = maxLoc.y; j < percent.rows; j++)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - y) > FAULTS_SPAN) break;
					//判断值是否大于阈值
					if (percent.ptr<float>(j)[0] > DIFF_THRESHOLD)
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
					if (percent.ptr<float>(j)[0] > DIFF_THRESHOLD && abs((float)j - j) <= FAULTS_SPAN)
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