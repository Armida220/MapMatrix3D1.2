// DlgExportArcgisMdb.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgExportArcgisMdb.h"
#include "editbasedoc.h "
#include "DlgDataSource.h "
#include "SymbolLib.h "
#include <set>
#include "Functions_temp.h"
#include "SQLiteAccess.h"
#include "GeoText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define EXPORTMDB_SETTINGS  "IO32\\ExportMDB_Settings"
extern BOOL BrowseFolder(LPCTSTR szTitle, CString& strPath, LPCTSTR szInitDir, HWND hOwner );
extern void SetLanguage(HMODULE hModule);

void GetTmpFilePath(char *path, char* filename)
{
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos;
	if ((pos = strrchr(path, '\\')))*pos = '\0';
	if ((pos = strrchr(path, '\\')))*pos = '\0';
	strcat(pos, "\\History\\");
	strcat(pos, filename);
}

void Linearize_forExportArcgisMdb(CGeometry *pObj)
{
	CGeometry *pObj2 = pObj->Linearize();
	CArray<PT_3DEX, PT_3DEX> arrPts;
	pObj2->GetShape(arrPts);
	pObj->CreateShape(arrPts.GetData(), arrPts.GetSize());
	delete pObj2;
}

inline int FieldTypeFrom(int esriType)
{
	if (esriType == mEsriFieldTypeSmallInteger)
	{
		return VT_I2;
	}
	else if (esriType == mEsriFieldTypeInteger)
	{
		return VT_I4;
	}
	else if (esriType == mEsriFieldTypeSingle)
	{
		return VT_R4;
	}
	else if (esriType == mEsriFieldTypeDouble)
	{
		return VT_R8;
	}
	else if (esriType == mEsriFieldTypeString)
	{
		return VT_BSTR;
	}
	else
		return VT_NULL;
}
inline int esriFieldTypeFrom(int Type)
{
	if (Type == VT_I2)
	{
		return mEsriFieldTypeSmallInteger;
	}
	else if (Type == VT_I4)
	{
		return mEsriFieldTypeInteger;
	}
	else if (Type == VT_R4)
	{
		return mEsriFieldTypeSingle;
	}
	else if (Type == VT_R8)
	{
		return mEsriFieldTypeDouble;
	}
	else if (Type == VT_BSTR)
	{
		return mEsriFieldTypeString;
	}
	else
		return VT_NULL;
}
inline int XdefineTypeToEsri(int valuetype)
{
	switch (valuetype)
	{
	case DP_CFT_BOOL:
	case DP_CFT_SMALLINT:
		return mEsriFieldTypeSmallInteger;
	case DP_CFT_COLOR:
	case DP_CFT_INTEGER:
		return mEsriFieldTypeInteger;
	case DP_CFT_VARCHAR:
	case DP_CFT_DATE:
		return mEsriFieldTypeString;
	case DP_CFT_DOUBLE:
		return mEsriFieldTypeDouble;
	case DP_CFT_FLOAT:
		return mEsriFieldTypeSingle;
	default:
		break;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgExportArcgisMdb dialog

CDlgExportArcgisMdb::CDlgExportArcgisMdb(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExportArcgisMdb::IDD, pParent)
{
	CWinApp *pApp = AfxGetApp();
	m_strCodeList = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strCodeList", _T(""));
	m_strTemplateFile = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strTemplateFile", _T(""));
	m_strPrjFile = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strPrjFile", _T(""));
	m_strvPrjFile = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strvPrjFile", _T(""));

	m_bBreakParallel = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bBreakParallel", FALSE);
	m_bGDB = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bGDB", FALSE);
	m_bReserveSurface = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bReserveSurface", TRUE);
	m_bUseIndex = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bUseIndex", FALSE);
	m_bCollect = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bCollect", TRUE);
	m_bMark = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bMark", TRUE);
	m_bEDB = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bEDB", TRUE);
	m_bXAttrOnly = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bXAttrOnly", TRUE);
	m_bExportPointZ = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportPointZ", FALSE);
	m_bExportCurveZ = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportCurveZ", FALSE);
	m_bExportSurfaceZ = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportSurfaceZ", FALSE);
	m_bClockWiseAngle = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bClockWiseAngle", TRUE);
	m_bExportEmptyLayer = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportEmptyLayer", TRUE);

	if (m_strCodeList.IsEmpty()) m_strCodeList = StrFromResID(IDS_NOLISTFILE);
	if (m_strTemplateFile.IsEmpty()) m_strTemplateFile = StrFromResID(IDS_USEDEFAULTTEMP);
	if (m_strPrjFile.IsEmpty()) m_strPrjFile = StrFromResID(IDS_NOPRJFILE);
	if (m_strvPrjFile.IsEmpty()) m_strvPrjFile = StrFromResID(IDS_NOPRJFILE);

	CString strPath = GetActiveDlgDoc()->GetPathName();
	strPath = strPath.Left(strPath.GetLength() - 4);
	if (m_bGDB)
		strPath += _T(".gdb");
	else
		strPath += _T(".mdb");
	m_strFilePath = strPath;
}


void CDlgExportArcgisMdb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportArcgisMdb)
	DDX_Text(pDX, IDC_EDIT_ID_FILEPATH, m_strFilePath);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strCodeList);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH3, m_strTemplateFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH4, m_strPrjFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH5, m_strvPrjFile);
	DDX_Check(pDX, IDC_CHECK_PARALLEL, m_bBreakParallel);
	DDX_Check(pDX, IDC_EXPORT_GEB, m_bGDB);
	DDX_Check(pDX, IDC_CHECK_USEINDEX, m_bUseIndex);
	DDX_Check(pDX, IDC_CHECK_COLLECT, m_bCollect);
	DDX_Check(pDX, IDC_CHECK_MARK, m_bMark);
	DDX_Check(pDX, IDC_CHECK_EDB, m_bEDB);
	DDX_Check(pDX, IDC_CHECK_XATTR_ONLY, m_bXAttrOnly);
	DDX_Check(pDX, IDC_CHECK_RESERVE_SURFACE, m_bReserveSurface);
	DDX_Check(pDX, IDC_CHECK_EXPORT_POINTZ, m_bExportPointZ);
	DDX_Check(pDX, IDC_CHECK_EXPORT_CURVEZ, m_bExportCurveZ);
	DDX_Check(pDX, IDC_CHECK_EXPORT_SURFACEZ, m_bExportSurfaceZ);
	DDX_Check(pDX, IDC_CHECK_CLOCKWISE_ANGLE, m_bClockWiseAngle);
	DDX_Check(pDX, IDC_CHECK3, m_bExportEmptyLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportArcgisMdb, CDialog)
	//{{AFX_MSG_MAP(CDlgExportArcgisMdb)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE, OnButtonIdBrowse)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnButtonEdBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE4, OnButtonBrowse4)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE5, OnButtonBrowse5)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditEdFilepath2)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditEdFilepath2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH3, OnSetfocusEditEdFilepath3)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH3, OnKillfocusEditEdFilepath3)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH4, OnSetfocusEditEdFilepath4)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH4, OnKillfocusEditEdFilepath4)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH5, OnSetfocusEditEdFilepath5)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH5, OnKillfocusEditEdFilepath5)
	ON_BN_CLICKED(IDC_EXPORT_GEB, OnCheckGDB)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportArcgisMdb message handlers

void CDlgExportArcgisMdb::OnButtonIdBrowse() 
{
	CString filter(StrFromResID(IDS_LOADARCGISMDB_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, _T(".mdt"), m_strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	m_strFileName = dlg.GetFileName();
	int n = m_strFilePath.Find(m_strFileName);
	m_strFileDir = m_strFilePath.Left(n);

	UpdateData(FALSE);
}

void CDlgExportArcgisMdb::OnButtonEdBrowse2() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strCodeList = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgExportArcgisMdb::OnOK()
{
	UpdateData(TRUE);
	
	CDialog::OnOK();
	
	CString strPath = m_strFilePath;
	int nSize = m_strFilePath.GetLength();
	int nIndex = m_strFilePath.ReverseFind('\\');
	if (nSize<=0||nIndex<=0)
	{
		m_strFileDir.Empty();
		m_strFileName.Empty();
		return;
	}
	else
	{
		m_strFileDir = m_strFilePath.Left(nIndex);
		m_strFileName = m_strFilePath.Right(nSize-nIndex);	
	}

	if (m_strCodeList.Compare(StrFromResID(IDS_NOLISTFILE)) == 0)
		m_strCodeList = _T("");
	if (m_strTemplateFile.Compare(StrFromResID(IDS_USEDEFAULTTEMP)) == 0)
		m_strCodeList = _T("");
	if (m_strPrjFile.Compare(StrFromResID(IDS_NOPRJFILE)) == 0)
		m_strPrjFile = _T("");
	if (m_strvPrjFile.Compare(StrFromResID(IDS_NOPRJFILE)) == 0)
		m_strvPrjFile = _T("");

	CWinApp *pApp = AfxGetApp();
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strCodeList", m_strCodeList);
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strTemplateFile", m_strTemplateFile);
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strPrjFile", m_strPrjFile);
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strvPrjFile", m_strvPrjFile);

	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bBreakParallel", m_bBreakParallel);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bGDB", m_bGDB);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bReserveSurface", m_bReserveSurface);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bUseIndex", m_bUseIndex);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bCollect", m_bCollect);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bMark", m_bMark);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bEDB", m_bEDB);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bXAttrOnly", m_bXAttrOnly);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportPointZ", m_bExportPointZ);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportCurveZ", m_bExportCurveZ);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportSurfaceZ", m_bExportSurfaceZ);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bClockWiseAngle", m_bClockWiseAngle);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportEmptyLayer", m_bExportEmptyLayer);

	BeginCheck41License

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (pDoc)
	{
		CString fdbPath = pDoc->GetPathName();
		fdbPath = fdbPath.Left(fdbPath.GetLength() - 4);
		fdbPath += "_mdb.fdb";
		pDoc->SaveAs(fdbPath, TRUE);

		AfxGetApp()->WriteProfileInt("IO32\\ExportMDB_Settings", "bNonArcgis", pDoc->m_bHideFtrsNotGIS);

		GOutPut(StrFromResID(IDS_PROCESSING));
		char path[_MAX_PATH] = { 0 };
		GetModuleFileName(NULL, path, _MAX_FNAME);
		char *pos1;
		if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
		if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
		strcat(path, "\\bin\\VectorConvert.exe");

		CString cmd;
		cmd.Format("\"%s\" exportMDB \"%s\" \"%s\"", path, fdbPath, m_strFilePath);

		STARTUPINFO stStartUpInfo;
		memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
		stStartUpInfo.cb = sizeof(STARTUPINFO);
		//stStartUpInfo.wShowWindow = SW_HIDE;

		PROCESS_INFORMATION stProcessInfo;
		memset(&stProcessInfo, 0, sizeof(stProcessInfo));
		if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
			return;
		while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
		{
		}
		GOutPut(StrFromResID(IDS_PROCESS_END));

		::DeleteFile(fdbPath);
	}

	EndCheck41License
}

void CDlgExportArcgisMdb::OnSetfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
 	if( m_strCodeList.Compare(StrFromResID(IDS_NOLISTFILE))==0 )
 	{
 		m_strCodeList = _T("");
 		UpdateData(FALSE);
 	}
}

void CDlgExportArcgisMdb::OnKillfocusEditEdFilepath2() 
{
	UpdateData(TRUE);
 	if( m_strCodeList.GetLength()<=0 )
 	{
 		m_strCodeList = StrFromResID(IDS_NOLISTFILE);
 		UpdateData(FALSE);
 	}	
}


void CDlgExportArcgisMdb::OnButtonBrowse3() 
{
	CString filter(StrFromResID(ID_STRING_MDTFILITER ));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	if( dlg.DoModal()!=IDOK )return;
	UpdateData(TRUE);
	m_strTemplateFile= dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgExportArcgisMdb::OnButtonBrowse4() 
{
	CString filter(StrFromResID(ID_STRING_PRJFILITER ));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	if( dlg.DoModal()!=IDOK )return;
	UpdateData(TRUE);
	m_strPrjFile = dlg.GetPathName();
	UpdateData(FALSE);	
}

void CDlgExportArcgisMdb::OnButtonBrowse5() 
{
	CString filter(StrFromResID(ID_STRING_PRJFILITER ));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	if( dlg.DoModal()!=IDOK )return;
	UpdateData(TRUE);
	m_strvPrjFile = dlg.GetPathName();
	UpdateData(FALSE);	
}


void CDlgExportArcgisMdb::OnSetfocusEditEdFilepath3() 
{
	UpdateData(TRUE);
 	if( m_strTemplateFile.Compare(StrFromResID(IDS_USEDEFAULTTEMP))==0 )
 	{
 		m_strTemplateFile = _T("");
 		UpdateData(FALSE);
 	}
}
void CDlgExportArcgisMdb::OnKillfocusEditEdFilepath3() 
{
	UpdateData(TRUE);
 	if( m_strTemplateFile.GetLength()<=0 )
 	{
 		m_strTemplateFile = StrFromResID(IDS_USEDEFAULTTEMP);
 		UpdateData(FALSE);
 	}	
}


void CDlgExportArcgisMdb::OnSetfocusEditEdFilepath4() 
{
	UpdateData(TRUE);
 	if( m_strPrjFile.Compare(StrFromResID(IDS_NOPRJFILE))==0 )
 	{
 		m_strPrjFile = _T("");
 		UpdateData(FALSE);
 	}
}

void CDlgExportArcgisMdb::OnKillfocusEditEdFilepath4() 
{
	UpdateData(TRUE);
 	if( m_strPrjFile.GetLength()<=0 )
 	{
 		m_strPrjFile = StrFromResID(IDS_NOPRJFILE);
 		UpdateData(FALSE);
 	}	
}

void CDlgExportArcgisMdb::OnSetfocusEditEdFilepath5() 
{
	UpdateData(TRUE);
	if( m_strvPrjFile.Compare(StrFromResID(IDS_NOPRJFILE))==0 )
	{
		m_strvPrjFile = _T("");
		UpdateData(FALSE);
	}
}

void CDlgExportArcgisMdb::OnKillfocusEditEdFilepath5() 
{
	UpdateData(TRUE);
	if( m_strvPrjFile.GetLength()<=0 )
	{
		m_strvPrjFile = StrFromResID(IDS_NOPRJFILE);
		UpdateData(FALSE);
	}	
}

void CDlgExportArcgisMdb::OnCheckGDB()
{
	UpdateData(TRUE);
	if (!m_bGDB)
	{
		int nsize = m_strFilePath.GetLength();
		int nindex = m_strFilePath.ReverseFind('.');
		if (nindex>=nsize||nindex<0)
		{
			UpdateData(FALSE);
			return;
		}
		m_strFilePath.Delete(nindex+1,nsize-nindex-1);
		m_strFilePath.Insert(nindex+1,_T("mdb"));	
	}
	else
	{
		int nsize = m_strFilePath.GetLength();
		int nindex = m_strFilePath.ReverseFind('.');
		if (nindex>=nsize||nindex<0)
		{
			UpdateData(FALSE);
			return;
		}
		m_strFilePath.Delete(nindex+1,nsize-nindex-1);
		m_strFilePath.Insert(nindex+1,_T("gdb"));
	}
	UpdateData(FALSE);
}

void PrintfVar(FILE* fp, const char *name, VARIANT& var)
{
	_bstr_t tmpstr;
	char* str1 = NULL;
	switch (var.vt)
	{
	case VT_UI1:
		fprintf(fp, "%s %i\n", name, var.bVal);
		break;
	case VT_UI2:
		fprintf(fp, "%s %i\n", name, var.uiVal);
		break;
	case VT_UI4:
		fprintf(fp, "%s %i\n", name, var.ulVal);
		break;
	case VT_UINT:
		fprintf(fp, "%s %i\n", name, var.uintVal);
		break;
	case VT_I1:
		fprintf(fp, "%s %c\n", name, var.cVal);
		break;
	case VT_I2:
		fprintf(fp, "%s %i\n", name, var.iVal);
		break;
	case VT_I4:
		fprintf(fp, "%s %d\n", name, var.lVal);
		break;
	case VT_INT:
		fprintf(fp, "%s %d\n", name, var.intVal);
		break;
	case VT_BOOL:
		if (var.boolVal == VARIANT_TRUE)
			fprintf(fp, "%s 1\n", name);
		else
			fprintf(fp, "%s 0\n", name);
		break;
	case VT_R4:
		fprintf(fp, "%s %f\n", name, var.fltVal);
		break;
	case VT_R8:
		fprintf(fp, "%s %.4f\n", name, var.dblVal);
		break;
	case VT_BSTR:
		tmpstr = var.bstrVal;
		str1 = (char*)tmpstr;
		if (strlen(str1) <= 0)
			fprintf(fp, "%s null\n", name);
		else
			fprintf(fp, "%s %s\n", name, str1);
		break;
	default:
		fprintf(fp, "continue 0\n");
		break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDlgExportArcgisMdb_Batch Dialog

CDlgExportArcgisMdb_Batch::CDlgExportArcgisMdb_Batch(CWnd* pParent /*=NULL*/)
	: CDlgExportArcgisMdb(CDlgExportArcgisMdb_Batch::IDD, pParent)
{
	CWinApp *pApp = AfxGetApp();
	m_strCodeList = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strCodeList", _T(""));
	m_strTemplateFile = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strTemplateFile", _T(""));
	m_strPrjFile = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strPrjFile", _T(""));
	m_strvPrjFile = pApp->GetProfileString(EXPORTMDB_SETTINGS, "strvPrjFile", _T(""));

	m_bBreakParallel = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bBreakParallel", FALSE);
	m_bGDB = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bGDB", FALSE);
	m_bReserveSurface = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bReserveSurface", TRUE);
	m_bUseIndex = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bUseIndex", FALSE);
	m_bCollect = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bCollect", TRUE);
	m_bMark = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bMark", TRUE);
	m_bEDB = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bEDB", TRUE);
	m_bXAttrOnly = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bXAttrOnly", TRUE);
	m_bExportPointZ = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportPointZ", FALSE);
	m_bExportCurveZ = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportCurveZ", FALSE);
	m_bExportSurfaceZ = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportSurfaceZ", FALSE);
	m_bClockWiseAngle = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bClockWiseAngle", TRUE);
	m_bExportEmptyLayer = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bExportEmptyLayer", TRUE);
	m_bNonArcgis = pApp->GetProfileInt(EXPORTMDB_SETTINGS, "bNonArcgis", TRUE);

	if (m_strCodeList.IsEmpty()) m_strCodeList = StrFromResID(IDS_NOLISTFILE);
	if (m_strTemplateFile.IsEmpty()) m_strTemplateFile = StrFromResID(IDS_USEDEFAULTTEMP);
	if (m_strPrjFile.IsEmpty()) m_strPrjFile = StrFromResID(IDS_NOPRJFILE);
	if (m_strvPrjFile.IsEmpty()) m_strvPrjFile = StrFromResID(IDS_NOPRJFILE);
}

void CDlgExportArcgisMdb_Batch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportArcgisMdb)
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH6, m_strFdbFiles);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH, m_strFileDir);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH2, m_strCodeList);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH3, m_strTemplateFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH4, m_strPrjFile);
	DDX_Text(pDX, IDC_EDIT_ED_FILEPATH5, m_strvPrjFile);
	DDX_Check(pDX, IDC_CHECK_PARALLEL, m_bBreakParallel);
	DDX_Check(pDX, IDC_EXPORT_GEB, m_bGDB);
	DDX_Check(pDX, IDC_CHECK_USEINDEX, m_bUseIndex);
	DDX_Check(pDX, IDC_CHECK_COLLECT, m_bCollect);
	DDX_Check(pDX, IDC_CHECK_MARK, m_bMark);
	DDX_Check(pDX, IDC_CHECK_EDB, m_bEDB);
	DDX_Check(pDX, IDC_CHECK_XATTR_ONLY, m_bXAttrOnly);
	DDX_Check(pDX, IDC_CHECK_RESERVE_SURFACE, m_bReserveSurface);
	DDX_Check(pDX, IDC_CHECK_NOEXPORT, m_bNonArcgis);
	DDX_Check(pDX, IDC_CHECK_EXPORT_POINTZ, m_bExportPointZ);
	DDX_Check(pDX, IDC_CHECK_EXPORT_CURVEZ, m_bExportCurveZ);
	DDX_Check(pDX, IDC_CHECK_EXPORT_SURFACEZ, m_bExportSurfaceZ);
	DDX_Check(pDX, IDC_CHECK_CLOCKWISE_ANGLE, m_bClockWiseAngle);
	DDX_Check(pDX, IDC_CHECK3, m_bExportEmptyLayer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportArcgisMdb_Batch, CDialog)
	//{{AFX_MSG_MAP(CDlgExportArcgisMdb)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE5, OnBrowseFdb)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE, OnBrowseFolder)
	ON_BN_CLICKED(IDC_BUTTON_ED_BROWSE2, OnButtonEdBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE4, OnButtonBrowse4)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE5, OnButtonBrowse5)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH2, OnSetfocusEditEdFilepath2)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH2, OnKillfocusEditEdFilepath2)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH3, OnSetfocusEditEdFilepath3)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH3, OnKillfocusEditEdFilepath3)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH4, OnSetfocusEditEdFilepath4)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH4, OnKillfocusEditEdFilepath4)
	ON_EN_SETFOCUS(IDC_EDIT_ED_FILEPATH5, OnSetfocusEditEdFilepath5)
	ON_EN_KILLFOCUS(IDC_EDIT_ED_FILEPATH5, OnKillfocusEditEdFilepath5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportArcgisMdb message handlers

static void HideFtrsNotGIS(CDlgDataSource* pDS)
{
	if (!pDS) return;
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	CFtrLayer *pLayer = NULL;
	CFeature *pFtr = NULL;

	for (int i = 0; i<pDS->GetFtrLayerCount(); i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer || pLayer->IsLocked() || !pLayer->IsVisible() || pLayer->IsEmpty())
			continue;
		int nSize = pLayer->GetObjectCount();
		if (nSize <= 0)
			continue;

		CFeature* pTfeature = NULL;
		for (int n = 0; n<nSize; n++)
		{
			pTfeature = pLayer->GetObject(n, FILTERMODE_DELETED);
			if (pTfeature)break;
		}
		if (!pTfeature)
			continue;

		int nClass = pTfeature->GetGeometry()->GetClassType();
		BOOL IsDBLayerNameEmpty = TRUE;

		//使用层方案配置中的入库几何体类型
		if (pScheme)
		{
			CSchemeLayerDefine *pDef = pScheme->GetLayerDefine(pLayer->GetName());
			if (pDef)
			{
				nClass = pDef->GetDbGeoClass();
				CString DBLayerName = pDef->GetDBLayerName();
				IsDBLayerNameEmpty = DBLayerName.IsEmpty();
			}
		}

		int nEsriClass = mEsriGeometryNull;

		switch (nClass)
		{
		case CLS_GEOPOINT:
		case CLS_GEODIRPOINT:
		case CLS_GEOSURFACEPOINT:
			nEsriClass = mEsriGeometryPoint;
			break;
		case CLS_GEOCURVE:
		case CLS_GEODCURVE:
		case CLS_GEOPARALLEL:
			nEsriClass = mEsriGeometryPolyline;
			break;
		case CLS_GEOMULTISURFACE:
		case CLS_GEOSURFACE:
			nEsriClass = mEsriGeometryPolygon;
			break;
		default:
			break;
		}

		for (int j = 0; j<pLayer->GetObjectCount(); j++)
		{
			pFtr = pLayer->GetObject(j, FILTERMODE_DELETED);
			if (!pFtr)continue;

			if (IsDBLayerNameEmpty)
			{
				pFtr->EnableVisible(FALSE);
				continue;
			}

			switch (nEsriClass)
			{
			case mEsriGeometryPoint:
				if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
				{
					pFtr->EnableVisible(FALSE);
				}
				break;
			case mEsriGeometryPolyline:
				if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
				{
					pFtr->EnableVisible(FALSE);
				}
				break;
			case mEsriGeometryPolygon:
				if (!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
				{
					pFtr->EnableVisible(FALSE);
				}
				break;
			default:
				pFtr->EnableVisible(FALSE);
			}
		}
	}
}

void CDlgExportArcgisMdb_Batch::OnOK() 
{
	UpdateData(TRUE);

	if (m_arrFdbStr.GetSize() == 0 || !PathFileExists(m_strFileDir))
	{
		AfxMessageBox(StrFromResID(IDS_NOPATH_OR_NOFDBFILES));
		return;
	}
	
	CDialog::OnOK();

	if (m_strCodeList.Compare(StrFromResID(IDS_NOLISTFILE)) == 0)
		m_strCodeList = _T("");
	if (m_strTemplateFile.Compare(StrFromResID(IDS_USEDEFAULTTEMP)) == 0)
		m_strTemplateFile = _T("");
	if (m_strPrjFile.Compare(StrFromResID(IDS_NOPRJFILE)) == 0)
		m_strPrjFile = _T("");
	if (m_strvPrjFile.Compare(StrFromResID(IDS_NOPRJFILE)) == 0)
		m_strvPrjFile = _T("");

	CWinApp *pApp = AfxGetApp();
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strCodeList", m_strCodeList);
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strTemplateFile", m_strTemplateFile);
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strPrjFile", m_strPrjFile);
	pApp->WriteProfileString(EXPORTMDB_SETTINGS, "strvPrjFile", m_strvPrjFile);

	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bBreakParallel", m_bBreakParallel);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bGDB", m_bGDB);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bReserveSurface", m_bReserveSurface);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bUseIndex", m_bUseIndex);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bCollect", m_bCollect);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bMark", m_bMark);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bEDB", m_bEDB);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bXAttrOnly", m_bXAttrOnly);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportPointZ", m_bExportPointZ);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportCurveZ", m_bExportCurveZ);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportSurfaceZ", m_bExportSurfaceZ);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bClockWiseAngle", m_bClockWiseAngle);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bExportEmptyLayer", m_bExportEmptyLayer);
	pApp->WriteProfileInt(EXPORTMDB_SETTINGS, "bNonArcgis", m_bNonArcgis);

	BeginCheck41License

	int nsize = m_arrFdbStr.GetSize();
	if (nsize <= 0) return;

	CString str;
	str.Format(StrFromResID(IDS_TOTAL_FDB_NUMBRE), nsize);
	GOutPut(str);

	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos1;
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	if ((pos1 = strrchr(path, '\\')))*pos1 = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");
	for (int i = 0; i < nsize; i++)
	{
		CString fdbName = m_arrFdbStr.GetAt(i);

		CString str_tmp;
		str_tmp.Format("(%d\\%d)", i + 1, nsize);
		str_tmp = StrFromResID(IDS_PROCESS_FILE) + str_tmp + _T(": ") + fdbName + _T("\n");
		GOutPut(str_tmp);

		int pos1 = fdbName.ReverseFind('\\');
		CString outname = fdbName.Mid(pos1 + 1);
		CString temp = outname.Left(outname.GetLength() - 4);
		if (m_bGDB)
		{
			m_strFileName = temp + ".gdb";
		}
		else
		{
			m_strFileName = temp + ".mdb";
		}

		m_strFilePath = m_strFileDir + "\\" + m_strFileName;
		
		CString cmd;
		cmd.Format("\"%s\" exportMDB \"%s\" \"%s\"", path, fdbName, m_strFilePath);

		STARTUPINFO stStartUpInfo;
		memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
		stStartUpInfo.cb = sizeof(STARTUPINFO);
		//stStartUpInfo.wShowWindow = SW_HIDE;

		PROCESS_INFORMATION stProcessInfo;
		memset(&stProcessInfo, 0, sizeof(stProcessInfo));
		if (!CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo))
			return;
		while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
		{
		}
	}
	GOutPut(StrFromResID(IDS_PROCESS_END));

	EndCheck41License
}

void CDlgExportArcgisMdb_Batch::OnBrowseFdb()
{
	CString filter(StrFromResID(IDS_LOADMDB_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, _T(".fdb"), _T(""), OFN_ENABLESIZING | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	dlg.m_ofn.nMaxFile = 128;
	char strFile[128 * MAX_PATH] = { 0 };
	dlg.m_ofn.lpstrFile = strFile;
	CStringArray arrFiles;
	m_arrFdbStr.RemoveAll();
	if( dlg.DoModal()!=IDOK ) return;

	UpdateData(TRUE);

	POSITION pos = dlg.GetStartPosition();//标示当前所选的第一个文件位置 
	while (pos !=NULL)
	{
		arrFiles.Add(dlg.GetNextPathName(pos));//添加多选择文件名到Cstring数组	
	}	
	
	m_arrFdbStr.Copy(arrFiles);
	int nsize = m_arrFdbStr.GetSize();
	
	m_strFdbFiles.Empty();
	for (int i=0;i<nsize;i++)
	{
		CString tmp2 = m_arrFdbStr.GetAt(i);		
		
		int pos2 = tmp2.ReverseFind(_T('\\'));
		if( pos2>=0 )tmp2.Delete(0,pos2+1);
		tmp2=_T("\"")+tmp2+_T("\"");
		m_strFdbFiles+=tmp2+_T(";");
	}

	if(nsize>0)
	{
		CString  fileDir = m_arrFdbStr.GetAt(0);
		int pos2 = fileDir.ReverseFind(_T('\\'));
		if( pos2>=0 )
		{
			m_strFileDir = fileDir.Left(pos2);
		}
	}
	
	UpdateData(FALSE);
}

void CDlgExportArcgisMdb_Batch::OnBrowseFolder() 
{
	UpdateData(TRUE);
	CString strInitDir = "c:\\";
	
	if( m_arrFdbStr.GetSize()>0 )
	{
		strInitDir = m_arrFdbStr[0];
		int pos = strInitDir.ReverseFind('\\');
		if( pos )strInitDir = strInitDir.Left(pos+1);
	}
	
	BrowseFolder(StrFromResID(IDS_SELECT_DIRECTORY), m_strFileDir, strInitDir, GetSafeHwnd());
	
	UpdateData(FALSE);
}