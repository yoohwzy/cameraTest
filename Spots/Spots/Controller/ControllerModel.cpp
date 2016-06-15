#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerModel.h"
#include <Class/Setting/SettingHelper.h>
#include <Class/Helper/StringHelper.h>
#include <Class/Debug/MFCConsole.h>
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //�ļ�Ŀ¼lib ���û�����У������link����


#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeSimilarDetector.h>
#include <Algorithm/BlockEdgeLineDetector.h>
#include <Algorithm/EdgeFaultLineDetector.h>

//ͨ�õĳ�ʼ������
//��ʼ��PCI1761�����ݿ⡢���ݲ�����ϵͳ�л�����ʵ/�������ģʽ
//��ʵģʽ�µ���TiggerStartWatch�����������߳�
void ControllerModel::baseInit()
{
	stringstream ss;

	//��ʼ��PCI1761
	if (IsRealModel)
	{
		if (!pci1761.init())
		{
			pci1761Inited = false;
		}
		else
			pci1761Inited = true;
	}


	// ���ݿ����ó�ʼ��
	if (1 == 1)
	{
		int accEnable = 0;
		SettingHelper::GetKeyInt("DATABASE", "ACCDB_ENABLE", accEnable);//��ȡ�Ƿ��������ݿ�ģ��

		ss.clear();
		ss << "ACCDB_ENABLE = " << accEnable << endl;
		MFCConsole::Output(ss.str());
		if (accEnable != 0)
		{
			string db_path;
			bool accConnFlag = false;
			if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//��ȡ���ݿ�����·��
			{
				accConnFlag = Statistics::InitDate(db_path);
				MFCConsole::Output("db_path:");
				MFCConsole::Output(db_path);
				MFCConsole::Output("\r\n");
			}
			else
			{
				accConnFlag = Statistics::InitDate("src//..//��שȱ�ݼ�����ݿ�.mdb");
				SettingHelper::AddKey("DATABASE", "ACCDB_PATH", "src//..//��שȱ�ݼ�����ݿ�.mdb");
				MFCConsole::Output("db_path:src//..//��שȱ�ݼ�����ݿ�.mdb\r\n");
			}
			// ͳ�����ݳ�ʼ��
			if (accConnFlag)
				spotsMainView->UpdateStatistics();
			else
				AfxMessageBox(L"�޷����ӵ����ݿ⣡");
		}
	}
	// ��ʼ�������߳�
	if (e2vInited && pci1761Inited)
	{
		//��Ϊ��ʵ���ģʽ����ʵ�����������ˣ����������������ӽ���

		//��ʼ������
		worker1 = new Worker("worker1:");
		worker2 = new Worker("worker2:");

		worker1->P_Controller = this;
		worker2->P_Controller = this;
		TiggerStartWatch();

		spotsMainView->SwitchModel2Virtual(false);

		MFCConsole::Output("���л�����ʵ���ģʽ��\r\n");
	}
	else
	{
		PauseFlag = true;
		if (IsRealModel)
		{
			IsRealModel = 0;
			if (!e2vInited && !pci1761Inited)
				AfxMessageBox(L"�������&pci1761��ʼ��ʧ�ܣ����л����������ģʽ��");
			else if (!pci1761Inited)
				AfxMessageBox(L"pci1761��ʼ��ʧ�ܣ����л����������ģʽ��");
			else if (!e2vInited)
				AfxMessageBox(L"���������ʼ��ʧ�ܣ����л����������ģʽ��");
		}
		else
			MFCConsole::Output("���л����������ģʽ��\r\n");
		//��Ϊ�������ģʽ����ʵ���������������1������
		//�����������
		cv::Mat virtualImg;
		worker1 = new Worker("Virtual Worker");
		worker1->image = virtualImg;
		worker1->P_Controller = this;
		spotsMainView->SwitchModel2Virtual(true);
	}

	//��ȡ��������
	//���ù����㷨����
	ResetParameter();


	if (pci1761Inited)
	{
		//��ʼ���������
		if (MainHueScanner::Enabled)
		{
			P_mainHueScanner = new MainHueScanner(this);
			if (!P_mainHueScanner->HasInited)
			{
				delete P_mainHueScanner;
				P_mainHueScanner = NULL;
				AfxMessageBox(L"δ�ܳ�ʼ�������������ģ�鱻���ã�");
			}
		}
	}



	//��ʼ��UI
	cv::Mat white(2, 2, CV_8U, cv::Scalar(255));
	logImg.InitDrawingBoard();
	spotsMainView->ShowBigImg(white);
	spotsMainView->ShowLogImg(logImg.DrawingBoard);
}



/*********************  �������ģʽ����  ***********************/

void ControllerModel::VirtualSelectImg(cv::Mat image)
{
	if (image.cols == 0)
		MessageBox(0, L"ͼƬ��ȡʧ�ܣ�", L"����", 0);
	else
	{
		worker1->image = image;
	}
}
void ControllerModel::VirtualWorkerStart()
{
	if (worker1->image.cols == 0)
	{
		MessageBox(0, L"���ȼ��������Ƭ��", L"����", 0);
		return;
	}
	if (worker1->MyStatus == Worker::Free)
	{
		worker1->SN = Statistics::TodayAll + 1;
		worker1->StartWork();
	}
	else
	{
		MessageBox(0, L"��һ�ִ�����δ������", L"����", 0);
	}
}
/*********************�������ģʽ��������***********************/


void ControllerModel::ResetParameter()
{
#pragma region ��INI�ļ��ж�ȡ���ò���

	int si = 0;
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "SAVE_IMG", si);
	this->SAVE_IMG = si;

	//�ߴ����
	SettingHelper::GetKeyInt("SYS_STANDARD", "Standard_Width_mm", Block::Standard_Width_mm);
	SettingHelper::GetKeyInt("SYS_STANDARD", "Standard_Length_mm", Block::Standard_Length_mm);
	SettingHelper::GetKeyDouble("SYS_STANDARD", "X_mmPerPix", Block::X_mmPerPix);
	SettingHelper::GetKeyDouble("SYS_STANDARD", "Y_mmPerPix", Block::Y_mmPerPix);

	//��������ͼ���
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "WaitTimeMSIn", this->Capture_WaitTimeMSIn);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "WaitTimeMSOut", this->Capture_WaitTimeMSOut);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "FrameTimeOut", this->Capture_FrameTimeOut);

	/*************��Եȱ�ݼ�����***********/
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", BlockLocalizer::THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", BlockLocalizer::ContinuePointCount);

	int _BlockEdgeSimilarDetector_Enable = 1;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeSimilarDetector_Enable", _BlockEdgeSimilarDetector_Enable);
	BlockEdgeSimilarDetector::Enabled = _BlockEdgeSimilarDetector_Enable;
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeSimilarDetector_DIFF_THRESHOLD", BlockEdgeSimilarDetector::DIFF_THRESHOLD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeSimilarDetector_FAULTS_SPAN", BlockEdgeSimilarDetector::FAULTS_SPAN);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeSimilarDetector_FAULTS_COUNT", BlockEdgeSimilarDetector::FAULTS_COUNT);

	int _BlockEdgeLineDetector_Enabled = 1;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_Enable", _BlockEdgeLineDetector_Enabled);
	BlockEdgeLineDetector::Enabled = _BlockEdgeLineDetector_Enabled;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_BINARY_THRESHOD", BlockEdgeLineDetector::BINARY_THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_LENGTH_THRESHOD", BlockEdgeLineDetector::LENGTH_THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_DEEP_THRESHOD", BlockEdgeLineDetector::DEEP_THRESHOD);

	/**************�ڲ�����****************/
	int _Pretreatment_Enable = 1;
	SettingHelper::GetKeyInt("SURFACE", "Pretreatment_Enable", _Pretreatment_Enable);
	this->Pretreatment_Enable = _Pretreatment_Enable;


	/**********************************/
	/*                                */
	/*            �������            */
	/*                                */
	/**********************************/
	int areatmpInt = 0;
	if (SettingHelper::GetKeyInt("AreaCam", "Enable", areatmpInt))
		MainHueScanner::Enabled = areatmpInt;
	areatmpInt = 0;
	if (SettingHelper::GetKeyInt("AreaCam", "SAVE_IMG", areatmpInt))
		MainHueScanner::SAVE_IMG = areatmpInt;
	string para = "para";
	if (SettingHelper::GetKeyString("AreaCam", "MainHSVs", para))
		MainHueScanner::SetStandardHSV(para);
	SettingHelper::GetKeyInt("AreaCam", "WaitTimeMSIn", MainHueScanner::WaitTimeMSIn);
	SettingHelper::GetKeyInt("AreaCam", "WaitTimeMSOut", MainHueScanner::WaitTimeMSOut);



	/**********************************/
	/*                                */
	/*            �ּ�����            */
	/*                                */
	/**********************************/
	//ȱ�� ��
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_A", this->Classify_EDGE_SINGLE_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_B", this->Classify_EDGE_SINGLE_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_C", this->Classify_EDGE_SINGLE_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_ACCEPT", this->Classify_EDGE_SINGLE_LENGTH_ACCEPT);
	//�ܳ�
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_A", this->Classify_EDGE_TOTAL_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_B", this->Classify_EDGE_TOTAL_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_C", this->Classify_EDGE_TOTAL_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_ACCEPT", this->Classify_EDGE_TOTAL_LENGTH_ACCEPT);
	//���
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_A", this->Classify_EDGE_SINGLE_DEEP_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_B", this->Classify_EDGE_SINGLE_DEEP_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_C", this->Classify_EDGE_SINGLE_DEEP_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_ACCEPT", this->Classify_EDGE_SINGLE_DEEP_ACCEPT);
	//�����
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_A", this->Classify_EDGE_TOTAL_DEEP_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_B", this->Classify_EDGE_TOTAL_DEEP_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_C", this->Classify_EDGE_TOTAL_DEEP_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_ACCEPT", this->Classify_EDGE_TOTAL_DEEP_ACCEPT);
	//��͹
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_A", this->Classify_HOAL_DIAMETER_A);
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_B", this->Classify_HOAL_DIAMETER_B);
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_C", this->Classify_HOAL_DIAMETER_C);
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_ACCEPT", this->Classify_HOAL_DIAMETER_ACCEPT);

	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_A", this->Classify_HOAL_COUNT_A);
	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_B", this->Classify_HOAL_COUNT_B);
	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_C", this->Classify_HOAL_COUNT_C);
	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_ACCEPT", this->Classify_HOAL_COUNT_ACCEPT);

	//���� ��
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_A", this->Classify_SCRATCH_SINGLE_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_B", this->Classify_SCRATCH_SINGLE_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_C", this->Classify_SCRATCH_SINGLE_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_ACCEPT", this->Classify_SCRATCH_SINGLE_LENGTH_ACCEPT);
	//�ܳ�
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_A", this->Classify_SCRATCH_TOTAL_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_B", this->Classify_SCRATCH_TOTAL_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_C", this->Classify_SCRATCH_TOTAL_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_ACCEPT", this->Classify_SCRATCH_TOTAL_LENGTH_ACCEPT);

#pragma endregion
#pragma region ���ò���Ӧ����worker
	for (int i = 0; i < 2; i++)
	{
		Worker *w = NULL;
		if (i == 0)
			w = worker1;
		else
			w = worker2;
		if (w != NULL)
		{
			w->Pretreatment_Enable = Pretreatment_Enable;


			/**************�ּ�����*************/

			//�߲���
			w->Classify_EDGE_SINGLE_LENGTH_A = Classify_EDGE_SINGLE_LENGTH_A;
			w->Classify_EDGE_SINGLE_LENGTH_B = Classify_EDGE_SINGLE_LENGTH_B;
			w->Classify_EDGE_SINGLE_LENGTH_C = Classify_EDGE_SINGLE_LENGTH_C;
			w->Classify_EDGE_SINGLE_LENGTH_ACCEPT = Classify_EDGE_SINGLE_LENGTH_ACCEPT;

			w->Classify_EDGE_TOTAL_LENGTH_A = Classify_EDGE_TOTAL_LENGTH_A;
			w->Classify_EDGE_TOTAL_LENGTH_B = Classify_EDGE_TOTAL_LENGTH_B;
			w->Classify_EDGE_TOTAL_LENGTH_C = Classify_EDGE_TOTAL_LENGTH_C;
			w->Classify_EDGE_TOTAL_LENGTH_ACCEPT = Classify_EDGE_TOTAL_LENGTH_ACCEPT;

			w->Classify_EDGE_SINGLE_DEEP_A = Classify_EDGE_SINGLE_DEEP_A;
			w->Classify_EDGE_SINGLE_DEEP_B = Classify_EDGE_SINGLE_DEEP_B;
			w->Classify_EDGE_SINGLE_DEEP_C = Classify_EDGE_SINGLE_DEEP_C;
			w->Classify_EDGE_SINGLE_DEEP_ACCEPT = Classify_EDGE_SINGLE_DEEP_ACCEPT;

			w->Classify_EDGE_TOTAL_DEEP_A = Classify_EDGE_TOTAL_DEEP_A;
			w->Classify_EDGE_TOTAL_DEEP_B = Classify_EDGE_TOTAL_DEEP_B;
			w->Classify_EDGE_TOTAL_DEEP_C = Classify_EDGE_TOTAL_DEEP_C;
			w->Classify_EDGE_TOTAL_DEEP_ACCEPT = Classify_EDGE_TOTAL_DEEP_ACCEPT;

			//���Ӳ���
			w->Classify_HOAL_DIAMETER_A = Classify_HOAL_DIAMETER_A;
			w->Classify_HOAL_DIAMETER_B = Classify_HOAL_DIAMETER_B;
			w->Classify_HOAL_DIAMETER_C = Classify_HOAL_DIAMETER_C;
			w->Classify_HOAL_DIAMETER_ACCEPT = Classify_HOAL_DIAMETER_ACCEPT;

			w->Classify_HOAL_COUNT_A = Classify_HOAL_COUNT_A;
			w->Classify_HOAL_COUNT_B = Classify_HOAL_COUNT_B;
			w->Classify_HOAL_COUNT_C = Classify_HOAL_COUNT_C;
			w->Classify_HOAL_COUNT_ACCEPT = Classify_HOAL_COUNT_ACCEPT;


			//����
			w->Classify_SCRATCH_SINGLE_LENGTH_A = Classify_SCRATCH_SINGLE_LENGTH_A;
			w->Classify_SCRATCH_SINGLE_LENGTH_B = Classify_SCRATCH_SINGLE_LENGTH_B;
			w->Classify_SCRATCH_SINGLE_LENGTH_C = Classify_SCRATCH_SINGLE_LENGTH_C;
			w->Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = Classify_SCRATCH_SINGLE_LENGTH_ACCEPT;

			w->Classify_SCRATCH_TOTAL_LENGTH_A = Classify_SCRATCH_TOTAL_LENGTH_A;
			w->Classify_SCRATCH_TOTAL_LENGTH_B = Classify_SCRATCH_TOTAL_LENGTH_B;
			w->Classify_SCRATCH_TOTAL_LENGTH_C = Classify_SCRATCH_TOTAL_LENGTH_C;
			w->Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = Classify_SCRATCH_TOTAL_LENGTH_ACCEPT;
		}
		w = NULL;
	}
#pragma endregion
}




//private:
void ControllerModel::imageSave(cv::Mat img)
{
	image_write_lock.lock();

	cv::Mat img2 = img.clone();
	//�ж��ļ����Ƿ����
	CString folderMonthName;
	CString folderDayName;
	CString imageName;
	CTime t = CTime::GetCurrentTime();

	folderMonthName = L"Images\\" + t.Format("%Y_%m");
	folderDayName = folderMonthName + "\\" + t.Format("%d");
	imageName = folderDayName + "\\" + t.Format("%Y_%m_%d__%H_%M_%S");
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString millsec;
	millsec.Format(L"%d", (int)st.wMilliseconds);
	imageName += (L"_" + millsec + L".jpg");

	if (!PathIsDirectory(L"Images"))
	{
		CreateDirectory(L"Images", NULL);
	}
	if (!PathIsDirectory(folderMonthName))
	{
		CreateDirectory(folderMonthName, NULL);
	}
	if (!PathIsDirectory(folderDayName))
	{
		CreateDirectory(folderDayName, NULL);
	}

	stringstream ss;
	cv::imwrite(StringHelper::CString2string(imageName), img2);

	image_write_lock.unlock();
}