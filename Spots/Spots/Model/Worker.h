#pragma once
#include <thread>

//�����㷨�ۺ���
#include <Class\Debug\MFCConsole.h>

#include <Algorithm/Base/Block.h>
#include <Algorithm/Base/Faults.h>
#include <Algorithm/Pretreatment.h>//һ��Ҫ������ �ŵ�cpp�к��opencv gpu.hpp����

//#define WORKER_DEBUG

class ControllerModel;// ����໥�����


//������
//������������ɼ�ͼ��󣬽����㷨����ĵ���
//�Լ���������ɺ�Ļ�ͼ
class Worker
{
public:
	enum Status
	{
		SizeError = -1,//�Խ��߳���
		NoImage = 0,//��ͼ
		NotFound = 1,//δ�ҵ���ש
		TypeA = 2,//A��
		TypeB = 3,//B��
		TypeC = 4,//C��
		Rejected = 5,//���ϸ�
	};

	//��ʼ��ʱ������ָ��ΪNULL�����ʾʹ�����������
	Worker(string WorkerInfo = "");
	~Worker();

	enum WorkerStatus{
		Free,
		InProcessing,
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;

	ControllerModel *P_Controller = NULL;
	Faults *P_Faults = NULL;
	Block *P_Block = NULL;
	cv::Mat image;

	// ��ש���к�
	int SN = 0;

	/**************�ڲ�����****************/
	int Pretreatment_Enable = 1;

	/**************�ּ�����*************/

	//�߲���
	double Classify_EDGE_SINGLE_LENGTH_A = 0;
	double Classify_EDGE_SINGLE_LENGTH_B = 0;
	double Classify_EDGE_SINGLE_LENGTH_C = 0;
	double Classify_EDGE_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_EDGE_TOTAL_LENGTH_A = 0;
	double Classify_EDGE_TOTAL_LENGTH_B = 0;
	double Classify_EDGE_TOTAL_LENGTH_C = 0;
	double Classify_EDGE_TOTAL_LENGTH_ACCEPT = 0;

	double Classify_EDGE_SINGLE_DEEP_A = 0;
	double Classify_EDGE_SINGLE_DEEP_B = 0;
	double Classify_EDGE_SINGLE_DEEP_C = 0;
	double Classify_EDGE_SINGLE_DEEP_ACCEPT = 0;

	double Classify_EDGE_TOTAL_DEEP_A = 0;
	double Classify_EDGE_TOTAL_DEEP_B = 0;
	double Classify_EDGE_TOTAL_DEEP_C = 0;
	double Classify_EDGE_TOTAL_DEEP_ACCEPT = 0;

	//���Ӳ���
	double Classify_HOAL_DIAMETER_A = 0;
	double Classify_HOAL_DIAMETER_B = 0;
	double Classify_HOAL_DIAMETER_C = 0;
	double Classify_HOAL_DIAMETER_ACCEPT = 0;

	double Classify_HOAL_COUNT_A = 0;
	double Classify_HOAL_COUNT_B = 0;
	double Classify_HOAL_COUNT_C = 0;
	double Classify_HOAL_COUNT_ACCEPT = 0;


	//����
	double Classify_SCRATCH_SINGLE_LENGTH_A = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_B = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_C = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_SCRATCH_TOTAL_LENGTH_A = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_B = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_C = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = 0;


private:
	string workerInfo = "";
	//double axis_x_mmPerPix = 1;//x�᷽����ÿ���ش�����ٺ���
	//double axis_y_mmPerPix = 1;//y�᷽����ÿ���ش�����ٺ���

	// ֱ�Ӷ�ȡN��ͼƬ
	void work();
	// ��ѭ��������ȡ��ͼƬ
	// ��ʼ�ɼ����У��ɼ�������Ϊ0ʱ�ȴ��½��ػ�ɼ���������frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// ����ȱ��
	void drawFaults(cv::Mat&);
	Status algorithmSynthesize(cv::Mat);
	//��ש��λ�������Ƿ��ҵ���ש
	//1 �ҵ�
	//0 δ�ҵ�
	//-1 ��ȱ��
	int positioning(cv::Mat grayImg);
	//��Եȱ�ݼ��
	//1 �ҵ�
	//-1 ��ȱ��
	int detectEdge(cv::Mat grayImg);
	//�ڲ�ȱ�ݼ��
	//1 �ҵ�
	//-1 ��ȱ��
	int detectInner(cv::Mat grayImg);
};