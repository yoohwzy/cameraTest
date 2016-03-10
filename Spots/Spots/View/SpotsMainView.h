#pragma once

#include <opencv2\opencv.hpp>
#include <string>
using namespace std;

class Controller;// ����໥�����

class SpotsMainView
{
public:
	SpotsMainView()
	{
		
	}
	~SpotsMainView()
	{

	}

	void AddController(Controller *c)
	{
		p_contrller = c;
	}
	// interface
	// ��ʾͼƬ��UI
	virtual void ShowBigImg(cv::Mat) {}
	// ��ʾͼƬ����¼ͼƬ
	virtual void ShowLogImg(cv::Mat) {}
	// ����UI�ϵļ�¼����
	virtual void UpdateStatistics() {}
	// �л����������ģʽ������true�л����������ģʽ��false�ص���ʵ���ģʽ
	virtual void SwitchModel2Virtual(bool switchToV) {}
protected:
	Controller* p_contrller;
};

