#include "Processor.h"

// ������Ӧ��ֵ
int  Processor::otsuThreshold(Mat img)
{
	int T = 0;
	int height = img.rows;
	int width = img.cols;
	int Channels = img.channels();
	const uchar* data;

	double gSum0;//��һ��Ҷ���ֵ  
	double gSum1;//�ڶ���Ҷ���ֵ  
	double N0 = 0;//ǰ��������  
	double N1 = 0;//����������  
	double u0 = 0;//ǰ������ƽ���Ҷ�  
	double u1 = 0;//��������ƽ���Ҷ�  
	double w0 = 0;//ǰ�����ص���ռ����ͼ��ı���Ϊ��0  
	double w1 = 0;//�������ص���ռ����ͼ��ı���Ϊ��1  
	double u = 0;//��ƽ���Ҷ�  
	double tempg = -1;//��ʱ��䷽��  
	double g = -1;//��䷽��  
	double Histogram[256] = { 0 };// = new double[256];//�Ҷ�ֱ��ͼ  
	double N = width*height;//�������� 

	for (int i = 0; i < height; i++)
	{
		data = img.ptr<const uchar>(i);
		for (int j = 0; j < width; j++)
		{
			if (Channels == 3)
			{
				double temp = data[j * 3] * 0.114 + data[j * 3 + 1] * 0.587 + data[j * 3 + 2] * 0.299;
				temp = temp<0 ? 0 : temp;
				temp = temp>255 ? 255 : temp;
				Histogram[(int)temp]++;
			}
			else if (Channels == 1)
			{
				double temp = data[j];
				temp = temp<0 ? 0 : temp;
				temp = temp>255 ? 255 : temp;
				Histogram[(int)temp]++;
			}

		}
	}
	//������ֵ  
	for (int i = 0; i<256; i++)
	{
		gSum0 = 0;
		gSum1 = 0;
		N0 += Histogram[i];
		N1 = N - N0;
		if (0 == N1)break;//������ǰ�������ص�ʱ������ѭ��  
		w0 = N0 / N;
		w1 = 1 - w0;
		for (int j = 0; j <= i; j++)
		{
			gSum0 += j*Histogram[j];
		}
		u0 = gSum0 / N0;
		for (int k = i + 1; k<256; k++)
		{
			gSum1 += k*Histogram[k];
		}
		u1 = gSum1 / N1;
		//u = w0*u0 + w1*u1;  
		g = w0*w1*(u0 - u1)*(u0 - u1);
		if (tempg<g)
		{
			tempg = g;
			T = i;
		}
	}
	return T;
}


int  Processor::peak_valleyThreshold(Mat img)
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
	// ��ȷ�����Ӧ��ֵThresNum
	MatND histo;
	calcHist(&img, 1, channels, Mat(), histo, 1, histSize, ranges);
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
	return ThresNum;
}


Mat Processor::Binaryzation(Mat &image)
{
	int ThresNum = peak_valleyThreshold(image);
	
	// ��ֵ��
	threshold(image, image, ThresNum+8, 255, THRESH_BINARY);
	return image;
}

Mat Processor::Binaryzation_otsu(Mat &image)
{
	int ThresNum1 = peak_valleyThreshold(image);
	int ThresNum2 = otsuThreshold(image);
	imwrite("C:\\Users\\43597_000\\Desktop\\roi.jpg",image);
	int ThresNum = (ThresNum1 + ThresNum2) / 2;
	// ��ֵ��
	threshold(image, image, ThresNum, 255, THRESH_BINARY);
	return image;
}

Mat Processor::Gaussian_Blur(Mat &img)
{
	GaussianBlur(img, img, Size(9, 9), 0, 0, BORDER_DEFAULT);
	return img;
}