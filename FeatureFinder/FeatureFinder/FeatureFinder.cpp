// FeatureFinder.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"


#include <direct.h>

//����ļ����Ƿ���ڣ��������򴴽�֮
//�ļ��д��ڷ��� 0
//�ļ��д���ʧ�ܷ���-1
//�ļ��д���ʧ�ܷ���1
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
			return 1;//�ļ��д����ɹ�
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
			int histSize = 63;


			//cv::equalizeHist(f, f);
			cv::normalize(f, f, histSize, 0, cv::NORM_MINMAX);
			//cv::bitwise_xor(f, cv::Scalar(255), f);//����


			// �趨ȡֵ��Χ���趨ÿ���Ҷȵķ�Χ��
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
			// ����ֱ��ͼ����
			int hist_w = 400; int hist_h = 400;
			int bin_w = cvRound((double)hist_w / histSize);

			cv::Mat histImage(hist_w, hist_h, CV_8UC3, cv::Scalar(0, 0, 0));

			/// ��ֱ��ͼ��һ������Χ [ 0, histImage.rows ]
			cv::normalize(grayHist, grayHist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

			/// ��ֱ��ͼ�����ϻ���ֱ��ͼ
			for (int i = 1; i < histSize; i++)
			{
			line(histImage, cv::Point(bin_w*(i - 1), hist_h - cvRound(grayHist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(grayHist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
			}

			stringstream s;
			s << "samples\\�������滯ֱ��ͼ\\" << str_dir[index] << "\\";
			//s << "samples\\���滯ֱ��ͼ\\" << str_dir[index] << "\\";
			//s << "samples\\���⻯ֱ��ͼ\\" << str_dir[index] << "\\";
			//s << "samples\\ֱ��ͼ\\" << str_dir[index] << "\\";
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
			s << "samples\\�������滯ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\���滯ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\���⻯ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			s >> path;
			cv::imwrite(path, grayHist);

			/// ��ʾֱ��ͼ
			//cv::namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
			//cv::imshow("calcHist Demo", histImage);
			//cv::waitKey(0);
			*/
		}
	}

	//����һ������
	cv::Mat layerSizes = (cv::Mat_<int>(1, 3) << featureCount, 25, sampleTypeCount);//����һ��featureCount����  IDC_EDIT_YinCangCount����  sampleTypeCount�������������


	CvANN_MLP_TrainParams param;
	param.term_crit = cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 5000, 0.01);
	param.train_method = CvANN_MLP_TrainParams::BACKPROP;
	param.bp_dw_scale = 0.2;
	param.bp_moment_scale = 0.1;

	cv::Mat inputs(sampleAllCount, featureCount, CV_32FC1, trainingData);//��Ʒ����������ά�����������������
	cv::Mat outputs(sampleAllCount, sampleTypeCount, CV_32FC1, outputData);

	bp.create(layerSizes, CvANN_MLP::SIGMOID_SYM);
	bp.train(inputs, outputs, cv::Mat(), cv::Mat(), param);
	bp.save("ANN_mlp.xml");

	itemIndex = 0;
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
			cout << str_dir[index] << "_" << i << "��" << outindex << "->" << possibility << "->" << str_dir[outindex] << endl;
			itemIndex++;

		}
	}

	return 0;
}

