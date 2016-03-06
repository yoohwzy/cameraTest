#include "BlockEdgeDetector.h"


BlockEdgeDetector::BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	if (image.channels() == 3)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	p_block = _block;
	p_faults = _faults;

#ifdef BED_OUTPUT_DEBUG_INFO
	drowDebugResult = _img.clone();
	if (drowDebugResult.channels() == 1)
		cv::cvtColor(drowDebugResult, drowDebugResult, CV_GRAY2BGR);
#endif
	thread t1 = thread(std::mem_fn(&BlockEdgeDetector::doUp), this);
	thread t2 = thread(std::mem_fn(&BlockEdgeDetector::doDown), this);
	thread t3 = thread(std::mem_fn(&BlockEdgeDetector::doLeft), this);
	thread t4 = thread(std::mem_fn(&BlockEdgeDetector::doRight), this);
	t1.join();
	t2.join();
	t3.join();
	t4.join();

	//doUp();
}


BlockEdgeDetector::~BlockEdgeDetector()
{
	p_faults = NULL;
	p_block = NULL;
}
void BlockEdgeDetector::doUp()
{
	const int ROI_WIDTH = 100;
	const int ROI_HEIGHT = 200;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;

	//上边界
	int startX = p_block->A.x + 50;
	int endX = p_block->B.x - 50;
	int inc = 50;//(float)(endX - startX) / 30 + 0.5;//范围增量
	for (int x = startX; x < endX && x < image.cols; x += inc, index++)
	{
		int y = p_block->GetPonintByX(x, &p_block->UpLine).y;
		if (x < 0 || x >= image.cols)
			continue;
		if (y < 0 || y >= image.rows)
			continue;

		if ((x + inc) > endX)
			y = p_block->GetPonintByX(endX - ROI_WIDTH, &p_block->UpLine).y;

		cv::Mat tmpROI = image(cv::Rect(x, y, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_ups.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\U\\上_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\U\\上_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif
		tmpROI.release();

		reduceImg = reduceImg.t();
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point(x, p_block->GetPonintByX(x, &p_block->UpLine).y));
	}
	processUpDown(reduceList, points);
}
void BlockEdgeDetector::doDown()
{
	const int ROI_WIDTH = 100;
	const int ROI_HEIGHT = 200;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//下边界
	int startX = p_block->D.x + 50;
	int endX = p_block->C.x - 50;
	int inc = 50;//(float)(endX - startX) / 30 + 0.5;//范围增量
	for (int x = startX; x < endX && x < image.cols; x += inc, index++)
	{
		int y = p_block->GetPonintByX(x, &p_block->DownLine).y;
		if (x < 0 || x >= image.cols)
			continue;
		if (y < 0 || y >= image.rows)
			continue;

		if ((x + inc) > endX)
			y = p_block->GetPonintByX(endX - ROI_WIDTH, &p_block->DownLine).y;
		cv::Mat tmpROI = image(cv::Rect(x, y - ROI_HEIGHT, ROI_WIDTH, ROI_HEIGHT)).clone();

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
	processLeftRight(reduceList, points);
}
void BlockEdgeDetector::doLeft()
{
	const int ROI_WIDTH = 200;
	const int ROI_HEIGHT = 40;
	int inc = 30;//(float)(endY - startY) / 60 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//左边界
	int startY = p_block->A.y + 150;
	int endY = p_block->D.y - 150;
	for (int y = startY; y < endY && y < image.rows; y += inc, index++)
	{
		int x = p_block->GetPonintByY(y, &p_block->LeftLine).x;
		if (x < 0 || x >= image.cols)
			continue;
		if (y < 0 || y >= image.rows)
			continue;

		cv::Mat tmpROI;
		if ((y + inc) > endY)
			x = p_block->GetPonintByY(endY - ROI_HEIGHT, &p_block->LeftLine).x;

		tmpROI = image(cv::Rect(x, y, 200, inc)).clone();
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
void BlockEdgeDetector::doRight()
{
	const int ROI_WIDTH = 200;
	const int ROI_HEIGHT = 40;
	int inc = 30;//(float)(endY - startY) / 60 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//右边界
	int startY = p_block->B.y + 150;
	int endY = p_block->D.y - 150;
	for (int y = startY; y < endY && y < image.rows; y += inc, index++)
	{
		int x = p_block->GetPonintByY(y, &p_block->RightLine).x;
		if (x < 0 || x >= image.cols)
			continue;
		if (y < 0 || y >= image.rows)
			continue;

		if ((y + inc) > endY)
			x = p_block->GetPonintByY(endY - ROI_HEIGHT, &p_block->RightLine).x;

		cv::Mat tmpROI = image(cv::Rect(x - ROI_WIDTH, y, ROI_WIDTH, ROI_HEIGHT)).clone();
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



void BlockEdgeDetector::processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point> points)
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
			cv::Mat diffresult;
			cv::absdiff(reduceList[i], reduceList[i + 1], diffresult);
			double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
			cv::Point maxLoc;
			minMaxLoc(diffresult, NULL, &maxVal, NULL, &maxLoc);
			if (maxdiff_X < maxVal) maxdiff_X = maxVal;//定标用统计
			if (maxVal > JUDGEMENT_THRESHOLD)
			{
				int count = 0;
				int x = maxLoc.x;
				for (int j = maxLoc.x; j < diffresult.cols; j++)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - x) > FAULTS_SPAN)
						break;
					//判断值是否大于阈值
					if (diffresult.ptr<float>(0)[j] > JUDGEMENT_THRESHOLD)
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
					if (diffresult.ptr<float>(0)[j] > JUDGEMENT_THRESHOLD && abs((float)j - x) <= FAULTS_SPAN)
					{
						count++;
						x = j;
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
			}
		}
	}
}
void BlockEdgeDetector::processUpDown(vector<cv::Mat> reduceList, vector<cv::Point> points)
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
			double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
			cv::Point maxLoc;
			minMaxLoc(diffresult, NULL, &maxVal, NULL, &maxLoc);
			if (maxdiff_Y < maxVal) maxdiff_Y = maxVal;//定标用统计
			if (maxVal > JUDGEMENT_THRESHOLD)
			{
				int count = 0;
				int y = maxLoc.y;
				for (int j = maxLoc.y; j < diffresult.rows; j++)
				{
					//判断新点到上一错误点的距离
					if (abs((float)j - y) > FAULTS_SPAN) break;
					//判断值是否大于阈值
					if (diffresult.ptr<float>(j)[0] > JUDGEMENT_THRESHOLD)
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
					if (diffresult.ptr<float>(j)[0] > JUDGEMENT_THRESHOLD && abs((float)j - j) <= FAULTS_SPAN)
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