#include "Processor.h"

Mat Processor::Binaryzation(Mat &image)
{
	int histSize[1];
	float hranges[2];
	const float *ranges[1];
	int channels[1];
	histSize[0] = 256;
	hranges[0] = 0;
	hranges[1] = 255.0;
	ranges[0] = hranges;
	channels[0] = 0;
// 峰谷法自适应阈值ThresNum
	MatND histo;
	calcHist(&image, 1, channels, Mat(), histo, 1, histSize, ranges);
	vector<Point> num;
	Point histopoint;
	int ThresNum = 0;
	for (int i = 1; i < 256; i++)
	{
		if (histo.at<float>(i) != 0)
		{
			histopoint.x = int(histo.at<float>(i));
			histopoint.y = i;
			num.push_back(histopoint);
		}
	}

	for (int i = 1; i < num.size(); i++)
	{
		if (num[i].x < num[i - 1].x)
		if (num[i].x < num[i + 1].x)
		{
			ThresNum = num[i].y;
			break;
		}
	}
	// 二值化
	threshold(image, image, ThresNum, 255, THRESH_BINARY);
	return image;
}

Mat Processor::Gaussian_Blur(Mat &img)
{
	GaussianBlur(img, img, Size(9, 9), 0, 0, BORDER_DEFAULT);
	return img;
}