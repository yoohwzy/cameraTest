#include "EdgeInnerDetctor.h"


EdgeInnerDetctor::EdgeInnerDetctor(cv::Mat& img, Block *b)
{
	if (img.channels() == 1)
		image = img.clone();
	else
		cv::cvtColor(img, image, CV_RGB2GRAY);
	block = b;

	block->ABCD2Lines();
	/*cv::line(image, block->A, block->GetPonintByY(3000, &block->LeftLine), cv::Scalar(255));
	cv::line(image, block->A, block->GetPonintByX(3000, &block->UpLine), cv::Scalar(255));
	cv::line(image, block->D, block->GetPonintByX(3000, &block->DownLine), cv::Scalar(255));
	cv::line(image, block->C, block->GetPonintByY(3000, &block->RightLine), cv::Scalar(255));*/

	

	if (1 == 1)
	{
		int index = 0;
		vector<vector<double>> reduceList;
		vector<vector<double>> differList;
		//上边界
		int startX = block->A.x + 30;
		int endX = block->B.x - 30;
		int inc = (float)(endX - startX) / 30 + 0.5;//范围增量
		for (size_t x = startX; x < endX; x += inc, index++)
		{
			cv::Mat tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->UpLine).y, inc, 200)).clone();
			if ((x + inc) > endX)
				tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->UpLine).y, (endX - x), 200)).clone();
			cv::Mat reduceImg;

			cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
			cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半
			cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

#ifdef WRITEIMG
			//保存图片
			char num[10];
			sprintf(num, "%03d", index);
			string strnum(num);
			stringstream ss;
			ss << "EdgeInner\\U\\上_" << strnum << "_reduce.jpg";
			cv::imwrite(ss.str(), reduceImg);
			ss.str("");
			ss << "EdgeInner\\U\\上_" << strnum << ".jpg";
			cv::imwrite(ss.str(), tmpROI);
#endif
			tmpROI.release();

			vector<double> reduceMat;
			vector<double> diffMat;

			for (size_t j = 0; j < reduceImg.rows - 1; j++)
			{
				reduceMat.push_back(reduceImg.ptr<uchar>(j)[0]);
				int different = reduceImg.ptr<uchar>(j + 1)[0] - reduceImg.ptr<uchar>(j)[0];
				diffMat.push_back(different);
			}

			reduceList.push_back(reduceMat);
			differList.push_back(diffMat);
		}
		processAndSaveData(reduceList, differList, "U\\上");
	}
	if (2 == 2)
	{
		int index = 0;
		vector<vector<double>> reduceList;
		vector<vector<double>> differList;
		//下边界
		int startX = block->D.x + 30;
		int endX = block->C.x - 30;
		int inc = (float)(endX - startX) / 30 + 0.5;//范围增量
		for (size_t x = startX; x < endX; x += inc, index++)
		{
			cv::Mat tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->DownLine).y - 200, inc, 200)).clone();
			if ((x + inc) > endX)
				tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->DownLine).y - 200, (endX - x), 200)).clone();
			cv::Mat reduceImg;

			cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
			cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半
			cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

#ifdef WRITEIMG
			//保存图片
			char num[10];
			sprintf(num, "%03d", index);
			string strnum(num);
			stringstream ss;
			ss << "EdgeInner\\D\\下_" << strnum << "_reduce.jpg";
			cv::imwrite(ss.str(), reduceImg);
			ss.str("");
			ss << "EdgeInner\\D\\下_" << strnum << ".jpg";
			cv::imwrite(ss.str(), tmpROI);
#endif

			tmpROI.release();

			vector<double> reduceMat;
			vector<double> diffMat;

			for (size_t j = 0; j < reduceImg.rows - 1; j++)
			{
				reduceMat.push_back(reduceImg.ptr<uchar>(j)[0]);
				int different = reduceImg.ptr<uchar>(j + 1)[0] - reduceImg.ptr<uchar>(j)[0];
				diffMat.push_back(different);
			}

			reduceList.push_back(reduceMat);
			differList.push_back(diffMat);
		}
		processAndSaveData(reduceList, differList, "D\\下");
	}




	if (3 == 3)
	{
		int index = 0;
		vector<vector<double>> reduceList;
		vector<vector<double>> differList;
		//左边界
		int startY = block->A.y + 100;
		int endY = block->D.y - 100;
		int inc = (float)(endY - startY) / 60 + 0.5;//范围增量
		for (size_t y = startY; y < endY; y += inc, index++)
		{
			cv::Mat tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->LeftLine).x, y, 200, inc)).clone();
			if ((y + inc) > endY)
				tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->LeftLine).x, y, 200, (endY - y))).clone();
			cv::Mat reduceImg;

			cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
			cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols/2, reduceImg.rows));//宽缩减为一半
			cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

#ifdef WRITEIMG
			//保存图片
			char num[10];
			sprintf(num, "%03d", index);
			string strnum(num);
			stringstream ss;
			ss << "EdgeInner\\L\\左_" << strnum << "_reduce.jpg";
			cv::imwrite(ss.str(), reduceImg);
			ss.str("");
			ss << "EdgeInner\\L\\左_" << strnum << ".jpg";
			cv::imwrite(ss.str(), tmpROI);
#endif

			tmpROI.release();

			vector<double> reduceMat;
			vector<double> diffMat;

			for (size_t j = 0; j < reduceImg.cols - 1; j++)
			{
				reduceMat.push_back(reduceImg.ptr<uchar>(0)[j]);
				int different = reduceImg.ptr<uchar>(0)[j + 1] - reduceImg.ptr<uchar>(0)[j];
				diffMat.push_back(different);
			}

			reduceList.push_back(reduceMat);
			differList.push_back(diffMat);
		}
		processAndSaveData(reduceList, differList, "L\\左");
	}


	if (4 == 4)
	{
		int index = 0;
		vector<vector<double>> reduceList;
		vector<vector<double>> differList;
		//右边界
		int startY = block->A.y + 100;
		int endY = block->D.y - 100;
		int inc = (float)(endY - startY) / 60 + 0.5;//范围增量
		for (size_t y = startY; y < endY; y += inc, index++)
		{
			cv::Point p = block->GetPonintByY(y, &block->RightLine);
			cv::Mat tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->RightLine).x - 200, y, 200, inc)).clone();
			if ((y + inc) > endY)
				tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->RightLine).x - 200, y, 200, (endY - y))).clone();
			cv::Mat reduceImg;

			cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
			cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半
			cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

#ifdef WRITEIMG
			//保存图片
			char num[10];
			sprintf(num, "%03d", index);
			string strnum(num);
			stringstream ss;
			ss << "EdgeInner\\R\\右_" << strnum << "_reduce.jpg";
			cv::imwrite(ss.str(), reduceImg);
			ss.str("");
			ss << "EdgeInner\\R\\右_" << strnum << ".jpg";
			cv::imwrite(ss.str(), tmpROI);
#endif

			tmpROI.release();

			vector<double> reduceMat;
			vector<double> diffMat;

			for (size_t j = 0; j < reduceImg.cols - 1; j++)
			{
				reduceMat.push_back(reduceImg.ptr<uchar>(0)[j]);
				int different = reduceImg.ptr<uchar>(0)[j + 1] - reduceImg.ptr<uchar>(0)[j];
				diffMat.push_back(different);
			}

			reduceList.push_back(reduceMat);
			differList.push_back(diffMat);
		}
		processAndSaveData(reduceList, differList, "R\\右");
	}
}

double EdgeInnerDetctor::getFrechetDistance(vector<double> lineA, vector<double> lineB)
{
	int lineASize = lineA.size();
	int lineBSize = lineB.size();

	double dist = 100000;
	if (lineBSize > lineASize)
		return dist;
	
	vector<double> tmpDists;
	//计算初始距离
	double tmpDist = -1;
	//左移、右移N次距离
	for (int offset = -10; offset < 10; offset++)
	{
		tmpDist = -1;
		int x2 = offset;
		for (int x1 = 0; x1 < lineASize; x1++)
		{
			if (x2 > (lineBSize-1))
				break;
			if (x2 < 0)
			{
				x2++;
				continue;
			}
			double y1 = (double)lineA[x1];
			double y2 = (double)lineB[x2];
			double thisTurnDist = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
			if (tmpDist < thisTurnDist)//保存每轮的最大值
				tmpDist = thisTurnDist;
			x2++;
		}
		if (tmpDist != -1)
			tmpDists.push_back(tmpDist);
		else if (tmpDist == 0)
		{
			return 0;
		}
	}
	for (size_t i = 0; i < tmpDists.size(); i++)//返回最大值数组中的最小值即是距离
	{
		if (dist > tmpDists[i])
			dist = tmpDists[i];
	}
	return dist;
}

void EdgeInnerDetctor::saveVector(vector<vector<double>> lines,string flodername, string filename)
{
	stringstream ss;
	ss << flodername << filename;
	ofstream of_DiffFrechetDist(ss.str(), ios::out);
	for (size_t i = 0; i < lines.size(); i++)
	{
		of_DiffFrechetDist << i << ",";
		for (size_t j = 0; j < lines[i].size(); j++)
			of_DiffFrechetDist << lines[i][j] << ",";
		of_DiffFrechetDist << "\r\n";
	}
}



void EdgeInnerDetctor::processAndSaveData(vector<vector<double>>& reduceList, vector<vector<double>>& differList, string prefix)
{
	////正规化
	//for (size_t i = 0; i < reduceList.size(); i++)
	//{
	//	double maxDiff = 0;
	//	for (size_t j = 0; j < reduceList[i].size(); j++)
	//	{
	//		if (maxDiff < abs(reduceList[i][j]))
	//			maxDiff = abs(reduceList[i][j]);
	//	}
	//	if (maxDiff > 0)
	//		for (size_t j = 0; j < reduceList[i].size(); j++)
	//		{
	//			if (reduceList[i][j] != 0)
	//				reduceList[i][j] = reduceList[i][j] / maxDiff;
	//		}
	//}


	//计算欧氏距离
	vector<vector<double>> reduceDists;
	for (size_t i = 0; i < reduceList.size(); i++)
	{
		vector<double> tmp;
		reduceDists.push_back(tmp);
		for (size_t j = 0; j < reduceList.size(); j++)
			if (i != j)
			{
				double sum = 0;
				for (size_t k = 0; k < reduceList[i].size(); k++)
					sum += (reduceList[i][k] - reduceList[j][k])*(reduceList[i][k] - reduceList[j][k]);
				reduceDists[i].push_back(sqrt(sum));
			}
			else
				reduceDists[i].push_back(0);
	}

	//计算弗雷歇距离
	vector<vector<double>> reduceFrechetDist;
	for (size_t i = 0; i < reduceList.size(); i++)
	{
		vector<double> tmp;
		reduceFrechetDist.push_back(tmp);
		for (size_t j = 0; j < reduceList.size(); j++)
			if (i != j)
				reduceFrechetDist[i].push_back(getFrechetDistance(reduceList[i], reduceList[j]));
			else
				reduceFrechetDist[i].push_back(0);
	}




	//将一阶导转化为相对值
	for (size_t i = 0; i < differList.size(); i++)
	{
		double maxDiff = 0;
		for (size_t j = 0; j < differList[i].size(); j++)
		{
			if (maxDiff < abs(differList[i][j]))
				maxDiff = abs(differList[i][j]);
		}
		if (maxDiff > 0)
			for (size_t j = 0; j < differList[i].size(); j++)
			{
				if (differList[i][j] != 0)
					differList[i][j] = differList[i][j] / maxDiff;
			}
	}


	//计算欧氏距离
	vector<vector<double>> diffDists;
	for (size_t i = 0; i < differList.size(); i++)
	{
		vector<double> tmp;
		diffDists.push_back(tmp);

		for (size_t j = 0; j < differList.size(); j++)
			if (i != j)
			{
				double sum = 0;
				for (size_t k = 0; k < differList[i].size(); k++)
					sum += (differList[i][k] - differList[j][k])*(differList[i][k] - differList[j][k]);
				diffDists[i].push_back(sqrt(sum));

			}
			else
				diffDists[i].push_back(0);
	}
	//计算弗雷歇距离
	vector<vector<double>> diffFDists;
	for (size_t i = 0; i < differList.size(); i++)
	{
		vector<double> tmp;
		diffFDists.push_back(tmp);
		for (size_t j = 0; j < differList.size(); j++)
			if (i != j)
				diffFDists[i].push_back(getFrechetDistance(differList[i], differList[j]));
			else
				diffFDists[i].push_back(0);
	}

#ifdef WRITEIMG
	stringstream ss_floder;
	ss_floder << "EdgeInner\\" << prefix;//加前缀以区别上下边界

	//保存数据 reduce
	saveVector(reduceList, ss_floder.str(), "reduce normalize.txt");
	//saveVector(reduceList, ss_floder.str(), "reduce.txt");
	saveVector(reduceDists, ss_floder.str(), "reduce欧氏距离.txt");
	saveVector(reduceFrechetDist, ss_floder.str(), "reduce弗雷歇距离.txt");


	//保存数据 diff
	saveVector(differList, ss_floder.str(), "diff.txt");
	saveVector(diffDists, ss_floder.str(), "diff欧氏距离.txt");
	saveVector(diffFDists, ss_floder.str(), "diff弗雷歇距离.txt");
#endif
}




EdgeInnerDetctor::~EdgeInnerDetctor()
{

}
 