// DlgWorkSpaceBound.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgWorkSpaceBound.h"
#include "SmartViewFunctions.h"
#include "DlgNameCoord.h"
#include "SQLiteAccess.h"
#include "DlgDataSource.h"
#include "Functions_temp.h"
#include "UVSModify.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static float defExtendWid = 0.0f;
static BOOL defFlag[4] = {TRUE,TRUE,TRUE,TRUE};

/////////////////////////////////////////////////////////////////////////////
// CDlgWorkSpaceBound dialog


CDlgWorkSpaceBound::CDlgWorkSpaceBound(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgWorkSpaceBound::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgWorkSpaceBound)
	m_nScale = 1000;
	m_lfZmax = 0.0;
	m_lfZmin = 0.0;
	//}}AFX_DATA_INIT

	m_bTmValid = FALSE;
	m_bUVS = FALSE;
}


void CDlgWorkSpaceBound::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgWorkSpaceBound)
	DDX_Text(pDX, IDC_EDIT_SWSB_X1, m_lfX1);
	DDX_Text(pDX, IDC_EDIT_SWSB_X2, m_lfX2);
	DDX_Text(pDX, IDC_EDIT_SWSB_X3, m_lfX3);
	DDX_Text(pDX, IDC_EDIT_SWSB_X4, m_lfX4);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y1, m_lfY1);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y2, m_lfY2);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y3, m_lfY3);
	DDX_Text(pDX, IDC_EDIT_SWSB_Y4, m_lfY4);
	DDX_Text(pDX, IDC_EDIT_SWSB_SCALE, m_nScale);
	DDX_Text(pDX, IDC_EDIT_SWSB_ZMAX, m_lfZmax);
	DDX_Text(pDX, IDC_EDIT_SWSB_ZMIN, m_lfZmin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgWorkSpaceBound, CDialog)
	//{{AFX_MSG_MAP(CDlgWorkSpaceBound)
	ON_BN_CLICKED(IDC_FILE_WORKSPACE, OnInsertWorkspace)
	ON_BN_CLICKED(IDC_AUTOALIGN, OnAutoalign)
	ON_BN_CLICKED(IDC_EXTENDRANGE, OnExtendRange)
	ON_BN_CLICKED(IDC_CALCFROMNAME, OnCalcfromname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgWorkSpaceBound message handlers

BOOL CDlgWorkSpaceBound::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_bUVS)
	{
		GetDlgItem(IDC_EDIT_SWSB_SCALE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SWSB_SCALE1)->ShowWindow(SW_SHOW);
		CString scale = CUVSModify::GetWorkspaceScale();
		GetDlgItem(IDC_EDIT_SWSB_SCALE1)->SetWindowText(scale);
	}
	else
	{
		GetDlgItem(IDC_EDIT_SWSB_SCALE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_SWSB_SCALE1)->ShowWindow(SW_HIDE);
	}

	return TRUE;
}

void CDlgWorkSpaceBound::OnInsertWorkspace() 
{
	UpdateData(TRUE);

	CString filter;
	filter.LoadString(IDS_LOADMDB_FILTER);

	CFileDialog OpenFile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	if( OpenFile.DoModal()!=IDOK )return;
	CString strPathName = OpenFile.GetPathName();

	char database[256]={0};
	_CheckLoadPath((void*)(LPCTSTR)strPathName,database,sizeof(database));

	if(strPathName.Right(4).CompareNoCase(_T(".fdb")) == 0)
	{
		CSQLiteAccess  *pAccess = new CSQLiteAccess;
		if( pAccess )
		{
			pAccess->Attach(strPathName);

			CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
			if( !pDataSource )
			{
				delete pAccess;
				return;
			}
//			pDataSource->SetAccessObject(pAccess);

			pAccess->ReadDataSourceInfo(pDataSource);
			
			PT_3D pts[4];
			double zmin = -1000,zmax = 1000;
			pDataSource->GetBound(pts,&zmin,&zmax);

			m_lfX1 = pts[0].x;
			m_lfY1 = pts[0].y;
			m_lfX2 = pts[1].x;
			m_lfY2 = pts[1].y;
			m_lfX3 = pts[2].x;
			m_lfY3 = pts[2].y;
			m_lfX4 = pts[3].x;
			m_lfY4 = pts[3].y;
			
			m_lfZmin = zmin;
			m_lfZmax = zmax;
			m_nScale = pDataSource->GetScale();
			
			delete pDataSource;			
		}
	}
	UpdateData(FALSE);
}

void CDlgWorkSpaceBound::OnAutoalign() 
{
	//UpdateData(TRUE);

	m_nScale = GetEditData(IDC_EDIT_SWSB_SCALE);

	m_lfX1 = GetEditData(IDC_EDIT_SWSB_X1);
	m_lfX2 = GetEditData(IDC_EDIT_SWSB_X2);
	m_lfX3 = GetEditData(IDC_EDIT_SWSB_X3);
	m_lfX4 = GetEditData(IDC_EDIT_SWSB_X4);
	m_lfY1 = GetEditData(IDC_EDIT_SWSB_Y1);
	m_lfY2 = GetEditData(IDC_EDIT_SWSB_Y2);
	m_lfY3 = GetEditData(IDC_EDIT_SWSB_Y3);
	m_lfY4 = GetEditData(IDC_EDIT_SWSB_Y4);

	m_lfX2 = m_lfX3;
	m_lfY2 = m_lfY1;

	m_lfX4 = m_lfX1;
	m_lfY4 = m_lfY3;

	UpdateData(FALSE);
	
}

void CDlgWorkSpaceBound::OnOK()
{
	UpdateData(TRUE);

	PT_3D pts[4];
	memset(pts,0,sizeof(pts));
	pts[0].x = m_lfX1;
	pts[0].y = m_lfY1;
	pts[1].x = m_lfX2;
	pts[1].y = m_lfY2;
	pts[2].x = m_lfX3;
	pts[2].y = m_lfY3;
	pts[3].x = m_lfX4;
	pts[3].y = m_lfY4;

	if( GraphAPI::GIsClockwise(pts,4)==-1 )
	{
		AfxMessageBox(IDS_DOC_BOUNDINVALID);
		return;
	}

	CDialog::OnOK();
}

double CDlgWorkSpaceBound::GetEditData(UINT id)
{
	CWnd *pCtrl = GetDlgItem(id);
	if( !pCtrl )return 0;
	CString text;
	pCtrl->GetWindowText(text);
	return atof(text);
}


void CDlgWorkSpaceBound::OnExtendRange() 
{
	UpdateData(TRUE);
	CDlgExtendWid dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	defExtendWid = dlg.m_fWidth;//增扩范围
	
	PT_3D pts[4];
	pts[0].x = m_lfX1; pts[0].y = m_lfY1; pts[0].z = 0;
	pts[1].x = m_lfX2; pts[1].y = m_lfY2; pts[1].z = 0;
	pts[2].x = m_lfX3; pts[2].y = m_lfY3; pts[2].z = 0;
	pts[3].x = m_lfX4; pts[3].y = m_lfY4; pts[3].z = 0;
	
	BOOL bwid[4] = {dlg.m_bBottom,dlg.m_bRight,dlg.m_bTop,dlg.m_bLeft};//cjc 2012年11月8日 顺序弄反了
	memcpy(defFlag,bwid,sizeof(bwid));//拷贝到静态成员defFlag
	
	PT_3D tpts[2],tret[2];
	for( int i=0; i<4; i++)
	{
		if( !bwid[i] )continue;	
		
		tpts[0] = pts[i]; tpts[1] = pts[(i+1)%4];
		GraphAPI::GGetParallelLine(tpts,2,-dlg.m_fWidth,tret);
		pts[i] = tret[0]; pts[(i+1)%4] = tret[1];
	}
	
	m_lfX1 = floor(pts[0].x*100000+0.5)/100000; m_lfY1 = floor(pts[0].y*100000+0.5)/100000;  
	m_lfX2 = floor(pts[1].x*100000+0.5)/100000; m_lfY2 = floor(pts[1].y*100000+0.5)/100000;  
	m_lfX3 = floor(pts[2].x*100000+0.5)/100000; m_lfY3 = floor(pts[2].y*100000+0.5)/100000;  
	m_lfX4 = floor(pts[3].x*100000+0.5)/100000; m_lfY4 = floor(pts[3].y*100000+0.5)/100000;
	
	UpdateData(FALSE);
}


void CDlgWorkSpaceBound::OnCalcfromname() 
{
	CDlgNameCoord dlg;
	memset(dlg.m_map.szMapName,0,sizeof(dlg.m_map.szMapName));
	strncpy(dlg.m_map.szMapName,m_strName,sizeof(dlg.m_map.szMapName)-1);

	if( IDOK!=dlg.DoModal() )return;

	m_strName = dlg.m_map.szMapName;

	MAP map;
	dlg.GetMap( &map );	

	m_nScale = map.dwScale;
	//已做修改解决外扩变内扩的问题
	m_lfX1 = map.lfX[0];
	m_lfX2 = map.lfX[1];
	m_lfX3 = map.lfX[2];
	m_lfX4 = map.lfX[3];

	m_lfY1 = map.lfY[0];
	m_lfY2 = map.lfY[1];
	m_lfY3 = map.lfY[2];
	m_lfY4 = map.lfY[3];

	m_tm = dlg.m_tm;
	m_bTmValid = TRUE;

	UpdateData(FALSE);
}



/////////////////////////////////////////////////////////////////////////////
// CDlgExtendWid dialog


CDlgExtendWid::CDlgExtendWid(CWnd* pParent /*=NULL*/)
: CDialog(CDlgExtendWid::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExtendWid)
	
	m_bLeft = defFlag[3];
	m_bBottom = defFlag[0];
	m_bRight = defFlag[1];
	m_bTop = defFlag[2];
	m_fWidth = defExtendWid;
	//}}AFX_DATA_INIT
}


void CDlgExtendWid::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExtendWid)
	DDX_Check(pDX, IDC_CHECK_BOTTOM, m_bBottom);
	DDX_Check(pDX, IDC_CHECK_LEFT, m_bLeft);
	DDX_Check(pDX, IDC_CHECK_RIGHT, m_bRight);
	DDX_Check(pDX, IDC_CHECK_TOP, m_bTop);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_fWidth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExtendWid, CDialog)
//{{AFX_MSG_MAP(CDlgExtendWid)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExtendWid message handlers
