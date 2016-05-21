#include "Pretreatment.h"
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include <numeric>

using namespace cv;
using namespace std;


static const int STAMP_WIDTH = 30;
static const int  STAMP_HEIGHT = 30;
static const int STAMP_SIZE = STAMP_WIDTH*STAMP_HEIGHT;
static const int kItemsToProduce = 10;   // ����������������
const Singleton *Singleton::m_Instance = new Singleton();

int flagdata = 0;//���ݳ�ʼ����ʶ
Mat MidImg, original_Img_D, original_Img_L, ThImg, LMidImg, CannyImg, PMidImg, re_Img_small, Mask_result_line;
vector<Rect> needContour;
static CvKNearest knn;
Rect recImg = Rect(Point(0, 0), Point(0, 0));

//����double��int����ת��
union luai_Cast { double l_d; long l_l; };
#define double2int(d,i) \
  { volatile union luai_Cast u; u.l_d = (d) + 6755399441055744.0; (i) = u.l_l; }


//Point��˳����������
inline bool SortBysize(vector<Point>const &v1, vector<Point>const &v2)
{
	return v1.size() > v2.size();//��������  
}

//int��˳����������  
inline bool SortBysize_int(vector<int>const &v1, vector<int>const &v2)
{
	return v1.size() > v2.size();//��������  
}

//���ݼ�������ʼ��
inline void Pretreatment::Dataload()
{
	FileStorage fs("Data//data.yaml", FileStorage::READ);
	Mat dataMat, labelMat;
	fs["data"] >> dataMat;
	fs["label"] >> labelMat;
	knn.train(dataMat, labelMat, Mat(), false, 2);
	flagdata = 1;
}

bool Fourier(const Mat &Img)
{
	Mat src = Img.clone();

	int m = getOptimalDFTSize(src.rows);//2,3,5�ı����и���Ч�ʵĸ���Ҷת��  
	int n = getOptimalDFTSize(src.cols);

	Mat dst;
	///�ѻҶ�ͼ��������Ͻǣ����ұߺ��±���չͼ����չ�������Ϊ0��  
	copyMakeBorder(src, dst, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));

	//�½�һ����ҳ��array�����е�һҳ����չ���ͼ���ʼ�����ڶ�ҳ��ʼ��Ϊ0  
	Mat planes[] = { Mat_<float>(dst), Mat::zeros(dst.size(), CV_32F) };
	Mat  completeI;
	merge(planes, 2, completeI);//����ҳ�ϳ�һ��2ͨ����mat  

	//���ϱߺϳɵ�mat���и���Ҷ�任��֧��ԭ�ز���������Ҷ�任���Ϊ������ͨ��1�����ʵ����ͨ��2������鲿��  
	dft(completeI, completeI);

	split(completeI, planes);//�ѱ任��Ľ���ָ�����������ҳ�У������������  
	magnitude(planes[0], planes[1], planes[0]);//����Ҷ�任��Ƶ�ʵķ�ֵ����ֵ���ڵ�һҳ�С�  

	Mat magI = planes[0];
	//����Ҷ�任�ķ���ֵ��Χ�󵽲��ʺ�����Ļ����ʾ����ֵ����Ļ����ʾΪ�׵㣬  
	//����ֵΪ�ڵ㣬�ߵ�ֵ�ı仯�޷���Ч�ֱ档Ϊ������Ļ��͹�Գ��ߵͱ仯�������ԣ����ǿ����ö����߶����滻���Գ߶�:  
	magI += Scalar::all(1);
	log(magI, magI);//ȡ����  
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));//ǰ�߶�ԭʼͼ���������չ������Ѷ�ԭʼͼ����Ҷ�任ȡ�����޳���չ���֡�  

	//���·������ޣ�ʹ��0,0���ƶ���ͼ�����ģ�  
	//�ڡ�����ͼ�����У�����Ҷ�任֮ǰҪ��Դͼ����ԣ�-1��^(x+y)�������Ļ���  
	//�����ǶԸ���Ҷ�任����������Ļ�  
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	//Mat tmp;
	//Mat q0(magI, Rect(0, 0, cx, cy));//����
	//Mat q1(magI, Rect(cx, 0, cx, cy));//����
	//Mat q2(magI, Rect(0, cy, cx, cy));//����
	//Mat q3(magI, Rect(cx, cy, cx, cy));//����


	//q0.copyTo(tmp);
	//q3.copyTo(q0);
	//tmp.copyTo(q3);

	//q1.copyTo(tmp);
	//q2.copyTo(q1);
	//tmp.copyTo(q2);

	normalize(magI, magI, 0, 1, NORM_MINMAX);
	Mat magImg(magI.size(), CV_8UC1);
	magI.convertTo(magImg, CV_8UC1, 255, 0);
	threshold(magImg, magImg, 150, 255, CV_THRESH_BINARY);
	//ʮ����������
	Rect zero_rect_x = Rect(Point(0, cy), Point(magI.cols - 1,cy+1));
	rectangle(magImg, zero_rect_x,Scalar(0),-1);
	Rect zero_rect_y = Rect(Point(cx, 0), Point(cx+1, magI.rows - 1));
	rectangle(magImg, zero_rect_y, Scalar(0), -1);

	Mat p0(magImg, Rect(0, 0, cx, cy));//����
	Mat p1(magImg, Rect(cx, 0, cx, cy));//����
	Mat p2(magImg, Rect(0, cy, cx, cy));//����
	Mat p3(magImg, Rect(cx, cy, cx, cy));//����
	int a_part = max(countNonZero(p0),1);//��ֹ��������0�������ʱ��Ϊ��ĸ
	int b_part = max(countNonZero(p1),1);//ͬ��
	int c_part = max(countNonZero(p2),1);//ͬ��
	int d_part = max(countNonZero(p3),1);//ͬ��
	float  ratio1 = max(float(a_part) / c_part, float(c_part) / a_part);//����ڵ���1�ı�ֵ
	float  ratio2 = max(float(b_part) / d_part, float(d_part) / b_part);//ͬ��
	float  ratio3 = max(float(a_part) / b_part, float(b_part) / a_part);//ͬ��

	if (ratio1 > 1.1 || ratio2 > 1.1 || ratio3 > 1.1)//���޻������ĸ�����Ӧ�þ���
		return 1;
	else
		return 0;
}

bool line_core(Mat &_Img)
{
	Mat line_temp,line_result;
	vector<int> flagline;
	if (_Img.cols > _Img.rows)
	{
		transpose(_Img, line_temp);
		for (int i = 0; i < _Img.cols; ++i)
		{
			uchar* data = line_temp.ptr<uchar>(i);
			int flagbit = 0;
			for (int j = 0; j < _Img.rows-1; ++j)
			{
				if (flagbit == 0)
				{
					if (data[j] != 0 && data[j + 1] == 0)
					{
						flagbit = 1;
					}
				}
				else if (data[j] != 0)
				{
					flagbit = 2;
					break;
				}
			}
			if (flagbit == 2)
				flagline.push_back(1);
			else
				flagline.push_back(0);
		}
	}
	else
	{
		for (int i = 0; i < _Img.cols; ++i)
		{
			uchar* data = _Img.ptr<uchar>(i);
			int flagbit = 0;
			for (int j = 0; j < _Img.rows - 1; ++j)
			{
				if (flagbit == 0)
				{
					if (data[j] != 0 && data[j + 1] == 0)
					{
						flagbit = 1;
					}
				}
				else if (data[j] != 0)
				{
					flagbit = 2;
					break;
				}	
			}
			if (flagbit == 2)
				flagline.push_back(1);
			else
				flagline.push_back(0);
		}
	}	
	int numcount = count(flagline.begin(), flagline.end(),1);
	if (numcount > int(0.7*_Img.cols) && numcount > int(0.7*_Img.rows))
		return 1;
	else
		return 0;
}

bool Pretreatment::defect_YoN(const Mat &_Img)
{
	Mat NorImg(STAMP_WIDTH, STAMP_HEIGHT, CV_8U);
	resize(_Img, NorImg, Size(STAMP_WIDTH, STAMP_HEIGHT), 0, 0, INTER_LINEAR);
	Mat data_sort(1, 450, CV_32F, Scalar(0));
	uchar *psrc;
	float gray = 0;
	int n = 0;
	for (int j = 0, idx = 0; j<30; ++j) {
		psrc = (NorImg).ptr<uchar>(j);
		for (int i = 0; i<30; i += 2, ++idx) {
			gray = psrc[i] + psrc[i + 1];
			data_sort.at<float>(0, n) = gray;
			n++;
		}
	}
	float resepone = knn.find_nearest(data_sort, 1);//�ǵü��뻮��Ϊ�������
	return int(resepone);
}

bool line_YoN(const Rect &_linesrect)
{
	bool flagbit = line_core(LMidImg(_linesrect));
	if (flagbit)
		return 1;//�ǻ���
	else
		return 0;//���ǻ���
}

bool WhetherLine(const Mat &oneImg, const Mat &G_Img, bool cor, bool boe)//�ж��Ƿ�Ϊline�ĺ��Ĳ���
{
	vector<Point> maxV_white_Points;
	if (countNonZero(oneImg) >= 1)//��ȡ�ıߵİ�ɫ������
	{
		if (cor == 1)
		{
			for (int i = 0; i < oneImg.cols - 1; ++i)
			{
				if (oneImg.at<uchar>(0, i) == 255 && oneImg.at<uchar>(0, i + 1) == 0)
					maxV_white_Points.push_back(Point(i + 0.2*G_Img.cols, 0));
			}
		}
		else
		{
			for (int i = 0; i < oneImg.rows - 1; ++i)
			{
				if (oneImg.at<uchar>(i, 0) == 255 && oneImg.at<uchar>(i + 1, 0) == 0)
					maxV_white_Points.push_back(Point(0, i + 0.2*G_Img.rows));
			}
		}
	}
	Mat Ataxon, Ltaxon, Rtaxon;
	for (int i = 0; i < maxV_white_Points.size(); ++i)
	{
		Point maxV_white_Point = maxV_white_Points[i];
		if (cor == 1 && boe == 0)//��һ��
		{
			Ataxon = G_Img(Rect(Point(maxV_white_Point.x - 2, 0), Point(maxV_white_Point.x + 2, 4)));//�����õ��ȫ4*4��Χ
			Ltaxon = G_Img(Rect(Point(maxV_white_Point.x - 2, 0), Point(maxV_white_Point.x, 4)));//�����õ����2*4��Χ
			Rtaxon = G_Img(Rect(Point(maxV_white_Point.x, 0), Point(maxV_white_Point.x + 2, 4)));//�����õ����2*4��Χ
		}
		else if (cor == 1 && boe == 1)//���һ��
		{
			Ataxon = G_Img(Rect(Point(maxV_white_Point.x - 2, G_Img.rows - 5), Point(maxV_white_Point.x + 2, G_Img.rows - 1)));
			Ltaxon = G_Img(Rect(Point(maxV_white_Point.x - 2, G_Img.rows - 5), Point(maxV_white_Point.x, G_Img.rows - 1)));
			Rtaxon = G_Img(Rect(Point(maxV_white_Point.x, G_Img.rows - 5), Point(maxV_white_Point.x + 2, G_Img.rows - 1)));
		}
		else if (cor == 0 && boe == 0)//��һ��
		{
			Ataxon = G_Img(Rect(Point(0, maxV_white_Point.y - 2), Point(4, maxV_white_Point.y + 2)));
			Ltaxon = G_Img(Rect(Point(0, maxV_white_Point.y - 2), Point(4, maxV_white_Point.y)));
			Rtaxon = G_Img(Rect(Point(0, maxV_white_Point.y), Point(4, maxV_white_Point.y + 2)));
		}
		else if (cor == 0 && boe == 1)//���һ��
		{
			Ataxon = G_Img(Rect(Point(G_Img.cols - 5, maxV_white_Point.y - 2), Point(G_Img.cols - 1, maxV_white_Point.y + 2)));
			Ltaxon = G_Img(Rect(Point(G_Img.cols - 5, maxV_white_Point.y - 2), Point(G_Img.cols - 1, maxV_white_Point.y)));
			Rtaxon = G_Img(Rect(Point(G_Img.cols - 5, maxV_white_Point.y), Point(G_Img.cols - 1, maxV_white_Point.y + 2)));
		}
		if (countNonZero(Ataxon) != countNonZero(Ltaxon) && countNonZero(Ataxon) != countNonZero(Rtaxon))//���߽��ϵ�������������ȫ�������Ұ���������ȫ����ʱ��֪�õ㲻�Ƕ˵�
			return 1;
	}
	return 0;
}

//������������
Point barycenter1(vector<Point> const &contoursi)
{
	Moments m = moments(contoursi);
	Point center = Point(0, 0);
	center.x = (int)(m.m10 / m.m00);
	center.y = (int)(m.m01 / m.m00);
	return center;
}

//��ש����Ԥ�����˹����˲���ֱ��ͼ���⻯
Mat Pretreatment::Equalize(const Mat &_Img)
{
	Mat re_Img_big,re_Img_temp;
	resize(_Img, re_Img_small, Size(_Img.cols / 16, _Img.rows / 16), 0, 0, INTER_LINEAR);
	double E_minvalue;
	minMaxLoc(re_Img_small, &E_minvalue, NULL);
	MatND E_hist;
	calcHist(&re_Img_small, 1, &channels, Mat(), E_hist, 1, &size, ranges);

	double sum = 0;
	int E_maxvalue = 90;
	for (int i = 0; i < 256; ++i)
	{
		sum += E_hist.at<float>(255 - i);
		if (sum > re_Img_small.cols*re_Img_small.rows*0.02)
		{
			E_maxvalue = 255 - i;
			break;
		}
	}
	double Linear_mul = 255 / (const int)(E_maxvalue - E_minvalue);//��ò�����Ա任����
	Mat E_lookUpTable = Mat(1, 256, CV_8U);
	uchar* p = E_lookUpTable.data;
	for (int j = 0; j < 256; ++j)
	{
		if (j <= E_maxvalue&&j>=E_minvalue)
			p[j] = j*Linear_mul;
		else if (j > E_maxvalue)
			p[j] = 255;
		else
			p[j] = 0;
	}
	Mat work_Img;
	GaussianBlur(re_Img_small, re_Img_temp, Size(15, 15), 0, 0);//��˹ģ��
	resize(re_Img_temp, re_Img_big, Size(_Img.cols, _Img.rows), 0, 0, INTER_LINEAR);
	Timer timer;
	timer.start();
	absdiff(_Img, re_Img_big, work_Img);//����Բ�ֵԤ����õĴ�ͼ
	timer.stop();
	cout<<timer.getElapsedTimeInMilliSec()<<"ms"<<endl;
	LUT(work_Img, E_lookUpTable, work_Img);
	/*LUT(re_Img_small, E_lookUpTable, re_Img_small);*/
	
	return work_Img;
}

//ȷ����ש�����ڲ�λ�ã�A�����ϣ�˳ʱ��
void Pretreatment::ProcessArea(Block *blockin)
{
	Point _A = blockin->A;
	Point _B = blockin->B;
	Point _C = blockin->C;
	Point _D = blockin->D;
	Point _A_remo = _A + Point(100, 100);
	Point _C_remo = _C - Point(100, 100);
	Point _D_remo = _D + Point(100, -100);
	Point _B_remo = _B + Point(-100, 100);
	
	pointlist.push_back(_A_remo);
	pointlist.push_back(_B_remo);
	pointlist.push_back(_C_remo);
	pointlist.push_back(_D_remo);

	//�������²�������Ӱ����������Ǳ�Եrect������Ӱ��ʱ����Ϊ�Ǳ��
	_A = _A + Point(0, 100);
	_C = _C + Point(0, -100);
	_D = _D + Point(0, -100);
	_B = _B + Point(0, 100);
	pointlist_r.push_back(_A);
	pointlist_r.push_back(_B);
	pointlist_r.push_back(_C);
	pointlist_r.push_back(_D);
}

inline int CrossProduct(const Point &pre, const Point &cur, const Point &next)//pre����һ���㣬cur�ǵ�ǰ�㣬next�ǽ�Ҫѡ��ĵ�    
{
	int x1 = cur.x - pre.x;
	int y1 = cur.y - pre.y;
	int x2 = cur.x - next.x;
	int y2 = cur.y - next.y;
	return (x1*x2 + y1*y2); //<0������͹���ĵ�   
}
inline int Distance(const Point &point1, const Point &point2)
{
	int distance_v = (point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y);
	return sqrt(distance_v);
}

//��һ��㼯�������������������ʱ�临�Ӷ�Ϊnlogn
int Pretreatment::Maxdistance(vector<Point> const &vec)
{
	vector<Point> hulldistance;
	convexHull(vec, hulldistance);
	int n = hulldistance.size() - 1;
	int j = 1;
	int maxLength = 0;//�洢���ֵ   
	hulldistance[n] = hulldistance[0];
	for (int i = 0; i<n; ++i)
	{
		while (CrossProduct(hulldistance[i + 1], hulldistance[j + 1], hulldistance[i]) > CrossProduct(hulldistance[i + 1], hulldistance[j], hulldistance[i]))
			j = (j + 1) % n;
		maxLength = max(maxLength, max(Distance(hulldistance[i], hulldistance[j]), Distance(hulldistance[i + 1], hulldistance[j + 1])));
	}
	return maxLength;
}

//�жϵ�ǰrect�Ƿ��Ѵ��ڵ�rect����
bool Contain(vector<vector<Point>> const &rectveclist, vector<Point> const &rectcandidate)
{
	int Contain_mark = 0;
	for (size_t i = 0; i < rectveclist.size(); ++i)
	{
		int j = 0;
		if (rectveclist[i][0].x <= rectcandidate[0].x&&rectveclist[i][0].y <= rectcandidate[0].y)//����
			j++;
		if (rectveclist[i][1].x >= rectcandidate[1].x&&rectveclist[i][1].y >= rectcandidate[1].y)//����
			j++;
		if (rectveclist[i][0].x >= rectcandidate[0].x&&rectveclist[i][0].y <= rectcandidate[0].y)//����
			j++;
		if (rectveclist[i][0].x <= rectcandidate[0].x&&rectveclist[i][0].y >= rectcandidate[0].y)//����
			j++;
		if (j == 4)//�ĸ���ͬʱ�ڵ�ǰrect���ڲ�
		{
			Contain_mark = 1;
			break;
		}
	}
	if (Contain_mark)//���ĸ��㶼���ڲ�ʱΪ��
		return 1;
	else
		return 0;
}

//�ж��˹��ʼ�
void Pretreatment::Handwriting(const Mat &_img)
{
	Mat Canny_Img;
	Mask_result_big = Mat(MidImg.size(), CV_8UC1, Scalar(0));
	Mask_result_small = Mat(MidImg.rows / 4, MidImg.cols / 4, CV_8UC1, Scalar(255));
	GaussianBlur(_img, Canny_Img, Size(3, 3), 0, 0);
	copyMakeBorder(Canny_Img, Canny_Img, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	Canny(Canny_Img, Canny_Img, 40, 120);//����ƽ��˫��ǿ��Ե
	vector<vector<Point>> Maskcontours;
	findContours(Canny_Img, Maskcontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	vector<vector<Point>> containlist;
	for (size_t i = 0; i < Maskcontours.size(); ++i)
	{
		if (Maskcontours[i].size() > 5)
		{
			Rect mask_rect = boundingRect(Maskcontours[i]);
			mask_rect.x -= 1;
			mask_rect.y -= 1;
			Mat watch_img = _img(mask_rect).clone();
			threshold(watch_img, watch_img,100,255,CV_THRESH_OTSU);
			double nonnum = countNonZero(watch_img);
			long numlong;
			double2int(nonnum, numlong);
			if (numlong< watch_img.cols || numlong < watch_img.rows)
				continue;
			int compare_v = Maxdistance(Maskcontours[i]);
			if (compare_v < 9)//�����㼯�������Զ������֮��ľ���
				continue;

			mask_rect.x = (16 * mask_rect.x - 64 > 0) ? 16 * mask_rect.x - 64 : 0;
			mask_rect.y = (16 * mask_rect.y - 64 > 0) ? 16 * mask_rect.y - 64 : 0;
			mask_rect.width = (16 * mask_rect.width + 128 < Mask_result_small.cols) ? 16 * mask_rect.width + 128 : Mask_result_small.cols;
			mask_rect.height = (16 * mask_rect.height + 128 < Mask_result_small.rows) ? 16 * mask_rect.height + 128 : Mask_result_small.rows;
			rectangle(Mask_result_big, mask_rect, Scalar(255), -1);//������Ĥ

			
			mask_rect.x += recImg.x;
			mask_rect.y += recImg.y;
			
			Point outpoint_a(mask_rect.x, mask_rect.y), outpoint_b(mask_rect.x + mask_rect.width, mask_rect.y + mask_rect.height);//���ϣ�����
			Point outpoint_c(mask_rect.x + mask_rect.width, mask_rect.y), outpoint_d(mask_rect.x, mask_rect.y + mask_rect.height);//���ϣ�����
			
			in_or_out.clear();
			in_or_out.push_back(outpoint_a);
			in_or_out.push_back(outpoint_b);
			in_or_out.push_back(outpoint_c);
			in_or_out.push_back(outpoint_d);

			if (Contain(containlist, in_or_out))//�жϵ�ǰrect�Ƿ��Ѵ���rect�İ���������findcontours����˳��һ�����ⲿ����������ȡ������������֮ǰ�������Ƿ���Ҫ���ų�
				continue;
			containlist.push_back(in_or_out);//��ǰrectδ�������򴢴�

			if (pointPolygonTest(pointlist_r, outpoint_a, 0) == 1 && pointPolygonTest(pointlist_r, outpoint_b, 0) == 1)//���ñ���Ƿ��ڴ�ש�ϣ���ֹ��ש��б����
				if (pointPolygonTest(pointlist_r, outpoint_c, 0) == 1 && pointPolygonTest(pointlist_r, outpoint_d, 0) == 1)
				{
					Faults::MarkPen markpen;
					markpen.markposition = mask_rect;
					_faults->MarkPens.push_back(markpen);
				}		
		}
	}
}

//�ֲ�����ͳ��
void statistics_gray(int _width, int _height, const MatND &hist, int _step)
{
	Singleton *singletonObj = Singleton::GetInstance();
	int &Midnum = singletonObj->Getvector()[_step];//��λ��
	int &Modnum = singletonObj->Getvector()[_step+10];//����
	int pixelSum = _width * _height, pixelmaxloc = 0;
	float pixelcumu = 0, pixelmax = 0, pixelnow = 0;
	for (int i = 0; i < 256; ++i)
	{
		pixelnow = hist.at<float>(i);
		pixelcumu += pixelnow;
		if (2 * pixelcumu > pixelSum && 2 * (pixelcumu - pixelnow) < pixelSum)
			Midnum = i;
		if (pixelmax < pixelnow&&i > 2)
		{
			pixelmax = pixelnow;
			pixelmaxloc = i;
		}
	}
	if (pixelmaxloc > 10)//���������ڹ�������
	Modnum = pixelmaxloc;//��һ��Ϊ��λ���ڶ���Ϊ����
}

//Ѱ����λ����������
int partition(vector<int> &array, int left, int right)
{
	int pos = right;
	right--;
	while (left <= right)
	{
		while (left < pos && array[left] <= array[pos])
			left++;

		while (right >= 0 && array[right] > array[pos])
			right--;

		if (left >= right)
			break;

		swap(array[left], array[right]);
	}
	swap(array[left], array[pos]);

	return left;
}

//����Ѱ����λ��
int getMidIndex(vector<int> &array, int size)
{
	if (size <= 0)
		return -1;

	int left = 0;
	int right = size - 1;
	int midPos = right >> 1;//�м�λ��
	int index = -1;

	while (index != midPos)
	{
		index = partition(array, left, right);

		if (index < midPos)
		{
			left = index + 1;
		}
		else if (index > midPos)
		{
			right = index - 1;
		}
		else
		{
			break;
		}
	}

	assert(index == midPos);
	return array[index];
}

//ͳ�����ݹ�һ��
void statis_nol(vector<int> &_statis)
{
	Singleton *singletonObj = Singleton::GetInstance();
	vector<int> &result = singletonObj->Getresult();
	result[0] = getMidIndex(_statis, 10);//����ܵ���λ������������λ������λ��
	double sum = std::accumulate(std::begin(_statis)+10, std::end(_statis), 0.0);
	result[1] = int(2 * sum / double(_statis.size()) + 0.5);//����ܵ�������������������ƽ��ֵ
}

//�ֲ���ֵ������ֵѡȡ
int Pretreatment::otsuThreshold(int _width, int _height, float origin, const MatND &hist)
{
	float pixelPro[256] = { 0 };
	int pixelSum = _width * _height, threshold = 0;
	vector<vector<int>> grayVlist;
	vector<int> grayV;

	int m = 0;
	for (int i = 0; i < 256; ++i)
	{
		pixelPro[i] = hist.at<float>(i);
		if (pixelPro[i] > 0)//�����Ҷȷֲ�ɸѡ
		{
			grayV.push_back(i);
			m = 0;
		}
		else 
		{
			m++;
		}
		if (m == 6)
		{
			grayV.pop_back();
			grayV.pop_back();
			grayV.pop_back();
			grayV.pop_back();
			grayV.pop_back();
			grayVlist.push_back(grayV);
			grayV.clear();
		}
		else if (m > 0&&m < 6)
		{
			grayV.push_back(i);
		}

	}
	if (m < 6)//��֤������255ʱ�Ա���¼
	{
		grayVlist.push_back(grayV);
		grayV.clear();
		m = 0;
	}
	if (grayVlist.size() == 0)
		return origin;
	sort(grayVlist.begin(), grayVlist.end(), SortBysize_int);
	threshold = (grayVlist[0][grayVlist[0].size()-1]-48)*0.2;//��ש�ϵı�׼�Ҷ�ֵ����Ϊ48
	grayVlist.clear();
	for (int i = 0; i < 256; ++i)
	{
		pixelPro[i] = hist.at<float>(i) / (const int)(pixelSum);

		if (pixelPro[i] > 0.001)//�����Ҷȷֲ�ɸѡ
		{
			grayV.push_back(i);
			m = 0;
		}
		else
		{
			m++;
		}
		if (m == 6)
		{
			grayV.pop_back();
			grayV.pop_back();
			grayVlist.push_back(grayV);
			grayV.clear();
		}
		else if (m > 0 && m < 6)
		{
			grayV.push_back(i);
		}
	}
	if (m < 6)//��֤������255ʱ�Ա���¼
	{
		grayVlist.push_back(grayV);
		grayV.clear();
		m = 0;
	}
	if (grayVlist.size() == 0)
		return origin;
	sort(grayVlist.begin(), grayVlist.end(), SortBysize_int);
	threshold = threshold+(grayVlist[0][grayVlist[0].size() - 1])+15;//�Դ�ש�ϵı�׼�Ҷ�ֵ����48Ϊ��׼ֵ���������ĻҶȷֲ����������������Ա仯��Ϊ0.2
	return threshold;
}

//��������ͼ�����ӵ�����
Mat Pretreatment::Grow(const Mat &image, const Point &seedpoint, const int th_v)
{
	Mat HyImg = image.clone();
	Mat SameImg(HyImg.size(), CV_8UC1, Scalar(0));
	vector<Point> seedq;
	seedq.push_back(seedpoint);
	while (!seedq.empty())
	{
		Point growpoint = seedq[seedq.size() - 1];
		seedq.pop_back();
		uchar* data = HyImg.ptr<uchar>(growpoint.y);//�Ҷ�ͼ�����׵�ַ
		uchar* dataT = SameImg.ptr<uchar>(growpoint.y);//��ֵͼ�����׵�ַ
		if ((growpoint.x > 0) && (growpoint.x < (image.cols - 1)) && (growpoint.y > 0) && (growpoint.y < (image.rows - 1)))
		{
			if (data[growpoint.x - 1] >= th_v)
			{
				if (dataT[growpoint.x - 1] == 0 && data[growpoint.x] - data[growpoint.x - 1] <= 3)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y));
					dataT[growpoint.x - 1] = 255;
				}
			}

			if (data[growpoint.x + 1] >= th_v)
			{
				if (dataT[growpoint.x + 1] == 0 && data[growpoint.x] - data[growpoint.x + 1] <= 3)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y));
					dataT[growpoint.x + 1] = 255;
				}
			}

			uchar* dataU = HyImg.ptr<uchar>(growpoint.y - 1);//�Ҷ�ͼ��һ���׵�ַ
			uchar* dataUT = SameImg.ptr<uchar>(growpoint.y - 1);//��ֵͼ��һ���׵�ַ

			if (dataU[growpoint.x] >= th_v)
			{
				if (dataUT[growpoint.x] == 0 && data[growpoint.x] - dataU[growpoint.x] <= 3)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y - 1));
					dataUT[growpoint.x] = 255;
				}
			}
			if (dataU[growpoint.x - 1] >= th_v)
			{
				if (dataUT[growpoint.x - 1] == 0 && data[growpoint.x] - dataU[growpoint.x - 1] <= 3)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y - 1));
					dataUT[growpoint.x - 1] = 255;
				}
			}

			if (dataU[growpoint.x + 1]  >= th_v)
			{
				if (dataUT[growpoint.x + 1] == 0 && data[growpoint.x] - dataU[growpoint.x + 1] <= 3)
				{
					seedq.push_back(Point(growpoint.x + 1, growpoint.y - 1));
					dataUT[growpoint.x + 1] = 255;
				}
			}

			uchar* dataD = HyImg.ptr<uchar>(growpoint.y + 1);//�Ҷ�ͼ��һ���׵�ַ
			uchar* dataDT = SameImg.ptr<uchar>(growpoint.y + 1);//��ֵͼ��һ���׵�ַ

			if (dataD[growpoint.x] >= th_v)
			{
				if (dataDT[growpoint.x] == 0 && data[growpoint.x] - dataD[growpoint.x] <= 3)
				{
					seedq.push_back(Point(growpoint.x, growpoint.y + 1));
					dataDT[growpoint.x] = 255;
				}
			}

			if (dataD[growpoint.x - 1] >= th_v)
			{
				if (dataDT[growpoint.x - 1] == 0 && data[growpoint.x] - dataD[growpoint.x - 1] <= 3)
				{
					seedq.push_back(Point(growpoint.x - 1, growpoint.y + 1));
					dataDT[growpoint.x - 1] = 255;
				}
			}
			if (dataD[growpoint.x + 1] >= th_v)
			{
				if (dataDT[growpoint.x + 1] == 0 && data[growpoint.x] - dataD[growpoint.x + 1] <= 3)
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

	needContour.push_back(location_rect);
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
	Point locationpoint = Point(CneedContours.back().x, CneedContours.back().y);
	CneedContours.pop_back();
	for (size_t i = 0; i < CneedContours.size(); ++i)
	{
		/*cout << i << endl;*/
		Rect box = CneedContours[i];
		Point pt1 = Point(box.x, box.y);
		Point pt2 = Point(box.x + box.width, box.y + box.height);

		Mat work_boxImg = original_Img_D(Rect(pt1 + locationpoint, pt2 + locationpoint));

		
		if (!defect_YoN(work_boxImg))
			continue;
		
		Faults::Hole hole;
		hole.position.x = 0.5*(pt1.x + pt2.x) + recImg.x + locationpoint.x;//����任��ԭͼ
		hole.position.y = 0.5*(pt1.y + pt2.y) + recImg.y + locationpoint.y;
		if (box.height > box.width)
			hole.diameter = box.height;
		else
			hole.diameter = box.height;
		
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
	//ThImg = Mat(MidImg.size(), CV_8UC1, Scalar(0));//��ֵ��ԭͼ
	if (_faults->MarkPens.size() != 0)
	{
		(cv::max)(MidImg, Mask_result_big, MidImg);
	}

	Mat ThImgROI, MidImgROI,original_Img_ROI;
	Point Thpt = Point(0, 0);//��ʼ��ԭ��
	int step = 0;
	for (int i = 1; i <= 5; ++i)//�ֲ���ֵ����Լ���Ҷȱ仯��
	{
		for (int j = 0; j < 2; ++j)
		{
			needContour.clear();
			Thpt.x = j*MidImg.cols*0.5;
			Rect ThRect = Rect(Thpt.x, Thpt.y, MidImg.cols*0.5, MidImg.rows*0.2);
			if (j)//�����Ҳ�����
				ThRect.width = MidImg.cols - MidImg.cols*0.5;
			if (i == 4)//����ײ�����
				ThRect.height = MidImg.rows - 4 * MidImg.rows*0.2;
			MidImgROI = MidImg(Rect(ThRect));
			original_Img_ROI = original_Img_D(Rect(ThRect));
			/*medianBlur(MidImgROI, MidImgROI,7);*/

			/*ThImgROI = ThImg(Rect(ThRect));*/

			MatND histolist, histolist_r;
			calcHist(&MidImgROI, 1, &channels, Mat(), histolist, 1, &size, ranges);//Ԥ�������ͼ��ûҶȷֲ�
			calcHist(&original_Img_ROI, 1, &channels, Mat(), histolist_r, 1, &size, ranges);//ԭͼ��ûҶȷֲ�
			statistics_gray(ThRect.width, ThRect.height, histolist_r, step);
			int OtsuV = otsuThreshold(ThRect.width, ThRect.height, MidImgROI.at<uchar>(0, 0), histolist);//�ҵ���ɢ�Ҷ�ֵ��������ֵ
			threshold(MidImgROI, ThImgROI, OtsuV, 255, 0);

			vector<vector<cv::Point>> decontours;
			cv::findContours(ThImgROI, decontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			for (size_t k = 0; k < decontours.size(); ++k)
			{
				if (decontours[k].size() > 8)
				{
					Rect decontours_rect = boundingRect(decontours[k]);
					needContour.push_back(decontours_rect);
				}
			}
			location_rect = Rect(Point(ThRect.x, ThRect.y), Point(ThRect.x, ThRect.y));//��ǰROI���ϽǶ�����Mid�е�����
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
	resize(Mask_result_big, Mask_result_line, Size(Mask_result_big.cols / 3, Mask_result_big.rows / 3), 0, 0, INTER_AREA);
	Canny(LMidImg, LMidImg, 40, 120);
	bitwise_xor(LMidImg, Mask_result_line, CannyImg);
	vector<vector<cv::Point>> linescontours;
	findContours(CannyImg, linescontours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < linescontours.size(); ++i)
	{
		if (linescontours[i].size() > 60)
		{
			vector<Point> km_contours;
			convexHull(linescontours[i], km_contours);//������ת��Ϊ͹��
			Rect linerect = boundingRect(linescontours[i]);
			Point a_sy = Point(0, 0);
			Point b_sy = Point(0, 0);
			if (linerect.width < 20 || linerect.height < 20)
			{
				a_sy = Point(linerect.x + linerect.width*0.25, linerect.y + linerect.height*0.75);
				b_sy = Point(linerect.x + linerect.width*0.75, linerect.y + linerect.height*0.25);
			}
			if (abs(original_Img_L.at<uchar>(a_sy)-original_Img_L.at<uchar>(b_sy) > 4))
				continue;
			if ((linerect.width-1)*(linerect.height-1) < 2 * int(contourArea(km_contours)))//����Ƿ�Ϊ�໮����״
				continue;
			if (line_YoN(linerect))
			{
				//����任��ԭͼ
				linerect.x = 3 * linerect.x;
				linerect.y = 3 * linerect.y;
				linerect.width = 3 * linerect.width;
				linerect.height = 3 * linerect.height;
				Faults::Scratch scratch;
				scratch.position.x = linerect.x + 0.5 * linerect.width + recImg.x;
				scratch.position.y = linerect.y + 0.5 * linerect.height + recImg.y;
				scratch.length = (linerect.width >linerect.height) ? linerect.width : linerect.height;
				_faults->Scratchs.push_back(scratch);
			}
		}
	}
}



void Pretreatment::line2preprocess()
{
	resize(original_Img_D, original_Img_L, Size(original_Img_D.cols / 3, original_Img_D.rows / 3), 0, 0, INTER_AREA);//��ȡ���MidImg��Сԭͼ��С
	resize(MidImg, LMidImg, Size(MidImg.cols / 3, MidImg.rows / 3), 0, 0, INTER_AREA);
}


Pretreatment::~Pretreatment()
{
}

void Pretreatment::pretreatment(Mat &image, Block *_block, Faults *faults)
{

	_faults = faults;
	Singleton *singletonObj = Singleton::GetInstance();
	vector<int> &result1 = singletonObj->Getresult();

	if (image.channels() == 3)//�ж��Ƿ�Ϊ��ͼ
		cvtColor(image, image, CV_RGB2GRAY);

	//ֱ��ͼ�任
	Mat E_image=Equalize(image);
	//���ڲ�С����
	ProcessArea(_block);

	//����������ݲ���ʼ��һ��
	static int ret = (Dataload(), 1);

	vector<vector<Point>>filterContours;
	filterContours.push_back(pointlist);
	Mat E_img_mask(E_image.size(),CV_8UC1,Scalar(0));
	drawContours(E_img_mask, filterContours, 0, Scalar(255), CV_FILLED);
	bitwise_and(E_image, E_img_mask, E_image);
	recImg = boundingRect(pointlist);//��ȡ��ש����,�����㲻ȫ������Ҳ���н�ȡ

	//��ȡ��Ҫ�Ĳ���
	MidImg = E_image(Rect(recImg));
	original_Img_D = image(Rect(recImg));//ȱ�ݼ��ԭͼ

	std::thread linepreprocess(std::mem_fn(&Pretreatment::line2preprocess), this);
	resize(MidImg, re_Img_small, Size(MidImg.cols / 16, MidImg.rows / 16), 0, 0, INTER_LINEAR);

	Handwriting(re_Img_small);//����˹����

	linepreprocess.join();

	//��Ϊ��͹�����߼���߳�
	InitItemRepository(&gItemRepository);
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // �����ȱ�ݵ�Ԥ����.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // ����ȱ����ˮ��.
	std::thread line(std::mem_fn(&Pretreatment::linedetect), this);//���ۼ�� 
	/*auto tn = line.native_handle();
	SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);*///�߳����ȼ�����
	producer.join();
	consumer.join();
	line.join();

	//����ͳ���������
	statis_nol(singletonObj->Getvector());

	//�����ڴ�
	vector<Rect>().swap(needContour);
	vector<Rect>().swap(CneedContours);
	vector<vector<Rect>>().swap(Warehousecontours);
	/*needContour.clear();
	CneedContours.clear();
	Warehousecontours.clear();*/
}
