// DlgSelectLayer.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSelectLayer.h"
#include "DlgDataSource.h"
#include "SymbolLib.h"
#include "Scheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectLayer dialog


CDlgSelectLayer::CDlgSelectLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSelectLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSelectLayer)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDoc = NULL;
}


void CDlgSelectLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectLayer)
	DDX_Control(pDX, IDC_LIST_LAYERS, m_wndLayers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectLayer)
	ON_EN_CHANGE(IDC_EDIT_CODE, OnChangeEditCode)
	ON_LBN_DBLCLK(IDC_LIST_LAYERS, OnDblclkListLayers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSelectLayer message handlers


int CompareStringFromStart(CString& a, CString& b)
{
	if(a.GetLength()<=0 || b.GetLength()<=0)
		return 0;

	wchar_t input1[200] = {0}, input2[200] = {0};
	wstring wstr;
	ConvertCharToWstring((LPCTSTR)a,wstr,CP_ACP);			
	wcscpy(input1,(wchar_t*)wstr.c_str());

	ConvertCharToWstring((LPCTSTR)b,wstr,CP_ACP);			
	wcscpy(input2,(wchar_t*)wstr.c_str());
	
	int len = wcslen(input1);

	wcsupr(input1);
	wcsupr(input2);

	if(wcsstr(input2,input1)==input2)
		return len;

	return 0;
}


int CompareStringFromAny(CString& a, CString& b)
{
	if(a.GetLength()<=0 || b.GetLength()<=0)
		return 0;
	
	wchar_t input1[200] = {0}, input2[200] = {0};
	wstring wstr;
	ConvertCharToWstring((LPCTSTR)a,wstr,CP_ACP);			
	wcscpy(input1,(wchar_t*)wstr.c_str());
	
	ConvertCharToWstring((LPCTSTR)b,wstr,CP_ACP);			
	wcscpy(input2,(wchar_t*)wstr.c_str());
	
	int len = wcslen(input1);
	
	wcsupr(input1);
	wcsupr(input2);
	
	if(wcsstr(input2,input1)!=0)
		return len;
	
	return 0;
}


void CDlgSelectLayer::OnChangeEditCode() 
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_CODE);
	if( !pWnd )return;

	CString text;
	pWnd->GetWindowText(text);

	CDlgDataSource *pds = m_pDoc->GetDlgDataSource();
	
	CScheme *psch = gpCfgLibMan->GetScheme(pds->GetScale());
	if( !psch )return;
	

	int len1,len2,max1=-1,max2=-1, k1, k2;
	for( int i=0; i<psch->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pd = psch->GetLayerDefine(i);
		
		CString text1;
		text1.Format("%I64d",pd->GetLayerCode());

		len1 = CompareStringFromStart(text,text1);
		len2 = CompareStringFromStart(text,pd->GetLayerName());

		if( max1<0 || len1>max1 )
		{
			max1 = len1;
			k1 = i;
		}
		if( max2<0 || len2>max2 )
		{
			max2 = len2;
			k2 = i;
		}
	}

	if( max1<max2 )
	{
		max1 = max2;
		k1 = k2;
	}
	if( max1>0 )
	{
		m_wndLayers.SetCurSel(k1);
	}
	
}

void CDlgSelectLayer::OnOK() 
{
	// TODO: Add extra validation here
	
	int nsel = m_wndLayers.GetCurSel();
	if( nsel>=0 )
	{
		m_strLayer = m_arrLayerNames.GetAt(nsel);
	}	

	CDialog::OnOK();
}

BOOL CDlgSelectLayer::OnInitDialog() 
{
	CDialog::OnInitDialog();

	FillLayers();

	CWnd *pWnd = GetDlgItem(IDC_EDIT_CODE);
	if( pWnd )pWnd->SetFocus();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSelectLayer::FillLayers()
{
	if( !m_pDoc )return;
	
	CDlgDataSource *pds = m_pDoc->GetDlgDataSource();
	
	CScheme *psch = gpCfgLibMan->GetScheme(pds->GetScale());
	if( !psch )return;
	
	m_wndLayers.ResetContent();
	m_arrLayerNames.RemoveAll();

	int nSelect = -1;
	
	for( int i=0; i<psch->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pd = psch->GetLayerDefine(i);

		if( pds->GetFtrLayer(pd->GetLayerName())==NULL )
			continue;

		CString text;
		text.Format("%I64d  %s",pd->GetLayerCode(),(LPCTSTR)pd->GetLayerName());

		if( m_strLayer.CompareNoCase(pd->GetLayerName())==0 )
			nSelect = i;

		m_wndLayers.AddString(text);
		m_arrLayerNames.Add(pd->GetLayerName());
	}

	m_wndLayers.SetCurSel(nSelect);
}

void CDlgSelectLayer::OnDblclkListLayers() 
{
	OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// DlgSelectFtrLayer dialog

static BOOL gbArea = TRUE;
static BOOL gbLine = TRUE;
static BOOL gbLocal = TRUE;
static BOOL gbNotLocal = FALSE;
static BOOL gbPoint = TRUE;
static BOOL gbText = TRUE;
static BOOL gbUsed = FALSE;


CDlgSelectFtrLayer::CDlgSelectFtrLayer(CWnd* pParent /*=NULL*/,int nMode)
: CDialog(CDlgSelectFtrLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgSelectFtrLayer)
	m_bArea = gbArea;
	m_bLine = gbLine;
	m_bLocal = gbLocal;
	m_bNotLocal = gbNotLocal;
	m_bPoint = gbPoint;
	m_bText = gbText;
	m_bUsed = gbUsed;
	//}}AFX_DATA_INIT

	m_pRetLayer = NULL;
	m_bEditChanged = FALSE;
	m_nTimeElapes = 0;
	m_nSelMode = nMode;
	m_strLayers = _T("");
}


void CDlgSelectFtrLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSelectFtrLayer)
	DDX_Control(pDX, IDC_LIST_LAYERS, m_wndLayers);
	DDX_CBString(pDX, IDC_EDIT_LAYERNAME, m_strLayer);
	DDX_Check(pDX, IDC_CHECK_AREA, m_bArea);
	DDX_Check(pDX, IDC_CHECK_LINE, m_bLine);
	DDX_Check(pDX, IDC_CHECK_LOCALLAYERS, m_bLocal);
	DDX_Check(pDX, IDC_CHECK_NOLOCALLAYERS, m_bNotLocal);
	DDX_Check(pDX, IDC_CHECK_POINT, m_bPoint);
	DDX_Check(pDX, IDC_CHECK_TEXT, m_bText);
	DDX_Check(pDX, IDC_CHECK_USED, m_bUsed);
	DDX_Control(pDX, IDC_EDIT_LAYERNAME, m_wndEdit);
	DDX_Text(pDX, IDC_EDIT_LAYERS, m_strLayers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSelectFtrLayer, CDialog)
	//{{AFX_MSG_MAP(CDlgSelectFtrLayer)
	ON_CBN_EDITCHANGE(IDC_EDIT_LAYERNAME, OnChangeEditLayername)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LAYERS, OnDblclkListLayers)
	ON_BN_CLICKED(IDC_CHECK_AREA, OnCheck)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_EDIT_LAYERNAME, OnSelchangeLayername)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_CHECK_LINE, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_LOCALLAYERS, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_NOLOCALLAYERS, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_POINT, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_TEXT, OnCheck)
	ON_BN_CLICKED(IDC_CHECK_USED, OnCheck)
	ON_EN_CHANGE(IDC_EDIT_LAYERS, OnChangeEditLayers)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgSelectFtrLayer message handlers

CScrollBar* CDlgSelectFtrLayer::GetScrollBarCtrl(int nBar) const
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::GetScrollBarCtrl(nBar);
}

void CDlgSelectFtrLayer::OnOK() 
{
	m_pRetLayer = NULL;

	int nsel = -1;
	POSITION pos = m_wndLayers.GetFirstSelectedItemPosition();
	if (pos != NULL)
		nsel = m_wndLayers.GetNextSelectedItem(pos);
	
	if( nsel<0 )
	{
		OnCancel();
		return;
	}
	else
	{
		DWORD_PTR data = m_wndLayers.GetItemData(nsel);
		m_pRetLayer = (CFtrLayer*)data;
	}

	UpdateData(TRUE);

	if(m_pRetLayer)
	{
		m_SingleLayer = m_pRetLayer->GetName();
	}	

	// 避免输入后直接回车结果不对的情况
	if (0 && m_pRetLayer && m_pDS)
	{
		CScheme *psch = gpCfgLibMan->GetScheme(m_pDS->GetScale());	
		if( !psch )return;

		CString name = m_pRetLayer->GetName();
		__int64 code = 0;
		BOOL bFind = psch->FindLayerIdx(FALSE,code,name);

		CString text;
		text.Format("%I64d",code);

		m_SingleLayer = name;

		CString strLayer;
		m_wndEdit.GetWindowText(strLayer);

		if ( strstr(name,strLayer) == NULL && (bFind && strstr(text,strLayer) == NULL))
		{
			m_pRetLayer = m_pDS->GetFtrLayer(strLayer);
			if (!m_pRetLayer)
			{
				int pos = -1;
				for (int i=0; i<m_pDS->GetFtrLayerCount(); i++)
				{
					CFtrLayer *pLayer = m_pDS->GetFtrLayerByIndex(i);
					if (!pLayer) continue;

					code = 0;
					name = pLayer->GetName();
					bFind = psch->FindLayerIdx(FALSE,code,name);
					text.Format("%I64d",code);

					const char *ret1, *ret2;
					if ((ret1=strstr(name,strLayer)) || (bFind && (ret2=strstr(text,strLayer))))
					{
						if (ret1 && (pos < 0 || ret1-name < pos))
						{
							m_pRetLayer = pLayer;
							pos = ret1-name;
							if (pos == 0)
							{
								break;
							}
						}						
						else if (ret2 && (pos < 0 || ret2-(LPCTSTR)text < pos))
						{
							m_pRetLayer = pLayer;
							pos = ret2-(LPCTSTR)text;
							if (pos == 0)
							{
								break;
							}
						}
					}
				}
			}
		}

	}
	if( m_wndEdit.GetCurSel()>=0 )
		m_wndEdit.GetLBText(m_wndEdit.GetCurSel(),m_strLayer);

	if( m_wndEdit.FindStringExact(-1,m_strLayer)<0 )
	{
		m_wndEdit.InsertString(0,m_strLayer);
	}

	gbArea = m_bArea;
	gbLine = m_bLine;
	gbLocal = m_bLocal;
	gbNotLocal = m_bNotLocal;
	gbPoint = m_bPoint;
	gbText = m_bText;
	gbUsed = m_bUsed;

	{
		//保存历史记录
		TCHAR module[_MAX_PATH]={0};	
		GetModuleFileName(NULL,module,_MAX_PATH);
		CString dir = module;
		int pos = dir.ReverseFind(_T('\\'));
		if( pos>0 )
		{
			dir = dir.Left(pos);
			pos = dir.ReverseFind(_T('\\'));		
		}
		
		if( pos>0 )
		{
			dir = dir.Left(pos+1) + _T("Config");
		}
		else
		{
			dir = _T("Config");
		}
		
		dir += "\\RecentFindLayer.txt";
		
		FILE *fp = fopen(dir,"wt");
		if( fp )
		{
			for( int i=0; i<m_wndEdit.GetCount() && i<20; i++)
			{
				CString text;
				m_wndEdit.GetLBText(i,text);
				if( !text.IsEmpty() )
					fprintf(fp,"%s\n",(LPCTSTR)text);
			}
			fclose(fp);
		}
	}

	CDialog::OnOK();
}

void CDlgSelectFtrLayer::OnChangeEditLayername() 
{
	m_bEditChanged = TRUE;
	m_nTimeElapes = 0;

	UpdateData(TRUE);
}

void CDlgSelectFtrLayer::OnDblclkListLayers(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(m_nSelMode==LAYER_SEL_MODE_SINGLESEL)
		OnOK();
	else
	{		
		int nsel = -1;
		POSITION pos = m_wndLayers.GetFirstSelectedItemPosition();
		if (pos != NULL)
			nsel = m_wndLayers.GetNextSelectedItem(pos);
		ASSERT(nsel>=0);	
		DWORD_PTR data = m_wndLayers.GetItemData(nsel);
		CFtrLayer *pLayer = (CFtrLayer*)data;

		if( m_strLayers.Find(CString(pLayer->GetName()) + ",")>=0 )
		{
			m_strLayers.Replace(CString(pLayer->GetName()) + ",","");
		}
		else if( m_strLayers.Find("," + CString(pLayer->GetName()))>=0 )
		{
			m_strLayers.Replace("," + CString(pLayer->GetName()),"");
		}
		else if(m_strLayers==CString(pLayer->GetName()))
		{
			m_strLayers.Empty();
		}
		else

		if(m_strLayers.IsEmpty())		//空格也是合法的层名，此处没排除
			m_strLayers = pLayer->GetName();
		else
		{
			m_strLayers+=_T(",");
			m_strLayers+=pLayer->GetName();
		}
		UpdateData(FALSE);	
		
		UpdateLayerListDisplay();
	}
}

BOOL CDlgSelectFtrLayer::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndLayers.ModifyStyleEx(0,LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	m_wndLayers.SetExtendedStyle(m_wndLayers.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	m_wndLayers.InsertColumn(0,StrFromResID(IDS_LAYER_NOCONFIG));	
	m_wndLayers.InsertColumn(1,StrFromResID(IDS_FIELDNAME_MAPNAME));
	m_wndLayers.InsertColumn(2,StrFromResID(IDS_FIELDNAME_INHERENT));
	
	m_wndLayers.SetColumnWidth(0,400);
	m_wndLayers.SetColumnWidth(1,80);
	m_wndLayers.SetColumnWidth(2,80);
	
	FillLayers();

	SetTimer(1,300,NULL);

	{
		//获取历史记录
		TCHAR module[_MAX_PATH]={0};	
		GetModuleFileName(NULL,module,_MAX_PATH);
		CString dir = module;
		int pos = dir.ReverseFind(_T('\\'));
		if( pos>0 )
		{
			dir = dir.Left(pos);
			pos = dir.ReverseFind(_T('\\'));		
		}
		
		if( pos>0 )
		{
			dir = dir.Left(pos+1) + _T("Config");
		}
		else
		{
			dir = _T("Config");
		}

		dir += "\\RecentFindLayer.txt";

		FILE *fp = fopen(dir,"rt");
		if( fp )
		{
			char line[256];
			while( !feof(fp) )
			{
				memset(line,0,sizeof(line));
				fgets(line,sizeof(line)-1,fp);
				CString strLine = line;
				strLine.Remove('\r');
				strLine.Remove('\n');
				if( !strLine.IsEmpty() )
				{
					m_wndEdit.AddString(strLine);
				}
			}
			fclose(fp);
		}
	}
	if (m_nSelMode==LAYER_SEL_MODE_SINGLESEL)
	{
		GetDlgItem(IDC_LAYER)->ShowWindow(SW_HIDE);
		CRect rect,rect0;
		GetDlgItem(IDC_POS1)->GetWindowRect(&rect);
		GetWindowRect(&rect0);
		rect0.bottom = rect.bottom;
		SetWindowPos(NULL,rect0.left,rect0.top,rect0.Width(),rect0.Height(),SWP_NOACTIVATE | SWP_NOZORDER);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_SHOW);
	}

	m_wndEdit.SetFocus();

	UpdateLayerListDisplay();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


CString ChineseToLetter(LPCTSTR text);

void CDlgSelectFtrLayer::FillLayers()
{
	if( !m_pDS )return;
	
	CDlgDataSource *pds = m_pDS;
	int nlayer = pds->GetFtrLayerCount();
	
	CScheme *psch = gpCfgLibMan->GetScheme(pds->GetScale());
	if( !psch )return;
	
	m_wndLayers.DeleteAllItems();

	if( m_wndEdit.GetCurSel()>=0 )
		m_wndEdit.GetLBText(m_wndEdit.GetCurSel(),m_strLayer);

	CString text = m_strLayer;
	int nItem = 0;

	int len1,len2,len3,len4, max1=-1, max2=-1, k1 = -1;
	CArray<int,int> arrLens;
	arrLens.SetSize(nlayer);

	for( int i=0; i<nlayer; i++)
	{
		arrLens[i] = 0;

		CFtrLayer *pLayer = pds->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		
		CString name = pLayer->GetName();
		__int64 code = 0;
		BOOL bFind = psch->FindLayerIdx(FALSE,code,name);
		
		if( bFind )
		{
			if( !m_bLocal )
				continue;
		}
		else
		{
			if( !m_bNotLocal )
				continue;
		}
		if( m_bUsed )
		{
			if( pLayer->IsEmpty() )
				continue;
		}
		CString text2;
		if( bFind )
		{
			CSchemeLayerDefine* pd = psch->GetLayerDefine(name);
			if( pd )
			{
				text2 = pd->GetAccel();
				
				if( (pd->GetGeoClass()==CLS_GEOPOINT||pd->GetGeoClass()==CLS_GEODIRPOINT) && m_bPoint );
				else if( (pd->GetGeoClass()==CLS_GEOCURVE||pd->GetGeoClass()==CLS_GEOPARALLEL||pd->GetGeoClass()==CLS_GEODCURVE) &&
					m_bLine );
				else if( (pd->GetGeoClass()==CLS_GEOSURFACE) && m_bArea );
				else if( (pd->GetGeoClass()==CLS_GEOTEXT) && m_bText );
				else
					continue;
			}
		}

		CString text1;
		text1.Format("%I64d",code);
		
		len1 = CompareStringFromStart(text,text1);
		len2 = CompareStringFromAny(text,CString(name));
		len3 = CompareStringFromStart(text,text2);
		len4 = CompareStringFromAny(text,ChineseToLetter(name));

		len1 = (len1>=len3?len1:len3);
		len2 = (len2>=len4?len2:len4);
		max1 = (len1>=len2?len1:len2);

		arrLens[i] = max1;

		if( max2<0 || max1>max2 )
		{
			max2 = max1;
			k1 = i;
		}
	}

	if(text.GetLength()>0 && max2==0)
		return;

	for( i=0; i<nlayer; i++)
	{
		CFtrLayer *pLayer = pds->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		
		CString name = pLayer->GetName();
		__int64 code = 0;
		BOOL bFind = psch->FindLayerIdx(FALSE,code,name);
		
		if( bFind )
		{
			if( !m_bLocal )
				continue;
		}
		else
		{
			if( !m_bNotLocal )
				continue;
		}
		if( m_bUsed )
		{
			if( pLayer->IsEmpty() )
				continue;
		}

		CString text2;
		if( bFind )
		{
			CSchemeLayerDefine* pd = psch->GetLayerDefine(name);
			if( pd )
			{
				text2 = pd->GetAccel();

				if( (pd->GetGeoClass()==CLS_GEOPOINT||pd->GetGeoClass()==CLS_GEODIRPOINT) && m_bPoint );
				else if( (pd->GetGeoClass()==CLS_GEOCURVE||pd->GetGeoClass()==CLS_GEOPARALLEL||pd->GetGeoClass()==CLS_GEODCURVE) &&
					m_bLine );
				else if( (pd->GetGeoClass()==CLS_GEOSURFACE) && m_bArea );
				else if( (pd->GetGeoClass()==CLS_GEOTEXT) && m_bText );
				else
					continue;
			}
		}
		
		if(arrLens[i]!=max2)
			continue;
		
		CString text1;
		text1.Format("%I64d  %s",code,name);
		m_wndLayers.InsertItem(nItem,"");
		m_wndLayers.SetItemText(nItem,0,text1);
		
		m_wndLayers.SetItemText(nItem,1,pLayer->GetMapName());
		if( bFind )
			m_wndLayers.SetItemText(nItem,2,CString("Y"));
		else
			m_wndLayers.SetItemText(nItem,2,CString());
		
		m_wndLayers.SetItemData(nItem, (DWORD_PTR)pLayer);

		nItem++;
	}

	if( k1>=0 )
	{
		m_wndLayers.SetItem(0, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, 
			LVIS_SELECTED, 0);

		m_wndLayers.EnsureVisible(0,FALSE);		
	}
}

void CDlgSelectFtrLayer::UpdateLayerListDisplay()
{
	CFtrLayerArray arr;
	if(m_strLayers.GetLength()>0)
	{
		m_pDS->GetFtrLayersByNameOrCode(m_strLayers,arr);
	}

	for(int j=0; j<m_wndLayers.GetItemCount(); j++)
	{
		DWORD_PTR data0 = m_wndLayers.GetItemData(j);
		for(int i=0; i<arr.GetSize(); i++)
		{
			CFtrLayer *pLayer = arr[i];
			if ((DWORD_PTR)pLayer == data0)
			{
				break;
			}
		}

		if(i<arr.GetSize())
			m_wndLayers.SetHiliteRow(j,TRUE);
		else
			m_wndLayers.SetHiliteRow(j,FALSE);
	}

	m_wndLayers.RedrawWindow();
}


void CDlgSelectFtrLayer::OnCheck() 
{
	UpdateData(TRUE);

	OnChangeEditLayername();
}


void CDlgSelectFtrLayer::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==1 )
	{
		if( m_nTimeElapes>0 && m_bEditChanged )
		{
			FillLayers();
			UpdateLayerListDisplay();
			m_bEditChanged = FALSE;
		}
		m_nTimeElapes++;
	}

	CDialog::OnTimer(nIDEvent);
}


void CDlgSelectFtrLayer::OnSelchangeLayername()
{
	OnChangeEditLayername();
}


void CDlgSelectFtrLayer::OnButtonAdd() 
{
	ASSERT(m_nSelMode==LAYER_SEL_MODE_MUTISEL);
	int nsel = -1;
	POSITION pos = m_wndLayers.GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		nsel = m_wndLayers.GetNextSelectedItem(pos);
	
		DWORD_PTR data = m_wndLayers.GetItemData(nsel);
		CFtrLayer *pLayer = (CFtrLayer*)data;
		if(m_strLayers.IsEmpty())		//空格也是合法的层名，此处没排除
			m_strLayers = pLayer->GetName();
		else
		{
			m_strLayers+=_T(",");
			m_strLayers+=pLayer->GetName();
		}
	}
	
	UpdateData(FALSE);	
	UpdateLayerListDisplay();
}


void CDlgSelectFtrLayer::OnChangeEditLayers() 
{
	UpdateData();
	UpdateLayerListDisplay();
}


void CDlgSelectFtrLayer::OnButtonClear()
{
	m_strLayers.Empty();
	UpdateData(FALSE);
	UpdateLayerListDisplay();
}