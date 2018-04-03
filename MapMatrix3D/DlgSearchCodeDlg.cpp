// DlgSearchCodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSearchCodeDlg.h"
#include "SmartViewFunctions.h"
#include "CollectionTreeCtrl.h"
#include "GrBuffer2d.h"
#include "Matrix.h"
#include "GeoText.h"
#include "GeoParallel.h"
#include "CollectionViewBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//extern void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol);

/////////////////////////////////////////////////////////////////////////////
// CDlgSearchCodeDlg dialog


CDlgSearchCodeDlg::CDlgSearchCodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSearchCodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSearchCodeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pScheme = NULL;
}


void CDlgSearchCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSearchCodeDlg)
	DDX_Control(pDX, IDC_PREVIEW_LIST, m_wndListSymbol);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSearchCodeDlg, CDialog)
	//{{AFX_MSG_MAP(CDlgSearchCodeDlg)
		// NOTE: the ClassWizard will add message map macros here
		ON_WM_TIMER()
		ON_NOTIFY(TVN_SELCHANGED,IDC_COLLECTVIEW_TREE,OnSelChanged)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_PREVIEW_LIST, OnSelchangeSymbolList)
		ON_NOTIFY(NM_DBLCLK, IDC_PREVIEW_LIST, OnDClickSymbolList)
		ON_NOTIFY(NM_CUSTOMDRAW, IDC_PREVIEW_LIST, OnCustomDrawList)
	//}}AFX_MSG_MAP
		ON_WM_CTLCOLOR()
		ON_WM_LBUTTONDOWN()
		ON_WM_DRAWITEM()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSearchCodeDlg message handlers
BOOL CDlgSearchCodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	CreateCtrls();
	FillTree();
	
	CRect rect;
	m_wndListSymbol.GetClientRect(&rect);
	m_wndListSymbol.SetIconSpacing(rect.Width()/4-5,128);
	return TRUE;
}

BOOL CDlgSearchCodeDlg::CreateCtrls() 
{

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | 
								TVS_HASLINES | WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN;

	if (!m_wndIdxTree.Create (dwViewStyle, rectDummy, this, IDC_COLLECTVIEW_TREE))
	{
		TRACE0("Failed to create Class View\n");
		return FALSE;      // fail to create
	}

	CRect rectClient;
	GetClientRect (rectClient);

	m_wndIdxTree.SetWindowPos (NULL, 
		rectClient.left, 
		rectClient.top ,
		100, 
		rectClient.Height(),
		SWP_NOACTIVATE|SWP_NOZORDER);

	return TRUE;
}

void CDlgSearchCodeDlg::SetScheme(CScheme *pSchme, int nScale)
{
	m_pScheme = pSchme;
	m_nScale = nScale;
}

BOOL CDlgSearchCodeDlg::FillTree()
{
	if (m_pScheme == NULL)
		return FALSE;
	// 将树信息存到USERIDX中
	m_UserIdx.Clear();

	m_wndIdxTree.DeleteAllItems();

	for (int i=0; i<m_pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		LAYGROUP group;
		strcpy(group.GroupName,m_pScheme->m_strSortedGroupName.GetAt(i));
		m_UserIdx.m_aGroup.Add(group);
	}

	for(i=0; i<m_pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *layer = m_pScheme->GetLayerDefine(i);

		for (int j=0; j<m_UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(m_UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
				idx.code = layer->GetLayerCode();
				strcpy(idx.FeatureName,layer->GetLayerName());
				strcpy(idx.strAccel,layer->GetAccel());
				idx.groupidx = j;

				m_UserIdx.m_aIdx.Add(idx);
				break;
			}
		}

	}

	//填充树
	char strR[_MAX_FNAME];
	int  recentidx = -1;
	CArray<HTREEITEM,HTREEITEM> aRootItem;

	m_wndIdxTree.InsertRoot(StrFromResID(IDS_ALL),0,0);
	m_wndIdxTree.InsertRoot(StrFromResID(IDS_USED_RECENT),0,0);

	for (i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
	{
		LAYGROUP gp;
		gp = m_UserIdx.m_aGroup.GetAt(i);

		if (stricmp(gp.GroupName,StrFromResID(IDS_SPECIALGROUP)) == 0x00)
		{
			continue;
		}

		if (stricmp(gp.GroupName,"Recent") == 0x00)
			recentidx = i;
		
		if (recentidx != i)
		{
			HTREEITEM hRoot = m_wndIdxTree.InsertRoot(_T(gp.GroupName),0,GROUPID_FROMIDX(i));
			aRootItem.Add(hRoot);
		}
		
	}

	if (aRootItem.GetSize() > 0)
	{
		m_wndIdxTree.SelectItem(aRootItem[0]);
	}

/*	for (i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
	{
		IDX idx;
		idx = m_UserIdx.m_aIdx.GetAt(i);
		
		if (idx.groupidx != recentidx)
		{
			if (strlen(idx.strAccel) <= 0)
			{
				sprintf(strR,"%d %s",idx.code,idx.FeatureName);
			}
			else
			{
				sprintf(strR,"%d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
			}
			
			// 对存储在recent组下的组，应在添加组时把新组加到recent组上面，避免出现问题(老版本格式)
			if (recentidx != -1 && idx.groupidx > recentidx)
			{
				m_wndIdxTree.InsertSubItem(_T(strR),aRootItem.GetAt(idx.groupidx-1),0,CODEID_FROMIDX(i));
			}
			else
			{
				HTREEITEM item = m_wndIdxTree.InsertSubItem(_T(strR),aRootItem.GetAt(idx.groupidx),0,CODEID_FROMIDX(i));
				// 若有预先选定的层则选中
				if (m_nLayerIndex >= 0)
				{			
					if (m_pScheme->GetLayerDefine(m_nLayerIndex)->GetLayerName().CompareNoCase(idx.FeatureName) == 0)
					{		
						m_wndIdxTree.SelectItem(item);
						//保存当前所选项目
						m_hCurItem = item;
					}
				}
			}
		}

	}
*/	
	m_wndIdxTree.SetItemHeight(22);
	
	m_wndIdxTree.RedrawWindow();

//	m_BackupUserIdx = m_UserIdx;

	return TRUE;

}

void CDlgSearchCodeDlg::OnDClickSymbolList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )return;
	
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	CSchemeLayerDefine *pLayer = (CSchemeLayerDefine*)m_wndListSymbol.GetItemData(nsel);
	
	if (!pLayer) return;
	
	m_strSelLayer = pLayer->GetLayerName();
	
	EndDialog(IDOK);
}

void CDlgSearchCodeDlg::OnCustomDrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast <NMLVCUSTOMDRAW*> ( pNMHDR );
    *pResult = 0; 
	
    if (CDDS_PREPAINT  == pLVCD->nmcd.dwDrawStage) 
	{ 
		*pResult = CDRF_NOTIFYITEMDRAW; 
	} 
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage) 
	{  
		int  nItem = static_cast<int>(pLVCD->nmcd.dwItemSpec); 
		
		CRect  rcList, rcItemBound; 
		m_wndListSymbol.GetClientRect(rcList);
		m_wndListSymbol.GetItemRect(nItem, rcItemBound, LVIR_BOUNDS);
		if (!rcList.PtInRect(rcItemBound.TopLeft()) && !rcList.PtInRect(rcItemBound.BottomRight()))
		{
			*pResult = CDRF_SKIPDEFAULT; 
			return;
		}
		
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc); 
		COLORREF crBkgnd; 		 
		CRect  rcItem; 
        BOOL bListHasFocus = (m_wndListSymbol.GetSafeHwnd() == ::GetFocus());

		LVITEM rItem;
		ZeroMemory(&rItem, sizeof(LVITEM)); 
		rItem.mask  = LVIF_IMAGE|LVIF_STATE; 
		rItem.iItem = nItem; 
		rItem.stateMask = LVIS_SELECTED|LVIS_FOCUSED; 
		m_wndListSymbol.GetItem (&rItem);
		m_wndListSymbol.GetItemRect (nItem, &rcItem, LVIR_ICON);
		UINT uFormat = ILD_TRANSPARENT;
        if ( (rItem.state&LVIS_SELECTED) && bListHasFocus ) 
			uFormat |= ILD_FOCUS;
        m_listImages.Draw(pDC, rItem.iImage, rcItem.TopLeft(), uFormat);
		m_wndListSymbol.GetItemRect(nItem, rcItem, LVIR_LABEL); 
		
        if (rItem.state&LVIS_SELECTED) 
		{ 
			if (bListHasFocus) 
			{ 
				crBkgnd = GetSysColor(COLOR_HIGHLIGHT); 
				pDC->SetTextColor(GetSysColor (COLOR_HIGHLIGHTTEXT)); 
			} 
			else 
			{ 
				crBkgnd = GetSysColor(COLOR_BTNFACE); 
				pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT)); 
			} 
		} 
		else 
		{ 
			crBkgnd = GetSysColor(COLOR_WINDOW); 
			pDC->SetTextColor(GetSysColor (COLOR_BTNTEXT)); 
		}
		
		CRect rcText = rcItem;
		rcText.bottom += 20;

		pDC->FillSolidRect(rcText, crBkgnd); 
		pDC->SetBkMode(TRANSPARENT);		
		 
		CString sText = m_wndListSymbol.GetItemText(nItem, 0);
        pDC->DrawText(sText, rcText, DT_CENTER);
		
		if (bListHasFocus && (rItem.state&LVIS_FOCUSED)) 
		{ 
			pDC->DrawFocusRect(rcText); 
		}

        *pResult = CDRF_SKIPDEFAULT;  
	}

}

void CDlgSearchCodeDlg::OnSelchangeSymbolList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )return;
	
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	// 更新控件
	if( nsel<m_arrIdxCreateFlag.GetSize() && m_arrIdxCreateFlag[nsel] == 0)
	{
		CreateImageItem(nsel);
		m_wndListSymbol.Update(nsel);
		m_arrIdxCreateFlag[nsel] = 1;
	}

	*pResult = 0;
}

void CDlgSearchCodeDlg::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0; 
	NMTREEVIEW *pTree = (NMTREEVIEW*)pNMHDR;
	
	HTREEITEM hItem = pTree->itemNew.hItem;	

	if( !hItem )
	{
		m_strGroupName.Empty();
		return;
	}

	m_strGroupName = m_wndIdxTree.GetItemText(hItem);

	FillSymbolList();


}

void CDlgSearchCodeDlg::FillSymbolList()
{
	m_wndListSymbol.DeleteAllItems();
	m_listImages.DeleteImageList();
	m_arrIdxCreateFlag.RemoveAll();

	if (!m_pScheme) return;

	int i, nLayerNum = m_pScheme->GetLayerDefineCount();

	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> arr;
	for (i=0; i<nLayerNum; i++)
	{
		CSchemeLayerDefine *pLayer = m_pScheme->GetLayerDefine(i);
		if (!pLayer)  continue;

		if (m_strGroupName.CompareNoCase(StrFromResID(IDS_ALL)) == 0 || 
			m_strGroupName.CompareNoCase(pLayer->GetGroupName()) == 0)
		{
			arr.Add(pLayer);
		} 
	}

	if (m_strGroupName.CompareNoCase(StrFromResID(IDS_USED_RECENT)) == 0)
	{
		USERIDX *pUserIdx = m_pScheme->GetRecentIdx();
		if (pUserIdx)
		{
			for (int j=0; j<pUserIdx->m_aIdx.GetSize(); j++)
			{
				for (i=0; i<nLayerNum; i++)
				{
					CSchemeLayerDefine *pLayer = m_pScheme->GetLayerDefine(i);
					if (!pLayer)  continue;

					if (pLayer->GetLayerName().CompareNoCase(pUserIdx->m_aIdx.GetAt(j).FeatureName) == 0)
					{
						arr.Add(pLayer);
						break;
					}
				}
				
			}
		}
		
	}
	
	CreateImageList(64,64,arr.GetSize());

//	m_wndListSymbol.SetFont();
	
	for( i=0; i<arr.GetSize(); i++)
	{
		CSchemeLayerDefine *pLayer = arr.GetAt(i);
		if (!pLayer)  continue;
		
		CString item;
		item.Format("%I64d",pLayer->GetLayerCode());
		item += "\n";
		CString accel = pLayer->GetAccel();
		if (accel.IsEmpty())
		{
			item += "-";
		}
		else
		{
			item += accel;
		}
		
		item += "\n";
		item += pLayer->GetLayerName();
		
		m_wndListSymbol.InsertItem(i,item,i);
		m_wndListSymbol.SetItemData(i, DWORD_PTR(pLayer));
		
	}
	
// 	m_wndListSymbol.EnsureVisible(0,FALSE);
// 	m_wndListSymbol.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}

void CDlgSearchCodeDlg::PreviewGroup(CString groupName)
{

}

void CDlgSearchCodeDlg::CreateImageList(int cx, int cy, int nsize)
{
	m_listImages.Create(cx,cy,ILC_COLOR24,0,nsize);
	
	m_listImages.SetImageCount(nsize);
	m_wndListSymbol.SetImageList(&m_listImages,LVSIL_NORMAL);
	m_arrIdxCreateFlag.SetSize(nsize);
	
	GrBuffer2d buf;
	for( int i=0; i<nsize; i++)
	{
		DrawImageItem(i,cx,cy,&buf);
	}
	
	m_nImageWid = cx;  m_nImageHei = cy;
	m_nIdxToCreateImage = 0;
	
	SetTimer(TIMERID_CREATEIMAGE,50,NULL);


}

void CDlgSearchCodeDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==TIMERID_CREATEIMAGE )
	{
		static CRect s_rect(0,0,0,0);

		static int nCreateTimes = 0;

		BOOL bKill = FALSE;
		
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
					CRect rect;
					m_wndListSymbol.GetItemRect(i,&rect,LVIR_BOUNDS);
					s_rect.UnionRect(&rect,&s_rect);

					m_arrIdxCreateFlag[i] = 1;
					m_nIdxToCreateImage = i+1;					
					
					nCreateTimes++;
					if( (nCreateTimes)%8==0 )
					{
						m_wndListSymbol.RedrawWindow(&s_rect,NULL,RDW_INTERNALPAINT|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
						s_rect.SetRectEmpty();
						nCreateTimes = 0;
					}
//					m_wndListSymbol.Update(i);
				}
			}
			
			
		}
		if(bKill)
		{
			m_wndListSymbol.RedrawWindow(&s_rect);
			s_rect.SetRectEmpty();
			nCreateTimes = 0;
			//m_wndListSymbol.RedrawWindow();
			KillTimer(TIMERID_CREATEIMAGE);
		}
		
	}
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CDlgSearchCodeDlg::CreateImageItem(int idx)
{
	if( !m_pScheme )return FALSE;
 	
	CSchemeLayerDefine *pLayer = (CSchemeLayerDefine*)m_wndListSymbol.GetItemData(idx);
	if (!pLayer) return FALSE;

 	int nSymbolNum = pLayer->GetSymbolCount();

	GrBuffer bufs;
	
	CFeature *pFeature = pLayer->CreateDefaultFeature();
	if (!pFeature)  return FALSE;

	CGeometry *pGeo = pFeature->GetGeometry();

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
		pts[0].y = 0;
		pts[0].pencode = penLine;
		pts[1].x = 20;
		pts[1].y = 0;
		pts[1].pencode = penLine;
		pGeo->CreateShape(pts,2);

		CGeoParallel *pParallele = (CGeoParallel*)pGeo;
		pParallele->SetWidth(10);

	}
	else if(nGeoClass == CLS_GEOCURVE)
	{
		// 检查是否有颜色或图元填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = pLayer->GetSymbol(i);
			int nType = pSym->GetType();
			if(nType==SYMTYPE_CELLHATCH || nType==SYMTYPE_COLORHATCH)
			{
				bHatch = TRUE;
				break;
			}
		}
		if (bHatch)
		{
			PT_3DEX pts[4];
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
			pGeo->CreateShape(pts,4);

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
		pts[0].y = 0;
		pts[0].pencode = penMove;
		pts[1].x = 100;
		pts[1].y = 0;
		pts[1].pencode = penLine;
		pts[2].x = 100;
		pts[2].y = 100;
		pts[2].pencode = penMove;
		pts[3].x = 0;
		pts[3].y = 100;
		pts[3].pencode = penLine;
		pGeo->CreateShape(pts,4);
	}
	else if(nGeoClass == CLS_GEOSURFACE)
	{
		PT_3DEX pts[5];
		pts[0].x = 0;
		pts[0].y = 0;
		pts[0].pencode = penLine;
		pts[1].x = 100;
		pts[1].y = 0;
		pts[1].pencode = penLine;
		pts[2].x = 100;
		pts[2].y = 100;
		pts[2].pencode = penLine;
		pts[3].x = 0;
		pts[3].y = 100;
		pts[3].pencode = penLine;
		pts[4].x = 0;
		pts[4].y = 0;
		pts[4].pencode = penLine;
		pGeo->CreateShape(pts,5);

	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		pGeo->CreateShape(&PT_3DEX(0,0,0,penMove),1);
		((CGeoText*)pGeo)->SetText("abc");
	}

	// 设置当前比例尺图元线型库
	CSwitchScale scale(m_nScale);
	
	for(int i=0; i<nSymbolNum; i++)
	{
		CSymbol *pSym = pLayer->GetSymbol(i);
		// 若为注记
		if (pSym->GetType() == SYMTYPE_ANNOTATION)
		{
			CValueTable tab;
			tab.BeginAddValueItem();
			if (pFeature)
			{		
				pFeature->WriteTo(tab);
			}
			pLayer->GetXDefaultValues(tab);			
			tab.EndAddValueItem();

			((CAnnotation*)pSym)->Draw(pFeature,&bufs,tab,(float)m_nScale/1000);
		}
		else
			pSym->Draw(pFeature,&bufs,(float)m_nScale/1000);
		
	}
	
	if (nSymbolNum == 0)
	{
		pFeature->Draw(&bufs);
	}

	COLORREF colBack = RGB(255,255,255);
	int nClassType = pLayer->GetGeoClass();
	if (nClassType == CLS_GEOPOINT || nClassType == CLS_GEODIRPOINT || nClassType == CLS_GEOTEXT)
	{
		colBack = RGB(0xFF,0xC1,0xC1);
	}
	else if (nClassType == CLS_GEOCURVE || nClassType == CLS_GEOPARALLEL || nClassType == CLS_GEODCURVE)
	{
		colBack = RGB(0xC1,0xFF,0xC1);
	}
	else if (nClassType == CLS_GEOSURFACE)
	{
		colBack = RGB(0xCA,0xE1,0xFF);
	}

	GrBuffer2d buf;
	buf.AddBuffer(&bufs);
	DrawImageItem(idx,m_nImageWid,m_nImageHei,&buf,pLayer->GetColor(),colBack);
	
	return TRUE;
}

void CDlgSearchCodeDlg::DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf, COLORREF col, COLORREF colBak)
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
	
	CBrush br(RGB(128,128,128));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);
//	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC,(HBRUSH)br);
	
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
	
	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,col,matrix,CSize(cx,cy),CRect(0,0,cx,cy),colBak);
	
	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBmp);
	
	m_listImages.Replace(iIndexImage,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
}

HBRUSH CDlgSearchCodeDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetBkColor(RGB(50, 50, 50));
	pDC->SetTextColor(RGB(255, 255, 255));
	// TODO:  Change any attributes of the DC here
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	return m_brush;
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CDlgSearchCodeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnLButtonDown(nFlags, point);

	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}


void CDlgSearchCodeDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
}
