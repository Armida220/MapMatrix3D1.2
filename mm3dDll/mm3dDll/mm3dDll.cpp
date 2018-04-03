// mm3dDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "mm3dDll.h"
#include "include/cef_app.h"
#include <include/cef_v8.h>
#include "mm3d_app.h"
#include "mm3d_v8handler.h"
#include "MM3DDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
Cmm3dDllApp theApp;

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

CMm3dDll::CMm3dDll()
{
}

CMm3dDll::~CMm3dDll()
{
}

void CMm3dDll::showDlg()
{
	
}

// Cmm3dDllApp

BEGIN_MESSAGE_MAP(Cmm3dDllApp, CWinApp)
END_MESSAGE_MAP()


// Cmm3dDllApp construction

Cmm3dDllApp::Cmm3dDllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	m_bCEFInitialized = FALSE;
}


// The one and only Cmm3dDllApp object



// Cmm3dDllApp initialization

BOOL Cmm3dDllApp::InitInstance()
{
	m_cefApp = new CMM3dApp();

	CString typeHtml = theApp.m_lpCmdLine;


	// get arguments
	CefMainArgs main_args(theApp.m_hInstance);

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, m_cefApp.get(), NULL);
	if (exit_code >= 0)
		return exit_code;


	CefSettings settings;
	settings.no_sandbox = true;
	settings.single_process = true;
	settings.multi_threaded_message_loop = true;

	//CefString(&settings.cache_path) = szCEFCache;

	void* sandbox_info = nullptr;

	//CEF Initiaized
	m_bCEFInitialized = CefInitialize(main_args, settings, m_cefApp.get(), sandbox_info);

	CefRefPtr<CefCommandLine> command_line;
	command_line = CefCommandLine::CreateCommandLine();
	command_line->AppendSwitch("no-proxy-server");

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

	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CMM3DDialog dlg(typeHtml);
	theApp.m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	

	return TRUE;
}


int Cmm3dDllApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_bCEFInitialized)
	{
		// closing stop work loop
		m_bCEFInitialized = FALSE;
		// release CEF app
		m_cefApp = NULL;
		// shutdown CEF
		CefShutdown();
	}

	return CWinApp::ExitInstance();
}
