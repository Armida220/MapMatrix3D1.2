
#if !defined(DLGCOMMAND__INCLUDED_)
#define DLGCOMMAND__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MapDecorateDlg.h"
#include "Command.h"
#include "Feature.h"
#include "FtrLayer.h "
#include "EditDef.h"
#include "MainFrm.h"
#include "DlgSetSpecialSymbol.h"
#include "DlgSymbolTable.h"
#include "ViewImgPosition.h"
#include "SQLiteAccess.h"
#include <vector>
using namespace std;

#define  PF_RECTIFYWAY                    _T("RectifyWay")
#define  PF_RECTIFYTOL                    _T("RectifyTol")
#define  PF_COMBINEHEIMATCH               _T("CombineHeightMatch")
#define  PF_COMBINESURFACEMODE            _T("CombineSurfaceMode")
#define  PF_COMBINEMODE		              _T("CombineMode")
#define  PF_COMBINEBREAK	              _T("CombineBreak")
#define  PF_EAVESADJUSTWAY                _T("EavesAdjustWay")  
#define  PF_EAVESADJUSTKEEPOLD            _T("EavesAdjustKeepOld")
#define  PF_EAVESADJUSTMWIDTH             _T("EavesAdjustMouseWidth")  
#define  PF_EAVESADJUSTWIDTH			  _T("EavesAdjustWidth")
#define  PF_EAVESADJUSTMHEIGHT            _T("EavesAdjustMouseHeight")  
#define  PF_EAVESADJUSTHEIGHT			  _T("EavesAdjustHeight")
#define  PF_EAVESADJUSTFIXINTERSECT       _T("EavesAdjustFixIntersect") 
#define  PF_EAVESADJUSTNEWLAYERCODE       _T("EavesAdjustNewLayerCode")
#define  PF_EAVESALTERMARKTEXT            _T("EavesAlterMarkText")
#define  PF_MODIFYZONLYCONTOR             _T("OnlyContor")
#define  PF_MODIFYZHEIGHT                 _T("Height")
#define  PF_CHAMFERWAY				      _T("ChamferWay")
#define  PF_LINESERIAL					  _T("Lineserial")
#define  PF_INTZSTEP                      _T("IntZStep")
#define  PF_INTZMAXOFFSET                 _T("MaxOffSet")
#define  PF_CHANGECODE                    _T("ChangeCode")
#define  PF_COMTOL                        _T("CompressToler")
#define  PF_COMMINANGLE                   _T("CompressMinAng")
#define  PF_COMWAY                        _T("CompressWay")
#define  PF_CLOSETOL                      _T("CloseToler")
#define  PF_INTERSECTWAY                  _T("IntersectWay")
#define  PF_INTERSECTFIRST                _T("IntersectFirst")
#define  PF_INTERSECTSECOND               _T("IntersectSecond")
#define  PF_MTRRERAXIS                    _T("MirrorAxis")
#define  PF_COPYROTATEKPOLD               _T("KeepOld")  
#define  PF_REPLACELINEWAY                _T("ReplaceLineWay")
#define  PF_REPLACELINE_WAY                _T("ReplaceLine_Way")
#define  PF_REPLACELINETWOOBJ             _T("ReplaceLineTwoObj") 
#define  PF_REPLACELINEHEIMATCH           _T("ReplaceLineHeiMatch")
#define  PF_REPLACELINESHOWMARK           _T("ReplaceLineShowMark")
#define  PF_REPLACELINESNAPRADIUS         _T("ReplaceLineSnapRadius")
#define  PF_REPLACELINEVECTOR             _T("ReplaceLineVector")
#define  PF_REPLACELINESTEREO             _T("ReplaceLineStereo")
#define  PF_REPLACELINESNAP               _T("ReplaceLineSnap")
#define  PF_VMPEN						  _T("PenVm")
#define  PF_3DPEN						  _T("Pen3D")
#define  PF_DRAWRECTWAY                   _T("DrawRectWay")
#define  PF_TEXT                          _T("DrawTextText")
#define  PF_FONT                          _T("DrawTextFont")
#define  PF_CHARH                         _T("DrawTextCharH")
#define  PF_CHARW                         _T("DrawTextCharW")
#define  PF_CHARI                         _T("DrawTextCharI")
#define  PF_LINEI                         _T("DrawTextLineI")
#define  PF_SHRUG                         _T("DrawTextShrug")
#define  PF_SHRUGA                        _T("DrawTextShrugA")
#define  PF_LAYOUT                        _T("DrawTextLayouT")
#define  PF_ALIGNTYPE					  _T("DrawTextAlignType")//对齐方式
#define  PF_GEOCOVERTYPE				  _T("DrawTextCoverType")//文字覆盖类型
#define  PF_EXTENDDIS					  _T("DrawTextExtendDistance")//文字外扩距离
#define  PF_TABLELINES					  _T("LineNumber")//行数
#define  PF_TABLECOLUMNS				  _T("ColumnNumber")//列数			
#define  PF_TABLESPACE					  _T("TableSpaceWidth")//行宽
#define  PF_TABLECOLUMNSPACE			  _T("TableColumnSpace")//列宽
#define  PF_LAYERNAME					  _T("LayerName")//层名
#define  PF_ALIGNMENT					  _T("Alignment")//字体对齐方式
#define  PF_CREATETYPE					  _T("CreateType")//生成方式
#define  PF_CREATE_SHAPE				  _T("CreateShape")//生成形状
#define  PF_SHAPEWIDTH					  _T("ShapeWidth")//宽度(1/N)
#define  PF_SHAPEHEIGHT					  _T("ShapeHeight")//高度(1/N)
#define  PF_SHAPEDIAMETER				  _T("ShapeDiameter")//直径(1/N)	 

#define  PF_SMOOTHWAY                     _T("SmoothWay")
#define  PF_SMOOTHSELMODE                 _T("SmoothSelmode")
#define  PF_SMOOTHTOLER                   _T("SmoothToler")
#define  PF_TRIMPART                      _T("TrimPart")
#define  PF_TRIMBOUND                     _T("TrimBound")
#define  PF_TRIMTOLER                     _T("TrimToler")
#define  PF_TRIMCLOSELINE				  _T("TrimCloseLine")
#define  PF_TRIMSURFACE 				  _T("TrimSurface")
#define  PF_LINKCONTORMATCHHEI            _T("LinkContourMatchHei")
#define  PF_LINKCONTORCROSSLAYER           _T("LinkContourCrossLayer")
#define  PF_AUTOLINKCONTORLAYERCODE       _T("AutoLinkContourLayID")
#define  PF_AUTOLINKCONTORSCOPE           _T("AutoLinkContourScope")
#define  PF_AUTOLINKCONTORTOLER           _T("AutoLinkContourToler")
#define  PF_INTERWAY                      _T("InterpolateWay")
#define  PF_INTERONLYCONTOUR              _T("InterpolateOnlyContour")
#define  PF_INTERLIMIT		              _T("InterpolateLimit")
#define  PF_INTERSONCODE                  _T("InterpolateSonCode")
#define  PF_INTERSONNUM                   _T("InterpolateSonNum")
#define  PF_INTERRANGE                    _T("InterpolateRange")
#define  PF_INTERRADIUS                   _T("InterpolateRadius")
#define  PF_INTERBASECODE                 _T("InterpolateBaseCode")
#define  PF_CONTOURCODE                   _T("ContourLayCode")
#define  PF_CONTOURTEXTCODE                _T("ContourTextLayCode")
#define  PF_CONTOURDIGITNUM               _T("ContourDigitNum")
#define  PF_POLYGONSIDESNUM               _T("PolygonSidesNum")
#define  PF_BYANGCOLSE                    _T("ByAngClosed")
#define  PF_AUTOATTRIBUTEFIELD            _T("AutoAttributeField")
#define  PF_AUTOATTRIBUTESTART            _T("AutoAttributeStart")
#define  PF_MODIFYATTRIBUTELAYERCODE      _T("ModifyAttributeLayerCode")
#define  PF_MODIFYATTRIBUTEFIELD          _T("ModifyAttributeField")
#define  PF_MODIFYATTRIBUTEVALUE		  _T("ModifyAttributeValue")
#define	 PF_MODIFCONTOURACROSSDCURVE	  _T("ModifyContourCrossDCurve")
#define  PF_CONTOURACROSSDCURVE_KEEPPART _T("ContourCrossDCurveKeepPart")
#define	 PF_MODIFYLAYLAYERNAME			  _T("ModifyLayLayerName")	 
#define  PF_SINGLEPARALLEWIDTHCHANGE      _T("SingleParChangeWidth")
#define  PF_SINGLEPARALLEBASECHANGE       _T("SingleParChangeBase")
#define  PF_TRIMLAYER_REF                 _T("TrimLayerRef")
#define  PF_TRIMLAYER_TRI                 _T("TrimLayerTri")
#define  PF_TRIMLAYER_DES                 _T("TrimLayerDes")
#define  PF_TRIMLAYER_SUR                 _T("TrimLayerSur")
#define  PF_TRIMLAYER_KEEPZ               _T("TrimLayerKeepZ")
#define  PF_INERPOLATEVERTEXS             _T("InterpolateVertexs")
#define  PF_COPYLINELAYER_DES               _T("CopyLineLayerDes")
#define  PF_MAPMATCH_SCOPE				  _T("MapMatchScope")
#define  PF_OPERATION_MODE				  _T("OperationMode")
#define  PF_LAYNAME_BOUNDNAME			  _T("BoundLayerName")
#define  PF_MODIFY_MODE					  _T("ModifyMode")//接线方式  
#define  PF_JOIN_MODE				      _T("JoinMode")
#define  PF_MATCH_COLOR				      _T("MatchColor")
#define  PF_MATCH_LINETYPE				  _T("MatchLineType")
#define  PF_MATCH_LINEWIDTH				  _T("MatchLineWidth")
#define  PF_MATCH_LINEHEI				  _T("MatchLineHei")
#define  PF_MATCH_TOLER					  _T("MatchToler")
#define  PF_MATCH_MAPSELECT				  _T("MatchMapSelect")	//选择图幅
#define  PF_MATCH_MARK					  _T("MatchMark")	
#define  PF_PLACE_MODE				      _T("PlaceMode")
#define  PF_PTLINE_LENGTH				  _T("Length")
#define  PF_CURVETOSURFACE_CLOSEDONLY	  _T("ClosedOnly")
#define  PF_DELCOMMONSIDE_LIMIT			  _T("Limit")
#define  PF_DELCOMMONSIDE_KEEPSIDE		  _T("KeepSide")
#define  PF_EXTEND_MULSEL				  _T("ExtendMulSel")
#define  PF_EXTEND_AUTO					  _T("ExtendAuto")
#define  PF_COPYDOC_WITHPT				  _T("CopyDocWithPt")
#define  PF_CHECKBOUND					  _T("CheckBound")
#define  PF_DIVIDETYPE					  _T("DivideType")
#define  PF_DIVIDENUM					  _T("DivideNum")
#define  PF_DIVIDEDIS					  _T("DivideDis")
#define  PF_RESETFLG					  _T("ResetFlg")
#define  PF_PARALLETOSINGLE				  _T("ParalleToSingle")
#define  PF_HANDLELAYERS                  _T("HandleLayers")
#define  PF_CULVERTREVERSE                _T("CulvertReverse")
#define  PF_LINETYPEREVERSE               _T("LinetypeReverse")

#define PDOC(a)             ((CDlgDoc*)(a))
#define GETCURFTRLAYER(a)   ((CDlgDoc*)(a))->GetDlgDataSource()->GetCurFtrLayer()
#define GETCURSCALE(a)		((CDlgDoc*)(a))->GetDlgDataSource()->GetScale()
#define GETDS(a)               ((CDlgDoc*)(a))->GetDlgDataSource()
#define GETXDS(a)              ((CDlgDoc*)(a))->GetDlgDataSource()->GetXAttributesSource() 
//#define CGEO(a) ((a)->GetGeometry())

BOOL CheckNameForLayerCode(CDlgDataSource *pDS, LPCTSTR name, LPCTSTR pstrCode);

class CPermanentExchanger;

class CDlgDrawPointCommand : public CDrawPointCommand  
{
	DECLARE_DYNCREATE(CDlgDrawPointCommand)
public:
	void OnSonEnd(CProcedure *son);
	virtual void Start();
	void Abort();
	virtual void PtMove(PT_3D &pt);
	CDlgDrawPointCommand();
	virtual ~CDlgDrawPointCommand();
	static CCommand* Create(){
		return new CDlgDrawPointCommand;
	}
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	virtual DrawingInfo GetCurDrawingInfo();

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();

	virtual CProcedure *GetActiveSonProc(int MsgType);
protected:
	void Finish();
	CPermanentExchanger *m_pSel;
};


class CDlgDrawSurfacePointCommand : public CDlgDrawPointCommand  
{
	DECLARE_DYNCREATE(CDlgDrawSurfacePointCommand)
public:
	CDlgDrawSurfacePointCommand();
	virtual ~CDlgDrawSurfacePointCommand();
	static CCommand* Create(){
		return new CDlgDrawSurfacePointCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawSurfacePoint");
	}
	virtual CString Name();
	virtual void Start();
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
protected:
	BOOL m_bCreateFace;
};


class CDlgDrawCurveCommand : public CDrawCurveCommand  
{
	DECLARE_DYNCREATE(CDlgDrawCurveCommand)
public:
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtReset(PT_3D &pt);
	virtual void PtMove(PT_3D &pt);
	void OnSonEnd(CProcedure *son);
	virtual void Start();
	CDlgDrawCurveCommand();
	virtual ~CDlgDrawCurveCommand();
	static CCommand* Create(){
		return new CDlgDrawCurveCommand;
	}
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	virtual DrawingInfo GetCurDrawingInfo();
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	void Finish();
	BOOL IsEndSnapped();
	
	CPermanentExchanger *m_pSel;

	BOOL m_bNeedDrawSymbol;

};


//湖南画线新需求：直角化的方法下，第2～N-1号点用边确定。直角化时，如果闭合了，自动补一个点。
//非直角化的情况下，与原来的画线功能无异；
//此类的实现过程基本等于 CDlgDrawCurveCommand 和 CDrawLinesbyAngCommand的综合
class CDlgDrawCurveCommand_HuNan : public CDlgDrawCurveCommand  
{
	DECLARE_DYNCREATE(CDlgDrawCurveCommand_HuNan)
public:
	CDlgDrawCurveCommand_HuNan();
	virtual ~CDlgDrawCurveCommand_HuNan();
	static CCommand* Create(){
		return new CDlgDrawCurveCommand_HuNan();
	}
	virtual CString AccelStr(){
		return _T("DrawCurveHunan");
	}
	virtual CString Name();
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	virtual void Back();
protected:
	void Finish();
	BOOL SnapRightAngle(PT_3D pt);

	BOOL m_bRightAngleMode;
	int  m_nStep;
};


class CDrawDHeightPointCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawDHeightPointCommand)
public:
	CDrawDHeightPointCommand();
	virtual ~CDrawDHeightPointCommand();
	static CCommand* Create(){
		return new CDrawDHeightPointCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawHeightCommand");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual int  GetCurPenCode();
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	void Finish();
	
protected:
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CPermanentExchanger *m_pSel;
};

class CDlgDrawSurfaceCommand : public CDrawSurfaceCommand  
{
	DECLARE_DYNCREATE(CDlgDrawSurfaceCommand)
public:
	virtual void PtClick(PT_3D &pt, int flag);
	void OnSonEnd(CProcedure *son);
	virtual void Start();
	void Abort();
	CDlgDrawSurfaceCommand(){m_pSel = NULL;};
	virtual ~CDlgDrawSurfaceCommand();
	static CCommand* Create(){
		return new CDlgDrawSurfaceCommand;
	}
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	virtual DrawingInfo GetCurDrawingInfo();
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}	
	void RefreshPropertiesPanel();
protected:
	void Finish();
	CPermanentExchanger *m_pSel;
};

//自动完成面
class CDlgAutoSurfaceCommand : public CDrawCurveCommand
{
	DECLARE_DYNCREATE(CDlgAutoSurfaceCommand)
public:
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtReset(PT_3D &pt);
	virtual void PtMove(PT_3D &pt);
	void OnSonEnd(CProcedure *son);
	virtual void Start();
	CDlgAutoSurfaceCommand();
	virtual ~CDlgAutoSurfaceCommand();
	static CCommand* Create(){
		return new CDlgAutoSurfaceCommand;
	}
	virtual CString Name();
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	virtual DrawingInfo GetCurDrawingInfo();
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
	
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1);
	void RefreshPropertiesPanel();
protected:
	void Finish();
	BOOL IsEndSnapped();

	void Complete(Envelope &e, CFtrLayer *pLayer, CFeature *pCurveFtr);
	
	CPermanentExchanger *m_pSel;
	
	BOOL m_bNeedDrawSymbol;
	int m_nHeightMode;
};

class  CDlgMoveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDlgMoveCommand)
public:
	CDlgMoveCommand();
	static CCommand *Create(){
		return new CDlgMoveCommand;
	}
	virtual CString AccelStr(){
		return _T("Move");
	}
	virtual ~CDlgMoveCommand();
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	virtual void Abort();
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
	
protected:
	PT_3D m_ptMoveStart;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrObjs;
	
	BOOL  m_bKeepOld;
	BOOL  m_bMouseDefine;
	BOOL  m_bOnlyChangeZ;
	
	double m_lfDeltaX, m_lfDeltaY, m_lfDeltaZ;
};

class CDeleteCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CDeleteCommand)
public:	
	CDeleteCommand();
	virtual ~CDeleteCommand();
	static CCommand *Create(){
		return new CDeleteCommand;
	}
	virtual CString AccelStr(){
		return _T("Delete");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
protected:
	

};


#define PF_TEXTCOVER_SAMECOLOR _T("SameColor")
// 文字遮掩
class CTextCoverCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTextCoverCommand)
public:	
	CTextCoverCommand();
	virtual ~CTextCoverCommand();
	static CCommand *Create(){
		return new CTextCoverCommand;
	}
	virtual CString AccelStr(){
		return _T("TextCover");
	}
	virtual CString Name();
	
	virtual void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	// 同色才压盖
	BOOL m_bSameColor;
	
	
};

#define PF_RESERVELEN _T("ReserveLen")
class CModifyCurveLengthCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CModifyCurveLengthCommand)
public:	
	CModifyCurveLengthCommand();
	virtual ~CModifyCurveLengthCommand();
	static CCommand *Create(){
		return new CModifyCurveLengthCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyCurveLength");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:

	// 保留的长度
	double   m_fLength;
	CFeature *m_pSelFtr;
	
	
};

#define PF_INCLUDE_REFDATA _T("IncludeRefData")
class CMovetoLocalCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMovetoLocalCommand)
public:	
	CMovetoLocalCommand();
	virtual ~CMovetoLocalCommand();
	static CCommand *Create(){
		return new CMovetoLocalCommand;
	}
	virtual CString AccelStr(){
		return _T("MovetoLoca");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void MovetoLocal(const FTR_HANDLE* handles,int num);
	CFtrLayer* GetFtrLayerOfObject(CFeature *pFtr, BOOL bIncRef=FALSE);

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:

	BOOL m_bIncludeRefData;	
	BOOL m_bModifiedIncludeRef;
	BOOL m_bOldRef;
	
};

class CFCodeChgAllCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CFCodeChgAllCommand)
public:	
	CFCodeChgAllCommand();
	virtual ~CFCodeChgAllCommand();
	static CCommand *Create(){
		return new CFCodeChgAllCommand;
	}
	virtual CString AccelStr(){
		return _T("FCodeChgAll");
	}
	virtual CString Name();
	
	virtual void Start();
protected:
	void ChangeFCode(const char *code1, const char *code2);
	
	
};
//生成符号样图命令
class CSymbolLayoutCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CSymbolLayoutCommand)
public:	
	CSymbolLayoutCommand();
	virtual ~CSymbolLayoutCommand();
	static CCommand *Create(){
		return new CSymbolLayoutCommand;
	}
	virtual CString AccelStr(){
		return _T("SymbolLayoutCommand");
	}
	virtual CString Name();
	virtual void Finish();
	virtual void Start();
	void Abort();
	void DrawSymbolItem(int cx, int cy, const GrBuffer2d *pBuf, COLORREF col, COLORREF colBak);
	BOOL CreateSymbolItem(CSchemeLayerDefine *pLayer,PT_3D &pt,double cx, double cy,int nLayerNum = 1);//play 为层定义对象 pt为画地物点 cx cy 为矩形框长宽 LayaerNum 为画的地物个数
	void DrawTable(PT_3D &pt,int nTableLine,int nTableColumn,int TableNumber,double nTableWidthSpace, CArray<double,double> &nTableColumnSpace);
	BOOL GetPartColumSpace(CArray<double,double> &nTableSpace,int i,double &Total);
	void DrawSymbolText(PT_3D &pt,CString StrTest,TEXT_SETTINGS0 sTxtSettings,CUndoFtrs &undo);
	BOOL SortSchemeLayerDefine(CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> &arrPLayDefines);
	int FindIndexInSchemeLayerDefine(CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> &arrPLayDefines,__int64 low,__int64 heigh,int &nSize);//从层定义数组中找到编码范围的下标 并且返回下边范围内的层定义个数
protected:
	CDlgSymbolTable m_SymbolTableDlg;
	double m_nScale;
	CFeature *m_pFtrSymbol;
	CFtrLayer *m_pLayer;

	CArray<double,double> m_nTableColumnSpace;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;			//记录点

	
};

class CFCodeChgSelectionCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CFCodeChgSelectionCommand)
public:	
	CFCodeChgSelectionCommand();
	virtual ~CFCodeChgSelectionCommand();
	static CCommand *Create(){
		return new CFCodeChgSelectionCommand;
	}
	virtual CString AccelStr(){
		return _T("FCodeChgSelection");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Abort();
protected:	
};


class CDrawDirPointCommand : public CDrawCommand  
{
public:
	DECLARE_DYNCREATE(CDrawDirPointCommand)
	CDrawDirPointCommand();
	static CCommand *Create(){
		return new CDrawDirPointCommand;
	}
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	virtual CString AccelStr(){
		return _T("DirPoint");
	}
	virtual ~CDrawDirPointCommand();
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual void Start();	
	virtual void Abort();
	virtual int  GetState();
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual int  GetCurPenCode();
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
protected:
	virtual void Finish();
	
	
protected:
	CDrawDirPointProcedure *m_pDrawProc;
	CPermanentExchanger *m_pSel;

	//角度偏移
	float m_fAngleOff;
};


class  CDrawAutoHeightPointCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawAutoHeightPointCommand)
public:
	CDrawAutoHeightPointCommand();
	static CCommand *Create(){
		return new CDrawAutoHeightPointCommand;
	}
	virtual CString AccelStr(){
		return _T("AutoHeightPoint");
	}
	virtual ~CDrawAutoHeightPointCommand();
	virtual CString Name();	
	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtDblClick(PT_3D &pt, int flag);
	virtual void Abort();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	virtual DrawingInfo GetCurDrawingInfo();
	virtual int  GetCurPenCode();

	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	void RefreshPropertiesPanel();
protected:
	virtual void Finish();

	int m_nStep;
	int   m_nBoundType;		//0, 工作区，1,绘制
	double m_lfDX, m_lfDY;
	CPermanentExchanger *m_pSel;

	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;

	CArray<FTR_HANDLE,FTR_HANDLE> m_arrFtrs;
	CArray<PT_3D,PT_3D> m_arrBound;
	Envelope m_eBound;

	// 不在区域内仅限制X方向，在其中则要限制Y
	BOOL m_bPtInArea;

	// 奇数行就向移m_lfDX/2
	BOOL m_bOdd;

	BOOL m_bDrawBound;

//	CFeature *m_pFtr;
};

class CSeparateParallelCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSeparateParallelCommand)
public:
	CSeparateParallelCommand();
	virtual ~CSeparateParallelCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("SeparateParallel");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CSeparateParallelCommand;
	}

	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

};

//画平行线命令
class CDrawParalCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawParalCommand)
public:
	BOOL IsEndSnapped();
	void GetParams(CValueTable& tab);
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	void OnSonEnd(CProcedure *son);
	virtual CString AccelStr(){
		return _T("Parallel");
	}
	virtual void Start();
	CDrawParalCommand();
	virtual ~CDrawParalCommand();
	virtual CString Name();
	static CCommand* Create(){
		return new CDrawParalCommand;
	}
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	CFeature* CreateCenterlineFeature();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual int  GetCurPenCode();
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	virtual BOOL Keyin(LPCTSTR text);
	void RefreshPropertiesPanel();
protected:
	virtual void Finish();
protected:
	CDrawParallelProcedure *m_pDrawProc;
//	CFeature *m_pFtr;
	CPermanentExchanger *m_pSel;
	BOOL m_bToSurface;

	//打散时，辅助线的图层
	CString m_strAssistLayer;
};


//变宽平行道路
class CDrawParalRoadCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawParalRoadCommand)
public:
	void GetParams(CValueTable& tab);
	CDrawParalRoadCommand();
	static CCommand *Create(){
		return new CDrawParalRoadCommand;
	}
	virtual CString AccelStr(){
		return _T("ParallelRoad");
	}
	virtual ~CDrawParalRoadCommand();
	virtual CString Name();
	virtual void Back();
	virtual void Start();
	void PtReset(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);
	//bool GGetParallelLine(PT_3DEX* pts, int Point_Size, double lfWidth, PT_3DEX* pRet);
	virtual void Abort();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);
	virtual int  GetCurPenCode();
	virtual DrawingInfo GetCurDrawingInfo();

	virtual BOOL AddObject(CPFeature pFtr, int layid = -1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr, layid);
	}
	virtual BOOL Keyin(LPCTSTR text);

protected:
	virtual void Finish();

protected:
	bool m_bMark;     //标志是否改变宽度
	bool m_bMark2;     //
	int m_nflag;      
	PT_3D m_lastpt;
	int m_nStep;
	double m_lfcurWidth;
	double m_lflastWidth;
	PT_3D m_DistancePoint;
	CArray<PT_3DEX, PT_3DEX> m_ptsFtr1;
	CArray<PT_3DEX, PT_3DEX> m_ptsFtr2;

	//宽度改变时
	PT_3D m_LastParalPoint;

	//是否打散
	BOOL m_bBreakup;
};




class CDrawDCurveCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawDCurveCommand)
public:
	CDrawDCurveCommand();
	static CCommand *Create(){
		return new CDrawDCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("DCurve");
	}
	virtual ~CDrawDCurveCommand();
	virtual CGeometry *GetCurDrawingObj();
	DrawingInfo GetCurDrawingInfo();
	virtual CString Name();
	virtual void Back();
	virtual void OnSonEnd(CProcedure *son);
	virtual void Start();	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtReset(PT_3D &pt);
	virtual void Abort();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	virtual int  GetCurPenCode();
	virtual CProcedure *GetActiveSonProc(int MsgType);
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
	virtual BOOL Keyin(LPCTSTR text);
private:
	BOOL IsEndSnapped();
protected:
	virtual void Finish();
	virtual void UpdateParams(BOOL bSave);
	
protected:
	CDrawDCurveProcedure *m_pDrawProc;
	CPermanentExchanger *m_pSel;
	BOOL m_bToSurface;
};


//画平行四边形命令类
class CDrawParallelogramCommand : public CDrawDCurveCommand  
{
	DECLARE_DYNCREATE(CDrawParallelogramCommand)
public:
	CDrawParallelogramCommand();
	static CCommand *Create(){
		return new CDrawParallelogramCommand;
	}
	virtual CString AccelStr(){
		return _T("Parallelogram");
	}
	virtual ~CDrawParallelogramCommand();
	virtual CString Name();	
	virtual void Start();
	virtual int  GetCurPenCode();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
};

class CCopyCommand : public CMoveCommand  
{
	DECLARE_DYNCREATE(CCopyCommand)
public:
	CCopyCommand();
	virtual ~CCopyCommand();
	static CCommand* Create()
	{
		return new CCopyCommand;
	}
	virtual CString AccelStr(){
		return _T("Copy");
	}
	virtual void PtMove(PT_3D &pt);
	virtual void PtClick(PT_3D &pt, int flag);
	virtual CString Name();
	
protected:

};

class  CLakeHatchCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CLakeHatchCommand)
public:
	CLakeHatchCommand();
	static CCommand *Create(){
		return new CLakeHatchCommand;
	}
	virtual CString AccelStr(){
		return _T("LakeHatch");
	}
	virtual ~CLakeHatchCommand();
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	virtual void Abort();
	
protected:
	PT_3D m_ptMoveStart;
	CFeature  *m_ObjFtr;
};


typedef struct tag_PtIntersect
{
	PT_3D pt;
	double lfRatio;
	int IntersectFlag;
}PtIntersect;

struct FtrIntersect
{
	FtrIntersect() {}
	FtrIntersect(const FtrIntersect &ftrInter)
	{
		*this = ftrInter;
	}
	FtrIntersect& operator=(const FtrIntersect &ftrInter)
	{
		ftr = ftrInter.ftr;
		pts.Copy(ftrInter.pts);
		return *this;
	}

	FTR_HANDLE ftr;
	CArray<PtIntersect,PtIntersect> pts;
};
// 修剪穿越坡坎等高线
class  CTrimContourAcrossBankCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CTrimContourAcrossBankCommand)
public:
	CTrimContourAcrossBankCommand();
	static CCommand *Create(){
		return new CTrimContourAcrossBankCommand;
	}
	virtual CString AccelStr(){
		return _T("TrimContourAcrossBank");
	}
	virtual ~CTrimContourAcrossBankCommand();
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	virtual void Abort(); 
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);		
protected:
	CString m_strLayerName;//等高线层
	CArray<FTR_HANDLE,FTR_HANDLE>  m_arrObjFtrs;
};


class  CTrimContourAcrossDCurveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CTrimContourAcrossDCurveCommand)
public:
	CTrimContourAcrossDCurveCommand();
	static CCommand *Create(){
		return new CTrimContourAcrossDCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("TrimContourAcrossDCurve");
	}
	virtual ~CTrimContourAcrossDCurveCommand();
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Abort(); 
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	
	BOOL LinesInPloygon(PT_3DEX *ptsLines,int ptsnum,PT_3DEX *ptsPloygon,int ploygonnum );
protected:
	CString m_strLayerName;//等高线层
	CString m_strLayLayerName;//存放打断的图层名
	BOOL m_bKeepDelPart;//是否保留删除部分
	CArray<FTR_HANDLE,FTR_HANDLE>  m_arrObjFtrs;
};

class CCopyDocCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCopyDocCommand)
public:
	CCopyDocCommand();
	virtual ~CCopyDocCommand();
	static CCommand* Create(){
		return new CCopyDocCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyDoc");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Finish();
	void Start();
	virtual CString Name();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	
	
protected:
	// 基点
	BOOL m_bCopyWithPt;
	PT_3D m_pt;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrCopyFtrs;	
};

class CPasteDocCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CPasteDocCommand)
public:
	CPasteDocCommand();
	virtual ~CPasteDocCommand();
	static CCommand* Create(){
		return new CPasteDocCommand;
	}
	virtual CString AccelStr(){
		return _T("Paste");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Finish();
	void Start();
	virtual CString Name();	

	virtual void PtMove(PT_3D &pt);
	
protected:
	// 粘贴到原坐标
	BOOL m_bPasteWithOldCoord;
	CopyData m_pasteData;	
};

class CPasteDocCoordCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CPasteDocCoordCommand)
public:
	CPasteDocCoordCommand();
	virtual ~CPasteDocCoordCommand();
	static CCommand* Create(){
		return new CPasteDocCoordCommand;
	}
	virtual CString AccelStr(){
		return _T("PasteCoord");
	}
	void Abort();
	void Finish();
	void Start();
	virtual CString Name();		
protected:

	CopyData m_pasteData;	
};


#define PF_ROTATE_REFERTYPE	"ReferType"
#define PF_ROTATE_REFERANGLE "ReferAng"
class CRotateCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CRotateCommand)
public:
	CRotateCommand();
	virtual ~CRotateCommand();
	static CCommand* Create(){
		return new CRotateCommand;
	}
	virtual CString AccelStr(){
		return _T("Rotate");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
	
protected:

	
protected:
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;	
	CPtrArray m_ptrObjs;	

	BOOL  m_bKeepOld;
	BOOL  m_bMouseDefineAngle;	
	double m_fAngle;

	// 参照类型：0，直接指定角度；1，线段确定角度；
	int   m_nReferType;
	double  m_fReferAngle, m_fReferAngle0;

	PT_3D m_ptReferStart;
	PT_3D m_ptReferEnd;
};

// 阵列
#define PF_ARRAYTYPE		_T("ArrayType")
#define PF_ARRAYROWNUM	    _T("RowNum")
#define PF_ARRAYCOLNUM	    _T("ColNum")
#define PF_ARRAYROWOFF	    _T("RowOff")
#define PF_ARRAYCOLOFF	    _T("ColOff")
#define PF_ARRAYANGLE	    _T("ArrayAngle")
#define PF_ARRAYCIRCLEMODE	_T("CircleMode")
#define PF_ARRAYITEMNUM 	_T("ItemNum")
#define PF_ARRAYFILLANGLE 	_T("FillAngle")
#define PF_ARRAYITEMANGLE 	_T("ItemAngle")
#define PF_ARRAYROTATEITEM 	_T("RotateItem")
#define PF_ARRAYDEFAULTBASEPT 	_T("DefaultBasePt")
class CModifyArrayCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CModifyArrayCommand)
public:
	CModifyArrayCommand();
	virtual ~CModifyArrayCommand();
	static CCommand* Create(){
		return new CModifyArrayCommand;
	}
	virtual CString AccelStr(){
		return _T("Array");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
protected:
	// 0：矩形阵列，1：环形阵列
	int  m_nType;
	
	// 矩形阵列参数
	int m_nRowNum, m_nColNum;       // 行数，列数
	double m_fRowOff, m_fColOff;    // 行偏移，列偏移
	double m_fArrayAngle;           // 阵列角度
	// 环形阵列参数
	PT_3D   m_ptCenter;             // 中心点
	int		m_nCircleMode;          // 环状确定方式：0，项目总数和填充角度，1，项目总数和项目间的角度，2，填充角度和项目间的角度
	int     m_nItemNum;             // 项目总数
	double  m_fFillAngle;           // 填充角度
	double  m_fItemAngle;           // 项目间角度
	BOOL    m_bRotateItem;          // 复制时旋转项目
	PT_3D   m_BaseptOfItem;         // 基点
	BOOL    m_bDefaultBasePt;       // 基点是否为默认值，缺省为是


	CArray<FTR_HANDLE,FTR_HANDLE> m_arrBaseObjs;
};


//command paralleltosingle 平行线转单线

class CParalleToSingleCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CParalleToSingleCommand)
public:
	CParalleToSingleCommand();
	virtual ~CParalleToSingleCommand();
	virtual CString AccelStr(){
		return _T("ParalleToSingleCommand");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CParalleToSingleCommand;
	}
	
	virtual void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void Process();
	void Abort();

protected:
	float m_fWidLimit; //小于该宽度的，予以转换
	CString m_strLayer0; //待处理的图层；
	CString m_strLayer1; //放置的图层；
};


//平行线转单线(0宽度全图)
class CParalleToCurveCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CParalleToCurveCommand)
public:
	CParalleToCurveCommand();
	virtual ~CParalleToCurveCommand();
	virtual CString AccelStr(){
		return _T("ParalleToCurveCommand");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CParalleToCurveCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void Abort();
};

//command surfacetopoint 面转点

class CSurfaceToPointCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSurfaceToPointCommand)
public:
	CSurfaceToPointCommand();
	virtual ~CSurfaceToPointCommand();
	virtual CString AccelStr(){
		return _T("SurfaceToPoint");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CSurfaceToPointCommand;
	}
	
	virtual void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void Process();
	void Abort();
	
protected:
	float m_fAreaLimit; //小于该面积的，予以转换
	CString m_strLayer0; //待处理的图层；
	CString m_strLayer1; //放置的图层；
};



//command  比高点删除

class CDeleteDHeightCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CDeleteDHeightCommand)
public:
	CDeleteDHeightCommand();
	virtual ~CDeleteDHeightCommand();
	virtual CString AccelStr(){
		return _T("DeleteDHeight");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CDeleteDHeightCommand;
	}
	
	virtual void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void Process();
	void Abort();
	
protected:
	float m_fDZLimit; //小于该高程差的，予以删除
	CString m_strLayer0; //待处理的图层；
};




//command  生成平行线的宽度注记

class CCreateWidthNoteCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCreateWidthNoteCommand)
public:
	enum
	{
		modeAllMap = 0,
		modeSelect = 1
	};
	CCreateWidthNoteCommand();
	virtual ~CCreateWidthNoteCommand();
	virtual CString AccelStr(){
		return _T("CreateWidthNote");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CCreateWidthNoteCommand;
	}
	
	virtual void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void Process();
	void Abort();
protected:
	BOOL ProcessOne(CFeature *pFtr, CUndoFtrs *pUndo);
	
protected:
	int m_nMode;
	CString m_strLayer0; //待处理的图层；
	CString m_strLayer1; //放置的图层；

	TEXT_SETTINGS0 m_sTxtSettings; //字体参数
	int m_nDigitNum; //小数点位数
};

class CReverseCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CReverseCommand)
public:
	CReverseCommand();
	virtual ~CReverseCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("Reverse");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CReverseCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();	
};

class CLinearizeCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CLinearizeCommand)
public:
	CLinearizeCommand();
	virtual ~CLinearizeCommand();
	void Abort();	
	virtual CString AccelStr(){
		return _T("Linearize");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CLinearizeCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();	
};


class CBreakCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CBreakCommand)
public:
	enum
	{
		modeSinglePoint = 0,
		modeDoublePoint = 1,
		modeLinesection = 2
	};
	CBreakCommand();
	virtual ~CBreakCommand();
	static CCommand* Create(){
		return new CBreakCommand;
	}
	virtual CString AccelStr(){
		return _T("Break");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();
	virtual void PtReset(PT_3D &pt);
	
protected:
	void Finish();
protected:
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
	
	int	  m_nMode;//0, 单选； 1， 多选
	
	CGeoCurveBase *m_pObj;
	FTR_HANDLE m_hOldHandle;
	int   m_nPtNum;

	CUndoFtrs m_undo;

	CValueTable m_xTab;
	
};

class CCopyLineCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCopyLineCommand)
public:
	enum
	{
		modeSinglePoint = 0,
			modeDoublePoint = 1,
			modeLinesection = 2
	};
	CCopyLineCommand();
	virtual ~CCopyLineCommand();
	static CCommand* Create(){
		return new CCopyLineCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyLine");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();
	virtual void PtReset(PT_3D &pt);
	
protected:
	void Finish();
protected:
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
		
	CGeoCurveBase *m_pObj;
	FTR_HANDLE m_hOldHandle;
	
	CUndoFtrs m_undo;

	CString m_strRetLay;

	BOOL m_bStartSnapLayerName;
	
};

class CRectifyCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CRectifyCommand)
public:
	enum
	{
		modeAllAngles = 0,
		modeOneAngle = 1	
	};
	static CGeometry *Rectify(CGeometry* pObj, double tolerance, bool bLockStart, bool bLockEnd);
	CRectifyCommand();
	virtual ~CRectifyCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("Rectify");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CRectifyCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

protected:
	int	m_nPtNum;
	CFeature *m_pFtr;
	int  m_nMode;
	double m_lfSigma;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrHandles;
};


class CCombineCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCombineCommand)
public:	

	CCombineCommand();
	virtual ~CCombineCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("Combine");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CCombineCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

protected:
	CFeature *m_pObjOld0;
	CFeature *m_pObjOld1;
	CFeature *m_pObjNew;
	int   m_ptNum0;
	int   m_ptNum1;
	
	BOOL m_bMatchHeight;
	
	BOOL m_bLinkCountor;  //是否连续连接

	// 是否在连接处断开
	BOOL   m_bCombinBreak;
};

// 平行移动
class CParallelMoveCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CParallelMoveCommand)
public:
	enum
	{
		modeAllSides = 0,
		modeOneSide = 1	
	};
	CParallelMoveCommand();
	virtual ~CParallelMoveCommand();
	static CCommand* Create(){
		return new CParallelMoveCommand;
	}
	virtual CString AccelStr(){
		return _T("ParallelMove");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();

protected:
	//根据已知对象和已知的边，得到生成的单边平行对象
	CGeometry *GetOneParaObj(CGeometry *pObj, double wid, BOOL bUsedir, BOOL bModifyZ, double dz, GrBuffer *pBuf=NULL);
	
	//根据已知对象，得到生成的全平行对象
	CGeometry *GetAllParaObj(CGeometry *pObj, double wid, BOOL bUsedir, BOOL bModifyZ, double dz);
	
	
protected:
	int	m_nPtNum;
	CPFeature m_pFtr,m_pSelFtr;
	int  m_nMode;
	BOOL m_bMouseWidth;
	BOOL m_bKeepOld;
	BOOL m_bMouseModifyZ;
	double m_lfWidth;
	double m_lfDeltaZ;
	PT_3D m_ptLine0, m_ptLine1;
	PT_3D m_ptStart;
	BOOL m_bFixIntersect;

	CString m_strLayer;
	
};

// 房檐改正
#define CEA_CMD_ORI_CODE	_T("original_eaves_layer")
class CEavesAdjustCommand : public CParallelMoveCommand  
{
	DECLARE_DYNCREATE(CEavesAdjustCommand)
public:
	CEavesAdjustCommand();
	virtual ~CEavesAdjustCommand();
	static CCommand* Create(){
		return new CEavesAdjustCommand;
	}
	virtual CString AccelStr(){
		return _T("EavesAdjust");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();
	// 标记原始房檐
	BOOL MarkText(PT_3DEX *pts, int num, float wid);
		
protected:
	TEXT_SETTINGS0 m_sTxtSettings;	
	BOOL           m_bMarkText;
	CString        m_strLayerCode;
	int m_nDigitNum;
private:
	BOOL m_bModified;
	CString m_ori_LayerCode;	//不标注记时的原始房檐层码
};

class CPartMoveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CPartMoveCommand)
public:
	CPartMoveCommand();
	virtual ~CPartMoveCommand();
	static CCommand* Create(){
		return new CPartMoveCommand;
	}
	virtual CString AccelStr(){
		return _T("PartMove");
	}

	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);	
	virtual void Start();
	virtual void Abort();
	virtual void Finish();

private:

	PT_3D m_ptMoveStart;
	FTR_HANDLE m_objHandle;
};

class CPlaceOrderCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CPlaceOrderCommand)
public:
	CPlaceOrderCommand();
	virtual ~CPlaceOrderCommand();
	static CCommand* Create(){
		return new CPlaceOrderCommand;
	}
	virtual CString AccelStr(){
		return _T("PlaceOrder");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	CFtrArray m_arrSrcFtrs, m_arrRefFtrs;
	FTR_HANDLE m_objHandle;
	int m_nPlaceMode;  // 0，前置；1，后置，2，置于对象之上；3，置于对象之下；4，将文字前置
};



class CPlaceOrderFrontCommand : public CPlaceOrderCommand
{
	DECLARE_DYNCREATE(CPlaceOrderFrontCommand)
public:
	CPlaceOrderFrontCommand(){}
	virtual ~CPlaceOrderFrontCommand(){}
	static CCommand* Create(){
		return new CPlaceOrderFrontCommand;
	}
	virtual CString AccelStr(){
		return _T("PlaceOrderFront");
	}
	
	virtual CString Name();	
	virtual void Start();	

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
};


class CPlaceOrderBackCommand : public CPlaceOrderCommand
{
	DECLARE_DYNCREATE(CPlaceOrderBackCommand)
public:
	CPlaceOrderBackCommand(){}
	virtual ~CPlaceOrderBackCommand(){}
	static CCommand* Create(){
		return new CPlaceOrderBackCommand;
	}
	virtual CString AccelStr(){
		return _T("PlaceOrderBack");
	}
	
	virtual CString Name();	
	virtual void Start();	

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
};

// 有向点转线段
class CDirPointToLineCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDirPointToLineCommand)
public:
	CDirPointToLineCommand();
	virtual ~CDirPointToLineCommand();
	static CCommand* Create(){
		return new CDirPointToLineCommand;
	}
	virtual CString AccelStr(){
		return _T("DirPointToLine");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	void DirPointToLine(const FTR_HANDLE* handles,int num);
	
private:
	
	double m_fLength;
};

// 线段转有向点
class CLineToDirPointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CLineToDirPointCommand)
public:
	CLineToDirPointCommand();
	virtual ~CLineToDirPointCommand();
	static CCommand* Create(){
		return new CLineToDirPointCommand;
	}
	virtual CString AccelStr(){
		return _T("LineToDirPoint");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void LineToDirPoint(const FTR_HANDLE* handles,int num);
};

// 线段转有向点(图层)
class CLineToDirPoint1Command : public CEditCommand
{
	DECLARE_DYNCREATE(CLineToDirPoint1Command)
public:
	CLineToDirPoint1Command();
	virtual ~CLineToDirPoint1Command();
	static CCommand* Create(){
		return new CLineToDirPoint1Command;
	}
	virtual CString AccelStr(){
		return _T("LineToDirPoint1");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

private:
	CString m_handleLayers;
	CString m_targetLayer;
	int m_layerID;
};



// 面域点转点
class CSurfacePointToPointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CSurfacePointToPointCommand)
public:
	CSurfacePointToPointCommand();
	virtual ~CSurfacePointToPointCommand();
	static CCommand* Create(){
		return new CSurfacePointToPointCommand;
	}
	virtual CString AccelStr(){
		return _T("SurfacePointToPoint");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
private:
	CString m_handleLayers;
	CString m_targetLayer;
	int m_layerID;
};


// 点转面域点
class CPointToSurfacePointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CPointToSurfacePointCommand)
public:
	CPointToSurfacePointCommand();
	virtual ~CPointToSurfacePointCommand();
	static CCommand* Create(){
		return new CPointToSurfacePointCommand;
	}
	virtual CString AccelStr(){
		return _T("PointToSurfacePoint");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
private:
	CString m_handleLayers;
	CString m_targetLayer;
};

// 文字转面域点
class CTextToSurfacePointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CTextToSurfacePointCommand)
public:
	CTextToSurfacePointCommand();
	virtual ~CTextToSurfacePointCommand();
	static CCommand* Create(){
		return new CTextToSurfacePointCommand;
	}
	virtual CString AccelStr(){
		return _T("TextToSurfacePoint");
	}

	virtual CString Name();

	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

private:
	CString m_handleLayers;
	CString m_targetLayer;
};

// 注记转面域点(对照表)
class CAnnoToSurfacePointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CAnnoToSurfacePointCommand)
public:
	CAnnoToSurfacePointCommand();
	virtual ~CAnnoToSurfacePointCommand();
	static CCommand* Create(){
		return new CAnnoToSurfacePointCommand;
	}
	virtual CString AccelStr(){
		return _T("AnnoToSurfacePoint");
	}

	virtual CString Name();

	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

private:
	CString m_strcode;
};

// 面转为线
class CModifySurfaceToCurveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CModifySurfaceToCurveCommand)
public:
	CModifySurfaceToCurveCommand();
	virtual ~CModifySurfaceToCurveCommand();
	static CCommand* Create(){
		return new CModifySurfaceToCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("SurfaceToCurve");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void SurfaceToCurve(const FTR_HANDLE* handles,int num);
	
private:
	
};

// 线转为面
class CModifyCurveToSurfaceCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CModifyCurveToSurfaceCommand)
public:
	CModifyCurveToSurfaceCommand();
	virtual ~CModifyCurveToSurfaceCommand();
	static CCommand* Create(){
		return new CModifyCurveToSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("CurveToSurface");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void CurveToSurface(const FTR_HANDLE* handles,int num);
	
private:
	
	// 是否仅处理闭合线
	BOOL  m_bClosedCurveOnly;
};

class CResetBaseAttributionCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CResetBaseAttributionCommand)
public:
	enum
	{
		DefaultFeature = 0,
		LayerCfgScheme = 1
			
	};

	CResetBaseAttributionCommand();
	virtual ~CResetBaseAttributionCommand();
	static CCommand* Create(){
		return new CResetBaseAttributionCommand;
	}
	virtual CString AccelStr(){
		return _T("ResetBaseAttribution");
	}
	
	virtual CString Name();
	virtual void Abort();	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	
	void ResetBaseAttributions(const FTR_HANDLE* handles,int num);
	
private:
	int m_nResetFlg;//重置方案 0 缺省特征配置 1 层方案配置


};


class CDeleteCommonSideCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDeleteCommonSideCommand)
public:
	struct CommonSide
	{
		CFeature *pFtr;
		double t1,t2;
	};
	struct IntersectItem
	{
		IntersectItem()
		{
			pFtr = NULL;
			ptIdx = -1;
			t = 0;
		}
		CFeature *pFtr;	
		PT_3D pt;	 //交点坐标
		int ptIdx;   //交点的位置（排序时用）
		double t;    //交点所在线段的比例位置，排序用
	};
	typedef CArray<IntersectItem,IntersectItem> IntersectItemArray;
public:
	CDeleteCommonSideCommand();
	virtual ~CDeleteCommonSideCommand();
	static CCommand* Create(){
		return new CDeleteCommonSideCommand;
	}
	virtual CString AccelStr(){
		return _T("DeleteCommonSide");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);	
	virtual void Start();

	
	void GetIntersects(CFeature* f1,CFeature* f2,IntersectItemArray &arr);
private:
	PT_3D m_ptLine[2];
	// 保留公共边
	BOOL   m_bKeepCommonSide;
	// 容差（距离）
	double m_lfLimit;
};

class CModifyZCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CModifyZCommand)
public:
	CModifyZCommand();
	virtual ~CModifyZCommand();
	void Abort();

	virtual CString AccelStr(){
		return _T("ModifyZ");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CModifyZCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	static BOOL CheckObjForContour(CGeometry *pObj);

protected:
	BOOL m_bOnlyContour;
	double m_lfResultZ;

	BOOL m_bStartSnapHeight;
	
};


class CModifyColorCommand : public CEditCommand
{
   DECLARE_DYNCREATE(CModifyColorCommand)
public:
	CModifyColorCommand();
	virtual ~CModifyColorCommand();
	void Abort();
	
	virtual CString AccelStr(){
		return _T("ModifyColor");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CModifyColorCommand();
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Finish();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
private:
	COLORREF m_nColor;
};


class CModifyIntersectionCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CModifyIntersectionCommand)
public:	
	CModifyIntersectionCommand();
	virtual ~CModifyIntersectionCommand();
	static CCommand* Create(){
		return new CModifyIntersectionCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyIntersection");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	
	
protected:
	bool IsInRange(PT_3DEX *pts,int nums,PT_3D pt1,PT_3D pt2);
	
	PT_3D m_start;
	PT_3D m_end;
};

class CChamferCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CChamferCommand)
public:
	enum
	{
		modeArcChamfer = 0,
		modePolylineChamfer = 1		
	};
	CChamferCommand();
	virtual ~CChamferCommand();
	static CCommand* Create(){
		return new CChamferCommand;
	}
	virtual CString AccelStr(){
		return _T("Chamfer");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();

protected:
	void Finish();
private:
/**************************************
判断点所在的区域
在这儿区域被分成4个部分。返回值分别用1 ，2，3，4来表示
当返回值是3或4时时，计算出两切点（圆弧倒角）或转折点（折线倒角）。

***************************************/
	
	int GPtinRegion(LINE_2D l1,LINE_2D l2,PT_2D &pt,PT_2D *ret1,PT_2D *ret2,double *t1,double *t2,int mode);
	
	
protected:
	int	  m_nMode;//0, 圆弧； 1， 折线
	BOOL  m_bLineserial;
	LINE_2D m_lineseg[2];
	FTR_HANDLE	m_pOldHandles[2];
	CPFeature	m_pNewFtrs[2];
//	OBJ_GUID	m_objID[2];
	
};

class CIntZCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CIntZCommand)
public:
	CIntZCommand();
	virtual ~CIntZCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("IntZ");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CIntZCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	BOOL m_bOnlyContour;
	double m_lfLimitDZ;
	float m_fUnitZ;
	
};

class CConnectCodeChgCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CConnectCodeChgCommand)
public:
	CConnectCodeChgCommand();
	virtual ~CConnectCodeChgCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("ConnectCodeChg");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CConnectCodeChgCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	long m_nConnectCode;
	
};


class CCompressCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCompressCommand)
public:
	enum
	{
		modeWhole = 0,
		modePart = 1
	};
	CCompressCommand();
	virtual ~CCompressCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("Compress");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CCompressCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
    void PtMove(PT_3D &pt);	
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
protected:
	virtual CGeometry* Compress(CGeometry *pObj);
	virtual CGeometry* CompressSimple(CGeometry *pObj);
	CGeometry* CompressSimpleCore(CGeometry *pObj);
protected:
	double m_lfLimit;
	double m_lfAngLimit;
	int m_nPart;
	PT_3D m_ptDragStart;
	
};



class CCompressRedundantCommand : public CCompressCommand  
{
	DECLARE_DYNCREATE(CCompressRedundantCommand)
public:
	enum
	{
		modeWhole = 0,
		modePart = 1
	};
	CCompressRedundantCommand();
	virtual ~CCompressRedundantCommand();
	virtual CString AccelStr(){
		return _T("CompressRedundant");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CCompressRedundantCommand;
	}
	
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
protected:
	virtual CGeometry* CompressSimple(CGeometry *pObj);
	
};

class CCloseCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCloseCommand)
public:
	CCloseCommand();
	virtual ~CCloseCommand();
	static CCommand *Create(){
		return new CCloseCommand;
	}
	virtual CString AccelStr(){
		return _T("Close");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	//virtual void Abort();
protected:
	void Finish();
	BOOL m_bClose;
	
};

class CCloseCommandWithTole : public CEditCommand  
{
	DECLARE_DYNCREATE(CCloseCommandWithTole)
public:	
	CCloseCommandWithTole();
	virtual ~CCloseCommandWithTole();
	static CCommand *Create(){
		return new CCloseCommandWithTole;
	}
	virtual CString AccelStr(){
		return _T("CloseWithTole");
	}
	virtual CString Name();	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	//virtual void Abort();
protected:
	void Finish();
	double m_lfTorlerance;
	
};

// 内插高程点
class CInterpolatePointZCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CInterpolatePointZCommand)
public:
	CInterpolatePointZCommand();
	virtual ~CInterpolatePointZCommand();
	static CCommand *Create(){
		return new CInterpolatePointZCommand;
	}
	virtual CString AccelStr(){
		return _T("InterpolatePointZ");
	}
	virtual CString Name();
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	void Finish();
	FTR_HANDLE	m_objHandles[2];
	CString        m_strLayerCode;
	
};

// 内插高程点
class CInterpolatePointZCommand_NoSelect : public CEditCommand  
{
	DECLARE_DYNCREATE(CInterpolatePointZCommand_NoSelect)
public:
	CInterpolatePointZCommand_NoSelect();
	virtual ~CInterpolatePointZCommand_NoSelect();
	static CCommand *Create(){
		return new CInterpolatePointZCommand_NoSelect;
	}
	virtual CString AccelStr(){
		return _T("InterpolatePointZ_NoSelect");
	}
	virtual CString Name();
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	void Finish();
	BOOL Do(PT_3D pt);
	BOOL Do_TIN(PT_3D pt);

	CString        m_strLayerPt;
	CString        m_strLayerContour;	
};


// 等分线生成点符号
class CDivideCurveCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CDivideCurveCommand)
public:
	CDivideCurveCommand();
	virtual ~CDivideCurveCommand();
	static CCommand *Create(){
		return new CDivideCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("DivideCurve");
	}
	virtual CString Name();
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	void Finish();

	// 0:定数等分；1:等距等分
	int         m_nMode;
	int         m_nDivideNum;
	float       m_fDivideDis;
	FTR_HANDLE	m_objHandles;
	CString        m_strLayerCode;
	
};

// 曲线相交于交点
class CIntersectCurveCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CIntersectCurveCommand)
public:
	CIntersectCurveCommand();
	virtual ~CIntersectCurveCommand();
	static CCommand *Create(){
		return new CIntersectCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("IntersectCurve");
	}
	virtual CString Name();
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	void Finish();
	// 交点处是否断开
	BOOL        m_bIntersectptBreak;
	FTR_HANDLE	m_objHandles[2];

	PT_3D       m_arrPts[2];
	
};

// 延伸到图廓
class CCurveToBoundCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCurveToBoundCommand)
public:
	
	struct BoundScope//边界线的一小块区域
	{
		PT_3D pt0;
		PT_3D pt1;
		Envelope env;//边界区域的外包
		BoundScope *next;
		
		BoundScope()
		{
			next = NULL;
		}

		void AddItem(BoundScope *pNewItem)
		{
			BoundScope *pItem = this;
			while(pItem->next)
			{
				PT_3D cen;
				cen.x = pItem->env.CenterX();
				cen.y = pItem->env.CenterY();
				if(pNewItem->env.bPtIn(&cen))
				{
					delete pNewItem;
					return;
				}
				pItem = pItem->next;
			}
			pItem->next = pNewItem;
		}
		
		void CreateOrAddFromEnv(Envelope e, double  toler)//用图廓外包创建四个小的区域
		{
			Envelope e0(e.m_xl-toler, e.m_xl+toler, e.m_yl, e.m_yh, e.m_zl, e.m_zh);//左侧
			Envelope e1(e.m_xl, e.m_xh, e.m_yl-toler, e.m_yl+toler, e.m_zl, e.m_zh);//底侧
			Envelope e2(e.m_xh-toler, e.m_xh+toler, e.m_yl, e.m_yh, e.m_zl, e.m_zh);//右侧
			Envelope e3(e.m_xl, e.m_xh, e.m_yh-toler, e.m_yh+toler, e.m_zl, e.m_zh);//顶侧

			if(next==NULL)//Create
			{
				pt0 = PT_3D(e.m_xl, e.m_yl, 0);
				pt1 = PT_3D(e.m_xl, e.m_yh, 0);
				env = e0;
				next = NULL;
			}
			else //Add
			{
				BoundScope *pItem = new BoundScope;
				pItem->pt0 = PT_3D(e.m_xl, e.m_yl, 0);
				pItem->pt1 = PT_3D(e.m_xl, e.m_yh, 0);
				pItem->env = e0;
				pItem->next = NULL;
				AddItem(pItem);
			}
			
			
			BoundScope *pItem = new BoundScope;
			pItem->pt0 = PT_3D(e.m_xl, e.m_yl, 0);
			pItem->pt1 = PT_3D(e.m_xh, e.m_yl, 0);
			pItem->env = e1;
			pItem->next = NULL;
			AddItem(pItem);
			
			pItem = new BoundScope;
			pItem->pt0 = PT_3D(e.m_xh, e.m_yl, 0);
			pItem->pt1 = PT_3D(e.m_xh, e.m_yh, 0);
			pItem->env = e2;
			pItem->next = NULL;
			AddItem(pItem);
			
			pItem = new BoundScope;
			pItem->pt0 = PT_3D(e.m_xl, e.m_yh, 0);
			pItem->pt1 = PT_3D(e.m_xh, e.m_yh, 0);
			pItem->env = e3;
			pItem->next = NULL;
			AddItem(pItem);
		}
	};
public:	
	CCurveToBoundCommand();
	virtual ~CCurveToBoundCommand();
	static CCommand* Create(){
		return new CCurveToBoundCommand;
	}
	virtual CString AccelStr(){
		return _T("CurveToBound");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	//virtual void OnSonEnd(CProcedure *son);
	//virtual CProcedure *GetActiveSonProc(int nMsgType);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();

private:
	void CurveToBound(CFeature* pFeature, CUndoModifyProperties& undo);

	BOOL m_bSnap3D;
	CString m_strLayer;
	double  m_lfToler;
	CString m_strLayNameOfTKName;
	BoundScope *m_boundScope;
};

// 内插交点
class CIntersectCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CIntersectCommand)
public:
	CIntersectCommand();
	virtual ~CIntersectCommand();
	static CCommand *Create(){
		return new CIntersectCommand;
	}
	virtual CString AccelStr(){
		return _T("Intersect");
	}
	virtual CString Name();
	void Abort();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	//virtual void Abort();
protected:
	void Finish();
	int m_nMode;
	CFeature	*m_pFtrs[2];
	FTR_HANDLE	m_objHandles[2];
	
};

class CLayerIntersectCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CLayerIntersectCommand)
		typedef struct tag_ObjItem
	{
		tag_ObjItem(CFeature *pFt = NULL)
		{
			pFtr = pFt;
			flag1 = FALSE;
			flag2 = FALSE;
		}
		CFeature *pFtr;
		BOOL flag1;
		BOOL flag2;
	}ObjItem;	
public:
	CLayerIntersectCommand();
	virtual ~CLayerIntersectCommand();
	virtual CString Name();
	static CCommand* Create();	

	virtual CString AccelStr(){
		return _T("LayerIntersect");
	}

	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

private:
	BOOL PreProcess1(CFeature *pFtr,CArray<ObjItem,ObjItem&> &arrItem);
	BOOL PreProcess2(PT_3D pt1, PT_3D pt2, CArray<ObjItem,ObjItem&> &arrItem);

	BOOL Process(CFtrArray& backupFtrs, CFtrArray& modifiedFtrs);

private:	
	int m_nMode;
	CString m_strFIDs;
	CString m_strFID1,m_strFID2;
};

class CDelAllSameCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CDelAllSameCommand)
public:

	CDelAllSameCommand();
	virtual ~CDelAllSameCommand();
	static CCommand *Create(){
		return new CDelAllSameCommand;
	}
	virtual CString AccelStr(){
		return _T("DelAllSame");
	}
	virtual CString Name();	

	virtual void Start();
	static BOOL IsWholeOverlapedObj(CDataSourceEx *pDS, CFeature *pObj1, CFeature *pObj2, BOOL bCheckLayName, int mode=0);

public:
	CString m_strLayers;
	BOOL m_bCheckLayName;
	int m_nMode;//0, 节点一致， 1 图形一致

protected:
	void Process();
	int  SelectKeepedOne(CArray<CFeature*,CFeature*>& arrObjs);
};



struct SurfaceTrimPt2
{
	SurfaceTrimPt2(){
		type = 0;
		no1 = no2 =0;
		t = 0;
	}
	PT_3D pt;	//点的坐标	
	int type;	//为0，表示是基线点，为1，则表示是交点
	int no1,no2;//如果type为-1，no1对象基线的索引号，no2为范围线的索引号
				//如果type为1，no1对象的总点列的索引号，no2为范围线的总点列的索引号
	double t;	//t是记录的此点在线段上的位置参数
};

class CXORSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CXORSurfaceCommand)
public:	
	CXORSurfaceCommand();
	virtual ~CXORSurfaceCommand();
	static CCommand* Create(){
		return new CXORSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("XORSurface");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	
protected:
	BOOL XORSurface(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arr);
	BOOL XORSurface_BothSimple(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arr);
	BOOL XORSurface_Muti_Simple(CGeoMultiSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arr);
};

class CSplitSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSplitSurfaceCommand)
public:	
	CSplitSurfaceCommand();
	virtual ~CSplitSurfaceCommand();
	static CCommand* Create(){
		return new CSplitSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("SplitSurface");
	}
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	void PtReset(PT_3D &pt);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Finish();
protected:
	void Split(CFeature *pFtr, CGeoArray& curves, CUndoFtrs& undo);

protected:
	CDrawCurveProcedure *m_pDrawProc;
	CGeoCurve *m_pGeoCurve;
	int m_nOperMode;//0, 选择，1,绘制
	BOOL m_bKeepSplitLine;
	CString m_strStoreLayer;
	BOOL m_bPlaneFirst;
	BOOL m_bKeepSrcVertexElevtion;
};

class CMergeSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMergeSurfaceCommand)
public:	
	CMergeSurfaceCommand();
	virtual ~CMergeSurfaceCommand();
	static CCommand* Create(){
		return new CMergeSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("MergeSurface");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:	
	BOOL FindSameEdgeObj(CArray<PT_3DEX,PT_3DEX>& arrPts, CFtrArray& arrFtrs);
	CGeoSurface* MergeSurface(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs);
	CGeoSurface* MergeSurface_new(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs);
	CGeoSurface* MergeSurface_BothSimple(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& commonsideFtrs);
	CGeoSurface* MergeSurface_BothMulti(CGeoMultiSurface *pObj1, CGeoMultiSurface *pObj2, CFtrArray& commonsideFtrs);

	//使用逻辑运算进行合并，较严密，但是无法获取公共边
	CGeoSurface *MergeSurface_BothMulti_Logic(CGeoMultiSurface *pObj1, CGeoMultiSurface *pObj2);
	
	//多边形区域并非共边，而是有重叠时，采用多边形的逻辑并的方法处理
	BOOL MergeSurface_BothSimple_LogicUnion(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arrRets);

	//多边形的逻辑减；第一个减第二个
	BOOL MergeSurface_BothSimple_LogicSubtract(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arrRets);

	//多边形的交集；
	BOOL MergeSurface_BothSimple_LogicIntersect(CGeoSurface *pObj1, CGeoSurface *pObj2, CArray<CGeoSurface*,CGeoSurface*>& arrRets);

	void CalcSurfaceFlags(CArray<CGeoSurface*,CGeoSurface*>& arrObjs, CArray<int,int>& flags);

	BOOL FindSameEdgeObj(CGeoSurface *pObj1, CGeoSurface *pObj2, CFtrArray& arrFtrs);

protected:
    PT_3D m_ptDragStart;

	//公共边所在图层
	CString m_strLayerCommonSide;
};



class CMergeSurfaceCommand_AllMap : public CMergeSurfaceCommand  
{
	DECLARE_DYNCREATE(CMergeSurfaceCommand_AllMap)
public:	
	CMergeSurfaceCommand_AllMap();
	virtual ~CMergeSurfaceCommand_AllMap();
	static CCommand* Create(){
		return new CMergeSurfaceCommand_AllMap;
	}
	virtual CString AccelStr(){
		return _T("MergeSurface_AllMap");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:	
	
	//属性值相等的条件限定
	CString m_strEqualFields;
};

#define PF_MERGECURVE_TYPE _T("DCurveType")
class CMergeCurveCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMergeCurveCommand)
public:	
	CMergeCurveCommand();
	virtual ~CMergeCurveCommand();
	static CCommand* Create(){
		return new CMergeCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("MergeCurve");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

protected:
    FTR_HANDLE m_arrFtrs[2];
	// 0：平行线；1：双线
	int    m_nType;
};

class CMirrorCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMirrorCommand)
public:
	enum
	{
		modeDraw = 0,
		modeSelect = 1
	};
	CMirrorCommand();
	virtual ~CMirrorCommand();
	static CCommand* Create(){
		return new CMirrorCommand;
	}
	virtual CString AccelStr(){
		return _T("Mirror");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();

private:
	CGeometry *MirrorObj(CGeometry *pObj, PT_3D pt0, PT_3D pt1);
protected:
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
	
	CPtrArray m_ptrObjs;
	int m_nGetAxisMode;
	
};


class CCopyWithRotationCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCopyWithRotationCommand)
public:
	CCopyWithRotationCommand();
	virtual ~CCopyWithRotationCommand();
	static CCommand* Create(){
		return new CCopyWithRotationCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyWithRotation");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
		
protected:
	PT_3D m_ptKey;
	PT_3D m_ptNew1, m_ptNew2;
	void Finish();
	FTR_HANDLE	m_objHandleOld;
	
	BOOL  m_bKeepOld;
	
};

class CExtendCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CExtendCommand)
public:	
	CExtendCommand();
	virtual ~CExtendCommand();
	static CCommand* Create(){
		return new CExtendCommand;
	}
	virtual CString AccelStr(){
		return _T("Extend");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
private:
	void GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, CArray<double,double> &ret, CArray<double,double> &ret_z);
	CArray<FTR_HANDLE,FTR_HANDLE> m_idsRefer;
	PT_3D m_ptOld, m_ptNew;

	// 支持多选
	BOOL  m_bMulSelMode, m_bAutoExtend;
	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;

	BOOL m_bSnap3D;
};

// 修剪
#define  PF_KEEPCUTPART				  _T("KeepCutPart")
#define  PF_CUTPARTSURFACE            _T("CutpartSurface")
class CCutPartCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCutPartCommand)
public:	
	CCutPartCommand();
	virtual ~CCutPartCommand();
	static CCommand* Create(){
		return new CCutPartCommand;
	}
	virtual CString AccelStr(){
		return _T("CutPart");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);

	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
private:
	BOOL IsSelfIntersect(CGeometry *pObj);//判断是否自相交，如果是，求出所有交点
	void BreakLine(PT_3DEX pt1, PT_3DEX pt2, CGeoCurve* pCurve, CGeometry*& pObj0, CGeometry*& pObj1, CGeometry*& pObj2);

	void GetIntersectPoints(CGeometry *pObj, CArray<double,double> &ret);
	void UpdateFtrHiliteDisplay();

	CArray<FTR_HANDLE,FTR_HANDLE> m_idsRefer;

	// 支持多选
	BOOL  m_bMulSelMode;
	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;

	BOOL m_bKeepCutPart;

	// 按照线的方式修剪面
	BOOL  m_bCutpartSurface;

};

class CReplaceLinesCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CReplaceLinesCommand)
public:
	enum
	{
		modeTwoPoint = 0,
		modeManyPoint = 1
	};
	CReplaceLinesCommand();
	virtual ~CReplaceLinesCommand();
	static CCommand* Create(){
		return new CReplaceLinesCommand;
	}
	virtual CString AccelStr(){
		return _T("ReplaceLines");
	}

	void OnSonEnd(CProcedure *son);
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void PtReset(PT_3D &pt);
	void Finish();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual int  GetCurPenCode();
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual CProcedure *GetActiveSonProc(int nMsgType);

protected:
	virtual CGeometry * GetReplaceObj();
	virtual CGeometry * GetReplaceObj_needSelect();
	virtual CGeometry * GetReplaceObj_same();
	virtual CGeometry * GetReplaceObj_needSelect_same();
	void DrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf);
protected:
	CDrawCurveProcedure *m_pDrawProc;
	
protected:	
	int m_nMode;
	int m_nVWPen, m_n3DPen;
	BOOL m_bShowMark, m_nWaitSelect;
	BOOL m_bVWsnaplinestyle;         //标志矢量窗口吸取线型
	BOOL m_b3Dsnaplinestyle;
	int m_nStep;
	int m_nStart, m_nEnd;
	PT_3D m_ptSel1, m_ptSel2, m_ptStart, m_ptEnd;
	FTR_HANDLE m_objOldID1, m_objOldID2;
//	FTR_HANDLE m_objNewID;
	
	CFeature *m_pOldObj1, *m_pOldObj2;
	CGeometry *m_pObj;
	BOOL m_bCanTwoObjs;
	BOOL m_bMatchHeight;
	
	BOOL m_bUseViewParam;
	int m_nSaveViewParam;
	
	long m_nSnapRadius;

	BOOL m_bExtendCollect;
};


//处理多个对象的共线部分的修测，同时修改过个对象
class CReplaceLinesCommand_overlap : public CReplaceLinesCommand  
{
	DECLARE_DYNCREATE(CReplaceLinesCommand_overlap)
public:
	CReplaceLinesCommand_overlap();
	virtual ~CReplaceLinesCommand_overlap();
	static CCommand* Create(){
		return new CReplaceLinesCommand_overlap;
	}
	virtual CString AccelStr(){
		return _T("ReplaceLines_overlap");
	}
	virtual CString Name();

	void OnSonEnd(CProcedure *son);
	void Start();
	void GetParams(CValueTable& tab);
	void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
protected:	

};


class CExtendCollectCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CExtendCollectCommand)
public:
	CExtendCollectCommand();
	virtual ~CExtendCollectCommand();
	static CCommand* Create(){
		return new CExtendCollectCommand;
	}
	virtual CString AccelStr(){
		return _T("ExtendCollect");
	}

	virtual void PtClick(PT_3D &pt, int flag);
 	void PtMove(PT_3D &pt);	
	void Abort();
	void PtReset(PT_3D &pt);
	void Finish();
	virtual CString Name();
	virtual void Back();
	virtual int  GetCurPenCode();
	virtual DrawingInfo GetCurDrawingInfo(); 
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Start();
protected:
	void Connect();
protected:
	int m_nStep;
	CDrawCurveProcedure *m_pDrawProc;
	CFeature *m_pOldFtr;
	BOOL m_bReverse;
	CUndoFtrs *pUndo;
};

class CReplaceLinesWithCatchIntersecCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CReplaceLinesWithCatchIntersecCommand)
public:
	CReplaceLinesWithCatchIntersecCommand();
	virtual ~CReplaceLinesWithCatchIntersecCommand();
	virtual DrawingInfo GetCurDrawingInfo(); 
	static CCommand* Create(){
		return new CReplaceLinesWithCatchIntersecCommand;
	}
	virtual CString AccelStr(){
		return _T("ReplaceLinesWithCatchIntersec");
	}
	
	void OnSonEnd(CProcedure *son);
	void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void PtReset(PT_3D &pt);
	void Finish();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual int  GetCurPenCode();

	virtual CProcedure *GetActiveSonProc(int nMsgType);

protected:
	bool FindPointOfSameZInObject(CGeometry *pObj,double r,double z,PT_3D pt,PT_3D *ret);
	CGeometry * GetReplaceObj();//单独使用，在PtReset()中
	CGeometry * GetReplaceObj_same();//不单独使用，仅仅供GetReplaceObj()调用
	void DrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf);


	
protected:
	CDrawCurveProcedure *m_pDrawProc;

	BOOL m_bVWsnaplinestyle;         //标志矢量窗口吸取线型
	BOOL m_b3Dsnaplinestyle;

	BOOL m_bShowMark;
	bool m_bSnap;       //标志穿越采集时是否吸取
	int m_nflag;        //标志穿越采集时是否已有吸取点 PtClick中据此决定是画一般点还是穿越点
	PT_3D m_pt;         //保存穿越采集时的吸取点（在PtMove中对其在必要时更新）
	int m_nStep;
	int m_nStart, m_nEnd;
	PT_3D  m_ptStart, m_ptEnd;
	FTR_HANDLE m_objOldID1, m_objOldID2;	
	CFeature *m_pOldObj1, *m_pOldObj2;
	//OBJ_GUID m_objNewID;
	CGeometry *m_pObj;
	
	
	BOOL m_bUseViewParam;
	int m_nSaveViewParam;
	
	long m_nSnapRadius;  //吸取半径
};


class CDrawRectCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawRectCommand)
public:
	enum
	{
		modeHoriz = 0,
		modeCustom = 1
	};
	CDrawRectCommand();
	virtual ~CDrawRectCommand();
	static CCommand* Create(){
		return new CDrawRectCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawRect");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual int  GetCurPenCode();
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	void Finish();
	
protected:
	int m_nMode;
//	CFeature *m_pFtr;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CPermanentExchanger *m_pSel;
};

class CDrawRegularPolygonCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawRegularPolygonCommand)
public:
	CDrawRegularPolygonCommand();
	virtual ~CDrawRegularPolygonCommand();
	static CCommand* Create(){
		return new CDrawRegularPolygonCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawRegularPloygon");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual int  GetCurPenCode();
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	void Finish();
	
protected:
	int m_nSides;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CPermanentExchanger *m_pSel;
};

#define PF_RADIUS  "Radius"
class CDrawCircleCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawCircleCommand)
public:	
	CDrawCircleCommand();
	virtual ~CDrawCircleCommand();
	static CCommand* Create(){
		return new CDrawCircleCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawCircle");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual int  GetCurPenCode();
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	virtual void Finish();
//	CFeature *m_pFtr;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CPermanentExchanger *m_pSel;

	// 画圆方式：0，二点；1，三点；2，圆心+半径
	int   m_nMode;
	BOOL m_bMouseDefineRadius;
	double m_fRadius;
};

class CDrawCircleTwoPtCommand : public CDrawCircleCommand  
{
	DECLARE_DYNCREATE(CDrawCircleTwoPtCommand)
public:	
	CDrawCircleTwoPtCommand();
	virtual ~CDrawCircleTwoPtCommand();
	static CCommand* Create(){
		return new CDrawCircleTwoPtCommand;
	}
	virtual CString AccelStr(){
		return _T("Circle2P");
	}
	
	virtual CString Name();
};

class CDrawCircleThreePtCommand : public CDrawCircleCommand  
{
	DECLARE_DYNCREATE(CDrawCircleThreePtCommand)
public:	
	CDrawCircleThreePtCommand();
	virtual ~CDrawCircleThreePtCommand();
	static CCommand* Create(){
		return new CDrawCircleThreePtCommand;
	}
	virtual CString AccelStr(){
		return _T("Circle3P");
	}
	
	virtual CString Name();
};

class CDrawCircleCenterRadiusCommand : public CDrawCircleCommand  
{
	DECLARE_DYNCREATE(CDrawCircleCenterRadiusCommand)
public:	
	CDrawCircleCenterRadiusCommand();
	virtual ~CDrawCircleCenterRadiusCommand();
	static CCommand* Create(){
		return new CDrawCircleCenterRadiusCommand;
	}
	virtual CString AccelStr(){
		return _T("CircleCenterRadius");
	}
	
	virtual CString Name();
};

class CDrawLinesBySide : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawLinesBySide)
public:
	CDrawLinesBySide();
	virtual ~CDrawLinesBySide();
	static CCommand* Create(){
		return new CDrawLinesBySide;
	}
	virtual CString AccelStr(){
		return _T("DrawLinesBySide");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtReset(PT_3D &pt);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual void Back();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
	BOOL IsEndSnapped();
protected:
	void Finish();

private:
	CFeature *CreateObjBySide(CArray<PT_3DEX,PT_3DEX>& pts, BOOL& finished);
	
	
protected:	
//	CFeature *m_pFtr;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CPermanentExchanger *m_pSel;
	BOOL m_bOpenIfSnapped;//端点捕捉
};

// 多点拟合边测房
class CDrawLinesByMultiPt : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawLinesByMultiPt)
public:
	CDrawLinesByMultiPt();
	virtual ~CDrawLinesByMultiPt();
	static CCommand* Create(){
		return new CDrawLinesByMultiPt;
	}
	virtual CString AccelStr(){
		return _T("DrawLinesByMultiPt");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtReset(PT_3D &pt);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual void Back();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	void Finish();
	
private:
	CFeature *CreateObjBySide(BOOL& finished);
	
	
protected:	
	//	CFeature *m_pFtr;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CPermanentExchanger *m_pSel;
	
	BOOL m_bLastRClick;
	CArray<PT_3DEX,PT_3DEX> m_arrBuildLine;
	
};

class CDrawTextCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawTextCommand)
public:	
	CDrawTextCommand();
	virtual ~CDrawTextCommand();
	static CCommand* Create(){
		return new CDrawTextCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawText");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	virtual void Back();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();

protected:
	virtual void Finish();

	TEXT_SETTINGS0 m_sTxtSettings;
	bool m_bDrag;
	CString m_strText;
	CPermanentExchanger *m_pSel;
};

class CContourTextCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CContourTextCommand)
public:	
	enum
	{
		modeSinglePoint = 0,
		modeLinesection = 1
	};
	CContourTextCommand();
	virtual ~CContourTextCommand();
	static CCommand* Create(){
		return new CContourTextCommand;
	}
	virtual CString AccelStr(){
		return _T("ContourText");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

	static int FindIndex(const CStringArray &arrStr,CString str);

	BOOL FilterFeature(LONG_PTR id);

	void PtMove(PT_3D &pt);	
	virtual void PtReset(PT_3D &pt);

protected:
	CFeature* MarkText(PT_3DEX *pts, int num, float height);
	// 注记朝向为：从高程低向高程高方向看注记为正
	CFeature* CreateText();
	BOOL FindAnotherNearestPt(PT_3D pt, PT_3D *ret);

	TEXT_SETTINGS0 m_sTxtSettings;
	int m_nDigitNum;
	int m_nTextCoverType;//文字压盖  压盖类型: 0, 无压盖 1,矩形压盖 2,圆形压盖 ok!
	float   m_fExtendDis;// 压盖外扩距离
	CString m_strTextLayerCode;
	// 操作方式 0，单点 1，线段
	int	  m_nMode;
	// 要处理的等高线层，仅在线段模式时有效，对应 计曲线注记 命令
	CString        m_strContourLayerCode;
	CFeature	   *m_pCurFtr;

	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
private:
	FTR_HANDLE m_idContour, m_idText;
	
};



class CNumberTextCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CNumberTextCommand)
public:	
	CNumberTextCommand();
	virtual ~CNumberTextCommand();
	static CCommand* Create(){
		return new CNumberTextCommand;
	}
	virtual CString AccelStr(){
		return _T("NumberTextCommand");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	virtual void Back();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	int FindIndex(const CStringArray &arrStr,CString str);
	void FinishObject();
	CFtrLayer *GetLayer(LPCTSTR fid);
	
	TEXT_SETTINGS0 m_sTxtSettings;
	bool m_bDrag;
	CString m_strText;
	CStringArray m_arrFont;
	CPermanentExchanger *m_pSel;
	
	CString m_strLayerCode;
	int m_nNumber;
	
};


class CSmoothCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSmoothCommand)
public:	
	enum
	{
		modeWhole=0,
		modePart=1
	};
	enum
	{
		modeDrag=0,
		modeTwoPT=1
	};
	struct OBJ_ITEM
	{
		FTR_HANDLE id;
		int pos;
		PT_3D ret;
		double t;
	};
	
	struct OBJ_PART
	{
		FTR_HANDLE id;
		int pos1, pos2;
		PT_3D pt1, pt2;
	};
	CSmoothCommand();
	virtual ~CSmoothCommand();
	static CCommand* Create(){
		return new CSmoothCommand;
	}
	virtual CString AccelStr(){
		return _T("Smooth");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

protected:
	void GetPartObjs(CArray<OBJ_PART,OBJ_PART> *arr);
	virtual void Finish();
private:
	PT_3D m_ptLine[4];                       //局部光滑->截取 选项中两条截取线段的首尾点
	CArray<PT_3D,PT_3D> m_arrPts;						 //用于画两条截取线段
	PT_3D m_ptEnd;
	PT_3D m_ptStart;
	int m_nMode;                            //标志局部还是整体
	int m_nPartWay;                         //局部光滑中的两个选项：拉框，截取
	double m_lfLimit;                        //容差
};


//	static CGeometry *ChangeObj(CGeometry *pObj, CGeoData *pSource, CGeometry *pTempl);
//********************画图例图表**********************//

class CDrawTableCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawTableCommand)
public:	

	CDrawTableCommand();
	virtual ~CDrawTableCommand();
	static CCommand* Create(){
		return new CDrawTableCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawTable");
	}
	virtual void PtClick(PT_3D &pt, int flag);
//	void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	BOOL GetPartColumSpace(CStringArray &StrColumnSpace,int i,double &Total);	
protected:
	
	virtual void Finish();
private:

	PT_3D m_ptStart;//鼠标开始单击点
	PT_3D m_ptLast;//保存上次单击点
	CArray<PT_3DEX,PT_3DEX> m_arrPts;			//记录点
	CArray<PT_3DEX,PT_3DEX> m_arrPts_back;		//备份点
	CStringArray m_StrColumnSpace;				//记录列宽区间
	int m_nMode;							//0 准备绘画 1 开始绘画 2完成绘画
      

	int m_nTableLine;						//表格行
	int m_nTableColumn;						//表格列 默认为2
	int	m_nTableWidthSpace;					//上下间隔宽高 默认为10
	CString m_StrTableColumnSpace;			//第一列列宽 长默认为100 第一列占100*1.5 后面的列占100*0.3 
		
};	
//******************自动生成文字符号或文字描述*************//
class CCreateSymbolAndDiscriptCommand :CDrawCommand
{
		DECLARE_DYNCREATE(CCreateSymbolAndDiscriptCommand)
public:
	enum
	{
		modeLeft = 0,
		modeCenter = 1,
		modeRight = 2
	};
	enum 
	{
		shap_Line  = 0,
		shap_Rect = 1,
		shap_CirCle = 2
	}DRAW_SHAP;
	CCreateSymbolAndDiscriptCommand();
	virtual ~CCreateSymbolAndDiscriptCommand();
	static CCommand* Create(){
		return new CCreateSymbolAndDiscriptCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateSymbolOrDiscriptCommand");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	virtual	void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	virtual void Finish();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	BOOL CreateSymbolItem(CSchemeLayerDefine *pLayer,PT_3D pt,double cx, double cy,int nLayerNum = 1);//play 为层定义对象 pt为画地物点 cx cy 为矩形框长宽 LayaerNum 为画的地物个数
	BOOL CreateSymbolItem(CFtrLayer *pLayer,PT_3D pt,double cx, double cy,int nLayerNum = 1);
	void DrawSymbolItem(int cx, int cy, const GrBuffer2d *pBuf, COLORREF col, COLORREF colBak);
	PT_3D GetAlignmentPt(PT_3D &pt,CArray<PT_3DEX,PT_3DEX> &arrPts);//pt为输出的对齐点 arrpts为所在表格的关键点
	BOOL CheckStrLayerName(CString SrcLayerName,CStringArray &StrArrLayerName,CString &DesLayerName);//
protected:
	
	TEXT_SETTINGS0 m_sTxtSettings;
	BOOL m_bType;//0 生成文字描述 1 生成符号
	int m_nAlignment; //文字对齐方式 0居左 1 居中 2居右 
	int		m_nScale;
	int m_nShape;// 0 直线 1 矩形 2 圆 
	double m_nWidth;//矩形宽度 宽度(1/N)
	double m_nHeight;//矩形高度 高度(1/N)
	double m_nDiameter;//圆形状直径
	CFeature *m_pFtrSymbol;
	CFtrLayer *pLayersymbol;
	CFtrLayer *pLayer;
private:
	CString m_strLayName;//层名
	CStringArray m_StrArrLayerName;//层名字符串数组
};








class CFormatFtrCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CFormatFtrCommand)
public:	
	CFormatFtrCommand();
	virtual ~CFormatFtrCommand();
	static CCommand* Create(){
		return new CFormatFtrCommand;
	}
	virtual CString AccelStr(){
		return _T("FormatFtr");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Start();
private:
	FTR_HANDLE m_handleRefer;
	
};

class CGrTrim;
class CTrimCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTrimCommand)
public:	
	CTrimCommand();
	virtual ~CTrimCommand();
	static CCommand* Create(){
		return new CTrimCommand;
	}
	virtual CString AccelStr(){
		return _T("Trim");
	}
	virtual void Back();
	int GetState();
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual void PtClick(PT_3D &pt, int flag);
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	static BOOL TrimCurve(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim, BOOL bTrimOutside);
	static BOOL TrimSurface(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim, BOOL bTrimOutside);
	static void GetPolyPts(CGeometry *pObj, CArray<PT_3DEX,PT_3DEX>& pts);
	
protected:	
	virtual void Finish();
protected:
	CDrawCurveProcedure *m_pDrawProc;
	
	BOOL  m_bTrimOut;
	int   m_nBoundType;//0, 绘制，1,选取
	FTR_HANDLE m_handleBound;
	CGeometry *m_pGeoCurve;

	// 闭合线按照面的方式裁剪
	BOOL  m_bTrimClosedCurve;
	// 面按照线的方式裁剪
	BOOL  m_bTrimSurface;
};


class CLinkContourCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CLinkContourCommand)
public:	
	struct OBJ_ITEM
	{
		FTR_HANDLE id;
		int pos;
		PT_3D ret;
		double t;
	};
	
// 	struct OBJ_PART
// 	{
// 		FTR_HANDLE id;
// 		int pos1, pos2;
// 		PT_3D pt1, pt2;
// 	};
	CLinkContourCommand();
	virtual ~CLinkContourCommand();
	static CCommand* Create(){
		return new CLinkContourCommand;
	}
	virtual CString AccelStr(){
		return _T("LinkContour");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void  PtMove(PT_3D &pt);
	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

protected:
	void LinkObjs();
private:
	PT_3D m_ptLine[4];
	CArray<PT_3D,PT_3D> m_arrPts;						 //用于画两条截取线段
	BOOL m_bMatchHeight;
	BOOL m_bCrossLayer;
	
};

class CDlgDataSource;
class CAutoLinkContourCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CAutoLinkContourCommand)
public:
	enum
	{
		modeWhole = 0,
		modePart  = 1		
	};
	CAutoLinkContourCommand();
	virtual ~CAutoLinkContourCommand();
	static CCommand* Create(){
		return new CAutoLinkContourCommand;
	}
	virtual CString AccelStr(){
		return _T("AutoLinkContour");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	static BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode);
	static bool CheckObjForZ(CGeometry *pObj);

protected:
	bool IsPtInRect(PT_3DEX *pt);
	bool IsSameLayer(FTR_HANDLE h1, FTR_HANDLE h2);

private:
	PT_4D m_pt4dEnd;
	
	//在ref中寻找与objid相等的句柄，找到则返回索引，否则返回-1
	int FindEqualObj(const CArray<FTR_HANDLE,FTR_HANDLE> & ref,FTR_HANDLE objid);
	int m_nPart;
	PT_3D m_pPts[2];
	CString m_strLayerCode;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrObjHandles;
	double m_lftolerace;
	BOOL m_bMatchHeight;
	BOOL m_bRiverHeightError;
};

typedef struct tag_ObjRatio
{
	FTR_HANDLE ftr;
	double lfRatio;
}ObjRatio;

//曲线内插命令
class CInterpolateCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CInterpolateCommand)
public:
	enum
	{
		modeWhole = 0,
		modePart  = 1		
	};
	enum
	{
		wayNdis=0,
		wayEdis=1,//等距离内插
		wayConstDis=2//固定距离内插
	};

	CInterpolateCommand();
	virtual ~CInterpolateCommand();
	static CCommand* Create(){
		return new CInterpolateCommand;
	}
	virtual CString AccelStr(){
		return _T("Interpolate");
	}
	virtual void Back();
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
//	static BOOL CheckObjForLayerCode(CDlgDataSource *pDS,CFeature *pFtr,LPCTSTR  Layercode);
//	static bool CheckObjForZ(CGeometry *pObj);
	void InitForFunction(int interp_num);
	void InterpWholeObjByEquidis1(CFeature *pFtr1, CFeature *pFtr2, CFeature *pTempl, CPtrArray *pArr);
	void InterpWholeObjByEquidis(CFeature *pFtr1, CFeature *pFtr2, CFeature *pTempl, CPtrArray *pArr);
	void InterpWholeObjByEquidis2(CFeature *pFtr1, CFeature *pFtr2, CFeature *pTempl, CPtrArray *pArr);
protected:
	void GetPartObjs(CPtrArray *arr);
	BOOL Compress(CArray<PT_3DEX,PT_3DEX> &arr);
protected:
	bool GetFirstPointOfIntersect(PT_3DEX *pts,int num,PT_3D pt1,PT_3D pt2,int *index,double *rett,PT_3D *ret);
	PT_3D m_ptLine[4];
	
	int	  m_nMode; //0, 等距离内插
	int	  m_nPart; //0, 整体； 1， 部分
	short m_nInNum; //内插条数
	long  m_nFindRadius; //搜索半径
	BOOL  m_bOnlyContour;
	CString m_strBaseFCode, m_strSonFCode;
	CArray<PT_3D,PT_3D> m_arrPts;
	double m_fLimit;   // 顶点密度
	double m_lfDis;//固定距离内插间距
protected:
	PT_3D m_pt[2];
	CArray<double,double> m_lft1;
	CArray<double,double> m_lft2;

	CArray<ObjRatio,ObjRatio> m_arrObjRatio1;
	CArray<ObjRatio,ObjRatio> m_arrObjRatio2;

	// 支持回退操作
	CArray<LINE_3D,LINE_3D> m_arrLine;
};


class CContinueInterpolateCommand:public CInterpolateCommand 
{
public:
	DECLARE_DYNCREATE(CContinueInterpolateCommand)
	virtual void PtReset(PT_3D &pt);
	void Back();
	CContinueInterpolateCommand();
	virtual ~CContinueInterpolateCommand();
	
	virtual CString Name();
	
	static CCommand* Create()
	{
		return new CContinueInterpolateCommand;
	}
	virtual void PtClick(PT_3D &pt, int flag);	
	virtual void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
protected:
	void Finish();
	void FillShowParams(CUIParam* param, BOOL bForLoad );
	void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	void GetPartObjs(CPtrArray *arr);
	
protected:
	int FindIndexInArray(FTR_HANDLE handle,const CArray<FTR_HANDLE,FTR_HANDLE> &arrUndoobjs);
	CArray<int,int> m_arrInt;
	CArray<LINE_3D,LINE_3D> m_arrLine;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrDelObjs;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrAddObjs;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrUndoobjs;
};

//内插中心线命令
class CInterpolateCenterLineCommand : public CInterpolateCommand
{
public:
	DECLARE_DYNCREATE(CInterpolateCenterLineCommand)

	CInterpolateCenterLineCommand();
	virtual ~CInterpolateCenterLineCommand();
	
	virtual CString Name();
	void Start();
	static CCommand* Create()
	{
		return new CInterpolateCenterLineCommand;
	}	

	void FillShowParams(CUIParam* param, BOOL bForLoad );
};

//内插道路中心线命令
class CInterpolateCenterLoadCommand : public CInterpolateCommand
{
	DECLARE_DYNCREATE(CInterpolateCenterLoadCommand)
private:
	struct CompaireItem
	{
		int constraint_id;
		int compaire_count;
		//
		CompaireItem()
		{
			constraint_id = -1;
			compaire_count = 0;
		}
	};

	static bool compaire_item(const CInterpolateCenterLoadCommand::CompaireItem& va, const CInterpolateCenterLoadCommand::CompaireItem& vb)
	{
		return va.compaire_count > vb.compaire_count;
	}
public:
	CInterpolateCenterLoadCommand()
	{

	}
	virtual ~CInterpolateCenterLoadCommand()
	{

	}
	static CCommand* Create(){
		return new CInterpolateCenterLoadCommand;
	}
	virtual CString AccelStr(){
		return _T("InterpolateCenterLoad");
	}
	virtual CString Name();
	void PtClick1(PT_3D &pt, int flag);
	void PtClick(PT_3D &pt, int flag);
	void Start();
	void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	void SetParams(CValueTable& tab,BOOL bInit = FALSE);
private:
    void KickoffSameGeoPts(CArray<GeoPoint,GeoPoint>& arr);
private:

};

class CShowDirCommand : public CCommand  
{
	DECLARE_DYNCREATE(CShowDirCommand)
public:	
	CShowDirCommand();
	virtual ~CShowDirCommand();
	static CCommand* Create(){
		return new CShowDirCommand;
	}
	virtual CString AccelStr(){
		return _T("ShowDir");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Start();
	
private:
	void DrawDir(CGeometry *pObj, double dirlen, GrBuffer *pBuf);
};

class CMeasureDistanceCommand : public CCommand  
{
	DECLARE_DYNCREATE(CMeasureDistanceCommand)
public:
	CMeasureDistanceCommand();
	virtual ~CMeasureDistanceCommand();
	static CCommand* Create(){
		return new CMeasureDistanceCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasureDistance");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual DrawingInfo GetCurDrawingInfo();

protected:    

private:
	int m_nStep;
	PT_3D m_ptStart;
};

class CMeasureDisOnMapCommand : public CCommand  
{
	DECLARE_DYNCREATE(CMeasureDisOnMapCommand)
public:
	CMeasureDisOnMapCommand();
	virtual ~CMeasureDisOnMapCommand();
	static CCommand* Create(){
		return new CMeasureDisOnMapCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasureDisOnMap");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual DrawingInfo GetCurDrawingInfo();
	
protected:    
	
private:
	int m_nStep;
	PT_3D m_ptStart;
};

//点到线串最近距离命令
class CMeasurePointToLineDistanceCommand :public CCommand
{
	DECLARE_DYNCREATE(CMeasurePointToLineDistanceCommand)
public:
	static BOOL GetDistanceFromPointToLine(PT_3D ptA,PT_3D ptB,PT_3D ptC,double *fDistance,PT_3D *rtPT);
	CMeasurePointToLineDistanceCommand();
	virtual ~CMeasurePointToLineDistanceCommand();
	static CCommand* Create(){
		return new CMeasurePointToLineDistanceCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasurePointToLineDistance");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	
protected:    
	
private:
	int m_nStep;
	PT_3D m_ptStart;
	PT_3D m_ptEnd;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	double m_fMindistance2d;
	CGeometry *m_pObj;//几何对象

};

//量测三点的夹角命令
class CMeasureThreePointAngleCommand : public CCommand
{
	DECLARE_DYNCREATE(CMeasureThreePointAngleCommand)
public:
	CMeasureThreePointAngleCommand();
	virtual ~CMeasureThreePointAngleCommand();
	static CCommand* Create(){
		return new CMeasureThreePointAngleCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasureThreePointAngleCommand");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual DrawingInfo GetCurDrawingInfo();
private:
	int m_nStep;	
	PT_3D m_pt[3];	//鼠标拾取的三个点
	
};


#define PF_MEASUREMODE  _T("MeasureMode")
class CMeasureLengthCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMeasureLengthCommand)
public:
	CMeasureLengthCommand();
	virtual ~CMeasureLengthCommand();
	static CCommand* Create(){
		return new CMeasureLengthCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasureLength");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual void Finish();
	virtual void PtReset(PT_3D &pt);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo();
protected:    
	
	int m_nStep;
	PT_3D m_ptStart;

	int m_nMode;   //0,选取;1,采集

	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;

	double m_fLength2d, m_fLength3d;
	PT_3D m_ptLast;
};


class CMeasurePhotoLengthCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMeasurePhotoLengthCommand)
public:
	CMeasurePhotoLengthCommand();
	virtual ~CMeasurePhotoLengthCommand();
	static CCommand* Create(){
		return new CMeasurePhotoLengthCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasurePhoto");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	
	void Abort();
	void Start();
	virtual void Finish();
	virtual void PtReset(PT_3D &pt);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:    
	
	int m_nStep;
	PT_3D m_ptStart;
	
	int m_nMode;   //0,选取;1,采集
	
	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;
	
	double m_fLength2d, m_fLength3d;
	PT_3D m_ptLast;

	float m_fPGScale;
	float m_fPixelScale;
};



class CMeasureAngleCommand : public CCommand  
{
	DECLARE_DYNCREATE(CMeasureAngleCommand)
public:	
	CMeasureAngleCommand();
	virtual ~CMeasureAngleCommand();
	static CCommand* Create(){
		return new CMeasureAngleCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasureAngle");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Start();
	
private:
	int m_nStep;
	PT_3D m_ptclick1;//记录第一次点击的坐标
	LINE_3D m_linesection[2];
};


class CMeasureAreaCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMeasureAreaCommand)
public:	
	CMeasureAreaCommand();
	virtual ~CMeasureAreaCommand();
	static CCommand* Create(){
		return new CMeasureAreaCommand;
	}
	virtual CString AccelStr(){
		return _T("MeasureArea");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Finish();
	void Start();
	void PtMove(PT_3D &pt);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
private:
	void TipArea(FTR_HANDLE handle);

	int m_nMode;   //0,选取;1,采集
	
	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;
	
	double m_fLength2d, m_fLength3d;
	PT_3D m_ptLast, m_ptStart;
	
};


class CMeasureImageCommand : public CCommand
{
	DECLARE_DYNCREATE(CMeasureImageCommand)
public:
	CMeasureImageCommand();
	virtual ~CMeasureImageCommand();

	virtual CString AccelStr(){
		return _T("AdjustImage");
	}
	
	static CCommand* Create();
	virtual CString Name();

	void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);
	void Abort();
	void Start();
	void PtReset(PT_3D &pt);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	
private:
	int m_nStep;
	CStringArray m_arrFileNames;
	CString m_strFileName;
	CArray<PT_3D,PT_3D> m_arrPts;
	ViewImgPosition m_SaveImgPos;
};


#define  CELLDEFNAME		_T("CellDef")
class CCellDefineCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCellDefineCommand)
public:	
	CCellDefineCommand();
	virtual ~CCellDefineCommand();
	static CCommand* Create(){
		return new CCellDefineCommand;
	}
	virtual CString AccelStr(){
		return _T("CellDefine");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Start();
	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
private:
	// 图元定义
	CellDef	m_CellDef;	
	CString m_strCellDefName;
};

class CCPResultList;
class CTriangle;

//检查点线矛盾
class CCheckPLErrorCommand :public CSearcherFilter,  public CEditCommand
{
	DECLARE_DYNCREATE(CCheckPLErrorCommand)
public:
	CCheckPLErrorCommand();
	virtual ~CCheckPLErrorCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("PLCheck");
	}
	void Start();
	void Abort();
	void Finish();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);

	virtual CProcedure *GetActiveSonProc(int nMsgType);	
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	
private:
	BOOL FilterFeature(LONG_PTR id);
	void CheckObj(CFeature *pFtr, CCPResultList *result);
	void MarkPt(PT_3D pt);
		
protected:
	CString m_strCheckFCode;
	CString m_strContourFCode;
	double	m_lfMarkWid;
	float   m_fContourInter;
	float	m_fMinDZ;
	short	m_nIntensity;

	// 检查范围
	int   m_nBoundType; //0, 全图，1,绘制

	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;
};

#define  CMDPLANE_CONTOURCODE "BaseFCode"
#define  CMDPLANE_CHECKCODE "CheckFCode"
//检查线线矛盾
class CCheckLLErrorCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCheckLLErrorCommand)
public:
	CCheckLLErrorCommand();
	virtual ~CCheckLLErrorCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("LLCheck");
	}
	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
private:
	void GetIntersectObjs(PT_3D pt0, PT_3D pt1, CArray<FTR_HANDLE,FTR_HANDLE>& arr);
	void CheckObj(CFeature *pFtr, CCPResultList *result);
	void MarkPt(PT_3D pt);
		
protected:
	CString m_strBaseFCode;
	CString m_strCheckFCode;
	double	m_lfMarkWid;
};

#define CMDPLANE_AREARADIUS "AreaRadius"
//检查点线矛盾
class CCheckPPErrorCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCheckPPErrorCommand)
public:
	CCheckPPErrorCommand();
	virtual ~CCheckPPErrorCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("PPCheck");
	}
	void Start();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
private:
	void CheckObj(CFeature *pFtr, CFtrLayer *pLayer, CCPResultList *result);
	static BOOL FilterObj(CFeature *pFtr, CFtrLayer *pLayer);
	
protected:
	double	m_lfFindRadius;
};

class CPointTextConvertBaseCommand : public CEditCommand
{
	DECLARE_DYNAMIC(CPointTextConvertBaseCommand)
public:
	CPointTextConvertBaseCommand();
	virtual ~CPointTextConvertBaseCommand();
	void PtClick(PT_3D &pt, int flag);
	void Start();
	virtual BOOL Convert() = 0;

protected:
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrFtrs;
};

class CAnnotToTextCommand : public CPointTextConvertBaseCommand  
{
	DECLARE_DYNCREATE(CAnnotToTextCommand)
public:
	CAnnotToTextCommand();
	virtual ~CAnnotToTextCommand();
	static CCommand* Create(){
		return new CAnnotToTextCommand;
	}
	virtual CString AccelStr(){
		return _T("AnnotToText");
	}
	virtual CString Name();
	
	BOOL Convert();
};

// 由点生成高程注记
class CPointZToTextCommand : public CPointTextConvertBaseCommand  
{
	DECLARE_DYNCREATE(CPointZToTextCommand)
public:
	CPointZToTextCommand();
	virtual ~CPointZToTextCommand();
	static CCommand* Create(){
		return new CPointZToTextCommand;
	}
	virtual CString AccelStr(){
		return _T("PointZToText");
	}
	virtual CString Name();
	
	virtual BOOL Convert();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);

	TEXT_SETTINGS0 m_sTxtSettings;
	int m_nDigitNum;
	CString m_strTextLayerCode;

};

// 由高程注记生成点
class CTextToPointCommand : public CPointTextConvertBaseCommand  
{
	DECLARE_DYNCREATE(CTextToPointCommand)
public:
	CTextToPointCommand();
	virtual ~CTextToPointCommand();
	static CCommand* Create(){
		return new CTextToPointCommand;
	}
	virtual CString AccelStr(){
		return _T("TextToPointZ");
	}
	virtual CString Name();
	
	virtual BOOL Convert();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);

	CString m_strLayerCode;
};

// 修改点高程及注记
class CModifyPointzAndTextCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CModifyPointzAndTextCommand)
public:
	CModifyPointzAndTextCommand();
	virtual ~CModifyPointzAndTextCommand();
	static CCommand* Create(){
		return new CModifyPointzAndTextCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyPointzAndText");
	}
	virtual CString Name();
	
	void PtClick(PT_3D &pt, int flag);
	void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	int m_nDigitNum;
};

// 修改注记小数点位数
class CModifyTextDigitCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CModifyTextDigitCommand)
public:
	CModifyTextDigitCommand();
	virtual ~CModifyTextDigitCommand();
	static CCommand* Create(){
		return new CModifyTextDigitCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyTextDigit");
	}
	virtual CString Name();
	
	void PtClick(PT_3D &pt, int flag);
	void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	int m_nDigitNum;
};

class CDrawMultiSurfaceCommand : public CDlgDrawSurfaceCommand  
{
	DECLARE_DYNCREATE(CDrawMultiSurfaceCommand)
public:	
	CDrawMultiSurfaceCommand();
	virtual ~CDrawMultiSurfaceCommand();
	static CCommand* Create(){
		return new CDrawMultiSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawMultiSurface");
	}
	virtual CString Name();
	virtual void OnSonEnd(CProcedure *son);
	virtual void PtClick(PT_3D &pt, int flag);
	void  PtMove(PT_3D &pt);
	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
	
protected:	
	CFeature * FindSmallClosureSurfaceOfEnvelope(CDlgDataSource *pDS,Envelope e);
	void Finish();
	BOOL m_bMulitSurface;		//是否产生了复杂面	
	FTR_HANDLE m_idMother;		//复杂面的母亲对象
	FTR_HANDLE m_idComplex;		//复杂面的母亲对象
	CPermanentExchanger *m_pSel;
		
};

class CDrawLinesbyAngCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CDrawLinesbyAngCommand)
	enum
	{
		modeLine = 0,
		modeArea = 1			
	};
public:		
	CDrawLinesbyAngCommand();
	virtual ~CDrawLinesbyAngCommand();
	static CCommand* Create(){
		return new CDrawLinesbyAngCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawLinesbyAng");
	}
	virtual CString Name();	
	virtual void PtClick(PT_3D &pt, int flag);
	void  PtMove(PT_3D &pt);
	void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	virtual void Back();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
	BOOL IsEndSnapped();

protected:
	void Finish();	
	
private:	
	int m_nMode;
	int m_nStep;
	BOOL m_bClosed;
	BOOL m_bOpenIfSnapped;
	int m_nEndSnapped;
	BOOL m_bSnapPt1;
	PT_3D m_SnapPt1;
//	CFeature* m_pFtr;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
protected:
	CPermanentExchanger *m_pSel;
};

class CCreateMultiSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCreateMultiSurfaceCommand)
public:
	CCreateMultiSurfaceCommand();
	virtual ~CCreateMultiSurfaceCommand();
	static CCommand *Create(){
		return new CCreateMultiSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateMultiSurface");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	void Finish();
	
	BOOL m_bKeepInnerBoundObj;
	
};

class CSeparateMultiSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSeparateMultiSurfaceCommand)
public:
	CSeparateMultiSurfaceCommand();
	virtual ~CSeparateMultiSurfaceCommand();
	static CCommand *Create(){
		return new CSeparateMultiSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("SeparateMultiSurface");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	//virtual void Abort();
protected:
	void Finish();
	static BOOL SeparateSurface(CFeature *pFtr, CFtrLayer *pLayer, CPtrArray& arr);
};

#define PF_SEPSYMMOD _T("SepSymMode")
class CSeparateSymbolsCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSeparateSymbolsCommand)
public:
	CSeparateSymbolsCommand();
	virtual ~CSeparateSymbolsCommand();
	static CCommand *Create(){
		return new CSeparateSymbolsCommand;
	}
	virtual CString AccelStr(){
		return _T("SeparateSymbols");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	//virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();

	BOOL SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arr);

	BOOL  m_bKeepOld;
	int   m_nSepMode;
};

// 双线符号生成
#define PF_CREATESYMBOLS_LAYERNAME _T("LayerName")
class CCreateSymbolsCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCreateSymbolsCommand)
public:
	CCreateSymbolsCommand();
	virtual ~CCreateSymbolsCommand();
	static CCommand *Create(){
		return new CCreateSymbolsCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateSymbols");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	//virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();
	
	BOOL SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arr);
	

	BOOL m_bKeepOld;
	CString m_strRetLay;

	FTR_HANDLE m_arrFtrs[2];
};


// 生成填充符号
#define PF_CREATESYMBOLS_LAYERNAME _T("LayerName")
class CCreateHatchSymbolsCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCreateHatchSymbolsCommand)
public:
	CCreateHatchSymbolsCommand();
	virtual ~CCreateHatchSymbolsCommand();
	static CCommand *Create(){
		return new CCreateHatchSymbolsCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateHatchSymbols");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	//virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();
	
	BOOL SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arr);
	
	void DeleteSomeSymbols(CPtrArray& arrPSyms);
	
	BOOL m_bKeepOld;
	CString m_strRetLay;
	
	FTR_HANDLE m_hFtr;
};


class CSeparateSurfaceSymbolsCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSeparateSurfaceSymbolsCommand)
public:
	CSeparateSurfaceSymbolsCommand();
	virtual ~CSeparateSurfaceSymbolsCommand();
	static CCommand *Create(){
		return new CSeparateSurfaceSymbolsCommand;
	}
	virtual CString AccelStr(){
		return _T("SeparateSurfaceSymbols");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	//virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();
	
	BOOL SeparateSymbols_new(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arr);
	
	BOOL  m_bKeepOld;
	int   m_nSepMode;
};


// 层间拷贝
#define PF_COPYFTRS_LAYERNAME _T("LayerName")
class CCopyFtrsBetweenLayerCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCopyFtrsBetweenLayerCommand)
public:
	CCopyFtrsBetweenLayerCommand();
	virtual ~CCopyFtrsBetweenLayerCommand();
	static CCommand *Create(){
		return new CCopyFtrsBetweenLayerCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyFtrsBetweenLayers");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();
		
	CString m_strRetLay;
	
};


//复制桥(涵洞、隧道)下地物，全图处理
class CCopyFtrsInsideBridgeCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCopyFtrsInsideBridgeCommand)
public:
	CCopyFtrsInsideBridgeCommand();
	virtual ~CCopyFtrsInsideBridgeCommand();
	static CCommand *Create(){
		return new CCopyFtrsInsideBridgeCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyFtrsInsideBridge");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();
	void GetEndFtrs(CFeature *pFtr, CFeature *& pFtr1, CFeature *& pFtr2);
				
	CString m_strRetLay;
	
};



//复制桥(涵洞、隧道)下地物，单个处理
class CCopyFtrsInsideBridgeCommand_One : public CEditCommand
{
	DECLARE_DYNCREATE(CCopyFtrsInsideBridgeCommand_One)
public:
	CCopyFtrsInsideBridgeCommand_One();
	virtual ~CCopyFtrsInsideBridgeCommand_One();
	static CCommand *Create(){
		return new CCopyFtrsInsideBridgeCommand_One;
	}
	virtual CString AccelStr(){
		return _T("CopyFtrsInsideBridgeOne");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

protected:

	FTR_HANDLE m_hBridgeFtr;
				
};


// 河流渐变
#define PF_RIVERCHANGE_STARTWID _T("StartWid")
#define PF_RIVERCHANGE_ENDWID _T("EndWid")
class CRiverChangeCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CRiverChangeCommand)
public:
	CRiverChangeCommand();
	virtual ~CRiverChangeCommand();
	static CCommand *Create(){
		return new CRiverChangeCommand;
	}
	virtual CString AccelStr(){
		return _T("RiverChange");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	void Finish();
	
	CFeature *m_pFtr;
	float m_fStartWid, m_fEndWid;
	PT_3D m_ptClick;
	
};

// 河流普染
#define PF_RIVERFILL_COLOR _T("RiverColor")
#define PF_RIVERFILL_TYPE _T("RiverFillType")
#define PF_RIVERFILL_RETNAME _T("RiverFillRetName")
class CRiverFillCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CRiverFillCommand)
public:
	CRiverFillCommand();
	virtual ~CRiverFillCommand();
	static CCommand *Create(){
		return new CRiverFillCommand;
	}
	virtual CString AccelStr(){
		return _T("RiverFill");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);

	
	virtual CProcedure *GetActiveSonProc(int nMsgType);	
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
protected:
	void Finish();
	void Abort();
	
	long m_nColor;

	int   m_nFillType; //0, 选择，1,绘制
	CString m_strRetLay;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;
	
};

class CExplodeTextCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CExplodeTextCommand)
public:
	CExplodeTextCommand();
	virtual ~CExplodeTextCommand();
	static CCommand *Create(){
		return new CExplodeTextCommand;
	}
	virtual CString AccelStr(){
		return _T("ExplodeText");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

protected:
	void Finish();
	
	BOOL ExplodeText(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arr);


};

class CCreatePartDemCommand : public CDrawCommand  
{
	DECLARE_DYNCREATE(CCreatePartDemCommand)
public:		
	CCreatePartDemCommand();
	virtual ~CCreatePartDemCommand();
	static CCommand* Create(){
		return new CCreatePartDemCommand;
	}
	virtual CString AccelStr(){
		return _T("CreatePartDem");
	}
	virtual CString Name();	
	virtual void OnSonEnd(CProcedure *son);
	virtual void PtClick(PT_3D &pt, int flag);
	void  PtMove(PT_3D &pt);
	void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual DrawingInfo GetCurDrawingInfo(); 
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	void RefreshPropertiesPanel();
protected:
	void Finish();
	
	int	  m_nStep;
	int   m_nBoundType;//0, 绘制，1,选取
//	CFeature *m_pFtr;

	CDrawCurveProcedure *m_pDrawProc;
	CPermanentExchanger *m_pSel;
};

class CTrimDemCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTrimDemCommand)
public:		
	CTrimDemCommand();
	virtual ~CTrimDemCommand();
	static CCommand* Create(){
		return new CTrimDemCommand;
	}
	virtual CString AccelStr(){
		return _T("TrimDem");
	}
	virtual CString Name();	
	virtual void OnSonEnd(CProcedure *son);
	virtual void PtClick(PT_3D &pt, int flag);
	void  PtMove(PT_3D &pt);
	void PtReset(PT_3D &pt);
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CProcedure *GetActiveSonProc(int nMsgType);
protected:
	void Finish();
	
	int	  m_nStep;
	int   m_nBoundType;//0:绘制 1:选取 2:工作区
	CGeometry *m_pGeo;
	FTR_HANDLE m_handleBound;
	
	CDrawCurveProcedure *m_pDrawProc;
//	CPermanentExchanger *m_pSel;
};


class CAutoAttributeCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CAutoAttributeCommand)
public:
	CAutoAttributeCommand();
	virtual ~CAutoAttributeCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("AutoNumber");
	}
	void Start();
	virtual void Back();
	int GetState();
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual CProcedure *GetActiveSonProc(int nMsgType);
	void Abort();	
	virtual void PtClick(PT_3D &pt, int flag);
	BOOL Convert(CFeature *pFtr, CFtrLayer *pLayer, CString value, CUndoModifyProperties& undo);

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	

private:
	void Add1(CString& textID);
	
private:
	CString m_strFieldName;
	CString	m_strStartValue;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrHandles;

	CDrawCurveProcedure *m_pDrawProc;
	
	int   m_nBoundType;//0, 绘制，1,选取
	FTR_HANDLE m_handleBound;
	CGeometry *m_pGeoCurve;
};


class CModifyAttributeCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CModifyAttributeCommand)
public:
	CModifyAttributeCommand();
	virtual ~CModifyAttributeCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("ModifyAttribute");
	}
	void Start();
	
	virtual void PtClick(PT_3D &pt, int flag);
	BOOL Convert(CFeature *pFtr, CFtrLayer *pLayer, CString value, CUndoModifyProperties& undo);
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	
		
private:
	CString m_strLayerName;
	CString m_strFieldName;
	CString m_strValue;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrHandles;
};

//末尾为0属性值作随机偏移
class CModifyAttribute1Command : public CEditCommand
{
	DECLARE_DYNCREATE(CModifyAttribute1Command)
public:
	enum
	{
		dirPntDir = 0,
		Width,
		elevation,
		extAttribute
	};
public:
	CModifyAttribute1Command();
	virtual ~CModifyAttribute1Command();
	virtual CString Name();
	static CCommand* Create(){
		return new CModifyAttribute1Command;
	}
	virtual CString AccelStr(){
		return _T("ModifyAttribute1");
	}
	void Start();
	
	virtual void PtClick(PT_3D &pt, int flag);

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	
protected:
	void Convert(CFeature *pFtr, int pos, double offset, CUndoModifyProperties& undo);
private:
	int m_nFieldOption;
	CString m_strLayerName;
	CString m_strFieldName;
	CString m_strValues;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrHandles;
};

class  CDrawSingleParallelCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawSingleParallelCommand)
public:
	void GetParams(CValueTable& tab);
	CDrawSingleParallelCommand();
	static CCommand *Create(){
		return new CDrawSingleParallelCommand;
	}
	virtual CString AccelStr(){
		return _T("SParallel");
	}
	virtual ~CDrawSingleParallelCommand();
	virtual CString Name();
	virtual void Back();
	virtual void Start();	
	void PtReset(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);
	virtual void Abort();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual int  GetCurPenCode();
	virtual DrawingInfo GetCurDrawingInfo();

	virtual BOOL AddObject(CPFeature pFtr, int layid=-1){
		return ((CDlgDoc*)m_pEditor)->AddObjectWithProperties(pFtr,layid);
	}
	virtual BOOL Keyin(LPCTSTR text);

protected:
	virtual void Finish();
	
protected:
	bool m_bTurnForChangeWid;
// 	OBJ_GUID m_objID2;
// 	OBJ_GUID m_objID1;
	bool m_bMark;     //标志是否改变宽度
	int m_nflag;      //标志改变基线
	PT_3D m_lastpt;
	CFeature *m_pFtr1;
	CFeature *m_pFtr2;
	int m_nStep;
	double m_lfcurWidth;
	PT_3D m_pts[3];
	CArray<PT_3DEX,PT_3DEX> m_ptsFtr1;
	CArray<PT_3DEX,PT_3DEX> m_ptsFtr2;

	// c键切换用
	PT_3D m_ptCross;
	PT_3DEX m_addPt;
};

class  CDlgEditVertexCommand : public CEditVertexCommand
{
	DECLARE_DYNCREATE(CDlgEditVertexCommand)
public:
	CDlgEditVertexCommand();
	virtual ~CDlgEditVertexCommand();
	static CCommand *Create(){
		return new CDlgEditVertexCommand;
	}

	virtual void PtDblClick(PT_3D &pt, int flag);
	virtual void PtClick(PT_3D &pt, int flag); 

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);

	virtual BOOL HitTest(PT_3D pt, int state)const;
	virtual void OutputVertexInfo(PT_KEYCTRL pt, CGeometry *pGeo);

	virtual void PtMove(PT_3D &pt);
	virtual void Abort();
	virtual void Finish();

	virtual DrawingInfo GetCurDrawingInfo();

protected:
	CPermanentExchanger *m_pSel;
	
};

class  CDlgInsertVertexCommand : public CInsertVertexCommand
{
	DECLARE_DYNCREATE(CDlgInsertVertexCommand)
public:
	CDlgInsertVertexCommand();
	virtual ~CDlgInsertVertexCommand();
	static CCommand *Create(){
		return new CDlgInsertVertexCommand;
	}
	
	DrawingInfo GetCurDrawingInfo();
	virtual void PtClick(PT_3D &pt, int flag); 
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
};

class CSnapLayerCommand : public CCommand
{
	DECLARE_DYNCREATE(CSnapLayerCommand)
public:
	CSnapLayerCommand();
	virtual ~CSnapLayerCommand();
	static CCommand* Create(){
		return new CSnapLayerCommand;
	}
	virtual CString AccelStr(){
		return _T("SnapLayer");
	}
	void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);
	virtual void GetParams(CValueTable& tab);
	void PtClick(PT_3D &pt, int flag);
	void Start();
	virtual CString Name();
protected:
	BOOL m_bSnapHeight;
};

struct SurfaceTrimPt
{
	PT_3D pt;	//点的坐标	
	int type;	//为0，表示是基线点，为1，则表示是交点
	int no1,no2;//如果type为-1，no1对象基线的索引号，no2为范围线的索引号
				//如果type为1，no1对象的总点列的索引号，no2为范围线的总点列的索引号
	double t;	//t是记录的此点在线段上的位置参数
	int trace;  //是否已经被跟踪过
};

class CGrTrim;
class CTrimLayerCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CTrimLayerCommand)
public:
	CTrimLayerCommand();
	virtual ~CTrimLayerCommand();
	virtual CString Name();
	static CCommand* Create()
	{
		return new CTrimLayerCommand;
	}
	virtual CString AccelStr(){
		return _T("TrimLayer");
	}
		
	void PtClick(PT_3D &pt, int flag);
	void Start();
	void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
protected:
	void Finish();
	CFtrLayer *GetLayer(LPCTSTR layname);	

private:
	BOOL TrimCurve(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim);
	BOOL TrimSurface(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim);
	
	BOOL TrimObj(CGeometry *pObj, CPtrArray *pPolys, CPtrArray *pInside, CPtrArray *pOutside);
	
	//在 TrimSurface 中，对结果地物处理：根据来自原地物的节点z坐标内插来自范围线的节点z坐标
	void InterZ(CGeometry *pObj,CArray<int,int>& flags);
	
	//在 TrimCurve 中，对结果地物处理：根据范围线的节点z坐标内插某个节点的z坐标
	void SnapZ(CGeometry *pObj, int idx, PT_3D *pts, int nPt);
	
	//在 TrimSurface 中，对结果地物处理：根据范围线的节点z坐标内插交点的z坐标
	void SnapZ2(CGeometry *pObj,CArray<int,int>& flags, PT_3D *pts, int nPt);
	
protected:
	BOOL  m_bTrimOut;
	BOOL  m_bNotChangeZ;
	CString m_strRefLay;
	CString m_strRetLay;
	CString m_strTrimLay;
};

// 顶点加密
class CInterpolateVertexsCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CInterpolateVertexsCommand)
public:
	CInterpolateVertexsCommand();
	virtual ~CInterpolateVertexsCommand();
	virtual CString Name();
	virtual CString AccelStr(){
		return _T("InterpolateVertexs");
	}
	static CCommand* Create(){
		return new CInterpolateVertexsCommand;
	}
	
	void Start();
	void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
protected:	
	CGeometry* Process(CGeometry *pObj);
	
protected:
	CArray<FTR_HANDLE,FTR_HANDLE> m_FtrHandles;
	double m_lfMaxLen;
};

class CMoveClosePtCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMoveClosePtCommand)
public:
	enum
	{
		modeSinglePoint = 0,
		modeLinesection = 1
	};
	CMoveClosePtCommand();
	virtual ~CMoveClosePtCommand();
	virtual CString Name();
	
	static CCommand* Create(){
		return new CMoveClosePtCommand;
	}
	virtual CString AccelStr(){
		return _T("MoveClosePt");
	}

	void PtMove(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
//	void KeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
protected:		
	int MyGetFirstPointOfIntersect(PT_3DEX *pts,int num,PT_3D pt1,PT_3D pt2,PT_3D *ret);
	int MyFindKeyPosOfObj(PT_3D pt, CGeometry *pObj);
	
private:
	int m_nMode;//0单点，1线段
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
};

#define PID_LINEFILL				_T("LineFill")
#define PF_FILLTYPE					_T("FillType")
#define PF_FILLINTV					_T("FillIntv")
class CLineFillCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CLineFillCommand)
public:
	CLineFillCommand();
	virtual ~CLineFillCommand();
	static CCommand* Create(){
		return new CLineFillCommand;
	}
	virtual CString AccelStr(){
		return _T("LineFill");
	}
	void Abort();
	void PtClick(PT_3D &pt, int flag);
	void Start();
	virtual CString Name();		
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
protected:
	virtual void Finish();
	virtual void UpdateParams(BOOL bSave);

	int m_nFillType;  // 横、竖、横+竖、左斜、右斜、左斜+右斜
	double m_fFillIntv;
};

class CColorFillCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CColorFillCommand)
public:
	CColorFillCommand();
	virtual ~CColorFillCommand();
	static CCommand* Create(){
		return new CColorFillCommand;
	}
	virtual CString AccelStr(){
		return _T("ColorFill");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
};

//删除晕线
class CDelHachureCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDelHachureCommand)
public:
	enum
	{
		modeSingle = 0,	//单个
		modeAll = 1		//全图
	};
	CDelHachureCommand();
	virtual ~CDelHachureCommand();
	static CCommand* Create(){
		return new CDelHachureCommand;
	}
	virtual CString AccelStr(){
		return _T("DelHachure");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
protected:
	CGeometry *FindSurroundObj(CGeometry *pObj);
	BOOL IsSurrounded(CGeometry *pObj_inside, CGeometry *pObj_outside);

	int m_nMode;
};

//用圆点替换晕线填充的圆圈
class CReplaceWithPointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CReplaceWithPointCommand)
public:
	CReplaceWithPointCommand();
	virtual ~CReplaceWithPointCommand();
	static CCommand* Create(){
		return new CReplaceWithPointCommand;
	}
	virtual CString AccelStr(){
		return _T("ReplaceWithPoint");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
};


class CSetBoundByDrawCommand : public CDrawCommand
{
public:
	CSetBoundByDrawCommand();
	virtual ~CSetBoundByDrawCommand();

	DECLARE_DYNCREATE(CSetBoundByDrawCommand)
public:
	void GetParams(CValueTable& tab);
	static CCommand *Create(){
		return new CSetBoundByDrawCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawBound");
	}
	virtual CGeometry *GetCurDrawingObj();
	virtual CString Name();
	virtual void Back();
	void PtClick(PT_3D &pt, int flag);
	void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	virtual void Start();	
	virtual void Abort();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual int  GetCurPenCode();
	virtual CProcedure *GetActiveSonProc(int MsgType);
protected:
	virtual void Finish();
	virtual void UpdateParams(BOOL bSave);	

	CDrawCurveProcedure *m_pDrawProc;
};


class CModifyDemPointCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CModifyDemPointCommand)
public:
	CModifyDemPointCommand();
	static CCommand *Create(){
		return new CModifyDemPointCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyDemPoint");
	}
	virtual ~CModifyDemPointCommand();
	virtual CString Name();	
	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtDblClick(PT_3D &pt, int flag);
	virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	virtual DrawingInfo GetCurDrawingInfo();
	virtual int  GetCurPenCode();
	
	virtual void PtReset(PT_3D &pt);
	
protected:
	virtual void Finish();

	FTR_HANDLE FindNearestVertex(PT_3D pt, int& index);
	BOOL GetBTofBound(double x, double &ymin, double& ymax);
	
	int m_nStep;
	int   m_nBoundType;		//0, 工作区，1,选择
	double m_lfDX, m_lfDY;
	
	CArray<PT_3DEX,PT_3DEX> m_arrBound;
	
	// 是否朝上走（Y 正向）
	BOOL m_bUp;

	bool m_bAutoGo;
};

class CMapManualMatchCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMapManualMatchCommand)
public:	
	
	CMapManualMatchCommand();
	virtual ~CMapManualMatchCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("MapManualMatch");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CMapManualMatchCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	CFeature *m_pObjOld0;
	CFeature *m_pObjOld1;
	CFeature *m_pObjNew;
	// 线
	int   m_ptNum0;
	int   m_ptNum1;
	// 面
	PT_3DEX m_SelPts0[3], m_SelPts1[3];
protected:
	CString m_strBoundLayer;
	int   m_nJoinMode;//0，在接边处断开；1，在接边处连通
	BOOL m_bMatchHeight;
	BOOL m_bSurfaceMode;
};

class CMapMatchCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMapMatchCommand)
public:
	struct itemEdge
	{
		CFeature *pFtr; //地物，有一个端点在接边缓冲区内
		itemEdge *pLinkItem;//在线上连通选项实现时用做临时存储
		BOOL bVisit; //特殊用途，双端点相接的情况
		int pos; //节点序号
		PT_3DEX ptex;
		PT_3DEX perpendicular;//垂足
		int idx;//垂足所在接边线线段的索引
		double dis;//垂足所在接边线线段的位置
		int nClsType;//*ppFtr的类型,区分面和线
	};
	struct itemNode
	{
		CFeature *pSurface;
		int pos1;
		CFeature *pCurve;
		int pos2; //节点序号
		bool operator==(const itemNode &x) const
		{
			return (pSurface==x.pSurface && pos1==x.pos1);
		}
	};
public:	
	void GetFtrCodes(CStringArray& names);
	CMapMatchCommand();
	virtual ~CMapMatchCommand();
	static CCommand* Create(){
		return new CMapMatchCommand;
	}
	virtual CString AccelStr(){
		return _T("MapMatch");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	BOOL UpdateMark(BOOL bMark=FALSE);
protected:	
	virtual void Finish();
	void InsertPt(CFtrArray& arrFtrs,CUndoBatchAction *pUndo);
	void GetMatchItem(CFtrArray& arrFtrs, CArray<itemEdge*,itemEdge*>& arrItems, CArray<itemNode,itemNode>& arrNodes);
	void CreateNewObjBreak(itemEdge *pItem, itemEdge *pItem0,PT_3DEX& ret,CUndoBatchAction* pUndo);
	BOOL GetIntersectPt(PT_3DEX pt1, PT_3DEX pt2, PT_3DEX *ret1);
	void MatchItem(CArray<itemEdge*,itemEdge*>& arrItems);
	void SnapSurfaceNode(CArray<itemNode,itemNode>& arrNodes, CUndoBatchAction* pUndo);
	
private:
	float m_fJoinToler;
	float m_lfZToler;
//	int   m_nScopeType;//0，当天激活数据源；1，所有数据源
	int   m_nJoinMode;//0，在接边处断开；1，在接边处连通
	int   m_nModifyMode;//0,全部 1,只改本地图幅 2，只改外部图
	BOOL  m_bMatchColor;
	BOOL  m_bMatchLineType;
//	BOOL  m_bMatchLineWidth;
	BOOL  m_bMatchLineHei;
	BOOL  m_bMark;//是否显示标识
	
	CString m_strLayNameOfTKName;//图廓所在的图层
	CStringArray m_sMapName; //所有的图幅名
	CString m_sSelectMap,m_sLastSlectMap;//当前所选图幅
	CArray<CGeometry*,CGeometry*> m_arrGeoCurve;
	CStringArray m_arrSelectmap;
	FTR_HANDLE m_handleBound;
};

class CMapMatchConnectCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMapMatchConnectCommand)
public:
	struct OBJ_ITEM1
	{
		OBJ_ITEM1(){
			pLayer = NULL;
			pFtr = NULL;
			segid = -1;
			pds = NULL;
			position = 0;
		}
		CFtrLayer *pLayer;
		CFeature *pFtr;
		PT_3DEX pt;
		int segid;
		int position;//1图廓线右侧，-1图廓线左侧，2图廓线上侧，-2图廓线下侧
		CDlgDataSource *pds;
	};
	struct OBJ_ITEM2 : public OBJ_ITEM1
	{
		OBJ_ITEM2(){
			pLayer = NULL;
			pFtr = NULL;
			segid = -1;
			pds = NULL;
			position = 0;
		}
		PT_3DEX pt1;
	};
public:	
	CMapMatchConnectCommand();
	virtual ~CMapMatchConnectCommand();
	static CCommand* Create(){
		return new CMapMatchConnectCommand;
	}
	virtual CString AccelStr(){
		return _T("MapMatch2");
	}
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	virtual CString Name();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:	
	virtual void Finish();
	BOOL MergeCurves(CFtrArray& ftrs);
	BOOL MergeSurface(CFtrArray& ftrs);
	void MakeAttributeSame(CFtrArray& ftrs, CUndoModifyProperties& undo);
	
private:
	int   m_nJoinMode;//0，在接边处断开；1，在接边处连通
	CString m_strLayNameOfTKName;//图廓所在的图层
};

class CDlgDrawTestPointCommand : public CDlgDrawPointCommand  
{
	DECLARE_DYNCREATE(CDlgDrawTestPointCommand)
public:
	void OnSonEnd(CProcedure *son);
	virtual void Start();
	virtual CString Name();
	CDlgDrawTestPointCommand();
	virtual ~CDlgDrawTestPointCommand();
	static CCommand* Create(){
		return new CDlgDrawTestPointCommand;
	}
	virtual CString AccelStr(){
		return _T("DrawTestPoint");
	}
protected:
	void Finish();
};

#define PF_FIXTOEXT_LAYER            _T("FixToExt_Layer")
#define PF_FIXTOEXT_OPTION			 _T("FixToExt_Option")
#define PF_FIXTOEXT_FIELD            _T("FixToExt_Field")

class CFixAttriToExtAttriCommand : public CEditCommand  
{	
	DECLARE_DYNCREATE(CFixAttriToExtAttriCommand)
public:
	enum
	{
		dirPntDir = 0,
		parrelWidth,
		layerCode,
		elevation,
		length,
		area,
		text,
		bigao//比高
	};
	CFixAttriToExtAttriCommand();
	virtual ~CFixAttriToExtAttriCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("FixAttriToExtAttri");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CFixAttriToExtAttriCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	BOOL CheckFieldValid(CValueTable& tab, LPCTSTR field);
	
protected:
	CString m_strLayerCodes;
	int m_nFieldOption;
	CString m_strFieldName;

	BOOL m_bNotCoverValidValue;
};



class CExtAttriToFixAttriCommand : public CFixAttriToExtAttriCommand  
{	
	DECLARE_DYNCREATE(CExtAttriToFixAttriCommand)
public:
	CExtAttriToFixAttriCommand();
	virtual ~CExtAttriToFixAttriCommand();
	virtual CString AccelStr(){
		return _T("ExtAttriToFixAttri");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CExtAttriToFixAttriCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	BOOL m_bOnlyUseValidValue;
};


#define		PF_CAL_INDEX_CONTOUR_CODE        _T("pf_index_contour_code")
#define     PF_CAL_STD_CONTOUR_CODE			 _T("pf_standard_contour_code")
#define     PF_CAL_STD_CONTOUR_CNT	         _T("pf_standard_contour_cnt")
#define     PF_CAL_CONTOUR_INTERVAL          _T("pf_contour_interval")

class CContourAutoLayerSettingCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CContourAutoLayerSettingCommand)
public:
	CContourAutoLayerSettingCommand();
	virtual ~CContourAutoLayerSettingCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("ContourAutoLayerSetting");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CContourAutoLayerSettingCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	CString m_strHandleLayer;
	CString  m_strIdxContourCode;
	CString  m_strStdContourCode;
	double  m_lfContourInterval;
	int  m_nStdContourCnt;
};

class CContourAutoRepairCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CContourAutoRepairCommand)
public:
	CContourAutoRepairCommand();
	virtual ~CContourAutoRepairCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("ContourAutoRepair");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CContourAutoRepairCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	
protected:
	CString  m_strIdxContourCode;
	CString  m_strStdContourCode;
	double  m_lfContourInterval;
//	int  m_nStdContourCnt;
	
	
};


//指定独立符号命令
class CSetSpecliaSymbolCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSetSpecliaSymbolCommand)
public:
	CSetSpecliaSymbolCommand();
	virtual ~CSetSpecliaSymbolCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("SetSpecliaSymbolCommand");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CSetSpecliaSymbolCommand;
	}
	
//	virtual void Finish();
	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);
	void SetSpecialSymbol(CString StrSymbolName);
	
protected:

	CDlgSetSpecialSymbol m_SpecialSymbolDlg;
	CString m_SymbolName;
	
};



#define  PF_CONTOUREVA_STARTZ    _T("ContourEvaluateStartZ")
#define  PF_CONTOUREVA_HEIGHTMODE    _T("ContourEvaluateHeightMode")
class CContourEvaluateCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CContourEvaluateCommand)
public:
	CContourEvaluateCommand();
	virtual ~CContourEvaluateCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("ContourEvaluate");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CContourEvaluateCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void PtMove(PT_3D &pt);
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	
protected:
// 	CString  m_strIdxContourCode;
// 	CString  m_strStdContourCode;
	double  m_lfContourInterval;
	double  m_fStartHeight;
	// 高程改变方向，0：依次增加，1：依次减少
	int    m_nHeightMode;  

	PT_3D m_ptLine[2];	
};

class CMapDecorateCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMapDecorateCommand)
public:
	enum
	{
		opNone = 0,
		opSelectBound = 1,
		opSelectCorner = 2
	};
	CMapDecorateCommand();
	virtual ~CMapDecorateCommand();
	virtual CString AccelStr(){
		return _T("MapDecorate");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CMapDecorateCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
protected:
	CMapDecorateDlg* m_pDlg;
	int m_nOperation;

	BOOL m_bSortBound;

public:
	static BOOL m_bStaticValid;
	static MapDecorate::CMapDecorator m_MapDecorator;
};



class CBatCreateRectMapBorderCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CBatCreateRectMapBorderCommand)
public:
	CBatCreateRectMapBorderCommand();
	virtual ~CBatCreateRectMapBorderCommand();
	virtual CString AccelStr(){
		return _T("BatCreateRectMapBorder");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CBatCreateRectMapBorderCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void CreateMapBorders();
protected:
	void CreateMapBorder(MapDecorate::CMapDecorator* pMapDecorator, PT_3D *pts, CFtrHArray *pArr); // pts-> PT_3D[4]	
	
	float m_fWid, m_fHei;
	CArray<PT_3DEX,PT_3DEX> m_arrBoundLines;

	//是否外扩一个图幅
	BOOL m_bExtendMap;
};



class CBatCreateInclinedMapBorderCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CBatCreateInclinedMapBorderCommand)
public:
	CBatCreateInclinedMapBorderCommand();
	virtual ~CBatCreateInclinedMapBorderCommand();
	virtual CString AccelStr(){
		return _T("BatCreateInclinedMapBorder");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CBatCreateInclinedMapBorderCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void CreateMapBorders();
protected:
	//ptx是矩形右侧边中点, 该函数用来获得矩形边界，结果存放在 ret_pts 中；
	//type=0时, pt0是矩形左上角点；type=1时，pt0是矩形坐下角点；
	//结果矩形的第一个点是左上角点，点列呈逆时针方向；
	void GetBorderPts(int type, PT_3D pt0, PT_3D ptx, double r, PT_3D *ret_pts);

	void CreateMapBorder(MapDecorate::CMapDecorator *pMapDecorator, PT_3D *pts, CFtrHArray *pArr); // pts-> PT_3D[4]	

	//计算矩形右侧边中点的坐标；pt0 是当前矩形与上一个矩形的公共角点；r 是半径（r*r=wid*wid+(hei/2)*(hei/2)）；len 是上一个矩形的右侧中点在基线上的里程
	//计算得到的当前矩形右侧边中点的里程应该大于len，如果不满足此条件，就不合理；如果不存在这样的点，FindPtX就返回失败（意味着基线走完了）
	//pts是基线节点，lens 是各个节点的累加长度; 找到ret后，len会被更新;
	BOOL FindPtX(PT_3D pt0, double r, double& len, PT_3D *pts, int npt, CArray<double,double>& lens, PT_3D& ret);
	
	float m_fWid, m_fHei;
	CArray<PT_3DEX,PT_3DEX> m_arrBoundLines;
};


class CBatCreateRoadMapBorderCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CBatCreateRoadMapBorderCommand)
public:
	CBatCreateRoadMapBorderCommand();
	virtual ~CBatCreateRoadMapBorderCommand();
	virtual CString AccelStr(){
		return _T("BatCreateRoadBorder");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CBatCreateRoadMapBorderCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void CreateMapBorders();

protected:
	void CreateMapBorder(MapDecorate::CMapDecorator *pMapDecorator, PT_3D *pts, CFtrHArray *pArr); // pts-> PT_3D[4]	
	int GetPosPt(PT_3D *pts, int npt, CArray<double,double>& lens, double len, PT_3D& ret);

	//pt0,pt1分别是矩形的左右侧边的中点
	void GetMapBorder(PT_3D pt0, PT_3D pt1, PT_3D *ret_pts);
	void CreatePartingLine(PT_3D pt0, PT_3D dir0, PT_3D pt1, PT_3D dir1, int type, double len, CFtrHArray *arr);
	
	float m_fWid, m_fHei;
	float m_fMileageInterval;
	double m_lfMileageStart;
	float m_fMileageTextSize;
	CArray<PT_3DEX,PT_3DEX> m_arrBoundLines;
};


class CBatExportMapsCommand : public CEditCommand
{
public:
	enum{
		typeTextInBound = 0, //使用图幅边界内的文字作为图幅名
		typeAutoNumber = 1
	};
	//找相邻图幅的图名时使用（GetNearMapName中）
	struct NameItem
	{
		NameItem(){
			pGeo = NULL;
			dis = -1;
		}
		CGeoText *pGeo;
		PT_3D pt0;
		double dis;
		char name[128];
	};

	CBatExportMapsCommand();
	virtual ~CBatExportMapsCommand();

	virtual CString AccelStr(){
		return _T("BatExportMaps");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CBatExportMapsCommand;
	}

	virtual void Start();

	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

	void MyFillShowParams(CUIParam* param, BOOL bForLoad );

	static BOOL TrimObj(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim, BOOL bClosedCurveAsSurface=FALSE, BOOL bSurfaceAsCurve=FALSE);
	static void AddFtrsToDS(CFtrArray& arr, CFtrLayer *pLayer0, CDlgDataSource *pDS0, CDlgDataSource *pDS1, CValueTable& xTab, CFtrArray *pRetArr=NULL);

	CString GetMapName(PT_3D pts[4], LPCTSTR layName);

	static BOOL GetNearMapName(CDlgDataSource *pDS, PT_3D pt0, float ang, LPCTSTR layname, CString names[9]);

	CDlgDataSource *OpenFDB(LPCTSTR fileName, PT_3D pts[4], CSQLiteAccess *& pAccess);
	void ExportMap(LPCTSTR fileName, CFeature *pFtr, PT_3D boundPts[4], MapDecorate::CMapDecorator *pMapDecorator);

protected:
	int m_nNameType;
	int m_nAutoNum;
	CString m_strLayNameOfBound;
	CString m_strLayNameOfMapName;
	CString m_strLayNameOfMapNum;

	CString m_strPathName;

	BOOL  m_bTrimClosedCurve;
	BOOL  m_bTrimSurface;
	int	m_nNearmapContent;
	BOOL m_bExportMapDecorator;//是否导出图廓
};


//批量分幅输出 dxf 
class CBatExportMapsCommand_dxf : public CEditCommand
{
public:
	enum{
		typeTextInBound = 0, //使用图幅边界内的文字作为图幅名
		typeAutoNumber = 1
	};
	//找相邻图幅的图名时使用（GetNearMapName中）
	struct NameItem
	{
		NameItem(){
			pGeo = NULL;
			dis = -1;
		}
		CGeoText *pGeo;
		PT_3D pt0;
		double dis;
		char name[128];
	};
	
	CBatExportMapsCommand_dxf();
	virtual ~CBatExportMapsCommand_dxf();
	
	virtual CString AccelStr(){
		return _T("BatExportMaps_dxf");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CBatExportMapsCommand_dxf();
	}
	
	virtual void Start();
	
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	
	void MyFillShowParams(CUIParam* param, BOOL bForLoad );
	
	static BOOL TrimObj(CGeometry *pObj, CPtrArray *pArray, Envelope evlp, PT_3D *polys, int polysnum, CGrTrim *ptrim);
	static void AddFtrsToDS(CFtrArray& arr, CFtrLayer *pLayer0, CDlgDataSource *pDS0, CDlgDataSource *pDS1, CFtrArray *pRetArr=NULL);
	
	CString GetMapName(PT_3D pts[4]);
	
	BOOL GetNearMapName(CDlgDataSource *pDS, PT_3D pt0, float ang, CString names[9]);
	
	void ExportMap(LPCTSTR fileName, CFeature *pFtr, PT_3D boundPts[4]);
	
protected:
	int m_nNameType;
	int m_nAutoNum;
	CString m_strLayNameOfBound;
	CString m_strPathName;
	CString m_strLayNameOfMapNum;
	int m_nFileFormat;
		
	BOOL m_bSymbolized;
	BOOL m_bAsLayerCode;
};


class CBatCreateTrapezoidMapBorderCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CBatCreateTrapezoidMapBorderCommand)
public:
	CBatCreateTrapezoidMapBorderCommand();
	virtual ~CBatCreateTrapezoidMapBorderCommand();
	virtual CString AccelStr(){
		return _T("BatCreateTrapezoidMapBorder");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CBatCreateTrapezoidMapBorderCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad );
	void CreateMapBorders();

	virtual void Finish();
	virtual void Abort();
protected:
	void CreateMapBorder(LPCTSTR name, PT_3D *pts, CFtrHArray *pArr); // pts-> PT_3D[4]	
	
	CArray<PT_3DEX,PT_3DEX> m_arrBoundLines;

	BOOL m_bExtendMap;
};




class CMapDecorateAutoSetNearmapFromFile : public CCommand
{
	DECLARE_DYNCREATE(CMapDecorateAutoSetNearmapFromFile)
public:
	struct NameObjInfo
	{
		CFeature *pFtr;
		PT_3D pt0;
	};
	CMapDecorateAutoSetNearmapFromFile();
	virtual ~CMapDecorateAutoSetNearmapFromFile();
	
	virtual CString AccelStr(){
		return _T("AutoSetNearmapFromFile");
	}
	
	static CCommand* Create(){
		return new CMapDecorateAutoSetNearmapFromFile;
	}
	virtual CString Name();

	virtual void Start();	
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	void MyFillShowParams(CUIParam* param, BOOL bForLoad );
	virtual void GetParams(CValueTable& tab);

protected:
	void GetNamesFromFile(LPCTSTR fileName, PT_3D pt, CString names[8]);
	void FindNameObjects(CFtrArray& arr, CFtrArray& arr2);
	PT_3D GetInnerBoundCenter();
	CDlgDataSource *OpenFDB(LPCTSTR fileName);

	CString m_strPathName, m_strLayNameOfMapName;
	CString m_strLayNameOfNearmap;
	CString m_strLayNameOfMapNameCorner;
};


class CConvertBySelectPtsCommand : public CCommand
{
public:
	CConvertBySelectPtsCommand();
	virtual ~CConvertBySelectPtsCommand();
	
	virtual CString AccelStr(){
		return _T("ConvertBySelectPts");
	}
	
	static CCommand* Create();
	virtual CString Name();
	
	void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);
	void PtReset(PT_3D& pt);
	void Abort();
	void Start();
	
protected:
	int m_nStep;
	int m_nIndex;
	CArray<PT_3D,PT_3D> m_pts;
	CFtrArray m_ftrs;
};

#define PF_LIMITED _T("Limited")
// 删除短线
class CDeleteShortCurveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDeleteShortCurveCommand)
public:
	CDeleteShortCurveCommand();
	virtual ~CDeleteShortCurveCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("DeleteShortCurve");
	}
	void Start();
	void Abort();
	void Finish();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	
	virtual CProcedure *GetActiveSonProc(int nMsgType);	
	virtual void PtReset(PT_3D &pt);
	virtual void OnSonEnd(CProcedure *son);
	
private:
	BOOL FilterFeature(LONG_PTR id);
	void CheckObj(CFeature *pFtr, CCPResultList *result);
	void MarkPt(PT_3D pt);
	
protected:
	double m_lfLimit;
	
	// 检查范围
	int   m_nBoundType; //0, 全图，1,绘制
	
	CDrawCurveProcedure *m_pDrawProc;
	CGeometry *m_pGeoCurve;
};


//删除重叠线
class CDeleteOverlapLinesCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDeleteOverlapLinesCommand)
public:
	CDeleteOverlapLinesCommand();
	virtual ~CDeleteOverlapLinesCommand();
	virtual CString Name();
	static CCommand* Create();
	virtual CString AccelStr(){
		return _T("DeleteOverlapLines");
	}
	void Start();
	void Abort();
	void Finish();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	
	
protected:
	void ProcessAll();
	void ProcessOne(CPFeature pFtr);
	
protected:
	CString m_strLayers;
	
	// 范围
	int   m_nBoundType; //0, 全图，1,选择
	BOOL  m_bCheckZ;//考虑z值
};




//视图旋转命令
class CManualRotateViewCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CManualRotateViewCommand)
public:
	CManualRotateViewCommand();
	virtual ~CManualRotateViewCommand();
	static CCommand* Create(){
		return new CManualRotateViewCommand;
	}
	virtual CString AccelStr(){
		return _T("ManualRotateView");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

protected:
	PT_3D m_ptClick;
};



class CRefFileCopyAllCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CRefFileCopyAllCommand)
public:
	enum
	{
		typeWSBound = 0,
		typeSelect = 1,
		typeDraw = 2
	};
	CRefFileCopyAllCommand();
	virtual ~CRefFileCopyAllCommand();
	static CCommand* Create(){
		return new CRefFileCopyAllCommand;
	}
	virtual CString AccelStr(){
		return _T("RefFileCopyAll");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Abort();

	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);

	virtual void OnSonEnd(CProcedure *son);
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	virtual void PtReset(PT_3D &pt);

protected:
	void DoCopy(PT_3D *boundPts, int npt);

	CString m_strRefFile;
	BOOL  m_bClip;		//是否裁剪
	int   m_nBoundType; //裁剪的边界类型：0, 工作区为边线，1,选择边线，2，绘制

	CDrawCurveProcedure *m_pDrawProc;
	CArray<PT_3D,PT_3D> m_arrBoundPts;
};



class CAlignCommand : CEditCommand
{
	DECLARE_DYNCREATE(CAlignCommand)
public:
	enum
	{
		Left = 0,		//左对齐
		Right = 1,		//右对齐
		Up = 2,			//上对齐
		Down = 3,		//下对齐
		CenterH = 4,		//中心水平对齐
		CenterV = 5		//中心垂直对齐
	};
	struct ObjInfo
	{
		CFeature *pFtr;
		CGeometry *pGeo;
		Envelope e;
		PT_3D center;
	};
	CAlignCommand();
	virtual ~CAlignCommand();
	void Abort();
	virtual CString AccelStr(){
		return _T("Align");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CAlignCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	int m_nAlign;
	
};



class  CDlgEditDirPointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDlgEditDirPointCommand)
public:
	CDlgEditDirPointCommand();
	virtual ~CDlgEditDirPointCommand();
	static CCommand *Create(){
		return new CDlgEditDirPointCommand;
	}
	virtual CString AccelStr(){
		return _T("EditDirPoint");
	}
	virtual CString Name();
	
	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag); 
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);	
	
	virtual void PtMove(PT_3D &pt);

protected:
	PT_KEYCTRL FindNearestKeyCtrlPt(CGeoPoint *pGeo, PT_3D spt, double r, CCoordSys *pCS);
	void DrawCtrlPts(CGeoPoint *pGeo);
	
protected:
	CPermanentExchanger *m_pSel;

	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
	
	BOOL  m_bSynchXY;

	PT_KEYCTRL m_ptCtrl;
	CPFeature m_pFtr;
};


class CCopyAssistLineCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCopyAssistLineCommand)
public:
	CCopyAssistLineCommand();
	virtual ~CCopyAssistLineCommand();
	static CCommand* Create(){
		return new CCopyAssistLineCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyAssistLine");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

protected:
	void ProcessObj(CFeature *pFtr, CDlgDoc *pDoc, CFtrLayer *pLayer, CUndoFtrs& undo);
	
protected:
	CString m_strLayerSrc;
	CString m_strLayerDest;
};



class CCopyBaselineCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCopyBaselineCommand)
public:
	CCopyBaselineCommand();
	virtual ~CCopyBaselineCommand();
	static CCommand* Create(){
		return new CCopyBaselineCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyBaseline");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	void ProcessObj(CFeature *pFtr, CDlgDoc *pDoc, CFtrLayer *pLayer, CUndoFtrs& undo);
	
protected:
	CString m_strLayerSrc;
	CString m_strLayerDest;
};


//by mzy 自动调整平行线中心线
#define CADJUST_CL_PARA	_T("adjust_cl_parallel_layer")
#define CADJUST_CL_CL	_T("adjust_cl_center_line_layer")
class CAutoAdjustParallelCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CAutoAdjustParallelCommand)
public:	
	CAutoAdjustParallelCommand();
	virtual ~CAutoAdjustParallelCommand();
	static CCommand* Create(){
		return new CAutoAdjustParallelCommand;
	}
	virtual CString AccelStr(){
		return _T("AdjustCenterLine");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void PtMove(PT_3D &pt);	

	void Abort();
	void Start();
	virtual void Finish();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	CString m_strParallelCode;
	CString m_strCenterLineCode;
protected:
	//判断当前平行线附近是否存在符合要求的中心线，若有，获取它的指针
	BOOL IsCenterLineValid(CFeature* pFtr, CFeature*& pCenterLine);

	//判断线pLine在平行线内包裹的长度是否达80%
	BOOL IsMostWraped(CFeature* pLine, CArray<PT_3DEX,PT_3DEX>& para_pts);
private:
	double GetWrapedLength(PT_3DEX p1, PT_3DEX p2, PT_3DEX* pts, int n);	//计算线段P1P2被多边形包裹的长度
};
//


//-----------------------------------------------------
//图廓整饰--------新版
class CMapDecorateCommand_New : public CEditCommand  
{
	DECLARE_DYNCREATE(CMapDecorateCommand_New)
public:
	enum
	{
		opNone = 0,
		opSelectBound = 1,
		opSelectCorner = 2
	};
	CMapDecorateCommand_New();
	virtual ~CMapDecorateCommand_New();
	virtual CString AccelStr(){
		return _T("MapDecorate");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CMapDecorateCommand_New;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);	

protected:
	CMapDecorateDlg* m_pDlg;
	int m_nOperation;

	BOOL m_bSortBound;
public:
	static BOOL m_bStaticValid;
	static MapDecorate::CMapDecorator m_MapDecorator;
};


class CMakeCheckPtSampleCommand : CEditCommand
{
	DECLARE_DYNCREATE(CMakeCheckPtSampleCommand)

	CMakeCheckPtSampleCommand(){}
	virtual ~CMakeCheckPtSampleCommand(){}
	virtual CString AccelStr(){
		return _T("MakeCheckPtSample");
	}
	virtual CString Name();

	static CCommand* Create(){
		return new CMakeCheckPtSampleCommand;
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
};

//替换小房屋
class CInsteadLittleHouseCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CInsteadLittleHouseCommand)
public:
	CInsteadLittleHouseCommand();
	virtual ~CInsteadLittleHouseCommand();
	static CCommand* Create(){
		return new CInsteadLittleHouseCommand;
	}
	virtual CString AccelStr(){
		return _T("InsteadLittleHouse");
	}
	virtual void PtClick(PT_3D &pt, int flag);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();
	virtual void Finish();

protected:
	CString m_HandleLayer;
	CString m_StoreLayer;
	double m_areaLimit;//面积限制
	double m_longLimit;//长边限制
	double m_shortLimit;//短边限制

	BOOL BInstead(CGeometry* pGeo, int clsType, CArray<PT_3DEX, PT_3DEX>& retPts);
};

//涵洞反向
class CCulvertReverseCommand : public CEditCommand
{
    DECLARE_DYNCREATE(CCulvertReverseCommand)
public:
	CCulvertReverseCommand();
	~CCulvertReverseCommand();
	static CCommand* Create(){
		return new CCulvertReverseCommand;
	}
	virtual CString AccelStr(){
		return _T("CulvertReverse");
	}
	virtual void PtClick(PT_3D &pt, int flag);	
	void Abort();
	void Start();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual CString Name();
	virtual void Finish();

protected:
	BOOL m_Reverse;
	BOOL m_linetypeReverse;
};

// 双线转面
class CDoubleLinesToSurfaceCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDoubleLinesToSurfaceCommand)
public:
	CDoubleLinesToSurfaceCommand();
	virtual ~CDoubleLinesToSurfaceCommand();
	static CCommand* Create(){
		return new CDoubleLinesToSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("DoubleLinesToSurface");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void DoubleLineToSurface(const FTR_HANDLE* handles,int num);
protected:
	CString m_targetLayer;
};

// 双线转面(全图)
class CDoubleLinesToSurfaceAllCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDoubleLinesToSurfaceAllCommand)
public:
	CDoubleLinesToSurfaceAllCommand();
	virtual ~CDoubleLinesToSurfaceAllCommand();
	static CCommand* Create(){
		return new CDoubleLinesToSurfaceAllCommand;
	}
	virtual CString AccelStr(){
		return _T("DoubleLinesToSurfaceAll");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
private:
	void ReadFileStrings();

	CString m_handleLayers;
	CString m_targetLayer;
};

// 将双线的辅助线反向
class CReverseAssistlineCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CReverseAssistlineCommand)
public:
	CReverseAssistlineCommand();
	virtual ~CReverseAssistlineCommand();
	static CCommand* Create(){
		return new CReverseAssistlineCommand;
	}
	virtual CString AccelStr(){
		return _T("ReverseAssistline");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
};



//内插高程点
class CInterpolateHeiPointCommand : public CCommand
{
	DECLARE_DYNCREATE(CInterpolateHeiPointCommand)
public:
	CInterpolateHeiPointCommand();
	virtual ~CInterpolateHeiPointCommand();
	virtual CString Name();
	static CCommand* Create(){
		return new CInterpolateHeiPointCommand;
	}
	virtual CString AccelStr(){
		return _T("InterpolateHeiPoint");
	}
	
	virtual int  GetState();
	void PtReset(PT_3D &pt);
	void PtMove(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void Start();
	void Abort();
	CProcedure *GetActiveSonProc(int nMsgType);	
	
	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	virtual void Finish();

	void Do(PT_3D *polyPts, int nPt);
	//三维测图时
	void DoOSG(CView * pOsgbView,PT_3D *polyPts, int nPt);
	
protected:
	int	  m_nStep;
	
	CString m_strLayerHeiPoint;
	//内插点间距
	float m_fGridSize;
	//随机范围
	float m_fRandRange;
	
	CDrawCurveProcedure *m_pDrawProc;
	CFeature *m_pFtr;
	
	CArray<FTR_HANDLE,FTR_HANDLE> m_idsOld;
	CArray<FTR_HANDLE,FTR_HANDLE> m_idsNew;
};

class CSelectLayerCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSelectLayerCommand)
public:	
	CSelectLayerCommand();
	virtual ~CSelectLayerCommand();
	static CCommand *Create(){
		return new CSelectLayerCommand;
	}
	virtual CString AccelStr(){
		return _T("SelectLayer");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
};

// 面转对称双线
class CSurfaceToDoubleLinesCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CSurfaceToDoubleLinesCommand)
public:
	CSurfaceToDoubleLinesCommand();
	virtual ~CSurfaceToDoubleLinesCommand();
	static CCommand* Create(){
		return new CSurfaceToDoubleLinesCommand;
	}
	virtual CString AccelStr(){
		return _T("SurfaceToDoubleLines");
	}
	
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
private:
	CString m_handleLayers;
	int     m_pos;
};



// 面转对称双线
class CSurfaceToDoubleLinesCommand_One : public CEditCommand
{
	DECLARE_DYNCREATE(CSurfaceToDoubleLinesCommand_One)
public:
	CSurfaceToDoubleLinesCommand_One();
	virtual ~CSurfaceToDoubleLinesCommand_One();
	static CCommand* Create(){
		return new CSurfaceToDoubleLinesCommand_One;
	}
	virtual CString AccelStr(){
		return _T("SurfaceToDoubleLines_One");
	}
	
	virtual CString Name();
	
	virtual void Start();	
	virtual void PtMove(PT_3D &pt);
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Abort();
protected:
	CGeometry *Convert(CGeometry *pObj0);
	
protected:
	PT_3D m_ptStart, m_ptEnd;
	CPFeature m_pFtr;
};

//自动合成首尾衔接的双线
class CMergeCurveFACommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CMergeCurveFACommand)
public:	
	struct FeaItem
	{
		FeaItem()
		{
			pFtr = NULL;
			pObj = NULL;
		}
		CFeature* pFtr;
		CGeometry* pObj;
		PT_3DEX sp;
		PT_3DEX ep;
	};
	CMergeCurveFACommand();
	virtual ~CMergeCurveFACommand();
	static CCommand* Create(){
		return new CMergeCurveFACommand;
	}
	virtual CString AccelStr(){
		return _T("MergeCurveFA");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
    CString m_lineLayer1;
	CString m_lineLayer2;
	CString m_DCurveLayer;
};

//辅助线另存为
class CSaveAssistLineCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSaveAssistLineCommand)
public:	
	CSaveAssistLineCommand();
	virtual ~CSaveAssistLineCommand();
	static CCommand* Create(){
		return new CSaveAssistLineCommand;
	}
	virtual CString AccelStr(){
		return _T("SaveAssistLine");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	CString m_HandleLayer;
	CString m_StoreLayer;
};

//删除植被面内的地类界
class CDeleteBorderInVEGACommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CDeleteBorderInVEGACommand)
public:	
	CDeleteBorderInVEGACommand();
	virtual ~CDeleteBorderInVEGACommand();
	static CCommand* Create(){
		return new CDeleteBorderInVEGACommand;
	}
	virtual CString AccelStr(){
		return _T("DeleteBorderInVEGA");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
protected:
	CString m_borderLayer;
	CString m_VEGALayers;
};

//删除线周围的点
class CDelPtAroundCurveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDelPtAroundCurveCommand)
public:
	CDelPtAroundCurveCommand();
	virtual ~CDelPtAroundCurveCommand();
	void Abort();
	
	virtual CString AccelStr(){
		return _T("DelPtAroundCurve");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CDelPtAroundCurveCommand();
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Finish();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
private:
	CString m_strPoint;
	CString m_strCurve;
	double m_dis;
};

//道路按阳光法则处理
class CModifyRoadBySunRuleCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CModifyRoadBySunRuleCommand)
public:
	CModifyRoadBySunRuleCommand();
	virtual ~CModifyRoadBySunRuleCommand();
	void Abort();
	
	virtual CString AccelStr(){
		return _T("ModifyRoadBySunRule");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CModifyRoadBySunRuleCommand();
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Finish();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
private:
	CString m_strLayers;//道路边线（实线）图层
	CString m_strVirLayers;//道路虚线图层
	double m_splitdis;

	void ModifyRoad(CFtrLayer* pLayer, CFtrLayer *pVirLayer, CUndoFtrs& undo);
	void ReplaceShortCurve(CFtrLayer* pLayer, CFtrLayer *pNewLayer);
};

//复制对象扩展属性
class CCopyEXTAttributionCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCopyEXTAttributionCommand)
public:	
	CCopyEXTAttributionCommand();
	virtual ~CCopyEXTAttributionCommand();
	static CCommand* Create(){
		return new CCopyEXTAttributionCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyEXTAttribution");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	void Abort();
	void Start();
private:
	FTR_HANDLE m_handleRefer;
	CString m_strFieldName;

	void ChkFieldName(CStringArray& arr, CValueTable& tab);
};

//换图层
class CModifyLayerCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CModifyLayerCommand)
public:
	CModifyLayerCommand();
	virtual ~CModifyLayerCommand();
	static CCommand* Create(){
		return new CModifyLayerCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyLayer");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void ModifyLayer(const FTR_HANDLE* handles,int num);

public:
	CString m_strLayer;
	BOOL m_bResetBaseAtt;
};

//换图层( 重置属性)
class CModifyLayer1Command : public CModifyLayerCommand
{
public:
	CModifyLayer1Command()
	{
		m_bResetBaseAtt = TRUE;
	}
	static CCommand* Create(){
		return new CModifyLayer1Command;
	}
	virtual CString AccelStr(){
		return _T("ModifyLayer1");
	}
};


//删除道路面
class CDelRoadSurfaceCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDelRoadSurfaceCommand)
public:
	CDelRoadSurfaceCommand();
	virtual ~CDelRoadSurfaceCommand();
	void Abort();
	
	virtual CString AccelStr(){
		return _T("DelRoadSurface");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CDelRoadSurfaceCommand();
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Finish();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
private:
	CString m_strLayer;
	CString m_strRoadLayers;
	CString m_strExceptLayers;
};



class CCreateSurfacePtFromSurfaceCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCreateSurfacePtFromSurfaceCommand)
public:
	CCreateSurfacePtFromSurfaceCommand();
	virtual ~CCreateSurfacePtFromSurfaceCommand();
	
	virtual CString AccelStr(){
		return _T("CreateSurfacePtFromSurface");
	}
	virtual CString Name();
	static CCommand* Create(){
		return new CCreateSurfacePtFromSurfaceCommand();
	}
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();

protected:
	void GetSurfaceInnerPoint(CGeometry *pObj, CArray<PT_3D,PT_3D>& arrPts);
	BOOL GetSurfaceInnerPoint_single(CGeometry *pObj, CGeoArray& sons, PT_3D& pt_ret);
};

//打散的双线生成面
class CExplodeDcurveToSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CExplodeDcurveToSurfaceCommand)
public:	
	CExplodeDcurveToSurfaceCommand();
	virtual ~CExplodeDcurveToSurfaceCommand();
	static CCommand* Create(){
		return new CExplodeDcurveToSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("ExplodeDcurveToSurface");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Start();
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
protected:
	void ToSurface(CFtrLayer* pLayer, CFtrLayer* pStoreLayer, CUndoFtrs& undo);
protected:
	CString m_HandleLayer;
	CString m_StoreLayer;
	double m_lfDis;
};

//高程修复
class CRepairZCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CRepairZCommand)
public:	
	CRepairZCommand();
	virtual ~CRepairZCommand();
	static CCommand* Create(){
		return new CRepairZCommand;
	}
	virtual CString AccelStr(){
		return _T("RepairZ");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Start();
};

//节点线型修复
class CRepairLineTypeCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CRepairLineTypeCommand)
public:	
	CRepairLineTypeCommand();
	virtual ~CRepairLineTypeCommand();
	static CCommand* Create(){
		return new CRepairLineTypeCommand;
	}
	virtual CString AccelStr(){
		return _T("RepairLineType");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	void Start();
};


//比高点转点
class CDHeightToPointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDHeightToPointCommand)
public:
	CDHeightToPointCommand();
	virtual ~CDHeightToPointCommand();
	static CCommand* Create(){
		return new CDHeightToPointCommand;
	}
	virtual CString AccelStr(){
		return _T("DHeightToPoint");
	}

	virtual CString Name();

	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

protected:
	CString m_HandleLayer;
	CString m_strField;
};

//双线拆分
class CDCurveSplitCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDCurveSplitCommand)
public:
	CDCurveSplitCommand();
	virtual ~CDCurveSplitCommand();
	static CCommand* Create(){
		return new CDCurveSplitCommand;
	}
	virtual CString AccelStr(){
		return _T("DCurveSplit");
	}

	virtual CString Name();

	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

protected:
	void ReadFileStrings();

	CString m_HandleLayer;
	CString m_strLayer1;
	CString m_strLayer2;
};

//平行线按宽度赋图层
class CChangeDcurveByWidthCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CChangeDcurveByWidthCommand)
public:
	CChangeDcurveByWidthCommand();
	virtual ~CChangeDcurveByWidthCommand();
	static CCommand* Create(){
		return new CChangeDcurveByWidthCommand;
	}
	virtual CString AccelStr(){
		return _T("ChangeDcurveByWidth");
	}

	virtual CString Name();

	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

protected:
	CString m_HandleLayer;
	CString m_strWidth;
	CString m_strLayers;
	BOOL m_bExplode;
};

//点转有向点
class CPointtoDirPointCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CPointtoDirPointCommand)
public:
	CPointtoDirPointCommand();
	virtual ~CPointtoDirPointCommand();
	static CCommand* Create(){
		return new CPointtoDirPointCommand;
	}
	virtual CString AccelStr(){
		return _T("PointtoDirPoint");
	}

	virtual CString Name();

	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

protected:
	CString m_HandleLayer;
	CString m_strLayer;
	BOOL m_bCopyAttr;
};

//局部更新等高线
class  CPartUpdateContourCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CPartUpdateContourCommand)
public:
	struct IntersectionLine_struct
	{
		//CArray<PT_3DEX, PT_3DEX> pts;
		vector<PT_3DEX> pts;
		bool bErgodic;
		IntersectionLine_struct(){ bErgodic = false; };
		IntersectionLine_struct(const IntersectionLine_struct& obj)
		{
			/*for (int i = 0; i < obj.pts.size(); i++)
			{
			pts.push_back(obj.pts.at(i));
			}*/
			pts = obj.pts;
			bErgodic = obj.bErgodic;
		}
	};
public:
	CPartUpdateContourCommand();
	static CCommand *Create(){
		return new CPartUpdateContourCommand;
	}
	virtual CString AccelStr(){
		return _T("PartUpdateContour");
	}
	virtual ~CPartUpdateContourCommand();
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);
	virtual void GetParams(CValueTable& tab);
protected:
	BOOL IsInContourLayer(CFtrLayer *pLayer);
	BOOL CreateTraiangle(Envelope& e);
	void ConnectTinLine(vector<IntersectionLine_struct> *Vector_intersection, vector <PT_3DEX>& NewPts, PT_3DEX& pt, bool flag, multimap<double, vector<PT_3DEX> >& Map_PointLine);
	void ConnectTinLine2(vector<IntersectionLine_struct> *Vector_intersection, vector <PT_3DEX>& NewPts, PT_3DEX& pt, bool flag, bool endflag, multimap<double, vector<PT_3DEX> >& Map_PointLine);
	void UpdateContours(Envelope &e);
private:
	map<double, vector<IntersectionLine_struct>>   m_mapInterLine;
	multimap<double, vector<PT_3DEX> >   m_mapInterPointLine;
	map<double, vector<IntersectionLine_struct>>   m_mapIntersectionLine;
	double  m_min_away;
	bool    m_bSmooth;
	vector<PT_3DEX> m_tempVector;
	CArray<GeoPoint, GeoPoint>  m_AllPts;
protected:
	CString m_strLayerName;//等高线层
	CArray<PT_3DEX, PT_3DEX> m_arrPts;
	CMYTinObj m_tin;
	CDataSourceEx *m_pDS;
	CFtrLayerArray m_arrContourLayers;
};

class COsgbView;
class CSetWhiteModelCommand:public CEditCommand
{
	DECLARE_DYNCREATE(CSetWhiteModelCommand)
public:
	CSetWhiteModelCommand();
	virtual CString Name();
	static CCommand *Create();
	virtual CString AccelStr();
	~CSetWhiteModelCommand();
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	virtual void PtClick(PT_3D &pt, int flag);

private:
	COsgbView * m_pView;
	double height;

};

#endif // !defined(DLGCOMMAND__INCLUDED_)
