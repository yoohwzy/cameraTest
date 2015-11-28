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




bool SortByM1(const Vec4i &v1, const Vec4i &v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1[3] < v2[3];//升序排列  
}

Point barycenter1(vector<Point> contoursi)//计算轮廓重心
{
	Moments m = moments(contoursi);
	Point center = Point(0, 0);
	center.x = (int)(m.m10 / m.m00);
	center.y = (int)(m.m01 / m.m00);
	return center;
}

int otsuThreshold(Mat &frame, MatND hist)//局部二值化的阈值选取
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

Mat Grow(Mat &image, Mat seedImg)//区域生长
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
		(ir->repo_not_full).wait(lock); // 生产者等待"产品库缓冲区不为满"这一条件发生.
	}

	(ir->item_buffer)[ir->write_position] = item; // 写入产品.

	Rect box = boundingRect(needContour[item]);
	Point pt7 = Point(box.x, box.y);
	Point pt8 = Point(box.x + box.width, box.y + box.height);
	Point pt_ROI_a = Point(0, 0);
	Point pt_ROI_b = Point(MidImg.cols - 1, MidImg.rows - 1);//截取原始点

	if (pt7.x - 10 * box.width > 0)
		pt_ROI_a.x = pt7.x - 10 * box.width;
	if (pt7.y - 10 * box.height > 0)
		pt_ROI_a.y = pt7.y - 10 * box.height;
	if (pt8.x + 10 * box.width < MidImg.cols - 1)
		pt_ROI_b.x = pt8.x + 10 * box.width;
	if (pt8.y + 10 * box.height < MidImg.rows - 1)
		pt_ROI_b.y = pt8.y + 10 * box.height;

	Point centermark = 0.5 * (pt7 + pt8) - pt_ROI_a;//在boxImg中待检测连通域的中心标记
	boxImg = MidImg(Rect(pt_ROI_a, pt_ROI_b));//截取连通域外界矩形面积100倍大的区域
	Mat boxTImg = ThImg(Rect(pt_ROI_a, pt_ROI_b)).clone();
	Mat growImg = Grow(boxImg, boxTImg);//区域生长
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
	convexHull(tempttours[n], hullsI, false);//寻找凸包
	convexityDefects(tempttours[n], hullsI, defects);
	Point gpoint = barycenter1(hull);
	int countnum = countNonZero(growImg(Rect(gpoint - Point(1, 1), gpoint + Point(2, 2))));
	double asextent = matchShapes(tempttours[n], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//椭圆形状匹配
	if (countnum < 9 || asextent>1.0)
		return;

	int d1, d2, d3, d4;
	d1 = d2 = d3 = d4 = 1;//d1,d2,d3,d4分别代表左，右，上，下

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
	}//选择选优后的截取范围

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

	defectcenter.x = 0.5*(pt7.x + pt8.x) + recImg.x + maxRect.x + 2;//坐标变换回原图
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

	(ir->write_position)++; // 写入位置后移.

	if (ir->write_position == kItemRepositorySize) // 写入位置若是在队列最后则重新设置为初始位置.
		ir->write_position = 0;

	(ir->repo_not_empty).notify_all(); // 通知消费者产品库不为空.
	lock.unlock(); // 解锁.
}

int Pretreatment::ConsumeItem(ItemRepository *ir)
{
	int data;
	std::unique_lock<std::mutex> lock(ir->mtx);
	// item buffer is empty, just wait here.
	while (ir->write_position == ir->read_position) {
		/*std::cout << "Consumer is waiting for items...\n";*/
		(ir->repo_not_empty).wait(lock); // 消费者等待"产品库缓冲区不为空"这一条件发生.
	}

	data = (ir->item_buffer)[ir->read_position]; // 读取某一产品

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
	if (JYON.JudgementYON(boxImg))//判断是否为缺陷
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

	(ir->read_position)++; // 读取位置后移

	if (ir->read_position >= kItemRepositorySize) // 读取位置若移到最后，则重新置位.
		ir->read_position = 0;

	(ir->repo_not_full).notify_all(); // 通知消费者产品库不为满.
	lock.unlock(); // 解锁.

	return data; // 返回产品.
}


void Pretreatment::ProducerTask() // 生产者任务
{
	for (size_t i = 0; i < needContour.size(); i++)
	{
		/*std::cout << "Produce the " << i << "^th item..." << std::endl;*/
		ProduceItem(&gItemRepository, i);
	}
}

void Pretreatment::ConsumerTask() // 消费者任务
{
	static int cnt = 1;
	while (1) {
		int item = ConsumeItem(&gItemRepository); // 消费一个产品.
		/*std::cout << "Consume the " << item << "^th item" << std::endl;*/
		if (++item == kItemsToProduce) break; // 如果产品消费个数为 kItemsToProduce, 则退出.
	}
}

void Pretreatment::InitItemRepository(ItemRepository *ir)
{
	ir->write_position = 0; // 初始化产品写入位置.
	ir->read_position = 0; // 初始化产品读取位置.
}


Mat thin(Mat &ROIImage)//细线化
{
	Mat dstImg = ROIImage.clone();


	int n = 0, i = 0, j = 0;
	for (n = 0; n < 40; n++)//开始进行迭代 40次 
	{
		Mat  t_image = dstImg.clone();
		for (i = 0; i < ROIImage.rows; i++)
		{
			for (j = 0; j < ROIImage.cols; j++)
			{
				if (t_image.at<uchar>(i, j) == 255)
				{
					int ap = 0;
					int p2 = (i == 0) ? 0 : t_image.at<uchar>(i - 1, j);
					int p3 = (i == 0 || j == ROIImage.cols - 1) ? 0 : t_image.at<uchar>(i - 1, j + 1);
					if (p2 == 0 && p3 == 255)
					{
						ap++;
					}

					int p4 = (j == ROIImage.cols - 1) ? 0 : t_image.at<uchar>(i, j + 1);
					if (p3 == 0 && p4 == 255)
					{
						ap++;
					}

					int p5 = (i == ROIImage.rows - 1 || j == ROIImage.cols - 1) ? 0 : t_image.at<uchar>(i + 1, j + 1);
					if (p4 == 0 && p5 == 255)
					{
						ap++;
					}

					int p6 = (i == ROIImage.rows - 1) ? 0 : t_image.at<uchar>(i + 1, j);
					if (p5 == 0 && p6 == 255)
					{
						ap++;
					}

					int p7 = (i == ROIImage.rows - 1 || j == 0) ? 0 : t_image.at<uchar>(i + 1, j - 1);
					if (p6 == 0 && p7 == 255)
					{
						ap++;
					}

					int p8 = (j == 0) ? 0 : t_image.at<uchar>(i, j - 1);
					if (p7 == 0 && p8 == 255)
					{
						ap++;
					}

					int p9 = (i == 0 || j == 0) ? 0 : t_image.at<uchar>(i - 1, j - 1);
					if (p8 == 0 && p9 == 255)
					{
						ap++;
					}
					if (p9 == 0 && p2 == 255)
					{
						ap++;
					}

					if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) > 255 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) < 1785)
					{
						if (ap == 1)
						{
							if (!(p2 && p4 && p6))
							{
								if (!(p4 && p6 && p8))
								{
									dstImg.at<uchar>(i, j) = 0;//设置目标图像中像素值为0的点  
								}
							}
						}
					}

				}
			}
		}
	}
	return dstImg;
}
Mat Hysteresis(Mat &image, Point seedpoint)//找到ROI最暗点，向周围8邻域生长
{
	Mat HyImg = image.clone();
	Scalar avgmean;
	Mat StdImg(HyImg.size(), CV_8UC1, Scalar(0));
	meanStdDev(HyImg, avgmean, StdImg);//找ROI的灰度平均值
	Mat SameImg(HyImg.size(), CV_8UC1, Scalar(0));//用来生长的全黑图
	vector<Point> seedq;//种子点序列
	seedq.push_back(seedpoint);
	while (!seedq.empty())//还有种子点
	{
		Point growpoint = seedq.back();
		seedq.pop_back();
		if ((growpoint.x > 0) && (growpoint.x < (image.cols - 1)) && (growpoint.y > 0) && (growpoint.y < (image.rows - 1)))//是否在ROI边缘上
		{
			if (HyImg.at<uchar>(growpoint.y, growpoint.x - 1) < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y, growpoint.x - 1) == 0 && HyImg.at<uchar>(growpoint.y, growpoint.x - 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y));//存入种子点序列
					SameImg.at<uchar>(growpoint.y, growpoint.x - 1) = 255;//标记该点
				}
			}

			if (HyImg.at<uchar>(growpoint.y, growpoint.x + 1) < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y, growpoint.x + 1) == 0 && HyImg.at<uchar>(growpoint.y, growpoint.x + 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y));
					SameImg.at<uchar>(growpoint.y, growpoint.x + 1) = 255;
				}
			}

			if (HyImg.at<uchar>(growpoint.y - 1, growpoint.x) < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y - 1, growpoint.x) == 0 && HyImg.at<uchar>(growpoint.y - 1, growpoint.x) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y - 1));
					SameImg.at<uchar>(growpoint.y - 1, growpoint.x) = 255;
				}
			}

			if (HyImg.at<uchar>(growpoint.y + 1, growpoint.x)  < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y + 1, growpoint.x) == 0 && HyImg.at<uchar>(growpoint.y + 1, growpoint.x) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y + 1));
					SameImg.at<uchar>(growpoint.y + 1, growpoint.x) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y - 1, growpoint.x - 1)  < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y - 1, growpoint.x - 1) == 0 && HyImg.at<uchar>(growpoint.y - 1, growpoint.x - 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y - 1));
					SameImg.at<uchar>(growpoint.y - 1, growpoint.x - 1) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y - 1, growpoint.x + 1)  < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y - 1, growpoint.x + 1) == 0 && HyImg.at<uchar>(growpoint.y - 1, growpoint.x + 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y - 1));
					SameImg.at<uchar>(growpoint.y - 1, growpoint.x + 1) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y + 1, growpoint.x - 1)  < avgmean[0] - 5)
			{
				if (SameImg.at<uchar>(growpoint.y + 1, growpoint.x - 1) == 0 && HyImg.at<uchar>(growpoint.y + 1, growpoint.x - 1) - HyImg.at<uchar>(growpoint.y, growpoint.x) <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y + 1));
					SameImg.at<uchar>(growpoint.y + 1, growpoint.x - 1) = 255;
				}
			}
			if (HyImg.at<uchar>(growpoint.y + 1, growpoint.x + 1)  < avgmean[0] - 5)
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


void Pretreatment::linedetect(Mat &image)
{

	Mat CcontourImg = image.clone();
	Rect cannyrect;
	int m = 0, linenumall = 0, linenum = 0;
	for (size_t i = 0; i < dilateneedcontours.size(); i++)
	{
		cannyrect = boundingRect(dilateneedcontours[i]);
		Mat waitImg = image(Rect(cannyrect));

		RotatedRect originalRect = minAreaRect(dilateneedcontours[i]);
		Point2f vertices[4];
		originalRect.points(vertices);

		double widthnum = sqrt((vertices[1].x - vertices[2].x)*(vertices[1].x - vertices[2].x) + (vertices[1].y - vertices[2].y)*(vertices[1].y - vertices[2].y));
		double hightnum = sqrt((vertices[3].x - vertices[2].x)*(vertices[3].x - vertices[2].x) + (vertices[3].y - vertices[2].y)*(vertices[3].y - vertices[2].y));
		if (double(widthnum / hightnum) < 3 && double(widthnum / hightnum) > 0.33)
			continue;


		/*waitImg = ~waitImg;*/
		Mat resultImg = thin(waitImg), proImg;
		int k = 1;
		if (resultImg.rows>resultImg.cols)
		{
			proImg = resultImg(Range(1, 1), Range::all());
		}
		else
		{
			proImg = resultImg(Range::all(), Range(1, 1));
			k = 0;
		}
		normalize(resultImg, resultImg, 1.0, 0.0, NORM_MINMAX);
		reduce(resultImg, proImg, k, CV_REDUCE_SUM, CV_32S);//若无该数据类型会报错
		Mat TproImg(resultImg.cols, 1, CV_8UC1, Scalar(0));
		proImg.convertTo(proImg, CV_8UC1);//threshold不支持32位
		linenumall = countNonZero(proImg);
		threshold(proImg, TproImg, 1, 255, CV_THRESH_BINARY);
		linenum = countNonZero(TproImg);
		m++;
		if (double(linenum) < double(0.7*linenumall))
		{
			Mat OrImg = MidImg(Rect(cannyrect));
			double LminV;
			Point Lminpoint;
			minMaxLoc(OrImg, &LminV, NULL, &Lminpoint);
			Mat minP_Img = Hysteresis(OrImg, Lminpoint);
			int Wmark = countNonZero(minP_Img);
			int colnum = (minP_Img.cols > minP_Img.rows) ? minP_Img.cols : minP_Img.rows;
			int cominnum = (minP_Img.cols < minP_Img.rows) ? minP_Img.cols : minP_Img.rows;
			colnum = (0.5*colnum > cominnum) ? 0.5*colnum : cominnum;
			if (Wmark < colnum || Wmark > 0.5*minP_Img.cols*minP_Img.rows)
				continue;

			Mat tent;

			vector<vector<Point>> hycontours;
			vector<Vec4i> hyhierarchy;
			Point pointSN = Point(0, 0);
			double maxs = 0;
			int maxnum = 0;
			findContours(minP_Img, hycontours, hyhierarchy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
			if (hycontours.size() == 0)
				continue;
			for (size_t j = 0; j < hycontours.size(); j++)
			{
				double area = cv::contourArea(hycontours[j]);
				if (maxs < area)
				{
					maxnum = j;
					maxs = area;
				}
			}
			int son = hyhierarchy[maxnum][2], Cmark = 0;;
			if (son >= 0)
			{
				for (size_t j = 0; j < hycontours.size(); j++)
				{
					if (hyhierarchy[j][3] == maxnum&&hycontours[j].size()>6)
						Cmark++;
				}

			}
			if (Cmark > 1)
				continue;
			originalRect = minAreaRect(hycontours[maxnum]);
			Point2f verticesgrow[4];
			originalRect.points(verticesgrow);
			if (cannyrect.x <= 5 || cannyrect.y <= 5 || cannyrect.x + cannyrect.width >= CcontourImg.cols - 5 || cannyrect.y + cannyrect.height >= CcontourImg.rows - 5)
				if ((verticesgrow[1].x - verticesgrow[2].x)*(verticesgrow[1].y - verticesgrow[2].y) == 0 || (verticesgrow[3].x - verticesgrow[2].x)*(verticesgrow[3].y - verticesgrow[2].y) == 0)
					continue;
			widthnum = sqrt((verticesgrow[1].x - verticesgrow[2].x)*(verticesgrow[1].x - verticesgrow[2].x) + (verticesgrow[1].y - verticesgrow[2].y)*(verticesgrow[1].y - verticesgrow[2].y));
			hightnum = sqrt((verticesgrow[3].x - verticesgrow[2].x)*(verticesgrow[3].x - verticesgrow[2].x) + (verticesgrow[3].y - verticesgrow[2].y)*(verticesgrow[3].y - verticesgrow[2].y));
			if (double(widthnum / hightnum) < 3 && double(widthnum / hightnum) > 0.33)
				continue;
			Faults::Scratch scratch;
			scratch.position.x = cannyrect.x + 0.5*cannyrect.width + recImg.x + maxRect.x + 2;
			scratch.position.y = cannyrect.y + 0.5*cannyrect.height + recImg.y + maxRect.y + 400;
			scratch.length = (cannyrect.width >cannyrect.height) ? 0.5*cannyrect.width : 0.5*cannyrect.height;
			_faults->Scratchs.push_back(scratch);
		}
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
	
	if (src.channels() == 3)//判断是否为彩图
		cvtColor(src, scr_gray, CV_RGB2GRAY);
	else
		scr_gray = src;
	vector<Point> pointlist;
	pointlist.push_back(_block->A);
	pointlist.push_back(_block->B);
	pointlist.push_back(_block->C);
	pointlist.push_back(_block->D);

	recImg = boundingRect(pointlist);//截取瓷砖区域

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
	add(ImgROI, Light, ImgROI);//光照补偿

	MidImg = ImgROI(Rect(2, 400, ImgROI.cols - 4, ImgROI.rows - 800));

	Mat CannyImg;
	ThImg = Mat(MidImg.size(), CV_8UC1, Scalar(0));//边缘检测
	Mat DilateImg = getStructuringElement(MORPH_RECT, Size(5, 5));

	Canny(MidImg, CannyImg, 5, 20);
	rectangle(CannyImg, Rect(Point(0, 0), Point(20, CannyImg.rows)), Scalar(0), -1);
	rectangle(CannyImg, Rect(Point(CannyImg.cols - 10, 0), Point(CannyImg.cols, CannyImg.rows)), Scalar(0), -1);//左右靠边边缘线舍弃

	dilate(CannyImg, CannyImg, DilateImg, Point(-1, -1), 2);

	vector<vector<cv::Point>> dilatecontours;
	vector<double> areanum;
	findContours(CannyImg, dilatecontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	Mat dilateImg(CannyImg.size(), CV_8UC1, Scalar(0));
	for (size_t i = 0; i < dilatecontours.size(); i++)
	{
		areanum.push_back(contourArea(dilatecontours[i]));
	}
	sort(areanum.begin(), areanum.end());
	for (size_t i = 0; i < dilatecontours.size(); i++)
	{
		if (contourArea(dilatecontours[i])>areanum[areanum.size() / 2] * 10)
			dilateneedcontours.push_back(dilatecontours[i]);
	}
	drawContours(dilateImg, dilateneedcontours, -1, Scalar(255), -1);
	erode(dilateImg, dilateImg, DilateImg, Point(-1, -1), 2);

	Mat ThImgROI, MidImgROI;
	Point Thpt = Point(0, 0);
	for (int i = 1; i <= 5; i++)//局部二值化，约束灰度变化量
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
	Mat anoImg = ThImg.clone();
	cv::findContours(anoImg, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);



	Mat cirlceImg(160, 160, CV_8UC1, Scalar(0));
	ellipse(cirlceImg, Point(80, 80), Size(40, 30), 90.0, 0, 360, Scalar(255), -1, 8);//画椭圆

	cv::findContours(cirlceImg, ecliptours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//比较轮廓的标准

	vector<double> listnum;
	for (size_t i = 0; i < decontours.size(); i++)
	{
		if (decontours[i].size() > 6)
		{
			double matchextent = matchShapes(decontours[i], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//比较待检测缺陷的形态
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
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // 待检测缺陷的预处理.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // 区分缺陷与水渍.
	std::thread line(std::mem_fn(&Pretreatment::linedetect),this, dilateImg);//划痕检测
	producer.join();
	consumer.join();
	line.join();
}
