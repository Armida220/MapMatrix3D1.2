#if !defined(AFX_DLGOVERMUTIIMGLAY_H__7F19702C_02A7_4307_A727_ED108FF647C8__INCLUDED_)
#define AFX_DLGOVERMUTIIMGLAY_H__7F19702C_02A7_4307_A727_ED108FF647C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ViewImgPosition.h"

// DlgOverMutiImgLay.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgMutiList dialog

class CDlgMutiList : public CDialog
{
	// Construction
public:
	CDlgMutiList(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgMutiList)
	enum { IDD = IDD_MUTILIST };
	CListBox	m_wndList;
	//}}AFX_DATA
	CStringArray m_strList;
	CStringArray m_strSelItems;
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgMutiList)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgMutiList)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CCustomizeListBox:public CListBox
{
public:
	CCustomizeListBox();
	virtual ~CCustomizeListBox();	
	
	void SetMainItem(int index = 0);
	int GetMainItem();
	int DeleteString(UINT nIndex);
protected:
	virtual int  CompareItem (LPCOMPAREITEMSTRUCT lpCIS);
	virtual void DrawItem (LPDRAWITEMSTRUCT lpDIS);
	virtual void MeasureItem (LPMEASUREITEMSTRUCT lpMIS);
	DECLARE_MESSAGE_MAP()
private:
	int m_nMainIdx;
	
	
};
/////////////////////////////////////////////////////////////////////////////
// CDlgOverMutiImgLay dialog

class CDlgOverMutiImgLay : public CDialog
{

// Construction
public:
	CDlgOverMutiImgLay(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgOverMutiImgLay)
	enum { IDD = IDD_OVERLAY_IMG };
	CCustomizeListBox	m_listMutiImg;
	BOOL	m_bVisible;
	double	m_lfMatrix1;
	double	m_lfMatrix2;
	double	m_lfMatrix3;
	double	m_lfMatrix4;
	double	m_lfXOff;
	double	m_lfYOff;
	int		m_nPixelBase;
	//}}AFX_DATA
	PT_3D m_ptsBound[4];
	double m_lfPixelSizeX, m_lfPixelSizeY;
	int m_nMainImageIdx;
	CArray<ViewImgPosition,ViewImgPosition> m_arrImgPos;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgOverMutiImgLay)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void Init()
	{
       OnInitDialog();
	}
    void AddImageFromAny()
	{
       OnButtonIdBrowseAny();
	   OnOK();
	}
	void AddImageFromWorkspace()
	{
       OnButtonBrowseWorkspace();
	   OnOK();
	}
private:
	void RefreshUIWithOption(LPCTSTR fileName = NULL);
	void RefreshUIForSelChange(LPCTSTR fileName);
	void SaveImgPos(LPCTSTR fileName = NULL);
	static CString m_curSelFileName;
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgOverMutiImgLay)
	afx_msg void OnButtonBrowseWorkspace();
	afx_msg void OnButtonIdBrowseAny();
	afx_msg void OnButtonUnload();
	afx_msg void OnButtonMapbound();
	afx_msg void OnSelchangeListMutiimgFilename();
	afx_msg void OnButtonRecoverOriginalParameters();
	afx_msg void OnButtonSetMainimg();
	afx_msg void OnButtonVisible();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGOVERMUTIIMGLAY_H__7F19702C_02A7_4307_A727_ED108FF647C8__INCLUDED_)
