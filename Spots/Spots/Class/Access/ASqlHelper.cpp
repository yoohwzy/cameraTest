#include "ASqlHelper.h"

// 面向Access的SQLHelper
ASqlHelper::ASqlHelper()
{
}


ASqlHelper::~ASqlHelper()
{
}

string ASqlHelper::CnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=D://cameraTest//AccessCtrl//11.mdb;Persist Security Info=False";
const string ASqlHelper::NONE = "_NONE_QUERY_NO_RESULT";


bool ASqlHelper::OpenCnn(_ConnectionPtr& m_pConnection)
{
	CoInitialize(NULL);//CoInitialize是Windows提供的API函数，用来告诉 Windows以单线程的方式创建com对象
	m_pConnection.CreateInstance(__uuidof(Connection));
	try
	{
		if (m_pConnection->Open(ASqlHelper::CnnStr.c_str(), "", "", adModeUnknown) == S_OK)
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

void ASqlHelper::CloseCnn(_ConnectionPtr& m_pConnection)
{
	if (m_pConnection != NULL)
	{
		m_pConnection->Close();
		m_pConnection = NULL;
	}
}
void ASqlHelper::CloseCnn(_ConnectionPtr& m_pConnection, _RecordsetPtr& m_pRecordset)
{
	//if (m_pRecordset->State)
	if (m_pRecordset != NULL)
	{
		m_pRecordset->Close();
		m_pRecordset = NULL;
	}
	CloseCnn(m_pConnection);
}




bool ASqlHelper::ExecuteNonQuery(string sql)
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
			return true;
		}
		catch (_com_error &e)
		{
			//AfxMessageBox(e.Description());
		}
	}
	CloseCnn(m_pConnection);
	return false;
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
	_ConnectionPtr m_pConnection;
	if (OpenCnn(m_pConnection))
	{
		_RecordsetPtr m_pRecordset;
		m_pRecordset.CreateInstance("ADODB.Recordset");
		_variant_t RecordsAffected;
		m_pRecordset = m_pConnection->Execute(sql.c_str(), &RecordsAffected, adCmdText);
		CloseCnn(m_pConnection);
		return m_pRecordset;
	}
	CloseCnn(m_pConnection);
	return NULL;
}
