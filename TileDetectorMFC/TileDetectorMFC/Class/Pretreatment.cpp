#include "Pretreatment.h"
#include "Judgement.h"


using namespace std;
using namespace cv;


int kItemsToProduce = 100;   // How many items we plan to produce.
vector<Point3f> defectpoint;
vector<Point3f> repoint;
Mat MidImg, ThImg, boxImg, CMidImg;
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

Mat Grow(Mat &image, Mat seedImg)//��������
{
	Point3f point;
	queue<Point3f> seedd;
	vector<Point3f> num;
	double sum = 0;

	for (int i = 0; i<image.rows; i++)
	{
		for (int j = 0; j<image.cols; j++)
		{
			if (seedImg.at<uchar>(i, j) == 255)
			{
				point.x = i;
				point.y = j;
				point.z = image.at<uchar>(i, j);
				seedd.push(point);
				num.push_back(point);
				sum += point.z;
			}
		}
	}
	double Tnum = sum / double(num.size());
	Point3f temppoint;
	while (!seedd.empty())
	{
		point = seedd.front();
		seedd.pop();

		if ((point.x>0) && (point.x<(image.rows - 1)) && (point.y>0) && (point.y<(image.cols - 1)))
		{
			if (seedImg.at<uchar>(point.x - 1, point.y) == 0 && (abs(image.at<uchar>(point.x - 1, point.y) - point.z) <= 1))
			{
				if (sum / double(num.size()) < Tnum + 5)
				{
					seedImg.at<uchar>(point.x - 1, point.y) = 255;
					temppoint.x = point.x - 1;
					temppoint.y = point.y;
					temppoint.z = image.at<uchar>(point.x - 1, point.y);
					seedd.push(temppoint);
					num.push_back(point);
					sum += temppoint.z;

				}

			}
			if (seedImg.at<uchar>(point.x, point.y + 1) == 0 && (abs(image.at<uchar>(point.x, point.y + 1) - point.z) <= 1))
			{
				if (sum / double(num.size()) < Tnum + 5)
				{
					seedImg.at<uchar>(point.x, point.y + 1) = 255;
					temppoint.x = point.x;
					temppoint.y = point.y + 1;
					temppoint.z = image.at<uchar>(point.x, point.y + 1);
					seedd.push(temppoint);
					num.push_back(point);
					sum += temppoint.z;

				}
			}
			if (seedImg.at<uchar>(point.x, point.y - 1) == 0 && (abs(image.at<uchar>(point.x, point.y - 1) - point.z) <= 1))
			{
				if (sum / double(num.size()) < Tnum + 5)
				{
					seedImg.at<uchar>(point.x, point.y - 1) = 255;
					temppoint.x = point.x;
					temppoint.y = point.y - 1;
					temppoint.z = image.at<uchar>(point.x, point.y - 1);
					seedd.push(temppoint);
					num.push_back(point);
					sum += temppoint.z;

				}
			}
			if (seedImg.at<uchar>(point.x + 1, point.y) == 0 && (abs(image.at<uchar>(point.x + 1, point.y) - point.z) <= 1))
			{
				if (sum / double(num.size()) < Tnum + 5)
				{
					seedImg.at<uchar>(point.x + 1, point.y) = 255;
					temppoint.x = point.x + 1;
					temppoint.y = point.y;
					temppoint.z = image.at<uchar>(point.x + 1, point.y);
					seedd.push(temppoint);
					num.push_back(point);
					sum += temppoint.z;

				}

			}
		}
	}
	return seedImg;
}



void Pretreatment::ProduceItem(ItemRepository *ir, int item)
{
	std::unique_lock<std::mutex> lock(ir->mtx);
	while (((ir->write_position + 1) % kItemRepositorySize)
		== ir->read_position) { // item buffer is full, just wait here.
		/*std::cout << "Producer is waiting for an empty slot...\n";*/
		(ir->repo_not_full).wait(lock); // �����ߵȴ�"��Ʒ�⻺������Ϊ��"��һ��������.
	}

	(ir->item_buffer)[ir->write_position] = item; // д���Ʒ.

	Rect box = boundingRect(needContour[item]);
	Point pt7 = Point(box.x, box.y);
	Point pt8 = Point(box.x + box.width, box.y + box.height);
	Point pt_ROI_a = Point(0, 0);
	Point pt_ROI_b = Point(MidImg.cols - 1, MidImg.rows - 1);//��ȡԭʼ��

	if (pt7.x - 10 * box.width > 0)
		pt_ROI_a.x = pt7.x - 10 * box.width;
	if (pt7.y - 10 * box.height > 0)
		pt_ROI_a.y = pt7.y - 10 * box.height;
	if (pt8.x + 10 * box.width < MidImg.cols - 1)
		pt_ROI_b.x = pt8.x + 10 * box.width;
	if (pt8.y + 10 * box.height < MidImg.rows - 1)
		pt_ROI_b.y = pt8.y + 10 * box.height;

	Point centermark = 0.5 * (pt7 + pt8) - pt_ROI_a;//��boxImg�д������ͨ������ı��
	boxImg = MidImg(Rect(pt_ROI_a, pt_ROI_b));//��ȡ��ͨ�����������100���������
	Mat boxTImg = ThImg(Rect(pt_ROI_a, pt_ROI_b)).clone();
	Mat growImg = Grow(boxImg, boxTImg);//��������
	Mat growtemp = growImg.clone();

	vector<vector<cv::Point>> tempttours;
	findContours(growtemp, tempttours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	int maxszie = 0;
	int n = -1;
	for (size_t j = 0; j < tempttours.size(); j++)
	{
		Rect rectemp = boundingRect(tempttours[j]);
		if (rectemp.x < centermark.x&&rectemp.y < centermark.y)
		{
			if (rectemp.x + rectemp.width>centermark.x && rectemp.y + rectemp.height>centermark.y)
				n = j;
		}
	}
	if (n == -1)
	{
		for (size_t j = 0; j < tempttours.size(); j++)
		{
			if (maxszie < tempttours[j].size())
			{
				maxszie = tempttours[j].size();
				n = j;
			}
		}
	}
	vector<int> hullsI;
	vector<Vec4i> defects;
	vector<Point> hull;
	convexHull(tempttours[n], hull, false);
	convexHull(tempttours[n], hullsI, false);//Ѱ��͹��
	convexityDefects(tempttours[n], hullsI, defects);
	Point gpoint = barycenter1(hull);
	int countnum = countNonZero(growImg(Rect(gpoint - Point(1, 1), gpoint + Point(2, 2))));
	double asextent = matchShapes(tempttours[n], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//��Բ��״ƥ��
	if (countnum < 9 || asextent>1.0)
		return;

	int d1, d2, d3, d4;
	d1 = d2 = d3 = d4 = 1;//d1,d2,d3,d4�ֱ�������ң��ϣ���

	if (defects.size() != 0)
	{
		sort(defects.begin(), defects.end(), SortByM1);
		Point farpoint = tempttours[n][defects.back()[2]];
		Point startpoint = tempttours[n][defects.back()[0]];
		Point endpoint = tempttours[n][defects.back()[1]];
		Point diffpoint = farpoint - 0.5*(startpoint + endpoint);
		if (diffpoint.x >= 0 && diffpoint.y >= 0 && diffpoint.x > diffpoint.y)
			d1 = 5;
		if (diffpoint.x > 0 && diffpoint.y > 0 && diffpoint.x <= diffpoint.y)
			d3 = 5;
		if (diffpoint.x <= 0 && diffpoint.y >= 0 && abs(diffpoint.x) > diffpoint.y)
			d2 = 5;
		if (diffpoint.x < 0 && diffpoint.y > 0 && abs(diffpoint.x) <= diffpoint.y)
			d3 = 5;
		if (diffpoint.x >= 0 && diffpoint.y <= 0 && diffpoint.x > abs(diffpoint.y))
			d1 = 5;
		if (diffpoint.x > 0 && diffpoint.y < 0 && diffpoint.x <= abs(diffpoint.y))
			d4 = 5;
		if (diffpoint.x <= 0 && diffpoint.y <= 0 && diffpoint.x > diffpoint.y)
			d4 = 5;
		if (diffpoint.x < 0 && diffpoint.y < 0 && diffpoint.x <= diffpoint.y)
			d2 = 5;
	}//ѡ��ѡ�ź�Ľ�ȡ��Χ

	box = boundingRect(tempttours[n]);
	pt7 = Point(box.x, box.y) + pt_ROI_a;
	pt8 = Point(box.x + box.width, box.y + box.height) + pt_ROI_a;

	if (pt7.x - 0.4 *d1* box.width > 0)
		pt_ROI_a.x = pt7.x - 0.4 *d1* box.width;
	if (pt7.y - 0.4 *d3* box.height > 0)
		pt_ROI_a.y = pt7.y - 0.4 *d3* box.height;
	if (pt8.x + 0.4 *d2* box.width < MidImg.cols - 1)
		pt_ROI_b.x = pt8.x + 0.4 *d2* box.width;
	if (pt8.y + 0.4 *d4* box.height < MidImg.rows - 1)
		pt_ROI_b.y = pt8.y + 0.4 *d4* box.height;

	defectcenter.x = 0.5*(pt7.x + pt8.x) + recImg.x + maxRect.x + 2;//����任��ԭͼ
	defectcenter.y = 0.5*(pt7.y + pt8.y) + recImg.y + maxRect.y + 400;
	if (abs(pt8.y - pt7.y)>abs(pt8.x - pt7.x))
		defectcenter.z = abs(pt8.x - pt7.x);
	else
		defectcenter.z = abs(pt8.y - pt7.y);
	p2cp.clear();
	p2cp.push_back(defectcenter);
	Point3f pttemp;
	pttemp.x = pt_ROI_b.x;
	pttemp.y = pt_ROI_b.y;
	pttemp.z = 0;
	p2cp.push_back(pttemp);
	pttemp.x = pt_ROI_a.x;
	pttemp.y = pt_ROI_a.y;
	p2cp.push_back(pttemp);
	p2c.push_back(p2cp);

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

	Judgement JYON;
	Point pt_ROI_a, pt_ROI_b;
	pt_ROI_a.x = p2c.back()[2].x;
	pt_ROI_a.y = p2c.back()[2].y;
	pt_ROI_b.x = p2c.back()[1].x;
	pt_ROI_b.y = p2c.back()[1].y;
	boxImg = CMidImg(Rect(pt_ROI_a, pt_ROI_b));
	defectcenter = p2c.back()[0];
	p2c.pop_back();
	Faults::Hole hole;
	if (JYON.JudgementYON(boxImg))//�ж��Ƿ�Ϊȱ��
	{
		if (defectpoint.size() == 0)
		{
			/*defectpoint.push_back(defectcenter);*/
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
				/*defectpoint.push_back(defectcenter);*/
				hole.position.x = defectcenter.x;
				hole.position.y = defectcenter.y;
				hole.diameter = defectcenter.z;
				_faults->Holes.push_back(hole);
				/*cout << data << endl;*/
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
		/*std::cout << "Produce the " << i << "^th item..." << std::endl;*/
		ProduceItem(&gItemRepository, i);
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
	kItemsToProduce = needContour.size();
	InitItemRepository(&gItemRepository);
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // �����ȱ�ݵ�Ԥ����.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // ����ȱ����ˮ��.
	std::thread line(std::mem_fn(&Pretreatment::linedetect), this);//���ۼ��
	
	consumer.join();
	producer.join();
	line.join();
	needContour.clear();
	dilateneedcontours.clear();
	p2c.clear();
	p2cp.clear();
}
