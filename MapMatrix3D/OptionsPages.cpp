// OptionsPages.cpp : implementation file
//

#include "stdafx.h"
#include "OptionsPages.h"
#include "EditBase.h"
#include "ExMessage.h"
#include "Snap.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "SmartViewFunctions.h"
#include "Selection.h"
#include "SilenceDDX.h"
#include "UVSModify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Base

IMPLEMENT_DYNCREATE(COp_Page_Base, CMFCPropertyPage)

#pragma warning (disable : 4355)

COp_Page_Base::COp_Page_Base() 
{
}

COp_Page_Base::COp_Page_Base(UINT nIDTemplate, UINT nIDCaption) :
CMFCPropertyPage(nIDTemplate, nIDCaption)
{
	
}

COp_Page_Base::COp_Page_Base(LPCTSTR lpszTemplateName, UINT nIDCaption) :
CMFCPropertyPage(lpszTemplateName, nIDCaption)
{

}


/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_Commonly property page

IMPLEMENT_DYNCREATE(COp_Page_View_Commonly, COp_Page_Base)

BOOL COp_Page_View_Commonly::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// ³õÊ¼»¯ÑÕÉ«ÉèÖÃ°´Å¥
	
 //	m_clrbg.SetColor (m_wndList.GetTextColor ());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=41466B7C02CE
COp_Page_View_Commonly::COp_Page_View_Commonly() : COp_Page_Base(COp_Page_View_Commonly::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_View_Commonly)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C02CF
COp_Page_View_Commonly::~COp_Page_View_Commonly()
{
}

//##ModelId=41466B7C02D0
void COp_Page_View_Commonly::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_View_Commonly)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_View_Commonly, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_View_Commonly)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_Commonly message handlers


/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_Measure property page

IMPLEMENT_DYNCREATE(COp_Page_View_Measure, COp_Page_Base)

//##ModelId=41466B7C0262
COp_Page_View_Measure::COp_Page_View_Measure() : COp_Page_Base(COp_Page_View_Measure::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_View_Measure)
	m_bCenter = FALSE;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C026F
COp_Page_View_Measure::~COp_Page_View_Measure()
{
}

//##ModelId=41466B7C0270
void COp_Page_View_Measure::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_View_Measure)
	//DDX_Check(pDX, IDC_CHECK_CENTER, m_bCenter);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_View_Measure, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_View_Measure)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_Measure message handlers



/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_VectView property page

IMPLEMENT_DYNCREATE(COp_Page_View_VectView, COp_Page_Base)

//##ModelId=41466B7C0262
COp_Page_View_VectView::COp_Page_View_VectView() : COp_Page_Base(COp_Page_View_VectView::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_View_VectView)
	m_bSymbolize = TRUE;
	m_bDriveVector = FALSE;
	m_bCorrectVectViewDisorder = FALSE;
	m_bGoodVectView = TRUE;
	m_bImageZoomNotLinear = FALSE;
//	m_bSolidText = FALSE;
//	m_bWhileBK = FALSE;
	//}}AFX_DATA_INIT
	
}

//##ModelId=41466B7C026F
COp_Page_View_VectView::~COp_Page_View_VectView()
{
}

//##ModelId=41466B7C0270
void COp_Page_View_VectView::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_View_VectView)
	DDX_Control( pDX, IDC_VECT_DRAGCOLOR,m_DragColor);
	DDX_Control( pDX, IDC_VECT_HILITECOLOR,m_HiliteColor);
	DDX_Control( pDX, IDC_VECT_BORDERCOLOR,m_BorderColor);
	DDX_Control( pDX, IDC_VECT_BAKCOLOR,m_BackColor);
	DDX_Check(pDX, IDC_CHECK_SYMBOLIZE, m_bSymbolize);
	DDX_Check(pDX, IDC_CHECK_DRIVEVECTOR, m_bDriveVector);
	DDX_Check(pDX, IDC_CHECK_CORRECTVECTDISORDER, m_bCorrectVectViewDisorder);
	DDX_Control( pDX, IDC_VECT_SPECIALPTCOLOR, m_FtrPtColor);
	DDX_Check( pDX, IDC_CHECK_GOODVECTVIEW, m_bGoodVectView);
	DDX_Check( pDX, IDC_CHECK_IMAGEZOOM_NOTLINEAR, m_bImageZoomNotLinear);
//	DDX_Check(pDX, IDC_CHECK_SOLIDTEXT, m_bSolidText);
//	DDX_Check(pDX, IDC_CHECK_WHILEBK, m_bWhileBK);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(COp_Page_View_VectView, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_View_VectView)
	// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_VECT_DRAGCOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_VECT_HILITECOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_VECT_BORDERCOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SYMBOLIZE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_WHILEBK, OnCheckModified)
	ON_BN_CLICKED(IDC_VECT_BAKCOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_VECT_SPECIALPTCOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_DRIVEVECTOR,OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_CORRECTVECTDISORDER,OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_GOODVECTVIEW,OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_IMAGEZOOM_NOTLINEAR,OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_ImageView message handlers
void COp_Page_View_VectView::OnOK() 
{
	UpdateData(TRUE);
	// Ð´×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	COLORREF clrDrag = m_DragColor.GetColor();
	COLORREF clrHilite = m_HiliteColor.GetColor();
	COLORREF clrBound = m_BorderColor.GetColor();
	COLORREF clrBack = m_BackColor.GetColor();
	COLORREF clrFtrPt = m_FtrPtColor.GetColor();
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_DRAGCOLOR,clrDrag);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_HILITECOLOR,clrHilite);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_BOUNDCOLOR,clrBound);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_BACKCOLOR,clrBack);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_SYMBOLIZE,m_bSymbolize);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_FEATUREPTCOLOR,clrFtrPt);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_DRIVEVECTOR,m_bDriveVector);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_CORRECTVECTDISORDER,m_bCorrectVectViewDisorder);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,"GoodVectView",m_bGoodVectView);
	pApp->WriteProfileInt(REGPATH_VIEW_VECT,"ImageZoomNotLinear",m_bImageZoomNotLinear);
//	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_SOLIDTEXT,m_bSolidText);
//	pApp->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_WHILEBK,m_bWhileBK);
	
	COp_Page_Base::OnOK();
}

void COp_Page_View_VectView::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_View_VectView::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// TODO: Add extra initialization here
	// ¶Á×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	m_DragColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_HiliteColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_BackColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_BorderColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_FtrPtColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	
	int value = gdef_clrDragVect;
	value = pApp->GetProfileInt(REGPATH_VIEW_VECT, REGITEM_DRAGCOLOR,value );
	m_DragColor.SetColor(value);
	
	value = gdef_clrHiVect;
	value = pApp->GetProfileInt(REGPATH_VIEW_VECT, REGITEM_HILITECOLOR,value );
	m_HiliteColor.SetColor(value);

	value = gdef_clrBndVect;
	value = pApp->GetProfileInt(REGPATH_VIEW_VECT, REGITEM_BOUNDCOLOR,value );
	m_BorderColor.SetColor(value);

	value = gdef_clrFtrPt;
	value = pApp->GetProfileInt(REGPATH_VIEW_VECT, REGITEM_FEATUREPTCOLOR,value );
	m_FtrPtColor.SetColor(value);

	value = gdef_clrBackVect;
	value = pApp->GetProfileInt(REGPATH_VIEW_VECT, REGITEM_BACKCOLOR,value );
	m_BackColor.SetColor(value);

	m_bSymbolize = pApp->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_SYMBOLIZE,m_bSymbolize);
	m_bDriveVector = pApp->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_DRIVEVECTOR,m_bDriveVector);

	m_bCorrectVectViewDisorder = pApp->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_CORRECTVECTDISORDER,m_bCorrectVectViewDisorder);
	m_bGoodVectView = pApp->GetProfileInt(REGPATH_VIEW_VECT,"GoodVectView",m_bGoodVectView);
	m_bImageZoomNotLinear = pApp->GetProfileInt(REGPATH_VIEW_VECT,"ImageZoomNotLinear",m_bImageZoomNotLinear);
//	m_bSolidText = pApp->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_SOLIDTEXT,m_bSolidText);
//	m_bWhileBK = pApp->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_WHILEBK,m_bWhileBK);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_ImageView property page

IMPLEMENT_DYNCREATE(COp_Page_View_ImageView, COp_Page_Base)

//##ModelId=41466B7C0262
COp_Page_View_ImageView::COp_Page_View_ImageView() : COp_Page_Base(COp_Page_View_ImageView::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_View_ImageView)
	m_bDoubleScreen = gdef_bDoubleScreen;
	m_bOutImgAlert = gdef_bOutImgAlert;
	m_bSymbolize = TRUE;
	m_bDisableMouse = TRUE;
	m_bFasterMappingForASD40 = FALSE;
	m_bFasterMappingForASD401 = FALSE;
	m_lfExtendDis = 50;
	m_bManualLoadVect = FALSE;
	m_bSupportStretch = FALSE;
	m_bTextModeStereo = gdef_bTextStereo;
	m_nMemNum = 200;
	m_bSharplyZoom = FALSE;
	m_bInterleavedStereo = FALSE;
	m_bReadViewPara = TRUE;
	m_bCorrectFlicker = FALSE;
	m_bModifyheightBy3DMouse = FALSE;
	m_bZoomImageWithCurMouse = FALSE;
	//}}AFX_DATA_INIT
	m_nDragLineWid = 0;
	m_nBlockSize = 256;
}

//##ModelId=41466B7C026F
COp_Page_View_ImageView::~COp_Page_View_ImageView()
{
}

//##ModelId=41466B7C0270
void COp_Page_View_ImageView::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_View_ImageView)
	DDX_Check(pDX, IDC_CHECK_DOUBLESCREEN, m_bDoubleScreen);
	DDX_Check(pDX, IDC_CHECK_OUTIMGALERT, m_bOutImgAlert);
	DDX_Control( pDX, IDC_IMG_DRAGCOLOR,m_DragColor);
	DDX_Control( pDX, IDC_IMG_HILITECOLOR,m_HiliteColor);
	DDX_Check(pDX, IDC_CHECK_SYMBOLIZE, m_bSymbolize);
	DDX_Check(pDX, IDC_CHECK_DISABLEMOUSE, m_bDisableMouse);
	DDX_Check(pDX, IDC_CHECK_ADS40FASTLOAD, m_bFasterMappingForASD40);
	DDX_Check(pDX, IDC_CHECK_ADS40FASTLOAD1, m_bFasterMappingForASD401);
	DDX_Text(pDX, IDC_EDIT_EXTEND, m_lfExtendDis);
	DDX_Check(pDX, IDC_CHECK_MANUALLOADVECT, m_bManualLoadVect);
	DDX_Check(pDX,IDC_CHECK_SUPPORTSTRETCH, m_bSupportStretch);
	DDX_Check(pDX,IDC_CHECK_GOODSTEREO, m_bTextModeStereo);	
	DDX_Text(pDX,IDC_EDIT_MEMNUM, m_nMemNum);	
	DDX_Check(pDX, IDC_CHECK_SHARPLYZOOM, m_bSharplyZoom);
	DDX_Check(pDX, IDC_CHECK_INTERLEAVEDSTEREO, m_bInterleavedStereo);
	DDX_Check(pDX, IDC_READVIEWPARA_CHECK, m_bReadViewPara);
	DDX_Check(pDX, IDC_CHECK_CORRECTFLICKER, m_bCorrectFlicker);
	DDX_Check(pDX, IDC_3DMOUSEMODIFYHEIGHT_CHECK, m_bModifyheightBy3DMouse);
	DDX_Check(pDX, IDC_CHECK_CURMOUSE_ZOOM, m_bZoomImageWithCurMouse);
	DDX_Text_Silence(pDX, IDC_EDIT_DRAGLINE_WIDTH, m_nDragLineWid);
	DDX_Control( pDX, IDC_COMBO_BLOCKSIZE,m_wndBlockSize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_View_ImageView, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_View_ImageView)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_CHECK_GOODSTEREO,OnGoodStereo)
	ON_BN_CLICKED(IDC_CHECK_DOUBLESCREEN, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_OUTIMGALERT, OnCheckModified)
	ON_BN_CLICKED(IDC_IMG_DRAGCOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_IMG_HILITECOLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SYMBOLIZE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_DISABLEMOUSE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_ADS40FASTLOAD, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_ADS40FASTLOAD, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_MANUALLOADVECT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SUPPORTSTRETCH, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_MEMNUM, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_DRAGLINE_WIDTH, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SHARPLYZOOM, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_INTERLEAVEDSTEREO, OnCheckModified)
	ON_BN_CLICKED(IDC_READVIEWPARA_CHECK, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_CORRECTFLICKER, OnCheckModified)
	ON_BN_CLICKED(IDC_3DMOUSEMODIFYHEIGHT_CHECK, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_CURMOUSE_ZOOM, OnCheckModified)
	ON_CBN_SELCHANGE(IDC_COMBO_BLOCKSIZE,OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_ImageView message handlers
void COp_Page_View_ImageView::OnOK() 
{
	UpdateData(TRUE);
	// Ð´×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();

	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, m_bDoubleScreen);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_OUTIMGALERT, m_bOutImgAlert);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_SYMBOLIZE,m_bSymbolize);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_DIABLEMOUSE,m_bDisableMouse);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_FASTERMAPPING,m_bFasterMappingForASD40);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG, "FasterMapping1", m_bFasterMappingForASD401);
	WriteProfileDouble(REGPATH_VIEW_IMG, "lfExtendDis", m_lfExtendDis);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_MANUALLOADVECT,m_bManualLoadVect);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_SUPPORTSTRETCH,m_bSupportStretch);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_TEXTSTEREO,m_bTextModeStereo);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_MEMNUM,m_nMemNum);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_SHARPLYZOOM,m_bSharplyZoom);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_INTERLEAVEDSTEREO,m_bInterleavedStereo);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_READVIEWPARA,m_bReadViewPara);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_MODIFYHEIGHTBY3DMOUSE,m_bModifyheightBy3DMouse);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_ZOOMIMAGEWITHCURMOUSE,m_bZoomImageWithCurMouse);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,"CorrectFlicker",m_bCorrectFlicker);

	COLORREF clrDrag = m_DragColor.GetColor();
	COLORREF clrHilite = m_HiliteColor.GetColor();
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_DRAGCOLOR,clrDrag);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_HILITECOLOR,clrHilite);
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_DRAGLINE_WID,m_nDragLineWid);
	
	CString strBlockSize;
	int nsel = m_wndBlockSize.GetCurSel();
	m_wndBlockSize.GetLBText(nsel,strBlockSize);
	
	m_nBlockSize = atol(strBlockSize);
	
	pApp->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_BLOCKSIZE,m_nBlockSize);
	
	COp_Page_Base::OnOK();
}

void COp_Page_View_ImageView::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_View_ImageView::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// TODO: Add extra initialization here
	// ¶Á×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	m_bDoubleScreen	= pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DOUBLESCREEN, m_bDoubleScreen);
	m_bOutImgAlert  = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_OUTIMGALERT,m_bOutImgAlert);
	m_bSymbolize    = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_SYMBOLIZE,m_bSymbolize);
	m_bDisableMouse  = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DIABLEMOUSE,m_bDisableMouse);
	m_bFasterMappingForASD40 = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_FASTERMAPPING,m_bFasterMappingForASD40);
	m_bFasterMappingForASD401 = pApp->GetProfileInt(REGPATH_VIEW_IMG, "FasterMapping1", m_bFasterMappingForASD401);
	m_lfExtendDis = GetProfileDouble(REGPATH_VIEW_IMG, "lfExtendDis", m_lfExtendDis);
	m_bManualLoadVect = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MANUALLOADVECT,m_bManualLoadVect);
	m_bSupportStretch = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_SUPPORTSTRETCH,m_bSupportStretch);
	m_bTextModeStereo = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_TEXTSTEREO,m_bTextModeStereo);
	m_nMemNum = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MEMNUM,m_nMemNum);
	m_bSharplyZoom = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_SHARPLYZOOM,m_bSharplyZoom);
	m_bInterleavedStereo = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_INTERLEAVEDSTEREO,m_bInterleavedStereo);
	m_bReadViewPara = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_READVIEWPARA,m_bReadViewPara);
	m_bModifyheightBy3DMouse = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_MODIFYHEIGHTBY3DMOUSE,m_bModifyheightBy3DMouse);
	m_bZoomImageWithCurMouse = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_ZOOMIMAGEWITHCURMOUSE,m_bZoomImageWithCurMouse);
	m_bCorrectFlicker = pApp->GetProfileInt(REGPATH_VIEW_IMG,"CorrectFlicker",m_bCorrectFlicker);
	m_nDragLineWid = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_DRAGLINE_WID,m_nDragLineWid);
	m_nBlockSize = pApp->GetProfileInt(REGPATH_VIEW_IMG,REGITEM_BLOCKSIZE,m_nBlockSize);

	CWnd *pWnd = GetDlgItem(IDC_EDIT_MEMNUM);
	if( pWnd )
		pWnd->EnableWindow(m_bTextModeStereo);

	m_DragColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_HiliteColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);

	int value = gdef_clrDragImg;
	value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG, REGITEM_DRAGCOLOR,value );
	m_DragColor.SetColor(value);

	value = gdef_clrHiImg;
	value = AfxGetApp()->GetProfileInt(REGPATH_VIEW_IMG, REGITEM_HILITECOLOR,value );
	m_HiliteColor.SetColor(value);
	
	CString strBlockSize;
	strBlockSize.Format("%d",m_nBlockSize);
	m_wndBlockSize.SelectString(0,strBlockSize);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_View_ImageView::OnGoodStereo()
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_MEMNUM);
	if( !pWnd )return;

	UpdateData(TRUE);
	if( pWnd )pWnd->EnableWindow(m_bTextModeStereo);

	pWnd = GetDlgItem(IDC_CHECK_SHARPLYZOOM);
	if( pWnd )pWnd->EnableWindow(m_bTextModeStereo);

	pWnd = GetDlgItem(IDC_CHECK_CORRECTFLICKER);
	if( pWnd )pWnd->EnableWindow(m_bTextModeStereo);

	pWnd = GetDlgItem(IDC_COMBO_BLOCKSIZE);
	if( pWnd )pWnd->EnableWindow(m_bTextModeStereo);

	SetModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Snap property page

IMPLEMENT_DYNCREATE(COp_Page_Doc_Snap, COp_Page_Base)

//##ModelId=41466B7C0290
COp_Page_Doc_Snap::COp_Page_Doc_Snap() : COp_Page_Base(COp_Page_Doc_Snap::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Doc_Snap)
	m_PerpPoint = FALSE;
	m_CenterPoint = FALSE;
	m_TangPoint = FALSE;
	m_bEnable   = TRUE;
	m_Intersect = FALSE;
	m_KeyPoint  = FALSE;
	m_MidPoint  = FALSE;
	m_NearPoint = FALSE;
	m_SnapSelf  = TRUE;
	m_nDistance = gdef_nSnapSize;
	m_b3D		= FALSE;
	m_bShowSnap = gdef_bShowSnap;
	m_bOnlyBaseLine = FALSE;
	m_EndPoint = FALSE;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C0291
COp_Page_Doc_Snap::~COp_Page_Doc_Snap()
{
}

//##ModelId=41466B7C0292
void COp_Page_Doc_Snap::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Doc_Snap)
	DDX_Radio(pDX , IDC_RADIO_PLANAR,m_b3D);
	DDX_Check(pDX, IDC_CHECK_ENDPOINT, m_EndPoint);
	DDX_Check(pDX, IDC_CHECK_SHOWSNAP, m_bShowSnap);
	DDX_Check(pDX, IDC_CHECK_APEAK_DOT, m_PerpPoint);
	DDX_Check(pDX, IDC_CHECK_CENTER_DOT, m_CenterPoint);
	DDX_Check(pDX, IDC_CHECK_CUT_DOT, m_TangPoint);
	DDX_Check(pDX, IDC_CHECK_ENABLE, m_bEnable);
	DDX_Check(pDX, IDC_CHECK_INTERSECT_DOT, m_Intersect);
	DDX_Check(pDX, IDC_CHECK_KEY_DOT, m_KeyPoint);
	DDX_Check(pDX, IDC_CHECK_MIDL_DOT, m_MidPoint);
	DDX_Check(pDX, IDC_CHECK_NEAREST_DOT, m_NearPoint);
	DDX_Check(pDX, IDC_CHECK_SNAPSELF, m_SnapSelf);
	DDX_Check(pDX, IDC_CHECK_SNAPBASE, m_bOnlyBaseLine);
	DDX_Text(pDX, IDC_EDIT_DISTANCE, m_nDistance);
	DDV_MinMaxDouble(pDX, m_nDistance, 1, 1000);
	DDX_Control( pDX, IDC_SNAP_COLOR,m_SnapColor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COp_Page_Doc_Snap, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Doc_Snap)
	ON_BN_CLICKED(IDC_RADIO_PLANAR, OnCheckModified)
	ON_BN_CLICKED(IDC_RADIO_THREE, OnCheckModified)
	ON_BN_CLICKED(IDC_SNAP_COLOR, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_ENDPOINT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SHOWSNAP, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_APEAK_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_CENTER_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_CUT_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_ENABLE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_INTERSECT_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_KEY_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_MIDL_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_NEAREST_DOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SNAPSELF, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SNAPBASE, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_DISTANCE, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Snap message handlers

void COp_Page_Doc_Snap::OnOK() 
{
	SNAPSETTINGS setting;
	memset(&setting,0,sizeof(setting));
	
	UpdateData();
	if(m_PerpPoint)  setting.nMode  |= CSnap::modePerpPoint;
	if(m_CenterPoint)setting.nMode  |= CSnap::modeCenterPoint;
	if(m_TangPoint)	 setting.nMode  |= CSnap::modeTangPoint;
	if(m_Intersect)  setting.nMode  |= CSnap::modeIntersect;
	if(m_KeyPoint)   setting.nMode  |= CSnap::modeKeyPoint;
	if(m_MidPoint)   setting.nMode  |= CSnap::modeMidPoint;
	if(m_NearPoint)  setting.nMode  |= CSnap::modeNearPoint;
	if(m_EndPoint)   setting.nMode  |= CSnap::modeEndPoint;
	
	setting.lfRadius = m_nDistance;
	setting.bOpen    = m_bEnable;
	setting.IS2D     = !m_b3D; 
	setting.bSnapSelf= m_SnapSelf;
	setting.bOnlyBase= m_bOnlyBaseLine;
	
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_RADIUS,m_nDistance);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_ENABLE,setting.bOpen);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_IS2D,setting.IS2D);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_CENTER,m_CenterPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_INTERSECT,m_Intersect);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_KEY,m_KeyPoint);	
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_MID,m_MidPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_NEAR,m_NearPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_PERP,m_PerpPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_TANG,m_TangPoint);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_SHOWSNAP,m_bShowSnap);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_SNAPSELF,m_SnapSelf);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_SNAPBASE,m_bOnlyBaseLine);
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_END,m_EndPoint);

	COLORREF clrSnap = m_SnapColor.GetColor();
	AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_SNAPCOLOR,clrSnap);
	
//	AfxCallMessage(FCCM_SNAPSETTING,0, (LPARAM)&setting);
	
	COp_Page_Base::OnOK();
}

void COp_Page_Doc_Snap::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_Doc_Snap::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	m_nDistance     =  AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_RADIUS,m_nDistance);
	
	m_bEnable       = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_ENABLE,TRUE);
	m_b3D           = !AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_IS2D,TRUE);
	m_EndPoint		= AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_END,0);
	m_CenterPoint   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_CENTER,0);
	m_Intersect     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_INTERSECT,0);
	m_KeyPoint      = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_KEY,TRUE);	
	m_MidPoint      = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_MID,0);
	m_NearPoint     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_NEAR,0);
	m_PerpPoint     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_PERP,0);
	m_TangPoint     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_TANG,0);
	m_bShowSnap     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SHOWSNAP,m_bShowSnap);
	m_SnapSelf		= AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SNAPSELF,m_SnapSelf);
	m_bOnlyBaseLine = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SNAPBASE,m_bOnlyBaseLine);

	m_SnapColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	int value = RGB(255,255,0);
	value = AfxGetApp()->GetProfileInt(REGPATH_SNAP, REGITEM_SNAPCOLOR,value );
	m_SnapColor.SetColor(value);
	
	UpdateData(FALSE);
	return TRUE; 
	
}


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Save property page

IMPLEMENT_DYNCREATE(COp_Page_Doc_Save, COp_Page_Base)

//##ModelId=41466B7C02AF
COp_Page_Doc_Save::COp_Page_Doc_Save() : COp_Page_Base(COp_Page_Doc_Save::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Doc_Save)
	m_bAutoSave = TRUE;
	m_nSaveTime = 60;
	m_bUseCache = TRUE;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C02B0
COp_Page_Doc_Save::~COp_Page_Doc_Save()
{
}

//##ModelId=41466B7C02B1
void COp_Page_Doc_Save::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Doc_Save)
	DDX_Check(pDX, IDC_CHECK_AUTOSAVE, m_bAutoSave);
	DDX_Text(pDX, IDC_EDIT_SAVETIME, m_nSaveTime);
	DDX_Check(pDX, IDC_CHECK_DELAY_SAVE, m_bUseCache);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_Doc_Save, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Doc_Save)
	ON_BN_CLICKED(IDC_CHECK_AUTOSAVE, OnAutosave)
	ON_EN_CHANGE(IDC_EDIT_SAVETIME, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_DELAY_SAVE, OnDelaysave)
	ON_EN_CHANGE(IDC_EDIT_OPERATOR_COUNT, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Save message handlers


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_User property page

IMPLEMENT_DYNCREATE(COp_Page_Doc_User, COp_Page_Base)

//##ModelId=41466B7C0290
COp_Page_Doc_User::COp_Page_Doc_User() : COp_Page_Base(COp_Page_Doc_User::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Doc_User)
	m_bRCLKSwitch = TRUE;
	m_bPlaceProperties = FALSE;
	m_bStereoMidButton = FALSE;
	m_bSliderZoom = FALSE;
	m_bMouseWheelZoom = FALSE;
	m_nRecentlyCmd = 20;
	m_nRecentlyLayer = 40;
	m_bCADStyle = FALSE;
	m_bFocusColletion = FALSE;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C0291
COp_Page_Doc_User::~COp_Page_Doc_User()
{
}

//##ModelId=41466B7C0292
void COp_Page_Doc_User::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Doc_User)
	DDX_Check(pDX, IDC_CHECK_RCLK_SWITCH, m_bRCLKSwitch);
	DDX_Check(pDX, IDC_CHECK_PLACEPROPERTIES, m_bPlaceProperties);
	DDX_Check(pDX, IDC_CHECK_STEREOMIDBUTTON, m_bStereoMidButton);
	DDX_Check(pDX, IDC_CHECK_SLIDER_ZOOM, m_bSliderZoom);
	DDX_Check(pDX, IDC_CHECK_MOUSEWHEEL_ZOOM, m_bMouseWheelZoom);
	DDX_Check(pDX, IDC_CHECK_CADSTYLE, m_bCADStyle);
	DDX_Check(pDX, IDC_CHECK_LOCATE_COLLECTION, m_bFocusColletion);
	DDX_Text(pDX, IDC_EDIT_RECENTCMD1, m_nRecentlyCmd);
	DDX_Text(pDX, IDC_EDIT_RECENTLAYER, m_nRecentlyLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_Doc_User, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Doc_User)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_CHECK_RCLK_SWITCH, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_PLACEPROPERTIES, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_STEREOMIDBUTTON, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SLIDER_ZOOM, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_MOUSEWHEEL_ZOOM, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_CADSTYLE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_LOCATE_COLLECTION, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_RECENTCMD1, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_RECENTLAYER, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_User message handlers



/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_Commonly property page

IMPLEMENT_DYNCREATE(COp_Page_Ui_Commonly, COp_Page_Base)

//##ModelId=41466B7C0243
COp_Page_Ui_Commonly::COp_Page_Ui_Commonly() : COp_Page_Base(COp_Page_Ui_Commonly::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Ui_Commonly)
	//m_bShowStartup = TRUE;
	m_bTopMost = FALSE;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C0250
COp_Page_Ui_Commonly::~COp_Page_Ui_Commonly()
{
}

//##ModelId=41466B7C0251
void COp_Page_Ui_Commonly::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Ui_Commonly)
	//DDX_Check(pDX, IDC_CHECK_SHOW_STARTUP, m_bShowStartup);
	DDX_Check(pDX, IDC_CHECK_TOPMOST, m_bTopMost);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_Ui_Commonly, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Ui_Commonly)
	//ON_BN_CLICKED(IDC_CHECK_SHOW_STARTUP, OnCheckShowStartup)
	ON_BN_CLICKED(IDC_CHECK_TOPMOST, OnCheckShowStartup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_Commonly message handlers
void COp_Page_Ui_Commonly::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	// Ð´×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();

	//pApp->WriteProfileInt(REGPATH_UICOMMON,REGITEM_SHOWSTARTUP, m_bShowStartup);
	pApp->WriteProfileInt(REGPATH_UICOMMON,REGITEM_TOPMOST, m_bTopMost);

	COp_Page_Base::OnOK();
}

BOOL COp_Page_Ui_Commonly::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// TODO: Add extra initialization here
	// ¶Á×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	//m_bShowStartup	= pApp->GetProfileInt(REGPATH_UICOMMON,REGITEM_SHOWSTARTUP, gdef_bShowStartup);
	m_bTopMost		= pApp->GetProfileInt(REGPATH_UICOMMON,REGITEM_TOPMOST, gdef_bTopMost);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_Ui_Commonly::OnCheckShowStartup() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}



/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_SaveOptions property page

IMPLEMENT_DYNCREATE(COp_Page_Ui_SaveOptions, COp_Page_Base)

//##ModelId=41466B7C02ED
COp_Page_Ui_SaveOptions::COp_Page_Ui_SaveOptions() : COp_Page_Base(COp_Page_Ui_SaveOptions::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Ui_SaveOptions)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C02EE
COp_Page_Ui_SaveOptions::~COp_Page_Ui_SaveOptions()
{
}

//##ModelId=41466B7C02EF
void COp_Page_Ui_SaveOptions::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Ui_SaveOptions)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_Ui_SaveOptions, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Ui_SaveOptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Ui_SaveOptions message handlers

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Alert property page

IMPLEMENT_DYNCREATE(COp_Page_Doc_Alert, COp_Page_Base)

//##ModelId=41466B7C02E0
COp_Page_Doc_Alert::COp_Page_Doc_Alert() : COp_Page_Base(COp_Page_Doc_Alert::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Doc_Alert)
	m_bAlertOver = FALSE;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C02E0
COp_Page_Doc_Alert::~COp_Page_Doc_Alert()
{
}

//##ModelId=41466B7C02E1
void COp_Page_Doc_Alert::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Doc_Alert)
	DDX_Check(pDX, IDC_CHECKOVER, m_bAlertOver);
	//}}AFX_DATA_MAP
}

BOOL COp_Page_Doc_Alert::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_bAlertOver = (BOOL) AfxGetApp()->GetProfileInt(REGPATH_ALERT,"AlertOver",0);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void COp_Page_Doc_Alert::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);

	AfxGetApp()->WriteProfileInt(REGPATH_ALERT,"AlertOver",(int)m_bAlertOver);

	COp_Page_Base::OnOK();
}

BEGIN_MESSAGE_MAP(COp_Page_Doc_Alert, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Doc_Alert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Alert message handlers


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Select property page

IMPLEMENT_DYNCREATE(COp_Page_Doc_Select, COp_Page_Base)

BOOL COp_Page_Doc_Select::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();

	m_nSelectSize = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_SELECTSIZE,m_nSelectSize);
	//m_bSymbolizedSelect = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_SYMBOLIZEDSELECT,m_bSymbolizedSelect);
	m_bObjectGroup = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_OBJECTGROUP,TRUE);
	m_bSurfaceInsideSelect = AfxGetApp()->GetProfileInt(REGPATH_SELECT, REGITEM_SURFACEINSIDESELECT, TRUE);
	CSliderCtrl *pSlider = (CSliderCtrl*)GetDlgItem( IDC_SLIDER_SELECTSIZE );
	if( pSlider )
	{
		pSlider->SetRange(0,12);
		pSlider->SetPageSize(1);
		pSlider->SetLineSize(1);
		pSlider->SetTicFreq(1);
		pSlider->SetPos(m_nSelectSize);
	}

	CDC *pDC = m_staticSelectSize.GetDC();
	m_dcMem.CreateCompatibleDC(pDC);
	m_bmpShow.CreateCompatibleBitmap(&m_dcMem,41,41);
	m_staticSelectSize.ReleaseDC(pDC);

	m_staticSelectSize.SetBitmap(m_bmpShow);
	DrawSelectSize();
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//##ModelId=41466B7C02CE
COp_Page_Doc_Select::COp_Page_Doc_Select() : COp_Page_Base(COp_Page_Doc_Select::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Doc_Select)
	//}}AFX_DATA_INIT
	m_nSelectSize = gdef_nSelectSize;
	//m_bSymbolizedSelect = TRUE;
	m_bObjectGroup = TRUE;
	m_bSurfaceInsideSelect = TRUE;
}

//##ModelId=41466B7C02CF
COp_Page_Doc_Select::~COp_Page_Doc_Select()
{
}


BEGIN_MESSAGE_MAP(COp_Page_Doc_Select, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Doc_Select)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_SYMBOLIZEDSELECT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_OBJECTGROUP, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_SURFACEINSIDESELECT, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void COp_Page_Doc_Select::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

void COp_Page_Doc_Select::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CSliderCtrl *pCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SELECTSIZE);
	if( pCtrl )
	{
		m_nSelectSize = pCtrl->GetPos();
		DrawSelectSize();
	}

	SetModified(TRUE);
	
	COp_Page_Base::OnHScroll(nSBCode, nPos, pScrollBar);
}


void COp_Page_Doc_Select::DrawSelectSize()
{
	if( NULL != (HDC)m_dcMem )
	{
		CBitmap *oldbmp = m_dcMem.SelectObject(&m_bmpShow);

		CRect rect(0,0,41,41);
		
		CBrush brush(RGB(0,0,0));
		m_dcMem.FillRect(&rect,&brush);

		CPen pen(PS_SOLID,1,RGB(255,255,255)), *oldpen;
		oldpen = m_dcMem.SelectObject(&pen);

		CPoint cpt = rect.CenterPoint(),t;

		t = cpt;
		t.Offset(-m_nSelectSize,-m_nSelectSize);
		m_dcMem.MoveTo(t);
		t.Offset(m_nSelectSize*2,0);
		m_dcMem.LineTo(t);
		t.Offset(0,m_nSelectSize*2);
		m_dcMem.LineTo(t);
		t.Offset(-m_nSelectSize*2,0);
		m_dcMem.LineTo(t);
		t.Offset(0,-m_nSelectSize*2);
		m_dcMem.LineTo(t);

		m_dcMem.SelectObject(oldpen);
		m_dcMem.SelectObject(oldbmp);

		m_staticSelectSize.Invalidate(TRUE);
	}
}





//##ModelId=41466B7C02D0
void COp_Page_Doc_Select::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Doc_Select)
	DDX_Control(pDX, IDC_STATIC_SELECTSIZE, m_staticSelectSize);
	DDX_Control(pDX, IDC_SLIDER_SELECTSIZE, m_sliderSelectSize);
	//DDX_Check(pDX, IDC_CHECK_SYMBOLIZEDSELECT, m_bSymbolizedSelect);
	DDX_Check(pDX, IDC_CHECK_OBJECTGROUP, m_bObjectGroup);
	DDX_Check(pDX, IDC_CHECK_SURFACEINSIDESELECT, m_bSurfaceInsideSelect);
	//}}AFX_DATA_MAP
}

void COp_Page_Doc_Select::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	UpdateData(TRUE);
	
	AfxGetApp()->WriteProfileInt(REGPATH_SELECT,REGITEM_SELECTSIZE,m_nSelectSize);
	//AfxGetApp()->WriteProfileInt(REGPATH_SELECT,REGITEM_SYMBOLIZEDSELECT,m_bSymbolizedSelect);
	AfxGetApp()->WriteProfileInt(REGPATH_SELECT,REGITEM_OBJECTGROUP,m_bObjectGroup);
	AfxGetApp()->WriteProfileInt(REGPATH_SELECT,REGITEM_SURFACEINSIDESELECT,m_bSurfaceInsideSelect);
	
//	SELECTSETTINGS setting;
//	setting.lfRadius = m_nSelectSize;
//	AfxCallMessage(FCCM_SELECTSETTING,0, (LPARAM)&setting);

	COp_Page_Base::OnOK();
}


void COp_Page_Doc_Save::OnAutosave() 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CWnd *pCtrl = GetDlgItem(IDC_EDIT_SAVETIME);
	if( pCtrl )pCtrl->EnableWindow(m_bAutoSave);

	SetModified(TRUE);
}

void COp_Page_Doc_Save::OnDelaysave() 
{
	UpdateData(TRUE);
	// TODO: Add your control notification handler code here
	CWnd *pCtrl = GetDlgItem(IDC_EDIT_OPERATOR_COUNT);
	if( pCtrl )pCtrl->EnableWindow(m_bUseCache);
	
	SetModified(TRUE);
}

BOOL COp_Page_Doc_Save::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bAutoSave	= AfxGetApp()->GetProfileInt(REGPATH_SAVE,REGITEM_AUTOSAVE,TRUE);
	m_nSaveTime = AfxGetApp()->GetProfileInt(REGPATH_SAVE,REGITEM_SAVETIME,60);
	m_bUseCache	= AfxGetApp()->GetProfileInt(REGPATH_SAVE,_T("UseCache"),TRUE);
	UpdateData(FALSE);
	CWnd *pCtrl = GetDlgItem(IDC_EDIT_SAVETIME);
	if( pCtrl )pCtrl->EnableWindow(m_bAutoSave);
	CWnd *pCtrl1 = GetDlgItem(IDC_EDIT_OPERATOR_COUNT);
	if( pCtrl1 )pCtrl1->EnableWindow(m_bUseCache);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_Doc_Save::OnOK() 
{
	COp_Page_Base::OnOK();
	AfxGetApp()->WriteProfileInt(REGPATH_SAVE,REGITEM_AUTOSAVE,m_bAutoSave);
	AfxGetApp()->WriteProfileInt(REGPATH_SAVE,REGITEM_SAVETIME,m_nSaveTime);
	AfxGetApp()->WriteProfileInt(REGPATH_SAVE,_T("UseCache"),m_bUseCache);

	if(CUVSModify::IsDBConnected())
	{
		CUVSModify::UpdateImmediately(!m_bUseCache);
	}
}

void COp_Page_Doc_Save::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

void COp_Page_Doc_User::OnCheckModified() 
{
	BOOL bRCLKSwitch_Old = m_bRCLKSwitch;
	BOOL bCADStyle_Old = m_bCADStyle;

	UpdateData(TRUE);
	SetModified(TRUE);

	if(m_bRCLKSwitch!=bRCLKSwitch_Old && m_bRCLKSwitch)
		m_bCADStyle = FALSE;
	else if(m_bCADStyle!=bCADStyle_Old && m_bCADStyle)
		m_bRCLKSwitch = FALSE;

	UpdateData(FALSE);
}

BOOL COp_Page_Doc_User::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_bRCLKSwitch	= AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RCLKSWITCH,TRUE);
	m_bPlaceProperties	= AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_PLACEPROPERTIES,FALSE);
	m_bStereoMidButton = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_STEREOMIDBUTTON,FALSE);
	m_bSliderZoom = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_SLIDERZOOM,FALSE);
	m_bMouseWheelZoom = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_MOUSEWHEELZOOM,FALSE);
	m_nRecentlyCmd	= AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RECENTCMD,m_nRecentlyCmd);
	m_nRecentlyLayer = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RECENTLAYER,m_nRecentlyLayer);
	m_bCADStyle = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_CADSTYLE,m_bCADStyle);
	m_bFocusColletion = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_FOCUSCOL,m_bFocusColletion);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_Doc_User::OnOK() 
{
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_RCLKSWITCH,m_bRCLKSwitch);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_PLACEPROPERTIES,m_bPlaceProperties);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_STEREOMIDBUTTON,m_bStereoMidButton);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_SLIDERZOOM,m_bSliderZoom);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_MOUSEWHEELZOOM,m_bMouseWheelZoom);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_RECENTCMD,m_nRecentlyCmd);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_RECENTLAYER,m_nRecentlyLayer);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_CADSTYLE,m_bCADStyle);
	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_FOCUSCOL,m_bFocusColletion);
	COp_Page_Base::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// COp_Page_Precision property page

IMPLEMENT_DYNCREATE(COp_Page_Precision, COp_Page_Base)

//##ModelId=41466B7C0262
COp_Page_Precision::COp_Page_Precision() : COp_Page_Base(COp_Page_Precision::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Precision)
	m_nXYPrecision = 3;
	m_nZPrecision = 3;
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C026F
COp_Page_Precision::~COp_Page_Precision()
{
}

//##ModelId=41466B7C0270
void COp_Page_Precision::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_View_ImageView)
	DDX_CBIndex(pDX, IDC_COMBO_XYPRECISION, m_nXYPrecision);
	DDX_CBIndex(pDX, IDC_COMBO_ZPRECISION, m_nZPrecision);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_Precision, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_View_ImageView)
		// NOTE: the ClassWizard will add message map macros here
	ON_CBN_SELCHANGE(IDC_COMBO_XYPRECISION, OnCheckModified)
	ON_CBN_SELCHANGE(IDC_COMBO_ZPRECISION, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_ImageView message handlers
void COp_Page_Precision::OnOK() 
{
	UpdateData(TRUE);
	// Ð´×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();

	WriteProfileDouble(REGPATH_PRECISION,REGITEM_XYPRECISION, pow(0.1,m_nXYPrecision+1));
	WriteProfileDouble(REGPATH_PRECISION,REGITEM_ZPRECISION, pow(0.1,m_nZPrecision+1));
	
	COp_Page_Base::OnOK();
}

void COp_Page_Precision::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}

BOOL COp_Page_Precision::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	// TODO: Add extra initialization here
	// ¶Á×¢²áµÇ¼Ç±í
	CEditBaseApp *pApp = (CEditBaseApp*)AfxGetApp();
	
	double lfXYPrec	= GetProfileDouble(REGPATH_PRECISION,REGITEM_XYPRECISION, pow(0.1,m_nXYPrecision+1));
	double lfZPrec = GetProfileDouble(REGPATH_PRECISION,REGITEM_ZPRECISION,pow(0.1,m_nZPrecision+1));

	m_nXYPrecision = floor(-log10(lfXYPrec)+0.5)-1;
	m_nZPrecision = floor(-log10(lfZPrec)+0.5)-1;
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// COp_Page_NetWork property page

IMPLEMENT_DYNCREATE(COp_Page_NetWork, COp_Page_Base)

//##ModelId=41466B7C0262
COp_Page_NetWork::COp_Page_NetWork() : COp_Page_Base(COp_Page_NetWork::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Precision)
	m_inipath = GetConfigPath(TRUE);
	m_inipath += "\\NetWork.ini";
	//}}AFX_DATA_INIT
}

//##ModelId=41466B7C026F
COp_Page_NetWork::~COp_Page_NetWork()
{
}

//##ModelId=41466B7C0270
void COp_Page_NetWork::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_View_ImageView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COp_Page_NetWork, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_NetWork)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_View_ImageView message handlers
void COp_Page_NetWork::OnOK()
{
	CString str;
	GetDlgItemText(IDC_IPADDRESS1, str);
	WritePrivateProfileString("uvs", "uvs_ip", str, m_inipath);
	GetDlgItemText(IDC_EDIT1, str);
	WritePrivateProfileString("uvs", "uvs_port", str, m_inipath);
	GetDlgItemText(IDC_IPADDRESS2, str);
	WritePrivateProfileString("uis", "uis_ip", str, m_inipath);
	GetDlgItemText(IDC_EDIT2, str);
	WritePrivateProfileString("uis", "uis_http_port", str, m_inipath);

	COp_Page_Base::OnOK();
}

BOOL COp_Page_NetWork::OnInitDialog()
{
	COp_Page_Base::OnInitDialog();

	// TODO: Add extra initialization here
	char uvs_ip[128] = { 0 };
	char uvs_port[128] = { 0 };
	char uis_ip[128] = { 0 };
	char uis_port[128] = { 0 };

	GetPrivateProfileString("uvs", "uvs_ip", "", uvs_ip, 128, m_inipath);
	GetPrivateProfileString("uvs", "uvs_port", "8888", uvs_port, 128, m_inipath);
	GetPrivateProfileString("uis", "uis_ip", "", uis_ip, 128, m_inipath);
	GetPrivateProfileString("uis", "uis_http_port", "8080", uis_port, 128, m_inipath);

	SetDlgItemText(IDC_IPADDRESS1, uvs_ip);
	SetDlgItemText(IDC_EDIT1, uvs_port);
	SetDlgItemText(IDC_IPADDRESS2, uis_ip);
	SetDlgItemText(IDC_EDIT2, uis_port);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}