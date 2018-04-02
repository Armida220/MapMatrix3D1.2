#if !defined(AFX_ACCUDLG_H__B92CA941_D6C2_4E02_B87C_8DFF2BFE161D__INCLUDED_)
#define AFX_ACCUDLG_H__B92CA941_D6C2_4E02_B87C_8DFF2BFE161D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccuDlg.h : header file
//
#include "AccuBox.h"
/////////////////////////////////////////////////////////////////////////////
// CAccuDlg dialog

class CReflectKeyDialog : public CDialog
{
public:
	CReflectKeyDialog(UINT id, CWnd* pParent = NULL)
		: CDialog(id, pParent){}
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	//{{AFX_MSG(CReflectKeyDialog)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/*
class CAccuDlg : public CReflectKeyDialog
{
public:
	// Dialog Data
	//{{AFX_DATA(CAccuDlg)
	enum { IDD = IDD_ACCU_DLG };
	CStatic	m_stcStatus;
	CMFCButton	m_btnAdvance;
	float	m_lfX;
	float	m_lfY;
	float	m_lfZ;
	float	m_lfDis;
	float	m_lfAng;
	BOOL	m_bUnlockFirstLine;
	BOOL	m_bLockX;
	BOOL	m_bLockY;
	BOOL	m_bLockZ;
	BOOL	m_bLockDis;
	BOOL	m_bLockAng;
	BOOL	m_bAutoReset;
	BOOL	m_bViewDraw;
	BOOL    m_bRightModeWithSnapPt;
	UINT	m_nTolerance;
	int		m_nDir;
	//}}AFX_DATA
	
	int		m_nStatus;
	BOOL	m_bLockingDir;
	BOOL	m_bUseDir;
	
private:
	int m_nCoordType;
	
	CRect m_rc1, m_rc2;
	BOOL  m_bPopup;
	BOOL  m_bPrivateMove;
	
	// Construction
public:
	CAccuDlg(CWnd* pParent = NULL);   // standard constructor
	
	void Show(int code, long param);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccuDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
private:
	BOOL ChangeFace(UINT id);
	void UpdateAccuData();
	void UpdateAdvanceCtrls();
	void UpdateStatusString();
	void PaintLockDirBtn();
	
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CAccuDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnCheckAccuxLock();
	afx_msg void OnCheckAccuyLock();
	afx_msg void OnCheckAccuzLock();
	afx_msg void OnCheckAccudisLock();
	afx_msg void OnCheckAccuangLock();
	afx_msg void OnCheckAccuViewDraw();
	afx_msg void OnCheckAccuAutoReset();
	afx_msg void OnCheckAccuUnlockFirstLine();
	afx_msg void OnChangeEditAccux();
	afx_msg void OnChangeEditAccuy();
	afx_msg void OnChangeEditAccuz();
	afx_msg void OnChangeEditAccudis();
	afx_msg void OnChangeEditAccuang();
	afx_msg void OnAdvance();
	afx_msg void OnChangeEditAccuTolerance();
	afx_msg void OnLockDir();
	afx_msg void OnUnlockDir();
	afx_msg void OnSelchangeDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
*/


class CAccuDlg : public CReflectKeyDialog
{
public:
	// Dialog Data
	//{{AFX_DATA(CAccuDlg)
	enum { IDD = IDD_ACCU_DLG3 };	
	float	m_lfX;
	float	m_lfY;
	float	m_lfDis;
	float	m_lfAng;
	BOOL	m_bLockX;
	BOOL	m_bLockY;
	BOOL	m_bLockDis;
	BOOL	m_bLockAng;
	CStatic	m_stcStatus;
	BOOL	m_bRelativeCoord;

	BOOL	m_bExtension;
	BOOL	m_bParallel;
	BOOL	m_bPerpendicular;
	//}}AFX_DATA
	
	int		m_nDir;
	int		m_nStatus;
	BOOL	m_bLockingDir;	
	// Construction
public:
	CAccuDlg(CWnd* pParent = NULL);   // standard constructor	
	void Show(WPARAM code, LPARAM param);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccuDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
private:
	void UpdateAccuData();
	void UpdateStatusString();
	
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CAccuDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

	afx_msg void OnCheckAccuxLock();
	afx_msg void OnCheckAccuyLock();
	afx_msg void OnCheckAccudisLock();
	afx_msg void OnCheckAccuangLock();
	afx_msg void OnChangeEditAccux();
	afx_msg void OnChangeEditAccuy();
	afx_msg void OnChangeEditAccudis();
	afx_msg void OnChangeEditAccuang();
	afx_msg void OnCheckCoordSys();

	afx_msg void OnCheckExtension();
	afx_msg void OnCheckParallel();
	afx_msg void OnCheckPerpendicular();
	afx_msg void OnUnlockDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_ACCUDLG_H__B92CA941_D6C2_4E02_B87C_8DFF2BFE161D__INCLUDED_)
