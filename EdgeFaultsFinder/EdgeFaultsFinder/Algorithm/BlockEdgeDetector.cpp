#include "BlockEdgeDetector.h"


BlockEdgeDetector::BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	img = _img;
	block = _block;
	faults = _faults;
}


BlockEdgeDetector::~BlockEdgeDetector()
{
	faults = NULL;
	block = NULL;
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