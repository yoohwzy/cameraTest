#pragma once

#include <afx.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class StringHelper
{
public:
	static string int2string(int i)
	{
		
		stringstream ss;
		ss << i;
		return ss.str();
	}
	static CString Int2CString(int i)
	{
		return string2CString(int2string(i));
	}
	static string CString2string(CString cstr)
	{
		return LPWSTR2string(cstr.GetBuffer(0));
	}
	static CString string2CString(string str)
	{
		CString ret = L"";
		ret += str.c_str();
		return ret;
	}
	static string LPWSTR2string(LPWSTR lpwstr)
	{
		wstring ws(lpwstr);
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
		return result;
	}
	static LPWSTR string2LPWSTR(string str)
	{
		int dwLen = str.length() + 1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), dwLen, NULL, 0);//算出合适的长度
		LPWSTR lpszPath = new WCHAR[dwLen];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), dwLen, lpszPath, nwLen);
		return lpszPath;
	}
};