#include "Judgement.h"


bool SortByM2(const Point &v1, const Point &v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{
	return v1.x < v2.x;//��������  
}


//Judgement::Judgement()
//{
//}


Judgement::~Judgement()
{
}

Point barycenter(vector<Point> contoursi)//������������
{
	Moments m = moments(contoursi);
	Point center = Point(0, 0);
	center.x = (int)(m.m10 / m.m00);
	center.y = (int)(m.m01 / m.m00);
	return center;
}

int Judgement::JudgementYON(Mat &image)
{
	int success = 0;
	MatND dstHist;
	Mat histoImg = image.clone();
	calcHist(&histoImg, 1, &channels, Mat(), dstHist, 1, &size, ranges);
	Mat dstImg(256, 256, CV_8U, Scalar(0));//��ֱ��ͼ
	double minValue = 0;
	double maxValue = 0;
	Point maxloc;
	minMaxLoc(dstHist, &minValue, &maxValue, NULL, &maxloc);
	//cout << "	     " << n << "." << m << "	     " << maxValue << endl;
	int hpt = saturate_cast<int>(0.9 * 256);
	vector<int> Boundnum;
	for (int j = 0; j < 256; j++)
	{
		float binValue = dstHist.at<float>(j);
		int realValue = saturate_cast<int>(binValue * hpt / maxValue);
		if (realValue != 0)
		{
			rectangle(dstImg, Point(j, 255), Point(j, 256 - realValue), Scalar(255));
			Boundnum.push_back(j);
		}

	}
	int  maxdata = *max_element(Boundnum.begin(), Boundnum.end());
	int  mindata = *min_element(Boundnum.begin(), Boundnum.end());//Ѱ��ֱ��ͼ��̬��Χ

	Rect recttemp;
	recttemp.x = maxloc.x;
	recttemp.y = maxloc.y - int((maxdata - mindata)*0.15);
	recttemp.width = 1;
	recttemp.height = int((maxdata - mindata)*0.3);
	rectangle(dstHist, recttemp, Scalar(0), -1);
	minMaxLoc(dstHist, &minValue, &maxValue, NULL, &maxloc);
	int anoThres = maxloc.y;//Ѱ�Ҵη�ֵ

	Scalar avgnum;
	Mat StdDevImg;
	meanStdDev(histoImg, avgnum, StdDevImg);
	double Stdnum = StdDevImg.at<double>(Point(0, 0));

	int ThreStep = maxdata - mindata;
	int StepNum = 30;
	int OrStep = mindata + int(ThreStep / 10);
	int Dstep = int(ThreStep / 30.0 + 0.5);
	if (Dstep == 0)
	{
		Dstep = 1;
		StepNum = ThreStep;
	}
	Mat TempImg;
	histoImg.copyTo(TempImg);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	Point pointSN, maxPoint = Point(0, 0);
	int Marknumone = 0;
	int Marknumtwo = 0;
	int Marknumthree = 0;
	for (int i = 0; i < StepNum; i++)
	{
		vector<Point> SN;
		OrStep = OrStep + Dstep;
		threshold(histoImg, TempImg, OrStep, 255, CV_THRESH_BINARY);


		/*Mat element = getStructuringElement(MORPH_RECT,Size(2,2));
		erode(TempImg, TempImg, cv::Mat());
		dilate(TempImg, TempImg, cv::Mat());*/
		TempImg = ~TempImg;


		/*stringstream strstrone;
		strstrone << "ˮ�ն�̬ͼ" << i << ".jpg";
		imwrite(strstrone.str(), TempImg);*/

		Mat BoundImg(TempImg.rows, TempImg.cols, CV_8UC1, Scalar(255));
		Rect Wrect;
		Wrect.x = 1;
		Wrect.y = 1;
		Wrect.width = BoundImg.cols - 2;
		Wrect.height = BoundImg.rows - 2;
		rectangle(BoundImg, Wrect, Scalar(0), -1);

		Mat PlusImg(TempImg.rows + 2, TempImg.cols + 2, CV_8UC1, Scalar(255));
		Mat PlusROI = PlusImg(Rect(1, 1, TempImg.cols, TempImg.rows));
		TempImg.copyTo(PlusROI);
		Mat ContoursImg = PlusImg.clone();

		findContours(ContoursImg, contours, hierarchy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		for (size_t j = 0; j < contours.size(); j++)
		{
			double area = cv::contourArea(contours[j]);
			pointSN.x = int(area);
			pointSN.y = j;
			SN.push_back(pointSN);
		}

		if (contours.size() != 0)
		{
			sort(SN.begin(), SN.end(), SortByM2);
			maxPoint = SN.back();
			if (OrStep > anoThres - 5 && OrStep<anoThres + 20)
				Dstep = 1;
			else
			{
				Dstep = int(ThreStep / 30.0 + 0.5);
			}
			if (Dstep == 0)
				Dstep = 1;
			int k = maxPoint.y;


			Mat MarkImg(TempImg.rows, TempImg.cols, CV_8UC1, Scalar(0));
			drawContours(MarkImg, contours, k, Scalar(255), -1);
			bitwise_and(BoundImg, MarkImg, MarkImg);
			int Mbound = 0;//�ж������Ƿ񵽱߽�
			Mbound = countNonZero(MarkImg);
			if (Mbound>0.5*(histoImg.cols))
				break;
			if (contours[k].size() <= 4)
				continue;
			int son = hierarchy[k][2];
			Point gravitycore = barycenter(contours[k]);//Ѱ����������

			Rect maxcontours = boundingRect(contours[k]);
			int wValue = maxcontours.width / 12;
			gravitycore = gravitycore + Point(wValue - 1, wValue - 1);

			Mat gravityImg(TempImg.rows + 2 * wValue, TempImg.cols + 2 * wValue, CV_8UC1, Scalar(0));
			Mat gravityImgROI = gravityImg(Rect(wValue, wValue, TempImg.cols, TempImg.rows));
			TempImg.copyTo(gravityImgROI);


			Rect gravityrect = Rect(gravitycore - Point(1, 1), gravitycore + Point(2 * wValue, 2 * wValue) - Point(2, 2));//����������Χ(2 * wValue)*(2 * wValue)�ľ�������
			if (gravityrect.x < 0 || gravityrect.y < 0)
				continue;

			int avnum = countNonZero(gravityImg(Rect(gravityrect)));
			vector<Point> hull;
			convexHull(contours[k], hull, false);
			double promark = (contourArea(contours[k])) / (contourArea(hull));

			if (son >= 0)//�ж��Ƿ�Ϊ������
			{
				int sonarea = 0;
				for (size_t j = 0; j < contours.size(); j++)
				{
					if (hierarchy[j][3] == k&&contourArea(contours[j])>4.0)
						sonarea = sonarea + contourArea(contours[j]);
				}
				if (50 * sonarea>maxPoint.x)//�˴�����һЩżȻ���ֵ��пյ�
					Marknumone++;
			}
			if (avnum < double(0.5 * gravityrect.width*gravityrect.width))//�����������еİ�ɫ��������Ƿ����
				Marknumtwo++;
			if (promark < 0.6)
				Marknumthree++;
		}

	}

	if (Marknumone > 2 || Marknumtwo >= 2 || Marknumthree > 3)//ȱ�ݵ�Ҳ����żȻ���ְ���
	{
		/*cout << "�õ���ˮ��2" << endl;*/

	}
	else
	{
		/*cout << "�õ���ȱ��2" << endl;*/
		success++;
	}
	return success;
}