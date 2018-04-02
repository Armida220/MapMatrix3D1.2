// DlgOverlayOrtho.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgOverlayOrtho.h"
#include "SmartViewFunctions.h"
#include "ImageRead.h "
#include "GeoBuilderPrj2.h"
#include "ExMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgList dialog


CDlgList::CDlgList(CWnd* pParent /*=NULL*/)
: CDialog(CDlgList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgList)
	//}}AFX_DATA_INIT
}


void CDlgList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgList)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgList, CDialog)
//{{AFX_MSG_MAP(CDlgList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgList message handlers

void CDlgList::OnOK() 
{
	int nsel = m_wndList.GetCurSel();
	if( nsel!=LB_ERR )m_wndList.GetText(nsel,m_strSelItem);
	CDialog::OnOK();
}


BOOL CDlgList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int num = m_strList.GetSize();
	int maxLength = 0;
	for( int i=0; i<num; i++)
	{
		m_wndList.AddString(m_strList[i]);
		if( maxLength<m_strList[i].GetLength() )
			maxLength = m_strList[i].GetLength();
	}

	CDC *pDC = m_wndList.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		m_wndList.ReleaseDC(pDC);

		maxLength = maxLength*tm.tmAveCharWidth;
		CRect rect;
		m_wndList.GetClientRect(&rect);
		if( maxLength>rect.Width() )m_wndList.SetHorizontalExtent(maxLength);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayOrtho dialog


CDlgOverlayOrtho::CDlgOverlayOrtho(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOverlayOrtho::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOverlayOrtho)
	m_lfMatrix1 = 1.0;
	m_lfMatrix2 = 0.0;
	m_lfMatrix3 = 0.0;
	m_lfMatrix4 = 1.0;
	m_lfXOff = 0.0;
	m_lfYOff = 0.0;
	m_strFileName = _T("");
	m_bVisible = TRUE;
	m_nPixelBase = 0;
	//}}AFX_DATA_INIT

	memset(m_ptsBound,0,sizeof(m_ptsBound));

	m_lfPixelSizeX = m_lfPixelSizeY = 0;
}


void CDlgOverlayOrtho::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOverlayOrtho)
	DDX_Text(pDX, IDC_EDIT_MATRIX1, m_lfMatrix1);
	DDX_Text(pDX, IDC_EDIT_MATRIX2, m_lfMatrix2);
	DDX_Text(pDX, IDC_EDIT_MATRIX3, m_lfMatrix3);
	DDX_Text(pDX, IDC_EDIT_MATRIX4, m_lfMatrix4);
	DDX_Text(pDX, IDC_EDIT_XOFF, m_lfXOff);
	DDX_Text(pDX, IDC_EDIT_YOFF, m_lfYOff);
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strFileName);
	DDX_Check(pDX, IDC_CHECK_VISIBLE, m_bVisible);
	DDX_Radio(pDX, IDC_RADIO_PIXELLBCORNER, m_nPixelBase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOverlayOrtho, CDialog)
	//{{AFX_MSG_MAP(CDlgOverlayOrtho)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnBrowseAny)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_ORTHO, OnBrowseLocal)
	ON_BN_CLICKED(IDC_BUTTON_UNLOAD, OnButtonUnload)
	ON_BN_CLICKED(IDC_BUTTON_MAPBOUND, OnButtonMapbound)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverlayOrtho message handlers

void CDlgOverlayOrtho::OnBrowseAny() 
{
	CString filter(StrFromResID(IDS_LOADTIF_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	
	m_strFileName = dlg.GetPathName();
	CGeoBuilderPrj prj;
	GeoHeader head;
	if( prj.IsGeoImage(m_strFileName,head) )
	{
		m_lfXOff = head.lfStartX; m_lfYOff = head.lfStartY;
		m_lfMatrix1 = head.lfDx*head.lfScale*0.001*cos(head.lfKapa); 
		m_lfMatrix2 = head.lfDy*head.lfScale*0.001*sin(head.lfKapa); 
		m_lfMatrix3 =-head.lfDx*head.lfScale*0.001*sin(head.lfKapa); 
		m_lfMatrix4 = head.lfDy*head.lfScale*0.001*cos(head.lfKapa);
		
		m_lfPixelSizeX = head.lfDx*head.lfScale*0.001;
		m_lfPixelSizeY = head.lfDy*head.lfScale*0.001;

		CString name = m_strFileName;
		name = name.Left(name.GetLength()-3) + "tfw";
		FILE *fp = fopen(name,"r");
		if( fp )
		{
			// CGeoBuilderPrj 并不支持带有旋转角的参数，这里对此情况特别处理；
			// 将基于左上角像素的参数转为基于左下角像素的参数
			double v[6];
			if( fscanf(fp,"%lf%lf%lf%lf%lf%lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5])==6 )
			{
				if( ((float)v[1])!=0.0f && ((float)v[2])!=0.0f )
				{
					m_lfMatrix1 = v[0]; 
					m_lfMatrix2 = v[1]; 
					m_lfMatrix3 = -v[2]; 
					m_lfMatrix4 = -v[3]; 
					
					m_lfXOff = v[4] + v[1]*head.iRow;
					m_lfYOff = v[5] + v[3]*head.iRow;

					m_lfPixelSizeX = sqrt(v[0]*v[0] + v[1]*v[1]);
					m_lfPixelSizeY = sqrt(v[2]*v[2] + v[3]*v[3]);
				}
			}
			fclose(fp);
		}
	}
	else
	{
		AfxMessageBox(IDS_ERR_READIMGGEOINFO);
	}
	
	m_bVisible = TRUE;
	
	UpdateData(FALSE);	
}

void CDlgOverlayOrtho::OnBrowseLocal() 
{
 	CGeoBuilderPrj *prj = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETPROJECT,0,(LPARAM)&prj);
	if( !prj )return;

	CDlgList dlg;

	CoreObject core = prj->GetCoreObject();
	for( int i=0; i<core.tmp.doms.iDomNum; i++)
	{
		dlg.m_strList.Add(core.tmp.doms.dom[i].strDomFile);
	}

	if( dlg.DoModal()!=IDOK )return;
	for( i=0; i<core.tmp.doms.iDomNum; i++)
	{
		if( core.tmp.doms.dom[i].strDomFile==dlg.m_strSelItem )
			break;
	}
	if( i>=core.tmp.doms.iDomNum )return;

	UpdateData(TRUE);

	m_strFileName = dlg.m_strSelItem;

	GeoHeader head;
	if( prj->IsGeoImage(m_strFileName,head) )
	{
		m_lfXOff = head.lfStartX; m_lfYOff = head.lfStartY;
		m_lfMatrix1 = head.lfDx*head.lfScale*0.001*cos(head.lfKapa); 
		m_lfMatrix2 = head.lfDy*head.lfScale*0.001*sin(head.lfKapa); 
		m_lfMatrix3 =-head.lfDx*head.lfScale*0.001*sin(head.lfKapa); 
		m_lfMatrix4 = head.lfDy*head.lfScale*0.001*cos(head.lfKapa); 

		m_lfPixelSizeX = head.lfDx*head.lfScale*0.001;
		m_lfPixelSizeY = head.lfDy*head.lfScale*0.001;
	}
	else
	{
		AfxMessageBox(IDS_ERR_READIMGGEOINFO);
	}

	m_bVisible = TRUE;

	UpdateData(FALSE);
}

BOOL CDlgOverlayOrtho::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CGeoBuilderPrj *prj = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETPROJECT,0,(LPARAM)&prj);
	if( !prj || prj->strCurXML.GetLength()<=0 )
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_BROWSE_ORTHO);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgOverlayOrtho::OnButtonUnload() 
{
	m_lfMatrix1 = 1.0;
	m_lfMatrix2 = 0.0;
	m_lfMatrix3 = 0.0;
	m_lfMatrix4 = 1.0;
	m_lfXOff = 0.0;
	m_lfYOff = 0.0;
	m_strFileName = _T("");
	m_bVisible = FALSE;	

	UpdateData(FALSE);
}

void CDlgOverlayOrtho::OnButtonMapbound() 
{
	UpdateData(TRUE);
	
	CSize szImage;
	CImageRead image;
	if( image.Load(m_strFileName) )
	{
		szImage = image.GetImageSize();
	}
	else
	{
		szImage.cx = szImage.cy = 0;
	}
	
	Envelope evlp;
	evlp.CreateFromPts(m_ptsBound,4);
	
	m_lfXOff = evlp.m_xl; m_lfYOff = evlp.m_yl;
	
	if( evlp.IsEmpty() || szImage.cx==0 || szImage.cy==0 )
	{
		m_lfMatrix1 = 1.0; m_lfMatrix4 = 1.0;
		m_lfMatrix2 = 0.0; m_lfMatrix3 = 0.0;
	}
	else
	{
		m_lfMatrix1 = (evlp.m_xh-evlp.m_xl)/szImage.cx;
		m_lfMatrix4 = (evlp.m_yh-evlp.m_yl)/szImage.cy;
		m_lfMatrix2 = 0;
		m_lfMatrix3 = 0;
	}
	UpdateData(FALSE);
}


void CDlgOverlayOrtho::OnOK() 
{
	UpdateData(TRUE);
	CDialog::OnOK();
}
