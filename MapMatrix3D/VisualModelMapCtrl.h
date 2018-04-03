#if !defined(AFX_VISUALMODELMAPCTRL_H__1D830052_E9B5_4B24_90F7_EF73ADB98E44__INCLUDED_)
#define AFX_VISUALMODELMAPCTRL_H__1D830052_E9B5_4B24_90F7_EF73ADB98E44__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VisualModelMapCtrl.h : header file
//
#include "GeoBuilderPrj2.h "
#include "DrawGraphStatic.h "

#include "../mm3dPrj/MyEditTip.h"
/////////////////////////////////////////////////////////////////////////////
// CVisualModelMapCtrl window

//立体范围文件
class CStereoBoundFile
{
public:
	CStereoBoundFile();
	~CStereoBoundFile();
	
	BOOL Load(LPCTSTR prj_path);
	BOOL Find(LPCTSTR stereoID, PT_3D pts_ret[4]);
	BOOL Add(LPCTSTR stereoID, PT_3D pts[4]);
	BOOL Save();
	
public:
	BOOL m_bLoadOK;
	CString m_strPath;

	CStringArray m_arrStereos;
	CArray<PT_3D,PT_3D> m_arrBounds;
};

class CVisualModelMapCtrl : public CWnd
{
	// Construction
public:
	CVisualModelMapCtrl();
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVisualModelMapCtrl)
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	void SetCurrentMS(int index);
	void ClearMSArray();
	LRESULT OnRefreshVecFlag(WPARAM wParam, LPARAM lParam);
	LRESULT OnGetToler(WPARAM wParam, LPARAM lParam);
	void SetMsArrayFlag(bool bIsExist);
	void SetVecWinFlag(bool bIsExist);
	void SetCurStereoWinFlag(bool bIsExist);
	void SetCurMSFlag(bool bIsExist);
	LRESULT OnRefreshStereoWin(WPARAM wParam, LPARAM lParam);
	void InitialDisplay();
	void SetCurrentVecWin(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);
	void SetCurrentStereoWin(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);
	void SetCurrentMS(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);
	void SetNearestMS(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);	
	void AddModelScope(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4,int index);
	LRESULT OnUnInstallProj(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshStereoMS(WPARAM wParam,LPARAM lParam);
	LRESULT OnRefreshVecWin(WPARAM wParam,LPARAM lParam);
	LRESULT OnSwitchStereoMS(WPARAM wParam,LPARAM lParam);
	LRESULT OnTestSwitchModel(WPARAM wParam, LPARAM lParam);
	void AdjustLayout();
	void SetModelScopeArray( CoreObject *pCoreObj, BOOL bReadBoundFile=TRUE);
	virtual ~CVisualModelMapCtrl();

	/**
	* @brief getGraphStatic 获得航片叠加的显示的static
	*/
	inline CDrawGraphStatic* getGraphStatic()
	{
		return &m_staticDrawGraph;
	}

	// Generated message map functions
protected:
	
	//{{AFX_MSG(CVisualModelMapCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_bIsMove;
	//	bool m_bIsDisplayForStart;
	CFont m_font;
	CDrawGraphStatic m_staticDrawGraph;
public:	
	void SetNearstMSFlag(bool bIsExist);
	void SetMoveFlag(bool flag);

	/*
	** brief 我自己定义的tip
	*/
	CMyEditTip m_editTipInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VISUALMODELMAPCTRL_H__1D830052_E9B5_4B24_90F7_EF73ADB98E44__INCLUDED_)
