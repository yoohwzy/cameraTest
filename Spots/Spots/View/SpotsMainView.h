#pragma once

#include <opencv2\opencv.hpp>

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
protected:
	Controller* p_contrller;
};

