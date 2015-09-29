// FeatureFinder.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "globle.h"


#include "Class\FeatureMaker.h"

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
			int histSize = 256;


			//cv::equalizeHist(f, f);
			cv::normalize(f, f, histSize, 0, cv::NORM_MINMAX);
			//cv::bitwise_xor(f, cv::Scalar(255), f);//反相

			FeatureMaker::GetGrayHist(f, grayHist, histSize);

			for (int j = 0; j < 256; j++)
			{
				trainingData[itemIndex][j] = grayHist.ptr<float>(j)[0];
			}
			itemIndex++;
		}
	}

	//创建一个网络
	cv::Mat layerSizes = (cv::Mat_<int>(1, 3) << featureCount, 25, sampleTypeCount);//创建一个featureCount输入  IDC_EDIT_YinCangCount隐藏  sampleTypeCount输出的三层网络


	CvANN_MLP_TrainParams param;
	param.term_crit = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 50000, 0.002);
	param.train_method = CvANN_MLP_TrainParams::BACKPROP;
	param.bp_dw_scale = 0.01;//权值更新率
	param.bp_moment_scale = 0.03;//权值更新冲量

	cv::Mat inputs(sampleAllCount, featureCount, CV_32FC1, trainingData);//样品总数，特征维数，储存的数据类型
	cv::Mat outputs(sampleAllCount, sampleTypeCount, CV_32FC1, outputData);

	bp.create(layerSizes, CvANN_MLP::SIGMOID_SYM);
	bp.train(inputs, outputs, cv::Mat(), cv::Mat(), param);
	bp.save("ANN_mlp.xml");

	itemIndex = 0;
	int zhengque = 0;
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
			if (outindex == index)
				zhengque++;
			cout << str_dir[index] << "_" << i << "：" << outindex << "->" << possibility << "->" << str_dir[outindex] << endl;
			itemIndex++;
		}
	}
	cout << "正确率" << ((double)zhengque / (double)sampleAllCount);
	return 0;
}

