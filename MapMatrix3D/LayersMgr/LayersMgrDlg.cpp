// LayersMgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "LayersMgrDlg.h"
#include "SmartViewFunctions.h"
#include <io.h>
#include "DlgDataSource.h"
#include "Markup.h"
#include "ObjectXmlIo.h"
#include "ExMessage.h "
#include "Scheme.h"
#include "SymbolLib.h"
#include "dlgxattributes.h"
#include "dlgchangefcode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//BOOL CLayersMgrDlg::m_bWarningAll = FALSE;

#define IDC_LM_TREE				1000
#define IDC_LM_PROPLIST			1001

#define FILTER_FILEPATH		(GetConfigPath(TRUE)+_T("\\Default.lcf"))

//extern void ExtractPath0(LPCTSTR pathname, CString *path, CString *name, CString *ext);
template<>
UINT AFXAPI HashKey<XAttributeItem&> (XAttributeItem &key)
{
    return (HashKey((LPCTSTR)(key.field))+HashKey((LPCTSTR)(key.value))+key.valuetype);
}

BOOL Variant2XAttribute(const CVariantEx &variant, XAttributeItem &item)
{
	switch(variant.GetType())
	{
	case VT_BOOL: 
		item.valuetype = DP_CFT_BOOL;
		item.value.Format("%d",(int)(bool)(_variant_t)variant);
		break;
	case VT_I2:
		item.valuetype = DP_CFT_SMALLINT;
		item.value.Format("%i",(int)(long)(_variant_t)variant);
		break;
	case VT_I4:
		item.valuetype = DP_CFT_INTEGER;
		item.value.Format("%i",(int)(long)(_variant_t)variant);
		break;
	case VT_BSTR:
		item.valuetype = DP_CFT_VARCHAR;
		item.value = (const char*)(_bstr_t)(_variant_t)variant;
		break;
	case VT_R4:
		item.valuetype = DP_CFT_FLOAT;
		item.value.Format("%f",(float)(_variant_t)variant);
		break;
	case VT_R8:
		item.valuetype = DP_CFT_DOUBLE;
		item.value.Format("%lf",(double)(_variant_t)variant);
		break;
	default:
		return FALSE;
		break;	
		
	}
	
	return TRUE;
	
}

CVariantEx XAttribute2Variant(const XAttributeItem &item)
{
	CVariantEx var;
	switch(item.valuetype)
	{
	case DP_CFT_BOOL: 
		var = (bool)atol(item.value);
		break;
	case DP_CFT_COLOR:
	case DP_CFT_INTEGER:
		var = atol(item.value);
		break;
	case DP_CFT_SMALLINT:
		var = (short)atol(item.value);
		break;
	case DP_CFT_VARCHAR:
		var = (LPCTSTR)item.value;
		break;
	case DP_CFT_FLOAT:
		var = (float)atof(item.value);
		break;
	case DP_CFT_DOUBLE:
		var = atof(item.value);
		break;
	case DP_CFT_DATE:
		var = (LPCTSTR)item.value;
		break;
	default:
		break;	
		
	}
	
	return var;
}

BOOL ConvertXAttributeItemAndValueTab(CArray<XAttributeItem,XAttributeItem> &arrXAttibutes,CValueTable &tab,int mode)
{
	// CValueTable --> XAttributeItem
	if (mode == 0)
	{
		int size = tab.GetFieldCount();
		for (int i=0; i<size ;i++)
		{
			CString field, name;
			int type;
			tab.GetField(i,field,type,name);
			
			XAttributeItem item;
			item.field = field;
			item.name = name;
			
			const CVariantEx *var;
			tab.GetValue(0,field,var);
			Variant2XAttribute(*var,item);
			
			arrXAttibutes.Add(item);
		}
		
	}
	// XAttributeItem --> CValueTable
	else
	{
		tab.BeginAddValueItem();
		for (int i=0; i<arrXAttibutes.GetSize(); i++)
		{
			XAttributeItem item = arrXAttibutes[i];
			CVariantEx var = XAttribute2Variant(item);
			tab.AddValue(item.field,&var,item.name);
		}
		tab.EndAddValueItem();
	}
	
	return TRUE;
}
/*
LayerGroup::LayerGroup(const LayerGroup& group)
{
	name = group.name;
	color = group.color;
	bVisible = group.bVisible;
	bSymbolized = group.bSymbolized;
	arr.Copy(group.arr);
}
LayerGroup& LayerGroup::operator=(const LayerGroup& group)
{
	name = group.name;
	color = group.color;
	bVisible = group.bVisible;
	bSymbolized = group.bSymbolized;
	arr.Copy(group.arr);
	return *this;
}

void LayerGroup::AddFtrLayer(CFtrLayer *pLayer)
{
	arr.Add(pLayer);
}

void LayerGroup::DelFtrLayer(CFtrLayer *pLayer)
{
	for (int i=arr.GetSize()-1; i>=0; i--)
	{
		if (arr[i] == pLayer)
		{
			arr.RemoveAt(i);
			break;
		}
	}
}
*/
CManageBaseDlg::CManageBaseDlg( UINT nIDTemplate, CWnd* pParentWnd /* = NULL */ )
: CDialog(nIDTemplate, pParentWnd)
{
	m_nSortColumn = -1;
}
/////////////////////////////////////////////////////////////////////////////
// CLayersMgrDlg dialog

CLayersMgrDlg::CLayersMgrDlg(CWnd* pParent /*=NULL*/)
: CManageBaseDlg(CLayersMgrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLayersMgrDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
		
	m_bMouseInSplitter	= false;
	m_bTrackSplitter	= false;	
		
	m_iMinTreeCtrlWidth	= 100;
	m_iMinListCtrlWidth	= 100;

	m_pDlgDoc = GetActiveDlgDoc();
	m_bIsCuttingObj = FALSE;

	m_pMovedLayer = NULL;

	m_dCurSelItem = 0;
	
}

void CLayersMgrDlg::SetDoc(CDlgDoc *pDoc)
{
	m_pDlgDoc = pDoc;
}

void CLayersMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayersMgrDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX,IDC_STATIC_STATUS,m_wndStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLayersMgrDlg, CDialog)
	//{{AFX_MSG_MAP(CLayersMgrDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_NEW_FILTER, OnNewFilter)
	ON_COMMAND(ID_RENAME_FILTER, OnRenameFilter)
	ON_COMMAND(ID_DELETE_FILTER, OnDeleteFilter)
	ON_UPDATE_COMMAND_UI(ID_DELETE_FILTER, OnUpdateDeleteFilter)
	ON_UPDATE_COMMAND_UI(ID_RENAME_FILTER, OnUpdateRenameFilter)
	ON_COMMAND(ID_NEWLAYER, OnNewLayer)
	ON_COMMAND(ID_TOTOPE,OnToTop)
	ON_COMMAND(ID_TOBOTTOM,OnToBottom)
	ON_COMMAND(ID_CONVERTLAYER,OnConvertLayer)
	ON_COMMAND(ID_NEWLAYERGROUP, OnNewLayerGroup) //新建层组菜单命令
	ON_COMMAND(ID_DELLAYERGROUP, OnDelLayerGroup)
	ON_UPDATE_COMMAND_UI(ID_NEWLAYER, OnUpdateNewLayer)
	ON_COMMAND(ID_DELLAYER, OnDelLayer)
	ON_COMMAND(ID_ADDLAYER_SCHEME, OnAddLayerScheme)
	ON_COMMAND(ID_DELLAYER_SCHEME, OnDelLayerScheme)
	ON_COMMAND(ID_SHOW_TOP, OnShowTop)
	ON_COMMAND(ID_SHOW_TAIL, OnShowBottom)
	ON_COMMAND(ID_SHOW_MOVE, OnShowMove)
	ON_COMMAND(ID_SHOW_INSERT, OnShowInsert)
	ON_UPDATE_COMMAND_UI(ID_DELLAYER, OnUpdateDelLayer)
	ON_NOTIFY(NM_RCLICK,IDC_LM_TREE,OnRclick)
	ON_NOTIFY(TVN_SELCHANGED,IDC_LM_TREE,OnSelChanged)
	ON_NOTIFY(TVN_BEGINLABELEDIT,IDC_LM_TREE,OnBeginlabeledit)
	ON_NOTIFY(TVN_ENDLABELEDIT,IDC_LM_TREE,OnEndlabeledit)
	//}}AFX_MSG_MAP
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_DELLAYEROBJ, OnDelLayerObj)
	ON_UPDATE_COMMAND_UI(ID_DELLAYEROBJ, OnUpdateDelLayerObj)
	ON_COMMAND(ID_ACTIVECURLAYER, OnActiveLayer)
	ON_UPDATE_COMMAND_UI(ID_ACTIVECURLAYER, OnUpdateActiveLayer)
	ON_COMMAND(ID_SELECTALLLAYERS, OnSelectAllLayers)
	ON_UPDATE_COMMAND_UI(ID_SELECTALLLAYERS, OnUpdateSelectAllLayers)
	ON_COMMAND(ID_DESELECTLAYERS, OnDeSelectLayers)
	ON_UPDATE_COMMAND_UI(ID_DESELECTLAYERS, OnUpdateDeSelectLayers)
	ON_COMMAND(ID_SELECTALLEXCPETCURSEL, OnSelectAllLayersExceptCurSels)
	ON_UPDATE_COMMAND_UI(ID_SELECTALLEXCPETCURSEL, OnUpdateSelectAllLayersExceptCurSels)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_CUTLAYEROBJ,OnCutSelectLayers)
	ON_UPDATE_COMMAND_UI(ID_CUTLAYEROBJ,OnUpdateCutSelectLayers)
	ON_COMMAND(ID_PASTELAYEROBJ,OnPasteLayer)
	ON_UPDATE_COMMAND_UI(ID_PASTELAYEROBJ,OnUpdatePasteLayer)
	ON_COMMAND(ID_SELECTFTRS,OnSelectFtrs)	
	ON_COMMAND(ID_DESELECTFTRS,OnDeSelectFtrs)
	ON_COMMAND(ID_PASTELAYEROBJEX,OnPasteLayerEx)
	ON_UPDATE_COMMAND_UI(ID_PASTELAYEROBJEX,OnUpdatePasteLayerEx)
	ON_COMMAND(ID_RESET_DISAPLAYORDER,OnResetDisplayOrder)
	ON_COMMAND(ID_RESET_DEFAULTCOLOR,OnResetDefalutLayerColor)
	ON_COMMAND(ID_LOAD_XATTRIBUTES,OnLoadXAttributes)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayersMgrDlg message handlers

BOOL CLayersMgrDlg::OnInitDialog()
{
	m_bMouseInSplitter	= false;
	m_bTrackSplitter	= false;	
	
	m_iMinTreeCtrlWidth	= 100;
	m_iMinListCtrlWidth	= 100;
	
	m_pDlgDoc = GetActiveDlgDoc();
	m_bIsCuttingObj = FALSE;
	
	m_pMovedLayer = NULL;
	
	m_dCurSelItem = 0;

	if( !m_pDlgDoc )
	{
		return FALSE;
	}
	
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon	
	// 新建一个工具条
	CreateToolbar(m_wndToolBarFilter,IDR_TOOLBARFILTER);
	
	if (!InitCtrls())
	{
		return FALSE;
	}

	m_wndStatus.SetWindowText(_T(""));

	AdjustLayout();

	LoadFilterFile(FILTER_FILEPATH);
	FillTree();
	
	m_pDlgDoc->BeginBatchUpdate();

//	m_bWarningAll = FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLayersMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);	
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CLayersMgrDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLayersMgrDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CLayersMgrDlg::CreateToolbar(CMFCToolBar& toolbar, UINT ID)
{
	toolbar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_HIDE_INPLACE, ID);
	toolbar.LoadToolBar (ID, 0, 0, TRUE /* Is locked */);
	
	toolbar.SetPaneStyle(toolbar.GetPaneStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	toolbar.SetPaneStyle(
		toolbar.GetPaneStyle() &
		~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP |
		CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
		
	toolbar.SetOwner (this);
	
	// All commands will be routed via this control , not via the parent frame:
	toolbar.SetRouteCommandsViaFrame (FALSE);
	
	return true;
}


void CLayersMgrDlg::AdjustLayout()
{
	if (GetSafeHwnd () == NULL)
		return;
	
	CRect rectClient;
	GetClientRect (rectClient);
	
	if (m_wndToolBarFilter.m_hWnd == NULL)
		return;
	
	int cyTlb = m_wndToolBarFilter.CalcFixedLayout (FALSE, TRUE).cy;
	
	CStatic*	sta = (CStatic*)GetDlgItem(IDC_STATIC_TOP);

	CRect rcStatus;
	m_wndStatus.GetWindowRect(&rcStatus);	
	
	//画工具条上边框
	RECT	rt;
	sta->GetWindowRect(&rt);
	rt.left		= rectClient.left;
	rt.right	= rt.left + rectClient.Width();
	int		iHeight	= rt.bottom - rt.top;
	rt.top		= rectClient.top;
	rt.bottom	= rt.top + iHeight;
	
	sta->SetWindowPos(NULL,rt.left,rt.top,rectClient.Width(),rt.bottom-rt.top,SWP_NOACTIVATE | SWP_NOZORDER);
	
	//修正工具条的位置
	m_wndToolBarFilter.SetWindowPos (NULL, rectClient.left, rectClient.top+2, 
		rectClient.Width (), cyTlb+2,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndStatus.SetWindowPos(NULL,rectClient.right-rcStatus.Width()-2,rectClient.top+2 + (cyTlb+2)/2-rcStatus.Height()/2, 
		rcStatus.Width(), rcStatus.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);
	
	//画工具条下边框
	sta = (CStatic*)GetDlgItem(IDC_STATIC_BOTTOM);
	OffsetRect(&rt,0,cyTlb+2);
	sta->SetWindowPos(NULL,rt.left,rt.top,rectClient.Width(),rt.bottom-rt.top,SWP_NOACTIVATE | SWP_NOZORDER);
	
	//修正树控件的位置
	CRect		rtTree;
	m_wndTreeCtrl.GetClientRect(&rtTree);
	if (rtTree.Width() < m_iMinTreeCtrlWidth)
	{
		m_iTreeCtrlWidth	= m_iMinTreeCtrlWidth + 50;
	}
	
	rt.left		= rectClient.left;
	rt.right	= rt.left + m_iTreeCtrlWidth;
	rt.top		= cyTlb + 3;
	rt.bottom	= rectClient.bottom;
	
	::InflateRect(&rt,-1,-1);
	
	m_wndTreeCtrl.SetWindowPos(NULL,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,SWP_NOACTIVATE | SWP_NOZORDER);
	
	//修正PropListCtrl的控件
	rt.left		= rt.right + SPLITTER_WIDTH;
	rt.right	= rectClient.right - 1;
	m_wndPropListCtrl.SetWindowPos(NULL,rt.left,rt.top,rt.right-rt.left,rt.bottom-rt.top,SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CLayersMgrDlg::InitCtrls()
{
	RECT rt;
	::SetRect(&rt,0,0,0,0);
	
	if (!m_wndPropListCtrl.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rt,this,IDC_LM_PROPLIST) || 
		!m_wndTreeCtrl.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rt,this,IDC_LM_TREE))
		return FALSE;

	m_ImageList.Create(IDB_TREE, 16, 0, RGB(255, 0, 255));
	
	m_wndTreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);
	
	return TRUE;
}

void CLayersMgrDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	AdjustLayout();	
}

void CLayersMgrDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bTrackSplitter)						//Splitter拖动操作
	{
		RECT	rtTreeCtrl;
		RECT	rtListCtrl;
		m_wndTreeCtrl.GetWindowRect(&rtTreeCtrl);
		m_wndPropListCtrl.GetWindowRect(&rtListCtrl);
		CPoint	pt;
		pt.x	= point.x;	pt.y	= point.y;
		ClientToScreen(&pt);
		
		rtTreeCtrl.right	= pt.x - 1;
		rtListCtrl.left		= rtTreeCtrl.right + SPLITTER_WIDTH;
		
		m_iTreeCtrlWidth	= rtTreeCtrl.right - rtTreeCtrl.left;
		
		if (m_iTreeCtrlWidth <= m_iMinTreeCtrlWidth)
			return;
		if ((rtListCtrl.right - rtListCtrl.left) <= m_iMinListCtrlWidth)
			return;
		
		ScreenToClient(&rtTreeCtrl);
		m_wndTreeCtrl.SetWindowPos(NULL,rtTreeCtrl.left,rtTreeCtrl.top,rtTreeCtrl.right-rtTreeCtrl.left,
			rtTreeCtrl.bottom-rtTreeCtrl.top,SWP_NOACTIVATE | SWP_NOZORDER);
		
		ScreenToClient(&rtListCtrl);
		m_wndPropListCtrl.SetWindowPos(NULL,rtListCtrl.left,rtListCtrl.top,rtListCtrl.right-rtListCtrl.left,
			rtListCtrl.bottom-rtListCtrl.top,SWP_NOACTIVATE | SWP_NOZORDER);
	}
	else
	{
		if (MouseCursorIsSplitter(point))
		{
			::SetCursor(LoadCursor(NULL,IDC_SIZEWE));
			m_bMouseInSplitter	= true;
		}
		else
		{
			::SetCursor(LoadCursor(NULL,IDC_ARROW));
			m_bMouseInSplitter	= false;
		}
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

//表示鼠标的光标变为用作切分窗口的光标
BOOL CLayersMgrDlg::MouseCursorIsSplitter(CPoint pt)
{
	RECT	rtTreeCtrl;
	RECT	rtListCtrl;
	m_wndTreeCtrl.GetWindowRect(&rtTreeCtrl);
	m_wndPropListCtrl.GetWindowRect(&rtListCtrl);
	
	RECT	rtSplitter;
	
	rtSplitter.left		= rtTreeCtrl.right;
	rtSplitter.right	= rtListCtrl.left;
	rtSplitter.top		= rtTreeCtrl.top;
	rtSplitter.bottom	= rtTreeCtrl.bottom;
	
	this->ClientToScreen(&pt);
	
	return ::PtInRect(&rtSplitter,pt);
}

void CLayersMgrDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonDown(nFlags, point);
	
	if (m_bMouseInSplitter)
	{
		::SetCursor(LoadCursor(NULL,IDC_SIZEWE));
		::SetCapture(m_hWnd);
		m_bTrackSplitter	= true;
	}	
}

void CLayersMgrDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CDialog::OnLButtonUp(nFlags, point);
	
	if (m_bTrackSplitter)
	{
		m_bTrackSplitter	= false;
		::ReleaseCapture();
	}	
}


BOOL CLayersMgrDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rectClient;	// Client area rectangle
	GetClientRect (&rectClient);
	
	//计算无效区域,使无效区域不刷新
	CRgn		rgnDraw;
	
	if (m_wndPropListCtrl.m_hWnd)
	{
		rgnDraw.CreateRectRgnIndirect(&rectClient);
		
		CRgn	rgnProp;
		CRect	rtProp;
		m_wndPropListCtrl.GetWindowRect(&rtProp);
		ScreenToClient(&rtProp);
		rtProp.DeflateRect(2,2,2,2);
		
		
		rgnProp.CreateRectRgnIndirect(&rtProp);
		
		rgnDraw.CombineRgn(&rgnDraw,&rgnProp,RGN_DIFF);
		
		
		CBrush	br(GetGlobalData()->clrBtnFace);
		pDC->FillRgn(&rgnDraw,&br);
	}
	else
	{
		pDC->FillSolidRect(&rectClient, GetGlobalData()->clrBtnFace);
	}
	
	return true;
}

void CLayersMgrDlg::OnCancel()
{

	CMFCHeaderCtrl *header = (CMFCHeaderCtrl*)&m_wndPropListCtrl.GetHeaderCtrl();
	m_nSortColumn = header->GetSortColumn();
	m_bSortAscending = header->IsAscending();

	SaveFilter(NULL);
	SaveFilterFile(FILTER_FILEPATH);
	
	m_pDlgDoc->EndBatchUpdate();
	EndDialog(IDNO);
}

void CLayersMgrDlg::OnDestroy() 
{
	m_arrFilters.RemoveAll();
	m_arrXAttibutes.RemoveAll();
	m_arrMemXAttr.RemoveAll();
	RemoveFilter(NULL);

	m_pMovedLayer = NULL;
	m_pDlgDoc = NULL;
	m_arrCutLayers.RemoveAll();
	
	RemoveFilter(NULL);
	CDialog::OnDestroy();
}

//新建层
void CLayersMgrDlg::OnNewLayer()
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CFtrLayer *pLayer = new CFtrLayer;
	CString name;

	for( int i=pDS->GetFtrLayerCount(); ; i++)
	{
		name.Format("NewLayer%d",i);
		if( pDS->GetFtrLayer(name)==NULL )break;
	}
//	pLayer->SetID(CreateLayerID());
	pLayer->SetName(name);
	m_pDlgDoc->AddFtrLayer(pLayer);//添加到文档类中

	CUndoNewDelLayer undo(m_pDlgDoc,"NewLayer");
	undo.isDel = FALSE;
	undo.arrObjs.Add(pLayer);
	undo.Commit();
	
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS )
	//	FillAllLayersList(pDS,FALSE);	
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),pDS,TRUE);
	else if (hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM)
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),pDS);
	}

}

#include "DlgNewGroup.h"
//新建层组
void CLayersMgrDlg::OnNewLayerGroup()
{
	CDlgNewGroup group;
	group.SetStytle(StrFromResID(IDS_DLG_NEWLAYERGROUP),StrFromResID(IDS_DLG_LAYERGROUPNAME));


	CString strGroupName;   //新建组名
	if (group.DoModal() != IDOK)
		return;
	
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();

	strGroupName = group.GetGroupName();
	CStringArray groupNames;
	for(int i=0;i<pDS->GetFtrLayerGroupCount();i++)
	{
		groupNames.Add( pDS->GetFtrLayerGroup(i)->Name );
	}
	//判断输入是否重名
	while(IsExist(strGroupName,groupNames))
	{
		CString szText,szCaption;
		szText.LoadString(IDS_GROUP_RENAME);
		szCaption.LoadString(IDS_RE_INPUT);
		if (IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) && IDOK == group.DoModal())
			strGroupName = group.GetGroupName();
		else
			return;
	}	

	
	FtrLayerGroup *pNewGroup = new FtrLayerGroup;
	CString name;
	
	pNewGroup->Name = strGroupName;

	pDS->AddFtrLayerGroup(pNewGroup);//添加新项到List中
	
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_GROUPS )
	FillAllGroups(pDS);	
	
}

//删除层组
void CLayersMgrDlg::OnDelLayerGroup()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 )return;

	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}

	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	//CUndoNewDelLayer undo(m_pDlgDoc,"DelLayer");

	//弹出提示框是否删除指定层组
	CString str;
	str.LoadString(IDS_LAYERGROUPMANGER_STR);
	if(AfxMessageBox(str,MB_OKCANCEL)==IDCANCEL)
		return;		
		
// 	CUndoModifyLayer undo(m_pDlgDoc,_T("ModifyLayer"));
// 	undo.field.Format("%s",FIELDNAME_LAYGROUPNAME);
	
	for( i=0; i<nCount; i++)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(i);

		FtrLayerGroup *pGroup = (FtrLayerGroup*)pSel->GetData();
		if (!pGroup) continue;

		int nLayerNum = pGroup->ftrLayers.GetSize();
		for (int i=0; i<nLayerNum; i++)
		{
			CFtrLayer *pLayer = (CFtrLayer*)pGroup->ftrLayers[i];
			if (pLayer && !pLayer->IsDeleted()) break;
		}

		if (i < nLayerNum)
		{
			CString str,format;
			format.LoadString(IDS_TIP_LAYERGROUPDELETE);
			str.Format(format,pGroup->Name);
			AfxMessageBox(str);
			return;
		}

		pDS->DelFtrLayerGroup(pGroup);

		/*for (int i=0; i<pGroup->ftrLayers.GetSize(); i++)
		{
			CFtrLayer *pLayer = (CFtrLayer*)pGroup->ftrLayers[i];
			if (!pLayer || pLayer->IsDeleted()) continue;
			
			//pLayer->SetGroupName("");
			_variant_t data = (_variant_t)(_bstr_t)pLayer->GetGroupName();
			undo.arrLayers.Add(pLayer);
			undo.arrOldVars.Add(data);
			m_pDlgDoc->ModifyLayer(pLayer,FIELDNAME_LAYGROUPNAME,(_variant_t)(_bstr_t)(""),TRUE);
		}*/

		m_wndPropListCtrl.RemovePropItem(pSel);
	}

// 	undo.newVar = (_variant_t)(_bstr_t)("");
// 	undo.Commit();

	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_GROUPS )
	{
		m_wndPropListCtrl.FilterPropItems(TRUE);
	}
}

void CLayersMgrDlg::OnUpdateSelectAllLayers(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_ALLLEVELS||dw==LM_TREEID_ALLLEVELS_REF
		||dw==LM_TREEID_FILTERITEM||dw==LM_TREEID_FILTERITEM_REF
		||dw==LM_TREEID_ALLFEATURES||dw==LM_TREEID_ALLFEATURES_REF
		||dw==LM_TREEID_GROUPS||dw==LM_TREEID_GROUPS_REF)
	{		
		flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnUpdateSelectAllLayersExceptCurSels(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_ALLLEVELS||dw==LM_TREEID_ALLLEVELS_REF
		||dw==LM_TREEID_FILTERITEM||dw==LM_TREEID_FILTERITEM_REF
		||dw==LM_TREEID_ALLFEATURES||dw==LM_TREEID_ALLFEATURES_REF
		||dw==LM_TREEID_GROUPS||dw==LM_TREEID_GROUPS_REF)
	{		
		flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}


void CLayersMgrDlg::OnUpdateDeSelectLayers(CCmdUI *pCmdUI)
{
	BOOL bValid = (m_wndPropListCtrl.GetSelectedCount() > 0);
	pCmdUI->Enable(bValid);
	
}

void CLayersMgrDlg::OnUpdateDelLayerObj(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_ALLLEVELS
		||dw==LM_TREEID_FILTERITEM)
	{
		if(m_wndPropListCtrl.GetSelectedCount()>0)
		flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnUpdateActiveLayer(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_ALLLEVELS
		||dw==LM_TREEID_FILTERITEM)
	{
		if(m_wndPropListCtrl.GetSelectedCount()>0)
			flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnUpdateDeleteFilter(CCmdUI *pCmdUI)
{
	DWORD dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_FILTERITEM||dw==LM_TREEID_FILTERITEM_REF)
	{		
		flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnUpdateRenameFilter(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_FILTERITEM||dw==LM_TREEID_FILTERITEM_REF)
	{		
		flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnUpdateNewLayer(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_ALLLEVELS
		||dw==LM_TREEID_FILTERITEM)
	{		
		flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnUpdateDelLayer(CCmdUI *pCmdUI)
{
	DWORD_PTR dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		goto end;
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_ALLLEVELS
		||dw==LM_TREEID_FILTERITEM)
	{
		if(m_wndPropListCtrl.GetSelectedCount()>0)
			flag = TRUE;
	}	
end:
	pCmdUI->Enable(flag);
}

void CLayersMgrDlg::OnDelLayer()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 )return;

	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}

	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CUndoNewDelLayer undo(m_pDlgDoc,"DelLayer");

	CString str;
	str.LoadString(IDS_LAYERMANGER_STR2);
	if(AfxMessageBox(str,MB_OKCANCEL)==IDCANCEL)
		return;		
		
	if (nCount==1)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(0);
		
// 		CString strLayerName = (LPCTSTR)(_bstr_t)pSel->GetValue(FIELDNAME_LAYERNAME);
// 		CFtrLayer *pLayer = pDS->GetFtrLayer(strLayerName);
		CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
		if( !pLayer||pLayer->IsInherent() )
		{
			str.LoadString(IDS_LAYERMANGER_STR0);
			AfxMessageBox(str);
			return;
		}
		if( pLayer->GetEditableObjsCount()>0 )
		{
			str.LoadString(IDS_LAYERMANGER_STR1);
			AfxMessageBox(str);
			return;
		}	
	}		
	for( i=0; i<nCount; i++)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(i);

// 		CString strLayerName = (LPCTSTR)(_bstr_t)pSel->GetValue(FIELDNAME_LAYERNAME);
// 		CFtrLayer *pLayer = pDS->GetFtrLayer(strLayerName);
		CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
		if( !pLayer||pLayer->IsInherent() )continue;

		if( pLayer->GetEditableObjsCount()>0 )continue;
		
		m_pDlgDoc->DelFtrLayer(pLayer);

		undo.isDel = TRUE;
		undo.arrObjs.Add(pLayer);

		m_wndPropListCtrl.RemovePropItem(pSel);
	}
	undo.Commit();
	if(nCount>0)
	{
		int nsz = undo.arrObjs.GetSize();
		int num = (nsz>5) ?5 : nsz;
		str.LoadString(IDS_LAYERMANGER_STR3);
		for (int j=0;j<num;j++)
		{
			str+=undo.arrObjs.GetAt(j)->GetName();
			if(j!=num-1)
				str+="; ";
			else
				str+=". \n";
		}
		if (nsz>5)
		{
			str+="...\n";			
		}
		if(nCount-nsz>0)
		{
			CString str0;
			str0.Format(StrFromResID(IDS_LAYERMANGER_STR4),nCount-nsz);
			str+=str0;
		}		
		AfxMessageBox(str);
	}
	if (!m_wndPropListCtrl.GetCurLayerPropItem())
	{
		CFtrLayer *pLayer2 = m_pDlgDoc->GetDlgDataSource()->GetCurFtrLayer();
		for(int i = 0;i<m_wndPropListCtrl.GetItemCount();i++)
		{
			CLVLPropItem *pPropItem = m_wndPropListCtrl.GetPropItem(i);
			if (pPropItem&&pPropItem->GetData() == (DWORD_PTR)pLayer2)
			{
				m_wndPropListCtrl.SetCurLayerPropItem(pPropItem);
			}
		}
	}
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS )
	{
//		m_wndPropListCtrl.SetFilterItem(NULL);
		m_wndPropListCtrl.FilterPropItems(TRUE);
	}
	else if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM)
	{
		m_wndPropListCtrl.FilterPropItems(TRUE);	
	}
}


void CLayersMgrDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{	
    ASSERT(pPopupMenu != NULL); 
    // Check the enabled state of various menu items. 
    CCmdUI state; 
    state.m_pMenu = pPopupMenu; 
    ASSERT(state.m_pOther == NULL); 
    ASSERT(state.m_pParentMenu == NULL); 
    // Determine if menu is popup in top-level menu and set m_pOther to 
    // it if so (m_pParentMenu == NULL indicates that it is secondary popup). 

	HMENU hParentMenu; 
    if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu) 
        state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup. 
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL) 
    { 
        CWnd* pParent = this; 
		// Child windows don't have menus--need to go to the top! 
        if (pParent != NULL && 
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL) 
        { 
			int nIndexMax = ::GetMenuItemCount(hParentMenu); 
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++) 
			{ 
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu) 
				{ 
					// When popup is found, m_pParentMenu is containing menu. 
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu); 
					break; 
				} 
			} 
        } 
    } 
    state.m_nIndexMax = pPopupMenu->GetMenuItemCount(); 
    for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax; 
	state.m_nIndex++) 
    { 
        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex); 
        if (state.m_nID == 0) 
			continue; // Menu separator or invalid cmd - ignore it. 
        ASSERT(state.m_pOther == NULL); 
        ASSERT(state.m_pMenu != NULL); 
        if (state.m_nID == (UINT)-1) 
        { 
			// Possibly a popup menu, route to first item of that popup. 
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex); 
			if (state.m_pSubMenu == NULL || 
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 || 
				state.m_nID == (UINT)-1) 
			{ 
				continue;       // First item of popup can't be routed to. 
			} 
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled. 
        } 
        else 
        { 
			// Normal menu item. 
			// Auto enable/disable if frame window has m_bAutoMenuEnable 
			// set and command is _not_ a system command. 
			state.m_pSubMenu = NULL; 
			state.DoUpdate(this, FALSE); 
        } 
        // Adjust for menu deletions and additions. 
        UINT nCount = pPopupMenu->GetMenuItemCount(); 
        if (nCount < state.m_nIndexMax) 
        { 
			state.m_nIndex -= (state.m_nIndexMax - nCount); 
			while (state.m_nIndex < nCount && 
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID) 
			{ 
				state.m_nIndex++; 
			} 
        } 
        state.m_nIndexMax = nCount; 
    } 
}

void CLayersMgrDlg::OnDelFtrs()
{
	if (m_dCurSelItem != LM_TREEID_ALLFEATURES || m_dCurSelItem == LM_TREEID_ALLFEATURES_REF) return;

	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 )return;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}	

	if (nCount == m_wndPropListCtrl.GetItemCount())
	{
		if (AfxMessageBox(StrFromResID(IDS_TIP_DELALL),MB_OKCANCEL)==IDCANCEL)
		{
			return;
		}
	}	
	else if(AfxMessageBox(StrFromResID(IDS_STRING_DELFTRS),MB_OKCANCEL)==IDCANCEL)
	{
		return;
	}
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CUndoFtrs undo(m_pDlgDoc,"DelFtrs");	
	CPFeature pFeature = NULL;
	for( i=0; i<nCount; i++)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(i);
		
		FeatureItem *pItem = (FeatureItem*)pSel->GetData();
		if( !pItem )continue;
		
		if( pItem->ftrs.GetSize()>0 )
		{
			for (int j=0;j<pItem->ftrs.GetSize();j++)
			{
				pFeature = pItem->ftrs.GetAt(j);
				if( !pFeature )continue;
				
				m_pDlgDoc->DeleteObject(FtrToHandle(pFeature));
				undo.AddOldFeature(FtrToHandle(pFeature));
				m_pDlgDoc->UpdateAllViews(NULL,hc_DelObject, (CObject*)(pFeature));				
			}	
		}		
	}
	undo.Commit();
	
	
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLFEATURES )
	{
		FillAllFeatures(_T("All Features"),pDS);
	}
}

void CLayersMgrDlg::OnDelLayerObj()
{
//	if (m_dCurSelItem != LM_TREEID_ALLLEVELS || m_dCurSelItem == LM_TREEID_ALLLEVELS_REF) return;
	if (m_dCurSelItem == LM_TREEID_ALLLEVELS_REF) return;

	if (LM_TREEID_ALLLEVELS  == m_dCurSelItem||m_dCurSelItem == LM_TREEID_FILTERITEM)
	{

	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 )return;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}	

//  	if(AfxMessageBox(StrFromResID(IDS_STRING_DELALLOBJ),MB_OKCANCEL)==IDCANCEL)
//  		return;
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CUndoFtrs undo(m_pDlgDoc,"DelLayerObjs");	
	CPFeature pFeature = NULL;
	//cjc 2012-10-31 修改多个选择地物项删除
	if (1 == nCount)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(0);
		
		CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
		if( !pLayer )return;
		if( pLayer->GetObjectCount()>0 )
		{
			if(AfxMessageBox(StrFromResID(IDS_STRING_DELALLOBJ),MB_OKCANCEL)==IDCANCEL)
				return;
			for (int j=0;j<pLayer->GetObjectCount();j++)
			{
				pFeature = pLayer->GetObject(j);
				if( !pFeature )continue;
				
				//pLayer->DeleteObject(pFeature);
				m_pDlgDoc->DeleteObject(FtrToHandle(pFeature));
				undo.AddOldFeature(FtrToHandle(pFeature));
				m_pDlgDoc->UpdateAllViews(NULL,hc_DelObject, (CObject*)(pFeature));				
			}	
		}
		else
			return;		
	}
	else if (nCount>1)
	{
		if(AfxMessageBox(StrFromResID(IDS_STRING_DELALLOBJ),MB_OKCANCEL)==IDCANCEL)
			return;
		for( i=0; i<nCount; i++)
		{
			CLVLPropItem* pSel = arrPSels.GetAt(i);
			
			// 		CString strLayerName = (LPCTSTR)(_bstr_t)pSel->GetValue(FIELDNAME_LAYERNAME);
			// 		CString strMapName = (LPCTSTR)(_bstr_t)pSel->GetValue(FIELDNAME_LAYMAPNAME);
			// 		CFtrLayer *pLayer = pDS->GetFtrLayer(strLayerName,strMapName);
			CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
			if( !pLayer )continue;
			if( pLayer->GetObjectCount()>0 )
			{
				
				for (int j=0;j<pLayer->GetObjectCount();j++)
				{
					pFeature = pLayer->GetObject(j);
					if( !pFeature )continue;
					
					//pLayer->DeleteObject(pFeature);
					m_pDlgDoc->DeleteObject(FtrToHandle(pFeature));
					undo.AddOldFeature(FtrToHandle(pFeature));
					m_pDlgDoc->UpdateAllViews(NULL,hc_DelObject, (CObject*)(pFeature));				
				}	
			}
			else
				continue;
		}	
	}
	

	undo.Commit();	

	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS )
	{
		//FillAllLayersList(pDS,FALSE);
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),pDS,TRUE);
	}
	else if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM) 
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	//	FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem));
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),pDS,FALSE);
	}

	}

	

}

void CLayersMgrDlg::OnActiveLayer()
{
	if (m_wndPropListCtrl.GetSelectedCount()<=0)
	{
		return;
	}
	CLVLPropItem* pPropItem = m_wndPropListCtrl.GetSelectedItem(0);
	if (pPropItem)
	{
		CFtrLayer * pLayer = (CFtrLayer*)pPropItem->GetData();
		if (!pLayer)
		{
			return;
		}
		m_pDlgDoc->GetDlgDataSource()->SetCurFtrLayer(pLayer->GetID());
		m_wndPropListCtrl.SetCurLayerPropItem(pPropItem);
		m_wndPropListCtrl.RedrawWindow();
	}
}
//选择所有层/层组
void CLayersMgrDlg::OnSelectAllLayers()
{
	m_wndPropListCtrl.SelectAll();
}
//删除所有层/层组
void CLayersMgrDlg::OnDeSelectLayers()
{
	m_wndPropListCtrl.DeselectAll();
}

void CLayersMgrDlg::OnSelectAllLayersExceptCurSels()
{
	CArray<CLVLPropItem*,CLVLPropItem*> arrCurSelItems;
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	for (int i=0;i<nCount;i++)
	{
		CLVLPropItem *pPropItem = m_wndPropListCtrl.GetSelectedItem(i);
		arrCurSelItems.Add(pPropItem);
	}

	m_wndPropListCtrl.SelectAll();

	for (i=0;i<nCount;i++)
	{
		m_wndPropListCtrl.DeselectItem(arrCurSelItems[i]);
	}
}

void CLayersMgrDlg::OnCutSelectLayers()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	for (int i=0;i<nCount;i++)
	{
		CLVLPropItem *pPropItem = m_wndPropListCtrl.GetSelectedItem(i);
		m_arrCutLayers.Add((CFtrLayer*)pPropItem->GetData());
	}
	m_bIsCuttingObj = TRUE;	
}
void CLayersMgrDlg::OnUpdateCutSelectLayers(CCmdUI *pCmdUI)
{
	DWORD_PTR  dw;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	BOOL flag = FALSE;
	if(hItem==NULL)
		goto end;

	dw = m_wndTreeCtrl.GetItemData(hItem);	
	if(dw == LM_TREEID_FILTERITEM || dw == LM_TREEID_ALLLEVELS)		
	{
		if (m_wndPropListCtrl.GetSelectedCount()>0)
		{
			flag = TRUE;
		}
	}
end:
	pCmdUI->Enable(flag);
}

//粘贴层
void CLayersMgrDlg::OnPasteLayer()
{
	if (!m_bIsCuttingObj||m_wndPropListCtrl.GetSelectedCount()!=1)return;
	CFtrLayer *pLayer = (CFtrLayer *)m_wndPropListCtrl.GetSelectedItem(0)->GetData();
	long lSum = 0;	
	CArray<CFeature*,CFeature*> arr;
	CArray<int,int> arrLayID;
	for (int i=0;i<m_arrCutLayers.GetSize();i++)
	{
		CFtrLayer *pLayer = m_arrCutLayers[i];
		if(!pLayer)continue;
		int nsz = pLayer->GetObjectCount();
		for (int j=0;j<nsz;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr)continue;
			arrLayID.Add(pLayer->GetID());
			arr.Add(pFtr);
			lSum++;
		}	
	}
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);
	CUndoModifyLayerOfObj undo(m_pDlgDoc,_T("Change_Layer"));
	CFeature *pFtr;
	for(i=0;i<arr.GetSize();i++)
	{
		AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
		pFtr = arr.GetAt(i);
		if( !pFtr )continue;
		m_pDlgDoc->DeleteObject(FtrToHandle(pFtr));
		m_pDlgDoc->GetDlgDataSource()->SetFtrLayerOfObject(pFtr,pLayer->GetID());
		m_pDlgDoc->RestoreObject(FtrToHandle(pFtr));
		undo.ModifyLayerOfObj(FtrToHandle(pFtr),arrLayID.GetAt(i),pLayer->GetID());	
	}
	undo.Commit();	

	m_pDlgDoc->RefreshView();
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
	m_arrCutLayers.RemoveAll();
	m_bIsCuttingObj = FALSE;
	m_pDlgDoc->RefreshView();
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return;
	DWORD_PTR  dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_FILTERITEM)
	{			
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),m_pDlgDoc->GetDlgDataSource(),FALSE);
	}
	else if (dw==LM_TREEID_ALLLEVELS)
	{
		//FillAllLayersList(m_pDlgDoc->GetDlgDataSource(),FALSE);
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),m_pDlgDoc->GetDlgDataSource(),TRUE);
	}
}
void CLayersMgrDlg::OnUpdatePasteLayer(CCmdUI *pCmdUI)
{
	DWORD_PTR  dw;
	BOOL flag = FALSE;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
	{
		goto end;
	}
	
	dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw == LM_TREEID_FILTERITEM || dw == LM_TREEID_ALLLEVELS)		
	{
		if (m_wndPropListCtrl.GetSelectedCount()==1)
		{
			flag = TRUE;
		}
	}
	
end:
	pCmdUI->Enable(m_bIsCuttingObj&&flag);	
}
void CLayersMgrDlg::OnPasteLayerEx()
{
	if (!m_bIsCuttingObj||m_wndPropListCtrl.GetSelectedCount()!=1)return;
	CFtrLayer *pLayer = (CFtrLayer *)m_wndPropListCtrl.GetSelectedItem(0)->GetData();
	long lSum = 0;	
	CArray<CFeature*,CFeature*> arr;
	CArray<int,int> arrLayID;
	for (int i=0;i<m_arrCutLayers.GetSize();i++)
	{
		CFtrLayer *pLayer = m_arrCutLayers[i];
		if(!pLayer)continue;
		int nsz = pLayer->GetObjectCount();
		for (int j=0;j<nsz;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(!pFtr)continue;
			arrLayID.Add(pLayer->GetID());
			arr.Add(pFtr);
			lSum++;
		}	
	}
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, lSum);
	CArray<PT_3DEX,PT_3DEX> pts;
	CFeature *pFtr0 = NULL;
	CFeature *pTempl = pLayer->CreateDefaultFeature(m_pDlgDoc->GetDlgDataSource()->GetScale());
	if (!pTempl)
		return;
	CUndoFtrs undo(m_pDlgDoc,_T("Change_Layer"));	
	int nCls = pTempl->GetGeometry()->GetClassType();
	if (nCls==CLS_GEOPOINT||nCls==CLS_GEOMULTIPOINT||nCls==CLS_GEODIRPOINT||nCls==CLS_GEODEMPOINT||nCls==CLS_GEOSURFACEPOINT)
	{
		for(i=0;i<arr.GetSize();i++)
		{
			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
			CFeature *pFtr = arr.GetAt(i);
			if( !pFtr )continue;
			if(pFtr->GetGeometry()->GetClassType()!=nCls)
			{
				continue;
			}
			else
			{
				pFtr0 = pTempl->Clone();
				if(!pFtr0)
					continue;
				pFtr->GetGeometry()->GetShape(pts);
				pFtr0->SetID(OUID());
				if(pFtr0->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
				{
					m_pDlgDoc->AddObject(pFtr0,pLayer->GetID());
					undo.AddNewFeature(FtrToHandle(pFtr0));
				}
				CAttributesSource *pXDS = m_pDlgDoc->GetDlgDataSource()->GetXAttributesSource();
				if(pXDS)pXDS->CopyXAttributes(pFtr,pFtr0);
				m_pDlgDoc->DeleteObject(FtrToHandle(pFtr));	
				undo.AddOldFeature(FtrToHandle(pFtr));
			}				
		}
	}
	else if (nCls==CLS_GEOCURVE||nCls==CLS_GEOPARALLEL||nCls==CLS_GEOSURFACE||nCls==CLS_GEOMULTISURFACE)
	{
		for(i=0;i<arr.GetSize();i++)
		{
			AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2, 1);
			CFeature *pFtr = arr.GetAt(i);
			if( !pFtr )continue;
			int cls = pFtr->GetGeometry()->GetClassType();
			if(cls!=CLS_GEOCURVE&&cls==CLS_GEOPARALLEL&&cls==CLS_GEOSURFACE&&cls==CLS_GEOMULTISURFACE)
			{
				continue;
			}
			else
			{
				pFtr0 = pTempl->Clone();
				if(!pFtr0)
					continue;
				pFtr->GetGeometry()->GetShape(pts);
				pFtr0->SetID(OUID());
				if(pFtr0->GetGeometry()->CreateShape(pts.GetData(),pts.GetSize()))
				{
					m_pDlgDoc->AddObject(pFtr0,pLayer->GetID());
					undo.AddNewFeature(FtrToHandle(pFtr0));
				}
				CAttributesSource *pXDS = m_pDlgDoc->GetDlgDataSource()->GetXAttributesSource();
				if(pXDS)pXDS->CopyXAttributes(pFtr,pFtr0);				
				m_pDlgDoc->DeleteObject(FtrToHandle(pFtr));					
				undo.AddOldFeature(FtrToHandle(pFtr));
			}
		}
	}
	else
	{
		delete pTempl;
		pTempl = NULL;
		return;
	}
	undo.Commit();
	delete pTempl;
	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1, -1);
	m_arrCutLayers.RemoveAll();
	m_bIsCuttingObj = FALSE;
	m_pDlgDoc->RefreshView();
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return;
	DWORD_PTR  dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw==LM_TREEID_FILTERITEM)
	{			
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),m_pDlgDoc->GetDlgDataSource(),FALSE);
	}
	else if (dw==LM_TREEID_ALLLEVELS)
	{
		//FillAllLayersList(m_pDlgDoc->GetDlgDataSource(),FALSE);
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),m_pDlgDoc->GetDlgDataSource(),TRUE);
	}
}
void CLayersMgrDlg::OnUpdatePasteLayerEx(CCmdUI *pCmdUI)
{
	if (m_wndPropListCtrl.GetSelectedCount()==1)
	{
		pCmdUI->Enable(m_bIsCuttingObj);
	}
	else
		pCmdUI->Enable(FALSE);
}

void CLayersMgrDlg::OnResetDisplayOrder()
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	pDS->ResetDisplayOrder();
	pDS->SaveAllLayers();
	UpdateOrderValue();
 	int col = GetCol(FIELDNAME_LAYERDISPLAYORDER);
	m_wndPropListCtrl.ReSortItems(col);
	m_pDlgDoc->UpdateAllViews(NULL,hc_UpdateLayerDisplayOrder);
}
void CLayersMgrDlg::OnResetDefalutLayerColor()
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return;
	for (int i=pScheme->GetLayerDefineCount()-1;i>=0;i--)
	{
		CPtrArray arrPtr;
		CSchemeLayerDefine* pdef = pScheme->GetLayerDefine(i);
		if(pDS->GetFtrLayer(pdef->GetLayerName(),NULL,&arrPtr))
		{
			for (int j=arrPtr.GetSize()-1;j>=0;j--)
			{
				CFtrLayer*pLayer = (CFtrLayer*)arrPtr.GetAt(j);
				if (pLayer->GetColor()!=pdef->GetColor())
				{
					pLayer->SetColor(pdef->GetColor());
					m_pDlgDoc->UpdateFtrLayer(pLayer);
				}
			}
		}
	}
	UpdateColorValue();
	m_pDlgDoc->UpdateAllViews(NULL,hc_UpdateAllObjects);
}
void CLayersMgrDlg::OnExit()
{
	OnCancel();
}

void CLayersMgrDlg::SaveLayers()
{
	int nCount = m_wndPropListCtrl.GetItemCount();

	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	for( int i=0; i<nCount; i++)
	{
		CLVLPropItem *pItem = m_wndPropListCtrl.GetPropItem(i);

		int nLayer = pDS->GetFtrLayerCount();
		for( int k=0; k<nLayer; k++)
		{
			if ((DWORD_PTR)pDS->GetFtrLayerByIndex(k) == pItem->GetData())
				break;
		}
		if( k>=nLayer )continue;
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(k);

		CValueTable tab;
		tab.BeginAddValueItem();

		int nfield = m_wndPropListCtrl.GetColumnCount();
		for( int j=0; j<nfield; j++)
		{
			CLVLPropColumn col = m_wndPropListCtrl.GetColumn(j);
			CVariantEx value = pItem->GetValue(col.FieldName);
			tab.AddValue(col.FieldName,&value);
		}

		tab.EndAddValueItem();

		pLayer->ReadFrom(tab);

		m_pDlgDoc->UpdateFtrLayer(pLayer);
	}	
}


void CLayersMgrDlg::SaveFilter(LPCTSTR name)
{
	CString strName;
	if( name )strName = name;
	else
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		if( hItem==NULL || (m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM&&m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM_REF&&
								m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_ALLLEVELS&&m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_ALLLEVELS_REF&&
								m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_ALLFEATURES&&m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_ALLFEATURES_REF) )
			return;

		strName = m_wndTreeCtrl.GetItemText(hItem);
	}

	FilterItem *pFilter = FindFilterItem(strName);
	if( !pFilter )return;

	CLVLPropItem *pItem = m_wndPropListCtrl.GetFilterItem();
	if( !pItem )return;

	pFilter->tab.DelAll();
	pFilter->tab.BeginAddValueItem();

	int nfield = m_wndPropListCtrl.GetColumnCount();
	for( int j=0; j<nfield; j++)
	{
		CLVLPropColumn col = m_wndPropListCtrl.GetColumn(j);
		CVariantEx value = pItem->GetValue(col.FieldName);
		pFilter->tab.AddValue(col.FieldName,&value);
	}
	
	pFilter->tab.EndAddValueItem();
}


void CLayersMgrDlg::UpdateStatusString()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem==NULL )
		return;

	while (1)
	{
		if (m_wndTreeCtrl.GetParentItem(hItem)!=NULL)
		{
			hItem = m_wndTreeCtrl.GetParentItem(hItem);
		}
		else
			break;			 
	}

	CDlgDataSource *pDS = (CDlgDataSource *)m_wndTreeCtrl.GetItemData(hItem);
	int nLayerCount = pDS->GetFtrLayerCount();
	int nSel = m_wndPropListCtrl.GetSelectedCount();

	CString strMsg;
	strMsg.Format(IDS_LAYERMAN_STATUS,nLayerCount,nSel);
	m_wndStatus.SetWindowText(strMsg);
}

void CLayersMgrDlg::FillTree()
{
	m_wndTreeCtrl.DeleteAllItems();

	CString pathname /*= m_pDlgDoc->GetPathName()*/;
	
	int nCount = m_pDlgDoc->GetDlgDataSourceCount();
	if (nCount<=0)
	{
		return;
	}
	CDlgDataSource *pDS = NULL;
	for (int i=0;i<nCount;i++)
	{
		pDS = m_pDlgDoc->GetDlgDataSource(i);
		pathname = pDS->GetName();

		int iActiveDS = m_pDlgDoc->GetActiveDataSourceIdx();

		CString name = ExtractFileNameExt(pathname);
		
		if( name.IsEmpty() )
			name = "Default File";
		if(i == iActiveDS)
		{
			name += _T("(");
			name += StrFromResID(IDS_MAINDS);
			name += _T(")");
			HTREEITEM hRoot = m_wndTreeCtrl.InsertRoot(name,LM_TREEID_FILE,(DWORD_PTR)pDS);
			m_wndTreeCtrl.SetItemImage(hRoot,0,0);
			HTREEITEM hLevel = m_wndTreeCtrl.InsertSubItem("All Used Levels",hRoot,LM_TREEID_ALLLEVELS,LM_TREEID_ALLLEVELS);
			m_wndTreeCtrl.SetItemImage(hLevel,2,2);
// 			HTREEITEM hFeatures = m_wndTreeCtrl.InsertSubItem("All Features",hRoot,LM_TREEID_ALLFEATURES,LM_TREEID_ALLFEATURES);
// 			m_wndTreeCtrl.SetItemImage(hFeatures,2,2);
			HTREEITEM hGroup = m_wndTreeCtrl.InsertSubItem("All Groups",hRoot,LM_TREEID_GROUPS,LM_TREEID_GROUPS);
			m_wndTreeCtrl.SetItemImage(hGroup,2,2);
			HTREEITEM hFilter = m_wndTreeCtrl.InsertSubItem("Filters",hRoot,LM_TREEID_FILTERS,LM_TREEID_FILTERS);
			m_wndTreeCtrl.SetItemImage(hFilter,3,3);
			
			int nsz = m_arrFilters.GetSize();
			for( int j=0; j<nsz; j++)
			{
				FilterItem* pItem = m_arrFilters.GetAt(j);
				if (pItem->name.CompareNoCase(_T("All Used Levels")) == 0 || 
					pItem->name.CompareNoCase(_T("All Features")) == 0 ) 
					continue;
				m_wndTreeCtrl.InsertSubItem(pItem->name,hFilter,LM_TREEID_FILTERITEM,LM_TREEID_FILTERITEM);
			}
			m_wndTreeCtrl.Expand(hFilter,TVE_EXPAND);			
			m_wndTreeCtrl.SelectItem(hLevel);
		}
		else
		{
			HTREEITEM hRoot = m_wndTreeCtrl.InsertRoot(name,LM_TREEID_FILE_REF,(DWORD_PTR)pDS);
			m_wndTreeCtrl.SetItemImage(hRoot,0,0);
			HTREEITEM hLevel = m_wndTreeCtrl.InsertSubItem("All Used Levels",hRoot,LM_TREEID_ALLLEVELS_REF,LM_TREEID_ALLLEVELS_REF);
			m_wndTreeCtrl.SetItemImage(hLevel,2,2);
// 			HTREEITEM hFeatures = m_wndTreeCtrl.InsertSubItem("All Features",hRoot,LM_TREEID_ALLFEATURES_REF,LM_TREEID_ALLFEATURES_REF);
// 			m_wndTreeCtrl.SetItemImage(hFeatures,2,2);
			HTREEITEM hGroup = m_wndTreeCtrl.InsertSubItem("All Groups",hRoot,LM_TREEID_GROUPS_REF,LM_TREEID_GROUPS_REF);
			m_wndTreeCtrl.SetItemImage(hGroup,2,2);
			HTREEITEM hFilter = m_wndTreeCtrl.InsertSubItem("Filters",hRoot,LM_TREEID_FILTERS_REF,LM_TREEID_FILTERS_REF);
			m_wndTreeCtrl.SetItemImage(hFilter,3,3);

			int nsz = m_arrFilters.GetSize();
			for( int j=0; j<nsz; j++)
			{
				FilterItem* pItem = m_arrFilters.GetAt(j);
				if (pItem->name.CompareNoCase(_T("All Used Levels")) == 0 ||
					pItem->name.CompareNoCase(_T("All Features")) == 0 ) 
					continue;
				m_wndTreeCtrl.InsertSubItem(pItem->name,hFilter,LM_TREEID_FILTERITEM_REF,LM_TREEID_FILTERITEM_REF);
			}
		}				
	}	
}

void CLayersMgrDlg::LoadFilterFile(const char *szFileName)
{
	RemoveFilter(NULL);
	
	CMarkup xmlfile;
	if( !xmlfile.Load(szFileName) )
	{
		FilterItem *pFilter = FindFilterItem(_T("All Used Levels"));
		if( !pFilter )
		{
			FilterItem *pFilter = new FilterItem;
			if( !pFilter ) return;
			
			pFilter->name = _T("All Used Levels");
			
			pFilter->tab.DelAll();
			pFilter->tab.BeginAddValueItem();
			
			pFilter->tab.AddValue(FIELDNAME_LAYVISIBLE,&CVariantEx((bool)1));
			pFilter->tab.AddValue(FIELDNAME_LAYUSED,&CVariantEx((bool)1));			
			pFilter->tab.AddValue(FIELDNAME_LAYERSYMBOLIZED,&CVariantEx((bool)1));
			pFilter->tab.AddValue(FIELDNAME_LAYINHERENT,&CVariantEx((bool)1));
			pFilter->tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx((long)-2));

			pFilter->tab.EndAddValueItem();
			
			m_arrFilters.Add(pFilter);
		}
		
		pFilter = FindFilterItem(_T("All Features"));
		if( !pFilter )
		{
			FilterItem *pFilter = new FilterItem;
			if( !pFilter ) return;
			
			pFilter->name = _T("All Features");
			
			pFilter->tab.DelAll();
			pFilter->tab.BeginAddValueItem();
			
			pFilter->tab.AddValue(FIELDNAME_FTRVISIBLE,&CVariantEx((bool)1));
			pFilter->tab.AddValue(FIELDNAME_LAYUSED,&CVariantEx((bool)1));
			pFilter->tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx((long)-2));
			
			pFilter->tab.EndAddValueItem();
			
			m_arrFilters.Add(pFilter);
		}

		return;
	}

	if( !xmlfile.IsWellFormed() )
		return;

	if( !xmlfile.FindElem(XMLTAG_FILTER_FILE) )
		return;

	xmlfile.IntoElem();

	while( xmlfile.FindElem(XMLTAG_FILTER_ITEM) )
	{
		FilterItem *pFilter = new FilterItem;
		if( !pFilter )continue;

		xmlfile.IntoElem();

		if( xmlfile.FindElem(XMLTAG_FILTER_NAME) )
		{
			pFilter->name = xmlfile.GetData();
		}
		if( xmlfile.FindElem(XMLTAG_FILTER_DATA) )
		{
			xmlfile.IntoElem();

			pFilter->tab.BeginAddValueItem();
			Xml_ReadValueTable(xmlfile,pFilter->tab);
			pFilter->tab.EndAddValueItem();

			xmlfile.OutOfElem();
		}
		xmlfile.OutOfElem();

		m_arrFilters.Add(pFilter);
	}

	xmlfile.OutOfElem();

	FilterItem *pFilter = FindFilterItem(_T("All Used Levels"));
	if( !pFilter )
	{
		FilterItem *pFilter = new FilterItem;
		if( !pFilter ) return;

		pFilter->name = _T("All Used Levels");

		pFilter->tab.DelAll();
		pFilter->tab.BeginAddValueItem();
		
		pFilter->tab.AddValue(FIELDNAME_LAYVISIBLE,&CVariantEx((bool)1));
		pFilter->tab.AddValue(FIELDNAME_LAYUSED,&CVariantEx((bool)1));
		pFilter->tab.AddValue(FIELDNAME_LAYERSYMBOLIZED,&CVariantEx((bool)1));
		pFilter->tab.AddValue(FIELDNAME_LAYINHERENT,&CVariantEx((bool)1));
		pFilter->tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx((long)-2));
		
		pFilter->tab.EndAddValueItem();

		m_arrFilters.Add(pFilter);
	}

	pFilter = FindFilterItem(_T("All Features"));
	if( !pFilter )
	{
		FilterItem *pFilter = new FilterItem;
		if( !pFilter ) return;
		
		pFilter->name = _T("All Features");

		pFilter->tab.DelAll();
		pFilter->tab.BeginAddValueItem();
		
		pFilter->tab.AddValue(FIELDNAME_FTRVISIBLE,&CVariantEx((bool)1));
		pFilter->tab.AddValue(FIELDNAME_LAYUSED,&CVariantEx((bool)1));
		pFilter->tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx((long)-2));
		pFilter->tab.EndAddValueItem();
		
		m_arrFilters.Add(pFilter);
	}
}

void CLayersMgrDlg::SaveFilterFile(const char *szFileName)
{
	CMarkup xmlfile;

	xmlfile.AddElem(XMLTAG_FILTER_FILE);
	xmlfile.IntoElem();

	int nsz = m_arrFilters.GetSize();
	for( int i=0; i<nsz; i++)
	{
		FilterItem *pFilter = m_arrFilters.GetAt(i);

		xmlfile.AddElem(XMLTAG_FILTER_ITEM);
		xmlfile.IntoElem();

		xmlfile.AddElem(XMLTAG_FILTER_NAME,pFilter->name);
		xmlfile.AddElem(XMLTAG_FILTER_DATA);
		xmlfile.IntoElem();
		Xml_WriteValueTable(xmlfile,pFilter->tab);
		xmlfile.OutOfElem();

		xmlfile.OutOfElem();
	}

	xmlfile.OutOfElem();
	
	CString strDoc = xmlfile.GetDoc();
	CMarkup::WriteTextFile(szFileName,strDoc);
}

void CLayersMgrDlg::RemoveFilter(LPCTSTR name)
{
	int nsz = m_arrFilters.GetSize();
	for( int i=nsz-1; i>=0; i--)
	{
		FilterItem *pItem = m_arrFilters.GetAt(i);
		if( name==NULL || pItem->name.CompareNoCase(name)==0 )
		{
			delete pItem;
			m_arrFilters.RemoveAt(i);
		}
	}
}

//用全部层填充列表,参数为空指针时，默认为主数据源
void CLayersMgrDlg::FillAllLayersList(CDlgDataSource *pDS,BOOL bForLoad )
{
	CValueTable tab;
	CFtrLayer *pLayer = new CFtrLayer;
	if( !pLayer )return;

	tab.BeginAddValueItem();
	pLayer->WriteTo(tab);
	tab.EndAddValueItem();
	tab.DelField(FIELDNAME_CLSTYPE);
	tab.DelField(FIELDNAME_LAYERID);
	tab.DelField(FIELDNAME_LAYVISIBLE);
	tab.DelField(FIELDNAME_LAYERDISPLAYORDER);

	delete pLayer;

	BOOL bIsRefDS = FALSE;
	if (pDS&&pDS!=m_pDlgDoc->GetDlgDataSource())
	{
		bIsRefDS = TRUE;
	}
	m_wndPropListCtrl.RemoveAll();//清空列表
	CLVLPropColumn col;

	int nfield = tab.GetFieldCount();
	CString field,name;
	int type;
	
	for( int i=0; i<nfield; i++)
	{
		if(bIsRefDS)
			col.bReadOnly = TRUE;
		else
			col.bReadOnly = FALSE;
		col.bReadOnlyRefItem = FALSE;
		tab.GetField(i,field,type,name);
		if( name.IsEmpty() )continue;	
		
 		strcpy(col.FieldName,field);
		strcpy(col.ColumnName,name);
// 		CString str ;
// 		str.LoadString(IDS_FIELDNAME_DEFCOLOR);
		col.ValueType = type;
		if (strcmp(LPCTSTR(name),LPCTSTR(StrFromResID(IDS_FIELDNAME_DEFCOLOR)))==0)
		{
			if (bIsRefDS)
			{
				col.bReadOnly = FALSE;
			}
			col.Type = CLVLPropColumn::COLOR;
		}
		else if (strcmp(LPCTSTR(name),LPCTSTR(StrFromResID(IDS_FIELDNAME_GROUPNAME)))==0)
		{
			col.Type = CLVLPropColumn::COMBO;
			for (int j=0; j<pDS->GetFtrLayerGroupCount(); j++)
			{				
				FtrLayerGroup *pGroup = pDS->GetFtrLayerGroup(j);
				if (pGroup)
				{
					col.arrComboValues.Add(pGroup->Name);
				}
				
			}
		}
		else
		{
			if( col.ValueType==VT_BOOL )
			{
				if (strcmp(col.FieldName,FIELDNAME_LAYVISIBLE)==0)
				{
					if (bIsRefDS)
					{
						col.bReadOnly = FALSE;
					}
					col.BitmapOffset = 1;
				}
				else if (strcmp(col.FieldName,FIELDNAME_LAYINHERENT)==0)
				{
					col.bReadOnly = TRUE;
					col.BitmapOffset = 1;
				}
				else if (strcmp(col.FieldName,FIELDNAME_LAYLOCKED)==0)
				{
					col.BitmapOffset = 0;
				}
				col.Type = CLVLPropColumn::CHECK;
			}
			else
			{
				if (strcmp(col.FieldName,FIELDNAME_LAYERDISPLAYORDER)==0)
				{
					col.bReadOnly = TRUE;
				}

				col.Type = CLVLPropColumn::EDIT;
			}

		}	
		if (i==0/*||i==1*/)
		{
			col.bReadOnlyRefItem = TRUE;
		}
		m_wndPropListCtrl.AddColumn(col);
	}

	if(1)
	{		
		strcpy(col.FieldName,FIELDNAME_LAYERCODE);
		strcpy(col.ColumnName,StrFromResID(IDS_LAYMAN_LAYERCODE));
		col.ValueType = VT_BSTR;
		col.Type = CLVLPropColumn::EDIT;
		col.bReadOnly = TRUE;
		m_wndPropListCtrl.InsertColumn(1,col);

		strcpy(col.FieldName,FIELDNAME_LAYUSED);
		strcpy(col.ColumnName,StrFromResID(IDS_LAYMAN_USED));
		col.ValueType = VT_I4;
		col.Type = CLVLPropColumn::EDIT;
		col.bReadOnly = TRUE;
		m_wndPropListCtrl.AddColumn(col);

		strcpy(col.FieldName,FIELDNAME_LAYERDEL);
		strcpy(col.ColumnName,StrFromResID(IDS_DELETED));
		col.ValueType = VT_BOOL;
		col.Type = CLVLPropColumn::CHECK;
		col.bReadOnly = FALSE;
		col.BitmapOffset = 1;
		m_wndPropListCtrl.InsertColumn(4,col);

		strcpy(col.FieldName,FIELDNAME_LAYVISIBLE);
		strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_VISIBLE));
		col.ValueType = VT_BOOL;
		col.Type = CLVLPropColumn::CHECK;
		col.bReadOnly = FALSE;
		col.BitmapOffset = 1.5;
		m_wndPropListCtrl.InsertColumn(5,col);

		strcpy(col.FieldName,FIELDNAME_GEOCLASS);
		strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_GEOCLASS));
		col.ValueType = VT_BSTR;
		col.BitmapOffset = 1;
		col.Type = CLVLPropColumn::COMBO;
		col.arrComboValues.RemoveAll();
		col.arrComboValues.Add(StrFromResID(IDS_GEO_POINT));//点
		col.arrComboValues.Add(StrFromResID(IDS_GEO_CURVE));//线
		col.arrComboValues.Add(StrFromResID(IDS_GEO_SURFACE));//面
		col.arrComboValues.Add(StrFromResID(IDS_GEO_TEXT));//文本
		col.arrComboValues.Add(_T(""));//缺省无
		col.bReadOnly = TRUE;
		m_wndPropListCtrl.InsertColumn(9,col);

		strcpy(col.FieldName,XMLTAG_DBLAYER);
		strcpy(col.ColumnName,StrFromResID(IDS_DBLAYERNAME));
		col.ValueType = VT_BSTR;
		col.Type = CLVLPropColumn::EDIT;
		col.bReadOnly = TRUE;
		m_wndPropListCtrl.InsertColumn(10,col);
	}
	m_wndPropListCtrl.InitHeaderCtrl();

	CDlgDataSource *pDS0 = NULL;
	if(!pDS)
		pDS0 = m_pDlgDoc->GetDlgDataSource();
	else
		pDS0 = pDS;
	int nLayer = pDS0->GetFtrLayerCount();
	CLVLPropItem *pItem;
	const CVariantEx* value;
	int nCol = m_wndPropListCtrl.GetColumnCount();
	__int64 code;
	for( i=0; i<nLayer; i++)
	{
		pLayer = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		if(!bForLoad&&pLayer->IsEmpty()) continue;
		tab.DelAll();

		tab.BeginAddValueItem();
		pLayer->WriteTo(tab);
		tab.EndAddValueItem();
		tab.DelField(FIELDNAME_CLSTYPE);
		tab.DelField(FIELDNAME_LAYERID);
		tab.DelField(FIELDNAME_LAYERDISPLAYORDER);

		pItem = new CLVLPropItem;
		if( !pItem )continue;
		if(pLayer->IsInherent()) 
			pItem->SetReadOnly(TRUE);
		
		m_wndPropListCtrl.AddItem(pItem);
		if (pDS0->GetCurFtrLayer()==pLayer)
		{
			m_wndPropListCtrl.SetCurLayerPropItem(pItem);
		}

		for( int j=0, k=0; j<nCol; j++)
		{
			if(j==1)
			{
				code = pDS0->GetLayerCodeOfFtrLayer(pLayer);
				CString strcode;
				strcode.Format("%I64d", code);
				pItem->SetValue((_variant_t)strcode,j);
				continue;
			}
			else if (j == 4)
			{
				continue;
			}
			else if (j == nCol-1)//最后一列地物数列
			{
				pItem->SetValue((long)pLayer->GetValidObjsCount(),j);
				continue;
			}
			else if (j == 9 )//几何体类型
			{
				CString strStyle;
				int GeoType = CLS_NULL;
				CScheme *pScheme = gpCfgLibMan->GetScheme(pDS0->GetScale());
				CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pLayer->GetName());
				if (!pSchemeLayer) 
				{
					int ncount = pLayer->GetObjectCount();
					CFeature *pFtr = NULL;
					for( int n=0; n<ncount; n++)
					{
						pFtr = pLayer->GetObject(0,FILTERMODE_DELETED);
						if( pFtr )break;
					}
					if (pFtr==NULL)
					{
						strStyle = StrFromResID(IDS_GEO_NONE);	
						pItem->SetValue((_variant_t)strStyle,j);		
						continue;
					}
					
					GeoType = pFtr->GetGeometry()->GetClassType();
				}
				else
				{
					GeoType = pSchemeLayer->GetGeoClass();
				}
				switch(GeoType) 
				{
				case CLS_GEOPOINT:
				case CLS_GEODIRPOINT:
				case CLS_GEOMULTIPOINT:
				case CLS_GEODEMPOINT:
				case CLS_GEOSURFACEPOINT:
					strStyle = StrFromResID(IDS_GEO_POINT);
					break;
				case CLS_GEOCURVE:
				case CLS_GEODCURVE:
				case CLS_GEOPARALLEL:
					strStyle = StrFromResID(IDS_GEO_CURVE);
					break;
				case CLS_GEOMULTISURFACE:
				case CLS_GEOSURFACE:
					strStyle = StrFromResID(IDS_GEO_SURFACE);
					break;
				case CLS_GEOTEXT:
					strStyle = StrFromResID(IDS_GEO_TEXT);
					break;
				default:
					strStyle = StrFromResID(IDS_GEO_NONE);
					;
				}
				pItem->SetValue((_variant_t)strStyle,j);		
				continue;
			}
			else  if (j == 10)
			{
				CString strDBLayerName;

				CScheme *pScheme = gpCfgLibMan->GetScheme(pDS0->GetScale());
				CSchemeLayerDefine *pSchemeLayer = pScheme->GetLayerDefine(pLayer->GetName());	
				
				strDBLayerName = (pSchemeLayer==NULL?CString():pSchemeLayer->GetDBLayerName());
				
				pItem->SetValue((_variant_t)(LPCTSTR)strDBLayerName,j);		
				continue;
			}
			
			col = m_wndPropListCtrl.GetColumn(j);
			if (tab.GetValue(0,col.FieldName,value))
			{
				pItem->SetValue((_variant_t)*value,j);
			}
			/*tab.GetField(k,field,type,name);
			if( name.IsEmpty() )
			continue;		
			
			  tab.GetValue(0,k++,value);
			pItem->SetValue((_variant_t)*value,j);*/
			
		}
		pItem->SetData((DWORD_PTR)pLayer);
	}
	if(!bForLoad)
		m_wndPropListCtrl.FilterPropItems();

	m_wndPropListCtrl.SetFocus();
}

CString XAttriColName(const XAttributeItem &item)
{
	CString type;
	switch(item.valuetype)
	{
	case DP_CFT_BOOL: 
		type = StrFromResID(IDS_BOOL_TYPE);
		break;
	case DP_CFT_COLOR:
		type = StrFromResID(IDS_COLOR_TYPE);
		break;
	case DP_CFT_VARCHAR:
		type = StrFromResID(IDS_CHAR_TYPE);
		break;
	case DP_CFT_FLOAT:
		type = StrFromResID(IDS_SINGLE_TYPE);
		break;
	case DP_CFT_DOUBLE:
		type = StrFromResID(IDS_DOUBLE_TYPE);
		break;
	case DP_CFT_SMALLINT:
		type = StrFromResID(IDS_SMALLINT_TYPE);
		break;
	case DP_CFT_INTEGER:
		type = StrFromResID(IDS_LONG_TYPE);
		break;
	case DP_CFT_DATE:
		type = StrFromResID(IDS_DATE_TYPE);
		break;
	default:
		break;		
		
	}
	
	CString str;
	str.Format("%s(%s)",item.name,type);

	return str;
}


//用全部地物填充列表,参数为空指针时，默认为主数据源
void CLayersMgrDlg::FillAllFeatures(LPCTSTR filter, CDlgDataSource *pDS)
{
	FilterItem *pFilter = FindFilterItem(filter);
	if( !pFilter )return;
	
	m_arrFtrItems.RemoveAll();

	CDlgDataSource *pDS0 = NULL;
	if(!pDS)
		pDS0 = m_pDlgDoc->GetDlgDataSource();
	else
		pDS0 = pDS;
	int nLayer = pDS0->GetFtrLayerCount();

	CValueTable tab;

	CString strGName;
	for(int i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		
		for (int j=0; j<pLayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
			if (!pFtr) continue;

			long color = pFtr->GetGeometry()->GetColor();
			
			int nFtrItemSize = m_arrFtrItems.GetSize();
			for (int k=0; k<nFtrItemSize; k++)
			{
				if (stricmp(m_arrFtrItems[k].pFtrLayer->GetName(),pLayer->GetName()) == 0 && 
					     m_arrFtrItems[k].color == color && m_arrFtrItems[k].bVisible == pFtr->IsVisible() &&
						 m_arrFtrItems[k].code.CompareNoCase(pFtr->GetCode()) == 0)
				{
					// 扩展属性
					BOOL bSame = TRUE;
					for (int m=0; m<m_arrFtrItems[k].arrXAttibutes.GetSize();m++)
					{
						XAttributeItem xItem = m_arrFtrItems[k].arrXAttibutes[m];
						CValueTableEx tab;
						const CVariantEx* value = NULL;
						XAttributeItem tmp(xItem);
						if (( !m_arrMemXAttr.Lookup(pFtr,tab) || !tab.GetValue(0,xItem.field,value) || 
							(Variant2XAttribute(*value,tmp) && tmp!=xItem) ) && stricmp(xItem.value,"-NA-")!=0 )
						{
							bSame = FALSE;
							break;
						}

					}
					
					if (bSame)
					{
						m_arrFtrItems[k].ftrs.Add(pFtr);
						break;
					}				
					
				}
			}

			if (k < nFtrItemSize)  continue;

			FeatureItem item;
			item.pFtrLayer = pLayer;
			item.layerCode = pDS0->GetLayerCodeOfFtrLayer(pLayer);
			item.color = color;
			item.code = pFtr->GetCode();
			item.bVisible = pFtr->IsVisible();
			item.ftrs.Add(pFtr);

			POSITION pos = m_arrXAttibutes.GetStartPosition();
			while (pos != NULL)
			{
				XAttributeItem xItem;
				BOOL show;
				m_arrXAttibutes.GetNextAssoc(pos,xItem,show);
					
				if (show)
				{
					CValueTableEx tab;
					const CVariantEx* value = NULL;

					XAttributeItem tmp;
					if (m_arrMemXAttr.Lookup(pFtr,tab) && tab.GetValue(0,xItem.field,value) &&
						   Variant2XAttribute(*value,tmp) && tmp.valuetype == xItem.valuetype )
					{						
						Variant2XAttribute(*value,xItem);
					}
					else
					{
						xItem.valuetype = DP_CFT_NULL;
						xItem.value = "-NA-";
					}
					
					item.arrXAttibutes.Add(xItem);
				}
					
			}
				
			m_arrFtrItems.Add(item);

		}
		
	}

	m_wndPropListCtrl.RemoveAll();

	BOOL bIsRefDS = FALSE;
	if (pDS&&pDS!=m_pDlgDoc->GetDlgDataSource())
	{
		bIsRefDS = TRUE;
	}
	//层名
	CLVLPropColumn col;
	strcpy(col.FieldName,FIELDNAME_LAYERNAME);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_LAYERNAME));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	//层码
	strcpy(col.FieldName,FIELDNAME_LAYERCODE);
	strcpy(col.ColumnName,StrFromResID(IDS_LAYMAN_LAYERCODE));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	//缺省颜色
	strcpy(col.FieldName,FIELDNAME_LAYCOLOR);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_DEFCOLOR));
	col.ValueType = VT_I4;
	col.Type = CLVLPropColumn::COLOR;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);
	//标识码
	strcpy(col.FieldName,FIELDNAME_FTRCODE);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_FTRCODE));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn::EDIT;
	if (bIsRefDS) 
	{
		col.bReadOnly = TRUE;
	}
	else
	{
		col.bReadOnly = FALSE;
	}
	m_wndPropListCtrl.AddColumn(col);
	//显示
	strcpy(col.FieldName,FIELDNAME_FTRVISIBLE);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_FTRVISIBLE));
	col.ValueType = VT_BOOL;
	col.Type = CLVLPropColumn::CHECK;
	col.BitmapOffset = 1;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);
	//地物数
	strcpy(col.FieldName,FIELDNAME_LAYUSED);
	strcpy(col.ColumnName,StrFromResID(IDS_LAYMAN_USED));
	col.ValueType = VT_I4;
	col.Type = CLVLPropColumn::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	//删除
	strcpy(col.FieldName,FIELDNAME_LAYERDEL);
	strcpy(col.ColumnName,StrFromResID(IDS_DELETED));
	col.ValueType = VT_BOOL;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn::CHECK;
	col.bReadOnly = FALSE;
	//	col.BitmapOffset = 2;
	m_wndPropListCtrl.InsertColumn(2,col);

	
	//扩展属性
	POSITION pos = m_arrXAttibutes.GetStartPosition();
	while (pos != NULL)
	{
		int colSize = m_wndPropListCtrl.GetColumnCount();

		XAttributeItem xItem;
		BOOL show;
		m_arrXAttibutes.GetNextAssoc(pos,xItem,show);
			
		if (show)
		{
			CString str = XAttriColName(xItem);
			strcpy(col.FieldName,str);
			strcpy(col.ColumnName,str);

			if (xItem.valuetype == DP_CFT_BOOL)
			{
				col.ValueType = VT_BOOL;
				col.Type = CLVLPropColumn::CHECK;
				col.BitmapOffset = 1;
				
			}
// 			else if (xItem.valuetype == DP_CFT_INTEGER)
// 			{
// 				col.ValueType = VT_I4;
// 				col.Type = CLVLPropColumn::EDIT;
// 			}
			else
			{
				col.ValueType = VT_BSTR;
				col.Type = CLVLPropColumn::EDIT;
			}

			col.bReadOnly = FALSE;

			if (bIsRefDS) 
			{
				col.bReadOnly = TRUE;
			}
			else
			{
				col.bReadOnly = FALSE;
			}

			m_wndPropListCtrl.InsertColumn(colSize-1,col);

		}
	}

	m_wndPropListCtrl.InitHeaderCtrl();

	CHeaderCtrl& header = m_wndPropListCtrl.GetHeaderCtrl();
	for( i=0; i<header.GetItemCount(); i++)
	{
		HDITEM hitem;
		hitem.mask = HDI_WIDTH;
		hitem.cxy = 80;
		header.SetItem(i,&hitem);
	}

	CLVLPropItem *pItem;
	for (i=0; i<m_arrFtrItems.GetSize(); i++)
	{
		pItem = new CLVLPropItem;
		if( !pItem )continue;
		
		m_wndPropListCtrl.AddItem(pItem);

		pItem->SetData((DWORD_PTR)(m_arrFtrItems.GetData() + i));
		pItem->SetValue((_variant_t)(LPCTSTR)m_arrFtrItems[i].pFtrLayer->GetName(),0);
		CString strcode;
		strcode.Format("%I64d", m_arrFtrItems[i].layerCode);
		pItem->SetValue((_variant_t)(LPCTSTR)strcode,1);
		long color = m_arrFtrItems[i].color;
// 		if (color == COLOUR_BYLAYER)
// 		{
// 			color = m_arrFtrItems[i].pFtrLayer->GetColor();
// 		}
		pItem->SetValue((_variant_t)(long)color,3);
		pItem->SetValue((_variant_t)(LPCTSTR)m_arrFtrItems[i].code,4);
		pItem->SetValue((_variant_t)(bool)m_arrFtrItems[i].bVisible,5);
		
		int xSize = m_arrFtrItems[i].arrXAttibutes.GetSize();
		for (int j=0; j<xSize; j++)
		{
			XAttributeItem xItem = m_arrFtrItems[i].arrXAttibutes[j];
			if (xItem.valuetype == DP_CFT_NULL)
			{
				pItem->SetValue((_variant_t)(LPCTSTR)(xItem.value),6+j);
				pItem->SetColReadOnly(6+j,TRUE);
			}
			else if (xItem.valuetype == DP_CFT_BOOL)
			{
				pItem->SetValue((_variant_t)(bool)atol(xItem.value),6+j);
			}
// 			else if (xItem.valuetype == DP_CFT_INTEGER)
// 			{
// 				pItem->SetValue((_variant_t)atol(xItem.value),6+j);
// 			}
			else
			{
				pItem->SetValue((_variant_t)(LPCTSTR)(xItem.value),6+j);
			}
		}

		pItem->SetValue((_variant_t)(long)m_arrFtrItems[i].ftrs.GetSize(),6+xSize);
		
	}

	pItem = new CLVLPropItem;
	if( !pItem )return;
	
	m_wndPropListCtrl.SetFilterItem(pItem);
	
	int nfield = m_wndPropListCtrl.GetColumnCount();
	const CVariantEx* value;
	for( int j=0, k=0; j<nfield; j++)
	{
		CLVLPropColumn col = m_wndPropListCtrl.GetColumn(j);
		//		col.bReadOnly = TRUE;
		if( !pFilter->tab.GetValue(0,col.FieldName,value) )
			pItem->SetValue(_variant_t(),k++);
		else
			pItem->SetValue((_variant_t)*value,k++);
	}

	m_wndPropListCtrl.FilterPropItems();
}


//用新的数据源填充层组
void CLayersMgrDlg::FillAllGroups(CDlgDataSource *pDS)
{
	/*m_arrLayerGroups.RemoveAll();*/

	CDlgDataSource *pDS0 = NULL;
	if(!pDS)
		pDS0 = m_pDlgDoc->GetDlgDataSource();
	else
		pDS0 = pDS;
	/*int nLayer = pDS0->GetFtrLayerCount();

	CValueTable tab;

	CString strGName;
	for(int i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer )continue;
		
		CString str = pLayer->GetGroupName();

		int nGroupSize = m_arrLayerGroups.GetSize();
		for (int j=0; j<nGroupSize; j++)
		{
			if (m_arrLayerGroups[j].name.CompareNoCase(str) == 0)
			{
				m_arrLayerGroups[j].arr.Add(pLayer);
				break;
			}
		}

		if (j < nGroupSize)  continue;

		LayerGroup group;
		group.name = pLayer->GetGroupName();
		group.color = pLayer->GetColor();
		group.bVisible = pLayer->IsVisible();
		group.bSymbolized = pLayer->IsSymbolized();
		group.arr.Add(pLayer);

		if (group.name.IsEmpty())
		{
			group.name = "";
		}

		m_arrLayerGroups.Add(group);
		
	}*/

	m_wndPropListCtrl.RemoveAll();//清空过滤器列表

	CLVLPropColumn col;
	strcpy(col.FieldName,FIELDNAME_LAYGROUPNAME);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_GROUPNAME));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn::EDIT;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);

	strcpy(col.FieldName,FIELDNAME_LAYCOLOR);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_DEFCOLOR));
	col.ValueType = VT_I4;
	col.Type = CLVLPropColumn::COLOR;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);

	strcpy(col.FieldName,FIELDNAME_LAYERDEL);
	strcpy(col.ColumnName,StrFromResID(IDS_DELETED));
	col.ValueType = VT_BOOL;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn::CHECK;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);

	strcpy(col.FieldName,FIELDNAME_LAYVISIBLE);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_VISIBLE));
	col.ValueType = VT_BOOL;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn::CHECK;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);

	strcpy(col.FieldName,FIELDNAME_LAYERSYMBOLIZED);
	strcpy(col.ColumnName,StrFromResID(IDS_FIELDNAME_SYMBOL));
	col.ValueType = VT_BOOL;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn::CHECK;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);

	strcpy(col.FieldName,FIELDNAME_LAYUSED);
	strcpy(col.ColumnName,StrFromResID(IDS_LAYMAN_USEDLAYERS));
	col.ValueType = VT_I4;
	col.Type = CLVLPropColumn::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);

	m_wndPropListCtrl.InitHeaderCtrl();

	CHeaderCtrl& header = m_wndPropListCtrl.GetHeaderCtrl();
	for(int i=0; i<header.GetItemCount(); i++)
	{
		HDITEM hitem;
		hitem.mask = HDI_WIDTH;
		hitem.cxy = 120;
		header.SetItem(i,&hitem);
	}

	CLVLPropItem *pItem;
	for (i=0; i<pDS0->GetFtrLayerGroupCount(); i++)
	{
		pItem = new CLVLPropItem;
		if( !pItem )continue;
		
		m_wndPropListCtrl.AddItem(pItem);//添加一个Item

		FtrLayerGroup *pGroup = pDS0->GetFtrLayerGroup(i);
		
		//设置Item各个列的值。
		pItem->SetValue((_variant_t)(LPCTSTR)pGroup->Name,0);
		pItem->SetValue((_variant_t)pGroup->Color,1);
		pItem->SetValue((_variant_t)(bool)pGroup->Visible,3);
		pItem->SetValue((_variant_t)(bool)pGroup->Symbolized,4);
		pItem->SetValue((_variant_t)(long)pGroup->ftrLayers.GetSize(),5);
		pItem->SetData((DWORD_PTR)(pGroup));
		
	}

	m_wndPropListCtrl.FilterPropItems();

	m_wndPropListCtrl.SetFocus();

}

//用带过滤器的层填充列表,pDS为空指针时默认主数据源
void CLayersMgrDlg::FillFilterLayersList(LPCTSTR filter, CDlgDataSource *pDS, BOOL bOnlyUsedLayers)
{
	FilterItem *pFilter = FindFilterItem(filter);
	if( !pFilter )return;

	FillAllLayersList(pDS,!bOnlyUsedLayers);

	CLVLPropItem *pItem = new CLVLPropItem;
	if( !pItem )return;

	m_wndPropListCtrl.SetFilterItem(pItem);

	int nfield = m_wndPropListCtrl.GetColumnCount();
	const CVariantEx* value;
	for( int j=0, k=0; j<nfield; j++)
	{
		CLVLPropColumn col = m_wndPropListCtrl.GetColumn(j);
//		col.bReadOnly = TRUE;
		if( !pFilter->tab.GetValue(0,col.FieldName,value) )
			pItem->SetValue(_variant_t(),k++);
		else
			pItem->SetValue((_variant_t)*value,k++);
	}
	m_wndPropListCtrl.FilterPropItems();

	m_wndPropListCtrl.SetFocus();

	if (m_nSortColumn >= 0 && m_nSortColumn < m_wndPropListCtrl.GetColumnCount())
	{
		CMFCHeaderCtrl *header = (CMFCHeaderCtrl*)&m_wndPropListCtrl.GetHeaderCtrl();
		header->SetSortColumn(m_nSortColumn,m_bSortAscending);
		m_wndPropListCtrl.SortItems(m_nSortColumn,m_bSortAscending);
	}	
}

//新建过滤器
void CLayersMgrDlg::OnNewFilter() 
{
	FilterItem *pItem = new FilterItem;
	if( !pItem )return;

	CString name;
	for( int i=0; ; i++ )
	{
		name.Format("New Filter %d", i);
		if( !FindFilterItem(name) )break;
	}

	pItem->name = name;
	pItem->tab.DelAll();
	pItem->tab.BeginAddValueItem();
	pItem->tab.AddValue(FIELDNAME_LAYVISIBLE,&CVariantEx((bool)1));
	pItem->tab.AddValue(FIELDNAME_LAYUSED,&CVariantEx((bool)1));
	pItem->tab.AddValue(FIELDNAME_LAYERSYMBOLIZED,&CVariantEx((bool)1));
	pItem->tab.AddValue(FIELDNAME_LAYINHERENT,&CVariantEx((bool)1));
	pItem->tab.AddValue(FIELDNAME_LAYCOLOR,&CVariantEx((long)-2));
	pItem->tab.EndAddValueItem();

	HTREEITEM hItem = m_wndTreeCtrl.FindItemByData(LM_TREEID_FILTERS);
	if( hItem==NULL )return;

	HTREEITEM hNew = m_wndTreeCtrl.InsertSubItem(name,hItem,LM_TREEID_FILTERITEM,LM_TREEID_FILTERITEM);
	m_wndTreeCtrl.Expand(hItem,TVE_EXPAND);
	m_wndTreeCtrl.EditLabel(hNew);
	

	m_arrFilters.Add(pItem);
}

FilterItem* CLayersMgrDlg::FindFilterItem(LPCTSTR name)
{
	if (!name) return NULL;

	for( int i=m_arrFilters.GetSize()-1; i>=0; i--)
	{
		FilterItem *pItem = m_arrFilters.GetAt(i);
		if( pItem->name.CompareNoCase(name)==0 )
			return pItem;
	}
	return NULL;
}

// 重命名过滤器
void CLayersMgrDlg::OnRenameFilter() 
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem==NULL || (m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM&&m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM_REF) )
		return;
	
	m_wndTreeCtrl.EditLabel(hItem);	 
}

//删除过滤器
void CLayersMgrDlg::OnDeleteFilter() 
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem==NULL || (m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM&&m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM_REF) )
		return;

	CString strFilter = m_wndTreeCtrl.GetItemText(hItem);
	RemoveFilter(strFilter);

	HTREEITEM hItem2 = m_wndTreeCtrl.GetNextSiblingItem(hItem);
	if( !hItem2 )hItem2 = m_wndTreeCtrl.GetPrevSiblingItem(hItem);
	if( !hItem2 )hItem2 = m_wndTreeCtrl.GetParentItem(hItem);

	m_wndTreeCtrl.DeleteItem(hItem);
	m_wndTreeCtrl.SelectItem(hItem2);
}


//选择更改时
void CLayersMgrDlg::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_dCurSelItem = 0;

	*pResult = 0; 
	NMTREEVIEW *pTree = (NMTREEVIEW*)pNMHDR;

	if (pTree->itemNew.hItem != NULL)
		m_dCurSelItem = m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem);
 
// 	if( pTree->itemOld.hItem!=NULL &&
// 		(m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_ALLLEVELS
// 		||m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_ALLLEVELS_REF))
// 	{
// 		/*SaveLayers()*/;
// 	}

	if( pTree->itemOld.hItem!=NULL &&
		(m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_FILTERITEM 
		||m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_FILTERITEM_REF 
		||m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_ALLLEVELS
		||m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_ALLLEVELS_REF
		||m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_ALLFEATURES
		||m_wndTreeCtrl.GetItemData(pTree->itemOld.hItem)==LM_TREEID_ALLFEATURES_REF))
	{
		/*SaveLayers()*/;//保存上一个的过滤器
		SaveFilter(m_wndTreeCtrl.GetItemText(pTree->itemOld.hItem));
	}
	
	if( pTree->itemNew.hItem!=NULL &&
		(m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_ALLLEVELS
		||m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_ALLLEVELS_REF))
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		while (1)
		{
			if (m_wndTreeCtrl.GetParentItem(hItem)!=NULL) //父节点不为空
			{
				hItem = m_wndTreeCtrl.GetParentItem(hItem);	//得到父节点
			}
			else
				break;			 
		}
		//数据源
		CDlgDataSource *pDS = (CDlgDataSource *)m_wndTreeCtrl.GetItemData(hItem);
		//FillAllLayersList(pDS,FALSE);	

		FillFilterLayersList(_T("All Used Levels"),pDS,TRUE);

		m_wndPropListCtrl.AdjustLayout();
		m_wndPropListCtrl.RedrawWindow();
	}
	else if( pTree->itemNew.hItem!=NULL &&
		(m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_ALLFEATURES
		||m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_ALLFEATURES_REF))
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		while (1)
		{
			if (m_wndTreeCtrl.GetParentItem(hItem)!=NULL)
			{
				hItem = m_wndTreeCtrl.GetParentItem(hItem);
			}
			else
				break;			 
		}
		CDlgDataSource *pDS = (CDlgDataSource *)m_wndTreeCtrl.GetItemData(hItem);
		FillAllFeatures(_T("All Features"),pDS);
		m_wndPropListCtrl.AdjustLayout();
		m_wndPropListCtrl.RedrawWindow();
	}
	else if( pTree->itemNew.hItem!=NULL &&
		(m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_GROUPS
		||m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_GROUPS_REF))
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		while (1)
		{
			if (m_wndTreeCtrl.GetParentItem(hItem)!=NULL)
			{
				hItem = m_wndTreeCtrl.GetParentItem(hItem);
			}
			else
				break;			 
		}
		CDlgDataSource *pDS = (CDlgDataSource *)m_wndTreeCtrl.GetItemData(hItem);
		FillAllGroups(pDS);		
		m_wndPropListCtrl.AdjustLayout();
		m_wndPropListCtrl.RedrawWindow();
	}
	else if( pTree->itemNew.hItem!=NULL &&
		(m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_FILTERITEM 
		||m_wndTreeCtrl.GetItemData(pTree->itemNew.hItem)==LM_TREEID_FILTERITEM_REF))
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		while (1)
		{
			if (m_wndTreeCtrl.GetParentItem(hItem)!=NULL)
			{
				hItem = m_wndTreeCtrl.GetParentItem(hItem);
			}
			else
				break;			 
		}
		CDlgDataSource *pDS = (CDlgDataSource *)m_wndTreeCtrl.GetItemData(hItem);
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(pTree->itemNew.hItem),pDS);

		m_wndPropListCtrl.AdjustLayout();
		m_wndPropListCtrl.RedrawWindow();
	}
	else
	{
		m_wndPropListCtrl.RemoveAll();
		m_wndPropListCtrl.RedrawWindow();
	}

	UpdateStatusString();	
}

void CLayersMgrDlg::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	// Create the pop up menu
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(!hItem)
		return;
	DWORD_PTR dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw!=LM_TREEID_FILTERS&&dw!=LM_TREEID_FILTERITEM)
		return;
	CMenu obMenu;
	obMenu.LoadMenu(IDR_TREE_POPUP); 
	
	CMenu* pPopupMenu = obMenu.GetSubMenu(0);
	ASSERT(pPopupMenu); 
	
	
	// Get the cursor position
	CPoint obCursorPoint = (0, 0);
	
	GetCursorPos(&obCursorPoint);
	
	// Track the popup menu
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, obCursorPoint.x, 
		obCursorPoint.y, this);
	
}


void CLayersMgrDlg::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	*pResult = -1;
	
	HTREEITEM hItem = pTVDispInfo->item.hItem;
	if( hItem==NULL || (m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM&&m_wndTreeCtrl.GetItemData(hItem)!=LM_TREEID_FILTERITEM_REF) )
	{
		return;
	}
	*pResult = 0;
}

void CLayersMgrDlg::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	*pResult = 0;
	if( pTVDispInfo->item.pszText!=NULL && FindFilterItem(pTVDispInfo->item.pszText)!=NULL )
		return;

	*pResult = 1;

	if( pTVDispInfo->item.pszText==NULL )
		return;

	CString strNewText = pTVDispInfo->item.pszText;
	CString strText = m_wndTreeCtrl.GetItemText(pTVDispInfo->item.hItem);
	FilterItem *pItem = FindFilterItem(strText);
	
	pItem->name = strNewText;
}


BOOL CLayersMgrDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return false;
		if (pMsg->wParam == 'A'||pMsg->wParam == 'a' )
		{
			if (GetKeyState(VK_CONTROL)&0x8000)
			{
				m_wndPropListCtrl.SelectAll();
			}			
		}
		//响应键盘Delete和Insert
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		if(hItem == NULL) return CDialog::PreTranslateMessage(pMsg);
		if (pMsg->wParam == VK_DELETE)//响应键盘删除
		{
		
			if(m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS|| m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLFEATURES)
			{
				OnDelLayerObj();//删除地物 
			}
			else if (m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_GROUPS	)
			{
				OnDelLayerGroup();//删除层组
			}
			else if ( m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM )
			{
				OnDelLayer();	//删除空层
			}
				
		}
		else if (pMsg->wParam == VK_INSERT)//响应键盘Insert键
		{
			if(m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS|| m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLFEATURES)
			{
				
			}
			else if (m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_GROUPS	)
			{
				OnNewLayerGroup() ;//新建层组
			}
			else if ( m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM )
			{
				OnNewLayer();	//新建层
			}		
			
		}	
	

	}

	for (int i = 0; i < m_wndToolBarFilter.GetCount(); i++)
	{
		m_wndToolBarFilter.UpdateButton(i);
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CLayersMgrDlg::CanModifyCheckState()
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem!=NULL && (m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS||m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM||
						m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLFEATURES) )
		return TRUE;

	return FALSE;
}

void CLayersMgrDlg::ModifyCheckState(CLVLPropItem **pItems, int num, CLVLPropColumn* pCol, int col, _variant_t value)
{
	if (pItems == NULL || num <= 0 || pCol == NULL) return;

	if (m_dCurSelItem == LM_TREEID_ALLFEATURES)
	{
		CUndoModifyProperties undo(m_pDlgDoc,_T("ModifyFtrPropertie"));
		
		for (int i=0; i<num; i++)
		{
			CLVLPropItem *pPropitem = pItems[i];
			if (!pPropitem) continue;
			
			if (pCol->bReadOnlyRefItem && pPropitem->IsReadOnly())
				continue;
			
			pPropitem->SetValue(value,col);
			
			{
				_variant_t data;	
				
				FeatureItem *pItem = (FeatureItem*)pPropitem->GetData();
				if (pItem)
				{	
					for ( int j=0; j<pItem->ftrs.GetSize(); j++)
					{
						if (stricmp(pCol->FieldName,FIELDNAME_FTRVISIBLE) != 0)
							continue;

						undo.arrHandles.Add(FtrToHandle(pItem->ftrs[j]));
						undo.oldVT.BeginAddValueItem();
						pItem->ftrs[j]->WriteTo(undo.oldVT);
						undo.oldVT.EndAddValueItem();
						
						m_pDlgDoc->DeleteObject(FtrToHandle(pItem->ftrs[j]),FALSE);

						pItem->ftrs[j]->EnableVisible((bool)value);
						
						if( !m_pDlgDoc->RestoreObject(FtrToHandle(pItem->ftrs[j])) )
						{			
							continue;
						}
						
						undo.newVT.BeginAddValueItem();
						pItem->ftrs[j]->WriteTo(undo.newVT);
						undo.newVT.EndAddValueItem();
					}

					
				}
				
			}
			
		}
		
		undo.Commit();

		return;
	}

	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL || !(m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS||m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM) )
		return;

	CUndoModifyLayer undo(m_pDlgDoc,_T("ModifyLayer"));
	undo.field.Format("%s",pCol->FieldName);

	for (int i=0; i<num; i++)
	{
		CLVLPropItem *pItem = pItems[i];
		if (!pItem) continue;

		if (pCol->bReadOnlyRefItem && pItem->IsReadOnly())
			continue;
		
		pItem->SetValue(value,col);

		{
			_variant_t data;	
			
			CFtrLayer *pLayer = (CFtrLayer*)pItem->GetData();
			if (pLayer)
			{					
				if (stricmp(pCol->FieldName,FIELDNAME_LAYLOCKED)==0)
				{
					data = (bool)pLayer->IsLocked();	
					
				}
				else if (stricmp(pCol->FieldName,FIELDNAME_LAYVISIBLE)==0)
				{
					data = (bool)pLayer->IsVisible();		
					
				}
				else if (stricmp(pCol->FieldName,FIELDNAME_LAYERSYMBOLIZED)==0)
				{
					data = (bool)pLayer->IsSymbolized();			
				}
				else
					continue;
				undo.arrLayers.Add(pLayer);
				undo.arrOldVars.Add(data);
				m_pDlgDoc->ModifyLayer(pLayer,pCol->FieldName,value,TRUE);
			}
					
		}

	}

	undo.newVar = value;
	undo.Commit();

	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CLayersMgrDlg::ModifyFtrLayerGroupAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc)
{
	if (stricmp(Pc->FieldName,FIELDNAME_LAYERDEL) == 0)
	{
		OnDelLayerGroup();
		return;
	}

	_variant_t value = pItem->GetValue(Pc->FieldName);
	CLVLPropItem* pPropitem;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if (hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_GROUPS)
	{
		if (m_wndPropListCtrl.GetSelectedCount()<=0)
		{
			return;
		}
		
		// 修改层组属性提示  sky modified 2012-9-7 
	//	AfxMessageBox(StrFromResID(IDS_TIP_LAYERGROUP_MODIFY),MB_OK);

		_variant_t data;
		
		CUndoModifyLayer undo(m_pDlgDoc,_T("ModifyLayer"));
		undo.field.Format("%s",Pc->FieldName);
		undo.newVar = value;
		
		for (int i=0;i<m_wndPropListCtrl.GetSelectedCount();i++)
		{
			pPropitem = m_wndPropListCtrl.GetSelectedItem(i);
			if (pPropitem)
			{
				FtrLayerGroup *pGroup = (FtrLayerGroup*)pPropitem->GetData();
				if (pGroup)
				{
					if (stricmp(Pc->FieldName,FIELDNAME_LAYGROUPNAME)==0)
					{
						pGroup->Name = (LPCTSTR)(_bstr_t)value;	
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYVISIBLE)==0)
					{
						pGroup->Visible = (long)value;							
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
					{
						pGroup->Color = (long)value;			
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYERSYMBOLIZED)==0)
					{
						pGroup->Symbolized = (long)value;			
					}

					m_pDlgDoc->GetDlgDataSource()->UpdateFtrLayerGroup(pGroup);

					for (int j=0; j<pGroup->ftrLayers.GetSize(); j++)
					{
						CFtrLayer *pLayer = pGroup->ftrLayers[j];
						if (stricmp(Pc->FieldName,FIELDNAME_LAYGROUPNAME)==0)
						{
							data = pGroup->Name;	
						}
						else if (stricmp(Pc->FieldName,FIELDNAME_LAYVISIBLE)==0)
						{
							data = (bool)pLayer->IsVisible();							
						}
						else if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
						{
							data = (long)pLayer->GetColor();			
						}
						else if (stricmp(Pc->FieldName,FIELDNAME_LAYERSYMBOLIZED)==0)
						{
							data = (bool)pLayer->IsSymbolized();			
						}
						
						undo.arrLayers.Add(pLayer);
						undo.arrOldVars.Add(data);
						m_pDlgDoc->ModifyLayer(pLayer,Pc->FieldName,value,TRUE);
						
					}					
				}
			}
			
		}
		
		undo.Commit();
	}
	else if (hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_GROUPS_REF)
	{
		if (m_wndPropListCtrl.GetSelectedCount()<=0)
		{
			return;
		}
		
		_variant_t data;
		
		for (int i=0;i<m_wndPropListCtrl.GetSelectedCount();i++)
		{
			pPropitem = m_wndPropListCtrl.GetSelectedItem(i);
			if (pPropitem)
			{
				FtrLayerGroup *pGroup = (FtrLayerGroup*)pPropitem->GetData();
				if (pGroup)
				{
					if (stricmp(Pc->FieldName,FIELDNAME_LAYVISIBLE)==0)
					{
						pGroup->Visible = (long)value;							
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
					{
						pGroup->Color = (long)value;			
					}

					for (int j=0; j<pGroup->ftrLayers.GetSize(); j++)
					{
						CFtrLayer *pLayer = pGroup->ftrLayers[j];
						if (stricmp(Pc->FieldName,FIELDNAME_LAYVISIBLE)==0)
						{
							data = (bool)pLayer->IsVisible();							
						}
						else if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
						{
							data = (long)pLayer->GetColor();			
						}
						else continue;
						
						m_pDlgDoc->ModifyLayer(pLayer,Pc->FieldName,value,FALSE);
						
					}					
				}
			}
			
		}
	}


	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CLayersMgrDlg::ModifyFtrsAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc)
{
    if (stricmp(Pc->FieldName,FIELDNAME_LAYERDEL) == 0)
	{
		OnDelFtrs();
		return;
	}

	_variant_t value = pItem->GetValue(Pc->FieldName);
	CLVLPropItem* pPropitem;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLFEATURES )
	{
		if (m_wndPropListCtrl.GetSelectedCount()<=0)
		{
			return;
		}		
		
		_variant_t data;	
		
		CUndoBatchAction batchundo(m_pDlgDoc,_T("ModifyFtrsProperties"));

		for (int i=0;i<m_wndPropListCtrl.GetSelectedCount();i++)
		{
			pPropitem = m_wndPropListCtrl.GetSelectedItem(i);
			if (pPropitem)
			{
				FeatureItem *pItem = (FeatureItem*)pPropitem->GetData();
				if (pItem)
				{	
					if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
					{
						pItem->color = (long)value;
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_FTRCODE)==0)
					{
						pItem->code = (const char*)(_bstr_t)value;
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_FTRVISIBLE)==0)
					{
						pItem->bVisible = (bool)value;
					}
					else
					{
						for (int j=0; j<pItem->arrXAttibutes.GetSize(); j++)
						{
							XAttributeItem &xItem = pItem->arrXAttibutes[j];
							CString str = XAttriColName(xItem);
							if (str.CompareNoCase(Pc->FieldName) == 0)
							{
								xItem.value = (const char*)(_bstr_t)value;
								break;
							}
						}

					}

					CUndoModifyProperties undo(m_pDlgDoc,_T("ModifyFtrPropertie"));

					if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0 || stricmp(Pc->FieldName,FIELDNAME_FTRCODE)==0 || stricmp(Pc->FieldName,FIELDNAME_FTRVISIBLE)==0)
					{
						for ( int j=0; j<pItem->ftrs.GetSize(); j++)
						{
							undo.arrHandles.Add(FtrToHandle(pItem->ftrs[j]));
							undo.oldVT.BeginAddValueItem();
							pItem->ftrs[j]->WriteTo(undo.oldVT);
							undo.oldVT.EndAddValueItem();
							
							m_pDlgDoc->DeleteObject(FtrToHandle(pItem->ftrs[j]),FALSE);

							if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
							{
								pItem->ftrs[j]->GetGeometry()->SetColor((long)value);
							}
							else if (stricmp(Pc->FieldName,FIELDNAME_FTRCODE)==0)
							{
								pItem->ftrs[j]->SetCode((const char*)(_bstr_t)value);
							}
							else if (stricmp(Pc->FieldName,FIELDNAME_FTRVISIBLE)==0)
							{
								pItem->ftrs[j]->EnableVisible((bool)value);
							}
							
							
							if( !m_pDlgDoc->RestoreObject(FtrToHandle(pItem->ftrs[j])) )
							{			
								continue;
							}
							
							undo.newVT.BeginAddValueItem();
							pItem->ftrs[j]->WriteTo(undo.newVT);
							undo.newVT.EndAddValueItem();
						}
					}
					else
					{
						for (int j=0; j<pItem->arrXAttibutes.GetSize(); j++)
						{
							XAttributeItem &xItem = pItem->arrXAttibutes[j];
							CString str = XAttriColName(xItem);
							if (str.CompareNoCase(Pc->FieldName) == 0)
							{								
								// 保存扩展属性到磁盘文件
								CAttributesSource  *pDXS = m_pDlgDoc->GetDlgDataSource()->GetXAttributesSource();
								if(!pDXS) return;
								
								for (int m=0;m<pItem->ftrs.GetSize();m++)
								{
									CValueTableEx old;
									const CVariantEx* value = NULL;
									
									XAttributeItem tmp;
									
									if (!m_arrMemXAttr.Lookup(pItem->ftrs[m],old) || !old.GetValue(0,xItem.field,value) ||
										(Variant2XAttribute(*value,tmp) && tmp.valuetype != xItem.valuetype) )
									{
										continue;
									}
									
									//if (m_arrMemXAttr.Lookup(pItem->ftrs[m],old))
									{	
										undo.arrHandles.Add(FtrToHandle(CPFeature(pItem->ftrs[m])));
										undo.XoldVT.BeginAddValueItem();
										undo.XoldVT.AddValuesFromTab(old);
										undo.XoldVT.EndAddValueItem();

										CArray<XAttributeItem,XAttributeItem> arr;
										arr.Add(xItem);
										CValueTableEx tab,tab1;
										ConvertXAttributeItemAndValueTab(arr,tab1,1);
										
										tab.AddValuesFromTab(old);

										CString field, name;
										int type;
										const CVariantEx *var = NULL;
										tab1.GetField(0,field,type,name);
										tab1.GetValue(0,field,var);

										tab.SetValue(0,field,var);

										m_arrMemXAttr.SetAt(pItem->ftrs[m],tab);

										undo.XnewVT.CopyFrom(tab);

										m_pDlgDoc->DeleteObject(FtrToHandle(pItem->ftrs[m]),FALSE);

										pDXS->SetXAttributes(pItem->ftrs[m],tab);

										if( !m_pDlgDoc->RestoreObject(FtrToHandle(pItem->ftrs[m])) )
										{			
											continue;
										}
									}
									
								}
									
								break;
							}
						}
						
					}
					

					//undo.Commit();

					batchundo.AddAction(&undo);
				}
			}

		}

		batchundo.Commit();
				
	}
	else if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLFEATURES_REF )
	{
		if (m_wndPropListCtrl.GetSelectedCount()<=0)
		{
			return;
		}		
		
		_variant_t data;	

		for (int i=0;i<m_wndPropListCtrl.GetSelectedCount();i++)
		{
			pPropitem = m_wndPropListCtrl.GetSelectedItem(i);
			if (pPropitem)
			{
				FeatureItem *pItem = (FeatureItem*)pPropitem->GetData();
				if (pItem)
				{	
					if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
					{
						pItem->color = (long)value;
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_FTRVISIBLE)==0)
					{
						pItem->bVisible = (bool)value;
					}
					else continue;

					if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0 || stricmp(Pc->FieldName,FIELDNAME_FTRVISIBLE)==0)
					{
						for ( int j=0; j<pItem->ftrs.GetSize(); j++)
						{
							UpdateDispyParam param;

							param.handle = (LONG_PTR)pItem->ftrs[j];
							if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
							{
								param.type = UpdateDispyParam::typeCOLOR;
								pItem->ftrs[j]->GetGeometry()->SetColor((long)value);
								param.data.lColor = (long)value;
							}
							else if (stricmp(Pc->FieldName,FIELDNAME_FTRVISIBLE)==0)
							{
								param.type = UpdateDispyParam::typeVISIBLE;
								pItem->ftrs[j]->EnableVisible((bool)value);
								param.data.bVisible = (bool)value;
							}
							else continue;					
							
							m_pDlgDoc->UpdateAllViews(NULL,hc_UpdateObjectDisplay,(CObject*)&param);
						}
					}
					
				}
			}

		}
	}


	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}

BOOL CLayersMgrDlg::IsValidNewValue(const char* field, _variant_t value)
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	while (1)
	{
		if (m_wndTreeCtrl.GetParentItem(hItem)!=NULL)
		{
			hItem = m_wndTreeCtrl.GetParentItem(hItem);
		}
		else
			break;			 
	}
	
	CDlgDataSource *pDS = (CDlgDataSource *)m_wndTreeCtrl.GetItemData(hItem);
	if (!pDS) return FALSE;

	if (stricmp(field,FIELDNAME_LAYERNAME) == 0)
	{
		// 检查是否重名
		if (pDS->GetFtrLayer((const char*)(_bstr_t)value))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CLayersMgrDlg::ModifyAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc, _variant_t exData)
{

	if (m_dCurSelItem == LM_TREEID_ALLFEATURES || m_dCurSelItem == LM_TREEID_ALLFEATURES_REF)
	{
		ModifyFtrsAttri(pItem,Pc);
		return;
	}
	else if (m_dCurSelItem == LM_TREEID_GROUPS || m_dCurSelItem == LM_TREEID_GROUPS_REF)
	{
		ModifyFtrLayerGroupAttri(pItem,Pc);
		return;
	}

	CFtrLayer* pLayer = NULL;

	pLayer = (CFtrLayer*)pItem->GetData();
    if (stricmp(Pc->FieldName,FIELDNAME_LAYERNAME)==0)
    {	
		if (pLayer->IsInherent())
		{
			return;
		}
	}
	else if (stricmp(Pc->FieldName,FIELDNAME_LAYERDEL)==0)
	{
		OnDelLayerObj();
		return;
	}

	_variant_t value = pItem->GetValue(Pc->FieldName);
	CLVLPropItem* pPropitem;
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem!=NULL && (m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS||m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM) )
	{
		if (m_wndPropListCtrl.GetSelectedCount()<=0)
		{
			return;
		}
		CUndoModifyLayer undo(m_pDlgDoc,_T("ModifyLayer"));
		undo.field.Format("%s",Pc->FieldName);
		
		_variant_t data;	
		
		for (int i=0;i<m_wndPropListCtrl.GetSelectedCount();i++)
		{
			pPropitem = m_wndPropListCtrl.GetSelectedItem(i);
			if (pPropitem)
			{
				pLayer = (CFtrLayer*)pPropitem->GetData();
				if (pLayer)
				{					
					if (stricmp(Pc->FieldName,FIELDNAME_LAYERNAME)==0)
					{
						data = pLayer->GetName();	
						CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
						const char *strLayerName = (const char*)(_bstr_t)value;
						if (pDS && pDS->GetFtrLayer(strLayerName) != NULL)
						{	
							AfxMessageBox(IDS_CMDTIP_LAYERNAMEREPEAT);
							return;
						}
					}	
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYMAPNAME)==0)
					{	
						data = pLayer->GetMapName();
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYGROUPNAME)==0)
					{	
						data = pLayer->GetGroupName();
						CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
						if (pDS)
						{
							FtrLayerGroup *pLayerGroup = pDS->GetFtrLayerGroupByName((const char*)(_bstr_t)data);
							if (pLayerGroup)
							{
								pLayerGroup->DelFtrLayer(pLayer);
							}
							pLayerGroup = pDS->GetFtrLayerGroupByName((const char*)(_bstr_t)value);
							if (pLayerGroup)
							{
								pLayerGroup->AddFtrLayer(pLayer);
							}
						}
						
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYLOCKED)==0)
					{
						data = (bool)pLayer->IsLocked();	
						
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYVISIBLE)==0)
					{
						data = (bool)pLayer->IsVisible();		
						
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYCOLOR)==0)
					{
						data = (long)pLayer->GetColor();			
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYERDISPLAYORDER)==0)
					{
						data = (long)pLayer->GetDisplayOrder();			
					}
					else if (stricmp(Pc->FieldName,FIELDNAME_LAYERSYMBOLIZED)==0)
					{
						data = (bool)pLayer->IsSymbolized();			
					}
					else
						continue;
					undo.arrLayers.Add(pLayer);
					undo.arrOldVars.Add(data);
					m_pDlgDoc->ModifyLayer(pLayer,Pc->FieldName,value,TRUE);
				}
			}

		}
		undo.newVar = value;
		undo.Commit();		
	}
	else if (hItem!=NULL && (m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS_REF||m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM_REF))
	{
		m_pDlgDoc->ModifyLayer(pLayer,Pc->FieldName,value,FALSE);
	}			

	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}

void CLayersMgrDlg::RButtonDown(UINT nFlags, CPoint point)
{
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if(hItem==NULL)
		return;
	DWORD_PTR dw = m_wndTreeCtrl.GetItemData(hItem);
	if(dw!=LM_TREEID_ALLLEVELS&&dw!=LM_TREEID_ALLLEVELS_REF
		&&dw!=LM_TREEID_FILTERITEM&&dw!=LM_TREEID_FILTERITEM_REF
		&&dw!=LM_TREEID_ALLFEATURES&&dw!=LM_TREEID_ALLFEATURES_REF
		&&dw!=LM_TREEID_GROUPS&&dw!=LM_TREEID_GROUPS_REF)
		return;

	CMenu obMenu;
	if (dw == LM_TREEID_GROUPS || dw == LM_TREEID_GROUPS_REF)
	{
		obMenu.LoadMenu(IDR_LAYERGROUPMANAGER_RB);
	}
	else
	{
		obMenu.LoadMenu(IDR_LAYERMANAGER_RB); 
	}
	
	CMenu* pPopupMenu = obMenu.GetSubMenu(0);
	ASSERT(pPopupMenu);
	CPoint obCursorPoint = (0, 0);
	
	GetCursorPos(&obCursorPoint);
			
	if(dw==LM_TREEID_ALLLEVELS_REF||dw==LM_TREEID_FILTERITEM_REF)
	{
		pPopupMenu->EnableMenuItem(ID_NEWLAYER,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_DELLAYER,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_DELLAYEROBJ,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_ACTIVECURLAYER,MF_GRAYED);

	}
	else if (dw==LM_TREEID_ALLFEATURES || dw==LM_TREEID_ALLFEATURES_REF)
	{
		pPopupMenu->EnableMenuItem(ID_NEWLAYER,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_DELLAYER,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_DELLAYEROBJ,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_CUTLAYEROBJ,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_PASTELAYEROBJ,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_PASTELAYEROBJEX,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_ACTIVECURLAYER,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_SHOW_TOP,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_SHOW_TAIL,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_SHOW_MOVE,MF_GRAYED);
		pPopupMenu->EnableMenuItem(ID_SHOW_INSERT,MF_GRAYED);
	}
	else if (dw == LM_TREEID_GROUPS || dw == LM_TREEID_GROUPS_REF)
	{
	}
	
	// Track the popup menu
	pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, obCursorPoint.x, 
		obCursorPoint.y, this);
}

void CLayersMgrDlg::OnAddLayerScheme()
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	if (!pDS) return;

	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return;

	for (int i=0; i<pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pItem = pScheme->GetLayerDefine(i);
		if (!pItem) continue;

		CString strLayerName = pItem->GetLayerName();

		// 增加方案中有数据中没有的图层
		if (!pDS->GetFtrLayer(strLayerName))
		{
			CFtrLayer *pLayer = new CFtrLayer;
			pLayer->SetName(strLayerName);
			pLayer->SetInherentFlag(TRUE);
			pLayer->SetColor(pItem->GetColor());
			m_pDlgDoc->AddFtrLayer(pLayer);
		}
	}
}

void CLayersMgrDlg::OnDelLayerScheme()
{
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	if (!pDS) return;
	
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return;
	
	for (int i=0; i<pDS->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrLayer = pDS->GetFtrLayerByIndex(i);
		if (!pFtrLayer || pFtrLayer->GetObjectCount()>0) continue;

		CString strLayerName = pFtrLayer->GetName();		
		
		// 删除方案中没有数据中有的空图层
		if (!pScheme->GetLayerDefine(strLayerName))
		{
			m_pDlgDoc->DelFtrLayer(pFtrLayer);
		}
	}
}

void CLayersMgrDlg::OnShowTop()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 ) return;

	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}
	
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	
	if (nCount==1)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(0);
		CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
		if (pLayer)
		{
			pDS->DisplayTop(pLayer);
			int col = GetCol(FIELDNAME_LAYERDISPLAYORDER);
			UpdateOrderValue();
			m_wndPropListCtrl.ReSortItems(col);

			m_pDlgDoc->UpdateAllViews(NULL,hc_UpdateLayerDisplayOrder);
		}
	}

}

void CLayersMgrDlg::OnShowBottom()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 ) return;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}
	
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	
	if (nCount==1)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(0);
		CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
		if (pLayer)
		{
			pDS->DisplayBottom(pLayer);
			int col = GetCol(FIELDNAME_LAYERDISPLAYORDER);
			UpdateOrderValue();
			m_wndPropListCtrl.ReSortItems(col);

			m_pDlgDoc->UpdateAllViews(NULL,hc_UpdateLayerDisplayOrder);
		}
	}

}

void CLayersMgrDlg::OnShowMove()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 ) return;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}
	
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	
	if (nCount==1)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(0);
		m_pMovedLayer = (CFtrLayer*)pSel->GetData();
	}
}

void CLayersMgrDlg::OnSelectFtrs()
{
	CArray<DWORD_PTR, DWORD_PTR> arrSelItems;
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	for (int i=0;i<nCount;i++)
	{
		CLVLPropItem *pPropItem = m_wndPropListCtrl.GetSelectedItem(i);
		arrSelItems.Add(pPropItem->GetData());
	}

	if (arrSelItems.GetSize() <= 0)
	{
		return;
	}

	if (m_dCurSelItem == LM_TREEID_ALLFEATURES || m_dCurSelItem == LM_TREEID_ALLFEATURES_REF)
	{
		for (i=0; i<arrSelItems.GetSize(); i++)
		{
			FeatureItem *ftrs = (FeatureItem*)arrSelItems[i];
			if (!ftrs) continue;
			
			for (int j=0; j<ftrs->ftrs.GetSize(); j++)
			{
				CFeature *pFtr = ftrs->ftrs[j];
				if (!pFtr) continue;

				m_pDlgDoc->SelectObj(FTR_HANDLE(pFtr));
			}
		}
	}
	else if (m_dCurSelItem == LM_TREEID_ALLLEVELS || m_dCurSelItem == LM_TREEID_ALLLEVELS_REF)
	{
		for (i=0; i<arrSelItems.GetSize(); i++)
		{
			CFtrLayer *pLayer = (CFtrLayer*)arrSelItems[i];
			if (!pLayer) continue;

			for (int j=0; j<pLayer->GetObjectCount(); j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (!pFtr) continue;

				m_pDlgDoc->SelectObj(FTR_HANDLE(pFtr));
			}
			
		}
	}
	else if (m_dCurSelItem == LM_TREEID_GROUPS || m_dCurSelItem == LM_TREEID_GROUPS_REF)
	{
		for (i=0; i<arrSelItems.GetSize(); i++)
		{
			FtrLayerGroup *pLayerGroup = (FtrLayerGroup*)arrSelItems[i];
			if (!pLayerGroup) continue;
			
			for (int i=0; i<pLayerGroup->ftrLayers.GetSize(); i++)
			{
				CFtrLayer *pLayer = (CFtrLayer*)pLayerGroup->ftrLayers[i];
				if (!pLayer) continue;
				
				for (int j=0; j<pLayer->GetObjectCount(); j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr) continue;
				
					m_pDlgDoc->SelectObj(FTR_HANDLE(pFtr));
				}
			}
			
		}
	}

	m_pDlgDoc->OnSelectChanged();

}

void CLayersMgrDlg::OnDeSelectFtrs()
{
	CArray<DWORD_PTR, DWORD_PTR> arrSelItems;
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	for (int i=0;i<nCount;i++)
	{
		CLVLPropItem *pPropItem = m_wndPropListCtrl.GetSelectedItem(i);
		arrSelItems.Add(pPropItem->GetData());
	}
	
	if (arrSelItems.GetSize() <= 0)
	{
		return;
	}
	
	if (m_dCurSelItem == LM_TREEID_ALLFEATURES || m_dCurSelItem == LM_TREEID_ALLFEATURES_REF)
	{
		for (i=0; i<arrSelItems.GetSize(); i++)
		{
			FeatureItem *ftrs = (FeatureItem*)arrSelItems[i];
			if (!ftrs) continue;
			
			for (int j=0; j<ftrs->ftrs.GetSize(); j++)
			{
				m_pDlgDoc->DeselectObj(FTR_HANDLE(ftrs->ftrs[j]));
			}
		}
	}
	else if (m_dCurSelItem == LM_TREEID_ALLLEVELS || m_dCurSelItem == LM_TREEID_ALLLEVELS_REF)
	{
		for (i=0; i<arrSelItems.GetSize(); i++)
		{
			CFtrLayer *pLayer = (CFtrLayer*)arrSelItems[i];
			if (!pLayer) continue;
			
			for (int j=0; j<pLayer->GetObjectCount(); j++)
			{
				m_pDlgDoc->DeselectObj(FTR_HANDLE(pLayer->GetObject(j)));
			}
			
		}
	}
	else if (m_dCurSelItem == LM_TREEID_GROUPS || m_dCurSelItem == LM_TREEID_GROUPS_REF)
	{
		for (i=0; i<arrSelItems.GetSize(); i++)
		{
			FtrLayerGroup *pLayerGroup = (FtrLayerGroup*)arrSelItems[i];
			if (!pLayerGroup) continue;
			
			for (int i=0; i<pLayerGroup->ftrLayers.GetSize(); i++)
			{
				CFtrLayer *pLayer = (CFtrLayer*)pLayerGroup->ftrLayers[i];
				if (!pLayer) continue;
				
				for (int j=0; j<pLayer->GetObjectCount(); j++)
				{
					m_pDlgDoc->DeselectObj(FTR_HANDLE(pLayer->GetObject(j)));
				}
			}
			
		}
	}
	
	m_pDlgDoc->OnSelectChanged();
	
}

void CLayersMgrDlg::OnShowInsert()
{
	if (!m_pMovedLayer) return;

	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 ) return;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}
	
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	
	if (nCount==1)
	{
		CLVLPropItem* pSel = arrPSels.GetAt(0);
		CFtrLayer *pLayer = (CFtrLayer*)pSel->GetData();
		if (pLayer)
		{
			pDS->DisplayInsert(m_pMovedLayer,pLayer);
			int col = GetCol(FIELDNAME_LAYERDISPLAYORDER);
			UpdateOrderValue();
			m_wndPropListCtrl.ReSortItems(col);

			m_pDlgDoc->UpdateAllViews(NULL,hc_UpdateLayerDisplayOrder);
		}
	}

}

int CLayersMgrDlg::GetCol(CString field)
{
	int nfield = m_wndPropListCtrl.GetColumnCount();
	const CVariantEx* value;
	for( int j=0; j<nfield; j++)
	{
		CLVLPropColumn col = m_wndPropListCtrl.GetColumn(j);
		if (field.CompareNoCase(col.FieldName) == 0)
		{
			return j;
		}
	}

	return 0;
}

void CLayersMgrDlg::UpdateOrderValue()
{
	int col = GetCol(FIELDNAME_LAYERDISPLAYORDER);

	int nCount = m_wndPropListCtrl.GetItemCount();
	if( nCount<=0 ) return;
	
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetPropItem(i));
	}
	
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	
	for (i=0; i<nCount; i++)
	{
		if (!arrPSels[i]) continue;

		// 层名
		_variant_t value = arrPSels[i]->GetValue(0);
		CFtrLayer *pLayer = pDS->GetFtrLayer((const char*)(_bstr_t)value);
		if (pLayer)
		{
			arrPSels[i]->SetValue((_variant_t)(long)pLayer->GetDisplayOrder(),col);
		}
	}
}
void CLayersMgrDlg::UpdateColorValue()
{
	int col = GetCol(FIELDNAME_LAYCOLOR);
	int nCount = m_wndPropListCtrl.GetItemCount();
	if( nCount<=0 ) return;
	CArray<CLVLPropItem*,CLVLPropItem*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetPropItem(i));
	}
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	for (i=0; i<nCount; i++)
	{
		if (!arrPSels[i]) continue;
		_variant_t value = arrPSels[i]->GetValue(0);
		CFtrLayer *pLayer = pDS->GetFtrLayer((const char*)(_bstr_t)value);
		if (pLayer)
		{
			arrPSels[i]->SetValue((_variant_t)(long)pLayer->GetColor(),col);
		}
	}
}

BOOL CLayersMgrDlg::IsFtrscolorByLayer(LONG_PTR dWord)
{
	if (m_dCurSelItem != LM_TREEID_ALLFEATURES) return FALSE;	
	
	FeatureItem *pFtrItem = (FeatureItem*)dWord;
	if (!pFtrItem)  return FALSE;

	if (pFtrItem->color == COLOUR_BYLAYER)
	{
		return TRUE;
	}
	
	return FALSE;
}

BOOL CLayersMgrDlg::IsFtrsManage()
{
	if (m_dCurSelItem == LM_TREEID_ALLFEATURES) return TRUE;

	return FALSE;
}

BOOL CLayersMgrDlg::LoadUniqueXAttributesDefine()
{
	m_arrXAttibutes.RemoveAll();
	m_arrXAttibutes.InitHashTable(97);

	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
	if (!pScheme) return FALSE;
	for (int i=0; i<pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine* pdef = pScheme->GetLayerDefine(i);
		if (!pdef) continue;

		int size;
		const XDefine *pXDefine = pdef->GetXDefines(size);

		const XDefine *pAttr = pXDefine;

		for (int j=0; j<size; j++,pAttr++)
		{
			XAttributeItem item;
			item.field = pAttr->field;
			item.name = pAttr->name;
			item.valuetype = pAttr->valuetype;
			
			int value;
			if (!m_arrXAttibutes.Lookup(item,value))
			{
				m_arrXAttibutes.SetAt(item,1);
			}
		}
		
	}

	return TRUE;
}

BOOL CLayersMgrDlg::LoadXAttributes(CDlgDataSource *pDS)
{
	m_arrMemXAttr.RemoveAll();

	CDlgDataSource *pDS0 = NULL;
	if(!pDS)
		pDS0 = m_pDlgDoc->GetDlgDataSource();
	else
		pDS0 = pDS;

	if (!pDS0)  return FALSE; 
	
	int nLayer = pDS0->GetFtrLayerCount();
	
	CString strGName;
	for(int i=0; i<nLayer; i++)
	{
		CFtrLayer *pLayer = pDS0->GetFtrLayerByIndex(i);
		if( !pLayer )continue;

// 		XAttributeItem item;
// 		m_arrLayerAttibutes.Lookup(pLayer->GetName(),item);
		
		for (int j=0; j<pLayer->GetObjectCount(); j++)
		{
			CFeature *pFtr = pLayer->GetObject(j,FILTERMODE_DELETED);
			if (!pFtr) continue;
			
			CValueTableEx tab;
			tab.BeginAddValueItem();
			if (pDS0->GetXAttributesSource()->GetXAttributes(pFtr,tab))
			{
				tab.EndAddValueItem();
				m_arrMemXAttr.SetAt(pFtr,tab);
			}
		}
	}

	return TRUE;
}

void CLayersMgrDlg::OnLoadXAttributes()
{
	if (m_dCurSelItem != LM_TREEID_ALLFEATURES) return;

	int size = m_arrXAttibutes.GetCount();
	if (size == 0)
	{
		LoadUniqueXAttributesDefine();
	}	

	// 选择要显示的扩展属性
	CDlgXAttributes dlg;
	dlg.XAttributes(m_arrXAttibutes,CDlgXAttributes::waySet);

	if (dlg.DoModal() == IDCANCEL) return; 	

	dlg.XAttributes(m_arrXAttibutes,CDlgXAttributes::wayGet);
	
	LoadXAttributes(m_pDlgDoc->GetDlgDataSource());

	FillAllFeatures(_T("All Features"),m_pDlgDoc->GetDlgDataSource());
}

void CLayersMgrDlg::OnToTop()
{
   int nCount = m_wndPropListCtrl.GetSelectedCount();
   if(nCount<=0)
   {
	   return;
   }
   CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
   if (!pDS)
   {
	   return;
   }
   CLVLPropItem *pSel=0;
   CFtrLayer *pLayer=0;
   CUndoModifyProperties undo(m_pDlgDoc,"DisplayAtTop");
   for(int k=0;k<nCount;++k)
   {
	   pSel=m_wndPropListCtrl.GetSelectedItem(k);
	   pLayer=(CFtrLayer*)pSel->GetData();
	   if(!pLayer)
	   {
		   continue;
	   }
	   //
	   CFtrArray arrSrcFtrs;
	   pLayer->GetAllFtrsByDisplayOrder(arrSrcFtrs);
	   int i=0;
	   for (i=0; i<arrSrcFtrs.GetSize(); i++)
	   {
		   if(arrSrcFtrs[i]->IsDeleted())
		   {
			   continue;
		   }
		   undo.arrHandles.Add(FTR_HANDLE(arrSrcFtrs[i]));
		   undo.oldVT.BeginAddValueItem();
		   arrSrcFtrs[i]->WriteTo(undo.oldVT);
		   undo.oldVT.EndAddValueItem();
	   }
	   //
	   pDS->DisplayTop(arrSrcFtrs.GetData(),arrSrcFtrs.GetSize());
	   //
	   for (i=0; i<arrSrcFtrs.GetSize(); i++)
	   {			
		   if(arrSrcFtrs[i]->IsDeleted())
		   {
			   continue;
		   }
		   m_pDlgDoc->DeleteObject(FTR_HANDLE(arrSrcFtrs[i]),FALSE);
		   m_pDlgDoc->RestoreObject(FTR_HANDLE(arrSrcFtrs[i]));
		   //
		   undo.newVT.BeginAddValueItem();
		   arrSrcFtrs[i]->WriteTo(undo.newVT);
		   undo.newVT.EndAddValueItem();
		}
   }
   //
   undo.Commit();
   m_pDlgDoc->UpdateAllViews(NULL);
}

void CLayersMgrDlg::OnToBottom()
{
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if(nCount<=0)
	{
		return;
	}
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	if (!pDS)
	{
		return;
	}
	CLVLPropItem *pSel=0;
	CFtrLayer *pLayer=0;
	CUndoModifyProperties undo(m_pDlgDoc,"DisplayAtTop");
	for(int k=0;k<nCount;++k)
	{
		pSel=m_wndPropListCtrl.GetSelectedItem(k);
		pLayer=(CFtrLayer*)pSel->GetData();
		if(!pLayer)
		{
			continue;
		}
		//
		CFtrArray arrSrcFtrs;
		pLayer->GetAllFtrsByDisplayOrder(arrSrcFtrs);
		int i=0;
		for (i=0; i<arrSrcFtrs.GetSize(); i++)
		{
			if(arrSrcFtrs[i]->IsDeleted())
			{
				continue;
			}
			undo.arrHandles.Add(FTR_HANDLE(arrSrcFtrs[i]));
			undo.oldVT.BeginAddValueItem();
			arrSrcFtrs[i]->WriteTo(undo.oldVT);
			undo.oldVT.EndAddValueItem();
		}
		//
		pDS->DisplayBottom(arrSrcFtrs.GetData(),arrSrcFtrs.GetSize());
		//
		for (i=0; i<arrSrcFtrs.GetSize(); i++)
		{			
			if(arrSrcFtrs[i]->IsDeleted())
			{
				continue;
			}
			m_pDlgDoc->DeleteObject(FTR_HANDLE(arrSrcFtrs[i]),FALSE);
			m_pDlgDoc->RestoreObject(FTR_HANDLE(arrSrcFtrs[i]));
			//
			undo.newVT.BeginAddValueItem();
			arrSrcFtrs[i]->WriteTo(undo.newVT);
			undo.newVT.EndAddValueItem();
		}
	}
	//
	undo.Commit();
    m_pDlgDoc->UpdateAllViews(NULL);
}

void CLayersMgrDlg::OnConvertLayer()
{
	//获取源层层码列表；
	int nCount = m_wndPropListCtrl.GetSelectedCount();
	if( nCount<=0 )return;
	CStringArray layer_code_list;
	int i=0;
	for(i=0; i<nCount; i++)
	{
		CLVLPropItem *temp_pSel=m_wndPropListCtrl.GetSelectedItem(i);
		CFtrLayer *temp_pLayer=(CFtrLayer*)temp_pSel->GetData();
		if(!temp_pLayer)
		{
			continue;
		}
		layer_code_list.Add((LPCTSTR)(_bstr_t)temp_pSel->GetValue(FIELDNAME_LAYERNAME));
	}
	//弹出对话框；
	CDlgChangeFCode dlg;
	if(layer_code_list.GetSize()==0)
	{
		return;
	}
	else if(layer_code_list.GetSize()==1)
	{
		dlg.str_fcode=layer_code_list[0];
	}
	else
	{
        dlg.str_fcode=layer_code_list[0]+",";
		for(int i=1;i<layer_code_list.GetSize()-1;++i)
		{
			dlg.str_fcode+=layer_code_list[i];
			dlg.str_fcode=dlg.str_fcode+",";
		}
        dlg.str_fcode+=layer_code_list[layer_code_list.GetSize()-1];
	}
	dlg.can_edit=FALSE;
	if(dlg.DoModal()!=IDOK  )
	{
		return;
	}
	//
	CDlgDataSource *pDS = m_pDlgDoc->GetDlgDataSource();
	if (!pDS) return;
	//判断目标层是否存在；
	char *code2=(LPTSTR)(LPCTSTR)dlg.m_strTarFCode;
	CString strMsg;
	CFtrLayer *pLayer2 = pDS->GetFtrLayer(code2);	
	if (!pLayer2)
	{
		strMsg.Format(IDS_ERR_INVALIDLAYER,(LPCTSTR)code2);
		AfxMessageBox(strMsg);//cjc 2012年11月8日 提示不存在输入层
		return;
	}
	//
	if (AfxMessageBox(StrFromResID(IDS_CMDTIP_CHANGELAYERNAME),MB_OKCANCEL)==IDCANCEL)
	{
		return;
	}
	CUndoFtrs undo(m_pDlgDoc,"ConvertLayer");
	//实现层转换功能；
	char *code1=0;
	CFtrLayer *pLayer1=0;
	CFeature *pFtr = NULL, *pTemp = NULL;

	for( i=0; i<layer_code_list.GetSize(); i++)
	{
		code1=(LPTSTR)(LPCTSTR)layer_code_list[i];
		pFtr = NULL;
		pTemp = NULL;
		pLayer1 = pDS->GetFtrLayer(code1);	
		if( !pLayer1 || pLayer1->GetObjectCount()<=0)
		{
			continue;
		}
		//获取实体对象总数
		long lSum = pLayer1->GetObjectCount();
		
		pTemp = pLayer2->CreateDefaultFeature(pDS->GetScale());
		if( !pTemp )continue;
		
		CScheme *pScheme = gpCfgLibMan->GetScheme(pDS->GetScale());
		
		for(int j=0; j<lSum; j++)
		{
			pFtr = pLayer1->GetObject(j);
			if( !pFtr )continue;

			CValueTable tab;
			tab.BeginAddValueItem();
			pDS->GetXAttributesSource()->GetXAttributes(pFtr,tab );
			tab.EndAddValueItem();
			
			m_pDlgDoc->DeleteObject(FtrToHandle(pFtr));
			undo.AddOldFeature(FtrToHandle(pFtr));
			
			CValueTable table;
			table.BeginAddValueItem();
			pFtr->WriteTo(table);
			table.EndAddValueItem();
			
			CFeature *pNewFtr = pTemp->Clone();
			pNewFtr->ReadFrom(table);
			pNewFtr->SetID(OUID());
			
			CGeometry *pGeo = pNewFtr->GetGeometry();
			
			float wid = 0;
			if (pScheme && pLayer2)
			{
				wid = pScheme->GetLayerDefineLineWidth(pLayer2->GetName());					
			}
			
			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
			{
				((CGeoCurveBase*)pNewFtr->GetGeometry())->m_fLineWidth = wid;
			}
			
			m_pDlgDoc->AddObject(pNewFtr,pLayer2->GetID());
			undo.AddNewFeature(FtrToHandle(pNewFtr));
			pDS->GetXAttributesSource()->SetXAttributes(pNewFtr,tab );
		}

		if( pTemp )delete pTemp;
	}
	
	if (undo.arrNewHandles.GetSize() > 0)
	{
		undo.Commit();
		m_pDlgDoc->UpdateAllViews(NULL);
	}

	//更新列表显示；
	HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
	if( hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_ALLLEVELS )
	{
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),pDS,TRUE);
	}
	else if(hItem!=NULL && m_wndTreeCtrl.GetItemData(hItem)==LM_TREEID_FILTERITEM) 
	{
		HTREEITEM hItem = m_wndTreeCtrl.GetSelectedItem();
		FillFilterLayersList(m_wndTreeCtrl.GetItemText(hItem),pDS,FALSE);
	}
}
