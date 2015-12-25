#include "Pretreatment.h"
#include "Judgement.h"


using namespace cv;
using namespace std;


#undef max
#undef min

int kItemsToProduce = 9;   // How many items we plan to produce.
int m = 0;
vector<Point3f> repoint;
Mat MidImg, ThImg, LMidImg, PMidImg;
vector<vector<cv::Point>> needContour;
Rect recImg = Rect(Point(0, 0), Point(0, 0));
vector<vector<cv::Point>> dilateneedcontours;



bool SortBysize(const vector<Point>v1, const vector<Point>v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1.size() > v2.size();//降序排列  
}

bool WhetherLine(Mat &oneImg,Mat &G_Img,bool cor,bool boe)//判断是否为line的核心部分
{
	vector<Point> maxV_white_Points;
	if (countNonZero(oneImg) >= 2)//获取四边的白色点坐标
	{
		if (cor == 1)
		{
			for (int i = 0; i < oneImg.cols - 1; i++)
			{
				if (oneImg.at<uchar>(0, i) == 255 && oneImg.at<uchar>(0, i + 1) == 0)
					maxV_white_Points.push_back(Point(i + 0.2*oneImg.cols, 0));
			}
		}
		else
		{
			for (int i = 0; i < oneImg.rows - 1; i++)
			{
				if (oneImg.at<uchar>(i, 0) == 255 && oneImg.at<uchar>(i + 1, 0) == 0)
					maxV_white_Points.push_back(Point(0, i + 0.2*oneImg.rows));
			}
		}
	}
	Mat Ataxon, Ltaxon, Rtaxon;
	for (int i = 0; i < maxV_white_Points.size(); i++)
	{
		Point maxV_white_Point = maxV_white_Points[i];
		if (cor == 1 && boe == 0)//第一行
		{
			Ataxon = G_Img(Rect(Point(maxV_white_Point.x - 2, 0), Point(maxV_white_Point.x + 2, 4)));//包含该点的全4*4范围
			Ltaxon = G_Img(Rect(Point(maxV_white_Point.x - 2, 0), Point(maxV_white_Point.x, 4)));//包含该点的左2*4范围
			Rtaxon = G_Img(Rect(Point(maxV_white_Point.x, 0), Point(maxV_white_Point.x + 2, 4)));//包含该点的右2*4范围
		}
		else if (cor == 1 && boe == 1)//最后一行
		{
			Ataxon = G_Img(Rect(Point(maxV_white_Point.x - 2, G_Img.rows - 5), Point(maxV_white_Point.x + 2, G_Img.rows - 1)));
			Ltaxon = G_Img(Rect(Point(maxV_white_Point.x - 2, G_Img.rows - 5), Point(maxV_white_Point.x, G_Img.rows - 1)));
			Rtaxon = G_Img(Rect(Point(maxV_white_Point.x, G_Img.rows - 5), Point(maxV_white_Point.x + 2, G_Img.rows - 1)));
		}
		else if (cor == 0 && boe == 0)//第一列
		{
			Ataxon = G_Img(Rect(Point(0, maxV_white_Point.y - 2), Point(4, maxV_white_Point.y + 2)));
			Ltaxon = G_Img(Rect(Point(0, maxV_white_Point.y - 2), Point(4, maxV_white_Point.y)));
			Rtaxon = G_Img(Rect(Point(0, maxV_white_Point.y), Point(4, maxV_white_Point.y + 2)));
		}
		else if (cor == 0 && boe == 1)//最后一列
		{
			Ataxon = G_Img(Rect(Point(G_Img.cols - 5, maxV_white_Point.y - 2), Point(G_Img.cols - 1, maxV_white_Point.y + 2)));
			Ltaxon = G_Img(Rect(Point(G_Img.cols - 5, maxV_white_Point.y - 2), Point(G_Img.cols - 1, maxV_white_Point.y)));
			Rtaxon = G_Img(Rect(Point(G_Img.cols - 5, maxV_white_Point.y), Point(G_Img.cols - 1, maxV_white_Point.y + 2)));
		}
		if (countNonZero(Ataxon) != countNonZero(Ltaxon) && countNonZero(Ataxon) != countNonZero(Rtaxon))//当边界上点的左包含不等于全包含且右包含不等于全包含时可知该点不是端点
			return 1;
	}
	return 0;
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

		if (pixelPro[i] > 0.00015)
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

Mat Grow(Mat &image, Point seedpoint, double th_v)//区域生长
{
	Mat HyImg = image.clone();
	Mat SameImg(HyImg.size(), CV_8UC1, Scalar(0));
	vector<Point> seedq;
	seedq.push_back(seedpoint);
	while (!seedq.empty())
	{
		Point growpoint = seedq[seedq.size() - 1];
		seedq.pop_back();
		uchar* data = HyImg.ptr<uchar>(growpoint.y);//灰度图本行首地址
		uchar* dataT = SameImg.ptr<uchar>(growpoint.y);//二值图本行首地址
		if ((growpoint.x > 0) && (growpoint.x < (image.cols - 1)) && (growpoint.y > 0) && (growpoint.y < (image.rows - 1)))
		{
			if (data[growpoint.x - 1] < th_v)
			{
				if (dataT[growpoint.x - 1] == 0 && data[growpoint.x - 1] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y));
					dataT[growpoint.x - 1] = 255;
				}
			}

			if (data[growpoint.x + 1] < th_v)
			{
				if (dataT[growpoint.x + 1] == 0 && data[growpoint.x + 1] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y));
					dataT[growpoint.x + 1] = 255;
				}
			}

			uchar* dataU = HyImg.ptr<uchar>(growpoint.y - 1);//灰度图上一行首地址
			uchar* dataUT = SameImg.ptr<uchar>(growpoint.y - 1);//二值图上一行首地址

			if (dataU[growpoint.x] < th_v)
			{
				if (dataUT[growpoint.x] == 0 && dataU[growpoint.x] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y - 1));
					dataUT[growpoint.x] = 255;
				}
			}
			if (dataU[growpoint.x - 1] < th_v)
			{
				if (dataUT[growpoint.x - 1] == 0 && dataU[growpoint.x - 1] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y - 1));
					dataUT[growpoint.x - 1] = 255;
				}
			}

			if (dataU[growpoint.x + 1]   < th_v)
			{
				if (dataUT[growpoint.x + 1] == 0 && dataU[growpoint.x + 1] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y - 1));
					dataUT[growpoint.x + 1] = 255;
				}
			}

			uchar* dataD = HyImg.ptr<uchar>(growpoint.y + 1);//灰度图下一行首地址
			uchar* dataDT = SameImg.ptr<uchar>(growpoint.y + 1);//二值图下一行首地址

			if (dataD[growpoint.x] < th_v)
			{
				if (dataDT[growpoint.x] == 0 && dataD[growpoint.x] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y + 1));
					dataDT[growpoint.x] = 255;
				}
			}

			if (dataD[growpoint.x - 1] < th_v)
			{
				if (dataDT[growpoint.x - 1] == 0 && dataD[growpoint.x - 1] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y + 1));
					dataDT[growpoint.x - 1] = 255;
				}
			}
			if (dataD[growpoint.x + 1] < th_v)
			{
				if (dataDT[growpoint.x + 1] == 0 && dataD[growpoint.x + 1] - data[growpoint.x] <= 2)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y + 1));
					dataDT[growpoint.x + 1] = 255;
				}
			}
		}


	}
	return SameImg;
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

	needContour.push_back(locationpoints);
	Warehousecontours.push_back(needContour);
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

	CneedContours = Warehousecontours.back();
	Warehousecontours.pop_back();
	Point locationpoint = CneedContours.back()[0];
	CneedContours.pop_back();
	for (size_t i = 0; i < CneedContours.size(); i++)
	{
		/*cout << i << endl;*/
		Rect box = boundingRect(CneedContours[i]);
		Point pt7 = Point(box.x, box.y);
		Point pt8 = Point(box.x + box.width, box.y + box.height);
		Point pt_ROI_a = Point(0, 0);
		Point pt_ROI_b = Point(PMidImg.cols / 2 - 1, PMidImg.rows / 5 - 1);//截取原始点

		if (pt7.x - 2 * box.width > 0)
			pt_ROI_a.x = pt7.x - 2 * box.width;
		if (pt7.y - 2 * box.height > 0)
			pt_ROI_a.y = pt7.y - 2 * box.height;
		if (pt8.x + 2 * box.width < MidImg.cols / 2 - 1)
			pt_ROI_b.x = pt8.x + 2 * box.width;
		if (pt8.y + 2 * box.height < MidImg.rows / 5 - 1)
			pt_ROI_b.y = pt8.y + 2 * box.height;

		Point centermark = 0.5 * (pt7 + pt8) - pt_ROI_a;//在boxImg中待检测连通域的中心标记
		Mat ready_boxImg = PMidImg(Rect(pt_ROI_a + locationpoint, pt_ROI_b + locationpoint));//截取连通域外界矩形面积25倍大的区域
		Scalar avgmean;
		avgmean = mean(ready_boxImg);
		double min_v = 0.0, max_v = 0.0, input_th_v = avgmean[0] - 5;
		minMaxLoc(ready_boxImg, &min_v, &max_v);
		if (avgmean[0] - min_v > 10)
		{
			if (max_v - min_v > 20)
				input_th_v = avgmean[0] - 5;
			else
				input_th_v = avgmean[0] - 7;
		}
		else
			input_th_v = (min_v + 3 > 0.5*(min_v + avgmean[0])) ? 0.5*(min_v + avgmean[0]) : min_v + 3;

		Mat growImg = Grow(ready_boxImg, centermark, input_th_v);//区域生长

		int whitenum = countNonZero(growImg);
		Mat growtemp = growImg.clone();

		/*erode(growtemp, growtemp,Mat(),Point(-1,-1),2);*/
		vector<vector<cv::Point>> tempttours;
		findContours(growtemp, tempttours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if (tempttours.size() == 0 || countNonZero(growImg) <= 4)
		{
			continue;
		}
		Rect growT_RECT = boundingRect(tempttours[0]);
		double proportion = double(growT_RECT.width) / double(growT_RECT.height);
		if (proportion > 3 || proportion < 0.33)
		{
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
			continue;
		}
		Mat biggerImg;
		vector<vector<Point>> biggercontours;
		resize(growImg, biggerImg, Size(growImg.cols *1.2, growImg.rows * 1.2), 0, 0, INTER_LINEAR);
		findContours(biggerImg, biggercontours, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		Mat mask_point_Img(biggerImg.size(),CV_8U,Scalar(0));
		Mat mask_point_ImgROI = mask_point_Img(Rect(Point(growImg.cols*0.1, growImg.rows*0.1), Point(growImg.cols*1.1, growImg.rows*1.1)));
		drawContours(mask_point_Img, biggercontours, -1, Scalar(255), -1);
		drawContours(mask_point_ImgROI, tempttours, -1, Scalar(0), -1);

		Mat ano_boxImg = ready_boxImg.clone();
		Scalar mount;
		mount = mean(ano_boxImg, mask_point_ImgROI);//分析环状区域的平均灰度值
		if (mount[0] < 0.95*avgmean[0])
			continue;
		if (2 * min_v > avgmean[0])
			continue;
		bitwise_and(ano_boxImg, growImg, ano_boxImg);
		threshold(ano_boxImg, ano_boxImg, 0.5*(avgmean[0] + min_v), 255, 0);
		bitwise_not(ano_boxImg, ano_boxImg, growImg);
		vector<vector<cv::Point>> lowTcontour;
		findContours(ano_boxImg, lowTcontour, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		int size_v = 1;
		sort(lowTcontour.begin(), lowTcontour.end(), SortBysize);
		for (size_t j = 1; j < lowTcontour.size(); j++)
		{
			if (contourArea(lowTcontour[0]) > 500 * contourArea(lowTcontour[j]))
				break;
			else
				size_v++;
		}
		if (size_v != 1)
		{
			continue;
		}
		Point pt9 = Point(growT_RECT.x, growT_RECT.y) + pt_ROI_a;
		Point pt10 = Point(growT_RECT.x + growT_RECT.width, growT_RECT.y + growT_RECT.height) + pt_ROI_a;
		Faults::Hole hole;
		hole.position.x = 0.5*(pt9.x + pt10.x) + recImg.x + locationpoint.x;//坐标变换回原图
		hole.position.y = 0.5*(pt9.y + pt10.y) + recImg.y + locationpoint.y;
		if (growT_RECT.height > growT_RECT.width)
			hole.diameter = growT_RECT.height;
		else
			hole.diameter = growT_RECT.height;
		Point hole_torect_a(hole.position.x - 0.5 * growT_RECT.width, hole.position.y - 0.5 * growT_RECT.height);
		Point hole_torect_b(hole.position.x - 0.5 * growT_RECT.width, hole.position.y + 0.5 * growT_RECT.height);
		Point hole_torect_c(hole.position.x + 0.5 * growT_RECT.width, hole.position.y - 0.5 * growT_RECT.height);
		Point hole_torect_d(hole.position.x + 0.5 * growT_RECT.width, hole.position.y + 0.5 * growT_RECT.height);
		if (pointPolygonTest(pointlist, hole_torect_a, 0) == 1 && pointPolygonTest(pointlist, hole_torect_b, 0) == 1)//检测该凹点是否在瓷砖内部，防止瓷砖倾斜误判
			if (pointPolygonTest(pointlist, hole_torect_c, 0) == 1 && pointPolygonTest(pointlist, hole_torect_d, 0) == 1)
				_faults->Holes.push_back(hole);

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
	ThImg = Mat(MidImg.size(), CV_8UC1, Scalar(0));//二值化原图
	Mat DilateImg = getStructuringElement(MORPH_RECT, Size(3, 3));
	if (_faults->MarkPens.size() != 0)
	{
		double t = (double)cv::getTickCount();
		cv::max(MidImg, Mask_result_big, MidImg);
		/*bitwise_and(MidImg, Mask_result_big, MidImg);
		Mat table;
		CreateLookupTable(table);
		LUT(MidImg, table, MidImg);*/
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cout << t << endl;
	}

	Mat ThImgROI, MidImgROI;
	Point Thpt = Point(0, 0);
	int step = 0;
	for (int i = 1; i <= 5; i++)//局部二值化，约束灰度变化量
	{
		for (int j = 0; j < 2; j++)
		{
			needContour.clear();
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

			threshold(MidImgROI, ThImgROI, OtsuV, 255, CV_THRESH_BINARY_INV);
			vector<vector<cv::Point>> decontours;

			cv::findContours(ThImgROI, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			for (size_t k = 0; k < decontours.size(); k++)
			{
				if (decontours[k].size() > 8)
				{
					double matchextent = matchShapes(decontours[k], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//比较待检测缺陷的形态
					if (matchextent < 1.0)
					{
						needContour.push_back(decontours[k]);
					}

				}

			}
			locationpoints.push_back(Point(ThRect.x, ThRect.y));
			ProduceItem(&gItemRepository, step);
			step++;
			locationpoints.clear();
		}
		Thpt.y = i*int(MidImg.rows*0.2);
	}
}

void Pretreatment::ConsumerTask() // 消费者任务
{
	static int cnt = 1;
	while (1) {
		int item = ConsumeItem(&gItemRepository); // 消费一个产品.
		if (++item == kItemsToProduce) break; // 如果产品消费个数为 kItemsToProduce, 则退出.
	}
}

void Pretreatment::InitItemRepository(ItemRepository *ir)
{
	ir->write_position = 0; // 初始化产品写入位置.
	ir->read_position = 0; // 初始化产品读取位置.
}


void Pretreatment::linedetect()
{
	vector<vector<cv::Point>> dilatecontours;
	findContours(BlurImg, dilatecontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	int move_n = 0;
	if (dilatecontours.size() > 100)
		move_n = 20;
	else
		move_n = int(dilatecontours.size()*0.5);
	partial_sort(dilatecontours.begin(), dilatecontours.begin() + move_n, dilatecontours.end(), SortBysize);
	Mat dilateImg(BlurImg.size(), CV_8UC1, Scalar(0));

	vector<vector<Point>> bigcontours;
	bigcontours.insert(bigcontours.end(), dilatecontours.begin(), dilatecontours.begin() + move_n);
	drawContours(dilateImg, bigcontours, -1, Scalar(255), -1);

	if (_faults->MarkPens.size() != 0)
	{
		bitwise_and(dilateImg, Mask_result_small, dilateImg);
		bigcontours.clear();
		findContours(dilateImg, bigcontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	}

	Mat CcontourImg = BlurImg.clone();
	Rect cannyrect;
	int m = 0, linenumall = 0, linenum = 0;
	for (size_t i = 0; i < bigcontours.size(); i++)
	{

		cannyrect = boundingRect(bigcontours[i]);
		Mat waitImg = dilateImg(Rect(cannyrect));
		if (!countNonZero(waitImg))
			continue;
		if ((cannyrect.width < 50 && cannyrect.height < 50) || cannyrect.width < 20 || cannyrect.height < 20)
			continue;
		int matchlinepro = matchShapes(bigcontours[i], Linecontours[0], CV_CONTOURS_MATCH_I1, 0);
		if (matchlinepro> 1.0)
			continue;
		Mat fingerprint_Img = CannyImg(Rect(cannyrect));//防止指纹误检
		LUT(fingerprint_Img, lookUpTable, fingerprint_Img);
		threshold(fingerprint_Img, fingerprint_Img, CannyImg.at<uchar>(bigcontours[i][0]), 255, 0);
		if (countNonZero(fingerprint_Img) <= 0.6 * fingerprint_Img.cols * fingerprint_Img.rows)
			continue;
		Mat I_rows = waitImg(Range(0, 1), Range(int(waitImg.cols*0.2), int(waitImg.cols*0.8)));//第一行
		Mat II_rows = waitImg(Range(waitImg.rows - 1, waitImg.rows), Range(int(waitImg.cols*0.2), int(waitImg.cols*0.8)));//最后一行
		Mat I_cols = waitImg(Range(int(waitImg.rows*0.2), int(waitImg.rows*0.8)), Range(0, 1));//第一列
		Mat II_cols = waitImg(Range(int(waitImg.rows*0.2), int(waitImg.rows*0.8)), Range(waitImg.cols - 1, waitImg.cols));//最后一列
		if (countNonZero(I_rows) != 0)
		{
			if (WhetherLine(I_rows, waitImg,1,0))
				continue;
		}
		if (countNonZero(I_cols) != 0)
		{
			if (WhetherLine(I_cols, waitImg, 0,0))
				continue;
		}
		if (countNonZero(II_rows) != 0)
		{
			if (WhetherLine(II_rows, waitImg,1,1))
				continue;
		}
		if (countNonZero(II_cols) != 0)
		{
			if (WhetherLine(II_cols, waitImg,0,1))
				continue;
		}
		if (countNonZero(I_rows) && countNonZero(I_cols) && countNonZero(II_rows) && countNonZero(II_cols))
			continue;
		Faults::Scratch scratch;
		scratch.position.x = 4 * cannyrect.x + 2 * cannyrect.width + recImg.x;
		scratch.position.y = 4 * cannyrect.y + 2 * cannyrect.height + recImg.y;
		scratch.length = (cannyrect.width >cannyrect.height) ? 2 * cannyrect.width : 2 * cannyrect.height;
		Point torect_a(scratch.position.x - 2 * cannyrect.width, scratch.position.y - 2 * cannyrect.height);
		Point torect_b(scratch.position.x - 2 * cannyrect.width, scratch.position.y + 2 * cannyrect.height);
		Point torect_c(scratch.position.x + 2 * cannyrect.width, scratch.position.y - 2 * cannyrect.height);
		Point torect_d(scratch.position.x + 2 * cannyrect.width, scratch.position.y + 2 * cannyrect.height);
		if (pointPolygonTest(pointlist, torect_a, 0) == 1 && pointPolygonTest(pointlist, torect_b, 0) == 1)//检测该划痕 是否在瓷砖上，防止瓷砖倾斜误判
			if (pointPolygonTest(pointlist, torect_c, 0) == 1 && pointPolygonTest(pointlist, torect_d, 0) == 1)
				_faults->Scratchs.push_back(scratch);
	}
}

void Pretreatment::tem_2plate()//模板轮廓匹配预处理线程
{
	Mat cirlceImg(160, 160, CV_8UC1, Scalar(0));
	ellipse(cirlceImg, Point(80, 80), Size(40, 30), 90.0, 0, 360, Scalar(255), -1, 8);//画椭圆
	cv::findContours(cirlceImg, ecliptours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//比较轮廓的标准
	Mat LineImg(160, 160, CV_8UC1, Scalar(0));
	line(LineImg, Point(4, 80), Point(156, 80), Scalar(255), 2, 8);//画直线
	cv::findContours(LineImg, Linecontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//比较轮廓的标准
}

void Pretreatment::img2clone()//待处理大图复制线程
{
	PMidImg = MidImg.clone();
	lookUpTable = Mat(1, 256, CV_8U);
	uchar* p = lookUpTable.data;
	p[0] = 255;
	for (int j = 1; j < 256; j++)
	{
		p[j] = j;
	}
	//LMidImg = PMidImg.clone();
}

void Pretreatment::img2zoom()//划痕检测缩放预处理
{
	resize(MidImg, CannyImg, Size(MidImg.cols / 4, MidImg.rows / 4), 0, 0, INTER_LINEAR);
	blur(CannyImg, BlurImg, Size(3, 3));
	Canny(BlurImg, BlurImg, 7, 20);
	/*bilateralFilter(CannyImg, BlurImg, 6, 12, 3);*/
}




Pretreatment::~Pretreatment()
{
}

void Pretreatment::pretreatment(Mat &image, Block *_block, Faults *faults)
{
	std::thread tem_plate(std::mem_fn(&Pretreatment::tem_2plate), this);
	_faults = faults;

	if (image.channels() == 3)//判断是否为彩图
		cvtColor(image, image, CV_RGB2GRAY);

	pointlist.push_back(_block->A);
	pointlist.push_back(_block->B);
	pointlist.push_back(_block->C);
	pointlist.push_back(_block->D);

	recImg = boundingRect(pointlist);//截取瓷砖区域,对拍摄不全的区域也进行截取
	if (recImg.x > 0)
		recImg.x += 100;
	if (recImg.y > 0)
		recImg.y += 400;
	if (recImg.x + recImg.width < image.cols && recImg.x + recImg.width > 200)
	{
		if (recImg.x > 0)
			recImg.width -= 200;
		else
			recImg.width -= 100;
	}
	else if (recImg.x + recImg.width >= image.cols && recImg.x > 0)
		recImg.width -= 100;

	if (recImg.y + recImg.height < image.rows && recImg.y + recImg.height > 800)
	{
		if (recImg.y > 0)
			recImg.height -= 800;
		else
			recImg.height -= 400;
	}
	else if (recImg.y + recImg.height >= image.rows && recImg.y > 0)
		recImg.height -= 400;

	MidImg = image(Rect(recImg));

	std::thread imgzoom(std::mem_fn(&Pretreatment::img2zoom), this);

	Mat MaskImg, Mask2CannyImg;
	Mask_result_big = Mat(MidImg.size(), CV_8UC1, Scalar(0));
	Mask_result_small = Mat(MidImg.rows / 4, MidImg.cols / 4, CV_8UC1, Scalar(255));
	resize(MidImg, MaskImg, Size(MidImg.cols / 16, MidImg.rows / 16), 0, 0, INTER_LINEAR);

	tem_plate.join();
	std::thread imgclone(std::mem_fn(&Pretreatment::img2clone), this);

	GaussianBlur(MaskImg, MaskImg, Size(5, 5), 0, 0);
	Canny(MaskImg, MaskImg, 40, 50);
	vector<vector<Point>> Maskcontours;
	findContours(MaskImg, Maskcontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	if (Maskcontours.size() != 0)
	{
		partial_sort(Maskcontours.begin(), Maskcontours.begin() + 1, Maskcontours.end(), SortBysize);
		if (Maskcontours.front().size() > 30)
		{
			for (size_t i = 0; i < Maskcontours.size(); i++)
			{
				if (Maskcontours[i].size() > 30)
				{
					Rect mask_rect = boundingRect(Maskcontours[i]);
					mask_rect.x = (4 * mask_rect.x - 16 > 0) ? 4 * mask_rect.x - 16 : 0;
					mask_rect.y = (4 * mask_rect.y - 16 > 0) ? 4 * mask_rect.y - 16 : 0;
					mask_rect.width = (4 * mask_rect.width + 32 < Mask_result_small.cols) ? 4 * mask_rect.width + 32 : Mask_result_small.cols;
					mask_rect.height = (4 * mask_rect.height + 32 < Mask_result_small.rows) ? 4 * mask_rect.height + 32 : Mask_result_small.rows;
					rectangle(Mask_result_small, mask_rect, Scalar(0), -1);
					mask_rect.x = (4 * mask_rect.x - 16 > 0) ? 4 * mask_rect.x - 16 : 0;
					mask_rect.y = (4 * mask_rect.y - 16 > 0) ? 4 * mask_rect.y - 16 : 0;
					mask_rect.width = (4 * mask_rect.width + 32 < Mask_result_big.cols) ? 4 * mask_rect.width + 32 : Mask_result_big.cols;;
					mask_rect.height = (4 * mask_rect.height + 32 < Mask_result_big.rows) ? 4 * mask_rect.height + 32 : Mask_result_big.rows;;
					rectangle(Mask_result_big, mask_rect, Scalar(255), -1);
					Faults::MarkPen markpen;
					mask_rect.x += recImg.x;
					mask_rect.y += recImg.y;
					markpen.markposition = mask_rect;
					Point outpoint_a(mask_rect.x, mask_rect.y), outpoint_b(mask_rect.x + mask_rect.width, mask_rect.y + mask_rect.height);
					Point outpoint_c(mask_rect.x + mask_rect.width, mask_rect.y), outpoint_d(mask_rect.x, mask_rect.y + mask_rect.height);
					int out_mark = 0;
					while (1)//标记重复框取判断
					{
						if (in_or_out.size() == 0)
							break;
						if (pointPolygonTest(in_or_out, 0.5*(outpoint_a + outpoint_b), 0) == 0)
						{
							out_mark = 1;
							break;
						}
						if (pointPolygonTest(in_or_out, 0.5*(outpoint_b + outpoint_c), 0) == 0)
						{
							out_mark = 1;
							break;
						}
						if (pointPolygonTest(in_or_out, 0.5*(outpoint_c + outpoint_d), 0) == 0)
						{
							out_mark = 1;
							break;
						}
						if (pointPolygonTest(in_or_out, 0.5*(outpoint_d + outpoint_a), 0) == 0)
						{
							out_mark = 1;
							break;
						}
						break;
					}
					if (out_mark == 1)
					{
						if (pointPolygonTest(in_or_out, outpoint_a, 0) == 1)
							_faults->MarkPens.pop_back();
						else
							continue;
					}
					in_or_out.clear();
					in_or_out.push_back(outpoint_a);
					in_or_out.push_back(outpoint_b);
					in_or_out.push_back(outpoint_c);
					in_or_out.push_back(outpoint_d);
					if (pointPolygonTest(pointlist, outpoint_a, 0) == 1 && pointPolygonTest(pointlist, outpoint_b, 0) == 1)//检测该标记是否在瓷砖上，防止瓷砖倾斜误判
						if (pointPolygonTest(pointlist, outpoint_c, 0) == 1 && pointPolygonTest(pointlist, outpoint_d, 0) == 1)
							_faults->MarkPens.push_back(markpen);
				}
			}
		}
	}
	imgclone.join();
	imgzoom.join();
	InitItemRepository(&gItemRepository);
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // 待检测缺陷的预处理.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // 区分缺陷与水渍.
	std::thread line(std::mem_fn(&Pretreatment::linedetect), this);//划痕检测 
	/*auto tn = line.native_handle();
	SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);*///线程优先级调整
	producer.join();
	line.join();
	consumer.join();
	needContour.clear();
	dilateneedcontours.clear();
	CneedContours.clear();
	Warehousecontours.clear();
}
