// EditplusListBoxDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "EditplusListBox.h"
#include "DlgSearchCheckPt.h"
//#include "EditInComboDll.h"
//#include "..\CtrlDll2\CtrlDll2Dlg.h"
//#include "CtrlDll2Dlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//extern "C" void _declspec(dllexport) GetStrArray(CString str, CStringArray& strArraySrc, CStringArray& strArrayResult);
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

// class CAboutDlg : public CDialog
// {
// public:
// 	CAboutDlg();
// 
// // Dialog Data
// 	//{{AFX_DATA(CAboutDlg)
// 	enum { IDD = IDD_ABOUTBOX };
// 	//}}AFX_DATA
// 
// 	// ClassWizard generated virtual function overrides
// 	//{{AFX_VIRTUAL(CAboutDlg)
// 	protected:
// 	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
// 	//}}AFX_VIRTUAL
// 
// // Implementation
// protected:
// 	//{{AFX_MSG(CAboutDlg)
// 	//}}AFX_MSG
// 	DECLARE_MESSAGE_MAP()
// };
// 
// CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
// {
// 	//{{AFX_DATA_INIT(CAboutDlg)
// 	//}}AFX_DATA_INIT
// }

// void CAboutDlg::DoDataExchange(CDataExchange* pDX)
// {
// 	CDialog::DoDataExchange(pDX);
// 	//{{AFX_DATA_MAP(CAboutDlg)
// 	//}}AFX_DATA_MAP
// }
// 
// BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
// 	//{{AFX_MSG_MAP(CAboutDlg)
// 		// No message handlers
// 	//}}AFX_MSG_MAP
// END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditplusListBoxDlg dialog
IMPLEMENT_DYNAMIC(CSearchCheckPtDlg, CDialog);
CSearchCheckPtDlg::CSearchCheckPtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearchCheckPtDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditplusListBoxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_bPoppedoutListSearchResult = FALSE;
	m_bHasSelectedItem = FALSE;
	m_pDlgChkPtsList = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSearchCheckPtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditplusListBoxDlg)
	DDX_Control(pDX, IDC_EDIT_KEYTEXT, m_editCtrlKeyText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchCheckPtDlg, CDialog)
	//{{AFX_MSG_MAP(CEditplusListBoxDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_EDIT_KEYTEXT, OnChangeEditKeytext)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_HSCROLL()
	ON_WM_CHAR()
	ON_MESSAGE(WM_MYDBLCLK,MyDbClkMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditplusListBoxDlg message handlers

BOOL CSearchCheckPtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;//  unless you set the focus to a control
}

void CSearchCheckPtDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSearchCheckPtDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSearchCheckPtDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSearchCheckPtDlg::OnChangeEditKeytext() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString str;
	CStringArray m_strArraySearchResult;
	m_editCtrlKeyText.GetWindowText(str);
	m_editCtrlKeyText.SetSel(str.GetLength(),-1);
	m_editCtrlKeyText.SetFocus();

	m_bHasSelectedItem = FALSE;
	
	GetStrArray(str, m_strArray, m_strArraySearchResult);

	if (!m_pDlgChkPtsList)
	{
		if(m_strArraySearchResult.GetSize()<=0)	
				return;
		m_pDlgChkPtsList = new CDlgChkPtsList();
		m_pDlgChkPtsList->Create(IDD_CHKPTLIST,this); //hcw,Add this,2013.1.22
	
	}
	if (!m_pDlgChkPtsList)
	{
		return;
	}
	//独立的CListBox
	m_pDlgChkPtsList->m_listChkPts.ResetContent();
	AddStrArraytoList(m_strArraySearchResult,m_pDlgChkPtsList);
	if(m_pDlgChkPtsList->m_listChkPts.GetCount()>0)
	{
			RepositionDlgPtsList();
			m_pDlgChkPtsList->ShowWindow(SW_SHOWDEFAULT);
			m_pDlgChkPtsList->m_listChkPts.SetCurSel(0);
 			m_bPoppedoutListSearchResult = TRUE;
			m_pDlgChkPtsList->BringWindowToTop();
			this->SetActiveWindow();			
	}	
	else
	{

		m_pDlgChkPtsList->ShowWindow(SW_HIDE);	
		m_bPoppedoutListSearchResult = FALSE;
		this->SetActiveWindow();
	}
	return;

	
}

int CSearchCheckPtDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rt;
	rt.SetRectEmpty();
	if(!m_wndListSearchResult.Create(WS_VISIBLE|WS_CHILD|LBS_SORT|LBS_STANDARD|WS_HSCROLL, rt, this, IDC_LIST_ChkPtNAME))
	{
		TRACE0("Failed to create listbox!");
		return -1;
	}

	return 0;
}

void CSearchCheckPtDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSearchCheckPtDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CSearchCheckPtDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CSearchCheckPtDlg::PreTranslateMessage( MSG* pMsg )
{
	if (!m_pDlgChkPtsList)
	{
		return CDialog::PreTranslateMessage(pMsg);
	}
	if (pMsg->message==WM_KEYDOWN)
	{
		int nIndex = 0;
		if(m_bPoppedoutListSearchResult)
		{
			if(pMsg->wParam==VK_DOWN)
			{
				nIndex = m_pDlgChkPtsList->m_listChkPts.GetCurSel();
				m_pDlgChkPtsList->m_listChkPts.SetCurSel(nIndex+1);
				return TRUE;
			}
			else if(pMsg->wParam==VK_UP)
			{
				nIndex = m_pDlgChkPtsList->m_listChkPts.GetCurSel();
				if (nIndex<=0)
				{
					m_pDlgChkPtsList->m_listChkPts.SetCurSel(0);	
				}
				else
					m_pDlgChkPtsList->m_listChkPts.SetCurSel(nIndex-1);
				return TRUE;
			}
			else if (pMsg->wParam==VK_RETURN)
			{
				CString strText;
				strText = m_pDlgChkPtsList->GetSelectedStr();
				m_editCtrlKeyText.SetWindowText(strText);
				m_editCtrlKeyText.SetSel(strText.GetLength(),-1);
				m_pDlgChkPtsList->ShowWindow(SW_HIDE);
				m_bHasSelectedItem = TRUE;
				m_bPoppedoutListSearchResult = FALSE;
				return TRUE;
			}

		}
	}
	CPoint pt;
	CRect rtDlg;
	CRect rtList;
	this->GetWindowRect(&rtDlg);
	m_pDlgChkPtsList->GetWindowRect(&rtList);
	::GetCursorPos(&pt);
	if (::GetActiveWindow()!=this->GetSafeHwnd()
		&&rtDlg.PtInRect(pt)==FALSE
		&&rtList.PtInRect(pt)==FALSE)
	{
		if (m_bPoppedoutListSearchResult)
		{
			m_pDlgChkPtsList->ShowWindow(SW_HIDE);
			m_bPoppedoutListSearchResult = FALSE;
		}
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSearchCheckPtDlg::GetStrArray( CString str, CStringArray& strArraySrc, CStringArray& strArrayResult )
{
	CStringArray strArraySrcTemp;
	strArraySrcTemp.Copy(strArraySrc);
	strArrayResult.RemoveAll();
	CString strKey = str;
	CString strSrc;
    strKey.TrimLeft(" ");
	strKey.TrimRight(" ");
	strKey.MakeUpper();
	//先完全匹配
	for(int j=0; j<strArraySrcTemp.GetSize(); j++)
	{
	  if (strKey.CompareNoCase(strArraySrcTemp.GetAt(j))==0)
	  {
		  strArrayResult.Add(strArraySrcTemp.GetAt(j));
		  strArraySrcTemp.RemoveAt(j);
	  }
	}
	//再模糊匹配
	for (int i=0; i<strArraySrcTemp.GetSize(); i++)
	{
		strSrc = strArraySrcTemp.GetAt(i);
		strSrc.MakeUpper();
		if (strSrc.Find(strKey)>=0
			&&strKey!="")
		{
			strArrayResult.Add(strArraySrcTemp.GetAt(i));
		}
	}	
	return;
}

void CSearchCheckPtDlg::AddStrArraytoList( CStringArray& strArrayResult, CListBox& listBoxCtrl )
{
	if(strArrayResult.GetSize()<=0)
	{
		return;
	}
	for(int i=0; i<strArrayResult.GetSize(); i++)
	{
		if (listBoxCtrl.FindString(-1,strArrayResult.GetAt(i))==LB_ERR)
		{
			listBoxCtrl.InsertString(-1,strArrayResult.GetAt(i));
		}
		
	}
	return;
}
//重载。
void CSearchCheckPtDlg::AddStrArraytoList( CStringArray& strArrayResult, CDlgChkPtsList* pDlgChkPtsList )
{
	if ((strArrayResult.GetSize()<=0)
		&&(!pDlgChkPtsList))
	{
		return;
	}
	pDlgChkPtsList->AddtoList(strArrayResult);
	return;
}

void CSearchCheckPtDlg::InitDlgMems( CStringArray& strArray )
{
	m_strArray.RemoveAll();
	m_strArray.Copy(strArray);
	return ;
}

void CSearchCheckPtDlg::OnOK()
{

	m_editCtrlKeyText.GetWindowText(m_strCurTxtforSearch);
	
	CDialog::OnOK();
}

void CSearchCheckPtDlg::UpDateEditCtrlKeyTxt()
{
	if (!m_pDlgChkPtsList)
	{
		return;
	}
	CString strText;
	strText = m_pDlgChkPtsList->GetSelectedStr();
	m_editCtrlKeyText.SetWindowText(strText);
	m_editCtrlKeyText.SetSel(strText.GetLength(),-1);
	m_pDlgChkPtsList->ShowWindow(SW_HIDE);

	m_bHasSelectedItem = TRUE;
	m_bPoppedoutListSearchResult = FALSE;
	return;
}

CString CSearchCheckPtDlg::GetEditTxt()
{
	return m_strCurTxtforSearch;
}

CSearchCheckPtDlg::~CSearchCheckPtDlg()
{
	if (m_pDlgChkPtsList)
	{
		delete m_pDlgChkPtsList;
		m_pDlgChkPtsList = NULL;
	}
	
}

LRESULT CSearchCheckPtDlg::MyDbClkMsg( WPARAM wParam,LPARAM lParam )
{
	m_pDlgChkPtsList->ShowWindow(SW_HIDE);
	return 0;
}

void CSearchCheckPtDlg::RepositionDlgPtsList()
{
	//定位
	CRect rtEdit;
	CRect rtListBox;
	CRect rtListwithFrame;
	CRect rtList;
	m_editCtrlKeyText.GetWindowRect(&rtEdit);
	m_pDlgChkPtsList->GetWindowRect(&rtListwithFrame);
	m_pDlgChkPtsList->GetDlgItem(IDC_LIST_CHKPTS)->GetWindowRect(&rtListBox);
	
	
	long lWidthofListwithFrame = (rtListwithFrame.right-rtListwithFrame.left);
	long lHeightofListwithFrame = (rtListwithFrame.bottom-rtListwithFrame.top);
	long lWidthofListBox = (rtListBox.right-rtListBox.left);
	long lHeightofListBox = (rtListBox.bottom-rtListBox.top);
	
	rtListwithFrame.left = rtEdit.left+(rtEdit.right-rtEdit.left)/2-lWidthofListwithFrame/2;
	rtListwithFrame.right = rtListwithFrame.left + lWidthofListwithFrame;
	rtListwithFrame.top = rtEdit.bottom;
	rtListwithFrame.bottom = rtListwithFrame.top + lHeightofListwithFrame;
	
	rtListBox.left = (rtListBox.right+rtListBox.left)/2-lWidthofListBox/2;
	rtListBox.right = rtListBox.left+lWidthofListBox;
	rtListBox.top = (rtListBox.bottom+rtListBox.top)/2-lHeightofListBox/2;
	rtListBox.bottom = rtListBox.top + lHeightofListBox;
	
	rtListBox.left = (rtListwithFrame.right+rtListwithFrame.left)/2-lWidthofListBox/2;
	rtListBox.right = rtListBox.left + lWidthofListBox;
	rtListBox.top = (rtListwithFrame.bottom+rtListwithFrame.top)/2-lHeightofListBox/2;
	rtListBox.bottom = rtListBox.top + lHeightofListBox;
	
	rtList.left = rtEdit.left;
	rtList.right = rtEdit.right;
	rtList.top = rtEdit.bottom;
	rtList.bottom = rtList.top + lHeightofListwithFrame;		
	m_pDlgChkPtsList->SetFont(this->GetFont(), TRUE);
	m_pDlgChkPtsList->MoveWindow(rtList); 
	m_pDlgChkPtsList->ShowWindow(SW_HIDE);
	return;
}
