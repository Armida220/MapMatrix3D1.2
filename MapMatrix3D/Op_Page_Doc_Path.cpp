// Op_Page_Doc_Path.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DataSource.h"
#include "OptionsPages.h "
#include "Op_Page_Doc_Path.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "SmartViewFunctions.h"
#include "SymbolLib.h"
#include "ExMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Path dialog

COp_Page_Doc_Path::COp_Page_Doc_Path(CWnd* pParent /*=NULL*/)
	: COp_Page_Base(COp_Page_Doc_Path::IDD)
{
	//{{AFX_DATA_INIT(COp_Page_Doc_Path)
	m_strSymPath = _T("");
	m_bColorByObj = TRUE;
	m_AutoSet = FALSE;
	m_RoundLine = FALSE;
	m_lfArcToler = 0.05;
	m_bNotDisplayAnnot = FALSE;
	m_bAnnotUpward = FALSE;
	m_bAnnotToText = FALSE;
	m_fDrawScale = 1.0f;
	m_fAnnoScale = 1.0;
	m_bTrimSurface = TRUE;
	//}}AFX_DATA_INIT
	m_bTextDrawingClear = FALSE;
	m_bDisplayAccel = TRUE;

	m_lfMultiPointSize = 0.2;
	m_bDisplayDemLine = TRUE;
	m_bkeepOneCell = TRUE;
	m_Transparency = 50;
}


void COp_Page_Doc_Path::DoDataExchange(CDataExchange* pDX)
{
	COp_Page_Base::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COp_Page_Doc_Path)
	DDX_Text(pDX, IDC_EDIT_SYMPATH, m_strSymPath);
	DDX_Check(pDX, IDC_CHECK_COLORBYOBJ, m_bColorByObj);
	DDX_Check(pDX, IDC_CHECK_AUTO_SET, m_AutoSet);
//	DDX_Check(pDX, IDC_CHECK_ROUNDLINE, m_RoundLine);
	DDX_Text(pDX, IDC_EDIT_TOLERANCE, m_lfArcToler);
	DDX_Check(pDX, IDC_CHECK_NOTDISPLAYANNOT, m_bNotDisplayAnnot);
	DDX_Check(pDX, IDC_CHECK_TEXTUPWARD, m_bAnnotUpward);
	DDX_Check(pDX, IDC_CHECK_ANNOTTOTEXT, m_bAnnotToText);
	DDX_Text(pDX, IDC_DRAWSCALE_EDIT, m_fDrawScale);
	DDX_Text(pDX, IDC_ANNOSCALE_EDIT, m_fAnnoScale);
	DDV_MinMaxDouble(pDX, m_fDrawScale, 1.e-004, 10000.);
	DDV_MinMaxDouble(pDX, m_fAnnoScale, 1.e-004, 10000.);
	DDX_Check(pDX, IDC_CHECK_TRIMSURFACE, m_bTrimSurface);
	DDX_Text(pDX, IDC_EDIT_TRANSPARENCY, m_Transparency);
	DDV_MinMaxInt(pDX, m_Transparency, 0, 100);
	//}}AFX_DATA_MAP
//	DDX_Check(pDX, IDC_CHECK_TEXTACCEL, m_bTextDrawingClear);
	DDX_Check(pDX, IDC_CHECK_DISPLAYFAST, m_bDisplayAccel);
	DDX_Text(pDX, IDC_EDIT_MULTIPOINT_SIZE, m_lfMultiPointSize);
	DDX_Control( pDX, IDC_BTN_MULTIPOINT_COLOR,m_MultipointColor);
	DDX_Check(pDX, IDC_CHECK_JOINTDEMPOINT, m_bDisplayDemLine);
	DDX_Check(pDX, IDC_CHECK_KEEPONECELL, m_bkeepOneCell);
}


BEGIN_MESSAGE_MAP(COp_Page_Doc_Path, COp_Page_Base)
	//{{AFX_MSG_MAP(COp_Page_Doc_Path)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_CHECK_AUTO_SET, OnAutoSet)
	ON_BN_CLICKED(IDC_CHECK_COLORBYOBJ, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_ROUNDLINE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_NOTDISPLAYANNOT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_TEXTUPWARD, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_ANNOTTOTEXT, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_TRIMSURFACE, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_TEXTACCEL, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_DISPLAYFAST, OnCheckModified)
	ON_BN_CLICKED(IDC_CHECK_JOINTDEMPOINT, OnCheckModified)
	ON_BN_CLICKED(IDC_BTN_MULTIPOINT_COLOR, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_SYMPATH, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_TOLERANCE, OnCheckModified)
	ON_EN_CHANGE(IDC_DRAWSCALE_EDIT, OnCheckModified)
	ON_EN_CHANGE(IDC_ANNOSCALE_EDIT, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_TRANSPARENCY, OnCheckModified)
	ON_EN_CHANGE(IDC_EDIT_MULTIPOINT_SIZE, OnCheckModified)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COp_Page_Doc_Path message handlers
extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
					LPTSTR lpszPath,		// [out] 返回的文件路径
					LPCTSTR lpszInitDir,	// [in] 初始文件路径
					HWND hWnd,				// [in] 父系窗口
					BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
					UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格

//static CString strInitDir = _T("");
void COp_Page_Doc_Path::OnBrowse() 
{
	UpdateData(TRUE);
	char retPath[_MAX_PATH]={0};

	CString strInitDir = m_strSymPath;
	if( strInitDir.GetLength()<=0 )
	{
//		GetConfigPath(strInitDir.GetBuffer(256));
		strInitDir = GetConfigPath();
//		strInitDir.ReleaseBuffer();
		
		int len = strInitDir.GetLength();
		if( len>0 && strInitDir[len-1]=='\\' )
			strInitDir.SetAt(len-1,0);
	}

	if( !BrowseFolderEx(StrFromResID(IDS_SYMLIB_DIR),retPath,strInitDir,m_hWnd) )return;
	
	strInitDir = retPath;
	
	int str_len = strlen(retPath);
	if( str_len<=0 )return;
	if( retPath[str_len-1]=='\\')
	{
		retPath[str_len-1] = 0;
	}
// 	if( retPath[str_len-1]!='\\')
// 	{
// 		retPath[str_len]='\\';
// 		retPath[str_len+1]=0;
// 	}

	m_strSymPath = retPath;	
	UpdateData(FALSE);

	OnCheckModified();
}


void COp_Page_Doc_Path::OnCheckModified() 
{
	// TODO: Add your control notification handler code here
	SetModified(TRUE);
}


void COp_Page_Doc_Path::OnAutoSet()
{
	UpdateData(TRUE);
	SetModified(TRUE);
}

BOOL COp_Page_Doc_Path::OnInitDialog() 
{
	COp_Page_Base::OnInitDialog();
	
	m_strSymPath  = AfxGetApp()->GetProfileString(REGPATH_SYMBOL,REGITEM_SYMPATH,"");
	
//	double  scale1;
//	scale0 = GetProfileDouble(REGPATH_SYMBOL,REGITEM_DOCDRAWSCALE,-1);
	m_fDrawScale = GetProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,1);
	m_fAnnoScale = GetProfileDouble(REGPATH_SYMBOL,REGITEM_ANNOSCALE,1);

	m_RoundLine	  = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_ROUNDLINE,FALSE);
	m_lfArcToler  = GetProfileDouble(REGPATH_SYMBOL,REGITEM_ARCTOLER,m_lfArcToler);
	m_bNotDisplayAnnot = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_NOTDISPLAYANNOT,FALSE);
	m_bAnnotUpward = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_ANNOTATIONUPWARD,FALSE);
	m_bTextDrawingClear = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_TEXTCLEAR,m_bTextDrawingClear);
	m_bDisplayAccel = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_DISPLAYFAST,m_bDisplayAccel);
	m_bAnnotToText = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_ANNOTTOTEXT,FALSE);
	m_bTrimSurface = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_TRIMSURFACE,m_bTrimSurface);

    m_bDisplayDemLine = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"DisplayDemLine",m_bDisplayDemLine);

	m_lfMultiPointSize  = GetProfileDouble(REGPATH_SYMBOL,"MultipointSize",m_lfMultiPointSize);

	m_MultipointColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);

	m_bkeepOneCell = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_KEEPONECELL,m_bkeepOneCell);

	m_Transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", m_Transparency);
	
	int value = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL, "MultipointColor",RGB(0,255,0) );
	m_MultipointColor.SetColor(value);

	if (m_strSymPath.IsEmpty())
	{
		m_strSymPath = GetConfigPath();
	}

	m_strSymPath0 = m_strSymPath;

	m_fDrawScale0 = m_fDrawScale;
	m_fAnnoScale0 = m_fAnnoScale;

	m_lfArcToler0 = m_lfArcToler;

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COp_Page_Doc_Path::OnOK() 
{
	COp_Page_Base::OnOK();
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileString(REGPATH_SYMBOL,REGITEM_SYMPATH,m_strSymPath);
/*	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL,REGITEM_SCALEAUTOSET,m_bAutoScale);*/
	WriteProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,m_fDrawScale);
	WriteProfileDouble(REGPATH_SYMBOL,REGITEM_ANNOSCALE,m_fAnnoScale);
	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL,REGITEM_ROUNDLINE,m_RoundLine);
	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL,REGITEM_ANNOTATIONUPWARD,m_bAnnotUpward);
	
	WriteProfileDouble(REGPATH_SYMBOL,REGITEM_ARCTOLER,m_lfArcToler);
	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL,REGITEM_NOTDISPLAYANNOT,m_bNotDisplayAnnot);

	AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_TEXTCLEAR,m_bTextDrawingClear);
	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL,REGITEM_DISPLAYFAST,m_bDisplayAccel);

	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_ANNOTTOTEXT,m_bAnnotToText);

	AfxGetApp()->WriteProfileInt(REGPATH_USER,REGITEM_TRIMSURFACE,m_bTrimSurface);

	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL,"DisplayDemLine",m_bDisplayDemLine);

	WriteProfileDouble(REGPATH_SYMBOL,"MultipointSize",m_lfMultiPointSize);

	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL, "MultipointColor",m_MultipointColor.GetColor());
	
	AfxGetApp()->WriteProfileInt(REGPATH_USER, REGITEM_KEEPONECELL,m_bkeepOneCell);
	AfxGetApp()->WriteProfileInt(REGPATH_SYMBOL, "Transparency",m_Transparency);
	
	if (m_strSymPath.CompareNoCase(m_strSymPath0)!=0)
	{
		((CEditBaseApp*)AfxGetApp())->ReloadConfig();
		
		m_strSymPath0 = m_strSymPath;
		//MessageBox(StrFromResID(IDS_CONFIGPATH_TIP));
		//gpCfgLibMan->LoadConfig(m_strSymPath);
	}

	if (fabs(m_fDrawScale0-m_fDrawScale)>1e-4)
	{
		AfxGetMainWnd()->SendMessage(FCCM_SETSYMDRAWSCALE,0,0);
		m_fDrawScale0 = m_fDrawScale;
	}

	if (fabs(m_fAnnoScale0-m_fAnnoScale)>1e-4)
	{
		AfxGetMainWnd()->SendMessage(FCCM_SETSYMDRAWSCALE,0,0);
		m_fAnnoScale0 = m_fAnnoScale;
	}

	if (fabs(m_lfArcToler-m_lfArcToler0) > 1e-4)
	{
		CLinearizer::m_fPrecision = m_lfArcToler;
		AfxGetMainWnd()->SendMessage(FCCM_DOCRECREATEFTRS,0,0);
		m_lfArcToler0 = m_lfArcToler;
	}
}



	


