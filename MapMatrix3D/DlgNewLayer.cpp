// DlgNewLayer.cpp : implementation file
//

#include "stdafx.h"
#include "DlgNewLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLayer dialog


CDlgNewLayer::CDlgNewLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewLayer)
	m_strLayerName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgNewLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewLayer)
	DDX_Text(pDX, IDC_LAYER_EDIT, m_strLayerName);
//	DDV_MaxChars(pDX, m_strLayerName, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgNewLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLayer message handlers

void CDlgNewLayer::SetLayerName(CString tmp)
{
	m_strLayerName = tmp;

}

void CDlgNewLayer::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	if(m_strLayerName == "")
	{
		CString msg,cap;
		msg.LoadString(IDS_CANNT_NULL);
		cap.LoadString(IDS_ATTENTION);
		MessageBox(msg,cap,MB_OK|MB_ICONASTERISK);
		return;
	}
	CDialog::OnOK();
}
