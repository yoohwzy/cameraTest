
// Spots.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Spots.h"

#include "Controller\ControllerCycleBuffer.h"
#include "Controller\ControllerDirectRead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpotsApp

BEGIN_MESSAGE_MAP(CSpotsApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSpotsApp ����

CSpotsApp::CSpotsApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSpotsApp ����

CSpotsApp theApp;


// CSpotsApp ��ʼ��

BOOL CSpotsApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


	//InitDevicesDlg dlgInit;
	//if (dlgInit.DoModal() == IDOK)
	{
		CSpotsMainDlg *dlgMain = new CSpotsMainDlg();
		//dlgMain->Create(IDD_SPOTS_DIALOG);
		ControllerDirectRead controller = ControllerDirectRead(dlgMain);
		m_pMainWnd = dlgMain;
		dlgMain->DoModal();

		// ɾ�����洴���� shell ��������
		if (pShellManager != NULL)
		{
			delete pShellManager;
		}

		// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
		//  ����������Ӧ�ó������Ϣ�á�
	}
	return FALSE;
}

