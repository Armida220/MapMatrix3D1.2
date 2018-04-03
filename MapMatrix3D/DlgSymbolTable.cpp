// DlgSymbolTable.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgSymbolTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSymbolTable dialog
static BOOL bInit = FALSE;
static int nHeight = 0;
static int nWidth = 0;
static int nSpaceTop = 0;
static int nSpaceBottom = 0;
static int nSpaceLeft = 0;
static int nSpaceRight = 0;
static int nNStart = 0;
static int nEStart = 0;
static int nWidthCode = 0;
static int nWidthLayerName = 0;
static int nWidthSymbolMode = 0;
static int nWidthSymbol = 0;
static int nSymbolDiscribtion = 0;
static int nTableNumber = 0;
static int nSpace = 0;
static int nRowWidth = 0;
static UINT nCodeBoundLow = 0;
static UINT nCodeBoundHigh = 0;


CDlgSymbolTable::CDlgSymbolTable(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSymbolTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSymbolTable)
		// NOTE: the ClassWizard will add member initialization here
	if (!bInit)
	{
		bInit = TRUE;
		nHeight		= 332;
		nWidth		= 210;
		nSpaceTop		= 18;
		nSpaceBottom	= 0;
		nSpaceLeft	= 20;
		nSpaceRight	= 0;
		nEStart		= 100;
		nNStart		= 10000;
	
		nWidthCode	=25;
		nWidthLayerName = 55;
		nWidthSymbolMode = 20;
		nWidthSymbol	 = 20;
		nSymbolDiscribtion = 40;

		nTableNumber = 2;
		nSpace = 5;
		nRowWidth = 10;
		nCodeBoundLow = 100;
		nCodeBoundHigh = 1000000000;
	}
	m_nHeight		= nHeight;
	m_nWidth		= nWidth;
	m_nSpaceTop		= nSpaceTop;
	m_nSpaceBottom	= nSpaceBottom;
	m_nSpaceLeft	= nSpaceLeft;
	m_nSpaceRight	= nSpaceRight;
	m_nEStart		= nEStart;
	m_nNStart		= nNStart;
	m_nWidthCode	=nWidthCode;
	m_nWidthLayerName = nWidthLayerName;
	m_nWidthSymbolMode = nWidthSymbolMode;
	m_nWidthSymbol	 = nWidthSymbol;
	m_nSymbolDiscribtion = nSymbolDiscribtion;
	m_nTableNumber = nTableNumber;
	m_nSpace = nSpace;
	m_nRowWidth = nRowWidth;
	m_nCodeBoundLow = nCodeBoundLow;
	m_nCodeBoundHigh = nCodeBoundHigh;
}


void DDX_Text_64(CDataExchange* pDX, int nIDC, __int64& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = _atoi64(text);
		}
	}
	else
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			text.Format("%I64d",value);
			pWnd->SetWindowText(text);
		}
	}
}

void CDlgSymbolTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSymbolTable)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_EDIT_HEIGH, m_nHeight);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_nWidth);
	DDX_Text(pDX, IDC_EDIT_TOP, m_nSpaceTop);
	DDX_Text(pDX, IDC_EDIT_BOTTOM, m_nSpaceBottom);
	DDX_Text(pDX, IDC_EDIT_LEFT, m_nSpaceLeft);
	DDX_Text(pDX, IDC_EDIT_RIGHT, m_nSpaceRight);
	DDX_Text(pDX, IDC_EDIT_ESTART, m_nEStart);
	DDX_Text(pDX, IDC_EDIT_NSTART, m_nNStart);

	DDX_Text(pDX, IDC_EDIT_CODE, m_nWidthCode);
	DDX_Text(pDX, IDC_EDIT_LAYERNAME, m_nWidthLayerName);
	DDX_Text(pDX, IDC_EDIT_SYMBOLMODE, m_nWidthSymbolMode);
	DDX_Text(pDX, IDC_EDIT_SYMBOL, m_nWidthSymbol);
	DDX_Text(pDX, IDC_EDIT_SYMBOLDISCRIBE, m_nSymbolDiscribtion);

	DDX_Text(pDX, IDC_EDIT_TABLENUMBER, m_nTableNumber);
	DDX_Text(pDX, IDC_EDIT_SPACE, m_nSpace);
	DDX_Text(pDX, IDC_EDIT_ROWHEIGHT, m_nRowWidth);

	DDX_Text_64(pDX, IDC_EDIT_CODEBOUND_LOW, m_nCodeBoundLow);
	DDX_Text_64(pDX, IDC_EDIT_CODEBOUND_HEIGH, m_nCodeBoundHigh);	

	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDlgSymbolTable, CDialog)
	//{{AFX_MSG_MAP(CDlgSymbolTable)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDB_OK, On_OK)
	ON_BN_CLICKED(IDB_RESTORE_DEFAULT, OnRestoreDefault)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSymbolTable message handlers


BOOL CDlgSymbolTable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE


}

void CDlgSymbolTable::On_OK()
{
	if (!DataCheck())
	{
	//	AfxMessageBox(StrFromResID(IDS_SYMBOLTABLE_ERROR));
		return;
	}
	nHeight		= m_nHeight;
	nWidth		= m_nWidth;
	nSpaceTop	= m_nSpaceTop;
	nSpaceBottom = m_nSpaceBottom;
	nSpaceLeft	= m_nSpaceLeft;
	nSpaceRight	= m_nSpaceRight;
	nEStart		= m_nEStart;
	nNStart		= m_nNStart;
	nWidthCode	=  m_nWidthCode;
	nWidthLayerName = m_nWidthLayerName;
	nWidthSymbolMode = m_nWidthSymbolMode;
	nWidthSymbol	 = m_nWidthSymbol;
	nSymbolDiscribtion = m_nSymbolDiscribtion;
	nTableNumber = m_nTableNumber;
	nSpace = m_nSpace;
	nRowWidth = m_nRowWidth;
	nCodeBoundLow = m_nCodeBoundLow;
	nCodeBoundHigh = m_nCodeBoundHigh;
	CDialog::OnOK();
}

BOOL CDlgSymbolTable::DataCheck()
{
	UpdateData(TRUE);
	if (m_nCodeBoundLow<0 || m_nCodeBoundLow>m_nCodeBoundHigh || m_nCodeBoundHigh<0)
	{
		AfxMessageBox(StrFromResID(IDS_CDOEBOUND_CHECK));
		return FALSE;
	}

	if (m_nTableNumber<=0 || m_nSpace<=0 || m_nRowWidth<=0)
	{
		AfxMessageBox(StrFromResID(IDS_TABLEPARAM_NOTE));
		return FALSE;
	}

	if (m_nWidthCode<=0 || m_nWidthLayerName<=0 || m_nWidthSymbol<=0 || m_nSymbolDiscribtion<=0 || m_nWidthSymbolMode<=0)
	{
		AfxMessageBox(StrFromResID(IDS_TABLECOLUNM_CHECK));
		return	FALSE;
	}
	
	if(m_nWidth<0 || m_nHeight<0 || m_nSpaceTop<0 || m_nSpaceBottom<0 || m_nSpaceLeft<0 ||m_nSpaceRight<0)
	{
		AfxMessageBox(StrFromResID(IDS_PAGE_CHECK));
		return FALSE;
	}
	
return TRUE;
		
}
void CDlgSymbolTable::OnRestoreDefault()
{
	m_nHeight		= 332;
	m_nWidth		= 210;
	m_nSpaceTop		= 18;
	m_nSpaceBottom	= 0;
	m_nSpaceLeft	= 20;
	m_nSpaceRight	= 0;
	m_nEStart		= 100;
	m_nNStart		= 10000;
	m_nWidthCode	=25;
	m_nWidthLayerName = 55;
	m_nWidthSymbolMode = 20;
	m_nWidthSymbol	 = 20;
	m_nSymbolDiscribtion = 40;
	m_nTableNumber = 2;
	m_nSpace = 5;
	m_nRowWidth = 10;
	m_nCodeBoundLow = 0;
	m_nCodeBoundHigh = 10000;
	UpdateData(FALSE);
}