// ProjectViewBar.h: interface for the CProjectViewBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTVIEWBAR_H__22EC624A_1E2C_45BF_ACBD_6068CFE688F5__INCLUDED_)
#define AFX_PROJECTVIEWBAR_H__22EC624A_1E2C_45BF_ACBD_6068CFE688F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GeometryViewBar.h"
#include "GeoBuilderPrj2.h  "
#include "VisualModelMapCtrl.h "
#include "../mm3dPrj/MyTreeCtrl.h"

class CProjectViewBar : public CDockablePane  
{
public:	
	LRESULT GetToler(WPARAM wParam, LPARAM lParam);
	LRESULT SwitchStereoMS(WPARAM wParam, LPARAM lParam);
	LRESULT RefreshVecflag(WPARAM wParam, LPARAM lParam);
	LRESULT RefreshCurrentStereoMS(WPARAM wParam, LPARAM lParam);
	LRESULT RefreshCurrentStereoWin(WPARAM wParam, LPARAM lParam);
	LRESULT RefreshVecWin(WPARAM wParam, LPARAM lParam);
	CProjectViewBar();
	virtual ~CProjectViewBar();
	void OnOpenDocimage(int index);
	//##ModelId=41466B7F00DA
	void AdjustLayout ();
	LRESULT UnInstall(WPARAM wParam,LPARAM lParam);
	BOOL LoadProject(LPCTSTR fileName);
	
	LONG OnCallLoad(WPARAM wParam,LPARAM lParam);
	LONG OnGetProject(WPARAM wParam,LPARAM lParam);
	CMyTreeCtrl			m_wndAttribView;
	CString GetCurPrjPath(){ return m_prjname; }

	/**
	* @brief getModelMapCtrl 获得模型地图控件
	*/
	inline CVisualModelMapCtrl* getModelMapCtrl()
	{
		return &m_mapctlModel;
	}

protected:
	//##ModelId=41466B7F00DC
	CClassToolBar		m_wndToolBar;
	//##ModelId=41466B7F00EA
	
	//##ModelId=41466B7F00EF
	CImageList			m_AttribViewImages;
	
	CGeoBuilderPrj		m_project;
	CString m_prjname;//当前打开的工程xml路径
	
	CVisualModelMapCtrl m_mapctlModel;
	CMruFile	m_CMruFile;		//最近文件菜单对象
	CString		m_exePath;		//模块当前路径
	CString      m_strUVSPath;
private:
	bool m_bShowMapInfo;
	BOOL CreateToolBar(UINT id);
	//##ModelId=41466B7F0108
	HTREEITEM InsertRoot(LPCTSTR lpszItem,DWORD_PTR dwData=0);
	HTREEITEM InsertSubRoot(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData = 0);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData = 0);
	
	void SetItemImage(HTREEITEM item, DWORD_PTR flag);
	
	void OpenDocFile(LPCTSTR name);
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectViewBar)
public:
	//##ModelId=41466B7F0109
	//}}AFX_VIRTUAL
	
	// Generated message map functions
protected:
	
	//{{AFX_MSG(CProjectViewBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLoadFile();
	afx_msg void OnOpenLocalFile();
	afx_msg void OnCommitLocalFile();
	afx_msg void OnLoadOther();
	afx_msg void OnUnloadOther();
	afx_msg void OnLoadPhotoStereo();
	afx_msg void OnLoadRealtimeStereo();
	afx_msg void OnLoadProject();
	afx_msg void OnUnloadProject();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHideMapinfo();
	afx_msg void OnPopupSwitchepip();
	afx_msg void OnOpenDocimage1();
	afx_msg void OnOpenDocimage2();
	afx_msg void OnOpenDocimage3();
	afx_msg void OnOpenDocimage4();
	afx_msg void OnOpenDocimage5();
	afx_msg void OnOpenDocimage6();
	afx_msg void OnOpenDocimage7();
	afx_msg void OnOpenDocimage8();
	afx_msg void OnOpenDocimage9();
	afx_msg void OnOpenDocimage10();
	afx_msg void OnOpenDocimage11();
	afx_msg void OnOpenDocimage12();
	afx_msg void OnOpenDocimage13();
	afx_msg void OnOpenDocimage14();
	afx_msg void OnOpenDocimage15();
	afx_msg void OnOpenDocimage16();
	afx_msg void OnMruClr();
	afx_msg void OnUpdateOpenEpip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpenPhoto(CCmdUI* pCmdUI);
	afx_msg void OnReCalcModelBound();
	//}}AFX_MSG
	//##ModelId=41466B7F0187
	afx_msg LRESULT OnChangeActiveTab (WPARAM, LPARAM);
	//##ModelId=41466B7F0196
	//##ModelId=41466B7F0199
	//by shy
	afx_msg void OnDestroy( );
	DECLARE_MESSAGE_MAP()


};

#endif // !defined(AFX_PROJECTVIEWBAR_H__22EC624A_1E2C_45BF_ACBD_6068CFE688F5__INCLUDED_)
