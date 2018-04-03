// GeometryViewBar.cpp: implementation of the CGeometryViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "GeometryViewBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DATASOURCEITEM_ID		1
#define LAYERITEM_ID			2
#define POINTITEM_ID			3
#define CURVEITEM_ID			4
#define SURFACEITEM_ID			5
#define TEXTITEM_ID				6
/*
//##ModelId=41466B7F029F
class CGeometryViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CGeometryViewBar;

	DECLARE_SERIAL(CGeometryViewMenuButton)

public:
	//##ModelId=41466B7F02AF
	CGeometryViewMenuButton(HMENU hMenu = NULL) :
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

IMPLEMENT_SERIAL(CGeometryViewMenuButton, CMFCToolBarMenuButton, 1)
*/
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//##ModelId=41466B7F00CC
CGeometryViewBar::CGeometryViewBar()
{
// 	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
// 	m_pData = NULL;
}

//##ModelId=41466B7F00CD
CGeometryViewBar::~CGeometryViewBar()
{
}

BEGIN_MESSAGE_MAP(CGeometryViewBar, CDockablePane)
	//{{AFX_MSG_MAP(CGeometryViewBar)
// 	ON_WM_CREATE()
// 	ON_WM_SIZE()
// 	ON_WM_CONTEXTMENU()
// 	ON_WM_PAINT()
	//}}AFX_MSG_MAP
// 	ON_COMMAND_RANGE(ID_LAYERITEM_CREATE, ID_LAYERITEM_MODIFY, OnLayerEdit)
// 	ON_UPDATE_COMMAND_UI_RANGE(ID_LAYERITEM_CREATE, ID_LAYERITEM_MODIFY, OnUpdateLayerEdit)
// 	ON_COMMAND(ID_LAYERITEM_CREATEANNOT, OnCreateAnnot)
// 	ON_COMMAND(ID_LAYERITEM_MODIFYANNOT, OnCreateAnnot)
// 	ON_UPDATE_COMMAND_UI(ID_LAYERITEM_CREATEANNOT, OnUpdateLayerEdit)
// 	ON_UPDATE_COMMAND_UI(ID_LAYERITEM_MODIFYANNOT, OnUpdateLayerEdit)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeometryViewBar message handlers
// int CGeometryViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
// {
// 	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
// 		return -1;
// 	
// 	CRect rectDummy;
// 	rectDummy.SetRectEmpty ();
// 
// 	// Create views:
// 	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES |
// 								TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS |
// 								WS_CLIPCHILDREN;
// 	
// 	if (!m_wndGeometryView.Create (dwViewStyle, rectDummy, this, 2))
// 	{
// 		TRACE0("Failed to create Class View\n");
// 		return -1;      // fail to create
// 	}
// 
// 	// Load images:
// 	m_GeometryViewImages.Create (IDB_CLASS_VIEW, 16, 0, RGB (255, 0, 0));
// 	m_wndGeometryView.SetImageList (&m_GeometryViewImages, TVSIL_NORMAL);
// 
// 	return 0;
// }
// 
// //##ModelId=41466B7F0127
// void CGeometryViewBar::OnSize(UINT nType, int cx, int cy) 
// {
// 	CDockablePane::OnSize(nType, cx, cy);
// 	AdjustLayout ();
// }
// 
// HTREEITEM CGeometryViewBar::InsertRoot(LPCTSTR lpszItem, DWORD data)
// {
// 	HTREEITEM hRoot = m_wndGeometryView.InsertItem (lpszItem,0,0);
// 	m_wndGeometryView.SetItemState (hRoot, TVIS_BOLD, TVIS_BOLD);
// 	m_wndGeometryView.SetItemData (hRoot, data);
// 	
// 	return hRoot;
// }
// 
// HTREEITEM CGeometryViewBar::InsertSubRoot(LPCTSTR lpszItem,HTREEITEM hParent, DWORD data,HTREEITEM hInsertAfter)
// { 
// 	HTREEITEM hSubRoot = m_wndGeometryView.InsertItem (lpszItem, 1, 1, hParent,hInsertAfter);
// 	m_wndGeometryView.SetItemData(hSubRoot, data);
// 	
// 	return hSubRoot;
// }
// 
// HTREEITEM CGeometryViewBar::InsertItem(LPCTSTR lpszItem,HTREEITEM hParent,DWORD dwData)
// {
// 	HTREEITEM hItem = m_wndGeometryView.InsertItem (lpszItem, 3, 5, hParent);
// 	m_wndGeometryView.SetItemData(hItem, dwData);
// 	
// 	return hItem;
// }
// 
// 
// void CGeometryViewBar::SetItemImage(HTREEITEM item, DWORD flag)
// {
// 	int idx = -1;
// 	switch( flag )
// 	{
// 	case DATASOURCEITEM_ID: idx = 0; break;
// 	case LAYERITEM_ID: idx = 4; break;
// 	case POINTITEM_ID: idx = 3; break;
// 	case CURVEITEM_ID: idx = 12; break;
// 	case SURFACEITEM_ID: idx = 15; break;
// 	case TEXTITEM_ID: idx = 16; break;
// 	case CONDITION_ID: idx = 5; break;
// 	case FEATURE_ID: idx = 7; break;
// 	}
// 	
// 	if( idx>=0 )
// 		m_wndGeometryView.SetItemImage(item,idx,idx);
// }
// 
// void CGeometryViewBar::FillGeometryView (CGeoData *pData)
// {
// 	m_wndGeometryView.DeleteAllItems();
// 	if (pData==NULL) return;
// 
// 	m_pData = pData;
// 	m_wndGeometryView.m_pData = pData;
// 
// 	int LayerNum = pData->GetLayersNum();
// 	
// 	HTREEITEM hPoint	= InsertRoot(StrFromResID(IDS_LAYER_POINT), 0);
// 	HTREEITEM hCurve	= InsertRoot(StrFromResID(IDS_LAYER_LINE), 0);
// 	HTREEITEM hSurface	= InsertRoot(StrFromResID(IDS_LAYER_SURFACE), 0);
// 	HTREEITEM hText		= InsertRoot(StrFromResID(IDS_LAYER_TEXT), 0);
// 	HTREEITEM hOther	= InsertRoot(StrFromResID(IDS_LAYER_OTHER), 0);
// 
// 	HTREEITEM hItem;
// 
// 	// data==1 点线面根节点
// 	// data==2 符号配置根节点
// 	// data==3 符号配置节点
// 	// data==4 特征快捷节点
// 	// data==5 注记符号配置节点
// 	for (int i=0;i<LayerNum;i++)
// 	{
// 		CString str;
// 		CGeoLayer *pLayer = pData->GetLayerByIndex(i);
// 		
// 		str.Format("%s",_T(pLayer->GetName()));
// 		switch(pLayer->GetLayerType())
// 		{
// 		case GEO_Point:
// 			hItem = InsertSubRoot(_T(str), hPoint,1);
// 			SetItemImage(hItem,POINTITEM_ID);
// 			break;
// 		case GEO_Line:
// 			hItem = InsertSubRoot(_T(str), hCurve,1);
// 			SetItemImage(hItem,CURVEITEM_ID);
// 			break;		
// 		case GEO_Polygon:
// 			hItem = InsertSubRoot(_T(str), hSurface,1);
// 			SetItemImage(hItem,SURFACEITEM_ID);
// 			break;
// 		case GEO_Annotation:
// 			hItem = InsertSubRoot(_T(str), hText,1);
// 			SetItemImage(hItem,TEXTITEM_ID);
// 			break;
// 		default:
// 			hItem = InsertSubRoot(_T(str), hOther,1);
// 			SetItemImage(hItem,LAYERITEM_ID);
// 		}
// 		
// 		// 符号配置信息
// 		HTREEITEM hItemS = InsertSubRoot(StrFromResID(IDS_CONFIG_SYMBOL), hItem,2);
// 		SetItemImage(hItemS, DATASOURCEITEM_ID);
// 		
// 		CArray<CONDITION,CONDITION&> *pConditions=NULL;
// 		pConditions= pLayer->GetConditions();
// 		for (int i=0;i<pConditions->GetSize();i++)
// 		{
// 			CONDITION cond = pConditions->GetAt(i);
// 			str.Format("%s(%s=%s)",cond.symname,cond.field,cond.value);
// 			HTREEITEM hCon;
// 			if (cond.ntype==1)
// 				hCon = InsertSubRoot(_T(str), hItemS,5);
// 			else
// 				hCon = InsertSubRoot(_T(str), hItemS,3);
// 			SetItemImage(hCon, CONDITION_ID);
// 		}
// 
// 		FTRCLASSLIST *pList = &pLayer->m_FtrClsList;
// 		CDpDBVariant var;
// 		DpCustomFieldType type;
// 		for (i=0;i<pList->List.GetSize();i++)
// 		{
// 			FTRCLASS ftrcls = pList->List.GetAt(i);
// 
// 			ftrcls.pGDX->GetAttrValue("FID",var,type);
// 			HTREEITEM hCon = InsertSubRoot(_T(var.m_pString),hItem,4);
// 			SetItemImage(hCon, FEATURE_ID);
// 		}
// 	}
// }
// 
// void CGeometryViewBar::OnLayerEdit (UINT id)
// {
// 	HTREEITEM hItem = m_wndGeometryView.GetSelectedItem();
// 	DWORD data = m_wndGeometryView.GetItemData(hItem);
// 
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndGeometryView.GetItemText(hItem);
// 	CString LayerName;
// 
// 	if (data==2)
// 	{
// 		HTREEITEM hParent = m_wndGeometryView.GetParentItem(hItem);
// 		LayerName = m_wndGeometryView.GetItemText(hParent);		
// 	}
// 	else if (data==3 || data==5)
// 	{
// 		HTREEITEM hParent = m_wndGeometryView.GetParentItem(hItem);
// 		hParent = m_wndGeometryView.GetParentItem(hParent);
// 		LayerName = m_wndGeometryView.GetItemText(hParent);		
// 	}
// 	else
// 		return;
// 
// 	pLayer = m_pData->GetLayerByName(LPCTSTR(LayerName));
// 	pCond  = pLayer->GetConditions();
// 
// 	if (id==ID_LAYERITEM_DELETE)
// 	{
// 		if (data==3 || data==5) // 层符号条件
// 		{
// 			for (int i=0;i<pCond->GetSize();i++)
// 			{
// 				CONDITION cond = pCond->GetAt(i);
// 				CString str;
// 				str.Format("%s(%s=%s)",cond.symname,cond.field,cond.value);
// 				if (str==strText) 
// 				{
// 					pCond->RemoveAt(i);
// 					m_wndGeometryView.DeleteItem(hItem);
// 
// 					if( cond.ntype==1 )
// 					{
// 						for( int j=pLayer->m_arrAnnots.GetSize()-1; j>=0; j-- )
// 						{
// 							CAnnotation *pAnnot = (CAnnotation*)pLayer->m_arrAnnots[j];
// 							if( pAnnot->GetName().CompareNoCase(cond.symname)==0 )
// 							{
// 								delete pAnnot;
// 								pLayer->m_arrAnnots.RemoveAt(j);
// 							}
// 						}
// 					}
// 					pLayer->m_bConfigModified = TRUE;
// 					break;
// 				}
// 			}
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_CREATE)
// 	{
// 		switch(data)
// 		{
// 		case 2: // 层
// 			{
// 				CONDITION cond;
// 
// 				CDlgConditionEdit dlg(NULL,pLayer,pLayer->GetLayerType(),&(m_pData->m_SymbolLib));
// 				dlg.m_SymbolName = "NULL";
// 				dlg.m_FieldName.LoadString(IDS_FID);
// 				dlg.m_Value = "NULL";
// 				if (dlg.DoModal()==IDOK)
// 				{
// 					// 将别名转化为字段名
// 					strcpy(cond.field,LPCTSTR(pLayer->GetAttrFieldNameFromAlias(dlg.m_FieldName)));
// 					strcpy(cond.value,LPCTSTR(dlg.m_Value));
// 					strcpy(cond.symname,LPCTSTR(dlg.m_SymbolName));
// 					
// 					for (int i=0;i<pCond->GetSize();i++)
// 					{
// 						CONDITION cond1 = pCond->GetAt(i);
// 						if (stricmp(cond1.field,cond.field)==0x00 &&
// 							stricmp(cond1.value,cond.value)==0x00)
// 						{
// 							AfxMessageBox(IDS_COND_REDUNDANT);
// 							break;
// 						}
// 					}
// 					pCond->InsertAt(0,cond);
// 
// 					CString str;
// 					str.Format("%s(%s=%s)",cond.symname,cond.field,cond.value);
// 					HTREEITEM hCon = InsertSubRoot(_T(str), hItem,3,TVI_FIRST);
// 					SetItemImage(hCon, CONDITION_ID);
// 					
// 					pLayer->m_bConfigModified = TRUE;
// 				}
// 			}
// 			break;		
// 		}
// 	}
// 	else if (id==ID_LAYERITEM_MODIFY)
// 	{
// 		switch(data)
// 		{
// 		case 3: // 层符号条件
// 			for (int i=0;i<pCond->GetSize();i++)
// 			{
// 				CONDITION cond = pCond->GetAt(i);
// 				CString str;
// 				str.Format("%s(%s=%s)",cond.symname,cond.field,cond.value);
// 				if (str==strText)
// 				{
// 					if(cond.ntype==0)
// 					{
// 						CDlgConditionEdit dlg(NULL,pLayer,pLayer->GetLayerType(),&(m_pData->m_SymbolLib));
// 						
// 						dlg.m_SymbolName = cond.symname;
// 						dlg.m_Value = cond.value;
// 						dlg.m_FieldName = pLayer->GetAliasFromAttrFieldName(cond.field);
// 						
// 						if (dlg.DoModal()==IDOK)
// 						{
// 							// 将别名转化为字段名
// 							strcpy(cond.field,LPCTSTR(pLayer->GetAttrFieldNameFromAlias(dlg.m_FieldName)));
// 							strcpy(cond.value,LPCTSTR(dlg.m_Value));
// 							strcpy(cond.symname,LPCTSTR(dlg.m_SymbolName));
// 							pCond->SetAt(i,cond);
// 
// 							str.Format("%s(%s=%s)",cond.symname,cond.field,cond.value);
// 							m_wndGeometryView.SetItemText(hItem,str);
// 							
// 							pLayer->m_bConfigModified = TRUE;
// 							break;
// 						}
// 					}
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	}
// }
// 
// void CGeometryViewBar::OnUpdateLayerEdit(CCmdUI *pCmdUI)
// {
// 	HTREEITEM hItem = m_wndGeometryView.GetSelectedItem();
// 	DWORD data = m_wndGeometryView.GetItemData(hItem);
// 
// 	pCmdUI->Enable(FALSE);
// 	switch(data)
// 	{
// 	// 根节点
// 	case 1:
// 		break;
// 	// 层节点
// 	case 2:
// 		if( pCmdUI->m_nID==ID_LAYERITEM_CREATE ||
// 			pCmdUI->m_nID==ID_LAYERITEM_CREATEANNOT )
// 		{
// 			pCmdUI->Enable(TRUE);
// 		}
// 		break;
// 	// 层符号条件
// 	case 3:
// 	case 5:
// 		if (pCmdUI->m_nID==ID_LAYERITEM_DELETE)
// 		{
// 			pCmdUI->Enable(TRUE);
// 		}
// 		else if( pCmdUI->m_nID==ID_LAYERITEM_MODIFY )
// 		{
// 			if (data==3) pCmdUI->Enable(TRUE);
// 		}
// 		else if( pCmdUI->m_nID==ID_LAYERITEM_MODIFYANNOT )
// 		{
// 			if (data==5) pCmdUI->Enable(TRUE);
// 		}
// 		break;
// 	}
// }
// 
// 
// void CGeometryViewBar::OnCreateAnnot()
// {
// 	//有效性判别
// 	HTREEITEM hItem = m_wndGeometryView.GetSelectedItem();
// 	CString itemText = m_wndGeometryView.GetItemText(hItem);
// 	DWORD data = m_wndGeometryView.GetItemData(hItem);
// 	
// 	CGeoLayer *pLayer = NULL;
// 	CArray<CONDITION,CONDITION&> *pCond = NULL;
// 	CString strText = m_wndGeometryView.GetItemText(hItem);
// 	CString LayerName;
// 	
// 	UINT id = GetCurrentMessage()->wParam;
// 	if( id==ID_LAYERITEM_CREATEANNOT && data!=2 )return;
// 	if( id==ID_LAYERITEM_MODIFYANNOT && data!=5 )return;
// 
// 	//获得所选的层
// 	if (data==2)
// 	{
// 		HTREEITEM hParent = m_wndGeometryView.GetParentItem(hItem);
// 		LayerName = m_wndGeometryView.GetItemText(hParent);		
// 	}
// 	else if (data==5)
// 	{
// 		HTREEITEM hParent = m_wndGeometryView.GetParentItem(hItem);
// 		hParent = m_wndGeometryView.GetParentItem(hParent);
// 		LayerName = m_wndGeometryView.GetItemText(hParent);		
// 	}
// 
// 	pLayer = m_pData->GetLayerByName(LPCTSTR(LayerName));
// 
// 	if( !pLayer )return;
// 	pCond  = pLayer->GetConditions();
// 
// 	if( !pCond )return;
// 
// 	CDlgCreateAnnot dlg;
// 	dlg.m_pLayer = pLayer;
// 	dlg.m_bCreate = (id==ID_LAYERITEM_CREATEANNOT);
// 
// 	int pos = itemText.ReverseFind('(');
// 	if( pos>=0 )dlg.m_strAnnotName = itemText.Left(pos);
// 	int pos2= itemText.Find('=',pos);
// 	if( pos2>=0)dlg.m_strField = itemText.Mid(pos+1,pos2-pos-1);
// 	int pos3= itemText.Find(')',pos2);
// 	if( pos3>=0)dlg.m_strValue = itemText.Mid(pos2+1,pos3-pos2-1);
// 	
// 	if( dlg.DoModal()!=IDOK )return;
// 
// 	//条件冲突的检测...
// 	CONDITION cond;
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
// 				stricmp(cond1.value,cond.value)==0x00 )
// 			{
// 				AfxMessageBox(IDS_COND_REDUNDANT);
// 				break;
// 			}
// 		}
// 		pCond->Add(cond);
// 	}
// 	
// 	pLayer->m_bConfigModified = TRUE;
// 
// 	//命名冲突的检测...
// 	CAnnotation *pAnnot = NULL;
// 	BOOL bFindSameAnnot = FALSE;
// 	for( int i=pLayer->m_arrAnnots.GetSize()-1; i>=0; i--)
// 	{
// 		CAnnotation* pA = (CAnnotation*)pLayer->m_arrAnnots[i];
// 		if( pA && pA->GetName().CompareNoCase(dlg.m_strAnnotName)==0 )
// 		{
// 			pAnnot = pA;
// 			break;
// 		}
// 	}
// 	
// 	if( dlg.m_bCreate )
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
// 		if( !bFindSameAnnot )pLayer->m_arrAnnots.InsertAt(0,pAnnot);
// 
// 		CString str;
// 		str.Format("%s(%s=%s)",cond.symname,cond.field,cond.value);
// 		HTREEITEM hCon = InsertSubRoot(_T(str), hItem,5,TVI_FIRST);
// 		SetItemImage(hCon, CONDITION_ID);
// 	}
// }
// 
// 
// void CGeometryViewBar::OnContextMenu(CWnd* pWnd, CPoint point) 
// {
// 	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndGeometryView;
// 	ASSERT_VALID (pWndTree);
// 
// 	if (point != CPoint (-1, -1))
// 	{
// 		//---------------------
// 		// Select clicked item:
// 		//---------------------
// 		CPoint ptTree = point;
// 		pWndTree->ScreenToClient (&ptTree);
// 
// 		HTREEITEM hTreeItem = pWndTree->HitTest (ptTree);
// 		if (hTreeItem != NULL)
// 		{
// 			pWndTree->SelectItem (hTreeItem);
// 		}
// 	}
// 
// 	pWndTree->SetFocus ();
// 	CMenu menu;
// 	menu.LoadMenu (IDR_POPUP_LAYER_ITEM);
// 
// 	CMenu* pSumMenu = menu.GetSubMenu(0);
// 
// 	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
// 	{
// 	   CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
// 	   
// 	   if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
// 		   return;
// 	   
// 	   ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu (pPopupMenu);
// 	   UpdateDialogControls(this, FALSE);
// 	}
// }
// 
// void CGeometryViewBar::AdjustLayout ()
// {
// 	if (GetSafeHwnd () == NULL)
// 	{
// 		return;
// 	}
// 
// 	CRect rectClient;
// 	GetClientRect (rectClient);
// 
// 	int cyTlb = 0; //m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;
// 
// 	m_wndGeometryView.SetWindowPos (NULL, rectClient.left + 1, rectClient.top + cyTlb + 1,
// 								rectClient.Width () - 2, rectClient.Height () - cyTlb - 2,
// 								SWP_NOACTIVATE | SWP_NOZORDER);
// }
// 
// BOOL CGeometryViewBar::PreTranslateMessage(MSG* pMsg) 
// {
// 	return CDockablePane::PreTranslateMessage(pMsg);
// }
// 
// void CGeometryViewBar::OnPaint() 
// {
// 	CPaintDC dc(this); // device context for painting
// 	
// 	CRect rectTree;
// 	m_wndGeometryView.GetWindowRect (rectTree);
// 	ScreenToClient (rectTree);
// 
// 	rectTree.InflateRect (1, 1);
// 	dc.Draw3dRect (rectTree, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
// }
BEGIN_MESSAGE_MAP(CClassToolBar, CMFCToolBar)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


HBRUSH CClassToolBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CMFCToolBar::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	pDC->SetTextColor(RGB(255, 255, 255));
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	return m_brush;

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}





void CClassToolBar::DoPaint(CDC* pDCPaint)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDCPaint);

	CRect rectClip;
	pDCPaint->GetClipBox(rectClip);

	BOOL bHorz = GetCurrentAlignment() & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	CRect rectClient;
	GetClientRect(rectClient);

	CMemDC memDC(*pDCPaint, this);
	CDC* pDC = &memDC.GetDC();

	if ((GetStyle() & TBSTYLE_TRANSPARENT) == 0)
	{
		/*CMFCVisualManager::GetInstance()->OnFillBarBackground(pDC, this, rectClient, rectClip);*/
		CRect rect;
		GetClientRect(rect);
		//填充客户区   
		pDC->FillSolidRect(rect, RGB(50, 50, 50));
	}
	else
	{
		/*m_Impl.GetBackgroundFromParent(pDC);*/
	}

	OnFillBackground(pDC);

	pDC->SetTextColor(GetGlobalData()->clrBtnText);
	pDC->SetBkMode(TRANSPARENT);

	CRect rect;
	GetClientRect(rect);

	// Force the full size of the button:
	if (bHorz)
	{
		rect.bottom = rect.top + GetRowHeight();
	}
	else
	{
		rect.right = rect.left + GetColumnWidth();
	}

	BOOL bDontScaleImages = m_bLocked ? m_bDontScaleLocked : m_bDontScaleImages;
	const double dblImageScale = bDontScaleImages ? 1.0 : GetGlobalData()->GetRibbonImageScale();

	CMFCToolBarImages* pImages = GetImageList(m_Images, m_ImagesLocked, m_LargeImages, m_LargeImagesLocked);
	CMFCToolBarImages* pHotImages = pImages;
	CMFCToolBarImages* pColdImages = GetImageList(m_ColdImages, m_ColdImagesLocked, m_LargeColdImages, m_LargeColdImagesLocked);
	CMFCToolBarImages* pDisabledImages = GetImageList(m_DisabledImages, m_DisabledImagesLocked, m_LargeDisabledImages, m_LargeDisabledImagesLocked);
	CMFCToolBarImages* pMenuImages = !m_bLocked ? &m_MenuImages : &m_MenuImagesLocked;
	CMFCToolBarImages* pDisabledMenuImages = !m_bLocked ? &m_DisabledMenuImages : &m_DisabledMenuImagesLocked;

	BOOL bDrawImages = pImages->IsValid();

	pHotImages->SetTransparentColor(GetGlobalData()->clrBtnFace);

	BOOL bFadeInactiveImages = CMFCVisualManager::GetInstance()->IsFadeInactiveImage();

	CSize sizeImageDest = m_bMenuMode ? m_sizeMenuImage : GetImageSize();
	if (dblImageScale != 1.)
	{
		if (m_bMenuMode && sizeImageDest == CSize(-1, -1))
		{
			sizeImageDest = GetImageSize();

			if (dblImageScale > 1. && m_bLargeIconsAreEnbaled)
			{
				sizeImageDest = m_sizeImage;
			}
		}

		sizeImageDest = CSize((int)(.5 + sizeImageDest.cx * dblImageScale), (int)(.5 + sizeImageDest.cy * dblImageScale));
	}

	CAfxDrawState ds;
	if (bDrawImages)
	{
		if (dblImageScale != 1.0 && pHotImages->GetScale() == 1.0)
		{
			pHotImages->SmoothResize(dblImageScale);
		}

		if (!pHotImages->PrepareDrawImage(ds, sizeImageDest, bFadeInactiveImages))
		{
			return;     // something went wrong
		}
	}

	CFont* pOldFont;
	if (bHorz)
	{
		pOldFont = SelectDefaultFont(pDC);
	}
	else
	{
		pOldFont = (CFont*)pDC->SelectObject(&(GetGlobalData()->fontVert));
	}

	if (pColdImages->GetCount() > 0)
	{
		// Disable fade effect for inactive buttons:
		CMFCVisualManager::GetInstance()->SetFadeInactiveImage(FALSE);
	}

	// Draw buttons:
	int iButton = 0;
	for (POSITION pos = m_Buttons.GetHeadPosition(); pos != NULL; iButton++)
	{
		CMFCToolBarButton* pButton = (CMFCToolBarButton*)m_Buttons.GetNext(pos);
		if (pButton == NULL)
		{
			break;
		}

		ASSERT_VALID(pButton);

		rect = pButton->Rect();
		CRect rectInter;

		if (pButton->m_nStyle & TBBS_SEPARATOR)
		{
			BOOL bHorzSeparator = bHorz;
			CRect rectSeparator; rectSeparator.SetRectEmpty();

			OnCalcSeparatorRect(pButton, rectSeparator, bHorz);

			if (pButton->m_bWrap && bHorz)
			{
				bHorzSeparator = FALSE;
			}

			if (rectInter.IntersectRect(rectSeparator, rectClip) && !pButton->IsHidden())
			{
				DrawSeparator(pDC, rectSeparator, bHorzSeparator);
			}

			continue;
		}

		if (!rectInter.IntersectRect(rect, rectClip))
		{
			continue;
		}

		BOOL bHighlighted = IsButtonHighlighted(iButton);
		BOOL bDisabled = (pButton->m_nStyle & TBBS_DISABLED) && !IsCustomizeMode();

		if (pDC->RectVisible(&rect))
		{
			BOOL bDrawDisabledImages = FALSE;

			if (bDrawImages)
			{
				CMFCToolBarImages* pNewImages = NULL;

				if (pButton->m_bUserButton)
				{
					if (pButton->GetImage() >= 0)
					{
						pNewImages = m_pUserImages;
					}
				}
				else
				{
					if (m_bMenuMode)
					{
						if (bDisabled && pDisabledMenuImages->GetCount() > 0)
						{
							bDrawDisabledImages = TRUE;
							pNewImages = pDisabledMenuImages;
						}
						else if (pMenuImages->GetCount() > 0)
						{
							pNewImages = pMenuImages;
						}
						else
						{
							bDrawDisabledImages = (bDisabled && pDisabledImages->GetCount() > 0);
							pNewImages = bDrawDisabledImages ? pDisabledImages : pHotImages;
						}
					}
					else // Toolbar mode
					{
						bDrawDisabledImages = (bDisabled && pDisabledImages->GetCount() > 0);
						pNewImages = bDrawDisabledImages ? pDisabledImages : pHotImages;

						if (!bHighlighted && !bDrawDisabledImages && (pButton->m_nStyle & TBBS_PRESSED) == 0 && pColdImages->GetCount() > 0 && !pButton->IsDroppedDown())
						{
							pNewImages = pColdImages;
						}
					}
				}

				if (bDrawImages && pNewImages != pImages && pNewImages != NULL)
				{
					pImages->EndDrawImage(ds);

					pNewImages->SetTransparentColor(GetGlobalData()->clrBtnFace);
					if (dblImageScale != 1.0 && pNewImages->GetScale() == 1.0)
					{
						pNewImages->SmoothResize(dblImageScale);
					}

					pNewImages->PrepareDrawImage(ds, sizeImageDest, bFadeInactiveImages);

					pImages = pNewImages;
				}
			}

			DrawButton(pDC, pButton, bDrawImages ? pImages : NULL, bHighlighted, bDrawDisabledImages);
		}
	}

	// Highlight selected button in the toolbar customization mode:
	if (m_iSelected >= m_Buttons.GetCount())
	{
		m_iSelected = -1;
	}

	if ((IsCustomizeMode() || m_bAltCustomizeMode) && m_iSelected >= 0 && !m_bLocked && m_pSelToolbar == this)
	{
		CMFCToolBarButton* pSelButton = GetButton(m_iSelected);
		ENSURE(pSelButton != NULL);

		if (pSelButton != NULL && pSelButton->CanBeStored())
		{
			CRect rectDrag1 = pSelButton->Rect();

			pDC->Draw3dRect(&rectDrag1, GetGlobalData()->clrBtnText, GetGlobalData()->clrBtnText);
			rectDrag1.DeflateRect(1, 1);
			pDC->Draw3dRect(&rectDrag1, GetGlobalData()->clrBtnText, GetGlobalData()->clrBtnText);
		}
	}

	if (IsCustomizeMode() && m_iDragIndex >= 0 && !m_bLocked)
	{
		DrawDragCursor(pDC);
	}

	pDC->SelectObject(pOldFont);

	if (bDrawImages)
	{
		pImages->EndDrawImage(ds);
	}

	CMFCVisualManager::GetInstance()->SetFadeInactiveImage(bFadeInactiveImages);
}
