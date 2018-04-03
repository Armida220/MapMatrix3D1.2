
// linkFetchCoord.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "linkFetchCoord.h"
#include "linkFetchCoordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ClinkFetchCoordApp

BEGIN_MESSAGE_MAP(ClinkFetchCoordApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// ClinkFetchCoordApp construction

ClinkFetchCoordApp::ClinkFetchCoordApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only ClinkFetchCoordApp object

ClinkFetchCoordApp theApp;
typedef LANGID(WINAPI *FP_SetThreadUILanguage)(LANGID LangId);

void SetLanguage(HMODULE hModule)
{
	char szFileName[MAX_PATH];
	GetModuleFileNameA(hModule, szFileName, MAX_PATH);
	char *pStr = strrchr(szFileName, '\\');
	if (NULL == pStr)pStr = szFileName;
	strcpy_s(pStr, szFileName - pStr + MAX_PATH, "\\..\\bin\\Resource.dat");
	FILE *fp = NULL;
	fopen_s(&fp, szFileName, "rt");
	szFileName[1] = 0;
	if (fp)
	{
		fgets(szFileName, MAX_PATH, fp);
		fclose(fp);
	}

	DWORD dwID;
	if (NULL != strstr(szFileName, "ENGLISH"))
	{
		WORD id = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		dwID = MAKELCID(id, SORT_DEFAULT);
	}
	else if (NULL != strstr(szFileName, "JAPANESE"))
	{
		WORD id = MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT);
		dwID = MAKELCID(id, SORT_DEFAULT);
	}
	else // CHINESE
	{
		WORD id = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
		dwID = MAKELCID(id, SORT_CHINESE_PRC);
		GetGlobalData()->UpdateFonts();
		LOGFONT lf;
		afxGlobalData.fontRegular.GetLogFont(&lf);
		afxGlobalData.fontRegular.DeleteObject();
		lf.lfCharSet = DEFAULT_CHARSET;
		lstrcpy(lf.lfFaceName, _T("MS Sans Serif"));
		afxGlobalData.fontRegular.CreateFontIndirect(&lf);
	}

	HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
	FARPROC pFn = GetProcAddress(hKernel32, "SetThreadUILanguage");

	FP_SetThreadUILanguage pSetThreadUILanguage = (FP_SetThreadUILanguage)pFn;
	if (pSetThreadUILanguage)pSetThreadUILanguage(dwID);

	SetThreadLocale(dwID);
}



// ClinkFetchCoordApp initialization

BOOL ClinkFetchCoordApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	SetLanguage(NULL);

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Visiontek\\FeatureOneGrid\\linkFetchCoord"));


	ClinkFetchCoordDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

