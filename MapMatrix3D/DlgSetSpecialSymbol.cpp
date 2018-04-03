// DlgSetSpecialSymbol.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgSetSpecialSymbol.h"
#include "Matrix.h"
#include "CollectionViewBar.h"
#include "GeoText.h"
#include "editbasedoc.h"
#include "DlgDataSource.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgSetSpecialSymbol dialog

extern CDocument *GetCurDocument();
CDlgSetSpecialSymbol::CDlgSetSpecialSymbol(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSetSpecialSymbol::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetSpecialSymbol)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nCurSchemeScale = 500;
	m_bSpecialLayer = FALSE;
	m_bMultiSelected = FALSE;
}


void CDlgSetSpecialSymbol::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetSpecialSymbol)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_TREE_SPECIALSYMBOL, m_wndIdxTree);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDlgSetSpecialSymbol, CDialog)
	//{{AFX_MSG_MAP(CDlgSetSpecialSymbol)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SPECIALSYMBOL, OnSelChanged)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetSpecialSymbol message handlers

BOOL CDlgSetSpecialSymbol::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (!CreatCtrls())
	return FALSE;
	CDlgDoc *pDoc =(CDlgDoc*)GetCurDocument();
	m_nCurSchemeScale = pDoc->GetDlgDataSource()->GetScale();
	FillTree();
	m_StrSelect = _T("");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgSetSpecialSymbol::CreatCtrls()
{
 	CRect  rectReview;
	CWnd *pWnd = GetDlgItem(IDC_STATIC_SHOW);
	if( !pWnd )return FALSE;
	CRect rcView;
	pWnd->GetWindowRect(&rectReview);
	return TRUE;
}

BOOL CDlgSetSpecialSymbol::FillTree()
{
	m_config = gpCfgLibMan->GetConfigLibItemByScale(m_nCurSchemeScale);

	if (m_config.pScheme == NULL)
		return FALSE;
	m_wndIdxTree.DeleteAllItems();
	m_UserIdx.Clear();
	for (int i=0; i<m_config.pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		LAYGROUP group;
		strcpy(group.GroupName,m_config.pScheme->m_strSortedGroupName.GetAt(i));
		if (stricmp(group.GroupName,StrFromResID(IDS_SPECIALGROUP)) == 0x00)
		m_UserIdx.m_aGroup.Add(group);//添加组名
	}

	int nSpecialCount = m_config.pScheme->GetLayerDefineCount(TRUE);

	for(i=0; i<m_config.pScheme->GetLayerDefineCount(TRUE); i++)
	{
		CSchemeLayerDefine *layer = m_config.pScheme->GetLayerDefine(i,TRUE);
		
		for (int j=0; j<m_UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(m_UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
				//	idx.index = i;
				idx.code = layer->GetLayerCode();
				strcpy(idx.FeatureName,layer->GetLayerName());
				strcpy(idx.strAccel,layer->GetAccel());
				idx.groupidx = j;
				
				m_UserIdx.m_aIdx.Add(idx);//添加层名
				break;
			}
		}
		
	}
	
	//填充树
	char strR[_MAX_FNAME];
	int  recentidx = -1, specialidx = -1;
	HTREEITEM hRoot;
	CArray<HTREEITEM,HTREEITEM> m_aRootItem;
	for (i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
	{
		LAYGROUP gp;
		gp = m_UserIdx.m_aGroup.GetAt(i);
		
		 hRoot = m_wndIdxTree.InsertRoot(_T(gp.GroupName),0,GROUPID_FROMIDX(i));
		m_aRootItem.Add(hRoot);	
	}

	for (i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
	{
		IDX idx;
		idx = m_UserIdx.m_aIdx.GetAt(i);
		
		if (idx.groupidx != recentidx)
		{
			if (idx.groupidx == specialidx)
			{
				if (strlen(idx.strAccel) <= 0)
				{
					sprintf(strR,"%s",idx.FeatureName);
				}
				else
				{
					sprintf(strR,"%s^%s",idx.FeatureName,idx.strAccel);
				}
			}
			else
			{
				if (strlen(idx.strAccel) <= 0)
				{
					sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
				}
				else
				{
					sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
				}
			}
		
			
			// 对存储在recent组下的组，应在添加组时把新组加到recent组上面，避免出现问题(老版本格式)
			if (recentidx != -1 && idx.groupidx > recentidx)
			{
				m_wndIdxTree.InsertSubItem(_T(strR),m_aRootItem.GetAt(idx.groupidx-1),0,CODEID_FROMIDX(i));
			}
			else
			{
				HTREEITEM item = m_wndIdxTree.InsertSubItem(_T(strR),m_aRootItem.GetAt(idx.groupidx),0,CODEID_FROMIDX(i));
				//hcw,2012.7.30,modified, 若有预先选定的层则选中
				if ((m_nLayerIndex >= 0)&&(m_nLayerIndex <= nSpecialCount))
				{	
					CSchemeLayerDefine* pLayerDefine = NULL;
					pLayerDefine = m_config.pScheme->GetLayerDefine(m_nLayerIndex,TRUE);
					if (!pLayerDefine)
					{
						continue;
					}
					if (m_config.pScheme->GetLayerDefine(m_nLayerIndex,TRUE)->GetLayerName().CompareNoCase(idx.FeatureName) == 0)
					{		
					//	m_wndIdxTree.SelectItem(item);
						//保存当前所选项目
						m_hCurItem = item;
					}
				}
			}
		}

	}
	m_wndIdxTree.Expand(hRoot,TVE_EXPAND);
	m_wndIdxTree.SetItemHeight(18);	
	m_wndIdxTree.RedrawWindow();
	m_wndIdxTree.SelectItem(m_hCurItem);
	m_BackupUserIdx = m_UserIdx;
	return TRUE;
}




void CDlgSetSpecialSymbol::GetCurAttribute(CValueTable &tab)
{
 	// 获取固有属性和扩展属性
 	CValueTable xtab;
 	CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer);
 	if (pLayer)
 	{
 		tab.BeginAddValueItem();
 		// 固有属性
 		CFeature *pFtr = pLayer->CreateDefaultFeature();
 		if (pFtr)
 		{		
 			pFtr->WriteTo(tab);
 			delete pFtr;
 		}
 		
 		// 扩展属性
 		pLayer->GetXDefaultValues(tab);
 		
 		tab.EndAddValueItem();
 		
		//		tab.AddItemFromTab(xtab);
 	}
 }


void CDlgSetSpecialSymbol::DrawPreview(int nLayerIndex)
{
	if (!m_config.pCellDefLib || nLayerIndex<0)
	{
	
		//{hcw,2012.7.26,方案路径不存在时直接退出。
		if (!m_config.pCellDefLib)
		{
			m_bAbsentSchemePath = TRUE;
		}
		else
		{
			m_bAbsentSchemePath = FALSE;
		}
		//}
		
		GrBuffer2d buf2d;
		double matrix[9];
		matrix_toIdentity(matrix,3);
		
		CWnd *pWnd = GetDlgItem(IDC_STATIC_SHOW);
		if( !pWnd )return;
		CRect rcView;
		pWnd->GetClientRect(&rcView);
		
		CClientDC cdc(pWnd);
		GDI_DrawGrBuffer2d(cdc.GetSafeHdc(),&buf2d,TRUE,RGB(255,255,255),matrix,rcView.Size(),rcView);
		
		return;
	}
	m_bAbsentSchemePath = FALSE;
	if(m_config.pScheme->GetLayerDefineCount(TRUE)<=m_nLayerIndex)
	{
		m_nLayerIndex = -1;
		return;
	}
	CSchemeLayerDefine *pLayDef = m_config.pScheme->GetLayerDefine(nLayerIndex,TRUE);
	
	int nSymbolNum = pLayDef->GetSymbolCount();
	
	GrBuffer bufs;
	
	CFeature *pFeature = pLayDef->CreateDefaultFeature();
	if (!pFeature)  return;
	
	CGeometry *pGeo = pFeature->GetGeometry();
	pGeo->SetColor(pLayDef->GetColor());
	if( pGeo->GetColor()==RGB(255,255,255) )
		pGeo->SetColor(0);
		int nGeoClass = pGeo->GetClassType();

	if (nGeoClass==CLS_GEOPOINT || nGeoClass==CLS_GEODIRPOINT)
	{
		pGeo->CreateShape(&PT_3DEX(0,0,0,penMove),1);
		pGeo->Draw(&bufs);
	}
	else if (nGeoClass == CLS_GEOPARALLEL)
	{
		// 平行线有填充时填充在基线和辅助线之间
		PT_3DEX pts[2];
		pts[0].x = 0;
		pts[0].y = 10;
		pts[0].pencode = penLine;
		pts[1].x = 20;
		pts[1].y = 10;
		pts[1].pencode = penLine;
		pGeo->CreateShape(pts,2);

		CGeoParallel *pParallele = (CGeoParallel*)pGeo;
		pParallele->SetWidth(-10);

	}
	else if(nGeoClass == CLS_GEOCURVE)
	{
		// 检查是否有颜色，图元填充，晕线填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		// 检查是否有依比例转盘，装卸漏斗（圆形）
		BOOL bArc = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = m_config.pScheme->GetLayerDefine(nLayerIndex,TRUE)->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALETURNPLATETYPE || nType==SYMTYPE_SCALE_LiangChang || nType==SYMTYPE_SCALE_YouGuan )
			{
				bArc = TRUE;
			}
			else if (nType == SYMTYPE_SCALEFUNNELTYPE && (((CScaleFunneltype*)pSym)->m_nFunnelType == 1 || ((CScaleFunneltype*)pSym)->m_nFunnelType == 3))
			{
				bArc = TRUE;
			}

			if( nType==SYMTYPE_CELLHATCH || nType==SYMTYPE_COLORHATCH || nType==SYMTYPE_LINEHATCH || nType == SYMTYPE_DIAGONAL || nType == SYMTYPE_SCALE_DiShangYaoDong ||
				nType==SYMTYPE_SCALE_JianFangWu || nType==SYMTYPE_ANGBISECTORTYPE  )
			{
				bHatch = TRUE;
				break;
			}
		}

		if (bArc)
		{
			PT_3DEX pts[4];
			pts[0].x = 0;
			pts[0].y = 10;
			pts[0].pencode = penArc;
			pts[1].x = 10;
			pts[1].y = 20;
			pts[1].pencode = penArc;
			pts[2].x = 20;
			pts[2].y = 10;
			pts[2].pencode = penArc;
			pts[3].x = 0;
			pts[3].y = 10;
			pts[3].pencode = penArc;
			pGeo->CreateShape(pts,4);
		}
		else if (bHatch)
		{
			PT_3DEX pts[5];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 0;
			pts[1].y = 40;
			pts[1].pencode = penLine;
			pts[2].x = 40;
			pts[2].y = 40;
			pts[2].pencode = penLine;
			pts[3].x = 40;
			pts[3].y = 0;
			pts[3].pencode = penLine;
 			pts[4].x = 0;
 			pts[4].y = 0;
 			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);

		}
		else
		{
			PT_3DEX pts[2];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 20;
			pts[1].y = 0;
			pts[1].pencode = penLine;
			pGeo->CreateShape(pts,2);

		}	
		
		
	}
	else if (nGeoClass == CLS_GEODCURVE)
	{
		PT_3DEX pts[5];
		pts[0].x = 0;
		pts[0].y = 10;
		pts[0].pencode = penMove;
		pts[1].x = 20;
		pts[1].y = 10;
		pts[1].pencode = penLine;
		pts[2].x = 0;
		pts[2].y = 0;
		pts[2].pencode = penMove;
		pts[3].x = 20;
		pts[3].y = 0;
		pts[3].pencode = penLine;
		pGeo->CreateShape(pts,4);
	}
	else if(nGeoClass == CLS_GEOSURFACE)
	{
		// 检查是否有依比例装卸漏斗（圆形）
		BOOL bArc = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = m_config.pScheme->GetLayerDefine(nLayerIndex,TRUE)->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALEFUNNELTYPE && ((CScaleFunneltype*)pSym)->m_nFunnelType == 1)
			{
				bArc = TRUE;
			}
		}

		if (bArc)
		{
			PT_3DEX pts[4];
			pts[0].x = 0;
			pts[0].y = 10;
			pts[0].pencode = penArc;
			pts[1].x = 10;
			pts[1].y = 20;
			pts[1].pencode = penArc;
			pts[2].x = 20;
			pts[2].y = 10;
			pts[2].pencode = penArc;
			pts[3].x = 0;
			pts[3].y = 10;
			pts[3].pencode = penArc;
			pGeo->CreateShape(pts,4);
		}
		else
		{
			PT_3DEX pts[5];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 40;
			pts[1].y = 0;
			pts[1].pencode = penLine;
			pts[2].x = 40;
			pts[2].y = 40;
			pts[2].pencode = penLine;
			pts[3].x = 0;
			pts[3].y = 40;
			pts[3].pencode = penLine;
			pts[4].x = 0;
			pts[4].y = 0;
			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);
		}

	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		pGeo->CreateShape(&PT_3DEX(0,0,0,penMove),1);
		((CGeoText*)pGeo)->SetText("abc");
	}

	double matrix_t[16];
	Matrix44FromZoom(1.0,1.0,1.0,matrix_t);
	pGeo->Transform(matrix_t);

//	pFeature->SetGeometry(pGeo);

	// 设置当前比例尺图元线型库
	CDlgDoc *pDoc =(CDlgDoc*)GetCurDocument();
	int nScale = pDoc->GetDlgDataSource()->GetScale();
	CSwitchScale scale(nScale);

	for(int i=0; i<nSymbolNum; i++)
	{
		CSymbol *pSym = m_config.pScheme->GetLayerDefine(nLayerIndex,TRUE)->GetSymbol(i);
		// 若为注记
		if (pSym->GetType() == SYMTYPE_ANNOTATION)
		{
			CValueTable tab;
			GetCurAttribute(tab);
			((CAnnotation*)pSym)->Draw(pFeature,&bufs,tab/*,(float)atoi(nScale)/1000*/);
		}
		else
		{
			// 图元填充过密预览调整
			if (pSym->GetType() == SYMTYPE_CELLHATCH)
			{
				Envelope e = pFeature->GetGeometry()->GetEnvelope();
				CCellHatch *pCellHath = (CCellHatch*)pSym;
				if ((fabs(pCellHath->m_fdx) > 1e-4 && e.Width()/pCellHath->m_fdx > 80) && 
					(fabs(pCellHath->m_fdy) > 1e-4 && e.Height()/pCellHath->m_fdx > 80))
				{
					CCellHatch hatch;
					hatch.CopyFrom(pSym);
					hatch.m_fdx = e.Width()/80;
					hatch.m_fdy = e.Height()/80;
					hatch.Draw(pFeature,&bufs);
					continue;
				}
			}
			
			pSym->Draw(pFeature,&bufs/*,(float)atoi(nScale)/1000*/);
		}

	}

	if (nSymbolNum == 0)
	{
		pFeature->Draw(&bufs);
	}

	CWnd *pWnd = GetDlgItem(IDC_STATIC_SHOW);
	if( !pWnd )return;
	CRect rcView;
	pWnd->GetClientRect(&rcView);

	//创建内存设备
	CClientDC cdc(pWnd);

	int cx = rcView.Width(), cy = rcView.Height();
		
	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = bufs.GetEnvelope();
	
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
//	matrix_reverse(matrix, 3, m_transformMatrix);

	GrBuffer2d buf2d;
	buf2d.AddBuffer(&bufs);
	buf2d.RefreshEnvelope(FALSE);
	
	GDI_DrawGrBuffer2d(cdc.GetSafeHdc(),&buf2d,FALSE,0,matrix,rcView.Size(),rcView);
	
	if(pFeature)
		delete pFeature;
	

}

void CDlgSetSpecialSymbol::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;


	NMTREEVIEW *pTree = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	//保存当前所选项目
	m_hCurItem = hItem;
	m_bMultiSelected = FALSE;
	if( !m_hCurItem )return;
	ClearCtrls();//全部清空
	DWORD_PTR data = m_wndIdxTree.GetItemData(m_hCurItem);
	if( data>=0xffff )
	{
		m_nLayerIndex = -1; 
		return;
	}

	HTREEITEM parentItem = m_wndIdxTree.GetParentItem(m_hCurItem);
	CString groupName = m_wndIdxTree.GetItemText(parentItem);

	CString strNameAndCode = m_wndIdxTree.GetItemText(m_hCurItem);
	char layName[_MAX_FNAME];
	int num;
	if (m_bSpecialLayer)
	{
		sscanf(strNameAndCode,"%s",layName);
	}
	else
	{
		sscanf(strNameAndCode,"%i %s",&num,layName);
	}
	m_StrLayName.Format("%s",layName);
	m_StrSelect = m_StrLayName;
	CString strAccel;		
	char *pdest = strstr(layName,"^");//寻找"^"位置
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}
	

	m_nLayerIndex = m_config.pScheme->GetLayerDefineIndex(layName,TRUE);

	if (m_nLayerIndex == -1)
	{
		return;
	}

	//判断是否支持该几何类型
/*	int nGeoType = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->GetGeoClass();
	BOOL  bSupport = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->IsSupportGeoClass(nGeoType);
	if (!bSupport)
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORT));
		return;
	}
*/
	// 重新选择一个层时不用保留临时线


	m_wndIdxTree.Invalidate(FALSE);
	m_wndIdxTree.UpdateData(TRUE);
	int nCount = m_wndIdxTree.GetSelectedCount();
	BOOL bMulti = m_wndIdxTree.m_bRealMulti;
    if (bMulti) 
    {	
		m_wndIdxTree.m_bRealMulti = FALSE; 
		return ;
    }
	DrawPreview(m_nLayerIndex);
	UpdateData(FALSE);
}

void CDlgSetSpecialSymbol::ClearCtrls()
{
	COLORREF bkColor = GetSysColor(COLOR_3DFACE);
	CRect rect;
	
	CWnd *pWnd = GetDlgItem(IDC_STATIC_SHOW);
	CClientDC cdcPreview(pWnd);
	pWnd->GetClientRect(&rect);
	cdcPreview.FillRect(&rect,&CBrush(bkColor));
	m_StrSelect = _T("");
	UpdateData(FALSE);
}

CString CDlgSetSpecialSymbol::GetSymbolName()
{
	return m_StrSelect;
}
