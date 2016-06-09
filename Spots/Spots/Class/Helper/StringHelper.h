#pragma once

#ifndef AFX_DATA
//#	include <afx.h>
#	include <afxwin.h>
#endif
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
#include <vector>
class StringHelper
{
public:
	static inline string int2string(int i)
	{
		
		stringstream ss;
		ss << i;
		return ss.str();
	}
	static inline string double2string(double d)
	{
		stringstream ss;
		ss << d;
		return ss.str();
	}
	static inline CString int2CString(int i)
	{
		return string2CString(int2string(i));
	}
	static inline CString double2CString(double d)
	{
		return string2CString(double2string(d));
	}
	static inline string CString2string(CString cstr)
	{
		return LPWSTR2string(cstr.GetBuffer(0));
	}
	static inline CString string2CString(string str)
	{
		CString ret = L"";
		ret += str.c_str();
		return ret;
	}
	static inline int string2int(string str)
	{
		int value;
		stringstream ss;
		ss << str;
		ss >> value;
		return value;
	}
	static inline string LPWSTR2string(LPWSTR lpwstr)
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
	static inline LPWSTR string2LPWSTR(string str)
	{
		int dwLen = str.length() + 1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), dwLen, NULL, 0);//算出合适的长度
		LPWSTR lpszPath = new WCHAR[dwLen];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), dwLen, lpszPath, nwLen);
		return lpszPath;
	}
	//注意：当字符串为空时，也会返回一个空字符串  
	static inline void split(std::string& s, std::string& delim, vector<string>* ret)
	{
		size_t last = 0;
		size_t index = s.find_first_of(delim, last);
		while (index != std::string::npos)
		{
			ret->push_back(s.substr(last, index - last));
			last = index + 1;
			index = s.find_first_of(delim, last);
		}
		if (index - last>0)
		{
			ret->push_back(s.substr(last, index - last));
		}
	}
};