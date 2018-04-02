// DlgSupportGeoClassName.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSupportGeoClassName.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSupportGeoClassName dialog


CDlgSupportGeoClassName::CDlgSupportGeoClassName(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSupportGeoClassName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSupportGeoClassName)
	//}}AFX_DATA_INIT
	m_nSupportFirstgeo = 0;
}


void CDlgSupportGeoClassName::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSupportGeoClassName)
	DDX_Control(pDX, IDC_DLINE_CHECK, m_cDLine);
	DDX_Control(pDX, IDC_TEXT_CHECK, m_cText);
	DDX_Control(pDX, IDC_SURFACE_CHECK, m_cSurface);
	DDX_Control(pDX, IDC_POINT_CHECK, m_cPoint);
	DDX_Control(pDX, IDC_PARALLEL_CHECK, m_cParallel);
	DDX_Control(pDX, IDC_LINE_CHECK, m_cLine);
	DDX_Control(pDX, IDC_DIRPOINT_CHECK, m_cDirPoint);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSupportGeoClassName, CDialog)
	//{{AFX_MSG_MAP(CDlgSupportGeoClassName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSupportGeoClassName message handlers
BOOL CDlgSupportGeoClassName::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i=0; i<m_strSupportGeoClsName.GetSize(); i++)
	{
		CString str = m_strSupportGeoClsName.GetAt(i);
		if (str.CompareNoCase(StrFromResID(IDS_POINT)) == 0)
		{
			m_cPoint.SetCheck(BST_CHECKED);
		}
		else if(str.CompareNoCase(StrFromResID(IDS_DIRPOINT)) == 0)
		{
			m_cDirPoint.SetCheck(BST_CHECKED);
		}
		else if(str.CompareNoCase(StrFromResID(IDS_LINE)) == 0)
		{
			m_cLine.SetCheck(BST_CHECKED);
		}
		else if(str.CompareNoCase(StrFromResID(IDS_GEO_DCURVE)) == 0)
		{
			m_cDLine.SetCheck(BST_CHECKED);
		}
		else if(str.CompareNoCase(StrFromResID(IDS_PARLINE)) == 0)
		{
			m_cParallel.SetCheck(BST_CHECKED);
		}
		else if(str.CompareNoCase(StrFromResID(IDS_SURFACE)) == 0)
		{
			m_cSurface.SetCheck(BST_CHECKED);
		}
		else if(str.CompareNoCase(StrFromResID(IDS_TEXT)) == 0)
		{
			m_cText.SetCheck(BST_CHECKED);
		}
		
	}

	if (m_strSupportGeoClsName.GetSize() < 1)
	{
		m_cPoint.SetCheck(BST_CHECKED);
		m_cDirPoint.SetCheck(BST_CHECKED);
		m_cLine.SetCheck(BST_CHECKED);
		m_cDLine.SetCheck(BST_CHECKED);
		m_cParallel.SetCheck(BST_CHECKED);
		m_cSurface.SetCheck(BST_CHECKED);
		m_cText.SetCheck(BST_CHECKED);
	}

	return TRUE;
}

void CDlgSupportGeoClassName::OnOK() 
{
	// TODO: Add extra validation here
	m_strSupportGeoClsName.RemoveAll();

	if (m_cPoint.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_POINT));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEOPOINT;
		}
	}
	if (m_cDirPoint.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_DIRPOINT));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEODIRPOINT;
		}
	}
	if (m_cLine.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_LINE));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEOCURVE;
		}
	}
	if (m_cDLine.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_GEO_DCURVE));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEODCURVE;
		}
	}
	if (m_cParallel.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_PARLINE));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEOPARALLEL;
		}
	}
	if (m_cSurface.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_SURFACE));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEOSURFACE;
		}
	}
	if (m_cText.GetCheck()) 
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_TEXT));
		if (m_nSupportFirstgeo == 0)
		{
			m_nSupportFirstgeo = CLS_GEOTEXT;
		}
	}

	if (m_strSupportGeoClsName.GetSize() < 1)
	{
		m_strSupportGeoClsName.Add(StrFromResID(IDS_POINT));
		m_strSupportGeoClsName.Add(StrFromResID(IDS_DIRPOINT));
		m_strSupportGeoClsName.Add(StrFromResID(IDS_LINE));
		m_strSupportGeoClsName.Add(StrFromResID(IDS_GEO_DCURVE));
		m_strSupportGeoClsName.Add(StrFromResID(IDS_PARLINE));
		m_strSupportGeoClsName.Add(StrFromResID(IDS_SURFACE));
		m_strSupportGeoClsName.Add(StrFromResID(IDS_TEXT));
		m_nSupportFirstgeo = CLS_GEOPOINT;
	}
	
	CDialog::OnOK();
}

void CDlgSupportGeoClassName::SetData(const CStringArray &data)
{
	m_strSupportGeoClsName.RemoveAll();
	m_strSupportGeoClsName.Copy(data);

}

BOOL CDlgSupportGeoClassName::GetData(CStringArray &str)
{
	str.RemoveAll();
	str.Copy(m_strSupportGeoClsName);

	return TRUE;
	
}
