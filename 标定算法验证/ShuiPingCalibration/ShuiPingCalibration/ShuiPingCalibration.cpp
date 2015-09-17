// ShuiPingCalibration.cpp : �������̨Ӧ�ó������ڵ㡣
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
	uchar* p = img_threshold.ptr<uchar>(0);//ÿ�е���ʼ��ַ
	uchar* op = img.ptr<uchar>(0);//ÿ�е���ʼ��ַ
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
	//ȷ���м��೤�ȣ���ΪУ���õı�׼���ȣ�
	int standard_length = Xs[b] - Xs[a];
	std::cout << "a = " << a << " b = " << b << endl;


	//ȷ��ʵ�ʵ�Ӧ��������λ��
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

	vector<double> map(4096, -1);//��Ƭӳ�䵽ʵ�� ����4096����ʾ��ƬX���ϣ�ÿ���㾭�궨��Ӧ��λ���ĸ�λ�á�

	//����У��
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
	//����У��
	for (size_t i = Xs[a]; i < Xs[b]; i++)
	{
		map[i] = i;
	}



	vector<double> map_anti(4096, -1);//ʵ��ӳ�䵽��Ƭ������ȷ��ʵ��Xλ�ô������ص���ɣ�
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
	int image_rows = calibrat.rows;//����
	int elementCount = calibrat.cols;//ÿ��Ԫ����
	for (int i = 0; i < image_rows; i++)
	{
		uchar* linehead = calibrat.ptr<uchar>(i);//ÿ�е���ʼ��ַ
		uchar* oldhead = img.ptr<uchar>(i);//ÿ�е���ʼ��ַ
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

