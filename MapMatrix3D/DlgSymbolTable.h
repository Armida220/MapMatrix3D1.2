#if !defined(AFX_DLGSYMBOLTABLE_H__20495F3F_9799_40B8_B293_45A4B33C908D__INCLUDED_)
#define AFX_DLGSYMBOLTABLE_H__20495F3F_9799_40B8_B293_45A4B33C908D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSymbolTable.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSymbolTable dialog

class CDlgSymbolTable : public CDialog
{
// Construction
public:
	CDlgSymbolTable(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgSymbolTable)
	enum { IDD = IDD_SYMBOLTABLEINPUT };
		// NOTE: the ClassWizard will add data members here
	//图纸参数
	int m_nHeight;
	int m_nWidth;
	int m_nSpaceTop;
	int	m_nSpaceBottom;
	int m_nSpaceLeft;
	int m_nSpaceRight;
	int m_nEStart;
	int	m_nNStart;
	//表格列宽
	int	m_nWidthCode;//列宽-编码
	int m_nWidthLayerName;//列宽-层名
	int m_nWidthSymbolMode;//列宽-符号类型
	int	m_nWidthSymbol;//列宽-符号
	int m_nSymbolDiscribtion;//列宽-符号说明
	//表格参数
	int m_nTableNumber;//表格数
	int m_nSpace;//间隔
	int m_nRowWidth;//行高
	
	//编码范围
	__int64 m_nCodeBoundLow;//范围-低
	__int64 m_nCodeBoundHigh;//范围-高
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSymbolTable)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL DataCheck();
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgSymbolTable)
		// NOTE: the ClassWizard will add member functions here

	virtual BOOL OnInitDialog();
	afx_msg void OnRestoreDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg void On_OK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSYMBOLTABLE_H__20495F3F_9799_40B8_B293_45A4B33C908D__INCLUDED_)
