// EditBase.h : main header file for the EDITBASE application
//

#if !defined(AFX_EDITBASE_H__5DCE265A_0982_4669_B5C1_D65DAAA065A2__INCLUDED_)
#define AFX_EDITBASE_H__5DCE265A_0982_4669_B5C1_D65DAAA065A2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "res\resource.h"       // main symbols
#include "Symbol.h"
#include "SymbolLib.h"
#include "PlugBase.h"
#include "AFXADV.H"
#include "UIFFileDialogEx.h"
#include "UIFSimpleEx.h"
#include "License.h"

extern UINT FCCM_CALL_HELP;

#define PLUGIN_ITEMCMDID_BEGIN    61000
#define PLUGIN_ITEMCMDID_END      61999
#define PLUGIN_ITEMPROCESSID_BEGIN    62000
#define PLUGIN_ITEMPROCESSID_END      62999

#define PLUGIN_COLLECTCMDID_BEGIN    61000
#define PLUGIN_COLLECTCMDID_END		 61250
#define PLUGIN_EDITCMDID_BEGIN		 61251
#define PLUGIN_EDITCMDID_END		 61500

#define PLUGIN_SECTION          _T("Plugin")

#define	FTRCOLOR_BYLAYER	-1


/////////////////////////////////////////////////////////////////////////////
// CEditBaseApp:
// See EditBase.cpp for the implementation of this class
//
typedef struct tag_PlugItem
{
	tag_PlugItem()
	{
		bUsed = FALSE;
	}
	CPlugBase * pObj;
	UINT *itemPlugID;  // 对应插件中命令ID
	UINT *itemID;      // 对应应用程序ID
	HINSTANCE hIns;
	bool bUsed;
}PlugItem, * LPPlugItem;

class CEditBaseApp : public CWinAppEx
{
protected:
	CConfigLibManager *m_pCfgLibManager;
	HMODULE m_hChsDll;
public:
	CEditBaseApp();
	~CEditBaseApp();

	CConfigLibManager *GetConfigLibManager()const;
	const PlugItem * GetPlugins(int &nCount);
	inline BOOL IsSupportPlugs(){return m_bSupportPlugs;}
	virtual void PreLoadState();
	void UpdateAllDocsByCmd(UINT nCmdID);
	void ReloadConfig();
	void OpenFileOnUVS(LPCTSTR mapname);

	HANDLE m_hAppMutex;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditBaseApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEditBaseApp)
	afx_msg void OnAppAbout();
	afx_msg void OnToolSymbolmanage();
	afx_msg void OnLoadProject();
	afx_msg void OnConnectDB();
	afx_msg void OnUninstallProject();
	afx_msg void OnFileNew();
	afx_msg void OnFileNewDB();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenDB();
	afx_msg void OnReorientation();
	afx_msg void OnSaveKeys();
	afx_msg void OnLoadKeys();
	//afx_msg void OnSaveLayout();
	//afx_msg void OnLoadLayout();
	afx_msg void OnMakeCheckPtSampleBatch();
	afx_msg void OnPopupModifyMousePoint();
	afx_msg void OnCompactData();
	afx_msg void OnRepairData();
	afx_msg void OnExportCodeTable();
	afx_msg void OnImportCodeTable();
	afx_msg void OnExportCodeTableTemp();
	afx_msg void OnToolConvertPts();
	afx_msg void OnToolConvertCoordSys();
	afx_msg void OnToolConvertParams();
	afx_msg void OnBatchExportDxf();
	afx_msg void OnUpdateFDBForScheme();
	afx_msg void OnBatchExportArcgismdb();
	afx_msg void OnToolUVSBrowser();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void RegisterFileAssociation();
	BOOL m_bSupportPlugs;
private:
	BOOL LoadPlugs();
	BOOL CreatePlug(LPCTSTR fileName);
	void FreePlugs();
	BOOL AlreadyRunning();

public:
	CArray<PlugItem,PlugItem> m_arrPlugObjs;
	virtual BOOL LoadState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);
	virtual void LoadCustomState();
};

extern CEditBaseApp theApp;

#define gpCfgLibMan	(((CEditBaseApp*)AfxGetApp())->GetConfigLibManager())
#define gpMainWnd	((CMainFrame*)AfxGetMainWnd())

CString GetConfigPath(BOOL bConfigRootFolder = FALSE);


class CSwitchScale
{
public:
	CSwitchScale(int iScale=0);
	void SwitchToScale(int iScale);
	~CSwitchScale();

private:
	CCellDefLib *m_CellLib;
	CBaseLineTypeLib *m_BaseLib;

};

#define BeginCheck42License  if(CheckLicense(66)){
#define EndCheck42License    }

#define BeginCheck50License  if(CheckLicense(73)){
#define EndCheck50License    }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITBASE_H__5DCE265A_0982_4669_B5C1_D65DAAA065A2__INCLUDED_)
