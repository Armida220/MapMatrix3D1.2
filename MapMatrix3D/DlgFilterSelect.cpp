// DlgFilterSelect.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgFilterSelect.h"
#include "DlgConditionSelectProp.h"
#include "UIFBoolProp.h"
#include "UIFLayerProp.h"
#include "FtrLayer.h"
#include "GeoText.h"
#include "EditbaseDoc.h"
#include "DlgDataSource.h"
#include "..\CORE\viewer\EditBase\res\resource.h"
#include "Functions_temp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004

extern void AlignToString(long& nAlign, CString& str, BOOL bOrder);
extern void AddAlignPropOptions(CUIFProp *pProp);
extern void InclinedTypeToString(long& type, CString& str, BOOL bOrder);
extern void AddInclinedTypePropOptions(CUIFProp *pProp);

extern CString PencodeToText(int pencode);

IMPLEMENT_DYNAMIC(CUIFConditionsProp,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFConditionsProp::CUIFConditionsProp(const CString& strName, const CString& varValue, 
                                      LPCTSTR lpszDescr, DWORD_PTR dwData)
									 :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
									 NULL,NULL,NULL)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CUIFConditionsProp::~CUIFConditionsProp()
{
	
}


void CUIFConditionsProp::OnClickButton(CPoint point)
{
	CDlgConditionSelectProp dlg;

	dlg.SetFields(m_arrFields);

	dlg.m_strCondsName = m_condMenu.name;
	dlg.m_arrConds.Copy(m_condMenu.arrConds);
	
	if( dlg.DoModal()!=IDOK )
		return;

	m_condMenu.name = dlg.m_strCondsName;
	m_condMenu.arrConds.Copy(dlg.m_arrConds);
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText(m_condMenu.name);
	}
	Redraw();
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		m_pWndList->SetFocus ();
	}
}



/////////////////////////////////////////////////////////////////////////////
// CDlgFilterSelect dialog


CDlgFilterSelect::CDlgFilterSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFilterSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFilterSelect)
	m_bArea = TRUE;
	m_bDirPoint = TRUE;
	m_bDLine = TRUE;
	m_bInverse = FALSE;
	m_bLine = TRUE;
	m_bParallel = TRUE;
	m_bPoint = TRUE;
	m_bText = TRUE;
	m_bAreaPoint = TRUE;
	m_nRange = 0;
	//}}AFX_DATA_INIT

	m_pDoc = NULL;
}


void CDlgFilterSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFilterSelect)
	DDX_Check(pDX, IDC_CHECK_AREA, m_bArea);
	DDX_Check(pDX, IDC_CHECK_DIRPOINT, m_bDirPoint);
	DDX_Check(pDX, IDC_CHECK_DLINE, m_bDLine);
	DDX_Check(pDX, IDC_CHECK_INVERSE, m_bInverse);
	DDX_Check(pDX, IDC_CHECK_LINES, m_bLine);
	DDX_Check(pDX, IDC_CHECK_PARALLEL, m_bParallel);
	DDX_Check(pDX, IDC_CHECK_POINT, m_bPoint);
	DDX_Check(pDX, IDC_CHECK_TEXT, m_bText);
	DDX_Check(pDX, IDC_CHECK_SURFACEPOINT, m_bAreaPoint);
	DDX_Radio(pDX, IDC_RADIO_ALLMAP, m_nRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFilterSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgFilterSelect)
	ON_BN_CLICKED(IDC_CHECK_POINT, OnCheckPoint)
	ON_BN_CLICKED(IDC_CHECK_LINES, OnCheckLine)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED,OnPropertyChanged)
	ON_COMMAND(ID_FSELECT_DO, OnDo)
	ON_COMMAND(ID_FSELECT_UNDO, OnUnDo)
	ON_COMMAND(ID_FSELECT_CLEARSETTINGS, OnClearSettings)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgFilterSelect message handlers

void CDlgFilterSelect::OnOK() 
{
	m_wndPropList.SaveCurValue();
	m_tab.DelAll();
	GetPropValues(m_tab);

	if( m_pDoc )
	{
		m_pDoc->DoFilterSelect(m_tab);
	}
}


void CDlgFilterSelect::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CDlgFilterSelect::OnCheckPoint() 
{
	UpdateData(TRUE);

	if( m_bDirPoint!=m_bPoint )
		m_bDirPoint = m_bPoint;

	if( m_bAreaPoint!=m_bPoint )
		m_bAreaPoint = m_bPoint;

	UpdateData(FALSE);
}

void CDlgFilterSelect::OnCheckLine() 
{
	BOOL bSame = (m_bLine==m_bParallel && m_bLine==m_bDLine);
	UpdateData(TRUE);

	if( bSame )
	{
		m_bParallel = m_bDLine = m_bLine;
	}

	UpdateData(FALSE);
}

BOOL CDlgFilterSelect::OnInitDialog() 
{
	if( !CDialog::OnInitDialog() )
		return FALSE;

	CRect rcDlg, rcFrame;
	GetWindowRect(&rcDlg);
	AfxGetMainWnd()->GetWindowRect(rcFrame);
	rcDlg.OffsetRect(rcFrame.right-rcDlg.right,rcFrame.CenterPoint().y-rcDlg.CenterPoint().y);
	MoveWindow(&rcDlg);
	
	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if( !pWnd )
		return FALSE;

	CRect rcPos;
	pWnd->GetWindowRect(rcPos);
	
	ScreenToClient(&rcPos);
	
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD | WS_BORDER, rcPos, this, 2))
	{
	}
	
	m_wndPropList.SetWindowPos(NULL,rcPos.left,rcPos.top,
		rcPos.Width(),rcPos.Height(),0);

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea(TRUE);

	FillPropList();
	
	m_rcStaticRect = rcPos;
	
	GetClientRect(m_rcClient);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CDlgFilterSelect::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message==WM_KEYDOWN )
	{
		if( pMsg->wParam==VK_ESCAPE )
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CDlgFilterSelect::GetPropValues(CValueTable& tab)
{
	UpdateData(TRUE);

	long clsType = 0;

	if( m_bPoint )
		clsType |= (1<<0);
	if( m_bDirPoint )
		clsType |= (1<<1);
	if( m_bLine )
		clsType |= (1<<2);
	if( m_bParallel )
		clsType |= (1<<3);
	if( m_bDLine )
		clsType |= (1<<4);
	if( m_bArea )
		clsType |= (1<<5);
	if( m_bText )
		clsType |= (1<<6);
	if( m_bAreaPoint )
		clsType |= (1<<7);

	tab.BeginAddValueItem();

	tab.AddValue(FIELDNAME_GEOCLASS,&CVariantEx((_variant_t)(long)clsType));
	tab.AddValue(FIELDNAME_RANGE,&CVariantEx((_variant_t)(long)m_nRange));
	tab.AddValue(FIELDNAME_REVERSE,&CVariantEx((_variant_t)(long)m_bInverse));

	for( int i=0; i<m_names.GetSize(); i+=2)
	{
		CUIFPropEx *pp = DYNAMIC_DOWNCAST(CUIFPropEx,m_wndPropList.FindProperty(m_names[i]));
		if( pp )
		{
			if( !pp->GetChecked() )
				continue;

			if( m_names[i+1].CompareNoCase(FIELDNAME_OTHERCONDS)==0 )
			{
				CUIFConditionsProp *pcdp = DYNAMIC_DOWNCAST(CUIFConditionsProp,pp);
				if( pcdp )
				{
					m_condMenu.name = pcdp->m_condMenu.name;
					m_condMenu.arrConds.Copy(pcdp->m_condMenu.arrConds);

					tab.AddValue(m_names[i+1],&CVariantEx((LONG_PTR)&m_condMenu));
				}
			}
			else
			{
				tab.AddValue(m_names[i+1],&CVariantEx(pp->GetValue()));
			}
		}
	}
	tab.EndAddValueItem();
}

static void RemoveFieldOfArray(CStringArray& arr1, CStringArray& arr2, LPCTSTR field)
{
	int nsz = arr1.GetSize();
	for( int i=nsz-1; i>=0; i--)
	{
		if( arr1[i].CompareNoCase(field)==0 )
		{
			arr1.RemoveAt(i);
			arr2.RemoveAt(i);
		}
	}
}


void CDlgFilterSelect::DeleteFieldFromArray(CStringArray& a1, CStringArray& a2, LPCTSTR field)
{
	int nsz = a1.GetSize();
	for( int i=nsz-1; i>=0; i--)
	{
		if( a1[i].CompareNoCase(field)==0 )
		{
			a1.RemoveAt(i);
			a2.RemoveAt(i);
		}
	}
}


static void InsertColorintoArray(CArray<COLORREF,COLORREF>& arrColors, COLORREF clr)
{
	int nsz = arrColors.GetSize();
	COLORREF *buf = arrColors.GetData();
	for( int i=0; i<nsz; i++)
	{
		if( buf[i]==clr )
			break;
	}
	if( i<nsz )
	{
		if( i!=0 )
		{
			arrColors.RemoveAt(i);
			arrColors.InsertAt(0,clr);
		}
	}
	else
	{
		arrColors.InsertAt(0,clr);
	}
}

static void InsertStringintoArray(CStringArray& names, LPCTSTR name)
{
	if( strlen(name)==0 )
		return;

	int nsz = names.GetSize();
	CString *buf = names.GetData();
	for( int i=0; i<nsz; i++)
	{
		if( buf[i].CompareNoCase(name)==0 )
			break;
	}
	if( i<nsz )
	{
		if( i!=0 )
		{
			names.RemoveAt(i);
			names.InsertAt(0,name);
		}
	}
	else
	{
		names.InsertAt(0,name);
	}
}

void CDlgFilterSelect::GetColors(CArray<COLORREF,COLORREF>& arrColors)
{
	arrColors.RemoveAll();

	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	for( int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrlayer = pDS->GetFtrLayerByIndex(i);
		if( !pFtrlayer )continue;
		if( pFtrlayer->IsEmpty() || pFtrlayer->IsDeleted() || !pFtrlayer->IsVisible() )
			continue;

		COLORREF clr = pFtrlayer->GetColor();
		BOOL bAddLayerColor = FALSE;
		for( int j=0; j<pFtrlayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pFtrlayer->GetObject(j);
			if( !pFtr )continue;

			long clr2 = pFtr->GetGeometry()->GetColor();
			if( clr2==-1 )
			{				
				if( !bAddLayerColor )
					InsertColorintoArray(arrColors,clr);
				bAddLayerColor = TRUE;
			}
			else
			{
				InsertColorintoArray(arrColors,clr2);
			}
		}
	}
}

void CDlgFilterSelect::GetFtrCodes(CStringArray& names)
{
	names.RemoveAll();
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	for( int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrlayer = pDS->GetFtrLayerByIndex(i);
		if( !pFtrlayer )continue;
		if( pFtrlayer->IsEmpty() || pFtrlayer->IsDeleted() || !pFtrlayer->IsVisible() )
			continue;
		
		for( int j=0; j<pFtrlayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pFtrlayer->GetObject(j);
			if( !pFtr )continue;

			InsertStringintoArray(names,pFtr->GetCode());
		}
	}
}

void CDlgFilterSelect::GetTextContents(CStringArray& names)
{
	names.RemoveAll();
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	for( int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrlayer = pDS->GetFtrLayerByIndex(i);
		if( !pFtrlayer )continue;
		if( pFtrlayer->IsEmpty() || pFtrlayer->IsDeleted() || !pFtrlayer->IsVisible() )
			continue;
		
		for( int j=0; j<pFtrlayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pFtrlayer->GetObject(j);
			if( !pFtr )continue;

			CGeoText *pText = DYNAMIC_DOWNCAST(CGeoText,pFtr->GetGeometry());
			if( !pText )continue;
			
			InsertStringintoArray(names,pText->GetText());
		}
	}
}

void CDlgFilterSelect::GetSymbolNames(CStringArray& names)
{
	names.RemoveAll();
	
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	for( int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrlayer = pDS->GetFtrLayerByIndex(i);
		if( !pFtrlayer )continue;
		if( pFtrlayer->IsEmpty() || pFtrlayer->IsDeleted() || !pFtrlayer->IsVisible() )
			continue;
		
		for( int j=0; j<pFtrlayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pFtrlayer->GetObject(j);
			if( !pFtr )continue;
			
			InsertStringintoArray(names,CString(pFtr->GetGeometry()->GetSymbolName()));
		}
	}
}


void CDlgFilterSelect::GetTextFonts(CStringArray& names)
{
	names.RemoveAll();
	
	TEXT_SETTINGS0 sets;
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	for( int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrlayer = pDS->GetFtrLayerByIndex(i);
		if( !pFtrlayer )continue;
		if( pFtrlayer->IsEmpty() || pFtrlayer->IsDeleted() || !pFtrlayer->IsVisible() )
			continue;
		
		for( int j=0; j<pFtrlayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pFtrlayer->GetObject(j);
			if( !pFtr )continue;
			
			CGeoText *pText = DYNAMIC_DOWNCAST(CGeoText,pFtr->GetGeometry());
			if( !pText )continue;
			
			pText->GetSettings(&sets);
			InsertStringintoArray(names,sets.strFontName);
		}
	}
}

void CDlgFilterSelect::GetGroupNames(CStringArray& names)
{
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();

	for( int i=0; i<pDS->GetObjectGroupCount(); i++)
	{
		InsertStringintoArray(names,pDS->GetObjectGroup(i)->name);
	}
}

void CDlgFilterSelect::RefreshProps()
{
	FillPropList();
	m_wndPropList.RedrawWindow();
}

void CDlgFilterSelect::FillPropList()
{
	m_names.RemoveAll();
	
	m_wndPropList.RemoveAll();

	CUIFProp *pGroup = new CUIFProp (StrFromResID(IDS_PROPERTIES_BASE));

	CUIFLayerProp *plp = new CUIFLayerProp(StrFromResID(IDS_LAYER_NAME),"");
	plp->EnableCheck(TRUE);
	plp->EnableMutiSelMode(TRUE);
	plp->EnableNotEmpty(TRUE);
	plp->EnableLocalLayers(TRUE);
	plp->EnableNotLocalLayers(TRUE);
	plp->SetLayersOption(m_pDoc->GetDlgDataSource());
	pGroup->AddSubItem(plp);
	m_names.Add(StrFromResID(IDS_LAYER_NAME)); m_names.Add(FIELDNAME_LAYERNAME);

	CUIFMultiColorProp *pcp = new CUIFMultiColorProp(StrFromResID(IDS_COLOR),"");
	pcp->EnableCheck(TRUE);
	GetColors(pcp->m_colors);
	pGroup->AddSubItem(pcp);
	m_names.Add(StrFromResID(IDS_COLOR)); m_names.Add(FIELDNAME_GEOCOLOR);

	CUIFCheckListProp *pchkp = new CUIFCheckListProp(StrFromResID(IDS_SELCOND_LINETYPE),"");
	pchkp->EnableCheck(TRUE);
	CStringArray names;
	names.Add(PencodeToText(penLine));
	names.Add(PencodeToText(penSpline));
	names.Add(PencodeToText(penStream));
	names.Add(PencodeToText(penArc));
	names.Add(PencodeToText(pen3PArc));
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromResID(IDS_SELCOND_LINETYPE)); m_names.Add(FIELDNAME_LINETYPE);

	CUIFPropEx *pp = new CUIFPropEx((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_GEOLINETYPESCALE),(_variant_t)(float)0);
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_GEOLINETYPESCALE)); m_names.Add(FIELDNAME_GEOCURVE_LINETYPESCALE);

	pp = new CUIFPropEx((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_GEOWIDTH),(_variant_t)(float)0);
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_GEOWIDTH)); m_names.Add(FIELDNAME_GEOCURVE_LINEWIDTH);

	pp = new CUIFPropEx((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_DIRPOINT_ANGLE),"",(LPCTSTR)StrFromResID(IDS_SYNTAX_RANGE));
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_DIRPOINT_ANGLE)); m_names.Add(FIELDNAME_GEOPOINT_ANGLE);

	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_PARALLEL_WIDTH),"",(LPCTSTR)StrFromResID(IDS_SYNTAX_RANGE));
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_PARALLEL_WIDTH)); m_names.Add(FIELDNAME_GEOCURVE_WIDTH);

	pchkp = new CUIFCheckListProp((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_FTRCODE),"");
	pchkp->EnableCheck(TRUE);
	names.RemoveAll();
	GetFtrCodes(names);
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_FTRCODE)); m_names.Add(FIELDNAME_FTRCODE);

	pchkp = new CUIFCheckListProp((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_GEOSYMBOLNAME),"");
	pchkp->EnableCheck(TRUE);
	names.RemoveAll();
	GetSymbolNames(names);
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_GEOSYMBOLNAME)); m_names.Add(FIELDNAME_SYMBOLNAME);

	pchkp = new CUIFCheckListProp(StrFromResID(IDS_GROUPNAME),"");
	pchkp->EnableCheck(TRUE);
	names.RemoveAll();
	GetGroupNames(names);
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromResID(IDS_GROUPNAME)); m_names.Add(FIELDNAME_FTRGROUPID);

	pchkp = new CUIFCheckListProp((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_FTRPURPOSE),"");
	pchkp->EnableCheck(TRUE);
	names.RemoveAll();
	names.Add(StrFromResID(IDS_PURPOSE_COLLECT));
	names.Add(StrFromResID(IDS_PURPOSE_EDB));
	names.Add(StrFromResID(IDS_PURPOSE_MARK));
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_FTRPURPOSE)); m_names.Add(FIELDNAME_FTRPURPOSE);

	pp = new CUIFSymbolizeFlagProp((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_SYMBOLIZEFLAG),(long)0);
	pp->EnableCheck(TRUE);
	pp->AllowEdit(FALSE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_SYMBOLIZEFLAG)); m_names.Add(FIELDNAME_SYMBOLIZEFLAG);

	m_wndPropList.AddProperty(pGroup);

	pGroup = new CUIFProp ((LPCTSTR)StrFromResID(IDS_TEXT_PROPS));

	pchkp = new CUIFCheckListProp((LPCTSTR)StrFromLocalResID(IDS_FIELDNAME_TEXT_CONTENT),"");
	pchkp->EnableCheck(TRUE);
	names.RemoveAll();
	GetTextContents(names);
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_CONTENT)); m_names.Add(FIELDNAME_GEOTEXT_CONTENT);

	pchkp = new CUIFCheckListProp(StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT),"");
	pchkp->EnableCheck(TRUE);
	names.RemoveAll();
	GetTextFonts(names);
	pchkp->SetList(names,NULL);
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_FONT)); m_names.Add(FIELDNAME_GEOTEXT_FONT);

	pp = new CUIFPropEx(StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT),(_variant_t)(float)10);
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARHEIGHT)); m_names.Add(FIELDNAME_GEOTEXT_CHARHEIGHT);

	pp = new CUIFPropEx(StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS),(_variant_t)(float)1);
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_CHARWIDTHS)); m_names.Add(FIELDNAME_GEOTEXT_CHARWIDTHS);

	long lv = TAH_LEFT|TAV_TOP;
	CString str;
	AlignToString(lv,str,TRUE);
	pchkp = new CUIFCheckListProp(StrFromLocalResID(IDS_FIELDNAME_TEXT_ALIGNTYPE),(LPCTSTR)str);
	AddAlignPropOptions(pchkp);
	pchkp->EnableCheck(TRUE);
	pchkp->OptionsToCheckList();
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_ALIGNTYPE)); m_names.Add(FIELDNAME_GEOTEXT_ALIGNTYPE);

	lv = SHRUGN;
	InclinedTypeToString(lv,str,TRUE);
	pchkp = new CUIFCheckListProp(StrFromLocalResID(IDS_FIELDNAME_TEXT_INCLINE),(LPCTSTR)str);
	AddInclinedTypePropOptions(pchkp);
	pchkp->EnableCheck(TRUE);
	pchkp->OptionsToCheckList();
	pGroup->AddSubItem(pchkp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_INCLINE)); m_names.Add(FIELDNAME_GEOTEXT_INCLINE);

	pp = new CUIFPropEx(StrFromLocalResID(IDS_FIELDNAME_TEXT_INCLINEANGLE),(_variant_t)(float)0);
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromLocalResID(IDS_FIELDNAME_TEXT_INCLINEANGLE)); m_names.Add(FIELDNAME_GEOTEXT_INCLINEANGLE);

	m_wndPropList.AddProperty(pGroup);	

	pGroup = new CUIFProp (StrFromResID(IDS_GRAPH_PROP));

	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_EQUALZ),(LPCTSTR)StrFromResID(IDS_YES));
	pp->EnableCheck(TRUE);
	pp->AllowEdit(FALSE);
	pp->AddOption(StrFromResID(IDS_YES));
	pp->AddOption(StrFromResID(IDS_NO));
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_EQUALZ)); m_names.Add(FIELDNAME_EQUALZ);
	
	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_CMDPLANE_CLOSETOLER),(LPCTSTR)StrFromResID(IDS_YES));
	pp->EnableCheck(TRUE);
	pp->AllowEdit(FALSE);
	pp->AddOption(StrFromResID(IDS_YES));
	pp->AddOption(StrFromResID(IDS_NO));
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_CMDPLANE_CLOSETOLER)); m_names.Add(FIELDNAME_CLOSED);	
	
	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_SELCOND_LINEDIR),(LPCTSTR)StrFromResID(IDS_COND_CLOCKWISE));
	pp->EnableCheck(TRUE);
	pp->AllowEdit(FALSE);
	pp->AddOption(StrFromResID(IDS_COND_CLOCKWISE));
	pp->AddOption(StrFromResID(IDS_COND_ANTICLOCKWISE));	
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_SELCOND_LINEDIR)); m_names.Add(FIELDNAME_CLOCKWISE);

	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_Z_VALUE),"",(LPCTSTR)StrFromResID(IDS_SYNTAX_RANGE));
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_Z_VALUE)); m_names.Add(FIELDNAME_HEIGHT);

	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_KEYPT_NUM),"",(LPCTSTR)StrFromResID(IDS_SYNTAX_RANGE));
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_KEYPT_NUM)); m_names.Add(FIELDNAME_POINTNUM);

	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_SELCOND_LEN),"",(LPCTSTR)StrFromResID(IDS_SYNTAX_RANGE));
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_SELCOND_LEN)); m_names.Add(FIELDNAME_LENGTH);

	pp = new CUIFPropEx((LPCTSTR)StrFromResID(IDS_SELCOND_AREA),"",(LPCTSTR)StrFromResID(IDS_SYNTAX_RANGE));
	pp->EnableCheck(TRUE);
	pGroup->AddSubItem(pp);
	m_names.Add(StrFromResID(IDS_SELCOND_AREA)); m_names.Add(FIELDNAME_AREA);

	m_wndPropList.AddProperty(pGroup);

	pGroup = new CUIFProp ((LPCTSTR)StrFromResID(IDS_OTHERS));

	CUIFConditionsProp *pcdp = new CUIFConditionsProp(StrFromResID(IDS_OTHERCONDS),"");
	pcdp->EnableCheck(TRUE);

	CStringArray arrFields, arNames;
	m_pDoc->GetDlgDataSource()->GetAllField(TRUE,arrFields,arNames);

	//删除上面已经列出的字段
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_LAYERNAME);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOCOLOR);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOCLASS);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRID);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRDELETED);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRDISPLAYORDER);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRGROUPID);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRCODE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_CONTENT);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_FONT);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_CHARHEIGHT);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRPURPOSE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_LINETYPE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOCURVE_LINETYPESCALE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_SYMBOLIZEFLAG);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_FTRVISIBLE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOPOINT_ANGLE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOCURVE_WIDTH);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOCURVE_LINEWIDTH);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_SYMBOLNAME);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_CHARWIDTHS);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_ALIGNTYPE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_INCLINE);
	DeleteFieldFromArray(arrFields,arNames,FIELDNAME_GEOTEXT_INCLINEANGLE);

	pcdp->m_arrFields.Copy(arNames);

	pGroup->AddSubItem(pcdp);
	m_names.Add(StrFromResID(IDS_OTHERCONDS)); m_names.Add(FIELDNAME_OTHERCONDS);

	m_wndPropList.AddProperty(pGroup);

}


LRESULT CDlgFilterSelect::OnPropertyChanged (WPARAM wParam, LPARAM lParam)
{
	CUIFProp* pProp = (CUIFProp*)lParam;
	if(!pProp)return 0;

	CUIFPropEx *pp = DYNAMIC_DOWNCAST(CUIFPropEx,pProp);
	if( !pp )
		return 0;

	pp->SetChecked(TRUE);
	pp->Redraw();

	if( StrFromResID(IDS_FIELDNAME_TEXT_CONTENT).CompareNoCase(pp->GetName())==0 ||
		StrFromResID(IDS_FIELDNAME_TEXT_FONT).CompareNoCase(pp->GetName())==0 ||
		StrFromResID(IDS_FIELDNAME_TEXT_CHARHEIGHT).CompareNoCase(pp->GetName())==0 )
	{
		UpdateData(TRUE);
		m_bText = TRUE;
		UpdateData(FALSE);
	}
	else if( StrFromResID(IDS_PARALLEL_WIDTH).CompareNoCase(pp->GetName())==0 )
	{
		UpdateData(TRUE);
		m_bParallel = TRUE;
		UpdateData(FALSE);
	}
	else if( StrFromResID(IDS_FIELDNAME_DIRPOINT_ANGLE).CompareNoCase(pp->GetName())==0 )
	{
		UpdateData(TRUE);
		m_bDirPoint = TRUE;
		UpdateData(FALSE);
	}
	return 0;
}


void CDlgFilterSelect::OnDo()
{
	OnOK();

}


void CDlgFilterSelect::OnUnDo()
{
	if( m_pDoc )
	{
		m_pDoc->DeselectAll();
		m_pDoc->OnSelectChanged();
	}
}


void CDlgFilterSelect::OnClearSettings()
{
	m_bArea = TRUE;
	m_bDirPoint = TRUE;
	m_bDLine = TRUE;
	m_bInverse = FALSE;
	m_bLine = TRUE;
	m_bParallel = TRUE;
	m_bPoint = TRUE;
	m_bText = TRUE;
	m_bAreaPoint = TRUE;
	m_nRange = 0;
	
	m_wndPropList.RemoveAll();

	FillPropList();
}


void CDlgFilterSelect::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType,cx,cy);

	CRect rcClient;
	GetClientRect(&rcClient);

	int x1 = m_rcStaticRect.left-m_rcClient.left;
	int x2 = m_rcStaticRect.right-m_rcClient.right;
	int y1 = m_rcStaticRect.top-m_rcClient.top;
	int y2 = m_rcStaticRect.bottom-m_rcClient.bottom;

	CRect rcNewPos;
	rcNewPos.left = rcClient.left+x1;
	rcNewPos.right = rcClient.right+x2;
	rcNewPos.top = rcClient.top+y1;
	rcNewPos.bottom = rcClient.bottom+y2;

	CWnd *pWnd = GetDlgItem(IDC_STATIC_RECT);
	if( pWnd )
		pWnd->MoveWindow(&rcNewPos);

	m_wndPropList.MoveWindow(&rcNewPos,FALSE);

}
