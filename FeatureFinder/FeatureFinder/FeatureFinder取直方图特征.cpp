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


	int itemIndex = 0;
	for (int index = 0; index < 7; index++)
	{
		for (int i = 1; i <= 50; i++)
		{
			cout << str_dir[index] << "_"<< i << endl;
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
			float range[] = { 0, 100 };
			const float* histRange = { range };
			bool uniform = true; bool accumulate = false;
			cv::calcHist(&f, 1, 0, cv::Mat(), grayHist, 1, &histSize, &histRange, uniform, accumulate);


			itemIndex++;
			
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
			//s << "samples\\�������滯ֱ��ͼ\\" << str_dir[index] << "\\";
			s << "samples\\���滯ֱ��ͼ\\" << str_dir[index] << "\\";
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
			//s << "samples\\�������滯ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			s << "samples\\���滯ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\���⻯ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			//s << "samples\\ֱ��ͼ\\" << str_dir[index] << "\\" << "Hist_" << num << ".jpg";
			s >> path;
			cv::imwrite(path, grayHist);

			/// ��ʾֱ��ͼ
			//cv::namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
			//cv::imshow("calcHist Demo", histImage);
			//cv::waitKey(0);
			
		}
	}

	return 0;
}

