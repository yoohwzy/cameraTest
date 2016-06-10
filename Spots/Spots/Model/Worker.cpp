#include "Worker.h"
#include <Controller/ControllerModel.h>
#include <Class/Debug/MFCConsole.h>


//引入算法

#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeSimilarDetector.h>
#include <Algorithm/BlockEdgeLineDetector.h>
#include <Algorithm/EdgeFaultLineDetector.h>
Worker::Worker(string WorkerInfo)
{
	workerInfo = WorkerInfo;
}


Worker::~Worker()
{
	if (P_Faults != NULL)
	{
		delete P_Faults;
		P_Faults = NULL;
	}
	if (P_Block != NULL)
	{
		delete P_Block;
		P_Block = NULL;
	}
}

void Worker::StartWork()
{
	std::thread t_workThread(std::mem_fn(&Worker::work), this);
	t_workThread.detach();
	//work();
}
void Worker::work()
{
	if (P_Faults != NULL)
	{
		delete P_Faults;
		P_Faults = NULL;
	}
	if (P_Block != NULL)
	{
		delete P_Block;
		P_Block = NULL;
	}
	P_Faults = new Faults();
	P_Block = new Block(image.cols, image.rows);
	MyStatus = WorkerStatus::InProcessing;
	cv::Mat grayImg = image;
	/********************************************/
	//开始图像处理
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//TODO:暂时用正规化处理图片
	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//开始图像处理
	Status status = algorithmSynthesize(grayImg);
	
	//cv::Mat tmp = grayImg.clone();
	if (status != Worker::Status::NotFound)
	{
		int type = 1;//产品级别  1 A级 2 B级 3 C级 4 不合格
		if (status == Status::SizeError)
		{
			type = 4;
		}
		else
		{
			//绘制缺陷图像
			drawFaults(grayImg);
			cv::line(grayImg, cv::Point(0, P_Block->UpLine.k * (0 - P_Block->UpLine.x0) + P_Block->UpLine.y0), cv::Point(grayImg.cols, P_Block->UpLine.k * (grayImg.cols - P_Block->UpLine.x0) + P_Block->UpLine.y0), cv::Scalar(0, 0, 255), 1);
			cv::line(grayImg, cv::Point(0, P_Block->DownLine.k * (0 - P_Block->DownLine.x0) + P_Block->DownLine.y0), cv::Point(grayImg.cols, P_Block->DownLine.k * (grayImg.cols - P_Block->DownLine.x0) + P_Block->DownLine.y0), cv::Scalar(0, 255, 255), 1);
			cv::line(grayImg, cv::Point((grayImg.rows - P_Block->LeftLine.y0) / P_Block->LeftLine.k + P_Block->LeftLine.x0, grayImg.rows), cv::Point((0 - P_Block->LeftLine.y0) / P_Block->LeftLine.k + P_Block->LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
			cv::line(grayImg, cv::Point((grayImg.rows - P_Block->RightLine.y0) / P_Block->RightLine.k + P_Block->RightLine.x0, grayImg.rows), cv::Point((0 - P_Block->RightLine.y0) / P_Block->RightLine.k + P_Block->RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);

			//产品分级
			if (P_Faults->EdgeFaults.size() > 0)
			{
				type = 4;
			}
			if (type != 4)//崩边分级
			{
				double _EDGE_TOTAL_LENGTH = 0;//崩边总长度
				double _EDGE_TOTAL_DEEP = 0;//崩边总深度
				for (int i = 0; i < P_Faults->BrokenEdges.size(); i++)
				{
					_EDGE_TOTAL_LENGTH += P_Faults->BrokenEdges[i].length_mm;
					_EDGE_TOTAL_DEEP += P_Faults->BrokenEdges[i].deep_mm;
					if (P_Faults->BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_C || P_Faults->BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_C)
					{
						type = 4;
						break;
					}
					if (type < 3 && (P_Faults->BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_B || P_Faults->BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_B))
					{
						type = 3;
					}

					if (type < 2 && (P_Faults->BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_A || P_Faults->BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_A))
					{
						type = 2;
					}
				}
				if (_EDGE_TOTAL_LENGTH > Classify_EDGE_TOTAL_LENGTH_C || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_C)
					type = 4;
				if (type < 3 && (_EDGE_TOTAL_LENGTH > Classify_EDGE_TOTAL_LENGTH_B || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_B))
					type = 3;
				if (type < 2 && (_EDGE_TOTAL_LENGTH > Classify_EDGE_TOTAL_LENGTH_A || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_A))
					type = 2;

				stringstream ss;
				ss << workerInfo << "崩边总长=" << _EDGE_TOTAL_LENGTH << "mm" << endl;
				ss << workerInfo << "崩边总深=" << _EDGE_TOTAL_DEEP << "mm" << endl;
				MFCConsole::Output(ss.str());
			}
			if (type != 4)//内部缺陷分级
			{
				for (size_t i = 0; i < P_Faults->Crazings.size(); i++)
				{

				}
			}
			if (1 == 1)
			{
				stringstream ss;
				ss << workerInfo << "type=" << type << endl;
				MFCConsole::Output(ss.str());
			}
		}
		P_Controller->ImgProcessOverCallBack(SN, grayImg, *(P_Block), type);
		//if (type > 1)
		//	P_Controller->imageSave(tmp);
	}
	else
	{
		P_Controller->ImgProcessOverCallBack(SN, grayImg, *(P_Block), 0);
	}


	//显示结果
	MyStatus = WorkerStatus::Free;
	return;
}
Worker::Status Worker::algorithmSynthesize(cv::Mat img)
{
	P_Block = new Block(img.size().width, img.size().height);

	if (img.cols == 0)
	{
		MFCConsole::Output("Img Empty!\r\n");
		return Status::NoImage;
	}
	cv::Mat grayImg = img;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//定位
	if (positioning(grayImg) == 0)
		return Status::NotFound;

	//若没有标定数据，则粗略计算以获得缺陷尺寸
	if (Block::X_mmPerPix < 0)
	{
		Block::X_mmPerPix = Block::Standard_Width_mm / (double)abs(P_Block->B.x - P_Block->A.x);
		Block::Y_mmPerPix = Block::Standard_Length_mm / (double)abs(P_Block->C.y - P_Block->B.y);
	}
	//计算对角线
	Block::Calculate(P_Block);
	double ac_bd_diff_mm = abs(P_Block->ac_mm - P_Block->bd_mm);



	// 边缘缺陷
	if (detectEdge(grayImg))
	{
		return Status::Rejected;
	}

	//表面缺陷
	detectInner(grayImg);
	return Status::TypeA;
}
//瓷砖定位，返回是否找到瓷砖
//1 找到
//0 未找到
//-1 有缺陷
int Worker::positioning(cv::Mat grayImg)
{
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//进行定位
	BlockLocalizer bl = BlockLocalizer(grayImg, P_Block, P_Faults);
	bl.Run();


	if (MFCConsole::IsOpened)
	{
		stringstream ss;
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		ss << "定位算法BlockLocalizer：" << t << endl;
		MFCConsole::Output(ss.str());
	}

	if (bl.NotFoundBlockFlag == true)
	{
		//未检测到瓷砖
		return 0;
	}
	//if (bl.BrokenEdgeFlag == true)
	//{
	//	//cv::cvtColor(img, img, CV_GRAY2BGR);
	//	//for (size_t i = 0; i < P_Faults->BrokenEdges.size(); i++)
	//	//{
	//	//	cv::circle(img, P_Faults->BrokenEdges[i].position, P_Faults->BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
	//	//}
	//	//cv::namedWindow("1", 0);
	//	//cv::imshow("1", img);
	//	//cv::waitKey(0);
	//	return _Status::_Edge_Broken;
	//}
	return 1;
}
//边缘缺陷检测
//1 找到
//-1 有缺陷
int Worker::detectEdge(cv::Mat grayImg)
{
	double t = 0;
	P_Block->Lines2ABCD();

	if (BlockEdgeLineDetector::Enabled)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeLineDetector  beld = BlockEdgeLineDetector(grayImg, P_Block, P_Faults);
		beld.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "算法BlockEdgeLineDetector用时：" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	if (BlockEdgeSimilarDetector::Enabled)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeSimilarDetector besd = BlockEdgeSimilarDetector(grayImg, P_Block, P_Faults);
		besd.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "算法BlockEdgeSimilarDetector用时：" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return 1;
}

//内部缺陷检测
//1 找到
//-1 有缺陷
int Worker::detectInner(cv::Mat grayImg)
{
	if (Pretreatment_Enable)
	{
		stringstream ss;
		double t = (double)cv::getTickCount();

		//瓷砖内部缺陷检测
		ss << "瓷砖内部缺陷检测 开始" << endl;
		Pretreatment p;
		p.pretreatment(grayImg, P_Block, P_Faults);

		if (MFCConsole::IsOpened)
		{
			ss << SN << " " << "内部有划痕：" << P_Faults->Scratchs.size() << endl;
			ss << SN << " " << "内部有凹点：" << P_Faults->Holes.size() << endl;
			ss << "瓷砖内部缺陷检测 结束" << endl;
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			ss << SN << " " << "内部缺陷检测：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return 1;
}

void Worker::drawFaults(cv::Mat& img)
{
	if (img.channels() == 1)
		cv::cvtColor(img, img, CV_GRAY2RGB);


	if (P_Faults->EdgeFaults.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << P_Faults->EdgeFaults.size() << " 处边缘不相似。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->EdgeFaults.size(); i++)
		{
			cv::rectangle(img, cv::Rect(P_Faults->EdgeFaults[i].position.x - P_Faults->EdgeFaults[i].width,
				P_Faults->EdgeFaults[i].position.y - P_Faults->EdgeFaults[i].height,
				P_Faults->EdgeFaults[i].width * 2,
				P_Faults->EdgeFaults[i].height * 2), Scalar(0, 30 * i, 250));
		}
	}
	if (P_Faults->BrokenEdges.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << P_Faults->BrokenEdges.size() << " 处崩边缺陷，红色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->BrokenEdges.size(); i++)
		{
			cv::circle(img, P_Faults->BrokenEdges[i].position, P_Faults->BrokenEdges[i].length, cv::Scalar(0, 0, 255), 4);
		}
	}
	//if (P_Faults->BrokenCorners.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << workerInfo << SN << " 存在 " << P_Faults->BrokenCorners.size() << " 处崩角缺陷，洋红标出。" << endl;
	//		MFCConsole::Output(ss.str());
	//	}

	//	for (size_t i = 0; i < P_Faults->BrokenCorners.size(); i++)
	//	{
	//		cv::circle(img, P_Faults->BrokenCorners[i].position, P_Faults->BrokenCorners[i].length, cv::Scalar(127, 0, 228), 5);
	//	}
	//}
	//if (P_Faults->SomethingBigs.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << SN << " 存在 " << P_Faults->BrokenEdges.size() << " 处崩角缺陷，洋红标出。" << endl;
	//		MFCConsole::Output( << ss.str();
	//	}

	//	CString str;
	//	str.Format(_T("%d 存在%d处EID缺陷，蓝色标出。\r\n"), SN, P_Faults->SomethingBigs.size());
	//	m_Info += str;
	//	clog += str;
	//	for (size_t i = 0; i < P_Faults->SomethingBigs.size(); i++)
	//	{
	//		cv::circle(img, P_Faults->SomethingBigs[i].position, P_Faults->SomethingBigs[i].diameter, cv::Scalar(255, 0, 0), 5);
	//	}
	//}
	if (P_Faults->Scratchs.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << P_Faults->Scratchs.size() << " 处划痕缺陷，绿色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->Scratchs.size(); i++)
		{
			cv::circle(img, P_Faults->Scratchs[i].position, P_Faults->Scratchs[i].length, cv::Scalar(0, 255, 0), 5);
		}
	}
	if (P_Faults->Holes.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << P_Faults->Holes.size() << " 处凹点缺陷，黄色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->Holes.size(); i++)
		{
			cv::circle(img, P_Faults->Holes[i].position, P_Faults->Holes[i].diameter, cv::Scalar(0, 255, 255), 5);
		}
	}
	if (P_Faults->MarkPens.size() > 0)
	{

		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << P_Faults->MarkPens.size() << " 处人工标记，橙色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->MarkPens.size(); i++)
		{
			cv::rectangle(img, P_Faults->MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
		}
	}
}