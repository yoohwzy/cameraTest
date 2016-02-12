#pragma once
#include "stdafx.h"
#include <iostream>
#include <string.h>
#include <cstdio>
using namespace std;
//����ADO��̬���ӿ�,�����
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

class ASqlHelper
{
public:
	ASqlHelper();
	~ASqlHelper();

	//ִ��һ��SQL��䣬������Ӱ������
	static int ExecuteNonQuery(string sql);
	//���ص�һ�е�һ��
	static string ExecuteScalar(string sql);
	//�������ݼ������ݼ�����������CloseCnn()�ر����ݿ�����
	static _RecordsetPtr ExecuteRecordset(string sql);

	// ���ݿ������ַ���
	static string CnnStr;
	//���ر����ʾδ��ѯ�����
	static const string NONE;
	static void CloseCnn();
	static void CloseCnn(_ConnectionPtr& m_pConnection);
	static void CloseCnn(_RecordsetPtr& m_pRecordset);
	static void CloseCnn(_ConnectionPtr& m_pConnection, _RecordsetPtr& m_pRecordset);
private:
	static bool OpenCnn(_ConnectionPtr& m_pConnection);
	static _ConnectionPtr _pConnection;
	static _RecordsetPtr _pRecordset;
};

