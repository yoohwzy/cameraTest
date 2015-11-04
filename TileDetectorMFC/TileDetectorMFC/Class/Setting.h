#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include <winnt.h>

using namespace std;
class Setting
{
public:
	enum Key
	{
		GRAB_MaxPics,
		GRAB_Width,
		GRAB_ColorType,
		TIGGER_WaitTime
	};



	Setting();
	~Setting();
	static void Init(Key key);

	static void AddKey(Key key, int value);
	static void AddKey(Key key, float value);
	static void AddKey(Key key, double value);
	static void AddKey(Key key, string value);
	static void AddKey(Key key, LPWSTR value);

	static bool GetKeyInt(Key key, int& value);
	static bool GetKeyFloat(Key key, float& value);
	static bool GetKeyDouble(Key key, double& value);
	static bool GetKeyString(Key key, string& value);
private:
	static string getSectionName(Key key);
	static string getKeyName(Key key);
	const static LPWSTR path;
};

