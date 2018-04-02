// DlgViewAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "DlgViewAdjust.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgViewAdjust dialog


CDlgViewAdjust::CDlgViewAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgViewAdjust::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgViewAdjust)
	m_brightness = 0;
	m_contrast = 0;
	m_bLBlue = TRUE;
	m_bLGreen = TRUE;
	m_bLRed = TRUE;
	m_bRBlue = TRUE;
	m_bRGreen = TRUE;
	m_bRRed = TRUE;
	m_bLAutoHistogram = FALSE;
	m_bRAutoHistogram = FALSE;
	m_bSameRight = FALSE;
	m_bShowRight = TRUE;
	//}}AFX_DATA_INIT
	memset(&m_updateMsg,0,sizeof(m_updateMsg));
	m_bInitDialog = FALSE;
	m_initb[1] = m_initb[0] = 0;
	m_initc[1] = m_initc[0] = 0;
	m_bShowBand = FALSE;

	m_pObj = NULL;
	m_pFunc = NULL;
}


void CDlgViewAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgViewAdjust)
	DDX_Text(pDX, IDC_EDIT_BRIGHTNESS, m_brightness);
	DDV_MinMaxInt(pDX, m_brightness, -100, 100);
	DDX_Text(pDX, IDC_EDIT_CONTRAST, m_contrast);
	DDV_MinMaxInt(pDX, m_contrast, -100, 100);

	DDX_Text(pDX, IDC_EDIT_BRIGHTNESS2, m_brightness2);
	DDV_MinMaxInt(pDX, m_brightness2, -100, 100);
	DDX_Text(pDX, IDC_EDIT_CONTRAST2, m_contrast2);
	DDV_MinMaxInt(pDX, m_contrast2, -100, 100);

	DDX_Check(pDX, IDC_CHECK_LBLUE, m_bLBlue);
	DDX_Check(pDX, IDC_CHECK_LGREEN, m_bLGreen);
	DDX_Check(pDX, IDC_CHECK_LRED, m_bLRed);
	DDX_Check(pDX, IDC_CHECK_RBLUE, m_bRBlue);
	DDX_Check(pDX, IDC_CHECK_RGREEN, m_bRGreen);
	DDX_Check(pDX, IDC_CHECK_RRED, m_bRRed);

	DDX_Check(pDX, IDC_CHECK_HISTOGRAM, m_bLAutoHistogram);
	DDX_Check(pDX, IDC_CHECK_HISTOGRAM2, m_bRAutoHistogram);
	DDX_Check(pDX, IDC_CHECK_SAMETOLEFT, m_bSameRight);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgViewAdjust, CDialog)
	//{{AFX_MSG_MAP(CDlgViewAdjust)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRIGHTNESS, OnDeltaposSpinBrightness)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CONTRAST, OnDeltaposSpinContrast)
	ON_EN_CHANGE(IDC_EDIT_BRIGHTNESS, OnChangeEditBrightness)
	ON_EN_CHANGE(IDC_EDIT_CONTRAST, OnChangeEditContrast)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BRIGHTNESS2, OnDeltaposSpinBrightness2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_CONTRAST2, OnDeltaposSpinContrast2)
	ON_EN_CHANGE(IDC_EDIT_BRIGHTNESS2, OnChangeEditBrightness2)
	ON_EN_CHANGE(IDC_EDIT_CONTRAST2, OnChangeEditContrast2)

	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_LRED, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_LGREEN, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_LBLUE, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_RRED, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_RGREEN, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_RBLUE, OnCheckBank)

	ON_BN_CLICKED(IDC_CHECK_HISTOGRAM, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_HISTOGRAM2, OnCheckBank)
	ON_BN_CLICKED(IDC_CHECK_SAMETOLEFT, OnCheckSametoleft)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgViewAdjust message handlers

BOOL CDlgViewAdjust::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_BRIGHTNESS );
	pSpin->SetRange(-100,100);
	pSpin->SetPos(m_brightness);

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_CONTRAST );
	pSpin->SetRange(-100,100);
	pSpin->SetPos(m_contrast);

	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_BRIGHTNESS );
	pSlider->SetRange(-100,100);
	pSlider->SetPageSize(20);
	pSlider->SetLineSize(20);
	pSlider->SetTicFreq(20);
	pSlider->SetPos(m_brightness);	

	pSlider = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_CONTRAST );
	pSlider->SetRange(-100,100);
	pSlider->SetPageSize(20);
	pSlider->SetLineSize(20);
	pSlider->SetTicFreq(20);
	pSlider->SetPos(m_contrast);

	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_BRIGHTNESS2 );
	pSpin->SetRange(-100,100);
	pSpin->SetPos(m_brightness2);
	
	pSpin = (CSpinButtonCtrl*)GetDlgItem( IDC_SPIN_CONTRAST2 );
	pSpin->SetRange(-100,100);
	pSpin->SetPos(m_contrast2);
	
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_BRIGHTNESS2 );
	pSlider->SetRange(-100,100);
	pSlider->SetPageSize(20);
	pSlider->SetLineSize(20);
	pSlider->SetTicFreq(20);
	pSlider->SetPos(m_brightness2);
	
	pSlider = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_CONTRAST2 );
	pSlider->SetRange(-100,100);
	pSlider->SetPageSize(20);
	pSlider->SetLineSize(20);
	pSlider->SetTicFreq(20);
	pSlider->SetPos(m_contrast2);

	if( !m_bShowRight )
	{
		CRect rcWnd;
		GetWindowRect(&rcWnd);

		CWnd *pWnd = GetDlgItem(IDC_CHECK_SAMETOLEFT);
		CRect rcWnd2;
		pWnd->GetWindowRect(&rcWnd2);

		rcWnd.bottom = rcWnd2.top;
		MoveWindow(&rcWnd);
	}

	CString str;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS);
	if( pEdit )
	{
		str.Format(_T("%d"),m_brightness);
		pEdit->SetWindowText(str);
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST);
	if( pEdit )
	{
		str.Format(_T("%d"),m_contrast);
		pEdit->SetWindowText(str);
	}

	if( pEdit && !m_bShowBand )
	{
		CWnd *pWnd = GetDlgItem(IDC_CHECK_LRED);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_LGREEN);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_LBLUE);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_RRED);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_RGREEN);
		if( pWnd )pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_CHECK_RBLUE);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}

	OnCheckSametoleft();
	m_bInitDialog = TRUE;	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgViewAdjust::OnDeltaposSpinBrightness(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( !m_bInitDialog )return;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
	if( pCtrl )
	{
		pCtrl->SetPos(pNMUpDown->iPos+pNMUpDown->iDelta);
		TRACE("OnSpinBrightness\n");
		UpdateViewWnd();
	}
	
	*pResult = 0;
}

void CDlgViewAdjust::OnDeltaposSpinContrast(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( !m_bInitDialog )return;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
	if( pCtrl )
	{
		pCtrl->SetPos(pNMUpDown->iPos+pNMUpDown->iDelta);
		TRACE("OnSpinContrast\n");
		UpdateViewWnd();
	}

	*pResult = 0;
}

void CDlgViewAdjust::OnChangeEditBrightness() 
{
	if( !m_bInitDialog )return;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS);
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
	if( pEdit && pCtrl )
	{
		CString str;
		pEdit->GetWindowText(str);
		int pos=0, nmin, nmax;
		_stscanf(str,_T("%d"),&pos);
		pCtrl->GetRange(nmin,nmax);

		if( pos<nmin )pos = nmin;
		else if( pos>nmax )pos = nmax;
		pCtrl->SetPos(pos);
		m_brightness = pos;

		UpdateViewWnd();
	}
}

void CDlgViewAdjust::OnChangeEditContrast() 
{
	if( !m_bInitDialog )return;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST);
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
	if( pEdit && pCtrl )
	{
		CString str;
		pEdit->GetWindowText(str);
		int pos=0, nmin, nmax;
		_stscanf(str,_T("%d"),&pos);
		pCtrl->GetRange(nmin,nmax);

		if( pos<nmin )pos = nmin;
		else if( pos>nmax )pos = nmax;
		pCtrl->SetPos(pos);
		m_contrast = pos;

		TRACE("OnChangeEditContrast\n");
		UpdateViewWnd();
	}
}



void CDlgViewAdjust::OnDeltaposSpinBrightness2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( !m_bInitDialog )return;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS2);
	if( pCtrl )
	{
		pCtrl->SetPos(pNMUpDown->iPos+pNMUpDown->iDelta);
		TRACE("OnSpinBrightness\n");
		UpdateViewWnd();
	}
	
	*pResult = 0;
}

void CDlgViewAdjust::OnDeltaposSpinContrast2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( !m_bInitDialog )return;
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST2);
	if( pCtrl )
	{
		pCtrl->SetPos(pNMUpDown->iPos+pNMUpDown->iDelta);
		TRACE("OnSpinContrast\n");
		UpdateViewWnd();
	}
	
	*pResult = 0;
}

void CDlgViewAdjust::OnChangeEditBrightness2() 
{
	if( !m_bInitDialog )return;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS2);
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS2);
	if( pEdit && pCtrl )
	{
		CString str;
		pEdit->GetWindowText(str);
		int pos=0, nmin, nmax;
		_stscanf(str,_T("%d"),&pos);
		pCtrl->GetRange(nmin,nmax);
		
		if( pos<nmin )pos = nmin;
		else if( pos>nmax )pos = nmax;
		pCtrl->SetPos(pos);
		m_brightness2 = pos;
		
		UpdateViewWnd();
	}
}

void CDlgViewAdjust::OnChangeEditContrast2() 
{
	if( !m_bInitDialog )return;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST2);
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST2);
	if( pEdit && pCtrl )
	{
		CString str;
		pEdit->GetWindowText(str);
		int pos=0, nmin, nmax;
		_stscanf(str,_T("%d"),&pos);
		pCtrl->GetRange(nmin,nmax);
		
		if( pos<nmin )pos = nmin;
		else if( pos>nmax )pos = nmax;
		pCtrl->SetPos(pos);
		m_contrast2 = pos;

		UpdateViewWnd();
	}
}

void CDlgViewAdjust::UpdateViewWnd()
{
	if( !m_bInitDialog )return;

	if( m_bSameRight )
	{
		m_brightness2 = m_brightness;
		m_contrast2 = m_contrast;
	}

	static DWORD lasttime = 0;
	DWORD curtime = GetTickCount();
	if (curtime - lasttime < 50)
	{
		return;
	}
	else
	{
		lasttime = curtime;
	}

	m_initb[0] = m_brightness;
	m_initc[0] = m_contrast;
	m_initb[1] = m_brightness2;
	m_initc[1] = m_contrast2;

	if( m_updateMsg.hwnd )
	{			
		::PostMessage(m_updateMsg.hwnd,m_updateMsg.message,m_updateMsg.wParam,m_updateMsg.lParam);
	}
	else if( m_pObj && m_pFunc!=NULL )
	{
		(m_pObj->*m_pFunc)(m_updateMsg.wParam,m_updateMsg.lParam);
	}
}

void CDlgViewAdjust::SetService(HWND hWnd, UINT msgId, WPARAM wParam, LPARAM lParam)
{
	m_updateMsg.hwnd   = hWnd;
	m_updateMsg.message= msgId;
	m_updateMsg.wParam = wParam;
	m_updateMsg.lParam = lParam;
}

void CDlgViewAdjust::SetService(CObject *pObj,PFUNCALLBACK func, WPARAM wParam, LPARAM lParam)
{
	m_pObj = pObj;
	m_pFunc = func;

	m_updateMsg.wParam = wParam;
	m_updateMsg.lParam = lParam;
}

void CDlgViewAdjust::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( !m_bInitDialog )return;
	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS);
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS);
	if( pEdit && pCtrl )
	{
		CString str;
		int pos=pCtrl->GetPos();
		if( m_brightness!=pos )
		{
			str.Format(_T("%d"),pos);
			pEdit->SetWindowText(str);
			
			m_brightness = pos;
		}
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST);
	if( pEdit && pCtrl )
	{
		CString str;
		int pos=pCtrl->GetPos();
		if( m_contrast!=pos )
		{
			str.Format(_T("%d"),pos);
			pEdit->SetWindowText(str);
			
			m_contrast = pos;
		}
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS2);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_BRIGHTNESS2);
	if( pEdit && pCtrl )
	{
		CString str;
		int pos=pCtrl->GetPos();
		if( m_brightness2!=pos )
		{
			str.Format(_T("%d"),pos);
			pEdit->SetWindowText(str);
			
			m_brightness2 = pos;
		}
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST2);
	pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_CONTRAST2);
	if( pEdit && pCtrl )
	{
		CString str;
		int pos=pCtrl->GetPos();
		if( m_contrast2!=pos )
		{
			str.Format(_T("%d"),pos);
			pEdit->SetWindowText(str);
			
			m_contrast2 = pos;
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlgViewAdjust::OnCheckSametoleft() 
{
	UpdateData(TRUE);
	
	if( m_bSameRight )
	{
		m_brightness2 = m_brightness;
		m_contrast2 = m_contrast;

		CWnd *pCtrl = GetDlgItem(IDC_EDIT_BRIGHTNESS2);
		if( pCtrl )pCtrl->EnableWindow(FALSE);

		pCtrl = GetDlgItem(IDC_EDIT_CONTRAST2);
		if( pCtrl )pCtrl->EnableWindow(FALSE);
	}
	else
	{
		CWnd *pCtrl = GetDlgItem(IDC_EDIT_BRIGHTNESS2);
		if( pCtrl )pCtrl->EnableWindow(TRUE);
		
		pCtrl = GetDlgItem(IDC_EDIT_CONTRAST2);
		if( pCtrl )pCtrl->EnableWindow(TRUE);
	}

	if( m_bInitDialog )
	{
		CString str1, str2;
		str1.Format(_T("%d"),m_brightness);
		str2.Format(_T("%d"),m_contrast);

		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS);
		if( pEdit )pEdit->SetWindowText(str1);

		pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST);
		if( pEdit )pEdit->SetWindowText(str2);

		if( !m_bSameRight )
		{
			str1.Format(_T("%d"),m_brightness2);
			str2.Format(_T("%d"),m_contrast2);
			
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_BRIGHTNESS2);
			if( pEdit )pEdit->SetWindowText(str1);
			
			pEdit = (CEdit*)GetDlgItem(IDC_EDIT_CONTRAST2);
			if( pEdit )pEdit->SetWindowText(str2);
		}
	}

	UpdateData(FALSE);
}

void CDlgViewAdjust::OnCheckBank() 
{
	UpdateData(TRUE);
	UpdateViewWnd();
}
