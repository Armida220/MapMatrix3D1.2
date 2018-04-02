// DlgSearchResult.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgSearchResult.h"
#include "editbasedoc.h"
#include "Linearizer.h"
#include "SmartViewFunctions.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "ExMessage.h"
#include "DlgMessageBox.h"
#include "DlgCommand.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void Copy_CP_ATTR(const CP_ATTR *a, CP_ATTR *b)
{
	b->index = a->index;
	b->value = a->value;
}

BOOL Comp_CP_ATTR(const CP_ATTR *a, const CP_ATTR *b)
{
	return (a->index==b->index && a->value==b->value);
}

BOOL Find_CP_ARR(const CArray<CP_ATTR,CP_ATTR>* arr, const CP_ATTR *a)
{
	int nsize = arr->GetSize();
	CP_ATTR t;
	for( int i=0; i<nsize; i++)
	{
		t = arr->GetAt(i);
		if( t.index==a->index && t.value==a->value )
			return TRUE;
	}
	return FALSE;
}

CDocument *GetCurDocument()
{
	CWnd *pWnd = AfxGetMainWnd();
	if( pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		CMDIFrameWnd *pMain = (CMDIFrameWnd*)pWnd;
		CMDIChildWnd *pChild= pMain->MDIGetActive();
		if( pChild )
		{
			CView *pView = pChild->GetActiveView();
			if( pView )
				return pView->GetDocument();
		}
	}
	else if( pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		CFrameWnd *pMain = (CFrameWnd*)pWnd;
		return pMain->GetActiveDocument();
	}
	return NULL;
}



CFeature* MarkErrorPt(CDlgDoc *pDoc, PT_3D pt, double lfMarkWid)
{
	CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));
	if( !pLayer )
	{
		pLayer = pDoc->GetDlgDataSource()->CreateFtrLayer(StrFromResID(IDS_MARKLAYER_NAME));

		if(!pDoc->AddFtrLayer(pLayer)) return NULL;
	}
	pLayer->EnableLocked(TRUE);
	int scale = pDoc->GetDlgDataSource()->GetScale();

	
// 		CDpDBVariant var;
// 		var = (bool)true;
// 		pGDX->SetAttrValue("VISI",var,DP_CFT_BOOL);
// 		var = (long)RGB(255,0,0);
// 		pGDX->SetAttrValue("COLOR",var,DP_CFT_COLOR);
	
	
	CFeature *pFtr = pLayer->CreateDefaultFeature(scale,CLS_GEODCURVE);
	if( !pFtr )
	{
		return NULL;
	}
	
	CGeometry *pObj = pFtr->GetGeometry();

	if (!pObj)
	{
		return NULL;
	}

	pObj->SetColor(RGB(255,0,0));

	lfMarkWid = lfMarkWid*scale*0.001;
	
//	pFtr->SetFID(StrFromResID(IDS_DEFLAYER_ERROR));
	
	CArray<PT_3DEX,PT_3DEX> pts;
	PT_3DEX expt;
	COPY_3DPT(expt,pt);
	
	expt.x = pt.x-lfMarkWid;  expt.y = pt.y-lfMarkWid; expt.pencode = penMove;
	pts.Add(expt);
	expt.x = pt.x+lfMarkWid;  expt.y = pt.y+lfMarkWid; expt.pencode = penLine;
	pts.Add(expt);

	//移动中心点
	//expt.x = pt.x; expt.y = pt.y; expt.pencode = penLine;
	//pts.Add(expt);
	
	expt.x = pt.x-lfMarkWid;  expt.y = pt.y+lfMarkWid; expt.pencode = penMove;
	pts.Add(expt);
	expt.x = pt.x+lfMarkWid;  expt.y = pt.y-lfMarkWid; expt.pencode = penLine;
	pts.Add(expt);

	pObj->CreateShape(pts.GetData(),pts.GetSize());

	pDoc->AddObject(pFtr,pLayer->GetID());

	return pFtr;
}


/////////////////////////////////////////////////////////////////////////////
// CDlgSearchResult dialog


CDlgSearchResult::CDlgSearchResult(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSearchResult::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSearchResult)
	//}}AFX_DATA_INIT
}


void CDlgSearchResult::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSearchResult)
	DDX_Control(pDX, IDC_COMBO_TYPE, m_wndComboType);
	DDX_Control(pDX, IDC_TREE_SEARCHRESULT, m_wndResultTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSearchResult, CDialog)
	//{{AFX_MSG_MAP(CDlgSearchResult)
	ON_WM_TIMER()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SEARCHRESULT, OnSelchangedTree)
	ON_COMMAND(ID_RESULT_CLEAR, OnResultClear)
	ON_COMMAND(ID_RESULT_DELALLPT, OnResultDelallpt)
	ON_COMMAND(ID_RESULT_DELOBJPT, OnResultDelobjpt)
	ON_COMMAND(ID_RESULT_DELPT, OnResultDelpt)
	ON_COMMAND(ID_RESULT_MARKALL, OnResultMarkall)
	ON_COMMAND(ID_RESULT_MARKOBJPT, OnResultMarkobjpt)
	ON_COMMAND(ID_RESULT_MARKPT, OnResultMarkpt)
	ON_COMMAND(ID_RESULT_SELALL, OnResultSelall)
	ON_COMMAND(ID_RESULT_SELOBJ, OnResultSelobj)
	ON_COMMAND(ID_RESULT_UNSELALL, OnResultUnselall)
	ON_COMMAND(ID_RESULT_UNSELOBJ, OnResultUnselobj)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnNeedTipText)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, OnSelchangeComboType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSearchResult message handlers


BOOL CDlgSearchResult::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_wndTool.CreateEx(this,TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_TOOLTIPS,
		CRect(0,0,0,0));
	m_wndTool.LoadToolBar(IDR_TOOLBAR_RESULT);
	m_wndTool.ShowWindow(SW_SHOW);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,0);

	m_wndComboType.AddString(StrFromResID(IDS_ALL));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void AttrToText(CP_ATTR *attr, CStringArray& arrStr, char *text)
{
	switch(attr->value.vt) 
	{
	case VT_UI1:
		sprintf(text,"%s[%u] ",(LPCTSTR)arrStr.GetAt(attr->index),(BYTE)attr->value);
		break;
	case VT_UI2:
		sprintf(text,"%s[%u] ",(LPCTSTR)arrStr.GetAt(attr->index),attr->value.uiVal);
		break;
	case VT_UI4:
		sprintf(text,"%s[%u] ",(LPCTSTR)arrStr.GetAt(attr->index),attr->value.ulVal);
		break;
	case VT_UINT:
		sprintf(text,"%s[%u] ",(LPCTSTR)arrStr.GetAt(attr->index),attr->value.uintVal);
		break;
	case VT_I1:
		sprintf(text,"%s[%d] ",(LPCTSTR)arrStr.GetAt(attr->index),attr->value.cVal);
		break;
	case VT_I2:
		sprintf(text,"%s[%d] ",(LPCTSTR)arrStr.GetAt(attr->index),(short)attr->value);
		break;	
	case VT_I4:
		sprintf(text,"%s[%d] ",(LPCTSTR)arrStr.GetAt(attr->index),(long)attr->value);
		break;
	case VT_INT:
		sprintf(text,"%s[%d] ",(LPCTSTR)arrStr.GetAt(attr->index),attr->value.intVal);
		break;
	case VT_BOOL:
		sprintf(text,"%s[%d] ",(LPCTSTR)arrStr.GetAt(attr->index),(int)(bool)attr->value);
		break;
	case VT_R4:
		sprintf(text,"%s[%.4f] ",(LPCTSTR)arrStr.GetAt(attr->index),(float)attr->value);
		break;
	case VT_R8:
		sprintf(text,"%s[%.4f] ",(LPCTSTR)arrStr.GetAt(attr->index),(double)attr->value);
		break;
	case VT_BSTR:
		sprintf(text,"%s[%s] ",(LPCTSTR)arrStr.GetAt(attr->index),(LPCTSTR)(_bstr_t)attr->value);
		break;
	default:
		text[0] = 0;
		break;
	}
}


void CDlgSearchResult::FillTree()
{
	if( !m_pDoc )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

	HTREEITEM hRoot0, hRoot1;
	char text[4096], text1[4096], textt[256];
	int nr = m_lstResult.resultList.GetSize(), data0;

	int nType = m_wndComboType.GetCurSel();
	if( nType==0 )
	{
		data0 = 0;
		for( int i=0; i<nr; i++)
		{
			CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
			if( !r )continue;
			int np = r->arrPoints.GetSize();

			CGeometry *pObj = r->m_pFtr->GetGeometry();
			if( !pObj )
			{
				data0 += (np+1);
				continue;
			}
			
			//对象的节点
			memset(text,0,sizeof(text));
			sprintf(text,"Object %d:",i);
			
			//加上属性信息
			int na = r->arrAttrs.GetSize();
			for( int k=0; k<na; k++)
			{
				CP_ATTR attr = r->arrAttrs.GetAt(k);
				AttrToText(&attr,m_lstResult.attrNameList,textt);
				strcat(text,textt);
			}
			
			//已经被删除
			if( m_arrData.GetAt(data0).bDeleted )
			{
				data0 += (np+1);
				continue;
			}

			hRoot0 = m_wndResultTree.InsertItem(text);

			m_wndResultTree.SetItemData(hRoot0, (DWORD_PTR)data0);
			
			//数据点的节点
			for( int j=0; j<np; j++)
			{
				PT_3DEX expt;
				CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
				expt = pObj->GetDataPoint(pt->ptidx);
				
				memset(text,0,sizeof(text));
				sprintf(text,"POINT(%d)[%.4f,%.4f,%.4f]:",pt->ptidx,expt.x,expt.y,expt.z);

				//已经被删除
				if( m_arrData.GetAt(data0+j+1).bDeleted )
					continue;
				
				hRoot1 = m_wndResultTree.InsertItem(text,hRoot0);
				m_wndResultTree.SetItemData(hRoot1, (DWORD_PTR)(data0 + j + 1));
				
				//加上属性信息
				memset(text,0,sizeof(text));
				strcat(text,"  ");
				na = pt->arrAttrs.GetSize();
				for( k=0; k<na; k++)
				{
					CP_ATTR attr = pt->arrAttrs.GetAt(k);
					AttrToText(&attr,m_lstResult.attrNameList,textt);
					strcat(text,textt);
				}
				hRoot1 = m_wndResultTree.InsertItem(text,hRoot0);
				
				m_wndResultTree.SetItemData(hRoot1, (DWORD_PTR)(data0 + j + 1));
			}

			data0 += (np+1);
			
			m_wndResultTree.Expand(hRoot0,TVE_EXPAND);
		}
	}
	else if( m_arrType.GetSize()>1 )
	{
		CP_ATTR filter = m_arrType.GetAt(nType-1);

		data0 = 0;
		for( int i=0; i<nr; i++)
		{
			CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
			if( !r )continue;
			int np = r->arrPoints.GetSize();
			
			CGeometry *pObj = r->m_pFtr->GetGeometry();
			if( !pObj )
			{
				data0 += (np+1);
				continue;
			}
			
			//对象的节点
			memset(text,0,sizeof(text));
			sprintf(text,"Object %d:",i);
			
			//加上属性信息
			int na1 = r->arrAttrs.GetSize(), bHaveType1 = 0;
			for( int k=0; k<na1; k++)
			{
				CP_ATTR attr = r->arrAttrs.GetAt(k);
				if( Comp_CP_ATTR(&filter,&attr) )bHaveType1 = 1;
				AttrToText(&attr,m_lstResult.attrNameList,textt);
				strcat(text,textt);
			}

			//已经被删除
			if( m_arrData.GetAt(data0).bDeleted )
			{
				data0 += (np+1);
				continue;
			}
			
			hRoot0 = m_wndResultTree.InsertItem(text);
			
			m_wndResultTree.SetItemData(hRoot0, (DWORD_PTR)data0);

			BOOL bHaveSubItems = FALSE;
			
			//数据点的节点
			for( int j=0; j<np; j++)
			{
				PT_3DEX expt;
				CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
				expt = pObj->GetDataPoint(pt->ptidx);
				
				memset(text,0,sizeof(text));
				sprintf(text,"POINT(%d)[%.4f,%.4f,%.4f]:",pt->ptidx,expt.x,expt.y,expt.z);

				if( m_arrData.GetAt(data0+j+1).bDeleted )
					continue;
				
				//加上属性信息
				memset(text1,0,sizeof(text1));
				strcat(text1,"  ");
				int na2 = pt->arrAttrs.GetSize();
				int bHaveType2 = 0;
				for( k=0; k<na2; k++)
				{
					CP_ATTR attr = pt->arrAttrs.GetAt(k);
					if( Comp_CP_ATTR(&filter,&attr) )bHaveType2 = 1;
					AttrToText(&attr,m_lstResult.attrNameList,textt);
					strcat(text1,textt);
				}

				//不符合此类型
				if( bHaveType2==0 )
				{
					continue;
				}

				hRoot1 = m_wndResultTree.InsertItem(text,hRoot0);
				m_wndResultTree.SetItemData(hRoot1, (DWORD_PTR)(data0 + j + 1));

				hRoot1 = m_wndResultTree.InsertItem(text1,hRoot0);
				
				m_wndResultTree.SetItemData(hRoot1, (DWORD_PTR)(data0 + j + 1));

				bHaveSubItems = TRUE;
			}
			
			data0 += (np+1);

			//如果有子节点，就展开，如果既没有子节点，也没有属性，就可以删除此节点了
			if( bHaveSubItems )
				m_wndResultTree.Expand(hRoot0,TVE_EXPAND);
			else if( bHaveType1==0 && !bHaveSubItems )
				m_wndResultTree.DeleteItem(hRoot0);
		}
	}
	
	hRoot0 = m_wndResultTree.GetRootItem();
	if( hRoot0 )m_wndResultTree.SelectSetFirstVisible(hRoot0);
}


LONG CDlgSearchResult::OnSetResult(WPARAM wParam,LPARAM lParam)
{
	if( lParam )
	{
		m_pDoc = NULL;
		m_arrData.RemoveAll();
		m_wndResultTree.SelectItem(NULL);
		m_wndResultTree.DeleteAllItems();
		m_wndComboType.ResetContent();
		m_wndComboType.AddString(StrFromResID(IDS_ALL));
		m_wndComboType.SetCurSel(0);
		
		if( wParam==0 )
			m_lstResult.CombineList(*(CCPResultList*)lParam);
		else if( wParam==1 )
			m_lstResult.SubtractList(*(CCPResultList*)lParam);
		else
			m_lstResult.Copy(*(CCPResultList*)lParam);

		m_pDoc = GetCurDocument();

		if( !m_pDoc )return 1;
		if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return 1;
		CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

		int i,j,k, na, np, nr;

		//提取所有可能的属性和值
		m_arrType.RemoveAll();
		nr = m_lstResult.resultList.GetSize();
		for( i=0; i<nr; i++)
		{
			CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
			if( !r )continue;
			na = r->arrAttrs.GetSize();
			for( j=0; j<na; j++)
			{
				if( !Find_CP_ARR(&m_arrType,&r->arrAttrs.GetAt(j)) )
				{
					m_arrType.Add(r->arrAttrs.GetAt(j));
				}
			}
			np = r->arrPoints.GetSize();
			for( j=0; j<np; j++)
			{
				CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
				na = pt->arrAttrs.GetSize();
				for( k=0; k<na; k++)
				{
					if( !Find_CP_ARR(&m_arrType,&pt->arrAttrs.GetAt(k)) )
					{
						m_arrType.Add(pt->arrAttrs.GetAt(k));
					}
				}
			}
		}

		//将属性相同的放在一起，并生成类型名
		{
			CArray<CP_ATTR,CP_ATTR> arrType;
			int nAttr = m_lstResult.attrNameList.GetSize(), maxLen = -1;
			CString name, maxName;
			char text[256];
			for( i=0; i<nAttr; i++)
			{
				int nType = m_arrType.GetSize();
				for( j=0; j<nType; j++)
				{
					if( m_arrType.GetAt(j).index==i )
					{
						arrType.Add(m_arrType.GetAt(j));

						AttrToText(&m_arrType.GetAt(j),m_lstResult.attrNameList,text);
						name = text;
						m_wndComboType.AddString(name);

						if( maxLen<name.GetLength() )
						{
							maxLen = name.GetLength();
							maxName = name;
						}
					}
				}
			}
			m_arrType.Copy(arrType);

			//获得ComboBox控件的下拉宽度
			CRect rcWnd;
			m_wndComboType.GetClientRect(&rcWnd);
			int nTextWid = 0;
			CDC *pDC = m_wndComboType.GetDC();
			if( pDC )
			{
				nTextWid = pDC->GetTextExtent(maxName).cx;
				m_wndComboType.ReleaseDC(pDC);
			}
			if( rcWnd.Width()<nTextWid )
				m_wndComboType.SetDroppedWidth(nTextWid);
			else
				m_wndComboType.SetDroppedWidth(rcWnd.Width());
		}

		//为树形控件的各项生成对应的信息
		ResultState item;
		nr = m_lstResult.resultList.GetSize();

		m_arrData.RemoveAll();
		for( i=0; i<nr; i++)
		{
			CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
			if( !r )continue;
			np = r->arrPoints.GetSize();

			item.nResultIdx = i;
			item.nPointIdx = -1;
			item.bDeleted = 0;
			m_arrData.Add(item);
			
			for( j=0; j<np; j++)
			{
				item.nPointIdx = j;
				item.bDeleted = 0;
				m_arrData.Add(item);
			}
		}

		FillTree();
	}
	
	ShowWindow(SW_SHOW);
	return 0;
}

void CDlgSearchResult::OnCancel() 
{
	KillTimer(1);
	UpdateDrawing(FALSE);
	m_nTimerState = 0;
	ShowWindow(SW_HIDE);
}


void CDlgSearchResult::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent==1 )
	{
		UpdateDrawing((m_nTimerState&1)==0);
		m_nTimerState++;

		if( m_nTimerState>=6 )KillTimer(1);
	}
	CDialog::OnTimer(nIDEvent);
}

void CDlgSearchResult::UpdateDrawing(BOOL bDraw)
{
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	if( bDraw )
	{
		pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)&m_gr);
		pDoc->UpdateAllViews(NULL,hc_UpdateVariantDrag);
	}
	else
	{
		pDoc->UpdateAllViews(NULL,hc_SetVariantDragLine,(CObject*)NULL);
		pDoc->UpdateAllViews(NULL,hc_UpdateVariantDrag);
	}
}


BOOL CDlgSearchResult::GetSelItem(HTREEITEM& item, DWORD_PTR& data)
{
	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return FALSE;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return FALSE;

	item = m_wndResultTree.GetSelectedItem();
	if( item )
	{
		data = m_wndResultTree.GetItemData(item);
		return TRUE;
	}
	return FALSE;
}

void CDlgSearchResult::DeleteSiblingItem(HTREEITEM item, BOOL bDelParent)
{
	HTREEITEM hRoot = m_wndResultTree.GetRootItem(), hChild = NULL;
	while( hRoot )
	{
		if( hRoot==item )break;
		hChild = m_wndResultTree.GetNextItem(hRoot,TVGN_CHILD);
		while( hChild && item!=hChild )
		{
			hChild = m_wndResultTree.GetNextItem(hChild, TVGN_NEXT);
		}
		if( hChild && item==hChild )break;
		hRoot = m_wndResultTree.GetNextItem(hRoot, TVGN_NEXT);
	}

	if( (hRoot&&item==hRoot) || (hChild&&item==hChild) )
	{
		if( bDelParent )
		{
			int idata = m_wndResultTree.GetItemData(hRoot);
			if( idata>=0 && idata<m_arrData.GetSize() )
			{
				ResultState state = m_arrData.GetAt(idata);
				state.bDeleted = 1;
				m_arrData.SetAt(idata,state);

				int saveIdx = state.nResultIdx;
				for( int i=idata+1; i<m_arrData.GetSize(); i++)
				{
					state = m_arrData.GetAt(i);
					state.bDeleted = 1;
					m_arrData.SetAt(i,state);
				}
			}
			
			
		}
		else
		{
			hChild = m_wndResultTree.GetNextItem(hRoot, TVGN_CHILD);
			do
			{
				if( hChild )
				{
					int idata = m_wndResultTree.GetItemData(hChild);
					if( idata>=0 && idata<m_arrData.GetSize() )
					{
						ResultState state = m_arrData.GetAt(idata);
						state.bDeleted = 1;
						m_arrData.SetAt(idata,state);
					}
				}
				hChild = m_wndResultTree.GetNextItem(hChild, TVGN_NEXT);
			}
			while( hChild );
			
			do
			{
				hChild = m_wndResultTree.GetNextItem(hRoot, TVGN_CHILD);
				if( hChild )
				{
					m_wndResultTree.DeleteItem(hChild);
				}
			}
			while( hChild );
		}

		m_wndResultTree.DeleteItem(hRoot);
	}
}


CFeature *CDlgSearchResult::GetSelFtr(CFeature *pFtr, int *idx)
{
	HTREEITEM item;
	DWORD_PTR data;
	if( !GetSelItem(item,data) )return NULL;
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

	int idata = data;
	if( idata<0 || idata>=m_arrData.GetSize() )return NULL;

	ResultState state = m_arrData.GetAt(idata);
	
	if( state.nResultIdx<0 )return NULL;
	
	CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[state.nResultIdx];
	CGeometry *pObj = r->m_pFtr->GetGeometry();
	if( !pObj )return NULL;

	if( pFtr ) pFtr = r->m_pFtr;
	if( idx )
	{
		if( state.nPointIdx<0 )*idx = -1;
		else
		{
			CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(state.nPointIdx);
			*idx= pt->ptidx;
		}
	}
	return r->m_pFtr;
}


void CDlgSearchResult::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if( !hItem )return;

	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	
	
	int idx = m_wndResultTree.GetItemData(hItem);
	if( idx<0 || idx>=m_arrData.GetSize() )return;

	ResultState state = m_arrData.GetAt(idx);
	
	if( state.nResultIdx<0 )return;
	
	CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[state.nResultIdx];
	CGeometry *pObj = r->m_pFtr->GetGeometry();
	if( !pObj )return;
	
	KillTimer(1);
	m_nTimerState = 0;
	
	//点中了非叶子节点（对象节点）
	if( state.nPointIdx<0 )
	{
		m_gr.DeleteAll();
		GrBuffer buf;
		r->m_pFtr->Draw(&buf);
		m_gr.CopyFrom(&buf);
// 		m_gr.SetAllWidth(3.0);
// 		m_gr.EnableWidFromGrd(FALSE);

		PT_3DEX pt0;
		PT_3D pt1;
		pt0 = pObj->GetDataPoint((pObj->GetDataPointSum()-1)/2);
		COPY_3DPT(pt1,pt0);
		pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt1);
		
		SetTimer(1,500,NULL);
	}
	//叶子节点
	else
	{
		m_gr.DeleteAll();
// 		m_gr.SetAllWidth(0.0);
// 		m_gr.EnableWidFromGrd(FALSE);
		
		CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(state.nPointIdx);
		if( pt )
		{
			PT_3DEX pt0;
			PT_3D pt1;
			pt0 = pObj->GetDataPoint(pt->ptidx);

			double markwid = 10*(pDoc->GetDlgDataSource())->GetScale()*0.001;

			pt1.x = pt0.x-markwid; pt1.y = pt0.y-markwid; pt1.z = pt0.z;
			m_gr.MoveTo(&pt1);
			pt1.x = pt0.x+markwid; pt1.y = pt0.y+markwid; pt1.z = pt0.z;
			m_gr.LineTo(&pt1);
			
			pt1.x = pt0.x-markwid; pt1.y = pt0.y+markwid; pt1.z = pt0.z;
			m_gr.MoveTo(&pt1);
			pt1.x = pt0.x+markwid; pt1.y = pt0.y-markwid; pt1.z = pt0.z;
			m_gr.LineTo(&pt1);

			pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt0);
			
			SetTimer(1,500,NULL);
		}
	}
	
	*pResult = 0;
}

void CDlgSearchResult::OnResultClear() 
{
	m_pDoc = NULL;
	m_arrData.RemoveAll();
	m_wndResultTree.DeleteAllItems();
	m_lstResult.Clear();

	m_gr.DeleteAll();
	m_nTimerState = 0;
	
	m_wndComboType.ResetContent();
	m_wndComboType.AddString(StrFromResID(IDS_ALL));
	m_wndComboType.SetCurSel(0);
}

static BOOL sbDelPtWarning = TRUE;

void CDlgSearchResult::OnResultDelallpt() 
{
	CDlgMessageBox::m_bCheck = FALSE;
	if( sbDelPtWarning )
	{
		int nret = CDlgMessageBox::Do(StrFromResID(IDS_DEL_RESULTPT),StrFromResID(IDS_WARNING_TITLE),
			MYMB_YES|MYMB_NO|MYMB_NEVER_WARNING);
		
		sbDelPtWarning = !CDlgMessageBox::m_bCheck;
		if( nret!=IDYES )return;
	}

	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

	CUndoFtrs undo(pDoc,StrFromResID(IDS_CMDNAME_DEL));
	
	int num = m_lstResult.resultList.GetSize();
	CArray<int,int> arrIdxs;

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESSING));
	GProgressStart(num);
	for( int i=0; i<num; i++)
	{
		GProgressStep();

		CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
		arrIdxs.RemoveAll();
		
		int nsz = r->arrPoints.GetSize();

		if (nsz <= 0)  continue;

		for( int j=0; j<nsz; j++)
		{
			CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
			arrIdxs.Add(pt->ptidx);
		}

		CFeature *pOld = new CFeature;
		if (pOld)
		{
			pOld->CopyFrom(r->m_pFtr);

			CFtrLayer *pFtrLayer = pDoc->GetDlgDataSource()->GetFtrLayerOfObject(r->m_pFtr);
			if (!pFtrLayer) continue;
			
			pDoc->DeleteObject(FTR_HANDLE(r->m_pFtr));
			undo.arrOldHandles.Add(FtrToHandle(r->m_pFtr));

			CArray<PT_3DEX,PT_3DEX> pts;
			pOld->GetGeometry()->GetShape(pts);
			//删除问题点
			for(int j=arrIdxs.GetSize()-1; j>=0; j--)
			{
				pts.RemoveAt(arrIdxs[j]);
			}

			if (pOld->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
			{
				undo.arrNewHandles.Add(FtrToHandle(pOld));
				
				pDoc->AddObject(pOld,pFtrLayer->GetID());
			}
		}

	}

	GProgressEnd();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END));

	undo.Commit();
	pDoc->UpdateAllViews(NULL,hc_Refresh);

	for( i=0; i<num; i++)
	{		
		CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
		
		int nsz = r->arrPoints.GetSize();
		if (nsz <= 0) continue;

		CGeometry* pObj = r->m_pFtr->GetGeometry();

		int idata;
		HTREEITEM hRoot = m_wndResultTree.GetRootItem();
		while( hRoot )
		{
			idata = m_wndResultTree.GetItemData(hRoot);
			if( m_arrData[idata].nResultIdx==i )
			{
				DeleteSiblingItem(hRoot,pObj==NULL);
				break;
			}
			hRoot = m_wndResultTree.GetNextItem(hRoot, TVGN_NEXT);
		}
	}
}

void CDlgSearchResult::OnResultDelobjpt() 
{
	CDlgMessageBox::m_bCheck = FALSE;
	if( sbDelPtWarning )
	{
		int nret = CDlgMessageBox::Do(StrFromResID(IDS_DEL_RESULTPT),StrFromResID(IDS_WARNING_TITLE),
			MYMB_YES|MYMB_NO|MYMB_NEVER_WARNING);

		sbDelPtWarning = !CDlgMessageBox::m_bCheck;
		if( nret!=IDYES )return;
	}
	
	HTREEITEM item;
	DWORD_PTR data;
	if( !GetSelItem(item,data) )return;
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	int idata = data;
	if( idata<0 || idata>=m_arrData.GetSize() )return;

	ResultState state = m_arrData.GetAt(idata);
	
	if( state.nResultIdx<0 )return;
	
	CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[state.nResultIdx];
	CGeometry *pObj = r->m_pFtr->GetGeometry();
	if( !pObj )return;

	CUndoFtrs undo(pDoc,StrFromResID(IDS_CMDNAME_DEL));
	
	CArray<int,int> arrIdxs;

	int nsz = r->arrPoints.GetSize();
	if (nsz <= 0) return;

	for( int i=0; i<nsz; i++)
	{
		CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(i);
		arrIdxs.Add(pt->ptidx);
	}

	CFeature *pOld = new CFeature;
	if (pOld)
	{
		pOld->CopyFrom(r->m_pFtr);

		CFtrLayer *pFtrLayer = pDoc->GetDlgDataSource()->GetFtrLayerOfObject(r->m_pFtr);
		if (!pFtrLayer) return;

		pDoc->DeleteObject(FTR_HANDLE(r->m_pFtr));
		undo.arrOldHandles.Add(FtrToHandle(r->m_pFtr));
		
		CArray<PT_3DEX,PT_3DEX> pts;
		pOld->GetGeometry()->GetShape(pts);
		//删除问题点
		for(int j=arrIdxs.GetSize()-1; j>=0; j--)
		{
			pts.RemoveAt(arrIdxs[j]);
		}

		if (pOld->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
		{
			undo.arrNewHandles.Add(FtrToHandle(pOld));
			
			pDoc->AddObject(pOld,pFtrLayer->GetID());
		}
		
	}

	pObj = r->m_pFtr->GetGeometry();
	DeleteSiblingItem(item,pObj==NULL);

	undo.Commit();
	
	pDoc->UpdateAllViews(NULL,hc_Refresh);
	
}

void CDlgSearchResult::OnResultDelpt() 
{
/*	HTREEITEM item;
	DWORD_PTR data;
	if( !GetSelItem(item,data) )return;
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	int idata = data;
	if( idata<0 || idata>=m_arrData.GetSize() )return;

	ResultState state = m_arrData.GetAt(idata);
	
	if( state.nResultIdx<0 )return;
	
	CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[state.nResultIdx];
	CGeometry *pObj = pDoc->m_pDataSource->GetObjectByGUID(r->id);
	if( !pObj )return;	

	CInstantCommand *pCmd = new CInstantCommand;
	if( !pCmd )return;
	
	pCmd->Init(pDoc);
	pCmd->Start();
	pCmd->m_strName.LoadString(ID_RESULT_DELPT);
	
	pCmd->m_idsOld.Add(r->id);
	pDoc->DeleteObject(r->id);
	
	pObj = pObj->Clone();
	pObj->DeletePt(idata);
	if( pObj->GetPtSum()<=0 || 
		(pObj->GetPtSum()<=1&&pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))) ||
		(pObj->GetPtSum()<=2&&pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface))) ||
		(pObj->GetPtSum()<=2&&pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve))&&((CGeoCurve*)pObj)->IsClosed()) )
	{
	}
	else
	{
		r->id = pDoc->AddObject(pObj,r->id.layhdl);
		if( r->id.IsValid() )pCmd->m_idsNew.Add(r->id);
	}
	
	pDoc->OnIntantCommand(0,(LPARAM)pCmd);
*/
}

void CDlgSearchResult::OnResultMarkall() 
{
	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	
	
	int num = m_lstResult.resultList.GetSize();

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESSING));
	GProgressStart(num);
	for( int i=0; i<num; i++)
	{
		CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
		CGeometry* pObj = r->m_pFtr->GetGeometry();
		if( !pObj )continue;	
		int nsz = r->arrPoints.GetSize();
		PT_3DEX expt;
		PT_3D pt3d;

		GProgressStep();
		
		for( int j=0; j<nsz; j++)
		{
			CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
			expt = pObj->GetDataPoint(pt->ptidx);
			COPY_3DPT(pt3d,expt);
			MarkErrorPt(pDoc,expt,1);	
		}	
	}
	GProgressEnd();
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END));

	pDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CDlgSearchResult::OnResultMarkobjpt() 
{
	HTREEITEM item;
	DWORD_PTR data;
	if( !GetSelItem(item,data) )return;
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	
	int idata = data;
	if( idata<0 || idata>=m_arrData.GetSize() )return;

	ResultState state = m_arrData.GetAt(idata);
	
	if( state.nResultIdx<0 )return;
	
	CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[state.nResultIdx];
	CGeometry *pObj = r->m_pFtr->GetGeometry();
	if( !pObj )return;

	int nsz = r->arrPoints.GetSize();
	PT_3DEX expt;
	PT_3D pt3d;

	for( int i=0; i<nsz; i++)
	{
		CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(i);
		expt = pObj->GetDataPoint(pt->ptidx);
		COPY_3DPT(pt3d,expt);
		MarkErrorPt(pDoc,expt,1);	
	}
	pDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CDlgSearchResult::OnResultMarkpt() 
{
	int idx;
	CFeature *pFtr = GetSelFtr(NULL,&idx);
	if (!pFtr)
	{
		return;
	}
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	if( idx<0 )
	{
		OnResultMarkobjpt();
		return;
	}

	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;

	PT_3DEX expt;
	PT_3D pt3d;
	expt = pObj->GetDataPoint(idx);
	COPY_3DPT(pt3d,expt);
	MarkErrorPt(pDoc,expt,1);
	pDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CDlgSearchResult::OnResultSelall() 
{
	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	

	int num = m_lstResult.resultList.GetSize();
	for( int i=0; i<num; i++)
	{
		CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
		pDoc->SelectObj(FtrToHandle(r->m_pFtr));
	}

	pDoc->OnSelectChanged();
}

void CDlgSearchResult::OnResultSelobj() 
{

	CFeature *pFtr = GetSelFtr();
	if( !pFtr )return;

	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	pDoc->SelectObj(FtrToHandle(pFtr));
	pDoc->OnSelectChanged();
}

void CDlgSearchResult::OnResultUnselall() 
{
	//避免文档指针m_pDoc失效
	if( !m_pDoc || m_pDoc!=GetCurDocument() )return;
	if( !m_pDoc->IsKindOf(RUNTIME_CLASS(CDlgDoc)) )return;
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;	
	
	int num = m_lstResult.resultList.GetSize();
	for( int i=0; i<num; i++)
	{
		CP_RESULT* r = (CP_RESULT*)m_lstResult.resultList[i];
		pDoc->DeselectObj(FtrToHandle(r->m_pFtr));
	}

	pDoc->OnSelectChanged();
}

void CDlgSearchResult::OnResultUnselobj() 
{
	CFeature *pFtr = GetSelFtr();
	if (!pFtr)
	{
		return;
	}
	CGeometry *pObj = pFtr->GetGeometry();
	if( !pObj )return;
	
	CDlgDoc *pDoc = (CDlgDoc*)m_pDoc;
	pDoc->DeselectObj(FtrToHandle(pFtr));
	pDoc->OnSelectChanged();
}

BOOL CDlgSearchResult::OnNeedTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
    TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
    UINT nID =pNMHDR->idFrom;
    pTTT->lpszText = MAKEINTRESOURCE(nID);
    pTTT->hinst = AfxGetResourceHandle();
    return(TRUE);	
}

void CDlgSearchResult::SelectLastResult()
{
	//获取当前项
	HTREEITEM item0;
	DWORD_PTR data;
	if( !GetSelItem(item0,data) )return;

	HTREEITEM item = item0;

	//找到最底端的对象节点
	HTREEITEM hRoot = m_wndResultTree.GetRootItem();
	HTREEITEM hBottom = hRoot;
	while( hRoot )
	{
		hBottom = hRoot;
		hRoot = m_wndResultTree.GetNextItem(hRoot,TVGN_NEXT);
	}

	//找到最底端的数据点节点
	HTREEITEM hChild = m_wndResultTree.GetNextItem(hBottom,TVGN_CHILD);
	while( hChild )
	{
		hBottom = hChild;
		hChild = m_wndResultTree.GetNextItem(hChild,TVGN_NEXT);
	}
		
	//寻找上一项	
	HTREEITEM next = NULL;
	while( 1 )
	{		
		BOOL bChild = (m_wndResultTree.GetNextItem(item,TVGN_CHILD)==NULL);
		if( bChild )
		{
			next = m_wndResultTree.GetNextItem(item,TVGN_PREVIOUS);
			//当前对象的顶项
			if( !next )
			{
				HTREEITEM parent = m_wndResultTree.GetNextItem(item,TVGN_PARENT);
				if( parent )parent = m_wndResultTree.GetNextItem(parent,TVGN_PREVIOUS);
				//整个目录的顶项
				if( !parent )next = hBottom;
				else 
				{
					next = parent;

					//找到最底端的数据点节点
					hChild = m_wndResultTree.GetNextItem(parent,TVGN_CHILD);
					while( hChild )
					{
						next = hChild;
						hChild = m_wndResultTree.GetNextItem(hChild,TVGN_NEXT);
					}
				}
			}
		}
		else
		{
			HTREEITEM parent = item;
			if( parent )parent = m_wndResultTree.GetNextItem(parent,TVGN_PREVIOUS);
			//整个目录的顶项
			if( !parent )next = hBottom;
			else 
			{
				next = parent;

				//找到最底端的数据点节点
				hChild = m_wndResultTree.GetNextItem(parent,TVGN_CHILD);
				while( hChild )
				{
					next = hChild;
					hChild = m_wndResultTree.GetNextItem(hChild,TVGN_NEXT);
				}
			}
		}
		
		if( next==item0 )break;
		if( next )
		{
			if( m_wndResultTree.GetItemData(next)==data )
			{
				item = next;
				continue;
			}
			m_wndResultTree.Select(next,TVGN_CARET);
		}
		
		break;
	}
}


void CDlgSearchResult::SelectNextResult()
{
	//获取当前项
	HTREEITEM item0;
	DWORD_PTR data;
	if( !GetSelItem(item0,data) )return;

	HTREEITEM item = item0;

	//找到最顶端的对象节点
	HTREEITEM hRoot = m_wndResultTree.GetRootItem();
	
	//找到最顶端的数据点节点
	HTREEITEM hTop = m_wndResultTree.GetNextItem(hRoot,TVGN_CHILD);
	if( !hTop )hTop = hRoot;
	
	//寻找上一项
	HTREEITEM next = NULL;

	while( 1 )
	{		
		BOOL bChild = (m_wndResultTree.GetNextItem(item,TVGN_CHILD)==NULL);
		if( bChild )
		{
			next = m_wndResultTree.GetNextItem(item,TVGN_NEXT);
			//当前对象的顶项
			if( !next )
			{
				HTREEITEM parent = m_wndResultTree.GetNextItem(item,TVGN_PARENT);
				if( parent )parent = m_wndResultTree.GetNextItem(parent,TVGN_NEXT);
				//整个目录的顶项
				if( !parent )next = hTop;
				else 
				{
					//找到最底端的数据点节点
					next = m_wndResultTree.GetNextItem(parent,TVGN_CHILD);
					if( !next )next = parent;
				}
			}
		}
		else
		{
			HTREEITEM parent = item;
			next = m_wndResultTree.GetNextItem(parent,TVGN_CHILD);
			if( !next )
			{
				next = m_wndResultTree.GetNextItem(parent,TVGN_NEXT);
				//整个目录的顶项
				if( !next )next = hTop;
			}
		}
		
		if( next==item0 )break;
		if( next )
		{
			if( m_wndResultTree.GetItemData(next)==data )
			{
				item = next;
				continue;
			}
			m_wndResultTree.Select(next,TVGN_CARET);
		}
		
		break;
	}
}

BOOL CDlgSearchResult::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_ESCAPE )
		return FALSE;	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgSearchResult::OnSelchangeComboType() 
{
	m_wndResultTree.SelectItem(NULL);
	m_wndResultTree.DeleteAllItems();
	FillTree();	
}
