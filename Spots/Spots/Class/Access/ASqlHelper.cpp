#include "ASqlHelper.h"

// 面向Access的SQLHelper
ASqlHelper::ASqlHelper()
{
}


ASqlHelper::~ASqlHelper()
{
}



string ASqlHelper::conStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=D://cameraTest//AccessCtrl//11.mdb;Persist Security Info=False";
const string ASqlHelper::NONE = "_NONE_QUERY_NO_RESULT";

_ConnectionPtr ASqlHelper::_pConnection = NULL;
_RecordsetPtr ASqlHelper::_pRecordset = NULL;

bool ASqlHelper::SetConStr(string constr)
{
	conStr = constr;
	_ConnectionPtr m_pConnection;
	if (OpenCnn(m_pConnection))
	{
		CloseCnn(m_pConnection);
		return true;
	}
	return false;
}
bool ASqlHelper::OpenCnn(_ConnectionPtr& m_pConnection)
{
	CloseCnn();
	CoInitialize(NULL);//CoInitialize是Windows提供的API函数，用来告诉 Windows以单线程的方式创建com对象
	m_pConnection.CreateInstance(__uuidof(Connection));
	try
	{
		if (m_pConnection->Open(ASqlHelper::conStr.c_str(), "", "", adModeUnknown) == S_OK)
		{
			return true;
		}
	}
	catch (_com_error e)
	{
		//cout << e.ErrorMessage();
	}
	return false;

	//if (m_pConnection->Open("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=D://cameraTest//AccessCtrl//11.mdb;Persist Security Info=False", "", "", adModeUnknown) == S_OK)
	//{
	//}
}
void ASqlHelper::CloseCnn()
{
	CloseCnn(ASqlHelper::_pConnection, ASqlHelper::_pRecordset);
}
void ASqlHelper::CloseCnn(_ConnectionPtr& m_pConnection)
{
	if (m_pConnection != NULL)
	{
		m_pConnection->Close();
		m_pConnection = NULL;
	}
}
void ASqlHelper::CloseCnn(_RecordsetPtr& m_pRecordset)
{
	if (m_pRecordset != NULL)
	{
		m_pRecordset->Close();
		m_pRecordset = NULL;
	}
}
void ASqlHelper::CloseCnn(_ConnectionPtr& m_pConnection, _RecordsetPtr& m_pRecordset)
{
	CloseCnn(m_pRecordset);
	CloseCnn(m_pConnection);
}



int ASqlHelper::ExecuteNonQuery(string sql)
{
	_ConnectionPtr m_pConnection;
	if (OpenCnn(m_pConnection))
	{
		try
		{
			_variant_t RecordsAffected;
			_bstr_t strCmd = sql.c_str();
			m_pConnection->Execute(strCmd, &RecordsAffected, adCmdText);
			CloseCnn(m_pConnection);
			return (int)RecordsAffected;
		}
		catch (_com_error &e)
		{
			//AfxMessageBox(e.Description());
		}
	}
	CloseCnn(m_pConnection);
	return 0;
}

string ASqlHelper::ExecuteScalar(string sql)
{
	_ConnectionPtr m_pConnection;
	if (OpenCnn(m_pConnection))
	{
		_RecordsetPtr m_pRecordset;
		m_pRecordset.CreateInstance("ADODB.Recordset");
		_variant_t RecordsAffected;
		m_pRecordset = m_pConnection->Execute(sql.c_str(), &RecordsAffected, adCmdText);
		if (m_pRecordset->BOF)
		{
			return ASqlHelper::NONE;
		}

		//取第一个字段的名称
		FieldPtr pField = m_pRecordset->GetFields()->GetItem(0L);
		//取得第一个字段的值
		_variant_t vValue = pField->GetValue();
		string ret = (const char*)_bstr_t(vValue);
		CloseCnn(m_pConnection, m_pRecordset);
		return ret;
	}
	CloseCnn(m_pConnection);
	return ASqlHelper::NONE;
}

_RecordsetPtr ASqlHelper::ExecuteRecordset(string sql)
{
	if (OpenCnn(ASqlHelper::_pConnection))
	{
		ASqlHelper::_pRecordset.CreateInstance("ADODB.Recordset");
		_variant_t RecordsAffected;
		ASqlHelper::_pRecordset = ASqlHelper::_pConnection->Execute(sql.c_str(), &RecordsAffected, adCmdText);
		if (!ASqlHelper::_pRecordset->BOF)
		{
			return ASqlHelper::_pRecordset;
		}
	}
	ASqlHelper::_pConnection = NULL;
	return NULL;
}
