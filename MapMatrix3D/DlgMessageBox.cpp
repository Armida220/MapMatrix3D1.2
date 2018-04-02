// DlgMessageBox.cpp: implementation of the CDlgMessageBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "SmartViewFunctions.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox dialog

BOOL CDlgMessageBox::m_bCheck = FALSE;

CDlgMessageBox::CDlgMessageBox(CWnd* pParent /*=NULL*/)
{
	//{{AFX_DATA_INIT(CDlgMessageBox)
	//}}AFX_DATA_INIT
	m_nRet = -1;
}


void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageBox)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageBox)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDYESALL, OnYesAll)
	ON_BN_CLICKED(IDC_MSGDLG_CHECK,OnClickCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMessageBox message handlers


void CDlgMessageBox::OnClickCheck()
{
	m_bCheck = !m_bCheck;

	CButton *pBtn = (CButton*)GetDlgItem(IDC_MSGDLG_CHECK);
	if( pBtn )
	{
		pBtn->SetCheck(m_bCheck?1:0);
	}
}

void CDlgMessageBox::OnYes() 
{
	m_nRet = IDYES;
	CDialog::OnOK();
}

void CDlgMessageBox::OnNo()
{
	m_nRet = IDNO;
	CDialog::OnOK();
}

void CDlgMessageBox::OnYesAll()
{
	m_nRet = IDYESALL;
	CDialog::OnOK();
}


BOOL CDlgMessageBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//标题
	SetWindowText(m_strCaption);

	//字体
	static CFont font;
	CFont *pfont = GetFont();

	//初始化对话框宽度
	int nScrWid = GetSystemMetrics(SM_CXSCREEN), nScrHei = GetSystemMetrics(SM_CYSCREEN);
	int nDlgWid0 = nScrWid/2, nDlgHei0 = nScrHei/2;

	//边界空白宽度，控键垂直水平间隔
	int nMargin = 15, nCtrlVIntv = 18, nCtrlHIntv = 5;
	int nBtnHei = 20, nBtnWid = 75, nTextVIntv = 4;
	int nBorder = GetSystemMetrics(SM_CXBORDER);
	int nCaption = GetSystemMetrics(SM_CYCAPTION);

	//文本
	CDC *pDC = GetDC();
	CSize szText(0,0);
	if( pDC )
	{
		::GetTextExtentPoint32(pDC->GetSafeHdc(),m_strText,m_strText.GetLength(),&szText);
	}

	//图标
	HICON hIcon = ::LoadIcon(NULL,IDI_WARNING);
	ICONINFO iinfo;
	::GetIconInfo(hIcon,&iinfo);
	HBITMAP hBmp = iinfo.hbmColor!=NULL?iinfo.hbmColor:iinfo.hbmMask;
	CSize szIcon(0,0);
	if( hBmp )
	{
		BITMAP binfo;
		::GetObject(hBmp,sizeof(binfo),&binfo);
		szIcon.cx = binfo.bmWidth; szIcon.cy = binfo.bmHeight;
	}

	//计算水平方向上按钮的数目
	int nHBtn = 0;
	if( m_nType&MYMB_YES )nHBtn++;
	if( m_nType&MYMB_NO )nHBtn++;
	if( m_nType&MYMB_YESALL )nHBtn++;

	int nHBtnWid = (nBtnWid+nCtrlHIntv+nBorder*2)*nHBtn;

	//计算对话框的尺寸
	int nDlgWid = 0, nDlgHei = 0;

	nDlgWid = (nMargin+nBorder)*2;
	if(szIcon.cx+szText.cx+nMargin>nHBtnWid )
		nDlgWid = nDlgWid + szIcon.cx + szText.cx + nMargin;
	else nDlgWid = nDlgWid + nHBtnWid;

	int nTextWid = szText.cx, nTextHei = szText.cy;
	int nLine = 1, nCheck = 0;
	if( m_nType&MYMB_NEVER_WARNING )nCheck++;
	if( nDlgWid>nDlgWid0 )
	{
		nTextWid = nDlgWid0-(nMargin+nBorder)*2-nMargin-szIcon.cx;
		nLine = (int)ceil((float)szText.cx/nTextWid);

		nTextHei = nLine*szText.cy+(nLine-1)*nTextVIntv;
		nDlgWid = nDlgWid0;
	}

	if( nTextHei+nCheck*(szText.cy+nCtrlVIntv)<szIcon.cy )
		nDlgHei = nCaption + (nMargin+nBorder)*2 + szIcon.cy + nCtrlVIntv + (nBtnHei+nBorder*2);
	else
		nDlgHei = nCaption + (nMargin+nBorder)*2 + nTextHei+nCheck*(szText.cy+nCtrlVIntv)
		+ nCtrlVIntv + (nBtnHei+nBorder*2);
	
	//重置窗口尺寸
	CRect rect;
	GetWindowRect(&rect);
	rect.right = rect.left+nDlgWid; rect.bottom = rect.top+nDlgHei;
	MoveWindow(rect);

	//创建图标
	CRect rcIcon;
	rcIcon.left = nMargin, rcIcon.top = nMargin;
	rcIcon.right = rcIcon.left+szIcon.cx, rcIcon.bottom = rcIcon.top+szIcon.cy;
	m_wndIcon.Create(NULL,WS_CHILD|WS_VISIBLE|SS_ICON,rcIcon,this);
	m_wndIcon.SetIcon(hIcon);

	//创建文本
	CRect rcText;
	rcText.left = rcIcon.right+nMargin, rcText.top = rcIcon.top;
	rcText.right = rcText.left+nTextWid, rcText.bottom = rcText.top+(nLine*szText.cy+(nLine-1)*nTextVIntv);
	m_wndText.Create(m_strText,WS_CHILD|WS_VISIBLE|SS_LEFT,rcText,this);
	m_wndText.SetFont(pfont);

	CRect rcYesBtn;
	rcYesBtn.left = nDlgWid/2-(nHBtn*(nBtnWid+nCtrlHIntv+nBorder*2)-nCtrlHIntv)/2;

	if( nTextHei+nCheck*(szText.cy+nCtrlVIntv)<szIcon.cy )
		rcYesBtn.top = rcIcon.bottom+nCtrlVIntv;
	else
		rcYesBtn.top = rcText.bottom+nCtrlVIntv;
	rcYesBtn.right = rcYesBtn.left+nBtnWid+nBorder+nBorder;
	rcYesBtn.bottom = rcYesBtn.top+nBtnHei+nBorder+nBorder;	

	UINT nDefID = 0;

	//创建按钮
	CButton *pBtn = NULL;
	if( m_nType&MYMB_NEVER_WARNING )
	{
		CRect rcBtn;
		rcBtn.left = rcText.left, rcBtn.top = rcText.bottom+nCtrlVIntv;
		rcBtn.right = rcBtn.left+nTextWid, rcBtn.bottom = rcBtn.top+szText.cy;

		pBtn = new CButton;
		pBtn->Create(StrFromResID(IDS_MSGDLG_NEVERWARNING),WS_CHILD|WS_VISIBLE|BS_CHECKBOX,rcBtn,this,IDC_MSGDLG_CHECK);
		pBtn->SetFont(pfont);
		m_arrBtn.Add(pBtn);

		rcYesBtn.top += (rcBtn.Height()+nCtrlVIntv);
		rcYesBtn.bottom += (rcBtn.Height()+nCtrlVIntv);
	}

	if( m_nType&MYMB_YES )
	{
		pBtn = new CButton;
		pBtn->Create(StrFromResID(IDS_MSGDLG_YES),WS_CHILD|WS_VISIBLE,rcYesBtn,this,IDYES);
		pBtn->SetFont(pfont);
		m_arrBtn.Add(pBtn);

		rcYesBtn.left += (nCtrlHIntv+nBtnWid);
		rcYesBtn.right += (nCtrlHIntv+nBtnWid);

		nDefID = IDNO;
	}
	
	if( m_nType&MYMB_NO )
	{
		pBtn = new CButton;
		pBtn->Create(StrFromResID(IDS_MSGDLG_NO),WS_CHILD|WS_VISIBLE,rcYesBtn,this,IDNO);
		pBtn->SetFont(pfont);
		m_arrBtn.Add(pBtn);
		
		rcYesBtn.left += (nCtrlHIntv+nBtnWid);
		rcYesBtn.right += (nCtrlHIntv+nBtnWid);
		
	}

	if( m_nType&MYMB_YESALL )
	{
		pBtn = new CButton;
		pBtn->Create(StrFromResID(IDS_MSGDLG_YESALL),WS_CHILD|WS_VISIBLE,rcYesBtn,this,IDYESALL);
		pBtn->SetFont(pfont);
		m_arrBtn.Add(pBtn);

		nDefID = IDNO;
	}

	SetDefID(nDefID);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

INT_PTR CDlgMessageBox::DoModal() 
{
	int size = 1000;                       
	BYTE* buf = new BYTE[size];            
	if( buf == NULL )return 0;             
	
	memset( buf, 0, size*sizeof(BYTE));    
	
	//write the dialog template data                                              
	DLGTEMPLATE* dlgTempl= (DLGTEMPLATE*)buf;                               
	
	dlgTempl->style = WS_CAPTION |WS_POPUP|WS_VISIBLE|WS_DLGFRAME|WS_CLIPSIBLINGS|DS_MODALFRAME;
	dlgTempl->dwExtendedStyle = 0;                                          
	dlgTempl->cdit = 0;                                                     
	dlgTempl->x = 0;                                                        
	dlgTempl->y = 0;                                                        
	dlgTempl->cx = 300;                                                     
	dlgTempl->cy = 100;                                                     
	
	BYTE* ptmpBuf = buf;                                                    
	ptmpBuf = ptmpBuf+sizeof(DLGTEMPLATE);                                  
	*((WORD*)ptmpBuf) = 0x0000; ptmpBuf += sizeof(WORD); // no menu         
	*((WORD*)ptmpBuf) = 0x0000; ptmpBuf += sizeof(WORD); // default window c
	
	InitModalIndirect(ptmpBuf,NULL);
	
	CDialog::DoModal();
	delete[] buf;

	for( int i=0; i<m_arrBtn.GetSize(); i++)
	{
		CButton *pBtn = (CButton*)m_arrBtn.GetAt(i);
		if( !pBtn )
		{
			pBtn->DestroyWindow();
			delete pBtn;
		}
	}

	m_arrBtn.RemoveAll();

	return m_nRet;
}

int CDlgMessageBox::Do(LPCTSTR lpszText, LPCTSTR lpszCaption, UINT nType )
{
	CDlgMessageBox dlg;
	if( lpszCaption )
		dlg.m_strCaption = lpszCaption;
	dlg.m_strText = lpszText;
	dlg.m_nType = nType;
	return dlg.DoModal();
}
