// DlgTip.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTip dialog


CDlgTip::CDlgTip(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTip::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTip)
	m_strTip = _T("");
	m_strTitle = _T("");
	//}}AFX_DATA_INIT
}


void CDlgTip::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTip)
	DDX_Text(pDX, IDC_EDIT_TIP, m_strTip);
	DDX_Text(pDX, IDC_STATIC_TITLE, m_strTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTip, CDialog)
	//{{AFX_MSG_MAP(CDlgTip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTip message handlers


void CDlgTip::SetTitle(LPCTSTR title)
{
	m_strTitle = title;
	m_strTitle += ":";
}


void CDlgTip::AddTip(LPCTSTR tip)
{
	m_arrTips.Add(CString(tip));
}

BOOL CDlgTip::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_strTip.Empty();
	for( int i=0; i<m_arrTips.GetSize(); i++)
	{
		if( i>0 )
		{
			m_strTip += "\r\n";
		}
		m_strTip += m_arrTips[i];
	}

	UpdateData(FALSE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
