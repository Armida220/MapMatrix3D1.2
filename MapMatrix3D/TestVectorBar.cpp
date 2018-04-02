// TestVectorBar.cpp: implementation of the CTestVectorBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestVectorBar.h"
#include "resource.h"
#include "editbasedoc.h "
#include "FtrLayer.h "
#include "DlgDataSource.h "
#include "UIFFileDialogEx.h "


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HWND g_hwnd = NULL; //hcw,2013.1.14
CTestVectorBar::CTestVectorBar()
{
	m_pActiveItem = NULL;
	m_pCurItem = NULL;
	m_bClosedBeforePopup = FALSE;
	m_bAutoHideBeforePopup= FALSE;
	m_pDoc = NULL;
}

CTestVectorBar::~CTestVectorBar()
{
	for (int i=0;i<m_arrDataItems.GetSize();i++)
	{
		delete m_arrDataItems[i];
	}	
	m_wndPropListCtrl.RemoveAll();	
	
	m_arrDataItems.RemoveAll();
	m_pActiveItem = NULL;
	m_pCurItem = NULL;
	m_pDoc = NULL;
}
void CTestVectorBar::ChangeDlgDoc(CDlgDoc *pDoc)
{
	if (pDoc==NULL||pDoc->GetDlgDataSource()==NULL)
	{
		m_pDoc = NULL;
		OnUpdateData();
		return;
	}
	if (pDoc!=m_pDoc)
	{		
		m_pDoc = pDoc;
		OnUpdateData();
	}
}

void CTestVectorBar::ClearData()
{
	for (int i=0;i<m_arrDataItems.GetSize();i++)
	{
		delete m_arrDataItems[i];
	}
	for (i = m_wndPropListCtrl.GetItemCount()-1;i>=0;i--)
	{		
		m_wndPropListCtrl.RemovePropItem(m_wndPropListCtrl.GetPropItem(i));
	}
	
	m_arrDataItems.RemoveAll();
	m_pActiveItem = NULL;
	m_pCurItem = NULL;
}
  
BEGIN_MESSAGE_MAP(CTestVectorBar, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()	
	ON_COMMAND(ID_DRAW_TEST_PT,OnDrawTestPt)
	ON_UPDATE_COMMAND_UI(ID_DRAW_TEST_PT,OnDrawTestPtUpdateUI)
	ON_COMMAND(ID_IMPORT_TESTPT,OnImportRefTestPt)
	ON_COMMAND(ID_TEST_EVALUE,OnTestEvalue)
	ON_COMMAND(ID_UPDATEDATA,OnUpdateData)
	ON_MESSAGE(WM_SEL_CHANGED,OnSelChange)
	ON_COMMAND(ID_DEL_ALLTESTPT,OnDelAllTestPt)
	ON_COMMAND(ID_SEARCH_TESTPT, OnSearchTestPt)//hcw,2013.1.8,搜索检查点
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CTestVectorBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	
	CRect rectClient;
	GetClientRect (rectClient);
	CClientDC dc(this);
	
	int cyTlb = m_wndToolBar.CalcFixedLayout (FALSE, TRUE).cy;	
	m_wndToolBar.SetWindowPos (NULL, rectClient.left, rectClient.top, 
		rectClient.Width (), cyTlb,
		SWP_NOACTIVATE | SWP_NOZORDER);
/*	m_wndStatic.SetWindowPos(NULL,rectClient.left-2,rectClient.top + cyTlb,rectClient.Width()+4,1,SWP_NOACTIVATE | SWP_NOZORDER);*/
	m_wndPropListCtrl.SetWindowPos(NULL,rectClient.left, rectClient.top + cyTlb,rectClient.Width(),rectClient.Height ()-cyTlb,	
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropListCtrl.AdjustLayout();	

}

void CTestVectorBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}

void CTestVectorBar::OnDrawTestPt()
{
	AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DRAW_TEST_PT,0);
}
void CTestVectorBar::OnDrawTestPtUpdateUI(CCmdUI* pCmdUI)
{
	CDlgDoc *pDoc = m_pDoc;
	if (!pDoc)return;
	pCmdUI->SetCheck(ID_DRAW_TEST_PT==pDoc->GetCurrentCommandId());
}

void CTestVectorBar::OnImportRefTestPt()
{
	CDlgDoc *pDoc = m_pDoc;
	if (!pDoc)return;
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("All Files(*.*)|*.*||"));
	
	if( dlg.DoModal()!=IDOK )
	{	
		return;
	}
	CString strFile = dlg.GetPathName();
	CStdioFile file;
	CString  line;
	char name[1024];
	BOOL bMsgBox = FALSE;
	double x,y,z;
	CTypedPtrArray<CPtrArray,RefPtToTestPtItem*> arrDataItems;
	RefPtToTestPtItem *pRefItem;
	if(file.Open(strFile,CFile::modeRead| CFile::typeText))
	{
		file.ReadString(line);
		line.TrimLeft();
		line.TrimRight();
		if(line.FindOneOf(_T(" "))>0)
			file.SeekToBegin();
		while(file.ReadString(line))
		{
			line.TrimLeft();
			line.TrimRight();
			if(line.GetLength()>1024)
				line = line.Left(1024);
			if(sscanf((LPCTSTR)line,"%s %lf %lf %lf",name,&x,&y,&z)==4)
			{	
				pRefItem = new RefPtToTestPtItem;
				if (!pRefItem)
				{
					AfxMessageBox(_T("Memory is not sufficient! "));
					file.Close();
					for (int i=0;i<arrDataItems.GetSize();i++)
					{
						delete arrDataItems[i];
					}
					arrDataItems.RemoveAll();
					return;
				}
				strncpy(pRefItem->Id,name,sizeof(pRefItem->Id)-1);
				pRefItem->lfRefX = x;
				pRefItem->lfRefY = y;
				pRefItem->lfRefZ = z;				
				arrDataItems.Add(pRefItem);
			}
			else
			{
				if( !bMsgBox )
				{
					bMsgBox = TRUE;
					AfxMessageBox(StrFromResID(IDS_FILEFORMAT_ERROR));
				}
			}
		}		
		file.Close();
		CFtrLayer *pLayer = pDoc->GetDlgDataSource()->GetFtrLayer(_T("CheckPointLayer"));
		if (!pLayer)
		{
			pLayer = pDoc->GetDlgDataSource()->CreateFtrLayer(_T("CheckPointLayer"));
			if (!pLayer)
			{
				return;
			}
			pDoc->GetDlgDataSource()->AddFtrLayer(pLayer);
		}
		if (!pLayer->IsLocked())
		{
			pLayer->EnableLocked(true);
			pDoc->UpdateFtrLayer(pLayer);
		}
	/*	int idx = m_wndPropListCtrl.GetItemCount();*/
		CString strText;
		MyList::CLVLPropItem0 *pItem;
		MyList::CLVLPropItem0 *pFirstItem = NULL;
		pDoc->BeginBatchUpdate();
		for (int i=0;i<arrDataItems.GetSize();i++)
		{
			CFeature* pFtr = pLayer->CreateDefaultFeature(pDoc->GetDlgDataSource()->GetScale(),CLS_GEOPOINT);
			if (pFtr)
			{
				pFtr->SetCode(arrDataItems[i]->Id);
				pFtr->GetGeometry()->CreateShape(&PT_3DEX(arrDataItems[i]->lfRefX,arrDataItems[i]->lfRefY,arrDataItems[i]->lfRefZ,penLine),1);
				pDoc->AddObject(pFtr,pLayer->GetID());
				pItem = new MyList::CLVLPropItem0;	
				if(!pFirstItem)
					pFirstItem = pItem;
				m_wndPropListCtrl.AddItem(pItem);
				pItem->SetValue((_variant_t)(LPCTSTR)arrDataItems[i]->Id,0);
				strText.Format(_T("(%lf,%lf,%lf)"),arrDataItems[i]->lfRefX,arrDataItems[i]->lfRefY,arrDataItems[i]->lfRefZ);
				pItem->SetValue((_variant_t)(LPCTSTR)strText,1);
				pItem->SetData((DWORD_PTR)arrDataItems[i]);
/*				m_listCtrl.SetItem(idx-1,2,strText);*/
			}
		}
		pDoc->EndBatchUpdate();
		m_wndPropListCtrl.FilterPropItems(TRUE);
		if(pFirstItem)
		{
			m_wndPropListCtrl.SelectItem(pFirstItem);
			m_pActiveItem = (RefPtToTestPtItem*)pFirstItem->GetData();
			m_pCurItem = pFirstItem;
		}
		m_arrDataItems.Append(arrDataItems);
		m_strArrColumninCheckList.RemoveAll();
		if (m_strArrColumninCheckList.GetSize()<=0)
		{
			GetListColumnArr(m_wndPropListCtrl, m_strArrColumninCheckList, 0);//
		}
		GOutPut(StrFromResID(IDS_OUTPUT_SUCCESS));
	}

}

LRESULT CTestVectorBar::OnSelChange(WPARAM   wParam,LPARAM   lParam)
{
	if(m_wndPropListCtrl.GetSelectedCount()>0)
	{
		CDlgDoc *pDoc = m_pDoc;
		if (!pDoc)return 0;
		m_pCurItem = m_wndPropListCtrl.GetSelectedItem(0);
		m_pActiveItem = (RefPtToTestPtItem*)m_pCurItem->GetData();
		PT_3D pt(m_pActiveItem->lfRefX,m_pActiveItem->lfRefY,m_pActiveItem->lfRefZ);
		pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);
	}
	else
	{
		m_pCurItem = NULL;
		m_pActiveItem = NULL;
	}
	return 0;
}

BOOL CTestVectorBar::GetFeatureId(CString &strId)
{
	if (m_pActiveItem)
	{
		strId = m_pActiveItem->Id;
		return TRUE;
	}
	return FALSE;
}

BOOL CTestVectorBar::SetCurActiveItem(double x,double y,double z)
{
	if (m_pActiveItem)
	{
		m_pActiveItem->lfTestX = x;
		m_pActiveItem->lfTestY = y;
		m_pActiveItem->lfTestZ = z;
		m_pActiveItem->bFinsh = TRUE;
		CString strValue;
		strValue.Format(_T("(%lf,%lf,%lf)"),x,y,z);
		m_pCurItem->SetValue((_variant_t)(_bstr_t)(LPCTSTR)strValue,2);
//		m_wndPropListCtrl.FilterPropItems();
		m_wndPropListCtrl.SelectItem(m_pCurItem);
		return TRUE;
	}
	return FALSE;
}

BOOL CTestVectorBar::IsFinishCurActiveItem()
{
	if (m_pActiveItem)
	{		
		return m_pActiveItem->bFinsh;	
	}
	return FALSE;
}

void CTestVectorBar::RefreshActiveItem()
{
	if (m_pActiveItem)
	{
		CDlgDoc *pDoc = m_pDoc;
		if (!pDoc)return;		
		CFtrLayer* pRefLayer = pDoc->GetDlgDataSource()->GetFtrLayer(_T("CheckPointLayer"));
		CFeature *pFtr0;
		PT_3DEX expt;
		if (pRefLayer)
		{
			int nObj = pRefLayer->GetObjectCount();
			for (int i=0;i<nObj;i++)
			{
				pFtr0 = pRefLayer->GetObject(i);
				if (!pFtr0||!pFtr0->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))continue;
				if (strncmp(m_pActiveItem->Id,pFtr0->GetCode(),sizeof(m_pActiveItem->Id))==0)
				{
					break;
				}
			}
			if(i<nObj)
			{
				expt = pFtr0->GetGeometry()->GetDataPoint(0);
				m_pActiveItem->lfRefX = expt.x;
				m_pActiveItem->lfRefY = expt.y;
				m_pActiveItem->lfRefZ = expt.z;
			}
		}
		CFtrLayer* pTestLayer = pDoc->GetDlgDataSource()->GetFtrLayer(_T("TestPointLayer"));
		if (pTestLayer)
		{
			int nObj = pTestLayer->GetObjectCount();
			for (int i=0;i<nObj;i++)
			{
				pFtr0 = pTestLayer->GetObject(i);
				if (!pFtr0||!pFtr0->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))continue;
				if (strncmp(m_pActiveItem->Id,pFtr0->GetCode(),sizeof(m_pActiveItem->Id))==0)
				{
					break;
				}
			}
			if(i<nObj)
			{
				expt = pFtr0->GetGeometry()->GetDataPoint(0);
				m_pActiveItem->lfTestX = expt.x;
				m_pActiveItem->lfTestY = expt.y;
				m_pActiveItem->lfTestZ = expt.z;
				m_pActiveItem->bFinsh = TRUE;
			}
			else
				m_pActiveItem->bFinsh = FALSE;

		}		
		
		CString strXYZ;
		strXYZ.Format(_T("(%lf,%lf,%lf)"),m_pActiveItem->lfRefX,m_pActiveItem->lfRefY,m_pActiveItem->lfRefZ);
		m_pCurItem->SetValue((_variant_t)(LPCTSTR)strXYZ,1);
		if(m_pActiveItem->bFinsh)
		{
			strXYZ.Format(_T("(%lf,%lf,%lf)"),m_pActiveItem->lfTestX,m_pActiveItem->lfTestY,m_pActiveItem->lfTestZ);
			m_pCurItem->SetValue((_variant_t)(LPCTSTR)strXYZ,2);
		}
		else
		{
			strXYZ = _T("");
			m_pCurItem->SetValue((_variant_t)(LPCTSTR)strXYZ,2);
		}	
		m_wndPropListCtrl.SelectItem(m_pCurItem);		
	}
}

void CTestVectorBar::Popup()
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
		Slide(TRUE, TRUE);
		OnTimer(AFX_TIMER_ID_AUTO_HIDE_SLIDE_OUT_EVENT);
		m_nSlideSteps = nOldStep;
	}
	//从tab窗口中脱离出来
	else if( pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMFCBaseTabCtrl)) )
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
		
		CPoint pt = rectPop.CenterPoint();
		rectPop.left = pt.x-rectCur.Width()/2; 
		rectPop.right = rectPop.left+rectCur.Width();
		rectPop.top = pt.y-rectCur.Height()/2; 
		rectPop.bottom = rectPop.top+rectCur.Height();
	
		CMultiPaneFrameWnd *pMin = (CMultiPaneFrameWnd*)pWnd;
		pMin->SetWindowPos(NULL,rectPop.left,rectPop.top,rectPop.Width(),rectPop.Height(),SWP_NOZORDER);
		pMin->BringWindowToTop();
	}
	else if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
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
		
		pMain->GetWindowRect(&rectPop);
		
		CPoint pt = rectPop.CenterPoint();
		rectPop.left = pt.x-rectCur.Width()/2; 
		rectPop.right = rectPop.left+rectCur.Width();
		rectPop.top = pt.y-rectCur.Height()/2; 
		rectPop.bottom = rectPop.top+rectCur.Height();

		StoreRecentDockSiteInfo();
		FloatPane(rectPop,DM_SHOW);
		pWnd = GetParent();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd)))
		{
			CMultiPaneFrameWnd *pMin = (CMultiPaneFrameWnd*)pWnd;
			pMin->SetWindowPos(NULL,rectPop.left,rectPop.top,rectPop.Width(),rectPop.Height(),SWP_NOZORDER);
			pMin->BringWindowToTop();
		}
		else BringWindowToTop();
		m_wndPropListCtrl.SetFocus();
	}
}

int CTestVectorBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	m_wndToolBar.CreateEx(this//,0 ,WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_TOOLTIPS,
		/*CRect(0,0,0,0)*/);	
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_TEST);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() |
 		CBRS_TOOLTIPS | CBRS_FLYBY);
	
	m_wndToolBar.SetPaneStyle(
		m_wndToolBar.GetPaneStyle() &
 		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | 
 		CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);
	m_wndToolBar.SetRouteCommandsViaFrame (FALSE);

	CRect rectDummy;
	rectDummy.SetRectEmpty();
/*	m_wndStatic.Create(_T(""),WS_CHILD|WS_VISIBLE|WS_BORDER,rectDummy,this);*/
	if (!m_wndPropListCtrl.Create(WS_VISIBLE|WS_CHILD|WS_BORDER ,rectDummy,this,124))
		return FALSE;
	MyList::CLVLPropColumn0 col;
	strcpy(col.FieldName,_T("RefPtName"));
	strcpy(col.ColumnName,StrFromResID(IDS_REF_POINT_NAME)); //hcw,2013.1.8,StrFromResID(IDS_REF_POINT_NAME)→“hcw test”
	col.ValueType = VT_BSTR;
	col.Type = MyList::CLVLPropColumn0::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	
	strcpy(col.FieldName,_T("RefXYZ"));
	strcpy(col.ColumnName,StrFromResID(IDS_REF_XYZ));
	col.ValueType = VT_BSTR/*VT_I4*/;
//	col.BitmapOffset = 2;
	col.Type = MyList::CLVLPropColumn0::EDIT/*COLOR*/;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	
	strcpy(col.FieldName,_T("TestXYZ"));
	strcpy(col.ColumnName,StrFromResID(IDS_TEST_XYZ));
	col.ValueType = VT_BSTR;
//	col.BitmapOffset = 1;
	col.Type = MyList::CLVLPropColumn0::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);	
	
	m_wndPropListCtrl.InitHeaderCtrl();
	
	CHeaderCtrl& header = m_wndPropListCtrl.GetHeaderCtrl();
	for(int i=0; i<header.GetItemCount(); i++)
	{
		HDITEM hitem;
		hitem.mask = HDI_WIDTH;
		hitem.cxy = 100;
		header.SetItem(i,&hitem);
	}
	AdjustLayout ();
	return 1;
}

void CTestVectorBar::OnTestEvalue()
{
	if (m_arrDataItems.GetSize()<=0)
	{
		return;
	}
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("text Files(*.txt)|*.txt|All Files(*.*)|*.*||"));
	
	if( dlg.DoModal()!=IDOK )
	{	
		return;
	}
	//
	double minDeltaX = -1, minDeltaY = -1, minDeltaXY = -1, minDeltaZ = -1;
	double maxDeltaX = -1, maxDeltaY = -1, maxDeltaXY = -1, maxDeltaZ = -1;

	double X = 0,Y = 0, XY = 0, Z = 0;
	int nSum = 0;
	for (int i = m_arrDataItems.GetSize()-1;i>=0;i--)
	{
		if(m_arrDataItems[i]->bFinsh)
		{
			nSum++;
			double temp = fabs(m_arrDataItems[i]->lfTestX-m_arrDataItems[i]->lfRefX);
			if (minDeltaX<0||temp<minDeltaX)
			{
				minDeltaX = temp;
			}
			if (maxDeltaX<0||temp>maxDeltaX)
			{
				maxDeltaX = temp;
			}
			X = X+temp*temp;

			temp = fabs(m_arrDataItems[i]->lfTestY-m_arrDataItems[i]->lfRefY);
			if (minDeltaY<0||temp<minDeltaY)
			{
				minDeltaY = temp;
			}
			if (maxDeltaY<0||temp>maxDeltaY)
			{
				maxDeltaY = temp;
			}
			Y = Y+temp*temp;

			temp = sqrt((m_arrDataItems[i]->lfTestX-m_arrDataItems[i]->lfRefX)*(m_arrDataItems[i]->lfTestX-m_arrDataItems[i]->lfRefX) + 
				(m_arrDataItems[i]->lfTestY-m_arrDataItems[i]->lfRefY)*(m_arrDataItems[i]->lfTestY-m_arrDataItems[i]->lfRefY));
			if (minDeltaXY<0||temp<minDeltaXY)
			{
				minDeltaXY = temp;
			}
			if (maxDeltaXY<0||temp>maxDeltaXY)
			{
				maxDeltaXY = temp;
			}
			XY = XY + temp*temp;

			temp = fabs(m_arrDataItems[i]->lfTestZ-m_arrDataItems[i]->lfRefZ);
			if (minDeltaZ<0||temp<minDeltaZ)
			{
				minDeltaZ = temp;
			}
			if (maxDeltaZ<0||temp>maxDeltaZ)
			{
				maxDeltaZ = temp;
			}
			Z = Z+temp*temp;
		}
	}
	double RMSEX = sqrt(X/nSum);
	double RMSEY = sqrt(Y/nSum);
	double RMSEZ = sqrt(Z/nSum);
	double RMSEXY = sqrt(XY/nSum);
 
	CString strFile = dlg.GetPathName();
	PathRenameExtension(strFile.GetBuffer(_MAX_PATH),_T(".txt"));
	strFile.ReleaseBuffer();
	strFile.FreeExtra();
	CStdioFile file;
	CString  line;

	RefPtToTestPtItem *pItem;
	if(file.Open(strFile,CFile::modeCreate|CFile::modeWrite| CFile::typeText))
	{	
		line.Format(_T("%-20.20s%-15s%-15s%-15s%-15s%-15s%-15s%-10s%-10s\n"),
			_T("name"),_T("x1"),_T("y1"),_T("z1"),_T("x2"),_T("y2"),_T("z2"),_T("dxy"),_T("dz") );
		file.WriteString(line);
		double dx,dy,dxy,dz;
		for ( i = 0;i<m_arrDataItems.GetSize();i++)
		{
			if(m_arrDataItems[i]->bFinsh)
			{	
				dx = fabs(m_arrDataItems[i]->lfTestX-m_arrDataItems[i]->lfRefX);
				dy = fabs(m_arrDataItems[i]->lfTestY-m_arrDataItems[i]->lfRefY);
				dz = fabs(m_arrDataItems[i]->lfTestZ-m_arrDataItems[i]->lfRefZ);
				dxy = sqrt((m_arrDataItems[i]->lfTestX-m_arrDataItems[i]->lfRefX)*(m_arrDataItems[i]->lfTestX-m_arrDataItems[i]->lfRefX) + 
				(m_arrDataItems[i]->lfTestY-m_arrDataItems[i]->lfRefY)*(m_arrDataItems[i]->lfTestY-m_arrDataItems[i]->lfRefY));

				line.Format(_T("%-20.20s%-15.4lf%-15.4lf%-15.4lf%-15.4lf%-15.4lf%-15.4lf%-10.4lf%-10.4lf\n"),
					m_arrDataItems[i]->Id,m_arrDataItems[i]->lfRefX,m_arrDataItems[i]->lfRefY,m_arrDataItems[i]->lfRefZ,
					m_arrDataItems[i]->lfTestX,m_arrDataItems[i]->lfTestY,m_arrDataItems[i]->lfTestZ,dxy,dz);
				file.WriteString(line);
			}
		}
		line.Format(_T("\n    %-15.15s%-15.15s%-15.15s"),_T("min"),_T("max"),_T("RMSE"));
		file.WriteString(line);
		line.Format(_T("\ndxy  %-15.4lf%-15.4lf%-15.4lf\n"),minDeltaXY,maxDeltaXY,RMSEXY);
		file.WriteString(line);
		line.Format(_T("dz  %-15.4lf%-15.4lf%-15.4lf"),minDeltaZ,maxDeltaZ,RMSEZ);
		file.WriteString(line);
		file.Close();
		ShellExecute(this->GetSafeHwnd(),"open","notepad.exe",strFile,NULL,SW_NORMAL);		
	}
}
// 
// BOOL CTestVectorBar::AddItem(RefPtToTestPtItem* pItem)
// {
// 	int i;
// 	for (i=0;i<m_arrDataItems.GetSize();i++)
// 	{
// 		if (strncmp(pItem->Id,m_arrDataItems[i]->Id,sizeof(pItem->Id))==0)
// 		{
// 			break;
// 		}
// 	}
// 	if (i<m_arrDataItems.GetSize())
// 	{
// 		return FALSE;
// 	}
// 	else
// 	{
// 		m_arrDataItems.Add(pItem);
// 		return TRUE;
// 	}
// }

void CTestVectorBar::OnUpdateData()
{
	ClearData();
	CDlgDoc *pDoc = m_pDoc;
	if (!pDoc||!pDoc->GetDlgDataSource())
	{
		m_wndPropListCtrl.FilterPropItems();
		return;
	}
	CFtrLayer* pRefLayer = pDoc->GetDlgDataSource()->GetFtrLayer(_T("CheckPointLayer"));
	CFtrLayer* pTestLayer = pDoc->GetDlgDataSource()->GetFtrLayer(_T("TestPointLayer"));
	CString error;
	if (pRefLayer)
	{
		CMap<CString,LPCTSTR,CFeature*,CFeature*> lookMap;
		lookMap.InitHashTable(1001);
		CMap<CString,LPCTSTR,int,int&> lookMap0;
		lookMap0.InitHashTable(501);
		CFeature* pFtr,*pFtr0;
		CString temp;
		int nValue;
		RefPtToTestPtItem* pItem;
		int nCnt = pRefLayer->GetObjectCount();
		for (int i=0;i<nCnt;i++)
		{
			pFtr = pRefLayer->GetObject(i);
			if(!pFtr||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))continue;
			temp = pFtr->GetCode();
			if (lookMap.Lookup(temp,pFtr0))
			{
				lookMap.RemoveKey(temp);
			
				lookMap0[temp]=2;
			}
			else
			{
				if(lookMap0.Lookup(temp,nValue))
				{
					lookMap0.SetAt(temp,++nValue);
				}
				else
				{
					lookMap.SetAt(temp,pFtr);
				}
				
			}
		}
		POSITION pos = lookMap.GetStartPosition();	
		while (pos != NULL)
		{
			lookMap.GetNextAssoc( pos, temp, pFtr0 );
			pItem = new RefPtToTestPtItem;
			if(!pItem)continue;
			PT_3DEX exPt = pFtr0->GetGeometry()->GetDataPoint(0);			
			strncpy(pItem->Id,pFtr0->GetCode(),sizeof(pItem->Id));
			pItem->lfRefX = exPt.x;
			pItem->lfRefY = exPt.y;
			pItem->lfRefZ = exPt.z;
			m_arrDataItems.Add(pItem);			
		}		
		error.LoadString(IDS_REFLAYERROR);
		GOutPut(error);	
		{
			CString temp0;
			POSITION pos = lookMap0.GetStartPosition();	
			while (pos != NULL)
			{			
				lookMap0.GetNextAssoc( pos, temp, nValue );
				temp0.Format(_T("%s  ,  %d\n"), temp,nValue);
				GOutPut(temp0);
			}
		}		
		
	}
	if (pTestLayer)
	{
		CMap<CString,LPCTSTR,CFeature*,CFeature*> lookMap;
		lookMap.InitHashTable(1001);
		CMap<CString,LPCTSTR,int,int&> lookMap0;
		lookMap0.InitHashTable(501);
		if (!pTestLayer->IsEmpty())
		{
			CFeature* pFtr,*pFtr0;
			PT_3DEX exPt;
			CString temp;			
			int nValue;
			int nCnt = pTestLayer->GetObjectCount();
			for (int j=0;j<nCnt;j++)
			{
				pFtr = pTestLayer->GetObject(j);
				if(!pFtr||!pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoPoint)))continue;
				temp = pFtr->GetCode();
				if(lookMap.Lookup(temp,pFtr0))
				{
					lookMap.RemoveKey(temp);
					
					lookMap0[temp]=2;				
				}
				else
				{
					if(lookMap0.Lookup(temp,nValue))
					{
						lookMap0.SetAt(temp,++nValue);
					}
					else
					{
						lookMap.SetAt(temp,pFtr);
					}
				}
			}
			for (int i=0;i<m_arrDataItems.GetSize();i++)
			{
				temp = m_arrDataItems[i]->Id;
				if(lookMap.Lookup(temp,pFtr0))
				{
					PT_3DEX exPt = pFtr0->GetGeometry()->GetDataPoint(0);
					m_arrDataItems[i]->bFinsh = TRUE;
					m_arrDataItems[i]->lfTestX = exPt.x;
					m_arrDataItems[i]->lfTestY = exPt.y;
					m_arrDataItems[i]->lfTestZ = exPt.z;
				}
			}
			GOutPut(StrFromResID(IDS_TESTLAYERROR));
			CString temp0;
			POSITION pos = lookMap0.GetStartPosition();	
			while (pos != NULL)
			{			
				lookMap0.GetNextAssoc( pos, temp, nValue );
				temp0.Format(_T("%s  ,  %d\n"), temp,nValue);
				GOutPut(temp0);
			}			
		}
	}
	CString strText;
	MyList::CLVLPropItem0 *pFirstItem = NULL;
	MyList::CLVLPropItem0 *pItem;	
	for (int k=0;k<m_arrDataItems.GetSize();k++)
	{	
		pItem = new MyList::CLVLPropItem0;	
		if(!pFirstItem)pFirstItem = pItem;
		m_wndPropListCtrl.AddItem(pItem);
		pItem->SetValue((_variant_t)(LPCTSTR)m_arrDataItems[k]->Id,0);
		strText.Format(_T("(%lf,%lf,%lf)"),m_arrDataItems[k]->lfRefX,m_arrDataItems[k]->lfRefY,m_arrDataItems[k]->lfRefZ);
		pItem->SetValue((_variant_t)(LPCTSTR)strText,1);
		if (m_arrDataItems[k]->bFinsh)
		{
			strText.Format(_T("(%lf,%lf,%lf)"),m_arrDataItems[k]->lfTestX,m_arrDataItems[k]->lfTestY,m_arrDataItems[k]->lfTestZ);
			pItem->SetValue((_variant_t)(LPCTSTR)strText,2);
		}
		pItem->SetData((DWORD_PTR)m_arrDataItems[k]);
	}		
	m_wndPropListCtrl.FilterPropItems();
	if(pFirstItem)
	{
		m_wndPropListCtrl.SelectItem(pFirstItem);
		m_pActiveItem = (RefPtToTestPtItem*)pFirstItem->GetData();
		m_pCurItem = pFirstItem;
	}
	m_strArrColumninCheckList.RemoveAll();	
	if (m_strArrColumninCheckList.GetSize()<=0)
	{
		GetListColumnArr(m_wndPropListCtrl, m_strArrColumninCheckList, 0);
	}
}


void CTestVectorBar::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillRect (rcClient, &CBrush(0xffffff));
	this->SetFocus();//hcw,2013.1.14.
}


void CTestVectorBar::OnDelAllTestPt()
{
	if( !m_pDoc )
		return;

	CUndoFtrs undo(m_pDoc,StrFromResID(ID_DEL_ALLTESTPT));

	ClearData();
	CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
	CFtrLayer *pTestLayer = pDS->GetFtrLayer("TestPointLayer");
	CFtrLayer *pCheckLayer = pDS->GetFtrLayer("CheckPointLayer");

	m_pDoc->BeginBatchUpdate();

	if( pCheckLayer && pTestLayer )
	{
		for( int i=0; i<pCheckLayer->GetObjectCount(); i++)
		{
			CFeature *pFtr = pCheckLayer->GetObject(i);
			if( !pFtr )continue;

			m_pDoc->DeleteObject(FtrToHandle(pFtr));

			undo.AddOldFeature(FtrToHandle(pFtr));
		}

		for( i=0; i<pTestLayer->GetObjectCount(); i++)
		{
			CFeature *pFtr = pTestLayer->GetObject(i);
			if( !pFtr )continue;
			
			m_pDoc->DeleteObject(FtrToHandle(pFtr));

			undo.AddOldFeature(FtrToHandle(pFtr));
		}
	}

	m_pDoc->EndBatchUpdate();

	undo.Commit();

	m_wndPropListCtrl.RedrawWindow();
}
void CTestVectorBar::OnSearchTestPt()
{
	CSearchCheckPtDlg dlg;	
	dlg.InitDlgMems(m_strArrColumninCheckList);	
	int nRtn = dlg.DoModal();
	if (nRtn==IDOK)
	{
		CString strCheckName = dlg.GetEditTxt();
		int nCurSelectedItem = GetSelectedItem(m_wndPropListCtrl);
 		int nFindee=FindPropItem( m_wndPropListCtrl,strCheckName, 0, nCurSelectedItem, TRUE, TRUE);
		if (nFindee>=0)
		{
			MyList::CLVLPropItem0* pSelectedPropItem =  m_wndPropListCtrl.GetPropItem(nFindee);
			m_wndPropListCtrl.SelectItem(pSelectedPropItem);
			m_wndPropListCtrl.EnsureItemVisible(pSelectedPropItem);
			CDlgDoc *pDoc = m_pDoc;
			if (!pDoc)return;
			m_pCurItem = pSelectedPropItem;
			m_pActiveItem = (RefPtToTestPtItem*)m_pCurItem->GetData();
			PT_3D pt(m_pActiveItem->lfRefX,m_pActiveItem->lfRefY,m_pActiveItem->lfRefZ);
		    pDoc->UpdateAllViews(NULL,hc_SetCrossPos,(CObject*)&pt);
		}
		else
		{
			MessageBox("检查点未找到!","提示");
		}
	}
}
BOOL CTestVectorBar::PreTranslateMessage( MSG* pMsg )
{	
	return CWnd::PreTranslateMessage(pMsg);
}
void CTestVectorBar::GetListColumnArr( MyList::CLVLPropList0 &wndPropListCtrl, CStringArray &strArr, int iCol )
{
	CString str;
	for (int i=0; i<wndPropListCtrl.GetItemCount(); i++)
	{
		str = (LPCTSTR)(_bstr_t)wndPropListCtrl.GetPropItem(i)->GetValue(iCol);
		strArr.Add(str);
	}
	return;
}
int CTestVectorBar::GetSelectedItem( MyList::CLVLPropList0 &wndPropListCtrl)
{
	int nSelected = -1;
	for (int i=0; i<wndPropListCtrl.GetItemCount(); i++)
	{
		if (wndPropListCtrl.GetPropItem(i)->IsSelected())
		{
			nSelected = i;
			return nSelected;
		}
	}
	return nSelected;
}
int CTestVectorBar::FindPropItem( MyList::CLVLPropList0 &wndPropListCtrl, CString str, int nCol, int nPos, BOOL bIncrement/*=TRUE*/, BOOL bIncludednPosItem/*=TRUE*/ )
{
	int iStart = -1;
	int nListCount = wndPropListCtrl.GetItemCount();
	if (nPos<0||nCol<0)
	{
		return -1;
	}
	if (bIncludednPosItem)
	{
		iStart = nPos;
	}
	else
	{
		if (bIncrement)
		{
			if (nPos==nListCount-1)
			{
				iStart = nPos-1;
			}
			else
			{
				iStart = nPos+1;
			}
		}
		else
		{
			if (nPos==0)
			{
				iStart = nPos;
			}
			else
			{
				iStart = nPos-1;
			}
		}
	}
	int i = 0;
	CString strFromListCtrl;
	if (bIncrement)
	{
		if (iStart>nListCount-1)
		{
			iStart=0;
		}
		for (i=iStart; i<nListCount; i++)
		{
			strFromListCtrl = (LPCTSTR)(_bstr_t)wndPropListCtrl.GetPropItem(i)->GetValue(nCol);
			if (strFromListCtrl.CompareNoCase(str)==0)
			{
				return i;
			}
		}
		for (i=0; i<=iStart-1; i++)
		{
			strFromListCtrl = (LPCTSTR)(_bstr_t)wndPropListCtrl.GetPropItem(i)->GetValue(nCol);
			if (strFromListCtrl.CompareNoCase(str)==0)
			{
				return i;
			}
		}
	}
	else
	{
		if (iStart<0)
		{
			iStart=nListCount-1;
		}
		for (i=iStart; i>=0; i--)
		{
			strFromListCtrl = (LPCTSTR)(_bstr_t)wndPropListCtrl.GetPropItem(i)->GetValue(nCol);
			if (strFromListCtrl.CompareNoCase(str)==0)
			{
				return i;
			}
		}
		for(i=nListCount-1; i>iStart; i--)
		{
			strFromListCtrl = (LPCTSTR)(_bstr_t)wndPropListCtrl.GetPropItem(i)->GetValue(nCol);
			if (strFromListCtrl.CompareNoCase(str)==0)
			{
				return i;
			}
		}
	}
	return -1;
}
void CTestVectorBar::SelectNextSameItem( BOOL bIncrement, int nListIndex, int nCol)
{
	if (nListIndex<0)
	{
		return;
	}
	int nNextIndex = -1;
	CString strPropItem;
    strPropItem = (LPCTSTR)(_bstr_t)m_wndPropListCtrl.GetPropItem(nListIndex)->GetValue(nCol);
	nNextIndex = FindPropItem(m_wndPropListCtrl, strPropItem, nCol, nListIndex, bIncrement,FALSE);
	if (nNextIndex>=0)
	{
		MyList::CLVLPropItem0 *pNextPropItem = m_wndPropListCtrl.GetPropItem(nNextIndex);
		m_wndPropListCtrl.SelectItem(pNextPropItem);
		m_wndPropListCtrl.EnsureItemVisible(pNextPropItem);
	}
	return;
}
