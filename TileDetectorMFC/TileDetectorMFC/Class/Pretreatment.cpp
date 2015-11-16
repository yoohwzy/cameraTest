#include "Pretreatment.h"
#include "Judgement.h"


using namespace std;
using namespace cv;

bool SortByM1(const Point &v1, const Point &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1.x < v2.x;//升序排列  
}

int otsuThreshold(Mat &frame,MatND hist)
{
	const int GrayScale = 256;
	int width = frame.cols;
	int height = frame.rows;
	float pixelPro[GrayScale] = {0};
	int pixelSum = width * height, threshold = 0;
	vector<vector<int>> grayVlist;
	vector<int> grayV;

	int n = 0,m = 0;
	for (int i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = hist.at<float>(i) / pixelSum;
		
		if (pixelPro[i] > 0.0001)
		{
			grayV.push_back(i);
			m = 1;
		}
		else if (m == 1)
		{
			grayVlist.push_back(grayV);
			grayV.clear();
			n++;
			m = 0;
		}
			
	}
	Point max = Point(0,0);
	for (int i = 0; i < grayVlist.size(); i++)
	{
		int j = grayVlist[i].size();
		if (max.x < j)
		{
			max.x = j;
			max.y = i;
		}
	}
	threshold = grayVlist[max.y][0];
	return threshold;
}


Pretreatment::~Pretreatment()
{
}

vector<Point3f> Pretreatment::pretreatment(Mat &image, vector<Point> pointlist)
{
	Mat scr_gray;
	Mat src = image.clone();
	vector<Point3f> defectpoint;
	int ThresNum, j = 0;
	cvtColor(src, scr_gray, CV_RGB2GRAY);

	//Point A, B, C, D;
	//pointlist.push_back(A);
	//pointlist.push_back(B);
	//pointlist.push_back(C);
	//pointlist.push_back(D);
	Rect recImg = Rect(Point(0, 0), Point(0, 0));
	recImg = boundingRect(pointlist);

	//MatND histo;
	//calcHist(&scr_gray, 1, &channels, Mat(), histo, 1, &size, ranges);

	//vector<Point> num;
	//Point histopoint;
	//for (int i = 1; i < 256; i++)
	//{
	//	if (histo.at<float>(i) != 0)
	//	{
	//		histopoint.x = int(histo.at<float>(i));
	//		histopoint.y = i;
	//		num.push_back(histopoint);
	//	}
	//}

	//for (int i = 2; i < num.size(); i++)
	//{
	//	if (num[i].x <= num[i - 1].x && num[i].x <= num[i - 2].x)
	//		if (num[i].x <= num[i + 1].x && num[i].x <= num[i + 2].x)
	//		{
	//			ThresNum = num[i].y;
	//			break;
	//		}
	//}

	//Mat ThresholdImg;
	//threshold(scr_gray, ThresholdImg, ThresNum, 255, CV_THRESH_BINARY);
	Mat DilateImg = getStructuringElement(MORPH_RECT, Size(15, 15));
	//dilate(ThresholdImg, ThresholdImg, DilateImg);
	//erode(ThresholdImg, ThresholdImg, DilateImg);
	//erode(ThresholdImg, ThresholdImg, DilateImg);
	//dilate(ThresholdImg, ThresholdImg, DilateImg);


	//Mat findrect = ThresholdImg.clone();
	///*findrect = ~ThresholdImg;*/
	//vector<vector<cv::Point>> contours;
	//cv::findContours(findrect, contours, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	//// 寻找最大连通域  
	//double maxArea = 0;
	//vector<cv::Point> maxContour;
	//for (size_t i = 0; i < contours.size(); i++)
	//{
	//	double area = cv::contourArea(contours[i]);
	//	if (area > maxArea)
	//	{
	//		maxArea = area;
	//		maxContour = contours[i];
	//	}
	//}

	/*double S = contourArea(maxContour);
	double C = arcLength(maxContour, true);
	double disc = pow(0.5*C, 2) - 4 * S;
	double a1 = (0.5*C + sqrt(disc)) / 2;
	double a2 = (0.5*C - sqrt(disc)) / 2;*/



	// 将轮廓转为矩形框  
	/*cv::Rect maxRect = cv::boundingRect(maxContour);*/

	//CvPoint pt1 = cvPoint(maxRect.x, maxRect.y);
	//CvPoint pt2 = cvPoint(maxRect.x + maxRect.width, maxRect.y + maxRect.height);
	//Mat ContourImg = Mat(scr_gray.rows, scr_gray.cols, CV_8U, Scalar(255));
	//rectangle(ContourImg, pt1, pt2, Scalar(0), -1, 8);

	//bitwise_or(scr_gray, ContourImg, scr_gray);//清除轨道干扰
	
	/*cv::Point2f center;*/


	//vector<Point> leftbound;
	//Point boundpoint;
	//int leftx = 1;
	//int rightx = 1000;
	//for (int i = 1; i < scr_gray.rows; i = i + 50)
	//{
	//	Mat findImg = ThresholdImg(Rect(leftx, i, rightx, 1));
	//	if (countNonZero(findImg) < rightx - leftx)
	//	{
	//		boundpoint.x = leftx + countNonZero(findImg) - 1;
	//		boundpoint.y = i;
	//		leftx = boundpoint.x - 1 - 50;
	//		rightx = boundpoint.x - 1 + 50;
	//		leftbound.push_back(boundpoint);
	//	}
	//}
	//Vec4f lines;
	//fitLine(leftbound, lines, CV_DIST_L2, 0, 0.01, 0.01);
	//double d = sqrt((double)lines[0] * lines[0] + (double)lines[1] * lines[1]);
	//lines[0] /= d;
	//lines[1] /= d;
	//Point pt5, pt6;
	//pt5.x = cvRound(lines[2] + 5000 * lines[0]);
	//pt5.y = cvRound(lines[3] + 5000 * lines[1]);
	//pt6.x = cvRound(lines[2] - 5000 * lines[0]);
	//pt6.y = cvRound(lines[3] - 5000 * lines[1]);
	//cvtColor(ThresholdImg, ThresholdImg, CV_GRAY2RGB);
	//line(ThresholdImg, pt5, pt6, Scalar(55, 100, 195), 8, CV_AA);




	//center.x = leftbound.back().x - pt1.x;
	//center.y = leftbound.back().y;
	//double angle = (180 * atan(lines[1] / lines[0]) / CV_PI) - 90;  // 旋转角度  
	//double k = 1; // 缩放尺度 

	//Mat rotateMat;
	//rotateMat = cv::getRotationMatrix2D(center, angle, k);
	//Mat rotateImg;
	//warpAffine(ImgROI, rotateImg, rotateMat, ImgROI.size());
	//rotateImg.copyTo(ImgROI);

	/*Mat ImgROI = scr_gray(Rect(maxRect));*/
	Mat ImgROI = scr_gray(Rect(recImg));
	Mat CalImg = ImgROI(Range::all(), Range(1, 1));//垂直投影，光照矫正

	reduce(ImgROI, CalImg, 0, CV_REDUCE_AVG);

	double min, max;
	minMaxLoc(CalImg, &min, &max);
	int Maxrows = (int)max;
	Mat LightPlusrows(CalImg.rows, CalImg.cols, CV_8U, Scalar(Maxrows));
	absdiff(LightPlusrows, CalImg, LightPlusrows);//找出补偿值

	Mat Light;
	for (int i = 0; i < ImgROI.rows; i++)//构建补偿矩阵
	{
		Light.push_back(LightPlusrows);
	}
	//Mat PoccessImg = ImgROI;
	add(ImgROI, Light, ImgROI);//光照补偿




	/*Mat UpImg = ImgROI(Rect(0, 0, ImgROI.cols, 400));*/
	Mat MidImg = ImgROI(Rect(2, 400, ImgROI.cols-4, ImgROI.rows - 800));
	/*Mat DownImg = ImgROI(Rect(0, ImgROI.rows - 400, ImgROI.cols, 400));*/

	Mat CannyImg,ThImg = Mat(MidImg.size(),CV_8UC1,Scalar(0));//边缘检测
	DilateImg = getStructuringElement(MORPH_RECT, Size(5, 5));
	

	/*MatND histolist;
	calcHist(&MidImg, 1, &channels, Mat(), histolist, 1, &size, ranges);
	int OtsuV = otsuThreshold(MidImg, histolist);*/
	Canny(MidImg, CannyImg, 20, 30);
	dilate(CannyImg, CannyImg, DilateImg);
	erode(CannyImg, CannyImg, DilateImg);

	/*DilateImg = getStructuringElement(MORPH_RECT, Size(4, 4));
	dilate(CannyImg, CannyImg, DilateImg);
	erode(CannyImg, CannyImg, DilateImg);*/
	/*CannyImg = ~CannyImg;*/
	/*CvPoint pt3 = Point(0, 0);
	CvPoint pt4 = Point(80, CannyImg.rows - 1);

	rectangle(CannyImg, pt3, pt4, Scalar(0), -1, 8);*/

	Mat ThImgROI, MidImgROI;
	Point Thpt = Point(0, 0);
	for (int i = 1; i <= 5; i++)//局部二值化，约束灰度变化量
	{
		for (int j = 0 ; j < 2; j++)
		{
			Thpt.x = j*MidImg.cols*0.5;
			Rect ThRect = Rect(Thpt.x, Thpt.y, MidImg.cols*0.5, MidImg.rows*0.2);
			if (j)
				ThRect.width = MidImg.cols - MidImg.cols*0.5;
			if (i == 4)
				ThRect.height = MidImg.rows - 4 * MidImg.rows*0.2;
			MidImgROI = MidImg(Rect(ThRect));
			ThImgROI = ThImg(Rect(ThRect));

			MatND histolist;
			calcHist(&MidImgROI, 1, &channels, Mat(), histolist, 1, &size, ranges);
			int OtsuV = otsuThreshold(MidImgROI, histolist);//找到离散灰度值导数的阈值

			threshold(MidImgROI, ThImgROI, OtsuV, 255, CV_THRESH_BINARY);
		}
		Thpt.y = i*int(MidImg.rows*0.2);
	}
	//threshold(MidImg, ThImg, ThresNum, 255, CV_THRESH_OTSU);
	ThImg = ~ThImg;
	DilateImg = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(ThImg, ThImg, DilateImg);
	

	vector<vector<cv::Point>> decontours;
	vector<vector<cv::Point>> needContour;
	Mat anoImg = ThImg.clone();
	cv::findContours(anoImg, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	//for (size_t i = 0; i < decontours.size(); i++)
	//{
	//	if (contourArea(decontours[i]) > ThImg.rows*ThImg.cols*0.0005)
	//	{
	//		cv::Rect reThRect = cv::boundingRect(decontours[i]);//重新二值化
	//		MidImgROI = MidImg(Rect(reThRect));
	//		ThImgROI = ThImg(Rect(reThRect));
	//		Scalar avgr;
	//		Mat StdDevtemp;
	//		meanStdDev(MidImgROI, avgr, StdDevtemp);
	//		double minV, maxV = 0;
	//		minMaxLoc(MidImgROI, &minV, &maxV);
	//		threshold(MidImgROI, ThImgROI, int((avgr[0] + minV -1)*0.5), 255, CV_THRESH_BINARY);
	//		ThImgROI = ~ThImgROI;
	//	}
	//}
	//anoImg = ThImg.clone();
	//cv::findContours(anoImg, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	Mat cirlceImg(160, 160, CV_8UC1, Scalar(0));
	ellipse(cirlceImg, Point(80,80), Size(40,30), 90.0, 0,360,Scalar(255),-1,8);
	vector<vector<cv::Point>> tempttours;
	cv::findContours(cirlceImg, tempttours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//比较轮廓的标准

	vector<double> listnum;
	for (size_t i = 0; i < decontours.size(); i++)
	{
		if (decontours[i].size() > 6)
		{
			double matchextent = matchShapes(decontours[i], tempttours[0], CV_CONTOURS_MATCH_I3,0);
			listnum.push_back(matchextent);

			/*cv::Rect reThRect = cv::boundingRect(decontours[i]);

			int Whitenum = countNonZero(CannyImg(Rect(reThRect)));*/

			if (matchextent < 1.0)
			{
				needContour.push_back(decontours[i]);
			}
			
		}

	}

	
	

	Mat deresult(CannyImg.size(), CV_8U, Scalar(0));
	drawContours(deresult, needContour, -1, Scalar(255), -1);
	imwrite("deresult.png", deresult);
	/*Mat Watchresult = deresult.clone();
	cvtColor(Watchresult, Watchresult, CV_GRAY2RGB);*/

	Mat AvgImg;
	for (size_t i = 0; i < needContour.size(); i++)
	{
		Rect box = boundingRect(needContour[i]);
		CvPoint pt7 = cvPoint(box.x, box.y);
		CvPoint pt8 = cvPoint(box.x + box.width, box.y + box.height);
		CvPoint pt_ROI_a = Point(0, 0);
		CvPoint pt_ROI_b = Point(MidImg.cols-1, MidImg.rows-1);

		if (pt7.x - 0.8*box.width > 0)
			pt_ROI_a.x = pt7.x - 0.8*box.width;
		if (pt7.y - 0.8*box.height > 0)
			pt_ROI_a.y = pt7.y - 0.8*box.height;
		if (pt8.x + 0.8*box.width < MidImg.cols - 1)
			pt_ROI_b.x = pt8.x + 0.8*box.width;
		if (pt8.y + 0.8*box.height < MidImg.rows - 1)
			pt_ROI_b.y = pt8.y + 0.8*box.height;

		Mat boxImg = MidImg(Rect(pt_ROI_a, pt_ROI_b));
		Judgement JYON;
		int n = 0;
		Point3f defectcenter;
		defectcenter.x = 0.5*(pt7.x + pt8.x) + recImg.x + 2;
		defectcenter.y = 0.5*(pt7.y + pt8.y) + recImg.y + 400;
		if (abs(pt8.y - pt7.y)>abs(pt8.x - pt7.x))
			defectcenter.z = 0.5*abs(pt8.x - pt7.x);
		else
			defectcenter.z = 0.5*abs(pt8.y - pt7.y);
		//cout << i << endl;
		if (JYON.JudgementYON(boxImg))
		{

			defectpoint.push_back(defectcenter);
		}


	}

	/*cv::MatND CalcHist;
	calcHist(&MidImg, 1, &channels, Mat(), CalcHist, 1, &size, ranges);

	Mat calcHistsub(256, 1, CV_8U, Scalar(0));
	vector<Point> Histnum;
	for (int i = 0; i < 255; i++)
	{
		histopoint.x = int(CalcHist.at<float>(i + 1) - CalcHist.at<float>(i));
		histopoint.y = i;
		Histnum.push_back(histopoint);
	}

	sort(Histnum.begin(), Histnum.end(), SortByM1);
	int nonenum = 0;
	ThresNum = 0;
	Mat ThresROI;
	do
	{
		j++;
		if (Histnum[Histnum.size() - j].y <= Histnum[Histnum.size() - 1].y)
		{
			ThresNum = Histnum[Histnum.size() - j].y;
			threshold(MidImg, ThresROI, ThresNum, 255, CV_THRESH_BINARY);
		}
		nonenum = countNonZero(ThresROI);
	} while (nonenum< 0.95*MidImg.rows*MidImg.cols);*/
	return defectpoint;
}
