// DlgCellDefLinetypeView.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgCellDefLinetypeView.h"
#include "SmartViewFunctions.h"
#include "DlgNewLinetype.h"
#include "SymbolLib.h"
#include "Matrix.h"
#include "Functions_temp.h"
#include "DxfAccess.h"
#include "DlgDataSource.h"
#include "editbasedoc.h"
#include "ExMessage.h"
#include "DlgNewGroup.h"
#include "Feature.h"
#include "CollectionViewBar.h"
#include "RegDef2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol);

#define TIP_SEARCH		StrFromResID(IDS_SEARCH_NAME)
#define TIMERID_CREATEIMAGE	1
#define TIMERID_PREVIEW		2
/////////////////////////////////////////////////////////////////////////////
// CDlgCellDefLinetypeView dialog


CDlgCellDefLinetypeView::CDlgCellDefLinetypeView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCellDefLinetypeView::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgCellDefLinetypeView)
	//}}AFX_DATA_INIT
	m_nLibType = -1;
	m_bShowOK = FALSE;
	m_bLibTypeValid = TRUE;
	m_bScaleValid = TRUE;
	m_bAddSymbol = FALSE;
	m_bModified = FALSE;
	m_bNewCellValid = FALSE;
	m_nClickNum = 0;
	m_pCellDoc = NULL;
	m_bClosing = FALSE;
	m_nLastSel = -1;
	m_bHideModifyCtrl = FALSE;
	m_hBmp = NULL;	
}

CDlgCellDefLinetypeView::~CDlgCellDefLinetypeView()
{
	if( m_hBmp )::DeleteObject( m_hBmp );
}



void CDlgCellDefLinetypeView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCellDefLinetypeView)
	DDX_Control(pDX, IDC_LIST_SYMBOL, m_wndListSymbol);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_comboLibType);
	DDX_Control(pDX, IDC_SCALE_COMBO, m_scaleCombo);
	DDX_Control(pDX, IDC_TYPE_LAYERCOMBO, m_comboLayers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCellDefLinetypeView, CDialog)
	//{{AFX_MSG_MAP(CDlgCellDefLinetypeView)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, OnChangeEditSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SYMBOL, OnItemchangedListSymbol)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_SYMBOL, OnEndEditListSymbol)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SYMBOL, OnDblclkListSymbol)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, OnSelchangeTypeCombo)
	ON_BN_CLICKED(IDCANCLE, OnCancle)
	ON_EN_SETFOCUS(IDC_EDIT_SEARCH, OnSetfocusEditSearch)
	ON_EN_KILLFOCUS(IDC_EDIT_SEARCH, OnKillfocusEditSearch)
	ON_BN_CLICKED(IDC_ADDLINE_BUTTON, OnAddlineButton)
	ON_BN_CLICKED(IDC_DELLINE_BUTTON, OnDellineButton)
	ON_BN_CLICKED(IDC_DELALL_BUTTON, OnDelallButton)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_SYMBOL, OnBeginlabeleditListSymbol)
	ON_BN_CLICKED(IDC_STATIC_PREVIEW, OnClickStatic)
	ON_BN_CLICKED(IDC_IMPORTCELLFROMDXF_BUTTON, OnImportcellfromdxfButton)
	ON_BN_CLICKED(IDC_EXPORTCELLTODXF_BUTTON, OnExportcelltodxfButton)
	ON_CBN_SELCHANGE(IDC_SCALE_COMBO, OnSelchangeScaleCombo)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_SYMBOL, OnItemchangingListSymbol)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgCellDefLinetypeView message handlers

void CDlgCellDefLinetypeView::SetConfig(ConfigLibItem config)
{
	m_config = config;

	m_scaleSwitch.SwitchToScale(m_config.GetScale());
}


// void CDlgCellDefLinetypeView::SetLineTypeLib(CBaseLineTypeLib *pLib)
// {
// 	m_config.pLinetypeLib = pLib;
// 	if (m_config.pLinetypeLib)
// 	{
// 		m_strLinetypeLib.Empty();
// 		m_config.pLinetypeLib->Save(m_strLinetypeLib);
// 	}
// 
// //	m_nLibType = 1;
// }



void CDlgCellDefLinetypeView::SetShowMode(BOOL bLibTypeValid, BOOL bShowOK, int nLibType, CString name, BOOL bScaleValid)
{
	m_bLibTypeValid = bLibTypeValid;

	m_bScaleValid = bScaleValid;

	m_bShowOK = bShowOK;

	m_nLibType = nLibType;

	m_strName = name;
	
}


void CDlgCellDefLinetypeView::SetAsSelectMode(int nLibType)
{
	m_bLibTypeValid = FALSE;
	m_bScaleValid = FALSE;
	m_nLibType = nLibType;
	m_bShowOK = TRUE;

	m_bHideModifyCtrl = TRUE;

}

void CDlgCellDefLinetypeView::InitInterFace()
{
	
	m_comboLibType.EnableWindow(m_bLibTypeValid);
	m_scaleCombo.EnableWindow(m_bScaleValid);
	
	if (!m_bShowOK)
	{
		CWnd *pWnd = GetDlgItem(IDOK);
		if( pWnd )	
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		
		pWnd = GetDlgItem(IDCANCLE);
		if( pWnd )	
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}

	if (m_nLibType == 0)
	{
		CWnd *pWnd = GetDlgItem(IDC_ADDLINE_BUTTON);
		if( pWnd )
		{
			pWnd->SetWindowText(StrFromResID(IDS_NEWCELL));
			pWnd->EnableWindow(m_bNewCellValid);
		}
		
		pWnd = GetDlgItem(IDC_IMPORTCELLFROMDXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_SHOW);

		pWnd = GetDlgItem(IDC_EXPORTCELLTODXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_SHOW);
		
		pWnd = GetDlgItem(IDC_DELLINE_BUTTON);
		if( pWnd )
		{
			pWnd->SetWindowText(StrFromResID(IDC_DELCELL));
			pWnd->ShowWindow(SW_SHOW);
		}
		
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_ADDLINE_BUTTON);
		if( pWnd )
		{
			pWnd->EnableWindow(TRUE);
			pWnd->SetWindowText(StrFromResID(IDS_NEWLINE));
		}

		pWnd = GetDlgItem(IDC_IMPORTCELLFROMDXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_HIDE);

		pWnd = GetDlgItem(IDC_EXPORTCELLTODXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
		
		pWnd = GetDlgItem(IDC_DELLINE_BUTTON);
		if( pWnd )
		{
			pWnd->SetWindowText(StrFromResID(IDC_DELLINE));
			pWnd->ShowWindow(SW_SHOW);
		}

	}

	if( m_bHideModifyCtrl )
	{
		CWnd *pWnd = GetDlgItem(IDC_ADDLINE_BUTTON);
		if( pWnd )
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		
		pWnd = GetDlgItem(IDC_IMPORTCELLFROMDXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
		
		pWnd = GetDlgItem(IDC_EXPORTCELLTODXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
		
		pWnd = GetDlgItem(IDC_DELLINE_BUTTON);
		if( pWnd )
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_IMPORTCELLFROMDXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
		
		pWnd = GetDlgItem(IDC_EXPORTCELLTODXF_BUTTON);
		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
		
		pWnd = GetDlgItem(IDC_DELLINE_BUTTON);
		if( pWnd )
		{
			pWnd->ShowWindow(SW_HIDE);
		}

		pWnd = GetDlgItem(IDC_DELALL_BUTTON);
		if( pWnd )
		{
			pWnd->ShowWindow(SW_HIDE);
		}
	}
}

BOOL CDlgCellDefLinetypeView::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 显示比例尺
	m_scaleCombo.ResetContent();
	if (gpCfgLibMan->GetConfigLibCount() < 1)
		return FALSE;
	for (int i=0; i<gpCfgLibMan->GetConfigLibCount(); i++)
	{
		CString data;
		for (int j=0; j<m_scaleCombo.GetCount(); j++)
		{
			
			m_scaleCombo.GetLBText(j,data);
			DWORD dScale = atoi(data);
			if (gpCfgLibMan->GetConfigLibItem(i).GetScale() < dScale)
				break;
		}
		data.Format("%d",gpCfgLibMan->GetConfigLibItem(i).GetScale());
		int index = m_scaleCombo.InsertString(j,data);
		m_scaleCombo.SetItemData(index,i);
	}
	
	//选择当前比例尺
	CString strScale;
	strScale.Format("%d",m_config.GetScale());
	m_scaleCombo.SelectString(-1,strScale);

	m_comboLibType.SetCurSel(m_nLibType);
	
	//IDC_STATIC_PREVIEW
	CWnd *pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	if( !pWnd )return -1;
	CRect rcView;
	pWnd->GetClientRect(&rcView);
	CClientDC cdc(pWnd);
	int cx = rcView.Width(), cy = rcView.Height();
	CBrush br(RGB(255,255,255));
	::FillRect(cdc.GetSafeHdc(),CRect(0,0,cx,cy),(HBRUSH)br);
	
	
	CWnd *pWndSize = GetDlgItem(IDC_STATIC_SIZE);
	if ( pWndSize )
	{
		CString strSize;
		strSize.Format("%.2fmm X %.2fmm",0,0);
		pWndSize->SetWindowText(strSize);
	}

	InitInterFace();

	FillSymbolList();

//	SetTimer(TIMERID_CREATEIMAGE,50,NULL);
//	SetTimer(TIMERID_PREVIEW,500,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int g_nupdate = -1;
void CDlgCellDefLinetypeView::OnItemchangedListSymbol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	// 避免更新两次的情况
	if (pNMListView->uChanged == LVIF_STATE)
	{
		if (pNMListView->uNewState&LVIS_SELECTED)
		{
			// 重新选择一个层时不用保留临时线
			m_nClickNum = 0;

			// 保存更改的图元
			if (m_pCellDoc && g_nupdate != 2)						
			{
				CDlgDataSource *pDS = m_pCellDoc->GetDlgDataSource();
				
				if (pDS && pDS->IsModified())
				{
					CString strName = m_strName;
					int nResult = AfxMessageBox(IDS_SAVECELLEDIT, MB_YESNOCANCEL);
					if (nResult == IDYES)
						SaveEditCell(strName);
					else if (nResult == IDNO)
					{
						pDS->SetModifiedFlag(FALSE);
					}
					else if (nResult == IDCANCEL)
					{
						g_nupdate = 0;
						*pResult = 1;
						
						return;
					}
				}
			}			

			OnStaticPreview();
			
			
			if (m_pCellDoc && g_nupdate != 2)
			{
				// 将图元显示到视图中
				m_pCellDoc->UpdateCell();
			}

			if (g_nupdate == 2)
			{
				g_nupdate = -1;
			}

			m_nLastSel = pNMListView->iItem;

			// 加载引用到该图元的层
			m_comboLayers.ResetContent();
			POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
			if ( pos )
			{
				int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
				CString name = m_wndListSymbol.GetItemText(nsel,0);
				if (name.IsEmpty()) return;
				
				for (int i=m_config.pScheme->GetLayerDefineCount()-1; i>=0; i--)
				{
					CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(i);
					if (!pLayer) continue;

					int nSymbolNum = pLayer->GetSymbolCount();
					for (int j=0; j<nSymbolNum; j++)
					{
						CSymbol *pSymbol = pLayer->GetSymbol(j);
						int nType = pSymbol->GetType();
						
						CString cellName, lineName;
						switch(nType)
						{
						case SYMTYPE_CELL:
							{
								CCell *pCell = (CCell*)pSymbol;
								cellName = pCell->m_strCellDefName;
								
								break;
							}
						case SYMTYPE_CELLLINETYPE:
							{
								CCellLinetype *pCellLine = (CCellLinetype*)pSymbol;
								cellName = pCellLine->m_strCellDefName;
								
								break;
							}
						case SYMTYPE_CELLHATCH:
							{
								CCellHatch *pCellHatch = (CCellHatch*)pSymbol;
								cellName = pCellHatch->m_strCellDefName;
								
								break;
							}
						case SYMTYPE_DASHLINETYPE:
							{
								lineName = ((CDashLinetype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALELINETYPE:
							{
								lineName = ((CScaleLinetype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_LINEHATCH:
							{
								lineName = ((CLineHatch*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_DIAGONAL:
							{
								lineName = ((CDiagonal*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_PARATYPE:
							{
								lineName = ((CParaLinetype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_ANGBISECTORTYPE:
							{
								lineName = ((CAngBisectortype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALEARCTYPE:
							{
								lineName = ((CScaleLinetype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALETURNPLATETYPE:
							{
								lineName = ((CScaleTurnplatetype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALECRANETYPE:
							{
								lineName = ((CScaleCranetype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALEFUNNELTYPE:
							{
								lineName = ((CScaleFunneltype*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALE_LiangChang:
						case SYMTYPE_SCALE_YouGuan:
						case SYMTYPE_SCALE_JianFangWu:
						case SYMTYPE_SCALE_TongCheShuiZha:
						case SYMTYPE_SCALE_BuTongCheShuiZha:
						case SYMTYPE_SCALE_DiShangYaoDong:
						case SYMTYPE_SCALE_ChuanSongDai:
						case SYMTYPE_SCALE_ChuRuKou:
							{
								lineName = ((CProcSymbol_LT*)pSymbol)->m_strBaseLinetypeName;
								break;
							}
						case SYMTYPE_SCALE_CellLinetype:
							{
								cellName = ((CScaleCellLinetype*)pSymbol)->m_strCellDefName;
								break;
							}
						case SYMTYPE_TIDALWATER:
							{
								cellName = ((CTidalWaterSymbol*)pSymbol)->m_strCellDefName;
								break;
							}
						case SYMTYPE_SCALE_Cell:
							{
								cellName = ((CScaleCell*)pSymbol)->m_strCellDefName;
								break;
							}
						default:
							break;
						}
						
						if (m_nLibType == 0 && name.CompareNoCase(cellName) == 0)
						{
							m_comboLayers.AddString(pLayer->GetLayerName());
							break;
						}
						else if (m_nLibType == 1 && name.CompareNoCase(lineName) == 0)
						{
							m_comboLayers.AddString(pLayer->GetLayerName());	
							break;
						}	
					}
				}
			}

			if (m_comboLayers.GetCount() > 0)
			{
				m_comboLayers.SetCurSel(0);
			}
			
			
		}
	}

	*pResult = 0;
}


void CDlgCellDefLinetypeView::FillSymbolList()
{
	if (!m_bAddSymbol)
	{
		m_wndListSymbol.DeleteAllItems();
		m_listImages.DeleteImageList();
		m_arrIdxCreateFlag.RemoveAll();
	}

	if (m_nLibType == 0)
	{
// 		CWnd *pWnd = GetDlgItem(IDC_ADDLINE_BUTTON);
// 		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
// 
// 		pWnd = GetDlgItem(IDC_DELCELL);
// 		if( pWnd )	pWnd->ShowWindow(SW_HIDE);
		

		if( !m_config.pCellDefLib )return;
		
		int i, nSymbol = m_config.pCellDefLib->GetCellDefCount();
		
		CreateImageList(32,32,nSymbol);

		if ( m_bAddSymbol )
		{
			for (i=m_nNewCell-1; i>=0; i--)
			{
				CellDef def = m_config.pCellDefLib->GetCellDef(nSymbol-i-1);
				m_wndListSymbol.InsertItem(nSymbol-i-1,def.m_name,nSymbol-i-1);
			}
			
			m_wndListSymbol.EnsureVisible(nSymbol-m_nNewCell,FALSE);
			m_wndListSymbol.SetItemState(nSymbol-m_nNewCell, LVIS_SELECTED, LVIS_SELECTED);
			m_strName = m_config.pCellDefLib->GetCellDef(nSymbol-m_nNewCell).m_name;
		}
		else
		{
			int nSel = 0;
			for( i=0; i<nSymbol; i++)
			{
				CellDef def = m_config.pCellDefLib->GetCellDef(i);

				m_wndListSymbol.InsertItem(i,def.m_name,i);
				//			m_wndListSymbol.InsertItem(LVIF_TEXT|LVIF_IMAGE,i,def.m_name,0,LVIS_SELECTED,i,0);
				if(!m_strName.IsEmpty() && m_strName.CompareNoCase(def.m_name) == 0)
					nSel = i;
			}
			m_strName = m_wndListSymbol.GetItemText(nSel,0);

			m_wndListSymbol.EnsureVisible(nSel,FALSE);
			m_wndListSymbol.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
			

		}		

	}
	else
	{
		if( !m_config.pLinetypeLib )return;

// 		CWnd *pWnd = GetDlgItem(IDC_ADDLINE_BUTTON);
// 		if( pWnd )	pWnd->ShowWindow(SW_SHOW);
// 		
// 		pWnd = GetDlgItem(IDC_DELLINE_BUTTON);
// 		if( pWnd )
// 		{
// 			pWnd->SetWindowText(StrFromLocalResID(IDC_DELLINE));
// 			pWnd->ShowWindow(SW_SHOW);
// 		}
		
		
		int i, nSymbol = m_config.pLinetypeLib->GetBaseLineTypeCount();
		
		CreateImageList(32,32,nSymbol);

		if ( m_bAddSymbol )
		{
			BaseLineType line = m_config.pLinetypeLib->GetBaseLineType(nSymbol-1);
			m_wndListSymbol.InsertItem(nSymbol-1,line.m_name,nSymbol-1);
			m_wndListSymbol.EnsureVisible(nSymbol-1,FALSE);
			m_wndListSymbol.SetItemState(nSymbol-1, LVIS_SELECTED, LVIS_SELECTED);
			m_strName = line.m_name;
		}
		else
		{	
			int nSel = 0;
			for( i=0; i<nSymbol; i++)
			{
				BaseLineType line = m_config.pLinetypeLib->GetBaseLineType(i);

				m_wndListSymbol.InsertItem(i,line.m_name,i);
				if(m_strName.CompareNoCase(line.m_name) == 0)
					nSel = i;
			}
			m_wndListSymbol.EnsureVisible(nSel,FALSE);
			m_wndListSymbol.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
			m_strName = m_wndListSymbol.GetItemText(nSel,0);

		}
		

	}

	
}

void CDlgCellDefLinetypeView::OnChangeEditSearch() 
{

	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )return;	

	//当用户输入时才搜索
	if (GetFocus() != pWnd)
		return;
	
	CString strText, strItem, strItem2;
	pWnd->GetWindowText(strText);
	strText.Remove(' ');

	strText.MakeUpper();

	int nInLen = strlen(strText);
	if( nInLen<=0 )return;

	int nCount = m_wndListSymbol.GetItemCount();
	
	int max = -1, k = -1;
	for( int i=0; i<nCount; i++)
	{
		strItem = m_wndListSymbol.GetItemText(i,0);
		strItem.MakeUpper();

		char test[256];
		strcpy(test,strItem);
		
		//比较，找到第一个字符不同的位置
		int pos = 0;
		while( test[pos]==strText[pos] && strText[pos]!=0 )pos++;

		if (max < pos)
		{
			max = pos;
			k = i;
		}
		if (pos >= nInLen) break;
	}

	if (k >= 0)
	{
		m_wndListSymbol.EnsureVisible(k,FALSE);
		m_wndListSymbol.SetItemState(k, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	
	}
	
}

void CDlgCellDefLinetypeView::OnSetfocusEditSearch() 
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )return;
	
	CString strText;
	pWnd->GetWindowText(strText);
	if( strText==TIP_SEARCH )
	{
		pWnd->SetWindowText("");
	}
}



void CDlgCellDefLinetypeView::OnKillfocusEditSearch() 
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )return;
	
	CString strText;
	pWnd->GetWindowText(strText);
	if( strText.IsEmpty() )
	{
		pWnd->SetWindowText(TIP_SEARCH);
	}
}

void CDlgCellDefLinetypeView::CreateImageList(int cx, int cy, int nsize)
{
	if(m_bAddSymbol && m_listImages.m_hImageList!=NULL)
	{
		m_listImages.Copy(0, m_wndListSymbol.GetImageList(LVSIL_NORMAL), 0, ILCF_MOVE);
	}
	else
		m_listImages.Create(cx,cy,ILC_COLOR24,0,nsize);
	
	m_listImages.SetImageCount(nsize);
	m_wndListSymbol.SetImageList(&m_listImages,LVSIL_NORMAL);
	m_arrIdxCreateFlag.SetSize(nsize);
	
	GrBuffer2d buf;
	if(m_bAddSymbol)
	{
		DrawImageItem(nsize-1,cx,cy,&buf);
	}
	else
	{
		for( int i=0; i<nsize; i++)
		{
			DrawImageItem(i,cx,cy,&buf);
		}
	}
	
	m_nImageWid = cx;  m_nImageHei = cy;
	m_nIdxToCreateImage = 0;
	
	SetTimer(TIMERID_CREATEIMAGE,50,NULL);
//	SetTimer(TIMERID_PREVIEW,500,NULL);
/*

 	if( m_listImages.m_hImageList )
 		m_listImages.DeleteImageList();
		
	m_listImages.Create(cx,cy,ILC_COLOR24,0,nsize);
	m_listImages.SetImageCount(nsize);
	m_wndListSymbol.SetImageList(&m_listImages,TVSIL_NORMAL);
	m_arrIdxCreateFlag.SetSize(nsize);
	
	GrBuffer2d buf;
	for( int i=0; i<nsize; i++)DrawImageItem(i,cx,cy,&buf);
//	m_arrIdxCreateFlag[0] = 1;
	m_nImageWid = cx;  m_nImageHei = cy;
	m_nIdxToCreateImage = 0;
	
	SetTimer(TIMERID_CREATEIMAGE,50,NULL);
	SetTimer(TIMERID_PREVIEW,500,NULL);
*/

}

void CDlgCellDefLinetypeView::DrawSymbol(CellDef *pDef, GrBuffer2d *buf)
{	
	if( pDef->m_pgr )
	{
		buf->AddBuffer(pDef->m_pgr);
		//buf->GetEnvelope();
		buf->SetAllColor(0);
	}
}

BOOL CDlgCellDefLinetypeView::CreateImageItem(int idx)
{
	if(m_nLibType == 0)
	{
		if( !m_config.pCellDefLib )return FALSE;
		
		CString strName = m_wndListSymbol.GetItemText(idx,0);
		CellDef def = m_config.pCellDefLib->GetCellDef(strName);
		
		GrBuffer2d buf;
		DrawSymbol(&def, &buf);
		DrawImageItem(idx,m_nImageWid,m_nImageHei,&buf);

		return TRUE;

	}
	else
	{
		if( !m_config.pLinetypeLib )return FALSE;
		
		CString strName = m_wndListSymbol.GetItemText(idx,0);
// 		int index = m_config.pLinetypeLib->GetBaseLineTypeIndex(strName) + 1;
// 		
// 		GrBuffer2d buf;
// 		buf.BeginLineString(RGB(255,0,0),1,TRUE,index);
// 
// 		buf.MoveTo(&PT_2D(0,0));
// 		buf.LineTo(&PT_2D(20,0));
// 
// 		buf.End();

		GrBuffer2d buf;

		CFeature ftr;
		ftr.CreateGeometry(CLS_GEOCURVE);
		CGeometry *pGeo = ftr.GetGeometry();
		PT_3DEX pts[2];
		pts[0].x = 0;
		pts[0].y = 0;
		pts[0].pencode = penLine;
		pts[1].x = 20;
		pts[1].y = 0;
		pts[1].pencode = penLine;
		pGeo->CreateShape(pts,2);
		
		CDashLinetype dash;
		dash.m_strBaseLinetypeName = strName;
		
		GrBuffer tBuf;
		dash.Draw(&ftr,&tBuf);
		buf.AddBuffer(&tBuf);

		DrawImageItem(idx,m_nImageWid,m_nImageHei,&buf);
		
		return TRUE;

	}
	
	
}


void CDlgCellDefLinetypeView::DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf)
{
	// 获取当前选中项的图像索引，防止删除项目时图像位置偏移
	LVITEM item = {0};
	item.iItem = idx;
	item.iSubItem = 0;
	item.iImage = -1;
	item.mask = LVIF_IMAGE|LVIF_TEXT;
	m_wndListSymbol.GetItem(&item);

	int iIndexImage = item.iImage>=0?item.iImage:idx;

	//创建内存设备
	CClientDC cdc(&m_wndListSymbol);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if( !hDC )return;
	
	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC,cx,cy);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}
	
	CBitmap bm;
	bm.Attach(hBmp);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(255,255,255));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);
	
	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);
	
	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}

	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};
	
	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,0,matrix,CSize(cx,cy),CRect(0,0,cx,cy));
	
	::SelectObject(hDC,hOldBmp);
	::SelectObject(hDC,hOldPen);

	m_listImages.Replace(iIndexImage,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
/*	
	//绘图
	COLORREF clr = 0;
	int wid = -1;
	const Graph2d* pGraph2d = pBuf->HeadGraph();
	while( pGraph2d ) 
	{
		switch(pGraph2d->type)
		{
		case 1:
			{
				GrPoint2d *pPoint = (GrPoint2d*)pGraph2d;
				if(pPoint == NULL)
					continue;
				
				break;
			}
		case 2:
			{
				GrLineString2d *pLineString = (GrLineString2d*)pGraph2d;
				if(pLineString == NULL)
					continue;
				GrVertex2d *pts = pLineString->ptlist.pts;
				for( int i=0; i<pLineString->ptlist.nuse; i++)
				{
					int x =(int)((pts[i].x-e.m_xl)*scalex+rect.left+xoff+0.5);
					int y =cy-(int)((pts[i].y-e.m_yl)*scalex+rect.top+yoff+0.5);
					
					//切换钢笔
					if( clr!=pLineString->color || wid!=pLineString->width )
					{
						::SelectObject(hDC,(HPEN)hOldPen);
						clr = pLineString->color;
						wid = pLineString->width;
						pen.DeleteObject();
						
						if( pLineString->width<=1 )
							pen.CreatePen(PS_SOLID,wid,clr);
						else
						{
							LOGBRUSH lbrush = {BS_SOLID,clr,0};
							pen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_FLAT,wid,&lbrush,0,NULL);
						}
						::SelectObject(hDC,(HPEN)pen);
					}
					
					//绘制直线
					if(pts[i].code==GRBUFFER_PTCODE_MOVETO )
						::MoveToEx(hDC, x, y, NULL);
					else
						::LineTo(hDC, x, y);
				}
								
				break;
			}
		case 3:
			{
				
				GrPolygon2d *pPolygon = (GrPolygon2d*)pGraph2d;
				if(pPolygon == NULL)
					continue;
				break;
			}
		case 4:
			{
				GrText2d *pText = (GrText2d*)pGraph2d;
				if(pText == NULL)
					continue;
				break;
			}
		default:
			pGraph2d = NULL;
			continue;
		}
		
		pGraph2d = pGraph2d->next;
	}
	
	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBmp);
	pen.DeleteObject();
	
	m_listImages.Replace(idx,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
*/
}

BOOL CDlgCellDefLinetypeView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return false;
	}

	//鼠标移动消息
	if (pMsg->message ==  WM_MOUSEMOVE)   
	{
		if (pMsg->hwnd == GetDlgItem(IDC_STATIC_PREVIEW)->m_hWnd)
		{
			//获得当前鼠标位置   
			POINT pt = pMsg->pt;   
			//转换为客户坐标系   
			ScreenToClient(&pt); 
			
			SetCursor(LoadCursor(NULL,IDC_CROSS)); 
			
			if (m_nClickNum == 1)
			{
				DrawTemLine(pt, 0);
			}
		}
/*		//获得当前鼠标位置   
		POINT pt = pMsg->pt;   
		//转换为客户坐标系   
		ScreenToClient(&pt);   
		//获得当前位置的控件窗口指针   
		CWnd*   pWnd = ChildWindowFromPoint(pt);   
		//获得该窗口的ID   
		if ( pWnd && (pWnd->GetDlgCtrlID() == IDC_STATIC_PREVIEW))   
		{

			SetCursor(LoadCursor(NULL,IDC_CROSS)); 
			
			if (m_nClickNum == 1)
			{
				DrawTemLine(pt,0);	
			}

		}
*/
	}
	else if(pMsg->hwnd==GetDlgItem(IDC_STATIC_PREVIEW)->m_hWnd && pMsg->message == WM_RBUTTONDOWN)
	{
		if (m_nClickNum == 1 || m_nClickNum == 2)
		{
			m_nClickNum = 3;
			OnClickStatic();
		}
		
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgCellDefLinetypeView::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==TIMERID_CREATEIMAGE )
	{
		BOOL bKill = FALSE;
		if( !m_config.pCellDefLib && !m_config.pLinetypeLib)
			bKill = TRUE;
		else
		{
			int nsize = m_arrIdxCreateFlag.GetSize();
			
			//检查是否已经没有需要生成的图像了
			for( int i=m_nIdxToCreateImage; i<nsize+m_nIdxToCreateImage; i++)
			{
				if( m_arrIdxCreateFlag[(i%nsize)]==0 )break;
			}
			
			if( i>=nsize+m_nIdxToCreateImage )bKill = TRUE;
			else
			{
				i = (i%nsize);
				if( CreateImageItem(i) )
				{
					m_arrIdxCreateFlag[i] = 1;
					m_nIdxToCreateImage = i+1;
					
//					static int nCreateTimes = 0;
//					if( (nCreateTimes+1)%10==0 )
//						m_wndListSymbol.RedrawWindow();
//					nCreateTimes++;

					CRect rect;
					m_wndListSymbol.GetItemRect(i,&rect,LVIR_BOUNDS);
					m_wndListSymbol.RedrawWindow(&rect);

				}
			}

		
		}
		if(bKill)
			KillTimer(TIMERID_CREATEIMAGE);
		
	}
	else if (nIDEvent == TIMERID_PREVIEW)
	{
//		OnStaticPreview();
	}
// 	else 
// 	{
// 		OnStaticPreview();
// 	}
	
	CDialog::OnTimer(nIDEvent);
}


void CDlgCellDefLinetypeView::DrawPreview(LPCTSTR strSymName)
{
	if (strSymName == NULL)
	{
		CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
		if( !pWnd )  return;
		
		if (GetFocus() != pWnd)
		{
			pWnd->SetWindowText("");
		}
		
		//IDC_STATIC_PREVIEW
		pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
		if( !pWnd )return;
		CRect rcView;
		pWnd->GetClientRect(&rcView);
		CClientDC cdc(pWnd);
		int cx = rcView.Width(), cy = rcView.Height();
		CBrush br(RGB(255,255,255));
		::FillRect(cdc.GetSafeHdc(),CRect(0,0,cx,cy),(HBRUSH)br);
		return;
	}

	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if( !pWnd )  return;
	if (GetFocus() != pWnd)
	{
		pWnd->SetWindowText(strSymName);
	}

	GrBuffer2d buf;

	if(m_nLibType == 0)
	{
		if( !m_config.pCellDefLib )return;
		
		CellDef def = m_config.pCellDefLib->GetCellDef(strSymName);		
		DrawSymbol(&def,&buf);
	}
	else
	{
		if( !m_config.pLinetypeLib )return;
		
		CFeature ftr;
		ftr.CreateGeometry(CLS_GEOCURVE);
		CGeometry *pGeo = ftr.GetGeometry();
		PT_3DEX pts[2];
		pts[0].x = 0;
		pts[0].y = 0;
		pts[0].pencode = penLine;
		pts[1].x = 20;
		pts[1].y = 0;
		pts[1].pencode = penLine;
		pGeo->CreateShape(pts,2);

		CDashLinetype dash;
		dash.m_strBaseLinetypeName = strSymName;

		GrBuffer tBuf;
		dash.Draw(&ftr,&tBuf);
		buf.AddBuffer(&tBuf);

		buf.SetAllColor(0);

	}
	
	pWnd = GetDlgItem(IDC_STATIC_PREVIEW);
	if( !pWnd )return;

	CStatic* previewStatic =(CStatic*)pWnd;
	if( !previewStatic ) return;

	CRect rcView;
	pWnd->GetClientRect(&rcView);

	//创建内存设备
	CClientDC cdc(pWnd);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if( !hDC )return;	
	
	int cx = rcView.Width(), cy = rcView.Height();
	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC,cx,cy);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}

	if( m_hBmp )
		::DeleteObject(m_hBmp);
	m_hBmp = hBmp;
	

	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(255,255,255));
	::FillRect(hDC/*cdc.GetSafeHdc()*/,CRect(0,0,cx,cy),(HBRUSH)br);

	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);
	
	//计算变换系数
	CRect rect(10,10,cx-20,cy-20);
	Envelope e = buf.GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}

	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};

	// 存储变换矩阵的逆矩阵
	matrix_reverse(matrix, 3, m_transformMatrix);

	// 将客户坐标的距离转化为图元的实际坐标距离
	double v1[3] = {14, 0, 0}, r1[3];
	matrix_multiply_byvector(m_transformMatrix, 3, 3, v1, r1);
	double dis = sqrt(r1[0]*r1[0]+(r1[1]*r1[1]))/2, dis1 = 0.6*dis;

	buf.BeginLineString(RGB(0,0,255),1,FALSE,0,1,TRUE);
	buf.MoveTo(&PT_2D(-dis,0));
	buf.LineTo(&PT_2D(dis,0));
	buf.MoveTo(&PT_2D(0,-dis));
	buf.LineTo(&PT_2D(0,dis));
	buf.MoveTo(&PT_2D(-dis1,-dis1));
	buf.LineTo(&PT_2D(-dis1,dis1));
	buf.LineTo(&PT_2D(dis1,dis1));
	buf.LineTo(&PT_2D(dis1,-dis1));
	buf.LineTo(&PT_2D(-dis1,-dis1));
	buf.End();

	GDI_DrawGrBuffer2d(hDC/*cdc.GetSafeHdc()*/,&buf,FALSE,0,matrix,rcView.Size(),rcView);
	
	previewStatic->SetBitmap(m_hBmp);
	previewStatic->Invalidate();
	
// 	::BitBlt(cdc.GetSafeHdc(),rcView.left,rcView.top,
// 		rcView.Width(),rcView.Height(),
// 		hDC,0,0,SRCCOPY);
	
	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBmp);

	::DeleteDC(hDC);
//	::DeleteObject(hBmp);

	CWnd *pWndSize = GetDlgItem(IDC_STATIC_SIZE);
	if( pWndSize )
	{
		CString strSize;
		strSize.Format("%.2fmm X %.2fmm",(e.m_xh-e.m_xl),(e.m_yh-e.m_yl));
		pWndSize->SetWindowText(strSize);
	}
}

void CDlgCellDefLinetypeView::OnStaticPreview() 
{
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if ( !pos )
	{
		DrawPreview(NULL);
		return;
	}
	
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	CString name = m_wndListSymbol.GetItemText(nsel,0);

	m_strName = name;

	// 更新预览
	DrawPreview(name);
	
	// 更新控件
	if( nsel<m_arrIdxCreateFlag.GetSize() )
	{
		CreateImageItem(nsel);
		m_wndListSymbol.Update(nsel);
	}

	// 量测的临时线
	if (m_nClickNum == 1 || m_nClickNum == 2)
	{
		DrawTemLine();	
	}
}

void CDlgCellDefLinetypeView::OnSelchangeTypeCombo() 
{
	// TODO: Add your control notification handler code here

	m_nLibType = m_comboLibType.GetCurSel();
		
	CWnd *pWnd = GetDlgItem(IDC_EDIT_SEARCH);
	if(pWnd != NULL)
	{
		pWnd->SetWindowText(TIP_SEARCH);
	}

	InitInterFace();

	FillSymbolList();
	
}

void CDlgCellDefLinetypeView::OnOK() 
{
	// TODO: Add extra validation here
//	SaveCellLinetype();

	if (m_bModified)
	{
		m_config.SaveCellLine();
	}
	

	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( pos )
	{
		int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
		m_strName = m_wndListSymbol.GetItemText(nsel,0);
	}

	KillTimer(TIMERID_CREATEIMAGE);
	KillTimer(TIMERID_PREVIEW);
	CDialog::OnOK();
}

void CDlgCellDefLinetypeView::OnCancle() 
{
	// TODO: Add your control notification handler code here
//	SaveCellLinetype();

	if (m_bModified)
	{
		m_config.LoadCellLine();
// 		m_config.pCellDefLib->ReadFrom(m_strCellDefLib);
// 		m_config.pLinetypeLib->ReadFrom(m_strLinetypeLib);
	}

	CDialog::OnCancel();
}

void CDlgCellDefLinetypeView::OnAddlineButton() 
{
	// TODO: Add your control notification handler code here
	if (m_pCellDoc)
	{
		// 保存更改的图元
		CDlgDataSource *pDS = m_pCellDoc->GetDlgDataSource();
		if (pDS && pDS->IsModified())
		{
			if(IDYES == AfxMessageBox(IDS_SAVECELLEDIT,MB_YESNO|MB_ICONASTERISK))
				SaveEditCell(m_strName);
			else
				pDS->SetModifiedFlag(FALSE);
			
		}	
		
	}

	if (m_nLibType == 0 && m_config.pCellDefLib)
	{
		m_nNewCell = 0;

		CDlgNewGroup dlg;
		dlg.SetStytle(StrFromResID(IDS_NEWCELL),StrFromResID(IDS_CMDPLANE_CELLNAME));

		CString strCellName;

		CString szText,szCaption;
		szText.LoadString(IDS_CELLDEF_EXIST);
		szCaption.LoadString(IDS_ATTENTION);

		BOOL bValid = FALSE;
		do 
		{
			dlg.m_strGroupName = strCellName;
			if (dlg.DoModal() != IDOK)
				break;

			strCellName = dlg.GetGroupName();
			if (!IsCellExist(strCellName, m_config.pCellDefLib))
			{
				bValid = TRUE;
				break;
			}

		} while (IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK));

		if (!bValid)  return;

		CellDef cell;
		cell.Create();
		strcpy(cell.m_name,LPCTSTR(strCellName));

		int idx = m_config.pCellDefLib->AddCellDef(cell);	
		
		m_nNewCell++;
		
	}
	else if (m_nLibType == 1 && m_config.pLinetypeLib)
	{
		CDlgNewLinetype dlg;
		if (dlg.DoModal() != IDOK) return;
		
		BaseLineType line;
		if (!dlg.GetLineType(line))
			return;
		
		//	if(m_config.pLinetypeLib->AddBaseLineType(line) < 0)
		if (IsLineExist(line.m_name, m_config.pLinetypeLib))
		{
			AfxMessageBox(StrFromResID(IDS_REPEAT_LINENAME),MB_OK|MB_ICONASTERISK);
			return;
		}
		
		m_config.pLinetypeLib->AddBaseLineType(line);
	}
	else return;
	
	m_bAddSymbol = TRUE;

	FillSymbolList();

	m_bAddSymbol = FALSE;

	m_bModified = TRUE;


}

void CDlgCellDefLinetypeView::OnDellineButton() 
{
	// TODO: Add your control notification handler code here

// 	CImageList *ppImage = m_wndListSymbol.GetImageList(LVSIL_NORMAL);
// 	int ssize = ppImage->GetImageCount();
//  		int sssize = m_listImages.GetImageCount();


	int nSel = -1;

	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos ) 	return;
	int nIndex = m_wndListSymbol.GetNextSelectedItem(pos);
	CString strName = m_wndListSymbol.GetItemText(nIndex,0);

	if (m_nLibType == 0)
	{
		if (!m_config.pCellDefLib)
			return;

		int nSymbol = m_config.pCellDefLib->GetCellDefCount();	
		for(int i=0; i<nSymbol; i++)
		{
			CellDef def = m_config.pCellDefLib->GetCellDef(i);
			if( def.m_nSourceID!=0 )continue;
			if(strName.CompareNoCase(def.m_name) == 0)
				nSel = i;
		}

		if( nSel<0 )
			return;

		//检测线型是否在方案中用到
		BOOL bUsed = FALSE;
		int nLayerNum = m_config.pScheme->GetLayerDefineCount();
		for(i=0; i<nLayerNum; i++)
		{
			int nSymbolNum = m_config.pScheme->GetLayerDefine(i)->GetSymbolCount();
			for (int j=0; j<nSymbolNum; j++)
			{
				CSymbol *pSymbol = m_config.pScheme->GetLayerDefine(i)->GetSymbol(j);
				int nType = pSymbol->GetType();
				
				switch(nType)
				{
				case SYMTYPE_CELL:
					{
						CCell *pCell = (CCell*)pSymbol;
						if (pCell->m_strCellDefName.CompareNoCase(strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				case SYMTYPE_CELLLINETYPE:
					{
						CCellLinetype *pCellLine = (CCellLinetype*)pSymbol;
						if (pCellLine->m_strCellDefName.CompareNoCase(strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				case SYMTYPE_CELLHATCH:
					{
						CCellHatch *pCellHatch = (CCellHatch*)pSymbol;
						if (pCellHatch->m_strCellDefName.CompareNoCase(strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				default:
					break;
				}
				if (bUsed)  break;
			}
			if (bUsed)  break;
			
		}
		
		if (bUsed)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_CELL_USED);
			szCaption.LoadString(IDS_ATTENTION);
			if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) != IDYES)
				return;
			
		}

		if (nSel>=0 && nSel<nSymbol)
		{			
			m_config.pCellDefLib->DelCellDef(strName);
			
			m_wndListSymbol.DeleteItem(nIndex);
		}

	}
	else if (m_nLibType == 1)
	{
		if (!m_config.pLinetypeLib)
			return;	
		
		int nSymbol = m_config.pLinetypeLib->GetBaseLineTypeCount();		
		for( int i=0; i<nSymbol; i++)
		{
			BaseLineType line = m_config.pLinetypeLib->GetBaseLineType(i);
			if( line.m_nSourceID!=0 )continue;
			if(strName.CompareNoCase(line.m_name) == 0)
				nSel = i;
		}

		if( nSel<0 )
			return;
		
		//检测线型是否在方案中用到
		BOOL bUsed = FALSE;
		int nLayerNum = m_config.pScheme->GetLayerDefineCount();
		for (i=0; i<nLayerNum; i++)
		{
			int nSymbolNum = m_config.pScheme->GetLayerDefine(i)->GetSymbolCount();
			for (int j=0; j<nSymbolNum; j++)
			{
				CSymbol *pSymbol = m_config.pScheme->GetLayerDefine(i)->GetSymbol(j);
				int nType = pSymbol->GetType();
				switch(nType)
				{
				case SYMTYPE_DASHLINETYPE:
					{
						CDashLinetype *pDash = (CDashLinetype*)pSymbol;
						if (pDash->m_strBaseLinetypeName.CompareNoCase(strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				case SYMTYPE_SCALELINETYPE:
					{
						CScaleLinetype *pScale = (CScaleLinetype*)pSymbol;
						if (pScale->m_strBaseLinetypeName.CompareNoCase(strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				default:
					break;
				}
				if (bUsed)  break;
			}
			if (bUsed)  break;
			
		}
		
		if (bUsed)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_LINETYPE_USED);
			szCaption.LoadString(IDS_ATTENTION);
			if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) != IDYES)
				return;
			
		}

		if (nSel>=0 && nSel<nSymbol)
		{
			m_config.pLinetypeLib->DelBaseLineType(strName);
			m_wndListSymbol.DeleteItem(nIndex);
		}

	}	
	
	if (nSel > -1)
	{
		//更新画图计时器
		m_arrIdxCreateFlag.RemoveAt(nIndex);
//		m_listImages.Remove(nIndex);
//		m_wndListSymbol.SetImageList(&m_listImages,TVSIL_NORMAL);
//  		CImageList *ppImage = m_wndListSymbol.GetImageList(LVSIL_NORMAL);
//   		int ssize = ppImage->GetImageCount();
//   		int sssize = m_listImages.GetImageCount();

// 		if (nSel < m_nIdxToCreateImage)
// 			m_nIdxToCreateImage -= 1;
		
		//更新选中状态
		if (nSel == 0)
		{
			m_wndListSymbol.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
			
		}
		else if(nIndex>0 && nSel<m_wndListSymbol.GetItemCount()+1)
		{
			m_wndListSymbol.SetItemState(nIndex-1, LVIS_SELECTED, LVIS_SELECTED);
		}
		
		//更新预览
		OnStaticPreview();
		
		m_bModified = TRUE;
	}
	
	
		
}


void CDlgCellDefLinetypeView::OnDelallButton()
{
	if (m_nLibType == 0)
	{
		if (!m_config.pCellDefLib)
			return;
		
		CString szText,szCaption;
		szText.LoadString(IDS_CELL_USED);
		szCaption.LoadString(IDS_ATTENTION);
		if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) != IDYES)
			return;

		CString text;
		for( int i=m_wndListSymbol.GetItemCount()-1; i>=0; i--)
		{
			text = m_wndListSymbol.GetItemText(i,0);
			if( m_config.pCellDefLib->DelCellDef(text)>=0 )
			{
				m_wndListSymbol.DeleteItem(i);
			}
		}

		//更新预览
		OnStaticPreview();

		m_bModified = TRUE;

	}
	else if (m_nLibType == 1)
	{
		if (!m_config.pLinetypeLib)
			return;	
		
		CString szText,szCaption;
		szText.LoadString(IDS_LINETYPE_USED);
		szCaption.LoadString(IDS_ATTENTION);
		if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) != IDYES)
			return;

		CString text;
		for( int i=m_wndListSymbol.GetItemCount()-1; i>=0; i--)
		{
			text = m_wndListSymbol.GetItemText(i,0);
			if( m_config.pLinetypeLib->DelBaseLineType(text)>=0 )
			{
				m_wndListSymbol.DeleteItem(i);
			}
		}

		//更新预览
		OnStaticPreview();

		m_bModified = TRUE;

	}	
	
}

void CDlgCellDefLinetypeView::OnDblclkListSymbol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW *)pNMHDR;
		
	//判断双击位置是否在有数据的列表项上面
	int nsel = pNMListView->iItem;
	
	if (nsel<0 || nsel>=m_wndListSymbol.GetItemCount())            
	{
		*pResult = 0;
		return;
	}

	//选择模式，双击时，直接OK 返回
	if( m_bHideModifyCtrl )
	{
		EndDialog(IDOK);
		return;
	}

	if( m_nLibType!=1 || !m_config.pLinetypeLib )
		return;
// 	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
// 	if( !pos )
// 		return;
// 	
// 	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	//	CString name = m_wndListSymbol.GetItemText(nsel,0);


	BaseLineType line;
	line = m_config.pLinetypeLib->GetBaseLineType(nsel);
	CDlgNewLinetype dlg;
	dlg.SetLineType(line);
	
	if (dlg.DoModal() != IDOK && !dlg.IsModified()) 	 return;
		
	if (!dlg.GetLineType(line))  return;
	
	if(m_config.pLinetypeLib->SetBaseLineType(nsel, line) < 0)
	{
		AfxMessageBox(StrFromResID(IDS_REPEAT_LINENAME),MB_OK|MB_ICONASTERISK);
		return;
	}

	m_bModified = TRUE;

	m_wndListSymbol.SetItemText(nsel,0,line.m_name);
	
	//更新画图计时器
	m_arrIdxCreateFlag[nsel] = 0;
	if (nsel < m_nIdxToCreateImage)
		m_nIdxToCreateImage = 0;
	
	OnStaticPreview();

	*pResult = 0;
}

void CDlgCellDefLinetypeView::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_bClosing = TRUE;
	if (m_pCellDoc)
	{
		if (m_pCellDoc->GetDlgDataSource() && m_pCellDoc->GetDlgDataSource()->IsModified())
		{
			if(IDYES == AfxMessageBox(IDS_SAVECELLEDIT,MB_YESNO|MB_ICONASTERISK))
				SaveEditCell();
			else
				m_pCellDoc->GetDlgDataSource()->SetModifiedFlag(FALSE);
		}
		
		if (!m_pCellDoc->IsClosing())
		{
			m_pCellDoc->OnCloseDocument();
		}
		
	}
	SaveCellLinetype();
	KillTimer(TIMERID_CREATEIMAGE);
	KillTimer(TIMERID_PREVIEW);
	CDialog::OnClose();
}

BOOL CDlgCellDefLinetypeView::SaveEditCell(CString name)
{
	CString strName = name.IsEmpty()?m_strName:name;
	if (!m_pCellDoc) return FALSE;

	CDlgDataSource *pDS = m_pCellDoc->GetDlgDataSource();

	if (pDS && pDS->IsModified())
	{
		m_bModified = TRUE;

// 		int nResult = AfxMessageBox(IDS_SAVECELLEDIT, MB_YESNOCANCEL);
// 		if( nResult==IDYES )
		{
			CellDef def = m_config.pCellDefLib->GetCellDef(strName);
			if (def.m_pgr==NULL)
			{
				def.m_pgr = new GrBuffer2d();
				strcpy(def.m_name,LPCTSTR(strName));
			}
			def.m_pgr->DeleteAll();

			/*int nsel = 0;
			const FTR_HANDLE *handles = m_pCellDoc->GetSelection()->GetSelectedObjs(nsel);
			// 生成图形基本单位
			for (int i=0;i<nsel;i++)
			{
				CFeature *pFtr = HandleToFtr(handles[i]);
				if (!pFtr) continue;

				GrBuffer buf,buf1;

				pDS->DrawFeature(pFtr,&buf1);
				buf1.GetAllGraph(&buf);
				
				def.m_pgr->AddBuffer(&buf);

			}*/

			long scaleOld = pDS->GetScale();
			pDS->SetScale(1000);

			for (int i=0; i<pDS->GetFtrLayerCount(); i++)
			{
				CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
				if ( !pLayer || !pLayer->IsVisible() )
					continue;
				
				int nobj = pLayer->GetObjectCount();
				for (int j=0;j<nobj;j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if( !pFtr )continue;

					GrBuffer buf,buf1;
				//	pFtr->Draw(&buf);
					pDS->DrawFeature(pFtr,&buf1);
					buf1.GetAllGraph(&buf);

					const Graph *pGr = buf1.HeadGraph();
					while( pGr )
					{
						if (IsGrPolygon(pGr))
						{
							GrPolygon *pPoly = (GrPolygon*)pGr;
							if (pPoly && pPoly->bUseSelfcolor)
							{
								pPoly->bUseSelfcolor = FALSE;
							}
						}

						pGr = pGr->next;
					}

					def.m_pgr->AddBuffer(&buf);
				}
			}

			pDS->SetScale(scaleOld);
			
//			m_CellDef.m_pgr->Move(-pt.x,-pt.y);
			def.m_pgr->RefreshEnvelope();
			
			
			m_config.pCellDefLib->SetCellDef(strName, def);
			// 更新预览
			DrawPreview(strName);

			// 更新控件
			LVFINDINFO info;
			int nIndex;
			
			info.flags = LVFI_PARTIAL|LVFI_STRING;
			info.psz = (LPCTSTR)strName;
			if ((nIndex=m_wndListSymbol.FindItem(&info)) != -1)
			{
				CreateImageItem(nIndex);
				m_wndListSymbol.Update(nIndex);
			}

//			pCellLib->Save();

		}

		pDS->SetModifiedFlag(FALSE);

		return TRUE;
	}

	return FALSE;

	
}

void CDlgCellDefLinetypeView::SaveCellLinetype()
{
	
	if (m_bModified)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_SAVE_LINETYPE);
		szCaption.LoadString(IDS_ATTENTION);
		if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) == IDYES)
		{
			m_config.SaveCellLine();
		}
		else
		{
			m_config.LoadCellLine();
// 			m_config.pCellDefLib->ReadFrom(m_strCellDefLib);
// 			m_config.pLinetypeLib->ReadFrom(m_strLinetypeLib);
		}

		m_bModified = FALSE;
	}
}

//DEL void CDlgCellDefLinetypeView::OnItemclickListSymbol(NMHDR* pNMHDR, LRESULT* pResult) 
//DEL {
//DEL 	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
//DEL 	// TODO: Add your control notification handler code here
//DEL 	
//DEL 	*pResult = 0;
//DEL }

void CDlgCellDefLinetypeView::OnBeginlabeleditListSymbol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_strName = m_wndListSymbol.GetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem);

	*pResult = 0;
}

void CDlgCellDefLinetypeView::OnEndEditListSymbol(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pLVDI = (LV_DISPINFO*)pNMHDR;

	CString strModName = pLVDI->item.pszText;
	BOOL bModName = FALSE;

	// 没有重新输入名称
	if (strModName.IsEmpty())  return;

	if (m_nLibType == 0)
	{
		if (!m_config.pCellDefLib || m_strName.CompareNoCase(strModName) == 0)
		{
			return;
		}
		// 重名检测
		if (IsCellExist(strModName, m_config.pCellDefLib))
		{
			CString szText,szCaption;
			szText.LoadString(IDS_CELLDEF_EXIST);
			szCaption.LoadString(IDS_ATTENTION);
			MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
			return;	
		}

		//检测线型是否在方案中用到
		BOOL bUsed = FALSE;
		int nLayerNum = m_config.pScheme->GetLayerDefineCount();
		for (int i=0; i<nLayerNum; i++)
		{
			int nSymbolNum = m_config.pScheme->GetLayerDefine(i)->GetSymbolCount();
			for (int j=0; j<nSymbolNum; j++)
			{
				CSymbol *pSymbol = m_config.pScheme->GetLayerDefine(i)->GetSymbol(j);
				int nType = pSymbol->GetType();
				
				switch(nType)
				{
				case SYMTYPE_CELL:
					{
						CCell *pCell = (CCell*)pSymbol;
						if (pCell->m_strCellDefName.CompareNoCase(m_strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				case SYMTYPE_CELLLINETYPE:
					{
						CCellLinetype *pCellLine = (CCellLinetype*)pSymbol;
						if (pCellLine->m_strCellDefName.CompareNoCase(m_strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				case SYMTYPE_CELLHATCH:
					{
						CCellHatch *pCellHatch = (CCellHatch*)pSymbol;
						if (pCellHatch->m_strCellDefName.CompareNoCase(m_strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				default:
					break;
				}
				if (bUsed)  break;
			}
			if (bUsed)  break;
			
		}
		
		if (bUsed)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_CELL_USEDMODIFY);
			szCaption.LoadString(IDS_ATTENTION);
			if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) != IDYES)
				return;			
			
		}

		CellDef def = m_config.pCellDefLib->GetCellDef(m_strName);
		strcpy(def.m_name, strModName);
		// 如果修改图元库中图元成功则更改所有使用该图元的层
		if (m_config.pCellDefLib->SetCellDef(m_strName, def))
		{
			bModName = TRUE;
			m_bModified = TRUE;
			m_wndListSymbol.SetItemText(pLVDI->item.iItem,pLVDI->item.iSubItem,strModName);
			if (bUsed)
			{
				for (i=0; i<nLayerNum; i++)
				{
					int nSymbolNum = m_config.pScheme->GetLayerDefine(i)->GetSymbolCount();
					for (int j=0; j<nSymbolNum; j++)
					{
						CSymbol *pSymbol = m_config.pScheme->GetLayerDefine(i)->GetSymbol(j);
						int nType = pSymbol->GetType();
						
						switch(nType)
						{
						case SYMTYPE_CELL:
							{
								CCell *pCell = (CCell*)pSymbol;
								if (pCell->m_strCellDefName.CompareNoCase(m_strName) == 0)
									pCell->m_strCellDefName = strModName;
								
								break;
							}
						case SYMTYPE_CELLLINETYPE:
							{
								CCellLinetype *pCellLine = (CCellLinetype*)pSymbol;
								if (pCellLine->m_strCellDefName.CompareNoCase(m_strName) == 0)
									pCellLine->m_strCellDefName = strModName;
								
								break;
							}
						case SYMTYPE_CELLHATCH:
							{
								CCellHatch *pCellHatch = (CCellHatch*)pSymbol;
								if (pCellHatch->m_strCellDefName.CompareNoCase(m_strName) == 0)
									pCellHatch->m_strCellDefName = strModName;
								
								break;
							}
						default:
							break;
						}
					}
					
				}


			}
		}

	}
	else if (m_nLibType == 1)
	{
		if (!m_config.pLinetypeLib || m_strName.CompareNoCase(strModName) == 0)
			return;	
		
		// 重名检测
		if (IsLineExist(strModName, m_config.pLinetypeLib))
		{
			CString szText,szCaption;
			szText.LoadString(IDS_LINETYPE_EXIST);
			szCaption.LoadString(IDS_ATTENTION);
			MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
			return;	
		}

		//检测线型是否在方案中用到
		BOOL bUsed = FALSE;
		int nLayerNum = m_config.pScheme->GetLayerDefineCount();
		for (int i=0; i<nLayerNum; i++)
		{
			int nSymbolNum = m_config.pScheme->GetLayerDefine(i)->GetSymbolCount();
			for (int j=0; j<nSymbolNum; j++)
			{
				CSymbol *pSymbol = m_config.pScheme->GetLayerDefine(i)->GetSymbol(j);
				int nType = pSymbol->GetType();
				switch(nType)
				{
				case SYMTYPE_DASHLINETYPE:
					{
						CDashLinetype *pDash = (CDashLinetype*)pSymbol;
						if (pDash->m_strBaseLinetypeName.CompareNoCase(m_strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				case SYMTYPE_SCALELINETYPE:
					{
						CScaleLinetype *pScale = (CScaleLinetype*)pSymbol;
						if (pScale->m_strBaseLinetypeName.CompareNoCase(m_strName) == 0)
							bUsed = TRUE;
						
						break;
					}
				default:
					break;
				}
				if (bUsed)  break;
			}
			if (bUsed)  break;
			
		}
		
		if (bUsed)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_LINETYPE_USEDMODIFY);
			szCaption.LoadString(IDS_ATTENTION);
			if (MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) != IDYES)
				return;
			
		}

		BaseLineType line = m_config.pLinetypeLib->GetBaseLineType(m_strName);
		strcpy(line.m_name, strModName);
		if(m_config.pLinetypeLib->SetBaseLineType(m_strName, line))
		{
			bModName = TRUE;
			m_bModified = TRUE;
			m_wndListSymbol.SetItemText(pLVDI->item.iItem,pLVDI->item.iSubItem,strModName);
			if (bUsed)
			{
				for (i=0; i<nLayerNum; i++)
				{
					int nSymbolNum = m_config.pScheme->GetLayerDefine(i)->GetSymbolCount();
					for (int j=0; j<nSymbolNum; j++)
					{
						CSymbol *pSymbol = m_config.pScheme->GetLayerDefine(i)->GetSymbol(j);
						int nType = pSymbol->GetType();
						switch(nType)
						{
						case SYMTYPE_DASHLINETYPE:
							{
								CDashLinetype *pDash = (CDashLinetype*)pSymbol;
								if (pDash->m_strBaseLinetypeName.CompareNoCase(m_strName) == 0)
									pDash->m_strBaseLinetypeName = strModName;
								
								break;
							}
						case SYMTYPE_SCALELINETYPE:
							{
								CScaleLinetype *pScale = (CScaleLinetype*)pSymbol;
								if (pScale->m_strBaseLinetypeName.CompareNoCase(m_strName) == 0)
									pScale->m_strBaseLinetypeName = strModName;
								
								break;
							}
						default:
							break;
						}
					}
					
				}
			}
			
		}

		


		

	}

	if (bModName)  m_strName = strModName;
	
	*pResult = 0;
	
}

//DEL void CDlgCellDefLinetypeView::OnMouseMove(UINT nFlags, CPoint point) 
//DEL {
//DEL 	// TODO: Add your message handler code here and/or call default
//DEL 	
//DEL 	CDialog::OnMouseMove(nFlags, point);
//DEL }

void CDlgCellDefLinetypeView::OnClickStatic() 
{
	// TODO: Add your control notification handler code here
	m_nClickNum++;

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	
	if (m_nClickNum == 1 || m_nClickNum == 3)
	{
		if (m_nClickNum == 3)
		{
			DrawTemLine();
		}
		
		m_ptStart = pt;
		m_ptCur = pt;
		
		m_nClickNum = 1;
		
	}
	else if (m_nClickNum == 2);   // 利用保留的线
	else if (m_nClickNum == 4)    // 供右键使用
	{
		DrawTemLine();
		m_nClickNum = 0;
	}	
	
	
}

void CDlgCellDefLinetypeView::DrawTemLine(CPoint pt, int type)
{
	
	CClientDC dc(this); 
	
	CPen pen;
	pen.CreatePen(PS_SOLID,1,RGB(128,128,128));
	HPEN hOldPen = (HPEN)::SelectObject(dc,(HPEN)pen);
	
	int nOldDrawMode = dc.SetROP2(R2_XORPEN); 
	
	if (type == 0)
	{
		//擦除原先直线 
		dc.MoveTo(m_ptStart); 
		dc.LineTo(m_ptCur); 
		
		m_ptCur = pt; 
		
	}
	
	//绘制新直线 或 擦除原先直线 
	dc.MoveTo(m_ptStart); 
	dc.LineTo(m_ptCur);		
	
	dc.SetROP2(nOldDrawMode);
	dc.SelectObject(hOldPen);
	
	// 将客户坐标的距离转化为图元的实际坐标距离
	double v1[3] = {m_ptStart.x, m_ptStart.y, 0};
	double v2[3] = {m_ptCur.x, m_ptCur.y, 0};
	double r1[3], r2[3];
	matrix_multiply_byvector(m_transformMatrix, 3, 3, v1, r1);
	matrix_multiply_byvector(m_transformMatrix, 3, 3, v2, r2);
	double dis = sqrt((r2[0]-r1[0])*(r2[0]-r1[0])+(r2[1]-r1[1])*(r2[1]-r1[1]));//GGetDisOf2P2D(r1[0],r1[1],r2[0],r2[1]);
	
	CWnd *pWndSize = GetDlgItem(IDC_DISTANCE_STATIC);
	if( pWndSize )
	{
		CString strSize;
		if (m_nClickNum == 0 || m_nClickNum == 4)
		{
			strSize = "0mm";
		}
		else
			strSize.Format("%.2fmm",dis);
		pWndSize->SetWindowText(strSize);
	}
}

void BatchDxf(CString path, CStringArray &dxf)
{
	if(path.Right(4).CompareNoCase(_T(".dxf")) == 0)
	{
		dxf.Add(path);
		return;
	}
	//检查路径是否有效
	BOOL bValid = FALSE;
	
	CFileFind finder;
	BOOL   bWorking = finder.FindFile( LPCTSTR((CString)path+"\\*.*") );
    while(bWorking)   
    {   
        bWorking = finder.FindNextFile(); 
        if(finder.IsDots() || finder.IsDirectory()) 
			continue;
		
		CString strfilename = finder.GetFilePath();
		if(strfilename.Right(4).CompareNoCase(_T(".dxf")) == 0)
		{
			dxf.Add(strfilename);
		}
        
	}
	finder.Close();
}

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] 设置风格

void CDlgCellDefLinetypeView::OnImportcellfromdxfButton() 
{
	m_nNewCell = 0;
	// TODO: Add your control notification handler code here
	char retPath[_MAX_PATH]={0};
	UINT ulFlags = BIF_BROWSEINCLUDEFILES;
	if( !BrowseFolderEx(StrFromResID(IDS_SYMLIB_DIR),retPath,NULL,m_hWnd,FALSE,ulFlags) )return;
	
	// 设置圆弧精度
	float fPrecision = CLinearizer::m_fPrecision;	
	CLinearizer::m_fPrecision = 0.005;

	CString strPathName(retPath);

	CStringArray arrDxf;
	BatchDxf(strPathName,arrDxf);

	CCellDefLib *pCellLib = GetCellDefLib();
	for (int i=0; i<arrDxf.GetSize(); i++)
	{
		CDlgDataSource data(NULL);
		CDxfRead dxfRead;
		
		dxfRead.OnImportDxf(&data,TRUE,arrDxf.GetAt(i));

		CString strCellName = arrDxf.GetAt(i);
		int pos = strCellName.ReverseFind(_T('\\'));
		int pos1 = strCellName.ReverseFind(_T('.'));
		
		strCellName = strCellName.Mid(pos+1,pos1-pos-1);
		if (IsCellExist(strCellName))
			continue;		
		
		// 创建单元
		CellDef cell = pCellLib->GetCellDef(strCellName);
		if (cell.m_pgr==NULL)
		{
			cell.m_pgr = new GrBuffer2d();
			strcpy(cell.m_name,LPCTSTR(strCellName));
		}
		cell.m_pgr->DeleteAll();

		for(int j=0; j<data.GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = data.GetFtrLayerByIndex(j);
			if (!pLayer) continue;
			
			for(int k=0; k<pLayer->GetObjectCount(); k++)
			{				
				CFeature *pFtr = pLayer->GetObject(k);
				if (!pFtr) continue;

				GrBuffer buf;
				pFtr->Draw(&buf);

				cell.m_pgr->AddBuffer(&buf);
			}
		}

//		cell.m_pgr->Move(-pt.x,-pt.y);
//		cell.m_pgr->SetOrigin(0,0);
		cell.m_pgr->RefreshEnvelope();
		
		// 将生成的单元加入符号库
		int idx = pCellLib->AddCellDef(cell);	
		
		m_nNewCell++;

	}

	if (m_nNewCell > 0)
	{
		m_bModified = TRUE;
	}

//	pCellLib->Save();

	m_bAddSymbol = TRUE;
	FillSymbolList();
	m_bAddSymbol = FALSE;

	CLinearizer::m_fPrecision = fPrecision;
	
	AfxMessageBox(IDS_IMPORTCADSYMBOLS_OK);
	
}

void CDlgCellDefLinetypeView::OnExportcelltodxfButton() 
{
	m_nNewCell = 0;
	// TODO: Add your control notification handler code here
	char retPath[_MAX_PATH]={0};
	UINT ulFlags = BIF_BROWSEINCLUDEFILES;
	if( !BrowseFolderEx(StrFromResID(IDS_SYMLIB_DIR),retPath,NULL,m_hWnd,FALSE,ulFlags) )return;
	
	// 设置圆弧精度
	float fPrecision = CLinearizer::m_fPrecision;	
	CLinearizer::m_fPrecision = 0.005;
	
	CString strPathName(retPath);
	
	CCellDefLib *pCellLib = GetCellDefLib();
	for (int i=0; i<pCellLib->GetCellDefCount(); i++)
	{
		CellDef cell = pCellLib->GetCellDef(i);
		CString dxfPath;
		dxfPath.Format("%s\\%s.dxf",strPathName,cell.m_name);

		CDxfWrite dxfWrite;
		CDlgDataSource data(NULL);
		dxfWrite.SetDlgDataSource(&data);
		dxfWrite.GrBuffer2dToDxf(cell.m_pgr,dxfPath);		
	}
	
	CLinearizer::m_fPrecision = fPrecision;
	
	AfxMessageBox(IDS_IMPORTOK);
	
}

void CDlgCellDefLinetypeView::OnSelchangeScaleCombo()
{
	if (m_pCellDoc)
	{
		// 保存更改的图元
		CDlgDataSource *pDS = m_pCellDoc->GetDlgDataSource();
		if (pDS && pDS->IsModified())
		{
			if(IDYES == AfxMessageBox(IDS_SAVECELLEDIT,MB_YESNO|MB_ICONASTERISK))
				SaveEditCell(m_strName);
			else
				pDS->SetModifiedFlag(FALSE);
					
		}	
		
	}

	// 若有修改，先保存
	SaveCellLinetype();
	
	int nComboIndex = m_scaleCombo.GetCurSel();
	if (nComboIndex == CB_ERR) return;
	//	int nConfigIndex = m_scaleCombo.GetItemData(nComboIndex);
	CString strScale;
	m_scaleCombo.GetLBText(nComboIndex,strScale);
	m_config = gpCfgLibMan->GetConfigLibItemByScale(atoi(strScale));

	m_scaleSwitch.SwitchToScale(m_config.GetScale());

	KillTimer(TIMERID_CREATEIMAGE);
	KillTimer(TIMERID_PREVIEW);
	
	m_strName.Empty();

	FillSymbolList();

	if (m_pCellDoc)
	{
		m_pCellDoc->InitDoc(m_config.GetScale());
		// 将图元显示到视图中
		m_pCellDoc->UpdateCell();
	}
}


void CDlgCellDefLinetypeView::OnItemchangingListSymbol(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	// 避免更新两次的情况
	if (pNMListView->uChanged == LVIF_STATE)
	{
		if (pNMListView->uNewState&LVIS_SELECTED)
		{
			if (g_nupdate >= 0 && g_nupdate!=2)
			{
				if (++g_nupdate == 1)
				{
					g_nupdate = 2;
					m_wndListSymbol.EnsureVisible(m_nLastSel,FALSE);
 					m_wndListSymbol.SetItemState(m_nLastSel, LVIS_SELECTED, LVIS_SELECTED);
					
					*pResult = 1;
					return;
				}
			}
			
		}
	}

	*pResult = 0;
}

