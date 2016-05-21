#include "Pretreatment.h"
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include <numeric>

using namespace cv;
using namespace std;


static const int STAMP_WIDTH = 30;
static const int  STAMP_HEIGHT = 30;
static const int STAMP_SIZE = STAMP_WIDTH*STAMP_HEIGHT;
static const int kItemsToProduce = 10;   // 生产者生产的总数
const Singleton *Singleton::m_Instance = new Singleton();

int flagdata = 0;//数据初始化标识
Mat MidImg, original_Img_D, original_Img_L, ThImg, LMidImg, CannyImg, PMidImg, re_Img_small, Mask_result_line;
vector<Rect> needContour;
static CvKNearest knn;
Rect recImg = Rect(Point(0, 0), Point(0, 0));

//快速double到int类型转换
union luai_Cast { double l_d; long l_l; };
#define double2int(d,i) \
  { volatile union luai_Cast u; u.l_d = (d) + 6755399441055744.0; (i) = u.l_l; }


//Point型顺序排列算子
inline bool SortBysize(vector<Point>const &v1, vector<Point>const &v2)
{
	return v1.size() > v2.size();//降序排列  
}

//int型顺序排列算子  
inline bool SortBysize_int(vector<int>const &v1, vector<int>const &v2)
{
	return v1.size() > v2.size();//降序排列  
}

//数据及参数初始化
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

	int m = getOptimalDFTSize(src.rows);//2,3,5的倍数有更高效率的傅里叶转换  
	int n = getOptimalDFTSize(src.cols);

	Mat dst;
	///把灰度图像放在左上角，在右边和下边扩展图像，扩展部分填充为0；  
	copyMakeBorder(src, dst, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));

	//新建一个两页的array，其中第一页用扩展后的图像初始化，第二页初始化为0  
	Mat planes[] = { Mat_<float>(dst), Mat::zeros(dst.size(), CV_32F) };
	Mat  completeI;
	merge(planes, 2, completeI);//把两页合成一个2通道的mat  

	//对上边合成的mat进行傅里叶变换，支持原地操作，傅里叶变换结果为复数。通道1存的是实部，通道2存的是虚部。  
	dft(completeI, completeI);

	split(completeI, planes);//把变换后的结果分割到各个数组的两页中，方便后续操作  
	magnitude(planes[0], planes[1], planes[0]);//求傅里叶变换各频率的幅值，幅值放在第一页中。  

	Mat magI = planes[0];
	//傅立叶变换的幅度值范围大到不适合在屏幕上显示。高值在屏幕上显示为白点，  
	//而低值为黑点，高低值的变化无法有效分辨。为了在屏幕上凸显出高低变化的连续性，我们可以用对数尺度来替换线性尺度:  
	magI += Scalar::all(1);
	log(magI, magI);//取对数  
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));//前边对原始图像进行了扩展，这里把对原始图像傅里叶变换取出，剔除扩展部分。  

	//重新分配象限，使（0,0）移动到图像中心，  
	//在《数字图像处理》中，傅里叶变换之前要对源图像乘以（-1）^(x+y)进行中心化。  
	//这是是对傅里叶变换结果进行中心化  
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	//Mat tmp;
	//Mat q0(magI, Rect(0, 0, cx, cy));//左上
	//Mat q1(magI, Rect(cx, 0, cx, cy));//右上
	//Mat q2(magI, Rect(0, cy, cx, cy));//左下
	//Mat q3(magI, Rect(cx, cy, cx, cy));//右下


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
	//十字亮线置零
	Rect zero_rect_x = Rect(Point(0, cy), Point(magI.cols - 1,cy+1));
	rectangle(magImg, zero_rect_x,Scalar(0),-1);
	Rect zero_rect_y = Rect(Point(cx, 0), Point(cx+1, magI.rows - 1));
	rectangle(magImg, zero_rect_y, Scalar(0), -1);

	Mat p0(magImg, Rect(0, 0, cx, cy));//左上
	Mat p1(magImg, Rect(cx, 0, cx, cy));//右上
	Mat p2(magImg, Rect(0, cy, cx, cy));//左下
	Mat p3(magImg, Rect(cx, cy, cx, cy));//右下
	int a_part = max(countNonZero(p0),1);//防止出现数字0在求比例时作为分母
	int b_part = max(countNonZero(p1),1);//同上
	int c_part = max(countNonZero(p2),1);//同上
	int d_part = max(countNonZero(p3),1);//同上
	float  ratio1 = max(float(a_part) / c_part, float(c_part) / a_part);//求大于等于1的比值
	float  ratio2 = max(float(b_part) / d_part, float(d_part) / b_part);//同上
	float  ratio3 = max(float(a_part) / b_part, float(b_part) / a_part);//同上

	if (ratio1 > 1.1 || ratio2 > 1.1 || ratio3 > 1.1)//若无划痕则四个部分应该均匀
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
	float resepone = knn.find_nearest(data_sort, 1);//记得加入划痕为否的数据
	return int(resepone);
}

bool line_YoN(const Rect &_linesrect)
{
	bool flagbit = line_core(LMidImg(_linesrect));
	if (flagbit)
		return 1;//是划痕
	else
		return 0;//不是划痕
}

bool WhetherLine(const Mat &oneImg, const Mat &G_Img, bool cor, bool boe)//判断是否为line的核心部分
{
	vector<Point> maxV_white_Points;
	if (countNonZero(oneImg) >= 1)//获取四边的白色点坐标
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

//计算轮廓重心
Point barycenter1(vector<Point> const &contoursi)
{
	Moments m = moments(contoursi);
	Point center = Point(0, 0);
	center.x = (int)(m.m10 / m.m00);
	center.y = (int)(m.m01 / m.m00);
	return center;
}

//瓷砖表面预处理高斯差分滤波与直方图均衡化
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
	double Linear_mul = 255 / (const int)(E_maxvalue - E_minvalue);//获得查表线性变换倍数
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
	GaussianBlur(re_Img_small, re_Img_temp, Size(15, 15), 0, 0);//高斯模糊
	resize(re_Img_temp, re_Img_big, Size(_Img.cols, _Img.rows), 0, 0, INTER_LINEAR);
	Timer timer;
	timer.start();
	absdiff(_Img, re_Img_big, work_Img);//求绝对差值预处理好的大图
	timer.stop();
	cout<<timer.getElapsedTimeInMilliSec()<<"ms"<<endl;
	LUT(work_Img, E_lookUpTable, work_Img);
	/*LUT(re_Img_small, E_lookUpTable, re_Img_small);*/
	
	return work_Img;
}

//确定瓷砖表面内部位置，A是左上，顺时针
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

	//由于上下部分有阴影，当待检测标记边缘rect落在阴影中时不认为是标记
	_A = _A + Point(0, 100);
	_C = _C + Point(0, -100);
	_D = _D + Point(0, -100);
	_B = _B + Point(0, 100);
	pointlist_r.push_back(_A);
	pointlist_r.push_back(_B);
	pointlist_r.push_back(_C);
	pointlist_r.push_back(_D);
}

inline int CrossProduct(const Point &pre, const Point &cur, const Point &next)//pre是上一个点，cur是当前点，next是将要选择的点    
{
	int x1 = cur.x - pre.x;
	int y1 = cur.y - pre.y;
	int x2 = cur.x - next.x;
	int y2 = cur.y - next.y;
	return (x1*x2 + y1*y2); //<0是满足凸包的点   
}
inline int Distance(const Point &point1, const Point &point2)
{
	int distance_v = (point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y);
	return sqrt(distance_v);
}

//给一组点集求出任意两点间的最大距离时间复杂度为nlogn
int Pretreatment::Maxdistance(vector<Point> const &vec)
{
	vector<Point> hulldistance;
	convexHull(vec, hulldistance);
	int n = hulldistance.size() - 1;
	int j = 1;
	int maxLength = 0;//存储最大值   
	hulldistance[n] = hulldistance[0];
	for (int i = 0; i<n; ++i)
	{
		while (CrossProduct(hulldistance[i + 1], hulldistance[j + 1], hulldistance[i]) > CrossProduct(hulldistance[i + 1], hulldistance[j], hulldistance[i]))
			j = (j + 1) % n;
		maxLength = max(maxLength, max(Distance(hulldistance[i], hulldistance[j]), Distance(hulldistance[i + 1], hulldistance[j + 1])));
	}
	return maxLength;
}

//判断当前rect是否被已存在的rect包含
bool Contain(vector<vector<Point>> const &rectveclist, vector<Point> const &rectcandidate)
{
	int Contain_mark = 0;
	for (size_t i = 0; i < rectveclist.size(); ++i)
	{
		int j = 0;
		if (rectveclist[i][0].x <= rectcandidate[0].x&&rectveclist[i][0].y <= rectcandidate[0].y)//左上
			j++;
		if (rectveclist[i][1].x >= rectcandidate[1].x&&rectveclist[i][1].y >= rectcandidate[1].y)//右下
			j++;
		if (rectveclist[i][0].x >= rectcandidate[0].x&&rectveclist[i][0].y <= rectcandidate[0].y)//右上
			j++;
		if (rectveclist[i][0].x <= rectcandidate[0].x&&rectveclist[i][0].y >= rectcandidate[0].y)//左下
			j++;
		if (j == 4)//四个点同时在当前rect的内部
		{
			Contain_mark = 1;
			break;
		}
	}
	if (Contain_mark)//在四个点都在内部时为真
		return 1;
	else
		return 0;
}

//判断人工笔迹
void Pretreatment::Handwriting(const Mat &_img)
{
	Mat Canny_Img;
	Mask_result_big = Mat(MidImg.size(), CV_8UC1, Scalar(0));
	Mask_result_small = Mat(MidImg.rows / 4, MidImg.cols / 4, CV_8UC1, Scalar(255));
	GaussianBlur(_img, Canny_Img, Size(3, 3), 0, 0);
	copyMakeBorder(Canny_Img, Canny_Img, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	Canny(Canny_Img, Canny_Img, 40, 120);//存在平行双边强边缘
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
			if (compare_v < 9)//给出点集中相距最远的两点之间的距离
				continue;

			mask_rect.x = (16 * mask_rect.x - 64 > 0) ? 16 * mask_rect.x - 64 : 0;
			mask_rect.y = (16 * mask_rect.y - 64 > 0) ? 16 * mask_rect.y - 64 : 0;
			mask_rect.width = (16 * mask_rect.width + 128 < Mask_result_small.cols) ? 16 * mask_rect.width + 128 : Mask_result_small.cols;
			mask_rect.height = (16 * mask_rect.height + 128 < Mask_result_small.rows) ? 16 * mask_rect.height + 128 : Mask_result_small.rows;
			rectangle(Mask_result_big, mask_rect, Scalar(255), -1);//制作掩膜

			
			mask_rect.x += recImg.x;
			mask_rect.y += recImg.y;
			
			Point outpoint_a(mask_rect.x, mask_rect.y), outpoint_b(mask_rect.x + mask_rect.width, mask_rect.y + mask_rect.height);//左上，右下
			Point outpoint_c(mask_rect.x + mask_rect.width, mask_rect.y), outpoint_d(mask_rect.x, mask_rect.y + mask_rect.height);//右上，左下
			
			in_or_out.clear();
			in_or_out.push_back(outpoint_a);
			in_or_out.push_back(outpoint_b);
			in_or_out.push_back(outpoint_c);
			in_or_out.push_back(outpoint_d);

			if (Contain(containlist, in_or_out))//判断当前rect是否被已存在rect的包含，根据findcontours轮廓顺序，一定是外部的轮廓被读取到，故无需检测之前的轮廓是否需要被排除
				continue;
			containlist.push_back(in_or_out);//当前rect未被包含则储存

			if (pointPolygonTest(pointlist_r, outpoint_a, 0) == 1 && pointPolygonTest(pointlist_r, outpoint_b, 0) == 1)//检测该标记是否在瓷砖上，防止瓷砖倾斜误判
				if (pointPolygonTest(pointlist_r, outpoint_c, 0) == 1 && pointPolygonTest(pointlist_r, outpoint_d, 0) == 1)
				{
					Faults::MarkPen markpen;
					markpen.markposition = mask_rect;
					_faults->MarkPens.push_back(markpen);
				}		
		}
	}
}

//局部数据统计
void statistics_gray(int _width, int _height, const MatND &hist, int _step)
{
	Singleton *singletonObj = Singleton::GetInstance();
	int &Midnum = singletonObj->Getvector()[_step];//中位数
	int &Modnum = singletonObj->Getvector()[_step+10];//众数
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
	if (pixelmaxloc > 10)//众数不能在过暗部分
	Modnum = pixelmaxloc;//第一个为中位数第二个为众数
}

//寻找中位数辅助函数
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

//快速寻找中位数
int getMidIndex(vector<int> &array, int size)
{
	if (size <= 0)
		return -1;

	int left = 0;
	int right = size - 1;
	int midPos = right >> 1;//中间位置
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

//统计数据归一化
void statis_nol(vector<int> &_statis)
{
	Singleton *singletonObj = Singleton::GetInstance();
	vector<int> &result = singletonObj->Getresult();
	result[0] = getMidIndex(_statis, 10);//算出总的中位数，各部分中位数的中位数
	double sum = std::accumulate(std::begin(_statis)+10, std::end(_statis), 0.0);
	result[1] = int(2 * sum / double(_statis.size()) + 0.5);//算出总的众数，各部分众数的平均值
}

//局部二值化的阈值选取
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
		if (pixelPro[i] > 0)//连续灰度分布筛选
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
	if (m < 6)//保证连续到255时仍被记录
	{
		grayVlist.push_back(grayV);
		grayV.clear();
		m = 0;
	}
	if (grayVlist.size() == 0)
		return origin;
	sort(grayVlist.begin(), grayVlist.end(), SortBysize_int);
	threshold = (grayVlist[0][grayVlist[0].size()-1]-48)*0.2;//瓷砖上的标准灰度值众数为48
	grayVlist.clear();
	for (int i = 0; i < 256; ++i)
	{
		pixelPro[i] = hist.at<float>(i) / (const int)(pixelSum);

		if (pixelPro[i] > 0.001)//连续灰度分布筛选
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
	if (m < 6)//保证连续到255时仍被记录
	{
		grayVlist.push_back(grayV);
		grayV.clear();
		m = 0;
	}
	if (grayVlist.size() == 0)
		return origin;
	sort(grayVlist.begin(), grayVlist.end(), SortBysize_int);
	threshold = threshold+(grayVlist[0][grayVlist[0].size() - 1])+15;//以瓷砖上的标准灰度值众数48为基准值，向连续的灰度分布右门限增长，线性变化率为0.2
	return threshold;
}

//区域生长图像种子点门限
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
		uchar* data = HyImg.ptr<uchar>(growpoint.y);//灰度图本行首地址
		uchar* dataT = SameImg.ptr<uchar>(growpoint.y);//二值图本行首地址
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

			uchar* dataU = HyImg.ptr<uchar>(growpoint.y - 1);//灰度图上一行首地址
			uchar* dataUT = SameImg.ptr<uchar>(growpoint.y - 1);//二值图上一行首地址

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

			uchar* dataD = HyImg.ptr<uchar>(growpoint.y + 1);//灰度图下一行首地址
			uchar* dataDT = SameImg.ptr<uchar>(growpoint.y + 1);//二值图下一行首地址

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
		(ir->repo_not_full).wait(lock); // 生产者等待"产品库缓冲区不为满"这一条件发生.
	}

	(ir->item_buffer)[ir->write_position] = item; // 写入产品.

	needContour.push_back(location_rect);
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
		hole.position.x = 0.5*(pt1.x + pt2.x) + recImg.x + locationpoint.x;//坐标变换回原图
		hole.position.y = 0.5*(pt1.y + pt2.y) + recImg.y + locationpoint.y;
		if (box.height > box.width)
			hole.diameter = box.height;
		else
			hole.diameter = box.height;
		
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
	//ThImg = Mat(MidImg.size(), CV_8UC1, Scalar(0));//二值化原图
	if (_faults->MarkPens.size() != 0)
	{
		(cv::max)(MidImg, Mask_result_big, MidImg);
	}

	Mat ThImgROI, MidImgROI,original_Img_ROI;
	Point Thpt = Point(0, 0);//初始化原点
	int step = 0;
	for (int i = 1; i <= 5; ++i)//局部二值化，约束灰度变化量
	{
		for (int j = 0; j < 2; ++j)
		{
			needContour.clear();
			Thpt.x = j*MidImg.cols*0.5;
			Rect ThRect = Rect(Thpt.x, Thpt.y, MidImg.cols*0.5, MidImg.rows*0.2);
			if (j)//到达右侧区域
				ThRect.width = MidImg.cols - MidImg.cols*0.5;
			if (i == 4)//到达底部区域
				ThRect.height = MidImg.rows - 4 * MidImg.rows*0.2;
			MidImgROI = MidImg(Rect(ThRect));
			original_Img_ROI = original_Img_D(Rect(ThRect));
			/*medianBlur(MidImgROI, MidImgROI,7);*/

			/*ThImgROI = ThImg(Rect(ThRect));*/

			MatND histolist, histolist_r;
			calcHist(&MidImgROI, 1, &channels, Mat(), histolist, 1, &size, ranges);//预处理过的图获得灰度分布
			calcHist(&original_Img_ROI, 1, &channels, Mat(), histolist_r, 1, &size, ranges);//原图获得灰度分布
			statistics_gray(ThRect.width, ThRect.height, histolist_r, step);
			int OtsuV = otsuThreshold(ThRect.width, ThRect.height, MidImgROI.at<uchar>(0, 0), histolist);//找到离散灰度值导数的阈值
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
			location_rect = Rect(Point(ThRect.x, ThRect.y), Point(ThRect.x, ThRect.y));//当前ROI左上角顶点在Mid中的坐标
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
			convexHull(linescontours[i], km_contours);//将轮廓转换为凸包
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
			if ((linerect.width-1)*(linerect.height-1) < 2 * int(contourArea(km_contours)))//检测是否为类划痕形状
				continue;
			if (line_YoN(linerect))
			{
				//坐标变换回原图
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
	resize(original_Img_D, original_Img_L, Size(original_Img_D.cols / 3, original_Img_D.rows / 3), 0, 0, INTER_AREA);//截取后的MidImg大小原图缩小
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

	if (image.channels() == 3)//判断是否为彩图
		cvtColor(image, image, CV_RGB2GRAY);

	//直方图变换
	Mat E_image=Equalize(image);
	//求内部小矩形
	ProcessArea(_block);

	//导入分类数据并初始化一次
	static int ret = (Dataload(), 1);

	vector<vector<Point>>filterContours;
	filterContours.push_back(pointlist);
	Mat E_img_mask(E_image.size(),CV_8UC1,Scalar(0));
	drawContours(E_img_mask, filterContours, 0, Scalar(255), CV_FILLED);
	bitwise_and(E_image, E_img_mask, E_image);
	recImg = boundingRect(pointlist);//截取瓷砖区域,对拍摄不全的区域也进行截取

	//截取需要的部分
	MidImg = E_image(Rect(recImg));
	original_Img_D = image(Rect(recImg));//缺陷检测原图

	std::thread linepreprocess(std::mem_fn(&Pretreatment::line2preprocess), this);
	resize(MidImg, re_Img_small, Size(MidImg.cols / 16, MidImg.rows / 16), 0, 0, INTER_LINEAR);

	Handwriting(re_Img_small);//检测人工标记

	linepreprocess.join();

	//分为凹凸检测和线检测线程
	InitItemRepository(&gItemRepository);
	std::thread producer(std::mem_fn(&Pretreatment::ProducerTask), this); // 待检测缺陷的预处理.
	std::thread consumer(std::mem_fn(&Pretreatment::ConsumerTask), this); // 区分缺陷与水渍.
	std::thread line(std::mem_fn(&Pretreatment::linedetect), this);//划痕检测 
	/*auto tn = line.native_handle();
	SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);*///线程优先级调整
	producer.join();
	consumer.join();
	line.join();

	//数据统计整理汇总
	statis_nol(singletonObj->Getvector());

	//回收内存
	vector<Rect>().swap(needContour);
	vector<Rect>().swap(CneedContours);
	vector<vector<Rect>>().swap(Warehousecontours);
	/*needContour.clear();
	CneedContours.clear();
	Warehousecontours.clear();*/
}
