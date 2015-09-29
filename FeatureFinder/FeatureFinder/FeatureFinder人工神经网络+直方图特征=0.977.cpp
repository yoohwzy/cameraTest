// FeatureFinder.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"


#include "Class\FeatureMaker.h"

int main()
{
	const int sampleTypeCount = 7;				//���м�������
	const int sampleCount = 50;					//ÿ�������������
	const int sampleAllCount = sampleCount*sampleTypeCount;
	const int featureCount = 256;				//����ά��
	CvANN_MLP bp;// = CvANN_MLP(layerSizes,CvANN_MLP::SIGMOID_SYM,1,1);


	string str_dir[sampleTypeCount];
	str_dir[0] = "Aˮ����";
	str_dir[1] = "Bˮ��";
	str_dir[2] = "Cָ��";
	str_dir[3] = "D���氼͹";
	str_dir[4] = "X����";
	str_dir[5] = "Y����";
	str_dir[6] = "Z����";

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
			ss << "����������\\" << str_dir[index] << "\\" << num << ".jpg";
			string path;
			ss >> path;
			//��ȡ�Ҷ�ͼ���Ա����Ҷ�ֱ��ͼ
			cv::Mat f = cv::imread(path, 0);


			cv::Mat grayHist;

			// �趨bin��Ŀ��Ҳ���ǻҶȼ�������ѡ�����0-255�Ҷ�
			int histSize = 256;


			//cv::equalizeHist(f, f);
			cv::normalize(f, f, histSize, 0, cv::NORM_MINMAX);
			//cv::bitwise_xor(f, cv::Scalar(255), f);//����

			FeatureMaker::GetGrayHist(f, grayHist, histSize);

			for (int j = 0; j < 256; j++)
			{
				trainingData[itemIndex][j] = grayHist.ptr<float>(j)[0];
			}
			itemIndex++;
		}
	}

	//����һ������
	cv::Mat layerSizes = (cv::Mat_<int>(1, 3) << featureCount, 25, sampleTypeCount);//����һ��featureCount����  IDC_EDIT_YinCangCount����  sampleTypeCount�������������


	CvANN_MLP_TrainParams param;
	param.term_crit = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 50000, 0.002);
	param.train_method = CvANN_MLP_TrainParams::BACKPROP;
	param.bp_dw_scale = 0.01;//Ȩֵ������
	param.bp_moment_scale = 0.03;//Ȩֵ���³���

	cv::Mat inputs(sampleAllCount, featureCount, CV_32FC1, trainingData);//��Ʒ����������ά�����������������
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
			cv::Mat sampleMat(1, featureCount, CV_32FC1, trainingData[itemIndex]);//��Ʒ����������ά�����������������
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
			cout << str_dir[index] << "_" << i << "��" << outindex << "->" << possibility << "->" << str_dir[outindex] << endl;
			itemIndex++;
		}
	}
	cout << "��ȷ��" << ((double)zhengque / (double)sampleAllCount);
	return 0;
}

