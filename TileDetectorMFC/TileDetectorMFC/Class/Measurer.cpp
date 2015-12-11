#include "Measurer.h"


Measurer::Measurer(Block *_b, int TileStandardWidth, int TileStandardHeight)
{
	b = _b;
	tileStandardWidth_mm = TileStandardWidth;
	tileStandardHeight_mm = TileStandardHeight;
	ObserveCalibration();
}
Measurer::Measurer()
{
	ifstream fin("MeasurerData.txt");
	if (!fin)
	{
		ExitWithError("MeasurerData.txt不能打开");
		return;
	}
	//MilliMeterPerPix_Width = tileStandardWidth_mm * 2 / (tileStandardWidth_AB_pix + tileStandardWidth_CD_pix);
	//MilliMeterPerPix_Height = tileStandardHeight_mm * 2 / (tileStandardWidth_AD_pix + tileStandardWidth_BC_pix);
	//MilliMeterPerPix_Diagonal = sqrt(tileStandardWidth_mm*tileStandardWidth_mm + tileStandardHeight_mm*tileStandardHeight_mm) * 2 / (tileStandardDiagonalAC_pix + tileStandardDiagonalBD_pix);

	fin >> MilliMeterPerPix_Width >> MilliMeterPerPix_Height >> MilliMeterPerPix_Diagonal;
}

Measurer::~Measurer()
{
	b = NULL;
}

void Measurer::PincushionCalibration(cv::Mat calibrationImg)
{
	//第一步获取每个格子的边界点
	//投影
	cv::Mat calibrationGrayImg;
	if (calibrationImg.channels() == 3)
		cv::cvtColor(calibrationImg, calibrationGrayImg, CV_BGR2GRAY);
	else
		calibrationGrayImg = calibrationImg.clone();



	vector<int> colssum;
	//初始化垂直投影数组
	for (int i = 0; i < calibrationGrayImg.cols; i++)
		colssum.push_back(0);
	//将图像垂直投影与水平投影到数组
	int height = calibrationGrayImg.rows >200 ? 200 : calibrationGrayImg.rows;
	for (int i = 0; i < height; i++)
	{
		uchar* linehead = calibrationGrayImg.ptr<uchar>(i);//每行的起始地址
		for (int j = 0; j < calibrationGrayImg.cols; j++)
				colssum[j] += linehead[j];
	}
	//求一阶导
	vector<int> colssum_diff;
	colssum_diff.push_back(0);
	for (int i = 1; i < colssum.size(); i++)
	{
		colssum_diff.push_back(colssum[i] - colssum[i - 1]);
	}

	//边界点位置
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
	//确定中间间距长度（作为校正用的标准长度）
	int standard_length = Xs[b] - Xs[a];
	//std::cout << "a = " << a << " b = " << b << endl;


	//确认实际点应该所处的位置
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


	vector<double> map(calibrationGrayImg.cols, -1);//照片映射到实际，表示照片X轴上，每个点经标定后应该位于哪个位置。
	//从左向右校正
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
	//无需校正
	for (int i = Xs[a]; i < Xs[b]; i++)
	{
		map[i] = i;
	}

	PincushionMap = map;
	vector<double> map_anti(calibrationGrayImg.cols, -1);//实际映射到照片（用以确定实际X位置处，像素的组成）
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
	//最左侧点的左边范围
	int j_tmp = Xs[0] - 1;
	for (int i = Xs_standard[0] - 1; i >=0 ; i--)
	{
		map_anti[i] = j_tmp;
		j_tmp--;
		if (j_tmp < 0 || i < 0)
			break;
	}
	//最右侧点的右边范围
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
		uchar* linehead = calibrat.ptr<uchar>(i);//每行的起始地址
		for (int j = 0; j < calibrat.cols; j++)
		{
			linehead[j*3 +0] = 0;
			linehead[j*3+1] = 0;
			linehead[j*3+2] = 0;
		}
	}

	int image_rows = calibrat.rows;//行数
	int elementCount = calibrat.cols;//每行元素数
	for (int i = 0; i < image_rows; i++)
	{
		uchar* linehead = calibrat.ptr<uchar>(i);//每行的起始地址
		uchar* oldhead = img.ptr<uchar>(i);//每行的起始地址
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
	int width = b->imageWidth;
	int max = 0;
	for (int i = 0; i < b->imageWidth; i++)
	{
		int v = (*b).UpLine.k*(i - (*b).UpLine.x0);
		if (v > max)
			max = v;
		RowAdjust.push_back(v);
	}
	for (int i = 0; i < width; i++)
	{
		RowAdjust[i] -= max;
	}

	//此处为复制，非指针
	A_Standard = (*b).A;
	B_Standard = (*b).B;
	C_Standard = (*b).C;
	D_Standard = (*b).D;

	//矫正ABCD四点
	A_Standard.y = A_Standard.y - RowAdjust[A_Standard.x];
	B_Standard.y = B_Standard.y - RowAdjust[B_Standard.x];
	C_Standard.y = C_Standard.y - RowAdjust[C_Standard.x];
	D_Standard.y = D_Standard.y - RowAdjust[D_Standard.x];
	//计算出四条边的长度
	tileStandardWidth_AB_pix = sqrt((A_Standard.x - B_Standard.x)*(A_Standard.x - B_Standard.x) + (A_Standard.y - B_Standard.y)*(A_Standard.y - B_Standard.y));
	tileStandardWidth_CD_pix = sqrt((C_Standard.x - D_Standard.x)*(C_Standard.x - D_Standard.x) + (C_Standard.y - D_Standard.y)*(C_Standard.y - D_Standard.y));

	tileStandardWidth_AD_pix = sqrt((A_Standard.x - D_Standard.x)*(A_Standard.x - D_Standard.x) + (A_Standard.y - D_Standard.y)*(A_Standard.y - D_Standard.y));
	tileStandardWidth_BC_pix = sqrt((B_Standard.x - C_Standard.x)*(B_Standard.x - C_Standard.x) + (B_Standard.y - C_Standard.y)*(B_Standard.y - C_Standard.y));

	//对角线（pix）
	tileStandardDiagonalAC_pix = sqrt((A_Standard.x - C_Standard.x)*(A_Standard.x - C_Standard.x) + (A_Standard.y - C_Standard.y)*(A_Standard.y - C_Standard.y));
	tileStandardDiagonalBD_pix = sqrt((B_Standard.x - D_Standard.x)*(B_Standard.x - D_Standard.x) + (B_Standard.y - D_Standard.y)*(B_Standard.y - D_Standard.y));

	//计算一个像素表示多少长度（mm）
	MilliMeterPerPix_Width = tileStandardWidth_mm * 2 / (tileStandardWidth_AB_pix + tileStandardWidth_CD_pix);
	MilliMeterPerPix_Height = tileStandardHeight_mm * 2 / (tileStandardWidth_AD_pix + tileStandardWidth_BC_pix);
	MilliMeterPerPix_Diagonal = sqrt(tileStandardWidth_mm*tileStandardWidth_mm + tileStandardHeight_mm*tileStandardHeight_mm) * 2 / (tileStandardDiagonalAC_pix + tileStandardDiagonalBD_pix);

	ofstream fout("MeasurerData.txt");
	fout << setprecision(24) << MilliMeterPerPix_Width << " " << MilliMeterPerPix_Height << " " << MilliMeterPerPix_Diagonal << endl;
}
void Measurer::ObserveImgAdjust(cv::Mat& Img)
{
	cv::Mat oldimg = Img.clone();
	for (int i = 0; i < b->imageHeight; i++)
	{
		uchar* linehead = Img.ptr<uchar>(i);//每行的起始地址
		for (int j = 0; j < b->imageWidth; j++)
		{
			if ((i + RowAdjust[j]) < 0 || (i + RowAdjust[j]) >= b->imageHeight)
				continue;
			uchar* linehead_tmp = oldimg.ptr<uchar>(i + RowAdjust[j]);//每行的起始地址
			linehead[j * 3 + 0] = linehead_tmp[j * 3 + 0];
			linehead[j * 3 + 1] = linehead_tmp[j * 3 + 1];
			linehead[j * 3 + 2] = linehead_tmp[j * 3 + 2];
		}
	}
}

void Measurer::CaculteSize(Block *_b)
{
	//实际的四个顶点
	cv::Point AA;
	cv::Point BB;
	cv::Point CC;
	cv::Point DD;

	//此处为复制，非指针
	AA = (*_b).A;
	BB = (*_b).B;
	CC = (*_b).C;
	DD = (*_b).D;

	//矫正ABCD四点，并计算一个像素长度为多少
	AA.y = AA.y - RowAdjust[AA.x];
	BB.y = BB.y - RowAdjust[BB.x];
	CC.y = CC.y - RowAdjust[CC.x];
	DD.y = DD.y - RowAdjust[DD.x];


	//上下边长（pix）
	int AB_Len_Square = (AA.x - BB.x)*(AA.x - BB.x) + (AA.y - BB.y)*(AA.y - BB.y);
	AB_Len = sqrt(AB_Len_Square);
	//double& BA_Len = AB_Len;

	int CD_Len_Square = (CC.x - DD.x)*(CC.x - DD.x) + (CC.y - DD.y)*(CC.y - DD.y);
	CD_Len = sqrt(CD_Len_Square);
	//double& DC_Len = CD_Len;

	//左右边长（pix）
	int AD_Len_Square = (AA.x - DD.x)*(AA.x - DD.x) + (AA.y - DD.y)*(AA.y - DD.y);
	AD_Len = sqrt(AD_Len_Square);
	//double& DA_Len = AD_Len;
	int BC_Len_Square = (BB.x - CC.x)*(BB.x - CC.x) + (BB.y - CC.y)*(BB.y - CC.y);
	BC_Len = sqrt(BC_Len_Square);
	//double& CB_Len = BC_Len;

	//对角线（pix）
	int AC_Len_Square = (AA.x - CC.x)*(AA.x - CC.x) + (AA.y - CC.y)*(AA.y - CC.y);
	AC_Len = sqrt(AC_Len_Square);
	//double& CA_Len = AC_Len;
	int BD_Len_Square = (BB.x - DD.x)*(BB.x - DD.x) + (BB.y - DD.y)*(BB.y - DD.y);
	BD_Len = sqrt(BD_Len_Square);
	//double& DB_Len = BD_Len;

	AB_mm = AB_Len*MilliMeterPerPix_Width;
	CD_mm = CD_Len*MilliMeterPerPix_Width;
	AD_mm = AD_Len*MilliMeterPerPix_Height;
	BC_mm = BC_Len*MilliMeterPerPix_Height;

	

	//余弦定理
	//	cosa＝（b ^ 2 + c ^ 2 - a ^ 2) / 2bc
	//	cosb＝（a ^ 2 + c ^ 2 - b ^ 2) / 2ac
	//	cosc＝（a ^ 2 + b ^ 2 - c ^ 2) / 2ab
	angleA = ANGLE(acos((AB_Len_Square + AD_Len_Square - BD_Len_Square) / (2 * AB_Len*AD_Len)));
	angleB = ANGLE(acos((AB_Len_Square + BC_Len_Square - AC_Len_Square) / (2 * AB_Len*BC_Len)));
	angleC = ANGLE(acos((CD_Len_Square + BC_Len_Square - BD_Len_Square) / (2 * CD_Len*CB_Len)));
	angleD = ANGLE(acos((CD_Len_Square + AD_Len_Square - AC_Len_Square) / (2 * CD_Len*AD_Len)));
}