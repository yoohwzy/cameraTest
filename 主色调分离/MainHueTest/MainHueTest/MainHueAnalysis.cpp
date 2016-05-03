#include "MainHueAnalysis.h"


MainHueAnalysis::MainHueAnalysis(cv::Mat img)
{
	img.convertTo(img, CV_32F, 1.0 / 255);	//转为32位浮点型进行计算
	cv::Mat imageHSV;
	cv::cvtColor(img, imageHSV, CV_BGR2HSV);
	//转换完成后hue范围为0-360，s、h 0-1


	//预处理，将亮度低或饱和度低的点的hue置于360°之外

	float dropV = 0.08;		//低灰度阈值，低于此灰度值的均视为黑色点
	float dropS = 0.05;		//低饱和度阈值，低于此饱和度的均视为灰色点
	float dropV2 = 0.6;		//低灰度阈值，低于此灰度值的均视为黑色点
	float dropS2 = 0.6;		//低饱和度阈值，低于此饱和度的均视为灰色点

	for (int i = 0; i < imageHSV.rows; i++)
	{
		for (int j = 0; j < imageHSV.cols; j++)
		{
			//float *ptr =
			float s = imageHSV.ptr<float>(i)[j * 3 + 1];
			float v = imageHSV.ptr<float>(i)[j * 3 + 2];
			if (s < dropS || v < dropV)
				imageHSV.ptr<float>(i)[j * 3 + 0] = 400;//将饱和度低的像素点的Hue值置为400（超出原范围[0，360]）
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


	int matTotalPixCount = 0;//经过预处理后一共还有多少个点
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

		// 计算最大值邻域附近的累积值 [6/6/2012 Isaac]
		float sum = getHistSum(hist, spanLeft, spanRight);
		sum /= matTotalPixCount;
		float lowPercentage = 0.01;
		if (sum >= lowPercentage)	//如果色调比例占到lowPercentage以下则认为没有其它主色调了
		{
			(this->colorProperties).push_back(ColorProp(sum, pos, 0, 0));

			// 然后清除这个区域 [6/6/2012 Isaac]
			eraseHist(hist, spanLeft, spanRight);
		}
		else
			break;


	}

	// 排序 [6/6/2012 Isaac]
	std::sort((this->colorProperties).begin(), (this->colorProperties).end(), [](const ColorProp& l, const ColorProp& r)->bool
	{
		return l.percentage>r.percentage;
	});



	vector<float> vS((this->colorProperties).size(), 0);
	vector<float> vV((this->colorProperties).size(), 0);
	vector<int> vCount((this->colorProperties).size(), 0);

	for (auto it = imageHSV.begin<cv::Vec3f>(); it != imageHSV.end<cv::Vec3f>(); ++it)	//遍历图像
	{
		cv::Vec3f& vHSV = *it;
		//对每个点看它的色调是否在某一主调范围内
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
		float finalS = vS[i] / vCount[i];	//计算平均饱和度
		float finalV = vV[i] / vCount[i];	//计算平均亮度
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
	double max_val;  //直方图的最大值
	minMaxLoc(hist, 0, &max_val, 0, 0); //计算直方图最大值

	cv::Mat hist_img = cv::Mat::zeros(hist_height, hist.cols, CV_8UC3); //创建一个直方图图像并初始化为0 

	//cout << "max_val = " << max_val << endl;
	//在直方图图像中写入直方图数据
	for (int i = 0; i < hist.cols; i++)
	{
		long bin_val = hist.at<long>(i); //	第i灰度级上的数	
		int intensity = cvRound(bin_val*hist_height / max_val);  //要绘制的高度  
		//填充第i灰度级的数据
		rectangle(hist_img, cv::Point(i, hist_height - 1),
			cv::Point((i + 1) - 1, hist_height - intensity),
			CV_RGB(255, 255, 255));
	}

	//imshow("Hist Histogram2", hist_img);
	return hist_img;

}

//统计直方图
//若用来统计彩色直方图，则hImg为32位浮点型
//high为直方图的X轴范围上限，bin为X轴上的区间数量
//area为传入的图像的各点的取值上限减下限
cv::Mat MainHueAnalysis::calcHist(cv::Mat& hImg, int bin)
{
	cv::Mat hist(1, bin, CV_32SC1);
	hist.setTo(0);

	//采用float进行计算， 色调最大值为360， area = 360 - 0
	int high = 360;
	int area = 360;
	for_each(hImg.begin<float>(), hImg.end<float>(), [&](float pix)
	{
		if (pix <= high)	//除开了area范围以外的色调值
		{
			int i = (int)(pix * bin / area + 0.5);
			i = (i == bin ? 0 : i);
			hist.at<int>(0, i)++;
		}
	});
	return hist;
}

//计算直方图Mat在某个区间段的和
//传入hist一维直方图mat
//leftIndex左边坐标，rightIndex右边坐标  
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
