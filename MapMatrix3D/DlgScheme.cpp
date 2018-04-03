// DlgScheme.cpp : implementation file
//

#include "stdafx.h"
#include "Symbol.h"
// #include <algorithm>//hcw,2012.4.5
// #include <functional>//hcw,2012.4.5

//#include "DlgSchemeSymbols.h"//hcw,2012.2.29
#include "editbase.h"
#include "DlgScheme.h"
#include "DlgNewGroup.h"
#include "DlgNewLayer.h"
#include "DlgScale.h"
#include "textfile.h"
#include "ExMessage.h"
#include "DlgCellSetting.h"
#include "DlgCellHatchSetting.h"
#include "DlgCellLineSetting.h"
#include "DlgColorHatchSetting.h"
#include "DlgDashLineSetting.h"
#include "DlgScaleLineSetting.h"
#include "DlgAnnotationSetting.h"
#include "DlgParalineSetting.h"
#include "DlgAngBisectorSetting.h"
#include "DlgScaleArcSetting.h"
#include "dlgscaleturnplatesetting.h"
#include "dlgscalecranesetting.h"
#include "dlgscalefunnelsetting.h"
#include "GeoParallel.h"
#include "SmartViewFunctions.h"
#include "DlgSupportGeoClassName.h"
#include "DlgCellDefLinetypeView.h"
#include "SymbolBase.h"
#include "Matrix.h"
#include "GeoText.h"
#include "DlgLinefillSetting.h"
#include "DlgDiagonalSetting.h"
#include "DlgClearLineType.h"
#include "CollectionViewBar.h"
#include "editbasedoc.h"
#include "DlgDataSource.h"
#include "dlgScaleCellLineSetting.h"
#include "dlgProcSymLTSetting.h"
#include "dlgscaleChuanSongDaisetting.h"
#include "DlgScaleCellSetting.h"
#include "DlgScaleOldDouyaSettings.h"
#include "DlgScaleXiepo.h"
#include "DlgTidalWaterSetting.h"
#include "export\shapefil.h"
#include "DlgConditionColorHatch.h"
#include "DlgExportCodeTable.h"
#include "DlgCulvertSurfaceSetting.h"
#include "Markup.h"
#include "ObjectXmlIO.h"
#include "DlgExportArcgisMdb.h"

#undef UNICODE
#undef _UNICODE


//extern void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol);

#define TIMERID_CREATEIMAGE	1
#define TIMERID_PREVIEW		2

#define ID_LIANGCANG_OLD               51534		//粮仓
#define ID_YOUGUAN_OLD                 51535		//油罐
#define ID_JIANFANGWU_OLD              51536		//简单房屋
#define ID_TONGCHE_SHUIZHA_OLD         51537		//通车水闸
#define ID_BUTONGCHE_SHUIZHA_OLD       51538		//不通车水闸
#define ID_DISHANGYAODONG_OLD		   51539		//地上窑洞

#define ID_SCALE_CELLLINETYPE	       51540		//依比例图元线型
#define ID_SCALE_CHURUKOU_OLD		   51541		//依比例地表出入口

#define ID_SCALE_CELL				   51542		//依比例图元
#define ID_SCALE_CHUANSONGDAI_OLD      51543        // 传送带
#define ID_SCALE_OLDDOUYA			   51544		//依比例陡崖
#define ID_SCALE_XIEPO				   51545		//依比例斜坡
#define ID_SCALE_WENSHI				   51546		//依比例温室
#define ID_TIDALWATER				   51547		//潮水沟
#define ID_COLORHATCH_CONDITION		   51548		//关联属性的颜色填充

#define ID_CULVERTSURFACE1             51550        //涵洞面1
#define ID_CULVERTSURFACE2             51551        //涵洞面2

#define IDC_LIST_NODENAME              51514

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgScheme dialog

//{hcw,2012.4.5
BOOL less_layCode(const IDX& idx1, const IDX& idx2){
return idx1.code<idx2.code;
 }
//}
CDlgScheme::CDlgScheme(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgScheme::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgScheme)
	m_strSupportGeoName = _T("");
	//}}AFX_DATA_INIT
// 	m_config.pScheme = NULL;
// 	m_config.pCellDefLib = NULL;
// 	m_config.pLinetypeLib = NULL;

	m_pCellDlg = NULL;
	m_pDashLineDlg = NULL;
	m_pCellLineDlg = NULL;
	m_pColorHatchDlg = NULL;
	m_pCondColorHatchDlg = NULL;
	m_pCellHatchDlg = NULL;
	m_pAnnoDlg = NULL;
	m_pScaleDlg = NULL;
	m_pLinefillDlg = NULL;
	m_pDiagonalDlg = NULL;
	m_pParalineDlg = NULL;
	m_pAngBisectorDlg = NULL;
	m_pScaleArcDlg = NULL;
	m_pScaleTurnplateSetting = NULL;
	m_pScaleCraneSetting = NULL;
	m_pScaleFunnelSetting = NULL;
	m_pScaleCellLTDlg = NULL;
	m_pScaleChuanSongDaiSetting = NULL;
	m_pScaleCellDlg = NULL;
	m_pScaleYouGuanDlg = NULL;
	m_pCulvertSurfaceDlg = NULL;

	m_bChgFromSelect = FALSE;
	m_bCanSelectDefault = FALSE;
	m_bAddSymbol = FALSE;
	m_bModified = FALSE;
	m_bInitialSelected = TRUE;//hcw,2012.3.26,初始化时选中了某个节点
	m_bSchemeDlgInitialized = FALSE;//hcw,20125.3.29

	m_nClickNum = 0;
	m_nCountTreeSelected = 0;//hcw,2012.3.15
	m_nSelSymbol = -1;//hcw,2012-2-13 
	m_binSymbolListRegion = FALSE;//hcw,2012-2-14
	m_bSymbolSelected = FALSE;//hcw,2012-2-14
	m_bUpDown = FALSE ;//HCW,2012.2.27
	m_bReturn = FALSE; //hcw,2012.2.27
	m_bLButtnUp = FALSE; //hcw,2012.2.27
	m_bFirstinSearchEdit = FALSE;//hcw,2012.3.6
	m_bShift = FALSE;//hcw,2012.3.15
	m_bCtrl = FALSE ; //hcw,2012.3.15
	m_bFirstDrawPreview = FALSE;//hcw,2012.3.29.
 	//m_nConfigIndex = -1;
	m_nLayerIndex = -1;
	m_nSelinSearchRsltList = -1;//hcw,2012.4.8.
	m_bSpecialLayer = FALSE;
	m_bEditLabel = FALSE;//hcw,2012.4.9
	m_strSupportGeoName = "";
	m_nCurDocScale = 0;
	m_nCurSchemeScale = 0;//hcw,2012.7.18
	m_nMovedGroup = -1;
	m_nMovedLayer = -1;

	m_bClickNewGroup = FALSE;
	m_bAbsentSchemePath = FALSE;//hcw,2012.7.26,方案路径是否存在。

	m_pDoc = NULL;

	m_pProcSymLTSetting = NULL;

	m_pScaleOldDouyaDlg = NULL;
	m_pScaleXiepoDlg = NULL;
	m_pTidalWaterDlg = NULL;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_fPreviewSize = 1;
	m_strOldPath = "";
	m_strNewPath = "";
}

CDlgScheme::~CDlgScheme()
{
	ReleaseDlgMemory();
}

LRESULT CDlgScheme::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_SCHEME_MODIFY)
	{
		return 1;
	}
	else 
		return CDialog::DefWindowProc(message,wParam,lParam);
}



void CDlgScheme::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgScheme)
	DDX_Control(pDX, IDC_COLOR_STATIC, m_Color);
	DDX_Control(pDX, IDC_DIAGONAL_BUTTON, m_diagonalButton);
	DDX_Control(pDX, IDC_LINEFILL_BUTTON, m_linefillButton);
	DDX_Control(pDX, IDC_SYMBOL_LIST, m_wndListSymbol);
	DDX_Control(pDX, IDC_DEFAULTGEO_COMBO, m_defaultgeoCombo);
	DDX_Control(pDX, IDC_DBGEO_COMBO, m_dbgeoCombo);
	DDX_Control(pDX, IDC_SCALE_COMBO, m_scaleCombo);
	DDX_Control(pDX, IDC_DELETESYMBOL_BUTTON, m_delsymbolButton);
	DDX_Control(pDX, IDC_COLORHATCH_BUTTON, m_colorhatchButton);
	DDX_Control(pDX, IDC_SCALELINE_BUTTON, m_scalelineButton);
	DDX_Control(pDX, IDC_ANNOTATION_BUTTON, m_annButton);
	DDX_Control(pDX, IDC_CELLHATCH_BUTTON, m_cellhatchButton);
	DDX_Control(pDX, IDC_CELLLINE_BUTTON, m_celllineButton);
	DDX_Control(pDX, IDC_DASHLINE_BUTTON, m_dashlineButton);
	DDX_Control(pDX, IDC_CELL_BUTTON, m_cellButton);
	DDX_Control(pDX, IDC_SLIDER_SIZE, m_wndSizeCtrl);
	DDX_Text(pDX, IDC_SUPPORTGEONAME_EDIT, m_strSupportGeoName);
	DDX_Text(pDX, IDC_EDIT_DBLAYNAME, m_strDBLayName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgScheme, CDialog)
	//{{AFX_MSG_MAP(CDlgScheme)
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_COLLECTVIEW_EDIT, OnChangeEditFcode)
	ON_CBN_SELCHANGE(IDC_SCALE_COMBO, OnSelchangeScaleCombo)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(TVN_SELCHANGED,IDC_COLLECTVIEW_TREE,OnSelChanged)
	ON_NOTIFY(TVN_SELCHANGING,IDC_COLLECTVIEW_TREE, OnSelItem)//hcw,2012.3.15
	ON_LBN_SELCHANGE(IDC_LIST_NODENAME, OnListSelChanged)
	
	ON_NOTIFY(TVN_BEGINLABELEDIT,IDC_COLLECTVIEW_TREE, OnBeginlabeledit)
	ON_NOTIFY(TVN_ENDLABELEDIT,IDC_COLLECTVIEW_TREE, OnEndlabeledit)	
	ON_COMMAND(ID_NEW_GROUP, OnNewGroup)
	ON_COMMAND(ID_NEW_LAYER, OnNewLayer)
	ON_COMMAND(ID_DEL_GROUP, OnDelGroup)
	ON_COMMAND(ID_DEL_LAYER, OnDelLayer)
	ON_COMMAND(ID_SCHEME_NEW, OnNewScheme)
	ON_COMMAND(ID_SETSCHEME_PATH, OnSetSchemePath)
	ON_COMMAND(ID_SCHEME_SAVE, OnSaveScheme)
	ON_MESSAGE(WM_SCHEME_MODIFY,OnSchemeModify)
	ON_MESSAGE(WM_SYMBOL_REPAINT,OnSymbolRepaint)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddAttButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON, OnDelAttButton)
	ON_BN_CLICKED(IDC_PASTE_BUTTON, OnPasteAttButton)
	ON_BN_CLICKED(IDC_COPY_BUTTON, OnCopyAttButton)
	ON_BN_CLICKED(IDC_UP_BUTTON, OnUpAttButton)
	ON_BN_CLICKED(IDC_DOWN_BUTTON, OnDownAttButton)
	ON_BN_CLICKED(IDC_COPYTO_OTHERDEFINES, OnCopyToOtherLayer)
	ON_MESSAGE(WM_VALIDATE, OnEndLabelEditVariableCriteria)
	ON_MESSAGE(WM_SET_ITEMS, PopulateComboList)
	ON_BN_CLICKED(IDC_COLOR_STATIC, OnColorStatic)
	ON_CBN_SELCHANGE(IDC_DEFAULTGEO_COMBO, OnSelchangeDefaultgeoCombo)
	ON_CBN_SELCHANGE(IDC_DBGEO_COMBO, OnSelchangeDbgeoCombo)
	ON_BN_CLICKED(IDC_CELL_BUTTON, OnCellButton)
	ON_BN_CLICKED(IDC_DASHLINE_BUTTON, OnDashlineButton)
	ON_BN_CLICKED(IDC_CELLLINE_BUTTON, OnCelllineButton)
	ON_BN_CLICKED(IDC_CELLHATCH_BUTTON, OnCellhatchButton)
	ON_BN_CLICKED(IDC_ANNOTATION_BUTTON, OnAnnotationButton)
	ON_BN_CLICKED(IDC_COLORHATCH_BUTTON, OnColorhatchButton)
	ON_BN_CLICKED(IDC_SCALELINE_BUTTON, OnScalelineButton)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SYMBOL_LIST, OnSelchangeSymbolList)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DELETESYMBOL_BUTTON, OnDeletesymbolButton)
	ON_BN_CLICKED(IDC_LAYERPREVIEW_STATIC, OnClickStatic)
	ON_COMMAND(ID_MOVE_GROUP, OnMoveGroup)
	ON_COMMAND(ID_MOVETO_GROUP, OnMoveToGroup)
	ON_COMMAND(ID_MOVE_LAYER, OnMoveLayer)
	ON_COMMAND(ID_MOVETO_LAYER, OnMoveToLayer)
	ON_COMMAND(ID_COPY_SYMBOL,OnCopySymbols)
	ON_COMMAND(ID_PASTE_SYMBOL,OnPasteSymbols)
	ON_COMMAND(ID_MOVTO_TOP,OnMovetoTop)
	ON_COMMAND(ID_MOVTO_TAIL,OnMovetoTail)	
	ON_COMMAND(ID_RESET_DEFAULTCOMMAND, OnResetDefaultcommand)
	ON_COMMAND(ID_RESET_ALLDEFAULTCOMMAND, OnResetAlldefaultcommand)
	ON_COMMAND(ID_RESET_LAYER_DEFAULTVALUES,OnResetAllLayerDefaultValues)
	ON_COMMAND(ID_RESET_LAYER_PLACE_LINE, OnResetLayerPlaceLine)
	ON_COMMAND(ID_RESET_LAYER_PLACE_AREA, OnResetLayerPlaceArea)
	ON_BN_CLICKED(IDC_LINEFILL_BUTTON, OnLinefillButton)
	ON_BN_CLICKED(IDC_DIAGONAL_BUTTON, OnDiagonalButton)
	ON_BN_CLICKED(IDC_SYMBOLS_BUTTON,OnSpecialSymbolsButton)
	ON_COMMAND(ID_RESET_ALLLAYER_ORDER, OnResetAllLayerOrder)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_SORT_IDCODE, OnSortIdcode)
	ON_COMMAND(ID_NEW_SYMLAYER, OnNewLayer)
	ON_COMMAND(ID_DEL_SYMLAYER, OnDelLayer)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_CLEAR_LINETYPE, OnClearLinetype) //hcw,2012.2.21,
	ON_COMMAND(ID_IMPORTLAYDEF_SHP, OnImportLayDefFromShp)
	ON_COMMAND(ID_IMPORTLAYDEF_MDB, OnImportLayDefFromMdb)
	ON_COMMAND(ID_IMPORTLAYDEF_MDB_REP, OnImportLayDefFromMdbRep)
	ON_EN_CHANGE(IDC_EDIT_DBLAYNAME, OnChangeDBLayName)
	ON_COMMAND(ID_TOOL_EXPORT_CODETAB_SYMBOLS, OnExportCodeTable_Symbols)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgScheme message handlers

BOOL CDlgScheme::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon	

	EnableToolTips(TRUE);
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);
	
	m_ToolTip.AddTool(GetDlgItem(IDC_CELL_BUTTON),StrFromResID(IDS_CELL));
	m_ToolTip.AddTool(GetDlgItem(IDC_DASHLINE_BUTTON),StrFromResID(IDS_DASH_LINETYPE));
	m_ToolTip.AddTool(GetDlgItem(IDC_CELLLINE_BUTTON),StrFromResID(IDS_CELL_LINETYPE));
	m_ToolTip.AddTool(GetDlgItem(IDC_COLORHATCH_BUTTON),StrFromResID(IDS_COLOR_HATCH));
	m_ToolTip.AddTool(GetDlgItem(IDC_CELLHATCH_BUTTON),StrFromResID(IDS_CELL_HATCH));
	m_ToolTip.AddTool(GetDlgItem(IDC_ANNOTATION_BUTTON),StrFromResID(IDS_ANNOTATION));
	m_ToolTip.AddTool(GetDlgItem(IDC_SCALELINE_BUTTON),StrFromResID(IDS_SCALE_LINETYPE));
	m_ToolTip.AddTool(GetDlgItem(IDC_LINEFILL_BUTTON),StrFromResID(IDS_LINEFILL));
	m_ToolTip.AddTool(GetDlgItem(IDC_DIAGONAL_BUTTON),StrFromResID(IDS_DIAGONAL));
	//m_ToolTip.AddTool(GetDlgItem(IDC_CELLSCALE_BUTTON),StrFromResID(IDS_SCALE_CELLTYPE));
	m_ToolTip.AddTool(GetDlgItem(IDC_DELETESYMBOL_BUTTON),StrFromResID(IDS_DELETE_SYMBOL));

	m_ToolTip.SetTipTextColor(RGB(125,125,125)); 
	m_ToolTip.SetDelayTime(150);   	

	if ( !CreateCtrls() )
		return FALSE;
	
	FillScaleCombo();
	FillTree();

	CString geo[8] = {StrFromResID(IDS_POINT),StrFromResID(IDS_DIRPOINT),StrFromResID(IDS_LINE),StrFromResID(IDS_GEO_DCURVE),StrFromResID(IDS_PARLINE),StrFromResID(IDS_SURFACE),StrFromResID(IDS_TEXT), StrFromResID(IDS_NO_EDB)};
	int nCls[8] = {CLS_GEOPOINT,CLS_GEODIRPOINT,CLS_GEOCURVE,CLS_GEODCURVE,CLS_GEOPARALLEL,CLS_GEOSURFACE,CLS_GEOTEXT,CLS_NULL};
	
	int dbIndex = 0;
	for (int i=0; i<8; i++)
	{
		if(i!=7)
		{
			m_defaultgeoCombo.AddString(geo[i]);
			m_defaultgeoCombo.SetItemData(i,nCls[i]);
		}

		if (i == 0 || i == 2 || i == 5 || i == 6 || i == 7)
		{
			m_dbgeoCombo.AddString(geo[i]);
			m_dbgeoCombo.SetItemData(dbIndex,nCls[i]);
			dbIndex++;
		}
		
	}

	m_defaultgeoCombo.SetCurSel(-1);
	m_dbgeoCombo.SetCurSel(-1);

	m_Color.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
	m_wndSizeCtrl.SetRange(0,100);
	m_wndSizeCtrl.SetPos(50);
	m_wndSizeCtrl.SetLineSize(1);
	m_wndSizeCtrl.SetPageSize(10);
	m_wndSizeCtrl.RedrawWindow();
	
	DrawPreview(m_nLayerIndex);
	
	FillAttList(m_nLayerIndex);
	
	FillColor();
	
	FillGeo();
	
	FillSymbolList();
	//{hcw,2012.7.26
	if (m_bAbsentSchemePath)
	{
		return FALSE;
	}
	//m_nLayerIndex= 10;
	// 支持开始选择指定层
	if (m_nLayerIndex < 0)
	{
		m_bFirstinSearchEdit = TRUE;//hcw,2012.4.10
		m_defaultgeoCombo.EnableWindow(FALSE);
		m_dbgeoCombo.EnableWindow(FALSE);
		return TRUE;
	}

	//}
	// 定位到选定的层
	CString strcode;
	strcode.Format("%I64d",m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetLayerCode());
	m_bCanSelectDefault = TRUE;
	m_wndCodeEdit.SetWindowText(strcode);
	m_CurStrCode = strcode;//hcw,2012.4.8
	m_wndSearchRsltList.SetWindowText(strcode);	//HCW,2012.2.26
	m_bCanSelectDefault = FALSE;
	//{hcw,2012.3.29
	if(!m_bSchemeDlgInitialized)
	{
		SetTreeNodeToTop(strcode);
		
		m_bSchemeDlgInitialized = TRUE;
	}
	//}
	return TRUE;

}





/////////////////////////////////////////////////////////////////////////////
//call back function for the SHBrowseForFolder(), just to set the initial path for the dialog.
int CALLBACK MyBrowseCallbackProc(HWND hwnd, UINT uMsg,	LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
	case BFFM_INITIALIZED:
		::SendMessage( hwnd, BFFM_SETSELECTION, TRUE, lpData );
		break;
	default:
		break;
	}
	
	return 0;
}

/*
browse the folder(not include the file name) in explorer-like style.
the return value indicate whether this operation succeed or not.
szTitle gives the title of browse dialog, szInitDir gives the initial directory when this dialog open,
strPath retrives the path selected.
*/


BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
					LPTSTR lpszPath,		// [out] 返回的文件路径
					LPCTSTR lpszInitDir,	// [in] 初始文件路径
					HWND hWnd,				// [in] 父系窗口
					BOOL bNetwork,			// [in] 限制在网络路径范围内
					UINT ulFlags)			// [in] 设置风格
					
{
	LPMALLOC pMalloc;
	BOOL     bResult = FALSE;
	
	// Get's the Shell's default allocator
	if(::SHGetMalloc(&pMalloc) == NOERROR)
    {
		BROWSEINFO bi;
		char szBuffer[MAX_PATH];
		LPITEMIDLIST pidl;
		LPITEMIDLIST pidlRoot=NULL;
		if( bNetwork )
			::SHGetSpecialFolderLocation( hWnd,CSIDL_NETWORK,&pidlRoot );
		
		bi.hwndOwner = hWnd;
		bi.pidlRoot = pidlRoot;
		bi.pszDisplayName = szBuffer;
		bi.lpszTitle = lpszTitle;
		bi.ulFlags = ulFlags;// BIF_EDITBOX;
        if( NULL==lpszInitDir || 0==strlen(lpszInitDir) )
		{
			bi.lpfn = NULL;
			bi.lParam = NULL;
		}
		else
		{
			bi.lpfn = MyBrowseCallbackProc;
			bi.lParam = LPARAM(lpszInitDir);
		}
		
		// This next call issues the dialog box
		if((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if(::SHGetPathFromIDList(pidl, szBuffer))
			{
				//At this point pszBuffer contains the selected path
				strcpy( lpszPath,szBuffer );
				bResult = TRUE;
			}
			// Free the PIDL allocated by SHBrowseForFolder
			pMalloc->Free(pidl);
		}
		if( pidlRoot )
			pMalloc->Free( pidlRoot );
    }
	// Release the shell's allocator
	pMalloc->Release();
	
	return bResult;
}

BOOL CopyFolder(CString fromPath,CString toPath)
{
	
	CFileFind finder;
	BOOL   bWorking = finder.FindFile( LPCTSTR(fromPath+"\\*.*") );   
    while(bWorking)   
    {   
        bWorking = finder.FindNextFile(); 
        if(finder.IsDots()) 
			continue;				//如果查到文件为   [.]   或   [..]   则忽略   
        if(finder.IsDirectory())	//是文件夹的情况，
        { 
			CString folderpath = finder.GetFilePath();
			char tmp[MAX_PATH];
			ZeroMemory(tmp,MAX_PATH);
			strcpy(tmp,folderpath.GetBuffer( folderpath.GetLength() ));
			SHFILEOPSTRUCT sfo;
			sfo.hwnd = NULL;
			sfo.wFunc = FO_COPY;
			sfo.pFrom = tmp;
			sfo.pTo = toPath.GetBuffer( toPath.GetLength() );
			sfo.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR ;
			SHFileOperation(&sfo);
			folderpath.ReleaseBuffer();
			toPath.ReleaseBuffer();
            continue;
        } 
        else
		{
			CString filename=finder.GetFileName();
			CopyFile(finder.GetFilePath(),toPath+"\\"+filename,false);
		}
        
	}
	finder.Close();
	return TRUE;
    

}

int CDlgScheme::CreateNewScale(int uScale)
{
	CString strTemplate = (CString)gpCfgLibMan->GetPath()+"\\Template";

	if (!PathFileExists(strTemplate))
	{
		return -1;
	}

	CString strScale;
	strScale.Format("%d",uScale);
	CString strNewScheme = (CString)gpCfgLibMan->GetPath()+"\\"+strScale;

	if (!PathFileExists(strNewScheme) && !CreateDirectory(strNewScheme,NULL))
	{
		return -1;
	}	
	
	CopyFolder(strTemplate,strNewScheme);

	return gpCfgLibMan->LoadScale(strNewScheme,TRUE);

}

void CDlgScheme::OnNewScheme()
{
	CDlgScale dlg;
	if (dlg.DoModal() != IDOK)
		return;
	UINT uScale = dlg.GetScale();

	int nItemIndex = CreateNewScale(uScale);

	if (nItemIndex < 0)
	{
		AfxMessageBox(StrFromResID(IDS_NEWSCHEME_ERROR),MB_OK|MB_ICONASTERISK);
		return;
	}

	m_config = gpCfgLibMan->GetConfigLibItem(nItemIndex);

// 	m_config.pScheme = conLibItem.pScheme;
// 	m_config.pCellDefLib = conLibItem.pCellDefLib;
// 	m_config.pLinetypeLib = conLibItem.pLinetypeLib;

	for(int j=0; j<m_scaleCombo.GetCount(); j++)
	{
		CString data;
		m_scaleCombo.GetLBText(j,data);
		DWORD dScale = atoi(data);
		if(uScale < dScale)
			break;
	}
	CString strScale;
	strScale.Format("%d",uScale);
	int nComboIndex = m_scaleCombo.InsertString(j,strScale);
	m_scaleCombo.SetItemData(nComboIndex,nItemIndex);
	m_scaleCombo.SetCurSel(nComboIndex);

	FillTree();

	ForBidDel();

//	m_bModified = TRUE;


}

BOOL CDlgScheme::IsModify()
{
	return m_bModified;
}

void CDlgScheme::ForBidDel()
{
	return;
	int nComboIndex = m_scaleCombo.GetCurSel();
	if (nComboIndex == CB_ERR) return;

	CString strScale;
	m_scaleCombo.GetLBText(nComboIndex,strScale);

	if (m_nCurDocScale == atoi(strScale))
	{
		CWnd *pWnd = GetDlgItem(IDC_DEL_BUTTON);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
		pWnd = GetDlgItem(IDC_DELETESYMBOL_BUTTON);
		if (pWnd)
		{
			pWnd->EnableWindow(FALSE);
		}
	}
	else
	{
		CWnd *pWnd = GetDlgItem(IDC_DEL_BUTTON);
		if (pWnd)
		{
			pWnd->EnableWindow(TRUE);
		}
		pWnd = GetDlgItem(IDC_DELETESYMBOL_BUTTON);
		if (pWnd)
		{
			pWnd->EnableWindow(TRUE);
		}
	}
	
}

void CDlgScheme::OnClose()
{
	CString szText,szCaption;
	szCaption.LoadString(IDS_ATTENTION);
	int nResult;
	if (m_bModified)
	{
		szText.LoadString(IDS_SAVE_SCHEME);
		nResult = MessageBox(szText,szCaption,MB_YESNOCANCEL|MB_ICONASTERISK);
	}
	else
	{
		szText.LoadString(IDS_CONFIRM_CLOSE);
		nResult = MessageBox(szText,szCaption,MB_OKCANCEL|MB_ICONASTERISK);
	}
	
	if (nResult == IDCANCEL)  return;

	//提示保存
	if (m_bModified)
	{
		if (nResult == IDYES)
		{
			// 更新组的顺序
			m_config.pScheme->m_strSortedGroupName.RemoveAll();
			for (int i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
			{
				m_config.pScheme->m_strSortedGroupName.Add(m_UserIdx.m_aGroup.GetAt(i).GroupName);
			}
			m_config.pScheme->Save();
			SaveCellLinebyScale();

			// 修改颜色
			if (m_pDoc)
			{
				CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
				if (pDS)
				{
					for(int i=0; i<m_arrModifyLayerColor.GetSize(); i++)
					{
						CFtrLayer *pFtrLayer = pDS->GetFtrLayer(m_arrModifyLayerColor[i].name);
						if (pFtrLayer)
						{
							pFtrLayer->SetColor(m_arrModifyLayerColor[i].color);
							m_pDoc->UpdateFtrLayer(pFtrLayer);
						}
					}
					m_arrModifyLayerColor.RemoveAll();
				}
			}

		}
		else
		{
			// 停止CCollectViewBar更新，防止方案重新加载出现的bug
			AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,0,0);
			m_config.pScheme->ReadFrom(m_BackupCurSchemeXML);	
			m_config.pScheme->LoadCADSymols();

			m_bModified = FALSE;

		}

		if (m_pDoc)
		{
			ConfigLibItem *pData = &gpCfgLibMan->GetConfigLibItemByScale(m_pDoc->GetDlgDataSource()->GetScale());
			AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,WPARAM(0),LPARAM(pData));
		}
	}

	gpCfgLibMan->ResetConfig();

	KillTimer(TIMERID_CREATEIMAGE);
	KillTimer(TIMERID_PREVIEW);
	CDialog::OnClose();

}

void CDlgScheme::SaveCellLinebyScale()
{
	for (int i=0; i<m_configsbyScale.GetSize(); i++)
	{
		m_configsbyScale.GetAt(i).SaveCellLine();
	}
	return;
}

void CDlgScheme::OnSetSchemePath()
{
	// 先保存当前更改的比例尺
	if(m_config.GetScale()>0 && m_bModified)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_SAVE_SCHEME);
		szCaption.LoadString(IDS_ATTENTION);
		if(MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) == IDYES)
		{
			// 更新组的顺序
			m_config.pScheme->m_strSortedGroupName.RemoveAll();
			for (int i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
			{
				m_config.pScheme->m_strSortedGroupName.Add(m_UserIdx.m_aGroup.GetAt(i).GroupName);
			}
			
			m_config.pScheme->Save();
		}
		else
		{
			m_config.pScheme->ReadFrom(m_BackupCurSchemeXML);
			m_config.pScheme->LoadCADSymols();
		}

// 		m_config.pScheme->Save();
 		m_bModified = FALSE;
	}	

	char szPath[MAX_PATH];//存放打开的文件夹路径
	CString strInitDir = GetConfigPath();
	int len = strInitDir.GetLength();
	if( len>0 && strInitDir[len-1]=='\\' )
		strInitDir.SetAt(len-1,0);
	m_strOldPath = strInitDir;
	m_strNewPath = strInitDir;
	
	
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_CATALOG),szPath,strInitDir,this->GetSafeHwnd(),//cjc 2012年11月7日句柄应该为当前窗口句柄而不是主窗口句柄
		FALSE,BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | 0x0040/*BIF_NEWDIALOGSTYLE*/) )
		return;
	if( gpCfgLibMan->LoadConfig(szPath,TRUE) )
	{
		m_strNewPath = szPath; //保存新路径 2012-9-6 sky modify
		ConfigLibItem item = gpCfgLibMan->GetConfigLibItemByScale(m_nCurDocScale);
		if (item.GetScale() != 0)
		{
			m_config = item;		
		}
		else
			m_config = gpCfgLibMan->GetConfigLibItem(0);
			
		FillScaleCombo();
		FillTree(); 
		m_strOldPath = m_strNewPath; //保存原路径 2012-9-6 sky modify
	}
	else
	{
		CString strText;
		strText.LoadString(IDS_PATH_INVALID);
		//提示路径无效
		AfxMessageBox(strText,MB_OK|MB_ICONASTERISK);
		gpCfgLibMan->ResetConfig(); //cjc 2012.9.26 加载前先重置方案 
		//加载失败替换之前可以加载的方案	
		if( gpCfgLibMan->LoadConfig(m_strOldPath,TRUE) )
		{
			ConfigLibItem item = gpCfgLibMan->GetConfigLibItemByScale(m_nCurDocScale);
			if (item.GetScale() != 0)
			{
				m_config = item;		
			}
			else
				m_config = gpCfgLibMan->GetConfigLibItem(0);
			
			FillScaleCombo();
			FillTree();		
		}
			
	}
	
}

void CDlgScheme::OnSaveScheme()
{
	if(m_config.GetScale()<=0 || !m_bModified)
		return;
	// 更新组的顺序
	m_config.pScheme->m_strSortedGroupName.RemoveAll();
	for (int i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
	{
		m_config.pScheme->m_strSortedGroupName.Add(m_UserIdx.m_aGroup.GetAt(i).GroupName);
	}
//	gpCfgLibMan->SaveScale(m_config.scale);
	m_config.pScheme->Save();
	m_bModified = FALSE;

	//备份
	m_BackupCurSchemeXML.Empty();
	m_config.pScheme->WriteTo(m_BackupCurSchemeXML);

	AfxMessageBox(StrFromResID(IDS_SAVE_SUCCEED),MB_OK|MB_ICONASTERISK);
}

BOOL CDlgScheme::FillTree()
{
	if (m_config.pScheme == NULL)
		return FALSE;
	// 将树信息存到USERIDX中
	m_UserIdx.Clear();

	m_wndIdxTree.DeleteAllItems();

	for (int i=0; i<m_config.pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		LAYGROUP group;
		strcpy(group.GroupName,m_config.pScheme->m_strSortedGroupName.GetAt(i));
		m_UserIdx.m_aGroup.Add(group);
	}
	//{hcw,2012.7.30.
	int nCount = m_config.pScheme->GetLayerDefineCount();
	//}
	for(i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *layer = m_config.pScheme->GetLayerDefine(i);

		for (int j=0; j<m_UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(m_UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
//				idx.index = i;
				idx.code = layer->GetLayerCode();
				strcpy(idx.FeatureName,layer->GetLayerName());
				strcpy(idx.strAccel,layer->GetAccel());
				idx.groupidx = j;

				m_UserIdx.m_aIdx.Add(idx);
				break;
			}
		}

	}
    //{hcw,2012.7.30.
	int nSpecialCount = m_config.pScheme->GetLayerDefineCount(TRUE);
	//}
	for(i=0; i<m_config.pScheme->GetLayerDefineCount(TRUE); i++)
	{
		CSchemeLayerDefine *layer = m_config.pScheme->GetLayerDefine(i,TRUE);
		
		for (int j=0; j<m_UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(m_UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
				//				idx.index = i;
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
	int  recentidx = -1, specialidx = -1;
	CArray<HTREEITEM,HTREEITEM> m_aRootItem;
	for (i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
	{
		LAYGROUP gp;
		gp = m_UserIdx.m_aGroup.GetAt(i);

		if (stricmp(gp.GroupName,"Recent") == 0x00)
			recentidx = i;

		if (stricmp(gp.GroupName,StrFromResID(IDS_SPECIALGROUP)) == 0x00)
			specialidx = i;
		
		if (recentidx != i)
		{
			HTREEITEM hRoot = m_wndIdxTree.InsertRoot(_T(gp.GroupName),0,GROUPID_FROMIDX(i));
			m_aRootItem.Add(hRoot);
		}
		
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
				if ((m_nLayerIndex >= 0)
					&&((m_nLayerIndex <= nCount)
					||(m_nLayerIndex <= nSpecialCount)))
				{	
					CSchemeLayerDefine* pLayerDefine = NULL;
					pLayerDefine = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer);
					if (!pLayerDefine)
					{
						continue;
					}
					if (m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetLayerName().CompareNoCase(idx.FeatureName) == 0)
					{		
						m_wndIdxTree.SelectItem(item);
						//保存当前所选项目
						m_hCurItem = item;
					}
				}
			}
		}

	}
	
	m_wndIdxTree.SetItemHeight(22);	
	m_wndIdxTree.RedrawWindow();
	m_BackupUserIdx = m_UserIdx;

	return TRUE;

}

BOOL CDlgScheme::FillAttList(int nLayerIndex)
{
	if (nLayerIndex < 0)
		return FALSE;

	//{hcw,2012.7.26,若方案库未加载直接退出。
	if (m_bAbsentSchemePath)
	{
		return FALSE;
	}
	//}
	m_wndAttList.DeleteAllItems();
//	if (m_config.pScheme->GetLayerDefineCount()<=nLayerIndex)//hcw,2012.7.27.
	if(((!m_bSpecialLayer)&&m_config.pScheme->GetLayerDefineCount(FALSE)<=m_nLayerIndex)||(m_bSpecialLayer&&m_config.pScheme->GetLayerDefineCount(TRUE)<=m_nLayerIndex))
	{
		nLayerIndex = -1;
		m_nLayerIndex = -1;
		return FALSE;
	}
	CSchemeLayerDefine *pLayerDef = m_config.pScheme->GetLayerDefine(nLayerIndex,m_bSpecialLayer);
	//{hcw,2012.7.26
	if (!pLayerDef)
	{
		return FALSE;
	}
	//}
	int nAttNum = 0;
	pLayerDef->GetXDefines(nAttNum);

	for (int i=0; i<nAttNum; i++)
	{
		XDefine xdef = pLayerDef->GetXDefine(i);

		int curSel = m_wndAttList.InsertItem(LVIF_TEXT|LVIF_STATE, m_wndAttList.GetItemCount(), 
			"1", 0, LVIS_SELECTED, 0, 0);

		m_wndAttList.SetItemText(curSel,0,xdef.field);
		m_wndAttList.SetItemText(curSel,1,xdef.name);
		m_wndAttList.SetItemText(curSel,2,xdef.defvalue);
		switch(xdef.valuetype)
		{
		case DP_CFT_BOOL: 
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_BOOL_TYPE));
			break;
		case DP_CFT_COLOR:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_COLOR_TYPE));
			break;
		case DP_CFT_VARCHAR:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_CHAR_TYPE));
			break;
		case DP_CFT_DOUBLE:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_DOUBLE_TYPE));
			break;
		case DP_CFT_INTEGER:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_LONG_TYPE));
			break;
		case DP_CFT_DATE:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_DATE_TYPE));
			break;
		case DP_CFT_FLOAT:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_SINGLE_TYPE));
			break;
		case DP_CFT_SMALLINT:
			m_wndAttList.SetItemText(curSel,3,StrFromResID(IDS_SMALLINT_TYPE));
			break;
		default:
			break;		
			
		}

		CString tmp;
		tmp.Format("%i",xdef.valuelen);
		m_wndAttList.SetItemText(curSel,4,tmp);

		tmp.Format("%i",xdef.isMust);
		m_wndAttList.SetItemText(curSel,5,tmp);	
		
		tmp.Format("%i",xdef.nDigit);
		m_wndAttList.SetItemText(curSel,6,tmp);	
		
		m_wndAttList.SetItemText(curSel,7,xdef.valueRange);	
	}
	return TRUE;
}

BOOL CDlgScheme::FillScaleCombo()
{
	m_scaleCombo.ResetContent();
	int nConifgCount = gpCfgLibMan->GetConfigLibCount();
	if (nConifgCount < 1) 
		return FALSE;
	
	int nMinScale = INT_MAX;
	for (int i=0; i<nConifgCount; i++)
	{
		int nConfigScale = gpCfgLibMan->GetConfigLibItem(i).GetScale();
		if (nMinScale > nConfigScale)
		{
			nMinScale = nConfigScale;
		}

		CString data;
		for (int j=0; j<m_scaleCombo.GetCount(); j++)
		{			
			m_scaleCombo.GetLBText(j,data);
			DWORD dScale = atoi(data);
			if (nConfigScale < dScale)
				break;
		}

		data.Format("%d",nConfigScale);
		int index = m_scaleCombo.InsertString(j,data);
		m_scaleCombo.SetItemData(index,i);
	}

	int nSelScale = m_nCurDocScale>0?m_config.GetScale():nMinScale;

	CString strScale;
	strScale.Format("%d",nSelScale);
	m_scaleCombo.SelectString(-1,strScale);
	
	if (m_nCurDocScale <= 0)
	{
		m_config = gpCfgLibMan->GetConfigLibItemByScale(atoi(strScale));
	}		
		
	m_wndCodeEdit.SetWindowText("");
	
	
	// 有活动文档时，禁用删除相关操作
	ForBidDel();

	//备份
	m_BackupCurSchemeXML.Empty();
	m_config.pScheme->WriteTo(m_BackupCurSchemeXML);

	return TRUE;

}

BOOL CDlgScheme::CreateCtrls()
{	
	//init ctrls	
	CRect rectEdit, rectReview, rectScale;
	CRect rectRslt;
	m_scaleCombo.GetWindowRect (&rectScale);
	ScreenToClient(&rectScale);
	
	CWnd *pWnd = GetDlgItem(IDC_LAYERPREVIEW_STATIC);
	if( !pWnd )return FALSE;
	CRect rcView;
	pWnd->GetWindowRect(&rectReview);

	CRect rectReview2 = rectReview;
	ScreenToClient(&rectReview2);
	int treeWidth = rectReview2.right + 2;
	
	//m_wndCodeEdit
	m_wndCodeEdit.SetFont(this->GetDlgItem(IDC_STATIC)->GetFont(), FALSE);//hcw,2012.3.5
	m_wndCodeEdit.SetWindowPos (NULL,2,rectScale.bottom+5,treeWidth,20,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndCodeEdit.GetWindowRect(&rectRslt);
	ScreenToClient(&rectRslt);
	//m_m_wndSearchRsltList,hcw,2012.2.27
	m_wndSearchRsltList.SetFont(this->GetDlgItem(IDC_STATIC)->GetFont(),TRUE);//hcw,2012.3.5
	m_wndSearchRsltList.SetWindowPos (NULL, 2,rectRslt.bottom,treeWidth,135, SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndSearchRsltList.ShowWindow(FALSE);
	//创建m_wndIdxTree
	m_wndCodeEdit.GetWindowRect (&rectEdit);
	
	m_wndIdxTree.SetWindowPos (NULL,2,rectScale.bottom+27,treeWidth,
		rectReview.top-rectEdit.bottom-40,
		SWP_NOACTIVATE|SWP_NOZORDER);
	
	//创建m_wndAttList	
	CRect rectClient;
	pWnd = (CWnd*)GetDlgItem(IDC_ATTRDEFINE_STATIC);
	pWnd->GetWindowRect(&rectClient);
	ScreenToClient(&rectClient);
	
	pWnd = (CWnd*)GetDlgItem(IDC_ADD_BUTTON);
	if( pWnd!=NULL )
	{
		CRect rcStatic;
		pWnd->GetWindowRect(&rcStatic);
		
		CRect rcPos = rcStatic;
		ScreenToClient(&rcPos);
		rcPos.left -= 2;
		rcPos.top = rcPos.bottom+1;
		rcPos.right = rcPos.left+500;//rectClient.right - 2;
		rcPos.bottom = rectClient.bottom -5;

		pWnd = (CWnd*)GetDlgItem(IDC_COPYTO_OTHERDEFINES);
		if( pWnd!=NULL )
		{
			pWnd->GetWindowRect(&rcStatic);
			ScreenToClient(&rcStatic);
			rcPos.right = rcStatic.left-4;
		}
		
		m_wndAttList.SetWindowPos(NULL,rcPos.left,rcPos.top,rcPos.Width(),rcPos.Height(),SWP_NOACTIVATE|SWP_NOZORDER);
		//对m_wndAttList进行设置
		m_wndAttList.SetExtendedStyle(m_wndAttList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	}

	//层属性
	m_wndAttList.InsertColumn(0,StrFromResID(IDS_FIELDNAME),LVCFMT_LEFT,80);
	m_wndAttList.InsertColumn(1,StrFromResID(IDS_NAME),LVCFMT_LEFT,80);
	m_wndAttList.InsertColumn(2,StrFromResID(IDS_DEFAULTVALUE),LVCFMT_LEFT,70);
	m_wndAttList.InsertColumn(3,StrFromResID(IDS_VALUETYPE),LVCFMT_LEFT,60);
	m_wndAttList.InsertColumn(4,StrFromResID(IDS_VALUELEN),LVCFMT_LEFT,50);
	m_wndAttList.InsertColumn(5,StrFromResID(IDS_ISMUST),LVCFMT_LEFT,40);
	m_wndAttList.InsertColumn(6,StrFromResID(IDS_DEMICAL),LVCFMT_LEFT,75);
	m_wndAttList.InsertColumn(7,StrFromResID(IDS_VALUERANGE),LVCFMT_LEFT,120);

	m_wndAttList.SetComboColumns(3,TRUE);

	
	//符号配置
	m_cellButton.SetImage(IDB_CELL_BITMAP);
	m_dashlineButton.SetImage(IDB_DASHLINE_BITMAP);
	m_celllineButton.SetImage(IDB_CELLLINE_BITMAP);
	m_scalelineButton.SetImage(IDB_SCALELINE_BITMAP);
	m_colorhatchButton.SetImage(IDB_COLORHATCH_BITMAP);
	m_cellhatchButton.SetImage(IDB_CELLHATCH_BITMAP);
	m_annButton.SetImage(IDB_ANNOTATION_BITMAP);
	m_linefillButton.SetImage(IDB_LINEFILL_BITMAP);
	m_diagonalButton.SetImage(IDB_DIAGONAL_BITMAP);
	//m_cellScaleButton.SetImage(IDB_CELLSCALE_BITMAP);
	m_delsymbolButton.SetImage(IDB_DELETESYMBOL_BITMAP);


	return TRUE;
}

BOOL CDlgScheme::InitMems(ConfigLibItem &config, int nCurDocScale)
{
	m_config = config;	
	m_nCurDocScale = nCurDocScale;	
	return TRUE;
}



BOOL CDlgScheme::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	int nIndex = 0;
	if (pMsg->message == WM_KEYDOWN)
	{	
		//m_bUpDown = FALSE;
		m_bReturn = FALSE; 
		
		//{hcw,2012.2.26
		if (pMsg->wParam== VK_DOWN)
		{
			//m_wndSearchRsltList.SetFocus();
			nIndex = m_wndSearchRsltList.GetCurSel();
			m_wndSearchRsltList.SetCurSel(nIndex+1);
			m_bUpDown = TRUE;
			return TRUE;
		}
		if(pMsg->wParam==VK_UP)
		{
			nIndex = m_wndSearchRsltList.GetCurSel();
			m_wndSearchRsltList.SetCurSel(nIndex-1);
			m_bUpDown = TRUE;//hcw,2012.4.8,false->true.
			return TRUE;
		}
		//}
		//hcw,2012.3.9,添加shift和ctrl的消息判断
		if (pMsg->wParam==VK_SHIFT)
		{
			m_bShift = TRUE;			
		}
		if(pMsg->wParam==VK_CONTROL)
		{
			m_bCtrl = TRUE;
		}
		if( (pMsg->wParam==VK_RETURN)&&(m_bUpDown||m_nSelinSearchRsltList>=0))
		{
			BOOL bSpecial = FALSE;
			m_bReturn = TRUE;
			m_bUpDown = FALSE;//hcw,2012.4.8
			HWND hFocus = ::GetFocus();
			m_SelLayNameandId="";
			//{hcw,2012.4.8,cancel off
			nIndex = m_wndSearchRsltList.GetCurSel();
			m_wndSearchRsltList.GetText(nIndex,m_SelLayNameandId);
			//{hcw,2012.4.9,cancel off	

			if (m_CurStrCode=="")
			{
				int iLayId = 0;
				sscanf(m_SelLayNameandId,"%i",&iLayId);
				m_CurStrCode.Format("%d",iLayId);
				
			}
			//{hcw,2012.7.13,判断是否为独立符号项下的层
			int iSpecialGroupIdx = -1;
			for (int i=0; i<m_UserIdx.m_aGroup.GetSize();i++)
			{
				CString str;
				str.Format("%s",m_UserIdx.m_aGroup[i].GroupName);
			   if (str.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
			   {
					iSpecialGroupIdx = i;
					break;
			   }
			}
			for (int j=0; j<m_UserIdx.m_aIdx.GetSize();j++)
			{
				if ((m_UserIdx.m_aIdx[j].groupidx==iSpecialGroupIdx)
					&&m_UserIdx.m_aIdx[j].FeatureName==m_SelLayNameandId)
				{
					bSpecial = TRUE;	
				}
			}
			//}
			if (!bSpecial)
			{
				m_wndCodeEdit.SetWindowText(m_CurStrCode);//hcw,2012.2.27
				m_wndCodeEdit.SetSel(m_CurStrCode.GetLength(),-1);
			}
			else
			{
				m_wndCodeEdit.SetWindowText(m_SelLayNameandId);
				m_wndCodeEdit.SetSel(m_SelLayNameandId.GetLength(),-1);
			}
			m_bChgFromSelect = FALSE; //hcw,2012.7.16
			m_wndSearchRsltList.ShowWindow(FALSE);
			//}
			if( hFocus==m_wndCodeEdit.GetSafeHwnd()&&(nIndex>=0) )
			{
				m_bCanSelectDefault = TRUE;
				m_bCanSelectDefault = FALSE;
			}
			
		}
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
			return false;
	}
	else if (pMsg->message==WM_KEYUP)//hcw,2012.2.27,keyup 消息
	{
		if (pMsg->wParam==VK_DOWN||pMsg->wParam==VK_UP)
		{
			m_bUpDown = TRUE;//hcw,2012.4.8,FALSE->TRUE
		}
		//hcw,2012.3.15
		if (pMsg->wParam==VK_SHIFT)
		{
			m_bShift = FALSE;
		}
		if (pMsg->wParam==VK_CONTROL)
		{
			m_bCtrl = FALSE;
		}
		return TRUE;//hcw,2012.3.16
	}
	else if(pMsg->message==WM_LBUTTONDOWN && pMsg->hwnd==m_wndIdxTree.GetSafeHwnd() )//hcw,2012.3.16
	{
		//{hcw,2012.4.6
		CPoint pt;
		GetCursorPos(&pt);

		if(isInRect(IDC_COLLECTVIEW_TREE,pt)) //hcw,2012.7.16,cancel off hFocus==m_wndIdxTree.GetSafeHwnd()
		//}
		{
			
			//{hcw,2012.3.16
			if ((!m_bChgFromSelect)&&(!m_bEditLabel))
			{
				FillColor();
				
				DrawPreview(m_nLayerIndex);
	
				FillAttList(m_nLayerIndex);
				
				FillGeo();
				
				FillSymbolList();
				m_wndIdxTree.SetFocus();
			}
			//}
		}else if (isInRect(IDC_COLLECTVIEW_EDIT,pt))//hcw,2012.7.13,7.16,cancel offhFocus==m_wndIdxTree.GetSafeHwnd()
		{
			m_bChgFromSelect = FALSE;
		}
			
	}
	else if (pMsg->message==WM_LBUTTONUP)//hcw,2012.2.27
	{
		//m_bLButtnUp = TRUE;//hcw,2012.3.12,remove  A;
		HWND hFocus = ::GetFocus();		
		
		if (hFocus==m_wndSearchRsltList.GetSafeHwnd())
		{
			m_bLButtnUp = TRUE;//hcw,2012.3.12,remove from A;

			m_bCanSelectDefault = TRUE;
			//OnChangeEditFcode();
			m_bCanSelectDefault = FALSE;
		}
		//{hcw,2012.3.15,added 
		if (hFocus==m_wndIdxTree.GetSafeHwnd())
		{
			m_bLButtnUp = FALSE;
			m_wndIdxTree.SelectItem(m_hCurItem);
		}
		//}
		
	}
	else if(pMsg->message == WM_MOUSEMOVE)
	{
		 m_ToolTip.RelayEvent(pMsg);

		 if ((pMsg->hwnd == GetDlgItem(IDC_LAYERPREVIEW_STATIC)->m_hWnd) && (m_nLayerIndex >= 0))
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
		 
	}
	else if(pMsg->hwnd==GetDlgItem(IDC_LAYERPREVIEW_STATIC)->m_hWnd && pMsg->message == WM_RBUTTONDOWN)
	{
		if (m_nClickNum == 1 || m_nClickNum == 2)
		{
			m_nClickNum = 3;
			OnClickStatic();
		}
		
	}
	else if (pMsg->hwnd==GetDlgItem(IDC_SUPPORTGEONAME_EDIT)->m_hWnd && pMsg->message==WM_LBUTTONDOWN)
	{
		OnSupportgeonameEdit();
	}

	BOOL bMulti = FALSE;
	bMulti = m_wndIdxTree.m_bRealMulti;
	if (!m_bFirstDrawPreview)
	{
		DrawPreview(m_nLayerIndex);
		m_bFirstDrawPreview = TRUE;
	}
		
	return CDialog::PreTranslateMessage(pMsg);
}

int CDlgScheme::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rt;
	rt.SetRectEmpty();
	
	if( !m_wndCodeEdit.Create(WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL|WS_BORDER,
		rt,this,IDC_COLLECTVIEW_EDIT) )
	{
		TRACE0("Failed to Code Edit\n");
		return -1;      // fail to create
	}

	//hcw,2012.2.27,Create listbox to get the result of searching in TreeCtrl; 
	if(!m_wndSearchRsltList.Create(WS_CHILD|WS_VISIBLE|LBS_STANDARD|WS_HSCROLL, rt, this, IDC_LIST_NODENAME))
	{
		TRACE0("Failed to Create List\n");
		return -1;
	}

	// Create views:
	
	const DWORD dwViewStyle =	WS_CHILD | WS_VISIBLE | 
								TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS |
								WS_CLIPCHILDREN | TVS_EDITLABELS;
	
	if (!m_wndIdxTree.Create (dwViewStyle, rt, this, IDC_COLLECTVIEW_TREE))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}
	
	//解决TVN_SELCHANGED接收不到的问题
	BOOL tIsUnicode; 
#ifdef UNICODE 
	tIsUnicode = TreeView_SetUnicodeFormat(m_wndIdxTree.m_hWnd, 1); 
#else 
	tIsUnicode = TreeView_SetUnicodeFormat(m_wndIdxTree.m_hWnd, 0); 
#endif // UNICODE 

	m_wndAttList.Create(WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_ALIGNLEFT|LVS_SHOWSELALWAYS,
		CRect(0,0,1,1),this,200);

	return 0;
}

void CDlgScheme::OnChangeEditFcode()
{

	if( m_bChgFromSelect||m_bEditLabel) 
	{
		//{hcw,2012.7.13,将编辑框的光标移至最后。
		CString str;
		m_wndCodeEdit.GetWindowText(str);
		int nStrLength = str.GetLength();
		m_wndCodeEdit.SetSel(str.GetLength(),-1); 
		m_wndCodeEdit.SetFocus();
		//}
		m_bChgFromSelect = FALSE; //HCW,2012.3.12
		m_bEditLabel = FALSE;
        return;
	}   
	char input[256];
	m_wndCodeEdit.GetWindowText(input,sizeof(input)-1);
	int nInLen = strlen(input);

	if((nInLen<=0))
	{
		m_bInitialSelected = FALSE;//hcw,2012.4.9,cancel off
		m_wndSearchRsltList.ResetContent();//hcw,2012.2.27
		m_wndSearchRsltList.ShowWindow(FALSE);//hcw,2012.2.27
		return;
	}
	
	//{hcw,2012.3.14,选中节点改了编辑框，而不弹搜索框
	if (m_bSchemeDlgInitialized||m_bFirstinSearchEdit)
	{
		m_wndSearchRsltList.ShowWindow(TRUE);//hcw,2012.3.12, transfer the searchlistbox code to here
	}
	


	CString strLayerName;
	CString strCodeId;
	CString strNodeName;
	int nGroupCount = m_UserIdx.m_aGroup.GetSize();
	int nLayCount = m_UserIdx.m_aIdx.GetSize();//hcw,2012.3.29,m_BackupUserIdx->m_UserIdx
	int iGroupIdx = -1; //hcw,2012.7.13,记录组名索引

	m_wndSearchRsltList.ResetContent();
	for (int idx=0; idx < nGroupCount;idx++)
	{	
		CString strGroupName = "";
		strGroupName.Format("%s",m_UserIdx.m_aGroup[idx].GroupName);
		if(strGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			iGroupIdx = idx;
			break;
		}
	}
	for(int j=0; j < nLayCount; j++)
	{
		strLayerName.Format("%s",m_UserIdx.m_aIdx.GetAt(j).FeatureName);//hcw,2012.3.29,m_BackupUserIdx->m_UserIdx
		strCodeId.Format("%I64d",m_UserIdx.m_aIdx.GetAt(j).code);//hcw,2012.3.29,m_BackupUserIdx->m_UserIdx
		if ((iGroupIdx>=0)&&(m_UserIdx.m_aIdx.GetAt(j).groupidx==iGroupIdx))
		{
			strNodeName = strLayerName;
		}
		else
			strNodeName = strCodeId +" "+strLayerName;

		CString strInput="";
		CString strTmpNodeName="";
		strInput.Format("%s",input);
		strTmpNodeName = strNodeName;
		strInput.MakeLower();
		strTmpNodeName.MakeLower();
		//hcw,2012.7.17,不区分大小写进行模糊搜索。
		if (strTmpNodeName.Find(strInput)>=0)
		{
			m_wndSearchRsltList.AddString(strNodeName);
			m_wndSearchRsltList.Invalidate(FALSE);//hcw,2012.3.13,TRUE->FALSE
			m_wndSearchRsltList.UpdateWindow();
		}
	}
	m_nSelinSearchRsltList = m_wndSearchRsltList.SetCurSel(0);//hcw,2012.3.27
	//{hcw,2012.2.27,将与input匹配的所有层写入m_wndSearchRsltList中
	if (m_bLButtnUp)
	{
		m_bLButtnUp = FALSE;		
		m_wndSearchRsltList.ShowWindow(FALSE); //hcw,2012.4.9,recover
		SetTreeNodeToTop(m_SelLayNameandId);		
		m_wndCodeEdit.SetSel(nInLen+1,nInLen+1);//hcw,2012.3.13,added
		m_wndCodeEdit.SetFocus();//hcw,2012.3.13,recover.
		return;
	}
	
	if ((!m_bEditLabel)
		&&m_bReturn
		&&(m_bUpDown||m_nSelinSearchRsltList>=0))
	{
		m_bReturn = FALSE; //hcw,2012.3.12,Recover 
		m_wndSearchRsltList.ShowWindow(FALSE);
		SetTreeNodeToTop(m_SelLayNameandId);
		m_wndCodeEdit.SetSel(m_SelLayNameandId.GetLength(),-1);
		m_wndCodeEdit.SetFocus();
		m_bUpDown = FALSE;//hcw,2012.4.8,
		return;
	}
    
	char test1[256], test2[256], test3[256];
	int nsize = m_UserIdx.m_aIdx.GetSize();
	if( nsize<=0 )return;

	int pos1, pos2, pos3, max0=-1, k0=-1, max=-1, k=-1;
	for(int i=0; i<nsize; i++)
	{
		IDX idx;
		idx = m_UserIdx.m_aIdx.GetAt(i);
		if( idx.groupidx!=-1 )
		{
			sprintf(test1,"%I64d",idx.code);
			strcpy(test2,idx.FeatureName);
			strcpy(test3,idx.strAccel);

			//比较，找到第一个字符不同的位置
			pos1 = pos2 = pos3 = 0;
			while( test1[pos1]==input[pos1] && input[pos1]!=0 )pos1++;
			while( test2[pos2]==input[pos2] && input[pos2]!=0 )pos2++;
			while( test3[pos3]==input[pos3] && input[pos2]!=0 )pos3++;

			//缺省层放在较低的优先级别
			if( idx.groupidx==0 && !m_bCanSelectDefault )
			{
				if( max0<0 || max0<pos1 || max0<pos2 || max0<pos3 )
				{
					max0 = (pos1>pos2?pos1:pos2);
					max0 = max0>pos3?max0:pos3;
					k0 = i;
				}
			}
			else
			{
				if( max<0 || max<pos1 || max<pos2 || max<pos3)
				{
					max = (pos1>pos2?pos1:pos2);
					max = max>pos3?max0:pos3;
					k = i;
				}

				if( pos1>=nInLen || pos2>=nInLen || pos3>=nInLen )break;
			}
		}
	}

	BOOL bFindDefault = FALSE;
	if( max<max0 )
	{
		bFindDefault = TRUE;
		max = max0;	k = k0;
	}
	if( k<0 )return;

	//查找序号 k 所在的树节点
	HTREEITEM hRoot = m_wndIdxTree.GetRootItem(), hChild = NULL;

	CString strCode;
	strCode.Format("%d",m_UserIdx.m_aIdx.GetAt(k).code);
	while( hRoot )
	{
		hChild = m_wndIdxTree.GetChildItem(hRoot);
		while( hChild )
		{
			CString str = m_wndIdxTree.GetItemText(hChild);
			
			if (str.Find(strCode) >= 0) goto FINDITEM;
			hChild = m_wndIdxTree.GetNextItem(hChild, TVGN_NEXT);
		}
		hRoot = m_wndIdxTree.GetNextItem(hRoot, TVGN_NEXT);
	}
	
FINDITEM:
	if( !hRoot || !hChild )return;
	
	m_bFirstinSearchEdit = TRUE;//hcw,2012.4.9,cancel off.
	
}


void CDlgScheme::OnChangeDBLayName()
{
	UpdateData(TRUE);

	if (m_bSpecialLayer) return;

	// 支持修改组颜色
	if (m_nLayerIndex < 0)
	{
		CString strGroupName = m_wndIdxTree.GetItemText(m_hCurItem);

		for(int i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
		{
			CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(i);
			if (pdef->GetGroupName().CompareNoCase(strGroupName) == 0)
			{
				pdef->SetDBLayerName(m_strDBLayName);
			}
		}
	}
	else
	{
		CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(m_nLayerIndex);
		pdef->SetDBLayerName(m_strDBLayName);
	}

	if (!m_bModified)
		m_bModified = TRUE;
}

void CDlgScheme::SetTreeNodeToTop(CString strLayerNameorID)
{
	//FillTree(); //HCW,2012.4.9,cancel off
	HTREEITEM hRoot = m_wndIdxTree.GetRootItem();
	
	HTREEITEM hChild = NULL;
	CString str="";
	while (hRoot)
	{
		hChild = m_wndIdxTree.GetChildItem(hRoot);
		while(hChild)
		{
			str = m_wndIdxTree.GetItemText(hChild);

			CString str2;
			int pos = str.Find(' ');
			if( pos>=0 )
			{
				str2 = str.Left(pos);
			}

			if (str==strLayerNameorID || str2==strLayerNameorID ) //hcw,2012.7.13,Find→==
			{
				goto FINDITEM;
			}
			hChild = m_wndIdxTree.GetNextItem(hChild, TVGN_NEXT);
		}
		hRoot = m_wndIdxTree.GetNextItem(hRoot,TVGN_NEXT);
	}
FINDITEM:
	if(!hRoot||!hChild) return;
	
	//显示在最上面
	HTREEITEM hRoot2=m_wndIdxTree.GetRootItem();
	while(hRoot2)
	{
		if (hRoot2!=hRoot)
		{
			m_wndIdxTree.Expand(hRoot2, TVE_COLLAPSE);
		}
		hRoot2 = m_wndIdxTree.GetNextItem(hRoot2, TVGN_NEXT);
	}
	m_wndIdxTree.Expand(hRoot, TVE_EXPAND);
	//{HCW,2012.3.9
	hRoot = m_wndIdxTree.GetNextSiblingItem(hRoot);
	m_wndIdxTree.Expand(hRoot, TVE_EXPAND);
	//}
	m_wndIdxTree.Select(hChild, TVGN_FIRSTVISIBLE);
	m_wndIdxTree.SetItem(hChild, TVIF_STATE,NULL,0,0,TVIS_SELECTED,TVIS_SELECTED,0);
	m_wndIdxTree.SelectItem(hChild);//hcw,2012.3.9
	m_wndIdxTree.SetFocus();
	
}

BOOL CDlgScheme::UpdateCodeOrNameOfCurScale()
{
	m_nLayerIndex = -1;

	//备份
	m_BackupCurSchemeXML.Empty();
	m_config.pScheme->WriteTo(m_BackupCurSchemeXML);	
	FillTree();	
	m_wndCodeEdit.SetWindowText("");
	m_bModified = TRUE;
	return TRUE;
}

void CDlgScheme::OnSelchangeScaleCombo() 
{
	// TODO: Add your control notification handler code here
	m_nLayerIndex = -1;
	int nComboIndex = m_scaleCombo.GetCurSel();
	if (nComboIndex == CB_ERR) return;
	CString strScale;
	m_scaleCombo.GetLBText(nComboIndex,strScale);
	int iScale = atoi(strScale);//hcw,2012.7.18
	//提示保存
	if(m_bModified)
	{
		if (iScale==m_nCurSchemeScale)
		{
			return;
		}
		CString szText,szCaption;
		szText.LoadString(IDS_SAVE_SCHEME);
		szCaption.LoadString(IDS_ATTENTION);
		if(MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) == IDYES)
		{
			// 更新组的顺序
			m_config.pScheme->m_strSortedGroupName.RemoveAll();
			for (int i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
			{
				m_config.pScheme->m_strSortedGroupName.Add(m_UserIdx.m_aGroup.GetAt(i).GroupName);
			}

			m_config.pScheme->Save();
		}
		else
		{
			m_config.pScheme->ReadFrom(m_BackupCurSchemeXML);
			m_config.pScheme->LoadCADSymols();
		}
		m_bModified = FALSE;
	}

	if ((m_nCurSchemeScale<=0)||(m_nCurSchemeScale!=iScale))
	{
		m_nCurSchemeScale = iScale;
	}

	m_config = gpCfgLibMan->GetConfigLibItemByScale(m_nCurSchemeScale);

	//备份
	m_BackupCurSchemeXML.Empty();
	m_config.pScheme->WriteTo(m_BackupCurSchemeXML);

	FillTree();
	//hcw,2012.7.20,清除所有选择。
	m_wndIdxTree.ClearSelection();	
	m_wndCodeEdit.SetWindowText("");
	ForBidDel();
	
}
BOOL CDlgScheme::isInRect(UINT uID, CPoint pt)
{
	RECT rc;
	this->GetDlgItem(uID)->GetWindowRect(&rc);
	if ((pt.x>rc.left)&&(pt.x<rc.right)
		&&(pt.y<rc.bottom)&&(pt.y>rc.top))
	{
		return TRUE;
	}
	else
		return FALSE;
}
void CDlgScheme::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndIdxTree;
	ASSERT_VALID (pWndTree);
	
	if (point != CPoint (-1, -1))
	{
		//---------------------
		// Select clicked item:
		//---------------------
		CPoint ptTree = point;
		pWndTree->ScreenToClient (&ptTree);
		
		UINT uFlags;
		HTREEITEM hTreeItem = pWndTree->HitTest (ptTree,&uFlags);

		CPoint ptList = point;
		m_wndListSymbol.ScreenToClient (&ptList);
		
		 
		
		UINT uFlags1;
		int nItem = m_wndListSymbol.HitTest(ptList, &uFlags1);

		if ((hTreeItem != NULL) && (TVHT_ONITEM & uFlags)) //if (hTreeItem != NULL)  
		{
			//Add your Code on Command:
			//pWndTree->SelectItem (hTreeItem);//hcw,2012.3.23, cancel off error key from RButtonDbClicked
		}
		// 比例尺下没有任何数据或点击的位置不在组名或层名时，弹出新建组菜单
		else if(uFlags&TVHT_NOWHERE)
		{
			if (m_config.GetScale() == 0)  return;			
			CMenu menu;
			menu.LoadMenu (IDR_POPMENU);
			ASSERT(menu);
			//获取第一个弹出菜单，所以第一个菜单必须有子菜单 
			CMenu* pSumMenu = menu.GetSubMenu(0);
			ASSERT(pSumMenu);			
			
			// 禁用删除相关操作
			pSumMenu->EnableMenuItem(ID_MOVE_LAYER,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MOVETO_LAYER,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_COPY_SYMBOL,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_PASTE_SYMBOL,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_DEL_GROUP,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_NEW_LAYER,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_DEL_LAYER,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MOVE_GROUP,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MOVETO_GROUP,MF_DISABLED|MF_GRAYED);			
			
			CPoint oPoint;          
			GetCursorPos( &oPoint);             //获取当前光标的位置，以便使得菜单可以跟随光标 
			pSumMenu->TrackPopupMenu (TPM_LEFTALIGN, oPoint.x, oPoint.y, this);	
			return;
		}
		else if (uFlags1&LVHT_ONITEMICON)
		{
			m_binSymbolListRegion = TRUE;
			m_wndListSymbol.SetFocus();
			CMenu menu;
			menu.LoadMenu (IDR_SYMBOLMENU);
			ASSERT(menu);
			CMenu* pSumMenu = menu.GetSubMenu(0);
			ASSERT(pSumMenu);
			CPoint oPoint;
			GetCursorPos( &oPoint);             //获取当前光标的位置，以便使得菜单可以跟随光标 
			pSumMenu->TrackPopupMenu (TPM_LEFTALIGN, oPoint.x, oPoint.y, this); 

			
			return;
		}
		else 
		{
			//{hcw,2012.2.14,符号配置列表框中的右键菜单
			CPoint curPoint;
			GetCursorPos(&curPoint);
			m_binSymbolListRegion =	TRUE;
			if (isInRect(IDC_SYMBOL_LIST,curPoint))
			{
				CMenu menu;
				menu.LoadMenu (IDR_SYMBOLMENU);
				ASSERT(menu);
				CMenu* pSumMenu = menu.GetSubMenu(0);
				ASSERT(pSumMenu);
				//获取当前光标的位置，以便使得菜单可以跟随光标 
				pSumMenu->TrackPopupMenu (TPM_LEFTALIGN, curPoint.x, curPoint.y, this); 
			}
			return ;
			//}
		}
		
		
		DWORD data = m_wndIdxTree.GetItemData(hTreeItem);

		//如果在组名前点击右键
		if(data >= 0xffff)
		{
			m_binSymbolListRegion = FALSE;//hcw,2012.2.14
			pWndTree->SetFocus ();
			CMenu menu;
			menu.LoadMenu (IDR_POPMENU);
			ASSERT(menu);
			//获取第一个弹出菜单，所以第一个菜单必须有子菜单 
			CMenu* pSumMenu = menu.GetSubMenu(0);
			ASSERT(pSumMenu);
			//{hcw,2012.2.29
			m_treeNodeList.RemoveAll();
			m_wndIdxTree.GetSelectedList(m_treeNodeList);
			//}
			// 禁用删除相关操作
			int nComboIndex = m_scaleCombo.GetCurSel();
			if (nComboIndex == CB_ERR) return;
			CString strScale;
			m_scaleCombo.GetLBText(nComboIndex,strScale);

			pSumMenu->EnableMenuItem(ID_MOVE_LAYER,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MOVETO_LAYER,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_COPY_SYMBOL,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_PASTE_SYMBOL,MF_DISABLED|MF_GRAYED);

			if (m_bSpecialLayer)
			{
				pSumMenu->EnableMenuItem(ID_NEW_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVE_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVETO_LAYER,MF_DISABLED|MF_GRAYED);
			}
			else
			{
				pSumMenu->EnableMenuItem(ID_DEL_SYMLAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_NEW_SYMLAYER,MF_DISABLED|MF_GRAYED);
			}
			
			if (m_nCurDocScale == atoi(strScale))
			{
				pSumMenu->EnableMenuItem(ID_DEL_GROUP,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_SYMLAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVE_GROUP,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVETO_GROUP,MF_DISABLED|MF_GRAYED);
			}
			else
			{
				if (m_nMovedGroup == -1)
					pSumMenu->EnableMenuItem(ID_MOVETO_GROUP,MF_DISABLED|MF_GRAYED);
			}

			CPoint oPoint;          
			GetCursorPos( &oPoint);             //获取当前光标的位置，以便使得菜单可以跟随光标 
			pSumMenu->TrackPopupMenu (TPM_LEFTALIGN, oPoint.x, oPoint.y, this);	
			
		}
		else
		{
			m_binSymbolListRegion = FALSE;//hcw,2012.2.14
			pWndTree->SetFocus ();

		    m_nCountTreeSelected = m_wndIdxTree.GetSelectedCount();
			//{hcw,2012.2.29
			if (m_treeNodeList.GetCount() > 0)
			{
				m_treeNodeList.RemoveAll();
			}
			//}
			m_wndIdxTree.GetSelectedList(m_treeNodeList);
			CMenu menu;
			menu.LoadMenu (IDR_POPMENU);
			ASSERT(menu);
			//获取第一个弹出菜单，所以第一个菜单必须有子菜单 
			CMenu* pSumMenu = menu.GetSubMenu(0);
			ASSERT(pSumMenu);
			pSumMenu->EnableMenuItem(ID_NEW_GROUP,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_DEL_GROUP,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MOVE_GROUP,MF_DISABLED|MF_GRAYED);
			pSumMenu->EnableMenuItem(ID_MOVETO_GROUP,MF_DISABLED|MF_GRAYED);
			//{hcw,2012.2.20,多选状态下的右键菜单，只留“删除层”。
			if (m_nCountTreeSelected>1)
			{
				pSumMenu->EnableMenuItem(ID_MOVETO_LAYER, MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_NEW_SYMLAYER, MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_SYMLAYER, MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_PASTE_SYMBOL, MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_COPY_SYMBOL, MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_NEW_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVE_LAYER, MF_DISABLED|MF_GRAYED);	
				pSumMenu->EnableMenuItem(ID_SORT_IDCODE, MF_DISABLED|MF_GRAYED);//hcw,2012.3.15.
				
			}
			//}
			if (m_bSpecialLayer)
			{
				pSumMenu->EnableMenuItem(ID_NEW_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVE_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVETO_LAYER,MF_DISABLED|MF_GRAYED);
			}
			else
			{
				pSumMenu->EnableMenuItem(ID_DEL_SYMLAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_NEW_SYMLAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_SORT_IDCODE,MF_DISABLED|MF_GRAYED);//hcw,2012.3.15
			}

			if (m_strcopyLayerName.IsEmpty())
			{
				
				pSumMenu->EnableMenuItem(ID_PASTE_SYMBOL,MF_DISABLED|MF_GRAYED);
			}
			
			// 禁用删除相关操作
			int nComboIndex = m_scaleCombo.GetCurSel();
			if (nComboIndex == CB_ERR) return;
			CString strScale;
			m_scaleCombo.GetLBText(nComboIndex,strScale);
			
			if (m_nCurDocScale == atoi(strScale))
			{
				pSumMenu->EnableMenuItem(ID_MOVE_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_MOVETO_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_LAYER,MF_DISABLED|MF_GRAYED);
				pSumMenu->EnableMenuItem(ID_DEL_SYMLAYER,MF_DISABLED|MF_GRAYED);
			}
			else
			{
				if (m_nMovedLayer == -1)
					pSumMenu->EnableMenuItem(ID_MOVETO_LAYER,MF_DISABLED|MF_GRAYED);
			}

			CPoint oPoint;          
			GetCursorPos( &oPoint);             //获取当前光标的位置，以便使得菜单可以跟随光标 
			pSumMenu->TrackPopupMenu (TPM_LEFTALIGN, oPoint.x, oPoint.y, this); 

		}
		
	}

}

void CDlgScheme::SetSelectedLayerIndex(CString strNameAndCode)
{
	char layName[_MAX_FNAME];
	int num;
	sscanf(strNameAndCode,"%i %s",&num,layName);

	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}
	
	for(int i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
	{
		if(m_config.pScheme->GetLayerDefine(i,m_bSpecialLayer)->GetLayerName().CompareNoCase(layName) == 0)
		{
			m_nLayerIndex = i;
			break;
		}	
	}
	
	if(i>m_config.pScheme->GetLayerDefineCount(m_bSpecialLayer)-1)
	{
		m_nLayerIndex = -1;
	}

}


void CDlgScheme::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	*pResult = 0;
	
	if (!(pTVDispInfo->item.pszText) || strlen(pTVDispInfo->item.pszText) <= 0)
	{
		m_SelLayNameandId = pTVDispInfo->item.pszText;//hcw,2012.4.6
		return;
	}
	//保存
	HTREEITEM parentItem = m_wndIdxTree.GetParentItem(pTVDispInfo->item.hItem);
	//如果修改的是组名
	if (parentItem == NULL)
	{
		if(pTVDispInfo->item.pszText == m_oldGroupName)
			return;

		CStringArray groupNames;
		for (int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
		{
			if (i != m_nGroup)
				groupNames.Add( m_UserIdx.m_aGroup[i].GroupName );
		}
		//检查是否重名,重名则重新输入
//		if(CheckName(pTVDispInfo->item.pszText,groupNames).CompareNoCase(pTVDispInfo->item.pszText) != 0)
		if (IsExist(pTVDispInfo->item.pszText,groupNames))
		{
			CString szText,szCaption;
			szText.LoadString(IDS_GROUP_EXIST);
			szCaption.LoadString(IDS_ATTENTION);
			MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
			return;
		}
		else
		{	
			//修改图层名称并显示
			m_wndIdxTree.SetItem(&pTVDispInfo->item);
			strcpy(m_UserIdx.m_aGroup[m_nGroup].GroupName,pTVDispInfo->item.pszText);

			for(i=0; i<m_config.pScheme->GetLayerDefineCount(m_bSpecialLayer); i++)
			{
				CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(i,m_bSpecialLayer);
				if (pdef->GetGroupName().CompareNoCase(m_oldGroupName) == 0)
				{
					pdef->SetGroupName(pTVDispInfo->item.pszText);
					m_bModified = TRUE;
				}
			}
		}
	}
	else
	{
		CString strChangedCodeName = pTVDispInfo->item.pszText;
		if (strChangedCodeName.CompareNoCase(m_oldLayerName) == 0)
			return;

		if (!m_bSpecialLayer)
		{
			if( !CheckLayerFormat(strChangedCodeName) )
			{
				CString szText,szCaption;
				szText.LoadString(IDS_LAYERFORMAT_WRONG);
				szCaption.LoadString(IDS_ATTENTION);
				MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
				return;
			}

			char changedLayerName[_MAX_FNAME];
			CStringArray layerNames;
			__int64 nChangedCode;
			sscanf(strChangedCodeName,"%I64d %s",&nChangedCode,changedLayerName);
			
			CString strAccel;		
			char *pdest = strstr(changedLayerName,"^");
			if (pdest != NULL)
			{
				strAccel = pdest+1;
				pdest[0] = 0;
			}
			
			//获取所有层的FID和编号
			int  recentidx = -1;
			for (int i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
			{
				LAYGROUP gp;
				gp = m_UserIdx.m_aGroup.GetAt(i);
				
				if (stricmp(gp.GroupName,"Recent") == 0x00)
				{
					recentidx = i;
					break;
				}
			}
			
			vector<__int64> code;
			for(i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
			{
				// 不要加最近使用列表
				if (i!=m_nIdx && m_UserIdx.m_aIdx[i].groupidx!=recentidx)
				{
					code.push_back(m_UserIdx.m_aIdx[i].code);
					layerNames.Add(m_UserIdx.m_aIdx[i].FeatureName);
				}
			}
			
			//检查是否重名,有则不保存
			if(FindNum(nChangedCode,code) || IsExist(changedLayerName,layerNames)/*CheckName(changedLayerName,layerNames).CompareNoCase(changedLayerName)!=0*/  )
			{
				CString szText,szCaption;
				szText.LoadString(IDS_LAYER_EXIST);
				szCaption.LoadString(IDS_ATTENTION);
				MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
				return;	
			}
			
			//修改图层名称并显示
			m_wndIdxTree.SetItem(&pTVDispInfo->item);
			m_UserIdx.m_aIdx[m_nIdx].code = nChangedCode;
			strcpy(m_UserIdx.m_aIdx[m_nIdx].FeatureName,changedLayerName);
			strcpy(m_UserIdx.m_aIdx[m_nIdx].strAccel,strAccel);
			
			//获取修改前的层名
			char oldLayerName[_MAX_FNAME];
			__int64 nOldCode;
			sscanf(m_oldLayerName,"%I64d %s",&nOldCode,oldLayerName);
			pdest = strstr(oldLayerName,"^");
			if (pdest != NULL)
			{
				pdest[0] = 0;
			}
			CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(oldLayerName,FALSE,0,m_bSpecialLayer); //hcw,2012.7.16,考虑独立符号项。
			if (!pdef) return;
			pdef->SetLayerName(changedLayerName);
			pdef->SetAccel(strAccel);
			pdef->SetLayerCode(nChangedCode);
			CString strChangedCode="";
			strChangedCode.Format("%I64d",nChangedCode);

			m_wndCodeEdit.SetWindowText(strChangedCode); //hcw,2012.7.13,如果选择的是普通层，就将层码置于编辑框中,否则就保持编辑框的内容不变。
									
			//Invalidate(FALSE);
// 			strChangedCode = "";

			//UpdateData(FALSE);
			m_bModified = TRUE;
			//{hcw,2012.4.9,在结束编辑时选中。
			m_wndIdxTree.SelectItem(pTVDispInfo->item.hItem);
			m_wndIdxTree.SetItemState(pTVDispInfo->item.hItem,TVIS_SELECTED,TVIS_SELECTED);
			//m_bReturn = FALSE;
			//m_bEditLabel = FALSE;
			//}
			
		}
		else
		{
			char changedLayerName[_MAX_FNAME];
			CStringArray layerNames;
			__int64 nChangedCode;
			sscanf(strChangedCodeName,"%s",changedLayerName);
			
			CString strAccel;		
			char *pdest = strstr(changedLayerName,"^");
			if (pdest != NULL)
			{
				strAccel = pdest+1;
				pdest[0] = 0;
			}
			
			//获取所有层的FID和编号
			int  recentidx = -1;
			for (int i=0; i<m_UserIdx.m_aGroup.GetSize(); i++)
			{
				LAYGROUP gp;
				gp = m_UserIdx.m_aGroup.GetAt(i);
				
				if (stricmp(gp.GroupName,"Recent") == 0x00)
				{
					recentidx = i;
					break;
				}
			}
			
			vector<__int64> code;
			for(i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
			{
				// 不要加最近使用列表
				if (i!=m_nIdx && m_UserIdx.m_aIdx[i].groupidx!=recentidx)
				{
					code.push_back(m_UserIdx.m_aIdx[i].code);
					layerNames.Add(m_UserIdx.m_aIdx[i].FeatureName);
				}
			}
			
			//检查是否重名,有则不保存
			if(IsExist(changedLayerName,layerNames))
			{
				CString szText,szCaption;
				szText.LoadString(IDS_LAYER_EXIST);
				szCaption.LoadString(IDS_ATTENTION);
				MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
				return;	
			}
			
			//修改图层名称并显示
			m_wndIdxTree.SetItem(&pTVDispInfo->item);
			//m_UserIdx.m_aIdx[m_nIdx].code = nChangedCode;
			strcpy(m_UserIdx.m_aIdx[m_nIdx].FeatureName,changedLayerName);
			strcpy(m_UserIdx.m_aIdx[m_nIdx].strAccel,strAccel);
			
			//获取修改前的层名
			char oldLayerName[_MAX_FNAME];
			__int64 nOldCode;
			sscanf(m_oldLayerName,"%I64d %s",&nOldCode,oldLayerName);
			pdest = strstr(oldLayerName,"^");
			if (pdest != NULL)
			{
				pdest[0] = 0;
			}
			CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(oldLayerName,FALSE,0,m_bSpecialLayer);
			if (!pdef) return;
			pdef->SetLayerName(changedLayerName);
			pdef->SetAccel(strAccel);
			//pdef->SetLayerCode(nChangedCode);
			m_bModified = TRUE;
		}

		

		// 修改文档
		/*if (m_pDoc && m_nCurDocScale>0)
		{
			int nComboIndex = m_scaleCombo.GetCurSel();
			if (nComboIndex >= 0)
			{
				CString strScale;
				m_scaleCombo.GetLBText(nComboIndex,strScale);
				
				if (m_nCurDocScale == atoi(strScale))
				{
					CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
					if (pDS)
					{ 
						CFtrLayer *pFtrLayer = pDS->GetFtrLayer(oldLayerName);
						if (pFtrLayer)
						{
							pFtrLayer->SetName(changedLayerName);
						}
					}
				}
			}		
			
		}*/
	
	}
}

void CDlgScheme::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	m_bEditLabel = TRUE;//hcw,2012.4.9
	if (!(pTVDispInfo->item.pszText) || strlen(pTVDispInfo->item.pszText) <= 0)
		return;
	HTREEITEM parentItem = m_wndIdxTree.GetParentItem(pTVDispInfo->item.hItem);
	//记录组位置
	if (parentItem == NULL)
	{
		CString group = m_wndIdxTree.GetItemText(pTVDispInfo->item.hItem);
		m_oldGroupName = group;
		for(int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
		{
			if(m_UserIdx.m_aGroup[i].GroupName == group)
			{
				m_nGroup = i;
				break;
			}
		}
		
	}
	//记录图层位置
	else 
	{
		//	SetItem(&pTVDispInfo->item);
		
		CString strParent = m_wndIdxTree.GetItemText(parentItem);
		CString slay = pTVDispInfo->item.pszText;
		m_oldLayerName = slay;
		char layName[_MAX_FNAME];
		__int64 num;
		//	slay.Format("%i %s",&num,layName);
		sscanf(slay,"%I64d %s",&num,layName);
		if (strParent.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)//hcw,2012.7.16,判断独立符号项。
		{
			int iGroupIdx = -1;
			CString strGroupName="";
			CString strLayerName="";
			for (int j=0; j<m_UserIdx.m_aGroup.GetSize();j++)
			{
				
				strGroupName.Format("%s",m_UserIdx.m_aGroup[j].GroupName);
				if (strGroupName.CompareNoCase(strParent)==0)
				{
					iGroupIdx = j;
					break;
				}
			}
			
			for (int k=0; k<m_UserIdx.m_aIdx.GetSize();k++)
			{
				strLayerName.Format("%s",m_UserIdx.m_aIdx[k].FeatureName);
				if ((m_UserIdx.m_aIdx[k].groupidx==iGroupIdx)
					&&(iGroupIdx>=0)
					&&(strLayerName==slay))
				{
					m_nIdx = k;
					break;
				}
			}
		}
		else
		{
			for(int i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
			{
				if(m_UserIdx.m_aIdx[i].code == num)
				{
					m_nIdx = i;
					break;
				}
			}
		
		}
	*pResult = 0;
	}
}
void CDlgScheme::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	*pResult = 0;

	ReleaseDlgMemory();
	
	NMTREEVIEW *pTree = (NMTREEVIEW*)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	//保存当前所选项目
	m_hCurItem = hItem;
	m_bMultiSelected = FALSE;
	ClearCtrls();
	if( !m_hCurItem )return;
	DWORD data = m_wndIdxTree.GetItemData(m_hCurItem);
	m_wndSearchRsltList.ShowWindow(FALSE);//hcw,2012.3.5.
	if( data>=0xffff )
	{
		m_bSpecialLayer = FALSE;
		CString groupName = m_wndIdxTree.GetItemText(m_hCurItem);
		if (groupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)
		{
			m_bSpecialLayer = TRUE;
		}
		
		m_wndCodeEdit.SetWindowText(""); //hcw,2012.3.12,Canceled;
		

		m_nLayerIndex = -1; //hcw,2012.4.9
	
		
		CWnd *pWndSize = GetDlgItem(IDC_LAYERSTATIC_SIZE);
		if( pWndSize )
		{
			CString strSize = "0mm X 0mm";
			pWndSize->SetWindowText(strSize);
		}
		m_bChgFromSelect = FALSE;//hcw,2012.3.15
		m_defaultgeoCombo.EnableWindow(FALSE);
		m_dbgeoCombo.EnableWindow(FALSE);
		return;
	}
	
	m_bSpecialLayer = FALSE;
	HTREEITEM parentItem = m_wndIdxTree.GetParentItem(m_hCurItem);
	CString groupName = m_wndIdxTree.GetItemText(parentItem);
	if (groupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP)) == 0)
	{
		m_bSpecialLayer = TRUE;
	}

	CString strNameAndCode = m_wndIdxTree.GetItemText(m_hCurItem);
	char layName[_MAX_FNAME];
	__int64 num;
	if (m_bSpecialLayer)
	{
		sscanf(strNameAndCode,"%s",layName);
	}
	else
	{
		sscanf(strNameAndCode,"%I64d %s",&num,layName);
	}
	m_StrLayName.Format("%s",layName);//hcw,2012.4.10
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}

	m_bChgFromSelect = TRUE; //{hcw,2012.3.14,recover from cancel off.
	if ((!m_bSpecialLayer)&&(m_bFirstinSearchEdit||m_bChgFromSelect))
	{
		CString strCode;
		strCode.Format("%I64d",num);
		
		m_wndCodeEdit.SetWindowText(strCode);
		m_CurStrCode = strCode;//hcw,2012.4.8
		m_wndIdxTree.m_bLButtonDown = FALSE;
	}	
//}
	m_nLayerIndex = m_config.pScheme->GetLayerDefineIndex(layName,m_bSpecialLayer);

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
	m_nClickNum = 0;

	FillColor();
	//{hcw,2012.3.15
	m_wndIdxTree.Invalidate(FALSE);
	m_wndIdxTree.UpdateData(TRUE);
	int nCount = m_wndIdxTree.GetSelectedCount();
	BOOL bMulti = m_wndIdxTree.m_bRealMulti;
    if (bMulti) 
    {	
		m_defaultgeoCombo.EnableWindow(FALSE);
		m_dbgeoCombo.EnableWindow(FALSE);
		m_wndIdxTree.m_bRealMulti = FALSE; //hcw,2012.4.27
		return ;
    }
	//}
	DrawPreview(m_nLayerIndex);
	
	FillAttList(m_nLayerIndex);
	
	FillGeo();
	
	FillSymbolList();
	


}
void CDlgScheme::OnSelItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	return;
}
void CDlgScheme::OnMoveLayer()
{
	if (m_bSpecialLayer)  return;

	CString strNameAndCode = m_wndIdxTree.GetItemText(m_hCurItem);
	char layName[_MAX_FNAME];
	int num;
	sscanf(strNameAndCode,"%i %s",&num,layName);
	
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}
	
	for(int i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
	{
		if(stricmp(m_UserIdx.m_aIdx[i].FeatureName,layName) == 0)
		{
			m_nMovedLayer = i;	
			break;
		}
	}	
	
	m_hMovedItem = m_hCurItem;
}

void CDlgScheme::OnMoveToLayer()
{
	if (m_nMovedLayer < 0 || m_bSpecialLayer)  return;

	CString strNameAndCode = m_wndIdxTree.GetItemText(m_hCurItem);
	char layName[_MAX_FNAME];
	int num;
	sscanf(strNameAndCode,"%i %s",&num,layName);
	
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}

	int nAfterLayer = -1;
	long groupidx   = -1;
	for(int i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
	{
		if(stricmp(m_UserIdx.m_aIdx[i].FeatureName,layName) == 0)
		{
			nAfterLayer = i;
			groupidx = m_UserIdx.m_aIdx[i].groupidx;
			break;
		}
	}	

	if (nAfterLayer == m_nMovedLayer || nAfterLayer+1==m_nMovedLayer)
	{
		m_nMovedLayer = -1;
		return;	
	}

	// 移动后的组号
	int nNewGroup = -1;

	if (nAfterLayer >= 0)
	{
		// 层顺序更改
		IDX idx = m_UserIdx.m_aIdx.GetAt(m_nMovedLayer);
		CSchemeLayerDefine *pLayerDefine = m_config.pScheme->GetLayerDefine(idx.FeatureName);
		BOOL bChangeGrouped = FALSE;
		if (idx.groupidx != groupidx)
		{
			idx.groupidx = groupidx;
			pLayerDefine->SetGroupName(m_UserIdx.m_aGroup[groupidx].GroupName);
		}

		m_UserIdx.m_aIdx.InsertAt(nAfterLayer+1,idx);
		m_config.pScheme->InsertLayerDefineAt(nAfterLayer+1,pLayerDefine);
		
		int index = nAfterLayer+1;
		if (nAfterLayer > m_nMovedLayer)
		{			
			m_UserIdx.m_aIdx.RemoveAt(m_nMovedLayer);
			m_config.pScheme->DelLayerDefine(m_nMovedLayer,FALSE);
			--index;
		}
		else
		{
			m_UserIdx.m_aIdx.RemoveAt(m_nMovedLayer+1);
			m_config.pScheme->DelLayerDefine(m_nMovedLayer+1,FALSE);
		}		

		// 界面显示
		char strR[_MAX_FNAME];
		if (strlen(idx.strAccel) <= 0)
		{
			sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
		}
		else
		{
			sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
		}

		HTREEITEM hParent = m_wndIdxTree.GetParentItem(m_hCurItem);
		HTREEITEM item = m_wndIdxTree.InsertSubItem(_T(strR),hParent,0,CODEID_FROMIDX(index),m_hCurItem);

		m_wndIdxTree.DeleteItem(m_hMovedItem);
		
		m_wndIdxTree.SelectItem(item);
		m_nMovedLayer = -1;
		m_bModified = TRUE;
	}
}

void CDlgScheme::OnMoveGroup()
{
	
	CString groupName = m_wndIdxTree.GetItemText(m_hCurItem);
	for(int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
	{
		if(stricmp(m_UserIdx.m_aGroup[i].GroupName,groupName) == 0)
		{
			m_nMovedGroup = i;	
			break;
		}
	}


	m_hMovedItem = m_hCurItem;

}

void CDlgScheme::OnMoveToGroup()
{
	
	if (m_nMovedGroup < 0)  return;

	CString groupName = m_wndIdxTree.GetItemText(m_hCurItem);
	int nGroup = -1;
	for(int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
	{
		if(m_UserIdx.m_aGroup[i].GroupName == groupName)
		{
			nGroup = i;	
			break;
		}
	}

	if (nGroup == m_nMovedGroup || nGroup+1==m_nMovedGroup)
	{
		m_nMovedGroup = -1;
		return;	
	}

	// 移动后的组号
	int nNewGroup = -1;

	if (nGroup >= 0)
	{
		// 受影响组中层的组序号更改
		if (nGroup > m_nMovedGroup)
		{	
			// 对于 m_nMovedGroup<groupid<=nGroup 层的组号减1，m_nMovedGroup层的组号变为nGroup
			for (i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
			{
				IDX &idx = m_UserIdx.m_aIdx.ElementAt(i);
				if (idx.groupidx == m_nMovedGroup)
				{
					idx.groupidx = nGroup;
					nNewGroup = nGroup;
				}
				else if (idx.groupidx > m_nMovedGroup && idx.groupidx <= nGroup)
				{
					idx.groupidx--;
				}		
				
				
			}
		}
		else
		{
			// 对于 nGroup<groupid<m_nMovedGroup 层的组号加1，m_nMovedGroup层的组号变为nGroup+1
			for (i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
			{
				IDX &idx = m_UserIdx.m_aIdx.ElementAt(i);
				if (idx.groupidx == m_nMovedGroup)
				{
					idx.groupidx = nGroup + 1;
					nNewGroup = nGroup + 1;
				}
				else if (idx.groupidx > nGroup && idx.groupidx < m_nMovedGroup)
				{
					idx.groupidx++;
				}				
				
				
			}
			
		}
		

		// 组顺序更改
		m_UserIdx.m_aGroup.InsertAt(nGroup+1,m_UserIdx.m_aGroup.GetAt(m_nMovedGroup));
		if (nGroup > m_nMovedGroup)
		{			
			m_UserIdx.m_aGroup.RemoveAt(m_nMovedGroup);
		}
		else
		{
			m_UserIdx.m_aGroup.RemoveAt(m_nMovedGroup+1);
		}

		// 界面显示
		CString str = m_wndIdxTree.GetItemText(m_hMovedItem);
		m_wndIdxTree.DeleteItem(m_hMovedItem);
		HTREEITEM item = m_wndIdxTree.InsertRoot(str,0,GROUPID_FROMIDX(m_UserIdx.m_aGroup.GetSize()),m_hCurItem);
		
		if (nNewGroup >= 0)
		{
			for (i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
			{
				IDX idx = m_UserIdx.m_aIdx.GetAt(i);
				
				if (idx.groupidx == nNewGroup)
				{
					char strR[_MAX_FNAME];
					if (strlen(idx.strAccel) <= 0)
					{
						sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
					}
					else
					{
						sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
					}
					m_wndIdxTree.InsertSubItem(_T(strR),item,0);
				}
				
			}
		}	
		
		m_wndIdxTree.SelectItem(item);
		m_nMovedGroup = -1;
		m_bModified = TRUE;
	}
}

void CDlgScheme::OnNewGroup() 
{
	// TODO: Add your command handler code here
	CDlgNewGroup group;
	CString strGroupName;
	if (group.DoModal() != IDOK)
		return;

	strGroupName = group.GetGroupName();
	CStringArray groupNames;
	for(int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
	{
		groupNames.Add( m_UserIdx.m_aGroup[i].GroupName );
	}
	//检查是否重名,重名则重新输入
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

	LAYGROUP lGroup;
	strcpy(lGroup.GroupName,strGroupName);
	// 没有选中任何组时，插到最后
	if (m_bClickNewGroup)
	{
		m_UserIdx.m_aGroup.InsertAt(m_UserIdx.m_aGroup.GetSize(), lGroup);
		m_wndIdxTree.InsertRoot(strGroupName,0,GROUPID_FROMIDX(m_UserIdx.m_aGroup.GetSize()));
		
		m_bModified = TRUE;
		m_bClickNewGroup = FALSE;
	}
	else
	{
		CString groupName = m_wndIdxTree.GetItemText(m_hCurItem);
		
		int nGroup = -1;
		for(i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
		{
			if(m_UserIdx.m_aGroup[i].GroupName == groupName)
			{
				nGroup = i;	
				break;
			}
		}
		if (nGroup >= 0)
		{
			m_UserIdx.m_aGroup.InsertAt(nGroup+1, lGroup);
			m_wndIdxTree.InsertRoot(strGroupName,0,GROUPID_FROMIDX(m_UserIdx.m_aGroup.GetSize()),m_hCurItem);
			
			// 更改新建组后面层的组索引号
			for (i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
			{
				IDX &idx = m_UserIdx.m_aIdx.ElementAt(i);
				if (idx.groupidx > nGroup)
				{
					idx.groupidx++;
				}			
				
			}

			m_bModified = TRUE;
		}		

	}
		
	
}


CString CDlgScheme::GetCheckedLayerName(CDlgNewLayer &layer, BOOL bShowCode)
{
	//获取所有的layerName和code
	vector<__int64> code;
	vector<CString> layerNames;
	for (int i=0; i<m_UserIdx.m_aIdx.GetSize(); i++)
	{
		layerNames.push_back(m_UserIdx.m_aIdx[i].FeatureName);
		code.push_back(m_UserIdx.m_aIdx[i].code);
	}
	__int64 curCode = 0;
	if (!code.empty())
	{
		curCode = *max_element(code.begin(),code.end())+1;
	}	
	
	CString strSetName;

	if (bShowCode)
	{
		curCode = -1;
		strSetName.Format("%I64d",curCode);
		strSetName += " ";
	}
	
	CString tmp = CheckName("layer",layerNames);	
	strSetName += tmp;
	layer.SetLayerName(strSetName);
	while (IDOK == layer.DoModal())
	{
		CString strNameAndCode = layer.GetLayerName();

		if (bShowCode)
		{
			//判断格式是否正确eg:"114 name"
			if ( !CheckLayerFormat(strNameAndCode) )
			{
				CString szText,szCaption;
				szText.LoadString(IDS_LAYERFORMAT_WRONG);
				szCaption.LoadString(IDS_ATTENTION);
				MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
				continue;
			}
		}
		
		char layerName[_MAX_FNAME];
		__int64 num;
		if (bShowCode)
		{
			sscanf(strNameAndCode, "%I64d %s", &num, layerName);
		}
		else
		{
			num = curCode;
			sscanf(strNameAndCode, "%s", layerName);
			
			strNameAndCode.Empty();
			strNameAndCode.Format("%I64d",num);
			strNameAndCode += " ";
			strNameAndCode += layerName;
		}
		
		CString strAccel;		
		char *pdest = strstr(layerName,"^");
		if (pdest != NULL)
		{
			strAccel = pdest+1;
			pdest[0] = 0;
		}
		
		//检查是否重名,重名则重新输入
		if ( FindNum(num,code) || CheckName(layerName,layerNames).CompareNoCase(layerName)!=0 )
		{
			CString szText,szCaption;
			szText.LoadString(IDS_LAYER_RENAME);
			szCaption.LoadString(IDS_RE_INPUT);
			if (IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) )
				continue;
			else 
				return "";
		}
		
		return strNameAndCode;
		
	}
	return"";	
	

}



void CDlgScheme::OnNewLayer() 
{
	// TODO: Add your command handler code here
	HTREEITEM parentItem = m_wndIdxTree.GetParentItem(m_hCurItem);
	HTREEITEM newItem;
	IDX idx;
	CString groupName;
	// 如果在组上添加层,则加在此层的最后,否在就加在选定层的下面
	if (parentItem == NULL)
	{
		//获取组名
		groupName = m_wndIdxTree.GetItemText(m_hCurItem);

		CDlgNewLayer layer;
		CString strNameAndCode = GetCheckedLayerName(layer,!m_bSpecialLayer);
		if (strNameAndCode.IsEmpty())
			return;
		
		//保存到user.idx
		int nGroup;
		for (int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
		{
			if (m_UserIdx.m_aGroup[i].GroupName == groupName)
			{
				nGroup = i;	
				break;
			}
		}
		char layerName[_MAX_FNAME];
		__int64 num;
		sscanf(strNameAndCode,"%I64d %s",&num,layerName);
		CString strAccel;		
		char *pdest = strstr(layerName,"^");
		if (pdest != NULL)
		{
			strAccel = pdest+1;
			pdest[0] = 0;
		}
		idx.code = num;
		strcpy(idx.FeatureName,layerName);
		strcpy(idx.strAccel,strAccel);
		idx.groupidx = nGroup;
		m_UserIdx.m_aIdx.Add(idx);

		//控件显示
		if (!m_bSpecialLayer)
		{
			newItem = m_wndIdxTree.InsertSubItem(strNameAndCode,m_hCurItem,0);
		}
		else
		{
			newItem = m_wndIdxTree.InsertSubItem(idx.FeatureName,m_hCurItem,0);
		}

	}
	else
	{
		//组名
		groupName = m_wndIdxTree.GetItemText(parentItem);
		//获取层名
		CString layerName = m_wndIdxTree.GetItemText(m_hCurItem);
		char curlayerName[_MAX_FNAME];
		memset(curlayerName,0x00,_MAX_FNAME);
		__int64 curnum;
		//////////////////////////////////////////////////////////////////////////
		sscanf(layerName,"%I64d %s",&curnum,curlayerName);
		//////////////////////////////////////////////////////////////////////////
		CDlgNewLayer layer;
		CString strNameAndCode = GetCheckedLayerName(layer,!m_bSpecialLayer);
		if (strNameAndCode.IsEmpty())
			return;

		//保存到user.idx
		int nGroup;
		for(int i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
		{
			if(m_UserIdx.m_aGroup[i].GroupName == groupName)
			{
				nGroup = i;	
				break;
			}
		}
		char newlayerName[_MAX_FNAME];
		__int64 newnum;
		sscanf(strNameAndCode,"%I64d %s",&newnum,newlayerName);
		CString strAccel;		
		char *pdest = strstr(newlayerName,"^");
		if (pdest != NULL)
		{
			strAccel = pdest+1;
			pdest[0] = 0;
		}
		idx.code = newnum;
		strcpy(idx.FeatureName,newlayerName);
		strcpy(idx.strAccel,strAccel);
		idx.groupidx = nGroup;
		//插到当前选择层的下面
		for(i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
		{
			
			if( strcmp(m_UserIdx.m_aIdx[i].FeatureName,curlayerName) == 0 )
			{
				m_UserIdx.m_aIdx.InsertAt(i+1,idx);	
				break;
			}
		}	

		//控件显示		
		if (!m_bSpecialLayer)
		{
			newItem = m_wndIdxTree.InsertSubItem(strNameAndCode,parentItem,0,0,m_hCurItem);
		}
		else
		{
			newItem = m_wndIdxTree.InsertSubItem(idx.FeatureName,parentItem,0,0,m_hCurItem);
		}

	}

	CSchemeLayerDefine *pdef = new CSchemeLayerDefine;
	pdef->SetGroupName(groupName);
	pdef->SetLayerCode(idx.code);
	pdef->SetLayerName(idx.FeatureName);
	pdef->SetAccel(idx.strAccel);
	pdef->SetSupportClsName(CStringArray());
	pdef->SetGeoClass(CLS_GEOPOINT);
	pdef->SetColor(RGB(255,255,255));

	if (m_nLayerIndex >= 0)
	{
		m_config.pScheme->InsertLayerDefineAt(m_nLayerIndex+1,pdef,m_bSpecialLayer);
	}
	else
		m_config.pScheme->AddLayerDefine(pdef,m_bSpecialLayer);	

	// 增加到文档
	/*if (m_pDoc && m_nCurDocScale>0)
	{
		int nComboIndex = m_scaleCombo.GetCurSel();
		if (nComboIndex >= 0)
		{
			CString strScale;
			m_scaleCombo.GetLBText(nComboIndex,strScale);
			
			if (m_nCurDocScale == atoi(strScale))
			{
				CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
				if (pDS && !pDS->GetFtrLayer(idx.FeatureName))
				{
					CFtrLayer *pLayer = new CFtrLayer;
					pLayer->SetName(idx.FeatureName);
					pLayer->SetInherentFlag(TRUE);
					pLayer->SetColor(pdef->GetColor());
					m_pDoc->AddFtrLayer(pLayer);
				}
			}
		}		
		
	}*/

	m_wndIdxTree.SelectItem(newItem/*m_hCurItem*/);

	m_wndIdxTree.RedrawWindow();

	m_bModified = TRUE;
	
}

void CDlgScheme::OnDelGroup() 
{
	// TODO: Add your command handler code here
	CString szText,szCaption;
	szText.LoadString(IDS_DEL_LAYERS);
	szCaption.LoadString(IDS_ATTENTION);
	if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
	{
		
		// 获取组名后再删除
		CString groupName = m_wndIdxTree.GetItemText(m_hCurItem);
		// 删除组并更新显示
		m_wndIdxTree.DeleteItem(m_hCurItem);

		for(int i=0;i<m_config.pScheme->GetLayerDefineCount(m_bSpecialLayer);i++)
		{
			CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(i,m_bSpecialLayer);
			if (groupName.CompareNoCase(pdef->GetGroupName()) == 0)
			{
				m_config.pScheme->DelLayerDefine(i,TRUE,m_bSpecialLayer);
				i--;
			}
		}

		// 更新USERIDX供CEdit搜索使用
		int nGroup;
		// 寻找选择的组名
		for(i=0;i<m_UserIdx.m_aGroup.GetSize();i++)
		{
			if(m_UserIdx.m_aGroup[i].GroupName == groupName)
			{
				nGroup = i;	
//				m_UserIdx.m_aGroup[nGroup].nFlag = -1;
				break;
			}
		}
		// 删除小层数据
		for(i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
		{
			//修改m_UserIdx
			if(m_UserIdx.m_aIdx[i].groupidx == nGroup)
			{
				m_UserIdx.m_aIdx.RemoveAt(i);
				i--;
			}
			// 更改此组下面所有组groupidx
			else if(m_UserIdx.m_aIdx[i].groupidx > nGroup)
			{
				m_UserIdx.m_aIdx[i].groupidx--;
			}
			
		}
		m_UserIdx.m_aGroup.RemoveAt(nGroup);

		m_bModified = TRUE;

	}
	
	
}

void CDlgScheme::OnDelLayer() 
{
	// TODO: Add your command handler code here
	
	
	UINT nSelectedCount;	
	nSelectedCount = m_wndIdxTree.GetSelectedCount();
	//{hcw,2012.2.27
	if (nSelectedCount==0)
	{
		nSelectedCount = m_nCountTreeSelected;
	}
	//}
	HTREEITEM parentItem = m_wndIdxTree.GetParentItem(m_hCurItem);

	//如果删除的是组
	if((nSelectedCount<=1)&&(parentItem == NULL))//根据选中的节点数来判断是否为多选
		OnDelGroup();
	else
	{
		//hcw,2012.2.20,添加多选支持。
		CTreeItemList treeNodeList;
		
		m_wndIdxTree.GetSelectedList(treeNodeList);
		if (treeNodeList.IsEmpty())
		{

			treeNodeList.RemoveAll();
			treeNodeList.AddHead(&m_treeNodeList);
								
		}
		CString szText,szCaption;
		szText.LoadString(IDS_DEL_LAYER);
		szCaption.LoadString(IDS_ATTENTION);
		if( IDYES != MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
				return;
		//hcw,2012.2.27, tmpTreeList to m_treeNodeList;
		while(!treeNodeList.IsEmpty())
		{
		
			if (!m_wndIdxTree.GetParentItem((HTREEITEM)treeNodeList.GetHead()))
			{
				treeNodeList.RemoveHead();
				continue;
			}

			CString slay = m_wndIdxTree.GetItemText((HTREEITEM)treeNodeList.GetHead());
			HTREEITEM hParentItem = m_wndIdxTree.GetParentItem((HTREEITEM)treeNodeList.GetHead());
			CString strParentTxt = m_wndIdxTree.GetItemText(hParentItem);

			char layerName[_MAX_FNAME];
			int num;
			sscanf(slay,"%i %s",&num,layerName);
			
			CString strAccel;		
			char *pdest = strstr(layerName,"^");
			if (pdest != NULL)
			{
				strAccel = pdest+1;
				pdest[0] = 0;
			}
			//{hcw,2012.7.17
			if (strParentTxt.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
			{
				strcpy(layerName,(LPSTR)(LPCTSTR)slay);
			}
			//}
			
			for(int i=0;i<m_config.pScheme->GetLayerDefineCount(m_bSpecialLayer);i++)
			{
				CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(i,m_bSpecialLayer);
				//{hcw,2012.5.2,for test;
				CString str = "" ;
				str.Format("%I64d", pdef->GetLayerCode());
				//}
				if(pdef->GetLayerName().CompareNoCase(layerName) == 0)
				{
					m_config.pScheme->DelLayerDefine(i,TRUE,m_bSpecialLayer);
					i--;
				}
			}
			
			//寻找选择的层名
			for(i=0;i<m_UserIdx.m_aIdx.GetSize();i++)
			{
				CString tt = m_UserIdx.m_aIdx[i].FeatureName;	
				if(strcmp(m_UserIdx.m_aIdx[i].FeatureName,layerName) == 0)
				{
					m_UserIdx.m_aIdx.RemoveAt(i);
					i--;
				}
			}
			
			//删除层并更新显示
			HTREEITEM hItem = m_wndIdxTree.GetPrevSiblingItem((HTREEITEM)treeNodeList.GetHead());
			m_wndIdxTree.DeleteItem((HTREEITEM)treeNodeList.GetHead());
			int nCount = treeNodeList.GetCount();
			//{hcw,2012.3.30			
			DrawPreview(m_nLayerIndex);			
			FillAttList(m_nLayerIndex);			
			FillGeo();			
			FillColor();//hcw,2012.4.9
			FillSymbolList();
			//}
		treeNodeList.RemoveHead();			
		m_bModified = TRUE;
		}
		

	}
	
}

LRESULT CDlgScheme::OnSchemeModify(WPARAM wParam, LPARAM lParam)
{
	if( !m_bModified )
		m_bModified = TRUE;
	return 1;
}

LRESULT CDlgScheme::OnSymbolRepaint(WPARAM wParam, LPARAM lParam)
{
	
	//重绘预览控件
	if (m_nLayerIndex >= 0)
	{
		DrawPreview(m_nLayerIndex);
	}
	if (wParam == 1)
	{
		int count = m_wndListSymbol.GetItemCount();
		for (int i=0; i<count; i++)
		{
			CreateImageItem(i);
		}
		m_wndListSymbol.RedrawWindow();
	}
	// 每次打开图元浏览前先停止预览更新
	else if (wParam == -1)
	{
		KillTimer(TIMERID_PREVIEW);
	}
	else if (wParam == -2)
	{
		SetTimer(TIMERID_PREVIEW,500,NULL);
	}
	else
	{	
		m_bModified = TRUE;

		SetTimer(TIMERID_PREVIEW,500,NULL);

		//重绘选定符号控件
		POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
		if( !pos )
			return -1;
		
		int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
		if(	CreateImageItem(nsel) )
		{
			m_wndListSymbol.RedrawWindow();
		}
		else 
			return -1;

	}
	return 1;
	
}

void CDlgScheme::DrawPreview(int nLayerIndex)
{
	if (!m_config.pCellDefLib || nLayerIndex<0)
	{
		CWnd *pWndSize = GetDlgItem(IDC_LAYERSTATIC_SIZE);
		if( pWndSize )
		{
			CString strSize = "0mm X 0mm";
			pWndSize->SetWindowText(strSize);
		}

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
		
		CWnd *pWnd = GetDlgItem(IDC_LAYERPREVIEW_STATIC);
		if( !pWnd )return;
		CRect rcView;
		pWnd->GetClientRect(&rcView);
		
		CClientDC cdc(pWnd);
		GDI_DrawGrBuffer2d(cdc.GetSafeHdc(),&buf2d,TRUE,RGB(255,255,255),matrix,rcView.Size(),rcView);

		return;
	}
	//{hcw,2012.7.26.
	m_bAbsentSchemePath = FALSE;
//	if (m_config.pScheme->GetLayerDefineCount()<=nLayerIndex)
	if(((!m_bSpecialLayer)&&m_config.pScheme->GetLayerDefineCount(FALSE)<=m_nLayerIndex)||(m_bSpecialLayer&&m_config.pScheme->GetLayerDefineCount(TRUE)<=m_nLayerIndex))
	{
		nLayerIndex = -1;
		m_nLayerIndex = -1;
		return;
	}
	//}

	CSchemeLayerDefine *pLayDef = m_config.pScheme->GetLayerDefine(nLayerIndex,m_bSpecialLayer);

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
			CSymbol *pSym = m_config.pScheme->GetLayerDefine(nLayerIndex,m_bSpecialLayer)->GetSymbol(i);
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
		pts[2].x = 20;
		pts[2].y = 0;
		pts[2].pencode = penMove;
		pts[3].x = 0;
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
			CSymbol *pSym = m_config.pScheme->GetLayerDefine(nLayerIndex,m_bSpecialLayer)->GetSymbol(i);
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
			pts[0].y = 40;
			pts[0].pencode = penLine;
			pts[1].x = 40;
			pts[1].y = 40;
			pts[1].pencode = penLine;
			pts[2].x = 40;
			pts[2].y = 0;
			pts[2].pencode = penLine;
			pts[3].x = 0;
			pts[3].y = 0;
			pts[3].pencode = penLine;
			pts[4].x = 0;
			pts[4].y = 40;
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
	Matrix44FromZoom(m_fPreviewSize,m_fPreviewSize,1.0,matrix_t);
	pGeo->Transform(matrix_t);

//	pFeature->SetGeometry(pGeo);

	// 设置当前比例尺图元线型库
	int nComboIndex = m_scaleCombo.GetCurSel();
	if (nComboIndex == CB_ERR) return;
	CString nScale;
	m_scaleCombo.GetLBText(nComboIndex,nScale);
	CSwitchScale scale(atoi(nScale));

	for(int i=0; i<nSymbolNum; i++)
	{
		CSymbol *pSym = m_config.pScheme->GetLayerDefine(nLayerIndex,m_bSpecialLayer)->GetSymbol(i);
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

	CWnd *pWnd = GetDlgItem(IDC_LAYERPREVIEW_STATIC);
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
	matrix_reverse(matrix, 3, m_transformMatrix);

	GrBuffer2d buf2d;
	buf2d.AddBuffer(&bufs);
	buf2d.RefreshEnvelope(FALSE);
	
	GDI_DrawGrBuffer2d(cdc.GetSafeHdc(),&buf2d,FALSE,0,matrix,rcView.Size(),rcView);
	
	if(pFeature)
		delete pFeature;

	if (m_nClickNum == 1 || m_nClickNum == 2)
	{
		DrawTemLine();	
	}

	CWnd *pWndSize = GetDlgItem(IDC_LAYERSTATIC_SIZE);
	if( pWndSize )
	{
		CString strSize;
		strSize.Format("%.2fmm X %.2fmm",e.Width(),e.Height());
		pWndSize->SetWindowText(strSize);
	}
}

LRESULT CDlgScheme::PopulateComboList(WPARAM wParam, LPARAM lParam)
{
	// Get the Combobox window pointer
	CComboBox* pInPlaceCombo = static_cast<CComboBox*> (GetFocus());
	
	// Get the inplace combbox top left
	CRect focWindowRect;
	
	pInPlaceCombo->GetWindowRect(&focWindowRect);
	
	CPoint obInPlaceComboTopLeft(focWindowRect.TopLeft()); 
	
	// Get the active list
	// Get the control window rect
	// If the inplace combobox top left is in the rect then
	// The control is the active control
	CRect rSymbolRect;
	m_wndAttList.GetWindowRect(&rSymbolRect);
	int iColIndex = (int )wParam;
	CStringList* pComboList = reinterpret_cast<CStringList*>(lParam);
	pComboList->RemoveAll(); 
	if (rSymbolRect.PtInRect(obInPlaceComboTopLeft)) 
	{				
		if(iColIndex == 3)
		{
			pComboList->AddTail(StrFromResID(IDS_BOOL_TYPE));
			pComboList->AddTail(StrFromResID(IDS_SMALLINT_TYPE));
			pComboList->AddTail(StrFromResID(IDS_LONG_TYPE));	
			pComboList->AddTail(StrFromResID(IDS_SINGLE_TYPE));
			pComboList->AddTail(StrFromResID(IDS_DOUBLE_TYPE));
			pComboList->AddTail(StrFromResID(IDS_CHAR_TYPE));
			pComboList->AddTail(StrFromResID(IDS_COLOR_TYPE));	
			pComboList->AddTail(StrFromResID(IDS_DATE_TYPE));	
		}
/*		else if(iColIndex == 5 ||iColIndex == 6)
		{
			pComboList->AddTail("是");
			pComboList->AddTail("否");
		}
*/	}
	
	return true;
	
}

LRESULT CDlgScheme::OnEndLabelEditVariableCriteria(WPARAM wParam, LPARAM lParam)
{
	if(m_wndAttList.m_bEdit == FALSE)
		return -1;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;

	//保存修改的属性值
	int curSel = pDispInfo->item.iItem;

	//获取当前属性值
//	ItemInfo itInfo = m_wndAttList.GetItemInfo(pDispInfo->item.iItem);

	XDefine xdef = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetXDefine(curSel);

	CString msg;
	msg.LoadString(IDS_FIELD_EXIST);	
	switch(pDispInfo->item.iSubItem)
	{
	case 0:
		{
			//检查是否重名
			vector<CString> fieldNames;
			for(int i=0;i<m_wndAttList.GetItemCount();i++)
			{
				if(i != curSel)
					fieldNames.push_back( m_wndAttList.GetItemText(i,0) );
			}			

			if( CheckName(pDispInfo->item.pszText,fieldNames).CompareNoCase(pDispInfo->item.pszText)  == 0 ) 
			{
				strcpy(xdef.field,pDispInfo->item.pszText);
				m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			}
			else
			{
				MessageBox(msg,NULL,MB_OK|MB_ICONASTERISK);
				return -1;
			}

			break;	
		}
	case 1:
		{
			vector<CString> alaisNames;
			for(int i=0;i<m_wndAttList.GetItemCount();i++)
			{
				if(i != curSel)
					alaisNames.push_back( m_wndAttList.GetItemText(i,1) );
			}			
			
			if( CheckName(pDispInfo->item.pszText,alaisNames).CompareNoCase(pDispInfo->item.pszText)  == 0 ) 
			{
				strcpy(xdef.name,pDispInfo->item.pszText);
				m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			}
			else
			{
				MessageBox(msg,NULL,MB_OK|MB_ICONASTERISK);
				return -1;
			}
			
			break;
		}
	case 2:
		{
			strcpy(xdef.defvalue,pDispInfo->item.pszText);
			m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			break;
		}
	case 3:
		{
			if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_LONG_TYPE)) == 0)
				xdef.valuetype = DP_CFT_INTEGER;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_BOOL_TYPE)) ==0)
				xdef.valuetype = DP_CFT_BOOL;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_SMALLINT_TYPE)) ==0)
				xdef.valuetype = DP_CFT_SMALLINT;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_SINGLE_TYPE)) ==0)
				xdef.valuetype = DP_CFT_FLOAT;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_DOUBLE_TYPE)) ==0)
				xdef.valuetype = DP_CFT_DOUBLE;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_CHAR_TYPE)) ==0)
				xdef.valuetype = DP_CFT_VARCHAR;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_COLOR_TYPE)) ==0)
				xdef.valuetype = DP_CFT_COLOR;
			else if(stricmp(pDispInfo->item.pszText,StrFromResID(IDS_DATE_TYPE)) ==0)
				xdef.valuetype = DP_CFT_DATE;

			m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			break;
		}
	case 4:
		{
			xdef.valuelen = atoi(pDispInfo->item.pszText);
			m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			break;
		}
	case 5:
		{
			xdef.isMust = atoi(pDispInfo->item.pszText);
			m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			break;
		}
	case 6:
		{
			xdef.nDigit = atoi(pDispInfo->item.pszText);
			m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			break;
		}
	case 7:
		{
			strcpy(xdef.valueRange,pDispInfo->item.pszText);
			m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetXDefine(curSel,xdef);
			break;
		}
	default:
		break;
	}

	m_bModified = TRUE;

	if (m_pAnnoDlg)
	{
		m_pAnnoDlg->UpdataFieldComobo();
	}

	return 1;
		

}

void CDlgScheme::OnAddAttButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0 || m_bSpecialLayer)
		return;

	if( !m_bModified )
		m_bModified = TRUE;

	CStringArray defaultField;
	defaultField.Add("name");
	defaultField.Add("alais");
	defaultField.Add("0");
	defaultField.Add(StrFromResID(IDS_CHAR_TYPE));
	defaultField.Add("100");

	vector<CString> fieldNames,alaisNames;

	// 属性重名判断赋值
	for(int i=0; i<m_wndAttList.GetItemCount(); i++)
		fieldNames.push_back( m_wndAttList.GetItemText(i,0) );

	defaultField[0] = CheckName("name",fieldNames);

	// 别名重名判断赋值
	for (i=0; i<m_wndAttList.GetItemCount(); i++)
		alaisNames.push_back( m_wndAttList.GetItemText(i,1) );
	
	defaultField[1] = CheckName("alais",alaisNames);

	//将所有项状态设置为没有选中,将增加的一项设置为选中状态
	for(i=0;i<m_wndAttList.GetItemCount();i++)
		m_wndAttList.SetItemState(i,0,LVIS_SELECTED);
	
	int curSel = m_wndAttList.InsertItem(LVIF_TEXT|LVIF_STATE, m_wndAttList.GetItemCount(), 
		"1", LVIS_SELECTED, LVIS_SELECTED, 0, 0);
	for(i=0;i<defaultField.GetSize();i++)			
		m_wndAttList.SetItemText(curSel,i,(LPCTSTR)defaultField[i]);

	//添加到相应层的属性中
	XDefine xdef;
	strcpy(xdef.field,defaultField[0]);
	strcpy(xdef.name,defaultField[1]);
	strcpy(xdef.defvalue,"0");
	xdef.valuetype = DP_CFT_VARCHAR;
	xdef.valuelen = 100;

	m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddXDefine(xdef);

	if (m_pAnnoDlg)
	{
		m_pAnnoDlg->UpdataFieldComobo();
	}
	
}

void CDlgScheme::OnDelAttButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0 || m_bSpecialLayer)
		return;

	if(m_wndAttList.GetSelectedCount() != 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_SEL_ONLYONE);
		szCaption.LoadString(IDS_ATTENTION);
		MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
		//将所有项状态设置为没有选中
		for(int i=0;i<m_wndAttList.GetItemCount();i++)
			m_wndAttList.SetItemState(i,0,LVIS_SELECTED);
		return;
	}
	if( !m_bModified )
		m_bModified = TRUE;

	POSITION pos = m_wndAttList.GetFirstSelectedItemPosition();
	int curSel = m_wndAttList.GetNextSelectedItem(pos);
	
	m_wndAttList.DeleteItem(curSel);

	m_config.pScheme->GetLayerDefine(m_nLayerIndex)->DelXDefine(curSel);


	//设置选中状态
	if(curSel == 0)
		m_wndAttList.SetItemState(curSel,LVIS_SELECTED,LVIS_SELECTED);
	else
		m_wndAttList.SetItemState(curSel-1,LVIS_SELECTED,LVIS_SELECTED);

	if (m_pAnnoDlg)
	{
		m_pAnnoDlg->UpdataFieldComobo();
	}
	
}

void CDlgScheme::OnPasteAttButton() 
{
	// TODO: Add your control notification handler code here
	if(m_bSpecialLayer)
		return;

	if( !m_bModified )
		m_bModified = TRUE;

	if(m_nLayerIndex<0)
	{
		if(m_hCurItem==NULL)
			return;
		
		CString strGroupName = m_wndIdxTree.GetItemText(m_hCurItem);
		if(strGroupName.IsEmpty())
			return;

		for(int k=0; k<m_config.pScheme->GetLayerDefineCount(); k++)
		{
			CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(k);
			if (pdef->GetGroupName().CompareNoCase(strGroupName) == 0)
			{
				for(int i=0;i<copyItemList.size();i++)
				{
					//重名检查,如果有重名属性,加后缀
					vector<CString> fieldNames;
					int size;
					const XDefine *defs = pdef->GetXDefines(size);
					for(int j=0; j<size; j++)
						fieldNames.push_back( defs[j].field );

					CString strCheckedName = CheckName(copyItemList[i].nSubItem[0],fieldNames);
					if(strCheckedName.CompareNoCase(copyItemList[i].nSubItem[0])!=0)
						continue;
					
					//将copyItemList内容添加到相应的层属性field中
					XDefine xdef;
					strcpy(xdef.field,strCheckedName);
					strcpy(xdef.name,(LPCTSTR)copyItemList[i].nSubItem[1]);
					strcpy(xdef.defvalue,(LPCTSTR)copyItemList[i].nSubItem[2]);
					if(stricmp(copyItemList[i].nSubItem[3],StrFromResID(IDS_LONG_TYPE)) == 0)
						xdef.valuetype = DP_CFT_INTEGER;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_BOOL_TYPE)) == 0)
						xdef.valuetype = DP_CFT_BOOL;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_SMALLINT_TYPE)) == 0)
						xdef.valuetype = DP_CFT_SMALLINT;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_SINGLE_TYPE)) == 0)
						xdef.valuetype = DP_CFT_FLOAT;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_DOUBLE_TYPE)) == 0)
						xdef.valuetype = DP_CFT_DOUBLE;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_CHAR_TYPE)) == 0)
						xdef.valuetype = DP_CFT_VARCHAR;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_COLOR_TYPE)) == 0)
						xdef.valuetype = DP_CFT_COLOR;
					else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_DATE_TYPE)) == 0)
						xdef.valuetype = DP_CFT_DATE;

					xdef.valuelen = atoi(copyItemList[i].nSubItem[4]);
					xdef.isMust = atoi(copyItemList[i].nSubItem[5]);
					xdef.nDigit = atoi(copyItemList[i].nSubItem[6]);
					strcpy(xdef.valueRange,(LPCTSTR)copyItemList[i].nSubItem[7]);
					
					pdef->AddXDefine(xdef);					
				}
			}
		}
		
		return;
	}

	for(int i=0;i<copyItemList.size();i++)
	{
		//重名检查,如果有重名属性,加后缀
		vector<CString> fieldNames;
		for(int j=0; j<m_wndAttList.GetItemCount(); j++)
			fieldNames.push_back( m_wndAttList.GetItemText(j,0) );

		CString strCheckedName = CheckName(copyItemList[i].nSubItem[0],fieldNames);
		
		//将copyItemList内容添加到相应的层属性field中
		XDefine xdef;
		strcpy(xdef.field,strCheckedName);
		strcpy(xdef.name,(LPCTSTR)copyItemList[i].nSubItem[1]);
		strcpy(xdef.defvalue,(LPCTSTR)copyItemList[i].nSubItem[2]);
		if(stricmp(copyItemList[i].nSubItem[3],StrFromResID(IDS_LONG_TYPE)) == 0)
			xdef.valuetype = DP_CFT_INTEGER;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_BOOL_TYPE)) == 0)
			xdef.valuetype = DP_CFT_BOOL;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_SMALLINT_TYPE)) == 0)
			xdef.valuetype = DP_CFT_SMALLINT;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_SINGLE_TYPE)) == 0)
			xdef.valuetype = DP_CFT_FLOAT;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_DOUBLE_TYPE)) == 0)
			xdef.valuetype = DP_CFT_DOUBLE;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_CHAR_TYPE)) == 0)
			xdef.valuetype = DP_CFT_VARCHAR;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_COLOR_TYPE)) == 0)
			xdef.valuetype = DP_CFT_COLOR;
		else if(stricmp(copyItemList[i].nSubItem[3], StrFromResID(IDS_DATE_TYPE)) == 0)
			xdef.valuetype = DP_CFT_DATE;

		xdef.valuelen = atoi(copyItemList[i].nSubItem[4]);
		xdef.isMust = atoi(copyItemList[i].nSubItem[5]);
		xdef.nDigit = atoi(copyItemList[i].nSubItem[6]);
		strcpy(xdef.valueRange,(LPCTSTR)copyItemList[i].nSubItem[7]);
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddXDefine(xdef);
	
		//在ListCtrl中显示
		int index = m_wndAttList.InsertItem(LVIF_TEXT|LVIF_STATE, m_wndAttList.GetItemCount(), 
			"1", 0, LVIS_SELECTED, 0, 0);
		for (j=0; j<copyItemList[i].nSubItem.size(); j++)
		{
			if(j == 0)
				m_wndAttList.SetItemText(index,j,strCheckedName);
			else
				m_wndAttList.SetItemText(index,j,copyItemList[i].nSubItem[j]);
		}
		
	}

	if (m_pAnnoDlg)
	{
		m_pAnnoDlg->UpdataFieldComobo();
	}
	
}

void CDlgScheme::OnCopyAttButton() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0 || m_bSpecialLayer)  return;

	copyItemList.clear();
	POSITION pos = m_wndAttList.GetFirstSelectedItemPosition();
	if (pos == NULL)
		MessageBox(StrFromResID(IDS_SEL_NOITEM),NULL,MB_OK|MB_ICONASTERISK);
	else
	{
		while (pos)
		{
			int nItem = m_wndAttList.GetNextSelectedItem(pos);

			ItemInfo tmpItem;
			tmpItem.nItem = nItem;
			for(int j=0;j<m_wndAttList.GetColumnCounts();j++)
			{
				CString text = m_wndAttList.GetItemText(nItem,j);
				tmpItem.nSubItem.push_back(text);
			}
			copyItemList.push_back(tmpItem);
		}
	}
	
}

void CDlgScheme::OnUpAttButton() 
{
	// TODO: Add your control notification handler code here
	if (m_bSpecialLayer) return;

	if (m_wndAttList.GetSelectedCount() != 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_SEL_ONLYONE);
		szCaption.LoadString(IDS_ATTENTION);
		MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
		for(int i=0;i<m_wndAttList.GetItemCount();i++)
			m_wndAttList.SetItemState(i,0,LVIS_SELECTED);
		return;
	}
	if (!m_bModified)
		m_bModified = TRUE;

	//get select index
	POSITION pos = m_wndAttList.GetFirstSelectedItemPosition();
	int nItem = m_wndAttList.GetNextSelectedItem(pos);


	if (nItem > 0)
	{
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->SwapXDefine(nItem-1,nItem);
		m_wndAttList.SwapItem(nItem,nItem-1);
		m_wndAttList.SetSelectionMark(nItem-1);
		m_wndAttList.SetItemState(nItem,0,LVIS_SELECTED);
		m_wndAttList.SetItemState(nItem-1,LVIS_SELECTED,LVIS_SELECTED);
		
	}
	
}

void CDlgScheme::OnDownAttButton() 
{
	// TODO: Add your control notification handler code here
	if (m_bSpecialLayer) return;

	if (m_wndAttList.GetSelectedCount() != 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_SEL_ONLYONE);
		szCaption.LoadString(IDS_ATTENTION);
		MessageBox(szText,szCaption,MB_OK|MB_ICONASTERISK);
		for(int i=0;i<m_wndAttList.GetItemCount();i++)
			m_wndAttList.SetItemState(i,0,LVIS_SELECTED);
		return;
	}
	if (!m_bModified)
		m_bModified = TRUE;

	//get select index
	POSITION pos = m_wndAttList.GetFirstSelectedItemPosition();
	int nItem = m_wndAttList.GetNextSelectedItem(pos);
	
	
	if (nItem>-1 && nItem<m_wndAttList.GetItemCount()-1)
	{
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->SwapXDefine(nItem,nItem+1);
		m_wndAttList.SwapItem(nItem,nItem+1);
		m_wndAttList.SetSelectionMark(nItem+1);
		m_wndAttList.SetItemState(nItem,0,LVIS_SELECTED);
		m_wndAttList.SetItemState(nItem+1,LVIS_SELECTED,LVIS_SELECTED);
		
	}
	
}


void CDlgScheme::OnCopyToOtherLayer() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0 || m_bSpecialLayer)  return;

	vector<ItemInfo> copyList;

	if(m_wndAttList.GetItemCount()<=0)
		return;

	UpdateData(TRUE);

	int ncount = m_wndAttList.GetItemCount();
	for(int i=0; i<ncount; i++)
	{
		ItemInfo tmpItem;
		tmpItem.nItem = i;
		for(int j=0;j<m_wndAttList.GetColumnCounts();j++)
		{
			CString text = m_wndAttList.GetItemText(i,j);
			tmpItem.nSubItem.push_back(text);
		}
		copyList.push_back(tmpItem);
	}

	for(i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(i);
		if(pdef->GetDBLayerName().CompareNoCase(m_strDBLayName) == 0)
		{
			pdef->DelXDefines();

			//将 copyList 内容添加到相应的层属性field中
			for(int j=0; j<copyList.size(); j++)
			{
				XDefine xdef;
				strcpy(xdef.field,(LPCTSTR)copyList[j].nSubItem[0]);
				strcpy(xdef.name,(LPCTSTR)copyList[j].nSubItem[1]);
				strcpy(xdef.defvalue,(LPCTSTR)copyList[j].nSubItem[2]);
				if(stricmp(copyList[j].nSubItem[3],StrFromResID(IDS_LONG_TYPE)) == 0)
					xdef.valuetype = DP_CFT_INTEGER;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_BOOL_TYPE)) == 0)
					xdef.valuetype = DP_CFT_BOOL;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_SMALLINT_TYPE)) == 0)
					xdef.valuetype = DP_CFT_SMALLINT;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_SINGLE_TYPE)) == 0)
					xdef.valuetype = DP_CFT_FLOAT;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_DOUBLE_TYPE)) == 0)
					xdef.valuetype = DP_CFT_DOUBLE;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_CHAR_TYPE)) == 0)
					xdef.valuetype = DP_CFT_VARCHAR;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_COLOR_TYPE)) == 0)
					xdef.valuetype = DP_CFT_COLOR;
				else if(stricmp(copyList[j].nSubItem[3], StrFromResID(IDS_DATE_TYPE)) == 0)
					xdef.valuetype = DP_CFT_DATE;

				xdef.valuelen = atoi(copyList[j].nSubItem[4]);
				xdef.isMust = atoi(copyList[j].nSubItem[5]);
				xdef.nDigit = atoi(copyList[j].nSubItem[6]);
				strcpy(xdef.valueRange,(LPCTSTR)copyList[j].nSubItem[7]);
				
				pdef->AddXDefine(xdef);	
				
				m_bModified = TRUE;
			}
		}
	}

	AfxMessageBox(IDS_PROCESS_OK);
}


void CDlgScheme::OnColorStatic() 
{
	// TODO: Add your control notification handler code here
	if (m_bSpecialLayer) return;

	COLORREF col = m_Color.GetColor();
	// 支持修改组颜色
	if (m_nLayerIndex < 0)
	{
		if (IDYES != AfxMessageBox(IDS_SCHEME_GROUPCOLORMODIFY,MB_YESNO|MB_ICONQUESTION))
		{
			COLORREF bkColor = GetSysColor(COLOR_3DFACE);
			m_Color.SetColor(bkColor);
			return;
		}

		CString strGroupName = m_wndIdxTree.GetItemText(m_hCurItem);

		for(int i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
		{
			CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(i);
			if (pdef->GetGroupName().CompareNoCase(strGroupName) == 0)
			{
				pdef->SetColor(col);
				ModifyLayerColor item;
				item.name = pdef->GetLayerName();
				item.color = col;
				for(int j=0; j<m_arrModifyLayerColor.GetSize(); j++)
				{
					if(item.name==m_arrModifyLayerColor[j].name)
					{
						m_arrModifyLayerColor[j].name = item.name;
						m_arrModifyLayerColor[j].color = item.color;
					}
				}
				if(j==m_arrModifyLayerColor.GetSize())
				{
					m_arrModifyLayerColor.Add(item);
				}
			}
		}
	}
	else
	{
		m_nLayerColor = col;	
		CSchemeLayerDefine *pdef = m_config.pScheme->GetLayerDefine(m_nLayerIndex);
		pdef->SetColor(m_nLayerColor);
		ModifyLayerColor item;
		item.name = pdef->GetLayerName();
		item.color = col;
		for(int j=0; j<m_arrModifyLayerColor.GetSize(); j++)
		{
			if(item.name==m_arrModifyLayerColor[j].name)
			{
				m_arrModifyLayerColor[j].name = item.name;
				m_arrModifyLayerColor[j].color = item.color;
			}
		}
		if(j==m_arrModifyLayerColor.GetSize())
		{
			m_arrModifyLayerColor.Add(item);
		}
	}

	// 修改文档
	/*if (m_pDoc && m_nCurDocScale>0)
	{
		int nComboIndex = m_scaleCombo.GetCurSel();
		if (nComboIndex >= 0)
		{
			CString strScale;
			m_scaleCombo.GetLBText(nComboIndex,strScale);
			
			if (m_nCurDocScale == atoi(strScale))
			{
				CDlgDataSource *pDS = m_pDoc->GetDlgDataSource();
				if (pDS)
				{ 
					for (int i=0; i<arrLayerName.GetSize(); i++)
					{
						CFtrLayer *pFtrLayer = pDS->GetFtrLayer(arrLayerName[i]);
						if (pFtrLayer)
						{
							pFtrLayer->SetColor(col);
							m_pDoc->UpdateFtrLayer(pFtrLayer);
						}
					}
					
				}
			}
		}		
		
	}*/

	if (!m_bModified)
		m_bModified = TRUE;
	//全部重绘
	SendMessage(WM_SYMBOL_REPAINT,1);

}

BOOL CDlgScheme::FillColor()
{
	if(m_nLayerIndex < 0)
		return FALSE;
	//{hcw,2012.7.26,若方案库未加载直接退出。
	if (m_bAbsentSchemePath)
	{
		return FALSE;
	}
//	if(m_config.pScheme->GetLayerDefineCount()<=m_nLayerIndex)
	if(((!m_bSpecialLayer)&&m_config.pScheme->GetLayerDefineCount(FALSE)<=m_nLayerIndex)||(m_bSpecialLayer&&m_config.pScheme->GetLayerDefineCount(TRUE)<=m_nLayerIndex))
	{
		m_nLayerIndex = -1;
		return FALSE;
	}
	//}
	m_nLayerColor = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetColor();		
	m_Color.SetColor(m_nLayerColor);	

	return TRUE;

}

BOOL CDlgScheme::FillGeo()
{
	if (m_nLayerIndex < 0)
	{
		m_defaultgeoCombo.EnableWindow(FALSE);
		m_dbgeoCombo.EnableWindow(FALSE);
		return FALSE;
	}
	//{hcw,2012.7.26,若方案库未加载直接退出。
	if (m_bAbsentSchemePath)
	{
		return FALSE;
	}
//	if(m_config.pScheme->GetLayerDefineCount()<=m_nLayerIndex)
	if(((!m_bSpecialLayer)&&m_config.pScheme->GetLayerDefineCount(FALSE)<=m_nLayerIndex)||(m_bSpecialLayer&&m_config.pScheme->GetLayerDefineCount(TRUE)<=m_nLayerIndex))
	{
		m_nLayerIndex=-1;
		return FALSE;
	}
	//}
	m_defaultgeoCombo.EnableWindow(TRUE);
	m_dbgeoCombo.EnableWindow(TRUE);

	int nGeo = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	int nDbGeo = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetDbGeoClass();
	
	for (int i=0; i<m_defaultgeoCombo.GetCount(); i++)
	{
		if (m_defaultgeoCombo.GetItemData(i) == nGeo)
		{
			m_defaultgeoCombo.SetCurSel(i);
			break;
		}
	}

	for ( i=0; i<m_dbgeoCombo.GetCount(); i++)
	{
		if (m_dbgeoCombo.GetItemData(i) == nDbGeo)
		{
			m_dbgeoCombo.SetCurSel(i);
			break;
		}
	}

 	CStringArray strGeoName;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSupportClsName(strGeoName);
	m_strSupportGeoName.Empty();
	for (i=0; i<strGeoName.GetSize(); i++)
	{
		m_strSupportGeoName += strGeoName.GetAt(i);
		if (i == strGeoName.GetSize()-1)
			break;
		
		m_strSupportGeoName += ",";

	}

	m_strDBLayName = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetDBLayerName();

	UpdateData(FALSE);

	return TRUE;
}

void CDlgScheme::OnSelchangeDefaultgeoCombo() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)
		return;

	int nComboIndex = m_defaultgeoCombo.GetCurSel();
	int nDefGeo = m_defaultgeoCombo.GetItemData(nComboIndex);

	BOOL  bSupport = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->IsSupportGeoClass(nDefGeo);
	if (!bSupport)
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORTLAYER),MB_OK|MB_ICONASTERISK);
		int nGeo = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		
		for (int i=0; i<m_defaultgeoCombo.GetCount(); i++)
		{
			if (m_defaultgeoCombo.GetItemData(i) == nGeo)
			{
				m_defaultgeoCombo.SetCurSel(i);
				break;
			}
		}
		return;
	}

	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetGeoClass(nDefGeo);
	// 重置当前层的缺省命令
	OnResetDefaultcommand();

	if (m_pAnnoDlg)
	{
		m_pAnnoDlg->UpdataFieldComobo();
	}


	if (!m_bModified)
		m_bModified = TRUE;

	SendMessage(WM_SYMBOL_REPAINT);
	
	
}

void CDlgScheme::OnSelchangeDbgeoCombo() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)
		return;
	
	int nComboIndex = m_dbgeoCombo.GetCurSel();
	int nDefGeo = m_dbgeoCombo.GetItemData(nComboIndex);
	
	BOOL  bSupport = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->IsSupportGeoClass(nDefGeo);
	if (nDefGeo!=CLS_NULL && !bSupport)
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORTLAYER),MB_OK|MB_ICONASTERISK);
		int nGeo = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetDbGeoClass();
		
		for (int i=0; i<m_dbgeoCombo.GetCount(); i++)
		{
			if (m_dbgeoCombo.GetItemData(i) == nGeo)
			{
				m_dbgeoCombo.SetCurSel(i);
				break;
			}
		}
		return;
	}
	
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetDbGeoClass(nDefGeo);
	// 重置当前层的缺省命令
	/*OnResetDefaultcommand();
	
	if (m_pAnnoDlg)
	{
		m_pAnnoDlg->UpdataFieldComobo();
	}*/
	
	
	if (!m_bModified)
		m_bModified = TRUE;
	
	SendMessage(WM_SYMBOL_REPAINT);
	
	
}

void CDlgScheme::ReleaseDlgMemory()
{
	if (m_pCellDlg)
	{
		delete m_pCellDlg;
		m_pCellDlg = NULL;
	}
	if (m_pDashLineDlg)
	{
		delete m_pDashLineDlg;
		m_pDashLineDlg = NULL;
	}
	if (m_pCellLineDlg)
	{
		delete m_pCellLineDlg;
		m_pCellLineDlg = NULL;
	}
	if (m_pScaleCellDlg)
	{
		delete m_pScaleCellDlg;
		m_pScaleCellDlg = NULL;
	}	
	if (m_pColorHatchDlg)
	{
		delete m_pColorHatchDlg;
		m_pColorHatchDlg = NULL;
	}
	if (m_pCondColorHatchDlg)
	{
		delete m_pCondColorHatchDlg;
		m_pCondColorHatchDlg = NULL;
	}
	if (m_pCellHatchDlg)
	{
		delete m_pCellHatchDlg;
		m_pCellHatchDlg = NULL;
	}
	if (m_pAnnoDlg)
	{
		delete m_pAnnoDlg;
		m_pAnnoDlg = NULL;
	}
	if (m_pScaleDlg)
	{
		delete m_pScaleDlg;
		m_pScaleDlg = NULL;
	}
	if (m_pLinefillDlg)
	{
		delete m_pLinefillDlg;
		m_pLinefillDlg = NULL;
	}
	if (m_pDiagonalDlg)
	{
		delete m_pDiagonalDlg;
		m_pDiagonalDlg = NULL;
	}
	if (m_pParalineDlg)
	{
		delete m_pParalineDlg;
		m_pParalineDlg = NULL;
	}
	if (m_pAngBisectorDlg)
	{
		delete m_pAngBisectorDlg;
		m_pAngBisectorDlg = NULL;
	}
	if (m_pScaleArcDlg)
	{
		delete m_pScaleArcDlg;
		m_pScaleArcDlg = NULL;
	}
	if (m_pScaleTurnplateSetting)
	{
		delete m_pScaleTurnplateSetting;
		m_pScaleTurnplateSetting = NULL;
	}
	if (m_pScaleCraneSetting)
	{
		delete m_pScaleCraneSetting;
		m_pScaleCraneSetting = NULL;
	}
	if (m_pScaleFunnelSetting)
	{
		delete m_pScaleFunnelSetting;
		m_pScaleFunnelSetting = NULL;
	}

	if (m_pScaleChuanSongDaiSetting)
	{
		delete m_pScaleChuanSongDaiSetting;
		m_pScaleChuanSongDaiSetting = NULL;
	}

	if( m_pProcSymLTSetting )
	{
		delete m_pProcSymLTSetting;
		m_pProcSymLTSetting = NULL;
	}

	if( m_pScaleYouGuanDlg )
	{
		delete m_pScaleYouGuanDlg;
		m_pScaleYouGuanDlg = NULL;
	}

	if( m_pScaleCellLTDlg )
	{
		delete m_pScaleCellLTDlg;
		m_pScaleCellLTDlg = NULL;
	}
	if( m_pScaleOldDouyaDlg )
	{
		delete m_pScaleOldDouyaDlg;
		m_pScaleOldDouyaDlg = NULL;
	}	
	if( m_pScaleXiepoDlg )
	{
		delete m_pScaleXiepoDlg;
		m_pScaleXiepoDlg = NULL;
	}

	if( m_pTidalWaterDlg )
	{
		delete m_pTidalWaterDlg;
		m_pTidalWaterDlg = NULL;
	}

	if(m_pCulvertSurfaceDlg)
	{
		delete m_pCulvertSurfaceDlg;
		m_pCulvertSurfaceDlg = NULL;
	}

	
}

void CDlgScheme::ClearCtrls()
{
	COLORREF bkColor = GetSysColor(COLOR_3DFACE);
	CRect rect;
	
	//preview
	CWnd *pWnd = GetDlgItem(IDC_LAYERPREVIEW_STATIC);
	CClientDC cdcPreview(pWnd);
	pWnd->GetClientRect(&rect);
	cdcPreview.FillRect(&rect,&CBrush(bkColor));

	//list 
	m_wndAttList.DeleteAllItems();
	
	//color
	m_Color.SetColor(bkColor);

	//geo
	m_defaultgeoCombo.SetCurSel(-1);
	m_dbgeoCombo.SetCurSel(-1);
	m_strSupportGeoName.Empty();
	
	//符号
	m_wndListSymbol.DeleteAllItems();
	m_listImages.DeleteImageList();
	m_arrIdxCreateFlag.RemoveAll();

	
	UpdateData(FALSE);
}

void CDlgScheme::DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf)
{
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
	
	// 设置当前比例尺图元线型库
	int nComboIndex = m_scaleCombo.GetCurSel();
	if (nComboIndex == CB_ERR) return;

	CString nScale;
	m_scaleCombo.GetLBText(nComboIndex,nScale);
	CSwitchScale scale(atoi(nScale));

	GDI_DrawGrBuffer2d(hDC, pBuf, FALSE,0, matrix, CSize(cx,cy), CRect(0,0,cx,cy));	
	
	::SelectObject(hDC,hOldPen);
	::SelectObject(hDC,hOldBmp);
	
	m_listImages.Replace(idx,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
}

void CDlgScheme::CreateImageList(int cx, int cy, int nsize)
{
	if(m_bAddSymbol && m_listImages.m_hImageList!=NULL)
	{
		m_listImages.Copy(0, m_wndListSymbol.GetImageList(LVSIL_NORMAL), 0, ILCF_MOVE);
	}
	else
		m_listImages.Create(cx,cy,ILC_COLOR24,0,nsize);
	
	m_listImages.SetImageCount(nsize);
	m_wndListSymbol.SetImageList(&m_listImages,TVSIL_NORMAL);
	m_arrIdxCreateFlag.SetSize(nsize);
	
	GrBuffer2d buf;
	if(m_bAddSymbol)
	{
		DrawImageItem(nsize-1,cx,cy,&buf);
		m_nIdxToCreateImage = nsize-1;
	}
	else
	{
		for( int i=0; i<nsize; i++)
		{
			DrawImageItem(i,cx,cy,&buf);
		}
		m_nIdxToCreateImage = 0;
	}
	
	m_nImageWid = cx;  m_nImageHei = cy;
//	m_nIdxToCreateImage = 0;
	
	SetTimer(TIMERID_CREATEIMAGE,50,NULL);
	SetTimer(TIMERID_PREVIEW,500,NULL);
}

BOOL CDlgScheme::CreateImageItem(int idx)
{
	if( !m_config.pCellDefLib )return FALSE;

	// 设置当前比例尺图元线型库
	int nComboIndex = m_scaleCombo.GetCurSel();
	if (nComboIndex == CB_ERR) return FALSE;
	CString nScale;
	m_scaleCombo.GetLBText(nComboIndex,nScale);
	CSwitchScale scale(atoi(nScale));
	
	CSymbol *pSym = (CSymbol*)m_wndListSymbol.GetItemData(idx);
	GrBuffer2d buf2d;
	CString strName;
	int nType = pSym->GetType();
	switch(nType)
	{
	case SYMTYPE_CELL:
		{
			CCell *pCell = (CCell*)pSym;
			strName = pCell->m_strCellDefName;

			CellDef def = m_config.pCellDefLib->GetCellDef(strName);

			DefToBuf(&def, &buf2d);
			break;

		}

	case SYMTYPE_CELLLINETYPE:
		{
			CCellLinetype *pCellLine = (CCellLinetype*)pSym;
			strName = pCellLine->m_strCellDefName;

			CellDef def = m_config.pCellDefLib->GetCellDef(strName);
			DefToBuf(&def, &buf2d);
			break;
		}
	case SYMTYPE_CELLHATCH:
		{
			CCellHatch *pCellHatch = (CCellHatch*)pSym;
			strName = pCellHatch->m_strCellDefName;

			CellDef def = m_config.pCellDefLib->GetCellDef(strName);
			
			DefToBuf(&def, &buf2d);
			break;
		}
	case SYMTYPE_DASHLINETYPE:
	case SYMTYPE_TIDALWATER:
		{
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();

			int nGeoClass = pGeo->GetClassType();

			if(nGeoClass==CLS_GEOCURVE || nGeoClass==CLS_GEOTEXT || nGeoClass==CLS_GEOPARALLEL)
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
			else if (nGeoClass == CLS_GEOSURFACE)
			{
				PT_3DEX pts[5];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 0;
				pts[1].y = 20;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 20;
				pts[3].y = 0;
				pts[3].pencode = penLine;
				pts[4].x = 0;
				pts[4].y = 0;
				pts[4].pencode = penLine;
				pGeo->CreateShape(pts,5);
			}
			
			GrBuffer buf;
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			buf2d.AddBuffer(&buf);

			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}

			break;
		}
	case SYMTYPE_SCALELINETYPE:
	case SYMTYPE_PARATYPE:
	case SYMTYPE_SCALEARCTYPE:
	case SYMTYPE_SCALECRANETYPE:
	case SYMTYPE_SCALE_CellLinetype:
	case SYMTYPE_SCALE_ChuanSongDai:
	case SYMTYPE_SCALE_OldDouya:
	case SYMTYPE_SCALE_Xiepo:
		{
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();

			if (nGeoClass == CLS_GEOPARALLEL)
			{
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

			GrBuffer buf;
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			buf2d.AddBuffer(&buf);

			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}

			break;
		}
	case SYMTYPE_SCALEFUNNELTYPE:
		{
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();
			
			CScaleFunneltype *pFunnel = (CScaleFunneltype*)pSym;
			if (!pFunnel) break;

			if (pFunnel->m_nFunnelType == 0)
			{
				if (nGeoClass == CLS_GEOPARALLEL)
				{
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
					pts[2].x = 0;
					pts[2].y = 0;
					pts[2].pencode = penMove;
					pts[3].x = 20;
					pts[3].y = 0;
					pts[3].pencode = penLine;
					pGeo->CreateShape(pts,4);
				}
			}	
			else
			{
				int pencode = penLine;
				if (pFunnel->m_nFunnelType == 1 || pFunnel->m_nFunnelType == 3)
				{
					pencode = penArc;
				}

				if (nGeoClass == CLS_GEOSURFACE)
				{
					PT_3DEX pts[5];
					pts[0].x = 0;
					pts[0].y = 0;
					pts[0].pencode = pencode;
					pts[1].x = 0;
					pts[1].y = 20;
					pts[1].pencode = pencode;
					pts[2].x = 20;
					pts[2].y = 20;
					pts[2].pencode = pencode;
					pts[3].x = 20;
					pts[3].y = 0;
					pts[3].pencode = pencode;
					pts[4].x = 0;
					pts[4].y = 0;
					pts[4].pencode = pencode;
					pGeo->CreateShape(pts,5);
				}
				else if (nGeoClass == CLS_GEOCURVE)
				{
					PT_3DEX pts[5];
					pts[0].x = 0;
					pts[0].y = 0;
					pts[0].pencode = pencode;
					pts[1].x = 0;
					pts[1].y = 20;
					pts[1].pencode = pencode;
					pts[2].x = 20;
					pts[2].y = 20;
					pts[2].pencode = pencode;
					pts[3].x = 20;
					pts[3].y = 0;
					pts[3].pencode = pencode;
					pts[4].x = 0;
					pts[4].y = 0;
					pts[4].pencode = pencode;
					pGeo->CreateShape(pts,5);
				}
			}
			
		
			
			GrBuffer buf;
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			buf2d.AddBuffer(&buf);
			
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			break;
		}
	case SYMTYPE_COLORHATCH:
		{
			CColorHatch *pColorHatch = (CColorHatch*)pSym;

			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();

			int nGeoClass = pGeo->GetClassType();

			if(nGeoClass==CLS_GEOCURVE || nGeoClass==CLS_GEOSURFACE || nGeoClass==CLS_GEOTEXT)
			{
				PT_3DEX pts[4];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 20;
				pts[1].y = 0;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 0;
				pts[3].y = 20;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
				
			}
			else if (nGeoClass==CLS_GEOPARALLEL)
			{
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
				pts[2].x = 0;
				pts[2].y = 0;
				pts[2].pencode = penMove;
				pts[3].x = 20;
				pts[3].y = 0;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
			}
						
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);

			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			
			break;
		}
	case SYMTYPE_COLORHATCH_COND:
		{
			CConditionColorHatch *pColorHatch = (CConditionColorHatch*)pSym;

			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();

			int nGeoClass = pGeo->GetClassType();

			if(nGeoClass==CLS_GEOCURVE || nGeoClass==CLS_GEOSURFACE || nGeoClass==CLS_GEOTEXT)
			{
				PT_3DEX pts[4];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 20;
				pts[1].y = 0;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 0;
				pts[3].y = 20;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
				
			}
			else if (nGeoClass==CLS_GEOPARALLEL)
			{
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
				pts[2].x = 0;
				pts[2].y = 0;
				pts[2].pencode = penMove;
				pts[3].x = 20;
				pts[3].y = 0;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
			}
						
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);

			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			
			break;
		}
		break;
	
	case SYMTYPE_ANNOTATION:
		{			
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}

			CGeometry *pGeo = pFeature->GetGeometry();

			int nGeoClass = pGeo->GetClassType();

			if(nGeoClass == CLS_GEOPOINT)
			{
				pGeo->CreateShape(&PT_3DEX(),1);
			}
			else if(nGeoClass == CLS_GEOCURVE)
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
			else if(nGeoClass == CLS_GEOPARALLEL)
			{
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
			else if(nGeoClass==CLS_GEOSURFACE )
			{
				PT_3DEX pts[5];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 20;
				pts[1].y = 0;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 0;
				pts[3].y = 20;
				pts[3].pencode = penLine;
				pts[4].x = 0;
				pts[4].y = 0;
				pts[4].pencode = penLine;
				pGeo->CreateShape(pts,5);
				
			}
			else if(nGeoClass==CLS_GEOTEXT)
			{
				PT_3DEX pts[4];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 20;
				pts[1].y = 0;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 0;
				pts[3].y = 20;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
				
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
			
			GrBuffer buf;
				
			{
				CValueTable tab;
				GetCurAttribute(tab);
				((CAnnotation*)pSym)->Draw(pFeature,&buf,tab);
			}

			buf2d.AddBuffer(&buf);
				
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			break;
		}

	case SYMTYPE_LINEHATCH:
	case SYMTYPE_DIAGONAL:
	case SYMTYPE_ANGBISECTORTYPE:
		{
			//CScaleCelltype *pScaleCell = (CScaleCelltype*)pSym;
			
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();
			
			if(nGeoClass==CLS_GEOCURVE || nGeoClass==CLS_GEOTEXT)
			{
				PT_3DEX pts[4];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 20;
				pts[1].y = 0;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 0;
				pts[3].y = 20;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
				
			}
			else if( nGeoClass==CLS_GEOSURFACE )
			{
				PT_3DEX pts[5];
				pts[0].x = 0;
				pts[0].y = 0;
				pts[0].pencode = penLine;
				pts[1].x = 0;
				pts[1].y = 20;
				pts[1].pencode = penLine;
				pts[2].x = 20;
				pts[2].y = 20;
				pts[2].pencode = penLine;
				pts[3].x = 20;
				pts[3].y = 0;
				pts[3].pencode = penLine;
				pts[4].x = 0;
				pts[4].y = 0;
				pts[4].pencode = penLine;
				pGeo->CreateShape(pts,5);
				
			}			
			else if (nGeoClass == CLS_GEOPARALLEL)
			{
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
				pts[2].x = 0;
				pts[2].y = 0;
				pts[2].pencode = penMove;
				pts[3].x = 20;
				pts[3].y = 0;
				pts[3].pencode = penLine;
				pGeo->CreateShape(pts,4);
			}
			
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);
			
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			
			break;
		}
	case SYMTYPE_SCALETURNPLATETYPE:
		{
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();
			
			if(nGeoClass==CLS_GEOCURVE || nGeoClass == CLS_GEOSURFACE)
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
			
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);
			
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			
			break;

		}
	case SYMTYPE_SCALE_LiangChang:
	case SYMTYPE_SCALE_YouGuan:
	case SYMTYPE_SCALE_JianFangWu:
	case SYMTYPE_SCALE_TongCheShuiZha:
	case SYMTYPE_SCALE_BuTongCheShuiZha:
	case SYMTYPE_SCALE_DiShangYaoDong:
	case SYMTYPE_SCALE_ChuRuKou:
	case SYMTYPE_SCALE_WenShi:
		{
			CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();
			
			if( (nGeoClass==CLS_GEOCURVE || nGeoClass == CLS_GEOSURFACE) )
			{
				if( (nType==SYMTYPE_SCALE_LiangChang || nType==SYMTYPE_SCALE_YouGuan) )
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
				else if( nType==SYMTYPE_SCALE_JianFangWu || nType==SYMTYPE_SCALE_ChuRuKou||nType==SYMTYPE_SCALE_WenShi )
				{
					PT_3DEX pts[5];
					pts[0].x = 0;
					pts[0].y = 0;
					pts[0].pencode = penLine;
					pts[1].x = 0;
					pts[1].y = 20;
					pts[1].pencode = penLine;
					pts[2].x = 20;
					pts[2].y = 20;
					pts[2].pencode = penLine;
					pts[3].x = 20;
					pts[3].y = 0;
					pts[3].pencode = penLine;
					pts[4].x = 0;
					pts[4].y = 0;
					pts[4].pencode = penLine;
					pGeo->CreateShape(pts,5);
				}
				else if (nType == SYMTYPE_SCALE_DiShangYaoDong)
				{
					PT_3DEX pts[4];
					pts[0].x = 10;
					pts[0].y = 0;
					pts[0].pencode = penLine;
					pts[1].x = 20;
					pts[1].y = 0;
					pts[1].pencode = penLine;
					pts[2].x = 15;
					pts[2].y = 10;
					pts[2].pencode = penLine;					
					pGeo->CreateShape(pts,3);
				}
			}
			else if( (nGeoClass==CLS_GEOPARALLEL || nGeoClass == CLS_GEODCURVE) )
			{
				if( nGeoClass==CLS_GEOPARALLEL )
				{
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
				else
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
			}
			
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);
			
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			
			break;
		}
	case SYMTYPE_CULVERTSURFACE1:
		{
            CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();
			
            PT_3DEX pts[5];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 0;
			pts[1].y = 20;
			pts[1].pencode = penLine;
			pts[2].x = 20;
			pts[2].y = 20;
			pts[2].pencode = penLine;
			pts[3].x = 20;
			pts[3].y = 0;
			pts[3].pencode = penLine;
			pts[4].x = 0;
			pts[4].y = 0;
			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);
			
			
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);
			
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			break;
		}
	case SYMTYPE_CULVERTSURFACE2:
		{
            CFeature *pFeature = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->CreateDefaultFeature();
			if (!pFeature)
			{
				AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
				return FALSE;
			}
			
			CGeometry *pGeo = pFeature->GetGeometry();
			
			int nGeoClass = pGeo->GetClassType();
			
            PT_3DEX pts[5];
			pts[0].x = 0;
			pts[0].y = 0;
			pts[0].pencode = penLine;
			pts[1].x = 0;
			pts[1].y = 20;
			pts[1].pencode = penLine;
			pts[2].x = 20;
			pts[2].y = 20;
			pts[2].pencode = penLine;
			pts[3].x = 20;
			pts[3].y = 0;
			pts[3].pencode = penLine;
			pts[4].x = 0;
			pts[4].y = 0;
			pts[4].pencode = penLine;
			pGeo->CreateShape(pts,5);
			
			
			GrBuffer buf;
			
			pSym->Draw(pFeature,&buf/*,(float)atoi(nScale)/1000*/);
			
			buf2d.AddBuffer(&buf);
			
			if (pFeature)
			{
				delete pFeature;
				pFeature = NULL;
			}
			
			break;
		}
	case SYMTYPE_SCALE_Cell:
		{
			CScaleCell *pScaleCell = (CScaleCell*)pSym;
			strName = pScaleCell->m_strCellDefName;
			
			CellDef def = m_config.pCellDefLib->GetCellDef(strName);
			DefToBuf(&def, &buf2d);
			break;
		}
		break;
	default:
		break;
	}

	buf2d.SetAllColor(0);
	DrawImageItem(idx,m_nImageWid,m_nImageHei,&buf2d);
	
	return TRUE;
}

void CDlgScheme::FillSymbolList()
{
	
	if(!m_bAddSymbol)
	{
		m_wndListSymbol.DeleteAllItems();
		m_listImages.DeleteImageList();
		m_arrIdxCreateFlag.RemoveAll();
	}
	
	if(m_nLayerIndex < 0)
		return;
	//{hcw,2012.7.26,若方案库未加载直接退出。
	if (m_bAbsentSchemePath)
	{
		return ;
	}
	if(((!m_bSpecialLayer)&&m_config.pScheme->GetLayerDefineCount(FALSE)<=m_nLayerIndex)||(m_bSpecialLayer&&m_config.pScheme->GetLayerDefineCount(TRUE)<=m_nLayerIndex))
	{
		m_nLayerIndex=-1;
		return;
	}
	//}
	int nSymbol = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSymbolCount();

	if(nSymbol < 1)
		return;
	
	CreateImageList(32,32,nSymbol);
	if(m_bAddSymbol)
	{
		int nType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSymbol(nSymbol-1)->GetType();
		CString strType;
		switch(nType)
		{
		case SYMTYPE_CELL:
			strType = StrFromResID(IDS_CELL);
			break;
		case SYMTYPE_DASHLINETYPE:
			strType = StrFromResID(IDS_DASH_LINETYPE);
			break;
		case SYMTYPE_CELLLINETYPE:
			strType = StrFromResID(IDS_CELL_LINETYPE);
			break;
		case SYMTYPE_COLORHATCH:
			strType = StrFromResID(IDS_COLOR_HATCH);
			break;
		case SYMTYPE_CELLHATCH:
			strType = StrFromResID(IDS_CELL_HATCH);
			break;
		case SYMTYPE_ANNOTATION:
			strType = StrFromResID(IDS_ANNOTATION);
			break;
		case SYMTYPE_SCALELINETYPE:
			strType = StrFromResID(IDS_SCALE_LINETYPE);
			break;
		case SYMTYPE_SCALE_CellLinetype:
			strType = StrFromResID(IDS_SCALE_CELLLINETYPE);
			break;
		case SYMTYPE_SCALE_OldDouya:
			strType = StrFromResID(IDS_SCALE_OLDDOUYA);
			break;
		case SYMTYPE_SCALE_Xiepo:
			strType = StrFromResID(IDS_SCALE_XIEPO);
			break;
		case SYMTYPE_LINEHATCH:
			strType = StrFromResID(IDS_LINEFILL);
			break;
		case SYMTYPE_DIAGONAL:
			strType = StrFromResID(IDS_DIAGONAL);
			break;
		case SYMTYPE_PARATYPE:
			strType = StrFromResID(IDS_PARA_SYMBOL);
			break;
		case SYMTYPE_ANGBISECTORTYPE:
			strType = StrFromResID(IDS_ANGBISECTOR_SYMBOL);
			break;
		case SYMTYPE_SCALEARCTYPE:
			strType = StrFromResID(IDS_SCALEARC_SYMBOL);
			break;
		case SYMTYPE_SCALETURNPLATETYPE:
			strType = StrFromResID(IDS_SCALETURNPLATE_SYMBOL);
			break;
		case SYMTYPE_SCALECRANETYPE:
			strType = StrFromResID(IDS_SCALECRANE_SYMBOL);
			break;
		case SYMTYPE_SCALEFUNNELTYPE:
			strType = StrFromResID(IDS_SCALEFUNNEL_SYMBOL);
			break;
		case SYMTYPE_SCALE_LiangChang:
			strType = StrFromResID(IDS_LIANGCANG_OLD);
			break;
		case SYMTYPE_SCALE_YouGuan:
			strType = StrFromResID(IDS_YOUGUAN_OLD);
			break;
		case SYMTYPE_SCALE_JianFangWu:
			strType = StrFromResID(IDS_JIANFANGWU_OLD);
			break;
		case SYMTYPE_SCALE_TongCheShuiZha:
			strType = StrFromResID(IDS_TONGCHE_SHUIZHA_OLD);
			break;
		case SYMTYPE_SCALE_BuTongCheShuiZha:
			strType = StrFromResID(IDS_BUTONGCHE_SHUIZHA_OLD);
			break;
		case SYMTYPE_SCALE_DiShangYaoDong:
			strType = StrFromResID(IDS_DISHANG_YAODONG_OLD);
			break;
		case SYMTYPE_SCALE_ChuanSongDai:
			strType = StrFromResID(IDS_CHUANSONGDAI_OLD);
			break;
		case SYMTYPE_SCALE_ChuRuKou:
			strType = StrFromResID(IDS_SCALE_CHURUKOU_OLD);
			break;
		case SYMTYPE_SCALE_Cell:
			strType = StrFromResID(IDS_SCALE_CELL);
			break;
		case SYMTYPE_SCALE_WenShi:
			strType = StrFromResID(IDS_WENSHI_NEW);
			break;
		case SYMTYPE_TIDALWATER:
			strType = StrFromResID(IDS_TIDALWATER);
			break;
		case SYMTYPE_COLORHATCH_COND:
			strType = StrFromResID(IDS_CONDITION_COLORHATCH);
			break;
		case SYMTYPE_CULVERTSURFACE1:
			strType = StrFromResID(IDS_CULVERTSURFACE)+"-1";
			break;
		case SYMTYPE_CULVERTSURFACE2:
			strType = StrFromResID(IDS_CULVERTSURFACE)+"-2";
			break;
		default:
			break;
		}
		m_wndListSymbol.InsertItem(nSymbol-1,strType,nSymbol-1);
		CSymbol *pSym = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSymbol(nSymbol-1);
		m_wndListSymbol.SetItemData(nSymbol - 1, DWORD_PTR(pSym));

		m_wndListSymbol.SetItemState(nSymbol-1, LVIS_SELECTED, LVIS_SELECTED);
	}
	else
	{
		for(int i=0; i<nSymbol; i++)
		{
			int nType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSymbol(i)->GetType();
			CString strType;
			switch(nType)
			{
			case SYMTYPE_CELL:
				strType = StrFromResID(IDS_CELL);
				break;
			case SYMTYPE_DASHLINETYPE:
				strType = StrFromResID(IDS_DASH_LINETYPE);
				break;
			case SYMTYPE_CELLLINETYPE:
				strType = StrFromResID(IDS_CELL_LINETYPE);
				break;
			case SYMTYPE_COLORHATCH:
				strType = StrFromResID(IDS_COLOR_HATCH);
				break;
			case SYMTYPE_CELLHATCH:
				strType = StrFromResID(IDS_CELL_HATCH);
				break;
			case SYMTYPE_ANNOTATION:
				strType = StrFromResID(IDS_ANNOTATION);
				break;
			case SYMTYPE_SCALELINETYPE:
				strType = StrFromResID(IDS_SCALE_LINETYPE);
				break;
			case SYMTYPE_SCALE_CellLinetype:
				strType = StrFromResID(IDS_SCALE_CELLLINETYPE);
				break;
			case SYMTYPE_SCALE_OldDouya:
				strType = StrFromResID(IDS_SCALE_OLDDOUYA);
				break;
			case SYMTYPE_SCALE_Xiepo:
				strType = StrFromResID(IDS_SCALE_XIEPO);
				break;
			case SYMTYPE_LINEHATCH:
				strType = StrFromResID(IDS_LINEFILL);
				break;
			case SYMTYPE_DIAGONAL:
				strType = StrFromResID(IDS_DIAGONAL);
				break;
			case SYMTYPE_PARATYPE:
				strType = StrFromResID(IDS_PARA_SYMBOL);
				break;
			case SYMTYPE_ANGBISECTORTYPE:
				strType = StrFromResID(IDS_ANGBISECTOR_SYMBOL);
				break;
			case SYMTYPE_SCALEARCTYPE:
				strType = StrFromResID(IDS_SCALEARC_SYMBOL);
				break;
			case SYMTYPE_SCALETURNPLATETYPE:
				strType = StrFromResID(IDS_SCALETURNPLATE_SYMBOL);
				break;
			case SYMTYPE_SCALECRANETYPE:
				strType = StrFromResID(IDS_SCALECRANE_SYMBOL);
				break;
			case SYMTYPE_SCALEFUNNELTYPE:
				strType = StrFromResID(IDS_SCALEFUNNEL_SYMBOL);
				break;
			case SYMTYPE_SCALE_LiangChang:
				strType = StrFromResID(IDS_LIANGCANG_OLD);
				break;
			case SYMTYPE_SCALE_YouGuan:
				strType = StrFromResID(IDS_YOUGUAN_OLD);
				break;
			case SYMTYPE_SCALE_JianFangWu:
				strType = StrFromResID(IDS_JIANFANGWU_OLD);
				break;
			case SYMTYPE_SCALE_TongCheShuiZha:
				strType = StrFromResID(IDS_TONGCHE_SHUIZHA_OLD);
				break;
			case SYMTYPE_SCALE_BuTongCheShuiZha:
				strType = StrFromResID(IDS_BUTONGCHE_SHUIZHA_OLD);
				break;
			case SYMTYPE_SCALE_DiShangYaoDong:
				strType = StrFromResID(IDS_DISHANG_YAODONG_OLD);
				break;
			case SYMTYPE_SCALE_ChuanSongDai:
				strType = StrFromResID(IDS_CHUANSONGDAI_OLD);
				break;
			case SYMTYPE_SCALE_ChuRuKou:
				strType = StrFromResID(IDS_SCALE_CHURUKOU_OLD);
				break;
			case SYMTYPE_SCALE_Cell:
				strType = StrFromResID(IDS_SCALE_CELL);
				break;
			case SYMTYPE_SCALE_WenShi:
				strType = StrFromResID(IDS_WENSHI_NEW);
				break;
			case SYMTYPE_TIDALWATER:
				strType = StrFromResID(IDS_TIDALWATER);
				break;
			case SYMTYPE_COLORHATCH_COND:
				strType = StrFromResID(IDS_CONDITION_COLORHATCH);
				break;
			case SYMTYPE_CULVERTSURFACE1:
				strType = StrFromResID(IDS_CULVERTSURFACE)+"-1";
				break;
			case SYMTYPE_CULVERTSURFACE2:
				strType = StrFromResID(IDS_CULVERTSURFACE)+"-2";
			    break;
			default:
				break;
			}
			m_wndListSymbol.InsertItem(i,strType,i);
			CSymbol *pSym = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSymbol(i);
			m_wndListSymbol.SetItemData(i, DWORD_PTR(pSym));
		}
		
		m_wndListSymbol.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

	}
	
}

void CDlgScheme::DefToBuf(CellDef *pDef, GrBuffer2d *buf)
{	
	if( pDef->m_pgr )
	{
		buf->AddBuffer(pDef->m_pgr);
		buf->GetEnvelope();
		buf->SetAllColor(0);
	}
}

void CDlgScheme::OnCellButton() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0 || m_config.pCellDefLib==NULL)
		return;
	
	if (m_config.pCellDefLib->GetCellDefCount() < 1)
	{
		AfxMessageBox(StrFromResID(IDS_NO_CELL),MB_OK|MB_ICONASTERISK);
		return;
	}

	CCell *pCel = new CCell();
	if (!pCel) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pCel->IsSupportGeoType(nClassType) )
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
		delete pCel;
		return;
	}

	pCel->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pCel);
	
/*
	int nSymbol = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->GetSymbolCount();
	for(int i=0; i<nSymbol; i++)
	{
		CSymbol *pSym = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->GetSymbol(i);
		if(pSym->GetType() == SYMTYPE_CELL)
		{
			CCell *pCel = new CCell();
			CCell *pTem = (CCell*)pSym;
//			memcpy(pCel,pTem,sizeof(CCell));
			pCel->CopyFrom(pTem);
			m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pCel);
			break;
				
		}
	}
	if(i > nSymbol-1)
	{
		MessageBox("该层无符号,添加失败!");
		return;
	}
*/
	m_bAddSymbol = TRUE;

	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();

	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
	
}



void CDlgScheme::OnDashlineButton() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)
		return;

	while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_NO_LINE);
		szCaption.LoadString(IDS_ATTENTION);
		if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
		{
			CDlgCellDefLinetypeView dlg;
			CSwitchScale scale(m_config.GetScale());
			dlg.SetShowMode(FALSE,FALSE,1);
			dlg.SetConfig(m_config);
			
			dlg.DoModal();
		}
		else
			return;
	}

	CDashLinetype *pDash = new CDashLinetype();
	if (!pDash) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pDash->IsSupportGeoType(nClassType) )
	{
		MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
		delete pDash;
		return;
	}
	
	pDash->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pDash);

	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
	
}

void CDlgScheme::OnCelllineButton() 
{
	// TODO: Add your control notification handler code here
	if ((m_nLayerIndex < 0) || (!m_config.pCellDefLib))
		return;
	
	if (m_config.pCellDefLib->GetCellDefCount() < 1)
	{
		MessageBox(StrFromResID(IDS_NO_CELL),NULL,MB_OK|MB_ICONASTERISK);
		return;
	}

	CCellLinetype *pCellLine = new CCellLinetype();
	if (!pCellLine) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pCellLine->IsSupportGeoType(nClassType) )
	{
		MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
		delete pCellLine;
		return;
	}
	
	pCellLine->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pCellLine);
	
	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
	
}

void CDlgScheme::OnCellhatchButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0 || m_config.pCellDefLib==NULL)
		return;
	
	if(m_config.pCellDefLib->GetCellDefCount() < 1)
	{
		MessageBox(StrFromResID(IDS_NO_CELL));
		return;
	}

	CCellHatch *pCellHatch = new CCellHatch();
	if (!pCellHatch) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pCellHatch->IsSupportGeoType(nClassType) )
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
		delete pCellHatch;
		return;
	}
	
	pCellHatch->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pCellHatch);

	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
	
}

extern int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, LONG_PTR lparam);
void CDlgScheme::OnAnnotationButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0)
		return;

	CAnnotation *pAnno = new CAnnotation();
	strcpy(pAnno->m_strText,_T("abc"));
	pAnno->m_nAnnoType = CAnnotation::Text;
	pAnno->m_nPlaceType = CAnnotation::Head;

	// 判断系统是否有默认字体
	CStringArray arrFont;
	
	LOGFONT logfont;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName,"");
	HDC hDC = ::CreateCompatibleDC(NULL);
	
	::EnumFontFamiliesEx(hDC,&logfont,
		(FONTENUMPROC)EnumFontFamProc,(LPARAM)&arrFont,0);
	::DeleteDC(hDC);
	
	int fontnum = arrFont.GetSize();
	if (fontnum > 0)
	{
		for (int i=0; i<fontnum; i++)
		{
			if (arrFont[i].CompareNoCase(pAnno->m_textSettings.tcFaceName) == 0)
			{
				break;
			}
		}
		
		if (i >= fontnum)
		{
			strcpy(pAnno->m_textSettings.tcFaceName,arrFont[0]);
		}
	}

	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pAnno);
	
	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;
	
	m_bModified = TRUE;
}

void CDlgScheme::OnColorhatchButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0)
		return;

	CColorHatch *pColorHatch = new CColorHatch();
	if (!pColorHatch) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pColorHatch->IsSupportGeoType(nClassType) )
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
		delete pColorHatch;
		return;
	}

	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pColorHatch);
	
	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
	
}

void CDlgScheme::OnScalelineButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0)
		return;

	while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_NO_LINE);
		szCaption.LoadString(IDS_ATTENTION);
		if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
		{
			CDlgCellDefLinetypeView dlg;
			CSwitchScale scale(m_config.GetScale());
			dlg.SetShowMode(FALSE,FALSE,1);
			dlg.SetConfig(m_config);
			
			dlg.DoModal();
		}
		else
			return;
	}
	
	CScaleLinetype *pScaleLine = new CScaleLinetype();
	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pScaleLine->IsSupportGeoType(nClassType) )
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
		delete pScaleLine;
		return;
	}

	
	
	pScaleLine->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pScaleLine);

	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
	
}

void CDlgScheme::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMERID_CREATEIMAGE)
	{
		BOOL bKill = FALSE;
		int nsize = m_arrIdxCreateFlag.GetSize();
		
		//检查是否已经没有需要生成的图像了
		for( int i=m_nIdxToCreateImage; i<nsize+m_nIdxToCreateImage; i++)
		{
			if( m_arrIdxCreateFlag[(i%nsize)]==0 )
				break;
		}
		
		if (i >= nsize+m_nIdxToCreateImage)
			bKill = TRUE;
		else
		{
			i = (i%nsize);
			if ( CreateImageItem(i) )
			{
				m_arrIdxCreateFlag[i] = 1;
				m_nIdxToCreateImage = i+1;
				
//				static int nCreateTimes = 0;
//				if( (nCreateTimes+1)%10==0 )
					m_wndListSymbol.RedrawWindow();
//				nCreateTimes++;
			}
		}	

		if (bKill)
			KillTimer(TIMERID_CREATEIMAGE);
		
		
	}
	else if (nIDEvent == TIMERID_PREVIEW)
	{
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgScheme::OnSelchangeSymbolList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	// 避免更新两次的情况
	if (!(pNMListView->uChanged == LVIF_STATE) || !(pNMListView->uNewState&LVIS_SELECTED))
	{
		*pResult = 0;
		return;
	}

	ReleaseDlgMemory();

	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )return;
	
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	CSymbol *pSym = (CSymbol*)m_wndListSymbol.GetItemData(nsel);
//	CSymbol *pSym = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->GetSymbol(nSymbolIndex);
	CString str = pSym->GetName();
	int nType = pSym->GetType();
	switch(nType)
	{
	case SYMTYPE_CELL:
		{
			if(m_pCellDlg == NULL)
			{
				m_pCellDlg = new CDlgCellSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pCellDlg != NULL)
				{
					m_pCellDlg->SetMem((CCell*)pSym,m_config);
					BOOL ret = m_pCellDlg->Create(IDD_CELL_SETTINGDLG,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);

				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pCellDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}

			m_pCellDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_DASHLINETYPE:
		{
			if(m_pDashLineDlg == NULL)
			{
				m_pDashLineDlg = new CDlgDashLineSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pDashLineDlg != NULL)
				{
					m_pDashLineDlg->SetMem((CDashLinetype*)pSym, m_config);
					BOOL ret = m_pDashLineDlg->Create(IDD_DASHLINE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pDashLineDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pDashLineDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_CELLLINETYPE:
		{
			if(m_pCellLineDlg == NULL)
			{
				m_pCellLineDlg = new CDlgCellLineSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pCellLineDlg != NULL)
				{
					m_pCellLineDlg->SetMem((CCellLinetype*)pSym,m_config);
					BOOL ret = m_pCellLineDlg->Create(IDD_CELLLINE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pCellLineDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pCellLineDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_COLORHATCH:
		{
			if(m_pColorHatchDlg == NULL)
			{
				m_pColorHatchDlg = new CDlgColorHatchSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pColorHatchDlg != NULL)
				{
					m_pColorHatchDlg->SetMem((CColorHatch*)pSym);
					BOOL ret = m_pColorHatchDlg->Create(IDD_COLORHATCH_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pColorHatchDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pColorHatchDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_COLORHATCH_COND:
		{
			if(m_pCondColorHatchDlg == NULL)
			{
				m_pCondColorHatchDlg = new CDlgConditionColorHatch();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pCondColorHatchDlg != NULL)
				{
					CValueTable tab;
					GetCurAttribute(tab);

					m_pCondColorHatchDlg->SetMem((CConditionColorHatch*)pSym,tab);
					BOOL ret = m_pCondColorHatchDlg->Create(IDD_COLORHATCH_CONDITION_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pCondColorHatchDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pCondColorHatchDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_CELLHATCH:
		{
			if(m_pCellHatchDlg == NULL)
			{
				m_pCellHatchDlg = new CDlgCellHatchSetting();

				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pCellHatchDlg != NULL)
				{
					m_pCellHatchDlg->SetMem((CCellHatch*)pSym,m_config);
					BOOL ret = m_pCellHatchDlg->Create(IDD_CELLHATCH_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pCellHatchDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
		
			m_pCellHatchDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_ANNOTATION:
		{
			if(m_pAnnoDlg == NULL)
			{
				m_pAnnoDlg = new CDlgAnnotationSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pAnnoDlg != NULL)
				{
					CValueTable tab;
					GetCurAttribute(tab);
					m_pAnnoDlg->SetMem((CAnnotation*)pSym, tab);

					BOOL ret = m_pAnnoDlg->Create(IDD_ANNOTATION_DIALOG,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pAnnoDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pAnnoDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALELINETYPE:
		{
			if(m_pScaleDlg== NULL)
			{
				m_pScaleDlg = new CDlgScaleLineSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleDlg != NULL)
				{
					m_pScaleDlg->SetMem((CScaleLinetype*)pSym, m_config);
					BOOL ret = m_pScaleDlg->Create(IDD_SCALELINE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pScaleDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_SCALE_CellLinetype:
		{
			if(m_pScaleCellLTDlg== NULL)
			{
				m_pScaleCellLTDlg = new CDlgScaleCellLineSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleCellLTDlg != NULL)
				{
					m_pScaleCellLTDlg->SetMem((CScaleCellLinetype*)pSym, m_config);
					BOOL ret = m_pScaleCellLTDlg->Create(IDD_SCALE_CELLLINE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleCellLTDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pScaleCellLTDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_TIDALWATER:
		{
			if(m_pTidalWaterDlg== NULL)
			{
				m_pTidalWaterDlg = new CDlgTidalWaterSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pTidalWaterDlg != NULL)
				{
					m_pTidalWaterDlg->SetMem((CTidalWaterSymbol*)pSym, m_config);
					BOOL ret = m_pTidalWaterDlg->Create(IDD_TIDALWATER_SETTINGDLG,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pTidalWaterDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pTidalWaterDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALE_OldDouya:
		{
			if(m_pScaleOldDouyaDlg== NULL)
			{
				m_pScaleOldDouyaDlg = new CDlgScaleOldDouyaSettings();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleOldDouyaDlg != NULL)
				{
					m_pScaleOldDouyaDlg->SetMem((CScaleOldDouya*)pSym, m_config);
					BOOL ret = m_pScaleOldDouyaDlg->Create(IDD_SCALE_OLDDOUYA,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleOldDouyaDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pScaleOldDouyaDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALE_Xiepo:
		{
			if(m_pScaleXiepoDlg== NULL)
			{
				m_pScaleXiepoDlg = new CDlgScaleXiepo();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleXiepoDlg != NULL)
				{
					m_pScaleXiepoDlg->SetMem((CScaleXiepo*)pSym, m_config);
					BOOL ret = m_pScaleXiepoDlg->Create(IDD_SCALE_XIEPO,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleXiepoDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pScaleXiepoDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_LINEHATCH:
		{
			if(m_pLinefillDlg == NULL)
			{
				m_pLinefillDlg = new CDlgLinefillSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pLinefillDlg != NULL)
				{
					m_pLinefillDlg->SetMem((CLineHatch*)pSym, m_config);
					BOOL ret = m_pLinefillDlg->Create(IDD_LINEFILL_SETTINGDLG,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pLinefillDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pLinefillDlg->ShowWindow(SW_SHOW);
			break;
		}
	case SYMTYPE_DIAGONAL:
		{
			if (m_pDiagonalDlg == NULL)
			{
				m_pDiagonalDlg = new CDlgDiagonalSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pDiagonalDlg != NULL)
				{
					m_pDiagonalDlg->SetMem((CDiagonal*)pSym, m_config);
					BOOL ret = m_pDiagonalDlg->Create(IDD_DIAGONAL_SETTINGDLG,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pDiagonalDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pDiagonalDlg->ShowWindow(SW_SHOW);

			break;
		}
	case SYMTYPE_PARATYPE:
		{
			if(m_pParalineDlg == NULL)
			{
				m_pParalineDlg = new CDlgParalineSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pParalineDlg != NULL)
				{
					m_pParalineDlg->SetMem((CParaLinetype*)pSym, m_config);
					BOOL ret = m_pParalineDlg->Create(IDD_PARALINE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pParalineDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pParalineDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_ANGBISECTORTYPE:
		{
			if(m_pParalineDlg == NULL)
			{
				m_pAngBisectorDlg = new CDlgAngbisectorSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pAngBisectorDlg != NULL)
				{
					m_pAngBisectorDlg->SetMem((CAngBisectortype*)pSym, m_config);
					BOOL ret = m_pAngBisectorDlg->Create(IDD_ANGBISECTOR_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pAngBisectorDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pAngBisectorDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_SCALEARCTYPE:
		{
			if(m_pScaleArcDlg == NULL)
			{
				m_pScaleArcDlg = new CDlgScaleArcSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleArcDlg != NULL)
				{
					m_pScaleArcDlg->SetMem((CScaleArctype*)pSym, m_config);
					BOOL ret = m_pScaleArcDlg->Create(IDD_SCALEARC_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleArcDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pScaleArcDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALETURNPLATETYPE:
		{
			if(m_pScaleTurnplateSetting == NULL)
			{
				m_pScaleTurnplateSetting = new CDlgScaleTurnplateSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleTurnplateSetting != NULL)
				{
					m_pScaleTurnplateSetting->SetMem((CScaleTurnplatetype*)pSym, m_config);
					BOOL ret = m_pScaleTurnplateSetting->Create(IDD_SCALETURNPLATE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleTurnplateSetting->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pScaleTurnplateSetting->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALECRANETYPE:
		{
			if(m_pScaleCraneSetting == NULL)
			{
				m_pScaleCraneSetting = new CDlgScaleCraneSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleCraneSetting != NULL)
				{
					m_pScaleCraneSetting->SetMem((CScaleCranetype*)pSym, m_config);
					BOOL ret = m_pScaleCraneSetting->Create(IDD_SCALECRANE_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleCraneSetting->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pScaleCraneSetting->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALEFUNNELTYPE:
		{
			if(m_pScaleFunnelSetting == NULL)
			{
				m_pScaleFunnelSetting = new CDlgScaleFunnelSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleFunnelSetting != NULL)
				{
					m_pScaleFunnelSetting->SetMem((CScaleFunneltype*)pSym, m_config);
					BOOL ret = m_pScaleFunnelSetting->Create(IDD_SCALEFUNNEL_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleFunnelSetting->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pScaleFunnelSetting->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALE_ChuanSongDai:
		{
			if(m_pScaleFunnelSetting == NULL)
			{
				m_pScaleChuanSongDaiSetting = new CDlgScaleChuanSongDaiSetting;
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleChuanSongDaiSetting != NULL)
				{
					m_pScaleChuanSongDaiSetting->SetMem((CScaleChuanSongDai*)pSym, m_config);
					BOOL ret = m_pScaleChuanSongDaiSetting->Create(IDD_SCALECHUANSONGDAI_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleChuanSongDaiSetting->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pScaleChuanSongDaiSetting->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALE_LiangChang:
	case SYMTYPE_SCALE_JianFangWu:
	case SYMTYPE_SCALE_TongCheShuiZha:
	case SYMTYPE_SCALE_BuTongCheShuiZha:
	case SYMTYPE_SCALE_DiShangYaoDong:
	case SYMTYPE_SCALE_ChuRuKou:
		{
			if(m_pProcSymLTSetting == NULL)
			{
				m_pProcSymLTSetting = new CDlgProcSymLTSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pProcSymLTSetting != NULL)
				{
					m_pProcSymLTSetting->SetMem((CProcSymbol_LT*)pSym, m_config);
					BOOL ret = m_pProcSymLTSetting->Create(IDD_PROCSYM_LT_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pProcSymLTSetting->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pProcSymLTSetting->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALE_YouGuan:
		{
			if(m_pScaleYouGuanDlg == NULL)
			{
				m_pScaleYouGuanDlg = new CDlgScaleYouGuanSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleYouGuanDlg != NULL)
				{
					m_pScaleYouGuanDlg->SetMem((CScaleYouGuan*)pSym, m_config);
					BOOL ret = m_pScaleYouGuanDlg->Create(IDD_YOUGUAN_SETTING,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleYouGuanDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
			}
			
			m_pScaleYouGuanDlg->ShowWindow(SW_SHOW);			
			
			break;
		}
	case SYMTYPE_SCALE_Cell:
		{
			if(m_pScaleCellDlg == NULL)
			{
				m_pScaleCellDlg = new CDlgScaleCellSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pScaleCellDlg != NULL)
				{
					m_pScaleCellDlg->SetMem((CScaleCell*)pSym,m_config);
					BOOL ret = m_pScaleCellDlg->Create(IDD_SCALECELL_SETTINGDLG,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pScaleCellDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
				
				
			}
			
			m_pScaleCellDlg->ShowWindow(SW_SHOW);
			
			
			break;
		}
	case SYMTYPE_CULVERTSURFACE1:
		{
            if(m_pCulvertSurfaceDlg==NULL)
			{
				m_pCulvertSurfaceDlg = new CDlgCulvertSurfaceSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pCulvertSurfaceDlg != NULL)
				{
					m_pCulvertSurfaceDlg->SetMem((CCulvertSurface1Symbol*)pSym,m_config);
					BOOL ret = m_pCulvertSurfaceDlg->Create(IDD_CULVERTSURFACE,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pCulvertSurfaceDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
			}
			
			m_pCulvertSurfaceDlg->ShowWindow(SW_SHOW);
			break;
		}
	case SYMTYPE_CULVERTSURFACE2:
		{
            if(m_pCulvertSurfaceDlg==NULL)
			{
				m_pCulvertSurfaceDlg = new CDlgCulvertSurfaceSetting();
				//Check if new succeeded and we got a valid pointer to a dialog object
				if(m_pCulvertSurfaceDlg != NULL)
				{
					m_pCulvertSurfaceDlg->SetMem((CCulvertSurface2Symbol*)pSym,m_config);
					BOOL ret = m_pCulvertSurfaceDlg->Create(IDD_CULVERTSURFACE,this);
					
					if(!ret)   //Create failed.
						AfxMessageBox(StrFromResID(IDS_CREATEDIA_ERROR),MB_OK|MB_ICONASTERISK);
					
				}
				else
					AfxMessageBox(StrFromResID(IDS_CREATEDIAOBJ_ERROR),MB_OK|MB_ICONASTERISK);
				
				CWnd *pWnd = GetDlgItem(IDC_PARA_STATIC);
				if( !pWnd )
					return;
				CRect paraRect;
				pWnd->GetWindowRect(&paraRect);
				ScreenToClient(&paraRect);
				
				m_pCulvertSurfaceDlg->SetWindowPos(NULL,paraRect.left+2,paraRect.top+15,paraRect.Width()-2,paraRect.Height()-5,SWP_NOACTIVATE|SWP_NOZORDER);
			}
			
			m_pCulvertSurfaceDlg->ShowWindow(SW_SHOW);
			break;
		}
	default:
		break;
	}
	
	*pResult = 0;
}

void CDlgScheme::GetCurAttribute(CValueTable &tab)
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

void CDlgScheme::OnDeletesymbolButton() 
{
	// TODO: Add your control notification handler code here
	if(m_nLayerIndex < 0)
		return;

	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )
	{
		AfxMessageBox(StrFromResID(IDS_SEL_ONLYONE),MB_OK|MB_ICONASTERISK);
		return;
	}
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
//	m_arrIdxCreateFlag.RemoveAt(nsel);
//	m_listImages.Remove(nsel);
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->DelSymbol(nsel);
	m_wndListSymbol.DeleteItem(nsel);

	FillSymbolList();
	
	if(m_wndListSymbol.GetItemCount() < 1)
	{		
		//隐藏参数设置对话框
		ReleaseDlgMemory();

	}
	else if(nsel == 0)
	{
		m_wndListSymbol.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		
	}
	else if(nsel>0 && nsel<m_wndListSymbol.GetItemCount()+1)
	{
		m_wndListSymbol.SetItemState(nsel-1, LVIS_SELECTED, LVIS_SELECTED);
	}
	

	DrawPreview(m_nLayerIndex);

	m_bModified = TRUE;
	
}

void CDlgScheme::OnSupportgeonameEdit() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)
		return;
	CStringArray strGeoName;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSupportClsName(strGeoName);

	CDlgSupportGeoClassName dlg;
	dlg.SetData(strGeoName);

	if (dlg.DoModal() != IDOK) 
		return;
	
	strGeoName.RemoveAll();
	dlg.GetData(strGeoName);

	// 验证是否支持当前设置的缺省几何类型
	int nComboIndex = m_defaultgeoCombo.GetCurSel();
	int nDefGeo = m_defaultgeoCombo.GetItemData(nComboIndex);

	int nComboIndex1 = m_dbgeoCombo.GetCurSel();
	int nDefGeo1 = m_dbgeoCombo.GetItemData(nComboIndex);

	BOOL bSupport = TRUE, bSupport1 = TRUE;
	CSchemeLayerDefine tmp;
	tmp.SetSupportClsName(strGeoName);
	bSupport = tmp.IsSupportGeoClass(nDefGeo);
	bSupport1 = tmp.IsSupportGeoClass(nDefGeo1);
	if ((!bSupport || !bSupport1) && 
		IDNO == AfxMessageBox(StrFromResID(IDS_NO_SUPPORTGEONAME),MB_YESNO|MB_ICONASTERISK))
	{
			return;
	}

	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetSupportClsName(strGeoName);

	m_strSupportGeoName.Empty();
	for (int i=0; i<strGeoName.GetSize(); i++)
	{
		m_strSupportGeoName += strGeoName.GetAt(i);
		if (i == strGeoName.GetSize()-1)
			break;
		
		m_strSupportGeoName += ",";
		
	}

	UpdateData(FALSE);

	if ( !m_bModified )
		m_bModified = TRUE;

	// 设置缺少几何类型控件
	if (!bSupport)
	{
		for (int i=0; i<m_defaultgeoCombo.GetCount(); i++)
		{
			if (m_defaultgeoCombo.GetItemData(i) == dlg.m_nSupportFirstgeo)
			{
				m_defaultgeoCombo.SetCurSel(i);
				break;
			}
		}
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetGeoClass(dlg.m_nSupportFirstgeo);
		// 重置当前层的缺省命令
		OnResetDefaultcommand();

		SendMessage(WM_SYMBOL_REPAINT);

	}
	// 设置入库几何体类型控件
	if (!bSupport1)
	{
		for (int i=0; i<m_dbgeoCombo.GetCount(); i++)
		{
			if (m_dbgeoCombo.GetItemData(i) == dlg.m_nSupportFirstgeo)
			{
				m_dbgeoCombo.SetCurSel(i);
				break;
			}
		}
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SetDbGeoClass(dlg.m_nSupportFirstgeo);
		
		SendMessage(WM_SYMBOL_REPAINT);
		
	}

	
	
}

void CDlgScheme::OnClickStatic() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)  return;

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

void CDlgScheme::DrawTemLine(CPoint pt, int type)
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
	
	CWnd *pWndSize = GetDlgItem(IDC_LAYERDISTANCE_STATIC);
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


void CDlgScheme::OnResetDefaultcommand() 
{
	// TODO: Add your command handler code here
	if (m_config.GetScale() <= 0 || m_nLayerIndex < 0)  	return;

	CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer);
	if (!pLayer)  return;
	int nGeoClass, nCommandID;
	nGeoClass = pLayer->GetGeoClass();
	if (nGeoClass == CLS_GEOPOINT)
	{
		nCommandID = ID_ELEMENT_DOT_DOT;
	}
	else if (nGeoClass == CLS_GEODIRPOINT)
	{
		nCommandID = ID_ELEMENT_DOT_VECTORDOT;
	}
	else if (nGeoClass == CLS_GEOCURVE)
	{
		nCommandID = ID_ELEMENT_LINE_LINE;
	}
	else if (nGeoClass == CLS_GEODCURVE)
	{
		nCommandID = ID_ELEMENT_LINE_DLINE;
	}
	else if (nGeoClass == CLS_GEOPARALLEL)
	{
		nCommandID = ID_ELEMENT_LINE_PARALLEL;
	}
	else if (nGeoClass == CLS_GEOSURFACE)
	{
		nCommandID = ID_ELEMENT_FACE_FACE;
	}
	else if (nGeoClass == CLS_GEOTEXT)
	{
		nCommandID = ID_ELEMENT_TEXT;
	}

	CString strLayerName = pLayer->GetLayerName();
	CPlaceConfig *pConfig = m_config.pPlaceCfgLib->GetConfig(strLayerName);
	
	if (!pConfig)
	{		
		CPlaceConfig config;		
		config.m_nCommandID = nCommandID;
		strcpy(config.m_strLayName, strLayerName);
		m_config.pPlaceCfgLib->AddConfig(config);
	}
	else
		pConfig->m_nCommandID = nCommandID;
	
	m_config.pPlaceCfgLib->Save();
	
}

void CDlgScheme::OnResetAlldefaultcommand() 
{
	// TODO: Add your command handler code here
	if (m_config.GetScale() <= 0)  return;

	for (int i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(i);
		if (!pLayer)  continue;
		int nGeoClass, nCommandID;
		nGeoClass = pLayer->GetGeoClass();
		if (nGeoClass == CLS_GEOPOINT)
		{
			nCommandID = ID_ELEMENT_DOT_DOT;
		}
		else if (nGeoClass == CLS_GEODIRPOINT)
		{
			nCommandID = ID_ELEMENT_DOT_VECTORDOT;
		}
		else if (nGeoClass == CLS_GEOCURVE)
		{
			nCommandID = ID_ELEMENT_LINE_LINE;
		}
		else if (nGeoClass == CLS_GEODCURVE)
		{
			nCommandID = ID_ELEMENT_LINE_DLINE;
		}
		else if (nGeoClass == CLS_GEOPARALLEL)
		{
			nCommandID = ID_ELEMENT_LINE_PARALLEL;
		}
		else if (nGeoClass == CLS_GEOSURFACE)
		{
			nCommandID = ID_ELEMENT_FACE_FACE;
		}
		else if (nGeoClass == CLS_GEOTEXT)
		{
			nCommandID = ID_ELEMENT_TEXT;
		}

		CString strLayerName = pLayer->GetLayerName();
		CPlaceConfig *pConfig = m_config.pPlaceCfgLib->GetConfig(strLayerName);

		CString cmd_params;
		CDlgDoc *pDoc = GetActiveDlgDoc();
		if(pDoc)
		{
			int nCmdNum = 0;
			const CMDREG *cmds = pDoc->GetCmdRegs(nCmdNum);
			for(int j=0; j<nCmdNum; j++)
			{
				if(cmds[j].id==nCommandID)
				{
					CCommand *pCmd = (cmds[j].lpProcCreate)();
					if(pCmd)
					{
						pCmd->Init(pDoc);
						CValueTable tab;
						pCmd->GetParams(tab);

						CMarkup xml;
						xml.AddElem("Data");
						xml.IntoElem();
						Xml_WriteValueTable(xml,tab);
						xml.OutOfElem();

						cmd_params = xml.GetDoc();
						delete pCmd;
						break;
					}
				}
			}
		}
		
		if (!pConfig)
		{		
			CPlaceConfig config;
			config.m_nCommandID = nCommandID;				
			strcpy(config.m_strLayName, strLayerName);

			if(!cmd_params.IsEmpty())
			{
				memset(config.m_strCmdParams,0,sizeof(config.m_strCmdParams));
				strncpy(config.m_strCmdParams,cmd_params,sizeof(config.m_strCmdParams));				
			}

			m_config.pPlaceCfgLib->AddConfig(config);
		}
		else
		{
			pConfig->m_nCommandID = nCommandID;

			if(!cmd_params.IsEmpty())
			{
				memset(pConfig->m_strCmdParams,0,sizeof(pConfig->m_strCmdParams));
				strncpy(pConfig->m_strCmdParams,cmd_params,sizeof(pConfig->m_strCmdParams));				
			}
		}	
	}
	
//	m_config.pScheme->Save();
	m_config.pPlaceCfgLib->Save();

	m_bModified = TRUE;

	AfxMessageBox(IDS_PROCESS_OK);
}



void CDlgScheme::OnResetLayerPlaceLine() 
{
	// TODO: Add your command handler code here
	if (m_config.GetScale() <= 0)  return;
	
	for (int i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(i);
		if (!pLayer)  continue;
		int nGeoClass = pLayer->GetDbGeoClass();
		if (nGeoClass != CLS_GEOSURFACE)
			continue;

		nGeoClass = pLayer->GetGeoClass();
		if ( nGeoClass==CLS_GEOPARALLEL || nGeoClass==CLS_GEODCURVE )
			continue;
		
		pLayer->SetGeoClass(CLS_GEOCURVE);
		
		CString strLayerName = pLayer->GetLayerName();
		CPlaceConfig *pConfig = m_config.pPlaceCfgLib->GetConfig(strLayerName);
		
		if (!pConfig)
		{		
			CPlaceConfig config;
			config.m_nCommandID = ID_ELEMENT_LINE_LINE;				
			strcpy(config.m_strLayName, strLayerName);
			m_config.pPlaceCfgLib->AddConfig(config);
		}
		else
		{
			pConfig->m_nCommandID = ID_ELEMENT_LINE_LINE;
		}		
	}
	
	//	m_config.pScheme->Save();
	m_config.pPlaceCfgLib->Save();

	m_bModified = TRUE;
	AfxMessageBox(IDS_PROCESS_OK);
}


void CDlgScheme::OnResetLayerPlaceArea() 
{
	// TODO: Add your command handler code here
	if (m_config.GetScale() <= 0)  return;
	
	for (int i=0; i<m_config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(i);
		if (!pLayer)  continue;
		int nGeoClass = pLayer->GetDbGeoClass();
		if (nGeoClass != CLS_GEOSURFACE)
			continue;

		nGeoClass = pLayer->GetGeoClass();
		if ( nGeoClass==CLS_GEOPARALLEL || nGeoClass==CLS_GEODCURVE )
			continue;
		
		pLayer->SetGeoClass(CLS_GEOSURFACE);
		
		CString strLayerName = pLayer->GetLayerName();
		CPlaceConfig *pConfig = m_config.pPlaceCfgLib->GetConfig(strLayerName);
		
		if (!pConfig)
		{		
			CPlaceConfig config;
			config.m_nCommandID = ID_ELEMENT_FACE_FACE;				
			strcpy(config.m_strLayName, strLayerName);
			m_config.pPlaceCfgLib->AddConfig(config);
		}
		else
			pConfig->m_nCommandID = ID_ELEMENT_FACE_FACE;
		
	}
	
	//	m_config.pScheme->Save();

	m_bModified = TRUE;
	m_config.pPlaceCfgLib->Save();
	AfxMessageBox(IDS_PROCESS_OK);
}

void CDlgScheme::OnResetAllLayerDefaultValues()
{
	if (m_config.GetScale() <= 0)  return;

	CScheme *pScheme = m_config.pScheme;
	if (!pScheme) return;
	
	int nCount = pScheme->GetLayerDefineCount();
	
	GProgressStart(nCount);
	for( int i=0; i<nCount; i++)
	{
		GProgressStep();
		
		CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(i);		
		pLayer->SetDefaultValues(CValueTable());
		
	}
	
	pScheme->Save();
	m_bModified = TRUE;
	
	GProgressEnd();
	AfxMessageBox(IDS_PROCESS_OK);
	
}




void CDlgScheme::OnResetAllLayerOrder()
{
	if (m_config.GetScale() <= 0)  return;
	
	for (int i=0; i<m_config.pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		CString strGroupName = m_UserIdx.m_aGroup[i].GroupName;
		CUIntArray arrIndex;
		for(int j=0; j<m_config.pScheme->GetLayerDefineCount(); j++)
		{
			CSchemeLayerDefine *pLayer = m_config.pScheme->GetLayerDefine(j);
			if (!pLayer) continue;
			if (strGroupName.CompareNoCase(pLayer->GetGroupName()) == 0)
			{
				BOOL bInserted = FALSE;
				for (int k=0; k<arrIndex.GetSize(); k++)
				{
					if (pLayer->GetLayerCode() < m_config.pScheme->GetLayerDefine(arrIndex[k])->GetLayerCode())
					{
						m_config.pScheme->DelLayerDefine(j,FALSE);
						m_config.pScheme->InsertLayerDefineAt(arrIndex[k],pLayer);
						
						arrIndex.InsertAt(k,arrIndex[k]);
						for (int m=k+1; m<arrIndex.GetSize(); m++)
						{
							arrIndex[m]++;
						}
						
						bInserted = TRUE;
						m_bModified = TRUE;
						break;
					}
				}

				if (!bInserted)
				{
					arrIndex.Add(j);
				}
			}

		}
	}
	
	// 界面更新，保存
	OnSelchangeScaleCombo();
	m_bModified = TRUE;
	AfxMessageBox(IDS_PROCESS_OK);
}

void CDlgScheme::OnCopySymbols()
{
	if (m_nLayerIndex == -1) return;
	//{Selected then Copy hcw 2012-2-13 
	if (m_binSymbolListRegion)
	{
		POSITION pos =  m_wndListSymbol.GetFirstSelectedItemPosition();
		if (!pos)
		{
			AfxMessageBox(StrFromResID(IDS_SEL_ONLYONE),MB_OK|MB_ICONASTERISK);
			return;
		}
		m_nSelSymbol = m_wndListSymbol.GetNextSelectedItem(pos);
		if (m_nSelSymbol<0) return;	
		m_bSymbolSelected = TRUE;
	}
	//}
	CString strNameAndCode = m_wndIdxTree.GetItemText(m_hCurItem);

	char layName[_MAX_FNAME];
	int num;
	sscanf(strNameAndCode,"%i %s",&num,layName);
	
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}

	m_strcopyLayerName = layName;

}

void CDlgScheme::OnPasteSymbols()
{
	if (m_strcopyLayerName.IsEmpty())  return;

	if (m_nLayerIndex == -1) return;
	
	CString strNameAndCode = m_wndIdxTree.GetItemText(m_hCurItem);
	
	char layName[_MAX_FNAME];
	int num;
	sscanf(strNameAndCode,"%i %s",&num,layName);
	
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}
	
	CSchemeLayerDefine *pCopyLayer = m_config.pScheme->GetLayerDefine(m_strcopyLayerName,FALSE,0,m_bSpecialLayer);
	if (!pCopyLayer)  return;

	CSchemeLayerDefine *pPasteLayer = m_config.pScheme->GetLayerDefine(layName,FALSE,0,m_bSpecialLayer);
	if (!pPasteLayer)  return;

	int nClassType0 = m_config.pScheme->GetLayerDefine(m_strcopyLayerName,FALSE,0,m_bSpecialLayer)->GetGeoClass();
	int nClassType1 = m_config.pScheme->GetLayerDefine(layName,FALSE,0,m_bSpecialLayer)->GetGeoClass();

	if (nClassType0 != nClassType1)
	{
		if (AfxMessageBox(StrFromResID(IDS_TIP_COPYSYMS),MB_OKCANCEL|MB_ICONASTERISK) == IDCANCEL)
			return;
	}

	CArray<CSymbol*,CSymbol*> arr;
	pCopyLayer->GetSymbols(arr);
	//hcw 2012-2-13 
	if (m_binSymbolListRegion&&m_bSymbolSelected)
	{
		for (int i=0; i<arr.GetSize(); i++)
		{
			if(m_nSelSymbol==i)
			{
				CSymbol *pSym = arr[i];		
				
				if( !pSym->IsSupportGeoType(nClassType1) )
					continue;
				
				int nType = pSym->GetType();
				CSymbol *pAddSym = GCreateSymbol(nType);
				if (pAddSym)
				{
					pAddSym->CopyFrom(pSym);
					pPasteLayer->AddSymbol(pAddSym);
				}

			}
			else 
				continue;
		}

	}
	else
	{
		for (int i=0; i<arr.GetSize(); i++)
		{
			
			CSymbol *pSym = arr[i];		
			
			if( !pSym->IsSupportGeoType(nClassType1) )
				continue;
			
			int nType = pSym->GetType();
			CSymbol *pAddSym = GCreateSymbol(nType);
			if (pAddSym)
			{
				pAddSym->CopyFrom(pSym);
				pPasteLayer->AddSymbol(pAddSym);
			}
			
		}
		m_bSymbolSelected = FALSE;//hcw,2012.2.14
	}

	FillSymbolList();

}

void CDlgScheme::OnMovetoTop()
{
	if(m_nLayerIndex < 0)
		return;
	
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )
	{
		AfxMessageBox(StrFromResID(IDS_SEL_ONLYONE),MB_OK|MB_ICONASTERISK);
		return;
	}

	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	if (nsel == 0) return;

	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SymbolToTop(nsel);

	FillSymbolList();

	m_bModified = TRUE;
}

void CDlgScheme::OnMovetoTail()
{
	if(m_nLayerIndex < 0)
		return;
	
	POSITION pos = m_wndListSymbol.GetFirstSelectedItemPosition();
	if( !pos )
	{
		AfxMessageBox(StrFromResID(IDS_SEL_ONLYONE),MB_OK|MB_ICONASTERISK);
		return;
	}
	
	int nsel = m_wndListSymbol.GetNextSelectedItem(pos);
	if (nsel == m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetSymbolCount()-1) return;
	
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->SymbolToTail(nsel);

	FillSymbolList();

	m_bModified = TRUE;
}
void CDlgScheme::OnLinefillButton() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)
		return;

	while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_NO_LINE);
		szCaption.LoadString(IDS_ATTENTION);
		if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
		{
			CDlgCellDefLinetypeView dlg;
			CSwitchScale scale(m_config.GetScale());
			dlg.SetShowMode(FALSE,FALSE,1);
			dlg.SetConfig(m_config);
			
			dlg.DoModal();
		}
		else
			return;
	}

	CLineHatch *pDash = new CLineHatch();
	pDash->m_fIntv = 1;
	if (!pDash) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pDash->IsSupportGeoType(nClassType) )
	{
		MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
		delete pDash;
		return;
	}
	
	pDash->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pDash);

	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
}

void CDlgScheme::OnDiagonalButton() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0)
		return;

	while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
	{
		CString szText,szCaption;
		szText.LoadString(IDS_NO_LINE);
		szCaption.LoadString(IDS_ATTENTION);
		if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
		{
			CDlgCellDefLinetypeView dlg;
			CSwitchScale scale(m_config.GetScale());
			dlg.SetShowMode(FALSE,FALSE,1);
			dlg.SetConfig(m_config);
			
			dlg.DoModal();
		}
		else
			return;
	}

	CDiagonal *pDash = new CDiagonal();
	if (!pDash) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
	if( !pDash->IsSupportGeoType(nClassType) )
	{
		MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
		delete pDash;
		return;
	}
	
	pDash->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pDash);

	m_bAddSymbol = TRUE;
	
	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();
	
	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
}
/*
void CDlgScheme::OnCellscaleButton() 
{
	// TODO: Add your control notification handler code here
	if (m_nLayerIndex < 0 || m_config.pCellDefLib==NULL)
		return;
	
	if (m_config.pCellDefLib->GetCellDefCount() < 1)
	{
		AfxMessageBox(StrFromResID(IDS_NO_CELL),MB_OK|MB_ICONASTERISK);
		return;
	}

	CScaleCelltype *pCellScale = new CScaleCelltype();
	if (!pCellScale) return;

	int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex)->GetGeoClass();
	if( !pCellScale->IsSupportGeoType(nClassType) )
	{
		AfxMessageBox(StrFromResID(IDS_NO_SUPPORT),MB_OK|MB_ICONASTERISK);
		delete pCellScale;
		return;
	}

	pCellScale->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
	m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pCellScale);
	
	m_bAddSymbol = TRUE;

	DrawPreview(m_nLayerIndex);
	
	FillSymbolList();

	m_bAddSymbol = FALSE;

	m_bModified = TRUE;
}
*/

void CDlgScheme::OnSpecialSymbolsButton()
{
	if (m_nLayerIndex < 0)
		return;	
	
	CMenu menu, popMenu;
	if (!menu.CreateMenu() || !popMenu.CreatePopupMenu()) return;	

	menu.AppendMenu(MF_POPUP,(UINT_PTR)(popMenu.m_hMenu),"sub");
	popMenu.AppendMenu(MF_STRING,ID_PARA_SYMBOL,StrFromResID(IDS_PARA_SYMBOL));
	popMenu.AppendMenu(MF_STRING,ID_ANGBISECTOR_SYMBOL,StrFromResID(IDS_ANGBISECTOR_SYMBOL));
	popMenu.AppendMenu(MF_STRING,ID_SCALEARC_SYMBOL,StrFromResID(IDS_SCALEARC_SYMBOL));
	popMenu.AppendMenu(MF_STRING,ID_SCALETURNPLATE_SYMBOL,StrFromResID(IDS_SCALETURNPLATE_SYMBOL));
	popMenu.AppendMenu(MF_STRING,ID_SCALECRANE_SYMBOL,StrFromResID(IDS_SCALECRANE_SYMBOL));
	popMenu.AppendMenu(MF_STRING,ID_SCALEFUNNEL_SYMBOL,StrFromResID(IDS_SCALEFUNNEL_SYMBOL));

	popMenu.AppendMenu(MF_STRING,ID_SCALE_CELLLINETYPE,StrFromResID(IDS_SCALE_CELLLINETYPE));
	popMenu.AppendMenu(MF_STRING,ID_SCALE_CELL,StrFromResID(IDS_SCALE_CELL));
	popMenu.AppendMenu(MF_STRING,ID_LIANGCANG_OLD,StrFromResID(IDS_LIANGCANG_OLD));
	popMenu.AppendMenu(MF_STRING,ID_YOUGUAN_OLD,StrFromResID(IDS_YOUGUAN_OLD));
	popMenu.AppendMenu(MF_STRING,ID_JIANFANGWU_OLD,StrFromResID(IDS_JIANFANGWU_OLD));
	popMenu.AppendMenu(MF_STRING,ID_SCALE_WENSHI,StrFromResID(IDS_WENSHI_NEW));
	popMenu.AppendMenu(MF_STRING,ID_SCALE_CHUANSONGDAI_OLD,StrFromResID(IDS_CHUANSONGDAI_OLD));
	popMenu.AppendMenu(MF_STRING,ID_SCALE_CHURUKOU_OLD,StrFromResID(IDS_SCALE_CHURUKOU_OLD));
	popMenu.AppendMenu(MF_STRING,ID_CULVERTSURFACE1,StrFromResID(IDS_CULVERTSURFACE)+"-1");
	popMenu.AppendMenu(MF_STRING,ID_CULVERTSURFACE2,StrFromResID(IDS_CULVERTSURFACE)+"-2");
	popMenu.AppendMenu(MF_SEPARATOR);

	popMenu.AppendMenu(MF_STRING,ID_TIDALWATER,StrFromResID(IDS_TIDALWATER));
	popMenu.AppendMenu(MF_STRING,ID_TONGCHE_SHUIZHA_OLD,StrFromResID(IDS_TONGCHE_SHUIZHA_OLD));
	popMenu.AppendMenu(MF_STRING,ID_BUTONGCHE_SHUIZHA_OLD,StrFromResID(IDS_BUTONGCHE_SHUIZHA_OLD));
	popMenu.AppendMenu(MF_STRING,ID_DISHANGYAODONG_OLD,StrFromResID(IDS_DISHANG_YAODONG_OLD));
	popMenu.AppendMenu(MF_STRING,ID_SCALE_OLDDOUYA,StrFromResID(IDS_SCALE_OLDDOUYA));
	popMenu.AppendMenu(MF_STRING,ID_SCALE_XIEPO,StrFromResID(IDS_SCALE_XIEPO));
	popMenu.AppendMenu(MF_SEPARATOR);
	popMenu.AppendMenu(MF_STRING,ID_COLORHATCH_CONDITION,StrFromResID(IDS_CONDITION_COLORHATCH));

	CPoint pos;
	::GetCursorPos(&pos);
	CWnd *pWnd = GetDlgItem(IDC_SYMBOLS_BUTTON);
	if( pWnd )
	{
		CRect rect;
		pWnd->GetWindowRect(&rect);
		pos = CPoint(rect.left,rect.bottom);
	}	

	CMenu* pSumMenu = menu.GetSubMenu(0);

	pSumMenu->TrackPopupMenu (TPM_LEFTALIGN, pos.x, pos.y, this);


}

BOOL CDlgScheme::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (m_nLayerIndex < 0)
		return CDialog::OnCommand(wParam,lParam);

	BOOL bSucceed = FALSE;
	int menuID = LOWORD(wParam);
    if (menuID == ID_PARA_SYMBOL)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CParaLinetype *pPara = new CParaLinetype();
		if (!pPara) return CDialog::OnCommand(wParam,lParam);
		
		pPara->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pPara) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pPara->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pPara;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pPara);
		
		bSucceed = TRUE;		
		
    }
	else if (menuID == ID_ANGBISECTOR_SYMBOL)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CAngBisectortype *pAng = new CAngBisectortype();
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		pAng->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pAng->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pAng;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pAng);
		
		bSucceed = TRUE;		
		
    }
	else if (menuID == ID_SCALEARC_SYMBOL)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleArctype *pAng = new CScaleArctype();
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		pAng->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pAng->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pAng;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pAng);
		
		bSucceed = TRUE;		
		
    }
	else if (menuID == ID_SCALETURNPLATE_SYMBOL)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleTurnplatetype *pAng = new CScaleTurnplatetype();
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		pAng->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pAng->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pAng;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pAng);
		
		bSucceed = TRUE;		
		
    }
	else if (menuID == ID_SCALECRANE_SYMBOL)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleCranetype *pAng = new CScaleCranetype();
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		pAng->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pAng->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pAng;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->AddSymbol(pAng);
		
		bSucceed = TRUE;		
		
    }
	else if (menuID == ID_SCALEFUNNEL_SYMBOL)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleFunneltype *pAng = new CScaleFunneltype();
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		pAng->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pAng->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pAng;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pAng);
		
		bSucceed = TRUE;		
		
    }
	else if (menuID == ID_SCALE_CHUANSONGDAI_OLD)
    {
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleChuanSongDai *pAng = new CScaleChuanSongDai();
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		pAng->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		if (!pAng) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pAng->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pAng;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pAng);
		
		bSucceed = TRUE;		
		
    }
	else if( menuID==ID_SCALE_CELLLINETYPE )
	{
		while (m_config.pCellDefLib->GetCellDefCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_CELL);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,0);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleCellLinetype *pSym = new CScaleCellLinetype();
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		pSym->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
		
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if( menuID==ID_TIDALWATER )
	{
		while (m_config.pCellDefLib->GetCellDefCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_CELL);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,0);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CTidalWaterSymbol *pSym = new CTidalWaterSymbol();
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		pSym->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
		
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if( menuID==ID_SCALE_CELL )
	{
		while (m_config.pCellDefLib->GetCellDefCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_CELL);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,0);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		CScaleCell *pSym = new CScaleCell();
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		pSym->m_strCellDefName = m_config.pCellDefLib->GetCellDef(0).m_name;
		
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if( menuID==ID_SCALE_OLDDOUYA )
	{
		CScaleOldDouya *pSym = new CScaleOldDouya();

		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if( menuID==ID_SCALE_XIEPO )
	{
		CScaleXiepo *pSym = new CScaleXiepo();
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if( menuID==ID_COLORHATCH_CONDITION )
	{
		CConditionColorHatch *pSym = new CConditionColorHatch();
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if( menuID>=ID_LIANGCANG_OLD && menuID<=ID_SCALE_WENSHI )
	{
		while (m_config.pLinetypeLib->GetBaseLineTypeCount() < 1)
		{
			CString szText,szCaption;
			szText.LoadString(IDS_NO_LINE);
			szCaption.LoadString(IDS_ATTENTION);
			if( IDYES == MessageBox(szText,szCaption,MB_YESNO|MB_ICONQUESTION) )
			{
				CDlgCellDefLinetypeView dlg;
				CSwitchScale scale(m_config.GetScale());
				dlg.SetShowMode(FALSE,FALSE,1);
				dlg.SetConfig(m_config);
				
				dlg.DoModal();
			}
			else
				return CDialog::OnCommand(wParam,lParam);
		}
		
		CProcSymbol_LT *pSym = NULL;
		switch( menuID )
		{
		case ID_LIANGCANG_OLD:
			pSym = new CScaleLiangCang();
			break;
		case ID_YOUGUAN_OLD:
			pSym = new CScaleYouGuan();
			break;
		case ID_JIANFANGWU_OLD:
			pSym = new CScaleJianFangWu();
			break;
		case ID_TONGCHE_SHUIZHA_OLD:
			pSym = new CScaleTongCheShuiZha();
			break;
		case ID_BUTONGCHE_SHUIZHA_OLD:
			pSym = new CScaleBuTongCheShuiZha();
			break;
		case ID_DISHANGYAODONG_OLD:
			pSym = new CScaleDiShangYaoDong();
			break;
		case ID_SCALE_CHURUKOU_OLD:
			pSym = new CScaleChuRuKou();
			break;
		case ID_SCALE_WENSHI:
			pSym = new CScaleWenShi();
			break;
		default:
			break;
		}
		if (!pSym) return CDialog::OnCommand(wParam,lParam);
		
		pSym->m_strBaseLinetypeName = m_config.pLinetypeLib->GetBaseLineType(0).m_name;
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if(menuID == ID_CULVERTSURFACE1)
	{
        CCulvertSurface1Symbol* pSym = new CCulvertSurface1Symbol();
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}
	else if(menuID == ID_CULVERTSURFACE2)
	{
        CCulvertSurface2Symbol* pSym = new CCulvertSurface2Symbol();
		
		int nClassType = m_config.pScheme->GetLayerDefine(m_nLayerIndex,m_bSpecialLayer)->GetGeoClass();
		if( !pSym->IsSupportGeoType(nClassType) )
		{
			MessageBox(StrFromResID(IDS_NO_SUPPORT),NULL,MB_OK|MB_ICONASTERISK);
			delete pSym;
			return CDialog::OnCommand(wParam,lParam);
		}
		
		m_config.pScheme->GetLayerDefine(m_nLayerIndex)->AddSymbol(pSym);
		
		bSucceed = TRUE;
	}

	if (bSucceed)
	{
		m_bAddSymbol = TRUE;
		
		DrawPreview(m_nLayerIndex);
		
		FillSymbolList();
		
		m_bAddSymbol = FALSE;
		
		m_bModified = TRUE;
	}
	
    return CDialog::OnCommand(wParam,lParam);
	
}

void CDlgScheme::OnPaint() 
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

		BOOL bMulti = m_wndIdxTree.m_bRealMulti;
		if (!bMulti)
		{
			//m_nLayerIndex = m_nLayerIndex-1;
			DrawPreview(m_nLayerIndex);//hcw,2012.3.29
		}
		
	}
}

HCURSOR CDlgScheme::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDlgScheme::OnSortIdcode() 
{
	// TODO: Add your command handler code here
	CTreeItemList tmpTreeList;
	m_wndIdxTree.GetSelectedList(tmpTreeList);
	//{hcw,2012.2.27, 
	if (tmpTreeList.IsEmpty())
	{
		tmpTreeList.AddHead(&m_treeNodeList);
	}
	//}
	HTREEITEM parentITEM;
	
	CString strGroupName = "";
	while(!tmpTreeList.IsEmpty())
	{
		parentITEM = m_wndIdxTree.GetParentItem((HTREEITEM)tmpTreeList.GetHead());
		if (parentITEM==NULL)
		{
			m_wndIdxTree.SortChildren((HTREEITEM)tmpTreeList.GetHead());
			strGroupName = m_wndIdxTree.GetItemText((HTREEITEM)tmpTreeList.GetHead());
		}
		SortLayerByGroup(strGroupName);//hcw,2012.7.13,transfer to here.
		tmpTreeList.RemoveHead();
	}
	
	m_bModified = TRUE;//hcw,2012.4.5,排序后提示保存。

}
void CDlgScheme::SortLayerByGroup(CString strGroupName)
{
	vector<IDX> vAIdx;
	vector<IDX>::iterator iter;
	for (int i = 0; i<m_UserIdx.m_aGroup.GetSize(); i++)
	{
		CString strTmp="";
		strTmp.Format("%s",m_UserIdx.m_aGroup[i].GroupName);
		if (strTmp==strGroupName)
		{
			
			for (int j = 0; j<m_UserIdx.m_aIdx.GetSize();j++)
			{
				if (m_UserIdx.m_aIdx[j].groupidx == i)
				{
					vAIdx.push_back( m_UserIdx.m_aIdx[j]);
					m_UserIdx.m_aIdx.RemoveAt(j);
					j--;
				}
				
			}
			
			sort(vAIdx.begin(),vAIdx.end(),less_layCode);
		
			for (iter = vAIdx.begin(); iter!=vAIdx.end(); iter++)
			{
				m_UserIdx.m_aIdx.Add(*iter);
			}
			
		}
	}
	m_config.pScheme->SetRecentIdx(m_UserIdx);
	m_config.pScheme->SortLayerDefinebyCode(strGroupName); //hcw,2012.7.13,根据组名来排序
	return;
}
void CDlgScheme::OnClearLinetype() 
{
	// TODO: Add your command handler code here
	CDlgClearLineType dlg;
	//dlg.SetFont(this->GetParent()->GetFont(),FALSE);
	//dlg.m_config = m_config;
	//while (IDOK==dlg.DoModal())//IDOK 为“清理”按钮
	if(AfxMessageBox(IDS_CLEARLINETYPE_YESORNO,MB_YESNO)==IDOK)
	{
		//hcw,2012.2.21,clear the name of LineType
		UpdateData(TRUE);
		//clear lineTypeLib below various scale
		//清理修改非法的线型名。
		if (1 || dlg.m_bOverZero)
		{
			CString strScale;
			int curScale = m_config.GetScale();
			int curScaleItem = 0;
			//若当前比例尺无效?
			int j = 0;//比例尺id;
			int nLineByteCount = 0;
			CString LineTypeName = ""; 
			BaseLineType lineType;
			BaseLineType lineTypeAfterClear;
			BOOL bChangeName = FALSE;
			BOOL bChangeCnTxt = FALSE;
			CString LTNamebeforeClear;//修改之前的线型名
			map<CString, CString> lineTypeNameMap; //修改前后线型名之间的映射。
			map<CString,BaseLineType> lineTypeMap; //hcw,2012.3.1,修前的线型名——>修改后的线型结构。   
			m_configsbyScale.RemoveAll();
			do 
			{
				nLineByteCount = m_config.pLinetypeLib->GetBaseLineTypeCount();
				LineTypeName = ""; 
				lineTypeNameMap.clear();
				lineTypeMap.clear();
				//修改线型库的过程中，对照修改scheme中的相关信息				
				for(int i=0; i<nLineByteCount; i++)		
				{	
					lineType = m_config.pLinetypeLib->GetBaseLineType(i);
					LineTypeName = (CString)lineType.m_name;
					
					LTNamebeforeClear = LineTypeName;
					
					/*delete the invalid "0" in LineType*/					
					if(bChangeName = hasValidZeroinLineTypeName(LineTypeName))
					{
						
						ClearInvalidZeroinLineTypeName(LineTypeName);
						//ClearInvalidSeparator(LineTypeName);
						lineTypeNameMap.insert(make_pair(LTNamebeforeClear, LineTypeName)); 
						memset(lineType.m_name,NULL,_MAX_FNAME);
						strcpy(lineType.m_name, LineTypeName.GetBuffer(0));
						
						
					}
					if (bChangeCnTxt = HasInvalidZeroinLineTypeCnTxt(lineType.m_fLens, lineType.m_nNum))
					{

						ClearInvalidZeroinLineTypeCnTxt(lineType.m_fLens, lineType.m_nNum);				

					}
					//for confict;
					if (bChangeName)
					{
						memcpy(lineTypeAfterClear.m_fLens,lineType.m_fLens,8);
						lineTypeAfterClear.m_nNum = lineType.m_nNum;
						strcpy(lineTypeAfterClear.m_name, LineTypeName.GetBuffer(0));
						lineTypeMap.insert(make_pair(LTNamebeforeClear,lineTypeAfterClear));
					}
					
					if (bChangeCnTxt||bChangeName)
					{
						m_config.pLinetypeLib->SetBaseLineType(i,lineType);
					}
					
										
				}	
				
				/*2012.3.1,处理冲突，并修改map(lineTypeNameMap),为后面修改层服务*/
				map<CString ,CString> lineTypeNameCflctMap;
				if (hasLineTypeConflict(lineTypeMap))
				{
					lineTypeNameCflctMap = ClearLineTypeConflict(lineTypeMap);
				}

				if (!lineTypeNameCflctMap.empty())
				{
					SetLineTypeNameMap(lineTypeNameMap, lineTypeNameCflctMap);
				}				

				/*对照修改同一比例尺层中线型名*/
				ModifiedLineTypeinLayerDef(lineTypeNameMap, m_config);
				m_scaleCombo.GetLBText(j,strScale);
				j++;
				if(curScale==atoi(strScale))
				{
					curScaleItem = j;
					continue;					
				}
				else//获取
				{
					m_config = gpCfgLibMan->GetConfigLibItemByScale(atoi(strScale));
				}
				m_configsbyScale.Add(m_config);		
			} while (j <= m_scaleCombo.GetCount()); //2012.4.9,"<"→"<="
			m_config = gpCfgLibMan->GetConfigLibItemByScale(curScale);
			//{2012.4.10,重新获取当前层序号。
			m_nLayerIndex = m_config.pScheme->GetLayerDefineIndex(m_StrLayName,m_bSpecialLayer);
			//}
 			m_bModified = TRUE;
			MessageBox(StrFromResID(IDS_CLEARLINETYPE_OK),StrFromResID(IDS_TIPS),MB_OK);//2012.4.9
		}		
	}
	
	FillSymbolList();
}
void CDlgScheme::SetLineTypeNameMap(map<CString, CString>& LineTypeNameMap, map<CString,CString> lineTypeNameCflctMap)
{
	map<CString,CString>::const_iterator iterF;
	map<CString,CString>::iterator iter2;

		for(iter2=lineTypeNameCflctMap.begin(); iter2!=lineTypeNameCflctMap.end(); iter2++)
		{
			iterF = LineTypeNameMap.find(iter2->first);
			if (iterF!=LineTypeNameMap.end())
			{
				iter2->second = iterF->second;
			}

		}
	
	return;
}
BOOL CDlgScheme::hasLineTypeConflict(map<CString,BaseLineType>& lineTypeMap)
{
	map<CString, BaseLineType>::const_iterator iter1;
	map<CString, BaseLineType>::const_iterator iter2;
	map<CString, BaseLineType> conflictCollectMap;
	conflictCollectMap.clear();
	CString str1;
	CString str2;
	for (iter1=lineTypeMap.begin(); iter1 != lineTypeMap.end(); iter1++)
	{
		for (iter2=lineTypeMap.begin(); iter2 != lineTypeMap.end(); iter2++)
		{
			if (iter1->first != iter2->first)
			{
				str1.Format(iter1->second.m_name);
				str2.Format(iter2->second.m_name);
				if(str1==str2)

				{

					conflictCollectMap.insert(*iter1);
					
				}
			}
		}
	}
	if (!conflictCollectMap.empty())
	{
		lineTypeMap = conflictCollectMap;
		return TRUE;
	}
	
	return FALSE;
}

map<CString, CString> CDlgScheme::ClearLineTypeConflict(map<CString,BaseLineType> lineTypeMap)
{
	map<CString, CString> tmpMap;
	map<CString, BaseLineType>::iterator iter1;
	map<CString, BaseLineType>::iterator iter2;
	map<CString, BaseLineType>::const_iterator iterJudge;
	int idx;
	CString stridx;
	CString strLTName;
	CString str1;
	CString str2;
	for (iter1 = lineTypeMap.begin(); iter1 != lineTypeMap.end(); iter1++)
	{
		idx = 0; 
		for (iter2 = lineTypeMap.begin(); iter2 != lineTypeMap.end(); iter2++)
		{
			if (iter1->first!=iter2->first)
			{
				str1.Format(iter1->second.m_name);
				str2.Format(iter2->second.m_name);
				if (str1==str2)
				{
					if (!CompareLineTypeCntxt(iter1->second, iter2->second))//若线型内容相同，删除
					{
						m_config.pLinetypeLib->DelBaseLineType(iter1->second.m_name);
					}
					else//若线型内容不同,改名，并返回相应的LineTypeNameMap;
					{
						strLTName = (CString)iter1->second.m_name;
						CString strLTNameOriginal = strLTName; 
						//修改->循环判断->修改...直到无重复位置
						BOOL bSame = TRUE;
						do 
						{
							strLTName = "";
							idx++;
							stridx.Format("%d", idx);
							strLTName = strLTNameOriginal + "_" +stridx;
							for (iterJudge = lineTypeMap.begin(); iterJudge!=lineTypeMap.end(); iterJudge++)
							{
								if (strLTName == (CString)iterJudge->second.m_name)
								{
									bSame = TRUE;
									break;
								}
								else
								{
									bSame = FALSE;
									continue;
								}
							}

						}while(bSame);
						//重新设置m_config
						strcpy(iter1->second.m_name,strLTName.GetBuffer(0));
						m_config.pLinetypeLib->SetBaseLineType(strLTNameOriginal,iter1->second);
						tmpMap.insert(make_pair(iter1->first, strLTName));
					}
					
				}
			}
		}

	}
	return tmpMap;
}

BOOL CDlgScheme::CompareLineTypeCntxt(BaseLineType LType1, BaseLineType LType2)
{
	if (LType1.m_nNum==LType2.m_nNum)
	{
		for (int i=0; i<LType1.m_nNum; i++)
		{
			if (LType1.m_fLens[i]!=LType2.m_fLens[i])
			{
				return TRUE;
			}
		}
		return FALSE;
	}
	else
		return TRUE;
}
BOOL CDlgScheme::IsValidDefine(CString lineTypeName)
{
	int pos = lineTypeName.Find("0.00");
	if (pos>=0)
	{
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CDlgScheme::hasMatchedSep(CString lineTypeName, CString strSep)
{
	char*pTypeName = lineTypeName.GetBuffer(0);
	char*pStrSep = strSep.GetBuffer(0);

	while(*pTypeName!=*pStrSep)
	{
		pTypeName++;
		if (*pTypeName=='\0')
		{
			pTypeName = lineTypeName.GetBuffer(0);
			pStrSep++;
		}
		if (*pStrSep=='\0')
		{
			break;
		}
	}

	if (*pTypeName=='\0')
	{
		return FALSE;
	}
	return TRUE;

}
BOOL CDlgScheme::hasValidZeroinLineTypeName(CString lineTypeName)
{
	if (lineTypeName == "")
	{
		return FALSE;
	}
	
	int zeroPos = 0;
	int dotPos = 0;

	CString tmpSep="Xx*";
	BOOL bMatchedSep = hasMatchedSep(lineTypeName, tmpSep);
   
	if (!bMatchedSep) //没找到分隔符
    {
		 zeroPos = lineTypeName.ReverseFind('0');
		 dotPos = lineTypeName.ReverseFind('.');
		 if (dotPos>=0)
		 {
			 if(zeroPos==lineTypeName.GetLength()-1)
				return TRUE;
		 }
		 else 
		 {
			 if (zeroPos==0)
			 {
				return TRUE;	
			 }

		 }
		 return FALSE;
    }
	//若在线型名中找到了匹配的分隔符	
	{
		CString strToken="";
		char*token = strtok(lineTypeName.GetBuffer(0),tmpSep);
		while (token != NULL)
		{
			
			strToken.Format("%s",token);
			zeroPos = strToken.ReverseFind('0');
			 dotPos = strToken.ReverseFind('.');
			if (dotPos>=0)
			{
				if (zeroPos == strToken.GetLength()-1)
				{
					return TRUE;
				}
			}
			else
			{
				if(zeroPos == 0)
				{
					return TRUE;
				}
			}
			token = strtok(NULL,tmpSep);
		}
	  return FALSE; 
	}
	
}
BOOL CDlgScheme::HasInvalidZeroinLineTypeCnTxt(float* fCnTxtArray, int iLen)
{
	BOOL bHasInvalidZero=FALSE;
	for (int i=0; i<iLen; i++)
	{
		if (i%2==0)
		{
			if (((fCnTxtArray[i]>=-EPSILON)
				&&(fCnTxtArray[i]<=EPSILON))
				&&(fCnTxtArray[i]==fCnTxtArray[i+1]))
			{
				bHasInvalidZero = TRUE;	
			}
			//{hcw,2012.3.31
			if (i==(iLen-1)
				&&fCnTxtArray[i]>=-EPSILON
				&&fCnTxtArray[i]<=EPSILON)
			{
				bHasInvalidZero = TRUE;
			}
			//}
		}
	}
	//{hcw,2012.3.31
	if (bHasInvalidZero)
	{
		return TRUE;
	}
	else
		return FALSE;
	//}
}

void CDlgScheme::ClearInvalidZeroinLineTypeName(CString& LineTypeName)
{  
	CString strSep="Xx*";
	CString tmpStr;
	CString tmpLineTypeName = LineTypeName;
	int zeroPos;
	int dotPos;
	//retrieve the Sequence of separators
	
	/*Clear the invalid Zero of various part of LineTypeName
	and save these in a MultiMap array
	*/
	int iSepPos = 0;
	CStringArray ctxtArray;
	char* token = strtok(tmpLineTypeName.GetBuffer(0), strSep);
	while(token != NULL)
	{
		tmpStr.Format("%s", token);
		ClearSingleCtxt(tmpStr);
		ctxtArray.Add(tmpStr);
		token = strtok(NULL, strSep);	
		iSepPos++;
	}
	/*Retrieve the seqSeps*/
	tmpLineTypeName = LineTypeName;
	CString seqSeps = RetrieveSeqSeps(tmpLineTypeName);
	//combine seqSeps and Ctxts
	tmpLineTypeName = CombineSepsandCtxts(seqSeps, ctxtArray);
	LineTypeName = tmpLineTypeName;
	//system("pause");
	return;

}
//hcw,2012.3.1,清理线型内容中多余的“0”
void CDlgScheme::ClearInvalidZeroinLineTypeCnTxt(float* fCnTxtArray, int& iLen)
{
	if ((iLen>8)&&(iLen<1))
	 return;
	int iTmpLen = 0;
	int j=0;
	int loopCount = 0;
	for (int i=0; i<iLen; i++)
	{
		
		if(i%2==0)
		{
			j = i;
			while ((fCnTxtArray[j]>=-EPSILON)
				&&(fCnTxtArray[j]<=EPSILON)
				&&(fCnTxtArray[j]==fCnTxtArray[j+1])
				&&(j<iLen-1))//hcw,2012.3.30
			{
				j = j+2;
			}
			if (j>i)
			{
				//"0"在尾部
				if(i+1==iLen-1)
				{
					fCnTxtArray[i] = '\0';
					iLen = iLen-2;
				}
				//"0"在首部或中间
				else
				{
					loopCount = (j-i);
					do 
					{
						for (int j = i; j<iLen; j++)
						{
							fCnTxtArray[j] = fCnTxtArray[j+1];
							
						}
						loopCount--;
						iLen--;
					} while (loopCount>0);
					
					fCnTxtArray[iLen] = '\0';
				}			
				
			}
			//{hcw,2012.3.31,最后一个元素序数为奇数个情况下，若值为0，则将其清除。
			if ((i==(iLen-1))
				&&(fCnTxtArray[i]>-EPSILON)
				&&(fCnTxtArray[i]<EPSILON))
			{
				fCnTxtArray[i]=='\0';
				iLen--;
			}
			//}
		}
		
	}
	
}
void CDlgScheme::ClearSingleCtxt(CString& lineTypeCtxt)
{
	//if the str is not digital
	if(!IsDigital(lineTypeCtxt))
	{
		return;
	}
	//if the str is a digital
	int dotPos = lineTypeCtxt.ReverseFind('.');
	int zeroPos = lineTypeCtxt.ReverseFind('0');
	
	if (dotPos < 0)// 没找到小数点
	{
		while (zeroPos==0)
		{
			lineTypeCtxt.Delete(zeroPos,1);
			zeroPos = lineTypeCtxt.Find('0');
		}
	}
	else//找到小数点
	{

		do 
		{
			if (zeroPos==lineTypeCtxt.GetLength()-1)
			{
				lineTypeCtxt.Delete(zeroPos,1);
			}
			else if(dotPos==lineTypeCtxt.GetLength()-1)
			{
				lineTypeCtxt.Delete(dotPos,1);
			}
			else
			{
				break;
			}
			zeroPos = lineTypeCtxt.ReverseFind('0');
			dotPos = lineTypeCtxt.ReverseFind('.');
		} while (lineTypeCtxt!="");
		        

	}

}

CString CDlgScheme::RetrieveSeqSeps(CString lineTypeName)
{
	
	CString seqSeps;
	CString strSeps = "Xx*";
	char cStrSeps[_MAX_FNAME];
    char *pLTN = lineTypeName.GetBuffer(0);
	char *pStrSeps = strSeps.GetBuffer(0);
	int iSep = 0;
	
	while (*pStrSeps!='\0')
	{
		while(*pLTN!='\0')
		{
			if (*pLTN==*pStrSeps)
			{
				cStrSeps[iSep] = *pLTN;
				iSep++;
			}

			pLTN++;
		}
		pStrSeps++;
		pLTN = lineTypeName.GetBuffer(0);
	}
	cStrSeps[iSep] = '\0';
	seqSeps.Format("%s", cStrSeps);
	return seqSeps;
}

CString CDlgScheme::CombineSepsandCtxts(CString seqSeps, CStringArray& ctxtArray)
{
	CString strRtn="";
	CString tmpChr=""; 

	for (int i=0; i<ctxtArray.GetSize(); i++ )
	{
		if (i<seqSeps.GetLength())
		{
			tmpChr = seqSeps.GetAt(i);
		}
		else
			tmpChr = "";
		
		if (ctxtArray.GetAt(i)=="")
		{
			tmpChr = "";

		}

		strRtn += ctxtArray.GetAt(i)+tmpChr; 
		
	}
	tmpChr = strRtn.GetAt(strRtn.GetLength()-1);
	if (!IsDigital(tmpChr))
	{
		strRtn.Delete(strRtn.GetLength()-1, 1);
	}
	return strRtn;
}

void CDlgScheme::ModifiedLineTypeinLayerDef(map<CString, CString> lineTypeNameMap, ConfigLibItem& config)
{
	int idx=0; //层序号
	int nLayerCount=config.pScheme->GetLayerDefineCount();
	CArray<CSymbol*,CSymbol*> symbolsArray;
	for (idx=0; idx<nLayerCount; idx++)
	{
		config.pScheme->GetLayerDefine(idx)->GetSymbols(symbolsArray);
		
		if(hasLineType(symbolsArray))
		{
			CString str = symbolsArray.GetAt(0)->GetName();
			substitudeLineNameLayer(lineTypeNameMap, symbolsArray);			
			config.pScheme->GetLayerDefine(idx)->SetSymbols(symbolsArray);
			CString Clearstr = symbolsArray.GetAt(0)->GetName();

		}

	}
	
}

BOOL CDlgScheme::hasLineType(CArray<CSymbol*, CSymbol*>& symbolsArray)
{
	int nCount = symbolsArray.GetSize();
	if(nCount<=0)
		return FALSE;
	for (int i=0; i<nCount;i++)
		{
			if(symbolsArray.GetAt(i)->GetType()==2)//2为线型类别
			{
				return TRUE;
			}
		}
		return FALSE;
}

void CDlgScheme::substitudeLineNameLayer(map<CString, CString> lineTypeNameMap,
										 CArray<CSymbol*,CSymbol*>& symbolsArray)
{
	if (symbolsArray.GetSize()<=0)
		return;
	
	map<CString, CString>::const_iterator iteror;
	CString LTName;
	long LTType;
	CString ClearLTName;
	long ClearLTType;
	CSymbol* pSymbol;
	
	for (int i=0; i<symbolsArray.GetSize();i++)
	{
		pSymbol = symbolsArray.GetAt(i);
		LTName = pSymbol->GetName();
		iteror = lineTypeNameMap.find(LTName);
		if (iteror != lineTypeNameMap.end() )
		{
			
			((CDashLinetype*)pSymbol)->m_strBaseLinetypeName = iteror->second;//hcw,2012.2.29,在确定为线型的前提下，直接修改成员，not SetName();

			ClearLTName = pSymbol->GetName();
		
		}
	}
	return;
}

void CDlgScheme::OnListSelChanged()
{
	CString str;
	m_wndSearchRsltList.GetCapture();
	int idx = m_wndSearchRsltList.GetCurSel();
	int iLength = m_wndSearchRsltList.GetTextLen(idx);
	m_wndSearchRsltList.GetText(idx, str.GetBuffer(iLength));
	m_SelLayNameandId = str;
 	m_wndCodeEdit.SetWindowText(m_SelLayNameandId);
 	//m_wndCodeEdit.SetSel(str.GetLength(),-1);//hcw,2012.3.13,cancel off
	m_wndCodeEdit.SetFocus();
	m_bUpDown = TRUE;//hcw,2012.4.8,false->true;
	UpdateData(FALSE);
	::ReleaseCapture();
	UpdateWindow();
	
}


void CDlgScheme::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd()==m_wndSizeCtrl.GetSafeHwnd() )
	{
		int pos = m_wndSizeCtrl.GetPos()-50;
		if( pos>=0 )
			m_fPreviewSize = 1 + 0.08*(pos);
		else if( pos<0 )
			m_fPreviewSize = 1 + 0.016*(pos);

		DrawPreview(m_nLayerIndex);
	}
}

#define MAX_FILE_NUM 2000

static void GetShpFiles(HWND hWnd, CStringArray& arrFileNames)
{
	OPENFILENAME ofn;
	// 存储获取的多个文件名
	TCHAR *filename = new TCHAR[MAX_PATH*MAX_FILE_NUM];
	memset(filename,0,sizeof(TCHAR)*MAX_PATH*MAX_FILE_NUM);

	TCHAR OldDir[MAX_PATH];
	
	CString strTitle;
	strTitle.LoadString(IDS_SELECT_FILE);
	
	::GetCurrentDirectory(MAX_PATH, OldDir);
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = _T("SHP files (*.shp)\0*.shp\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename; // 指定返回的文件名
	ofn.nMaxFile = MAX_PATH*MAX_FILE_NUM;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = strTitle.GetBuffer(0);
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST
		| OFN_ALLOWMULTISELECT | OFN_EXPLORER; // 允许多文件选择
	ofn.lpstrDefExt = _T("*.shp");
	
	if(!GetOpenFileName(&ofn))
	{
		delete[] filename;
		strTitle.ReleaseBuffer();
		return;
	}
	
	strTitle.ReleaseBuffer();
	
	CString path = ofn.lpstrFile; //遇到第一个\0为止前的就是路径: path\0filename的path
	TCHAR *fname = ofn.lpstrFile;
		
	DWORD attr = ::GetFileAttributes(path);
	
	//单个文件
	if( attr!=-1 && (attr&FILE_ATTRIBUTE_DIRECTORY)==0 )
	{
		arrFileNames.Add(path);
	}
	else
	{
		if( path.Right(1)!=_T("\\") )
			path += _T("\\");
		
		// 跳过path, 找到第一个文件名的起始位置
		while (*fname++);  
		// 此时fname指向第一个文件名
		int i = 0;
		while(*fname)
		{
			CString shpFile = path+CString(fname);
			arrFileNames.Add(shpFile);
			
			// 找下一个文件名， 找到第一个分隔符\0后循环停止，同时偏移加一，刚好指向下一个文件名
			while (*fname++);
			i++;
		}
	}

	delete[] filename;
}


void CDlgScheme::OnImportLayDefFromShp()
{
	CStringArray arrFileNames;
	GetShpFiles(GetSafeHwnd(),arrFileNames);
	if(arrFileNames.GetSize()==0)
		return;

	int nFiles = arrFileNames.GetSize();

	CScheme *pScheme = m_config.pScheme;

	CString strGroupName = "ShpLayers";

	for(int i=0; i<pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		if(pScheme->m_strSortedGroupName[i].CompareNoCase(strGroupName)==0)
		{
			break;
		}
	}

	if(i>=pScheme->m_strSortedGroupName.GetSize())
	{
		pScheme->m_strSortedGroupName.Add(strGroupName);
	}

	__int64 maxLayCode = 0;
	int nLayDef = pScheme->GetLayerDefineCount(FALSE);
	for(i=0; i<nLayDef; i++)
	{
		CSchemeLayerDefine *pLayDef = pScheme->GetLayerDefine(i);
		if(pLayDef)
		{
			__int64 code = pLayDef->GetLayerCode();
			if(maxLayCode<code)
				maxLayCode = code;
		}
	}

	__int64 nCurCode = maxLayCode+1;
	
	for( i=0; i<nFiles; i++)
	{
		CString strName = arrFileNames.GetAt(i);

		strName = strName.Left(strName.GetLength()-4);

		int index = strName.ReverseFind('\\');
		CString code = strName.Right(strName.GetLength()-index-1);

		CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(code);
		if(pdef)
		{
			pScheme->DelLayerDefine(pScheme->GetLayerDefineIndex(code));
		}

		SHPHandle hShp = SHPOpen(strName,"rb");
		DBFHandle hDbf = DBFOpen(strName,"rb");

		if( hShp!=NULL && hDbf!=NULL )
		{
			int nEntities, nShapeType;
			SHPGetInfo(hShp,&nEntities,&nShapeType,NULL,NULL);

			int clstype = CLS_GEOCURVE;
			switch( nShapeType ) 
			{
			case SHPT_POINT:
			case SHPT_POINTZ:
				clstype = CLS_GEOPOINT;
				break;
			case SHPT_ARC:
			case SHPT_ARCZ:
				clstype = CLS_GEOCURVE;
				break;
			case SHPT_POLYGON:
			case SHPT_POLYGONZ:
				clstype = CLS_GEOSURFACE;
				break;
			default:
				clstype = CLS_GEOCURVE;
				break;
			}

			pdef = new CSchemeLayerDefine;
			pdef->SetGroupName(strGroupName);
			pdef->SetLayerCode(nCurCode++);
			pdef->SetLayerName(code);
			pdef->SetAccel("");
			pdef->SetSupportClsName(CStringArray());
			pdef->SetGeoClass(clstype);
			pdef->SetColor(RGB(255,255,255));

			int nFields = DBFGetFieldCount(hDbf);

			for(int j=0; j<nFields; j++)
			{
				char fieldName[100] = {0};
				int nFieldWidth = 0, nDecimal = 0;
				DBFFieldType fieldType = DBFGetFieldInfo(hDbf,j,fieldName,&nFieldWidth, &nDecimal);

				int fieldType2 = 0;
				switch(fieldType)
				{
				case FTString:
					fieldType2 = DP_CFT_VARCHAR;
					break;
				case FTInteger:
					fieldType2 = DP_CFT_INTEGER;
					break;
				case FTDouble:
					fieldType2 = DP_CFT_DOUBLE;
					break;
				case FTLogical:
					fieldType2 = DP_CFT_INTEGER;
					break;
				case FTInvalid:
					fieldType2 = DP_CFT_VARCHAR;
					break;
				default:
					fieldType2 = DP_CFT_VARCHAR;
					break;
				}

				XDefine item;
				memset(&item,0,sizeof(item));
				strncpy(item.field,fieldName,sizeof(item.field)-1);
				strncpy(item.name,fieldName,sizeof(item.name)-1);
				item.valuetype = fieldType2;
				item.valuelen = nFieldWidth;
				pdef->AddXDefine(item);
			}

			pScheme->AddLayerDefine(pdef,FALSE);
		
			if( hShp )SHPClose(hShp);
			if( hDbf )DBFClose(hDbf);

			m_bModified = TRUE;
		}
	}

	if(!m_bModified)
		return;

	FillTree();
	
	DrawPreview(m_nLayerIndex);
	
	FillAttList(m_nLayerIndex);
	
	FillColor();
	
	FillGeo();
	
	FillSymbolList();

	AfxMessageBox("Import OK!");
}

void SetLanguage( HMODULE hModule );

void CDlgScheme::OnImportLayDefFromMdb()
{
	static CString InitPath;

	char szPath[MAX_PATH];//存放打开的文件夹路径
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),szPath,InitPath,this->GetSafeHwnd(),//cjc 2012年11月7日句柄应该为当前窗口句柄而不是主窗口句柄
		FALSE,BIF_RETURNFSANCESTORS | BIF_EDITBOX |BIF_BROWSEINCLUDEFILES) )
		return;

	CString strmdbfilepath(szPath);
	InitPath = strmdbfilepath;

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));

	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos;
	if ((pos = strrchr(path, '\\')))*pos = '\0';
	if ((pos = strrchr(path, '\\')))*pos = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");

	CString cmd;
	cmd.Format("\"%s\" MdbLayDef \"%s\"", path, strmdbfilepath);
	CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo);
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}

	char tmpfilepath[_MAX_PATH] = { 0 };
	GetTmpFilePath(tmpfilepath, "mdblaydef.txt");
	FILE *fp = fopen(tmpfilepath, "rt");
	if (!fp)
	{
		AfxMessageBox(StrFromResID(IDS_MDBFILE_OPEN_FAILED));
		return;
	}

	CScheme *pScheme = m_config.pScheme;

	CString strGroupName = "MdbLayers";

	for(int i=0; i<pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		if(pScheme->m_strSortedGroupName[i].CompareNoCase(strGroupName)==0)
		{
			break;
		}
	}

	if(i>=pScheme->m_strSortedGroupName.GetSize())
	{
		pScheme->m_strSortedGroupName.Add(strGroupName);
	}

	__int64 maxLayCode = 0;
	int nLayDef = pScheme->GetLayerDefineCount(FALSE);
	for(i=0; i<nLayDef; i++)
	{
		CSchemeLayerDefine *pLayDef = pScheme->GetLayerDefine(i);
		if(pLayDef)
		{
			__int64 code = pLayDef->GetLayerCode();
			if(maxLayCode<code)
				maxLayCode = code;
		}
	}

	__int64 nCurCode = maxLayCode+1;//层码流水号

	char line[256];
	while (!feof(fp))
	{
		memset(line, 0, sizeof(line));
		fgets(line, sizeof(line) - 1, fp);

		if (line[0] == '\n') continue;
		if (0 == strncmp(line, "LayerSize", 9))
			break;

		char layername[32];
		int nEsriGeoType=3;
		int xDefSize = 0;
		sscanf(line, "%s%d%d", layername, &nEsriGeoType, &xDefSize);
		int clstype = CLS_GEOCURVE;
		switch (nEsriGeoType)
		{
		case mEsriGeometryPoint:
		case mEsriGeometryMultipoint:
			clstype = CLS_GEOPOINT;
			break;
		case mEsriGeometryLine:
			clstype = CLS_GEOCURVE;
			break;
		case mEsriGeometryPolyline:
			clstype = CLS_GEOCURVE;
			break;
		case mEsriGeometryPolygon:
			clstype = CLS_GEOSURFACE;
			break;
		default:
			clstype = CLS_GEOCURVE;
			break;
		}

		//创建新层
		CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(layername);
		if(pdef)
		{
			pScheme->DelLayerDefine(pScheme->GetLayerDefineIndex(layername));
		}
		pdef = new CSchemeLayerDefine;
		pdef->SetGroupName(strGroupName);
		pdef->SetLayerCode(nCurCode++);
		pdef->SetLayerName(layername);
		pdef->SetAccel("");
		pdef->SetSupportClsName(CStringArray());
		pdef->SetGeoClass(clstype);
		pdef->SetDbGeoClass(clstype);
		pdef->SetColor(RGB(255,255,255));

		for (int j = 0; j < xDefSize; j++)
		{
			char fieldname[32];
			int fieldType2, nLength, isNullable;
			fscanf(fp, "%s%d%d%d", fieldname, &fieldType2, &nLength, &isNullable);
			if (0 == strcmp(fieldname, "continue"))
				continue;

			XDefine item;
			memset(&item, 0, sizeof(item));
			strncpy(item.field, fieldname, sizeof(item.field) - 1);
			strncpy(item.name, fieldname, sizeof(item.name) - 1);
			item.valuetype = fieldType2;
			item.valuelen = nLength;
			item.isMust = (isNullable == 1? 0 : 1);
			pdef->AddXDefine(item);
		}
		pScheme->AddLayerDefine(pdef,FALSE);
		m_bModified = TRUE;
	}

	if(!m_bModified)  return;

	FillTree();
	
	DrawPreview(m_nLayerIndex);
	
	FillAttList(m_nLayerIndex);
	
	FillColor();
	
	FillGeo();
	
	FillSymbolList();

	AfxMessageBox("Import OK!");
}


void CDlgScheme::OnExportCodeTable_Symbols()
{
	CDlgExportCodeTable_Symbols dlg(NULL,gpCfgLibMan);
	dlg.DoModal();
}

extern BOOL Split(CString source, LPCTSTR divKey, CStringArray& dest);
void CDlgScheme::OnImportLayDefFromMdbRep()
{
	static CString InitPath;

	char szPath[MAX_PATH];//存放打开的文件夹路径
	if (!BrowseFolderEx(StrFromResID(IDS_SELECT_FILE), szPath, InitPath, this->GetSafeHwnd(),//cjc 2012年11月7日句柄应该为当前窗口句柄而不是主窗口句柄
		FALSE, BIF_RETURNFSANCESTORS | BIF_EDITBOX | BIF_BROWSEINCLUDEFILES))
		return;

	CString strmdbfilepath(szPath);
	InitPath = strmdbfilepath;

	STARTUPINFO stStartUpInfo;
	memset(&stStartUpInfo, 0, sizeof(STARTUPINFO));
	stStartUpInfo.cb = sizeof(STARTUPINFO);
	stStartUpInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION stProcessInfo;
	memset(&stProcessInfo, 0, sizeof(stProcessInfo));

	char path[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, path, _MAX_FNAME);
	char *pos;
	if ((pos = strrchr(path, '\\')))*pos = '\0';
	if ((pos = strrchr(path, '\\')))*pos = '\0';
	strcat(path, "\\bin\\VectorConvert.exe");

	CString cmd;
	cmd.Format("\"%s\" MdbLayDefRep \"%s\"", path, strmdbfilepath);
	CreateProcess(NULL, (LPSTR)(LPCTSTR)cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &stStartUpInfo, &stProcessInfo);
	while (WaitForSingleObject(stProcessInfo.hProcess, 5) != WAIT_OBJECT_0)
	{
	}

	char tmpfilepath[_MAX_PATH] = { 0 };
	GetTmpFilePath(tmpfilepath, "mdblaydefrep.txt");
	FILE *fp = fopen(tmpfilepath, "rt");
	if (!fp)
	{
		AfxMessageBox(StrFromResID(IDS_MDBFILE_OPEN_FAILED));
		return;
	}

	CScheme *pScheme = m_config.pScheme;

	CString strGroupName = "MdbLayers";

	for (int i = 0; i < pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		if (pScheme->m_strSortedGroupName[i].CompareNoCase(strGroupName) == 0)
		{
			break;
		}
	}

	if (i >= pScheme->m_strSortedGroupName.GetSize())
	{
		pScheme->m_strSortedGroupName.Add(strGroupName);
	}

	__int64 maxLayCode = 0;
	int nLayDef = pScheme->GetLayerDefineCount(FALSE);
	for (i = 0; i < nLayDef; i++)
	{
		CSchemeLayerDefine *pLayDef = pScheme->GetLayerDefine(i);
		if (pLayDef)
		{
			__int64 code = pLayDef->GetLayerCode();
			if (maxLayCode < code)
				maxLayCode = code;
		}
	}

	__int64 nCurCode = maxLayCode + 1;//层码流水号

	char line[256];
	while (!feof(fp))
	{
		memset(line, 0, sizeof(line));
		fgets(line, sizeof(line) - 1, fp);

		if (line[0] == '\n') continue;
		if (0 == strncmp(line, "LayerSize", 9))
			break;

		char layername[32];
		int nEsriGeoType = 3;
		int xDefSize = 0;
		sscanf(line, "%s%d%d", layername, &nEsriGeoType, &xDefSize);
		int clstype = CLS_GEOCURVE;
		switch (nEsriGeoType)
		{
		case mEsriGeometryPoint:
		case mEsriGeometryMultipoint:
			clstype = CLS_GEOPOINT;
			break;
		case mEsriGeometryLine:
			clstype = CLS_GEOCURVE;
			break;
		case mEsriGeometryPolyline:
			clstype = CLS_GEOCURVE;
			break;
		case mEsriGeometryPolygon:
			clstype = CLS_GEOSURFACE;
			break;
		default:
			clstype = CLS_GEOCURVE;
			break;
		}

		CArray<XDefine, XDefine> arrDefs;
		for (int j = 0; j < xDefSize; j++)
		{
			char fieldname[32];
			int fieldType2, nLength, isNullable;
			fscanf(fp, "%s%d%d%d", fieldname, &fieldType2, &nLength, &isNullable);
			if (0 == strcmp(fieldname, "continue"))
				continue;

			XDefine item;
			memset(&item, 0, sizeof(item));
			strncpy(item.field, fieldname, sizeof(item.field) - 1);
			strncpy(item.name, fieldname, sizeof(item.name) - 1);
			item.valuetype = fieldType2;
			item.valuelen = nLength;
			item.isMust = (isNullable == 1 ? 0 : 1);
			arrDefs.Add(item);
		}

		CStringArray layerNamesArr;
		Split(layername, ",", layerNamesArr);
		for (int k = 0; k < layerNamesArr.GetSize(); k++)
		{
			CString name = layerNamesArr[k];
			//创建新层
			CSchemeLayerDefine *pdef = pScheme->GetLayerDefine(name);
			if (pdef)
			{
				pScheme->DelLayerDefine(pScheme->GetLayerDefineIndex(name));
			}
			pdef = new CSchemeLayerDefine;
			pdef->SetGroupName(strGroupName);
			pdef->SetLayerCode(nCurCode++);
			pdef->SetLayerName(layername);
			pdef->SetAccel("");
			pdef->SetSupportClsName(CStringArray());
			pdef->SetGeoClass(clstype);
			pdef->SetDbGeoClass(clstype);
			pdef->SetColor(RGB(255, 255, 255));
			pScheme->AddLayerDefine(pdef, FALSE);
			m_bModified = TRUE;
		}
	}

	if (!m_bModified)  return;

	FillTree();

	DrawPreview(m_nLayerIndex);

	FillAttList(m_nLayerIndex);

	FillColor();

	FillGeo();

	FillSymbolList();

	AfxMessageBox("Import OK!");
}