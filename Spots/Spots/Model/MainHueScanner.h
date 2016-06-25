#pragma once
#include <thread>

//�����㷨�ۺ���
class ControllerModel;// ����໥�����


#include <Class\Camera\MVCAM\MVCAM.h>
//��ɫ��������
//�����ж���������ȱ�ݣ�����������պ�����������ɫ����
//ʵ������ֱ�ӿ������������ͼ�߳�
class MainHueScanner
{
public:
	MainHueScanner(ControllerModel *pController);
	~MainHueScanner();
	int GetSN(){ return sn; };
	void Run(int SN){ sn = SN; };
	void Pause()
	{
		thread t = thread(std::mem_fn(&MainHueScanner::waitPause), this);
		t.detach();
	};
	void Stop(){ this->sn = 0; stopFlag = true; };
	bool HasInited = false;
	//���꣬�������׼HSV���洢
	static void DingBiao(cv::Mat);
	//���ñ�׼ֵ��������ʽ��string = "H1,S1,V1;H2,S2,V2;"��ʾ������
	static void SetStandardHSV(string str);
	static int Standard_H;
	static int Standard_S;
	static int Standard_V;


	static bool Enabled;
	static bool SAVE_IMG;
	static int WaitTimeMSIn;//�ȴ���ש������������ʱ��
	static int WaitTimeMSOut;//�ȴ���ש�뿪��������ʱ��
private:
	void waitPause()
	{
		if (MainHueScanner::WaitTimeMSOut > 0)
			Sleep(MainHueScanner::WaitTimeMSOut);
		this->sn = 0;
	}
	ControllerModel *p_Controller = NULL;
	int sn = 0;//ͼ�����кţ�Ϊ0ʱ��ͣ
	bool stopFlag = false;
	bool endFlag = true;//���̽�����־
	MVCAM mvcam;
	void scanImg();
	//�����Ƿ���ȱ�ݣ���ȱ�ݷ���0
	int analysis(cv::Mat);

	static inline bool SortBysize(vector<cv::Point>const &v1, vector<cv::Point>const &v2)	
	{
		return v1.size()>v2.size();
		//�������� 
	};
	//����ͼƬ ��ͼƬ���
	void imageSave(cv::Mat img, int _sn);
};

