#include "Pretreatment.h"
#include "Judgement.h"


using namespace std;
using namespace cv;

bool SortByM1(const Point &v1, const Point &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1.x < v2.x;//升序排列  
}


Pretreatment::~Pretreatment()
{
}

vector<Point> Pretreatment::pretreatment(Mat &image)
{
	Mat scr_gray, MaskRect_gray;
	Mat src = image.clone();
	vector<Point> defectpoint;
	int ThresNum, j = 0;
	cvtColor(src, scr_gray, CV_RGB2GRAY);

	MatND histo;
	calcHist(&scr_gray, 1, &channels, Mat(), histo, 1, &size, ranges);

	vector<Point> num;
	Point histopoint;
	for (int i = 1; i < 256; i++)
	{
		if (histo.at<float>(i) != 0)
		{
			histopoint.x = int(histo.at<float>(i));
			histopoint.y = i;
			num.push_back(histopoint);
		}
	}

	for (int i = 2; i < num.size(); i++)
	{
		if (num[i].x <= num[i - 1].x && num[i - 1].x <= num[i - 2].x)
			if (num[i].x <= num[i + 1].x && num[i + 1].x <= num[i + 2].x)
			{
				ThresNum = num[i].y;
				break;
			}
	}

	Mat ThresholdImg;
	threshold(scr_gray, ThresholdImg, ThresNum, 255, CV_THRESH_BINARY);
	Mat DilateImg = getStructuringElement(MORPH_RECT, Size(15, 15));
	dilate(ThresholdImg, ThresholdImg, DilateImg);
	erode(ThresholdImg, ThresholdImg, DilateImg);
	erode(ThresholdImg, ThresholdImg, DilateImg);
	dilate(ThresholdImg, ThresholdImg, DilateImg);


	Mat findrect = ThresholdImg.clone();
	/*findrect = ~ThresholdImg;*/
	vector<vector<cv::Point>> contours;
	cv::findContours(findrect, contours, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

	// 寻找最大连通域  
	double maxArea = 0;
	vector<cv::Point> maxContour;
	for (size_t i = 0; i < contours.size(); i++)
	{
		double area = cv::contourArea(contours[i]);
		if (area > maxArea)
		{
			maxArea = area;
			maxContour = contours[i];
		}
	}

	//double S = contourArea(maxContour);
	//double C = arcLength(maxContour, true);
	//double disc = pow(0.5*C, 2) - 4 * S;
	//double a1 = (0.5*C + sqrt(disc)) / 2;
	//double a2 = (0.5*C - sqrt(disc)) / 2;



	// 将轮廓转为矩形框  
	cv::Rect maxRect = cv::boundingRect(maxContour);

	CvPoint pt1 = cvPoint(maxRect.x, maxRect.y);
	CvPoint pt2 = cvPoint(maxRect.x + maxRect.width, maxRect.y + maxRect.height);
	Mat ContourImg = Mat(scr_gray.rows, scr_gray.cols, CV_8U, Scalar(255));
	rectangle(ContourImg, pt1, pt2, Scalar(0), -1, 8);

	bitwise_or(scr_gray, ContourImg, scr_gray);//清除轨道干扰
	Mat ImgROI = scr_gray(Rect(pt1, pt2));//寻找旋转参数
	cv::Point2f center;


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




	Mat UpImg = ImgROI(Rect(0, 0, ImgROI.cols, 400));
	Mat MidImg = ImgROI(Rect(0, 400, ImgROI.cols, ImgROI.rows - 800));
	Mat DownImg = ImgROI(Rect(0, ImgROI.rows - 400, ImgROI.cols, 400));

	Mat CannyImg, MidImgone, MidImgtwo;//边缘检测
	DilateImg = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(MidImg, MidImg, DilateImg);
	erode(MidImg, MidImg, DilateImg);
	threshold(MidImg, CannyImg, ThresNum, 255, CV_THRESH_OTSU);


	DilateImg = getStructuringElement(MORPH_RECT, Size(10, 10));
	dilate(CannyImg, CannyImg, DilateImg);
	erode(CannyImg, CannyImg, DilateImg);
	CvPoint pt3 = Point(0, 0);
	CvPoint pt4 = Point(80, CannyImg.rows - 1);

	rectangle(CannyImg, pt3, pt4, Scalar(0), -1, 8);

	vector<vector<cv::Point>> decontours;
	cv::findContours(CannyImg, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	vector<vector<cv::Point>> needContour;
	for (size_t i = 0; i < decontours.size(); i++)
	{
		if (decontours[i].size() > 90)
		{
			needContour.push_back(decontours[i]);
		}
	}
	Mat deresult(CannyImg.size(), CV_8U, Scalar(0));
	drawContours(deresult, needContour, -1, Scalar(255), 5);
	Mat Watchresult = deresult.clone();
	cvtColor(Watchresult, Watchresult, CV_GRAY2RGB);

	Mat AvgImg;
	for (size_t i = 0; i < needContour.size(); i++)
	{
		Rect box = boundingRect(needContour[i]);
		CvPoint pt7 = cvPoint(box.x, box.y);
		CvPoint pt8 = cvPoint(box.x + box.width, box.y + box.height);
		Mat boxImg = MidImg(Rect(pt7, pt8));
		Judgement JYON;
		int n = 0;
		Point defectcenter;
		defectcenter.x = 0.5*(pt7.x + pt8.x);
		defectcenter.y = 0.5*(pt7.y + pt8.y);
		if (JYON.JudgementYON(boxImg, i, n))
		{

			defectpoint.push_back(defectcenter);
		}


	}

	cv::MatND CalcHist;
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
	} while (nonenum< 0.95*MidImg.rows*MidImg.cols);
	return defectpoint;
}
