#include "MainHueAnalysis.h"


MainHueAnalysis::MainHueAnalysis(cv::Mat img)
{
	img.convertTo(img, CV_32F, 1.0 / 255);	//תΪ32λ�����ͽ��м���
	cv::Mat imageHSV;
	cv::cvtColor(img, imageHSV, CV_BGR2HSV);
	//ת����ɺ�hue��ΧΪ0-360��s��h 0-1


	//Ԥ���������ȵͻ򱥺Ͷȵ͵ĵ��hue����360��֮��

	float dropV = 0.08;		//�ͻҶ���ֵ�����ڴ˻Ҷ�ֵ�ľ���Ϊ��ɫ��
	float dropS = 0.05;		//�ͱ��Ͷ���ֵ�����ڴ˱��Ͷȵľ���Ϊ��ɫ��
	float dropV2 = 0.6;		//�ͻҶ���ֵ�����ڴ˻Ҷ�ֵ�ľ���Ϊ��ɫ��
	float dropS2 = 0.6;		//�ͱ��Ͷ���ֵ�����ڴ˱��Ͷȵľ���Ϊ��ɫ��

	for (int i = 0; i < imageHSV.rows; i++)
	{
		for (int j = 0; j < imageHSV.cols; j++)
		{
			//float *ptr =
			float s = imageHSV.ptr<float>(i)[j * 3 + 1];
			float v = imageHSV.ptr<float>(i)[j * 3 + 2];
			if (s < dropS || v < dropV)
				imageHSV.ptr<float>(i)[j * 3 + 0] = 400;//�����Ͷȵ͵����ص��Hueֵ��Ϊ400������ԭ��Χ[0��360]��
			else if (v > dropV2)
				imageHSV.ptr<float>(i)[j * 3 + 0] = 500;
		}
	}


	vector<cv::Mat> hsv;
	cv::split(imageHSV, hsv);
	cv::Mat h = hsv[0];
	cv::Mat s = hsv[1];
	cv::Mat v = hsv[2];

	cv::Mat hist = calcHist(h, 360);
	cv::Mat hist_img = drawHist(hist);


	int matTotalPixCount = 0;//����Ԥ�����һ�����ж��ٸ���
	for (size_t i = 0; i < hist.cols; i++)
	{
		matTotalPixCount += hist.ptr<long>(0)[i];
	}

	const int dx = 10;
	while (true)
	{
		auto maxIter = std::max_element(hist.begin<int>(), hist.end<int>());
		int maxV = *maxIter;

		int pos = maxIter.pos().x;
		int spanLeft = pos - dx;
		int spanRight = pos + dx;

		// �������ֵ���򸽽����ۻ�ֵ [6/6/2012 Isaac]
		float sum = getHistSum(hist, spanLeft, spanRight);
		sum /= matTotalPixCount;
		float lowPercentage = 0.01;
		if (sum >= lowPercentage)	//���ɫ������ռ��lowPercentage��������Ϊû��������ɫ����
		{
			(this->colorProperties).push_back(ColorProp(sum, pos, 0, 0));

			// Ȼ������������ [6/6/2012 Isaac]
			eraseHist(hist, spanLeft, spanRight);
		}
		else
			break;


	}

	// ���� [6/6/2012 Isaac]
	std::sort((this->colorProperties).begin(), (this->colorProperties).end(), [](const ColorProp& l, const ColorProp& r)->bool
	{
		return l.percentage>r.percentage;
	});



	vector<float> vS((this->colorProperties).size(), 0);
	vector<float> vV((this->colorProperties).size(), 0);
	vector<int> vCount((this->colorProperties).size(), 0);

	for (auto it = imageHSV.begin<cv::Vec3f>(); it != imageHSV.end<cv::Vec3f>(); ++it)	//����ͼ��
	{
		cv::Vec3f& vHSV = *it;
		//��ÿ���㿴����ɫ���Ƿ���ĳһ������Χ��
		for (int k = 0; k < (this->colorProperties).size(); k++)
		{
			float mainH = (this->colorProperties)[k].h;

			if (mainH - dx < 0)
			{
				if (vHSV[0] > 360 - (dx - mainH) || vHSV[0] < mainH + dx)
				{
					vS[k] += vHSV[1];
					vV[k] += vHSV[2];
					vCount[k]++;
					break;
				}
			}
			else if (mainH + dx > 360)
			{
				if (vHSV[0] > mainH - dx || vHSV[0] < dx - (360 - mainH))
				{
					vS[k] += vHSV[1];
					vV[k] += vHSV[2];
					vCount[k]++;
					break;
				}
			}
			else if (vHSV[0] > mainH - dx && vHSV[0] < mainH + dx)
			{
				vS[k] += vHSV[1];
				vV[k] += vHSV[2];
				vCount[k]++;
				break;
			}
		}
	}
	for (int i = 0; i<(this->colorProperties).size(); i++)
	{
		float finalS = vS[i] / vCount[i];	//����ƽ�����Ͷ�
		float finalV = vV[i] / vCount[i];	//����ƽ������
		(this->colorProperties)[i].s = finalS;
		(this->colorProperties)[i].v = finalV;
	}
}



MainHueAnalysis::~MainHueAnalysis()
{
}



cv::Mat MainHueAnalysis::drawHist(cv::Mat hist, int histHight)
{
	int hist_height = 1000;
	//int bins = 256;
	double max_val;  //ֱ��ͼ�����ֵ
	minMaxLoc(hist, 0, &max_val, 0, 0); //����ֱ��ͼ���ֵ

	cv::Mat hist_img = cv::Mat::zeros(hist_height, hist.cols, CV_8UC3); //����һ��ֱ��ͼͼ�񲢳�ʼ��Ϊ0 

	//cout << "max_val = " << max_val << endl;
	//��ֱ��ͼͼ����д��ֱ��ͼ����
	for (int i = 0; i < hist.cols; i++)
	{
		long bin_val = hist.at<long>(i); //	��i�Ҷȼ��ϵ���	
		int intensity = cvRound(bin_val*hist_height / max_val);  //Ҫ���Ƶĸ߶�  
		//����i�Ҷȼ�������
		rectangle(hist_img, cv::Point(i, hist_height - 1),
			cv::Point((i + 1) - 1, hist_height - intensity),
			CV_RGB(255, 255, 255));
	}

	//imshow("Hist Histogram2", hist_img);
	return hist_img;

}

//ͳ��ֱ��ͼ
//������ͳ�Ʋ�ɫֱ��ͼ����hImgΪ32λ������
//highΪֱ��ͼ��X�᷶Χ���ޣ�binΪX���ϵ���������
//areaΪ�����ͼ��ĸ����ȡֵ���޼�����
cv::Mat MainHueAnalysis::calcHist(cv::Mat& hImg, int bin)
{
	cv::Mat hist(1, bin, CV_32SC1);
	hist.setTo(0);

	//����float���м��㣬 ɫ�����ֵΪ360�� area = 360 - 0
	int high = 360;
	int area = 360;
	for_each(hImg.begin<float>(), hImg.end<float>(), [&](float pix)
	{
		if (pix <= high)	//������area��Χ�����ɫ��ֵ
		{
			int i = (int)(pix * bin / area + 0.5);
			i = (i == bin ? 0 : i);
			hist.at<int>(0, i)++;
		}
	});
	return hist;
}

//����ֱ��ͼMat��ĳ������εĺ�
//����histһάֱ��ͼmat
//leftIndex������꣬rightIndex�ұ�����  
int MainHueAnalysis::getHistSum(cv::Mat& hist, int leftIndex, int rightIndex)
{
	int matSize = hist.cols;
	int acc = 0;
	if (leftIndex < 0)
	{
		acc += getHistSum(hist, 0, rightIndex);
		acc += getHistSum(hist, matSize + leftIndex, matSize);
	}
	else if (rightIndex > matSize)
	{
		acc += getHistSum(hist, leftIndex, matSize);
		acc += getHistSum(hist, 0, rightIndex - matSize);
	}
	else
	{
		
		int sum = 0;
		for (size_t i = leftIndex; i < rightIndex; i++)
		{
			sum += hist.ptr<long>(0)[i];
		}
		return sum;
	}
	return acc;
}

void MainHueAnalysis::eraseHist(cv::Mat& hist, int leftIndex, int rightIndex)
{
	int matSize = hist.cols;
	if (leftIndex<0)
	{
		eraseHist(hist, 0, rightIndex);
		eraseHist(hist, matSize + rightIndex, matSize);
	}
	else if (rightIndex > matSize)
	{
		eraseHist(hist, leftIndex, matSize);
		eraseHist(hist, 0, rightIndex - matSize);
	}
	else
	{
		for (int i = leftIndex; i <= rightIndex; i++)
		{
			hist.ptr<long>(0)[i] = 0;
		}
	}
}
