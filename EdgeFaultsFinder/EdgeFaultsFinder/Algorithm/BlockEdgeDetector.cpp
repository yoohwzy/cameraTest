#include "BlockEdgeDetector.h"


BlockEdgeDetector::BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	p_block = _block;
	p_faults = _faults;

	doUp();
	doRight();
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
	vector<cv::Point3f> points;

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
		points.push_back(cv::Point3f(x, p_block->GetPonintByX(x, &p_block->UpLine).y, inc));
	}
	//processAndSaveData(reduceList, points, "U\\上");
}

void BlockEdgeDetector::doDown()
{
	const int ROI_WIDTH = 100;
	const int ROI_HEIGHT = 200;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
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
		points.push_back(cv::Point3f(x, p_block->GetPonintByX(x, &p_block->DownLine).y, inc));
	}
	//processAndSaveData(reduceList, points, "D\\下");
}

void BlockEdgeDetector::doLeft()
{
	const int ROI_WIDTH = 200;
	const int ROI_HEIGHT = 100;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
	//左边界
	int startY = p_block->A.y + 100;
	int endY = p_block->D.y - 100;
	int inc = 50;//(float)(endY - startY) / 60 + 0.5;//范围增量
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
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
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

		tmpROI.release();

		//reduceImg.convertTo(reduceImg, CV_32F);
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point3f(p_block->GetPonintByY(y, &p_block->LeftLine).x, y, inc));

	}
	//processAndSaveData(reduceList, points, "L\\左");
}
void BlockEdgeDetector::doRight()
{
	const int ROI_WIDTH = 200;
	const int ROI_HEIGHT = 100;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
	//右边界
	int startY = p_block->A.y + 100;
	int endY = p_block->D.y - 100;
	int inc = 50;//(float)(endY - startY) / 60 + 0.5;//范围增量
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
		points.push_back(cv::Point3f(p_block->GetPonintByY(y, &p_block->RightLine).x, y, inc));
	}
	processLeftRight(reduceList, points);
}

int BlockEdgeDetector::processLeftRight(vector<cv::Mat> reduceList, vector<cv::Point3f> points)
{
	double maxdiff = 0;
	vector<cv::Point3f> errorPoints;
	for (int i = 0; i < reduceList.size() - 2; i++)
	{
		cv::Mat result;
		cv::absdiff(reduceList[i], reduceList[i + 1], result);
		double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
		cv::Point maxLoc;
		minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
		if (maxdiff < maxVal)
			maxdiff = maxVal;
		if (maxVal > JUDGEMENT_THRESHOLD)
		{
			int count = 0;
			int x = maxLoc.x;
			for (int j = maxLoc.x; j < result.cols; j++)
			{
				//判断新点到上一错误点的距离
				if (abs((float)j - x) > FAULTS_SPAN)
					break;
				//判断值是否大于阈值
				if (result.ptr<float>(0)[j] > JUDGEMENT_THRESHOLD)
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
				if (result.ptr<float>(0)[j] > JUDGEMENT_THRESHOLD && abs((float)j - x) <= FAULTS_SPAN)
				{
					count++;
					x = j;
				}
			}


			if (count > FAULTS_COUNT)
				errorPoints.push_back(points[i]);
		}
		continue;
	}
	return maxdiff;
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