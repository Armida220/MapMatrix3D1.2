// DlgMetaInput.cpp : implementation file
//

#include "stdafx.h"
#include "editBase.h"
#include "DlgMetaInput.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDlgMetaInput dialog

CDlgMetaInput::CDlgMetaInput(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMetaInput::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMetaInput)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pData = NULL;
}


void CDlgMetaInput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMetaInput)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMetaInput, CDialog)
	//{{AFX_MSG_MAP(CDlgMetaInput)
	ON_BN_CLICKED(ID_META_EDITFILE, OnMetaEditfile)
	ON_BN_CLICKED(ID_META_REFRESH, OnMetaRefresh)
	ON_BN_CLICKED(ID_META_RELOAD, OnMetaReload)
	ON_BN_CLICKED(ID_META_SETPROJECT, OnMetaSetProject)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMetaInput message handlers

BOOL CDlgMetaInput::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( m_pData==NULL )
		return FALSE;
	
	if (!m_wndPropList.Create (WS_VISIBLE | WS_CHILD, CRect(), this, 2))
	{
		TRACE0("Failed to create Properies Grid \n");
		return -1;      // fail to create
	}
	
	CWnd *pWnd = GetDlgItem(IDC_STATIC_LISTPOS);
	if( pWnd!=NULL )
	{
		CRect rcWnd;
		pWnd->GetWindowRect(&rcWnd);
		ScreenToClient(&rcWnd);
		
		m_wndPropList.SetWindowPos(NULL,rcWnd.left,rcWnd.top,
			rcWnd.Width(),rcWnd.Height(),0);
	}

	m_wndPropList.EnableHeaderCtrl(TRUE,StrFromResID(IDS_NAME0),StrFromResID(IDS_VALUE));
	
	FillList();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RemoveSpace(CString& text)
{
	char *p = new char[text.GetLength()+1];

	char *t = p;
	int i = 0;
	while( text[i]!=0 )
	{
		if( text[i]!=' ' )
			*t++ = text[i];
		i++;
	}
	*t = 0;
	text = p;
	delete[] p;
}


void CDlgMetaInput::FillList()
{
	m_wndPropList.RemoveAll();
	int nField = m_pData->m_mapValues.GetFieldCount(), type;
	_variant_t var;
	const CVariantEx *pv;
	CString field,name;
	for( int i=0; i<nField; i++)
	{
		m_pData->m_mapValues.GetField(i,field,type,name);
		if( m_pData->m_mapValues.GetValue(0,i,pv) )
		{
			var = pv->m_variant;
		}

		if( m_pData->m_mapItems[i].IsHaveEnumValues() )
		{
			CStringArray arrValues;
			m_pData->m_mapItems[i].GetEnumValues(&arrValues);
			if( arrValues.GetSize()>0 )
			{
				CString strValue;
				m_pData->m_mapItems[i].ConvertValueToString(var,strValue);
				RemoveSpace(strValue);
				var = (LPCTSTR)m_pData->m_mapItems[i].ConvertEnumValueToName((LPCTSTR)strValue);
				COleVariant var1(var);
				CUIFProp *pProp = new CUIFProp(field, var1);
				for( int i=0; i<arrValues.GetSize(); i++)
				{
					pProp->AddOption((LPCTSTR)arrValues[i]);
				}
				m_wndPropList.AddProperty(pProp,FALSE);
			}
			else
			{
				COleVariant var1(var);
				CUIFProp *pProp = new CUIFProp(field, var1);
				m_wndPropList.AddProperty(pProp,FALSE);
			}
		}
		else
		{
			COleVariant var1(var);
			CUIFProp *pProp = new CUIFProp(field, var1);
			m_wndPropList.AddProperty(pProp,FALSE);
		}
	}
	
	if( m_pData->m_layValues.GetItemCount()>0 )
	{
		nField = m_pData->m_layValues.GetFieldCount();

		for( int j=0; j<m_pData->m_layValues.GetItemCount(); j++)
		{
			CString strName;
			strName.Format(StrFromResID(IDS_LAYERMETA_FORMAT),j+1);
			CUIFProp *pProp0 = new CUIFProp(strName);

			for( i=0; i<nField; i++)
			{
				m_pData->m_layValues.GetField(i,field,type,name);
				if( m_pData->m_layValues.GetValue(j,i,pv) )
				{
					var = pv->m_variant;
				}
				COleVariant var1(var);
				CUIFProp *pProp = new CUIFProp(field,var1);
				pProp0->AddSubItem(pProp);
			}
			
			m_wndPropList.AddProperty(pProp0);
		}
	}


	m_wndPropList.RedrawWindow();
}


void CDlgMetaInput::GetValues()
{
	int nField = m_pData->m_mapValues.GetFieldCount(), type;
	_variant_t var;
	CString field,name;
	for( int i=0; i<nField; i++)
	{
		m_pData->m_mapValues.GetField(i,field,type,name);

		CUIFProp *pProp = m_wndPropList.GetProperty(i);

		if( m_pData->m_mapItems[i].IsHaveEnumValues() )
		{
			CString strValue = (LPCTSTR)m_pData->m_mapItems[i].ConvertEnumNameToValue((LPCTSTR)(_bstr_t)pProp->GetValue());
			m_pData->m_mapItems[i].ConvertStringToValue(strValue,var);
		}
		else
			var = pProp->GetValue();

		m_pData->m_mapValues.SetValue(0,field,&CVariantEx(var));
	}

	const CVariantEx *pv;
	if( m_pData->m_mapValues.GetValue(0,m_pData->m_strLayNumField,pv) )
	{
		int nLay = (short)pv->m_variant;
		m_pData->CreateLayerValues(nLay);
	}

	for( int j=0; j<m_pData->m_layValues.GetItemCount(); j++)
	{
		CUIFProp *pProp0 = m_wndPropList.GetProperty(i+j);
		if( pProp0==NULL )break;
		
		if( pProp0->IsGroup() )
		{
			int nCount = pProp0->GetSubItemsCount();
			for( int k=0; k<nCount; k++)
			{
				m_pData->m_layValues.GetField(k,field,type,name);
				
				CUIFProp *pProp = pProp0->GetSubItem(k);
				var = pProp->GetValue();
				m_pData->m_layValues.SetValue(j,field,&CVariantEx(var));
			}
		}
	}
	
}

void CDlgMetaInput::OnOK() 
{
	GetValues();
	
	CDialog::OnOK();
}

void CDlgMetaInput::OnMetaEditfile() 
{
	CString strCmd;
	strCmd.Format("notepad %s",m_pData->m_strValuePath);
	WinExec(strCmd,SW_SHOW);
}

void CDlgMetaInput::OnMetaRefresh() 
{
	GetValues();
	FillList();
}

void CDlgMetaInput::OnMetaReload() 
{
	m_pData->ReloadValues();
	FillList();
}

#include "tm.h"


char *FindNumber( char *p, int *x)
{
	char buf[100] = {0};
	int bfind = 0;
	char *t = p, *d = buf;
	while( *t!='\0' )
	{
		if( !bfind )
		{
			if( *t>='0' && *t<='9' )
				bfind = 1;
		}

		if( bfind )
		{
			if( *t>='0' && *t<='9' )
				*d++ = *t;
			else break;
		}
		t++;
	}
	*x = atol(buf);
	return t;
}

CString PrintDegree(double x, int bfloor)
{
	x = (x*180/PI);

	int x1 = (int)(x*3600 + 0.1);

	int nx, ny, nz;

	nx = x1/3600;
	ny = (x1/60)%60;
	nz = (x1%60);

	CString text;
	text.Format("%2d%02d%02d",nx,ny,0);
	RemoveSpace(text);

	return text;
}

void RoundGeoValue(double& x, int bfloor)
{
	x = (x*180/PI);
	
	if( bfloor )
		x = floor(x*60 + 1e-6 )/60;
	else
		x = ceil(x*60 - 1e-6 )/60;

	x = x*PI/180;
}


CString PrintDegreeRange(double x0, double x1)
{
	return PrintDegree(x0,1)+"-"+PrintDegree(x1,0);
}

void CDlgMetaInput::OnMetaSetProject()
{
	CTM tm;
	if( !tm.ShowDatum() )
		return;

	TMProjectionZone t1;
	TMDatum t2;

	memset(&t1,0,sizeof(t1));
	memset(&t2,0,sizeof(t2));

	tm.GetDatum((BYTE*)t2.tmName,&t2.a,&t2.b,&t2.dx,&t2.dy,&t2.dz);
	tm.GetProjection((BYTE*)t1.zoneName,&t1.central,&t1.origin_Lat,&t1.false_Easting,&t1.false_Northing,&t1.scale);
	
	CString tmName = t2.tmName;
	tmName.Remove('*');

	//--需要获取以下参数
	//大地基准
	CVariantEx varex;
	if( stricmp(tmName,StrFromResID(IDS_BEIJING_54))==0 )
	{
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_BEIJING_54_CS);
	}
	else if( stricmp(tmName,StrFromResID(IDS_XIAN_80))==0 )
	{
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_XIAN_80_CS);
	}
	else if( stricmp(tmName,"CGCS2000")==0 )
	{
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_CGCS2000_CS);
	}
	else
	{
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_BEIJING_54_CS);
	}

	m_pData->m_mapValues.SetValue(0,JBFIELD_JIZHUN,&varex);

	//中央经线
	//varex = (long)(t1.central*180/PI+0.1);
	varex = (long)t1.central;

	m_pData->m_mapValues.SetValue(0,JBFIELD_CENTERLONG,&varex);
	m_pData->m_mapValues.SetValue(0,JBFIELD_CENTERLONG0,&varex);

	//分带方式
	int x = 0;
	char *pos = FindNumber(t1.zoneName, &x);
	if( x==3 )
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_3DEGREE_STRIP);
	else if( x==6 )
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_6DEGREE_STRIP);
	else
		varex = (_variant_t)(LPCTSTR)StrFromResID(IDS_0DEGREE_STRIP);

	m_pData->m_mapValues.SetValue(0,JBFIELD_STRIPTYPE,&varex);
	m_pData->m_mapValues.SetValue(0,JBFIELD_STRIPTYPE0,&varex);
	
	//投影带号
	FindNumber(pos, &x);
	varex = (_variant_t)(long)x;

	m_pData->m_mapValues.SetValue(0,JBFIELD_STRIPNUM,&varex);

	//更新大数
	m_pData->RefreshBigNumber();

	//--需要计算以下参数
	//图廓经度纬度范围
	const CVariantEx *p;
	double x0, y0, x1, y1;
	m_pData->m_mapValues.GetValue(0, JBFIELD_MAPLBX,p);
	x0 = (double)(_variant_t)*p;
	m_pData->m_mapValues.GetValue(0, JBFIELD_MAPLBY,p);
	y0 = (double)(_variant_t)*p;
	m_pData->m_mapValues.GetValue(0, JBFIELD_MAPRTX,p);
	x1 = (double)(_variant_t)*p;
	m_pData->m_mapValues.GetValue(0, JBFIELD_MAPRTY,p);
	y1 = (double)(_variant_t)*p;

	double lat0 = 0,lon0 = 0,lat1 = 0,lon1 = 0;
	tm.ConvertToGeodetic(x0,y0,&lat0,&lon0);
	tm.ConvertToGeodetic(x1,y1,&lat1,&lon1);

	RoundGeoValue(lat0,1);
	RoundGeoValue(lon0,1);
	RoundGeoValue(lat1,0);
	RoundGeoValue(lon1,0);

	tm.ConvertFromGeodetic(lat0,lon0,&x0,&y0);
	tm.ConvertFromGeodetic(lat1,lon1,&x1,&y1);

	m_pData->m_mapValues.SetValue(0,JBFIELD_LONGRANGE,&CVariantEx((_variant_t)(LPCTSTR)PrintDegreeRange(lon0,lon1)));
	m_pData->m_mapValues.SetValue(0,JBFIELD_LATRANGE,&CVariantEx((_variant_t)(LPCTSTR)PrintDegreeRange(lat0,lat1)));

	PT_3D pts[4];
	pts[0].x = x0; pts[0].y = y0; 
	pts[1].x = x1; pts[1].y = y0; 
	pts[2].x = x1; pts[2].y = y1; 
	pts[3].x = x0; pts[3].y = y1; 
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPLBX,&CVariantEx((_variant_t)pts[0].x));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPLBY,&CVariantEx((_variant_t)pts[0].y));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPRBX,&CVariantEx((_variant_t)pts[1].x));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPRBY,&CVariantEx((_variant_t)pts[1].y));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPRTX,&CVariantEx((_variant_t)pts[2].x));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPRTY,&CVariantEx((_variant_t)pts[2].y));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPLTX,&CVariantEx((_variant_t)pts[3].x));
	m_pData->m_mapValues.SetValue(0,JBFIELD_MAPLTY,&CVariantEx((_variant_t)pts[3].y));
	
	m_pData->m_mapValues.SetValue(0,JBFIELD_ORIGINX,&CVariantEx((_variant_t)(pts[0].x-1000)));
	m_pData->m_mapValues.SetValue(0,JBFIELD_ORIGINY,&CVariantEx((_variant_t)(pts[0].y-1000)));
	
	FillList();
}