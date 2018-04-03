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
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
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

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO:  应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CMM3DDialog dlg(typeHtml);
	theApp.m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
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
