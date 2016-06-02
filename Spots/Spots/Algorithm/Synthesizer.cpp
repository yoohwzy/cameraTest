#include "Synthesizer.h"
#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeSimilarDetector.h>
#include <Algorithm/BlockEdgeLineDetector.h>
#include <Algorithm/EdgeFaultLineDetector.h>
#include <Class\Debug\MFCConsole.h>
#include "Measurer.h"
Synthesizer::Synthesizer(int _SN)
{
	SN = _SN;
}


Synthesizer::~Synthesizer()
{
	//delete p_block;
}

//运行算法，返回状态
Synthesizer::Status Synthesizer::Run(cv::Mat TileImg)
{
	p_block = new Block(TileImg.size().width, TileImg.size().height);

	if (TileImg.cols == 0)
	{
		MFCConsole::Output("Img Empty!\r\n");
		return Status::NoImage;
	}
	cv::Mat grayImg = TileImg;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//定位
	int status = positioning(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return status == _Status::_NotFound ? Status::NotFound : Status::Rejected; 
	}

	//若没有标定数据，则粗略计算以获得缺陷尺寸
	if (Block::X_mmPerPix < 0)
	{
		Block::X_mmPerPix = Block::Standard_Width_mm / (double)abs(p_block->B.x - p_block->A.x);
		Block::Y_mmPerPix = Block::Standard_Length_mm / (double)abs(p_block->C.y - p_block->B.y);
	}

	Measurer m;
	m.CalculateDuiJiaoXian(*p_block);

	//EdgeFaultLineDetector efld = EdgeFaultLineDetector(grayImg, p_block, &faults);
	//efld.Run();
	// 边缘缺陷
	status = detectEdge(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return Status::Rejected;
	}

	//表面缺陷
	detectInner(grayImg);
	return Status::TypeA;
}


//瓷砖定位，返回是否找到瓷砖
Synthesizer::_Status Synthesizer::positioning(cv::Mat grayImg)
{
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//进行定位
	BlockLocalizer bl = BlockLocalizer(grayImg, p_block, &faults);
	bl.THRESHOD = BlockLocalizer_THRESHOD;
	bl.ContinuePointCount = BlockLocalizer_ContinuePointCount;
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
		return _Status::_NotFound;
	}
	if (bl.BrokenEdgeFlag == true)
	{
		//cv::cvtColor(img, img, CV_GRAY2BGR);
		//for (size_t i = 0; i < faults.BrokenEdges.size(); i++)
		//{
		//	cv::circle(img, faults.BrokenEdges[i].position, faults.BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
		//}
		//cv::namedWindow("1", 0);
		//cv::imshow("1", img);
		//cv::waitKey(0);
		return _Status::_Edge_Broken;
	}
	return _Status::_NEXT;
}
//边缘缺陷检测
Synthesizer::_Status Synthesizer::detectEdge(cv::Mat grayImg)
{
	double t = 0;
	p_block->Lines2ABCD();

	if (BlockEdgeLineDetector_Enable != 0)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeLineDetector  beld = BlockEdgeLineDetector(grayImg, p_block, &faults);
		beld.BINARY_THRESHOD = BlockEdgeLineDetector_BINARY_THRESHOD;
		beld.DEEP_THRESHOD = BlockEdgeLineDetector_DEEP_THRESHOD;
		beld.LENGTH_THRESHOD = BlockEdgeLineDetector_LENGTH_THRESHOD;
		beld.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "算法BlockEdgeLineDetector用时：" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	if (BlockEdgeDetector_Enable != 0)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeSimilarDetector besd = BlockEdgeSimilarDetector(grayImg, p_block, &faults);
		besd.DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		besd.FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		besd.FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;
		besd.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "算法BlockEdgeSimilarDetector用时：" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return _Status::_NEXT;
}
//内部缺陷检测
Synthesizer::_Status Synthesizer::detectInner(cv::Mat grayImg)
{
	if (Pretreatment_Enable)
	{
		stringstream ss;
		double t = (double)cv::getTickCount();

		//瓷砖内部缺陷检测
		ss << "瓷砖内部缺陷检测 开始" << endl;
		Pretreatment p;
		p.pretreatment(grayImg, p_block, &faults);

		if (MFCConsole::IsOpened)
		{
			ss << SN << " " << "内部有划痕：" << faults.Scratchs.size() << endl;
			ss << SN << " " << "内部有凹点：" << faults.Holes.size() << endl;
			ss << "瓷砖内部缺陷检测 结束" << endl;
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			ss << SN << " " << "内部缺陷检测：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return _Status::_NEXT;
}