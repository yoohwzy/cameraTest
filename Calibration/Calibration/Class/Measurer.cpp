#include "Measurer.h"


Measurer::Measurer(BlocksDetector *b, MicroDisplayInit *mdii, int TileStandardWidth, int TileStandardHeight)
{
	bd_Standard = b;
	mdi = mdii;
	tileStandardWidth_mm = TileStandardWidth;
	tileStandardHeight_mm = TileStandardHeight;
}


Measurer::~Measurer()
{
}

void Measurer::PincushionCalibration(cv::Mat calibrationImg)
{
	//��һ����ȡÿ�����ӵı߽��
	//ͶӰ
	cv::Mat calibrationGrayImg;
	if (calibrationImg.channels() == 3)
		cv::cvtColor(calibrationImg, calibrationGrayImg, CV_BGR2GRAY);
	else
		calibrationGrayImg = calibrationImg.clone();



	vector<int> colssum;
	//��ʼ����ֱͶӰ����
	for (int i = 0; i < calibrationGrayImg.cols; i++)
		colssum.push_back(0);
	//��ͼ��ֱͶӰ��ˮƽͶӰ������
	int height = calibrationGrayImg.rows >200 ? 200 : calibrationGrayImg.rows;
	for (int i = 0; i < height; i++)
	{
		uchar* linehead = calibrationGrayImg.ptr<uchar>(i);//ÿ�е���ʼ��ַ
		for (int j = 0; j < calibrationGrayImg.cols; j++)
				colssum[j] += linehead[j];
	}
	//��һ�׵�
	vector<int> colssum_diff;
	colssum_diff.push_back(0);
	for (int i = 1; i < colssum.size(); i++)
	{
		colssum_diff.push_back(colssum[i] - colssum[i - 1]);
	}

	//�߽��λ��
	vector<int> Xs;
	for (int i = 20; i < colssum_diff.size() - 20; i++)
	{
		if (abs(colssum_diff[i]) > 1000)
		{
			int max = abs(colssum_diff[i]);
			int max_j = i;
			for (int j = i - 15; j < i + 20; j++)
			{
				if (abs(colssum_diff[j]) > max)
				{
					max = abs(colssum_diff[j]);
				}
			}
			Xs.push_back(max_j);
			i = i + 40;
		}
	}


	int a = Xs.size() / 2;
	int b = a + 1;
	//ȷ���м��೤�ȣ���ΪУ���õı�׼���ȣ�
	int standard_length = Xs[b] - Xs[a];
	//std::cout << "a = " << a << " b = " << b << endl;


	//ȷ��ʵ�ʵ�Ӧ��������λ��
	vector<int> Xs_standard(Xs.size(), -1);
	for (int i = a; i >= 0; i--)
	{
		if (i == a)
			Xs_standard[i] = Xs[i];
		else
		{
			Xs_standard[i] = Xs_standard[i + 1] - standard_length;
		}
	}
	for (int i = b; i < Xs.size(); i++)
	{
		if (i == b)
			Xs_standard[i] = Xs[i];
		else
		{
			Xs_standard[i] = Xs_standard[i - 1] + standard_length;
		}
	}


	vector<double> map(calibrationGrayImg.cols, -1);//��Ƭӳ�䵽ʵ�ʣ���ʾ��ƬX���ϣ�ÿ���㾭�궨��Ӧ��λ���ĸ�λ�á�
	//��������У��
	for (int i = 0; i < Xs.size() - 1; i++)
	{
		int a1 = Xs[i];
		int a2 = Xs[i + 1];
		int aa1 = Xs_standard[i];
		int aa2 = Xs_standard[i + 1];
		double k = standard_length / (double)(a2 - a1);
		double b = aa2 - k * a2;
		if ((i + 1) == (Xs.size() - 1))
			a2 = 4095;
		for (int j = a1; j <= a2; j++)
		{
			double xx = k * j + b;
			map[j] = xx;
		}
	}
	//����У��
	for (int i = Xs[a]; i < Xs[b]; i++)
	{
		map[i] = i;
	}

	PincushionMap = map;
	vector<double> map_anti(calibrationGrayImg.cols, -1);//ʵ��ӳ�䵽��Ƭ������ȷ��ʵ��Xλ�ô������ص���ɣ�
	for (int i = 0; i < Xs.size() - 2; i++)
	{
		int a1 = Xs[i];
		int a2 = Xs[i + 1];
		int aa1 = Xs_standard[i];
		int aa2 = Xs_standard[i + 1];
		double k = standard_length / (double)(a2 - a1);
		double b = aa2 - k * a2;
		for (int j = aa1; j < aa2; j++)
		{
			double x = (j - b) / k;
			map_anti[j] = x;
		}
	}
	//���������߷�Χ
	int j_tmp = Xs[0] - 1;
	for (int i = Xs_standard[0] - 1; i >=0 ; i--)
	{
		map_anti[i] = j_tmp;
		j_tmp--;
		if (j_tmp < 0 || i < 0)
			break;
	}
	//���Ҳ����ұ߷�Χ
	j_tmp = Xs[Xs.size() - 1];
	for (int i = Xs_standard[Xs_standard.size() - 1]; i < calibrationImg.cols; i++)
	{
		map_anti[i] = j_tmp;
		j_tmp++;
		if (j_tmp >= calibrationImg.cols)
			break;
	}
	PincushionMap_Anti = map_anti;
}
void Measurer::PincushionImgAdjust(cv::Mat& Img)
{
	cv::Mat img = Img.clone();
	cv::Mat calibrat = Img;

	for (int i = 0; i < calibrat.rows; i++)
	{
		uchar* linehead = calibrat.ptr<uchar>(i);//ÿ�е���ʼ��ַ
		for (int j = 0; j < calibrat.cols; j++)
		{
			linehead[j*3 +0] = 0;
			linehead[j*3+1] = 0;
			linehead[j*3+2] = 0;
		}
	}

	int image_rows = calibrat.rows;//����
	int elementCount = calibrat.cols;//ÿ��Ԫ����
	for (int i = 0; i < image_rows; i++)
	{
		uchar* linehead = calibrat.ptr<uchar>(i);//ÿ�е���ʼ��ַ
		uchar* oldhead = img.ptr<uchar>(i);//ÿ�е���ʼ��ַ
		for (int j = 0; j < elementCount; j++)
		{
			if (PincushionMap_Anti[j] < 0)
				continue;
			else
			{
				unsigned int pos1 = PincushionMap_Anti[j];
				unsigned int pos2 = pos1 + 1;
				double percent1 = PincushionMap_Anti[j] - pos1;
				double percent2 = 1 - percent1;
				if (pos1 >= 4096 && pos2 >= 4096)
					continue;
				linehead[j * 3 + 0] = oldhead[pos1 * 3 + 0] * percent1 + oldhead[pos2 * 3 + 0] * percent2;
				linehead[j * 3 + 1] = oldhead[pos1 * 3 + 1] * percent1 + oldhead[pos2 * 3 + 1] * percent2;
				linehead[j * 3 + 2] = oldhead[pos1 * 3 + 2] * percent1 + oldhead[pos2 * 3 + 2] * percent2;
			}
		}
	}
}




void Measurer::ObserveCalibration()
{
	int width = (*mdi).width;
	int max = 0;
	for (int i = 0; i < (*mdi).width; i++)
	{
		int v = (*bd_Standard).UpLine.k*(i - (*bd_Standard).UpLine.x0);
		if (v > max)
			max = v;
		RowAdjust.push_back(v);
	}
	for (int i = 0; i < width; i++)
	{
		RowAdjust[i] -= max;
	}

	//�˴�Ϊ���ƣ���ָ��
	A_Standard = (*bd_Standard).A;
	B_Standard = (*bd_Standard).B;
	C_Standard = (*bd_Standard).C;
	D_Standard = (*bd_Standard).D;

	//����ABCD�ĵ�
	A_Standard.y = A_Standard.y - RowAdjust[A_Standard.x];
	B_Standard.y = B_Standard.y - RowAdjust[B_Standard.x];
	C_Standard.y = C_Standard.y - RowAdjust[C_Standard.x];
	D_Standard.y = D_Standard.y - RowAdjust[D_Standard.x];
	//����������ߵĳ���
	tileStandardWidthUp_pix = sqrt((A_Standard.x - B_Standard.x)*(A_Standard.x - B_Standard.x) + (A_Standard.y - B_Standard.y)*(A_Standard.y - B_Standard.y));
	tileStandardWidthDown_pix = sqrt((C_Standard.x - D_Standard.x)*(C_Standard.x - D_Standard.x) + (C_Standard.y - D_Standard.y)*(C_Standard.y - D_Standard.y));

	tileStandardHeightLeft_pix = sqrt((A_Standard.x - D_Standard.x)*(A_Standard.x - D_Standard.x) + (A_Standard.y - D_Standard.y)*(A_Standard.y - D_Standard.y));
	tileStandardHeightRight_pix = sqrt((B_Standard.x - C_Standard.x)*(B_Standard.x - C_Standard.x) + (B_Standard.y - C_Standard.y)*(B_Standard.y - C_Standard.y));

	//�Խ��ߣ�pix��
	tileStandardDiagonalAC_pix = sqrt((A_Standard.x - C_Standard.x)*(A_Standard.x - C_Standard.x) + (A_Standard.y - C_Standard.y)*(A_Standard.y - C_Standard.y));
	tileStandardDiagonalBD_pix = sqrt((B_Standard.x - D_Standard.x)*(B_Standard.x - D_Standard.x) + (B_Standard.y - D_Standard.y)*(B_Standard.y - D_Standard.y));

	//����һ�����ر�ʾ���ٳ��ȣ�mm��
	MilliMeterPerPix_Width = tileStandardWidth_mm * 2 / (tileStandardWidthUp_pix + tileStandardWidthDown_pix);
	MilliMeterPerPix_Height = tileStandardHeight_mm * 2 / (tileStandardHeightLeft_pix + tileStandardHeightRight_pix);
}
void Measurer::ObserveImgAdjust(cv::Mat& Img)
{
	cv::Mat oldimg = Img.clone();
	for (int i = 0; i < (*mdi).MaxPics; i++)
	{
		uchar* linehead = Img.ptr<uchar>(i);//ÿ�е���ʼ��ַ
		for (int j = 0; j < (*mdi).width; j++)
		{
			if ((i + RowAdjust[j]) < 0 || (i + RowAdjust[j]) >= (*mdi).MaxPics)
				continue;
			uchar* linehead_tmp = oldimg.ptr<uchar>(i + RowAdjust[j]);//ÿ�е���ʼ��ַ
			linehead[j * 3 + 0] = linehead_tmp[j * 3 + 0];
			linehead[j * 3 + 1] = linehead_tmp[j * 3 + 1];
			linehead[j * 3 + 2] = linehead_tmp[j * 3 + 2];
		}
	}
}

void Measurer::CaculteSize(BlocksDetector *bd)
{
	//ʵ�ʵ��ĸ�����
	cv::Point AA;
	cv::Point BB;
	cv::Point CC;
	cv::Point DD;

	//�˴�Ϊ���ƣ���ָ��
	AA = (*bd).A;
	BB = (*bd).B;
	CC = (*bd).C;
	DD = (*bd).D;

	//����ABCD�ĵ㣬������һ�����س���Ϊ����
	AA.y = AA.y - RowAdjust[AA.x];
	BB.y = BB.y - RowAdjust[BB.x];
	CC.y = CC.y - RowAdjust[CC.x];
	DD.y = DD.y - RowAdjust[DD.x];


	//���±߳���pix��
	double upLen = sqrt((AA.x - BB.x)*(AA.x - BB.x) + (AA.y - BB.y)*(AA.y - BB.y));
	double downLen = sqrt((CC.x - DD.x)*(CC.x - DD.x) + (CC.y - DD.y)*(CC.y - DD.y));
	//���ұ߳���pix��
	double leftLen = sqrt((AA.x - DD.x)*(AA.x - DD.x) + (AA.y - DD.y)*(AA.y - DD.y));
	double rightLen = sqrt((BB.x - CC.x)*(BB.x - CC.x) + (BB.y - CC.y)*(BB.y - CC.y));
	//�Խ��ߣ�pix��
	double diagonal1 = sqrt((AA.x - CC.x)*(AA.x - CC.x) + (AA.y - CC.y)*(AA.y - CC.y));
	double diagonal2 = sqrt((BB.x - DD.x)*(BB.x - DD.x) + (BB.y - DD.y)*(BB.y - DD.y));

	double up_mm = upLen*MilliMeterPerPix_Width;
	double down_mm = downLen*MilliMeterPerPix_Width;
	double left_mm = leftLen*MilliMeterPerPix_Height;
	double right_mm = rightLen*MilliMeterPerPix_Height;

}