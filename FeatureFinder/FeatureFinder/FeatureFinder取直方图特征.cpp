// FeatureFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"

#include "Class\FeatureMaker.h"

#include <direct.h>

//检查文件夹是否存在，不存在则创建之
//文件夹存在返回 0
//文件夹创建失败返回-1
//文件夹创建失败返回1
int CheckDir(char* Dir)
{
	FILE *fp = NULL;
	char TempDir[200];
	memset(TempDir, '\0', sizeof(TempDir));
	sprintf(TempDir, Dir);
	strcat(TempDir, "\\");
	strcat(TempDir, ".temp.fortest");
	fp = fopen(TempDir, "w");
	if (!fp)
	{
		if (_mkdir(Dir) == 0)
		{
			return 1;//文件夹创建成功
		}
		else
		{
			return -1;//can not make a dir;
		}
	}
	else
	{
		fclose(fp);
	}
	return 0;
}


int main()
{
	const int sampleTypeCount = 7;				//共有几种字体
	const int sampleCount = 80;					//每种字体的样本数
	const int sampleAllCount = sampleCount*sampleTypeCount;
	const int featureCount = 256 + 2;				//特征维数 histSize + 2

	const int histSize = 255;

	CvANN_MLP bp;// = CvANN_MLP(layerSizes,CvANN_MLP::SIGMOID_SYM,1,1);


	string str_dir[sampleTypeCount];
	str_dir[0] = "A水滴渍";
	str_dir[1] = "B水纹";
	str_dir[2] = "C指纹";
	str_dir[3] = "D釉面凹凸";
	str_dir[4] = "X凹点";
	str_dir[5] = "Y杂质";
	str_dir[6] = "Z划痕";
	float trainingData[sampleAllCount][featureCount] = { 0 };
	float outputData[sampleAllCount][sampleTypeCount] = { 0 };

	int itemIndex = 0;
	for (int index = 0; index < sampleTypeCount; index++)
	{
		char filename[255]; // 此处写入文件名 
		strcpy(filename, ("特征样本库\\" + str_dir[index] + "_stdDev.txt").c_str());
		ofstream fout(filename);
		fout << "序号" << "			" << "图像平均值" << "		" << "图像标准差" << "		" << "hist平均值" << "		" << "hist标准差" << endl;

		for (int i = 1; i <= sampleCount; i++)
		{
			cout << str_dir[index] << "_" << i << endl;
			stringstream ss;
			char num[4];
			sprintf(num, "%03d", i);
			ss << "特征样本库\\" << str_dir[index] << "\\" << num << ".jpg";
			string path;
			ss >> path;

			//读取灰度图像以便计算灰度直方图
			cv::Mat f = cv::imread(path, 0);
			cv::Mat grayHist;

			//cv::equalizeHist(f, f);
			//cv::normalize(f, f, histSize, 0, cv::NORM_MINMAX);
			//cv::bitwise_xor(f, cv::Scalar(255), f);//反相

			FeatureMaker::GetGrayHist(f, grayHist, histSize);
			double avg = 0, stdDev = 0;
			FeatureMaker::GetGrayAvgStdDev(f, avg, stdDev);

			float histAvg = 0;
			int histCount = 0;
			for (int j = 0; j < histSize; j++)
			{
				if (grayHist.ptr<float>(j)[0] != 0)
				{
					histCount++;
					histAvg += grayHist.ptr<float>(j)[0];
				}
				trainingData[itemIndex][j] = grayHist.ptr<float>(j)[0];
			}
			float histStdDev = 0;
			if (histCount != 0)
			{
				histAvg = histAvg / histCount;
				for (int j = 0; j < histSize; j++)
				{
					if (grayHist.ptr<float>(j)[0] != 0)
					{
						histStdDev = (grayHist.ptr<float>(j)[0] - histAvg)*(grayHist.ptr<float>(j)[0] - histAvg);
					}
				}
				histStdDev = sqrt(histStdDev / histCount);
			}
			fout << i << "			" << avg << "			" << stdDev << "			" << histAvg << "			" << histStdDev << endl;
			itemIndex++;

			/*
			// 创建直方图画布
			int hist_w = 400; int hist_h = 400;
			int bin_w = cvRound((double)hist_w / histSize);

			cv::Mat histImage(hist_w, hist_h, CV_8UC3, cv::Scalar(0, 0, 0));

			/// 在直方图画布上画出直方图
			for (int i = 1; i < histSize; i++)
			{
				line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(grayHist.at<float>(i - 1))),
					cv::Point(bin_w*(i), hist_h - cvRound(grayHist.at<float>(i))),
					cv::Scalar(0, 0, 255), 2, 8, 0);
			}
			*/

			/*stringstream s;
			//s << "samples\\反相正规化直方图\\" << str_dir[index] << "\\";
			s << "samples\\正规化直方图\\" << str_dir[index] << "\\";
			//s << "samples\\均衡化直方图\\" << str_dir[index] << "\\";
			//s << "samples\\直方图\\" << str_dir[index] << "\\";
			//string dir = s.str();
			//char* c;
			//int len = dir.length();
			//c = new char[len + 1];
			//strcpy(c, dir.c_str());
			//CheckDir(c);
			s << "" << num << ".jpg";
			s >> path;

			cv::imwrite(path, histImage);

			s.clear();
			//s << "samples\\反相正规化直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			s << "samples\\正规化直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\均衡化直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			s >> path;
			cv::imwrite(path, grayHist);*/

			/// 显示直方图
			//cv::namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
			//cv::imshow("calcHist Demo", histImage);
			//cv::waitKey(0);
		}
	}

	return 0;
}

