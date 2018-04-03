// FTRLayerViewBar.cpp: implementation of the CFTRLayerViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <map>
#include <vector>
#include "MainFrm.h"
#include "EditBase.h"
#include "FTRLayerViewBar.h"
#include "ColorDialogEX.h"
#include "multicombo.h"
#include "Feature.h"
#include "GeoText.h"
#include "VectorView_new.h"
#include "DlgOverMutiImgLay.h"
#include "DlgRefDataFile.h"
#include "DlgScheme.h"
#include "DlgColorLayout.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ITEM_SOURCE  0x01
#define ITEM_GROUP   0x02
#define ITEM_LAYER   0x03

#define REGPATH_SUBTYPE			"FTRLayerView"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFTRLayerViewBar::CFTRLayerViewBar()
{
	img_height = 16;
	img_width = 22;
	CString temp;
	symbol_point.LoadString(IDS_LAYERFTRTYPE_POINT);
	symbol_line.LoadString(IDS_LAYERFTRTYPE_LINE);
	symbol_polygon.LoadString(IDS_LAYERFTRTYPE_SURFACE);
	current_subtype = AfxGetApp()->GetProfileInt(REGPATH_SUBTYPE,"SubType",GROUP);
}

CFTRLayerViewBar::~CFTRLayerViewBar()
{

}


BEGIN_MESSAGE_MAP(CFTRLayerViewBar, CDockablePane)
//{{AFX_MSG_MAP(CFTRLayerViewBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_MESSAGE(TREELBTNDOWN, OnTreeLBtnDown)
	ON_MESSAGE(TREELBTNDBCLK, OnTreeLBtnDBClk) 
	ON_COMMAND(ID_LAYERVIEW_EXTERNAL_PRO, OnEditExProp)
	ON_COMMAND(ID_LAYERVIEW_SUBTYPE_GROUP, OnSubWithGroup)
	ON_COMMAND(ID_LAYERVIEW_SUBTYPE_DBLAYER, OnSubwithDBLayer)
	ON_COMMAND(ID_LAYERVIEW_IMAGE_ADDANY, OnAddRefImageFromAny)
	ON_COMMAND(ID_LAYERVIEW_IMAGE_ADDWORKSPACE, OnAddRefImageFromWorkspace)
	ON_COMMAND(ID_LAYERVIEW_IMAGE_REMOVE,OnRemoveRefImage)
	ON_COMMAND(ID_LAYERVIEW_FDB_ADD, OnAddRefDatasource)
	ON_COMMAND(ID_LAYERVIEW_FDB_ADD1, OnAddRefDatasourceUVS)
	ON_COMMAND(ID_LAYERVIEW_FDB_REMOVE, OnRemoveRefDatasource)
	ON_COMMAND(ID_LAYERVIEW_FDB_ACTIVE, OnActiveDatasource)
	ON_COMMAND(ID_LAYERVIEW_SELECT_OBJ, OnSelectObjectsByLayer)
	ON_COMMAND(ID_LAYERVIEW_SYMBOL, OnSymbolConfig)
	ON_COMMAND(ID_LAYERVIEW_RESETCOLOR, OnResetColor)
	ON_COMMAND(ID_LAYERVIEW_COLORLAYOUT, OnColorLayout)
	ON_COMMAND(ID_LAYERVIEW_MOVEUP, OnMoveUp)
	ON_COMMAND(ID_LAYERVIEW_MOVEDOWN, OnMoveDown)
	ON_COMMAND(ID_LAYERVIEW_REFRESH, OnUpdateManual)
//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


void CFTRLayerViewBar::AdjustLayout ()
{
	CRect rectClient;
	GetClientRect(&rectClient);
	
	m_wndLayerView.SetWindowPos (NULL, rectClient.left, rectClient.top,
		rectClient.Width (), rectClient.Height (),
		SWP_NOACTIVATE | SWP_NOZORDER);
}


int CFTRLayerViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	
	// Create views:
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | TVS_HASLINES |
								TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | 
								WS_CLIPCHILDREN | TVS_CHECKBOXES;
	
	if (!m_wndLayerView.Create (dwViewStyle, rectDummy, this, 3))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}
	// Load images:
	//m_AttribViewImages.Create (IDB_CLASS_VIEW, 16, 0, RGB (255, 0, 0));
	//m_wndAttribView.SetImageList (&m_AttribViewImages, TVSIL_NORMAL);
	
	//	CreateToolBar(IDR_PROJECTBAR);
	images.Create(img_width,img_height,ILC_COLOR24,0,10000);
    m_wndLayerView.SetImageList(&images,TVSIL_NORMAL);
	AdjustLayout ();
	
	//	AfxLinkCallback(FCCM_LOADPROJECT,this,(PFUNCALLBACK)OnCallLoad);
	//	AfxLinkCallback(FCCM_GETPROJECT,this,(PFUNCALLBACK)OnGetProject);

	AddRootImage(0);
	AddSubRootImage();
	AddRootImage(1);
	AddRootImage(2);
	AddRootImage(3);
	return 0;
}


void CFTRLayerViewBar::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}


void CFTRLayerViewBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rect;
	GetClientRect(&rect);
	//ScreenToClient (rect);
	dc.FillSolidRect(&rect,RGB(0,255,255));
	
	m_wndLayerView.GetWindowRect (rect);
	ScreenToClient (rect);
	
	rect.InflateRect (1, 1);
	dc.Draw3dRect (rect, ::GetSysColor (COLOR_3DSHADOW), ::GetSysColor (COLOR_3DSHADOW));
}


void CFTRLayerViewBar::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	
	CDockablePane::OnLButtonDown(nFlags, point);
}

void CFTRLayerViewBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDockablePane::OnLButtonUp(nFlags, point);
}

void CFTRLayerViewBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CDockablePane::OnLButtonDblClk(nFlags,point);
}

void CFTRLayerViewBar::OnSubWithGroup()
{
	if(current_subtype != CFTRLayerViewBar::SUBTYPE::GROUP)
	{
        current_subtype = CFTRLayerViewBar::SUBTYPE::GROUP;
		AfxGetApp()->WriteProfileInt(REGPATH_SUBTYPE,"SubType",current_subtype);
		UpdateLayerView();
	}
}

void CFTRLayerViewBar::OnSubwithDBLayer()
{
	if(current_subtype != CFTRLayerViewBar::SUBTYPE::DBLAYER)
	{
        current_subtype = CFTRLayerViewBar::SUBTYPE::DBLAYER;
		AfxGetApp()->WriteProfileInt(REGPATH_SUBTYPE,"SubType",current_subtype);
		UpdateLayerView();
	}
}

void CFTRLayerViewBar::OnEditExProp()
{
	BeginCheck42License

	HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	ItemType item_type = GetItemType(hitem);
	if(item_type!=CFTRLayerViewBar::ItemType::ITEMDBLAYER && item_type!=CFTRLayerViewBar::ItemType::ITEMLAYER)
		return;
	//

	CDlgDoc* pdc = GetActiveDlgDoc();
	if(!pdc) return;
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();

	if (pMainFrame->m_EditExPropDlg == NULL)
	{
		pMainFrame->m_EditExPropDlg = new CEditExProp();
		pMainFrame->m_EditExPropDlg->Create(IDD_CHECKEXPROP, AfxGetMainWnd());
	}
	if (pMainFrame->m_EditExPropDlg == NULL)
		return;
	pMainFrame->m_EditExPropDlg->m_pDoc = pdc;
	pMainFrame->m_EditExPropDlg->ShowWindow(SW_SHOW);
	pMainFrame->m_EditExPropDlg->LoadRuleFile();
    //
	if(item_type==CFTRLayerViewBar::ItemType::ITEMDBLAYER)
	{
		pMainFrame->m_EditExPropDlg->setInitLayerName(m_wndLayerView.GetItemText(hitem));
		pMainFrame->m_EditExPropDlg->checkByDBLayerName(true);
	}	
	else
	{
		CDlgDataSource* pds = NULL;
		HTREEITEM parent_item = m_wndLayerView.GetParentItem(hitem);
		parent_item = m_wndLayerView.GetParentItem(parent_item);
		for(int i=0; i<pdc->GetDataSourceCount(); ++i)
		{
			if(m_wndLayerView.GetItemText(parent_item) == pdc->GetDlgDataSource(i)->GetName())
			{
				pds = pdc->GetDlgDataSource(i);
			}	
		}
		if(pds == NULL)
			return ;
		RootInfo* prootinfo = ds_root[pds->GetName()];
		CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[hitem]);
		if(player == NULL)
		return ;
        //
		pMainFrame->m_EditExPropDlg->setInitLayerName(player->GetName());
		pMainFrame->m_EditExPropDlg->checkByDBLayerName(false);
	}        

	pMainFrame->m_EditExPropDlg->Check();

	EndCheck42License
}

void CFTRLayerViewBar::OnAddRefImageFromAny()
{
	CView* pview = GetActiveDlgDoc()->GetCurActiveView();
	if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
		return;
	CVectorView_new* pvectorview = (CVectorView_new*)pview;
	//
   CDlgOverMutiImgLay dlg;
   CArray<ViewImgPosition,ViewImgPosition> old_imgs;
   pvectorview->GetViewImagePositions(old_imgs);
   memcpy(&dlg.m_ptsBound,pvectorview->GetptBounds(),4*sizeof(PT_3D));
   dlg.m_arrImgPos.Copy(old_imgs);
   dlg.Create(CDlgOverMutiImgLay::IDD);
   dlg.AddImageFromAny();
   pvectorview->UpdateOverlayImage(dlg.m_arrImgPos);
   //
   Refresh(false,true);
}

void CFTRLayerViewBar::OnAddRefImageFromWorkspace()
{
	CView* pview = GetActiveDlgDoc()->GetCurActiveView();
	if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
		return;
	CVectorView_new* pvectorview = (CVectorView_new*)pview;
	//
	CDlgOverMutiImgLay dlg;
	CArray<ViewImgPosition,ViewImgPosition> old_imgs;
	pvectorview->GetViewImagePositions(old_imgs);
	memcpy(&dlg.m_ptsBound,pvectorview->GetptBounds(),4*sizeof(PT_3D));
	dlg.m_arrImgPos.Copy(old_imgs);
	dlg.Create(CDlgOverMutiImgLay::IDD);
	dlg.AddImageFromWorkspace();
    pvectorview->UpdateOverlayImage(dlg.m_arrImgPos);
	//
	Refresh(false,true);
}

void CFTRLayerViewBar::OnRemoveRefImage()
{
	int sel_count = m_wndLayerView.getSelCount();
	if(sel_count==0)
		return;
	//
    CView* pview = GetActiveDlgDoc()->GetCurActiveView();
	if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
		return;
	CVectorView_new* pvectorview = (CVectorView_new*)pview;
	//
	std::map<CString,bool> removed_imgs;
	bool changed = false;
	for(int i=0; i<sel_count; ++i)
	{
		HTREEITEM img_item = m_wndLayerView.getSelitem(i);
		ItemType item_type = GetItemType(img_item);
		if(item_type != CFTRLayerViewBar::ItemType::ITEMIMG)
			continue;
		//
		CString img_name = m_wndLayerView.GetItemText(img_item);
		std::map<CString,HTREEITEM>::iterator itr_img = refimg_item.find(img_name);
		if(itr_img != refimg_item.end())
		{
            removed_imgs[img_name] = true;
			changed = true;
		}
	}
	if(!changed)
		return;
	//
    CArray<ViewImgPosition,ViewImgPosition> old_imgs;
	pvectorview->GetViewImagePositions(old_imgs);
    CArray<ViewImgPosition,ViewImgPosition> new_imgs;
	for(i=0; i<old_imgs.GetSize(); ++i)
	{
		std::map<CString,bool>::iterator itr = removed_imgs.find(old_imgs[i].fileName);
		if(itr==removed_imgs.end())
		{
			new_imgs.Add(old_imgs[i]);
		}
	}
	//
    pvectorview->UpdateOverlayImage(new_imgs);
	//
	Refresh(false,true);
}

void CFTRLayerViewBar::OnAddRefDatasource()
{
   CDlgDoc* pdc = GetActiveDlgDoc();
   if(pdc == NULL)
	   return;
   //
   CDlgRefDataFile dlg;
   dlg.Create(CDlgRefDataFile::IDD);
   dlg.AddRefDataFile();

   pdc = GetActiveDlgDoc();//重新获取doc
   if(pdc == NULL) return;
   pdc->UpdateRefDatafile(dlg.m_arrDataParams);
   //
   Refresh(true,false);
}

void CFTRLayerViewBar::OnAddRefDatasourceUVS()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if (pdc == NULL)
		return;
	//
	CDlgRefDataFile dlg;
	dlg.Create(CDlgRefDataFile::IDD);
	dlg.AddRefDataFileUVS();

	pdc = GetActiveDlgDoc();//重新获取doc
	if (pdc == NULL) return;
	pdc->UpdateRefDatafile(dlg.m_arrDataParams);
	//
	Refresh(true, false);
}

void CFTRLayerViewBar::OnRemoveRefDatasource()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL)
		return;
    //
	int sel_count = m_wndLayerView.getSelCount();
	if(sel_count==0)
		return;
	//
	for(int i=0; i<sel_count; ++i)
	{
		HTREEITEM itm = m_wndLayerView.getSelitem(i);
        ItemType item_type = GetItemType(itm);
		if(item_type != CFTRLayerViewBar::ItemType::ITEMFDB)
			continue;
		//
		CString itm_name = m_wndLayerView.GetItemText(itm);
		std::map<CString,RootInfo*>::iterator itr = ds_root.find(itm_name);
		if(itr!=ds_root.end())
		{
            CDlgRefDataFile dlg;
			dlg.Create(CDlgRefDataFile::IDD);
			dlg.DelRefDataFile(itm_name);
			pdc->UpdateRefDatafile(dlg.m_arrDataParams);
			Refresh(true,false);
		}
	}
}

void CFTRLayerViewBar::OnActiveDatasource()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL)
		return;
    //
	int sel_count = m_wndLayerView.getSelCount();
	if(sel_count==0)
		return;
	//
	for(int i=0; i<sel_count; ++i)
	{
		HTREEITEM itm = m_wndLayerView.getSelitem(i);
		ItemType item_type = GetItemType(itm);
		if(item_type != CFTRLayerViewBar::ItemType::ITEMFDB)
			continue;
		//
		CString itm_name = m_wndLayerView.GetItemText(itm);
		std::map<CString,RootInfo*>::iterator itr = ds_root.find(itm_name);
		if(itr!=ds_root.end())
		{
            CDlgRefDataFile dlg;
			dlg.Create(CDlgRefDataFile::IDD);
			dlg.ActiveRefDataFile(itm_name);
			pdc->UpdateRefDatafile(dlg.m_arrDataParams);
			Refresh(true,false);
			break;
		}
	}
}

void CFTRLayerViewBar::OnSelectObjectsByLayer()
{
    HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	HTREEITEM parent = m_wndLayerView.GetParentItem(hitem);
	parent = m_wndLayerView.GetParentItem(parent);

	CDlgDoc *pDoc = GetActiveDlgDoc();
	CDlgDataSource* pds = pDoc->GetDlgDataSource(pDoc->GetActiveDataSourceIdx());
	if(pds->GetName() != m_wndLayerView.GetItemText(parent))
		return;
	CFtrLayer* player = pds->GetFtrLayer(ds_root[pds->GetName()]->item_layerid[hitem]);
	if(player==NULL)
		return;
	//
	CArray<FTR_HANDLE,FTR_HANDLE> handles;
	for(int i=0; i<player->GetObjectCount(); ++i)
	{
	   if(player->GetObject(i)!=NULL)
         handles.Add(FtrToHandle(player->GetObject(i)));
	}
	//
	pDoc->DeselectAll();
	pDoc->SelectObj(handles.GetData(),handles.GetSize(),TRUE);
	pDoc->OnSelectChanged();
}

void CFTRLayerViewBar::OnSymbolConfig()
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(pDoc == NULL)
		return ;
	//
	for( int m=0; m<pDoc->GetDlgDataSourceCount(); m++)
	{
		pDoc->GetDlgDataSource(m)->ClearLayerSymbolCache();
	}
	//
    HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	HTREEITEM parent = m_wndLayerView.GetParentItem(hitem);
	parent = m_wndLayerView.GetParentItem(parent);
	CFtrLayer* player = pDoc->GetDlgDataSource(pDoc->GetActiveDataSourceIdx())->GetFtrLayer(ds_root[m_wndLayerView.GetItemText(parent)]->item_layerid[hitem]);
	if(player==NULL)
		return;
	//
    CString NameAndCode;
	NameAndCode.Format("%d ",pDoc->GetDlgDataSource(pDoc->GetActiveDataSourceIdx())->GetFtrLayerCode(player->GetName()));
	NameAndCode+=player->GetName();
    //
	CDlgScheme dlg;
	dlg.InitMems(gpCfgLibMan->GetConfigLibItemByScale(pDoc->GetDlgDataSource()->GetScale()),pDoc->GetDlgDataSource()->GetScale());
	dlg.m_pDoc = pDoc;
	dlg.SetSelectedLayerIndex(NameAndCode);
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

void CFTRLayerViewBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL)
		return;
	//
	HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	if(m_wndLayerView.getSelCount()>0)
        hitem = m_wndLayerView.getSelitem(m_wndLayerView.getSelCount()-1);
	//
	ItemType item_type = GetItemType(hitem);
    //
	CMenu menu;
	menu.LoadMenu(IDR_POP_LAYER_VIEW);
	
	CMenu* pSumMenu = menu.GetSubMenu(0);

	if(item_type!=CFTRLayerViewBar::ItemType::ITEMLAYER && item_type!=CFTRLayerViewBar::ItemType::ITEMDBLAYER)
	{
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_EXTERNAL_PRO,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}
	if(item_type!=CFTRLayerViewBar::ItemType::ITEMFDB)
	{
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_FDB_ACTIVE,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
        pSumMenu->EnableMenuItem(ID_LAYERVIEW_FDB_REMOVE,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}
    if(item_type!=CFTRLayerViewBar::ItemType::ITEMGROUP && item_type!=CFTRLayerViewBar::ItemType::ITEMDBLAYER &&
		item_type!=CFTRLayerViewBar::ItemType::ITEMFDB && item_type!=CFTRLayerViewBar::ItemType::ITEMLAYER)
	{
        pSumMenu->EnableMenuItem(ID_LAYERVIEW_RESETCOLOR,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}
	if(item_type!=CFTRLayerViewBar::ItemType::ITEMGROUP && item_type!=CFTRLayerViewBar::ItemType::ITEMDBLAYER)
	{
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_COLORLAYOUT,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}
	if(item_type!=CFTRLayerViewBar::ItemType::ITEMIMG)
	{
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_IMAGE_REMOVE,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}
	if(item_type!=CFTRLayerViewBar::ItemType::ITEMLAYER)
	{
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_SYMBOL,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_SELECT_OBJ,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}
	else
	{
		HTREEITEM parent = m_wndLayerView.GetParentItem(hitem);
		parent = m_wndLayerView.GetParentItem(parent);
		if(ds_root.find(m_wndLayerView.GetItemText(parent))==ds_root.end() ||
		   pdc->GetDlgDataSource(pdc->GetActiveDataSourceIdx())->GetName()!=m_wndLayerView.GetItemText(parent))
		{
			pSumMenu->EnableMenuItem(ID_LAYERVIEW_SYMBOL,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		    pSumMenu->EnableMenuItem(ID_LAYERVIEW_SELECT_OBJ,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		}
	}

	if(item_type!=CFTRLayerViewBar::ItemType::ITEMIMG &&
	   item_type!=CFTRLayerViewBar::ItemType::ITEMDBLAYER && item_type!=CFTRLayerViewBar::ItemType::ITEMGROUP)
	{
        pSumMenu->EnableMenuItem(ID_LAYERVIEW_MOVEUP,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		pSumMenu->EnableMenuItem(ID_LAYERVIEW_MOVEDOWN,MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}

	if (!CheckLicense(86))
	{
		pSumMenu->RemoveMenu(ID_LAYERVIEW_FDB_ADD1, MF_BYCOMMAND);
	}
	
	pSumMenu->CheckMenuItem(ID_LAYERVIEW_SUBTYPE_GROUP,
		MF_BYCOMMAND|(current_subtype==GROUP?MF_CHECKED:MF_UNCHECKED));
	pSumMenu->CheckMenuItem(ID_LAYERVIEW_SUBTYPE_DBLAYER,
		MF_BYCOMMAND|(current_subtype==DBLAYER?MF_CHECKED:MF_UNCHECKED));

	pSumMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, 
		point.y, this);
}


void CFTRLayerViewBar::SwitchVisible()
{
	HTREEITEM focus_item = m_wndLayerView.getCurrentItem();
	if(focus_item == NULL)
		return;
	//
	bool visible = !item_check[focus_item];
    for(int i=0; i<m_wndLayerView.getSelCount(); ++i)
	{
		CDlgDoc* pdc = GetActiveDlgDoc();
		if(pdc == NULL)
			return;
		//
		CDlgDataSource* pds = NULL;
		//选中的项
		HTREEITEM hItem = m_wndLayerView.getSelitem(i);
		HTREEITEM parent_item = m_wndLayerView.GetParentItem(hItem);
		ItemType item_type = GetItemType(hItem);
		if(item_type==CFTRLayerViewBar::ItemType::NONE)
			continue;
		//
		item_check[hItem] = visible;
		m_wndLayerView.SetCheck(hItem,visible);
        //
		if(item_type==CFTRLayerViewBar::ItemType::ROOTFDB)
		{
			for(int i=0; i<pdc->GetDataSourceCount(); ++i)
			{
				pds = pdc->GetDlgDataSource(i);
				if(pds != NULL)
				{
					RootInfo* prootinfo = ds_root[pds->GetName()];
					m_wndLayerView.SetCheck(prootinfo->root_item,visible);
					//
					for(HTREEITEM itr = m_wndLayerView.GetChildItem(prootinfo->root_item); itr != NULL; itr = m_wndLayerView.GetNextSiblingItem(itr))
					{
						m_wndLayerView.SetCheck(itr,visible);
						item_check[itr] = visible;
						for(HTREEITEM itr1 = m_wndLayerView.GetChildItem(itr); itr1 != NULL; itr1 = m_wndLayerView.GetNextSiblingItem(itr1))
						{
							CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[itr1]);
							if(player == NULL)
								continue;
							//
							player->EnableVisible(visible);

							UpdateDispyParam param;
							param.type = UpdateDispyParam::typeVISIBLE;
							param.handle = (LONG_PTR)player;
							param.data.bVisible = visible;
							
							pdc->UpdateAllViews(NULL,hc_UpdateLayerDisplay,(CObject*)&param);
							
							m_wndLayerView.SetCheck(itr1, visible);
							item_check[itr1] = visible;
						}
					}
				}
			}
		}
		else if(item_type==CFTRLayerViewBar::ItemType::ITEMFDB)
		{
			for(int i=0; i<pdc->GetDataSourceCount(); ++i)
			{
				if(m_wndLayerView.GetItemText(hItem) == pdc->GetDlgDataSource(i)->GetName())
				{
					pds = pdc->GetDlgDataSource(i);
					break;
				}
			}
			if(pds != NULL)
			{
				RootInfo* prootinfo = ds_root[pds->GetName()];
				//
				for(HTREEITEM itr = m_wndLayerView.GetChildItem(hItem); itr != NULL; itr = m_wndLayerView.GetNextSiblingItem(itr))
				{
					m_wndLayerView.SetCheck(itr,visible);
					item_check[itr] = visible;
					for(HTREEITEM itr1 = m_wndLayerView.GetChildItem(itr); itr1 != NULL; itr1 = m_wndLayerView.GetNextSiblingItem(itr1))
					{
						CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[itr1]);
						if(player == NULL)
							continue;
						//
						player->EnableVisible(visible);

						UpdateDispyParam param;
						param.type = UpdateDispyParam::typeVISIBLE;
						param.handle = (LONG_PTR)player;
						param.data.bVisible = visible;
						
						pdc->UpdateAllViews(NULL,hc_UpdateLayerDisplay,(CObject*)&param);
						
						m_wndLayerView.SetCheck(itr1, visible);
						item_check[itr1] = visible;
					}
				}
			}
		}
		else if(item_type==CFTRLayerViewBar::ItemType::ITEMDBLAYER || item_type==CFTRLayerViewBar::ItemType::ITEMGROUP)
		{
			for(int i=0; i<pdc->GetDataSourceCount(); ++i)
			{
				if(m_wndLayerView.GetItemText(m_wndLayerView.GetParentItem(hItem)) == pdc->GetDlgDataSource(i)->GetName())
				{
					pds = pdc->GetDlgDataSource(i);
				}
			}
			if(pds == NULL)
				return;
			//
			RootInfo* prootinfo = ds_root[pds->GetName()];
			for(HTREEITEM itr = m_wndLayerView.GetChildItem(hItem); itr != NULL; itr = m_wndLayerView.GetNextSiblingItem(itr))
			{
				CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[itr]);
				if(player == NULL)
					continue;
				//
				player->EnableVisible(visible);

				UpdateDispyParam param;
				param.type = UpdateDispyParam::typeVISIBLE;
				param.handle = (LONG_PTR)player;
				param.data.bVisible = visible;
				
				pdc->UpdateAllViews(NULL,hc_UpdateLayerDisplay,(CObject*)&param);
				
				m_wndLayerView.SetCheck(itr, visible);
				item_check[itr] = visible;
			}
		}
		else if(item_type==CFTRLayerViewBar::ItemType::ITEMLAYER)
		{
			HTREEITEM parent_item = m_wndLayerView.GetParentItem(hItem);
			parent_item = m_wndLayerView.GetParentItem(parent_item);
			//
			for(int i=0; i<pdc->GetDataSourceCount(); ++i)
			{
				if(m_wndLayerView.GetItemText(parent_item) == pdc->GetDlgDataSource(i)->GetName())
				{
					pds = pdc->GetDlgDataSource(i);
				}	
			}
			if(pds == NULL)
				return;
			//
			RootInfo* prootinfo = ds_root[pds->GetName()];
			CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[hItem]);
			if(player == NULL)
				continue;
			player->EnableVisible(visible);

			UpdateDispyParam param;
			param.type = UpdateDispyParam::typeVISIBLE;
			param.handle = (LONG_PTR)player;
			param.data.bVisible = visible;
			
			pdc->UpdateAllViews(NULL,hc_UpdateLayerDisplay,(CObject*)&param);
		}
		else if(item_type == CFTRLayerViewBar::ItemType::ROOTIMG)
		{
			CView* pview = GetActiveDlgDoc()->GetCurActiveView();
			if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
				return;
			CVectorView_new* pvectorview = (CVectorView_new*)pview;
			//
			CArray<ViewImgPosition,ViewImgPosition> imgs;
			pvectorview->GetViewImagePositions(imgs);
			for(int i=0; i<imgs.GetSize(); ++i)
			{
				std::map<CString,HTREEITEM>::iterator itr_img = refimg_item.find(imgs[i].fileName);
				if(itr_img!=refimg_item.end())
				{
				   imgs[i].is_visible = visible;
				   item_check[itr_img->second] = visible;
				   m_wndLayerView.SetCheck(itr_img->second,visible);
				}
			}
			//
			pvectorview->UpdateOverlayImage(imgs);		
		}
		else if(item_type == CFTRLayerViewBar::ItemType::ITEMIMG)
		{
			CView* pview = GetActiveDlgDoc()->GetCurActiveView();
			if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
				return;
			CVectorView_new* pvectorview = (CVectorView_new*)pview;
			//
			CString img_name = m_wndLayerView.GetItemText(hItem);
			std::map<CString,HTREEITEM>::iterator itr_img = refimg_item.find(img_name);
			if(itr_img == refimg_item.end())
				continue;
			//
			CArray<ViewImgPosition,ViewImgPosition> imgs;
			pvectorview->GetViewImagePositions(imgs);
			for(int i=0; i<imgs.GetSize(); ++i)
			{
				if(imgs[i].fileName == img_name)
				{
					imgs[i].is_visible = visible;
					break;
				}
			}
			//
			pvectorview->UpdateOverlayImage(imgs);
			m_wndLayerView.SetCheck(itr_img->second,visible);		
		}
	}
}

void CFTRLayerViewBar::ChangeLayerColor(CFtrLayer* player, COLORREF color, CUndoBatchAction *pUndo)
{
	if(player==NULL || player->GetColor()==color)
		return;

	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return;

	CUndoModifyLayer undo1(pDoc,_T("ModifyLayerColor"));
	undo1.field = FIELDNAME_LAYCOLOR;
	undo1.arrLayers.Add(player);
	_variant_t oldColor = (long)player->GetColor();
	_variant_t newColor = (long)color;
	undo1.arrOldVars.Add(oldColor);
	pDoc->ModifyLayer(player,FIELDNAME_LAYCOLOR,newColor,TRUE);
	undo1.newVar = newColor;
	pUndo->AddAction(&undo1);

	CUndoModifyProperties undo2(pDoc, "ResetColor");
	int nObj = player->GetObjectCount();
	for(int j=0; j<nObj; ++j)
	{
		CPFeature pftr = player->GetObject(j);
		if(pftr==NULL || pftr->GetGeometry()==NULL)
			continue;
		if(-1==pftr->GetGeometry()->GetColor())
			continue;
		//
		undo2.arrHandles.Add(FtrToHandle(pftr));
		undo2.oldVT.BeginAddValueItem();
		pftr->WriteTo(undo2.oldVT);
		undo2.oldVT.EndAddValueItem();
		pDoc->DeleteObject(FtrToHandle(pftr),FALSE);
		
		pftr->GetGeometry()->SetColor(-1);
		
		pDoc->RestoreObject(FtrToHandle(pftr));
		undo2.newVT.BeginAddValueItem();
		pftr->WriteTo(undo2.newVT);
		undo2.newVT.EndAddValueItem();
	}
	pUndo->AddAction(&undo2);
}

LRESULT CFTRLayerViewBar::OnTreeLBtnDown(WPARAM wParam, LPARAM lParam)
{
	if( m_wndLayerView.GetCount()<=0 )
	{
		return 0;
	}
	
	switch (m_wndLayerView.getMBtnDownPos())
	{
	case CClassTreeWndLayer::MBTNDOWMPOS::NONE:
		break;
	case CClassTreeWndLayer::MBTNDOWMPOS::BUTTON:
		break;
	case CClassTreeWndLayer::MBTNDOWMPOS::ICON:
		{
			//选中的项
			HTREEITEM hItem = m_wndLayerView.GetSelectedItem();
			ItemType item_type = GetItemType(hItem);
			if(item_type!=CFTRLayerViewBar::ItemType::ITEMLAYER)
				break;
			//
			CColorDialogEX color_dlg;
			if(color_dlg.DoModal() == IDOK)
			{
				CDlgDoc* pdc = GetActiveDlgDoc();
				CDlgDataSource* pds = NULL;
				HTREEITEM parent_item = m_wndLayerView.GetParentItem(hItem);
				parent_item = m_wndLayerView.GetParentItem(parent_item);
				for(int i=0; i<pdc->GetDataSourceCount(); ++i)
				{
					if(m_wndLayerView.GetItemText(parent_item) == pdc->GetDlgDataSource(i)->GetName())
					{
						pds = pdc->GetDlgDataSource(i);
						break;
					}	
				}
				if(pds == NULL)
					return 0;
				
				RootInfo* prootinfo = ds_root[pds->GetName()];
				CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[hItem]);
				if(player == NULL)
					return 0;
				//
				CUndoBatchAction undo(pdc, "ModifyLayerColor");
				pdc->BeginBatchUpdate();
				ChangeLayerColor(player,color_dlg.GetColor(), &undo);
				pdc->EndBatchUpdate();
				undo.Commit();
				//
				std::map<COLORREF,int>::iterator itr = color_img.find(color_dlg.GetColor());
				if(itr==color_img.end())
				{
					AddColorImage(color_dlg.GetColor());
				}
				
				m_wndLayerView.SetItemImage(hItem,color_img[color_dlg.GetColor()],color_img[color_dlg.GetColor()]);
			}
		}
		break;
	case CClassTreeWndLayer::MBTNDOWMPOS::LABEL:
		break;
	case CClassTreeWndLayer::MBTNDOWMPOS::CHECKBOX:
		{
			SwitchVisible();
		}
		break;
	default:
		break;		
	}

    return 0;
}

LRESULT CFTRLayerViewBar::OnTreeLBtnDBClk(WPARAM wParam, LPARAM lParam)
{
	switch (m_wndLayerView.getMBtnDownPos())
	{
	case CClassTreeWndLayer::MBTNDOWMPOS::ICON:
		break;
	case CClassTreeWndLayer::MBTNDOWMPOS::LABEL:
		{
			HTREEITEM hItem = m_wndLayerView.GetSelectedItem();
			ItemType item_type = GetItemType(hItem);
			if(item_type!=CFTRLayerViewBar::ItemType::ITEMLAYER)
				break;
            //
			HTREEITEM parent_item = m_wndLayerView.GetParentItem(hItem);
			parent_item = m_wndLayerView.GetParentItem(parent_item);
			//
			CDlgDoc* pdc = GetActiveDlgDoc();
			if(pdc==NULL) 
				return 0;
			CDlgDataSource* pds = NULL;
			for(int i=0; i<pdc->GetDataSourceCount(); ++i)
			{
				if(m_wndLayerView.GetItemText(parent_item) == pdc->GetDlgDataSource(i)->GetName())
				{
					pds = pdc->GetDlgDataSource(i);
					break;
				}	
			}
			if(pds == NULL)
				return 0;
			
			RootInfo* prootinfo = ds_root[pds->GetName()];
			CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[hItem]);
			if(player == NULL)
				return 0;
			CString layer_name = player->GetName();
			//
			const FTR_HANDLE * handles;
			int num ;
			handles = pdc->GetSelection()->GetSelectedObjs(num);
            if(num<=0)
			{
				return 0;
			}
			//
			CScheme *pScheme = gpCfgLibMan->GetScheme(pds->GetScale());
			if(!pScheme) break;
			
			float wid = pScheme->GetLayerDefineLineWidth(player->GetName());
			
			CUndoBatchAction batchUndo(pdc,"ModifyLayerOfObj");
			
			for (i=0;i<num;i++)
			{
				CUndoModifyLayerOfObj undo(pdc,"ModifyLayerOfObj");
				
				CPFeature pftr = HandleToFtr(handles[i]);

				undo.arrHandles.Add(handles[i]);
				
				undo.oldVT.BeginAddValueItem();
				pftr->WriteTo(undo.oldVT);
				undo.oldVT.EndAddValueItem();
				
				pdc->DeleteObject(handles[i],FALSE);
				
				CGeometry *pGeo = pftr->GetGeometry();
				if(!pGeo) continue;
				
				if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				{
					((CGeoCurveBase*)pGeo)->m_fLineWidth = wid;
				}
				
				CFtrLayer* player_old = pds->GetFtrLayerOfObject(pftr);
				if (!player)  continue;
				undo.oldLayerArr.Add(player_old->GetID());
				pds->SetFtrLayerOfObject(pftr,player->GetID());	
				undo.newLayerArr.Add(player->GetID()); 
				
				//改变层之后恢复缺省参数
				CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(player->GetName());
				if(pSchemeLayer)
				{
					CValueTable tab;
					pSchemeLayer->GetDefaultValues(tab);
					tab.DelField(FIELDNAME_FTRDISPLAYORDER);
					tab.DelField(FIELDNAME_GEOTEXT_CONTENT);
					tab.DelField(FIELDNAME_FTRMODIFYTIME);
					tab.DelField(FIELDNAME_GEOCLASS);
					tab.DelField(FIELDNAME_CLSTYPE);
					tab.DelField(FIELDNAME_GEOPOINT_ANGLE);
					tab.DelField(FIELDNAME_GEOCURVE_WIDTH);
					tab.DelField(FIELDNAME_GEOCURVE_DHEIGHT);
					tab.DelField(FIELDNAME_FTRPURPOSE);
					pftr->ReadFrom(tab);
				}
				
				pftr->GetGeometry()->SetSymbolName("");
				
				undo.newVT.BeginAddValueItem();
				pftr->WriteTo(undo.newVT);
				undo.newVT.EndAddValueItem();
				
				pdc->RestoreObject(FtrToHandle(pftr));
				
				batchUndo.AddAction(&undo);
			}
			
			batchUndo.Commit();
			
			pdc->RefreshView();
	        pdc->UpdateView(0,hc_SelChanged,TRUE);

		}

		break;
	default:
		break;
	}

    return 0;
}

HTREEITEM CFTRLayerViewBar::InsertRoot(LPCTSTR lpszItem, DWORD_PTR data)
{
	HTREEITEM hRoot = m_wndLayerView.InsertItem (lpszItem,0,0);
	if( hRoot )
	{
		m_wndLayerView.SetItemData(hRoot, data);
		m_wndLayerView.SetItemState (hRoot, TVIS_BOLD, TVIS_BOLD);
		m_wndLayerView.SetCheck(hRoot,TRUE);
	}
	
	return hRoot;
}

HTREEITEM CFTRLayerViewBar::InsertSubRoot(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR data, ItemPosition p)
{ 
	HTREEITEM hSubRoot;
	switch(p)
	{
	case IP_TOP:
		hSubRoot = m_wndLayerView.InsertItem (lpszItem, hParent,TVI_FIRST);
		break;
	case IP_LAST:
		hSubRoot = m_wndLayerView.InsertItem (lpszItem, hParent,TVI_LAST);
		break;
	case IP_SORT:
		hSubRoot = m_wndLayerView.InsertItem (lpszItem, hParent,TVI_SORT);
		break;
	default:
        hSubRoot = m_wndLayerView.InsertItem (lpszItem, hParent,0);
		break;
	}
	if( hSubRoot )
	{
		m_wndLayerView.SetItemData(hSubRoot, data);
		m_wndLayerView.SetCheck(hSubRoot,TRUE);
	}

	return hSubRoot;
}

HTREEITEM CFTRLayerViewBar::InsertItem(LPCTSTR lpszItem, HTREEITEM hParent, DWORD_PTR dwData, ItemPosition p)
{
	HTREEITEM hItem;
	switch(p)
	{
	case IP_TOP:
		hItem = m_wndLayerView.InsertItem (lpszItem, hParent,TVI_FIRST);
		break;
	case IP_LAST:
		hItem = m_wndLayerView.InsertItem (lpszItem, hParent,TVI_LAST);
		break;
	case IP_SORT:
		hItem = m_wndLayerView.InsertItem (lpszItem, hParent,TVI_SORT);
		break;
	default:
        hItem = m_wndLayerView.InsertItem (lpszItem, hParent,0);
		break;
	}
	if( hItem )
	{
		m_wndLayerView.SetItemData(hItem, dwData);
	}
	return hItem;
}

CString CFTRLayerViewBar::GetItemLabel(CString layer_name, int dblayer_type, int ftr_count)
{
	CString label_text;
	/*switch(dblayer_type)
	{
	case CLS_GEOPOINT:
	case CLS_GEOMULTIPOINT:
	case CLS_GEOTEXT:
	case CLS_GEODIRPOINT:
	case CLS_GEODEMPOINT:
	case CLS_GEOSURFACEPOINT:
		label_text+=symbol_point;
		label_text+=" ";
		break;
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOPARALLEL:
		label_text+=symbol_line;
		label_text+=" ";
		break;
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
		label_text+=symbol_polygon;
		label_text+=" ";
		break;
	default:
		break;
	}*/
	//
	label_text+=layer_name;
	char str_ftr_count[50];
    sprintf(str_ftr_count,"%d",ftr_count);
	label_text+="  ";
	label_text+=CString(str_ftr_count);

	//
    return label_text;
}

HTREEITEM CFTRLayerViewBar::GetDBLayerItem(CDlgDataSource* pds, CString strDBLayerName)
{
	if(!pds) return NULL;
	RootInfo* prootinfo = ds_root[pds->GetName()];
	
	std::map<CString,HTREEITEM>::iterator itr_tree_item = prootinfo->dblayer_item.find(strDBLayerName);
	
	HTREEITEM dblayer_item;
	if(itr_tree_item!=prootinfo->dblayer_item.end())
	{
		dblayer_item = prootinfo->dblayer_item[strDBLayerName];
	}
	else
	{
        dblayer_item = InsertSubRoot(strDBLayerName,prootinfo->root_item,ITEM_GROUP, IP_SORT);
		//
		m_wndLayerView.SetItemImage(dblayer_item,1,1);
		prootinfo->dblayer_item[strDBLayerName] = dblayer_item;
		item_check[dblayer_item] = TRUE;
		item_image[dblayer_item] = 1;
	}
	m_wndLayerView.SetCheck(dblayer_item,TRUE);

	//
	return dblayer_item;
}

HTREEITEM CFTRLayerViewBar::InsertLayerItem(CString ds_name, int dblayer_type, CFtrLayer* player, HTREEITEM hParent, DWORD_PTR dwData, ItemPosition p)
{
	if(!player) return NULL;
	CString label_text = GetItemLabel(player->GetName(),dblayer_type,player->GetValidObjsCount());
    //
	HTREEITEM layer_item = InsertItem(label_text,hParent,dwData,p);
	m_wndLayerView.SetCheck(layer_item, player->IsVisible());
	//
	COLORREF layer_color = player->GetColor();
	std::map<COLORREF,int>::iterator itr_img = color_img.find(layer_color);
	if(itr_img==color_img.end())
	{
		AddColorImage(layer_color);
	}
	//
	m_wndLayerView.SetItemImage(layer_item,color_img[layer_color],color_img[layer_color]);
	//
	item_check[layer_item] = player->IsVisible();
	item_image[layer_item] = color_img[layer_color];
    RootInfo* prootinfo = ds_root[ds_name];
	prootinfo->item_layerid[layer_item] = player->GetID();
	prootinfo->layerid_item[player->GetID()] = layer_item;
	//
	return layer_item;
}

void CFTRLayerViewBar::AddRootImage(int type)
{
	CDC memDC;                      //定义一个显示设备对象  
	CBitmap* memBitmap = new CBitmap();                      //定义位图  
	memDC.CreateCompatibleDC(m_wndLayerView.GetDC());         //创建CDC兼容设备  
	memBitmap->CreateCompatibleBitmap(m_wndLayerView.GetDC(),img_width,img_height);  //创建CDC兼容图片  
	memDC.SelectObject(memBitmap);                  //设备选择当前的图纸-位图  
 	memDC.FillSolidRect(0, 0, img_width, img_height, RGB(50,50,50));  //图纸上绘制信息 
	if(type == 0)
	{
		//memDC.Draw3dRect(0,0,img_width,img_height,RGB(0,0,0),RGB(0,0,0));
		memDC.FillSolidRect(3,3,img_width-6,img_height/2-4,RGB(255,150,45));
		memDC.FillSolidRect(3,img_height/2+1,img_width-6,img_height/2-4,RGB(255,150,45));
		memDC.DeleteDC();
		bmps.push_back(memBitmap);
	    images.Add(memBitmap,RGB(0,0,0));
	}
	else if(type == 1)
	{
        memDC.FillSolidRect(3,3,img_width-6,img_height/2-4,RGB(128,128,128));
		memDC.FillSolidRect(3,img_height/2+1,img_width-6,img_height/2-4,RGB(128,128,128));
		memDC.DeleteDC();
		bmps.push_back(memBitmap);
	    images.Add(memBitmap,RGB(0,0,0));
	}
	else if(type == 2)
	{
		memDC.FillSolidRect(2, 2, img_width - 4, img_height - 4, RGB(100, 207, 58));
		memDC.FillSolidRect(7, 4, img_width - 14, img_height - 8, RGB(40, 147, 225));
		memDC.DeleteDC();
		bmps.push_back(memBitmap);
		images.Add(memBitmap, RGB(0, 0, 0));
	}
	else if(type == 3)
	{
		memDC.FillSolidRect(2,2,img_width-4,img_height-4,RGB(50,50,50));
		memDC.DeleteDC();
		bmps.push_back(memBitmap);
	    images.Add(memBitmap,RGB(0,0,0));
	}
	
}

void CFTRLayerViewBar::AddSubRootImage()
{
	CDC memDC;                      //定义一个显示设备对象  
	CBitmap* memBitmap = new CBitmap();                      //定义位图  
	memDC.CreateCompatibleDC(m_wndLayerView.GetDC());         //创建CDC兼容设备  
	memBitmap->CreateCompatibleBitmap(m_wndLayerView.GetDC(),img_width,img_height);  //创建CDC兼容图片  
	memDC.SelectObject(memBitmap);                  //设备选择当前的图纸-位图  
	memDC.FillSolidRect(0, 0, img_width, img_height, RGB(50,50,50));  //图纸上绘制信息  
	//memDC.Draw3dRect(0,0,img_width,img_height,RGB(0,0,0),RGB(0,0,0));

	CPen* pPen; //创建一支新的笔
	CPen* pOldPen = new CPen;
	pPen = new CPen;
	pPen->CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	pOldPen = memDC.SelectObject(pPen);
	memDC.MoveTo(8,6);
	memDC.LineTo(img_width-3,6);
	memDC.LineTo(img_width-8,img_height-6);
	memDC.LineTo(3,img_height-6);
	memDC.LineTo(8,6);
	memDC.DeleteDC();
	bmps.push_back(memBitmap);
	images.Add(memBitmap,RGB(0,0,0));
}

void CFTRLayerViewBar::AddColorImage(COLORREF color)
{
	 std::map<COLORREF, int>::iterator itr = color_img.find(color);
	 if(itr != color_img.end())
		return;
	 //
	 CDC memDC;                      //定义一个显示设备对象  
	 CBitmap* memBitmap = new CBitmap();                      //定义位图  
	 memDC.CreateCompatibleDC(m_wndLayerView.GetDC());         //创建CDC兼容设备  
	 memBitmap->CreateCompatibleBitmap(m_wndLayerView.GetDC(),img_width,img_height);  //创建CDC兼容图片  
	 memDC.SelectObject(memBitmap);                  //设备选择当前的图纸-位图  
	 memDC.FillSolidRect(0, 0, img_width, img_height, color);  //图纸上绘制信息  
	 CRect rect;
	 rect.left = 0; rect.top = 0; rect.right = img_width; rect.bottom = img_height;
	 memDC.DrawFocusRect(&rect);
	 memDC.DeleteDC();
	 bmps.push_back(memBitmap);
	 //
	 images.Add(memBitmap,RGB(0,0,0));
	 color_img[color] = images.GetImageCount()-1;
}

void CFTRLayerViewBar::OnResetColor()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL) return;
	CDlgDataSource* pds = pdc->GetDlgDataSource();
	CFtrLayerArray arr;
	//
    for(int i=0; i<m_wndLayerView.getSelCount(); ++i)
	{
		HTREEITEM hitem = m_wndLayerView.getSelitem(i);
		ItemType item_type = GetItemType(hitem);
		//
		if(item_type == CFTRLayerViewBar::ItemType::ITEMFDB)
		{
            for(int t=0; t<pds->GetFtrLayerCount(); ++t)
			{
				CFtrLayer* player = pds->GetFtrLayerByIndex(t);
                if(player==NULL || player->GetObjectCount()<=0)
					continue;
				//
				arr.Add(player);
			}
		}
		else if(item_type == CFTRLayerViewBar::ItemType::ITEMDBLAYER || item_type == CFTRLayerViewBar::ItemType::ITEMGROUP)
		{
            HTREEITEM parent_item = m_wndLayerView.GetParentItem(hitem);
			for(HTREEITEM itm = m_wndLayerView.GetChildItem(hitem); itm!=NULL; itm = m_wndLayerView.GetNextSiblingItem(itm))
			{
                RootInfo* prootinfo = ds_root[pds->GetName()];
				CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[itm]);  
				if(player == NULL || player->GetObjectCount()<=0)
					continue;
				//
			    arr.Add(player);
			}
		}
		else if(item_type == CFTRLayerViewBar::ItemType::ITEMLAYER)
		{
            RootInfo* prootinfo = ds_root[pds->GetName()];
			CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[hitem]);  
			if(player == NULL || player->GetObjectCount()<=0)
				return;
			//
			arr.Add(player);
		}
	}
	
	CUndoModifyProperties undo(pdc, "ResetColor");
	pdc->BeginBatchUpdate();
	for(i=0; i<arr.GetSize(); i++)
	{
		int nObj = arr[i]->GetObjectCount();
		for(int j=0; j<nObj; ++j)
		{
			CPFeature pftr = arr[i]->GetObject(j);
			if(pftr==NULL || pftr->GetGeometry()==NULL)
				continue;
			if(-1==pftr->GetGeometry()->GetColor())
				continue;
			//
			undo.arrHandles.Add(FtrToHandle(pftr));
			undo.oldVT.BeginAddValueItem();
			pftr->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
			pdc->DeleteObject(FtrToHandle(pftr),FALSE);
			
			pftr->GetGeometry()->SetColor(-1);

			pdc->RestoreObject(FtrToHandle(pftr));
			undo.newVT.BeginAddValueItem();
			pftr->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();
		}
	}
	pdc->EndBatchUpdate();
	undo.Commit();
}

void CFTRLayerViewBar::OnColorLayout()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL)
		return;
	//
	HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	//
	ItemType item_type = GetItemType(hitem);
	if(item_type!=CFTRLayerViewBar::ItemType::ITEMDBLAYER && item_type!=CFTRLayerViewBar::ItemType::ITEMGROUP)
		return;
	HTREEITEM parent_item = m_wndLayerView.GetParentItem(hitem);
	CDlgDataSource* pds = NULL;
	int ds_index = 0;
	for(int i=0; i<pdc->GetDataSourceCount(); ++i)
	{
		if(m_wndLayerView.GetItemText(parent_item) == pdc->GetDlgDataSource(i)->GetName())
		{
			pds = pdc->GetDlgDataSource(i);
			ds_index = i;
			break;
		}	
	}
	if(pds == NULL)
	   return ;
	//
    CDlgColorLayout dlg;
	dlg.m_ColorCombobox.InitColor();
	dlg.m_ColorCombobox.SetCurSelByZoneid(item_colorzone[hitem]);
	if(dlg.DoModal() == IDOK)
	{
		if(pdc->GetActiveDataSourceIdx() != ds_index)
			return;
		//
		std::vector<COLORREF> color_zone;
		int zone_type;
		CString zone_id;
		dlg.m_ColorCombobox.GetSelectColorZone(color_zone, zone_type, zone_id);
		if(item_colorzone[hitem] == zone_id)
			return;
		//
		int item_count = 0;
		for(HTREEITEM itm = m_wndLayerView.GetChildItem(hitem); itm!=NULL; itm = m_wndLayerView.GetNextSiblingItem(itm))
		{
           ++item_count;
		}
		int p = 0;
		CUndoBatchAction undo(pdc, "SetColorZone");
		pdc->BeginBatchUpdate();
        for(itm = m_wndLayerView.GetChildItem(hitem); itm!=NULL; itm = m_wndLayerView.GetNextSiblingItem(itm))
		{
			RootInfo* prootinfo = ds_root[pds->GetName()];
			CFtrLayer* player = pds->GetFtrLayer(prootinfo->item_layerid[itm]);  
			if(player == NULL)
				continue;
			//
			COLORREF color;
			if(zone_type == BLOCK_ZONE)
			{
                color = color_zone[p%color_zone.size()];
			}
			else if(zone_type == GRADUAL_ZONE)
			{
				color = color_zone[(INT)(((double)color_zone.size()/(double)item_count)*p)];
			}
            //
			ChangeLayerColor(player,color,&undo);
			//
			std::map<COLORREF,int>::iterator itr = color_img.find(color);
			if(itr==color_img.end())
			{
				AddColorImage(color);
			}
			
			m_wndLayerView.SetItemImage(itm,color_img[color],color_img[color]);

			++p;
		}
		pdc->EndBatchUpdate();
		undo.Commit();
		//
		item_colorzone[hitem] = zone_id;
	}
}


int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    if(lParam1 == lParam2)
		return 0;
	else if(lParam2 > lParam1)
        return -1;
	else if(lParam2 < lParam1)
		return 1;

	return 0;
}

void CFTRLayerViewBar::OnMoveUp()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL)
		return;
	//
	HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	ItemType item_type = GetItemType(hitem);
	//
	if(item_type == CFTRLayerViewBar::ItemType::ITEMIMG)
	{
		CView* pview = GetActiveDlgDoc()->GetCurActiveView();
		if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
			return;
		CVectorView_new* pvectorview = (CVectorView_new*)pview;
		//
		CArray<ViewImgPosition,ViewImgPosition> imgs;
		pvectorview->GetViewImagePositions(imgs);
		for(int i=0; i<imgs.GetSize(); ++i)
		{
			if(imgs[i].fileName == m_wndLayerView.GetItemText(hitem))
			{
// 				if(i==0)
// 					return;
				//
// 				ViewImgPosition temp = imgs[i];
// 				imgs[i] = imgs[i-1];
// 				imgs[i-1] = temp;
				//
				if(i==imgs.GetSize()-1)
					return;
				ViewImgPosition temp = imgs[i];
				imgs[i] = imgs[i+1];
				imgs[i+1] = temp;
				//
				break;
			}
		}
		//
		pvectorview->UpdateOverlayImage(imgs);
		//
		for(std::map<CString,HTREEITEM>::iterator itr = refimg_item.begin(); itr!= refimg_item.end(); ++itr)
		{
			m_wndLayerView.DeleteItem(itr->second);
		}
		refimg_item.clear();
		//
	    Refresh(false,true);
	}
	else if(item_type == CFTRLayerViewBar::ItemType::ITEMDBLAYER || 
		    item_type == CFTRLayerViewBar::ItemType::ITEMGROUP)
	{
		HTREEITEM parent = m_wndLayerView.GetParentItem(hitem);
        CString ds_name = m_wndLayerView.GetItemText(parent);
		//
	    RootInfo* proot = ds_root[ds_name];
		std::vector<HTREEITEM> targets;
		//
		for(HTREEITEM target_item = m_wndLayerView.GetChildItem(parent); target_item!=NULL; target_item = m_wndLayerView.GetNextSiblingItem(target_item))
		{
			if(target_item != hitem)
              targets.push_back(target_item);
		}
		targets.insert(targets.begin(),hitem);
		//
		for(int i=0; i<targets.size(); ++i)
		{
			m_wndLayerView.SetItemData(targets[i],i);
		}
		//
        TVSORTCB tvs;
		// Sort the tree control's items using my
		// callback procedure.
		tvs.hParent = parent;
		tvs.lpfnCompare = MyCompareProc;
		tvs.lParam = (LPARAM)(&m_wndLayerView);
        m_wndLayerView.SortChildrenCB(&tvs);
	}
}

void CFTRLayerViewBar::OnMoveDown()
{
	CDlgDoc* pdc = GetActiveDlgDoc();
	if(pdc == NULL)
		return;
	//
	HTREEITEM hitem = m_wndLayerView.GetSelectedItem();
	ItemType item_type = GetItemType(hitem);
	//
	if(item_type == CFTRLayerViewBar::ItemType::ITEMIMG)
	{
		CView* pview = GetActiveDlgDoc()->GetCurActiveView();
		if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
			return;
		CVectorView_new* pvectorview = (CVectorView_new*)pview;
		//
		CArray<ViewImgPosition,ViewImgPosition> imgs;
		pvectorview->GetViewImagePositions(imgs);
		for(int i=0; i<imgs.GetSize(); ++i)
		{
			if(imgs[i].fileName == m_wndLayerView.GetItemText(hitem))
			{
// 				if(i==imgs.GetSize()-1)
// 					return;
				//
// 				ViewImgPosition temp = imgs[i];
// 				imgs[i] = imgs[i+1];
// 				imgs[i+1] = temp;
				//
				if(i==0)
					return;
				ViewImgPosition temp = imgs[i];
				imgs[i] = imgs[i-1];
				imgs[i-1] = temp;
				//
				break;
			}
		}
		//
		pvectorview->UpdateOverlayImage(imgs);
		//
		for(std::map<CString,HTREEITEM>::iterator itr = refimg_item.begin(); itr!= refimg_item.end(); ++itr)
		{
			m_wndLayerView.DeleteItem(itr->second);
		}
		refimg_item.clear();
		//
		Refresh(false,true);
	}
	else if(item_type == CFTRLayerViewBar::ItemType::ITEMDBLAYER || 
		item_type == CFTRLayerViewBar::ItemType::ITEMGROUP)
	{
		HTREEITEM parent = m_wndLayerView.GetParentItem(hitem);
        CString ds_name = m_wndLayerView.GetItemText(parent);
		//
		RootInfo* proot = ds_root[ds_name];
		std::vector<HTREEITEM> targets;
		//
		for(HTREEITEM target_item = m_wndLayerView.GetChildItem(parent); target_item!=NULL; target_item = m_wndLayerView.GetNextSiblingItem(target_item))
		{
			if(target_item != hitem)
				targets.push_back(target_item);
		}
		targets.push_back(hitem);
		//
		for(int i=0; i<targets.size(); ++i)
		{
			m_wndLayerView.SetItemData(targets[i],i);
		}
		//
        TVSORTCB tvs;
		// Sort the tree control's items using my
		// callback procedure.
		tvs.hParent = parent;
		tvs.lpfnCompare = MyCompareProc;
		tvs.lParam = (LPARAM)(&m_wndLayerView);
        m_wndLayerView.SortChildrenCB(&tvs);
	}
}

void CFTRLayerViewBar::clear()
{
	item_check.clear();
	item_image.clear();
	std::map<CString, RootInfo*>::iterator it = ds_root.begin();
	while (it != ds_root.end())
	{
		it = ds_root.erase(it);
	}
	ds_root.clear();
	refimg_item.clear();
	item_colorzone.clear();
	//
	m_wndLayerView.DeleteAllItems();
	//
	root_fdb = InsertRoot(StrFromResID(IDS_LAYERVIEW_NODE_FDB),0);
	root_img = InsertRoot(StrFromResID(IDS_LAYERVIEW_NODE_IMG),0);
	
	m_wndLayerView.SetItemImage(root_fdb,4,4);
	m_wndLayerView.SetItemImage(root_img,4,4);

	item_check[root_fdb] = TRUE;
	item_check[root_img] = TRUE;
}

void CFTRLayerViewBar::UpdateLayerView ()
{
	 clear();
	 //
	 CDlgDoc* pdoc = GetActiveDlgDoc();
	 if(pdoc == NULL)
		 return ;
	 //
	 int ds_count = pdoc->GetDlgDataSourceCount();
	 for(int i=0;i<ds_count;++i)
	 {
		 BOOL is_active_ds = pdoc->GetActiveDataSourceIdx()==i?TRUE:FALSE;
		 CDlgDataSource* pds = pdoc->GetDlgDataSource(i);
		 AddDatasourse(pds,is_active_ds);
	 }
	 m_wndLayerView.Expand(root_fdb,TVE_EXPAND);
	 //
	 CView* pview = GetActiveDlgDoc()->GetCurActiveView();
	 if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
		 return ;
	 CVectorView_new* pvectorview = (CVectorView_new*)pview;
	 //	 
	 CArray<ViewImgPosition,ViewImgPosition> ref_imgs;
	 pvectorview->GetViewImagePositions(ref_imgs);
	 
	 for(i=ref_imgs.GetSize()-1; i>=0; --i)
	 {
		 HTREEITEM itm = InsertSubRoot(ref_imgs[i].fileName,root_img,0,IP_LAST);
		 m_wndLayerView.SetCheck(itm,ref_imgs[i].is_visible);
		 m_wndLayerView.SetItemImage(itm,3,3);
		 refimg_item[ref_imgs[i].fileName] = itm;
		 item_check[itm] = ref_imgs[i].is_visible;
	 }

	 m_wndLayerView.Expand(root_img,TVE_EXPAND);
}

void CFTRLayerViewBar::Refresh(bool with_datasource, bool with_refimg)
{
	CDlgDoc* pdoc = GetActiveDlgDoc();
	if(pdoc == NULL)
		return ;
	//
	if(with_datasource)
	{
		std::map<CString,bool> db_existed;
		//
		int ds_count = pdoc->GetDlgDataSourceCount();
		for(int i=0;i<ds_count;++i)
		{
			CDlgDataSource* pds = pdoc->GetDlgDataSource(i);
			BOOL is_active_ds = pdoc->GetActiveDataSourceIdx()==i?TRUE:FALSE;
			std::map<CString,RootInfo*>::iterator itr = ds_root.find(pds->GetName());
			if(itr==ds_root.end())
			{
				AddDatasourse(pds,is_active_ds);
				db_existed[pds->GetName()] = true;
			}
			else
			{
				RefreshDatasourse(pds,is_active_ds);
				db_existed[pds->GetName()] = true;
			}
		}
		//
		for(std::map<CString,RootInfo*>::iterator itr_root=ds_root.begin(); itr_root != ds_root.end();)
		{
			std::map<CString,bool>::iterator itr_existed = db_existed.find(itr_root->first);
			if(itr_existed == db_existed.end())
			{
				DeleteDatasourse(itr_root->first);
				itr_root = ds_root.begin();
				//
				continue;
			}
			//
			++itr_root;
		}
	}
	//
	if(with_refimg)
	{
        CView* pview = GetActiveDlgDoc()->GetCurActiveView();
		if(pview==NULL || !pview->IsKindOf(RUNTIME_CLASS(CVectorView_new)))
			return;
	    CVectorView_new* pvectorview = (CVectorView_new*)pview;
		//	
		CArray<ViewImgPosition,ViewImgPosition> ref_imgs;
		pvectorview->GetViewImagePositions(ref_imgs);
		//
		for(std::map<CString,HTREEITEM>::iterator itr_img = refimg_item.begin(); itr_img!=refimg_item.end();)
		{
			m_wndLayerView.DeleteItem(itr_img->second);
			item_check.erase(itr_img->second);
			refimg_item.erase(itr_img);	
			if(refimg_item.size()>0)
			{
				itr_img = refimg_item.begin();
			    continue;
			}
			else
				break;
		}
		//
		for(int i=ref_imgs.GetSize()-1; i>=0; --i)
		{
			HTREEITEM itm = InsertSubRoot(ref_imgs[i].fileName,root_img,0,IP_LAST);
			m_wndLayerView.SetCheck(itm,ref_imgs[i].is_visible);
			m_wndLayerView.SetItemImage(itm,3,3);
			refimg_item[ref_imgs[i].fileName] = itm;
			item_check[itm] = ref_imgs[i].is_visible;
		}
	    //
		m_wndLayerView.Expand(root_img,TVE_EXPAND);
	}
}


void CFTRLayerViewBar::AddDatasourse(CDlgDataSource* pds, BOOL is_actived)
{
	if(!pds) return;
	CString ds_name = pds->GetName();
	HTREEITEM root = InsertSubRoot(ds_name,root_fdb,0);
	m_wndLayerView.SetCheck(root,TRUE);
	if(is_actived)
	  m_wndLayerView.SetItemImage(root,0,0);
	else
	  m_wndLayerView.SetItemImage(root,2,2);
	item_check[root] = TRUE;
	item_image[root] = 0;

	ds_root[ds_name] = new RootInfo();
	ds_root[ds_name]->root_item = root;
	//
	int layer_count = pds->GetFtrLayerCount();
	CScheme *pScheme = gpCfgLibMan->GetScheme(pds->GetScale());
	for(int j=0; j<layer_count; ++j)
	{
		CFtrLayer* player = pds->GetFtrLayerByIndex(j);
		if(!player || player->GetValidObjsCount()==0)
			continue;
		//
		if(current_subtype == CFTRLayerViewBar::SUBTYPE::DBLAYER)
		{
			CSchemeLayerDefine *pSchemeLayer = pScheme==NULL?NULL:pScheme->GetLayerDefine(player->GetName());
			HTREEITEM dblayer_item = GetDBLayerItem(pds,pSchemeLayer==NULL?"":pSchemeLayer->GetDBLayerName());
			int dblayer_ftr_type = pSchemeLayer==NULL?player->GetClassType():pSchemeLayer->GetDbGeoClass();
            InsertLayerItem(pds->GetName(), dblayer_ftr_type, player, dblayer_item,ITEM_LAYER);
		}
		else if(current_subtype == CFTRLayerViewBar::SUBTYPE::GROUP)
		{
			HTREEITEM dblayer_item = GetDBLayerItem(pds,player->GetGroupName());
            InsertLayerItem(pds->GetName(),player->GetClassType(),player,dblayer_item,ITEM_LAYER);
		}
			
	}
	//
	m_wndLayerView.Expand(root, TVE_EXPAND);
}

void CFTRLayerViewBar::RefreshDatasourse(CDlgDataSource* pds, BOOL is_actived)
{
	if(!pds) return;
	std::map<int,bool> layer_existed;
	RootInfo* prootinfo = ds_root[pds->GetName()];

	CScheme *pScheme = gpCfgLibMan->GetScheme(pds->GetScale());
	if(!pScheme) return;
	//
	if(is_actived)
		m_wndLayerView.SetItemImage(prootinfo->root_item,0,0);
	else
        m_wndLayerView.SetItemImage(prootinfo->root_item,2,2);
	//
    for(int i=0; i<pds->GetFtrLayerCount(); ++i)
	{
		CFtrLayer* player = pds->GetFtrLayerByIndex(i);
		if(!player) continue;
		//
		std::map<int,HTREEITEM>::iterator itr_item = prootinfo->layerid_item.find(player->GetID());
		if(itr_item==prootinfo->layerid_item.end() && player->GetValidObjsCount()==0)
		{
            continue;
		}
		//
		CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(player->GetName());		
		int layer_ftr_type;
		if(current_subtype == CFTRLayerViewBar::SUBTYPE::DBLAYER)
		{
			layer_ftr_type = pSchemeLayer==NULL?player->GetClassType():pSchemeLayer->GetDbGeoClass();
		}
		else if(current_subtype == CFTRLayerViewBar::SUBTYPE::GROUP)
		{
			layer_ftr_type = player->GetClassType();
		}
		//label
        if(itr_item==prootinfo->layerid_item.end() && player->GetValidObjsCount()>0)
		{
			if(current_subtype == CFTRLayerViewBar::SUBTYPE::DBLAYER)
			{
				HTREEITEM dblayer_item = GetDBLayerItem(pds,pSchemeLayer==NULL?"":pSchemeLayer->GetDBLayerName());	
				InsertLayerItem(pds->GetName(),layer_ftr_type,player,dblayer_item,ITEM_LAYER,IP_SORT);
			}
			else if(current_subtype == CFTRLayerViewBar::SUBTYPE::GROUP)
			{
				HTREEITEM dblayer_item = GetDBLayerItem(pds,player->GetGroupName());
				InsertLayerItem(pds->GetName(),layer_ftr_type,player,dblayer_item,ITEM_LAYER,IP_SORT);
			}	
		}
		else if(itr_item!=prootinfo->layerid_item.end() && player->GetValidObjsCount()>0)
		{	
			CString label_text = GetItemLabel(player->GetName(),layer_ftr_type,player->GetValidObjsCount());
			m_wndLayerView.SetItemText(prootinfo->layerid_item[player->GetID()],label_text);
		}
		else if(itr_item!=prootinfo->layerid_item.end() && player->GetValidObjsCount()==0)
		{
			DeleteFtrLayer(pds->GetName(),player->GetID());
		}
		//checked  color
        itr_item = prootinfo->layerid_item.find(player->GetID());
		if(itr_item==prootinfo->layerid_item.end())
		{
			continue;
		}
		else
		{
			m_wndLayerView.SetCheck(prootinfo->layerid_item[player->GetID()],player->IsVisible());
			//
			std::map<COLORREF,int>::iterator itr_color = color_img.find(player->GetColor());
			if(itr_color==color_img.end())
			{
				AddColorImage(player->GetColor());
			}
			//
			m_wndLayerView.SetItemImage(prootinfo->layerid_item[player->GetID()],color_img[player->GetColor()],color_img[player->GetColor()]);
		}
	}
}

void CFTRLayerViewBar::DeleteDatasourse(CString ds_name)
{
	CString root_label = ds_name;
	HTREEITEM root = GetSubRootItemByLabel(root_fdb,root_label);
	if(root == NULL)
		return;
	//
	std::map<CString,RootInfo*>::iterator itr_root = ds_root.find(ds_name);
	if(itr_root == ds_root.end())
		return;
	//
	RootInfo* prootinfo = ds_root[ds_name];
    for(std::map<int,HTREEITEM>::iterator itr = prootinfo->layerid_item.begin(); itr!= prootinfo->layerid_item.end();)
	{
		DeleteFtrLayer(ds_name,itr->first);
		itr = prootinfo->layerid_item.begin();
	}
	//
    item_check.erase(root);
	item_image.erase(root);
	m_wndLayerView.DeleteItem(root);
	//
	delete prootinfo;
	ds_root.erase(ds_name);
}
// 
void CFTRLayerViewBar::DeleteFtrLayer(CString ds_name, int layer_id)
{
    CString root_label = ds_name;
	HTREEITEM root = GetSubRootItemByLabel(root_fdb,root_label);
	if(root == NULL)
		return;
	//
	RootInfo* prootinfo = ds_root[ds_name];
	std::map<int,HTREEITEM>::iterator itr = prootinfo->layerid_item.find(layer_id);
	if(itr == prootinfo->layerid_item.end())
		return;
	//
	DeleteFtrLayer(prootinfo,layer_id);
}

void CFTRLayerViewBar::DeleteFtrLayer(RootInfo* prootinfo, int layer_id)
{
	HTREEITEM layer_item = prootinfo->layerid_item[layer_id];
	HTREEITEM sub_root = m_wndLayerView.GetParentItem(layer_item);
	//
	std::map<int,HTREEITEM>::iterator itr = prootinfo->layerid_item.find(layer_id);
	//
	item_check.erase(layer_item);
	item_image.erase(layer_item);
	prootinfo->item_layerid.erase(layer_item);
	prootinfo->layerid_item.erase(itr);
	
	m_wndLayerView.DeleteItem(layer_item);
    
	//
	HTREEITEM temp = m_wndLayerView.GetChildItem(sub_root);
	if(temp == NULL)
	{
		item_check.erase(sub_root);
		item_image.erase(sub_root);
		prootinfo->dblayer_item.erase(m_wndLayerView.GetItemText(sub_root));
		m_wndLayerView.DeleteItem(sub_root);	
	}
}

void CFTRLayerViewBar::DeleteLayerGroup(CString ds_name, CString group_name)
{
    CString root_label = ds_name;
	HTREEITEM root = GetSubRootItemByLabel(root_fdb,root_label);
	if(root == NULL)
		return;
	//
	RootInfo* prootinfo = ds_root[ds_name];
	
	//
	std::vector<int> temps;
	for(std::map<HTREEITEM,int>::iterator itr_layer = prootinfo->item_layerid.begin(); itr_layer != prootinfo->item_layerid.end(); ++itr_layer)
	{
		temps.push_back(itr_layer->second);
	}
	//
	for(int i=0; i<temps.size(); ++i)
	{
		std::map<int,HTREEITEM>::iterator itr = prootinfo->layerid_item.find(temps[i]);
		if(itr == prootinfo->layerid_item.end())
		   continue;
		//
		DeleteFtrLayer(prootinfo, temps[i]);
	}
}

void CFTRLayerViewBar::OnUpdateManual()
{
    Refresh();
}





HBRUSH CFTRLayerViewBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);
	m_brush.CreateSolidBrush(RGB(50, 50, 50));
	// TODO:  Return a different brush if the default is not desired
	return m_brush;
	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
