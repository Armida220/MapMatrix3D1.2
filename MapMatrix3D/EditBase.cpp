// EditBase.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "..\mm3dPrj\simple_app.h"
#include "Winsock2.h"
#include "EditBase.h"
#include "SymbolLib.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "EditBaseDoc.h"
#include <initguid.h>
#include "VectorView_EX.h "
#include "VectorView_GL.h "
#include "XFileDialog.h "
#include "RegDef.h "
#include "RegDef2.h "
#include "ExMessage.h "
#include "StereoView.h "
#include "UIFFileDialogEx.h "
#include "DlgReorientation.h"
#include "VectorView_new.h"
#include "StereoFrame.h "
#include "License.h"
#include "SQLiteAccess.h "
#include "SinglePhotoStereoView.h"
#include "PlugFunc.h "
#include "DlgExportCodeTable.h "
#include "DlgImportCodeTable.h "
#include "DlgExportCodeTabTemp.h "
#include "DlgConvertByPts.h"
#include "DlgConvertCoordSys.h"
#include "DlgDataTransform.h"
#include "ConvertCoords.h"
#include "GFindWindows.h"
#include "DlgTrialMsg.h"
#include "DlgSetSpecialSymbol.h"
#include "DxfAccess.h"
#include "DlgMakeCheckPtSampleBatch.h"
#include "dlgexportarcgismdb.h"
#include "DlgConnectDB.h"
#include "DlgChooseMP.h"
#include "DlgAddMapsheet.h"
#include "UVSModify.h"
#include "DlgWorkSpaceBound.h"
#include "license.h"
#include "..\mm3dPrj\OsgbDoc.h"
#include "..\mm3dPrj\OsgbView.h"
#include "..\mm3dPrj\SplashWnd.h"
#include "gdal.h"
#include "cpl_conv.h"

#ifdef TRIAL_VERSION
#include "WinlicenseSDK.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL g_isFileOnUVS = FALSE;

static int  pluginItemCmdID = PLUGIN_ITEMCMDID_BEGIN;
static int  pluginItemProcessID = PLUGIN_ITEMPROCESSID_BEGIN;

static int  pluginCollectCmdID = PLUGIN_COLLECTCMDID_BEGIN;
static int  pluginEditCmdID = PLUGIN_EDITCMDID_BEGIN;
typedef BOOL (*PFN_Plug_CreateObject)(void ** pobj);
typedef BOOL (*PFN_Plug_DeleteObject)(void ** pobj);
DWORD_PTR gdwInnerCmdData[2];
extern CDocument *GetCurDocument();
extern int GetAccessType(CDataSourceEx *pDS);

class CInitConfigFile
{
public:
	CInitConfigFile()
	{
		m_bLoaded = FALSE;
	}
	~CInitConfigFile()
	{
	}

	CString GetPath()
	{		
		//��ȡ�ļ�·��
		TCHAR module[_MAX_PATH]={0};	
		GetModuleFileName(NULL,module,_MAX_PATH);
		CString path = module;
		int pos = path.ReverseFind('\\');
		if(pos)path = path.Left(pos);
		pos = path.ReverseFind('\\');
		if(pos)path = path.Left(pos);
		path = path+"\\config\\FeatureOneInit.txt";

		return path;
	}

	void Load()
	{
		if(m_bLoaded)return;
		
		CString path = GetPath();

		FILE *fp = fopen(path,"rt");
		if(!fp)return;
		
		char line[1024];
		while(!feof(fp))
		{
			memset(line,0,sizeof(line));
			fgets(line,sizeof(line)-1,fp);
			char *pos = strchr(line,'\n');
			if(pos)*pos = '\0';

			if(strlen(line)<=0)
				continue;

			m_params.Add(CString(line));
		}

		fclose(fp);
		m_bLoaded = TRUE;

		return;
	}

	BOOL Find(LPCTSTR key,CString& value)
	{
		for(int i=0; i<m_params.GetSize()-1; i+=2)
		{
			if(m_params[i].CompareNoCase(key)==0)
			{
				value = m_params[i+1];
				return TRUE;
			}
		}
		return FALSE;
	}

	void Clear()
	{
		m_params.RemoveAll();
		m_bLoaded = FALSE;
	}

	BOOL m_bLoaded;
	CStringArray m_params;
};

CInitConfigFile g_InitConfigFile;

BOOL IsValidConfigPath(CString path)
{
	CFileFind finder;
	BOOL bWorking = finder.FindFile( LPCTSTR(path+"\\*.*") );   
    while(bWorking)   
    {   
        bWorking = finder.FindNextFile(); 
        if(finder.IsDots())   continue;	
		
        if(finder.IsDirectory())	
        { 
			CString strScale = finder.GetFileTitle();
			if( IsDigital(strScale) )	
				return TRUE;
            
        } 
        
	}

	return FALSE;
}

CString GetConfigPath(BOOL bConfigRootFolder)
{
	if (bConfigRootFolder)
	{
		char path[_MAX_PATH];
		// ���ִ���ļ�����
		if( GetModuleFileName(NULL,path,_MAX_FNAME)==0 )
			return CString();
		
		char *pos;
		// ����config·��
		if( (pos=strrchr(path,'\\')) )*pos = '\0';
		if( (pos=strrchr(path,'\\')) )*pos = '\0';
		strcat(path,"\\config");
		return CString(path);
	}
	else
	{
		CString strConfig  = AfxGetApp()->GetProfileString(REGPATH_SYMBOL,REGITEM_SYMPATH,"");	
		
		if( !strConfig.IsEmpty() )
		{
			return strConfig;
		}
		else
		{
			CString strConfigPath = gpCfgLibMan->GetPath();
			if (!strConfigPath.IsEmpty())
			{
				return strConfigPath;
			}
			
			char path[_MAX_PATH];
			// ���ִ���ļ�����
			if( GetModuleFileName(NULL,path,_MAX_FNAME)==0 )
				return CString();
			
			char *pos;
			// ����config·��
			if( (pos=strrchr(path,'\\')) )*pos = '\0';
			if( (pos=strrchr(path,'\\')) )*pos = '\0';
			strcat(path,"\\config");
			
			strConfigPath = path;

			CString symbolPath;
			if(g_InitConfigFile.Find("Symbolpath",symbolPath))
			{
				CString symbolPath2 = strConfigPath+"\\"+symbolPath;
				if(GetFileAttributes(symbolPath2)!=-1)
				{
					if (IsValidConfigPath(symbolPath2))
					{
						return symbolPath2;
					}
				}
			}
			
			CString strFirstValidConfigPath;
			CFileFind finder;
			
			BOOL bWorking = finder.FindFile( LPCTSTR(strConfigPath+"\\*.*") );   
			while(bWorking)   
			{   
				bWorking = finder.FindNextFile(); 
				if(finder.IsDots())   continue;	
				
				if(finder.IsDirectory())	
				{ 
					strConfigPath = finder.GetFilePath();

					if (IsValidConfigPath(strConfigPath))
					{	
						if (strConfigPath.Find(StrFromResID(IDS_DEFAULT_SCHEMEPATHNAME)) != -1)
						{
							finder.Close();
							return strConfigPath;
						}

						if (strFirstValidConfigPath.IsEmpty())
						{
							strFirstValidConfigPath = strConfigPath;
						}
					}			
				} 
				
			}
			
			finder.Close();

			// �����ҵ���һ����Ч�������ļ���
			return strFirstValidConfigPath;
		}	

	}
}


UINT FCCM_CALL_HELP = ::RegisterWindowMessage( "FCCM_CallHelp" );
/////////////////////////////////////////////////////////////////////////////
// CEditBaseApp

BEGIN_MESSAGE_MAP(CEditBaseApp, CWinAppEx)
	//{{AFX_MSG_MAP(CEditBaseApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_TOOL_SYMBOLMANAGE, OnToolSymbolmanage)
    ON_COMMAND(ID_LOAD_PROJECT, OnLoadProject)
	ON_COMMAND(ID_CONNECT_UVS, OnConnectDB)
	ON_COMMAND(ID_UNINSTALL_PROJECT, OnUninstallProject)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_NEW_DB, OnFileNewDB)
	ON_COMMAND(ID_FILE_OPEN, /*CWinApp::*/OnFileOpen)
	ON_COMMAND(ID_FILE_OPEN_DB, OnFileOpenDB)
	ON_COMMAND(ID_TOOL_REORIENTATION, OnReorientation)
	ON_COMMAND(ID_WINDOW_SAVEKEYS, OnSaveKeys)
	ON_COMMAND(ID_WINDOW_LOADKEYS, OnLoadKeys)
	//ON_COMMAND(ID_WINDOW_SAVELAYOUT,OnSaveLayout)
	//ON_COMMAND(ID_WINDOW_LOADLAYOUT,OnLoadLayout)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, CWinApp::OnOpenRecentFile)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_POPUP_MODIFYMOUSEPOINT, OnPopupModifyMousePoint)
	// Standard file based document commands
	
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_TOOL_COMPACTDATA,OnCompactData)
	ON_COMMAND(ID_TOOL_REPAIRDATA,OnRepairData)
	ON_COMMAND(ID_TOOL_EXPORT_CODETAB,OnExportCodeTable)
	ON_COMMAND(ID_TOOL_IMPORT_CODETAB,OnImportCodeTable)
	ON_COMMAND(ID_EXPORT_CODETABTEMP,OnExportCodeTableTemp)

	ON_COMMAND(ID_TOOL_DATATRANSFORM, OnToolConvertParams)
	ON_COMMAND(ID_TOOL_CSCONVERT, OnToolConvertCoordSys)
	ON_COMMAND(ID_TOOL_PTCONVERT, OnToolConvertPts)
	ON_COMMAND(ID_TOOL_BATCHEXPORTDXF,OnBatchExportDxf)
	ON_COMMAND(ID_TOOL_MAKE_CHECKPTSAMPLE_BATCH, OnMakeCheckPtSampleBatch)
	ON_COMMAND(ID_TOOL_UPDATEFDB_FORSCHEME, OnUpdateFDBForScheme)
	ON_COMMAND(ID_TOOL_BATCHEXPORTMDB,OnBatchExportArcgismdb)
	ON_COMMAND(ID_TOOL_UVSBROWSER, OnToolUVSBrowser)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditBaseApp construction

CEditBaseApp::CEditBaseApp()
{
	m_hAppMutex = NULL;
	m_bSupportPlugs = TRUE;
	m_hChsDll = NULL;
}

CEditBaseApp::~CEditBaseApp()
{
	CUVSModify::ReleaseUVSServer();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEditBaseApp object

CEditBaseApp theApp;

extern void RegisterPermanentObjects();

typedef LANGID (WINAPI *FP_SetThreadUILanguage)(LANGID LangId);

void SetLanguage( HMODULE hModule )
{
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName( hModule,szFileName,MAX_PATH );
	TCHAR *pStr = strrchr( szFileName,'\\' );
	if( NULL == pStr )pStr = szFileName;
	strcpy( pStr,"\\..\\bin\\Resource.dat" );
	FILE *fp = fopen( szFileName,"rt" );
	szFileName[1] = 0;
	if( fp )
	{
		fgets( szFileName,MAX_PATH,fp );
		fclose( fp );
	}
	
	DWORD dwID;
	if( NULL!=strstr(szFileName,"ENGLISH") )
	{
		WORD id = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
		dwID = MAKELCID( id,SORT_DEFAULT );
	}
	else if( NULL!=strstr(szFileName,"JAPANESE") )
	{
		WORD id = MAKELANGID( LANG_JAPANESE,SUBLANG_DEFAULT );
		dwID = MAKELCID( id,SORT_DEFAULT );
	}
	else // CHINESE
	{
		WORD id = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED);
		dwID = MAKELCID( id,SORT_CHINESE_PRC );
		GetGlobalData()->UpdateFonts();
		LOGFONT lf;
		afxGlobalData.fontRegular.GetLogFont(&lf);
		afxGlobalData.fontRegular.DeleteObject();
		lf.lfCharSet = DEFAULT_CHARSET;
		lstrcpy(lf.lfFaceName, "MS Sans Serif");   
		afxGlobalData.fontRegular.CreateFontIndirect(&lf);
	}

	HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
	FARPROC pFn = GetProcAddress(hKernel32, "SetThreadUILanguage");
	
	FP_SetThreadUILanguage pSetThreadUILanguage = (FP_SetThreadUILanguage)pFn;
	if (pSetThreadUILanguage)pSetThreadUILanguage(dwID);

	SetThreadLocale( dwID );
}


/////////////////////////////////////////////////////////////////////////////
BOOL CEditBaseApp::AlreadyRunning()
{
	BOOL bFound = FALSE;

	m_hAppMutex = ::CreateMutex(NULL,FALSE,AfxGetAppName());
	if (::GetLastError() == ERROR_ALREADY_EXISTS)
	{
		bFound =TRUE;
	}
	//Release the mutex
	if (m_hAppMutex)
	{
		::ReleaseMutex(m_hAppMutex);
	}
	return(bFound);
}

// CEditBaseApp initialization
extern void CombinConfig(CString path, CString path1);

BOOL CEditBaseApp::InitInstance()
{
	//add by ������ 2017.9.21��feature one����Ƕcef�����
	void* sandbox_info = NULL;
	CefMainArgs main_args(m_hInstance);
	CefRefPtr<SimpleApp> app(new SimpleApp);
	CefSettings settings;
	settings.no_sandbox = true;
	settings.multi_threaded_message_loop = true;
	settings.single_process = true;

	CefInitialize(main_args, settings, app.get(), sandbox_info);

	CefRefPtr<CefCommandLine> command_line;
	command_line = CefCommandLine::CreateCommandLine();
	command_line->AppendSwitch("no-proxy-server");

	CWinAppEx::InitInstance();
	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		return FALSE;
	}

	AfxEnableControlContainer();
	EnableTaskbarInteraction();
	
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");//����֧��
	//
	int res = 0;
	CString strcmd = AfxGetApp()->m_lpCmdLine;
	if (AlreadyRunning())
	{
		if (strcmd.IsEmpty())
		{
			AfxMessageBox(IDS_ERROR_CLOSEAPP);
			return FALSE;
		}

		COPYDATASTRUCT cpd;
		cpd.dwData = 0;
		cpd.cbData = strcmd.GetLength() + 1;//���ݵ����ݳ���,������ʵ������, Ӧ��+1�Ŷ�,��Ȼ�ַ�������ȥ������������, �������������.

		cpd.lpData = (void*)strcmd.GetBuffer(cpd.cbData);

		HWND hWnds[10];
		memset(hWnds, 0, sizeof(hWnds));

		res = GFindWindows(_T("FeatureOne.exe"), hWnds, 10);

		if (res > 0 && !strcmd.IsEmpty()) //�Ѿ���һ��ʵ��
		{
			::SendMessage(hWnds[0], WM_COPYDATA, 0, (LPARAM)&cpd);
		}

		return FALSE;
	}

	SetRegistryKey(_T("Visiontek\\FeatureOneGrid"));
	LoadStdProfileSettings(16);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	SetRegistryBase(_T("Settings"));

	/*CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));*///CMFCVisualManagerWindows7 CMFCVisualManagerOfficeXP

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	RegisterPermanentObjects();

	SetLanguage(NULL);

	g_InitConfigFile.Load();
	m_pCfgLibManager = new CConfigLibManager;
	if (!m_pCfgLibManager)return FALSE;
	BOOL bValidConfig = m_pCfgLibManager->LoadConfig(GetConfigPath());
	SetConfigLibManager(m_pCfgLibManager);

	//����ȱʡͼԪ������Ϳ�
	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	if ((!pCellLib || !pLineLib) && m_pCfgLibManager->GetConfigLibCount() > 0)
	{
		SetCellDefLib(m_pCfgLibManager->GetConfigLibItem(0).pCellDefLib);
		SetBaseLineTypeLib(m_pCfgLibManager->GetConfigLibItem(0).pLinetypeLib);
	}

	if (m_bSupportPlugs)
	{
		if (LoadPlugs())
			plugins::initPluginApi();
	}

	// ע��Ӧ�ó�����ĵ�ģ�塣  �ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������

	//�������滻�ɴ�����б��ͼ��view ��doc

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_EDITBATYPE,
		RUNTIME_CLASS(CDlgDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CVectorView_new));
	AddDocTemplate(pDocTemplate);

	//pDocTemplate = new CMultiDocTemplate(
	//	IDR_EDITBATYPE_STEREO,
	//	RUNTIME_CLASS(CDlgDoc),
	//	RUNTIME_CLASS(CChildFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CStereoView));
	//AddDocTemplate(pDocTemplate);

	//pDocTemplate = new CMultiDocTemplate(
	//	IDR_EDITBATYPE_STEREO2,
	//	RUNTIME_CLASS(CDlgDoc),
	//	RUNTIME_CLASS(CChildFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CSinglePhotoStereoView));
	//AddDocTemplate(pDocTemplate);

	//pDocTemplate = new CMultiDocTemplate(
	//	IDR_CELLEDITBATYPE,
	//	RUNTIME_CLASS(CDlgCellDoc),
	//	RUNTIME_CLASS(CChildFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CVectorCellView_new));
	//AddDocTemplate(pDocTemplate);

	//CSingleDocTemplate *pSinTempl = new CSingleDocTemplate(
	//	IDR_EDITBATYPE_STEREO,
	//	RUNTIME_CLASS(CDlgDoc),
	//	RUNTIME_CLASS(CStereoFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CStereoView));
	//AddDocTemplate(pSinTempl);

	//pSinTempl = new CSingleDocTemplate(
	//	IDR_EDITBATYPE_STEREO2,
	//	RUNTIME_CLASS(CDlgDoc),
	//	RUNTIME_CLASS(CStereoFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CSinglePhotoStereoView));
	//AddDocTemplate(pSinTempl);

	//pDocTemplate = new CMultiDocTemplate(
	//	IDR_EDITBATYPE_SIDE,
	//	RUNTIME_CLASS(CDlgDoc),
	//	RUNTIME_CLASS(CChildFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CVectorView_EX));
	//AddDocTemplate(pDocTemplate);
	//pDocTemplate = new CMultiDocTemplate(
	//	IDR_EDITBATYPE_GL,
	//	RUNTIME_CLASS(CDlgDoc),
	//	RUNTIME_CLASS(CChildFrame), // custom MDI child frame
	//	RUNTIME_CLASS(CVectorView_GL));
	//AddDocTemplate(pDocTemplate);

	//�����µ�osgb��ͼ���ڣ�������2017.5.4
	pDocTemplate = new CMultiDocTemplate(
		IDR_EDITBATYPE,
		RUNTIME_CLASS(CDlgDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(COsgbView));
	AddDocTemplate(pDocTemplate);

	// ������ MDI ��ܴ���
	CMainFrame* pMainFrame = new CMainFrame;
#ifdef TRIAL_VERSION
	REGISTERED_START
		if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
			return FALSE;
	REGISTERED_END
#else
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
#endif
	m_pMainWnd = pMainFrame;

	//OnLoadLayout();
	RegisterFileAssociation();

	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� MDI Ӧ�ó����У���Ӧ������ m_pMainWnd ֮����������
	// ������/��
	m_pMainWnd->DragAcceptFiles();

	if (!CheckLicense(1))
	{
		GOutPut(StrFromResID(IDS_ERR_DONGLENOTFOUND));
	}

	// ������׼ shell ���DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ���á�DDE ִ�С�
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// �������ѳ�ʼ���������ʾ����������и���
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();
	pMainFrame->DockPlugPane();

	{
		char strParam[2][256];
		char *pos = strchr(m_lpCmdLine, '*');
		if (pos && strlen(pos + 1) > 0)
		{
			*pos = 0;
			strcpy(strParam[0], m_lpCmdLine);
			strcpy(strParam[1], pos + 1);
			*pos = '*';
			AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT, FCPV_PROJECT, (LPARAM)strParam[0]);
			AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT, FCPV_DLGFILE, (LPARAM)strParam[1]);

		}
		else if (pos == NULL && strlen(m_lpCmdLine) > 0 && res != 2)
		{
			AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT, FCPV_DLGFILE, (LPARAM)m_lpCmdLine);
		}
	}

	if (!bValidConfig)
	{
		GOutPut(StrFromResID(IDS_INVALID_SYMLIBPATH));
	}

	//������  2018.2.1
	/*m_pMainWnd->SetWindowText(_T("����Զ��MapMatrix3Dͼ����ά���ܲ�ͼϵͳ"));*/

	//��������
	CSplashWnd *pSplashWindow = new CSplashWnd;//��������
	pSplashWindow->create();
	pSplashWindow->CenterWindow();
	pSplashWindow->ShowWindow(SW_SHOW);  //��ʾ����
	pSplashWindow->UpdateWindow();
	Sleep(2000);  //��ʾ�����������ʱ��
	pSplashWindow->DestroyWindow(); //������������
	delete pSplashWindow; //ɾ��

	return TRUE;
}

void CEditBaseApp::ReloadConfig()
{
	if( !m_pCfgLibManager )return;
	
	m_pCfgLibManager->Clear();
	m_pCfgLibManager->LoadConfig(GetConfigPath());
	
	//����ȱʡͼԪ������Ϳ�
	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLineLib = GetBaseLineTypeLib();
	if(m_pCfgLibManager->GetConfigLibCount()>0)
	{
		SetCellDefLib(m_pCfgLibManager->GetConfigLibItem(0).pCellDefLib);
		SetBaseLineTypeLib(m_pCfgLibManager->GetConfigLibItem(0).pLinetypeLib);
	}

	UpdateAllDocsByCmd(ID_REFRESH_DRAWING);

	if (((CMDIFrameWndEx*)m_pMainWnd)->MDIGetActive() != NULL)
		m_pMainWnd->SendMessage (AFX_WM_CHANGE_ACTIVE_TAB, (UINT)-1);
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CEditBaseApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CEditBaseApp message handlers

CConfigLibManager *CEditBaseApp::GetConfigLibManager()const
{
	return m_pCfgLibManager;
}

const PlugItem * CEditBaseApp::GetPlugins(int &nCount)
{
	nCount = m_arrPlugObjs.GetSize();
	return m_arrPlugObjs.GetData();
}

int CEditBaseApp::ExitInstance() 
{
	AfxOleTerm(FALSE);

	CefShutdown();

	CefQuitMessageLoop();

	ClearLicense(0);

	CleanState();

	g_InitConfigFile.Clear();

	if( m_pCfgLibManager )
	{
		for( int i=0; i<m_pCfgLibManager->GetConfigLibCount(); i++)
		{
			m_pCfgLibManager->GetConfigLibItem(i).pPlaceCfgLib->Save();
		}

		SetConfigLibManager(NULL);

		delete m_pCfgLibManager;
		m_pCfgLibManager = NULL;
	}

	if( m_hAppMutex )
		CloseHandle(m_hAppMutex);
	if (m_bSupportPlugs)
	{	
		FreePlugs();	
	}

	return CWinApp::ExitInstance();
}

void CEditBaseApp::OnToolSymbolmanage() 
{
}

void CEditBaseApp::OnLoadProject() 
{
	//����Ƿ������崰�ڴ��ˣ�����ǣ�����ʾ�Ƿ�ر��������崰��
	CArray<CFrameWnd*,CFrameWnd*> arrFrames;
	CView *pView = NULL;
	POSITION DocTempPos=AfxGetApp()->GetFirstDocTemplatePosition();
	while(DocTempPos!=NULL)
	{
		CDocTemplate* curTemplate=AfxGetApp()->GetNextDocTemplate(DocTempPos);
		POSITION DocPos=curTemplate->GetFirstDocPosition();
		while(DocPos!=NULL)
		{
			CDocument *pDoc=curTemplate->GetNextDoc(DocPos);
			if (pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)))
			{
				POSITION ViewPos=pDoc->GetFirstViewPosition();
				while(ViewPos)
				{
					pView=pDoc->GetNextView(ViewPos);
					if(pView->IsKindOf(RUNTIME_CLASS(CStereoView)))
					{
						arrFrames.Add(pView->GetParentFrame());
					}		
				}
			}
		}		
	}

	if( arrFrames.GetSize()>0 )
	{
		if( AfxMessageBox(IDS_TIP_CLOSE_PRJ_STEREOVIEW,MB_YESNO)!=IDYES )
			return;

		for( int i=0; i<arrFrames.GetSize(); i++)
		{
			arrFrames[i]->SendMessage(WM_CLOSE,0,0);
		}
	}

	CString filter,title;
	filter.LoadString(IDS_PROJECT_FILTER);
	
	CXFileDialog dlg(StrFromResID(IDS_LOADPROJECT),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	dlg.m_bCheckLoadAllMdl = GetProfileInt(REGPATH_CONFIG,_T("LoadProject_AllMdl"),FALSE);

	if( dlg.DoModal()!=IDOK )return;
	
	
	CString pathName = dlg.GetPathName();
	if( _access(pathName,0)==-1 )
	{
		CString strMsg;
		strMsg.FormatMessage( IDS_FILE_OPEN_ERR,dlg.GetPathName() );
		AfxMessageBox(strMsg);
		return;
	}
	
	WriteProfileInt(REGPATH_CONFIG,_T("LoadProject_AllMdl"),dlg.m_bCheckLoadAllMdl);
	
	if( dlg.m_bCheckLoadAllMdl )
		pathName = _T("*") + pathName;
	AfxGetMainWnd()->SendMessage(FCCM_UNINSTALLPROJ,0,0);
	AfxGetMainWnd()->SendMessage(FCCM_LOADPROJECT,FCPV_PROJECT,(LPARAM)(LPCTSTR)pathName);
}

void CEditBaseApp::OnUninstallProject() 
{
	((CMainFrame*)AfxGetMainWnd())->SendMessage(FCCM_LOADPROJECT,FCPV_PROJECT,NULL);	
	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		if( /*curTemplate!=m_pStartDocTemplate && */curTemplate!=NULL )
			curTemplate->CloseAllDocuments(FALSE);
	}
	
	//by shy
	//AfxCallMessage(FCCM_UNINSTALLPROJ,0,0);
	AfxGetMainWnd()->SendMessage(FCCM_UNINSTALLPROJ,0,0);
	
}

BOOL IsOutOfDiskSpace(LPCSTR lpDirectoryName)
{
	ULARGE_INTEGER nFreeBytesAvailable;
    ULARGE_INTEGER nTotalNumberOfBytes;
    ULARGE_INTEGER nTotalNumberOfFreeBytes;
   
    if (GetDiskFreeSpaceEx(lpDirectoryName,
         &nFreeBytesAvailable,
         &nTotalNumberOfBytes,
         &nTotalNumberOfFreeBytes))
    {
		 // С��100k���̿ռ䲻��
         if (nFreeBytesAvailable.QuadPart < 100*1024)
         {
			 return TRUE;
         }
    }

	return FALSE;
}

void CEditBaseApp::OnFileNew() 
{
	CString filter,str1,str2;
	CStringArray extlist;
	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		
		str1.Empty();
		str2.Empty();
		if( curTemplate->GetDocString(str1,CDocTemplate::filterName) &&
			curTemplate->GetDocString(str2,CDocTemplate::filterExt) )
		{
			if( str2.CompareNoCase(_T(".mdl"))==0
				||str2.CompareNoCase(_T(".eps"))==0
				||str2.CompareNoCase(_T(".abc"))==0 
				||str2.CompareNoCase(_T(".st2"))==0 )continue;
			filter = filter + str1 + _T("|*") + str2 + _T("|");
			extlist.Add(str2);
		}
	}
	
	if (IsSupportPlugs())
	{
		for (int i=m_arrPlugObjs.GetSize()-1;i>=0;i--)
		{
			if (!m_arrPlugObjs[i].bUsed) continue;

			if(m_arrPlugObjs[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
			{
				CPlugDocs *p = (CPlugDocs*)(m_arrPlugObjs[i].pObj);
				CString exstr = p->GetExtString();
				CString str(exstr);
				str.Delete(0);
				str = str + _T(" files")+_T("(") + exstr+_T(")");
				filter.Delete(filter.GetLength()-3,3);
				filter = filter + str + _T("|*") + exstr + _T("|");
				filter = filter + _T("|*") + _T("|");
			}
		}
	}
	if( filter.GetLength()<=0 )filter.LoadString(IDS_ALLFILE_FILTER);
	
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, ".fdb", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	if( dlg.DoModal()!=IDOK ) return;
	CString pathName = dlg.GetPathName();
	BOOL bIsUsePlug = FALSE;
	if (IsSupportPlugs())
	{
		for (int i=m_arrPlugObjs.GetSize()-1;i>=0;i--)
		{
			if (!m_arrPlugObjs[i].bUsed) continue;

			if(m_arrPlugObjs[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
			{
				CPlugDocs *p = (CPlugDocs*)(m_arrPlugObjs[i].pObj);
				CString exstr = p->GetExtString();
				if (pathName.Right(4).CompareNoCase(exstr)==0)
				{
					bIsUsePlug = TRUE;
				}
			}
		}
	}
	gbCreatingWithPath = TRUE;
	
	int index = pathName.Find('\\');
	CString strRoot = pathName.Left(index+1);
	if (IsOutOfDiskSpace(strRoot))
	{
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_ERR_DISKSPACEINSUFFICIENT));
		return;
	}

	//���ĵ�
	curTemplatePos = GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		if(!bIsUsePlug && curTemplate->MatchDocType( pathName, rpDocMatch )==CDocTemplate::yesAttemptNative )
		{
			curTemplate->OpenDocumentFile(pathName);
			break;
		}
		if (bIsUsePlug && curTemplate->MatchDocType( _T(".fdb"), rpDocMatch )==CDocTemplate::yesAttemptNative)
		{
			curTemplate->OpenDocumentFile(pathName);
			break;
		}
	}
	
	gbCreatingWithPath = FALSE;
	
}

void CEditBaseApp::OnFileNewDB() 
{
	if(!CUVSModify::IsDBConnected())
	{
		AfxMessageBox(IDS_NOT_CONN_UVS);
		return;
	}

	CDlgAddMapsheet dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	CString pathName = dlg.m_mapsheet_name;

	CDlgWorkSpaceBound dlgB;
	dlgB.m_lfX1 = 0;
	dlgB.m_lfY1 = 0;
	dlgB.m_lfX2 = 1000;
	dlgB.m_lfY2 = 0;
	dlgB.m_lfX3 = 1000;
	dlgB.m_lfY3 = 1000;
	dlgB.m_lfX4 = 0;
	dlgB.m_lfY4 = 1000;
	dlgB.m_lfZmin = -1000.0;
	dlgB.m_lfZmax = 1000.0;
	dlgB.m_bUVS = TRUE;
	if (dlgB.DoModal() != IDOK)
		return;

	if (!CUVSModify::AddMapsheet(pathName, dlgB.m_lfX1, dlgB.m_lfY1, dlgB.m_lfX2, dlgB.m_lfY2, 
		dlgB.m_lfX3, dlgB.m_lfY3, dlgB.m_lfX4, dlgB.m_lfY4, dlgB.m_lfZmin, dlgB.m_lfZmax))
	{
		AfxMessageBox(IDS_CREATE_UVS_FAILE);
		return;
	}

	OpenFileOnUVS(pathName);
}

//�������ļ�(UVS)
void CEditBaseApp::OnFileOpenDB() 
{
	CDlgChooseMP dlg;
	if( dlg.DoModal()!=IDOK )
	{
		g_isFileOnUVS = FALSE;
		return;
	}

	for (int j = 0; j < dlg.m_arrSelectedMaps.GetSize(); j++)
	{
		CString fileName = dlg.m_arrSelectedMaps[j];
		fileName.TrimLeft();
		fileName.TrimRight();
		if (fileName.IsEmpty())
		{
			continue;
		}
		OpenFileOnUVS(fileName);
	}
}

void CEditBaseApp::OpenFileOnUVS(LPCTSTR mapname)
{
	g_isFileOnUVS = TRUE;

	BOOL IsAlreadyOpen = FALSE;

	CString pathName = mapname;

	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	while (curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		POSITION pos = curTemplate->GetFirstDocPosition();
		while (pos != NULL)
		{
			CDocument* pDoc = curTemplate->GetNextDoc(pos);
			if (pathName == pDoc->GetTitle())
			{
				// already open
				IsAlreadyOpen = TRUE;
				break;
			}
		}
		if (IsAlreadyOpen) break;
	}

	if (IsAlreadyOpen)
	{
		g_isFileOnUVS = FALSE;
		return;
	}

	curTemplatePos = GetFirstDocTemplatePosition();
	while (curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		if (curTemplate->MatchDocType(pathName + ".fdb", rpDocMatch) == CDocTemplate::yesAttemptNative)
		{
			curTemplate->OpenDocumentFile(pathName);
			break;
		}
	}

	g_isFileOnUVS = FALSE;
}

//�������ļ�
void CEditBaseApp::OnFileOpen() 
{
	g_isFileOnUVS = FALSE;
	// ��������ļ���
	CString filter,str1,str2;
	CStringArray extlist;
	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		
		str1.Empty();
		str2.Empty();
		if( curTemplate->GetDocString(str1,CDocTemplate::filterName) &&
			curTemplate->GetDocString(str2,CDocTemplate::filterExt) )
		{
			if( str2.CompareNoCase(_T(".mdl"))==0
				||str2.CompareNoCase(_T(".eps"))==0
				||str2.CompareNoCase(_T(".abc"))==0
				||str2.CompareNoCase(_T(".st2"))==0)continue;			
			filter = filter + str1 + _T("|*") + str2 + _T("|");
			extlist.Add(str2);
		}
	}
	if (IsSupportPlugs())
	{
		for (int i=m_arrPlugObjs.GetSize()-1;i>=0;i--)
		{
			if (!m_arrPlugObjs[i].bUsed) continue;

			if(m_arrPlugObjs[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
			{
				CPlugDocs *p = (CPlugDocs*)(m_arrPlugObjs[i].pObj);
				CString exstr = p->GetExtString();
				CString str(exstr);
				str.Delete(0);
				str = str + _T(" files")+_T("(") + exstr+_T(")");
				filter.Delete(filter.GetLength()-3,3);
				filter = filter + str + _T("|*") + exstr + _T("|");
				filter = filter + _T("|*") + _T("|");
			}
		}
	}
	if( filter.GetLength()<=0 )filter.LoadString(IDS_ALLFILE_FILTER);
	
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, ".fdb", NULL, OFN_ENABLESIZING | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	//This Code was Added by cjc
//********************************************************************//
	dlg.m_ofn.nMaxFile = 4096;
	dlg.m_ofn.lpstrFile = new char[dlg.m_ofn.nMaxFile*MAX_PATH];
	ZeroMemory(dlg.m_ofn.lpstrFile,sizeof(char)*dlg.m_ofn.nMaxFile*MAX_PATH);
	
//*******************************************************************//
	if( dlg.DoModal()!=IDOK ) return;

	CStringArray arrFiles;
		
//	if( dlg.GetPage()==CFileDialogEx::UIFFileOpen )
	{
		POSITION pos = dlg.GetStartPosition();//��ʾ��ǰ��ѡ�ĵ�һ���ļ�λ�� 
		while (pos !=NULL)
		{
			arrFiles.Add(dlg.GetNextPathName(pos));//��Ӷ�ѡ���ļ�����Cstring����	
		}	
	}
//	else
//	{
//		arrFiles.Add(dlg.GetPathName());
//	}

	for (int i=0;i<arrFiles.GetSize();i++ )
	{
		CString pathName = arrFiles.GetAt(i);	

		BOOL bIsUsePlug = FALSE;
		if (IsSupportPlugs())
		{
			for (int i=m_arrPlugObjs.GetSize()-1;i>=0;i--)
			{
				if (!m_arrPlugObjs[i].bUsed) continue;
				
				if(m_arrPlugObjs[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
				{
					CPlugDocs *p = (CPlugDocs*)(m_arrPlugObjs[i].pObj);
					CString exstr = p->GetExtString();
					if (pathName.Right(4).CompareNoCase(exstr)==0)
					{
						bIsUsePlug = TRUE;
					}
				}
			}
		}
		curTemplatePos = GetFirstDocTemplatePosition();
		
		while(curTemplatePos!=NULL)
		{
			CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
			CDocument *rpDocMatch;
			if(!bIsUsePlug && curTemplate->MatchDocType( pathName, rpDocMatch )==CDocTemplate::yesAttemptNative )
			{
				curTemplate->OpenDocumentFile(pathName);
				break;
			}
			if (bIsUsePlug && curTemplate->MatchDocType( _T(".fdb"), rpDocMatch )==CDocTemplate::yesAttemptNative)
			{
				curTemplate->OpenDocumentFile(pathName);
				break;
			}
		}		
	}
}


void CEditBaseApp::OnPopupModifyMousePoint() 
{
/*	CFeatureCollectorApp *pMyApp = (CFeatureCollectorApp*)AfxGetApp(); //�õ�Ӧ�ó���ָ��*/
	POSITION p = /*pMyApp->*/GetFirstDocTemplatePosition();//�õ���1���ĵ�ģ��
	while (p != NULL) //�����ĵ�ģ��
	{
		CDocTemplate *pDocTemplate = /*pMyApp->*/GetNextDocTemplate(p);
		POSITION p1 = pDocTemplate->GetFirstDocPosition();//�õ��ĵ�ģ���Ӧ�ĵ�1���ĵ�
		while (p1 != NULL) //�����ĵ�ģ���Ӧ���ĵ�
		{
			CDocument *pDocument = pDocTemplate->GetNextDoc(p1);
			POSITION p2 = pDocument->GetFirstViewPosition(); //�õ��ĵ���Ӧ�ĵ�1����ͼ
			while (p2 != NULL) //�����ĵ���Ӧ����ͼ
			{
				CView *pView = (CView*)pDocument->GetNextView(p2);
				if(pView)
					SendMessage(pView->m_hWnd,FCCM_MODIFYMOUSEPOINT,0,0);
			}
		}
	}
			
}

BOOL CompactDataFile(LPCTSTR fileName)
{
	CString tmpDbPathName;
	CString strName = fileName;
	
	CSQLiteAccess  Access ;
	if(!Access.OpenWrite(fileName))
	{
		return FALSE;
	}
	Access.Compress();	
	Access.CloseWrite();
	return TRUE;
}

BOOL RepairDataFile(LPCTSTR fileName)
{
	CString tmpDbPathName;
	CString strName = fileName;
	
	CSQLiteAccess  Access ;
	if(!Access.OpenWrite(fileName))
	{
		return FALSE;
	}
	Access.Repair();	
	Access.CloseWrite();
	return TRUE;
}

#define MAX_FILE_NUM	2000
void CEditBaseApp::OnCompactData()
{
	OPENFILENAME ofn;
	// �洢��ȡ�Ķ���ļ���
	TCHAR *filename = new TCHAR[MAX_PATH*MAX_FILE_NUM];
	memset(filename,0,sizeof(TCHAR)*MAX_PATH*MAX_FILE_NUM);

	TCHAR OldDir[MAX_PATH];
	
	CString strTitle;
	strTitle.LoadString(IDS_SELECT_FILE);
	
	::GetCurrentDirectory(MAX_PATH, OldDir);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_pMainWnd->GetSafeHwnd();
	ofn.lpstrFilter = _T("FDB files (*.fdb)\0*.fdb\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename; // ָ�����ص��ļ���
	ofn.nMaxFile = MAX_PATH*MAX_FILE_NUM;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = strTitle.GetBuffer(0);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER; // ������ļ�ѡ��
	ofn.lpstrDefExt = _T("*.fdb");
	*filename = '\0';
	
	if(!GetOpenFileName(&ofn))
	{
		delete[] filename;
		strTitle.ReleaseBuffer();
		return;
	}
	
	strTitle.ReleaseBuffer();
	
	CString path = ofn.lpstrFile; //������һ��\0Ϊֹǰ�ľ���·��: path\0filename��path
	TCHAR *fname = ofn.lpstrFile;
	
	CStringArray fileNames;
	
	DWORD attr = ::GetFileAttributes(path);
	
	//�����ļ�
	if( attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY)==0 )
	{
		fileNames.Add(path);
	}
	else
	{
		if( path.Right(1)!=_T("\\") )
			path += _T("\\");
		
		// ����path, �ҵ���һ���ļ�������ʼλ��
		while (*fname++);  
		// ��ʱfnameָ���һ���ļ���
		int i = 0;
		while(*fname)
		{
			CString dxfFile = path+CString(fname);
			fileNames.Add(dxfFile);
			
			// ����һ���ļ����� �ҵ���һ���ָ���\0��ѭ��ֹͣ��ͬʱƫ�Ƽ�һ���պ�ָ����һ���ļ���
			while (*fname++);
			i++;
		}
	}

	delete[] filename;
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PROCESS));
	for( int i=fileNames.GetSize()-1; i>=0; i--)
	{
		CString strMsg;
		if( CompactDataFile(fileNames.GetAt(i)) )
			strMsg = StrFromResID(IDS_OUTPUT_SUCCESS) + _T(": ") + fileNames.GetAt(i) + _T("\n");
		else
			strMsg = StrFromResID(IDS_OUTPUT_FAILED) + _T(": ") + fileNames.GetAt(i) + _T("\n");
		
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)strMsg);
	}
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END));
}

void CEditBaseApp::OnRepairData()
{
	OPENFILENAME ofn;
	// �洢��ȡ�Ķ���ļ���
	TCHAR *filename = new TCHAR[MAX_PATH*MAX_FILE_NUM];
	memset(filename,0,sizeof(TCHAR)*MAX_PATH*MAX_FILE_NUM);

	TCHAR OldDir[MAX_PATH];
	
	CString strTitle;
	strTitle.LoadString(IDS_SELECT_FILE);
	
	::GetCurrentDirectory(MAX_PATH, OldDir);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = m_pMainWnd->GetSafeHwnd();
	ofn.lpstrFilter = _T("FDB files (*.fdb)\0*.fdb\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename; // ָ�����ص��ļ���
	ofn.nMaxFile = MAX_PATH*MAX_FILE_NUM;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = strTitle.GetBuffer(0);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER; // ������ļ�ѡ��
	ofn.lpstrDefExt = _T("*.fdb");
	*filename = '\0';
	
	if(!GetOpenFileName(&ofn))
	{
		delete[] filename;
		strTitle.ReleaseBuffer();
		return;
	}
	
	strTitle.ReleaseBuffer();
	
	CString path = ofn.lpstrFile; //������һ��\0Ϊֹǰ�ľ���·��: path\0filename��path
	TCHAR *fname = ofn.lpstrFile;
	
	CStringArray fileNames;
	
	DWORD attr = ::GetFileAttributes(path);
	
	//�����ļ�
	if( attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY)==0 )
	{
		fileNames.Add(path);
	}
	else
	{
		if( path.Right(1)!=_T("\\") )
			path += _T("\\");
		
		// ����path, �ҵ���һ���ļ�������ʼλ��
		while (*fname++);  
		// ��ʱfnameָ���һ���ļ���
		int i = 0;
		while(*fname)
		{
			CString dxfFile = path+CString(fname);
			fileNames.Add(dxfFile);
			
			// ����һ���ļ����� �ҵ���һ���ָ���\0��ѭ��ֹͣ��ͬʱƫ�Ƽ�һ���պ�ָ����һ���ļ���
			while (*fname++);
			i++;
		}
	}
	
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_PROCESS));
	for( int i=fileNames.GetSize()-1; i>=0; i--)
	{
		CString strMsg;
		if( RepairDataFile(fileNames.GetAt(i)) )
			strMsg = StrFromResID(IDS_OUTPUT_SUCCESS) + _T(": ") + fileNames.GetAt(i) + _T("\n");
		else
			strMsg = StrFromResID(IDS_OUTPUT_FAILED) + _T(": ") + fileNames.GetAt(i) + _T("\n");
		
		AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)strMsg);
	}
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END));
	delete[] filename;
}

void CEditBaseApp::OnExportCodeTable()
{
	CDlgExportCodeTable dlg(NULL,m_pCfgLibManager);
	dlg.DoModal();
}

void CEditBaseApp::OnExportCodeTableTemp()
{
	CDlgExportCodeTabTemp dlg(NULL,m_pCfgLibManager);
	dlg.DoModal();
}
void CEditBaseApp::OnImportCodeTable()
{
	CDlgImportCodeTable dlg(NULL,m_pCfgLibManager);
	dlg.DoModal();
}

void CEditBaseApp::OnReorientation() 
{
	CDlgReorientation dlg;
	dlg.DoModal();
}


void CEditBaseApp::OnMakeCheckPtSampleBatch() 
{
	CDlgMakeCheckPtSampleBatch dlg;
	dlg.DoModal();
}


void CEditBaseApp::RegisterFileAssociation()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName( NULL, szPath, MAX_PATH );
	CString strCommand;
	strCommand.Format( "\"%s\" \"%%1\"", szPath );
	
	TCHAR szKey[] = _T("FeatureOneGrid1.0");
	TCHAR szKeyName[] = _T("FeatureOneGrid1.0");
	TCHAR szKeyOpen[256];
	strcpy(szKeyOpen,(LPCTSTR)StrFromResID(IDS_FILEASSOSIATION_DESC));
	
	HKEY hKey = NULL;
	long ret;
	ret = RegCreateKey( HKEY_CLASSES_ROOT, szKey, &hKey );
	if( ERROR_SUCCESS == ret && hKey )
	{	
		RegSetValue( hKey, _T(""), REG_SZ, szKeyName, 0 );
		HKEY hKeyShell;
		ret = RegCreateKey( hKey, "shell", &hKeyShell );
		RegSetValue( hKeyShell, _T(""), REG_SZ, _T("Open"), 0 );
		
		HKEY hKeyOpen;
		ret = RegCreateKey( hKeyShell, "Open", &hKeyOpen );
		RegSetValue( hKeyOpen, _T(""), REG_SZ, szKeyOpen, 0 );
		
		HKEY hKeyCommand;
		ret = RegCreateKey( hKeyOpen, "command", &hKeyCommand );
		RegSetValue( hKeyCommand, _T(""), REG_SZ, strCommand, 0 );
		
		strCommand.Format( "%s,0", szPath );
		RegSetValue( hKey, _T("DefaultIcon"), REG_SZ, strCommand, 0 );
		
		RegCloseKey( hKeyCommand );
		RegCloseKey( hKeyOpen );
		RegCloseKey( hKeyShell );
		RegCloseKey( hKey );
		
		ret = RegCreateKey( HKEY_CLASSES_ROOT, _T(".fdb"), &hKey );
		if( ERROR_SUCCESS == ret )
		{
			RegSetValue( hKey, _T(""), REG_SZ, szKey, 0 );
			RegCloseKey( hKey );
		}
		ret = RegCreateKey( HKEY_CLASSES_ROOT, _T(".gdb"), &hKey );
		if( ERROR_SUCCESS == ret )
		{
			RegSetValue( hKey, _T(""), REG_SZ, szKey, 0 );
			RegCloseKey( hKey );
		}		
	}
}

	
CEditBaseModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

LONG CEditBaseModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}

LONG CEditBaseModule::Lock()
{
	AfxOleLockApp();
	return 1;
}
LPCTSTR CEditBaseModule::FindOneOf(LPCTSTR p1, LPCTSTR p2)
{
	while (*p1 != NULL)
	{
		LPCTSTR p = p2;
		while (*p != NULL)
		{
			if (*p1 == *p)
				return CharNext(p1);
			p = CharNext(p);
		}
		p1++;
	}
	return NULL;
}


void CEditBaseApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu (StrFromResID(IDS_MAIN_CONTEXT_MENU), IDR_POPUP_MAIN_AREA);
}

void CEditBaseApp::UpdateAllDocsByCmd(UINT nCmdID)
{
	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		
		CDocument *pDoc;
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while(curDocPos!=NULL)
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if( pDoc )pDoc->OnCmdMsg(nCmdID,0,0,0);
		}
	}
}

void CEditBaseApp::OnSaveKeys()
{
	CString filter("Reg File(*.reg)|*.reg|All Files(*.*)|*.*||");
	
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,".reg","KeyBoard.reg",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filter);
	if( dlg.DoModal()!=IDOK ) return;
	CString pathName = dlg.GetPathName();

	CString cmd;
	cmd.Format("/e %s HKEY_CURRENT_USER\\Software\\%s\\%s\\Settings\\Keyboard-0", pathName, m_pszRegistryKey, m_pszAppName);

	ShellExecute(AfxGetMainWnd()->m_hWnd, "open", "regedit.exe", cmd, NULL, SW_NORMAL);
}


void CEditBaseApp::OnLoadKeys()
{
	CString filter("Reg File(*.reg)|*.reg|All Files(*.*)|*.*||");

	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, ".reg", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	if (dlg.DoModal() != IDOK) return;
	CString pathName = dlg.GetPathName();

	CString cmd;
	cmd.Format("/s %s", pathName);

	ShellExecute(AfxGetMainWnd()->m_hWnd, "open", "regedit.exe", cmd, NULL, SW_NORMAL);
	LoadState();
}


/*void CEditBaseApp::OnSaveLayout()
{
//	CString old = m_strRegSection;
//	SaveState((CMDIFrameWndEx*)AfxGetMainWnd(),"Layout");
//	m_strRegSection = old;

	//������ͼ�ӿ�ܵ�λ��
	char path[256];
	CString curClass;
	CStringArray classList;
//	CFramePlaceAssist place;
	int samenum = 1;

	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		
		CDocument *pDoc;
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while(curDocPos!=NULL)
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if( pDoc )
			{
				CView *pView;
				CFrameWnd *pFrame;
				POSITION curViewPos = pDoc->GetFirstViewPosition();
				while( curViewPos )
				{
					pView = pDoc->GetNextView(curViewPos);
					if( !pView )continue;
					
					pFrame = pView->GetParentFrame();
					if( !pFrame )continue;
					
					//������ͬ������
					curClass = pView->GetRuntimeClass()->m_lpszClassName;
					for( int i=classList.GetSize()-1; i>=0; i--)
					{
						if( curClass.Compare(classList[i])==0 )break;
					}

					if( i>=0 )
					{
						curClass.Format("%s%d",pView->GetRuntimeClass()->m_lpszClassName,samenum++);
					}
					else classList.Add(curClass);
					
					strcpy(path,"Layout\\MDIPlace\\");
					strcat(path,curClass);
//					place.Attach(pFrame,path,FALSE);
//					place.SavePlace();
				}
			}
		}
	}
}

void CEditBaseApp::OnLoadLayout()
{
	return;
	CMDIChildWnd *pMDIChild = ((CMDIFrameWndEx*)AfxGetMainWnd())->MDIGetActive();
	CView *pView = NULL;
	if (pMDIChild)
	{
		pView = pMDIChild->GetActiveView();
	}

	//�ж��Ƿ���ڲ�������
	HKEY hkey;
	char path[256];
	sprintf(path,"Software\\%s\\%s\\Layout",m_pszRegistryKey,m_pszAppName);

//	CString old = m_strRegSection;
//	LoadLayout((CMDIFrameWndEx*)AfxGetMainWnd(),"Layout");
//	m_strRegSection = old;


	//����װ����ͼ�ӿ�ܵ�λ��
	CString curClass;
	CStringArray classList;
//	CFramePlaceAssist place;
	int samenum = 1;

	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		
		CDocument *pDoc;
		POSITION curDocPos = curTemplate->GetFirstDocPosition();
		while(curDocPos!=NULL)
		{
			pDoc = curTemplate->GetNextDoc(curDocPos);
			if( pDoc )
			{
				CView *pView;
				CFrameWnd *pFrame;
				POSITION curViewPos = pDoc->GetFirstViewPosition();
				while( curViewPos )
				{
					pView = pDoc->GetNextView(curViewPos);
					if( !pView )continue;
					
					pFrame = pView->GetParentFrame();
					if( !pFrame )continue;

					//������ͬ������
					curClass = pView->GetRuntimeClass()->m_lpszClassName;
					for( int i=classList.GetSize()-1; i>=0; i--)
					{
						if( curClass.Compare(classList[i])==0 )break;
					}
					
					if( i>=0 )
					{
						curClass.Format("%s%d",pView->GetRuntimeClass()->m_lpszClassName,samenum++);
					}
					else classList.Add(curClass);
					
					strcpy(path,"Layout\\MDIPlace\\");
					strcat(path,curClass);
//					place.Attach(pFrame,path,FALSE);
//					place.LoadPlace();
				}
			}
		}
	}

	if (pView)
	{
		((CChildFrame*)pMDIChild)->MDIActivate(TRUE,pView,NULL);

		CDocument *pDoc = pView->GetDocument();
		if (pDoc)
		{
			AfxGetMainWnd()->SendMessage(FCCM_INITLAYERTOOL, WPARAM(0), LPARAM(pDoc));
			AfxGetMainWnd()->SendMessage(FCCM_INITFTRTOOL, WPARAM(0), LPARAM(pDoc));
		}
	}
}*/

BOOL CEditBaseApp::LoadPlugs()
{
	TCHAR filename[_MAX_PATH];
	GetModuleFileName(NULL, filename, _MAX_PATH); // ��ȡӦ�ó���·��
	CString strPath(filename);
 	int pos = strPath.ReverseFind(_T('\\'));
	if(pos==-1)return FALSE;
	strPath = strPath.Left(pos+1)+_T("Plugins\\");
	CString strFindFile = strPath + "*.dll";
	WIN32_FIND_DATA wfd;
	HANDLE hf = FindFirstFile(strFindFile, &wfd); //Ѱ�ҵ�һ��
	if (hf != INVALID_HANDLE_VALUE)
	{
		CreatePlug(strPath + wfd.cFileName);
		while (FindNextFile(hf, &wfd)) //����Ѱ����һ��
			CreatePlug(strPath + wfd.cFileName);
		FindClose(hf); // ������Ѱ
	}
	return TRUE;
}
void CEditBaseApp::FreePlugs()
{
	for (int i=0;i<m_arrPlugObjs.GetSize();i++)
	{
		if (m_arrPlugObjs[i].itemID)
		{
			delete [](m_arrPlugObjs[i].itemID);
		}
		if (m_arrPlugObjs[i].pObj)
			m_arrPlugObjs[i].pObj->Release();
		PFN_Plug_DeleteObject pFunc = (PFN_Plug_DeleteObject)GetProcAddress(m_arrPlugObjs[i].hIns, "Plug_DeleteObject");
		if (pFunc((void**)&m_arrPlugObjs[i].pObj));
		if(m_arrPlugObjs[i].hIns)
			FreeLibrary(m_arrPlugObjs[i].hIns);
	}
	m_arrPlugObjs.RemoveAll();
}
BOOL CEditBaseApp::CreatePlug(LPCTSTR fileName)
{
	PlugItem plugItem;
	plugItem.itemID = NULL;
	ZeroMemory(&plugItem, sizeof(PlugItem));
	plugItem.hIns = LoadLibrary(fileName);
	if (!plugItem.hIns)
	{
		return FALSE;
	}
	PFN_Plug_CreateObject pFunc = (PFN_Plug_CreateObject)GetProcAddress(plugItem.hIns, "Plug_CreateObject");
	if (pFunc((void **)&plugItem.pObj))
	{
		plugItem.pObj->Init();

		int itemCount = plugItem.pObj->GetItemCount();
		const int *funTypes = plugItem.pObj->GetFuncTypes();
		const UINT *cmdIds = plugItem.pObj->GetItemCommandIDs();
		plugItem.itemID = new UINT[itemCount];
		plugItem.itemPlugID = new UINT[itemCount];
		if (!plugItem.itemID || !plugItem.itemPlugID)
		{			
			plugItem.pObj->Release();
			PFN_Plug_DeleteObject pFunc0 = (PFN_Plug_DeleteObject)GetProcAddress(plugItem.hIns, ("Plug_DeleteObject"));
			pFunc0((void**)&plugItem.pObj);		
			FreeLibrary(plugItem.hIns);
			return FALSE;
		}
		memcpy(plugItem.itemPlugID,cmdIds,sizeof(UINT)*itemCount);

		for (int i=0;i<itemCount;i++)
		{
			if ((funTypes[i]&PLUGFUNC_TYPE_CREATE_COLLECT) == PLUGFUNC_TYPE_CREATE_COLLECT)
			{
				plugItem.itemID[i] = pluginCollectCmdID++;
			}
			else if ((funTypes[i]&PLUGFUNC_TYPE_CREATE_EDIT) == PLUGFUNC_TYPE_CREATE_EDIT)
			{
				plugItem.itemID[i] = pluginEditCmdID++;
			}
			else if ((funTypes[i]&PLUGFUNC_TYPE_PROCESS) == PLUGFUNC_TYPE_PROCESS)
			{
				plugItem.itemID[i] = pluginItemProcessID++;
			}
		}
		
		int bUsed = GetProfileInt(PLUGIN_SECTION,plugItem.pObj->GetPlugName(),0);
		plugItem.bUsed = bUsed;
		m_arrPlugObjs.Add(plugItem);
		return TRUE;
	}
	return FALSE;
}

BOOL CreateFolder(LPCTSTR path)
{
	char line[1024];
	sprintf(line, "cmd /c md \"%s\"",path);

	STARTUPINFO stStartUpInfo;
	
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	
	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));
	
	CreateProcess(NULL, (LPTSTR)line, NULL, NULL, TRUE,
		CREATE_NO_WINDOW, NULL, 
		NULL, &stStartUpInfo, &stProcessInfo);

	if( WaitForSingleObject(stProcessInfo.hProcess,INFINITE)!=WAIT_OBJECT_0 )
		return FALSE;

	return TRUE;
}

BOOL CreateFolder2(LPCTSTR path)
{
	CString strPath = path;
	CString strWPath = strPath;
	CString strTemp;
	if(!PathFileExists(strPath))//�ļ��в������򴴽�
	{		
		strPath = strPath.Mid(3);
		int i = strPath.Find("\\");
		if(i>0)
		{
			strTemp = strWPath.Left(3) + strPath.Left(i);
		}
		else
		{
			strTemp = strWPath;
		}
		strPath = strPath.Mid(i);
		if(!PathFileExists(strTemp))
			CreateDirectory(strTemp,NULL);
		
		while(strPath.Find("\\") == 0)
		{
			strPath = strPath.Mid(1);
			int j = strPath.Find("\\");
			if(j > 0)
			{
				strTemp = strTemp + "\\" + strPath.Left(j);
				strPath = strPath.Mid(j);
			}
			else
				strTemp = strTemp + "\\" + strPath;
			if(!PathFileExists(strTemp))
				CreateDirectory(strTemp, NULL);
		}
	}
	return TRUE;
}

void CEditBaseApp::OnToolConvertCoordSys()
{
	CDlgConvertCoordSys dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	//����ȫͼ
	if( dlg.m_nRange==0 )
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( !pDoc )
			return;

		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		ConvertDS(pDS,&dlg.m_tm0,&dlg.m_tm1,TRUE,NULL);

		pDoc->BeginBatchUpdate();
		pDS->SaveAll(NULL);
		pDoc->EndBatchUpdate();

		pDS->SetModifiedFlag(TRUE);

		pDS->UpdateFtrQuery(NULL);

		pDoc->UpdateAllViews(NULL,hc_UpdateAllObjects);
	}
	//����ѡ��
	else if( dlg.m_nRange==1 )
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( !pDoc )
			return;

		int num = 0;
		const FTR_HANDLE *ftrs = pDoc->GetSelection()->GetSelectedObjs(num);
		CFtrArray arr;
		for( int i=0; i<num; i++)
		{
			arr.Add(HandleToFtr(ftrs[i]));
		}

		ConvertFtrs(arr,&dlg.m_tm0,&dlg.m_tm1,TRUE);

		pDoc->BeginBatchUpdate();

		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		for( i=0; i<num; i++)
		{
			pDoc->DeleteObject(FtrToHandle(arr[i]));
			pDoc->RestoreObject(FtrToHandle(arr[i]));
		}
		
		pDoc->EndBatchUpdate();
		pDoc->DeselectAll();

		for( i=0; i<num; i++)
		{
			pDoc->GetSelection()->SelectObj(FtrToHandle(arr[i]));
		}

		pDoc->OnSelectChanged();
	}
	//�����ļ�
	else
	{
		CStringArray filePaths;
		filePaths.Copy(dlg.m_arrFileNames);

		int nsz = filePaths.GetSize();
		if( nsz<=0 )return;

		CString path = filePaths[0];
		int pos = path.ReverseFind('\\');
		if( pos>=0 )
		{
			path = path.Left(pos) + "\\Converted";
		}
		else
		{
			path = "Converted";
		}

		CreateFolder(path);

		for( int i=0; i<nsz; i++)
		{
			CString path1 = filePaths[i], path2;

			pos = path1.ReverseFind('\\');
			if( pos>=0 )
			{
				path2 = path + path1.Mid(pos);
			}
			else
			{
				path2 = (path + "\\") + path1;
			}

			CString strMsg;
			strMsg.Format(StrFromResID(IDS_PROCESSFILE),i+1,(LPCTSTR)path1);
			GOutPut(strMsg,TRUE);

			ConvertVectFile(path1,path2,&dlg.m_tm0,&dlg.m_tm1,TRUE);
		}
	}
}


void CEditBaseApp::OnToolConvertPts()
{
	CDlgConvertByPts dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	CArray<PT_3D,PT_3D> pts1, pts2;
	if( !ReadCtrlPts(dlg.m_strPath2,pts1) )
		return;

	if( !ReadCtrlPts(dlg.m_strPath3,pts2) )
		return;

	int nsz1 = pts1.GetSize(), nsz2 = pts2.GetSize();
	if( nsz1>nsz2 )nsz1 = nsz2;

	if( nsz1<=0 )
		return;

	double m[16];
	matrix_toIdentity(m,4);
	if( nsz1<4 )
	{
		m[3] = pts2[0].x - pts1[0].x;
		m[7] = pts2[0].y - pts1[0].y;
		m[11] = pts2[0].z - pts1[0].z;

		if( dlg.m_b2D )
			m[11] = 0;
	}
	else if( dlg.m_b2D )
	{
		double m2[6];

		double *buf = new double[nsz1*4];
		double *x1 = buf, *y1 = buf+nsz1;
		double *x2 = y1+nsz1, *y2 = y1+nsz1+nsz1;
		
		for( int i=0; i<nsz1; i++)
		{
			x1[i] = pts1[i].x; y1[i] = pts1[i].y;
			x2[i] = pts2[i].x; y2[i] = pts2[i].y;
		}
		
		CalcAffineParams(x1,y1,x2,y2,nsz1,m2,m2+3);
		
		m[0] = m2[0]; m[1] = m2[1]; m[3] = m2[2]; 
		m[4] = m2[3]; m[5] = m2[4]; m[7] = m2[5]; 
		
		delete[] buf;
	}
	else
	{
		double *buf = new double[nsz1*6];
		double *x1 = buf, *y1 = buf+nsz1, *z1 = buf+nsz1+nsz1;
		double *x2 = z1+nsz1, *y2 = z1+nsz1+nsz1, *z2 = z1+nsz1+nsz1+nsz1;

		for( int i=0; i<nsz1; i++)
		{
			x1[i] = pts1[i].x; y1[i] = pts1[i].y; z1[i] = pts1[i].z;
			x2[i] = pts2[i].x; y2[i] = pts2[i].y; z2[i] = pts2[i].z;
		}

		CalcAffineParams(x1,y1,z1,x2,y2,z2,nsz1,m,m+4,m+8);

		delete[] buf;
	}
	
	//����ȫͼ
	if( dlg.m_nRange==0 )
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( !pDoc )
			return;

		CUndoTransform undo(pDoc,StrFromResID(IDS_CONVERT_PTS));
		
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		ConvertDS(pDS,m,&undo,TRUE,NULL);
		undo.Commit();

		pDoc->BeginBatchUpdate();
		pDS->SaveAll(NULL);
		pDoc->EndBatchUpdate();

		pDS->SetModifiedFlag(TRUE);

		pDS->UpdateFtrQuery(NULL);

		pDoc->UpdateAllViews(NULL,hc_UpdateAllObjects);

	}
	//����ѡ��
	else if( dlg.m_nRange==1 )
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( !pDoc )
			return;

		int num = 0;
		
		const FTR_HANDLE *ftrs = pDoc->GetSelection()->GetSelectedObjs(num);
		CFtrArray arr;
		for( int i=0; i<num; i++)
		{
			arr.Add(HandleToFtr(ftrs[i]));
		}

		CUndoTransform undo(pDoc,StrFromResID(IDS_CONVERT_PTS));
		
		ConvertFtrs(arr,m,&undo,TRUE);

		undo.Commit();

		pDoc->BeginBatchUpdate();

		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		for( i=0; i<num; i++)
		{
			pDoc->DeleteObject(FtrToHandle(arr[i]));
			pDoc->RestoreObject(FtrToHandle(arr[i]));
		}
		
		pDoc->EndBatchUpdate();
		pDoc->DeselectAll();

		for( i=0; i<num; i++)
		{
			pDoc->GetSelection()->SelectObj(FtrToHandle(arr[i]));
		}

		pDoc->OnSelectChanged();

	}
	//�����ļ�
	else
	{
		CStringArray filePaths;
		filePaths.Copy(dlg.m_arrFileNames);
		
		int nsz = filePaths.GetSize();
		if( nsz<=0 )return;
		
		CString path = filePaths[0];
		int pos = path.ReverseFind('\\');
		if( pos>=0 )
		{
			path = path.Left(pos) + "\\Converted";
		}
		else
		{
			path = "Converted";
		}
		
		CreateFolder(path);
		
		for( int i=0; i<nsz; i++)
		{
			CString path1 = filePaths[i], path2;
			
			pos = path1.ReverseFind('\\');
			if( pos>=0 )
			{
				path2 = path + path1.Mid(pos);
			}
			else
			{
				path2 = (path + "\\") + path1;
			}

			CString strMsg;
			strMsg.Format(StrFromResID(IDS_PROCESSFILE),i+1,(LPCTSTR)path1);
			GOutPut(strMsg,TRUE);
			
			ConvertVectFile(path1,path2,m,TRUE);
		}
	}
}


void CEditBaseApp::OnToolConvertParams()
{
	CDlgDataTransform dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	double m1[16], m2[16], m[16];
	Matrix44FromMove(dlg.m_lfDX,dlg.m_lfDY,dlg.m_lfDZ,m1);
	Matrix44FromZoom(dlg.m_lfKX,dlg.m_lfKY,dlg.m_lfKZ,m2);

	matrix_multiply(m2,m1,4,m);
	
	//����ȫͼ
	if( dlg.m_nRange==0 )
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( !pDoc )
			return;
		
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();

		CUndoTransform undo(pDoc,StrFromResID(IDS_CONVERT_PTS));
		ConvertDS(pDS,m,&undo,TRUE,NULL);

		undo.Commit();

		pDoc->BeginBatchUpdate();
		pDS->SaveAll(NULL);
		pDoc->EndBatchUpdate();

		pDS->SetModifiedFlag(TRUE);

		pDS->UpdateFtrQuery(NULL);

		pDoc->UpdateAllViews(NULL,hc_UpdateAllObjects);

	}
	//����ѡ��
	else if( dlg.m_nRange==1 )
	{
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if( !pDoc )
			return;

		int num = 0;
		
		const FTR_HANDLE *ftrs = pDoc->GetSelection()->GetSelectedObjs(num);
		CFtrArray arr;
		for( int i=0; i<num; i++)
		{
			arr.Add(HandleToFtr(ftrs[i]));
		}
		
		CUndoTransform undo(pDoc,StrFromResID(IDS_CONVERT_PTS));
		ConvertFtrs(arr,m,&undo,TRUE);
		undo.Commit();

		pDoc->BeginBatchUpdate();
		
		CDlgDataSource *pDS = pDoc->GetDlgDataSource();
		for( i=0; i<num; i++)
		{
			pDoc->DeleteObject(FtrToHandle(arr[i]));
			pDoc->RestoreObject(FtrToHandle(arr[i]));
		}
		
		pDoc->EndBatchUpdate();
		pDoc->DeselectAll();

		for( i=0; i<num; i++)
		{
			pDoc->GetSelection()->SelectObj(FtrToHandle(arr[i]));
		}

		pDoc->OnSelectChanged();

	}
	//�����ļ�
	else
	{
		CStringArray filePaths;
		filePaths.Copy(dlg.m_arrFileNames);
		
		int nsz = filePaths.GetSize();
		if( nsz<=0 )return;
		
		CString path = filePaths[0];
		int pos = path.ReverseFind('\\');
		if( pos>=0 )
		{
			path = path.Left(pos) + "\\Converted";
		}
		else
		{
			path = "Converted";
		}
		
		CreateFolder(path);
		
		for( int i=0; i<nsz; i++)
		{
			CString path1 = filePaths[i], path2;
			
			pos = path1.ReverseFind('\\');
			if( pos>=0 )
			{
				path2 = path + path1.Mid(pos);
			}
			else
			{
				path2 = (path + "\\") + path1;
			}

			CString strMsg;
			strMsg.Format(StrFromResID(IDS_PROCESSFILE),i+1,(LPCTSTR)path1);
			GOutPut(strMsg,TRUE);
			
			ConvertVectFile(path1,path2,m,TRUE);
		}
	}	
}

extern BOOL CreateFdbFromCurSchemAndTemplate(LPCTSTR newFdb, LPCTSTR templFdb);

void CEditBaseApp::OnUpdateFDBForScheme()
{
	//����
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc)
		return;

	if (!CheckLicense(1))
	{
		GOutPut(StrFromResID(IDS_ERR_INVALIDLICENSE));
		return;
	}

	//uvs��֧���������ݵĸ��£����������ڷ������˸��·���
	CDataSourceEx *pDS = pDoc->GetDlgDataSource();
	if (!pDS || UVSModify==GetAccessType(pDS) )
	{
		AfxMessageBox(IDS_UVS_NOT_UPDATE);
		return;
	}

	POSITION pos = pDoc->GetFirstViewPosition();
	CView *pView = pDoc->GetNextView(pos);
	if(!pView)
		return;

	WINDOWPLACEMENT wndPlace;
	pView->GetParentFrame()->GetWindowPlacement(&wndPlace);

	float zoomRate = 1.0;
	PT_3D centerPoint;
	BOOL bDriveXYZ = FALSE;
	if(pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
	{
		CVectorView_new *pVV = (CVectorView_new*)pView;
		zoomRate = pVV->GetZoomRate();
		
		CRect rcClient;
		pVV->GetClientRect(rcClient);
		pVV->ClientToGround(&PT_4D(rcClient.CenterPoint().x,rcClient.CenterPoint().y,rcClient.CenterPoint().x,rcClient.CenterPoint().y),&centerPoint);
		centerPoint.z = pVV->GetCoordWnd().m_ptGrd.z;
		bDriveXYZ = TRUE;
	}
	
	pView->SendMessage(WM_COMMAND,ID_FILE_SAVE);

	//��ȡ�ĵ���
	CString fdbName = pDoc->GetPathName();

	//�ر��ĵ�
	pDoc->m_bPromptSave = TRUE;
	pView->GetParent()->SendMessage(WM_CLOSE);

	//������
	CString oldFdbName = fdbName.Left(fdbName.GetLength()-4) + "_beforeUpdateScheme.fdb";
	::DeleteFile(oldFdbName);
	::MoveFile(fdbName,oldFdbName);

	//�½�FDB�ļ�
	if(!CreateFdbFromCurSchemAndTemplate(fdbName,oldFdbName))
		return;
	
	//����ԭ�ļ�������
	POSITION curTemplatePos = GetFirstDocTemplatePosition();
	
	while(curTemplatePos!=NULL)
	{
		CDocTemplate* curTemplate = GetNextDocTemplate(curTemplatePos);
		CDocument *rpDocMatch;
		if (curTemplate->MatchDocType( _T(".fdb"), rpDocMatch )==CDocTemplate::yesAttemptNative)
		{
			curTemplate->OpenDocumentFile(fdbName);
			break;
		}
	}

	pDoc = GetActiveDlgDoc();
	if(!pDoc)
		return;

	pDoc->ImportFDB(oldFdbName);

	pos = pDoc->GetFirstViewPosition();
	pView = pDoc->GetNextView(pos);
	if(!pView)
		return;

	//����
	pView->SendMessage(WM_COMMAND,ID_FILE_SAVE);
	pDoc->DeleteUndoItems();

	//�ָ���ʾ������λ��
	pView->GetParentFrame()->SetWindowPlacement(&wndPlace);

	if(bDriveXYZ && pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
	{
		CVectorView_new *pVV = (CVectorView_new*)pView;
		pVV->ZoomChange(CPoint(),zoomRate,FALSE);
		pVV->DriveToXyz(&centerPoint,TRUE);
	}
}


//����UVS��
void CEditBaseApp::OnConnectDB()
{
	CDlgConnectDB dlg;
	dlg.DoModal();
}


void CEditBaseApp::OnBatchExportDxf() //��������dxf
{
	CDxfWrite dxfWrite; 
	dxfWrite.OnExportDxf_Batch();
}

void CEditBaseApp::OnBatchExportArcgismdb()
{
	CDlgExportArcgisMdb_Batch dlg;
	dlg.DoModal();
}

void CEditBaseApp::OnToolUVSBrowser()
{
	if (!CheckLicense(86))
	{
		GOutPut(StrFromResID(IDS_ERR_INVALIDLICENSE));
		return;
	}

	char szPath[_MAX_PATH];
	GetModuleFileName(NULL, szPath, _MAX_PATH);
	CString strModuleDir = szPath;
	strModuleDir = strModuleDir.Left(strModuleDir.ReverseFind('\\'));

	CString strCmdLine = strModuleDir + "\\uvsbrowser.exe";
	WinExec(strCmdLine, SW_SHOW);
}

CSwitchScale::CSwitchScale(int iScale)
{
	m_CellLib = GetCellDefLib();
	m_BaseLib = GetBaseLineTypeLib();

	CConfigLibManager *pCfg = gpCfgLibMan;
	if( pCfg && iScale!=0)
	{
		ConfigLibItem config = pCfg->GetConfigLibItemByScale(iScale);
		SetCellDefLib(config.pCellDefLib);
		SetBaseLineTypeLib(config.pLinetypeLib);
	}
}

void CSwitchScale::SwitchToScale(int iScale)
{
	if (m_CellLib == NULL || m_BaseLib == NULL)
	{
		m_CellLib = GetCellDefLib();
		m_BaseLib = GetBaseLineTypeLib();
	}
	CConfigLibManager *pCfg = gpCfgLibMan;
	if( pCfg )
	{
		ConfigLibItem config = pCfg->GetConfigLibItemByScale(iScale);
		SetCellDefLib(config.pCellDefLib);
		SetBaseLineTypeLib(config.pLinetypeLib);
	}
}

CSwitchScale::~CSwitchScale()
{	
	SetCellDefLib(m_CellLib);
	SetBaseLineTypeLib(m_BaseLib);
}


BOOL CEditBaseApp::LoadState(LPCTSTR lpszSectionName, CFrameImpl* pFrameImpl)
{
	// TODO: Add your specialized code here and/or call the base class

	return false;
}


void CEditBaseApp::LoadCustomState()
{
	// TODO: Add your specialized code here and/or call the base class
	this->m_bSaveState = FALSE;
}
