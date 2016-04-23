#include "Synthesizer.h"
#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeDetector.h>
#include <Algorithm/BlockEdgeLineDetector.h>
#include <Class\Debug\MFCConsole.h>

Synthesizer::Synthesizer(int _SN)
{
	SN = _SN;
}


Synthesizer::~Synthesizer()
{
	//delete p_block;
}

//�����㷨������״̬
Synthesizer::Status Synthesizer::Run(cv::Mat TileImg)
{
	p_block = new Block(TileImg.size().width, TileImg.size().height);

	////��ȡ��ֵ��ͼ��
	if (TileImg.cols == 0)
	{
		MFCConsole::Output("Img Empty!\r\n");
		return Status::NoImage;
	}
	cv::Mat grayImg = TileImg;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//��λ
	int status = positioning(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return status == _Status::_NotFound ? Status::NotFound : Status::Rejected; 
	}


	// ��Եȱ��
	status = detectEdge(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return Status::Rejected;
	}

	//����ȱ��
	//detectInner(grayImg);
	return Status::TypeA;
}


//��ש��λ�������Ƿ��ҵ���ש
Synthesizer::_Status Synthesizer::positioning(cv::Mat grayImg)
{
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//���ж�λ
	BlockLocalizer bl = BlockLocalizer(grayImg, p_block, &faults);
	bl.THRESHOD = BlockLocalizer_THRESHOD;
	bl.ContinuePointCount = BlockLocalizer_ContinuePointCount;
	bl.Run();


	if (MFCConsole::IsOpened)
	{
		stringstream ss;
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		ss << "��λ�㷨BlockLocalizer��" << t << endl;
		MFCConsole::Output(ss.str());
	}

	if (bl.NotFoundBlockFlag == true)
	{
		//δ��⵽��ש
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
//��Եȱ�ݼ��
Synthesizer::_Status Synthesizer::detectEdge(cv::Mat grayImg)
{
	double t = 0;
	p_block->Lines2ABCD();
	if (BlockEdgeDetector_Enable != 0)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeDetector bed = BlockEdgeDetector(grayImg, p_block, &faults);
		bed.DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		bed.FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		bed.FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;
		bed.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "�㷨BlockEdgeDetector��" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	if (BlockEdgeLineDetector_Enable != 0)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeLineDetector  beld = BlockEdgeLineDetector(grayImg, p_block, &faults);
		beld.DEEP_THRESHOD = 5;
		beld.THRESHOD = 5;
		beld.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "�㷨BlockEdgeLineDetector��" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return _Status::_NEXT;
}
//�ڲ�ȱ�ݼ��
Synthesizer::_Status Synthesizer::detectInner(cv::Mat grayImg)
{
	stringstream ss;
	double t = (double)cv::getTickCount();

	//��ש�ڲ�ȱ�ݼ��
	//ss << "��ש�ڲ�ȱ�ݼ�� ��ʼ" << endl;
	//Pretreatment p;
	//p.pretreatment(grayImg, p_block, &faults);

	if (MFCConsole::IsOpened)
	{
		ss << SN << " " << "�ڲ��л��ۣ�" << faults.Scratchs.size() << endl;
		ss << SN << " " << "�ڲ��а��㣺" << faults.Holes.size() << endl;
		ss << "��ש�ڲ�ȱ�ݼ�� ����" << endl;
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		ss << SN << " " << "�ڲ�ȱ�ݼ�⣺" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		MFCConsole::Output(ss.str());
	}
	return _Status::_NEXT;
}