// DlgExportCodeTable.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgExportCodeTable.h"
#include "SymbolLib.h "
#include "SmartViewFunctions.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTable dialog


CDlgExportCodeTable::CDlgExportCodeTable(CWnd* pParent /*=NULL*/, CConfigLibManager* pCon)
	: CDialog(CDlgExportCodeTable::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportCodeTable)
	m_strFilePath = _T("");
	//}}AFX_DATA_INIT
	m_pData = NULL;
	SetConfigData(pCon);
}


void CDlgExportCodeTable::SetConfigData(CConfigLibManager* pCon)
{
	m_pData = pCon;
}

void CDlgExportCodeTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportCodeTable)
	DDX_Control(pDX, IDC_COMBO_SCALE, m_comBoxScale);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_strFilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportCodeTable, CDialog)
	//{{AFX_MSG_MAP(CDlgExportCodeTable)
	ON_BN_CLICKED(IDC_BUTTON_BROWER, OnButtonBrower)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTable message handlers

void CDlgExportCodeTable::OnButtonBrower() 
{
	CString filter(StrFromResID(IDS_LOADTXT_FILTER2));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, _T(".txt"), m_strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);	
}

BOOL CDlgExportCodeTable::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pData)return FALSE;
	int  nNum = m_pData->GetConfigLibCount();
	ConfigLibItem item;
	for (int i=0;i<nNum;i++)
	{
		item = m_pData->GetConfigLibItem(i);
		char buffer[20];
		_itoa(item.GetScale(), buffer, 10 );
		m_comBoxScale.AddString(buffer);
	}
	m_comBoxScale.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportCodeTable::OnOK() 
{
	//得到比例尺
	int i = m_comBoxScale.GetCurSel();
	if (i < 0) return;
	CString str;
	m_comBoxScale.GetLBText(i,str);
	DWORD scale;
	scale = atoi(LPCTSTR(str));
	CScheme *pSch = m_pData->GetScheme(scale);
	if (!pSch)return;
	FILE *stream;
	__int64 code;
	CString layName;
	CSchemeLayerDefine *pLayDef;
	if((stream = _tfopen(LPCTSTR(m_strFilePath), _T("w+")))!=NULL ) 
	{
		for (int i=0;i<pSch->GetLayerDefineCount();i++)
		{
			pLayDef = pSch->GetLayerDefine(i);
			if (!pLayDef)continue;
			code = pLayDef->GetLayerCode();
			layName = pLayDef->GetLayerName();
			_ftprintf(stream,_T("%12lI64d%64s%16s\n"),code,(LPCTSTR)layName,(LPCTSTR)pLayDef->GetDBLayerName());
		}
		fclose(stream);
	}
	else
		AfxMessageBox("The file  was not opened\n");
	CDialog::OnOK();
}



/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTable_Symbols dialog


CDlgExportCodeTable_Symbols::CDlgExportCodeTable_Symbols(CWnd* pParent /*=NULL*/, CConfigLibManager* pCon)
	: CDialog(CDlgExportCodeTable_Symbols::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExportCodeTable_Symbols)
	m_strFilePath = _T("");
	//}}AFX_DATA_INIT
	m_pData = NULL;
	SetConfigData(pCon);
}


void CDlgExportCodeTable_Symbols::SetConfigData(CConfigLibManager* pCon)
{
	m_pData = pCon;
}

void CDlgExportCodeTable_Symbols::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExportCodeTable_Symbols)
	DDX_Control(pDX, IDC_COMBO_SCALE, m_comBoxScale);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_strFilePath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExportCodeTable_Symbols, CDialog)
	//{{AFX_MSG_MAP(CDlgExportCodeTable_Symbols)
	ON_BN_CLICKED(IDC_BUTTON_BROWER, OnButtonBrower)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExportCodeTable_Symbols message handlers

void CDlgExportCodeTable_Symbols::OnButtonBrower() 
{
	CString filter(StrFromResID(IDS_LOADTXT_FILTER2));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, _T(".txt"), m_strFilePath, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	UpdateData(TRUE);
	m_strFilePath = dlg.GetPathName();
	UpdateData(FALSE);	
}

BOOL CDlgExportCodeTable_Symbols::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_pData)return FALSE;
	int  nNum = m_pData->GetConfigLibCount();
	ConfigLibItem item;
	for (int i=0;i<nNum;i++)
	{
		item = m_pData->GetConfigLibItem(i);
		char buffer[20];
		_itoa(item.GetScale(), buffer, 10 );
		m_comBoxScale.AddString(buffer);
	}
	m_comBoxScale.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExportCodeTable_Symbols::OnOK() 
{
	//得到比例尺
	int i = m_comBoxScale.GetCurSel();
	CString str;
	m_comBoxScale.GetLBText(i,str);
	DWORD scale;
	scale = atoi(LPCTSTR(str));
	CScheme *pSch = m_pData->GetScheme(scale);
	if (!pSch)return;
	FILE *stream;
	__int64 code;
	CString layName;
	CSchemeLayerDefine *pLayDef;
	if((stream = _tfopen(LPCTSTR(m_strFilePath), _T("w+")))!=NULL ) 
	{		
		CStringArray arrBlockNames, arrExportBlockNames;
		
		for (int i=0;i<pSch->GetLayerDefineCount();i++)
		{
			pLayDef = pSch->GetLayerDefine(i);
			if (!pLayDef)continue;
			code = pLayDef->GetLayerCode();
			layName = pLayDef->GetLayerName();

			int nSymbols = pLayDef->GetSymbolCount();

			//平行线或双线，一般要求导出为面，为此增加一个母线转换行
			if(pLayDef->GetGeoClass()==CLS_GEOPARALLEL || pLayDef->GetGeoClass()==CLS_GEODCURVE )
			{
				_ftprintf(stream,_T("%121I64d %16s %2ld %32s %2ld %12lI64d %32s %32s\n"),code,(LPCTSTR)pLayDef->GetDBLayerName(),0,_T("Continuous"),3,code,_T("NULL"),(LPCTSTR)layName);
			}
			else
			{
				_ftprintf(stream,_T("%121I64d %16s %2ld %32s %2ld %12lI64d %32s %32s\n"),code,(LPCTSTR)pLayDef->GetDBLayerName(),0,_T("Continuous"),0,code,_T("NULL"),(LPCTSTR)layName);
			}

			CStringArray arrSymNames;
			CArray<int,int> arrIndex;
			int m = 0;

			for(int j=0; j<nSymbols; j++)
			{
				CSymbol *pSym = pLayDef->GetSymbol(j);
				CString symName;
				int nSymType = pSym->GetType();

				BOOL bBlock = FALSE, bLinetype = FALSE;

				if(nSymType==SYMTYPE_DASHLINETYPE || nSymType==SYMTYPE_CELLLINETYPE || nSymType==SYMTYPE_SCALELINETYPE || nSymType==SYMTYPE_LINEHATCH ||
					nSymType==SYMTYPE_DIAGONAL || nSymType==SYMTYPE_ANGBISECTORTYPE || nSymType==SYMTYPE_SCALEARCTYPE)
				{
					symName.Format(_T("L%04d_%d"),i,j+1);
					bLinetype = TRUE;
				}
				else if( nSymType==SYMTYPE_ANNOTATION )
				{
					symName.Format(_T("F%04d_%d"),i,j+1);
				}
				else
				{
					symName.Format(_T("B%04d_%d"),i,j+1);
					bBlock = TRUE;
				}

				//缺省0是自动判断
				int nExportSymType = 0;

				//将作为点符号放在头尾处的齿打散导出，标志是8
				if(nSymType==SYMTYPE_CELL)
				{
					CCell *pCell = (CCell*)pSym;
					if( (pCell->m_nPlaceType==CCell::Head || pCell->m_nPlaceType==CCell::Tail) )
					{
						CCellDefLib *cellLib = GetCellDefLib();
						CellDef def = cellLib->GetCellDef(pCell->m_strCellDefName);
						if(def.m_pgr && def.m_pgr->GetLinePts(NULL)==2 )
						{
							const Graph2d *pHead = def.m_pgr->HeadGraph();
							if(pHead && IsGrLineString2d(pHead) )
							{
								const GrLineString2d *pLS = (GrLineString2d*)pHead;
								if(pLS->ptlist.nuse==2)
								{
									nExportSymType = 8;
								}
							}
						}
					}
				}

				//图元线型仅当块符号为齿时，按照线型导出，其他的都按照打散的图块导出
				if(nSymType==SYMTYPE_CELLLINETYPE)
				{
					nExportSymType = 1;
					
					CCellLinetype *pCellLT = (CCellLinetype*)pSym;
					CCellDefLib *cellLib = GetCellDefLib();
					CellDef def = cellLib->GetCellDef(pCellLT->m_strCellDefName);
					if(def.m_pgr && def.m_pgr->GetLinePts(NULL)==2 )
					{
						const Graph2d *pHead = def.m_pgr->HeadGraph();
						if(pHead && IsGrLineString2d(pHead) )
						{
							const GrLineString2d *pLS = (GrLineString2d*)pHead;
							if(pLS->ptlist.nuse==2)
							{
								nExportSymType = 0;
							}
						}
					}

					if(nExportSymType==1)
					{
						symName.Format(_T("B%04d_%d"),i,j+1);
						bBlock = TRUE;
					}
				}

				//相同块名保证输出名是一致的
				if(bBlock)
				{
					CString blkName = pSym->GetName();
					for(int k=0; k<arrBlockNames.GetSize(); k++)
					{
						if(blkName.CompareNoCase(arrBlockNames[k])==0)
						{
							break;
						}
					}
					
					if(k<arrBlockNames.GetSize())
					{
						symName = arrExportBlockNames[k];
					}
					else
					{
						arrBlockNames.Add(blkName);
						arrExportBlockNames.Add(symName);
					}
				}

				//相同符号名，保证输出代码（CASS码）是一致的
				if(1)
				{
					CString blkName = pSym->GetName();
					for(int k=0; k<arrSymNames.GetSize(); k++)
					{
						if(blkName.CompareNoCase(arrSymNames[k])==0)
						{
							break;
						}
					}
					
					if(k<arrSymNames.GetSize())
					{
					}
					else
					{
						arrSymNames.Add(blkName);
						arrIndex.Add(m);
						m++;
					}
				}

				//与母线完全重叠的实线，可以不用导出
				if( nSymType==SYMTYPE_DASHLINETYPE && pLayDef->GetGeoClass()==CLS_GEOCURVE )
				{
					CDashLinetype *pDashLT = (CDashLinetype*)pSym;
					CBaseLineTypeLib *ltLib = GetBaseLineTypeLib();
					BaseLineType lt = ltLib->GetBaseLineType(pDashLT->m_strBaseLinetypeName);
					if( fabs(pDashLT->m_fBaseOffset)<1e-6 && lt.IsSolidLine() )
					{
						symName = "*";
					}
				}
				
				_ftprintf(stream,_T("%121I64d %16s %2ld %32s %2ld %12lI64d %32s\n"),code,(LPCTSTR)pLayDef->GetDBLayerName(),j+1,(LPCTSTR)symName,nExportSymType,code*10+m,(LPCTSTR)pSym->GetName());
			}
		}
		fclose(stream);
	}
	else
		AfxMessageBox("The file  was not opened\n");
	CDialog::OnOK();
}
