#pragma once

#include "../Controller/ControllerBase.h"

// MVC中视图的基类，所有的View都本类
// 用于将控制器绑定至视图
// @author VShawn
class ViewBase
{
public:
	ViewBase();
	~ViewBase();
private:
	ControllerBase *c;
};

