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
	//ͼֽ����
	int m_nHeight;
	int m_nWidth;
	int m_nSpaceTop;
	int	m_nSpaceBottom;
	int m_nSpaceLeft;
	int m_nSpaceRight;
	int m_nEStart;
	int	m_nNStart;
	//����п�
	int	m_nWidthCode;//�п�-����
	int m_nWidthLayerName;//�п�-����
	int m_nWidthSymbolMode;//�п�-��������
	int	m_nWidthSymbol;//�п�-����
	int m_nSymbolDiscribtion;//�п�-����˵��
	//������
	int m_nTableNumber;//�����
	int m_nSpace;//���
	int m_nRowWidth;//�и�
	
	//���뷶Χ
	__int64 m_nCodeBoundLow;//��Χ-��
	__int64 m_nCodeBoundHigh;//��Χ-��
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
