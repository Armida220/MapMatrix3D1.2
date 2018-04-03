// CollectionViewBar.cpp: implementation of the CCollectionViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "CollectionViewBar.h"
#include "GeoParallel.h"
#include "ExMessage.h"
#include "DrawingContext.h"
#include "GrElementList.h"
#include "GeoText.h"
#include "DlgScheme.h"
#include "GlobalFunc.h"
#include "DlgSearchCodeDlg.h"
#include "DlgDataSource.h"
#include "CMyChiLetter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define GROUPID_FROMIDX(x)		((x<<16)|0xffff)
#define GROUPID_TOIDX(x)		(x>>16)
#define CODEID_FROMIDX(x)		(x)
#define CODEID_TOIDX(x)			(x)

#define IDC_LIST_NODENAME              51514

extern CDocument *GetCurDocument();

void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol)
{
	if( clr==RGB(255,255,255) )
		clr = 0;

	HDC hMemDC = ::CreateCompatibleDC(hdc);
	HBITMAP hMemBmp = Create24BitDIB(szDC.cx, szDC.cy, TRUE,/*backCol*/0xff);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC,hMemBmp);
	
	C2DGDIDrawingContext dc;
	if (bUseClr)dc.EnableMonocolor(TRUE, clr);
	dc.CreateContext(hMemDC);
	dc.SetDCSize(szDC);
	dc.SetViewRect(rcView);

	::FillRect(hMemDC, &CRect(0, 0, szDC.cx, szDC.cy), CBrush(backCol));
	
	CCoordSys cs;
	cs.Create33Matrix(m);
	dc.SetCoordSys(&cs);
	dc.SetBackColor(RGB(220,220,220));  //用浅灰色的背景色，能更好的显示压盖效果
	
	dc.BeginDrawing();	
	dc.DrawGrBuffer2d(&GrElementList((void*)pBuf),CDrawingContext::modeNormal);
	dc.EndDrawing();

	::BitBlt(hdc,0,0,szDC.cx,szDC.cy,hMemDC,0,0,SRCCOPY);

	::SelectObject(hMemDC,hOldBmp);
	::DeleteObject(hMemBmp);
	::DeleteDC(hMemDC);
}

/*
//##ModelId=41466B7F029F
class CCollectionViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CCollectionViewBar;

	DECLARE_SERIAL(CCollectionViewMenuButton)

public:
	//##ModelId=41466B7F02AF
	CCollectionViewMenuButton(HMENU hMenu = NULL) :
		CMFCToolBarMenuButton ((UINT)-1, hMenu, -1)
	{
	}

	//##ModelId=41466B7F02B1
	virtual void OnDraw (CDC* pDC, const CRect& rect, CUIFToolBarImages* pImages,
						BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
						BOOL bHighlight = FALSE,
						BOOL bDrawBorder = TRUE,
						BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages ();

		CUIFDrawState ds;
		pImages->PrepareDrawImage (ds);

		CMFCToolBarMenuButton::OnDraw (pDC, rect, 
			pImages, bHorz, 
			bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage (ds);
	}
};

IMPLEMENT_SERIAL(CCollectionViewMenuButton, CMFCToolBarMenuButton, 1)
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=41466B7F00CC
CCollectionViewBar::CCollectionViewBar()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
//	m_pData = NULL;
	m_bChgFromSelect = FALSE;
	m_bCanSelectDefault = FALSE;
	m_bClosedBeforePopup = FALSE;
	m_bAutoHideBeforePopup= FALSE;
	m_nIdxToCreateImage = -1;
	m_nImageWid = m_nImageHei = 0;
	m_bUpDown = FALSE;
	m_bReturn = FALSE;
	m_bCommonUse = AfxGetApp()->GetProfileInt("Config","CollectionViewBarCommon", FALSE);
}

CCollectionViewBar::~CCollectionViewBar()
{
// 	if (m_pData)
// 	{
// 		delete m_pData;
// 		m_pData = NULL;
// 	}
}

BEGIN_MESSAGE_MAP(CCollectionViewBar, CDockablePane)
	//{{AFX_MSG_MAP(CCollectionViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_COLLECTVIEW_EDIT, OnChangeEditFcode)
	ON_NOTIFY(NM_DBLCLK,IDC_COLLECTVIEW_TREE,OnDblclk)
	ON_NOTIFY(TVN_SELCHANGED,IDC_COLLECTVIEW_TREE,OnSelChanged)
	ON_WM_TIMER()
	ON_COMMAND(IDS_SYMBOL_CONFIG, OnSymbolConfig)
	ON_BN_CLICKED(IDC_LAYERPREVIEW_BUTTON, OnPreview)
	ON_LBN_SELCHANGE(IDC_LIST_NODENAME, OnListSelChanged)
	ON_WM_NCPAINT()
	ON_COMMAND(IDS_MODIFY_LAYER, OnModifyLayer)
	ON_COMMAND(IDS_MODIFY_LAYER1, OnModifyLayer1)
	ON_COMMAND(ID_SWITCH_TO_COMMON, OnSwitchCommon)
	ON_COMMAND(ID_SWITCH_TO_ALL, OnSwitchCommon)
	ON_COMMAND(ID_ADD_TO_COMMON, OnAddToCommon)
	ON_COMMAND(ID_DELETE_FROM_COMMON, OnDeleteFromCommon)
//	ON_NOTIFY(NM_CLICK,IDC_COLLECTVIEW_TREE,OnPreview)
	//}}AFX_MSG_MAP
// 	ON_COMMAND_RANGE(ID_LAYERITEM_CREATE, ID_LAYERITEM_MODIFY, OnLayerEdit)
// 	ON_COMMAND_RANGE(ID_LAYERITEM_DELETEANNOT, ID_LAYERITEM_DELETEANNOT, OnLayerEdit)
// 	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYERITEM_CREATE, ID_LAYERITEM_MODIFY, OnUpdateLayerEdit)
// 	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYERITEM_DELETEANNOT, ID_LAYERITEM_DELETEANNOT, OnUpdateLayerEdit)
// 	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYERITEM_MODIFYANNOT, ID_LAYERITEM_MODIFYANNOT, OnUpdateLayerEdit)
// 	ON_COMMAND(ID_LAYERITEM_CREATEANNOT, OnCreateAnnot)
// 	ON_COMMAND(ID_LAYERITEM_MODIFYANNOT, OnCreateAnnot)
// 	ON_UPDATE_COMMAND_UI(ID_LAYERITEM_CREATEANNOT, OnUpdateLayerEdit)
// 	ON_UPDATE_COMMAND_UI(ID_LAYERITEM_MODIFYANNOT, OnUpdateLayerEdit)
// 	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
// 	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
// 	ON_COMMAND_RANGE(ID_LAYERITEM_CREATE2, ID_LAYERITEM_DELETE2, OnLayerEdit2)
// 	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYERITEM_CREATE2, ID_LAYERITEM_DELETE2, OnUpdateLayerEdit2)
	ON_WM_NCCALCSIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCollectionViewBar message handlers

//##ModelId=41466B7F0119
int CCollectionViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if( !m_wndCodeEdit.Create(WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL|WS_BORDER,
		rectDummy,this,IDC_COLLECTVIEW_EDIT) )
	{
		TRACE0("Failed to Code Edit\n");
		return -1;      // fail to create
	}

	if (!m_wndSearchRsltList.Create(WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_BORDER | WS_HSCROLL | LBS_OWNERDRAWVARIABLE, rectDummy, this, IDC_LIST_NODENAME))
	{
		TRACE0("Failed to Create List\n");
		return -1;
	}

	m_wndSearchRsltList.ShowWindow(SW_HIDE);

	if( !m_wndLayerPreview.Create("",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_FLAT,
		rectDummy,this,IDC_LAYERPREVIEW_BUTTON) )
	{
		TRACE0("Failed to Button\n");
		return -1;      // fail to create
	}

	// Create views:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | 
								TVS_HASLINES | WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN;
	
	if (!m_wndCollectionView.Create (dwViewStyle, rectDummy, this, IDC_COLLECTVIEW_TREE))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	m_wndSearchRsltList.SetFont(m_wndCollectionView.GetFont(),TRUE);

	return 0;
}

//##ModelId=41466B7F0127
void CCollectionViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

HTREEITEM CCollectionViewBar::InsertRoot(LPCTSTR lpszItem, DWORD_PTR data)
{
	HTREEITEM hRoot = m_wndCollectionView.InsertItem (lpszItem);
	m_wndCollectionView.SetItemState (hRoot, TVIS_BOLD, TVIS_BOLD);
	m_wndCollectionView.SetItemData (hRoot, data);
	SetItemImage(hRoot, data);
	return hRoot;
}


HTREEITEM CCollectionViewBar::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData)
{
	HTREEITEM hItem = m_wndCollectionView.InsertItem (lpszItem, hParent);
	m_wndCollectionView.SetItemData(hItem, dwData);
	SetItemImage(hItem, dwData);
	
	return hItem;
}

void CCollectionViewBar::SetItemImage(HTREEITEM item, DWORD_PTR flag)
{
	if (!item)return;
	if (flag >= 0xffff)
		m_wndCollectionView.SetItemImage(item, 0, 0);
	else
		m_wndCollectionView.SetItemImage(item, 1 + CODEID_TOIDX(flag), 1 + CODEID_TOIDX(flag));
}


void CCollectionViewBar::CreateImageList(int cx, int cy, int nsize)
{
	if( m_CollectionViewImages.m_hImageList )
		return;

//	if( !m_pData )return;

	m_CollectionViewImages.Create(cx, cy, ILC_COLOR24, 0, nsize + 1);
	m_CollectionViewImages.SetImageCount(nsize + 1);
	m_wndCollectionView.SetImageList(&m_CollectionViewImages, TVSIL_NORMAL);
	m_arrIdxCreateFlag.SetSize(nsize + 1);

	GrBuffer2d buf;
	for (int i = 0; i < nsize + 1; i++)DrawImageItem(i, cx, cy, &buf);
	m_arrIdxCreateFlag[0] = 1;
	m_nImageWid = cx;  m_nImageHei = cy;
	m_nIdxToCreateImage = 1;

	SetTimer(TIMERID_CREATEIMAGE, 200, NULL);
}


BOOL CCollectionViewBar::IsValid()
{
// 	CDlgDoc *pDoc = NULL;
// 	CDocument *doc = GetCurDocument();
// 	if( !doc )return FALSE;
// 	if( doc->IsKindOf(RUNTIME_CLASS(CTDoc)) )
// 		pDoc = (CDlgDoc*)doc;
// 	
// 	if( !pDoc )return FALSE;
// 	if( m_pData!=NULL && pDoc->m_pDataSource!=NULL && m_pData!=pDoc->m_pDataSource &&
// 		m_arrIdxCreateFlag.GetSize()==(pDoc->m_pDataSource->m_UserIdx.m_aIdx.GetSize()+1) )
// 	{
// 		m_pData = pDoc->m_pDataSource;
// 		return TRUE;
// 	}
// 	if( m_pData!=pDoc->m_pDataSource )
// 		return FALSE;
	return TRUE;
}


BOOL CCollectionViewBar::CreateImageItem(int idx)
{
	int cx = m_nImageWid, cy = m_nImageHei;
	CSchemeLayerDefine *pLayerDefine = NULL;
	if(m_bCommonUse)
	{
		pLayerDefine = m_config.pScheme->GetLayerDefine(m_UserIdx.m_aIdx[idx-1].FeatureName);
	}
	else
	{
		pLayerDefine = m_config.pScheme->GetLayerDefine(idx-1);
	}

	if( pLayerDefine==NULL )
		return FALSE;

	int nSymbolNum = pLayerDefine->GetSymbolCount();

	GrBuffer bufs;
	CFeature cFtr;
//	CGeometry *pGeo = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->CreateDefaultGeometry();
	
	int nGeoClass = pLayerDefine->GetGeoClass();
	
	if (!cFtr.CreateGeometry(nGeoClass))
	{
//		AfxMessageBox("地物几何类型不支持!");
		return FALSE;
	}

	CGeometry *pGeo = cFtr.GetGeometry();

	if (nGeoClass==CLS_GEOPOINT || nGeoClass==CLS_GEODIRPOINT)
	{
		pGeo->CreateShape(&PT_3DEX(),1);
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
	else if (nGeoClass == CLS_GEODCURVE)
	{
		PT_3DEX pts[5];
		pts[0].x = 0;
		pts[0].y = 10;
		pts[0].pencode = penMove;
		pts[1].x = 20;
		pts[1].y = 10;
		pts[1].pencode = penLine;
		pts[2].x = 20;
		pts[2].y = 0;
		pts[2].pencode = penMove;
		pts[3].x = 0;
		pts[3].y = 0;
		pts[3].pencode = penLine;
		pGeo->CreateShape(pts,4);
	}
	else if(nGeoClass == CLS_GEOCURVE)
	{
		//检查是否有颜色，图元填充，晕线填充,有则添加四个点方便填充
		BOOL bHatch = FALSE;
		BOOL bScaleTurnPlate = FALSE;
		for(int i=0; i<nSymbolNum; i++)
		{
			CSymbol *pSym = pLayerDefine->GetSymbol(i);
			int nType = pSym->GetType();
			if (nType == SYMTYPE_SCALETURNPLATETYPE)
			{
				bScaleTurnPlate = TRUE;
			}
			if(nType==SYMTYPE_CELLHATCH || nType==SYMTYPE_COLORHATCH || nType==SYMTYPE_LINEHATCH)
			{
				bHatch = TRUE;
				break;
			}
		}

		if (bScaleTurnPlate)
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
		else if(bHatch)
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
	else if(nGeoClass == CLS_GEOSURFACE)
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
	else if (nGeoClass == CLS_GEOTEXT)
	{
		pGeo->CreateShape(&PT_3DEX(0,0,0,penMove),1);
		((CGeoText*)pGeo)->SetText("abc");
	}

	for(int i=0; i<nSymbolNum; i++)
	{
		CSymbol *pSym = pLayerDefine->GetSymbol(i);

		if (pSym->GetType() == SYMTYPE_CELLHATCH)
		{
			GrBuffer tbuf;
			cFtr.Draw(&tbuf);

			Envelope e = tbuf.GetEnvelope();
			PT_3D startpt((e.m_xh+e.m_xl)/4, (e.m_yh+e.m_yl)/4, 0);			
			double xoff = (e.m_xh-e.m_xl)/4, yoff = (e.m_yh-e.m_yl)/4;
			
			long col = cFtr.GetGeometry()->GetColor();
			PT_3D pt;
			for (int xnum=0; xnum<3; xnum++)
			{
				pt.x = startpt.x + xnum*xoff;
				for (int ynum=0; ynum<3; ynum++)
				{
					pt.y = startpt.y + ynum*yoff;
					tbuf.Point(col,&pt,1,1);
				}
			}

			bufs.AddBuffer(&tbuf);

		}
		else
			pSym->Draw(&cFtr,&bufs);

	}

	if (nSymbolNum < 1)
	{
		cFtr.Draw(&bufs);
	}

	GrBuffer2d pGr;
	pGr.AddBuffer(&bufs);
	DrawImageItem(idx,cx,cy,&pGr);

	return TRUE;
}


void CCollectionViewBar::DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf)
{
	//创建内存设备
	CClientDC cdc(&m_wndCollectionView);
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
	
	CBrush br(RGB(0,0,150));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);

	CPen pen;
	pen.CreatePen(PS_SOLID,0, RGB(0,150,0));
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);

	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
/*
	if(scalex > 20)
		scalex = 20;
	if(scaley > 20)
		scaley = 20;
*/
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

	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,RGB(255,255,254),matrix,CSize(cx,cy),CRect(0,0,cx,cy), RGB(50, 50, 50));
	
	::SelectObject(hDC,hOldBmp);
	::SelectObject(hDC,hOldPen);
	
	m_CollectionViewImages.Replace(idx,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
}


void CCollectionViewBar::FillCollectionView(int flag, ConfigLibItem *pData)
{
	KillTimer(TIMERID_CREATEIMAGE);
	m_wndCollectionView.DeleteAllItems();
	m_aRootItem.RemoveAll();
	m_CollectionViewImages.DeleteImageList();
	m_arrIdxCreateFlag.RemoveAll();
	//将树信息存到USERIDX中
	m_UserIdx.Clear();
	m_UserRencent.Clear();

	if (pData == NULL)
		return;

	m_config = *pData;

	if (m_config.pScheme == NULL) return;

	int i=0;
	if(m_bCommonUse)
	{
		m_arrCommonLayers.RemoveAll();
		CString path = GetConfigPath(FALSE) + "\\CommonLayers.txt";
		FILE *fp = fopen((LPCTSTR)path, "r");
		if(fp)
		{
			char line[1024];
			while(!feof(fp))
			{
				memset(line,0,sizeof(line));
				fgets(line,sizeof(line)-1,fp);
				if(strlen(line)<=0)  continue;
				
				CString str(line);
				str.TrimLeft();
				str.TrimRight();
				m_arrCommonLayers.Add(str);
			}
			fclose(fp);
		}
		LAYGROUP group;
		strcpy(group.GroupName,StrFromResID(IDS_COMMONLAYERS));
		m_UserIdx.m_aGroup.Add(group);

		for(i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
		{
			CSchemeLayerDefine *layer = m_config.pScheme->GetLayerDefine(i);
					
			IDX idx;
			idx.code = layer->GetLayerCode();
			strcpy(idx.FeatureName,layer->GetLayerName());
			strcpy(idx.strAccel,layer->GetAccel());
			idx.groupidx = 0;

			for(int j=0; j<m_arrCommonLayers.GetSize(); j++)
			{
				if(m_arrCommonLayers[j].CompareNoCase(idx.FeatureName) == 0)
				{
					break;
				}
			}
			if(j>=m_arrCommonLayers.GetSize())
				continue;
			
			m_UserIdx.m_aIdx.Add(idx);
		}
	}
	else
	{
		for (i=0; i<m_config.pScheme->m_strSortedGroupName.GetSize(); i++)
		{
			CString strGroupName = m_config.pScheme->m_strSortedGroupName.GetAt(i);
			if (strGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)
			{
				continue;
			}
			LAYGROUP group;
			strcpy(group.GroupName,strGroupName);
			m_UserIdx.m_aGroup.Add(group);
		}
	
		for(i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
		{
			CSchemeLayerDefine *layer = m_config.pScheme->GetLayerDefine(i);
			for(int j=0; j<m_UserIdx.m_aGroup.GetSize(); j++)
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
	}

	CreateImageList(20,20,m_UserIdx.m_aIdx.GetSize());

	char strR[_MAX_FNAME];
	int  recentidx = -1;
	
	for (i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
	{
		LAYGROUP gp;
		gp = m_UserIdx.m_aGroup.GetAt(i);
		if (stricmp(gp.GroupName,"Recent")==0x00) recentidx = i;
		
		if (recentidx != i)
		{
			HTREEITEM hRoot = InsertRoot(_T(gp.GroupName),GROUPID_FROMIDX(i));
			m_aRootItem.Add(hRoot);
		}
		else
		{
			HTREEITEM hRoot = NULL;
			m_aRootItem.Add(hRoot);
		}
	}
	for (i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
	{
		IDX idx;
		idx = m_UserIdx.m_aIdx.GetAt(i);
		if (strlen(idx.strAccel) <= 0)
		{
			sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
		}
		else
		{
			sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
		}
		if (idx.groupidx != recentidx)
		{
			InsertItem(_T(strR),m_aRootItem.GetAt(idx.groupidx),CODEID_FROMIDX(i));
		}
	}

	m_wndCollectionView.SetItemHeight(22);
	
	m_wndCollectionView.RedrawWindow();

	if(m_bCommonUse)
	{
		HTREEITEM hRoot = m_wndCollectionView.GetRootItem();
		m_wndCollectionView.Expand(hRoot,TVE_EXPAND);
	}
}

//##ModelId=41466B7F0138
void CCollectionViewBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndCollectionView;
	ASSERT_VALID (pWndTree);

	CPoint ptTree = point;
	pWndTree->ScreenToClient(&ptTree);
	if (ptTree.y < 0)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	BOOL bSubItem = FALSE;
	UINT uFlags;
	HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &uFlags);
	if (hTreeItem && (TVHT_ONITEM & uFlags))
	{
		DWORD_PTR data = m_wndCollectionView.GetItemData(hTreeItem);
		if (data < 0xffff && IsValid() )
		{
			pWndTree->SelectItem(hTreeItem);
			bSubItem = TRUE;
		}
	}

	pWndTree->SetFocus ();
	CMenu menu;
	menu.LoadMenu (IDR_SYMBOL_CONFIG);
	
	CMenu* pSumMenu = menu.GetSubMenu(0);
	if(m_bCommonUse)
	{
		menu.GetSubMenu(0)->RemoveMenu(ID_SWITCH_TO_COMMON,MF_BYCOMMAND);
	}
	else
	{
		menu.GetSubMenu(0)->RemoveMenu(ID_SWITCH_TO_ALL,MF_BYCOMMAND);
	}
	if (bSubItem)
	{
		if(m_bCommonUse)
		{
			menu.GetSubMenu(0)->RemoveMenu(ID_ADD_TO_COMMON,MF_BYCOMMAND);
		}
		else
		{
			menu.GetSubMenu(0)->RemoveMenu(ID_DELETE_FROM_COMMON,MF_BYCOMMAND);
		}
	}
	else
	{
		menu.GetSubMenu(0)->RemoveMenu(IDS_SYMBOL_CONFIG,MF_BYCOMMAND);
		menu.GetSubMenu(0)->RemoveMenu(IDS_MODIFY_LAYER,MF_BYCOMMAND);
		menu.GetSubMenu(0)->RemoveMenu(IDS_MODIFY_LAYER1,MF_BYCOMMAND);
		menu.GetSubMenu(0)->RemoveMenu(ID_ADD_TO_COMMON,MF_BYCOMMAND);
		menu.GetSubMenu(0)->RemoveMenu(ID_DELETE_FROM_COMMON,MF_BYCOMMAND);
	}
	
	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		
		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;
		
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

//##ModelId=41466B7F00DA
void CCollectionViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}

	CRect rectClient, rectEdit;
	GetClientRect (rectClient);

	m_wndCodeEdit.GetWindowRect (&rectEdit);
	
	int cyTlb = 0;
	
	m_wndCodeEdit.SetWindowPos (NULL,
		rectClient.left, 
		rectClient.top,
		rectClient.Width()-20,
		20,
		SWP_NOACTIVATE|SWP_NOZORDER);

	m_wndSearchRsltList.SetWindowPos (NULL, rectClient.left,rectClient.top+20,rectClient.Width()-20,135, SWP_NOACTIVATE|SWP_NOZORDER);

	m_wndLayerPreview.SetWindowPos (NULL,
		rectClient.left+rectClient.Width()-20, 
		rectClient.top,
		20,
		20,
		SWP_NOACTIVATE|SWP_NOZORDER);

	cyTlb = 20;
	m_wndCollectionView.SetWindowPos (NULL, 
		rectClient.left + 1, 
		rectClient.top + cyTlb + 1,
		rectClient.Width() - 2, 
		rectClient.Height() - cyTlb - 2,
		SWP_NOACTIVATE|SWP_NOZORDER);
}

//##ModelId=41466B7F0109
BOOL CCollectionViewBar::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN )
	{
		if (pMsg->wParam== VK_DOWN)
		{
			int nIndex = m_wndSearchRsltList.GetCurSel();
			m_wndSearchRsltList.SetCurSel(nIndex+1);

			m_bUpDown = TRUE;

			if (nIndex >= 0)
			{
				CString selLayerNameId;
				m_wndSearchRsltList.GetText(nIndex,selLayerNameId);
				
				char layName[_MAX_FNAME];
				char num[_MAX_FNAME];
				sscanf(selLayerNameId,"%s %s",&num,layName);
				
				m_wndCodeEdit.SetWindowText(num);
			}

			return TRUE;
		}
		else if(pMsg->wParam==VK_UP)
		{
			int nIndex = m_wndSearchRsltList.GetCurSel();
			m_wndSearchRsltList.SetCurSel(nIndex-1);

			m_bUpDown = TRUE;

			if (nIndex >= 0)
			{
				CString selLayerNameId = m_wndSearchRsltList.getString(nIndex);
				/*CString selLayerNameId;
				m_wndSearchRsltList.GetText(nIndex,selLayerNameId);*/
				
				char layName[_MAX_FNAME];
				char num[_MAX_FNAME];
				sscanf(selLayerNameId,"%s %s",&num,layName);
				
				m_wndCodeEdit.SetWindowText(num);
			}

			return TRUE;
		}
		else if( pMsg->wParam==VK_ESCAPE )
		{
			HWND hFocus = ::GetFocus();
			if (IsFloating() || IsAutoHideMode())
			{
				if( hFocus==m_wndCodeEdit.GetSafeHwnd() || hFocus==m_wndCollectionView.GetSafeHwnd() || 
					hFocus==GetSafeHwnd() )
				{
					DockRecent();
				}
			}

			m_wndSearchRsltList.ShowWindow(SW_HIDE);
			m_bUpDown = FALSE;
			return TRUE;
		}
		else if( pMsg->wParam==VK_RETURN )
		{
			m_bReturn = TRUE;
			m_bUpDown = FALSE;
			m_wndSearchRsltList.ShowWindow(SW_HIDE);

			int nIndex = m_wndSearchRsltList.GetCurSel();
			if (nIndex >= 0)
			{
				/*CString selLayerNameId;
				m_wndSearchRsltList.GetText(nIndex,selLayerNameId);*/
				CString selLayerNameId = m_wndSearchRsltList.getString(nIndex);

				char layName[_MAX_FNAME];
				char num[_MAX_FNAME];
				sscanf(selLayerNameId,"%s %s",&num,layName);

				//m_bCanSelectDefault = TRUE;
				//OnChangeEditFcode();
				//m_bCanSelectDefault = FALSE;

				m_wndCodeEdit.SetWindowText("");
				m_wndSearchRsltList.ShowWindow(SW_HIDE);
				m_wndCollectionView.UpdateWindow();
				
				__int64 code = _atoi64(num);
				if(code!=0)
				{
					int nsize = m_UserIdx.m_aIdx.GetSize();
					for(int i=0; i<nsize; i++)
					{
						IDX idx;
						idx = m_UserIdx.m_aIdx.GetAt(i);
						if( code==idx.code )
						{
							SetLayerByUseridx(i);
							break;
						}
					}
				}

				DockRecent();
				
			}
			else
			{
				HWND hFocus = ::GetFocus();
				if( hFocus==m_wndCodeEdit.GetSafeHwnd() )
				{
					m_bCanSelectDefault = TRUE;
					OnChangeEditFcode();
					m_bCanSelectDefault = FALSE;
				}
			}

			m_bReturn = FALSE;
			return TRUE;
		}
		else
		{
			m_bUpDown = FALSE;
			HWND hFocus = ::GetFocus();
			if( hFocus==m_wndCodeEdit.GetSafeHwnd() && pMsg->wParam!=VK_F2 )
			{
				return PreTranslateInput(pMsg);
			}
			
		}
	}

	if (pMsg->message == WM_LBUTTONDOWN)
	{
		CRect rectClient;
		m_wndLayerPreview.GetClientRect (rectClient);

		CPoint pt = pMsg->pt;
		m_wndLayerPreview.ScreenToClient(&pt);

		if (rectClient.PtInRect(pt))
		{
			OnPreview();
		}

		m_bUpDown = FALSE;

	}
	return CDockablePane::PreTranslateMessage(pMsg);
}

//##ModelId=41466B7F0196
void CCollectionViewBar::OnSort (UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;
}

//##ModelId=41466B7F0199
void CCollectionViewBar::OnUpdateSort (CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck (pCmdUI->m_nID == m_nCurrSort);
}


//##ModelId=41466B7F0185
void CCollectionViewBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rectTree;
	m_wndCollectionView.GetWindowRect (rectTree);
	ScreenToClient (rectTree);

	rectTree.InflateRect (1, 1);
	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));

}


static void BigRect(CRect& rect)
{
	if(rect.Width()<300)
		rect.right = rect.left + 300;
}

void CCollectionViewBar::PopupforEditCode()
{
	m_bClosedBeforePopup = FALSE;
	m_bAutoHideBeforePopup = FALSE;

	CWnd *pWnd = GetParent();
	CWnd *pMain = AfxGetMainWnd();

	if( IsAutoHideMode() )
	{
		m_bAutoHideBeforePopup = TRUE;
		int nOldStep = m_nSlideSteps;
		m_nSlideSteps = 1;
		Slide(TRUE,TRUE);
		OnTimer(AFX_TIMER_ID_AUTO_HIDE_SLIDE_OUT_EVENT);
		m_nSlideSteps = nOldStep;
	}
	//从tab窗口中脱离出来
	else if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMFCBaseTabCtrl)))
	{
		CMFCBaseTabCtrl *pTab = (CMFCBaseTabCtrl*)pWnd;
		for( int i=0; i<pTab->GetTabsNum(); i++)
		{
			CDockablePane* pDockingBar =
				DYNAMIC_DOWNCAST(CDockablePane, pTab->GetTabWnd(i));
			if( pDockingBar==this )break;
		}

		if( i<pTab->GetTabsNum() )
		{
			pTab->DetachTab(DM_SHOW,i);
		}
		else
		{
			m_bClosedBeforePopup = TRUE;
			pMain->SendMessage(WM_COMMAND,ID_VIEW_COLLECTION);
			for( i=0; i<pTab->GetVisibleTabsNum(); i++)
			{
				CDockablePane* pDockingBar =
					DYNAMIC_DOWNCAST(CDockablePane, pTab->GetTabWnd(i));
				if( pDockingBar==this )break;
			}
			if( i<pTab->GetVisibleTabsNum() )
			{
				pTab->DetachTab(DM_SHOW,i);
			}
		}

		pWnd = GetParent();

		//计算合适的弹出位置
		CRect rectPop(400,200,600,600);
		
		CRect rectCur;
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd)))
			pWnd->GetWindowRect(&rectCur);
		else
			GetWindowRect(&rectCur);

		BigRect(rectCur);
		
		pMain->GetWindowRect(&rectPop);
		
		CPoint pt = rectPop.CenterPoint();
		rectPop.left = pt.x-rectCur.Width()/2; 
		rectPop.right = rectPop.left+rectCur.Width();
		rectPop.top = pt.y-rectCur.Height()/2; 
		rectPop.bottom = rectPop.top+rectCur.Height();

		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd)))
		{
			CMultiPaneFrameWnd *pMin = (CMultiPaneFrameWnd*)pWnd;
			pMin->SetWindowPos(NULL,rectPop.left,rectPop.top,rectPop.Width(),rectPop.Height(),SWP_NOZORDER);
			pMin->BringWindowToTop();
		}
		else BringWindowToTop();
	}
	else if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd)))
	{
		m_bClosedBeforePopup = (!IsWindowVisible());
		if( !pWnd->IsWindowVisible() )
		{
			pMain->SendMessage(WM_COMMAND,ID_VIEW_COLLECTION);
		}

		//计算合适的弹出位置
		CRect rectPop(400,200,600,600);
		
		CRect rectCur;
		pWnd->GetWindowRect(&rectCur);		
		pMain->GetWindowRect(&rectPop);
		BigRect(rectCur);
		
		CPoint pt = rectPop.CenterPoint();
		rectPop.left = pt.x-rectCur.Width()/2; 
		rectPop.right = rectPop.left+rectCur.Width();
		rectPop.top = pt.y-rectCur.Height()/2; 
		rectPop.bottom = rectPop.top+rectCur.Height();
	
		CMultiPaneFrameWnd *pMin = (CMultiPaneFrameWnd*)pWnd;
		pMin->SetWindowPos(NULL,rectPop.left,rectPop.top,rectPop.Width(),rectPop.Height(),SWP_NOZORDER);
		pMin->BringWindowToTop();
	}
	else if( pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)) )
	{
		m_bClosedBeforePopup = (!IsWindowVisible());
		if( !IsWindowVisible() )
		{
			pMain->SendMessage(WM_COMMAND,ID_VIEW_COLLECTION);
		}

		//计算合适的弹出位置
		CRect rectPop(400,200,600,600);
		
		CRect rectCur;
		GetWindowRect(&rectCur);
		BigRect(rectCur);
		
		pMain->GetWindowRect(&rectPop);
		
		CPoint pt = rectPop.CenterPoint();
		rectPop.left = pt.x-rectCur.Width()/2; 
		rectPop.right = rectPop.left+rectCur.Width();
		rectPop.top = pt.y-rectCur.Height()/2; 
		rectPop.bottom = rectPop.top+rectCur.Height();

		StoreRecentDockSiteInfo();
		FloatPane(rectPop, DM_SHOW);
		pWnd = GetParent();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd)))
		{
			CMultiPaneFrameWnd *pMin = (CMultiPaneFrameWnd*)pWnd;
			pMin->SetWindowPos(NULL,rectPop.left,rectPop.top,rectPop.Width(),rectPop.Height(),SWP_NOZORDER);
			pMin->BringWindowToTop();
		}
		else BringWindowToTop();
	}
	
	m_wndCodeEdit.SetSel(0,-1);
	m_wndCodeEdit.SetFocus();
}

void CCollectionViewBar::DockRecent()
{
	CWnd *pWnd = GetParent();
	if( m_bAutoHideBeforePopup )
	{
		Slide(FALSE,FALSE);
	}
	else if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd)))
	{
		CMultiPaneFrameWnd *pMin = (CMultiPaneFrameWnd*)pWnd;
		if( m_bClosedBeforePopup )pMin->CloseMiniFrame();
		else pMin->SendMessage(WM_LBUTTONDBLCLK);
	}
	AfxGetMainWnd()->SetFocus();
}

CString ChineseToLetter(LPCTSTR text)
{
	CMyChiToLetter cc;
	cc.SetFlags(TRUE,FALSE,FALSE,FALSE);

	return cc.GetLetter(text);
}



void CCollectionViewBar::OnChangeEditFcode()
{
	if( !IsValid() )return;
	if( m_bChgFromSelect || m_bUpDown )return;

	CString strInput;
	wchar_t input[256], inputPinYin[255];
	m_wndCodeEdit.GetWindowText(strInput);

	strInput.MakeLower();

	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;	
	wstring wstr;
	ConvertCharToWstring((LPCTSTR)strInput,wstr,codepage);			
	wcscpy(input,(wchar_t*)wstr.c_str());

	CString strInputPinYin = ChineseToLetter(strInput);
	ConvertCharToWstring((LPCTSTR)strInputPinYin,wstr,codepage);			
	wcscpy(inputPinYin,(wchar_t*)wstr.c_str());
	
	int nInLen = wcslen(input);
	if( nInLen<=0 )
	{
		m_wndSearchRsltList.ResetContent();
		m_wndSearchRsltList.ShowWindow(SW_HIDE);
		return;
	}

	if (!m_bReturn)
	{
		m_wndSearchRsltList.ShowWindow(SW_SHOW);

		CRect cltRect;
		m_wndSearchRsltList.GetClientRect(&cltRect);
		m_wndSearchRsltList.InvalidateRect(cltRect);
		m_wndSearchRsltList.UpdateWindow();
	}	

	CString strLayerName;
	CString strCodeId;
	CString strNodeName;
	
	m_wndSearchRsltList.ResetContent();	

	wchar_t test1[256], test2[256], test3[256], test4[256];
	int nsize = m_UserIdx.m_aIdx.GetSize();
	if( nsize<=0 )return;

	//记录各个图层的匹配长度
	CArray<int,int> arrMaxPos1, arrMaxPos2;

	int pos1, pos2, pos3, pos4, maxPos1=-1, k1=-1, maxPos2=-1, k2=-1;
	int bMaxMatch1 = 0, bMaxMatch2 = 0;
	for(int i=0; i<nsize; i++)
	{
		IDX idx;
		idx = m_UserIdx.m_aIdx.GetAt(i);
		if( idx.groupidx!=-1 )
		{
			wsprintfW(test1,L"%I64d",idx.code);			
			
			wstring wstr0;
			ConvertCharToWstring(idx.FeatureName,wstr0,codepage);			
			wcscpy(test2,(wchar_t*)wstr0.c_str());
			wstring wstr1;
			CString strAccel = idx.strAccel;
			strAccel.MakeLower();
			ConvertCharToWstring(strAccel,wstr1,codepage);			
			wcscpy(test3,(wchar_t*)wstr1.c_str());

			pos1 = pos2 = pos3 = pos4 = 0;

			//代码、快捷键必须从第一个字开始匹配
			if(wcsstr(test1,input)==test1)
				pos1 = nInLen;

			if(wcsstr(test3,input)==test3)
				pos3 = nInLen;

			int maxPos_t = (pos1>pos3?pos1:pos3);
			arrMaxPos1.Add(maxPos_t);

			//找最大匹配
			{
				if( maxPos1<0 || maxPos1<pos1 || maxPos1<pos3 )
				{
					maxPos1 = maxPos_t;
					k1 = i;
				}

				//代码或快捷键精确匹配了，可以退出了
				if( maxPos1==maxPos_t && maxPos1 == nInLen && (wcslen(test1)==nInLen || wcslen(test3)==nInLen) )
				{
					bMaxMatch1 = 1;
					k1 = i;
				}
			}

			ConvertCharToWstring(ChineseToLetter(idx.FeatureName),wstr1,codepage);			
			wcscpy(test4,(wchar_t*)wstr1.c_str());

			//层名或者拼音，可以从非首字开始匹配
			if(wcsstr(test2,input)!=0)
				pos2 = nInLen;

			if(wcsstr(test4,inputPinYin)!=0)
				pos4 = nInLen;

			maxPos_t = (pos2>pos4?pos2:pos4);
	
			arrMaxPos2.Add((pos2>pos4?pos2:pos4));
			
			//找最大匹配
			{
				if( maxPos2<0 || maxPos2<pos2 || maxPos2<pos4 )
				{
					maxPos2 = maxPos_t;
					k2 = i;
				}
				
				//层名或其拼音精确匹配了，可以退出了
				if( maxPos2==maxPos_t && maxPos2 == nInLen && (wcslen(test2)==nInLen || wcslen(test4)==nInLen) )
				{
					bMaxMatch2 = 1;
					k2 = i;
				}
			}
		}
	}

 	if( k1<0 && k2<0 )return;
	int k = (maxPos1>=maxPos2?k1:k2);
	int maxPos = (maxPos1>=maxPos2?maxPos1:maxPos2);
	
	if(strInput.GetLength()>0 && maxPos<=0 )
		return;

	CArray<int,int> *pMaxPosArr = (maxPos1>=maxPos2?&arrMaxPos1:&arrMaxPos2);

	BOOL bFindDefault = m_UserIdx.m_aIdx[k].groupidx==0?TRUE:FALSE;

	int selIndex = 0;
	for(int j=0; j < nsize; j++)
	{
		strLayerName.Format("%s",m_UserIdx.m_aIdx.GetAt(j).FeatureName);
		strCodeId.Format("%I64d",m_UserIdx.m_aIdx.GetAt(j).code);
		strNodeName = strCodeId +" "+strLayerName;
		if (maxPos==pMaxPosArr->GetAt(j))
		{
			int index = m_wndSearchRsltList.AppendString(strNodeName, RGB(255, 255, 255), RGB(50, 50, 50));
			m_wndSearchRsltList.Invalidate(FALSE);
			m_wndSearchRsltList.UpdateWindow();
			if (j == k) 
				selIndex = index;
		}
	}
	m_wndSearchRsltList.SetCurSel(selIndex);

	//查找序号 k 所在的树节点
	HTREEITEM hRoot = m_wndCollectionView.GetRootItem(), hChild = NULL;
	while( hRoot )
	{
		hChild = m_wndCollectionView.GetChildItem(hRoot);
		while( hChild )
		{
			DWORD_PTR data = m_wndCollectionView.GetItemData(hChild);
			if( k==CODEID_TOIDX(data) )goto FINDITEM;
			hChild = m_wndCollectionView.GetNextItem(hChild, TVGN_NEXT);
		}
		hRoot = m_wndCollectionView.GetNextItem(hRoot, TVGN_NEXT);
	}
	
FINDITEM:
	if( !hRoot || !hChild )return;

	//跳到这里显示
	if( m_nIdxToCreateImage<m_arrIdxCreateFlag.GetSize() )
	{
		for( int i=k+1; i<m_arrIdxCreateFlag.GetSize(); i++)
		{
			if( m_arrIdxCreateFlag[i]==0 )break;
		}
		if( i<m_arrIdxCreateFlag.GetSize() )
		{
			m_nIdxToCreateImage = i;
			OnTimer(TIMERID_CREATEIMAGE);
		}
	}

	//显示到最上面
	HTREEITEM hRoot2 = m_wndCollectionView.GetRootItem();
	while( hRoot2 )
	{
		if( hRoot2!=hRoot )m_wndCollectionView.Expand(hRoot2,TVE_COLLAPSE);
		hRoot2 = m_wndCollectionView.GetNextItem(hRoot2, TVGN_NEXT);
	}
	m_wndCollectionView.Expand(hRoot,TVE_EXPAND);
	m_wndCollectionView.Select(hChild,TVGN_FIRSTVISIBLE);


	//完全匹配，且只有一个
	if( (m_bCanSelectDefault || !bFindDefault) && (bMaxMatch1 || bMaxMatch2) && m_wndSearchRsltList.GetCount()==1 )
	{
		m_wndSearchRsltList.ShowWindow(SW_HIDE);
		m_wndCollectionView.UpdateWindow();

		SetLayerByUseridx(k);
		DockRecent();
		
	}
}


BOOL CCollectionViewBar::SetLayerByUseridx(int idx)
{
	if( !IsValid() )return FALSE;
	if( idx<0 || idx>=m_UserIdx.m_aIdx.GetSize() )return FALSE;

	IDX useridx = m_UserIdx.m_aIdx.ElementAt(idx);
	m_UserRencent.m_aIdx.Add(useridx);
	// 激发特征缺省命令
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return FALSE;
	
	if (pDoc->ActiveLayer(useridx.FeatureName))
	{
//		AfxGetMainWnd()->SendMessage (FCCM_INITFTRCLASS,WPARAM(0),LPARAM(&m_UserRencent));
	}
	else
	{
		AfxMessageBox(IDS_ERR_ACTIVELAYER_FAILED);
		return FALSE;
	}
		
	
// 	AfxGetMainWnd()->SendMessage(FCCM_FTRCLASS_START,(WPARAM)useridx.code,(LPARAM)useridx.FeatureName);

// 	for (int i=0;i<m_config.GetLayersNum();i++)
// 	{
// 		CGeoLayer *Lay = m_config.GetLayerByIndex(i);
// 		for (int j=0;j<Lay->m_FtrClsList.List.GetSize();j++)
// 		{
// 			CDpDBVariant		var;
// 			DpCustomFieldType	type;
// 			FTRCLASS ftrcls = Lay->m_FtrClsList.List.GetAt(j);
// 			CGeoDataExchange *pGDX = ftrcls.pGDX;
// 			pGDX->GetAttrValue("FID",var,type);
// 			if( stricmp(var.m_pString,useridx.FeatureName)!=0 )continue;
// 
// 			CGeoLayer *pCurLayer = m_config.SetCurrentLayer(Lay->GetHandle());
// 			if(!pCurLayer)continue;
// 			if (ftrcls.nDefCommandID==0)continue;
// 
// 			Lay->m_FtrClsList.nCur = j;
// 
// 			// 激发特征缺省命令
// 			AfxGetMainWnd()->SendMessage(WM_COMMAND,ftrcls.nDefCommandID|0x10000);
// 
// 			// 添加到上次最近采集特征列表
// 			m_config.AddToRecent(useridx.code,useridx.FeatureName);
// 			return TRUE;
// 		}		
// 	}

 	return TRUE;
}


void CCollectionViewBar::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if( !IsValid() )return;

	NMTREEVIEW *pTree = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	if( !hItem )return;

	CString strNameAndCode = m_wndCollectionView.GetItemText(hItem);
	char layName[256];
	__int64 num;
	sscanf(strNameAndCode,"%I64d %s",&num,layName);
	
	m_strCurNameAndCode = strNameAndCode;
	
	// 选择的组还是层
	DWORD_PTR data = m_wndCollectionView.GetItemData(hItem);
	m_wndSearchRsltList.ShowWindow(SW_HIDE);
	m_bUpDown = FALSE;
	if( data>=0xffff )
	{
		m_wndCodeEdit.SetWindowText("");
		return;
	}

	CString strCode;
	strCode.Format("%I64d",num);
	m_bChgFromSelect = TRUE;
	m_wndCodeEdit.SetWindowText(strCode);
	m_bChgFromSelect = FALSE;

	// 优先画选择的项
	int nsize = m_arrIdxCreateFlag.GetSize();
	if( m_nIdxToCreateImage>=nsize )return;

	for ( int i=CODEID_TOIDX(data)+1; i<nsize; i++)
	{
		if( m_arrIdxCreateFlag[i]==0 )break;
	}

	if ( i<nsize )
	{
		CreateImageItem(i);
		m_wndCollectionView.RedrawWindow();		
	}

}


void CCollectionViewBar::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM item = m_wndCollectionView.GetSelectedItem();
	DWORD_PTR dwData = m_wndCollectionView.GetItemData(item);
	
	//Child Item
	if( dwData<0xffff && IsValid() )
	{
		m_strCurNameAndCode = m_wndCollectionView.GetItemText(item);

		if( !SetLayerByUseridx(dwData) )
		{
			//AfxMessageBox(IDS_FAILTOFINDLAYER);
		}
		else
		{		
			IDX useridx = m_UserIdx.m_aIdx.GetAt(dwData);
			
			//更新编辑框
			CString strCode;
			strCode.Format("%I64d",useridx.code);
			m_bChgFromSelect = TRUE;
			m_wndCodeEdit.SetWindowText(strCode);
			m_bChgFromSelect = FALSE;

			DockRecent();
		}
	}
	
	*pResult = 0;
}

void CCollectionViewBar::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==TIMERID_CREATEIMAGE )
	{
		BOOL bKill = FALSE;
		if( !m_config.pScheme )bKill = TRUE;
		else
		{
			int nsize = m_arrIdxCreateFlag.GetSize();

			//检查是否已经没有需要生成的图像了
			for( int i=m_nIdxToCreateImage; i<nsize+m_nIdxToCreateImage; i++)
			{
				if( m_arrIdxCreateFlag[i%nsize]==0 )break;
			}
			
			if( i>=nsize+m_nIdxToCreateImage )bKill = TRUE;
			else
			{
				i = (i%nsize);
				if( CreateImageItem(i) )
				{
					m_arrIdxCreateFlag[i] = 1;
					m_nIdxToCreateImage = i+1;
					
					static int nCreateTimes = 0;
					if( (nCreateTimes+1)%10==0 )m_wndCollectionView.RedrawWindow();
					nCreateTimes++;
				}
			}
		}

		if( bKill )
			KillTimer(TIMERID_CREATEIMAGE);
	}
	CDockablePane::OnTimer(nIDEvent);
}

void CCollectionViewBar::OnLayerEdit (UINT id)
{
// 	HTREEITEM hItem = m_wndCollectionView.GetSelectedItem();
// 	DWORD data = m_wndCollectionView.GetItemData(hItem);
// 
// 	if( data>=0xffff || !IsValid() )
// 		return;
// 
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndCollectionView.GetItemText(hItem);
// 	int code;
// 	char name[256] = {0};
// 	if( sscanf(strText,"%d %s",&code,name)!=2 )
// 		return;
// 
// 	CString LayerName = name;
// 
// 	pLayer = m_config.GetLayerByFIDOrUserIdx(LPCTSTR(LayerName));
// 	if( !pLayer )return;
// 	pCond  = pLayer->GetConditions();
// 
// 	if (id==ID_LAYERITEM_DELETE)
// 	{
// 		for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond = pCond->GetAt(i);
// 			if( cond.ntype==0 &&
// 				strcmp(cond.field,"FID")==0 && strcmp(cond.value,LayerName)==0 ) 
// 			{
// 				pCond->RemoveAt(i);
// 				pLayer->m_bConfigModified = TRUE;
// 				break;
// 			}
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_DELETEANNOT)
// 	{
// 		for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond = pCond->GetAt(i);
// 			if( cond.ntype==1 &&
// 				strcmp(cond.field,"FID")==0 && strcmp(cond.value,LayerName)==0 ) 
// 			{
// 				pCond->RemoveAt(i);
// 				
// 				if( cond.ntype==1 )
// 				{
// 					for( int j=pLayer->m_arrAnnots.GetSize()-1; j>=0; j-- )
// 					{
// 						CAnnotation *pAnnot = (CAnnotation*)pLayer->m_arrAnnots[j];
// 						if( pAnnot->GetName().CompareNoCase(cond.symname)==0 )
// 						{
// 							delete pAnnot;
// 							pLayer->m_arrAnnots.RemoveAt(j);
// 						}
// 					}
// 				}
// 				pLayer->m_bConfigModified = TRUE;
// 				break;
// 			}
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_CREATE)
// 	{
// 		//for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond;			
// 			CDlgConditionEdit dlg(NULL,pLayer,pLayer->GetLayerType(),&(m_config.m_SymbolLib));
// 			dlg.m_SymbolName = "NULL";
// 			dlg.m_FieldName.LoadString(IDS_FID);
// 			dlg.m_Value = LayerName;
// 			if( dlg.DoModal()==IDOK && dlg.m_SymbolName!="NULL" )
// 			{
// 				// 将别名转化为字段名
// 				strcpy(cond.field,LPCTSTR(pLayer->GetAttrFieldNameFromAlias(dlg.m_FieldName)));
// 				strcpy(cond.value,LPCTSTR(dlg.m_Value));
// 				strcpy(cond.symname,LPCTSTR(dlg.m_SymbolName));
// 				
// 				for (int i=0;i<pCond->GetSize();i++)
// 				{
// 					CONDITION cond1 = pCond->GetAt(i);
// 					if (cond1.ntype==0 &&
// 						stricmp(cond1.field,cond.field)==0x00 &&
// 						stricmp(cond1.value,cond.value)==0x00 && 
// 						(stricmp(cond1.symname,"NULL")!=0&&strlen(cond1.symname)>0) )
// 					{
// 						AfxMessageBox(IDS_COND_REDUNDANT);
// 						break;
// 					}
// 					if (cond1.ntype==0 &&
// 						stricmp(cond1.field,cond.field)==0x00 &&
// 						stricmp(cond1.value,cond.value)==0x00 && 
// 						(stricmp(cond1.symname,"NULL")==0||strlen(cond1.symname)==0) )
// 					{
// 						pCond->RemoveAt(i);
// 						i--;
// 					}
// 				}
// 				pCond->InsertAt(0,cond);
// 								
// 				pLayer->m_bConfigModified = TRUE;
// 			}
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_MODIFY)
// 	{
// 		for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond = pCond->GetAt(i);
// 			if( strcmp(cond.field,"FID")==0 && strcmp(cond.value,LayerName)==0 )
// 			{
// 				if(cond.ntype==0)
// 				{
// 					CDlgConditionEdit dlg(NULL,pLayer,pLayer->GetLayerType(),&(m_config.m_SymbolLib));
// 					
// 					dlg.m_SymbolName = cond.symname;
// 					dlg.m_Value = cond.value;
// 					dlg.m_FieldName = pLayer->GetAliasFromAttrFieldName(cond.field);
// 					
// 					if (dlg.DoModal()==IDOK)
// 					{
// 						// 将别名转化为字段名
// 						strcpy(cond.field,LPCTSTR(pLayer->GetAttrFieldNameFromAlias(dlg.m_FieldName)));
// 						strcpy(cond.value,LPCTSTR(dlg.m_Value));
// 						strcpy(cond.symname,LPCTSTR(dlg.m_SymbolName));
// 						pCond->SetAt(i,cond);
// 						
// 						pLayer->m_bConfigModified = TRUE;
// 						break;
// 					}
// 					break;
// 				}
// 			}
// 		}
// 	}
}

void CCollectionViewBar::OnUpdateLayerEdit(CCmdUI *pCmdUI)
{
// 	pCmdUI->Enable(FALSE);
// 
// 	HTREEITEM hItem = m_wndCollectionView.GetSelectedItem();
// 	DWORD data = m_wndCollectionView.GetItemData(hItem);
// 
// 	if( data>=0xffff || !IsValid() )
// 		return;
// 
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndCollectionView.GetItemText(hItem);
// 	int code;
// 	char name[256] = {0};
// 	if( sscanf(strText,"%d %s",&code,name)!=2 )
// 		return;
// 	
// 	CString LayerName = name;
// 	
// 	pLayer = m_config.GetLayerByFIDOrUserIdx(LPCTSTR(LayerName));
// 	if( !pLayer )return;
// 	pCond  = pLayer->GetConditions();
// 
// 	CGeometry *pObj = m_config.CreateObjByFIDOrUserIdx(LayerName,0,FALSE);
// 	if( !pObj )return;
// 
// 	if( pCmdUI->m_nID==ID_LAYERITEM_DELETE || 
// 		pCmdUI->m_nID==ID_LAYERITEM_MODIFY )
// 	{
// 		pCmdUI->Enable(pLayer->GetSymbol(pObj)!=NULL);
// 	}
// 	else if( pCmdUI->m_nID==ID_LAYERITEM_CREATE )
// 	{
// 		pCmdUI->Enable(pLayer->GetSymbol(pObj)==NULL);
// 	}
// 	else if( pCmdUI->m_nID==ID_LAYERITEM_DELETEANNOT || 
// 		pCmdUI->m_nID==ID_LAYERITEM_MODIFYANNOT )
// 	{
// 		pCmdUI->Enable(pLayer->GetAnnotation(pObj)!=NULL);
// 	}
// 	else if( pCmdUI->m_nID==ID_LAYERITEM_CREATEANNOT )
// 	{
// 		pCmdUI->Enable(pLayer->GetAnnotation(pObj)==NULL);
// 	}
// 	
// 	if( pObj )delete pObj;

	return;
}

void CCollectionViewBar::OnCreateAnnot()
{
// 	//有效性判别
// 	HTREEITEM hItem = m_wndCollectionView.GetSelectedItem();
// 	CString itemText = m_wndCollectionView.GetItemText(hItem);
// 	DWORD data = m_wndCollectionView.GetItemData(hItem);
// 
// 	if( data>=0xffff || !IsValid() )
// 		return;
// 	
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndCollectionView.GetItemText(hItem);
// 	int code;
// 	char name[256] = {0};
// 	if( sscanf(strText,"%d %s",&code,name)!=2 )
// 		return;
// 	
// 	CString LayerName = name;
// 	
// 	pLayer = m_config.GetLayerByFIDOrUserIdx(LPCTSTR(LayerName));
// 	if( !pLayer )return;
// 	pCond  = pLayer->GetConditions();
// 	
// 	UINT id = GetCurrentMessage()->wParam;
// 	if( id!=ID_LAYERITEM_CREATEANNOT && id!=ID_LAYERITEM_MODIFYANNOT )
// 		return;
// 
// 	CGeometry *pObj = m_config.CreateObjByFIDOrUserIdx(LayerName,0,FALSE);
// 	if( !pObj )return;
// 
// 	CAnnotation *pAnnot = (CAnnotation*)pLayer->GetAnnotation(pObj);
// 	CONDITION cond;
// 	int i;
// 	
// 	for (i=0;i<pCond->GetSize();i++)
// 	{
// 		cond = pCond->GetAt(i);
// 		if( cond.ntype==1 && 
// 			strcmp(cond.field,"FID")==0 && strcmp(cond.value,LayerName)==0 )
// 		{
// 			break;
// 		}
// 	}
// 
// 	CDlgCreateAnnot dlg;
// 	dlg.m_pLayer = pLayer;
// 	dlg.m_bCreate = (id==ID_LAYERITEM_CREATEANNOT);
// 
// 	if( pAnnot && i<pCond->GetSize() )
// 	{
// 		dlg.m_strAnnotName = cond.symname;
// 		dlg.m_strField = cond.field;
// 		dlg.m_strValue = cond.value;
// 	}
// 	else
// 	{
// 		dlg.m_strField = "FID";
// 		dlg.m_strValue = LayerName;
// 	}
// 	
// 	if( dlg.DoModal()!=IDOK )return;
// 
// 	//条件冲突的检测...
// 	if( dlg.m_bCreate )
// 	{
// 		cond.ntype = 1;
// 		strcpy(cond.field,dlg.m_strField);
// 		strcpy(cond.value,dlg.m_strValue);
// 		strcpy(cond.symname,dlg.m_strAnnotName);
// 		for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond1 = pCond->GetAt(i);
// 			if( cond1.ntype==1 &&
// 				stricmp(cond1.field,cond.field)==0x00 &&
// 				stricmp(cond1.value,cond.value)==0x00 && 
// 				(stricmp(cond1.symname,"NULL")!=0&&strlen(cond1.symname)>0) )
// 			{
// 				AfxMessageBox(IDS_COND_REDUNDANT);
// 				break;
// 			}
// 			if( cond1.ntype==1 &&
// 				stricmp(cond1.field,cond.field)==0x00 &&
// 				stricmp(cond1.value,cond.value)==0x00 && 
// 				(stricmp(cond1.symname,"NULL")==0||strlen(cond1.symname)==0) )
// 			{
// 				pCond->RemoveAt(i);
// 				i--;
// 			}
// 		}
// 		pCond->Add(cond);
// 	}
// 	
// 	pLayer->m_bConfigModified = TRUE;
// 
// 	//命名冲突的检测...
// 	pAnnot = NULL;
// 	BOOL bFindSameAnnot = FALSE;
// 	for( i=pLayer->m_arrAnnots.GetSize()-1; i>=0; i--)
// 	{
// 		CAnnotation* pA = (CAnnotation*)pLayer->m_arrAnnots[i];
// 		if( pA && pA->GetName().CompareNoCase(dlg.m_strAnnotName)==0 )
// 		{
// 			pAnnot = pA;
// 			bFindSameAnnot = TRUE;
// 			break;
// 		}
// 	}
// 	
// 	if( dlg.m_bCreate && pAnnot==NULL )
// 		pAnnot = (CAnnotation*)CAnnotation::Create(dlg.m_strAnnotName);
// 	if( !pAnnot )return;
// 
// 	//得到设定的值
// 	pAnnot->m_nType = dlg.m_nType+1;
// 	pAnnot->m_nDecDigit = dlg.m_nDigitNum;
// 	pAnnot->m_lfXOff = dlg.m_lfXOff;
// 	pAnnot->m_lfYOff = dlg.m_lfYOff;
// 	pAnnot->m_settings.lfWid = dlg.m_lfWidth;
// 	pAnnot->m_settings.lfHei = dlg.m_lfHeight;
// 	pAnnot->m_settings.lfCharIntv = dlg.m_lfCharIntv;
// 	pAnnot->m_settings.lfLineIntv = dlg.m_lfLineIntv;
// 	pAnnot->m_settings.nShrugType = dlg.m_nShrugType;
// 	pAnnot->m_settings.lfShrugAngle = dlg.m_lfShrugAngle;
// 	strcpy(pAnnot->m_settings.strFontName,dlg.m_strFont);
// 
// 	if( pAnnot->m_nType==3 )
// 	{
// 		pAnnot->m_strData = dlg.m_strField2;
// 	}
// 
// 	//存储
// 	if( dlg.m_bCreate )
// 	{
// 		if( !bFindSameAnnot )
// 			pLayer->m_arrAnnots.InsertAt(0,pAnnot);
// 	}
}

void CCollectionViewBar::OnLayerEdit2 (UINT id)
{
// 	HTREEITEM hItem = m_wndCollectionView.GetSelectedItem();
// 	DWORD data = m_wndCollectionView.GetItemData(hItem);
// 
// 	if( data>=0xffff || !IsValid() )
// 		return;
// 
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndCollectionView.GetItemText(hItem);
// 	int code;
// 	char name[256] = {0};
// 	if( sscanf(strText,"%d %s",&code,name)!=2 )
// 		return;
// 
// 	CString LayerName = name;
// 
// 	pLayer = m_config.GetLayerByFIDOrUserIdx(LPCTSTR(LayerName));
// 	if( !pLayer )return;
// 	pCond  = pLayer->GetConditions();
// 
// 	if (id==ID_LAYERITEM_DELETE2)
// 	{
// 		for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond = pCond->GetAt(i);
// 			if( cond.ntype==2 &&
// 				strcmp(cond.field,"FID")==0 && strcmp(cond.value,LayerName)==0 ) 
// 			{
// 				pCond->RemoveAt(i);
// 				pLayer->m_bConfigModified = TRUE;
// 				break;
// 			}
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_CREATE2)
// 	{
// 		//for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond;			
// 			CDlgConditionEdit dlg(NULL,pLayer,GEO_Line,&(m_config.m_SymbolLib));
// 			dlg.m_SymbolName = "NULL";
// 			dlg.m_FieldName.LoadString(IDS_FID);
// 			dlg.m_Value = LayerName;
// 			if( dlg.DoModal()==IDOK && dlg.m_SymbolName!="NULL" )
// 			{
// 				// 将别名转化为字段名
// 				strcpy(cond.field,LPCTSTR(pLayer->GetAttrFieldNameFromAlias(dlg.m_FieldName)));
// 				strcpy(cond.value,LPCTSTR(dlg.m_Value));
// 				strcpy(cond.symname,LPCTSTR(dlg.m_SymbolName));
// 				cond.ntype = 2;
// 				
// 				for (int i=0;i<pCond->GetSize();i++)
// 				{
// 					CONDITION cond1 = pCond->GetAt(i);
// 					if (cond1.ntype==2 &&
// 						stricmp(cond1.field,cond.field)==0x00 &&
// 						stricmp(cond1.value,cond.value)==0x00 && 
// 						(stricmp(cond1.symname,"NULL")!=0&&strlen(cond1.symname)>0) )
// 					{
// 						AfxMessageBox(IDS_COND_REDUNDANT);
// 						break;
// 					}
// 					if (cond1.ntype==2 &&
// 						stricmp(cond1.field,cond.field)==0x00 &&
// 						stricmp(cond1.value,cond.value)==0x00 && 
// 						(stricmp(cond1.symname,"NULL")==0||strlen(cond1.symname)==0) )
// 					{
// 						pCond->RemoveAt(i);
// 						i--;
// 					}
// 				}
// 				pCond->InsertAt(0,cond);
// 								
// 				pLayer->m_bConfigModified = TRUE;
// 			}
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_MODIFY2)
// 	{
// 		for (int i=0;i<pCond->GetSize();i++)
// 		{
// 			CONDITION cond = pCond->GetAt(i);
// 			if( strcmp(cond.field,"FID")==0 && strcmp(cond.value,LayerName)==0 )
// 			{
// 				if(cond.ntype==2)
// 				{
// 					CDlgConditionEdit dlg(NULL,pLayer,GEO_Line,&(m_config.m_SymbolLib));
// 					
// 					dlg.m_SymbolName = cond.symname;
// 					dlg.m_Value = cond.value;
// 					dlg.m_FieldName = pLayer->GetAliasFromAttrFieldName(cond.field);
// 					
// 					if (dlg.DoModal()==IDOK)
// 					{
// 						// 将别名转化为字段名
// 						strcpy(cond.field,LPCTSTR(pLayer->GetAttrFieldNameFromAlias(dlg.m_FieldName)));
// 						strcpy(cond.value,LPCTSTR(dlg.m_Value));
// 						strcpy(cond.symname,LPCTSTR(dlg.m_SymbolName));
// 						pCond->SetAt(i,cond);
// 						
// 						pLayer->m_bConfigModified = TRUE;
// 						break;
// 					}
// 					break;
// 				}
// 			}
// 		}
// 	}
}

void CCollectionViewBar::OnUpdateLayerEdit2(CCmdUI *pCmdUI)
{
// 	pCmdUI->Enable(FALSE);
// 
// 	HTREEITEM hItem = m_wndCollectionView.GetSelectedItem();
// 	DWORD data = m_wndCollectionView.GetItemData(hItem);
// 
// 	if( data>=0xffff || !IsValid() )
// 		return;
// 
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndCollectionView.GetItemText(hItem);
// 	int code;
// 	char name[256] = {0};
// 	if( sscanf(strText,"%d %s",&code,name)!=2 )
// 		return;
// 	
// 	CString LayerName = name;
// 	
// 	pLayer = m_config.GetLayerByFIDOrUserIdx(LPCTSTR(LayerName));
// 	if( !pLayer )return;
// 	pCond  = pLayer->GetConditions();
// 
// 	CGeometry *pObj = m_config.CreateObjByFIDOrUserIdx(LayerName,0,FALSE);
// 	if( !pObj )return;
// 
// 	if( pCmdUI->m_nID==ID_LAYERITEM_DELETE2 || 
// 		pCmdUI->m_nID==ID_LAYERITEM_MODIFY2 )
// 	{
// 		pCmdUI->Enable(pLayer->GetSymbol2(pObj)!=NULL);
// 	}
// 	else if( pCmdUI->m_nID==ID_LAYERITEM_CREATE2 )
// 	{
// 		pCmdUI->Enable(pLayer->GetLayerType()==GEO_Polygon &&pLayer->GetSymbol2(pObj)==NULL);
// 	}
// 	
// 	if( pObj )delete pObj;

	return;
}

void CCollectionViewBar::OnSymbolConfig() 
{
	// TODO: Add your command handler code here
	CDlgDoc *pDoc = GetActiveDlgDoc();
	
	if( pDoc )
	{
		for( int m=0; m<pDoc->GetDlgDataSourceCount(); m++)
		{
			pDoc->GetDlgDataSource(m)->ClearLayerSymbolCache();
		}
	}


	CDlgScheme dlg;
	dlg.InitMems(m_config,m_config.GetScale());
	dlg.m_pDoc = pDoc;
	dlg.SetSelectedLayerIndex(m_strCurNameAndCode);
	dlg.DoModal();

	if( pDoc )
	{
		for( int m=0; m<pDoc->GetDlgDataSourceCount(); m++)
		{
			pDoc->GetDlgDataSource(m)->LoadLayerSymbolCache();
		}
	}
	
	if (dlg.IsModify())
	{
		//FillCollectionView(0,&dlg.m_config);
		
		if (pDoc) 
		{
			pDoc->GetDlgDataSource()->UpdateFtrQuery();
			pDoc->UpdateAllViews(NULL,hc_UpdateAllObjects);
		}
		
	}
	
}

void CCollectionViewBar::OnPreview()
{
	CDlgSearchCodeDlg dlg;
	dlg.SetScheme(m_config.pScheme,m_config.GetScale());
	if ( dlg.DoModal() != IDOK )
		return;

	CString name = dlg.GetSelName();

	// 激发特征缺省命令
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if (!pDoc) return;
	
	if (pDoc->ActiveLayer(name))
	{
	}
	else
	{
		AfxMessageBox(IDS_ERR_ACTIVELAYER_FAILED);
	}
}

void CCollectionViewBar::OnListSelChanged()
{
	m_bReturn = TRUE;
	m_bUpDown = FALSE;
	m_wndSearchRsltList.ShowWindow(SW_HIDE);

	CString str;
	int idx = m_wndSearchRsltList.GetCurSel();
	m_wndSearchRsltList.GetText(idx, str);
		
	char layName[_MAX_FNAME];
	char num[_MAX_FNAME];
	sscanf(str,"%s %s",&num,layName);
				
	m_bCanSelectDefault = TRUE;					
	m_wndCodeEdit.SetWindowText(num);
	m_bCanSelectDefault = FALSE;

	m_bReturn = FALSE;
}

void CCollectionViewBar::OnNcPaint()
{
	CRect rect;
	GetWindowRect(&rect);

	CPoint pt;
	GetCursorPos(&pt);

	if (!rect.PtInRect(pt))
	{
// 		m_bUpDown = FALSE;
// 		m_wndSearchRsltList.ShowWindow(SW_HIDE);
	}	

	CDockablePane::OnNcPaint();
}

void CCollectionViewBar::OnSetFocusEdit()
{
	this->SetFocus();
	m_wndCodeEdit.SetFocus();
	m_wndCodeEdit.SetWindowText(_T(""));
}

#include "DlgCommand.h"

void CCollectionViewBar::OnModifyLayer()
{
	char layName[_MAX_FNAME];
	int num;
	sscanf(m_strCurNameAndCode,"%i %s",&num,layName);
	CString name(layName);
	int pos = name.Find("^");
	if(pos>0)
	{
		name = name.Left(pos);
	}

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return;
	int numsel;	
	const FTR_HANDLE* handles = pDoc->GetSelection()->GetSelectedObjs(numsel);
	if(numsel>0)
	{
		CModifyLayerCommand cmd;
		cmd.Init(pDoc);
		cmd.m_strLayer = name;
		cmd.ModifyLayer(handles, numsel);
		pDoc->DeselectAll();
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_MODIFY_LAYER, 0);
		CCommand* pCurCmd = pDoc->GetCurrentCommand();
		if(pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CModifyLayerCommand)))
		{
			CValueTable tab;
			tab.BeginAddValueItem();
			_variant_t var;
			var = (_bstr_t)(LPCTSTR)name;
			tab.AddValue("Layer",&CVariantEx(var));
			tab.EndAddValueItem();
			pCurCmd->SetParams(tab, TRUE);
		}
	}
}

void CCollectionViewBar::OnModifyLayer1()
{
	char layName[_MAX_FNAME];
	int num;
	sscanf(m_strCurNameAndCode,"%i %s",&num,layName);
	CString name(layName);
	int pos = name.Find("^");
	if(pos>0)
	{
		name = name.Left(pos);
	}
	
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return;
	int numsel;	
	const FTR_HANDLE* handles = pDoc->GetSelection()->GetSelectedObjs(numsel);
	if(numsel>0)
	{
		CModifyLayer1Command cmd;
		cmd.Init(pDoc);
		cmd.m_strLayer = name;
		cmd.ModifyLayer(handles, numsel);
		pDoc->DeselectAll();
	}
	else
	{
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_MODIFY_LAYER1, 0);
		CCommand* pCurCmd = pDoc->GetCurrentCommand();
		if(pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CModifyLayerCommand)))
		{
			CValueTable tab;
			tab.BeginAddValueItem();
			_variant_t var;
			var = (_bstr_t)(LPCTSTR)name;
			tab.AddValue("Layer",&CVariantEx(var));
			tab.EndAddValueItem();
			pCurCmd->SetParams(tab, TRUE);
		}
	}
}

void CCollectionViewBar::OnSwitchCommon()
{
	m_bCommonUse = !m_bCommonUse;
	AfxGetApp()->WriteProfileInt("Config","CollectionViewBarCommon", m_bCommonUse);

	FillCollectionView(0,&m_config);
}

void CCollectionViewBar::OnAddToCommon()
{
	HTREEITEM hSel = m_wndCollectionView.GetSelectedItem();
	DWORD_PTR dwData = m_wndCollectionView.GetItemData(hSel);
	CString layname = m_UserIdx.m_aIdx[dwData].FeatureName;

	CString path = GetConfigPath(FALSE) + "\\CommonLayers.txt";
	FILE *fp = fopen((LPCTSTR)path, "r");
	if(fp)
	{
		char line[1024];
		while(!feof(fp))
		{
			memset(line,0,sizeof(line));
			fgets(line,sizeof(line)-1,fp);
			if(strlen(line)<=0)  continue;
			
			CString str(line);
			str.TrimLeft();
			str.TrimRight();
			if(layname==str) return;
		}
		fclose(fp);
		fp = NULL;
	}
	fp = fopen((LPCTSTR)path, "a");
	if(fp)
	{
		fprintf(fp, "%s\n", layname);
		fclose(fp);
		fp = NULL;
	}
}

void CCollectionViewBar::OnDeleteFromCommon()
{
	HTREEITEM hSel = m_wndCollectionView.GetSelectedItem();
	DWORD_PTR dwData = m_wndCollectionView.GetItemData(hSel);
	CString layname = m_UserIdx.m_aIdx[dwData].FeatureName;

	for(int i=0; i<m_arrCommonLayers.GetSize(); i++)
	{
		if(layname==m_arrCommonLayers[i])
		{
			m_arrCommonLayers.RemoveAt(i);
		}
	}

	CString path = GetConfigPath(FALSE) + "\\CommonLayers.txt";
	FILE *fp = fopen((LPCTSTR)path, "w");
	if(fp)
	{
		for(int i=0; i<m_arrCommonLayers.GetSize(); i++)
		{
			fprintf(fp, "%s\n", m_arrCommonLayers[i]);
		}
		fclose(fp);
		fp = NULL;
	}

	FillCollectionView(0,&m_config);
}


void CCollectionViewBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	CDockablePane::OnNcCalcSize(bCalcValidRects, lpncsp);
}


HBRUSH CCollectionViewBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// TODO:  Return a different brush if the default is not desired
	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	// TODO:  Return a different brush if the default is not desired
	return m_brush;
}
