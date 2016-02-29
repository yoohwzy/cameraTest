#include "BlockEdgeDetector.h"


BlockEdgeDetector::BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	img = _img;
	p_block = _block;
	p_faults = _faults;

	doRight();
}


BlockEdgeDetector::~BlockEdgeDetector()
{
	p_faults = NULL;
	p_block = NULL;
}

void BlockEdgeDetector::doRight()
{
	const int ROI_WIDTH = 200;
	const int ROI_HEIGHT = 100;

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
	//ÓÒ±ß½ç
	int startY = p_block->A.y + 100;
	int endY = p_block->D.y - 100;
	int inc = 50;//(float)(endY - startY) / 60 + 0.5;//·¶Î§ÔöÁ¿
	for (int y = startY; y < endY && y < img.rows; y += inc, index++)
	{
		int x = p_block->GetPonintByY(y, &p_block->RightLine).x;
		if (x < 0 || x > img.cols)
			continue;

		cv::Mat tmpROI;
		if ((y + inc) > endY)
			x = p_block->GetPonintByY(endY - ROI_HEIGHT, &p_block->RightLine).x;

		tmpROI = img(cv::Rect(x - ROI_WIDTH, y, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::Mat reduceImg;

		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//¿íËõ¼õÎªÒ»°ë
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

#ifdef SAVE_IMG
		//±£´æÍ¼Æ¬
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\R\\ÓÒ_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\R\\ÓÒ_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		tmpROI.release();

		//reduceImg.convertTo(reduceImg, CV_64F);
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point3f(p_block->GetPonintByY(y, &p_block->RightLine).x, y, inc));
	}

	for (size_t i = 0; i < reduceList.size() - 2; i++)
	{
		cv::Mat result;
		cv::absdiff(reduceList[i], reduceList[i + 1], result);
		double max;
		cv::minMaxLoc(result, NULL, &max);
		continue;
	}
	//processAndSaveData(reduceList, points, "R\\ÓÒ");
}





void ImageBinarization(IplImage *src)
{
	int i, j, width, height, step, chanel, threshold;

	float size, avg, va, maxVa, p, a, s;
	unsigned char *dataSrc;
	float histogram[256];

	width = src->width;
	height = src->height;
	dataSrc = (unsigned char *)src->imageData;
	step = src->widthStep / sizeof(char);
	chanel = src->nChannels;

	for (i = 0; i<256; i++)
		histogram[i] = 0;
	for (i = 0; i<height; i++)
		for (j = 0; j<width*chanel; j++)
		{
			histogram[dataSrc[i*step + j] - '0' + 48]++;
		}
	size = width * height;
	for (i = 0; i<256; i++)
		histogram[i] /= size;

	avg = 0;
	for (i = 0; i<256; i++)
		avg += i*histogram[i];
	va = 0;
	for (i = 0; i<256; i++)
		va += fabs(i*i*histogram[i] - avg*avg);

	threshold = 20;
	maxVa = 0;
	p = a = s = 0;
	for (i = 0; i<256; i++)
	{
		p += histogram[i];
		a += i*histogram[i];
		s = (avg*p - a)*(avg*p - a) / p / (1 - p);
		if (s > maxVa)
		{
			threshold = i;
			maxVa = s;
		}
	}

	for (i = 0; i<height; i++)
		for (j = 0; j<width*chanel; j++)
		{
			if (dataSrc[i*step + j] > threshold)
				dataSrc[i*step + j] = 255;
			else
				dataSrc[i*step + j] = 0;
		}
}