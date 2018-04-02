// AccuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "SilenceDDX.h"
#include "AccuDlg.h"
#include "ExMessage.h "
#include "SmartViewFunctions.h"
#include "editbasedoc.h "
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REG_ENTRY_WP_ACCUX			_T("AccuWndX")
#define REG_ENTRY_WP_ACCUY			_T("AccuWndY")

static const CString strOptsRegEntryName = _T("WindowPlacement");

BEGIN_MESSAGE_MAP(CReflectKeyDialog, CDialog)
	//{{AFX_DATA_MAP(CReflectKeyDialog)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CReflectKeyDialog::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN )
	{
		char clsName[256] = {0};
		GetClassName(pMsg->hwnd,clsName,sizeof(clsName)-1);
		if( strcmp(clsName,"Edit")!=0 && strcmp(clsName,"ComboBox")!=0 )
		{
			pMsg->hwnd = m_hWnd;
			return FALSE;
		}
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CReflectKeyDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	MSG msg = *GetCurrentMessage();
	AfxGetMainWnd()->SendMessage(FCCM_TRANSLATEMSG,0,(LPARAM)&msg);
	::SetFocus(AfxGetMainWnd()->m_hWnd);
}


/////////////////////////////////////////////////////////////////////////////
// CAccuDlg Dialog

//CAccuDlg::CAccuDlg(CWnd* pParent /*=NULL*/)
//	: CReflectKeyDialog(CAccuDlg::IDD, pParent)
/*{
	//{{AFX_DATA_INIT(CAccuDlg)
	m_lfX = 0.0f;
	m_lfY = 0.0f;
	m_lfZ = 0.0f;
	m_lfDis = 0.0f;
	m_lfAng = 0.0f;
	m_bLockX = FALSE;
	m_bLockY = FALSE;
	m_bLockZ = FALSE;
	m_bLockDis = FALSE;
	m_bLockAng = FALSE;
	m_bUnlockFirstLine = FALSE;
	m_bAutoReset = FALSE;
	m_bViewDraw = FALSE;
	m_bRightModeWithSnapPt = FALSE;
	m_nTolerance = 0;
	m_nDir = 0;
	//}}AFX_DATA_INIT

	m_bPopup = FALSE;
	m_nCoordType = 0;
	m_bPrivateMove = FALSE;

	m_bLockingDir = FALSE;
}


void CAccuDlg::DoDataExchange(CDataExchange* pDX)
{
	CUIFDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccuDlg)
	DDX_Control(pDX, IDC_STATIC_ACCU_STATUS, m_stcStatus);
	DDX_Control(pDX, IDC_BUTTON_ACCU_ADVANCE, m_btnAdvance);
	DDX_Check(pDX, IDC_CHECK_ACCU_UNLOCKFIRSTPT, m_bUnlockFirstLine);
	DDX_Check(pDX, IDC_CHECK_ACCU_AUTORESET, m_bAutoReset);
	DDX_Check(pDX, IDC_CHECK_ACCU_VIEW, m_bViewDraw);
	DDX_Text_Silence(pDX, IDC_EDIT_ACCU_TOLERANCE, m_nTolerance);
	DDX_CBIndex(pDX, IDC_COMBO_ACCU_DIR, m_nDir);
	//}}AFX_DATA_MAP
	if( m_nCoordType==0 )
	{
		DDX_Text_Silence(pDX, IDC_EDIT_ACCUZ, m_lfZ);
		DDX_Text_Silence(pDX, IDC_EDIT_ACCUY, m_lfY);
		DDX_Text_Silence(pDX, IDC_EDIT_ACCUX, m_lfX);
		DDX_Check(pDX, IDC_CHECK_ACCUX_LOCK, m_bLockX);
		DDX_Check(pDX, IDC_CHECK_ACCUY_LOCK, m_bLockY);
		DDX_Check(pDX, IDC_CHECK_ACCUZ_LOCK, m_bLockZ);
	}
	else if( m_nCoordType==1 )
	{
		DDX_Text_Silence(pDX, IDC_EDIT_ACCUDIS, m_lfDis);
		DDX_Text_Silence(pDX, IDC_EDIT_ACCUANG, m_lfAng);
		DDX_Check(pDX, IDC_CHECK_ACCUDIS_LOCK, m_bLockDis);
		DDX_Check(pDX, IDC_CHECK_ACCUANG_LOCK, m_bLockAng);
		DDX_Check(pDX, IDC_CHECK_ACCU_RIGHTMODE, m_bRightModeWithSnapPt);
	}
}


BEGIN_MESSAGE_MAP(CAccuDlg, CReflectKeyDialog)
	//{{AFX_MSG_MAP(CAccuDlg)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CHECK_ACCUX_LOCK, OnCheckAccuxLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUY_LOCK, OnCheckAccuyLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUZ_LOCK, OnCheckAccuzLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUDIS_LOCK, OnCheckAccudisLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUANG_LOCK, OnCheckAccuangLock)
	ON_BN_CLICKED(IDC_CHECK_ACCU_VIEW, OnCheckAccuViewDraw)
	ON_BN_CLICKED(IDC_CHECK_ACCU_RIGHTMODE, OnCheckAccuViewDraw)
	ON_BN_CLICKED(IDC_CHECK_ACCU_AUTORESET, OnCheckAccuAutoReset)
	ON_BN_CLICKED(IDC_CHECK_ACCU_UNLOCKFIRSTPT, OnCheckAccuUnlockFirstLine)
	ON_EN_CHANGE(IDC_EDIT_ACCUX, OnChangeEditAccux)
	ON_EN_CHANGE(IDC_EDIT_ACCUY, OnChangeEditAccuy)
	ON_EN_CHANGE(IDC_EDIT_ACCUZ, OnChangeEditAccuz)
	ON_EN_CHANGE(IDC_EDIT_ACCUDIS, OnChangeEditAccudis)
	ON_EN_CHANGE(IDC_EDIT_ACCUANG, OnChangeEditAccuang)
	ON_BN_CLICKED(IDC_BUTTON_ACCU_ADVANCE, OnAdvance)
	ON_EN_CHANGE(IDC_EDIT_ACCU_TOLERANCE, OnChangeEditAccuTolerance)
	ON_BN_CLICKED(IDC_BUTTON_ACCU_LOCKDIR, OnLockDir)
	ON_BN_CLICKED(IDC_BUTTON_ACCU_UNLOCKDIR, OnUnlockDir)
	ON_CBN_SELCHANGE(IDC_COMBO_ACCU_DIR, OnSelchangeDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccuDlg message handlers

void CAccuDlg::Show(int code, long param)
{
	if( code==0 )
	{
		ACCU_DATA *data = (ACCU_DATA*)param;
		if( data )
		{
			UINT id = 0;
			int oldtype = m_nCoordType;
			if( data->type==CAccuBox::sysCartesian )
			{
				m_nCoordType = 0;
				id = IDD_ACCU_DLG;
				m_lfX = data->u.Cartesian.pt.x; 
				m_lfY = data->u.Cartesian.pt.y; 
				m_lfZ = data->u.Cartesian.pt.z;

				m_bLockX = (data->u.Cartesian.bLockX!=FALSE); 
				m_bLockY = (data->u.Cartesian.bLockY!=FALSE); 
				m_bLockZ = (data->u.Cartesian.bLockZ!=FALSE);
			}
			else if( data->type==CAccuBox::sysPolar )
			{
				m_nCoordType = 1;
				id = IDD_ACCU_DLG2;
				m_lfDis = data->u.Polar.dis; 
				m_lfAng = data->u.Polar.ang*180/PI; 
				
				m_bLockDis = (data->u.Polar.bLockDis!=FALSE); 
				m_bLockAng = (data->u.Polar.bLockAng!=FALSE); 
			}

			m_bUnlockFirstLine = (data->bUnlockFirstLine!=FALSE);
			m_bAutoReset = (data->bAutoReset!=FALSE);
			m_bViewDraw  = (data->bViewDraw!=FALSE);
			m_bRightModeWithSnapPt = data->bRightModeWithSnapPt;
			m_nStatus    = data->status;
			m_nTolerance = data->tolerance;

			if( oldtype!=m_nCoordType && id!=0 )
			{
				ChangeFace(id);
			}

			UpdateData (FALSE);
			UpdateStatusString();
		}
	}
	else if( code==1 )
	{
		CComboBox *pCtrl = (CComboBox*)GetDlgItem(IDC_COMBO_ACCU_DIR);
		if( pCtrl )
		{
			int dir = -1;
			switch(param) {
			case ACCU_DIR::extension:		dir = 0; break;
			case ACCU_DIR::parallel:		dir = 1; break;
			case ACCU_DIR::perpendicular:	dir = 2; break;
			}
			if( dir>=0 )
			{
				m_bLockingDir = TRUE;
				m_nDir = dir;
				pCtrl->SetCurSel(m_nDir);
			}
			else m_bLockingDir = FALSE;
		}
		
		PaintLockDirBtn();
	}
}

BOOL CAccuDlg::OnInitDialog() 
{
	CUIFDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	if(pApp!=NULL)
	{
		// ¶Á×¢²áµÇ¼Ç±í
		int x = pApp->GetSectionInt(strOptsRegEntryName,REG_ENTRY_WP_ACCUX, 600);
		int y = pApp->GetSectionInt(strOptsRegEntryName,REG_ENTRY_WP_ACCUY, 150);
		SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE);
	}

	m_btnAdvance.m_bDrawFocus = FALSE;
	m_btnAdvance.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;

	if( m_nCoordType==0 )GetWindowRect(m_rc1);
	else GetWindowRect(m_rc2);
	UpdateAdvanceCtrls();

	PaintLockDirBtn();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAccuDlg::OnDestroy() 
{
	CUIFDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	if(pApp!=NULL)
	{
		// Ð´×¢²áµÇ¼Ç±í
		pApp->WriteSectionInt(strOptsRegEntryName,REG_ENTRY_WP_ACCUX, wp.rcNormalPosition.left);
		pApp->WriteSectionInt(strOptsRegEntryName,REG_ENTRY_WP_ACCUY, wp.rcNormalPosition.top);		
	}	
}



BOOL CAccuDlg::ChangeFace(UINT id)
{
	HWND hWnd = ::GetFocus();
	BOOL bVisible = IsWindowVisible();
	DestroyWindow();
	
	if( !Create(id) )return FALSE;

	if( bVisible )ShowWindow(SW_SHOW);
	else ShowWindow(SW_HIDE);

	if( hWnd )::SetFocus(hWnd);
	return TRUE;
}


void CAccuDlg::UpdateAccuData()
{
	ACCU_DATA data;
	if( m_nCoordType==0 )
	{
		data.type = CAccuBox::sysCartesian;
		data.u.Cartesian.pt.x = m_lfX ; 
		data.u.Cartesian.pt.y = m_lfY ; 
		data.u.Cartesian.pt.z = m_lfZ ;
			     
		data.u.Cartesian.bLockX = (m_bLockX!=FALSE); 
		data.u.Cartesian.bLockY = (m_bLockY!=FALSE); 
		data.u.Cartesian.bLockZ = (m_bLockZ!=FALSE);
	}
	else if( m_nCoordType==1 )
	{
		data.type = CAccuBox::sysPolar;
		data.u.Polar.dis = m_lfDis; 
		data.u.Polar.ang = m_lfAng*PI/180; 
		
		data.u.Polar.bLockDis = (m_bLockDis!=FALSE); 
		data.u.Polar.bLockAng = (m_bLockAng!=FALSE); 
	}

	data.bUnlockFirstLine = (m_bUnlockFirstLine!=FALSE);
	data.bAutoReset = (m_bAutoReset!=FALSE);
	data.bViewDraw  = (m_bViewDraw!=FALSE);
	data.bRightModeWithSnapPt = m_bRightModeWithSnapPt;
	data.status = m_nStatus;
	data.tolerance = m_nTolerance;
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(pDoc)pDoc->m_accuBox.OnUpdateData(0,(LPARAM)&data);
//	AfxGetMainWnd()->SendMessage(FCCM_UPDATE_ACCUDATA,0,(LPARAM)&data);
//	AfxCallMessage(FCCM_UPDATE_ACCUDATA,0,(LPARAM)&data);
}


BOOL CAccuDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN || pMsg->message==WM_SYSKEYDOWN )
	{
		if( pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE )
		{
			pMsg->hwnd = m_hWnd;
			return FALSE;
		}
	}
	
	return CReflectKeyDialog::PreTranslateMessage(pMsg);
}


void CAccuDlg::OnCheckAccuxLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuyLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuzLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccudisLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuangLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuViewDraw() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuUnlockFirstLine() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuAutoReset() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccux() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUX_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccuy() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUY_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccuz() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUZ_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}


void CAccuDlg::OnChangeEditAccudis() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUDIS_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccuang() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUANG_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnAdvance() 
{
	m_bPopup = !m_bPopup;
	UpdateAdvanceCtrls();
}


void CAccuDlg::UpdateStatusString()
{
	CString str = StrFromResID(IDS_ACCUSTATUS);
	if( m_nStatus==0 )
	{
		//¿Õ
	}
	else if( m_nStatus&1 )
	{
		//Ëø¶¨»æÍ¼
		str += StrFromResID(IDS_ACCULOCK);
	}
	else if( m_nStatus&2 )
	{
		//Ë®Æ½»æÍ¼
		str += StrFromResID(IDS_ACCUHORIZ);
	}
	else if( m_nStatus&4 )
	{
		//´¹Ö±»æÍ¼
		str += StrFromResID(IDS_ACCUPERP);
	}

	CString old;
	m_stcStatus.GetWindowText(old);
	if( old!=str )
	{
		m_stcStatus.SetWindowText(str);
		m_stcStatus.RedrawWindow();
	}
}


void CAccuDlg::UpdateAdvanceCtrls()
{
	CRect rc, newrc;
	m_btnAdvance.GetWindowRect(rc);

	CRect mainrc;
	GetWindowRect(mainrc);
	
	//µ¯¿ª¸ß¼¶¿Ø¼ü
	if( m_bPopup )
	{
		if( m_nCoordType==0 )
			newrc.SetRect(mainrc.left,mainrc.top,mainrc.left+m_rc1.Width(),mainrc.top+m_rc1.Height());
		else
			newrc.SetRect(mainrc.left,mainrc.top,mainrc.left+m_rc2.Width(),mainrc.top+m_rc2.Height());
	}
	//²»µ¯¿ª
	else
	{
		if( m_nCoordType==0 )
			newrc.SetRect(mainrc.left,mainrc.top,mainrc.left+m_rc1.Width(),rc.bottom+mainrc.right-rc.right );
		else
			newrc.SetRect(mainrc.left,mainrc.top,mainrc.left+m_rc2.Width(),rc.bottom+mainrc.right-rc.right );
	}
	
	m_bPrivateMove = TRUE;
	SetWindowPos(NULL,newrc.left,newrc.top,newrc.Width(),newrc.Height(),SWP_NOACTIVATE);
	m_bPrivateMove = FALSE;
}

void CAccuDlg::OnChangeEditAccuTolerance() 
{
	UpdateData(TRUE);
	UpdateAccuData();
}

void CAccuDlg::OnLockDir() 
{
	m_bLockingDir = !m_bLockingDir;
	
	if( m_bLockingDir )
	{
		int dir = ACCU_DIR::invalid;
		switch(m_nDir) {
		case 0: dir = ACCU_DIR::extension; break;
		case 1:	dir = ACCU_DIR::parallel; break;
		case 2:	dir = ACCU_DIR::perpendicular; break;
		}
		GetActiveDlgDoc()->m_accuBox.OnUpdateData(1,dir);
//		AfxGetMainWnd()->SendMessage(FCCM_UPDATE_ACCUDATA,1,dir);
//		AfxCallMessage(FCCM_UPDATE_ACCUDATA,1,dir);
	}
	else
		GetActiveDlgDoc()->m_accuBox.OnUpdateData(1,ACCU_DIR::invalid);
//		AfxGetMainWnd()->SendMessage(FCCM_UPDATE_ACCUDATA,1,ACCU_DIR::invalid);
//		AfxCallMessage(FCCM_UPDATE_ACCUDATA,1,ACCU_DIR::invalid);

	PaintLockDirBtn();
}

void CAccuDlg::OnUnlockDir() 
{
	m_bLockingDir = FALSE;
	GetActiveDlgDoc()->m_accuBox.OnUpdateData(2,0);
//	AfxGetMainWnd()->SendMessage(FCCM_UPDATE_ACCUDATA,2,0);
//	AfxCallMessage(FCCM_UPDATE_ACCUDATA,2,0);
	PaintLockDirBtn();
}

void CAccuDlg::OnSelchangeDir() 
{
	UpdateData(TRUE);
	m_bLockingDir = FALSE;
	OnLockDir();
}
*/
/*
void CAccuDlg::PaintLockDirBtn()
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_BUTTON_ACCU_LOCKDIR);
	if( pBtn )
	{
		if( m_bLockingDir )
			pBtn->SetWindowText(StrFromResID(IDS_ACCUCANCEL));
		else
			pBtn->SetWindowText(StrFromResID(IDS_ACCULOCKDIR));
	}

	CWnd *pCtrl = GetDlgItem(IDC_COMBO_ACCU_DIR);
	if( pCtrl )
	{
		pCtrl->EnableWindow(m_bLockingDir);
	}
}
*/




/////////////////////////////////////////////////////////////////////////////
// CAccuDlg Dialog

CAccuDlg::CAccuDlg(CWnd* pParent /*=NULL*/)
	: CReflectKeyDialog(CAccuDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAccuDlg)
	m_lfX = 0.0f;
	m_lfY = 0.0f;
	m_lfDis = 0.0f;
	m_lfAng = 0.0f;
	m_bLockX = FALSE;
	m_bLockY = FALSE;
	m_bLockDis = FALSE;
	m_bLockAng = FALSE;
	m_bRelativeCoord = TRUE;
	m_bExtension = FALSE;
	m_bParallel = FALSE;
	m_bPerpendicular = FALSE;
	//}}AFX_DATA_INIT
	m_nDir = 0;
	m_nStatus = 0;
	m_bLockingDir = FALSE;
}

void CAccuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAccuDlg)	
	DDX_Text_Silence(pDX, IDC_EDIT_ACCUY, m_lfY);
	DDX_Text_Silence(pDX, IDC_EDIT_ACCUX, m_lfX);
	DDX_Check(pDX, IDC_CHECK_ACCUX_LOCK, m_bLockX);
	DDX_Check(pDX, IDC_CHECK_ACCUY_LOCK, m_bLockY);	
	DDX_Text_Silence(pDX, IDC_EDIT_ACCUDIS, m_lfDis);
	DDX_Text_Silence(pDX, IDC_EDIT_ACCUANG, m_lfAng);
	DDX_Check(pDX, IDC_CHECK_ACCUDIS_LOCK, m_bLockDis);
	DDX_Check(pDX, IDC_CHECK_ACCUANG_LOCK, m_bLockAng);
	DDX_Check(pDX, IDC_CHECK_ACCU_RELATIVE_COORD, m_bRelativeCoord);
	DDX_Control(pDX, IDC_STATIC_ACCU_STATUS, m_stcStatus);
	DDX_Check(pDX, IDC_CHECK_ACCU_EXTENSION, m_bExtension);
	DDX_Check(pDX, IDC_CHECK_ACCU_PARALLEL, m_bParallel);
	DDX_Check(pDX, IDC_CHECK_ACCU_PERPENDICULAR, m_bPerpendicular);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAccuDlg, CReflectKeyDialog)
	//{{AFX_MSG_MAP(CAccuDlg)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_ACCUX, OnChangeEditAccux)
	ON_EN_CHANGE(IDC_EDIT_ACCUY, OnChangeEditAccuy)
	ON_EN_CHANGE(IDC_EDIT_ACCUDIS, OnChangeEditAccudis)
	ON_EN_CHANGE(IDC_EDIT_ACCUANG, OnChangeEditAccuang)
	ON_BN_CLICKED(IDC_CHECK_ACCUX_LOCK, OnCheckAccuxLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUY_LOCK, OnCheckAccuyLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUDIS_LOCK, OnCheckAccudisLock)
	ON_BN_CLICKED(IDC_CHECK_ACCUANG_LOCK, OnCheckAccuangLock)
	ON_BN_CLICKED(IDC_CHECK_ACCU_RELATIVE_COORD, OnCheckCoordSys)
	ON_BN_CLICKED(IDC_CHECK_ACCU_EXTENSION, OnCheckExtension)
	ON_BN_CLICKED(IDC_CHECK_ACCU_PARALLEL, OnCheckParallel)
	ON_BN_CLICKED(IDC_CHECK_ACCU_PERPENDICULAR, OnCheckPerpendicular)
	ON_BN_CLICKED(IDC_BUTTON_ACCU_UNLOCKDIR, OnUnlockDir)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CAccuDlg::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN || pMsg->message==WM_SYSKEYDOWN )
	{
		if( pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE )
		{
			pMsg->hwnd = m_hWnd;
			return FALSE;
		}
	}
	
	return CReflectKeyDialog::PreTranslateMessage(pMsg);
}

void CAccuDlg::Show(WPARAM code, LPARAM param)
{
	if( code==0 )
	{
		ACCU_DATA *data = (ACCU_DATA*)param;
		if( data )
		{			
			m_lfX = data->pt.x; 
			m_lfY = data->pt.y; 			
			m_bLockX = data->bLockX; 
			m_bLockY = data->bLockY; 
			
			m_lfDis = data->dis; 
			m_lfAng = data->ang*180/PI; 			
			m_bLockDis = data->bLockDis; 
			m_bLockAng = data->bLockAng;
			
			m_nStatus    = data->status;
			m_bRelativeCoord = data->bRelativeCoord;

			UpdateData (FALSE);
			UpdateStatusString();
		}
	}
	else if( code==1 )
	{
		switch(param)
		{
		case ACCU_DIR::invalid:
			m_bExtension = FALSE;
			m_bParallel = FALSE;
			m_bPerpendicular = FALSE;
			m_bLockingDir = FALSE;
			break;
		case ACCU_DIR::extension:
			m_bExtension = TRUE;
			m_bLockingDir = TRUE;
			break;
		case ACCU_DIR::parallel:
			m_bParallel = TRUE;
			m_bLockingDir = TRUE;
			break;
		case ACCU_DIR::perpendicular:
			m_bPerpendicular = TRUE;
			m_bLockingDir = TRUE;
			break;
		}
	}

	UpdateStatusString();
	
	BOOL bVisible = IsWindowVisible();	
	if( bVisible )ShowWindow(SW_SHOW);
	else ShowWindow(SW_HIDE);

	HWND hWnd = ::GetFocus();
	if( hWnd )::SetFocus(hWnd);

	UpdateData(FALSE);
}

BOOL CAccuDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();	
	if(pApp!=NULL)
	{
		// ¶Á×¢²áµÇ¼Ç±í
		int x = pApp->GetProfileIntA(strOptsRegEntryName, REG_ENTRY_WP_ACCUX, 600);
		int y = pApp->GetProfileIntA(strOptsRegEntryName, REG_ENTRY_WP_ACCUY, 150);
		SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE);
	}

	return TRUE;
}

void CAccuDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();	
	if(pApp!=NULL)
	{
		// Ð´×¢²áµÇ¼Ç±í
		pApp->WriteProfileInt(strOptsRegEntryName,REG_ENTRY_WP_ACCUX, wp.rcNormalPosition.left);
		pApp->WriteProfileInt(strOptsRegEntryName, REG_ENTRY_WP_ACCUY, wp.rcNormalPosition.top);
	}	
}

void CAccuDlg::UpdateAccuData()
{
	ACCU_DATA data;
	
	data.pt.x = m_lfX ; 
	data.pt.y = m_lfY ; 	
	data.bLockX = m_bLockX; 
	data.bLockY = m_bLockY; 
	
	data.dis = m_lfDis; 
	data.ang = m_lfAng*PI/180; 	
	data.bLockDis = m_bLockDis; 
	data.bLockAng = m_bLockAng; 	
	
	data.bRelativeCoord = m_bRelativeCoord;
	data.status = m_nStatus;

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(pDoc)pDoc->m_accuBox.OnUpdateData(0,(LPARAM)&data);
}

void CAccuDlg::UpdateStatusString()
{
	CString str = StrFromResID(IDS_ACCUSTATUS);
	CStringArray arr;
	CString tmp;

	if( m_bLockX )
	{
		tmp = "X";
		arr.Add( tmp );
	}
	if( m_bLockY )
	{
		tmp = "Y";
		arr.Add( tmp );
	}
	if( m_bLockDis )
	{
		tmp = StrFromResID(IDS_ACCU_DISTANCE);
		arr.Add( tmp );
	}
	if( m_bLockAng )
	{
		tmp = StrFromResID(IDS_ACCU_ANGLE);
		arr.Add( tmp );
	}

	int n = -1;
	const ACCU_DIR* pDirs = GetActiveDlgDoc()->m_accuBox.GetLockedDirs(n);
	for(int j = 0; j < n; j++)
	{
		ACCU_DIR dir = pDirs[j];
		if( dir.type == ACCU_DIR::extension )
		{
			tmp = StrFromResID(IDS_ACCU_EXTENSION);
			arr.Add( tmp );
		}
		else if( dir.type == ACCU_DIR::parallel )
		{
			tmp = StrFromResID(IDS_ACCU_PARALLEL);
			arr.Add( tmp );
		}
		else if( dir.type == ACCU_DIR::perpendicular)
		{
			tmp = StrFromResID(IDS_ACCU_PERPENDICULAR);
			arr.Add( tmp );
		}
	}


	int num = arr.GetSize();
	if(num>0) 
	{
		str += arr.GetAt(0);
		for(int i = 1; i < num; i++)
			str = str + "+" + arr.GetAt(i);
	}
	
	m_stcStatus.SetWindowText(str);
	m_stcStatus.RedrawWindow();
}

void CAccuDlg::OnCheckCoordSys() 
{
	UpdateData(TRUE);
	GetActiveDlgDoc()->m_accuBox.SetCoordSysBase(m_bRelativeCoord);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuxLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuyLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccudisLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckAccuangLock() 
{
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccux() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUX_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccuy() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUY_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccudis() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUDIS_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnChangeEditAccuang() 
{
	CButton *pBtn = (CButton*)GetDlgItem(IDC_CHECK_ACCUANG_LOCK);
	if( pBtn )pBtn->SetCheck(1);
	UpdateData(TRUE);
	UpdateStatusString();
	UpdateAccuData();
}

void CAccuDlg::OnCheckExtension()
{
	UpdateData(TRUE);
	UpdateStatusString();

	m_bParallel = FALSE;
	m_bPerpendicular = FALSE;
	UpdateData(FALSE);

	int dir = ACCU_DIR::invalid;
	if( m_bExtension )
	{
		m_bLockingDir = TRUE;
		dir = ACCU_DIR::extension;
	}
	else
	{
		m_bLockingDir = FALSE;
	}
	GetActiveDlgDoc()->m_accuBox.OnUpdateData(1,dir);
}

void CAccuDlg::OnCheckParallel()
{
	UpdateData(TRUE);
	UpdateStatusString();

	m_bExtension = FALSE;
	m_bPerpendicular = FALSE;
	UpdateData(FALSE);

	int dir = ACCU_DIR::invalid;
	if( m_bParallel )
	{
		m_bLockingDir = TRUE;
		dir = ACCU_DIR::parallel;
	}
	else
	{
		m_bLockingDir = FALSE;
	}
	GetActiveDlgDoc()->m_accuBox.OnUpdateData(1,dir);
}

void CAccuDlg::OnCheckPerpendicular()
{
	UpdateData(TRUE);
	UpdateStatusString();

	m_bExtension = FALSE;
	m_bParallel = FALSE;
	UpdateData(FALSE);

	int dir = ACCU_DIR::invalid;
	if( m_bPerpendicular )
	{
		m_bLockingDir = TRUE;
		dir = ACCU_DIR::perpendicular;
	}
	else
	{
		m_bLockingDir = FALSE;
	}
	GetActiveDlgDoc()->m_accuBox.OnUpdateData(1,dir);
}

void CAccuDlg::OnUnlockDir() 
{
	m_bLockingDir = FALSE;
	m_bParallel = FALSE;
	m_bPerpendicular = FALSE;
	m_bLockingDir = FALSE;

	GetActiveDlgDoc()->m_accuBox.OnUpdateData(2,0);
}
