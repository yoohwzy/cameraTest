#pragma once

#include "../Controller/ControllerBase.h"

// MVC����ͼ�Ļ��࣬���е�View������
// ���ڽ�������������ͼ
// @author VShawn
class ViewBase
{
public:
	ViewBase();
	~ViewBase();
private:
	ControllerBase *c;
};

