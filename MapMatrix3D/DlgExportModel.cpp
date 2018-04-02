// DlgExportModel.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgExportModel.h"
#include "UIFFileDialogEx.h"
#include "SmartViewFunctions.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportModel dialog


CDlgExportModel::CDlgExportModel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportModel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportModel)
	m_strMdlFile = _T("");
	m_strDEMFile = _T("");
	m_bTexture = FALSE;
	m_nLeftImg = 2;
	m_strPrjFile = _T("");
	m_bFixHide = TRUE;
	m_bExportSideFace = TRUE;
	m_nTextureSource = 0;
	//}}AFX_DATA_INIT
	m_nCurPrj = -1;
}


void CDlgExportModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportModel)
	DDX_Control(pDX, IDC_COMBO_STEREO, m_wndStereo);
	DDX_Control(pDX, IDC_LIST_FEATURECODE, m_lstCtrl);
	DDX_Text(pDX, IDC_EDIT_MODELFILE, m_strMdlFile);
	DDX_Text(pDX, IDC_EDIT_DEMFILE, m_strDEMFile);
	DDX_Check(pDX, IDC_CHECK_TEXTURE, m_bTexture);
	DDX_Radio(pDX , IDC_RADIO_LEFTIMG,m_nLeftImg);
	DDX_Text(pDX, IDC_EDIT_PRJFILE, m_strPrjFile);
	DDX_Text(pDX, IDC_EDIT_ORTHOFILE, m_strOrthoFile);
	DDX_Check(pDX, IDC_CHECK_FIXHIDE, m_bFixHide);
	DDX_Check(pDX, IDC_CHECK_SIDEFACE, m_bExportSideFace);
	DDX_Radio(pDX , IDC_RADIO_STEREO,m_nTextureSource);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportModel, CDialog)
	//{{AFX_MSG_MAP(CDlgExportModel)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEXML, OnBrowseXML)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEDEM, OnBrowseDEM)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEPRJ, OnBrowsePrj)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEORTHO, OnBrowseOrtho)
	ON_BN_CLICKED(IDC_CHECK_TEXTURE, OnCheckTexture)
	ON_BN_CLICKED(IDC_CHECK_FIXHIDE,OnCheckFixHide)
	ON_BN_CLICKED(IDC_CHECK_SIDEFACE,OnCheckExportSideFace)
	ON_CBN_SELCHANGE(IDC_COMBO_STEREO,OnStereoSelChanged)
	ON_BN_CLICKED(IDC_RADIO_STEREO, OnRadioStereo)
	ON_BN_CLICKED(IDC_RADIO_ORTHO, OnRadioStereo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportModel message handlers

BOOL CDlgExportModel::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	m_lstCtrl.SetExtendedStyle(m_lstCtrl.GetExtendedStyle()|LVS_EX_CHECKBOXES);
	
    m_lstCtrl.InsertColumn (0, StrFromResID(IDS_SELECT_FIELD),LVCFMT_CENTER);
	m_lstCtrl.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER);
	int nsize = m_strInitList.GetSize();
	for( int i=0; i<nsize; i++)
	{
		int idx = m_lstCtrl.InsertItem(i,m_strInitList[i]);
		m_lstCtrl.SetCheck(idx);
	}

	OnCheckTexture();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

static int ParsePaths(CString str, CStringArray &ret)
{
	int nIndex = 0;
	while (nIndex < str.GetLength())
	{
		int nfirst = nIndex;
		nIndex = str.Find(';', nfirst);
		if (nIndex < 0)
		{
			nIndex = str.GetLength();
		}
		
		CString strKeyValue = str.Mid(nfirst,nIndex-nfirst);
		ret.Add(strKeyValue);
	}
	
	return ret.GetSize();
}

void CDlgExportModel::OnOK() 
{
	CString str;
	int	iCount = m_lstCtrl.GetItemCount();
	for (int i=0; i<iCount; i++)
	{	
		if(m_lstCtrl.GetCheck(i))
		{
			str = m_lstCtrl.GetItemText(i,0);
			m_strSelList.Add(str);
		}
	}

	m_strStereo = _T("");
	int nsel = m_wndStereo.GetCurSel();
	if( nsel>0 )m_wndStereo.GetLBText(nsel,m_strStereo);

	//ParsePaths(m_strDEMFile,m_arrDEMFileNames);


	CDialog::OnOK();
}


void CDlgExportModel::OnBrowseXML() 
{
	CString filter(StrFromResID(IDS_LOADTXT_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, _T(".xml"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strMdlFile = dlg.GetPathName();
	UpdateData(FALSE);	

}


void CDlgExportModel::OnBrowseDEM() 
{
	CString filter(StrFromResID(IDS_LOADDEM_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	CString path = dlg.GetPathName();
	UpdateData(TRUE);
	if (!m_strDEMFile.IsEmpty())
	{
		m_strDEMFile += "; ";
	}
	m_strDEMFile += path;
	UpdateData(FALSE);

	m_arrDEMFileNames.Add(path);
	
}

void CDlgExportModel::OnBrowsePrj() 
{
	CString filter(StrFromResID(IDS_LOADXML_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;


	CString path = dlg.GetPathName();

	UpdateData(TRUE);
	if (!m_strPrjFile.IsEmpty())
	{
		m_strPrjFile += "; ";
	}
	
	m_strPrjFile += path;
	UpdateData(FALSE);


	CGeoBuilderPrj *prj = new CGeoBuilderPrj;
	if (prj == NULL) return;
	if (prj->LoadProject(path))
	{
		m_arrPrjs.Add(prj);
	}

// 	if (m_arrPrjs.GetSize() < 1) return;
// 	
// 	m_wndStereo.ResetContent();

	int nCount = m_wndStereo.GetCount();
	int nItem = -1;
	if (nCount == 0)
	{
		nItem = m_wndStereo.AddString(StrFromResID(IDS_AUTOSELECT));
	}

	int nCurPrj = m_arrPrjs.GetSize()-1;
	
	CoreObject core = prj->GetCoreObject();
	for( int i=0; i<core.iStereoNum; i++)
	{
		m_wndStereo.AddString(core.stereo[i].sp.stereoID);
		m_wndStereo.SetItemData(nCount+i,nCurPrj);
	}


	if (m_wndStereo.GetCount() > 0)
	{
		m_wndStereo.SetCurSel(0);
	}
	
	UpdateData(FALSE);	
}



void CDlgExportModel::OnBrowseOrtho() 
{
	CString filter = (StrFromResID(IDS_LOADTIF_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	CString path = dlg.GetPathName();
	UpdateData(TRUE);
	if (!m_strOrthoFile.IsEmpty())
	{
		m_strOrthoFile += "; ";
	}
	m_strOrthoFile += path;
	UpdateData(FALSE);
}


void CDlgExportModel::OnCheckTexture() 
{
	UpdateData(TRUE);	

	CWnd *pWnd = GetDlgItem(IDC_RADIO_STEREO);
	if( pWnd )pWnd->EnableWindow(m_bTexture);

	pWnd = GetDlgItem(IDC_RADIO_ORTHO);
	if( pWnd )pWnd->EnableWindow(m_bTexture);

	if( !m_bTexture )
	{
		EnableTextureCtrls(FALSE,FALSE);
	}
	else
	{
		EnableTextureCtrls(m_nTextureSource==0,m_nTextureSource==1);
	}
}


void CDlgExportModel::EnableTextureCtrls(BOOL bEnableStereo, BOOL bEnableOrtho)
{
	CWnd *p[10]={NULL};
	p[0] = GetDlgItem(IDC_RADIO_LEFTIMG), p[1] = GetDlgItem(IDC_RADIO_RIGHTIMG);
	p[2] = GetDlgItem(IDC_EDIT_PRJFILE), p[3] = GetDlgItem(IDC_COMBO_STEREO);
	p[4] = GetDlgItem(IDC_RADIO_AUTOIMG), p[5] = GetDlgItem(IDC_BUTTON_BROWSEPRJ);
	p[6] = GetDlgItem(IDC_CHECK_SIDEFACE), p[7] = GetDlgItem(IDC_CHECK_FIXHIDE); 
	
	for( int i=0; i<sizeof(p)/sizeof(*p); i++)
	{
		if( p[i] )p[i]->EnableWindow(bEnableStereo);

		p[i] = NULL;
	}

	p[0] = GetDlgItem(IDC_EDIT_ORTHOFILE), p[1] = GetDlgItem(IDC_BUTTON_BROWSEORTHO);
	for( i=0; i<sizeof(p)/sizeof(*p); i++)
	{
		if( p[i] )p[i]->EnableWindow(bEnableOrtho);
		
		p[i] = NULL;
	}
}

void CDlgExportModel::OnCheckFixHide()
{
	UpdateData(TRUE);

	if (m_bFixHide)
	{
		m_bExportSideFace = TRUE;
		UpdateData(FALSE);
	}
}

void CDlgExportModel::OnCheckExportSideFace()
{
	UpdateData(TRUE);
	
	if (!m_bExportSideFace)
	{
		m_bFixHide = FALSE;
		UpdateData(FALSE);
	}
}

void CDlgExportModel::OnStereoSelChanged()
{
	int curSel = m_wndStereo.GetCurSel();
	m_nCurPrj = m_wndStereo.GetItemData(curSel);
}

void CDlgExportModel::OnRadioStereo()
{
	UpdateData(TRUE);
	EnableTextureCtrls(m_nTextureSource==0,m_nTextureSource==1);
}