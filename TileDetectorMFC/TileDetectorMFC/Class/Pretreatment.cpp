#include "Pretreatment.h"
#include "Judgement.h"


using namespace std;
using namespace cv;


int kItemsToProduce = 100;   // How many items we plan to produce.
int m = 0;
vector<Point3f> defectpoint;
vector<Point3f> repoint;
Mat MidImg, ThImg, CMidImg,PMidImg;
vector<vector<cv::Point>> needContour;
Rect recImg = Rect(Point(0, 0), Point(0, 0));
Rect maxRect = Rect(Point(0, 0), Point(0, 0));
vector<vector<cv::Point>> ecliptours;
Point3f defectcenter;
vector<vector<Point3f>> p2c;
vector<Point3f> p2cp;
vector<vector<cv::Point>> dilateneedcontours;




bool SortByM1(const Vec4i &v1, const Vec4i &v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{
	return v1[3] < v2[3];//��������  
}

bool SortBysize(const vector<Point>v1, const vector<Point>v2)//ע�⣺�������Ĳ���������һ��Ҫ��vector��Ԫ�ص�����һ��  
{
	return v1.size() > v2.size();//��������  
}

Point barycenter1(vector<Point> contoursi)//������������
{
	Moments m = moments(contoursi);
	Point center = Point(0, 0);
	center.x = (int)(m.m10 / m.m00);
	center.y = (int)(m.m01 / m.m00);
	return center;
}

int otsuThreshold(Mat &frame, MatND hist)//�ֲ���ֵ������ֵѡȡ
{
	const int GrayScale = 256;
	int width = frame.cols;
	int height = frame.rows;
	float pixelPro[GrayScale] = { 0 };
	int pixelSum = width * height, threshold = 0;
	vector<vector<int>> grayVlist;
	vector<int> grayV;

	int n = 0, m = 0;
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
	Point max = Point(0, 0);
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

Mat Grow(Mat &image, Point seedpoint,double th_v)//��������
{
	Mat HyImg = image.clone();
	Mat SameImg(HyImg.size(), CV_8UC1, Scalar(0));
	vector<Point> seedq;
	seedq.push_back(seedpoint);
	while (!seedq.empty())
	{
		Point growpoint = seedq.back();
		seedq.pop_back();
		if ((growpoint.x > 0) && (growpoint.x < (image.cols - 1)) && (growpoint.y > 0) && (growpoint.y < (image.rows - 1)))
		{
			if (HyImg.at<uchar>(growpoint.y, growpoint.x - 1) < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y, growpoint.x - 1) == 0 && HyImg.at<uchar>(growpoint.y, growpoint.x - 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y));
					SameImg.at<uchar>(growpoint.y, growpoint.x - 1) = 255;
				}
			}

			if (HyImg.at<uchar>(growpoint.y, growpoint.x + 1) < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y, growpoint.x + 1) == 0 && HyImg.at<uchar>(growpoint.y, growpoint.x + 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y));
					SameImg.at<uchar>(growpoint.y, growpoint.x + 1) = 255;
				}
			}

			if (HyImg.at<uchar>(growpoint.y - 1, growpoint.x) < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y - 1, growpoint.x) == 0 && HyImg.at<uchar>(growpoint.y - 1, growpoint.x) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y - 1));
					SameImg.at<uchar>(growpoint.y - 1, growpoint.x) = 255;
				}
			}

			if (HyImg.at<uchar>(growpoint.y + 1, growpoint.x)  < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y + 1, growpoint.x) == 0 && HyImg.at<uchar>(growpoint.y + 1, growpoint.x) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y + 1));
					SameImg.at<uchar>(growpoint.y + 1, growpoint.x) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y - 1, growpoint.x - 1)  < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y - 1, growpoint.x - 1) == 0 && HyImg.at<uchar>(growpoint.y - 1, growpoint.x - 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y - 1));
					SameImg.at<uchar>(growpoint.y - 1, growpoint.x - 1) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y - 1, growpoint.x + 1)  < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y - 1, growpoint.x + 1) == 0 && HyImg.at<uchar>(growpoint.y - 1, growpoint.x + 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y - 1));
					SameImg.at<uchar>(growpoint.y - 1, growpoint.x + 1) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y + 1, growpoint.x - 1)  < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y + 1, growpoint.x - 1) == 0 && HyImg.at<uchar>(growpoint.y + 1, growpoint.x - 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y + 1));
					SameImg.at<uchar>(growpoint.y + 1, growpoint.x - 1) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y + 1, growpoint.x + 1)  < th_v)
			{
				if (SameImg.at<uchar>(growpoint.y + 1, growpoint.x + 1) == 0 && HyImg.at<uchar>(growpoint.y + 1, growpoint.x + 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y + 1));
					SameImg.at<uchar>(growpoint.y + 1, growpoint.x + 1) = 255;
				}
			}
		}


	}
	return SameImg;
}



void Pretreatment::ProduceItem(ItemRepository *ir, int item, Rect rect_grow, Point a, Point b,bool write_data)
{
	std::unique_lock<std::mutex> lock(ir->mtx);
	while (((ir->write_position + 1) % kItemRepositorySize)
		== ir->read_position) { // item buffer is full, just wait here.
		/*std::cout << "Producer is waiting for an empty slot...\n";*/
		(ir->repo_not_full).wait(lock); // �����ߵȴ�"��Ʒ�⻺������Ϊ��"��һ��������.
	}

	(ir->item_buffer)[ir->write_position] = item; // д���Ʒ.

	

	//int maxszie = 0;
	//int n = -1;
	//for (size_t j = 0; j < tempttours.size(); j++)
	//{
	//	Rect rectemp = boundingRect(tempttours[j]);
	//	if (rectemp.x < centermark.x&&rectemp.y < centermark.y)
	//	{
	//		if (rectemp.x + rectemp.width>centermark.x && rectemp.y + rectemp.height>centermark.y)
	//			n = j;
	//	}
	//}
	//if (n == -1)
	//{
	//	for (size_t j = 0; j < tempttours.size(); j++)
	//	{
	//		if (maxszie < tempttours[j].size())
	//		{
	//			maxszie = tempttours[j].size();
	//			n = j;
	//		}
	//	}
	//}
	//vector<int> hullsI;
	//vector<Vec4i> defects;
	//vector<Point> hull;
	//convexHull(tempttours[n], hull, false);
	//convexHull(tempttours[n], hullsI, false);//Ѱ��͹��
	//convexityDefects(tempttours[n], hullsI, defects);
	//Point gpoint = barycenter1(hull);
	//int countnum = countNonZero(growImg(Rect(gpoint - Point(1, 1), gpoint + Point(2, 2))));
	//double asextent = matchShapes(tempttours[n], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//��Բ��״ƥ��
	//if (countnum < 9 || asextent>1.0)
	//	return;

	//int d1, d2, d3, d4;
	//d1 = d2 = d3 = d4 = 1;//d1,d2,d3,d4�ֱ�������ң��ϣ���
	if (write_data)
	{
		Point pt7 = Point(rect_grow.x, rect_grow.y) + a;
		Point pt8 = Point(rect_grow.x + rect_grow.width, rect_grow.y + rect_grow.height) + a;

		defectcenter.x = 0.5*(pt7.x + pt8.x) + recImg.x + maxRect.x + 2;//����任��ԭͼ
		defectcenter.y = 0.5*(pt7.y + pt8.y) + recImg.y + maxRect.y + 400;
		if (abs(pt8.y - pt7.y) > abs(pt8.x - pt7.x))
			defectcenter.z = abs(pt8.x - pt7.x);
		else
			defectcenter.z = abs(pt8.y - pt7.y);
		p2cp.clear();
		p2cp.push_back(defectcenter);
		Point3f pttemp;
		pttemp.x = b.x;
		pttemp.y = b.y;
		pttemp.z = 0;
		p2cp.push_back(pttemp);
		pttemp.x = a.x;
		pttemp.y = a.y;
		p2cp.push_back(pttemp);
		p2c.push_back(p2cp);
		m++;
	}
	(ir->write_position)++; // д��λ�ú���.

	if (ir->write_position == kItemRepositorySize) // д��λ�������ڶ����������������Ϊ��ʼλ��.
		ir->write_position = 0;

	(ir->repo_not_empty).notify_all(); // ֪ͨ�����߲�Ʒ�ⲻΪ��.
	lock.unlock(); // ����.
}

int Pretreatment::ConsumeItem(ItemRepository *ir)
{
	int data;
	std::unique_lock<std::mutex> lock(ir->mtx);
	// item buffer is empty, just wait here.
	while (ir->write_position == ir->read_position) {
		/*std::cout << "Consumer is waiting for items...\n";*/
		(ir->repo_not_empty).wait(lock); // �����ߵȴ�"��Ʒ�⻺������Ϊ��"��һ��������.
	}

	data = (ir->item_buffer)[ir->read_position]; // ��ȡĳһ��Ʒ
	if (p2c.size() != 0 )
	{
		Judgement JYON;
		Point a, b;
		a.x = p2c.back()[2].x;
		a.y = p2c.back()[2].y;
		b.x = p2c.back()[1].x;
		b.y = p2c.back()[1].y;
		Mat boxJImg = PMidImg(Rect(a, b));
		defectcenter = p2c.back()[0];
		p2c.pop_back();
		Faults::Hole hole;
		if (1)//�ж��Ƿ�Ϊȱ��
		{
			if (defectpoint.size() == 0)
			{
				defectpoint.push_back(defectcenter);
				hole.position.x = defectcenter.x;
				hole.position.y = defectcenter.y;
				hole.diameter = defectcenter.z;
				_faults->Holes.push_back(hole);
				/*cout << data << endl;*/
			}
			else
			{
				int k = 0;
				for (int j = 0; j < defectpoint.size(); j++)
				{
					if (abs(defectpoint[j].x - defectcenter.x) + abs(defectpoint[j].y - defectcenter.y) > 20)
						k++;
				}
				if (k == defectpoint.size())
				{
					defectpoint.push_back(defectcenter);
					hole.position.x = defectcenter.x;
					hole.position.y = defectcenter.y;
					hole.diameter = defectcenter.z;
					_faults->Holes.push_back(hole);
					/*cout << data << endl;*/
				}
			}
		}
	}
	

	(ir->read_position)++; // ��ȡλ�ú���

	if (ir->read_position >= kItemRepositorySize) // ��ȡλ�����Ƶ������������λ.
		ir->read_position = 0;

	(ir->repo_not_full).notify_all(); // ֪ͨ�����߲�Ʒ�ⲻΪ��.
	lock.unlock(); // ����.

	return data; // ���ز�Ʒ.
}


void Pretreatment::ProducerTask() // ����������
{
	for (size_t i = 0; i < needContour.size(); i++)
	{
		Rect box = boundingRect(needContour[i]);
		Point pt7 = Point(box.x, box.y);
		Point pt8 = Point(box.x + box.width, box.y + box.height);
		Point pt_ROI_a = Point(0, 0);
		Point pt_ROI_b = Point(PMidImg.cols - 1, PMidImg.rows - 1);//��ȡԭʼ��

		if (pt7.x - 10 * box.width > 0)
			pt_ROI_a.x = pt7.x - 10 * box.width;
		if (pt7.y - 10 * box.height > 0)
			pt_ROI_a.y = pt7.y - 10 * box.height;
		if (pt8.x + 10 * box.width < MidImg.cols - 1)
			pt_ROI_b.x = pt8.x + 10 * box.width;
		if (pt8.y + 10 * box.height < MidImg.rows - 1)
			pt_ROI_b.y = pt8.y + 10 * box.height;

		Point centermark = 0.5 * (pt7 + pt8) - pt_ROI_a;//��boxImg�д������ͨ������ı��
		Mat ready_boxImg = PMidImg(Rect(pt_ROI_a, pt_ROI_b));//��ȡ��ͨ�����������100���������
		Scalar avgmean;
		Mat StdImg(ready_boxImg.size(), CV_8UC1, Scalar(0));
		meanStdDev(ready_boxImg, avgmean, StdImg);
		Mat growImg = Grow(ready_boxImg, centermark, avgmean[0] - 5);//��������

		int whitenum = countNonZero(growImg);
		Mat growtemp = growImg.clone();

		/*erode(growtemp, growtemp,Mat(),Point(-1,-1),2);*/
		vector<vector<cv::Point>> tempttours;
		findContours(growtemp, tempttours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if (tempttours.size() == 0 || countNonZero(growImg) <= 2)
		{
			ProduceItem(&gItemRepository, i, box, pt_ROI_a, pt_ROI_b, 0);
			continue;
		}
		Rect growT_RECT = boundingRect(tempttours[0]);
		double proportion = double(growT_RECT.width) / double(growT_RECT.height);
		if (proportion > 3 || proportion < 0.33)
		{
			ProduceItem(&gItemRepository, i, growT_RECT, pt_ROI_a, pt_ROI_b, 0);
			continue;
		}

		int k = 1;
		Mat oneImg;
		if (growImg.rows>growImg.cols)
		{
			oneImg = growImg(Range(1, 1), Range::all());
		}
		else
		{
			oneImg = growImg(Range::all(), Range(1, 1));
			k = 0;
		}
		reduce(growImg, oneImg, k, CV_REDUCE_AVG);
		double maxV_white;
		minMaxLoc(oneImg, NULL, &maxV_white);
		int white_range = countNonZero(oneImg);
		maxV_white = (k == 1) ? growImg.rows * maxV_white : growImg.cols * maxV_white;
		maxV_white /= 255;
		if (1.5*whitenum / white_range > maxV_white || 2 * whitenum < growT_RECT.width*growT_RECT.height)
		{
			ProduceItem(&gItemRepository, i, growT_RECT, pt_ROI_a, pt_ROI_b, 0);
			continue;
		}
		Mat ano_boxImg = ready_boxImg.clone();
		threshold(ano_boxImg, ano_boxImg, avgmean[0] - 10, 255, 1);
		bitwise_and(ano_boxImg, growImg, ano_boxImg);
		vector<vector<cv::Point>> lowTcontour;
		findContours(ano_boxImg, lowTcontour, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		int size_v = lowTcontour.size();
		sort(lowTcontour.begin(), lowTcontour.end(), SortBysize);
		for (size_t j = 1; j < lowTcontour.size(); j++)
		{
			if (contourArea(lowTcontour[0]) > 100 * contourArea(lowTcontour[j]))
				size_v--;
		}
		if (size_v > 1)
		{
			ProduceItem(&gItemRepository, i, growT_RECT, pt_ROI_a, pt_ROI_b, 0);
			continue;
		}
		/*std::cout << "Produce the " << i << "^th item..." << std::endl;*/
		ProduceItem(&gItemRepository, i, growT_RECT, pt_ROI_a,pt_ROI_b,1);
	}
}

void Pretreatment::ConsumerTask() // ����������
{
	static int cnt = 1;
	while (1) {
		int item = ConsumeItem(&gItemRepository); // ����һ����Ʒ.
		/*std::cout << "Consume the " << item << "^th item" << std::endl;*/
		if (++item == kItemsToProduce) break; // �����Ʒ���Ѹ���Ϊ kItemsToProduce, ���˳�.
	}
}

void Pretreatment::InitItemRepository(ItemRepository *ir)
{
	ir->write_position = 0; // ��ʼ����Ʒд��λ��.
	ir->read_position = 0; // ��ʼ����Ʒ��ȡλ��.
}


void Pretreatment::linedetect()
{
	Mat CannyImg,BlurImg;
	resize(MidImg, CannyImg, Size(MidImg.cols / 4, MidImg.rows / 4), 0, 0, INTER_AREA);
	bilateralFilter(CannyImg, BlurImg, 5, 5, 5);
	Canny(BlurImg, BlurImg, 5, 20);
	rectangle(BlurImg, Rect(Point(0, 0), Point(20, BlurImg.rows)), Scalar(0), -1);
	rectangle(BlurImg, Rect(Point(BlurImg.cols - 10, 0), Point(BlurImg.cols, BlurImg.rows)), Scalar(0), -1);//���ҿ��߱�Ե������

	vector<vector<cv::Point>> dilatecontours;
	findContours(BlurImg, dilatecontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	int move_n = 0;
	if (dilatecontours.size() > 100)
		move_n = 20;
	else
		move_n = dilatecontours.size()*0.5;
	partial_sort(dilatecontours.begin(), dilatecontours.begin() + move_n, dilatecontours.end(), SortBysize);
	Mat dilateImg(BlurImg.size(), CV_8UC1, Scalar(0));

	vector<vector<Point>> bigcontours;
	bigcontours.insert(bigcontours.end(), dilatecontours.begin(), dilatecontours.begin() + move_n);
	drawContours(dilateImg, bigcontours, -1, Scalar(255), -1);
	/*dilate(dilateImg, dilateImg, Mat());
	erode(dilateImg, dilateImg, Mat());*/

	Mat LineImg(160, 160, CV_8UC1, Scalar(0));
	vector<vector<Point>> Linecontours;
	line(LineImg, Point(4, 80), Point(156, 80), Scalar(255), 2, 8);//��ֱ��
	cv::findContours(LineImg, Linecontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//�Ƚ������ı�׼

	Mat CcontourImg = BlurImg.clone();
	Rect cannyrect;
	int m = 0, linenumall = 0, linenum = 0;
	for (size_t i = 0; i < bigcontours.size(); i++)
	{
		
		cannyrect = boundingRect(bigcontours[i]);
		Mat waitImg = dilateImg(Rect(cannyrect));
		if (cannyrect.width < 50 && cannyrect.height < 50)
			continue;
		int matchlinepro = matchShapes(bigcontours[i], Linecontours[0], CV_CONTOURS_MATCH_I3, 0);
		if (matchlinepro>1.0)
			continue;
		Faults::Scratch scratch;
		scratch.position.x = 4*cannyrect.x + 2*cannyrect.width + recImg.x + maxRect.x + 2;
		scratch.position.y = 4*cannyrect.y + 2*cannyrect.height + recImg.y + maxRect.y + 400;
		scratch.length = (cannyrect.width >cannyrect.height) ? 2*cannyrect.width : 2*cannyrect.height;
		if (scratch.length < 35)
			continue;
		_faults->Scratchs.push_back(scratch);
	}
}





Pretreatment::~Pretreatment()
{
}

void Pretreatment::pretreatment(Mat &image, Block *_block, Faults *faults)
{
	_faults = faults;
	Mat scr_gray;
	Mat src = image.clone();
	
	if (src.channels() == 3)//�ж��Ƿ�Ϊ��ͼ
		cvtColor(src, scr_gray, CV_RGB2GRAY);
	else
		scr_gray = src;
	vector<Point> pointlist;
	pointlist.push_back(_block->A);
	pointlist.push_back(_block->B);
	pointlist.push_back(_block->C);
	pointlist.push_back(_block->D);

	recImg = boundingRect(pointlist);//��ȡ��ש����

	Mat ImgROI = scr_gray(Rect(recImg));
	Mat CalImg = ImgROI(Range::all(), Range(1, 1));//��ֱͶӰ�����ս���

	reduce(ImgROI, CalImg, 0, CV_REDUCE_AVG);

	double min, max;
	minMaxLoc(CalImg, &min, &max);
	int Maxrows = (int)max;
	Mat LightPlusrows(CalImg.rows, CalImg.cols, CV_8U, Scalar(Maxrows));
	absdiff(LightPlusrows, CalImg, LightPlusrows);//�ҳ�����ֵ

	Mat Light;
	for (int i = 0; i < ImgROI.rows; i++)//������������
	{
		Light.push_back(LightPlusrows);
	}
	add(ImgROI, Light, ImgROI);//���ղ���

	MidImg = ImgROI(Rect(2, 400, ImgROI.cols - 4, ImgROI.rows - 800));

	ThImg = Mat(MidImg.size(), CV_8UC1, Scalar(0));//��Ե���
	Mat DilateImg = getStructuringElement(MORPH_RECT, Size(5, 5));


	Mat ThImgROI, MidImgROI;
	Point Thpt = Point(0, 0);
	for (int i = 1; i <= 5; i++)//�ֲ���ֵ����Լ���Ҷȱ仯��
	{
		for (int j = 0; j < 2; j++)
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
			int OtsuV = otsuThreshold(MidImgROI, histolist);//�ҵ���ɢ�Ҷ�ֵ��������ֵ

			threshold(MidImgROI, ThImgROI, OtsuV, 255, CV_THRESH_BINARY);
		}
		Thpt.y = i*int(MidImg.rows*0.2);
	}
	//threshold(MidImg, ThImg, ThresNum, 255, CV_THRESH_OTSU);
	ThImg = ~ThImg;
	DilateImg = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(ThImg, ThImg, DilateImg);


	vector<vector<cv::Point>> decontours;
	Mat anoImg = ThImg.clone();
	cv::findContours(anoImg, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);



	Mat cirlceImg(160, 160, CV_8UC1, Scalar(0));
	ellipse(cirlceImg, Point(80, 80), Size(40, 30), 90.0, 0, 360, Scalar(255), -1, 8);//����Բ

	cv::findContours(cirlceImg, ecliptours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//�Ƚ������ı�׼

	vector<double> listnum;
	for (size_t i = 0; i < decontours.size(); i++)
	{
		if (decontours[i].size() > 6)
		{
			double matchextent = matchShapes(decontours[i], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//�Ƚϴ����ȱ�ݵ���̬
			listnum.push_back(matchextent);

			if (matchextent < 1.0)
			{
				needContour.push_back(decontours[i]);
			}

		}

	}

	CMidImg = MidImg.clone();
	PMidImg = MidImg.clone();
	kItemsToProduce = needContour.size();
	InitItemRepository(&gItemRepository);
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // �����ȱ�ݵ�Ԥ����.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // ����ȱ����ˮ��.
	std::thread line(std::mem_fn(&Pretreatment::linedetect), this);//���ۼ��
	
	producer.join();
	consumer.join();
	line.join();
	needContour.clear();
	dilateneedcontours.clear();
	p2c.clear();
	p2cp.clear();
}
