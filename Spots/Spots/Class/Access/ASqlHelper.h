#pragma once
#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <cstdio>
using namespace std;
//导入ADO动态链接库,必须的
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

class ASqlHelper
{
public:
	ASqlHelper();
	~ASqlHelper();

	//执行一条SQL语句，返回是否成功
	static bool ExecuteNonQuery(string sql);
	//返回第一行第一列
	static string ExecuteScalar(string sql);
	//返回数据集
	static _RecordsetPtr ExecuteRecordset(string sql);

	// 数据库连接字符串
	static string CnnStr;
	//返回本项表示未查询到结果
	static const string NONE;
	static void CloseCnn(_ConnectionPtr& m_pConnection);
	static void CloseCnn(_ConnectionPtr& m_pConnection, _RecordsetPtr& m_pRecordset);
private:
	static bool OpenCnn(_ConnectionPtr& m_pConnection);
};

