// DlgExportDxf.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgExportDxf.h"
#include "UIFFileDialogEx.h"
#include "SmartViewFunctions.h"
#include "DlgCheckList.h"
#include "editbasedoc.h"
#include "SymbolBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EXPORTDXF_SETTINGS  "IO32\\ExportDXF_Settings"
extern CString FormatFileNameByExt(const char *oldName, const char *ext);

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDxf dialog
static CString lstFile = _T("");
static CString dxfFile = _T("");
static CString fdbFile = _T("");
static CString colourFile = _T("");
static BOOL    bInit   = FALSE;
static CString dxfFile_dir = _T("");


CDlgExportDxf::CDlgExportDxf(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportDxf::IDD, pParent)
{
	if( !bInit )
	{
		bInit = TRUE;
		colourFile = StrFromResID(IDS_NOLISTFILE);
		lstFile = StrFromResID(IDS_NOLISTFILE);
		dxfFile = _T("");
		fdbFile = _T("");
		dxfFile_dir = _T("");
	}
	m_Type = 1;
	//{{AFX_DATA_INIT(CDlgExportDxf)
	m_strLstFile = lstFile;
	m_strColourFile = colourFile;
	m_strFdbFile = fdbFile;
	m_bUseIndex = FALSE;
	m_bUseLayerGroup = FALSE;
	m_bAddAnnot = FALSE;
	m_bMapFrame = FALSE;
	m_bLinearize = FALSE;
	m_bLines = TRUE;
	m_bLineSym = FALSE;
	m_bPoint = TRUE;
	m_bPointSym = FALSE;
	m_bSepPara = TRUE;
	m_bParaCenline = FALSE;
	m_bSurface = TRUE;
	m_bSurfaceSym = FALSE;
	m_bText = TRUE;
	m_b3DLines = FALSE;
	m_bDirPtAsLine = FALSE;
    m_bClosedCurve = FALSE;
	m_bTextSym = FALSE;
	m_bDifferBaselineSym = FALSE;
	m_nDxfFormat = 0;
	m_fBlockScale = 1.0f;
	m_bBlockScale = FALSE;
	m_bExportAsBlock = FALSE;
	m_bUseRGBTrueColor = TRUE;
	//}}AFX_DATA_INIT

	CWinApp *pApp = AfxGetApp();
	//m_strDxfFile = pApp->GetProfileString(EXPORTDXF_SETTINGS,"DXFFile",dxfFile);
	m_strLstFile = pApp->GetProfileString(EXPORTDXF_SETTINGS,"LstFile",lstFile);
	m_strColourFile = pApp->GetProfileString(EXPORTDXF_SETTINGS,"ColorFile",colourFile);
	if( m_strLstFile.GetLength()<=0 )m_strLstFile = lstFile;
	if( m_strColourFile.GetLength()<=0 )m_strColourFile = colourFile;
	m_bUseIndex = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"UseIndex",m_bUseIndex);
	m_bUseLayerGroup = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"UseLayerGroup",m_bUseLayerGroup);
	m_bMapFrame = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"MapFrame",m_bMapFrame);
	m_bText = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"Text",m_bText);
	m_bAddAnnot = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"Annot",m_bAddAnnot);
	m_bPoint = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"Point",m_bPoint);
	m_bPointSym = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"PointSym",m_bPointSym);
	m_bLines = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"Line",m_bLines);
	m_bLineSym = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"LineSym",m_bLineSym);
	m_b3DLines = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"3DLine",m_b3DLines);
	m_bSurface = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"Surface",m_bSurface);
	m_bSurfaceSym = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"SurfaceSym",m_bSurfaceSym);
	m_bLinearize = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"Linearize",m_bLinearize);
	m_bSepPara = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"SeparatePara",m_bSepPara);
	m_bParaCenline = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"ParaCenline",m_bParaCenline);
	m_bDirPtAsLine = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"DirPtAsLine",m_bDirPtAsLine);
	m_bClosedCurve = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"ClosedCurve",m_bClosedCurve);
	m_bTextSym = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"TextSym",m_bTextSym);
	m_bDifferBaselineSym = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"DifferBaselineSym",m_bDifferBaselineSym);
	m_nDxfFormat = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"DxfFormat",m_nDxfFormat);
	m_bBlockScale = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"bBlockScale",m_bBlockScale);
	m_fBlockScale = GetProfileDouble(EXPORTDXF_SETTINGS,"BlockScale",m_fBlockScale);
	m_bExportAsBlock = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"ExportAsBlock",m_bExportAsBlock);
	m_bUseRGBTrueColor = pApp->GetProfileInt(EXPORTDXF_SETTINGS,"UseRGBTrueColor",m_bUseRGBTrueColor);
}


void CDlgExportDxf::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportDxf)
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH, m_strDxfFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strLstFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH4, m_strColourFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH5, m_strFdbFile);
	DDX_Check(pDX, IDC_CHECK_USEINDEX, m_bUseIndex);
	DDX_Check(pDX, IDC_CHECK_LAYERGROUP, m_bUseLayerGroup);
	DDX_Check(pDX, IDC_CHECK_ADDANNOT, m_bAddAnnot);
	DDX_Check(pDX, IDC_CHECK_MAPFRAME, m_bMapFrame);
	DDX_Check(pDX, IDC_CHECK_LINEARIZE, m_bLinearize);
	DDX_Check(pDX, IDC_CHECK_LINES, m_bLines);
	DDX_Check(pDX, IDC_CHECK_LINESYM, m_bLineSym);
	DDX_Check(pDX, IDC_CHECK_POINTS, m_bPoint);
	DDX_Check(pDX, IDC_CHECK_POINTSYM, m_bPointSym);
	DDX_Check(pDX, IDC_CHECK_SEPARATEPARA, m_bSepPara);
	DDX_Check(pDX, IDC_CHECK_PARACENLINE, m_bParaCenline);
	DDX_Check(pDX, IDC_CHECK_SURFACES, m_bSurface);
	DDX_Check(pDX, IDC_CHECK_SURFACESYM, m_bSurfaceSym);
	DDX_Check(pDX, IDC_CHECK_TEXT, m_bText);
	DDX_Check(pDX, IDC_CHECK_3DLINE, m_b3DLines);
	DDX_Check(pDX, IDC_CHECK_DIRPTASLINE, m_bDirPtAsLine);
	DDX_Check(pDX, IDC_CHECK_CLOSED, m_bClosedCurve);
	DDX_Check(pDX, IDC_CHECK_TEXTSYM, m_bTextSym);
	DDX_Check(pDX, IDC_CHECK_DIFFERLAYER, m_bDifferBaselineSym);
	DDX_Check(pDX, IDC_CHECK_BLOCKSCALE, m_bBlockScale);
	DDX_Radio(pDX, IDC_RADIO_DXF, m_nDxfFormat);
	DDX_Control(pDX, IDC_BUTTON_ED_BROWSE5, m_addfdbBtn);
	DDX_Text(pDX, IDC_EDIT_SCALE, m_fBlockScale);
	DDX_Check(pDX, IDC_CHECK_EXPORTASBLOCK, m_bExportAsBlock);
	DDX_Check(pDX, IDC_CHECK_USERGB32, m_bUseRGBTrueColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportDxf, CDialog)
	//{{AFX_MSG_MAP(CDlgExportDxf)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE, OnBrowseDxf)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnBrowseLst)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE4, OnBrowseColour)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE5, OnBrowseFdb)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditLstFile)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditLstFile)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH4, OnSetfocusEditColorFile)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH4, OnKillfocusEdiColorFile)
	ON_BN_CLICKED(IDC_CHECK_POINTS, OnCheckPoints)
	ON_BN_CLICKED(IDC_CHECK_LINES, OnCheckLines)
	ON_BN_CLICKED(IDC_CHECK_SURFACES, OnCheckSurfaces)
	ON_BN_CLICKED(IDC_CHECK_USEINDEX, OnCheckUseIndex)
	ON_BN_CLICKED(IDC_CHECK_LAYERGROUP, OnCheckLayerGroup)
	ON_BN_CLICKED(IDC_RADIO_DXF, OnShiftDxfDwg)
	ON_BN_CLICKED(IDC_RADIO_DWG, OnShiftDxfDwg)
	ON_EN_CHANGE(IDC_EDIT_ED_FILEPATH, OnEditChangeDxfFile)
	ON_BN_CLICKED(ID_BTN_EXPORTMLINE, OnBtnExportMLine)
	ON_BN_CLICKED(ID_BTN_EXPORTMLINEANDSYMBOL, OnBtnExportMLineAndSymbol)
	ON_BN_CLICKED(ID_BTN_EXPORTSYMBOL, OnBtnExportSymbol)
	ON_BN_CLICKED(IDC_CHECK_BLOCKSCALE, OnBlockScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportDxf message handlers
//按照指定字符串分解CString添加到CStringArray中
//source 为待分解的字符串 divkey为分隔符 dest 为存储的字符串数组
extern BOOL Split(CString source, LPCTSTR divKey, CStringArray& dest);
extern BOOL BrowseFolder(LPCTSTR szTitle, CString& strPath, LPCTSTR szInitDir, HWND hOwner );


void CDlgExportDxf::OnBrowseDxf() 
{
	if (BATCHEXPORTDXF == m_Type)
	{
		CString strInitDir = "c:\\";

		if( !m_strDxfFile.IsEmpty() )
		{
			CString dir = m_strDxfFile;
			
			if( dir.Right(1)=="\\" )
				dir = dir.Left(dir.GetLength()-1);
			
			if( GetFileAttributes(dir)==FILE_ATTRIBUTE_DIRECTORY )				
			{
				strInitDir = m_strDxfFile;
			}
		}

		if( m_arrFdbStr.GetSize()>0 )
		{
			strInitDir = m_arrFdbStr[0];
			int pos = strInitDir.ReverseFind('\\');
			if( pos )strInitDir = strInitDir.Left(pos+1);
		}

		BrowseFolder(StrFromResID(IDS_SELECT_DIRECTORY),m_strDxfFile,strInitDir,GetSafeHwnd());

		UpdateData(FALSE);
	}
	else
	{
		CString filter(StrFromResID(IDS_LOADDXF_FILTER));
		CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, NULL, m_strDxfFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			filter);
		
		if( dlg.DoModal()!=IDOK )return;
		
		UpdateData(TRUE);
		m_strDxfFile = dlg.GetPathName();
		UpdateData(FALSE);	

		OnShiftDxfDwg();
	}
}

void CDlgExportDxf::OnBrowseFdb()
{
	CString filter(StrFromResID(IDS_LOADMDB_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, _T(".fdb"), m_strDxfFile, OFN_ENABLESIZING | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	dlg.m_ofn.nMaxFile = 4096;
	dlg.m_ofn.lpstrFile = new char[dlg.m_ofn.nMaxFile*MAX_PATH];
	ZeroMemory(dlg.m_ofn.lpstrFile,sizeof(char)*dlg.m_ofn.nMaxFile*MAX_PATH);
	CStringArray arrFiles;
	m_arrFdbStr.RemoveAll();
	if( dlg.DoModal()!=IDOK ) return;

	POSITION pos = dlg.GetStartPosition();//标示当前所选的第一个文件位置 
	while (pos !=NULL)
	{
		arrFiles.Add(dlg.GetNextPathName(pos));//添加多选择文件名到Cstring数组	
	}
	
	m_arrFdbStr.Copy(arrFiles);
	int nsize = m_arrFdbStr.GetSize();	

	if( nsize==1 )
	{
		m_strDxfFile = ExtractPath(dlg.m_ofn.lpstrFile);
		if( m_strDxfFile.GetLength()>3 )
			m_strDxfFile.Delete(m_strDxfFile.GetLength()-1);
	}
	else
		m_strDxfFile = dlg.m_ofn.lpstrFile;

	m_strFdbFile.Empty();
	for (int i=0;i<nsize;i++)
	{
		CString tmp2 = m_arrFdbStr.GetAt(i);		
				
		int pos2 = tmp2.ReverseFind(_T('\\'));
		if( pos2>=0 )tmp2.Delete(0,pos2+1);
		tmp2=_T("\"")+tmp2+_T("\"");
		m_strFdbFile+=tmp2+_T(";");
	}
	
	UpdateData(FALSE);
	
}

void CDlgExportDxf::OnBrowseLst() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLstFile = dlg.GetPathName();
	UpdateData(FALSE);		
}

void CDlgExportDxf::OnBrowseColour() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strColourFile = dlg.GetPathName();
	UpdateData(FALSE);		
}



CString FormatFileNameByExt2(const char *oldName)
{
	if( oldName==NULL || strlen(oldName)<=0 )
		return CString(_T(""));
		
	CString name = oldName;
	int pos = name.ReverseFind(_T('.'));
	if( pos<0 )
		name = name + _T(".dxf");
	else
	{
		CString old_ext = name.Mid(pos+1);
		old_ext.Remove(_T(' '));
		
		if( old_ext.GetLength()<=0 )
			name = name + _T("dxf");
		else if( old_ext.CompareNoCase("dxf")==0 || old_ext.CompareNoCase("dwg")==0 )
		{
			return name;
		}
		else
		{
			name = name + _T(".dxf");
		}
	}
	return name;
}

void CDlgExportDxf::OnOK() 
{
	UpdateData(TRUE);

	lstFile = m_strLstFile;

	if (m_Type == BATCHEXPORTDXF)
	{
		if(!PathFileExists(m_strDxfFile))
		{
			AfxMessageBox(StrFromResID(IDS_NOPATH_OR_NOFDBFILES));
			return;
		}		
		
		dxfFile_dir = m_strDxfFile;
		
		m_arrDxfStr.RemoveAll();
		for (int i = 0;i<m_arrFdbStr.GetSize();i++)
		{
			CString tmp =m_arrFdbStr.GetAt(i);
			
			int pos = tmp.ReverseFind(_T('\\'));
			if( pos>=0 )tmp.Delete(0,pos);
			pos = tmp.ReverseFind(_T('.'));
			if(pos>=0)tmp.Delete(pos,tmp.GetLength()-pos);

			if( m_nDxfFormat==0 )
				tmp += _T(".dxf");
			else
				tmp += _T(".dwg");

			tmp=m_strDxfFile+tmp;
			m_arrDxfStr.Add(tmp);//添加到dxf字符串组中
		}
	}
	else
	{
		dxfFile = FormatFileNameByExt2(m_strDxfFile);
		m_strDxfFile = dxfFile;
	}

	if (m_strDxfFile.IsEmpty()||( m_Type == BATCHEXPORTDXF && m_strFdbFile.IsEmpty()) )
	{
		AfxMessageBox(StrFromResID(IDS_NOPATH_OR_NOFDBFILES));
		return;
	}

	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strLstFile = _T("");

	if( m_strColourFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
		m_strColourFile = _T("");

	CWinApp *pApp = AfxGetApp();
	//pApp->WriteProfileString(EXPORTDXF_SETTINGS,"DXFFile",m_strDxfFile);
	//if( m_strLstFile.GetLength()>0 )
	pApp->WriteProfileString(EXPORTDXF_SETTINGS,"LstFile",m_strLstFile);
	pApp->WriteProfileString(EXPORTDXF_SETTINGS,"ColorFile",m_strColourFile);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"UseIndex",m_bUseIndex);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"UseLayerGroup",m_bUseLayerGroup);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"MapFrame",m_bMapFrame);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"Text",m_bText);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"Annot",m_bAddAnnot);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"Point",m_bPoint);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"PointSym",m_bPointSym);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"Line",m_bLines);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"LineSym",m_bLineSym);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"3DLine",m_b3DLines);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"Surface",m_bSurface);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"SurfaceSym",m_bSurfaceSym);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"Linearize",m_bLinearize);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"SeparatePara",m_bSepPara);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"ParaCenline",m_bParaCenline);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"DirPtAsLine",m_bDirPtAsLine);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"ClosedCurve",m_bClosedCurve);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"TextSym",m_bTextSym);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"DifferBaselineSym",m_bDifferBaselineSym);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"DxfFormat",m_nDxfFormat);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"bBlockScale",m_bBlockScale);
	WriteProfileDouble(EXPORTDXF_SETTINGS,"BlockScale",m_fBlockScale);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"ExportAsBlock",m_bExportAsBlock);
	pApp->WriteProfileInt(EXPORTDXF_SETTINGS,"UseRGBTrueColor",m_bUseRGBTrueColor);
	
	CDialog::OnOK();
}


void CDlgExportDxf::OnSetfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strLstFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportDxf::OnKillfocusEditLstFile() 
{
	UpdateData(TRUE);
	if( m_strLstFile.GetLength()<=0 )
	{
		m_strLstFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}
}


void CDlgExportDxf::OnSetfocusEditColorFile() 
{
	UpdateData(TRUE);
	if( m_strColourFile.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
	{
		m_strColourFile = _T("");
		UpdateData(FALSE);
	}
	
}

void CDlgExportDxf::OnKillfocusEdiColorFile() 
{
	UpdateData(TRUE);
	if( m_strColourFile.GetLength()<=0 )
	{
		m_strColourFile = StrFromResID(IDS_NOLISTFILE);
		UpdateData(FALSE);
	}
}


void CDlgExportDxf::OnCheckPoints() 
{
	UpdateData(TRUE);
/*
	CWnd *pWnd = GetDlgItem(IDC_CHECK_POINTSYM);
	if( pWnd )pWnd->EnableWindow(m_bPoint);*/
}

void CDlgExportDxf::OnCheckLines() 
{
	UpdateData(TRUE);

	CWnd *pWnd = GetDlgItem(IDC_CHECK_LINESYM);
/*	if( pWnd )pWnd->EnableWindow(m_bLines);	
*/
// 	pWnd = GetDlgItem(IDC_CHECK_3DLINE);
// 	if( pWnd )pWnd->EnableWindow(m_bLines);	
// 
// 	pWnd = GetDlgItem(IDC_CHECK_CLOSED);
// 	if( pWnd )pWnd->EnableWindow(m_bLines);	
// 
 	pWnd = GetDlgItem(IDC_CHECK_PARACENLINE);
 	if( pWnd )pWnd->EnableWindow(m_bLines);	

	pWnd = GetDlgItem(IDC_CHECK_SEPARATEPARA);
 	if( pWnd )pWnd->EnableWindow(m_bLines);	

// 
// 	pWnd = GetDlgItem(IDC_CHECK_LINEARIZE);
// 	if( pWnd )pWnd->EnableWindow(m_bLines);	
}

void CDlgExportDxf::OnCheckSurfaces() 
{
	UpdateData(TRUE);
/*	
	CWnd *pWnd = GetDlgItem(IDC_CHECK_SURFACESYM);
	if( pWnd )pWnd->EnableWindow(m_bSurface);*/
}

BOOL CDlgExportDxf::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( m_Type==EXPORTEXF )
	{
		if (m_strDxfFile.IsEmpty())
		{
			m_strDxfFile = m_strFdbFile.Left(m_strFdbFile.GetLength() - 4) + ".dxf";
		}
		CString ext = m_strDxfFile.Right(4);

		if( m_nDxfFormat==0 )
		{
			if( ext.CompareNoCase(".dwg")==0 )
				m_strDxfFile = m_strDxfFile.Left(m_strDxfFile.GetLength()-4) + ".dxf";
			else if( m_strDxfFile.GetLength()>0 && ext.CompareNoCase(".dxf")!=0 )
				m_strDxfFile += ".dxf";

			GetDlgItem(IDC_CHECK_USERGB32)->ShowWindow(SW_HIDE);
		}
		else
		{
			if( ext.CompareNoCase(".dxf")==0 )
				m_strDxfFile = m_strDxfFile.Left(m_strDxfFile.GetLength()-4) + ".dwg";
			else if( m_strDxfFile.GetLength()>0 && ext.CompareNoCase(".dwg")!=0 )
				m_strDxfFile += ".dwg";

			GetDlgItem(IDC_CHECK_USERGB32)->ShowWindow(SW_SHOW);
		}
	}
	
	UpdateData(FALSE);
	OnCheckPoints();
	OnCheckLines();
	OnCheckSurfaces();
	if (m_Type == EXPORTEXF)
	{
		m_addfdbBtn.EnableWindow(FALSE);
		
	}

	UpdateFormatButton();
	OnBlockScale();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportDxf::OnBtnExportMLine()
{
	UpdateData(TRUE);

	m_bAddAnnot = TRUE;
	m_bLinearize = TRUE;
	m_bLines = TRUE;
	m_bLineSym = FALSE;
	m_bPoint = TRUE;
	m_bPointSym = FALSE;
	m_bSepPara = TRUE;
	m_bSurface = TRUE;
	m_bSurfaceSym = FALSE;
	m_bText = TRUE;
	m_bTextSym = FALSE;
	m_bDifferBaselineSym = FALSE;

	UpdateData(FALSE);

	OnCheckLines();
}


void CDlgExportDxf::OnBtnExportMLineAndSymbol()
{
	UpdateData(TRUE);

	m_bAddAnnot = TRUE;
	m_bLinearize = TRUE;
	m_bLines = TRUE;
	m_bLineSym = TRUE;
	m_bPoint = TRUE;
	m_bPointSym = TRUE;
	m_bSepPara = TRUE;
	m_bSurface = TRUE;
	m_bSurfaceSym = TRUE;
	m_bText = TRUE;
	m_bTextSym = TRUE;
	m_bDifferBaselineSym = FALSE;

	UpdateData(FALSE);
	OnCheckLines();
}

void CDlgExportDxf::OnBtnExportSymbol()
{
	UpdateData(TRUE);

	m_bAddAnnot = TRUE;
	m_bLinearize = TRUE;
	m_bLines = FALSE;
	m_bLineSym = TRUE;
	m_bPoint = FALSE;
	m_bPointSym = TRUE;
	m_bSepPara = TRUE;
	m_bSurface = FALSE;
	m_bSurfaceSym = TRUE;
	m_bText = FALSE;
	m_bTextSym = TRUE;
	m_bDifferBaselineSym = FALSE;
	
	UpdateData(FALSE);
	OnCheckLines();
}


void CDlgExportDxf::OnCheckUseIndex()
{
	UpdateData(TRUE);
	if( m_bUseIndex && m_bUseLayerGroup )
		m_bUseLayerGroup = FALSE;
	UpdateData(FALSE);
}



void CDlgExportDxf::OnCheckLayerGroup()
{
	UpdateData(TRUE);
	if( m_bUseIndex && m_bUseLayerGroup )
		m_bUseIndex = FALSE;
	UpdateData(FALSE);	
}


void CDlgExportDxf::OnShiftDxfDwg()
{
	if( m_Type==BATCHEXPORTDXF )
		return;

	UpdateData(TRUE);

	CWnd *pWnd = GetDlgItem(IDC_RADIO_DXF);
	if( pWnd )
	{
		m_nDxfFormat = pWnd->SendMessage(BM_GETCHECK,0,0);
		if( m_nDxfFormat==BST_CHECKED )
			m_nDxfFormat = 0;
		else
			m_nDxfFormat = 1;
	}

	CString ext = m_strDxfFile.Right(4);
	if( m_nDxfFormat==0 )
	{
		if( ext.CompareNoCase(".dwg")==0 )
			m_strDxfFile = m_strDxfFile.Left(m_strDxfFile.GetLength()-4) + ".dxf";
		else if( m_strDxfFile.GetLength()>0 && ext.CompareNoCase(".dxf")!=0 )
			m_strDxfFile += ".dxf";

		GetDlgItem(IDC_CHECK_USERGB32)->ShowWindow(SW_HIDE);
	}
	else
	{
		if( ext.CompareNoCase(".dxf")==0 )
			m_strDxfFile = m_strDxfFile.Left(m_strDxfFile.GetLength()-4) + ".dwg";
		else if( m_strDxfFile.GetLength()>0 && ext.CompareNoCase(".dwg")!=0 )
			m_strDxfFile += ".dwg";

		GetDlgItem(IDC_CHECK_USERGB32)->ShowWindow(SW_SHOW);
	}

	UpdateData(FALSE);	
}


void CDlgExportDxf::UpdateFormatButton()
{	
	CString ext = m_strDxfFile.Right(4);
	if( ext.CompareNoCase(".dxf")==0 )
	{
		CWnd *pWnd = GetDlgItem(IDC_RADIO_DXF);
		if( pWnd )
		{
			pWnd->SendMessage(BM_SETCHECK,1,0);
		}
		pWnd = GetDlgItem(IDC_RADIO_DWG);
		if( pWnd )
		{
			pWnd->SendMessage(BM_SETCHECK,0,0);
		}
	}
	else if( ext.CompareNoCase(".dwg")==0 )
	{
		CWnd *pWnd = GetDlgItem(IDC_RADIO_DXF);
		if( pWnd )
		{
			pWnd->SendMessage(BM_SETCHECK,0,0);
		}
		pWnd = GetDlgItem(IDC_RADIO_DWG);
		if( pWnd )
		{
			pWnd->SendMessage(BM_SETCHECK,1,0);
		}
	}
}


void CDlgExportDxf::OnEditChangeDxfFile()
{
	UpdateData(TRUE);
	UpdateFormatButton();
}


void CDlgExportDxf::OnBlockScale()
{
	UpdateData(TRUE);

	CWnd *pWnd = GetDlgItem(IDC_EDIT_SCALE);
	if( pWnd )pWnd->EnableWindow(m_bBlockScale);
}