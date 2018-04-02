
// osgMultiFileLinkCreator.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "osgMultiFileLinkCreator.h"
#include "osgMultiFileLinkCreatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CosgMultiFileLinkCreatorApp

BEGIN_MESSAGE_MAP(CosgMultiFileLinkCreatorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CosgMultiFileLinkCreatorApp ����

CosgMultiFileLinkCreatorApp::CosgMultiFileLinkCreatorApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CosgMultiFileLinkCreatorApp ����

CosgMultiFileLinkCreatorApp theApp;

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

// CosgMultiFileLinkCreatorApp ��ʼ��

BOOL CosgMultiFileLinkCreatorApp::InitInstance()
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

	SetLanguage(NULL);

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
	SetRegistryKey(_T("Visiontek\\FeatureOneGrid\\osgMultiFileLinkCreator"));


	CosgMultiFileLinkCreatorDlg dlg;
	m_pMainWnd = &dlg;
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

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

