// EditBaseDoc.h : interface of the CDlgDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_EDITBASEDOC_H__9448A513_8BBA_4039_AA0E_3B63324F32AD__INCLUDED_)
#define AFX_EDITBASEDOC_H__9448A513_8BBA_4039_AA0E_3B63324F32AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Worker.h"
#include "PropertiesViewBar.h "
#include "AccuBox.h"	// Added by ClassView
#include "Snap.h "
#include "EditDef.h"
#include "cadlib.h"
#include "dsm.h"
#include "RecentCmd.h "
#include "EditBase.h "
#include "ChkCmd.h"
#include "DlgDataSource.h"
#include "CheckExProp.h"
#include "DlgSetColor.h"
#include "DlgAnnoTip.h"
#include "access.h"
#include <afxmt.h>
#include "triangle2.h"

#define UNDO_TYPE_ASSOCIATION		5
#define UNDO_TYPE_NEWDELLAYER		6
#define UNDO_TYPE_MODIFYLAYER		7
#define UNDO_TYPE_MODIFYATTRI		8
#define UNDO_TYPE_MODIFYLAYEROFOBJ  9
#define UNDO_TYPE_ADJUSTIMAGEPOSITION	10


class CDlgCellDefLinetypeView;
class CUndoButton;
class CDlgSetupGroup;
class CDlgSelectShift;
class COsgbView;

BOOL Create3DPointArray(PT_3D *pts, int num, VARIANT *pvar, int size=sizeof(PT_3D), BOOL b3d=TRUE);
BOOL Get3DPointArray(CArray<PT_3D,PT_3D> &pts, VARIANT *pvar,BOOL b3d=TRUE);

struct GRIDParam
{
	double ox,oy;
	double dx,dy;
	double xr,yr;
	char strz[_MAX_PATH];
	COLORREF color;
	BOOL bVisible;
	BOOL bViewVect, bViewImg;
};

enum VIEW_TYPE
{
	VIEW_ALL = 0,
	VIEW_LOCAL = 1,
	VIEW_CURRENT_LAYER = 2,
	VIEW_CURRENT_FEATURECLASS = 3,
	VIEW_EXTERNAL = 4,
	VIEW_SHOWALL = 5,
	VIEW_HIDEALL = 6
};

//伪节点项
struct PseudoPtItem
{
	PseudoPtItem(){
		pFtr0 = pFtr1 = NULL;
		idx0 = idx1 = -1;
	}
	PT_3D pt;
	CFeature* pFtr0;
	int idx0;
	CFeature* pFtr1;
	int idx1;
};

//节点匹配项
struct VertexMatchItem
{
	VertexMatchItem(){
		pFtr = NULL;
		vertexFlag = 0;
	}
	CFeature *pFtr;
	int vertexFlag;  //0, none, 1, first pt, 2, last pt, 3, first and last
	PT_3D pt_new;
};

//线重叠的信息段
struct OverlapSection
{
	int idx0,idx1;
	double t0,t1;
	double x0,x1;
	double y0,y1;
	double z0,z1;
};

//线重叠的数据结构
struct OverlapLineItem
{
	OverlapLineItem(){
		pFtr = NULL;
	}
	OverlapLineItem(const OverlapLineItem &item){
		pFtr = item.pFtr;
		arry.Copy(item.arry);
	}
	
	OverlapLineItem& operator=(const OverlapLineItem &item){
		pFtr = item.pFtr;
		arry.Copy(item.arry);
		return *this;
	}
	
	CFeature *pFtr;
	CArray<OverlapSection,OverlapSection> arry;
};

//叠加参考矢量参数
struct dataParam
{
	CString strDataName;
	BOOL bIsActive;
	BOOL bShowBound;
	BOOL bEnableMono;
	COLORREF clrMono;
	CString strDataState;
};

extern HMENU FindMenuByID(HMENU hMenu, UINT id);
extern BOOL gbCreatingWithPath;
extern int FindColorOfVVT(COLORREF clr, BOOL bGo);
int GetPenCode(int cd, BOOL bToVVT=TRUE);
class CSelChangedExchanger;

struct ChkCmdReg
{
	ChkCmdReg(){
		lpProc_Create = NULL;
	}
	ChkCmdReg(const ChkCmdReg& a){
		lpProc_Create = a.lpProc_Create;
		checkCategory = a.checkCategory;
		checkName = a.checkName;
		reasons.Copy(a.reasons);
	}
	const ChkCmdReg& operator =( const ChkCmdReg& a ){
		lpProc_Create = a.lpProc_Create;
		checkCategory = a.checkCategory;
		checkName = a.checkName;
		reasons.Copy(a.reasons);
		return *this;
	}
	CString checkCategory;
	CString checkName;
	CStringArray reasons;
	LPPROC_CHKCMDCREATE lpProc_Create;

	
};
class CChkCmd;

class CDlgFilterSelect;


struct CMYTinObj
{
	CMYTinObj()
	{
		tins = NULL;
		pts = NULL;
		nTIN = 0;
		npt = 0;
		bounds = NULL;
	}
	~CMYTinObj()
	{
		Clear();
	}
	void Clear()
	{
		if (tins)
			delete[] tins;
		if (pts)
			delete[] pts;
		if (bounds)
			delete[] bounds;

		tins = NULL;
		pts = NULL;
		nTIN = 0;
		npt = 0;
		bounds = NULL;
	}
	BOOL GetTriangle(int i, MYPT_3D *pts);
	BOOL GetZFromTriangle(MYPT_3D *tri, double x, double y, double& z);

	void RefreshTriangleBounds();
	int FindTriangle(PT_3D pt);

	MYTIN			*tins;
	int				nTIN;
	MYPT_3D			*pts;
	int				npt;

	//每个三角形用4个float存储它的外包，外包值对pts[0]作了差分，减少内存占用;
	//bounds的长度为nTIN*4，每个外包的4个float依次存储xl,yl,xh,yh；
	float			*bounds;
};


class CDlgDoc : public CDocument, public CWorker
{
protected: // create from serialization only
	CDlgDoc();
	DECLARE_DYNCREATE(CDlgDoc)

// Attributes
public:
	BOOL UpdateAllViewsParams(WPARAM wParam, LPARAM lParam, BOOL send=TRUE);

	void UpdateAllOtherViews(LPARAM lHint = 0L,
		CObject* pHint = NULL);

	void SetMapArea();
	virtual int ActiveLayer(LPCTSTR strLayerName=NULL, BOOL bSaveRecnet=TRUE);
	CView* GetCurActiveView();
	void DxfToDem(LPCTSTR strDxf,LPCTSTR strDem,double dx,double dy);
	virtual CDataQueryEx* GetDataQuery();	
	virtual void UpdateDrag(long updatecode, const GrBuffer *pBuf=NULL, UpdateViewType updateview=uv_AllView);
	virtual void RefreshView();
	virtual BOOL UIRequest(long reqtype, LONG_PTR param);

	CDlgDataSource *GetDlgDataSource(int idx=-1);
	int GetDlgDataSourceCount();

	virtual void KeyDown(UINT nChar, int flag);
	virtual BOOL StartCommand(int id);
	virtual BOOL AddFtrLayer(CFtrLayer *pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer *pLayer);
	virtual BOOL UpdateFtrLayer(CFtrLayer *pLayer);
	virtual BOOL ModifyLayer(CFtrLayer *pLayer, LPCTSTR field, _variant_t& value, BOOL bUndo=TRUE, BOOL bAllView=TRUE);

	virtual BOOL AddObjectWithProperties(CPFeature pFtr, int layid = -1);
	virtual BOOL AddObject(CPFeature pFtr, int layid = -1);	
	virtual BOOL RestoreObject(FTR_HANDLE handle);
	virtual BOOL DeleteObject(FTR_HANDLE handle, BOOL bUpdateSel = TRUE);
	virtual BOOL ClearObject(FTR_HANDLE handle, BOOL bUpdateSel = TRUE);//彻底删除
	virtual BOOL UpdateObject(FTR_HANDLE handle, BOOL bOnlyUpdateView = FALSE);

	BOOL UpdateObjectKeepOrder(FTR_HANDLE handle, BOOL bOnlyUpdateView = FALSE);

	virtual BOOL ModifyLayerGroup(FtrLayerGroup *pGroup, LPCTSTR field, _variant_t& value, BOOL bUndo=TRUE, BOOL bAllView=TRUE);

	virtual void SetBound(Envelope e);
	void SetBound(const PT_3D *pts, double zmin, double zmax);
	virtual void SetScale(DWORD scale);

	virtual int GetFtrLayerIDOfFtr(FTR_HANDLE handle);
	CDlgDataSource* GetDlgDataSourceOfFtr(CPFeature pFtr);

	virtual void LButtonDown(PT_3D &pt, int flag);
	virtual void LButtonDblClk(PT_3D pt, int flag);
	virtual void RButtonDown(PT_3D pt, int flag);
	virtual void OnSelectState(PT_3D pt, int flag);

	//是否显示右键菜单
	BOOL CanShowRBDMenu();

	inline void SetAnchorPoint(PT_3D pt){ m_ptAnchor=pt; m_nAnchorState=2; }
	inline BOOL CanSetAnchor(){ return m_nAnchorState==1; }
	inline BOOL IsAutoSetAnchor(){ return m_bAutoSetAnchor; }
	CString  GetLastCmdName();

	// 被Undo Bar调用用于Undo界面操作
	virtual void GetUndoActions(CStringList& lstActions) const;
	virtual void GetRedoActions(CStringList& lstActions) const;
	void DoUndo(CUndoButton* pUndoBtn);

	void SetDataQueryType(BOOL bSymbolized);

	void CutObjsToLayer(CFtrLayer *pLayer1, CFtrLayer *pLayer2, BOOL bUndo=TRUE);
	int GetActiveDataSourceIdx()const{
		return m_nActiveData;
	}
	void ExecuteCheck(LPCTSTR pGroupName,LPCTSTR pItem, BOOL bForSelection);
	CChkCmd *CreateChkCmd(const CString &checkCategory,const CString &checkName);
	BOOL RegisterChkCmd(LPPROC_CHKCMDCREATE lpProc_Create);
	ChkCmdReg *GetChkCmdRegs(int &num);

	void SaveViewPreviewImage();

	void FindPseudoPoints(LPCTSTR layNames, double lfToler, BOOL bCheckZ, BOOL bCrossLay, CArray<PseudoPtItem,PseudoPtItem>& pts);
	void FindPseudoPointOne(PT_3D pt, LPCTSTR layNames, double lfToler, BOOL bCheckZ, BOOL bCrossLay, CArray<PseudoPtItem,PseudoPtItem>& pts);
	BOOL RestorePseudoPtItemFromPt(PT_3D pt, PseudoPtItem& item, BOOL bCheckZ);

	BOOL LoadCellDefs(CDlgDataSource *pDS);
	BOOL SaveCellDefToFile(LPCTSTR name, GrBuffer2d *pBuf);
	void CreateCADOptAttributes();

    //
	void UpdateRefDatafile(CArray<dataParam,dataParam&>& data_params);
protected:
	virtual void Post_InitDoc();
	virtual CSymbol* GetSymbol(CFeature *pObj);

	// for need of the father class 
	virtual CDataSource *GetDataSource();

	virtual HMENU GetDefaultMenu();
	void Destroy();

	CArray<ChkCmdReg,ChkCmdReg> m_arrChkCmdRegItems;

public:
	int m_nRefAll;
	float			m_fRefBoundExt;
	
	//图元浏览
	CDlgCellDefLinetypeView *m_pCellDefDlg;

	CDlgFilterSelect *m_pFilterSelect;
	CDlgSetColor *m_SetColorDlg;
	CDlgOutputLayerCheck *m_OutputLayerCheckDlg;

	CDlgSetupGroup	*m_pDlgGroup;
	CDlgAnnoTip     *m_pDlgAnnotip;

	CDlgSelectShift *m_pDlgSelectShift;

	BOOL  m_bOverlayGrid;
	BOOL  m_bShowLayDir;

    inline BOOL IsOverlayGrid(){ return m_bOverlayGrid; }
	inline int GetPluginItemIdx(){return m_nPlugItem;}

	CSnap			m_snap;
	CAccuBox		m_accuBox;

	BOOL			m_bPlaceProperties;

	CMYTinObj		m_tin;

	DWORD			m_nLastSaveTime;

	CString			m_strTmpFile;

// 	CDSM		m_dem;
// 	CString			m_strDemPath;
// 	BOOL			m_bModifyDEM;

	//是否鼠标右键切换采编状态
	BOOL m_bRCLKSwitch;

	//仿CAD操作风格
	BOOL m_bCADStyle;

	//采集后激活采集栏
	BOOL m_bFocusColletion;

	int m_nPlugItem;
private:

	HMENU CreatePluginMenu(const CArray<PlugItem,PlugItem> &arr);
	void SilenceToDxf(LPCTSTR strDxf);
	void LoadDemPoints(CDSM *pDem);
	BOOL LoadLayerConfig(int nScale = 0);
	
	PT_3D			m_ptAnchor;
	int				m_nAnchorState;
	BOOL			m_bAutoSetAnchor;

	int				m_nBeginBatchUpdateOp;

	BOOL			m_bLoadCmdParams;

	int             m_nShowLayerState;
	
public:
	CRecentCmd		m_RecentCmd;//最近命令

	CSelChangedExchanger * m_pSelEx;

	BOOL			m_bPromptSave;
	int				m_nPromptRet;
	BOOL	m_bObjectGroup;

	//点线面文本的开关，文档打开时，根据feature的visible状态重新计算这个标志的值；
	//当点击点、线、面、文本开关时，修改feature的visible值，并修改这些标志值；
	BOOL   m_bViewPoint, m_bViewCurve, m_bViewSurface, m_bViewText;

	//隐藏不入库的数据，该功能与 OnExportArcgisMDB 强相关
	BOOL   m_bHideFtrsNotGIS;

// Operations
public:
	void SetActualPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL
	BOOL SaveModified(CString strDataName);//必须是当前激活的数据源
	BOOL SaveModified(CDataSourceEx *pDS);
	BOOL ActiveDataSource(CString strDataName);
	BOOL ActiveDataSource(CDataSourceEx *pDS);
// Implementation
public:
	BOOL ReadTempRefData(CString strPathName, CDlgDataSource *&pDS);
	BOOL ReadRefData(CString path, CDlgDataSource *&pDS);
	BOOL LoadDatabase(LPCTSTR lpszPathName,int cfgscale =-1);
	BOOL LoadDatabase0(LPCTSTR lpszPathName,int cfgscale =-1);
//	virtual void SetCurDrawingObj(struct DrawingInfo info);
	virtual void SelectAll();
	void OnSelectChanged(BOOL bAlways=FALSE);
	void RefreshSelProperties();
	virtual BOOL  RestoreFtrLayer(CFtrLayer *pLayer);
	virtual ~CDlgDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void UpdateView(long viewid, long updateid, LPARAM param);

	BOOL ImportFDB(LPCTSTR path, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle);
	BOOL ImportXML(LPCTSTR path, CFtrLayer *pLayer, PT_3D move_pt, PT_3D rotate_pt, double angle, CFtrArray *pArr=NULL);

	BOOL ImportFDB(LPCTSTR path);

	void DoFilterSelect(CValueTable& tab);

protected:
	void ReadRefInfo(CStringArray &arrDSName);
	void SaveRefInfo();
	void UpdateBound();

	BOOL UpgradeFDB(LPCTSTR path);

	virtual void OnStartCommand(CCommand* pCmd);

	// called after ending the command
	virtual void OnEndCommand(CCommand* pCmd);

public:
	void OnRepairAllPtZText();
	void OnRepairOnePtZText();
	enum
	{
		BakType12 = 0,
		BakTypeLatest = 1,
		BakTypeUVS = 2
	};
	void SaveBakFiles(int nBakType=0);
	void BeginBatchUpdate();
	void EndBatchUpdate();
	void Save();
	BOOL SaveAs(LPCTSTR fileName, BOOL bfastcopy = FALSE);

	void ReplaceDemPoints(void *pDem, PT_3D *polys, int nPt);
	void BuildPartDem(PT_3D *polys, int nPt, double dx, double dy);
	void TrimDem(LPCTSTR demPath, PT_3D *polys, int nPt);
	void ObjectsToDxf(CPtrArray& arrFtrs, LPCTSTR strDxf);
	void ObjectsToDxf2(CPtrArray& arrFtrs, LPCTSTR strDxf);

	void UpdateSymDrawScale();
	void ReCreateAllFtrs();

	void CreateFigure(double xoff, double yoff);

	void StopWork();
	BOOL GetTextParamsFromScheme(CString strLayerName, CValueTable &tab);

	//计算单个地物与其他地物重叠的情况，结果存放在 arrItemFtrs 中
	void FindOverlapLines(CPFeature pFtr, CArray<OverlapLineItem,OverlapLineItem>& arrItemFtrs, BOOL bAutoSelectTarget, BOOL bCheckZ);
	void ProcessOverlapLineItems(CArray<OverlapLineItem,OverlapLineItem>& arrItemFtrs, CUndoFtrs& undo);
	void MakeCheckPtSample();

protected:
	void DoRepairAnno(CFeature* pFtr,CUndoFtrs& undo,int flag);
	//0，处理成功，1，无效点，2，已经不是悬挂点了，3，点在容差范围外
	int KillSuspendOne(CFeature *pFtr, PT_3D pt, double lfSusToler, double lfSusZToler, BOOL bCrossLay, BOOL bModifyZ, BOOL bVep, BOOL bInsertPoint, CUndoFtrs& undo, CFtrArray *pArrReplacedFtrs, BOOL bAllowVSuspend);
	void OnToolShowAllLayer();

	void UpdateSwitchStates();
	void SetFtrsVisibleBySwitchStates(const CRuntimeClass* pGeoClass, BOOL bVisible);
	void UpdateFlagOfHideFtrsNotGIS();
	void HideFtrsNotGIS(BOOL bHide);
	void GetFtrsNotGIS(CFtrArray& ftrs);

	BOOL DeleteCurVertexs();
	BOOL DeleteCurSegments();
	BOOL BreakByPt();//单点打断

// Generated message map functions
protected:
	//{{AFX_MSG(CDlgDoc)
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnRedo();
	afx_msg void OnUndo();
	afx_msg void OnFileSaveAsxml();
	afx_msg void OnFileOpenxml();
	afx_msg void OnImportCtrlpts();
	afx_msg void OnExportCtrlpts();
	afx_msg void OnFileReferrences();
	afx_msg void OnToolLayermanage();
	afx_msg void OnToolShowSelectLayer();
	afx_msg void OnToolhideSelectLayer();
	afx_msg void OnToolScripts();
	afx_msg void OnToolFtrsmanage();
	afx_msg void OnToolFtrsorganize();
	afx_msg void OnToolSchememanage();
	afx_msg void OnCellDefView();
	afx_msg void OnFileOpendb();
	afx_msg void OnFileOpenfdb();
	afx_msg void OnFileOpenfdbDB();
	afx_msg void OnFileSaveAsdb();
	afx_msg void OnImportDxf();
	afx_msg void OnExportdxf();
	afx_msg void OnWsDatabound();
	afx_msg void OnDataTransform();
	afx_msg void OnUpdateOption();
    afx_msg void OnViewXz();
	afx_msg void OnViewSection();
	afx_msg void OnViewRoad();
	afx_msg void OnOverlayGrid();
	afx_msg void OnShowLayerDir();
	afx_msg void OnUpdateShowLayerDir(CCmdUI* pCmdUI);
	afx_msg void OnWsSetbound();
	afx_msg void OnRecoverZ();
	afx_msg void OnAnchorptAutoset();
	afx_msg void OnUpdateAnchorptAutoset(CCmdUI* pCmdUI);
	afx_msg void OnWorkInputVvt();
	afx_msg void OnPhotoToGround();
	afx_msg void OnBuildTIN();
	afx_msg void OnDestroyTIN();
	afx_msg void OnExportTIN();
	afx_msg void OnImportDgn();
	afx_msg void OnExportDgn();
	afx_msg void OnCreateContours();
	afx_msg void OnCreateContoursFromDEM();
	afx_msg void OnConditionSelect();	
	afx_msg void OnConditionSelectConfig();
	afx_msg void OnFilterSelect();
	afx_msg void OnSelectReverse();
	afx_msg void OnQueryMenu(UINT id);
	afx_msg void OnSaveTimer();
	afx_msg void OnSetDefaultCommand();
	afx_msg void OnUpdateSetDefaultCommand(CCmdUI* pCmdUI);
	afx_msg void OnDummySelectFtrClass();
	afx_msg void OnUpdateDummySelectFtrClass(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowSelectLayer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideSelectLayer(CCmdUI* pCmdUI);
	afx_msg void OnDummySelectLayer();
	afx_msg void OnUpdateDummySelectLayer(CCmdUI* pCmdUI);
	afx_msg void OnDummySelectLayerGroup();
	afx_msg void OnUpdateDummySelectLayerGroup(CCmdUI* pCmdUI);
	afx_msg void OnDummySelectFtrWidth();
	afx_msg void OnDummyModifyFtrColor();
	afx_msg void OnUpdateDummySelectFtrWidth(CCmdUI* pCmdUI);
	afx_msg void OnDummySelectFtrSym();
	afx_msg void OnUpdateDummySelectFtrSym(CCmdUI* pCmdUI);
	afx_msg void OnExportShp();
	afx_msg void OnImportShp();
	afx_msg void OnExportModel();
	afx_msg void OnExportMapStarText();
	afx_msg void OnExportVVT();
	afx_msg void OnExportCas();
	afx_msg void OnTestCreateContourSettings();
	afx_msg void OnBuildDEM();
	afx_msg void OnOverlayDEM();
//	afx_msg void OnSaveDEM();
//	afx_msg void OnUpdateSaveDEM(CCmdUI* pCmdUI);
//	afx_msg void OnRemoveDEM();
	afx_msg void OnOverlayBound();
	afx_msg void OnReferenceDatafile();
	afx_msg void OnResetDatafileBound();
//	afx_msg void OnReferenceDataFileUpdate();
	afx_msg void OnUpdateReferenceDatafile(CCmdUI* pCmdUI);
	afx_msg void OnAnchorptRecord();
	afx_msg void OnAnchorptJump();
	afx_msg void OnRefreshDrawing();
	afx_msg void OnToggleAccuBox();
	afx_msg void OnUpdateToggleAccuBox(CCmdUI* pCmdUI);
	afx_msg void OnToggleRightAng();
	afx_msg void OnUpdateToggleRightAng(CCmdUI* pCmdUI);
	afx_msg void OnDrawAutoClosed();
	afx_msg void OnUpdateDrawAutoClosed(CCmdUI* pCmdUI);
	afx_msg void OnDrawAutoRectify();
	afx_msg void OnUpdateOnDrawAutoRectify(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileProperty();
	afx_msg void OnFileSaveAs();
	afx_msg void OnExecLastCommand();
	afx_msg void OnAccuLockX();
	afx_msg void OnAccuLockY();
	afx_msg void OnAccuLockZ();
	afx_msg void OnAccuLockAngle();
	afx_msg void OnAccuLockDistance();
	afx_msg void OnAccuLockIndex();
	afx_msg void OnAccuCoordSysBase();
	afx_msg void OnAccuLockExtension();
	afx_msg void OnAccuLockParallel();
	afx_msg void OnAccuLockPerp();
	afx_msg void OnAccuUnlockCurDir();
	afx_msg void OnAccuUnlockAllDir();
	afx_msg void OnDeviceOnlyStereo();
	afx_msg void OnUpdateDeviceOnlyStereo(CCmdUI* pCmdUI);
	afx_msg void OnDeviceOnlyCursor();
	afx_msg void OnUpdateDeviceOnlyCursor(CCmdUI* pCmdUI);
	afx_msg void OnDocInfoReload();
	afx_msg void OnSelectNext();
	afx_msg void OnUpdateSelectNext(CCmdUI* pCmdUI);
	afx_msg void OnSinglePhotoStereo();
	afx_msg void OnSaveDEMPointstoDEM();
	afx_msg void OnRemoveDem();
	afx_msg void OnWheelSpeedSet();
	afx_msg void OnMapDecorate();
	afx_msg void OnClearCADBlocks();
	afx_msg void OnImportCADSymbols();
	afx_msg void OnDeleteCADSymbols();
	//}}AFX_MSG
	afx_msg void OnMutiimgview();
	afx_msg void OnShowSnapStatus();
	afx_msg void OnSnapMode(UINT nID);
	afx_msg void OnLoadStereo();
	afx_msg void OnUpdateRecentClass();
	afx_msg void OnDummySelectRecentCmd();
	afx_msg void OnUpdateDummySelectRecentCmd(CCmdUI* pCmdUI);
	afx_msg void OnFileOpeneps();
	afx_msg void OnExporteps();
	afx_msg void OnLayersCheck();
	afx_msg void OnFileOpenE00();
	afx_msg void OnExportE00();
	afx_msg void OnImportJB();
	afx_msg void OnExportJB();
	afx_msg void OnExportASC();
	afx_msg void OnDelOverlapLines();
	afx_msg void OnDelOverlapPtInLines();
	afx_msg void OnDelAllPseudoPoints();
	afx_msg void OnDelDirPseudoPoints();
	afx_msg void OnDelOnePseudoPoint();
	afx_msg void OnRemoveAllSuspend();
	afx_msg void OnStatistics();
	afx_msg void OnStatisticsClosed();
	afx_msg void OnExportFdb();
	afx_msg void OnImportArcgisMdb();
	afx_msg void OnExportArcgisMdb();
	afx_msg void OnSetMapInfo();
	afx_msg void OnCheckPseudoNode();
	afx_msg void OnCheckSuspendPoints();
	afx_msg void OnDelAllSameObjs();
	afx_msg void OnRemoveOneSuspend();
	afx_msg void OnResetAllLayerCmdParams();
	afx_msg void OnSelectLocate();
	afx_msg void OnLayerLocate();
	afx_msg void OnDlgOutputLayerCheck();
	afx_msg void OnDlgSetColor();
	afx_msg void OnDlgAnnoTip();
	afx_msg void OnFillRandColor();
	afx_msg void OnSwitchPoint();
	afx_msg void OnSwitchLine();
	afx_msg void OnSwitchSurface();
	afx_msg void OnSwitchText();
	afx_msg void OnUpdateSwitchPoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwitchLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwitchSurface(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSwitchText(CCmdUI* pCmdUI);
	afx_msg void OnHideFtrsNotGIS();
	afx_msg void OnSelectToChecklist();
	afx_msg void OnUpdateHideFtrsNotGIS(CCmdUI* pCmdUI);
	afx_msg void OnNonLocalLayersRename();

	DECLARE_MESSAGE_MAP()
	//曾进翀2017/8/6

	/**
	* @brief OnSetOsgbExtent 设置倾斜数据范围
	*/
	afx_msg void OnSetOsgbExtent();
	
	/**
	* @brief OnTopView 顶视图
	*/
	afx_msg void OnTopView();

	/**
	* @brief OnLeftView 侧视图
	*/
	afx_msg void OnLeftView();

	/**
	* @brief getOsgbView 获取osgbview指针
	*/
	COsgbView* getOsgbView();
public:
	afx_msg void OnOpenOsgb();
	afx_msg void OnRemoveOsgb();
};


#include "SymbolBase.h"
class CDlgCellDoc : public CDlgDoc
{
	DECLARE_DYNCREATE(CDlgCellDoc)

public:
	
	CDlgCellDoc();
	BOOL IsClosing() { return m_bClosing; }
	BOOL UpdateCell();
	void SetDlgCellEditDef(CDlgCellDefLinetypeView *pCell);
	BOOL ClearAllObject();
	void InitDoc(int nscale);

protected:

	CDlgCellDefLinetypeView *m_pCellEditDef;
	BOOL m_bClosing;

	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCellDoc)
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgDoc)
	afx_msg void OnMovetoCenter();
	afx_msg void OnOverlayGrid();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	
};

class CSelChangedExchanger:public CPermanentExchanger
{	 
	DECLARE_DYNAMIC(CSelChangedExchanger)
public:	
	CSelChangedExchanger(CPermanent *pFtr,CDlgDoc *pDoc, int index=0) : CPermanentExchanger(pFtr,index)
	{
		ASSERT(pDoc!=NULL);
		ASSERT(pFtr!=NULL);
		m_pDlgDoc = pDoc;
		
		if( m_pDlgDoc )
			m_pDlgDoc->BeginBatchUpdate();
	}
	CSelChangedExchanger(CArray<CPermanent*,CPermanent*>& arrPFtr,CDlgDoc *pDoc, int index=0) :CPermanentExchanger(arrPFtr,index)
	{
		ASSERT(pDoc!=NULL);
		m_pDlgDoc = pDoc;
		if( m_pDlgDoc )
			m_pDlgDoc->BeginBatchUpdate();
	}
	virtual ~CSelChangedExchanger(){
		if( m_pDlgDoc )
			m_pDlgDoc->EndBatchUpdate();
	}
	virtual void OnModifyLayer(LPCSTR str);
	virtual void OnModifyLayerGroup(LPCSTR str);
	virtual void OnModifyLinetype(int code);
	virtual void OnModifyNodeWid(int index, float wid);
	virtual void OnModifyFtrWid(float wid);
	virtual void OnModifyFtrColor(COLORREF col); 
	virtual void OnModifyNodeType(int index, short type);
	virtual CPermanentExchanger * Clone();
	virtual BOOL GetProperties(CValueTable& tab);
	void OnModifyProperties(CValueTable& tab );
	virtual void OnModifyTransparency(long transparency);
	virtual void OnModifyFillColor(BOOL bFill, COLORREF col);
	
	virtual BOOL GetXAttributes(CValueTable& tab, BOOL bAll=FALSE);
	virtual void OnModifyXAttributes(CValueTable& tab);
protected:

	BOOL IsActiveDataSource();

private:
	CDlgDoc *m_pDlgDoc;
};

CView *GetActiveView();
CDlgDoc *GetActiveDlgDoc();

class CFtrsHighliteDisplay
{
public:
	CFtrsHighliteDisplay();
	~CFtrsHighliteDisplay();

	void SetDoc(CDlgDoc *pDoc);
	void Add(CFeature *pFtr);
	void Highlite();
	static void ClearHighlite();

	CDlgDoc *m_pDoc;
	CFtrArray m_arrFtrs;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITBASEDOC_H__9448A513_8BBA_4039_AA0E_3B63324F32AD__INCLUDED_)
