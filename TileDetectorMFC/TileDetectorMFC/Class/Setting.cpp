#include "Setting.h"


Setting::Setting()
{
}


Setting::~Setting()
{
}
//void Setting::Init(Key key)
//{
//
//	switch (key)
//	{
//	case Setting::GRAB_MaxPics:
//		break;
//	case Setting::GRAB_Width:
//		break;
//	case Setting::GRAB_ColorType:
//		break;
//	case Setting::TIGGER_WaitTime:
//		break;
//	default:
//		break;
//	}
//}


void Setting::AddKey(Key key, int value)
{
	std::stringstream ss;
	ss << value;
	AddKey(key, ss.str());
}
void Setting::AddKey(Key key, float value)
{
	std::stringstream ss;
	ss << value;
	AddKey(key, ss.str());
}
void Setting::AddKey(Key key, double value)
{
	std::stringstream ss;
	ss << value;
	AddKey(key, ss.str());
}
void Setting::AddKey(Key key, LPWSTR value)
{
	wstring ws(value);
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());

	AddKey(key, result);
}
void Setting::AddKey(Key key, string value)
{
	string strkey = getKeyName(key);
	string strsection = getSectionName(key);


	size_t size = strsection.length();
	wchar_t *sectionbuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strsection.c_str(), size, sectionbuffer, size * sizeof(wchar_t));
	sectionbuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = strkey.length();
	wchar_t *keybuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strkey.c_str(), size, keybuffer, size * sizeof(wchar_t));
	keybuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = value.length();
	wchar_t *valuebuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, value.c_str(), size, valuebuffer, size * sizeof(wchar_t));
	valuebuffer[size] = 0;  // 确保以 '\0' 结尾 

	::WritePrivateProfileStringW(sectionbuffer, keybuffer, valuebuffer, Setting::path);

	delete valuebuffer;
	delete keybuffer;
	delete sectionbuffer;
}
bool Setting::GetKeyInt(Key key, int& value)
{
	string str;
	bool flag = GetKeyString(key, str);
	if (!flag)
		return false;
	stringstream ss;
	ss << str;
	ss >> value;
	//value = atoi(str.c_str());
	return true;
}
bool Setting::GetKeyFloat(Key key, float& value)
{
	string str;
	bool flag = GetKeyString(key, str);
	if (!flag)
		return false;
	stringstream ss;
	ss << str;
	ss >> value;
	//value = atof(str.c_str());
	return true;
}

bool Setting::GetKeyDouble(Key key, double& value)
{
	string str;
	bool flag = GetKeyString(key, str);
	if (!flag)
		return false;

	stringstream ss;
	ss << str;
	ss >> value;
	//value = atof(str.c_str());
	return true;
}
bool Setting::GetKeyString(Key key, string& value)
{
	string strkey = getKeyName(key);
	string strsection = getSectionName(key);

	LPWSTR lwpstr;

	size_t size = strsection.length();
	wchar_t *sectionbuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strsection.c_str(), size, sectionbuffer, size * sizeof(wchar_t));
	sectionbuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = strkey.length();
	wchar_t *keybuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strkey.c_str(), size, keybuffer, size * sizeof(wchar_t));
	keybuffer[size] = 0;  // 确保以 '\0' 结尾 

	char tmp[128];
	LPSTR lpstr = tmp;
	lwpstr = (LPWSTR)lpstr;
	::GetPrivateProfileString(sectionbuffer, keybuffer, NULL, lwpstr, 128, Setting::path);

	wstring ws(lwpstr);
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	value = result;
	if (value == "")
		return false;
	return true;
}




string Setting::getSectionName(Key key)
{
	//return "GRAB";
	//return "TIGGER";
	switch (key)
	{
	case Setting::GRAB_MaxPics:
		return "GRAB";
	case Setting::GRAB_Width:
		return "GRAB";
	case Setting::GRAB_ColorType:
		return "GRAB";
	case Setting::TIGGER_WaitTime:
		return "TIGGER";
	default:
		break;
	}
	return "";
}
string Setting::getKeyName(Key key)
{
	switch (key)
	{
	case Setting::GRAB_MaxPics:
		return "GRAB_MaxPics";
	case Setting::GRAB_Width:
		return "GRAB_Width";
	case Setting::GRAB_ColorType:
		return "GRAB_ColorType";
	case Setting::TIGGER_WaitTime:
		return "TIGGER_WaitTime";
	default:
		break;
	}
	return "";
}

const LPWSTR Setting::path = _T("src/../setting.ini");