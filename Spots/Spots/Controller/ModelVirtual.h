#pragma once

#include <opencv2\opencv.hpp>

class Controller;// 解决类互相包含

class ModelVirtual
{
public:
	ModelVirtual();
	~ModelVirtual();

	void AddController(Controller *c)
	{
		p_contrller = c;
	}
	// interface
protected:
	Controller* p_contrller;
};

