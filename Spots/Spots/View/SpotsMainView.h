#pragma once

#include <opencv2\opencv.hpp>
#include <string>
using namespace std;

class ControllerModel;// 解决类互相包含

class SpotsMainView
{
public:
	SpotsMainView()
	{
		
	}
	~SpotsMainView()
	{

	}

	void AddController(ControllerModel *c)
	{
		p_contrller = c;
	}
	// interface
	// 显示图片到UI
	virtual void ShowBigImg(cv::Mat) = 0;
	// 显示图片到记录图片
	virtual void ShowLogImg(cv::Mat) = 0;
	// 更新UI上的记录数据
	virtual void UpdateStatistics() = 0;
	// 更新尺寸记录
	virtual void UpdateSizeInfo(string info) = 0;
	// 切换到虚拟相机模式，传入true切换到虚拟相机模式，false回到真实相机模式
	virtual void SwitchModel2Virtual(bool switchToV) {}
protected:
	ControllerModel* p_contrller;
};

