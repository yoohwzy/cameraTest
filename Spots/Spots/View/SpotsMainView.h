#pragma once

#include <opencv2\opencv.hpp>
#include <string>
using namespace std;

class Controller;// 解决类互相包含

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
	// 显示图片到UI
	virtual void ShowBigImg(cv::Mat) {}
	// 更新UI上的记录数据
	virtual void UpdateRecord() {}
	// 切换到虚拟相机模式，传入true切换到虚拟相机模式，false回到真实相机模式
	virtual void SwitchModel2Virtual(bool switchToV) {}
	void SetText(int nID, string str);
protected:
	Controller* p_contrller;
};

