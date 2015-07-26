#pragma once

#include "../globle.h"
#include "BufferStorage.h";
class BlocksDetector
{
public:
	BlocksDetector();
	~BlocksDetector();
	static int GetEdgeRight(int line, int x, BufferStorage& s);
	static int GetEdgeLeft(int line, int x, BufferStorage& s);
};

