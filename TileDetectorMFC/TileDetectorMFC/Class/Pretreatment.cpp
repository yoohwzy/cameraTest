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
vector<vector<cv::Point>> ecliptours;
vector<vector<cv::Point>> dilateneedcontours;



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

Mat Grow(Mat &image, Point seedpoint, double th_v)//��������
{
	Mat HyImg = image.clone();
	Mat SameImg(HyImg.size(), CV_8UC1, Scalar(0));
	vector<Point> seedq;
	seedq.push_back(seedpoint);
	while (!seedq.empty())
	{
		Point growpoint = seedq.back();
		seedq.pop_back();
		uchar* data = HyImg.ptr<uchar>(growpoint.y);//�Ҷ�ͼ�����׵�ַ
		uchar* dataT = SameImg.ptr<uchar>(growpoint.y);//��ֵͼ�����׵�ַ
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

			uchar* dataU = HyImg.ptr<uchar>(growpoint.y - 1);//�Ҷ�ͼ��һ���׵�ַ
			uchar* dataUT = SameImg.ptr<uchar>(growpoint.y - 1);//��ֵͼ��һ���׵�ַ

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

			uchar* dataD = HyImg.ptr<uchar>(growpoint.y + 1);//�Ҷ�ͼ��һ���׵�ַ
			uchar* dataDT = SameImg.ptr<uchar>(growpoint.y + 1);//��ֵͼ��һ���׵�ַ

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
		(ir->repo_not_full).wait(lock); // �����ߵȴ�"��Ʒ�⻺������Ϊ��"��һ��������.
	}

	(ir->item_buffer)[ir->write_position] = item; // д���Ʒ.

	needContour.push_back(locationpoints);
	Warehousecontours.push_back(needContour);
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
		Point pt_ROI_b = Point(PMidImg.cols / 2 - 1, PMidImg.rows / 5 - 1);//��ȡԭʼ��

		if (pt7.x - 10 * box.width > 0)
			pt_ROI_a.x = pt7.x - 10 * box.width;
		if (pt7.y - 10 * box.height > 0)
			pt_ROI_a.y = pt7.y - 10 * box.height;
		if (pt8.x + 10 * box.width < MidImg.cols / 2 - 1)
			pt_ROI_b.x = pt8.x + 10 * box.width;
		if (pt8.y + 10 * box.height < MidImg.rows / 5 - 1)
			pt_ROI_b.y = pt8.y + 10 * box.height;

		Point centermark = 0.5 * (pt7 + pt8) - pt_ROI_a;//��boxImg�д������ͨ������ı��
		Mat ready_boxImg = PMidImg(Rect(pt_ROI_a + locationpoint, pt_ROI_b + locationpoint));//��ȡ��ͨ�����������100���������
		Scalar avgmean;
		avgmean = mean(ready_boxImg);
		Mat growImg = Grow(ready_boxImg, centermark, avgmean[0] - 5);//��������

		int whitenum = countNonZero(growImg);
		Mat growtemp = growImg.clone();

		/*erode(growtemp, growtemp,Mat(),Point(-1,-1),2);*/
		vector<vector<cv::Point>> tempttours;
		findContours(growtemp, tempttours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
		if (tempttours.size() == 0 || countNonZero(growImg) <= 2)
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
		Mat ano_boxImg = ready_boxImg.clone();
		double min_v = 0.0;
		minMaxLoc(ano_boxImg, &min_v);
		if (2 * min_v > avgmean[0])
			continue;
		bitwise_and(ano_boxImg, growImg, ano_boxImg);
		threshold(ano_boxImg, ano_boxImg, 0.5*(avgmean[0] + min_v), 255, 0);
		bitwise_not(ano_boxImg, ano_boxImg, growImg);
		vector<vector<cv::Point>> lowTcontour;
		findContours(ano_boxImg, lowTcontour, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
		int size_v = lowTcontour.size();
		sort(lowTcontour.begin(), lowTcontour.end(), SortBysize);
		for (size_t j = 1; j < lowTcontour.size(); j++)
		{
			if (contourArea(lowTcontour[0]) > 1000 * contourArea(lowTcontour[j]))
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
		hole.position.x = 0.5*(pt9.x + pt10.x) + recImg.x + locationpoint.x;//����任��ԭͼ
		hole.position.y = 0.5*(pt9.y + pt10.y) + recImg.y + locationpoint.y;
		if (growT_RECT.height > growT_RECT.width)
			hole.diameter = growT_RECT.height;
		else
			hole.diameter = growT_RECT.height;
		_faults->Holes.push_back(hole);

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
	ThImg = Mat(MidImg.size(), CV_8UC1, Scalar(0));//��ֵ��ԭͼ
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
	Mat cirlceImg(160, 160, CV_8UC1, Scalar(0));
	ellipse(cirlceImg, Point(80, 80), Size(40, 30), 90.0, 0, 360, Scalar(255), -1, 8);//����Բ
	cv::findContours(cirlceImg, ecliptours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//�Ƚ������ı�׼

	Mat ThImgROI, MidImgROI;
	Point Thpt = Point(0, 0);
	int step = 0;
	for (int i = 1; i <= 5; i++)//�ֲ���ֵ����Լ���Ҷȱ仯��
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
			int OtsuV = otsuThreshold(MidImgROI, histolist);//�ҵ���ɢ�Ҷ�ֵ��������ֵ

			threshold(MidImgROI, ThImgROI, OtsuV, 255, CV_THRESH_BINARY_INV);
			vector<vector<cv::Point>> decontours;

			cv::findContours(ThImgROI, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			vector<double> listnum;
			for (size_t k = 0; k < decontours.size(); k++)
			{
				if (decontours[k].size() > 8)
				{
					double matchextent = matchShapes(decontours[k], ecliptours[0], CV_CONTOURS_MATCH_I3, 0);//�Ƚϴ����ȱ�ݵ���̬
					listnum.push_back(matchextent);

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
	Mat CannyImg, BlurImg;
	resize(LMidImg, CannyImg, Size(MidImg.cols / 4, MidImg.rows / 4), 0, 0, INTER_LINEAR);
	bilateralFilter(CannyImg, BlurImg, 6, 12, 3);
	Canny(BlurImg, BlurImg, 5, 20);

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

	if (_faults->MarkPens.size() != 0)
	{
		bitwise_and(dilateImg, Mask_result_small, dilateImg);
		bigcontours.clear();
		findContours(dilateImg, bigcontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	}


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
		if (!countNonZero(waitImg))
			continue;
		if (cannyrect.width < 50 && cannyrect.height < 50)
			continue;
		int matchlinepro = matchShapes(bigcontours[i], Linecontours[0], CV_CONTOURS_MATCH_I1, 0);
		if (matchlinepro> 1.0)
			continue;
		Faults::Scratch scratch;
		scratch.position.x = 4 * cannyrect.x + 2 * cannyrect.width + recImg.x;
		scratch.position.y = 4 * cannyrect.y + 2 * cannyrect.height + recImg.y;
		scratch.length = (cannyrect.width >cannyrect.height) ? 2 * cannyrect.width : 2 * cannyrect.height;
		//if (scratch.length < 35)
		//	continue;
		_faults->Scratchs.push_back(scratch);
	}
}





Pretreatment::~Pretreatment()
{
}

void Pretreatment::pretreatment(Mat &image, Block *_block, Faults *faults)
{
	_faults = faults;

	if (image.channels() == 3)//�ж��Ƿ�Ϊ��ͼ
		cvtColor(image, image, CV_RGB2GRAY);

	vector<Point> pointlist;
	pointlist.push_back(_block->A);
	pointlist.push_back(_block->B);
	pointlist.push_back(_block->C);
	pointlist.push_back(_block->D);

	recImg = boundingRect(pointlist);//��ȡ��ש����
	recImg.x += 100;
	recImg.y += 400;
	recImg.width -= 200;
	recImg.height -= 800;
	MidImg = image(Rect(recImg));


	Mat MaskImg, Mask2CannyImg;
	Mask_result_big = Mat(MidImg.size(), CV_8UC1, Scalar(0));
	Mask_result_small = Mat(MidImg.rows / 4, MidImg.cols / 4, CV_8UC1, Scalar(255));
	resize(MidImg, MaskImg, Size(MidImg.cols / 16, MidImg.rows / 16), 0, 0, INTER_LINEAR);
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
					_faults->MarkPens.push_back(markpen);
				}
			}
		}


	}

	PMidImg = MidImg.clone();
	LMidImg = MidImg.clone();
	InitItemRepository(&gItemRepository);
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // �����ȱ�ݵ�Ԥ����.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // ����ȱ����ˮ��.
	std::thread line(std::mem_fn(&Pretreatment::linedetect), this);//���ۼ��
	producer.join();
	consumer.join();
	line.join();
	needContour.clear();
	dilateneedcontours.clear();
	CneedContours.clear();
	Warehousecontours.clear();
}
