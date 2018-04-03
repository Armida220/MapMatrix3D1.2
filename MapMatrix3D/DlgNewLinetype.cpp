// DlgNewLinetype.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgNewLinetype.h"
#include "SmartViewFunctions.h"
#include "SymbolLib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLinetype dialog


CDlgNewLinetype::CDlgNewLinetype(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewLinetype::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewLinetype)
	m_strContent = _T("");
	m_strName = _T("");
	//}}AFX_DATA_INIT
	m_bModified = FALSE;
}


void CDlgNewLinetype::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewLinetype)
	DDX_Text(pDX, IDC_LINENAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_CONTENT_EDIT, m_strContent);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewLinetype, CDialog)
	//{{AFX_MSG_MAP(CDlgNewLinetype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewLinetype message handlers

void CDlgNewLinetype::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);

	if (m_strName.CompareNoCase(m_strOldName) != 0 || m_strContent.CompareNoCase(m_strOldContent) != 0)
	{
		m_bModified = TRUE;
	}

	CDialog::OnOK();
}



BOOL CDlgNewLinetype::GetLineType(BaseLineType &line)
{
	line.m_nNum = 0;

	//去掉空格
	m_strContent.Replace(" ",NULL);

	if (m_strContent.IsEmpty())
	{
		AfxMessageBox(StrFromResID(IDS_CANNT_EMPTY),MB_OK|MB_ICONASTERISK);
		return FALSE;
	}

	//格式检测
	for(int i=0; i<m_strContent.GetLength(); i++)
	{
		if(m_strContent.GetAt(i)!=0x2B && m_strContent.GetAt(i)!=0x2D && !isdigit(m_strContent.GetAt(i)) && m_strContent.GetAt(i)!= L'.' && m_strContent.GetAt(i)!=',')
			return FALSE;
	}

	strcpy(line.m_name,m_strName);

	i = 0;
	int nIndex = 0;
	while (nIndex<m_strContent.GetLength() && i<8)
	{
		int nfirst = nIndex;
		nIndex = m_strContent.Find(',', nfirst);
		//仅一个和最一个的情况
		if (nIndex < 0)
		{
			nIndex = m_strContent.GetLength();
		}

		CString strDigital = m_strContent.Mid(nfirst,nIndex-nfirst);

		if (IsDigital(strDigital))
		{
			line.m_fLens[i] = atof(strDigital);
			line.m_nNum++;
			i++;
		}

		nIndex++;
		
	}	

	return TRUE;


}

void CDlgNewLinetype::SetLineType(const BaseLineType &line)
{
	m_strName = line.m_name;
	for (int i=0; i<line.m_nNum; i++)
	{
		CString tmp;
		tmp.Format("%.3f",line.m_fLens[i]);
		m_strContent += tmp;

		if (i < line.m_nNum-1)
		{
			m_strContent += ", ";
		}
		
	}

	m_strOldName = m_strName;
	m_strOldContent = m_strContent;

//	UpdateData(FALSE);
}
