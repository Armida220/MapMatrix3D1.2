// DlgUsedFonts.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgUsedFonts.h"

#include "DrawingContext.h"
#include "GrElementList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CDlgNewTextStyle dialog


CDlgNewTextStyle::CDlgNewTextStyle(CWnd* pParent /*=NULL*/)
: CDialog(CDlgNewTextStyle::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewTextStyle)
	m_strStyleName = _T("");
	m_strFontName = _T("");
	m_fInclinedAngle = 0.0f;
	m_fWidScale = 0.0f;
	m_nInclinedType = 0;
	m_nBold = 1;
	//}}AFX_DATA_INIT
}


void CDlgNewTextStyle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewTextStyle)
	DDX_Text(pDX, IDC_EDIT_NAME, m_strStyleName);
	DDX_Text(pDX, IDC_FONT_NAME, m_strFontName);
	DDX_Text(pDX, IDC_INCLINED_ANGLE, m_fInclinedAngle);
	DDX_Text(pDX, IDC_WIDTH_SCALE, m_fWidScale);
	DDX_CBIndex(pDX, IDC_COMBO_INCLINETYPE, m_nInclinedType);
	DDX_CBIndex(pDX, IDC_COMBO_BOLD, m_nBold);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewTextStyle, CDialog)
//{{AFX_MSG_MAP(CDlgNewTextStyle)
ON_BN_CLICKED(IDC_BUTTON_BROWSEFONT, OnBrowsefont)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewTextStyle message handlers

void CDlgNewTextStyle::OnBrowsefont() 
{
	UpdateData(TRUE);

	LOGFONT logFont;

	memset(&logFont,0,sizeof(logFont));
	strcpy(logFont.lfFaceName,m_style.font);
	logFont.lfHeight = 30;
	CFontDialog dlg(&logFont);
	if( dlg.DoModal()!=IDOK )
		return;

	dlg.GetCurrentFont(&logFont);

	strcpy(m_style.font,logFont.lfFaceName);
	m_strFontName = logFont.lfFaceName;
	
	UpdateData(FALSE);
}

void CDlgNewTextStyle::OnOK() 
{
	UpdateData(TRUE);

	strncpy(m_style.font,m_strFontName,sizeof(m_style.font)-1);
	strncpy(m_style.name,m_strStyleName,sizeof(m_style.name)-1);
	m_style.fWidScale = m_fWidScale;
	m_style.fInclinedAngle = m_fInclinedAngle;
	m_style.nInclineType = m_nInclinedType;
	m_style.bBold = (m_nBold==0);
	
	CDialog::OnOK();
}

BOOL CDlgNewTextStyle::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_strFontName = m_style.font;
	m_strStyleName = m_style.name;
	m_fWidScale = m_style.fWidScale;
	m_fInclinedAngle = m_style.fInclinedAngle;
	m_nInclinedType = m_style.nInclineType;
	m_nBold = (m_style.bBold?0:1);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


IMPLEMENT_DYNAMIC(CStatic_TextStylePreview, CStatic)

BEGIN_MESSAGE_MAP(CStatic_TextStylePreview, CStatic)
	//{{AFX_MSG_MAP(CStatic_TextStylePreview)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CStatic_TextStylePreview::CStatic_TextStylePreview()
{
}


CStatic_TextStylePreview::~CStatic_TextStylePreview()
{
}

void CStatic_TextStylePreview::OnPaint()
{
	CPaintDC dc0(this);
	CRect rcClient;
	GetClientRect(&rcClient);

	dc0.FillRect(&rcClient,&CBrush(RGB(255,255,255)));

	C2DGDIDrawingContext dc;
	dc.EnableMonocolor(TRUE,0);
	dc.CreateContext(dc0.GetSafeHdc());
	dc.SetDCSize(rcClient.Size());
	dc.SetViewRect(rcClient);

	GrBuffer2d buf;
	TextSettings ts;

	strncpy(ts.tcFaceName,m_style.font,sizeof(ts.tcFaceName)-1);
	ts.fHeight = 20;
	ts.fWidScale = m_style.fWidScale;
	ts.nInclineType = m_style.nInclineType;
	ts.fInclineAngle = m_style.fInclinedAngle;
	ts.SetBold(m_style.bBold);
	buf.Text(0,&PT_2D(10,rcClient.bottom-10),StrFromResID(IDS_FONT_PREVIEW),&ts);
	
	dc.BeginDrawing();	
	dc.DrawGrBuffer2d(&GrElementList((void*)&buf),CDrawingContext::modeNormal);
	dc.EndDrawing();
}

/////////////////////////////////////////////////////////////////////////////
// CDlgUsedFonts dialog


CDlgUsedFonts::CDlgUsedFonts(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgUsedFonts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgUsedFonts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgUsedFonts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgUsedFonts)
	DDX_Control(pDX, IDC_LIST_TEXTSTYLE, m_wndStyles);
	DDX_Control(pDX, IDC_STATIC_PREVIEW, m_wndPreview);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgUsedFonts, CDialog)
	//{{AFX_MSG_MAP(CDlgUsedFonts)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_MODIFY, OnModify)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_TEXTSTYLE, OnSelchangeStyles)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TEXTSTYLE, OnDblclkListStyles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgUsedFonts message handlers

static CString FormatValue(float v)
{
	CString str;
	str.Format("%.2f",v);

	return str;
}

void CDlgUsedFonts::OnAdd() 
{	
	CDlgNewTextStyle dlg;

	while( 1 )
	{
		if( dlg.DoModal()!=IDOK )
			return;

		TextStyle item = dlg.m_style;
		if( !item.IsValid() )
		{
			AfxMessageBox(IDS_ERR_INVALIDSTYLE);
			continue;
		}
		
		for( int i=0; i<m_styles.GetSize(); i++)
		{
			if( stricmp(item.name,m_styles[i].name)==0 )
			{
				AfxMessageBox(IDS_ERR_SAMESTYLENAME);
				break;
			}
		}

		if( i>=m_styles.GetSize() )
			break;
	}

	TextStyle item = dlg.m_style;

	int index = m_wndStyles.GetItemCount();
	InsertItem(index,item);

	m_styles.Add(item);
}


void CDlgUsedFonts::InsertItem(int index, const TextStyle& item)
{
	m_wndStyles.InsertItem(index,item.name);

	CString strInclinedType;
	if( item.nInclineType==0 )
		strInclinedType = StrFromResID(IDS_CMDPLANE_SHRUGN);
	else if( item.nInclineType==1 )
		strInclinedType = StrFromResID(IDS_CMDPLANE_SHRUGL);
	else if( item.nInclineType==2 )
		strInclinedType = StrFromResID(IDS_CMDPLANE_SHRUGR);
	else if( item.nInclineType==3 )
		strInclinedType = StrFromResID(IDS_CMDPLANE_SHRUGU);
	else if( item.nInclineType==4 )
		strInclinedType = StrFromResID(IDS_CMDPLANE_SHRUGD);
	
	m_wndStyles.SetItemText(index,1,item.font);
	m_wndStyles.SetItemText(index,2,FormatValue(item.fWidScale));
	m_wndStyles.SetItemText(index,3,strInclinedType);
	m_wndStyles.SetItemText(index,4,FormatValue(item.fInclinedAngle));

	if( item.bBold )
		m_wndStyles.SetItemText(index,5,StrFromResID(IDS_YES));
	else
		m_wndStyles.SetItemText(index,5,StrFromResID(IDS_NO));
}

void CDlgUsedFonts::OnDel() 
{
	POSITION pos = m_wndStyles.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;
	int nSel = m_wndStyles.GetNextSelectedItem(pos);
	if( nSel<0 )
		return;

	if( AfxMessageBox(IDS_DEL_CONFIRM,MB_YESNO)!=IDYES )
		return;

	m_styles.RemoveAt(nSel);

	m_wndStyles.DeleteItem(nSel);	
}


void CDlgUsedFonts::OnModify()
{
	POSITION pos = m_wndStyles.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;
	int nSel = m_wndStyles.GetNextSelectedItem(pos);
	if( nSel<0 )
		return;
	
	TextStyle item = m_styles[nSel];
	CDlgNewTextStyle dlg;
	
	dlg.m_style = item;
	
	while( 1 )
	{
		if( dlg.DoModal()!=IDOK )
			return;
		
		TextStyle item = dlg.m_style;
		if( strlen(item.name)<=0 )
			return;
		
		for( int i=0; i<m_styles.GetSize(); i++)
		{
			if( i!=nSel && stricmp(item.name,m_styles[i].name)==0 )
			{
				AfxMessageBox(IDS_ERR_SAMESTYLENAME);
				break;
			}
		}
		
		if( i>=m_styles.GetSize() )
			break;
	}
	
	item = dlg.m_style;
	m_styles[nSel] = item;
	
	m_wndStyles.DeleteItem(nSel);	
	InsertItem(nSel,item);	
	
	m_wndStyles.SetItem(nSel, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
}

void CDlgUsedFonts::OnUp()
{
	POSITION pos = m_wndStyles.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;
	int nSel = m_wndStyles.GetNextSelectedItem(pos);
	if( nSel<=0 )
		return;

	TextStyle item = m_styles[nSel];

	m_wndStyles.DeleteItem(nSel);
	InsertItem(nSel-1,item);

	m_styles.RemoveAt(nSel);
	m_styles.InsertAt(nSel-1,item);

	m_wndStyles.SetItem(nSel-1, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);

}

void CDlgUsedFonts::OnDown()
{
	POSITION pos = m_wndStyles.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;
	int nSel = m_wndStyles.GetNextSelectedItem(pos);
	if( nSel<0 || nSel==(m_wndStyles.GetItemCount()-1) )
		return;
	
	TextStyle item = m_styles[nSel];
	
	m_wndStyles.DeleteItem(nSel);
	InsertItem(nSel+1,item);
	
	m_styles.RemoveAt(nSel);
	m_styles.InsertAt(nSel+1,item);
	m_wndStyles.SetItem(nSel+1, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
}

void CDlgUsedFonts::OnOK() 
{
	POSITION pos = m_wndStyles.GetFirstSelectedItemPosition();
	if( pos )
	{
		int nSel = m_wndStyles.GetNextSelectedItem(pos);
		if( nSel>=0 )
		{
			m_strResult = m_styles[nSel].name;
		}
	}

	GetUsedTextStyles()->Save(m_styles);
	
	CDialog::OnOK();
}

BOOL CDlgUsedFonts::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetUsedTextStyles()->GetTextStyles(m_styles);

	m_wndStyles.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	m_wndStyles.InsertColumn(0,StrFromResID(IDS_TEXTSTYLE_NAME),LVCFMT_CENTER,80);
	m_wndStyles.InsertColumn(1,StrFromResID(IDS_TEXTSTYLE_FONT),LVCFMT_CENTER,80);
	m_wndStyles.InsertColumn(2,StrFromResID(IDS_TEXTSTYLE_WIDTHSCALE),LVCFMT_CENTER,70);
	m_wndStyles.InsertColumn(3,StrFromResID(IDS_CMDPLANE_SHRUG),LVCFMT_CENTER,70);
	m_wndStyles.InsertColumn(4,StrFromResID(IDS_TEXTSTYLE_INCLINEDANGLE),LVCFMT_CENTER,70);
	m_wndStyles.InsertColumn(5,StrFromResID(IDS_BOLD),LVCFMT_CENTER,70);

	int nSel = -1;
	
	for( int i=0; i<m_styles.GetSize(); i++)
	{
		TextStyle item = m_styles[i];

		if( m_strResult.CompareNoCase(item.name)==0 )
			nSel = i;

		InsertItem(i,item);
	}

	if( nSel>=0 )
	{
		m_wndStyles.SetItem(nSel, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		m_wndPreview.m_style = m_styles[nSel];

		m_wndPreview.RedrawWindow();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgUsedFonts::OnDblclkListStyles(NMHDR * pNotifyStruct, LRESULT * result) 
{
	*result = 0;

	OnModify();
}


void CDlgUsedFonts::OnSelchangeStyles(NMHDR * pNotifyStruct, LRESULT * result)
{
	*result = 0;

	POSITION pos = m_wndStyles.GetFirstSelectedItemPosition();
	if( pos==NULL )
		return;
	int nSel = m_wndStyles.GetNextSelectedItem(pos);
	if( nSel<0 )
		return;

	m_wndPreview.m_style = m_styles[nSel];
	
	m_wndPreview.RedrawWindow();
}