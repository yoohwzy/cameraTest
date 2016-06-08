#include "Worker.h"
#include <Controller/ControllerModel.h>
#include <Class/Debug/MFCConsole.h>

Worker::Worker(string WorkerInfo)
{
	workerInfo = WorkerInfo;
}


Worker::~Worker()
{
}

void Worker::StartWork()
{
	std::thread t_workThread(std::mem_fn(&Worker::work), this);
	t_workThread.detach();
	//work();
}
void Worker::work()
{
	MyStatus = WorkerStatus::InProcessing;
	cv::Mat grayImg = image;
	/********************************************/
	//开始图像处理
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//TODO:暂时用正规化处理图片
	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//开始图像处理
	Synthesizer s = Synthesizer(SN);
	s.BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
	s.BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;

	s.BlockEdgeLineDetector_Enable = BlockEdgeLineDetector_Enable;
	s.BlockEdgeLineDetector_BINARY_THRESHOD = BlockEdgeLineDetector_BINARY_THRESHOD;
	s.BlockEdgeLineDetector_LENGTH_THRESHOD = BlockEdgeLineDetector_LENGTH_THRESHOD;
	s.BlockEdgeLineDetector_DEEP_THRESHOD = BlockEdgeLineDetector_DEEP_THRESHOD;

	s.BlockEdgeDetector_Enable = BlockEdgeDetector_Enable;
	s.BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
	s.BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
	s.BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;


	s.Pretreatment_Enable = Pretreatment_Enable;

	Synthesizer::Status status = s.Run(grayImg);
	
	//cv::Mat tmp = grayImg.clone();
	if (status != Synthesizer::Status::NotFound)
	{
		//绘制缺陷图像
		drawFaults(grayImg, s.faults);
		cv::line(grayImg, cv::Point(0, s.p_block->UpLine.k * (0 - s.p_block->UpLine.x0) + s.p_block->UpLine.y0), cv::Point(grayImg.cols, s.p_block->UpLine.k * (grayImg.cols - s.p_block->UpLine.x0) + s.p_block->UpLine.y0), cv::Scalar(0, 0, 255), 1);
		cv::line(grayImg, cv::Point(0, s.p_block->DownLine.k * (0 - s.p_block->DownLine.x0) + s.p_block->DownLine.y0), cv::Point(grayImg.cols, s.p_block->DownLine.k * (grayImg.cols - s.p_block->DownLine.x0) + s.p_block->DownLine.y0), cv::Scalar(0, 255, 255), 1);
		cv::line(grayImg, cv::Point((grayImg.rows - s.p_block->LeftLine.y0) / s.p_block->LeftLine.k + s.p_block->LeftLine.x0, grayImg.rows), cv::Point((0 - s.p_block->LeftLine.y0) / s.p_block->LeftLine.k + s.p_block->LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
		cv::line(grayImg, cv::Point((grayImg.rows - s.p_block->RightLine.y0) / s.p_block->RightLine.k + s.p_block->RightLine.x0, grayImg.rows), cv::Point((0 - s.p_block->RightLine.y0) / s.p_block->RightLine.k + s.p_block->RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);


		int type = 1;//产品级别  1 A级 2 B级 3 C级 4 不合格
		//直接判断为不合格的情况，若有则type=4

		//产品分级
		if (s.faults.EdgeFaults.size() > 0)
		{
			type = 4;
		}
		if (type != 4)//崩边分级
		{
			double _EDGE_TOTAL_LENGTH = 0;//崩边总长度
			double _EDGE_TOTAL_DEEP = 0;//崩边总深度
			for (int i = 0; i < s.faults.BrokenEdges.size(); i++)
			{
				_EDGE_TOTAL_LENGTH += s.faults.BrokenEdges[i].length_mm;
				_EDGE_TOTAL_DEEP += s.faults.BrokenEdges[i].deep_mm;
				if (s.faults.BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_C || s.faults.BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_C)
				{
					type = 4;
					break;
				}
				if (type < 3 && (s.faults.BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_B || s.faults.BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_B))
				{
					type = 3;
				}

				if (type < 2 && (s.faults.BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_A || s.faults.BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_A))
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
			for (size_t i = 0; i < s.faults.Crazings.size(); i++)
			{

			}
		}
		if (1 == 1)
		{
			stringstream ss;
			ss << workerInfo << "type=" << type << endl;
			MFCConsole::Output(ss.str());
		}
		P_Controller->ImgProcessOverCallBack(SN, grayImg, *(s.p_block), type);
		//if (type > 1)
		//	P_Controller->imageSave(tmp);
	}
	else
	{
		P_Controller->ImgProcessOverCallBack(SN, grayImg, *(s.p_block), 0);
	}


	//显示结果
	MyStatus = WorkerStatus::Free;
	return;
}


void Worker::drawFaults(cv::Mat& img, Faults& faults)
{
	if (img.channels() == 1)
		cv::cvtColor(img, img, CV_GRAY2RGB);


	if (faults.EdgeFaults.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << faults.EdgeFaults.size() << " 处边缘不相似。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.EdgeFaults.size(); i++)
		{
			cv::rectangle(img, cv::Rect(faults.EdgeFaults[i].position.x - faults.EdgeFaults[i].width,
				faults.EdgeFaults[i].position.y - faults.EdgeFaults[i].height,
				faults.EdgeFaults[i].width * 2,
				faults.EdgeFaults[i].height * 2), Scalar(0, 30 * i, 250));
		}
	}
	if (faults.BrokenEdges.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << faults.BrokenEdges.size() << " 处崩边缺陷，红色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.BrokenEdges.size(); i++)
		{
			cv::circle(img, faults.BrokenEdges[i].position, faults.BrokenEdges[i].length, cv::Scalar(0, 0, 255), 4);
		}
	}
	//if (faults.BrokenCorners.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << workerInfo << SN << " 存在 " << faults.BrokenCorners.size() << " 处崩角缺陷，洋红标出。" << endl;
	//		MFCConsole::Output(ss.str());
	//	}

	//	for (size_t i = 0; i < faults.BrokenCorners.size(); i++)
	//	{
	//		cv::circle(img, faults.BrokenCorners[i].position, faults.BrokenCorners[i].length, cv::Scalar(127, 0, 228), 5);
	//	}
	//}
	//if (faults.SomethingBigs.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << SN << " 存在 " << faults.BrokenEdges.size() << " 处崩角缺陷，洋红标出。" << endl;
	//		MFCConsole::Output( << ss.str();
	//	}

	//	CString str;
	//	str.Format(_T("%d 存在%d处EID缺陷，蓝色标出。\r\n"), SN, faults.SomethingBigs.size());
	//	m_Info += str;
	//	clog += str;
	//	for (size_t i = 0; i < faults.SomethingBigs.size(); i++)
	//	{
	//		cv::circle(img, faults.SomethingBigs[i].position, faults.SomethingBigs[i].diameter, cv::Scalar(255, 0, 0), 5);
	//	}
	//}
	if (faults.Scratchs.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << faults.Scratchs.size() << " 处划痕缺陷，绿色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.Scratchs.size(); i++)
		{
			cv::circle(img, faults.Scratchs[i].position, faults.Scratchs[i].length, cv::Scalar(0, 255, 0), 5);
		}
	}
	if (faults.Holes.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << faults.Holes.size() << " 处凹点缺陷，黄色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.Holes.size(); i++)
		{
			cv::circle(img, faults.Holes[i].position, faults.Holes[i].diameter, cv::Scalar(0, 255, 255), 5);
		}
	}
	if (faults.MarkPens.size() > 0)
	{

		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " 存在 " << faults.MarkPens.size() << " 处人工标记，橙色标出。" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < faults.MarkPens.size(); i++)
		{
			cv::rectangle(img, faults.MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
		}
	}
}