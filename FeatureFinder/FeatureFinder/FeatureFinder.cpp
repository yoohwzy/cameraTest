// FeatureFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"


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
	const int sampleCount = 50;					//每种字体的样本数
	const int sampleAllCount = sampleCount*sampleTypeCount;
	const int featureCount = 256;				//特征维数
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
	for (int index = 0; index < 7; index++)
	{
		for (int i = 1; i <= 50; i++)
		{
			outputData[itemIndex][index] = 1;

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

			// 设定bin数目，也就是灰度级别，这里选择的是0-255灰度
			int histSize = 63;


			//cv::equalizeHist(f, f);
			cv::normalize(f, f, histSize, 0, cv::NORM_MINMAX);
			//cv::bitwise_xor(f, cv::Scalar(255), f);//反相


			// 设定取值范围，设定每级灰度的范围。
			float range[] = { 0, 255 };
			const float* histRange = { range };
			bool uniform = true; bool accumulate = false;
			cv::calcHist(&f, 1, 0, cv::Mat(), grayHist, 1, &histSize, &histRange, uniform, accumulate);

			for (int j = 0; j < 256; j++)
			{
				trainingData[itemIndex][j] = grayHist.ptr<float>(0)[0];
			}
			itemIndex++;
			/*
			// 创建直方图画布
			int hist_w = 400; int hist_h = 400;
			int bin_w = cvRound((double)hist_w / histSize);

			cv::Mat histImage(hist_w, hist_h, CV_8UC3, cv::Scalar(0, 0, 0));

			/// 将直方图归一化到范围 [ 0, histImage.rows ]
			cv::normalize(grayHist, grayHist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

			/// 在直方图画布上画出直方图
			for (int i = 1; i < histSize; i++)
			{
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(grayHist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(grayHist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
			}

			stringstream s;
			s << "samples\\反相正规化直方图\\" << str_dir[index] << "\\";
			//s << "samples\\正规化直方图\\" << str_dir[index] << "\\";
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
			s << "samples\\反相正规化直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\正规化直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\均衡化直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\直方图\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			s >> path;
			cv::imwrite(path, grayHist);

			/// 显示直方图
			//cv::namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
			//cv::imshow("calcHist Demo", histImage);
			//cv::waitKey(0);
			*/
		}
	}

	//创建一个网络
	cv::Mat layerSizes = (cv::Mat_<int>(1, 3) << featureCount, 25, sampleTypeCount);//创建一个featureCount输入  IDC_EDIT_YinCangCount隐藏  sampleTypeCount输出的三层网络


	CvANN_MLP_TrainParams param;
	param.term_crit = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 5000, 0.01);
	param.train_method = CvANN_MLP_TrainParams::BACKPROP;
	param.bp_dw_scale = 0.2;
	param.bp_moment_scale = 0.1;

	cv::Mat inputs(sampleAllCount, featureCount, CV_32FC1, trainingData);//样品总数，特征维数，储存的数据类型
	cv::Mat outputs(sampleAllCount, sampleTypeCount, CV_32FC1, outputData);

	bp.create(layerSizes, CvANN_MLP::SIGMOID_SYM);
	bp.train(inputs, outputs, cv::Mat(), cv::Mat(), param);
	bp.save("ANN_mlp.xml");

	itemIndex = 0;
	for (int index = 0; index < 7; index++)
	{
		for (int i = 1; i <= 50; i++)
		{
			cv::Mat sampleMat(1, featureCount, CV_32FC1, trainingData[itemIndex]);//样品总数，特征维数，储存的数据类型
			cv::Mat nearest(1, sampleTypeCount, CV_32FC1, cv::Scalar(0));
			bp.predict(sampleMat, nearest);
			float possibility = -1;
			int outindex = 0;
			for (int i = 0; i < nearest.size().width; i++){
				float x = nearest.at<float>(0, i);
				if (x>possibility){
					possibility = x;
					outindex = i;
				}
			}
			cout << str_dir[index] << "_" << i << "：" << outindex << "->" << possibility << "->" << str_dir[outindex] << endl;
			itemIndex++;

		}
	}

	return 0;
}

