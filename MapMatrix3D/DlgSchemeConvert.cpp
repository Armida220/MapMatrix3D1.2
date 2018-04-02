// DlgSchemeConvert.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSchemeConvert.h"
#include "UIFFileDialogEx.h"
#include "io.h"
#include "editbaseDoc.h"
#include "SQLiteAccess.h"
#include "ListFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSchemeConvert dialog


CDlgSchemeConvert::CDlgSchemeConvert(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSchemeConvert::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSchemeConvert)
	m_strFdbFile = _T("");
	m_strSrcConfig = _T("");
	m_strDestConfig = _T("");
	m_strLayerMapFile = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSchemeConvert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSchemeConvert)
	DDX_Text(pDX, IDC_EDIT_FDB, m_strFdbFile);
	DDX_Text(pDX, IDC_EDIT_SRCCONFIG, m_strSrcConfig);
	DDX_Text(pDX, IDC_EDIT_DESTCONFIG, m_strDestConfig);
	DDX_Text(pDX, IDC_EDIT_LAYERMAPFILE, m_strLayerMapFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSchemeConvert, CDialog)
	//{{AFX_MSG_MAP(CDlgSchemeConvert)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnButtonBrowse2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnButtonBrowse3)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE4, OnButtonBrowse4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSchemeConvert message handlers

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
					LPTSTR lpszPath,		// [out] 返回的文件路径
					LPCTSTR lpszInitDir,	// [in] 初始文件路径
					HWND hWnd,				// [in] 父系窗口
					BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
					UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格

//fdb
void CDlgSchemeConvert::OnButtonBrowse1() 
{
	CString filter(StrFromResID(IDS_LOADMDB_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, _T(".fdb"), NULL, OFN_ENABLESIZING | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter);
	dlg.m_ofn.nMaxFile = 4096;
	dlg.m_ofn.lpstrFile = new char[dlg.m_ofn.nMaxFile*MAX_PATH];
	ZeroMemory(dlg.m_ofn.lpstrFile,sizeof(char)*dlg.m_ofn.nMaxFile*MAX_PATH);
	CStringArray arrFiles;
	m_arrFdbStr.RemoveAll();
	if( dlg.DoModal()!=IDOK ) return;

//	if( dlg.GetPage()==CFileDialogEx::UIFFileOpen )
	{
		POSITION pos = dlg.GetStartPosition();//标示当前所选的第一个文件位置 
		while (pos !=NULL)
		{
			arrFiles.Add(dlg.GetNextPathName(pos));//添加多选择文件名到Cstring数组	
		}	
	}
//	else
//	{
//		arrFiles.Add(dlg.GetPathName());
//	}
	
	m_arrFdbStr.Copy(arrFiles);

	m_strFdbFile.Empty();
	for (int i=0;i<m_arrFdbStr.GetSize();i++)
	{
		CString tmp2 = m_arrFdbStr.GetAt(i);		
				
		int pos2 = tmp2.ReverseFind(_T('\\'));
		if( pos2>=0 )tmp2.Delete(0,pos2+1);
		tmp2=_T("\"")+tmp2+_T("\"");
		m_strFdbFile+=tmp2+_T(";");
	}
	
	UpdateData(FALSE);	
}

//src config
void CDlgSchemeConvert::OnButtonBrowse2() 
{
	UpdateData(TRUE);
	char retPath[_MAX_PATH]={0};

	CString strInitDir = m_strSrcConfig;
	if( strInitDir.GetLength()<=0 )
	{
		strInitDir = GetConfigPath();
		
		int len = strInitDir.GetLength();
		if( len>0 && strInitDir[len-1]=='\\' )
			strInitDir.SetAt(len-1,0);
	}

	if( !BrowseFolderEx(StrFromResID(IDS_SYMLIB_DIR),retPath,strInitDir,m_hWnd) )return;
	
	strInitDir = retPath;
	
	int str_len = strlen(retPath);
	if( str_len<=0 )return;
	if( retPath[str_len-1]=='\\')
	{
		retPath[str_len-1] = 0;
	}

	m_strSrcConfig = retPath;	
	UpdateData(FALSE);	
}

//dest config
void CDlgSchemeConvert::OnButtonBrowse3() 
{
	UpdateData(TRUE);
	char retPath[_MAX_PATH]={0};

	CString strInitDir = m_strDestConfig;
	if( strInitDir.GetLength()<=0 )
	{
		strInitDir = GetConfigPath();
		
		int len = strInitDir.GetLength();
		if( len>0 && strInitDir[len-1]=='\\' )
			strInitDir.SetAt(len-1,0);
	}

	if( !BrowseFolderEx(StrFromResID(IDS_SYMLIB_DIR),retPath,strInitDir,m_hWnd) )return;
	
	strInitDir = retPath;
	
	int str_len = strlen(retPath);
	if( str_len<=0 )return;
	if( retPath[str_len-1]=='\\')
	{
		retPath[str_len-1] = 0;
	}

	m_strDestConfig = retPath;	
	UpdateData(FALSE);
	
}

//layer map file
void CDlgSchemeConvert::OnButtonBrowse4() 
{
	CString filter(StrFromResID(IDS_LAYFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strLayerMapFile = dlg.GetPathName();
	UpdateData(FALSE);	
	
}


BOOL CDlgSchemeConvert::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



static CDlgDataSource *_NewFDB(LPCTSTR fileName, CConfigLibManager *pCfgLibManager, long scale, PT_3D *boundPts, double zmin, double zmax, CSQLiteAccess *& pAccess)
{
	CString strPath = fileName;

	if( strPath.IsEmpty() )
		return NULL;

	if(::PathFileExists(strPath))
	{
		if(::DeleteFile(strPath)==FALSE)
			return NULL;
	}

	CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
	if( !pDataSource )
	{	
		return NULL;
	}
	CSQLiteAccess *pSqlAccess = new CSQLiteAccess();
	if(!pSqlAccess) 
	{
		delete pDataSource;
		return NULL;
	}
	pSqlAccess->Attach(strPath);
	
	pDataSource->SetAccessObject(pSqlAccess);
	CScheme *pScheme = pCfgLibManager->GetScheme(scale);
	DWORD scale1 = pCfgLibManager->GetScaleByScheme(pScheme);
	if (scale1==0)
	{
		delete pDataSource;
		return NULL;
	}
	pSqlAccess->BatchUpdateBegin();
	pSqlAccess->CreateFileSys(scale, pScheme);
	pSqlAccess->BatchUpdateEnd();

	pSqlAccess->BatchUpdateBegin();
	
	pDataSource->LoadAll(NULL,FALSE);	
	//写入工作区信息

	pDataSource->SetBound(boundPts,zmin,zmax);
	pDataSource->SetScale(scale);

	pSqlAccess->BatchUpdateEnd();

	pAccess = pSqlAccess;

	return pDataSource;
}



static CDlgDataSource *_OpenFDB(LPCTSTR fileName) 
{
	CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
	if( !pDataSource )
	{	
		return NULL;
	}
	CSQLiteAccess *pSqlAccess = new CSQLiteAccess;
	if(!pSqlAccess) 
	{
		delete pDataSource;
		return NULL;
	}

	if(!pSqlAccess->Attach(fileName))
	{		
		delete pDataSource;
		delete pSqlAccess;
		return NULL;
	}
	
	pDataSource->SetAccessObject(pSqlAccess);
	pSqlAccess->ReadDataSourceInfo(pDataSource);	
	pDataSource->LoadAll(NULL,FALSE);	
	//写入工作区信息

	return pDataSource;	
}


static BOOL ConvertDS(CConfigLibManager *pCfgLib1, CConfigLibManager *pCfgLib2, CDlgDataSource *pDS1, CDlgDataSource *pDS2, CListFile& listFile)
{
	CScheme *pScheme1 = pCfgLib1->GetScheme(pDS1->GetScale());
	CScheme *pScheme2 = pCfgLib2->GetScheme(pDS2->GetScale());

	if(!pScheme1 || !pScheme2)
		return FALSE;

	long lObjSum = 0;

	for (int i=0; i<pDS1->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = pDS1->GetFtrLayerByIndex(i);		
		if( !pLayer )continue;
		
		int nObjs = pLayer->GetObjectCount();
		lObjSum += nObjs;
	}

	GProgressStart(lObjSum);

	for(i=0; i<pDS1->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer1 = pDS1->GetFtrLayerByIndex(i);
		if(!pLayer1)
			continue;

		CFtrLayer *pLayer2 = NULL;
		CSchemeLayerDefine *pLayDef2 = NULL;

		BOOL bAddLayer = FALSE;
		
		CSchemeLayerDefine *pLayDef1 = pScheme1->GetLayerDefine(pLayer1->GetName());
		if(!pLayDef1)
		{
			bAddLayer = TRUE;
		}
		else
		{
			__int64 code1 = pLayDef1->GetLayerCode();
			CString strcode1;
			strcode1.Format("%I64d",code1);

			CString strcode2 = listFile.FindMatchItem(strcode1,TRUE,TRUE);
			if(strcode2.IsEmpty())
				bAddLayer = TRUE;
			else
			{
				__int64 code2 = _atoi64(strcode2);
				pLayDef2 = pScheme2->GetLayerDefine(NULL,TRUE,code2);
				if(!pLayDef2)
					bAddLayer = TRUE;
				else
				{
					pLayer2 = pDS2->GetFtrLayer(pLayDef2->GetLayerName());
					if(!pLayer2)
					{
						bAddLayer = TRUE;
					}
				}
			}
		}

		if(bAddLayer)
		{
			pLayer2 = pDS2->GetFtrLayer(pLayer1->GetName());
			if(pLayer2)
			{
				bAddLayer = FALSE;
			}
		}

		if(bAddLayer)
		{
			pLayer2 = new CFtrLayer();

			CValueTable tab;
			tab.BeginAddValueItem();
			pLayer1->WriteTo(tab);
			tab.EndAddValueItem();

			pLayer2->ReadFrom(tab);
			pLayer2->SetID(0);
			pLayer2->SetDisplayOrder(-1);
			
			pDS2->AddFtrLayer(pLayer2);
		}

		for(int j=0; j<pLayer1->GetObjectCount(); j++)
		{
			GProgressStep();

			CFeature *pFtr = pLayer1->GetObject(j, FILTERMODE_DELETED);
			if(!pFtr)
				continue;

			CFeature *pNewFtr = pFtr->Clone();

			//CValueTable tab;
			//if( pLayDef2 && pLayDef2->GetDefaultValues(tab) )
			//{
			//	pNewFtr->ReadFrom(tab);
			//}
			pDS2->AddObject(pNewFtr,pLayer2->GetID());
		}
	}

	GProgressEnd();
	
	return TRUE;
}


void CDlgSchemeConvert::OnOK() 
{
	UpdateData(TRUE);

	if(_access(m_strFdbFile,4)==0)
	{
		m_arrFdbStr.RemoveAll();
		m_arrFdbStr.Add(m_strFdbFile);
	}

	if(m_arrFdbStr.GetSize()<=0)
	{
		AfxMessageBox("No file are converted!");
		CDialog::OnOK();
		return;
	}

	CListFile file;
	if(!file.Open(m_strLayerMapFile))
	{
		AfxMessageBox("The list file is invalid!");
		CDialog::OnOK();
		return;		
	}

	CConfigLibManager *pCfgLib1 = new CConfigLibManager();
	CConfigLibManager *pCfgLib2 = new CConfigLibManager();

	if( !pCfgLib1->LoadConfig(m_strSrcConfig) || !pCfgLib2->LoadConfig(m_strDestConfig) )
	{
		AfxMessageBox("Config pathes are invalid!");	
		CDialog::OnOK();
		return;
	}

	for(int i=0; i<m_arrFdbStr.GetSize(); i++)
	{
		CString path = m_arrFdbStr[i];

		GOutPut("Convert: " + path + "......", FALSE);

		CString pathNew;
		if(path.Right(4).CompareNoCase(".fdb")==0)
		{
			pathNew = path.Left(path.GetLength()-4) + "_new.fdb";
		}
		else
		{
			pathNew = path + "_new.fdb";
		}

		CDlgDataSource *pDS1 = _OpenFDB(path);
		if(!pDS1)
			continue;
		
		long scale = pDS1->GetScale();
		PT_3D pts[4];
		double z1,z2;
		pDS1->GetBound(pts,&z1,&z2);

		CSQLiteAccess *pAccess = NULL;
		
		CDlgDataSource *pDS2 = _NewFDB(pathNew,pCfgLib2,scale,pts,z1,z2,pAccess);
		if(!pDS2)
		{
			delete pDS1;
			continue;
		}

		pAccess->BatchUpdateBegin();

		if( ConvertDS(pCfgLib1,pCfgLib2,pDS1,pDS2,file) )
		{
			GOutPut("OK!",TRUE);
		}
		else
		{
			GOutPut("Failed!",TRUE);
		}
		pDS2->SaveAll(NULL);

		pAccess->BatchUpdateEnd();
		
		delete pDS1;
		delete pDS2;
	}
	
	CDialog::OnOK();
}