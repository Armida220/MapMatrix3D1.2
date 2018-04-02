// BaseView.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "BaseView.h"
#include "editbasedoc.h "
#include "DlgDataSource.h "
#include "ExMessage.h "
#include "SymbolLib.h "
#include "RegDef.h "
#include "RegDef2.h "
#include "UIParam2.h"
#include "FullScreenAssist.h "

#include "DlgCommand.h "
#include "CommandLidar.h"

#include "DrawingDef.h "
#include "Functions_temp.h"
#include "PluginAPI.h "

#include "StereoView.h"
#include "dlglayervisible.h"
#include "MainFrm.h"

#include "CheckDongleLicenseFile.h"
#include "DlgCommand2.h"
#include "License.h"
#ifdef TRIAL_VERSION
#include "WinlicenseSDK.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#define CHEDATA_TYPE_BMP CHEDATA_TYPE_GLLIST

#define REG_CMDCLASS(pWorker,id,clsname)  (pWorker)->RegisterCommand((id),clsname::Create)

void CreateSnapBuffer(GrBuffer2d *buf, SNAPITEM item, PT_3D cpt, int r, int w, BOOL bIs2D);

static void CreateFlatPen(CPen &pen, int wid, COLORREF clr)
{
	if( wid<=1 )
		pen.CreatePen(PS_SOLID,wid,clr);
	else
	{
		LOGBRUSH lbrush = {BS_SOLID,clr,0};
		pen.CreatePen(PS_SOLID|PS_GEOMETRIC|PS_ENDCAP_FLAT,wid,&lbrush);
	}
}


extern BYTE clrTable_CAD[];


/////////////////////////////////////////////////////////////////////////////
// CBaseView

IMPLEMENT_DYNCREATE(CBaseView, CInteractiveView)

CBaseView::CBaseView():
m_vectLayL(CVectorLayer::typeGrBuffer2d),
m_vectLayR(CVectorLayer::typeGrBuffer2d),
m_markLayL(CVectorLayer::typeGrBuffer2d),
m_markLayR(CVectorLayer::typeGrBuffer2d)
{
// 	m_bAccuBoxChanged = FALSE;
// 	m_bAccuDrgChanged = FALSE;
	m_bAccuLeave = FALSE;
	
	m_clrSnap = gdef_clrSnap;
	m_bShowSnap = gdef_bShowSnap;
	m_clrFtrPtColor = gdef_clrFtrPt;
	
	m_ViewType = VIEW_ALL;
	
//	m_bRCLKSwitch = TRUE;
	m_bSymbolize = TRUE;
	m_bSymbolscolorByLayer = FALSE;
	m_bVectLayCompress = TRUE;
	
	m_bEraseGrdDrag = TRUE;
	m_bDrawGrdDrag = TRUE;
	
	memset(&m_sGridParam,0,sizeof(m_sGridParam));
	memset(m_lfMatrixLic,0,sizeof(m_lfMatrixLic));
	memset(m_lfRMatrixLic,0,sizeof(m_lfRMatrixLic));

	m_markLayL.SetDrawType(FALSE,TRUE,FALSE);
	m_markLayR.SetDrawType(FALSE,TRUE,FALSE);

	m_bShowKeyPoint = AfxGetApp()->GetProfileInt(m_strRegSection,"DisplayKeyPoint",FALSE);

	m_mapLayerVisible.InitHashTable(997);
	m_bShowSusPoint = FALSE;
	m_bShowPseudoNode = FALSE;

	m_lfMapScaleX = 1.0;
	m_lfMapScaleY = 1.0;

	m_Transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", 50);

	m_bSymbolizePoint = m_bSymbolizeCurve = m_bSymbolizeSurface = TRUE;
}

CBaseView::~CBaseView()
{	
	
}


BEGIN_MESSAGE_MAP(CBaseView, CInteractiveView)
	//{{AFX_MSG_MAP(CBaseView)
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_POPUP_SNAPCLICK, OnSnapClick)
	ON_COMMAND(ID_SELECTALL, OnSelectAll)
	ON_COMMAND(ID_VIEW_KEYPOINT, OnViewKeypoint)
	ON_UPDATE_COMMAND_UI(ID_VIEW_KEYPOINT, OnUpdateViewKeypoint)
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_TYPE_ALL, OnViewTypeAll)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TYPE_ALL, OnUpdateViewType)
	ON_COMMAND(ID_VIEW_TYPE_LOCAL, OnViewTypeLocal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TYPE_LOCAL, OnUpdateViewType)
	ON_COMMAND(ID_VIEW_TYPE_EXTERNAL, OnViewTypeExternal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TYPE_EXTERNAL, OnUpdateViewType)
	ON_COMMAND(ID_VIEW_TYPE_ALLORLOCAL, OnViewTypeAllorlocal)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TYPE_ALLORLOCAL, OnUpdateViewType)
	ON_COMMAND(ID_SELECT_POLYGON,OnSelectPolygon)
	ON_UPDATE_COMMAND_UI(ID_SELECT_POLYGON,OnUpdateSelectPolygon)
	ON_COMMAND(ID_SELECT_RECT,OnSelectRect)
	ON_UPDATE_COMMAND_UI(ID_SELECT_RECT,OnUpdateSelectRect)
	ON_COMMAND(ID_VIEW_TYPE_SHOWORHIDE, OnViewTypeShoworhide)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TYPE_SHOWORHIDE, OnUpdateViewType)
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_KEY_HSCROLL_LEFT , OnHscrollLeft)
	ON_COMMAND(ID_KEY_HSCROLL_RIGHT, OnHscrollRight)
	ON_COMMAND(ID_KEY_VSCROLL_DOWN, OnVscrollDown)
	ON_COMMAND(ID_KEY_VSCROLL_UP, OnVscrollUp)
	ON_COMMAND(ID_SWITCHMODEL,OnSwitchmodel)
	ON_WM_MOUSELEAVE()

	ON_COMMAND_RANGE(ID_ELE_START,ID_ELE_END,OnCommandType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ELE_START,ID_ELE_END,OnUpdateCommandType)

	ON_COMMAND_RANGE(ID_MOD_START,ID_MOD_END,OnCommandType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MOD_START,ID_MOD_END,OnUpdateCommandType)

	ON_COMMAND_RANGE(ID_STATE_CURVETYPE_START,ID_STATE_CURVETYPE_END,OnState_CurveType)
	ON_UPDATE_COMMAND_UI_RANGE(ID_STATE_CURVETYPE_START,ID_STATE_CURVETYPE_END,OnUpdateState_CurveType)

	ON_COMMAND_RANGE(PLUGIN_ITEMCMDID_BEGIN,PLUGIN_ITEMCMDID_END,OnPlugCommandType)
	ON_UPDATE_COMMAND_UI_RANGE(PLUGIN_ITEMCMDID_BEGIN,PLUGIN_ITEMCMDID_END,OnUpdatePlugCommandType)
	ON_COMMAND_RANGE(PLUGIN_ITEMPROCESSID_BEGIN,PLUGIN_ITEMPROCESSID_END,OnPlugFuncType)
	ON_UPDATE_COMMAND_UI_RANGE(PLUGIN_ITEMPROCESSID_BEGIN,PLUGIN_ITEMPROCESSID_END,OnUpdatePlugFuncType)
#ifndef _NOT_USE_DPWPACK
	ON_COMMAND(ID_TESTCREATE_CREATE,OnTestCreateContourCreate)
#endif
	ON_COMMAND(ID_TESTCREATE_REMOVE,OnTestCreateContourRemove)
	ON_COMMAND(IDC_BUTTON_REFRESH,OnRefreshVectoryDisplayOrder)
	ON_COMMAND(ID_VIEW_SYMBOLIZED, OnViewSymbolized)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SYMBOLIZED, OnUpdateViewSymbolized)
	ON_COMMAND(ID_WS_SETBOUND_BYDRAW, OnCommandOne)
	ON_UPDATE_COMMAND_UI(ID_WS_SETBOUND_BYDRAW, OnUpdateCommandType)
	ON_COMMAND(ID_MEASURELENGTH_PHOTO, OnCommandOne)
	ON_UPDATE_COMMAND_UI(ID_MEASURELENGTH_PHOTO, OnUpdateCommandType)
	ON_COMMAND(IDC_BUTTON_SYMCOLBYLAYER, OnSymbolscolorByLayer)
	ON_COMMAND(ID_MENU_LAYERVISIBLE,OnLayerVisible)
	ON_COMMAND(ID_BATCH_RECTMAPBORDER,OnCommandOne)
	ON_COMMAND(ID_BATCH_INCLINEDMAPBORDER,OnCommandOne)
	ON_COMMAND(ID_BATCH_ROADMAPBORDER,OnCommandOne)
	ON_COMMAND(ID_BATCH_EXPORTMAPS,OnCommandOne)
	ON_COMMAND(ID_BATCH_EXPORTMAPS_DXF,OnCommandOne)
	ON_COMMAND(ID_TOOL_INPUTPTCONVERT,OnCommandOne)
	ON_COMMAND(ID_REFFILE_CLIP,OnCommandOne)
	ON_COMMAND(ID_BATCH_TRAPZOIDMAPBORDER,OnCommandOne)
	ON_COMMAND(ID_MAPDECORATE_AUTOSETMAPNUMFROMFILE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_ALIGN,OnCommandOne)
	ON_COMMAND(ID_MAPDECORATE_CREATEXYNOTELINE,OnCommandOne)
	ON_COMMAND(ID_MAPDECORATE_CREATEXYNOTE,OnCommandOne)
	ON_COMMAND(ID_MAPDECORATE_SETNOTEANGLE_FROMLINE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_EXPLODETEXT,OnCommandOne)
	ON_COMMAND(ID_MODIFY_MODIFYPOINTSCALE,OnCommandOne)
	ON_COMMAND(ID_DEL_OVERLAPLINE,OnCommandOne)
	ON_COMMAND(ID_DEAL_SELF_INTERSECTION, OnCommandOne)
	ON_COMMAND(ID_MODIFY_SEPERATESURFACESYMBOLS,OnCommandOne)
	ON_COMMAND(ID_MODIFY_COPY_ASSISTLINE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_COPY_BASELINE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_CONVERTLAYER_SELECTION,OnCommandOne)
	ON_COMMAND(ID_MEASUREPTTOLINEDIS,OnCommandOne)//量测点到线串距离
	ON_COMMAND(ID_MODIFY_CONTOURACROSSDCURVE,OnCommandOne)//等高线遇双线断开
	ON_COMMAND(ID_MODIFY_PARTUPDATECONTOUR, OnCommandOne)//等高线遇双线断开
	ON_COMMAND(ID_MODIFY_EXTENDORTRIM,OnCommandOne)
	ON_COMMAND(ID_MODIFY_EXTENDORTRIM_ALL,OnCommandOne)
	ON_COMMAND(ID_MOD_LL_SUSPEND1,OnCommandOne)
	ON_COMMAND(ID_MOD_LL_SUSPEND2,OnCommandOne)
	ON_COMMAND(ID_MOD_PL_SUSPEND,OnCommandOne)
	ON_COMMAND(ID_TOPO_PRETREAT,OnCommandOne)
	ON_COMMAND(ID_ELEMENT_NUMBERTEXT,OnCommandOne)
    ON_COMMAND(ID_MODIFY_CURVEBOUND,OnCommandOne)
    //拓扑构面
	ON_COMMAND(ID_TOPO_SURFACE,OnCommandOne)
 	ON_COMMAND(ID_TOPO_REPLACELINES,OnCommandOne)
	ON_COMMAND(ID_TOPO_SURFACE_NOBREAK,OnCommandOne)
	ON_COMMAND(ID_TOPO_SURFACE_FROMPT,OnCommandOne)
   //删除拓扑面
    ON_COMMAND(ID_TOPO_DELETE,OnCommandOne)
	//通过面域点刷面的图层和属性
	ON_COMMAND(ID_SURFACE_FILLATT_FROMPT, OnCommandOne)
	ON_COMMAND(ID_SURFACE_NOOVERPART_TOAREA, OnCommandOne)
	ON_COMMAND(ID_SURFACE_FILLATT_FROMPT_SEL, OnCommandOne)
	ON_COMMAND(ID_SURFACE_OVERPART_TOAREA, OnCommandOne)
	ON_COMMAND(ID_SURFACE_TO_AREA, OnCommandOne)
	ON_COMMAND(ID_TOPO_SURFACE_REGION_PT, OnCommandOne)
	//合并拓扑面
	ON_COMMAND(ID_TOPO_UNION,OnCommandOne)	
	ON_COMMAND(ID_MODIFY_DELETE_DZPOINT,OnCommandOne)	
	ON_COMMAND(ID_MODIFY_DCURVEOCURVE,OnCommandOne)	
	ON_COMMAND(ID_MODIFY_PARALLELTOCURVE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_SURFACETOPOINT,OnCommandOne)	
	ON_COMMAND(ID_ELEMENT_WIDTHNOTE,OnCommandOne)	
	ON_COMMAND(ID_QT_VERTEX_AUTOMATCH, OnCommandOne)
	ON_COMMAND(ID_MAPDECORATE_CREATE_FIGURE,OnCommandOne)
	ON_UPDATE_COMMAND_UI(ID_MEASUREPTTOLINEDIS,OnUpdateCommandType)
	ON_COMMAND(ID_MEASURETHREEPTANGLE,OnCommandOne)//量测三点夹角
	ON_UPDATE_COMMAND_UI(ID_MEASURETHREEPTANGLE,OnUpdateCommandType)
	ON_COMMAND(ID_MODIFY_SETSPECIALSYMBOL,OnCommandOne)//指定独立符号
	ON_UPDATE_COMMAND_UI(ID_MODIFY_SETSPECIALSYMBOL,OnUpdateCommandType)
	ON_COMMAND(ID_DRAWTABLE,OnCommandOne)//绘制表格
	ON_UPDATE_COMMAND_UI(ID_DRAWTABLE,OnUpdateCommandType)
	ON_COMMAND(ID_CREATESYMBOLORDISCRIPTION,OnCommandOne)//生成符号或文字描述
	ON_UPDATE_COMMAND_UI(ID_CREATESYMBOLORDISCRIPTION,OnUpdateCommandType)
	ON_COMMAND(ID_TOOL_SYMBOLLAYOUT,OnCommandOne)//生成符号样图
	ON_UPDATE_COMMAND_UI(ID_TOOL_SYMBOLLAYOUT,OnUpdateCommandType)
	ON_UPDATE_COMMAND_UI(ID_MODIFY_CONTOURACROSSDCURVE,OnUpdateCommandType)

	ON_COMMAND(ID_ADJUST_CENTER_LINE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_PXATTR_TO_SURFACE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_GB2CC,OnCommandOne)
	ON_COMMAND(ID_MODIFY_CC2GB,OnCommandOne)
	ON_COMMAND(ID_TOOL_MAKE_CHECKPTSAMPLE,OnCommandOne)
	ON_COMMAND(ID_MAPDECORATE_NEW,OnCommandOne)
	ON_COMMAND(ID_MODIFY_EXTTOFIX,OnCommandOne)
	ON_COMMAND(ID_DCURVE_REVERSE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_COPYOBJ_INSIDEBRIDGE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_COPYOBJ_INSIDEBRIDGE_ONE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_INTERPOLATE_HEIPOINTS_REGION,OnCommandOne)
	ON_COMMAND(ID_ELEMENT_SURFACEPOINT,OnCommandOne)
	ON_COMMAND(ID_DELETE_PT_AROUNDCURVE,OnCommandOne)
	ON_COMMAND(ID_MODIFY_ROAD_BYSUNRULE,OnCommandOne)

	ON_COMMAND(ID_MODIFY_POINTTOSURFACEPOINT,OnCommandOne)
	ON_COMMAND(ID_MODIFY_SURFACEPOINTTOPOINT,OnCommandOne)
	ON_COMMAND(ID_MODIFY_TEXTTOSURFACEPOINT, OnCommandOne)
	ON_COMMAND(ID_MODIFY_ANNOTOSURFACEPOINT, OnCommandOne)
	ON_COMMAND(ID_MODIFY_FTRORDER_FRONT,OnCommandOne)
	ON_COMMAND(ID_MODIFY_FTRORDER_BACK,OnCommandOne)
	ON_COMMAND(ID_SELECT_LAYER,OnCommandOne)
	ON_COMMAND(ID_ELEMENT_LINE_WITH_RIGHTANG,OnCommandOne)
	ON_COMMAND(ID_MODIFY_INTERPOLATEPOINTZ_NOSELECT,OnCommandOne)
	ON_COMMAND(ID_TOPO_UNION_ALLMAP,OnCommandOne)
	ON_COMMAND(ID_MODIFY_MERGECURVE_FA,OnCommandOne)
	ON_COMMAND(ID_MODIFY_REPLACECURVE_OVERLAP,OnCommandOne)
	ON_COMMAND(ID_SURFACE_TO_DCURVE_ONE,OnCommandOne)

	ON_COMMAND(ID_SWITCH_SYMBOLIZE,OnViewSymbolized)
	ON_COMMAND(ID_SWITCH_SYMBOLIZE_POINT,OnSwitchSymbolizePoint)
	ON_COMMAND(ID_SWITCH_SYMBOLIZE_LINE,OnSwitchSymbolizeLine)
	ON_COMMAND(ID_SWITCH_SYMBOLIZE_SURFACE,OnSwitchSymbolizeSurface)
	ON_COMMAND(ID_SWITCH_SYMBOLIZE_ANNOTATION,OnSwitchSymbolizeAnnotation)
	ON_COMMAND(ID_CREATE_SURFACEPT_FROM_SURFACE,OnCommandOne)
	ON_COMMAND(ID_EXPLODE_DCURVE_TO_SURFACE,OnCommandOne)
	ON_COMMAND(ID_DCURVE_TO_TWO_CURVE, OnCommandOne)
	ON_COMMAND(ID_CHANGE_DCURVE_BY_WIDTH, OnCommandOne)
	ON_COMMAND(ID_CREATEPARTDEM, OnCommandOne)

	ON_UPDATE_COMMAND_UI(ID_SWITCH_SYMBOLIZE,OnUpdateViewSymbolized)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_SYMBOLIZE_POINT,OnUpdateSwitchSymbolizePoint)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_SYMBOLIZE_LINE,OnUpdateSwitchSymbolizeLine)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_SYMBOLIZE_SURFACE,OnUpdateSwitchSymbolizeSurface)
	ON_UPDATE_COMMAND_UI(ID_SWITCH_SYMBOLIZE_ANNOTATION,OnUpdateSwitchSymbolizeAnnotation)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseView drawing

void CBaseView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CBaseView diagnostics

#ifdef _DEBUG
void CBaseView::AssertValid() const
{
	CInteractiveView::AssertValid();
}

void CBaseView::Dump(CDumpContext& dc) const
{
	CInteractiveView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBaseView message handlers

void CBaseView::ClientToImage(PT_4D *pt0, PT_4D *pt1)
{

}

void CBaseView::ImageToClient(PT_4D *pt0, PT_4D *pt1)
{

}

void CBaseView::GroundToImage(PT_3D *pt0, PT_4D *pt1)
{

}

void CBaseView::ImageToGround(PT_4D *pt0, PT_3D *pt1)
{

}

// UINT ToolBtnIds0[]={
// 	IDC_BUTTON_SYMCOLBYLAYER
// };
// 
// UINT ToolBtnBmps0[]={
// 	IDB_BUTTON_SYMCOLBYLAYER
// };
// 
// UINT ToolButTips0[] = { 
// 	IDS_TIPS_SYMCOLBYLAYER
// };

int CBaseView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CInteractiveView::OnCreate(lpCreateStruct) == -1)
		return -1;

// 	CBitmap bmp;
// 	for( int i=0; i<sizeof(ToolBtnIds0)/sizeof(ToolBtnIds0[0]); i++)
// 	{
// 		bmp.LoadBitmap(ToolBtnBmps0[i]);
// 		HBITMAP hBmp = (HBITMAP)bmp;
// 		m_ctrlBarHorz.AddButtons(ToolBtnIds0+i,&hBmp,1);
// 		bmp.DeleteObject();
// 	}
// 	
// 	for( i=0; i<sizeof(ToolBtnIds0)/sizeof(ToolBtnIds0[0]); i++)
// 	{
// 		CMFCButton *pBtn = (CMFCButton*)m_ctrlBarHorz.GetButton(ToolBtnIds0[i]);
// 		if( pBtn )
// 		{
// 			CString str;
// 			str.LoadString(ToolButTips0[i]);
// 			pBtn->SetTooltip(str);
// 		}
//  	}

	CMenu* pSumMenu = m_muDisplay.GetSubMenu(0);
	if( pSumMenu )
	{
		pSumMenu->AppendMenu(MF_SEPARATOR);
		if (m_bSymbolscolorByLayer)
		{
			pSumMenu->AppendMenu(MF_STRING|MF_CHECKED|MF_BYCOMMAND,IDC_BUTTON_SYMCOLBYLAYER,StrFromResID(IDS_TIPS_SYMCOLBYLAYER));
		}
		else
		{
			pSumMenu->AppendMenu(MF_STRING|MF_UNCHECKED|MF_BYCOMMAND,IDC_BUTTON_SYMCOLBYLAYER,StrFromResID(IDS_TIPS_SYMCOLBYLAYER));
		}

		pSumMenu->AppendMenu(MF_STRING,ID_MENU_LAYERVISIBLE,StrFromResID(IDS_TIPS_LAYERVISIBLE));
		
		
	}

	m_menuRBD.LoadMenu(IDR_POPUP_VECTVIEW_RBD);
		
	return 0;
}

void CBaseView::OnSymbolscolorByLayer() 
{
	m_bSymbolscolorByLayer = !m_bSymbolscolorByLayer;

	CMenu* pSumMenu = m_muDisplay.GetSubMenu(0);
	if( pSumMenu )
	{
		if (m_bSymbolscolorByLayer)
		{
			pSumMenu->CheckMenuItem(IDC_BUTTON_SYMCOLBYLAYER,MF_CHECKED);
		}
		else
		{
			pSumMenu->CheckMenuItem(IDC_BUTTON_SYMCOLBYLAYER,MF_UNCHECKED);
		}
	}

	OnUpdate(NULL,hc_UpdateAllObjects,NULL);

}

void CBaseView::OnLayerVisible()
{
	CDlgLayerVisible dlg;
	dlg.m_pBaseView = this;
	dlg.DoModal();
}

void CBaseView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
// 	CPoint pt1, pt2;
// 	pt1 = GetVirtualLTCornerPosition();
	int oldPos = m_ScrollCtrl.GetPos(SB_HORZ);
	
	CInteractiveView::OnHScroll(nSBCode, nPos, pScrollBar);
//	pt2 = GetVirtualLTCornerPosition();
	int dx = oldPos - m_ScrollCtrl.GetPos(SB_HORZ);
	
	ScrollView(dx,0,0,0);

}


void CBaseView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
// 	CPoint pt1, pt2;
// 	pt1 = GetVirtualLTCornerPosition();
	int oldPos = m_ScrollCtrl.GetPos(SB_VERT);
	
	CInteractiveView::OnVScroll(nSBCode, nPos, pScrollBar);
//	pt2 = GetVirtualLTCornerPosition();	
	int dy = oldPos - m_ScrollCtrl.GetPos(SB_VERT);
	ScrollView(0,dy,0,0);
}

void CBaseView::OnHscrollLeft()
{
	UINT	nSBCode = SB_PAGELEFT;
	UINT	nPos = 0;
	OnHScroll(nSBCode, nPos, NULL);
}

void CBaseView::OnHscrollRight()
{
	UINT	nSBCode = SB_PAGERIGHT;
	UINT	nPos = 0;
	OnHScroll(nSBCode, nPos, NULL);
}

void CBaseView::OnVscrollDown()
{
	UINT	nSBCode = SB_PAGEDOWN;
	UINT	nPos = 0;
	OnVScroll(nSBCode, nPos, NULL);
}

void CBaseView::OnVscrollUp()
{
	UINT	nSBCode = SB_PAGEUP;
    UINT	nPos = 0;
	OnVScroll(nSBCode, nPos, NULL);
}

void CBaseView::OnSwitchmodel() 
{
	AfxGetMainWnd()->SendMessage(FCCM_SWITCHSTEREOMS,0,0);	
}

void CBaseView::OnPreInnerCmd(UINT nInnerCmd)
{
	CInteractiveView::OnPreInnerCmd(nInnerCmd);
	if( m_nInnerCmd==IDC_BUTTON_REFRESH )
	{
		m_markLayL.ClearAll();
		m_markLayR.ClearAll();
		m_vectLayL.ClearAll();
		m_vectLayR.ClearAll();
	}
}

void CBaseView::OnPostInnerCmd()
{
	if (m_nInnerCmd == IDC_BUTTON_HANDMOVE)
	{
		CDlgDoc *pDoc = GetDocument();
		if( pDoc->m_accuBox.IsOpen() && !m_bAccuLeave && pDoc->m_accuBox.IsActive() )
		{
			pDoc->m_accuBox.SetReDrawFlag(TRUE);
			//return;
		}
	}
	else if (m_nInnerCmd == IDC_BUTTON_ZOOMRECT)
	{
		if( m_bUseSaveCursor && m_hSaveCursor )
			SetCursor(m_hSaveCursor);
		m_bUseSaveCursor = FALSE;
		m_hSaveCursor = NULL;
	}

	CInteractiveView::OnPostInnerCmd();
}

#include "GeoText.h"

BOOL CBaseView::LoadDocData()
{
	if( !m_vectLayL.IsValid() )
		return FALSE;

	m_vectLayL.DelAllObjs();
	m_vectLayR.DelAllObjs();
	m_markLayL.DelAllObjs();
	m_markLayR.DelAllObjs();
	
	CDlgDoc *pDoc = GetDocument();
	
	int count = pDoc->GetDlgDataSourceCount();

	CDlgDataSource* pDS = NULL;
	//获取实体对象总数
	long lSum = 0;
	for( int k=0; k<count; k++)	
	{
		pDS = pDoc->GetDlgDataSource(k);
		int nlaynum = pDS->GetFtrLayerCount();
		for( int i=0; i<nlaynum; i++ )
		{
			CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
			if( pLayer )
			{
				lSum += pLayer->GetObjectCount();
			}
		}
	}
	
	GOutPut(StrFromResID(IDS_VIEW_VECTOR));
	GProgressStart(lSum);
	BOOL bEnable;
	COLORREF monoClr;
	for( k=0; k<count; k++)
	{
		pDS = pDoc->GetDlgDataSource(k);
		pDS->GetMonoColor(&bEnable,&monoClr);
		CFtrLayerArray arrLayers;
		pDS->GetAllFtrLayersByDisplayOrder(arrLayers);
		int laynum = arrLayers.GetSize();
		if( laynum<=0 )continue;
		
		CFeature *pFtr;
//		CFtrLayer *pCurLayer = pDoc->GetDlgDataSource()->GetFtrLayer();		

		for( int i=0; i<laynum; i++)
		{
			CFtrLayer *pLayer = arrLayers[i];
			if( !pLayer )
				continue;
			if( pLayer->IsDeleted() /*|| !pLayer->IsVisible()*/ )
				continue;

			if( pLayer )
			{
				BOOL bViewValue = TRUE;
				m_mapLayerVisible.Lookup(pLayer,bViewValue);
				
				bViewValue &= pLayer->IsVisible();

 				int objnum = pLayer->GetObjectCount();
				for( int j=0; j<objnum; j++ )
				{
					//增长进度条

					GProgressStep();
					
					pFtr = pLayer->GetObject(j);
										
					if( pFtr )
					{						
						if( CanSymbolized(pFtr) && pLayer->IsSymbolized())
						{							
							GrBuffer buf,buf0;
							if (pDS)
							{
								if((!pDS->DrawFeature(pFtr,&buf,pFtr->GetGeometry()->IsSymbolizeText(),m_gsa.lfAngle)&&pFtr->GetGeometry()->GetClassType()==CLS_GEOTEXT)||!CAnnotation::m_bUpwardText)
									;//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
								else if(CAnnotation::m_bUpwardText)
								{
									buf.RotateGrTextEnvelope(m_gsa.lfAngle);
								}
								if (bEnable)
								{
									buf.SetAllColor(monoClr);
								}
								else if(pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER || m_bSymbolscolorByLayer)
								{
									buf.SetAllColor(pLayer->GetColor());
								}
							}
							AddObjtoVectLay((LONG_PTR)pFtr, &buf);
						}
						else
						{
							GrBuffer buf;
							pFtr->Draw(&buf,pDS->GetSymbolDrawScale());
							if (bEnable)
							{
								buf.SetAllColor(monoClr);
							}								
							else if(pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER || m_bSymbolscolorByLayer)
							{
								buf.SetAllColor(pLayer->GetColor());
							}						
							if (pFtr->GetGeometry()->GetClassType()==CLS_GEOTEXT)
							{
								//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
							}							
							AddObjtoVectLay((LONG_PTR)pFtr, &buf);
						}

 						if(!bViewValue || !pFtr->IsVisible())
						{
							m_vectLayL.SetObjVisible((LONG_PTR)pFtr, FALSE);
							m_vectLayR.SetObjVisible((LONG_PTR)pFtr, FALSE);
						}
 						AddObjtoMarkVectLay(pFtr);						

					}					
				}				
			}
		}
	}
	
	//进度条复位
	GProgressEnd();
	GOutPut(StrFromResID(IDS_VIEW_VECTOREND));

	return TRUE;
}

BOOL CBaseView::InitBmpLayer()
{
	int imgbufsize = 7;
	int vctbufsize = 7;	
	
	m_vectLayL.SetContext(m_pContext->GetLeftContext());
	m_markLayL.SetContext(m_pContext->GetLeftContext());
	m_vectLayR.SetContext(m_pContext->GetRightContext());
	m_markLayR.SetContext(m_pContext->GetRightContext());
	
	m_snapLayL.SetContext(m_pContext);
	m_snapLayR.SetContext(m_pContext);
	
	m_accurboxLayL.SetContext(m_pContext);
	m_accurboxLayR.SetContext(m_pContext);
	
	m_vectLayL.Destroy();
	m_vectLayR.Destroy();
	m_markLayL.Destroy();
	m_markLayR.Destroy();
	
	if( !m_vectLayL.InitCache( GetBestCacheTypeForLayer(&m_vectLayL),
		CSize(512,512),vctbufsize*vctbufsize ) )
		return FALSE;

	if( !m_vectLayR.InitCache( GetBestCacheTypeForLayer(&m_vectLayR),
		CSize(512,512),vctbufsize*vctbufsize ) )
		return FALSE;
	
	if( !m_markLayL.InitCache( GetBestCacheTypeForLayer(&m_markLayL),
		CSize(512,512),vctbufsize*vctbufsize ) )
		return FALSE;
	
	if( !m_markLayR.InitCache( GetBestCacheTypeForLayer(&m_markLayR),
		CSize(512,512),vctbufsize*vctbufsize ) )
		return FALSE;
	
	m_snapLayL.m_nEraseType = CVariantDragLayer::eraseBMP;
	m_snapLayR.m_nEraseType = CVariantDragLayer::eraseBMP;
	m_snapLayL.m_bUseCoordSys = FALSE;
	m_snapLayR.m_bUseCoordSys = FALSE;
	
	m_accurboxLayL.m_nEraseType = CVariantDragLayer::eraseXOR;
	m_accurboxLayR.m_nEraseType = CVariantDragLayer::eraseXOR;
	m_accurboxLayL.m_bUseCoordSys = FALSE;
	m_accurboxLayR.m_bUseCoordSys = FALSE;
	// 	BOOL bRoundLine  = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_ROUNDLINE,FALSE);
	// 	if( bRoundLine )CXVElementBank::SetEndCapType(PS_ENDCAP_ROUND);
	// 	else CXVElementBank::SetEndCapType(PS_ENDCAP_FLAT);

	m_markLayL.SetAllObjsMark2(m_bShowKeyPoint);
	m_markLayR.SetAllObjsMark2(m_bShowKeyPoint);

	Envelope dataBound = GetDocument()->GetDlgDataSource()->GetBound();	
	m_vectLayL.SetDataBound(dataBound);
	m_vectLayR.SetDataBound(dataBound);
	m_markLayL.SetDataBound(dataBound);
	m_markLayR.SetDataBound(dataBound);
	return TRUE;
}

CDlgDoc* CBaseView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDlgDoc)));
	return (CDlgDoc*)m_pDocument;
}

void CBaseView::ClearAccuDrawing()
{	
	m_accurboxLayL.SetBuffer((GrBuffer2d*)NULL);
	m_accurboxLayR.SetBuffer((GrBuffer2d*)NULL);
	
	UpdateDrawingLayers(&m_accurboxLayL,&m_accurboxLayR);
}


void CBaseView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CDlgDoc *pDoc = GetDocument();
	CDlgDataSource *pDS=pDoc->GetDlgDataSource();
	switch( lHint )
	{
	case hc_Attach_Accubox:
		pDoc->m_accuBox.Reset();
		pDoc->m_accuBox.Activate(TRUE);
		break;
	case hc_Detach_Accubox:
		ClearAccuDrawing();
		pDoc->m_accuBox.Reset();
		pDoc->m_accuBox.Activate(FALSE);
		break;
	case hc_Update_Accubox:
// 		m_bAccuBoxChanged = TRUE;
// 		m_bAccuDrgChanged = TRUE;
		UpdateAccuDrawing();
		break;
	case hc_SelChanged:
		UpdateSelDrawing(pHint!=0);
		return;
	case hc_UpdateGrid:
		{
			UpdateGridDrawing();
			Invalidate(FALSE);
			
		}
		break;
	case hc_AddGraph:
		if( pHint )
		{
			AddObjtoVectLay(HANDLE_GRAPH,(const GrBuffer*)pHint);
			
// 			CGrDrawer2d *pDrawL, *pDrawR;
// 			pDrawL = m_vectLayL.GetDrawer();
// 			pDrawR = m_vectLayR.GetDrawer();
// //  			if( pBankL )pBankL->ResetUpdateInfo();
// //  			if( pBankR )pBankL->ResetUpdateInfo();
// 			if( pDrawL )pDrawL->Delete(HANDLE_GRID);
// 			if( pDrawR )pDrawR->Delete(HANDLE_GRID);

			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
			Invalidate(FALSE);
		}
		break;
	case hc_DelGraph:
		DelObjfromVectLay(HANDLE_GRAPH);
// 		m_vectLayL.UpdateVector();
// 		m_vectLayR.UpdateVector();
		Invalidate(FALSE);
		break;
	case hc_AddObject:
		{
			CFeature *pFtr = (CFeature*)pHint;
			if( !pFtr )break;

			CFtrLayer *pLayer = pDS->GetFtrLayerOfObject(pFtr);
			if (!pLayer) break;
			BOOL bEnable;
			COLORREF monoClr;
			pDS->GetMonoColor(&bEnable,&monoClr);
			GrBuffer buf;
			if( CanSymbolized(pFtr) && pLayer->IsSymbolized() )
			{							
				if (pDS)
				{
					if((!pDS->DrawFeature(pFtr,&buf,pFtr->GetGeometry()->IsSymbolizeText(),m_gsa.lfAngle)&&pFtr->GetGeometry()->GetClassType()==CLS_GEOTEXT)||!CAnnotation::m_bUpwardText)
						;//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
					else if(CAnnotation::m_bUpwardText)
					{
						buf.RotateGrTextEnvelope(m_gsa.lfAngle);
					}
				}
			}
			else
			{
				pFtr->Draw(&buf,pDS->GetSymbolDrawScale());
				//if (pFtr->GetGeometry()->GetClassType()==CLS_GEOTEXT)
				//buf.RotateGrTextPar(m_gsa.lfAngle*180/PI);
			}

			if (bEnable)
			{
				buf.SetAllColor(monoClr);
			}
			else if (pLayer && (pFtr->GetGeometry()->GetColor() == FTRCOLOR_BYLAYER || m_bSymbolscolorByLayer))
			{
				buf.SetAllColor(pLayer->GetColor());
			}

			AddObjtoVectLay((LONG_PTR)pFtr, &buf);
			if((pLayer && !pLayer->IsVisible()) || !pFtr->IsVisible())
			{
				m_vectLayL.SetObjVisible((LONG_PTR)pFtr, FALSE);
				m_vectLayR.SetObjVisible((LONG_PTR)pFtr, FALSE);
			}

			AddObjtoMarkVectLay(pFtr);
		
		}
		return;
	case hc_DelObject:
		{
			CFeature *pFtr = (CFeature*)pHint;
			if( !pFtr )break;
			//EraseObject(pObj);
			DelObjfromVectLay((LONG_PTR)pFtr);
			DelObjfromVectLay((LONG_PTR)pFtr, &m_markLayL, &m_markLayR);
			
			//针对大量更新的优化
			//	SetViewUpdateFlags(VIEWUPDATE_ADDORDELOBJ);
		}
		return;
	case hc_UpdateOption:
		{
			int value = AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_SELECTSIZE,gdef_nSelectSize);
			SetCrossSelMarkWid(value);
			
			m_clrSnap = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SNAPCOLOR,m_clrSnap);

			m_clrFtrPtColor = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_FEATUREPTCOLOR,m_clrFtrPtColor);
			m_markLayL.SetFtrPtColor(m_clrFtrPtColor);
			m_markLayR.SetFtrPtColor(m_clrFtrPtColor);

			m_bShowSnap = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SHOWSNAP,m_bShowSnap);
			
			m_bRCLKSwitch = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RCLKSWITCH,m_bRCLKSwitch);

			CSymbol::m_bBreakCell = FALSE; //AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGITEM_DISPLAYFAST,FALSE);

			m_nDragLineWid = AfxGetApp()->GetProfileInt(REGPATH_VIEW_VECT,REGITEM_DRAGLINE_WID,m_nDragLineWid);

			m_Transparency = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,"Transparency", m_Transparency);
		}
		break;
	case hc_UpdateLayerDisplay:
		{
			UpdateDispyParam *pParam = (UpdateDispyParam*)pHint;
			if( !pParam )break;
			CFtrLayer *pLayer = ((CFtrLayer *)pParam->handle);
			if (!pLayer)
			{
				return;
			}
			if(pParam->type==UpdateDispyParam::typeVISIBLE)
			{
				BOOL bViewValue = TRUE;
				m_mapLayerVisible.Lookup(pLayer,bViewValue);

				bViewValue &= pLayer->IsVisible();				

				for (int i=0;i<pLayer->GetObjectCount();i++)
				{
					m_vectLayL.SetObjVisible((LONG_PTR)(pLayer->GetObject(i)),bViewValue);
					m_vectLayR.SetObjVisible((LONG_PTR)(pLayer->GetObject(i)),bViewValue);
				}
				
			}
			if (pParam->type==UpdateDispyParam::typeCOLOR)
			{
				for (int i=0;i<pLayer->GetObjectCount();i++)
				{
					CPFeature pFtr = pLayer->GetObject(i);
					if (pFtr && ( (pFtr->GetGeometry() && pFtr->GetGeometry()->GetColor() == FTRCOLOR_BYLAYER) || m_bSymbolscolorByLayer) )
					{
						m_vectLayL.SetObjColor((LONG_PTR)(pFtr),pParam->data.lColor);
						m_vectLayR.SetObjColor((LONG_PTR)(pFtr),pParam->data.lColor);
					}
				}				
			}
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
		}	
		break;
	case hc_UpdateObjectDisplay:
		{
			UpdateDispyParam *pParam = (UpdateDispyParam*)pHint;
			if( !pParam )break;
			CFeature *pFtr = ((CFeature*)pParam->handle);
			if (!pFtr)
			{
				return;
			}
			if(pParam->type==UpdateDispyParam::typeVISIBLE)
			{
				m_vectLayL.SetObjVisible((LONG_PTR)pFtr, pParam->data.bVisible);
				m_vectLayR.SetObjVisible((LONG_PTR)pFtr, pParam->data.bVisible);
			}
			if (pParam->type==UpdateDispyParam::typeCOLOR)
			{
				m_vectLayL.SetObjColor((LONG_PTR)(pFtr), pParam->data.lColor);
				m_vectLayR.SetObjColor((LONG_PTR)(pFtr), pParam->data.lColor);
			}
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
		}	
		break;
	case hc_UpdateWindow:
		FinishUpdateDrawing();
		UpdateWindow();
		break;
	case hc_UpdateLayerDisplayOrder:
		{
			CFtrLayerArray arrLayers;
			pDS->GetAllFtrLayersByDisplayOrder(arrLayers);
			int num = arrLayers.GetSize();

			for (int i=0; i<num; i++)
			{
				CFtrLayer *pLayer = arrLayers[i];
				if (!pLayer) continue;

				for (int j=0; j<pLayer->GetObjectCount(); j++)
				{
					CFeature *pFtr = pLayer->GetObject(j);
					if (!pFtr) continue;

					m_vectLayL.MoveToTail(LONG_PTR(pFtr));
					m_vectLayR.MoveToTail(LONG_PTR(pFtr));
				}
			}
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
			Invalidate(FALSE);
		}
		break;
	case hc_UpdateDataColor:
		{
			CDlgDataSource *pDS = (CDlgDataSource*)pHint;
			if( !pDS )break;			
			CFtrLayer* pLayer = NULL;
			CFeature *pFtr = NULL;
			int j = 0;
			BOOL bEnable;
			COLORREF clrMono;
			COLORREF clrFtr;
			pDS->GetMonoColor(&bEnable,&clrMono);
			for (int i=0;i<pDS->GetFtrLayerCount();i++)
			{
				pLayer = pDS->GetFtrLayerByIndex(i);
				if (!pLayer||pLayer->IsDeleted()) continue;		
				for(j=0;j<pLayer->GetObjectCount();j++)
				{
					pFtr = pLayer->GetObject(j);
					if (pFtr)
					{
						if(pFtr->GetGeometry()->GetColor() == FTRCOLOR_BYLAYER || m_bSymbolscolorByLayer)
						{
							clrFtr = pLayer->GetColor();
						}
						else
							clrFtr = pFtr->GetGeometry()->GetColor();
						m_vectLayL.SetObjColor(LONG_PTR(pFtr), bEnable ? clrMono : clrFtr);
						m_vectLayR.SetObjColor(LONG_PTR(pFtr), bEnable ? clrMono : clrFtr);
					}
				}
			}
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
			Invalidate(FALSE);
		}
		break;
	case hc_AddSusPtColor:
		if( pHint )
		{
			AddObjtoVectLay(HANDLE_SUSPT,(const GrBuffer*)pHint);	
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
			Invalidate(FALSE);
		}
		break;
	case hc_DelSusPtColor:
		DelObjfromVectLay(HANDLE_SUSPT);		
		Invalidate(FALSE);
		break;
	case hc_AddPseudoPtColor:
		if( pHint )
		{
			AddObjtoVectLay(HANDLE_PSEPT,(const GrBuffer*)pHint);	
			m_vectLayL.ClearAll();
			m_vectLayR.ClearAll();
			Invalidate(FALSE);
		}
		break;
	case hc_DelPseudoPtColor:
		DelObjfromVectLay(HANDLE_PSEPT);		
		Invalidate(FALSE);
		break;
	default:
		CInteractiveView::OnUpdate( pSender, lHint, pHint);
		break;
	}
	
}


void CBaseView::ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer2d *outL, GrBuffer2d *outR)
{
	const GrBuffer *pBuf = in;

	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;
	
	GrBuffer2d* pBufL=outL;
	GrBuffer2d* pBufR=outR;

	GrBuffer2d bufL1,bufR1;

	bufL1.AddBuffer(pBuf);
	if( !IsSingle() )bufR1.AddBuffer(pBuf);
	
	float lxoff,lyoff;
	float rxoff,ryoff;
	
	PT_3D pt0;
	PT_4D pt1;

	pt0 = pBuf->GetFirstPt();
	GroundToVectorLay( &pt0, &pt1 );

	//定义原点
	if( pBufL )
	{
		if( !pBufL->HeadGraph() )
		{
			lxoff = floor(pt1.x/1e+0)*1e+0;
			lyoff = floor(pt1.y/1e+0)*1e+0;
			
			pBufL->SetOrigin(lxoff,lyoff);
			bufL1.SetOrigin(lxoff,lyoff);
		}
		else
		{		
			pBufL->GetOrigin(lxoff,lyoff);
			bufL1.SetOrigin(lxoff,lyoff);
		}
	}

	if( pBufR )
	{
		if( !pBufR->HeadGraph() )
		{		
			rxoff = floor(pt1.z/1e+0)*1e+0;
			ryoff = floor(pt1.yr/1e+0)*1e+0;
			
			pBufR->SetOrigin(rxoff,ryoff);
			bufR1.SetOrigin(rxoff,ryoff);
		}
		else
		{
			pBufR->GetOrigin(rxoff,ryoff);
			bufR1.SetOrigin(rxoff,ryoff);
		}
	}

	//遍历坐标点 并转换坐标
	
	const GrVertexList *pList;
	GrVertex2d *grpt;
	
	const Graph *gr = pBuf->HeadGraph();
	if(!gr) return;
	
	Graph2d *grl=bufL1.HeadGraph();
	Graph2d *grr=bufR1.HeadGraph();

	while(gr)
	{
		if (IsGrPoint(gr))
		{
			const GrPoint *cgr = (const GrPoint*)gr;

			pt0 = cgr->pt;
			GroundToVectorLay( &pt0, &pt1 );
			if (grl)
			{
				((GrPoint2d*)grl)->pt.x = pt1.x-lxoff;
				((GrPoint2d*)grl)->pt.y = pt1.y-lyoff;
//				((GrPoint2d*)grl)->angle -= m_gsa.lfAngle;

// 				((GrPoint2d*)grl)->kx *= m_gsa.lfGKX;
// 				((GrPoint2d*)grl)->ky *= m_gsa.lfGKY;
			}
			if (grr)
			{
				((GrPoint2d*)grr)->pt.x = pt1.z-rxoff;
				((GrPoint2d*)grr)->pt.y = pt1.yr-ryoff;
//				((GrPoint2d*)grr)->angle -= m_gsa.lfAngle;

// 				((GrPoint2d*)grr)->kx *= m_gsa.lfGKX;
// 				((GrPoint2d*)grr)->ky *= m_gsa.lfGKY;
			}
		
		}
		else if (IsGrPointString(gr))
		{
			const GrPointString *cgr = (const GrPointString*)gr;
			GrPointString2d *grlLine = NULL;
			GrPointString2d *grrLine = NULL;
			if(grl)grlLine = (GrPointString2d *)grl;
			if(grr)grrLine = (GrPointString2d *)grr;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrLine)
				{
					grpt = &((grrLine->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
			}
		}
		else if (IsGrLineString(gr))
		{
			const GrLineString *cgr = (const GrLineString*)gr;
			GrLineString2d *grlLine = NULL;
			GrLineString2d *grrLine = NULL;
			if(grl)grlLine = (GrLineString2d *)grl;
			if(grr)grrLine = (GrLineString2d *)grr;

			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlLine)
				{
					grpt = &((grlLine->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrLine)
				{
					grpt = &((grrLine->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
				
			}		
		}
		else if (IsGrPolygon(gr))
		{
			const GrPolygon *cgr = (const GrPolygon*)gr;
			GrPolygon2d *grlPlg = NULL;
			GrPolygon2d *grrPlg = NULL;
			if(grl)grlPlg = (GrPolygon2d *)grl;
			if(grr)grrPlg = (GrPolygon2d *)grr;
			
			pList = &(cgr->ptlist);
			for (int i=0;i<pList->nuse;i++)
			{
				pt0 = pList->pts[i];
				GroundToVectorLay( &pt0, &pt1 );
				if (grlPlg)
				{
					grpt = &((grlPlg->ptlist.pts)[i]);
					grpt->x = pt1.x-lxoff;
					grpt->y = pt1.y-lyoff;
				}
				if (grrPlg)
				{
					grpt = &((grrPlg->ptlist.pts)[i]);
					grpt->x = pt1.z-rxoff;
					grpt->y = pt1.yr-ryoff;
				}
			
			}		
		}
		else
		{
			const GrText *cgr = (const GrText*)gr;			
			pt0 = cgr->pt;
			GroundToVectorLay( &pt0, &pt1 );
			PT_4D pt4d[4];
			for (int i=0;i<4;i++)
			{
				pt0.x = cgr->x[i]+cgr->pt.x;
				pt0.y = cgr->y[i]+cgr->pt.y;
				pt0.z = cgr->pt.z;
				GroundToVectorLay(&pt0,&(pt4d[i]));
			}		
			if (grl)
			{
				((GrText2d*)grl)->pt.x = pt1.x-lxoff;
				((GrText2d*)grl)->pt.y = pt1.y-lyoff;
				for (int i=0;i<4;i++)
				{
					((GrText2d*)grl)->x[i] = pt4d[i].x-pt1.x;
					((GrText2d*)grl)->y[i] = pt4d[i].y-pt1.y;
				}
			}
			if (grr)
			{
				((GrText2d*)grr)->pt.x = pt1.z-rxoff;
				((GrText2d*)grr)->pt.y = pt1.yr-ryoff;
				for (int i=0;i<4;i++)
				{
					((GrText2d*)grr)->x[i] = pt4d[i].z-pt1.z;
					((GrText2d*)grr)->y[i] = pt4d[i].yr-pt1.yr;
				}
			}

		}
		gr = gr->next;
		if(grl)grl = grl->next;
		if(grr)grr = grr->next;
	}

	if(pBufL)
	{
		pBufL->AddBuffer(&bufL1);
		pBufL->RefreshEnvelope(TRUE);
	}
	if(pBufR)
	{
		pBufR->AddBuffer(&bufR1);
		pBufR->RefreshEnvelope(TRUE);
	}
}

void CBaseView::AddObjtoVectLay(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround, CVectorLayer *pLL, CVectorLayer *pLR)
{
	if( !pBuf )return;
	if( !pBuf->HeadGraph() )
		return;

	if( !pLL )pLL = &m_vectLayL;
	if( !pLR )pLR = &m_vectLayR;
	
	GrBuffer2d* pBufL=NULL;
	GrBuffer2d* pBufR=NULL;
	
	pBufL = (GrBuffer2d*)pLL->OpenObj(handle);
	if( !IsSingle() )pBufR = (GrBuffer2d*)pLR->OpenObj(handle);

	ConvertGrBufferToVectLayer(pBuf,pBufL,pBufR);
	
 	pLL->ClearAll();
 	pLR->ClearAll();

	pLL->FinishObj(handle);
	if( !IsSingle() )pLR->FinishObj(handle);
}

void CBaseView::GroundToVectorLay(PT_3D *pt0, PT_4D *pt1)
{
	
}

void CBaseView::AddObjtoMarkVectLay(CFeature *pFtr)
{
	ASSERT(pFtr!=NULL);

	CGeometry * pObj = pFtr->GetGeometry();
	if (!pObj) return;	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	int i;	
	PT_3D pt0;
	PT_4D pt1;
	pObj->GetShape(arrPts);
	if (arrPts.GetSize()<1)
	{
		return;
	}
	GrBuffer2d *pBufL = (GrBuffer2d*)m_markLayL.OpenObj((LONG_PTR)pFtr);
	GrBuffer2d *pBufR = IsSingle() ? NULL : (GrBuffer2d*)m_markLayR.OpenObj((LONG_PTR)pFtr);
	
	COPY_3DPT(pt0,(arrPts[0]));
	GroundToVectorLay(&pt0,&pt1);

	float lxoff = floor(pt1.x/1e+0)*1e+0, lyoff = floor(pt1.y/1e+0)*1e+0;
	float rxoff = floor(pt1.z/1e+0)*1e+0, ryoff = floor(pt1.yr/1e+0)*1e+0;

	PT_2D pt2d0,pt2d1;
	if (pBufL)
	{
		pBufL->SetOrigin(lxoff,lyoff);
		pBufL->BeginLineString(RGB(255,255,255),1);
		pt2d0.x = pt1.x;
		pt2d0.y = pt1.y;
		pBufL->MoveTo(&pt2d0);
	}
	if (pBufR)
	{
		pBufR->SetOrigin(rxoff,ryoff);
		pBufR->BeginLineString(RGB(255,255,255),1);
		pt2d1.x = pt1.z;
		pt2d1.y = pt1.yr;
		pBufR->MoveTo(&pt2d1);
	}
		
	for (i=1;i<arrPts.GetSize();i++)
	{	
		COPY_3DPT(pt0,(arrPts[i]));
		GroundToVectorLay(&pt0,&pt1);
		if (pBufL)
		{
			pt2d0.x = pt1.x;
			pt2d0.y = pt1.y;
			pBufL->LineTo(&pt2d0);
		}
		if (pBufR)
		{
			pt2d1.x = pt1.z;
			pt2d1.y = pt1.yr;
			pBufR->LineTo(&pt2d1);
		}		
	}	
	
	int infos[3] = {-1,-1,-1};
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)) )
	{
		infos[0] = -1;
		infos[1] = GRPT_MARK_DIAMOND;
		infos[2] = GRPT_MARK_DIAMOND;
	}

	for (i=0;i<pObj->GetCtrlPointSum();i++)
	{
		pt0 = pObj->GetCtrlPoint(i);
		GroundToVectorLay(&pt0,&pt1);
		if (pBufL)
		{
			pt2d0.x = pt1.x;
			pt2d0.y = pt1.y;
			pBufL->LineTo(&pt2d0,infos[i]);
		}
		if (pBufR)
		{
			pt2d1.x = pt1.z;
			pt2d1.y = pt1.yr;
			pBufR->LineTo(&pt2d1,infos[i]);
		}		
	}	
	pBufL->End();
	if( pBufR )pBufR->End();

	m_markLayL.SetObjMark2((LONG_PTR)pFtr,m_bShowKeyPoint);
	if (pBufR)m_markLayR.SetObjMark2((LONG_PTR)pFtr, m_bShowKeyPoint);

}

void CBaseView::DelObjfromVectLay(LONG_PTR handle, CVectorLayer *pLL, CVectorLayer *pLR)
{
	if( !pLL )pLL = &m_vectLayL;
	if( !pLR )pLR = &m_vectLayR;

	pLL->DelObj(handle);
	pLR->DelObj(handle);
	
	if (pLL) pLL->ClearAll();
	if (pLR) pLR->ClearAll();
}


void CBaseView::OnInitialUpdate() 
{
// 	m_pContext->CreateD3DContext(GetSafeHwnd());
// 	m_pContext->EnableD3D(TRUE);
// 	m_pContext->OnCreateD3D();

	//VM_START

	//LoadViewPlace(this,FALSE);
	CInteractiveView::OnInitialUpdate();

	if(((CEditBaseApp*)AfxGetApp())->IsSupportPlugs())
		RegisterPlugsInSelectCmds();

	GetWorker()->RegisterSelectStateCommand(CDlgEditVertexCommand::Create);
	GetWorker()->RegisterSelectStateCommand(CDlgInsertVertexCommand::Create);

	_UpdateMatrix(m_lfMatrixLic,m_lfRMatrixLic,1);

	DWORD dwScale = ((CDlgDoc*)GetWorker())->GetDlgDataSource()->GetScale();
	//考虑显示设备的Ppi ,1英寸=2.54cm
	int nXPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSX);
	int nYPpi = ::GetDeviceCaps(m_hViewDC,LOGPIXELSY);
	//当前比例尺下1m对应多少像素
	m_lfMapScaleX = ((100.0*nXPpi)/(dwScale*2.54));
	m_lfMapScaleY = ((100.0*nYPpi)/(dwScale*2.54));

	OnUpdate(NULL,hc_UpdateOption,NULL);
	InitBmpLayer();	

	PT_3D pts[4];
/*	Envelope e = */
	((CDlgDoc*)GetWorker())->GetDlgDataSource()->GetBound(pts,NULL,NULL);
	OnUpdate(NULL,hc_SetMapArea,(CObject*)(pts));
	
 	SetTimer(TIMERID_UPDATEVIEW,100,NULL);
 	SetTimer(TIMERID_SIMPLIFY_UPDATEVIEW,20,NULL);

	REG_CMDCLASS(GetWorker(),ID_ELEMENT_DOT_DOT,CDlgDrawPointCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_DOT_VECTORDOT,CDrawDirPointCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_DOT_AUTOHEIGHT,CDrawAutoHeightPointCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_WITH_RIGHTANG,CDlgDrawCurveCommand_HuNan);
	
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_PARALLELOGRAM,CDrawParallelogramCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_LINE,CDlgDrawCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_DOT_HEIGHTDOT,CDrawDHeightPointCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_DLINE,CDrawDCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_REGULAR_POLYGON,CDrawRegularPolygonCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_PARALLEL,CDrawParalCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_FACE_FACE,CDlgDrawSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MOVE,CDlgMoveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYVERTEX,CDlgEditVertexCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INSERTVERTEX,CDlgInsertVertexCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DELETEVERTEX,CDeleteVertexCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DEL,CDeleteCommand);	
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CONVERTLAYER,CFCodeChgAllCommand);	
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MOVETOLOCAL,CMovetoLocalCommand);	
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPY,CCopyCommand);	
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SEPARATE,CSeparateParallelCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_ROTATE,CRotateCommand);
	REG_CMDCLASS(GetWorker(),ID_MIDIFY_REVERSE,CReverseCommand);	
	REG_CMDCLASS(GetWorker(),ID_MODIFY_LINESERIAL,CLinearizeCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_BREAK,CBreakCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYLINE,CCopyLineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_RECTIFY,CRectifyCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COMBINE,CCombineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_PARALLELMOVE,CParallelMoveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_FTRPLACEORDER,CPlaceOrderCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_FTRORDER_FRONT,CPlaceOrderFrontCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_FTRORDER_BACK,CPlaceOrderBackCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_PXATTR_TO_SURFACE,CCopyXAttrOfPointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_GB2CC,CTransformGB2CCCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CC2GB,CTransformCC2GBCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SURFACETOCURVE,CModifySurfaceToCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CURVETOSURFACE,CModifyCurveToSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_RESETBASEATTRIBUTION,CResetBaseAttributionCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DELETECOMMONSIDE,CDeleteCommonSideCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYZ,CModifyZCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COLOR, CModifyColorCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYINTERSCCTION,CModifyIntersectionCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYCHAMFER,CChamferCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTZ,CIntZCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CHANGECONNECTCODE,CConnectCodeChgCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CLOSE,CCloseCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CLOSETOLE,CCloseCommandWithTole);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INSERTINTERSECT,CIntersectCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFI_LAYERINSERTINTERSEC,CLayerIntersectCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MIRROR,CMirrorCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYWITHROTATION,CCopyWithRotationCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_EXTEND,CExtendCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CUTPART,CCutPartCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_REPLACECURVE,CReplaceLinesCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_REPLACECURVE_OVERLAP,CReplaceLinesCommand_overlap);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_EXTENDCOLLECT,CExtendCollectCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_RECT,CDrawRectCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_THREEDOT,CDrawCircleThreePtCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_CIRCLE_TWODOT,CDrawCircleTwoPtCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_CIRCLE_CENTERRADIUS,CDrawCircleCenterRadiusCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_BYSIDE,CDrawLinesBySide);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_TEXT,CDrawTextCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COMPRESS,CCompressCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SMOOTHCURVES,CSmoothCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_FORMATOBJ,CFormatFtrCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_TRIM,CTrimCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_TRIMLAY,CTrimLayerCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_REPLACECURVECATCHINTERSEC,CReplaceLinesWithCatchIntersecCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_LINKCONTOUR,CLinkContourCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_AUTOLINKCONTOUR,CAutoLinkContourCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPCONTOUR,CInterpolateCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DIVIDECURVE,CDivideCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CONTINUEINTERPCONTOUR,CContinueInterpolateCommand);
	REG_CMDCLASS(GetWorker(),ID_SHOWDIR,CShowDirCommand);
	REG_CMDCLASS(GetWorker(),ID_MEASUREDIS,CMeasureDistanceCommand);
	REG_CMDCLASS(GetWorker(),ID_MEASUREDIS_ONMAP,CMeasureDisOnMapCommand);
	REG_CMDCLASS(GetWorker(),ID_MEASURELENGTH,CMeasureLengthCommand);
	REG_CMDCLASS(GetWorker(),ID_MEASUREANGLE,CMeasureAngleCommand);
	REG_CMDCLASS(GetWorker(),ID_MEASUREAREA,CMeasureAreaCommand);
	REG_CMDCLASS(GetWorker(),ID_CELL_DEFINE,CCellDefineCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_CONTOURTEXT,CContourTextCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_FACE_COMPLEX,CDrawMultiSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_BYANG,CDrawLinesbyAngCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CREATEMULTISURFACE,CCreateMultiSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SEPERATEMULTISURFACE,CSeparateMultiSurfaceCommand);
	
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPOLATEVERTEXS,CInterpolateVertexsCommand);
	REG_CMDCLASS(GetWorker(),ID_CHECK_PLERROR,CCheckPLErrorCommand);
	REG_CMDCLASS(GetWorker(),ID_CHECK_LLERROR,CCheckLLErrorCommand);
	REG_CMDCLASS(GetWorker(),ID_CHECK_PPERROR,CCheckPPErrorCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFI_DEL_LIDAR,CDeleteLidarCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFI_MOVE_LIDAR,CMoveLidarCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFI_SETZ_LIDAR,CModifyZLidarCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFI_INTHEI_LIDAR,CInterpolateLidarCommand);

	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_BYMULITPT,CDrawLinesByMultiPt);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_ANNOTTOTEXT,CAnnotToTextCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MOVECLOSEPT,CMoveClosePtCommand);
	REG_CMDCLASS(GetWorker(),ID_CREATEPARTDEM,CCreatePartDemCommand);
	REG_CMDCLASS(GetWorker(),ID_TRIMDEM,CTrimDemCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_AUTOATTRIBUTE,CAutoAttributeCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYATTRIBUTE,CModifyAttributeCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_LINE_SINGLEPARALLEL,CDrawSingleParallelCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_SNAPLAYER,CSnapLayerCommand);
	REG_CMDCLASS(GetWorker(),ID_RASTER_POSITION, CMeasureImageCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPOLATECENTERLINE, CInterpolateCenterLineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPOLATECENTERLOAD, CInterpolateCenterLoadCommand);
	REG_CMDCLASS(GetWorker(),ID_WS_SETBOUND_BYDRAW, CSetBoundByDrawCommand);
	REG_CMDCLASS(GetWorker(),ID_MEASURELENGTH_PHOTO, CMeasurePhotoLengthCommand);
	REG_CMDCLASS(GetWorker(),ID_DRAW_TEST_PT,CDlgDrawTestPointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYDEMPOINT,CModifyDemPointCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_NUMBERTEXT,CNumberTextCommand);
	REG_CMDCLASS(GetWorker(),ID_BATCH_EXPORTMAPS_DXF,CBatExportMapsCommand_dxf);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_FIXTOEXT,CFixAttriToExtAttriCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CONVERTLAYER_SELECTION,CFCodeChgSelectionCommand);
	REG_CMDCLASS(GetWorker(),ID_TOOL_MAKE_CHECKPTSAMPLE,CMakeCheckPtSampleCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_EXTTOFIX,CExtAttriToFixAttriCommand);
	REG_CMDCLASS(GetWorker(),ID_SELECT_LAYER,CSelectLayerCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MERGECURVE_FA,CMergeCurveFACommand);
	REG_CMDCLASS(GetWorker(),ID_SURFACE_TO_DCURVE_ONE,CSurfaceToDoubleLinesCommand_One);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_ATTRIBUTE,CModifyAttribute1Command);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COMPRESS_REDUNDANT,CCompressRedundantCommand);

	if(((CEditBaseApp*)AfxGetApp())->IsSupportPlugs())
		RegisterPlugsInCmds();

	BeginCheck41License

	REG_CMDCLASS(GetWorker(),ID_MODIFY_PARTMOVE,CPartMoveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYBETWEENLAYER,CCopyFtrsBetweenLayerCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPY_ASSISTLINE,CSaveAssistLineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPY_BASELINE,CCopyBaselineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MERGECURVE,CMergeCurveCommand);		
	REG_CMDCLASS(GetWorker(),ID_ADJUST_CENTER_LINE,CAutoAdjustParallelCommand);
	REG_CMDCLASS(GetWorker(),ID_DCURVE_REVERSE,CReverseAssistlineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CONTOURACROSSBANK,CTrimContourAcrossBankCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CONTOURACROSSDCURVE,CTrimContourAcrossDCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DIRPOINTTOLINE,CDirPointToLineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_LINETODIRPOINT,CLineToDirPointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_LINETODIRPOINT1,CLineToDirPoint1Command);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPOLATEPOINTZ,CInterpolatePointZCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CURVEINTERSECT,CIntersectCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_POINTZCREATETEXT,CPointZToTextCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_TEXTCREATEPOINT,CTextToPointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_TEXTDIGIT,CModifyTextDigitCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_EAVESADJUST,CEavesAdjustCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INSTEADLITTLEHOUSE, CInsteadLittleHouseCommand);
	REG_CMDCLASS(GetWorker(),ID_MOD_LINEFILL,CLineFillCommand);
	REG_CMDCLASS(GetWorker(),ID_MOD_DELHACHURE,CDelHachureCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CURVEBOUND,CCurveToBoundCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CULVERTREVERSE,CCulvertReverseCommand);
	REG_CMDCLASS(GetWorker(),ID_BUILD_SURFACE,CDoubleLinesToSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_BUILD_SURFACE_ALL,CDoubleLinesToSurfaceAllCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_POINTTOSURFACEPOINT,CPointToSurfacePointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SURFACEPOINTTOPOINT,CSurfacePointToPointCommand);
	REG_CMDCLASS(GetWorker(),ID_RIVER_ADD_DIRECT, CRiverAddDirectCommand);
	REG_CMDCLASS(GetWorker(),ID_SURFACE_TO_DCURVE,CSurfaceToDoubleLinesCommand);

	REG_CMDCLASS(GetWorker(), ID_DHEIGHT_TO_POINT, CDHeightToPointCommand);
	REG_CMDCLASS(GetWorker(), ID_POINT_TO_DIRPOINT, CPointtoDirPointCommand);
	REG_CMDCLASS(GetWorker(), ID_DCURVE_TO_TWO_CURVE, CDCurveSplitCommand);
	REG_CMDCLASS(GetWorker(), ID_MODIFY_TEXTTOSURFACEPOINT, CTextToSurfacePointCommand);
	REG_CMDCLASS(GetWorker(), ID_MODIFY_ANNOTOSURFACEPOINT, CAnnoToSurfacePointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_TEXTCOVER,CTextCoverCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYDOC,CCopyDocCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_PASTEDOC,CPasteDocCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_PASTEDOCCOORD,CPasteDocCoordCommand);

	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE_NEW,CMapDecorateCommand_New);
	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE,CMapDecorateCommand);
	REG_CMDCLASS(GetWorker(),ID_BATCH_RECTMAPBORDER,CBatCreateRectMapBorderCommand);
	REG_CMDCLASS(GetWorker(),ID_BATCH_INCLINEDMAPBORDER,CBatCreateInclinedMapBorderCommand);
	REG_CMDCLASS(GetWorker(),ID_BATCH_ROADMAPBORDER,CBatCreateRoadMapBorderCommand);
	REG_CMDCLASS(GetWorker(),ID_BATCH_EXPORTMAPS,CBatExportMapsCommand);
	REG_CMDCLASS(GetWorker(),ID_TOOL_INPUTPTCONVERT,CConvertBySelectPtsCommand);
	REG_CMDCLASS(GetWorker(),ID_REFFILE_CLIP,CRefFileCopyAllCommand);
	REG_CMDCLASS(GetWorker(),ID_BATCH_TRAPZOIDMAPBORDER,CBatCreateTrapezoidMapBorderCommand);
	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE_AUTOSETMAPNUMFROMFILE,CMapDecorateAutoSetNearmapFromFile);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_ALIGN,CAlignCommand);
	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE_CREATEXYNOTELINE,CCreateCoordNoteLineCommand);
	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE_CREATEXYNOTE,CCreateCoordNoteCommand);
	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE_SETNOTEANGLE_FROMLINE,CSetTextAngleFromLineCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MODIFYPOINTSCALE,CDlgEditDirPointCommand);
	REG_CMDCLASS(GetWorker(), ID_DEL_OVERLAPLINE, CDeleteOverlapLinesCommand); 
	REG_CMDCLASS(GetWorker(), ID_DEAL_SELF_INTERSECTION, CDealSelfIntersectionCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFY_SEPARATESYMBOLS,CSeparateSymbolsCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CREATESYMBOLS,CCreateSymbolsCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CREATEHATCHSYMBOLS,CCreateHatchSymbolsCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MATCHVERTEXS,CMatchVertexsCommand);

	REG_CMDCLASS(GetWorker(),ID_MEASUREPTTOLINEDIS,CMeasurePointToLineDistanceCommand);//量测点到线串命令
	REG_CMDCLASS(GetWorker(),ID_MEASURETHREEPTANGLE,CMeasureThreePointAngleCommand);//量测三点夹角命令
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SETSPECIALSYMBOL,CSetSpecliaSymbolCommand);//指定独立符号项
	REG_CMDCLASS(GetWorker(),ID_DRAWTABLE,CDrawTableCommand);//绘制表格
	REG_CMDCLASS(GetWorker(),ID_CREATESYMBOLORDISCRIPTION,CCreateSymbolAndDiscriptCommand);//生成符号或文字描述
	REG_CMDCLASS(GetWorker(),ID_TOOL_SYMBOLLAYOUT,CSymbolLayoutCommand);//生成符号或文字描述

	REG_CMDCLASS(GetWorker(),ID_MODIFY_LAKEHATCH,CLakeHatchCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_CURVELENGTH,CModifyCurveLengthCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFI_CUTLINE,CModifyCutLinesCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DELETESHORTCURVE,CDeleteShortCurveCommand);

	REG_CMDCLASS(GetWorker(),ID_MAP_MATCH,CMapMatchCommand);
	REG_CMDCLASS(GetWorker(),ID_MAP_MANUALMATCH,CMapManualMatchCommand);
	REG_CMDCLASS(GetWorker(),ID_MAP_MATCH_CONNECT,CMapMatchConnectCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFY_RIVERCHANGE,CRiverChangeCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_RIVERCOLORFILL,CRiverFillCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_EXPLODETEXT,CExplodeTextCommand);

	REG_CMDCLASS(GetWorker(),ID_CONTOUR_AUTO_LAYERSETTING,CContourAutoLayerSettingCommand);
	REG_CMDCLASS(GetWorker(),ID_CONTOUR_AUTO_REPAIR,CContourAutoRepairCommand);
	REG_CMDCLASS(GetWorker(),ID_CONTOUR_EVALUATE,CContourEvaluateCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_ARRAY,CModifyArrayCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_POINTZANDTEXT,CModifyPointzAndTextCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFY_SEPERATESURFACESYMBOLS,CSeparateSurfaceSymbolsCommand);

	REG_CMDCLASS(GetWorker(),ID_MOD_COLORFILLCELL,CColorFillCommand);
	REG_CMDCLASS(GetWorker(),ID_MOD_REPLACE_WITHPOINT,CReplaceWithPointCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFY_EXTENDORTRIM,CExtendOrTrimCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_EXTENDORTRIM_ALL,CExtendOrTrimAllCommand);

	REG_CMDCLASS(GetWorker(),ID_MOD_LL_SUSPEND1,CLLSuspendProcess1Command);
	REG_CMDCLASS(GetWorker(),ID_MOD_PL_SUSPEND,CPLSuspendProcessCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_PRETREAT,CTopoPreTreatCommand);
    
	//TOPO
	REG_CMDCLASS(GetWorker(),ID_TOPO_SURFACE,CTopoBuildSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_DELETE,CTopoDelSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_QT_VERTEX_AUTOMATCH,CMatchAllVertexsCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_REPLACELINES,CTopoReplaceLinesCommand);

	BeginCheck42License
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_SURFACEPOINT,CDlgDrawSurfacePointCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_SURFACE_NOBREAK,CTopoSurfaceNoBreakCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_SURFACE_FROMPT,CTopoSurfaceFromPtCommand);
	REG_CMDCLASS(GetWorker(),ID_CHECK_SURFACE_GAP,CCheckSurfaceGapCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_UNION,CMergeSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_XORSURFACE,CXORSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_MERGESURFACE,CMergeSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_FACE_AUTO,CDlgAutoSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_PARALLEL_ROAD, CDrawParalRoadCommand);
	REG_CMDCLASS(GetWorker(),ID_TOPO_UNION_ALLMAP,CMergeSurfaceCommand_AllMap);
	REG_CMDCLASS(GetWorker(),ID_SURFACE_NOOVERPART_TOAREA, CSurNoOverPartCommand);
	REG_CMDCLASS(GetWorker(),ID_SURFACE_OVERPART_TOAREA, CSurOverPartCommand);
	REG_CMDCLASS(GetWorker(), ID_SURFACE_TO_AREA, CSurfaceToBoundCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SPLITSURFACE,CSplitSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_DEL_BORDER_IN_VEGA,CDeleteBorderInVEGACommand);
	REG_CMDCLASS(GetWorker(),ID_DELETE_PT_AROUNDCURVE,CDelPtAroundCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_ROAD_BYSUNRULE,CModifyRoadBySunRuleCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYEXT,CCopyEXTAttributionCommand);
	REG_CMDCLASS(GetWorker(),ID_DELETE_ROAD_SURFACE,CDelRoadSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_CREATE_SURFACEPT_FROM_SURFACE,CCreateSurfacePtFromSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_EXPLODE_DCURVE_TO_SURFACE,CExplodeDcurveToSurfaceCommand);
	REG_CMDCLASS(GetWorker(),ID_RIVER_Z_CHANGE,CRiverZChangeCommand);
	REG_CMDCLASS(GetWorker(), ID_TOPO_SURFACE_REGION_PT, CTopoSurfaceRegionCommand);
	REG_CMDCLASS(GetWorker(), ID_CHANGE_DCURVE_BY_WIDTH, CChangeDcurveByWidthCommand);

	BeginCheck50License
	REG_CMDCLASS(GetWorker(), ID_MODIFY_PARTUPDATECONTOUR, CPartUpdateContourCommand);
	EndCheck50License

	EndCheck42License
		
	REG_CMDCLASS(GetWorker(),ID_SURFACE_FILLATT_FROMPT,CFillSurfaceATTFromPtCommand);
	REG_CMDCLASS(GetWorker(),ID_SURFACE_FILLATT_FROMPT_SEL,CFillSurfaceATTFromPtSelCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFY_DELETE_DZPOINT,CDeleteDHeightCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_DCURVEOCURVE,CParalleToSingleCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_PARALLELTOCURVE,CParalleToCurveCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_SURFACETOPOINT,CSurfaceToPointCommand);
	REG_CMDCLASS(GetWorker(),ID_ELEMENT_WIDTHNOTE,CCreateWidthNoteCommand);
	REG_CMDCLASS(GetWorker(),ID_MAPDECORATE_CREATE_FIGURE,CCreateFigureCommand);

	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYOBJ_INSIDEBRIDGE,CCopyFtrsInsideBridgeCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_COPYOBJ_INSIDEBRIDGE_ONE,CCopyFtrsInsideBridgeCommand_One);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPOLATE_HEIPOINTS_REGION,CInterpolateHeiPointCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_INTERPOLATEPOINTZ_NOSELECT,CInterpolatePointZCommand_NoSelect);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_LAYER,CModifyLayerCommand);
	REG_CMDCLASS(GetWorker(),ID_MODIFY_LAYER1,CModifyLayer1Command);
	REG_CMDCLASS(GetWorker(),ID_REPAIR_Z,CRepairZCommand);
	REG_CMDCLASS(GetWorker(),ID_REPAIR_LINETYPE,CRepairLineTypeCommand);
	EndCheck41License

	char path[256]={0};
	GetModuleFileName(NULL,path,sizeof(path));
	char *pos = strrchr(path,'\\');
	if( pos )*pos = '\0';
	strcat(path,"\\license.dat");

#ifndef TRIAL_VERSION
//	if( !CheckDongleLicenseFile(path) )
//	{
//		m_pContext->~CStereoDrawingContext();
//	}
#endif
	
	//VM_END
}


void CBaseView::RegisterPlugsInSelectCmds()
{
	int nNum = 0;
	const PlugItem * pItem = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nNum);	
	for (int i=0;i<nNum;i++)
	{
		if(pItem[i].bUsed && pItem[i].pObj->GetPlugType()==PLUG_TYPE_CMD)
		{
			CPlugCmds *p = (CPlugCmds *)pItem[i].pObj;
			const int *funTypes = p->GetFuncTypes();
			const FUNC_PTR*  pFun = p->GetFuncPtr();
			for (int j=0;j<p->GetItemCount();j++)
			{
				if (funTypes[j] == PLUGFUNC_TYPE_SELECT)
				{
					GetWorker()->RegisterSelectStateCommand((LPPROC_CMDCREATE)(pFun[j]));
				}
			}			
		}
		if(pItem[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
		{
			
		}
	}
}

void CBaseView::RegisterPlugsInCmds()
{
	int nNum = 0;
	const PlugItem * pItem = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nNum);	
	for (int i=0;i<nNum;i++)
	{
		if( pItem[i].bUsed && pItem[i].pObj->GetPlugType()==PLUG_TYPE_CMD)
		{
			CPlugCmds *p = (CPlugCmds *)pItem[i].pObj;
			const int *funTypes = p->GetFuncTypes();
			const FUNC_PTR*  pFun = p->GetFuncPtr();
			for (int j=0;j<p->GetItemCount();j++)
			{
				if(funTypes[j]&PLUGFUNC_TYPE_CREATE)
					GetWorker()->RegisterCommand(pItem[i].itemID[j], (LPPROC_CMDCREATE)(pFun[j]));
			}			
		}
		if(pItem[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
		{
			
		}
	}
	int idx = GetDocument()->GetPluginItemIdx();
	if (idx==-1)return;
	CPlugBase *p = pItem[idx].pObj;
	const int *funTypes = p->GetFuncTypes();
	const FUNC_PTR*  pFun = p->GetFuncPtr();
	for (int j=0;j<p->GetItemCount();j++)
	{
		if(funTypes[j]&PLUGFUNC_TYPE_CREATE)
			GetWorker()->RegisterCommand(pItem[idx].itemID[j], (LPPROC_CMDCREATE)(pFun[j]));
	}			
}

void CBaseView::ClientToGround(PT_4D *pt0, PT_3D *pt1)
{
 	PT_4D t;
 	ClientToImage(pt0, &t);	
	ImageToGround(&t,pt1);
}

void CBaseView::GroundToClient(PT_3D *pt0, PT_4D *pt1)
{
	PT_4D t;
	GroundToImage(pt0, &t);
	ImageToClient(&t,pt1);
//	CInteractiveView::GroundToClient(pt0,pt1);
}

void CBaseView::ZoomChange(CPoint point, float change, BOOL bToCenter)
{
//	CInteractiveView::ZoomCustom(point,change,bToCenter);
	CSceneViewEx::ZoomChange(point,change,bToCenter);
}


PT_3D CBaseView::GetCrossPoint()
{
	return m_gCurPoint;
}

CCoordWnd CBaseView::GetCoordWnd()
{
	return CInteractiveView::GetCoordWnd();
}

void CBaseView::DrawTmptoVectLay(const GrBuffer *pBuf, CVectorLayer *pLL, CVectorLayer *pLR)
{

}

void CBaseView::OnMouseLeave()
{
	CDlgDoc *pDoc = GetDocument();
	m_bAccuLeave = TRUE;
	UpdateAccuDrawing();
	FinishUpdateDrawing();

	ClearCross();
	EnableCrossVisible(FALSE);
 //   ResetCursorDrawing();
}

void CBaseView::UpdateAccuDrawing()
{
	CDlgDoc *pDoc = GetDocument();
	if( !pDoc->m_accuBox.IsOpen() || m_bAccuLeave || !pDoc->m_accuBox.IsActive() )
	{
		ClearAccuDrawing();
		return;
	}
//  	if( !m_accurboxLayL.IsBufferChanged() )
//  		return;
	GrBuffer2d bufL0,bufR0,bufL1,bufR1;
	if(pDoc->m_accuBox.IsNeedRedraw())
	{
		pDoc->m_accuBox.DrawBox(GetCoordWnd(),&bufL0,&bufR0);
		pDoc->m_accuBox.DrawDrg(GetCoordWnd(),&bufL1,&bufR1);
		bufL0.AddBuffer(&bufL1);
		bufR0.AddBuffer(&bufR1);
		m_accurboxLayL.SetBuffer(&bufL0);
		m_accurboxLayR.SetBuffer(&bufR0);
		UpdateDrawingLayers(&m_accurboxLayL,&m_accurboxLayR);
		pDoc->m_accuBox.SetReDrawFlag(FALSE);
		return;
	}
	
// 	OnPreUpdateRegion(NULL, UPDATEREGION_ACCUBOX);
// 	EraseAccuBox();
// 	DrawAccuBox();
// 	OnPostUpdateRegion(NULL, UPDATEREGION_ACCUBOX);

}

void CBaseView::OnTimer(UINT_PTR nIDEvent)
{
	//合并更新项
	if( nIDEvent==TIMERID_SIMPLIFY_UPDATEVIEW )
	{
		//暂时没有代码
		return;
	}
	
	//执行更新
	if( nIDEvent==TIMERID_UPDATEVIEW )
	{
// 		if( m_UpdateFlags[VIEWUPDATE_ADDORDELOBJ]>0 )
// 		{
// 			m_vectLayL.UpdateVector(FALSE);
// 			m_vectLayR.UpdateVector(FALSE);
// 			m_nUpdateFlags[VIEWUPDATE_ADDORDELOBJ] = 0;
// 			
// 			Invalidate(FALSE);
// 		}
// 		
		return;
	}
	CInteractiveView::OnTimer(nIDEvent);
}

void CBaseView::HiliteObject(CFeature *pFtr)
{
	m_markLayL.SetDrawType(FALSE,TRUE,m_bShowKeyPoint);
	m_markLayL.SetMark1Color(m_clrHilite);
	m_markLayL.SetMark1Width(2);
	m_markLayL.SetObjMark1((LONG_PTR)pFtr, TRUE);

	m_markLayR.SetDrawType(FALSE,TRUE,m_bShowKeyPoint);
	m_markLayR.SetMark1Color(m_clrHilite);
	m_markLayR.SetMark1Width(2);
	m_markLayR.SetObjMark1((LONG_PTR)pFtr, TRUE);
	
	return;

}

void CBaseView::UnHiliteObject(CFeature *pFtr)
{
	m_markLayL.SetDrawType(FALSE,TRUE,m_bShowKeyPoint);
	m_markLayL.SetObjMark1((LONG_PTR)pFtr, FALSE);
	m_markLayR.SetDrawType(FALSE,TRUE,m_bShowKeyPoint);
	m_markLayR.SetObjMark1((LONG_PTR)pFtr, FALSE);
	
	return;
}

void CBaseView::UpdateSelDrawing(BOOL bAlways)
{
	CDlgDoc *pDoc = GetDocument();
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if( !pDS )return;

	CSelection *pSel = pDoc->GetSelection();	
	if( !bAlways )
	{
		int num;
		SELCHG_STATE *states;
		num = pSel->GetSelChgObjs(NULL);
		if( num>0 )
		{
			states = new SELCHG_STATE[num];
			if( states )
			{
				pSel->GetSelChgObjs(states);
				CFeature *pFtr;
				int hinum = 0;
				BOOL bUpdateAll = FALSE;
				for( int i=0; i<num; i++ )
				{
					pFtr = HandleToFtr(states[i].handle);
					if( pFtr )
					{
						if( states[i].tobesel==SELCHG_TOBEUNSEL )
							UnHiliteObject(pFtr);
						else 
							HiliteObject(pFtr);
					}
				}
				
				m_markLayL.ClearAll();
				m_markLayR.ClearAll();
				
				Invalidate(FALSE);
				
				delete[] states;
			}
		}
	}
	else
	{
		m_markLayL.ClearAll();
		m_markLayR.ClearAll();
		
		m_markLayL.SetAllObjsMark1(FALSE);
		m_markLayR.SetAllObjsMark1(FALSE);

		int num;
		const FTR_HANDLE *phs = pSel->GetSelectedObjs(num);

		for( int i=0; i<num; i++ )
		{
			HiliteObject(HandleToFtr(phs[i]));
		}
	}

}

extern BOOL CheckStrZ(LPCTSTR strz, float& z);
void CBaseView::UpdateSnapDrawing()
{
	GroundToClient(&m_itemCurSnap.pt,&m_ptCurSnapClt);

	//需要比较客户坐标，因为手轮脚盘运动时可能会导致捕捉点大地坐标不变，但客户坐标变化，这时也需要更新
	BOOL bUnChanged = (_FABS(m_ptCurSnapClt.x-m_ptOldSnapClt.x)<0.5 &&
		_FABS(m_ptCurSnapClt.y-m_ptOldSnapClt.y)<0.5 &&
		_FABS(m_ptCurSnapClt.z-m_ptOldSnapClt.z)<0.5 && 
		m_itemCurSnap.nSnapMode==m_itemOldSnap.nSnapMode );
	
	//如果咬合点没有变化，而且咬合点已经被显示，那么就不需要再重画了
	if( m_itemOldSnap.IsValid() && bUnChanged )
		return;
	
	CString tip;
	switch(m_itemCurSnap.nSnapMode) 
	{
	case CSnap::modeEndPoint:   tip.LoadString(IDS_SNAPITEM_END);	break;
	case CSnap::modeKeyPoint:	tip.LoadString(IDS_SNAPITEM_KEY);	break;
	case CSnap::modeNearPoint:	tip.LoadString(IDS_SNAPITEM_NEAREST);	break;
	case CSnap::modeMidPoint:	tip.LoadString(IDS_SNAPITEM_MID);	break;
	case CSnap::modeIntersect:	tip.LoadString(IDS_SNAPITEM_INTER);	break;
	case CSnap::modePerpPoint:	tip.LoadString(IDS_SNAPITEM_PERP);	break;
	case CSnap::modeCenterPoint:tip.LoadString(IDS_SNAPITEM_CENTER);	break;
	case CSnap::modeTangPoint:	tip.LoadString(IDS_SNAPITEM_TANG);	break;
	case CSnap::modeGrid:		tip.LoadString(IDS_SNAPITEM_GRID);	break;
	case CSnap::modePolar:		tip.LoadString(IDS_SNAPITEM_POLAR);	break;
	default:;
	}
	
	if( tip.IsEmpty() )
		tip.LoadString(ID_INDICATOR_SNAP);
	else
		tip = tip + (GetDocument()->m_snap.Is2D()?_T(".2D"):_T(".3D"));
	
	if( !m_itemCurSnap.IsValid() )tip.LoadString(ID_INDICATOR_SNAP);
	AfxGetMainWnd()->SendMessage(FCCM_UPDATEINDICATOR,ID_INDICATOR_SNAP,(LPARAM)(LPCTSTR)tip);
	
	if( m_itemCurSnap.pFtr && m_itemCurSnap.pFtr!=m_itemOldSnap.pFtr )
	{
		CDlgDoc *pDoc = GetDocument();
		if (pDoc)
		{
			for (int i=0; i<pDoc->GetDlgDataSourceCount(); i++)
			{
				CFtrLayer *pLayer = pDoc->GetDlgDataSource(i)->GetFtrLayerOfObject(m_itemCurSnap.pFtr);
				if (pLayer)
				{
					AfxGetMainWnd()->SendMessage(FCCM_UPDATEINDICATOR,ID_INDICATOR_SNAP_FTR,(LPARAM)(LPCTSTR)pLayer->GetName());
					break;
				}

			}
			
		}
	}
	
	if( m_bShowSnap )
	{
		GrBuffer2d buf;
		PT_3D t;
		PT_4D t0;
		
		COPY_3DPT(t,m_itemCurSnap.pt);
		GroundToClient(&t,&t0);
		
		t.x = t0.x; t.y = t0.y; t.z = 0;
		
		::CreateSnapBuffer(&buf,m_itemCurSnap,t,6,2,GetDocument()->m_snap.Is2D());

		buf.SetAllColor(m_clrSnap);
		m_snapLayL.SetBuffer(&buf);

		buf.DeleteAll();

		t.x = t0.z; t.y = t0.yr; t.z = 0;

		::CreateSnapBuffer(&buf,m_itemCurSnap,t,6,2,GetDocument()->m_snap.Is2D());

		buf.SetAllColor(m_clrSnap);
		m_snapLayR.SetBuffer(&buf);
		
		m_itemOldSnap = m_itemCurSnap;
		m_ptOldSnapClt = m_ptCurSnapClt;

		UpdateDrawingLayers(&m_snapLayL,&m_snapLayR);

	}

	//用于三维视图的显示捕捉
	{		
		SNAPITEM item = m_itemCurSnap;
		//PT_3D t0 = m_itemCurSnap.pt;

		//通知各个view
		GetDocument()->UpdateAllViews(NULL, 77, (CObject*)&item);
	}
	
}

void CBaseView::ClearSnapDrawing()
{
	if( !m_itemOldSnap.IsValid() && !m_itemCurSnap.IsValid() )
		return;
	
	m_snapLayL.SetBuffer((GrBuffer2d*)NULL);
	m_snapLayR.SetBuffer((GrBuffer2d*)NULL);

	UpdateDrawingLayers(&m_snapLayL,&m_snapLayR);

	CString tip;
	tip.LoadString(ID_INDICATOR_SNAP);
	AfxGetMainWnd()->SendMessage(FCCM_UPDATEINDICATOR,ID_INDICATOR_SNAP,(LPARAM)(LPCTSTR)tip);
	m_itemOldSnap.pFtr = NULL;
	m_itemOldSnap.bInPreObj = 0;
	m_itemOldSnap.nSnapMode = 0;

	m_itemCurSnap.pFtr = NULL;
	m_itemCurSnap.bInPreObj = 0;
	m_itemCurSnap.nSnapMode = 0;

	//通知其他view
	GetDocument()->UpdateAllViews(NULL, 77, (CObject*)NULL);

}


void CreateSnapBuffer(GrBuffer2d *buf, SNAPITEM item, PT_3D cpt, int r, int w, BOOL bIs2D )
{
	if( !item.IsValid() )
		return;
	
	PT_3D t0, t1;		
	t0.x = PIXEL(cpt.x); t0.y = PIXEL(cpt.y); t0.z = 0;
	t1.z = 0;

	//端点：菱形 ◇
	if( item.nSnapMode==CSnap::modeEndPoint )
	{
		int offlen = r*1.414, width = w+1, i;
	
		
		for( i=0; i<width; i++,offlen-- )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y;
			buf->MoveTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y;
			buf->LineTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y;
			buf->LineTo(&t1);
			buf->End();
		}
		
	}
	//格网点：方形 □
	else if( item.nSnapMode==CSnap::modeGrid )
	{
		int offlen = r, width = w, i;
		
		for( i=0; i<width; i++,offlen-- )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			buf->End();
		}
		
	}
	//关键点：方形 □
	else if( item.nSnapMode==CSnap::modeKeyPoint )
	{
		int offlen = r, width = w, i;
		
		for( i=0; i<width; i++,offlen-- )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			buf->End();
		}
		
	}
	//中点：直线＋中点
	else if( item.nSnapMode==CSnap::modeMidPoint )
	{
		int offlen = r, width = w, i;

		t0.y -= (width/2+2);

		t1.x = t0.x-(width/2+1); t1.y = t0.y;
		buf->BeginLineString(RGB(255,255,0),1,FALSE);
		buf->MoveTo(&t1);		
		t1.x = t0.x+(width/2+1)+1; t1.y = t0.y;
		buf->LineTo(&t1);
		t0.y += 1.0;
		buf->End();

		buf->BeginLineString(RGB(255,255,0),1,FALSE);
		t1.x = t0.x-(width/2+1); t1.y = t0.y;
		buf->MoveTo(&t1);		
		t1.x = t0.x+(width/2+1)+1; t1.y = t0.y;
		buf->LineTo(&t1);
		t0.y += 1.0;
		buf->End();

		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen+1; t1.y = t0.y;
			buf->LineTo(&t1);
			t0.y += 1.0;
			buf->End();
		}
		
		buf->BeginLineString(RGB(255,255,0),1,FALSE);
		t1.x = t0.x-(width/2+1); t1.y = t0.y;
		buf->MoveTo(&t1);		
		t1.x = t0.x+(width/2+1)+1; t1.y = t0.y;
		buf->LineTo(&t1);
		t0.y += 1.0;
		buf->End();

		buf->BeginLineString(RGB(255,255,0),1,FALSE);
		t1.x = t0.x-(width/2+1); t1.y = t0.y;
		buf->MoveTo(&t1);		
		t1.x = t0.x+(width/2+1)+1; t1.y = t0.y;
		buf->LineTo(&t1);
		buf->End();

	}
	//相交点：X 字形
	else if( item.nSnapMode==CSnap::modeIntersect )
	{
		int offlen = r, width = w+1, i;

		t0.x -= width/2;
		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			buf->End();
			
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y+offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			
			buf->End();
			t0.x += 1.0;
		}
	}
	//垂点：直角形
	else if( item.nSnapMode==CSnap::modePerpPoint )
	{
		int offlen = r, width = w, i;
		
		t0.x -= width/2;
		t0.y += width/2;
		double x0 = PIXEL(cpt.x)+offlen*2, y0 = PIXEL(cpt.y)-offlen*2;
		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x; t1.y = y0;
			buf->MoveTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y;
			buf->LineTo(&t1);
			
			t1.x = x0; t1.y = t0.y;
			buf->LineTo(&t1);
			
			t0.x += 1.0;
			t0.y -= 1.0;
			buf->End();
		}

		x0 = t0.x, y0 = t0.y+1.0;

		offlen = r-width/2;
		t0.x = PIXEL(cpt.x)+r; t0.y = PIXEL(cpt.y)-r;
		t0.x -= width/2;
		t0.y += width/2;		
		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = x0; t1.y = t0.y;
			buf->MoveTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y;
			buf->LineTo(&t1);
			
			t1.x = t0.x; t1.y = y0;
			buf->LineTo(&t1);
			buf->End();

			t0.x += 1.0;
			t0.y -= 1.0;
		}
	}
	//中心点：回字形
	else if( item.nSnapMode==CSnap::modeCenterPoint )
	{
		
		int offlen = r, width = w+2, i;
		for( i=0; i<width; i++,offlen-- )
		{
			if( i==1 )continue;
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x+offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y+offlen;
			buf->LineTo(&t1);
			
			t1.x = t0.x-offlen; t1.y = t0.y-offlen;
			buf->LineTo(&t1);
			buf->End();
		}
	}
	//切点：V＋切线
	else if( item.nSnapMode==CSnap::modeTangPoint )
	{
		int offlen = r, width = w, i;
		
		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y+i+1;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen+1; t1.y = t0.y+i+1;
			buf->LineTo(&t1);
			buf->End();
		}
		
		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen+i; t1.y = t0.y-offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y-i;
			buf->LineTo(&t1);
			
			t1.x = t0.x+offlen-i+1; t1.y = t0.y-offlen-1;
			buf->LineTo(&t1);
			buf->End();
		}

	}
	//最近点：倒三角
	else if( item.nSnapMode==CSnap::modeNearPoint )
	{
		int offlen = r, width = w, i;

		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen; t1.y = t0.y-offlen-i-1;
			buf->MoveTo(&t1);
			
			t1.x = t0.x+offlen+1; t1.y = t0.y-offlen-i-1;
			buf->LineTo(&t1);
			buf->End();
		}
		
		for( i=0; i<width; i++ )
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x-offlen+i; t1.y = t0.y-offlen;
			buf->MoveTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y-i;
			buf->LineTo(&t1);
			
			t1.x = t0.x+offlen-i+1; t1.y = t0.y-offlen-1;
			buf->LineTo(&t1);
			buf->End();
		}
	}
	// 极轴：X 字形（小）
	else if( item.nSnapMode==CSnap::modePolar )
	{
		int offlen = r/2;
		
		buf->BeginLineString(RGB(255,255,255),1,FALSE);
		t1.x = t0.x-offlen; t1.y = t0.y-offlen;
		buf->MoveTo(&t1);
		
		t1.x = t0.x+offlen; t1.y = t0.y+offlen;
		buf->LineTo(&t1);
		buf->End();
		
		buf->BeginLineString(RGB(255,255,255),1,FALSE);
		t1.x = t0.x-offlen; t1.y = t0.y+offlen;
		buf->MoveTo(&t1);
		
		t1.x = t0.x+offlen; t1.y = t0.y-offlen;
		buf->LineTo(&t1);
		
		buf->End();

		return;
	}

	t0.x = PIXEL(cpt.x); t0.y = PIXEL(cpt.y); t0.z = 0;
	t0.x -= (r+6);
	if( bIs2D )
	{
	}
	else
	{
		for( int i=0; i<3; i++)
		{
			buf->BeginLineString(RGB(255,255,0),1,FALSE);
			t1.x = t0.x; t1.y = t0.y;
			buf->MoveTo(&t1);
			
			t1.x = t0.x; t1.y = t0.y+r/2;
			buf->LineTo(&t1);
			buf->End();
			
			t0.x += 2.0;
		}
	}
}


void CBaseView::OnMouseMove(UINT nFlags, CPoint point) 
{
	//如果有内部命令(比如拖动影像)正在操作，应该优先执行内部命令
	if( m_nInnerCmd!=0 )
	{
		CSceneViewEx::OnMouseMove(nFlags, point);
		return;
	}

	CDlgDoc *pDoc = GetDocument();
	pDoc->SetDataQueryType(m_bSymbolize);

	if( !pDoc->GetDlgDataSource() )return;

	PT_3D pt3d;
	PT_4D cltpt;
	pt3d = m_gCurPoint;

	GetCoordWnd().m_pSearchCS->GroundToClient(&pt3d,&cltpt);
	pDoc->m_snap.m_bWithSymbol = m_bSymbolize;	
	//获取上一个点
	PT_3D lastPt;
	PT_3D *pLastPt=NULL;
	int lastNum = 0;
	CCommand* pCurCmd = pDoc->GetCurrentCommand();
	DrawingInfo Info;
	if(pCurCmd/*&&pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand))*/)
	{
		Info = (/*(CDrawCommand*)*/pCurCmd)->GetCurDrawingInfo();
		lastNum = Info.GetLastPts(pLastPt);
	}	
	double r = pDoc->m_snap.GetSnapRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS();
	Envelope e ;
	e.CreateFromPtAndRadius(cltpt,r);	

	//判断是否咬中
	//首先判断自身对象
	if( pDoc->m_snap.bOpen() && pDoc->m_snap.bSnapSelf() )
	{
		pDoc->m_snap.PreSnap(e,pLastPt,lastNum,&m_gCurPoint,GetCoordWnd().m_pSearchCS,Info.GetDrawGeo());

	}

	if( pDoc->m_snap.bOpen() && pDoc->m_snap.DoSnap(e,pLastPt,lastNum,&m_gCurPoint,GetCoordWnd().m_pSearchCS) )
	{
		m_itemCurSnap = pDoc->m_snap.GetFirstSnapResult();

		if(m_itemCurSnap.IsValid())
		{
			UpdateSnapDrawing();
			m_gCurPoint = m_itemCurSnap.pt;
		}
		else
			ClearSnapDrawing();
	}
	//没有咬中，再看是否可以利用accubox
	else
	{
		ClearSnapDrawing();
	}

	if( pDoc->m_accuBox.IsOpen() )
	{
		pt3d = m_gCurPoint;
		if( !pDoc->m_accuBox.IsActive() )pDoc->m_accuBox.SetReDrawFlag(TRUE);

		//将咬合的最近点和精确绘图的轴线吸附或者方向锁定求交
		BOOL bWithSnap = FALSE;
		ACCU_DATA data;
		pDoc->m_accuBox.OnUpdateData(3,(LPARAM)&data);
		int  nDir = 0, status = pDoc->m_accuBox.GetStatus();
		pDoc->m_accuBox.GetLockedDirs(nDir);

		if( pDoc->m_accuBox.IsWaitDir() )
		{			
			Envelope et = EnvelopeFromPtAndRadius(cltpt,pDoc->GetSelection()->GetSelectRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS());
			PT_3D searchPt;
			GetCoordWnd().m_pSearchCS->GroundToClient(&pt3d,&searchPt);
			CFeature *pFtr = pDoc->GetDataQuery()->FindNearestObject(searchPt,pDoc->GetSelection()->GetSelectRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS(),GetCoordWnd().m_pSearchCS);
		
			PT_3D ptline0(0,0,0), ptline1(0,0,0);
			if( pFtr )
			{
				if( !pFtr->GetGeometry()->FindNearestBaseLine(searchPt,et,GetCoordWnd().m_pSearchCS,&ptline0,&ptline1,NULL) )
				{
					ptline0.x = ptline0.y = ptline0.z = 0;
					ptline1.x = ptline1.y = ptline1.z = 0;
				}
			}

			pDoc->m_accuBox.MoveDir(ptline0,ptline1);
		}
		else
		{
			//将捕捉到的最近点对应的基线线段作为延长线方向，临时添加到精确绘图中
			if( pDoc->m_snap.bOpen() && m_itemCurSnap.nSnapMode==CSnap::modeNearPoint &&
				nDir<2 )
			{					
				Envelope et = EnvelopeFromPtAndRadius(cltpt,pDoc->GetSelection()->GetSelectRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS());
								
				PT_3D ptline0(0,0,0), ptline1(0,0,0);
				if( m_itemCurSnap.pFtr )
				{
					PT_3D searchPt;
					GetCoordWnd().m_pSearchCS->GroundToClient(&pt3d,&searchPt);
					if( m_itemCurSnap.pFtr->GetGeometry()->FindNearestBaseLine(searchPt,et,GetCoordWnd().m_pSearchCS,&ptline0,&ptline1,NULL) )
					{
						pDoc->m_accuBox.LockDir(ACCU_DIR::extension);
						pDoc->m_accuBox.MoveDir(ptline0,ptline1);
						bWithSnap = TRUE;
					}
				}
			}
		}
		
		pDoc->m_accuBox.MoveDataPt(GetCoordWnd(),pt3d);		

		//如果采用捕捉的最近点处理的结果与不采用捕捉的最近点处理的结果差别超出了捕捉范围，
		//就不要采用捕捉的最近点
		if( bWithSnap )
		{
			//去除当前临时加的延长线方向
			pDoc->m_accuBox.UnlockDir(-1);

			//备份当前的状态数据
			ACCU_DATA data1;
			pDoc->m_accuBox.OnUpdateData(3,(LPARAM)&data1);

			//恢复到以前的状态数据
			pDoc->m_accuBox.OnUpdateData(0,(LPARAM)&data);
			
			//按照不采用捕捉的最近点处理，得到结果 pt3d2
			PT_3D pt3d2 = m_gCurPoint;
			pDoc->m_accuBox.MoveDataPt(GetCoordWnd(),pt3d2);		

			//比较前后差距
			double dis = sqrt( (pt3d.x-pt3d2.x)*(pt3d.x-pt3d2.x) +
				(pt3d.y-pt3d2.y)*(pt3d.y-pt3d2.y) );

			//超出了，获得不采用捕捉的最近点处理的结果
			if( dis>(e.m_xh-e.m_xl)/2 )
				pt3d = pt3d2;
			//没有超出，获得采用捕捉的最近点处理的结果，恢复到当前状态数据
			else
			{
				pDoc->m_accuBox.OnUpdateData(0,(LPARAM)&data1);
			}
		}

		GroundToClient(&pt3d,&cltpt);		
		UpdateAccuDrawing();
		if (m_bAccuLeave)
		{
			ClearAccuDrawing();
		}
// 		else
// 		{
// 			ClearAccuDrawing();
// // 			OnPreUpdateRegion(NULL,UPDATEREGION_ACCUDRG);
// // 			EraseAccuDrg();
// // 			DrawAccuDrg();
// // 			OnPostUpdateRegion(NULL,UPDATEREGION_ACCUDRG);
// 		}

		m_gCurPoint = pt3d;
	}

	//显示当前测标点的坐标
	AfxGetMainWnd()->SendMessage(FCCM_SHOW_XYZ,0,LPARAM(&m_gCurPoint));
	

	CInteractiveView::OnMouseMove(nFlags, point);
}

Envelope CBaseView::EnvelopeFromPtAndRadius(PT_4D cltpt, double r)
{
//	PT_4D t0;
	PT_3D t1[4];
	t1[0].x = cltpt.x-r; t1[0].y = cltpt.y-r; t1[0].z = cltpt.z-r; 
//	ClientToGround(&t0,t1);
	t1[1].x = cltpt.x+r; t1[1].y = cltpt.y-r; t1[1].z = cltpt.z+r; 
//	ClientToGround(&t0,t1+1);
	t1[2].x = cltpt.x+r; t1[2].y = cltpt.y+r; t1[2].z = cltpt.z+r; 
//	ClientToGround(&t0,t1+2);
	t1[3].x = cltpt.x-r; t1[3].y = cltpt.y+r; t1[3].z = cltpt.z-r; 
//	ClientToGround(&t0,t1+3);
	
	Envelope e;
	e.CreateFromPts(t1,4);
	return e;
}

void CBaseView::UpdateGridDrawing()
{
	DWORD dwScale = GetDocument()->GetDlgDataSource()->GetScale();
	double lfRatio = 0.01*dwScale;//图上1cm对应实际lfRatio米
	m_sGridParam.ox = GetProfileDouble(REGPATH_OVERLAYGRID,"MinX",m_sGridParam.ox);
	m_sGridParam.oy = GetProfileDouble(REGPATH_OVERLAYGRID,"MinY",m_sGridParam.oy);
	m_sGridParam.dx = lfRatio*GetProfileDouble(REGPATH_OVERLAYGRID,"Width",10);	
	m_sGridParam.dy = lfRatio*GetProfileDouble(REGPATH_OVERLAYGRID,"Height",10);
	m_sGridParam.xr = GetProfileDouble(REGPATH_OVERLAYGRID,"XRange",m_sGridParam.xr);
	m_sGridParam.yr = GetProfileDouble(REGPATH_OVERLAYGRID,"YRange",m_sGridParam.yr);
	strcpy(m_sGridParam.strz,AfxGetApp()->GetProfileString(REGPATH_OVERLAYGRID,"Z",_T("0.0")));
	m_sGridParam.bVisible = GetDocument()->IsOverlayGrid();
	m_sGridParam.bViewVect= GetProfileInt(REGPATH_OVERLAYGRID,"ViewVect",FALSE);
	m_sGridParam.bViewImg = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"ViewImg",FALSE);
	m_sGridParam.color = AfxGetApp()->GetProfileInt(REGPATH_OVERLAYGRID,"Color",RGB(128,128,128));
	
	BOOL bVisible = m_sGridParam.bVisible;
	if( m_sGridParam.dx<=0 || m_sGridParam.dy<=0 )bVisible = FALSE;
	if( m_sGridParam.xr<=0 || m_sGridParam.yr<=0 )bVisible = FALSE;
		
	m_vectLayL.DelObj(HANDLE_GRID);
	m_vectLayR.DelObj(HANDLE_GRID);
	
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();
	
	if( !bVisible )
	{
	}
	else
	{
		GrBuffer buf;
		
		
		float gridz = 0;
		BOOL bDirectZ = CheckStrZ(m_sGridParam.strz,gridz);
		
		int i, nx = ceil(m_sGridParam.xr/m_sGridParam.dx), ny = ceil(m_sGridParam.yr/m_sGridParam.dy);
		double xr = m_sGridParam.ox+nx*m_sGridParam.dx, yr = m_sGridParam.oy+ny*m_sGridParam.dy, x,y;
		PT_3D pt (0,0,gridz);
		for( x=0, i=0; i<=nx; i++, x+=m_sGridParam.dx )
		{
			buf.BeginLineString(m_sGridParam.color,0);
			pt.x = m_sGridParam.ox+x; pt.y = m_sGridParam.oy;
			buf.MoveTo(&pt);
			
			pt.y = yr;
			buf.LineTo(&pt);
			buf.End();
		}
		
		for( y=0, i=0; i<=ny; i++, y+=m_sGridParam.dy )
		{
			pt.y = m_sGridParam.oy+y; pt.x = m_sGridParam.ox;
			buf.BeginLineString(m_sGridParam.color,0);
			buf.MoveTo(&pt);
			
			pt.x = xr;
			buf.LineTo(&pt);
			buf.End();
		}
		
		AddObjtoVectLay(HANDLE_GRID,&buf);
		m_vectLayL.ClearAll();
		m_vectLayR.ClearAll();
	}

}

CWorker * CBaseView::GetWorker()
{
	return GetDocument();
}


void CBaseView::OnCommandOne()
{
	const MSG *pMsg = GetCurrentMessage(); 
	if( pMsg!=NULL )
	{
		OnCommandType(LOWORD(pMsg->wParam));
	}
}

void CBaseView::OnCommandType(UINT nID)
{
// 	if (nID==ID_MODIFI_PROPCHANGE)
// 	{
// 		CTDoc *pDoc = GetDocument();
// 		ASSERT(pDoc!=NULL);
// 		
// 		CCommand *pCmd = pDoc->GetCurrentCommand();
// 		if (pCmd)
// 		{
// 			if (pCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand))&&!pCmd->IsKindOf(RUNTIME_CLASS(CDrawSingleParallelCommand)))
// 			{ // 当当前的命令为Draw命令时，设置Draw中的地物属性
// 				
// 				CGeometry *pObj = ((CDrawCommand*)pCmd)->GetDrawObject();
// 				if( !pObj )return;
// 				
// 				CUIDataExchange *pdx = CUIDataExchange::Create(pDoc->m_pDataSource);
// 				if( !pdx )return;
// 				
// 				pObj->UpdateData(FALSE,pdx);
// 				delete pdx;	
// 				return;
// 			}
// 			//by shy解决平行单线无法改变及记住颜色的问题
// 			if (pCmd->IsKindOf(RUNTIME_CLASS(CDrawSingleParallelCommand)))
// 			{
// 				CGeometry *pObj1 = ((CDrawSingleParallelCommand*)pCmd)->GetDrawObject1();
// 				if( !pObj1 )return;
// 				CGeometry *pObj2 = ((CDrawSingleParallelCommand*)pCmd)->GetDrawObject2();
// 				if( !pObj2 )return;
// 				CGeometry *pObj = ((CDrawCommand*)pCmd)->GetDrawObject();
// 				if( !pObj )return;
// 				
// 				CUIDataExchange *pdx = CUIDataExchange::Create(pDoc->m_pDataSource);
// 				if( !pdx )return;
// 				
// 				pObj->UpdateData(FALSE,pdx);
// 				pObj1->UpdateData(FALSE,pdx);
// 				pObj2->UpdateData(FALSE,pdx);
// 				delete pdx;	
// 				return;
// 				
// 			}
// 		}
// 	}

	//VM_START

	if( m_nInnerCmd )
	{
		OnPostInnerCmd();
	}

	// 否则直接激发CPropChgCommand
	GetWorker()->CancelCurrentCommand();
	GetWorker()->SetCoordWnd(GetCoordWnd());
	
	GetWorker()->StartCommand(nID);
	
	_UpdateMatrix(m_lfMatrixLic,m_lfRMatrixLic,1);

	//VM_END
}

void CBaseView::OnUpdateCommandType(CCmdUI* pCmdUI) 
{
	CDlgDoc *pDoc = GetDocument();
	ASSERT(pDoc!=NULL);
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
 	if( !pDS )return;
	
 	pCmdUI->Enable(TRUE);
 	pCmdUI->SetCheck(pDoc->GetCurrentCommandId()==pCmdUI->m_nID);

	if (pCmdUI->m_nID == ID_MODIFY_PASTEDOCCOORD || pCmdUI->m_nID == ID_MODIFY_PASTEDOC)
	{
		CopyData data;
		AfxGetMainWnd()->SendMessage(FCCM_COPYDATA,WPARAM(0),LPARAM(&data));
		pCmdUI->Enable((data.ftrs.GetSize()>0));
	}
	
// 	long  lh = pDoc->m_pDataSource->GetCurLayerHandle();
// 	CGeoLayer *pCurLay = pDoc->m_pDataSource->GetLayerByHandle(lh);
// 	ASSERT(pCurLay!=NULL);
// 	
// 	switch(pCmdUI->m_nID)
// 	{
// 		// 采集
// 	case ID_ELEMENT_DOT_AUTOHEIGHT:
// 	case ID_ELEMENT_DOT_DOT:
// 		pCmdUI->Enable( pCurLay->IsClassSupported(CLS_GEOPOINT) );
// 		break;
// 	case ID_ELEMENT_DOT_VECTORDOT:
// 		pCmdUI->Enable( pCurLay->IsClassSupported(CLS_GEODIRPOINT) );
// 		break;
// 	case ID_ELEMENT_FACE_FACE:
// 	case ID_ELEMENT_FACE_FACEDOT:
// 		pCmdUI->Enable( pCurLay->IsClassSupported(CLS_GEOSURFACE) );
// 		break;
// 	case ID_ELEMENT_LINE_LINE:
// 	case ID_ELEMENT_LINE_THREEDOT:
// 	case ID_ELEMENT_LINE_RECT:
// 	case ID_ELEMENT_LINE_RIGHTANGLE:
// 	case ID_ELEMENT_LINE_BYSIDE:
// 	case ID_ELEMENT_LINE_SINGLEPARALLEL:
// 	case ID_ELEMENT_LINE_BYANG:
// 		pCmdUI->Enable( pCurLay->IsClassSupported(CLS_GEOCURVE) );
// 		break;
// 	case ID_ELEMENT_LINE_PARALLEL:
// 		pCmdUI->Enable( pCurLay->IsClassSupported(CLS_PARALLELCURVE) );
// 		break;
// 	case ID_ELEMENT_TEXT:
// 		pCmdUI->Enable( pCurLay->IsClassSupported(CLS_GEOTEXT) );
// 		break;
// 		// 辅助工具
// 	case ID_ELEMENT_ASSISTANT_CLOSE:
// 		break;
// 	case ID_ELEMENT_ASSISTANT_MARKER:
// 		break;
// 	case ID_ELEMENT_ASSISTANT_RIGHTANGLE:
// 		break;
// 	}
}

void CBaseView::OnPlugCommandType(UINT nID)
{
	//VM_START

	GetWorker()->CancelCurrentCommand();
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->StartCommand(nID);
	
	_UpdateMatrix(m_lfMatrixLic,m_lfRMatrixLic,0);

	//VM_END
}


void CBaseView::OnUpdatePlugCommandType(CCmdUI* pCmdUI) 
{
	CDlgDoc *pDoc = GetDocument();
	ASSERT(pDoc!=NULL);
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
 	if( !pDS )return;
 	pCmdUI->Enable(TRUE);
 	pCmdUI->SetCheck(pDoc->GetCurrentCommandId()==pCmdUI->m_nID);
}


void CBaseView::OnPlugFuncType(UINT nID)
{
	int nsz = 0;
	const PlugItem *item = ((CEditBaseApp*)AfxGetApp())->GetPlugins(nsz);
	for (int i=0;i<nsz;i++)
	{
		if (item[i].pObj->GetPlugType()==PLUG_TYPE_CMD)
		{
			int num = item[i].pObj->GetItemCount();
			for (int j=0;j<num;j++)
			{
				if(item[i].itemID[j]==nID)
				{
					const FUNC_PTR* p = (item[i].pObj)->GetFuncPtr();
					((LPPROC_DOCMAP)p[j])();
				}
			}
		}
		if (i==GetDocument()->GetPluginItemIdx()&&item[i].pObj->GetPlugType()==PLUG_TYPE_DOC)
		{
			int num = item[i].pObj->GetItemCount();
			for (int j=0;j<num;j++)
			{
				if(item[i].itemID[j]==nID)
				{
					const FUNC_PTR* p = (item[i].pObj)->GetFuncPtr();
					((LPPROC_DOCMAP)p[j])();
				}				
			}
		}
	}
}

void CBaseView::OnUpdatePlugFuncType(CCmdUI* pCmdUI)
{
}

void CBaseView::OnState_CurveType(UINT nID)
{
	UINT id = nID;
	CDlgDoc *pDoc = GetDocument();
// 	pDoc->m_selection.m_bWithSymbol = m_bSymbolize;
	
	CCommand *pCurCmd = pDoc->GetCurrentCommand();
	if( pCurCmd )
	{
		char c=0;
		UINT	flag = 0;
		switch( id )
		{
		case ID_CURVETYPE_LINESTRING:	
			c='L'; 
			break;
		case ID_CURVETYPE_CURVE:	
			c='V'; 
			break;
		case ID_CURVETYPE_CIRCLE:
			c='C'; 
			break;
		case ID_CURVETYPE_ARC:	
			c='A'; 
			break;
		case ID_CURVETYPE_3ARC:	
			c='Z'; 
			break;
		case ID_CURVETYPE_SYNCH:	
			c='S';
			break;
// 		case ID_CURVETYPE_HIDELINE:	
// 			c='H'; 
// 			break;
		}
		AfxGetMainWnd()->SendMessage(FCCM_KEYIN,WPARAM(c),0);

// 		pCurCmd->SetCoordMan(GetCoordMan());
// 		pCurCmd->KeyDown(c,1,flag);
	}

}

void CBaseView::OnUpdateState_CurveType(CCmdUI* pCmdUI) 
{
	BOOL bCheck = FALSE;
 	CCommand *pCurCmd = GetDocument()->GetCurrentCommand();
	if (!pCurCmd)
	{
		return;
	}
	CValueTable tab;
	tab.BeginAddValueItem();
	pCurCmd->GetParams(tab);
	tab.EndAddValueItem();
	const CVariantEx *var;
 	if( tab.GetValue(0,PF_PENCODE,var) )
	{
 		int penCode = (long)(_variant_t)*var;
		switch( pCmdUI->m_nID )
		{
		case ID_CURVETYPE_LINESTRING:	
			bCheck = (penCode==penLine); 
			break;
		case ID_CURVETYPE_CURVE:	
			bCheck = (penCode==penSpline); 
			break;
		case ID_CURVETYPE_ARC:	
			bCheck = (penCode==penArc); 
			break;
		case ID_CURVETYPE_3ARC:	
			bCheck = (penCode==pen3PArc); 
			break;
		case ID_CURVETYPE_SYNCH:	
			bCheck = (penCode==penStream); 
			break;
		}
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
	
	pCmdUI->SetCheck(bCheck);
}

#include "GeoSurface.h"
#include "GeoText.h"
#include "cadlib.h"
#include "DSM.h"

#ifndef _NOT_USE_DPWPACK
static BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  fid)
{
	if( !fid )return FALSE;
	CString text = fid;
	text.Remove(' ');
	int startpos = 0, findpos = 0;
	while( findpos>=0 )
	{
		findpos = text.Find(',',startpos);
		
		CString sub;
		if( findpos>startpos )
			sub = text.Mid(startpos,(findpos-startpos));
		else
			sub = text.Mid(startpos);
		
		if( CAutoLinkContourCommand::CheckObjForLayerCode(pDS,pFtr,sub) )
		{
			return TRUE;
		}
		
		startpos = findpos+1;
	}
	
	return FALSE;
}

static BOOL S_CreateDem(CDlgDoc *pDoc, LPCTSTR layers, float fInterval, Envelope e, CString& strDemFile)
{
	CDSM dem2;

	//创建 DEM 
	//生成范围

	double w = e.m_xh-e.m_xl, h = e.m_yh-e.m_yl;
	e.m_xh += w*0.5; e.m_xl -= w*0.5;
	e.m_yh += h*0.5; e.m_yl -= h*0.5;

	//查找对象
// 	pDoc->m_selection.FindObjectInRect(e,FALSE);
// 	int nObj = pDoc->m_selection.GetFoundObjectHandles(NULL);
	Envelope rect(e.m_xl,e.m_xh,e.m_yl,e.m_yh,e.m_zl,e.m_zh);
	rect.TransformGrdToClt(pDoc->GetCoordWnd().m_pSearchCS,1);
	pDoc->GetDataQuery()->FindObjectInRect(rect,pDoc->GetCoordWnd().m_pSearchCS);
	int nObj;
	const CPFeature *ppFtr = pDoc->GetDataQuery()->GetFoundHandles(nObj);

	if( nObj<=0 )return FALSE;
// 	OBJ_GUID *ids = new OBJ_GUID[nObj];
// 
// 	//获得对象
// 	pDoc->m_selection.GetFoundObjectHandles(ids);

//	CGeometry *pObj = NULL, *pObj2;
	PT_3DEX expt;
	CPtrArray arr;

//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1,3);
	GProgressStart(3);

	CString strLayers = layers;
	strLayers.Remove(' ');

	//过滤对象
	for(int i=0; i<nObj; i++)
	{
//		pObj = pDoc->m_pDataSource->GetObjectByGUID(ids[i]);
		CFeature *pFtr = ppFtr[i];
		if (!pFtr) continue;
		CGeometry *pObj = pFtr->GetGeometry();
		if( !pObj )continue;

		if( !strLayers.IsEmpty() && !CheckObjForLayerCode(pDoc->GetDlgDataSource(),ppFtr[i],layers) )
			continue;

		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		{
			continue;
		}
		if( pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
		{
			continue;
		}

		CFeature *pFtr2 = pFtr->Clone();
		CGeometry *pObj2 = pFtr2->GetGeometry();
		if( !pObj2 )continue;

		//找到第一个在框内的点		
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pObj2->GetShape(arrPts);
		int nPtSum = arrPts.GetSize();
		for( int j=0; j<nPtSum; j++)
		{
			expt = arrPts[j];
			if( e.bPtIn(&expt) )break;
		}

		if( j>=nPtSum )
		{
			delete pFtr2;
			continue;
		}

		//删除框外的点
		for( j=j-1; j>=0; j--)
		{
			arrPts.RemoveAt(j);
		}

		//删除尾部出框的点
		nPtSum = arrPts.GetSize();
		for( j=nPtSum-1; j>=0; j--)
		{
			expt = arrPts[j];
			if( e.bPtIn(&expt) )break;
			arrPts.RemoveAt(j);
		}

		pObj2->CreateShape(arrPts.GetData(),arrPts.GetSize());

		if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoPoint)) && pObj2->GetDataPointSum()<=0 )
		{
			delete pFtr2;
			continue;
		}

		if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && pObj2->GetDataPointSum()<=1 )
		{
			delete pFtr2;
			continue;
		}

		if( pObj2->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && pObj2->GetDataPointSum()<=2 )
		{
			delete pFtr2;
			continue;
		}

		arr.Add(pFtr2);
	}


//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2,3);
	GProgressStep();

	if( arr.GetSize()<=0 )
	{
		GProgressEnd();
//		AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1,-1);
		return FALSE;
	}

	//生成DEM
	CString dxfName = "c:\\##$$$@@@.dxf";
	CString demName = "c:\\##$$$@@@.dem";
	pDoc->ObjectsToDxf2(arr,dxfName);
//	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2,3);
	GProgressStep();

	for( i=0; i<arr.GetSize(); i++)
	{
		delete (CFeature*)arr.GetAt(i);
	}

	pDoc->DxfToDem(dxfName,demName,fInterval,fInterval);
	GProgressStep();
	GProgressEnd();
// 	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-2,3);
// 	AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1,-1);

	::DeleteFile(dxfName);
	strDemFile = demName;
	return TRUE;
}
#endif
#ifndef _NOT_USE_DPWPACK


static BOOL S_DemToContours(CDlgDoc *pDoc, LPCTSTR demName, float fInterval, LPCTSTR fid)
{ 
	//生成等高线
	
	CDSM demObj;
	if( !demObj.Open(demName) )
		return FALSE;
	
	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return FALSE;
	int nScale = pDS->GetScale();
	//构建等高线
// 	CFtrLayer *pLayer0 = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
// 	CFtrLayer *pLayer1 = pDS->GetFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
// 	CGeometry *pObj0 = (pLayer0==NULL?NULL:pLayer0->CreateDefaultFeature(nScale)->GetGeometry());
// 	CGeometry *pObj1 = (pLayer1==NULL?NULL:pLayer1->CreateDefaultFeature(nScale)->GetGeometry());
	
	CFtrLayer *pLayer = pDS->GetFtrLayer(fid);
	if (!pLayer)
	{
		pLayer = pDS->CreateFtrLayer(fid);
		if (!pLayer) return FALSE;
		pDS->AddFtrLayer(pLayer);
	}
//	CGeometry *pObj = (pLayer==NULL?NULL:pLayer->CreateDefaultFeature(nScale)->GetGeometry());
	
	CGeoBuilderContourX gbc; 
//	gbc.m_dwParent = 0;
	DEMHEADERINFO dh = demObj.m_demInfo;
//	if( pObj0 && pObj1 )
	{
		double *x = NULL;
		double *y = NULL;
		double *z = NULL;
		int iNum = 0;
		float fContourInterval = fInterval;
		gbc.m_iIndexInterval = 4;
		iNum = demObj.GetPoints(&x,&y,&z,0); 
		if(iNum>20)
		{
			if( gbc.Init(x,y,z,iNum,dh.lfDx,dh.lfDy,fContourInterval) ) //set contour interval here  
			{
				GeoLine *pLine;
				
//				CGeometry *pObj;
// 				long lh0 = pLayer0->GetHandle();
// 				long lh1 = pLayer1->GetHandle();
				
// 				CDpDBVariant var;
// 				var = (long)0;
				
				int nContours = gbc.GetContourNumber(), i, j;
				
				GProgressStart(nContours);
//				AfxGetMainWnd()->SendMessage(FCCM_PROGRESS,-1,nContours);
				pDoc->BeginBatchUpdate();

				for( i=0; i<nContours; i++)
				{
					GProgressStep();

					pLine = NULL;
					if( !gbc.GetLine(i,pLine) || pLine==NULL )continue;
					if( pLine->iPtNum<=1 )continue;
					
					double z0 = 5*fInterval*floor(pLine->gp[0].z/(5*fInterval));
					
// 					//计曲线
// 					if( fabs(pLine->gp[0].z-z0)<1e-4 )
// 						pObj = (pObj0!=NULL?pObj0->Clone():NULL);
// 					//首曲线
// 					else
// 						pObj = (pObj1!=NULL?pObj1->Clone():NULL);

//					CGeometry *pNewObj = pObj->Clone();
					
//					if( !pNewObj )continue;

					CFeature *pFt = pLayer->CreateDefaultFeature(pDS->GetScale());
					if (!pFt) return FALSE;
					CGeometry *pObj = pFt->GetGeometry();
					if (!pObj) return FALSE;
					
					CArray<PT_3DEX,PT_3DEX> expt;
					for( j=0; j<pLine->iPtNum; j++)
					{
						PT_3DEX pt;
						pt.x = pLine->gp[j].x; pt.y = pLine->gp[j].y; pt.z = pLine->gp[j].z;
						pt.pencode = penLine;
						expt.Add(pt);
					}

					pObj->CreateShape(expt.GetData(),expt.GetSize());
					
					
//					pFt->SetGeometry(pNewObj);
					
					pDoc->AddObject(pFt,pLayer->GetID());
					
// 					pObj->GetBind()->SetAttrValue("EXCODE",var,DP_CFT_INTEGER);
// 					pObj->UpdateData(FALSE);
// 					pObj->SetFID(fid);
// 					pDoc->AddObject(pObj,pLine->iLineType==1?lh0:lh1);
				}

				pDoc->EndBatchUpdate();
				
				GProgressEnd();
			}
		}
		
	}

	return TRUE;

}

void CBaseView::OnTestCreateContourCreate()
{
	CString strRegPath = "Config\\TestCreateContour";
	float fInterval =  GetProfileDouble(strRegPath,"Interval",5.0);
	CString strLayers = AfxGetApp()->GetProfileString(strRegPath,"Layers","");

	CDlgDoc *pDoc = GetDocument();
	CString strDemFile;
	Envelope e;

	//计算视图的大地坐标范围
    int		i;
	
	PT_4D pts[4],pts1[4];
	PT_3D pts2[4];
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//赋予合适的影像坐标
	int sx=0, ex=0, sy=0, ey=0;
	
	sx = rcClient.left;
	ex = rcClient.right;
	sy = rcClient.top;
	ey = rcClient.bottom;
	
	pts[0].x = pts[3].x = sx;			
	pts[1].x = pts[2].x = ex;
	pts[0].y = pts[1].y = sy;
	pts[2].y = pts[3].y = ey;
	
	pts[0].z = pts[3].z = pts[0].x;			
	pts[1].z = pts[2].z = pts[1].x;
	pts[0].yr= pts[1].yr= pts[0].y;
	pts[2].yr= pts[3].yr= pts[2].y;
	
	//计算相应的大地坐标
	for ( i=0;i<4; i++)
	{
		ClientToGround(pts+i,pts2+i);
	}

	e.CreateFromPts(pts2,4);

	if( !S_CreateDem(pDoc,strLayers,fInterval*2,e,strDemFile) )
		return;
	if( !S_DemToContours(pDoc,strDemFile,fInterval,"tempContours") )
		return;
	
	::DeleteFile(strDemFile);

	Invalidate(FALSE);
	GOutPut(StrFromResID(IDS_OUTPUT_END));
//	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_OUTPUT_END));

}
#endif

void CBaseView::OnTestCreateContourRemove()
{
	CDlgDoc *pDoc = GetDocument();

	CDlgDataSource *pDS = pDoc->GetDlgDataSource();
	if (!pDS) return;

	int nlaynum = pDS->GetFtrLayerCount();

	//获取实体对象总数
	long lSum = 0;
	for( int i=0; i<nlaynum; i++ )
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if( pLayer&&!pLayer->IsLocked()&&pLayer->IsVisible() )
		{
			lSum += pLayer->GetObjectCount();
		}
	}

	pDoc->BeginBatchUpdate();

	GProgressStart(lSum);
	
	CFeature *pFtr = NULL;
	CString str;
	//遍历所有层
	for ( i=0; i<nlaynum; i++)
	{
		CFtrLayer *pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer||pLayer->IsLocked()||!pLayer->IsVisible()) continue;
		if (stricmp(pLayer->GetName(),"tempContours") != 0) 
			continue;

		int nObjs = pLayer->GetObjectCount();
		if( nObjs>0 )
		{			
			//遍历层中的所有对象
			for(int j=0; j<nObjs; j++)
			{
				GProgressStep();
				pFtr = pLayer->GetObject(j);
				
				if( pFtr )
				{
					pDoc->DeleteObject(FtrToHandle(pFtr));
				}

				//增长进度条
				
			}
		}
	}
	pDoc->RefreshView();
	//进度条复位
	GProgressEnd();

	pDoc->EndBatchUpdate();

	return;
}

void CBaseView::OnRefreshVectoryDisplayOrder()
{
	GetDocument()->UpdateAllViews(NULL,hc_UpdateLayerDisplayOrder);
	OnRefresh();	
}
void CBaseView::OnSnapClick() 
{
	CDlgDoc *pDoc = GetDocument();
	if( !pDoc->GetDlgDataSource() )return;
	
	if( pDoc->m_snap.bOpen() )
	{
		if( m_itemCurSnap.IsValid() )
		{
			CPoint pt;
			::GetCursorPos(&pt);
			OnLButtonDown(0,pt);
			return;
		}
	}
	else
	{
		pDoc->m_snap.Enable(TRUE);
		
		PT_3D pt3d;
		PT_4D cltpt;
		pt3d = m_gCurPoint;
		GetCoordWnd().m_pSearchCS->GroundToClient(&pt3d,&cltpt);
		
// 		pDoc->m_selection.m_bWithSymbol = m_bSymbolize;
// 		pDoc->m_snap.m_bWithSymbol = m_bSymbolize;
		
		//获取上一个点
		CCommand *pCurCmd = pDoc->GetCurrentCommand();
		if (pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)))
		{
			PT_3DEX pt ;
			PT_3D lastPt,*pLastPt=NULL;
			int lastNum = 0;
			DrawingInfo info = ((CDrawCommand*)pCurCmd)->GetCurDrawingInfo();
			lastNum = info.GetLastPts(pLastPt);
			
			Envelope e = EnvelopeFromPtAndRadius(cltpt,pDoc->m_snap.GetSnapRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS());
			
			//判断是否咬中
			//首先判断自身对象
			if( pDoc->m_snap.bOpen() && pDoc->m_snap.bSnapSelf() )
			{
				pDoc->m_snap.PreSnap(e,pLastPt,lastNum,&m_gCurPoint,GetCoordWnd().m_pSearchCS,info.GetDrawGeo());		
// 				CGeometry *pCurObj = info.GetDrawGeo();			
// 				if( pCurObj )
// 				{					
// 					pDoc->m_snap.PreSnap(e,pLastPt,&m_gCurPoint,GetCoordWnd().m_pSearchCS,pCurObj);					
// 				}			
// 				else
// 				{
// 					pDoc->m_snap.PreSnap(e,pLastPt,&m_gCurPoint,GetCoordWnd().m_pSearchCS,NULL);
// 				}
			}

			if( pDoc->m_snap.bOpen() && pDoc->m_snap.DoSnap(e,pLastPt,lastNum,&m_gCurPoint,GetCoordWnd().m_pSearchCS) )
			{
				SNAPITEM item = pDoc->m_snap.GetFirstSnapResult();
				
				m_gCurPoint = item.pt;
				
				CPoint pt;
				::GetCursorPos(&pt);
				OnLButtonDown(0,pt);
				
				m_gCurPoint = pt3d;
			}
		}	
		
		pDoc->m_snap.Enable(FALSE);
	}
}

void CBaseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_nInnerCmd!=0 )
	{
		CSceneViewEx::OnLButtonDown(nFlags, point);
		return;
	}
	
	if( IsPolarized() )
	{
		point.y /= 2;
	}
	
	PT_3D pt3d;
	PT_4D cltpt;
	pt3d = m_gCurPoint;
	GroundToClient(&pt3d,&cltpt);
	
	CDlgDoc *pDoc = GetDocument();
	pDoc->SetDataQueryType(m_bSymbolize);
	if( pDoc->m_accuBox.IsWaitDir() )
	{
		if( pDoc->m_accuBox.ClickDir(pt3d) )
		{
// 			m_bAccuBoxChanged = TRUE;
// 			m_bAccuDrgChanged = TRUE;
			UpdateAccuDrawing();
			CSceneViewEx::OnLButtonDown(nFlags, point);
			return;
		}
	}
	
	pDoc->m_accuBox.Click(pt3d);	

	UpdateAccuDrawing();

	// 纠正在精确绘图开启时拉框选择的问题
	if (pDoc->IsSelectorOpen())
	{
		pDoc->m_accuBox.Reset();
	}

	if (pDoc->m_accuBox.IsOpen())
	{
		CCommand *pCurCmd = pDoc->GetCurrentCommand();
		if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
		{
			AccuRightAngleAdjust(m_gCurPoint);
		}
	}
	
	
	CInteractiveView::OnLButtonDown(nFlags, point);
}

void CBaseView::OnSelectAll() 
{
	GetDocument()->SelectAll();
	GetDocument()->OnSelectChanged();	
}

void CBaseView::OnViewKeypoint() 
{
	m_bShowKeyPoint = !m_bShowKeyPoint;
	
	AfxGetApp()->WriteProfileInt(m_strRegSection,"DisplayKeyPoint",m_bShowKeyPoint);

	{
		m_markLayL.ClearAll();
		m_markLayR.ClearAll();
		
		m_markLayL.SetDrawType(FALSE,TRUE,m_bShowKeyPoint);
		m_markLayR.SetDrawType(FALSE,TRUE,m_bShowKeyPoint);
		m_markLayL.SetMark2Width(1);
		m_markLayR.SetMark2Width(1);
		m_markLayL.SetAllObjsMark2(m_bShowKeyPoint);
		m_markLayR.SetAllObjsMark2(m_bShowKeyPoint);	
	}
	
	Invalidate(FALSE);
}

void CBaseView::OnUpdateViewKeypoint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bShowKeyPoint);
}

void CBaseView::ViewAll()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	for(int i=0;i<pDoc->GetDlgDataSourceCount();i++)
	{
		SetDSVisibleState(i,TRUE);
	}
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();

	Invalidate(FALSE);
}

void CBaseView::ViewHideAll()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	for(int i=0;i<pDoc->GetDlgDataSourceCount();i++)
	{
		SetDSVisibleState(i,FALSE);
	}
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();
	
	Invalidate(FALSE);
}

void CBaseView::ViewLocal()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	

	for (int d=0; d<pDoc->GetDlgDataSourceCount(); d++)
	{
		CDlgDataSource *pDS = pDoc->GetDlgDataSource(d);
		for (int i=0;i<pDS->GetFtrLayerCount();i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer) continue;

			BOOL bViewValue = TRUE;
			m_mapLayerVisible.Lookup(pLayer,bViewValue);
			
			bViewValue &= pLayer->IsVisible();

			if (!bViewValue) continue;	
	
			for(int j=0;j<pLayer->GetObjectCount();j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					const char *code = pFtr->GetCode();
					BOOL bLocal = (code==NULL || strlen(code)<=0);
					m_vectLayL.SetObjVisible((LONG_PTR)pFtr,bLocal);
					m_vectLayR.SetObjVisible((LONG_PTR)pFtr, bLocal);
				}
			}
		}
	}

	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();

	Invalidate(FALSE);
}

void CBaseView::ViewExternal()
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	for (int d=0; d<pDoc->GetDlgDataSourceCount(); d++)
	{
		CDlgDataSource *pDS = pDoc->GetDlgDataSource(d);
		for (int i=0;i<pDS->GetFtrLayerCount();i++)
		{
			CFtrLayer* pLayer = pDS->GetFtrLayerByIndex(i);
			if (!pLayer) continue;	

			BOOL bViewValue = TRUE;
			m_mapLayerVisible.Lookup(pLayer,bViewValue);
			
			bViewValue &= pLayer->IsVisible();
			
			if (!bViewValue) continue;	

			for(int j=0;j<pLayer->GetObjectCount();j++)
			{
				CFeature *pFtr = pLayer->GetObject(j);
				if (pFtr)
				{
					const char *code = pFtr->GetCode();
					BOOL bLocal = (code==NULL || strlen(code)<=0);
					m_vectLayL.SetObjVisible(LONG_PTR(pFtr), !bLocal);
					m_vectLayR.SetObjVisible(LONG_PTR(pFtr), !bLocal);
				}
			}
		}
	}
	
	m_vectLayL.ClearAll();
	m_vectLayR.ClearAll();

	Invalidate(FALSE);
}

void CBaseView::OnViewTypeAll() 
{
	m_ViewType = VIEW_ALL;
	ViewAll();
}


void CBaseView::OnViewTypeLocal() 
{
	m_ViewType = VIEW_LOCAL;
	ViewLocal();
}

void CBaseView::OnViewTypeExternal() 
{
	m_ViewType = VIEW_EXTERNAL;
	ViewExternal();
}

void CBaseView::OnViewTypeAllorlocal() 
{
	if( m_ViewType!=VIEW_ALL )
		OnViewTypeAll();
	else
		OnViewTypeLocal();
}

void CBaseView::OnViewTypeShoworhide()
{
	if (m_ViewType != VIEW_HIDEALL)
	{
		m_ViewType = VIEW_HIDEALL;
		ViewHideAll();
	}
	else
	{
		OnViewTypeAll();
		m_ViewType = VIEW_SHOWALL;
	}
}

void CBaseView::OnUpdateViewType(CCmdUI* pCmdUI) 
{
	if( pCmdUI->m_nID==ID_VIEW_TYPE_ALL )
		pCmdUI->SetCheck(m_ViewType==VIEW_ALL?1:0);
	else if( pCmdUI->m_nID==ID_VIEW_TYPE_LOCAL )
		pCmdUI->SetCheck(m_ViewType==VIEW_LOCAL?1:0);
	else if( pCmdUI->m_nID==ID_VIEW_TYPE_EXTERNAL )
		pCmdUI->SetCheck(m_ViewType==VIEW_EXTERNAL?1:0);
	else if( pCmdUI->m_nID==ID_VIEW_TYPE_SHOWORHIDE )
		pCmdUI->SetCheck(m_ViewType==VIEW_HIDEALL?1:0);
	else
		pCmdUI->SetCheck(0);
}

void CBaseView::SetDSVisibleState(int idx, BOOL bVisible)
{
	CDlgDoc* pDoc = GetDocument();
	if (!pDoc)	
		return;	
	if (idx<0||idx>=pDoc->GetDlgDataSourceCount())
	{
		return;
	}
	CDlgDataSource *pDS = pDoc->GetDlgDataSource(idx);
	CFtrLayer* pLayer = NULL;
	CFeature *pFtr = NULL;
	int j = 0;
	for (int i=0;i<pDS->GetFtrLayerCount();i++)
	{
		pLayer = pDS->GetFtrLayerByIndex(i);
		if (!pLayer) continue;	
		
		BOOL bViewValue = TRUE;
		m_mapLayerVisible.Lookup(pLayer,bViewValue);
		
		bViewValue &= pLayer->IsVisible();
		
		if (!bViewValue) continue;

		for(j=0;j<pLayer->GetObjectCount();j++)
		{
			pFtr = pLayer->GetObject(j);
			if (pFtr)
			{
				m_vectLayL.SetObjVisible(LONG_PTR(pFtr), bVisible);
				m_vectLayR.SetObjVisible(LONG_PTR(pFtr), bVisible);
			}
		}
	}	
}

#include "VDBaseDef.h"
void CBaseView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	static float xs=1.0, zs=1.0;
	static UINT nLastChar = 0;
	VDSEND_DATA data;
	memset(&data,0,sizeof(data));
	
	if( GetKeyState(VK_CONTROL)<0 && GetKeyState(VK_MENU)<0 )
	{
		switch( nChar )
		{
		case '1': data.x = 0.1*xs; SendMessage(WM_DEVICE_MOVE, 0x0008, (LPARAM)&data); break;
		case '2': data.x = -0.1*xs; SendMessage(WM_DEVICE_MOVE, 0x0008, (LPARAM)&data); break;
		case '3': data.y = 0.1*xs; SendMessage(WM_DEVICE_MOVE, 0x0008, (LPARAM)&data); break;
		case '4': data.y = -0.1*xs; SendMessage(WM_DEVICE_MOVE, 0x0008, (LPARAM)&data); break;
		case '5': data.z = 0.1*xs; SendMessage(WM_DEVICE_MOVE, 0x0008, (LPARAM)&data); break;
		case '6': data.z = -0.1*xs; SendMessage(WM_DEVICE_MOVE, 0x0008, (LPARAM)&data); break;
		case '7': 
			{
				xs *= 2;
				break;
			}
		case '8':
			{
				xs /= 2;				
				break;
			}
		}
	}

	if (!(GetKeyState(VK_SHIFT)<0 || GetKeyState(VK_CONTROL)<0 || GetKeyState(VK_MENU)<0))
	{
		switch( nChar )
		{
			
		/*case 'X':
			{
				CDlgDoc *pDoc = GetDocument();
				pDoc->m_accuBox.ClearDir();
				if( pDoc->m_accuBox.m_nRightAnglePlace!=CAccuBox::rightangleIM )
				{
					pDoc->m_accuBox.SetRightAngleMode(CAccuBox::rightangleIM);
					CCommand *pCurCmd = pDoc->GetCurrentCommand();
					if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
					{
						DrawingInfo info = ((CDrawCommand*)pCurCmd)->GetCurDrawingInfo();
						if( !info.pFtr )return;
						
						CArray<PT_3DEX,PT_3DEX> pts;
						pts.Copy(info.pts);
						int size = pts.GetSize();
						//按照直角方式采集
						if (size == 1)
						{
							pDoc->m_accuBox.Click(pts[0]);
						}
						else if( size>=2 )
						{
							pDoc->m_accuBox.Click(pts[size-2]);
							pDoc->m_accuBox.Click(pts[size-1]);
						}
					}
				}
				else
				{
					pDoc->m_accuBox.SetRightAngleMode(CAccuBox::rightangleNone);
				}
				
			}
			break;*/
		case '1':
		case '2':
		case '3':
			{
				if (nLastChar == 'Q')
				{
					int dir = 0;
					if (nChar == '1')
					{
						dir = 1;
					}
					else if (nChar == '2')
					{
						dir = 2;
					}
					else if (nChar == '3')
					{
						dir = 3;
					}
					CDlgDoc *pDoc = GetDocument();
//					pDoc->m_accuBox.LockFirstPt(FALSE);
					pDoc->m_accuBox.LockDir(dir,TRUE);
				}
				
			}
			break;
		default:
			break;
		}
		
		nLastChar = nChar;
	}

	GetDocument()->SetDataQueryType(m_bSymbolize);
	CInteractiveView::OnKeyDown(nChar,nRepCnt,nFlags);
}


void CBaseView::OnRButtonDown(UINT nFlags, CPoint point)
{
	BOOL bShowRBDMenu = GetDocument()->CanShowRBDMenu();
	GetDocument()->SetDataQueryType(m_bSymbolize);
	CInteractiveView::OnRButtonDown(nFlags,point);

	if( bShowRBDMenu )
	{
		CRecentCmd& recent = GetDocument()->m_RecentCmd;

		CString strRecent;
		strRecent.LoadString(IDS_RECENTCOMMAND);

		//更新前面10个命令
		CMenu* pSumMenu = m_menuRBD.GetSubMenu(0);
		while(1)
		{
			UINT state = pSumMenu->GetMenuState(0,MF_BYPOSITION);
			if( state!=0xFFFFFFFF && (state&MF_SEPARATOR)==0 )
			{
				pSumMenu->DeleteMenu(0,MF_BYPOSITION);
			}
			else
			{
				break;
			}
		}

		int nCmd = recent.GetCmdsCount();
		if(nCmd>10)nCmd = 10;

		if(nCmd==0)
		{
			for(int i=0; i<nCmd; i++)
			{
				pSumMenu->InsertMenu(0,MF_BYPOSITION,ID_LASTCOMMAND_1,strRecent + ": --");
			}
		}
		else
		{
			CString strCmd;
			for(int i=0; i<nCmd; i++)
			{
				CRecentCmd::CmdItem item = recent.GetCmdAt(i);
				strCmd.Format("%02d: %s",i+1,item.name);
				pSumMenu->InsertMenu(i,MF_BYPOSITION,ID_LASTCOMMAND_1+i,strCmd);
			}
		}

		ClientToScreen(&point);
		int cmd = pSumMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RETURNCMD,point.x,point.y,this,NULL);	
		if(cmd>=ID_LASTCOMMAND_1 && cmd<=(ID_LASTCOMMAND_1+9) )
		{
			int index = cmd-ID_LASTCOMMAND_1;

			int nCmd = recent.GetCmdsCount();
			if(index>=0 && index<nCmd)
			{
				CRecentCmd::CmdItem item = recent.GetCmdAt(index);

				GetWorker()->CancelCurrentCommand();
				GetWorker()->SetCoordWnd(GetCoordWnd());
				GetWorker()->StartCommand(item.id);
			}
		}
		else
		{
			SendMessage(WM_COMMAND,cmd);
		}
	}
}

void CBaseView::AccuRightAngleAdjust(PT_3D &pt)
{
	CDlgDoc *pDoc = GetDocument();
	BOOL bFindLine = FALSE;
	if( pDoc->m_accuBox.m_nRightAnglePlace==CAccuBox::rightangleIM )
	{
		CCommand *pCurCmd = pDoc->GetCurrentCommand();
		if( pCurCmd && pCurCmd->IsKindOf(RUNTIME_CLASS(CDrawCommand)) )
		{
			DrawingInfo info = ((CDrawCommand*)pCurCmd)->GetCurDrawingInfo();
			
			PT_3D *ex;
			int lastNum = info.GetLastPts(ex);
			//捕捉到最近的节点
			if( lastNum==0 )
			{
				CCoordWnd cw = GetCoordWnd();
				PT_4D cltpt;
				cw.m_pSearchCS->GroundToClient(&pt,&cltpt);
				CPFeature pFtr = pDoc->GetDataQuery()->FindNearestObject(cltpt,pDoc->m_snap.GetSnapRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS(),cw.m_pSearchCS);
				if (!pFtr) return;
				CGeometry *pGeo = pFtr->GetGeometry();
				
				PT_3D ptline0, ptline1;
				if( pGeo )
				{
					double r = pDoc->m_snap.GetSnapRadius()*GetCoordWnd().GetScaleOfSearchCSToViewCS();
					Envelope e = EnvelopeFromPtAndRadius(cltpt,r);
					
					if(pGeo->FindNearestBaseLine(cltpt, e, cw.m_pSearchCS, &ptline0, &ptline1,NULL))
					{
						PT_3D pt2;
						GraphAPI::GGetNearestDisOfPtToLine(ptline0.x,ptline0.y,
							ptline1.x,ptline1.y,pt.x,pt.y,&pt2.x,&pt2.y,false);
						
						pt.x = pt2.x; pt.y = pt2.y;

						PT_3D pt3 = GetCrossPoint();

						//在折角的地方，究竟是前一线段还是后一线段，有歧义
						PT_KEYCTRL kc = pGeo->FindNearestKeyCtrlPt(cltpt,r,cw.m_pSearchCS,1);
						if( kc.type==PT_KEYCTRL::typeKey && kc.index>=0 )
						{
							PT_3D pt4 = pGeo->GetDataPoint(kc.index);
							if( GraphAPI::GIsEqual2DPoint(&pt2,&pt4) )
							{
								BOOL bMidPoint = (kc.index>0 && kc.index<pGeo->GetDataPointSum()-1);
								BOOL bClosedPoint = ((kc.index==0 || kc.index==pGeo->GetDataPointSum()-1) &&
									((pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) && ((CGeoCurve*)pGeo)->IsClosed() && pGeo->GetDataPointSum()>=4)||pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) && pGeo->GetDataPointSum()>=3));
								
								if( bMidPoint )									
								{
									PT_3D pt5 = pGeo->GetDataPoint(kc.index-1);
									PT_3D pt6 = pGeo->GetDataPoint(kc.index+1);

									PT_3D pt7, pt8;
									pt7.x = pt4.x - (pt5.y-pt4.y); pt7.y = pt4.y + (pt5.x-pt4.x);
									pt8.x = pt4.x - (pt6.y-pt4.y); pt8.y = pt4.y + (pt6.x-pt4.x);

									double dis1 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x,pt4.y,
										pt7.x,pt7.y,pt3.x,pt3.y);
									double dis2 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x,pt4.y,
										pt8.x,pt8.y,pt3.x,pt3.y);

									if( dis1<dis2 )
									{
										ptline0 = pt5; ptline1 = pt4;
									}
									else
									{
										ptline0 = pt6; ptline1 = pt4;
									}

								}
								else if( bClosedPoint )
								{
									PT_3D pt5, pt6;

									if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
									{
										pt5 = pGeo->GetDataPoint(pGeo->GetDataPointSum()-2);
										pt6 = pGeo->GetDataPoint(1);
									}
									else
									{
										pt5 = pGeo->GetDataPoint(pGeo->GetDataPointSum()-1);
										pt6 = pGeo->GetDataPoint(1);
									}
									
									PT_3D pt7, pt8;
									pt7.x = pt4.x - (pt5.y-pt4.y); pt7.y = pt4.y + (pt5.x-pt4.x);
									pt8.x = pt4.x - (pt6.y-pt4.y); pt8.y = pt4.y + (pt6.x-pt4.x);
									
									double dis1 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x,pt4.y,
										pt7.x,pt7.y,pt3.x,pt3.y);
									double dis2 = GraphAPI::GGetNearestDisOfPtToLine(pt4.x,pt4.y,
										pt8.x,pt8.y,pt3.x,pt3.y);
									
									if( dis1<dis2 )
									{
										ptline0 = pt5; ptline1 = pt4;
									}
									else
									{
										ptline0 = pt6; ptline1 = pt4;
									}
									
								}
							}
						}
						
						pDoc->m_accuBox.LockDir(ACCU_DIR::perpendicular);
						pDoc->m_accuBox.MoveDir(ptline0,ptline1);
						pDoc->m_accuBox.ClickDir(ptline0);
						
						bFindLine = TRUE;
					}
				}
			}
		}
	}
}



void CBaseView::OnSelectPolygon()
{
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->StartPolygonSelect();
}


void CBaseView::OnUpdateSelectPolygon(CCmdUI* pCmdUI)
{
	int id = GetWorker()->GetCurrentCommandId();
	if (id==0)
	{
		if( GetWorker()->IsSelectorOpen() && GetWorker()->GetSelectorMode()==SELMODE_POLYGON )
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
		pCmdUI->SetCheck(FALSE);
}


void CBaseView::OnSelectRect()
{
	GetWorker()->SetCoordWnd(GetCoordWnd());
	GetWorker()->StartRectSelect();
}


void CBaseView::OnUpdateSelectRect(CCmdUI* pCmdUI)
{
	int id = GetWorker()->GetCurrentCommandId();
	if (id==0)
	{
		if( GetWorker()->IsSelectorOpen() && GetWorker()->GetSelectorMode()==SELMODE_RECT )
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}
	else
		pCmdUI->SetCheck(FALSE);
}



void CBaseView::OnViewSymbolized()
{
	m_bSymbolize = !m_bSymbolize;

	if( IsKindOf(RUNTIME_CLASS(CStereoView)) )
	{
		AfxGetApp()->WriteProfileInt(REGPATH_VIEW_IMG,REGITEM_SYMBOLIZE,m_bSymbolize);
	}
	else
	{
		AfxGetApp()->WriteProfileInt(REGPATH_VIEW_VECT,REGITEM_SYMBOLIZE,m_bSymbolize);
	}

	OnUpdate(NULL,hc_UpdateAllObjects,NULL);
}


void CBaseView::OnUpdateViewSymbolized(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bSymbolize);
}


void CBaseView::OnDestroy()
{
	m_vectLayL.Destroy();
	m_vectLayR.Destroy();
	
	m_markLayL.Destroy();
	m_markLayR.Destroy();
	
	m_accurboxLayL.Destroy();
	m_accurboxLayR.Destroy();
	
	m_snapLayL.Destroy();
	m_snapLayR.Destroy();

	m_vectLayL.SetContext(NULL);
	m_vectLayR.SetContext(NULL);
	
	m_markLayL.SetContext(NULL);
	m_markLayR.SetContext(NULL);
	
	m_accurboxLayL.SetContext(NULL);
	m_accurboxLayR.SetContext(NULL);
	
	m_snapLayL.SetContext(NULL);
	m_snapLayR.SetContext(NULL);

	CInteractiveView::OnDestroy();
}
namespace
{
	struct ptItem
	{
		ptItem()
		{
			data = 0;	
		}
		CString strLayName;
		PT_3D pt;
		CFeature* pFtr;
		int idx;
		int data;//内部使用
	};
	struct gridItem
	{
		CArray<ptItem,ptItem&> arrEndPts;
		CArray<ptItem,ptItem&> arrVertexs;
	};
	struct psePtItem
	{
		PT_3D pt;
		CFeature* pFtr0;
		int idx0;
		CFeature* pFtr1;
		int idx1;
	};
	void DrawMarkBoxPt(GrBuffer &buf, const PT_3D &pt, COLORREF clr = RGB(255,0,0), double width = 5.0)
	{
		PT_3D temp(pt);
		buf.BeginLineString(clr,0);
		temp.x -= width/2;
		temp.y -= width/2;
		buf.MoveTo(&temp);
		temp.x += width;	
		buf.LineTo(&temp);
		temp.y += width;	
		buf.LineTo(&temp);
		temp.x -= width;	
		buf.LineTo(&temp);
		temp.y -= width;	
		buf.LineTo(&temp);
		buf.End();
	}
}



void CBaseView::OnChangeCoordSys(BOOL bJustScroll)
{
	GetCoordWnd();

	m_markLayL.OnChangeCoordSys(bJustScroll);
	m_markLayR.OnChangeCoordSys(bJustScroll);
}

BOOL CBaseView::CanSymbolized(CFeature *pFtr)
{
	if(!m_bSymbolize)
		return FALSE;

	CGeometry *pGeo = pFtr->GetGeometry();
	
	if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
		return m_bSymbolizePoint;

	if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
		return m_bSymbolizeCurve;
	
	if(pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		return m_bSymbolizeSurface;
  
	return TRUE;
}



void CBaseView::OnSwitchSymbolizePoint()
{
	m_bSymbolizePoint = !m_bSymbolizePoint;

	OnUpdate(NULL,hc_UpdateAllObjects_VectorView,NULL);	
}


void CBaseView::OnSwitchSymbolizeLine()
{
	m_bSymbolizeCurve = !m_bSymbolizeCurve;

	OnUpdate(NULL,hc_UpdateAllObjects_VectorView,NULL);	
}


void CBaseView::OnSwitchSymbolizeSurface()
{
	m_bSymbolizeSurface = !m_bSymbolizeSurface;

	OnUpdate(NULL,hc_UpdateAllObjects_VectorView,NULL);	
}


void CBaseView::OnSwitchSymbolizeAnnotation()
{
	CAnnotation::m_bNotDisplayAnnot = !CAnnotation::m_bNotDisplayAnnot;

	OnUpdate(NULL,hc_UpdateAllObjects_VectorView,NULL);	
}


void CBaseView::OnUpdateSwitchSymbolizePoint(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bSymbolize && m_bSymbolizePoint);
}

void CBaseView::OnUpdateSwitchSymbolizeLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bSymbolize && m_bSymbolizeCurve);
}

void CBaseView::OnUpdateSwitchSymbolizeSurface(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bSymbolize && m_bSymbolizeSurface);
}

void CBaseView::OnUpdateSwitchSymbolizeAnnotation(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bSymbolize && !CAnnotation::m_bNotDisplayAnnot);
}
