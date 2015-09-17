// ShuiPingCalibration.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h" 

using namespace cv;

int _tmain(int argc, _TCHAR* argv[])
{
	cv::Mat img = imread("1.png");
	cv::Mat img_threshold;
	cv::cvtColor(img, img_threshold, CV_RGB2GRAY);
	threshold(img_threshold, img_threshold, 128, 255, THRESH_BINARY);
	vector<int> Xs;
	uchar* p = img_threshold.ptr<uchar>(0);//每行的起始地址
	uchar* op = img.ptr<uchar>(0);//每行的起始地址
	for (int i = 1; i < img_threshold.cols; i++)
	{
		if (abs(p[i] - p[i - 1]) > 200)
		{
			op[i * 3] = 255;
			circle(img, cv::Point(i,20), 5, Scalar(0, 255, 0), -1);
			Xs.push_back(i);
		}
	}


	int a = Xs.size() / 2;
	int b = a + 1;
	//确定中间间距长度（作为校正用的标准长度）
	int standard_length = Xs[b] - Xs[a];
	std::cout << "a = " << a << " b = " << b << endl;


	//确认实际点应该所处的位置
	vector<int> Xs_standard(Xs.size(), -1);
	for (int i = a; i >= 0; i--)
	{
		if (i == a)
			Xs_standard[i] = Xs[i];
		else
		{
			Xs_standard[i] = Xs_standard[i + 1] - standard_length;
		}
	}
	for (int i = b; i < Xs.size(); i++)
	{
		if (i == b)
			Xs_standard[i] = Xs[i];
		else
		{
			Xs_standard[i] = Xs_standard[i - 1] + standard_length;
		}
	}

	vector<double> map(4096, -1);//照片映射到实际 长度4096，表示照片X轴上，每个点经标定后应该位于哪个位置。

	//向右校正
	for (int i = 0; i < Xs.size() - 1; i++)
	{
		int a1 = Xs[i];
		int a2 = Xs[i + 1];
		int aa1 = Xs_standard[i];
		int aa2 = Xs_standard[i + 1];
		double k = standard_length / (double)(a2 - a1);
		double b = aa2 - k * a2;
		if ((i + 1) == (Xs.size() - 1))
			a2 = 4095;
		for (int j = a1; j <= a2; j++)
		{
			double xx = k * j + b;
			map[j] = xx;
		}
	}
	//无需校正
	for (size_t i = Xs[a]; i < Xs[b]; i++)
	{
		map[i] = i;
	}



	vector<double> map_anti(4096, -1);//实际映射到照片（用以确定实际X位置处，像素的组成）
	for (size_t i = 0; i < Xs.size() - 2; i++)
	{
		int a1 = Xs[i];
		int a2 = Xs[i + 1];
		int aa1 = Xs_standard[i];
		int aa2 = Xs_standard[i + 1];
		double k = standard_length / (double)(a2 - a1);
		double b = aa2 - k * a2;
		for (size_t j = aa1; j < aa2; j++)
		{
			double x = (j - b) / k;
			map_anti[j] = x;
		}
	}

	cv::Mat calibrat(cv::Size(img.cols, img.rows), CV_8UC3, cv::Scalar(0, 0, 0));
	int image_rows = calibrat.rows;//行数
	int elementCount = calibrat.cols;//每行元素数
	for (int i = 0; i < image_rows; i++)
	{
		uchar* linehead = calibrat.ptr<uchar>(i);//每行的起始地址
		uchar* oldhead = img.ptr<uchar>(i);//每行的起始地址
		for (int j = 0; j < elementCount; j++)
		{
			if (map_anti[j] < 0)
				continue;
			else
			{
				unsigned int pos1 = map_anti[j];
				unsigned int pos2 = pos1 + 1;
				double percent1 = map_anti[j] - pos1;
				double percent2 = 1 - percent1;
				if (pos1 >= 4096 && pos2 >= 4096)
					continue;
				linehead[j * 3 + 0] = oldhead[pos1 * 3 + 0] * percent1 + oldhead[pos2 * 3 + 0] * percent2;
				linehead[j * 3 + 1] = oldhead[pos1 * 3 + 1] * percent1 + oldhead[pos2 * 3 + 1] * percent2;
				linehead[j * 3 + 2] = oldhead[pos1 * 3 + 2] * percent1 + oldhead[pos2 * 3 + 2] * percent2;
			}
		}
	}
	//int aaa[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	//int **p;
	//*p = a;
	//int a11 = p[1];
	////int a12 = *(*(aaa+1));
	//int a13 = *(&a[1]);
	//int a14 = p[1];

	cv::imshow("1", img);
	cv::imwrite("2.jpg", calibrat);
	cv::imshow("2", calibrat);

	cv::waitKey(0);
	return 0;
}

