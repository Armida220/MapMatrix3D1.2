// EBCommand.h: interface for the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBCOMMAND_H__EAE098E6_E73D_4732_9BC5_5057B6FACB4B__INCLUDED_)
#define AFX_EBCOMMAND_H__EAE098E6_E73D_4732_9BC5_5057B6FACB4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"
#include "Geometry.h"
#include "GeoPoint.h"
#include "GeoDirPoint.h"
#include "GeoCurve.h"
#include "GeoParallel.h "
#include "GeoSurface.h"
#include "UIParam.h"
#include "StreamCompress.h"
#include "undoAction.h"

MyNameSpaceBegin

#define CMPEC_NULL		                   0
#define CMPEC_RESTART	                   1
#define CMPEC_STARTOLD	                   2
#define CMPEC_RESTARTWITHPARAMS	           3

// 过程状态
// 还没有初始化
#define PROCSTATE_NOT_INIT					-1
// 初始化了，但还没有启动
#define PROCSTATE_NOT_START					0
// 已经启动了，正等待用户输入（数据点等）
#define PROCSTATE_READY						1
// 接受到了用户的输入，正在操作中
#define PROCSTATE_PROCESSING				2
// 过程已经成功完成了
#define PROCSTATE_FINISHED					3
// 过程已经失败或者被取消了
#define PROCSTATE_ABORTED					4

#define IsProcReady(p)					((p)->GetState()==PROCSTATE_READY)
#define IsProcProcessing(p)				((p)->GetState()==PROCSTATE_PROCESSING)
#define IsProcFinished(p)				((p)->GetState()==PROCSTATE_FINISHED)
#define IsProcAborted(p)				((p)->GetState()==PROCSTATE_ABORTED)
#define IsProcOver(p)					(IsProcFinished(p)||IsProcAborted(p))

struct DrawingInfo
{
	DrawingInfo(){pFtr = NULL;}
	DrawingInfo(const CFeature *pFt,const CArray<PT_3DEX,PT_3DEX> &arr)
	{ 		
		if( pFt==NULL )
		{
			pFtr = NULL;
			return;
		}
		pFtr = pFt->Clone();		
		pts.Copy(arr);		
	}
	DrawingInfo(const DrawingInfo& Info)
	{
		if(Info.pFtr)
		{
			pFtr = Info.pFtr->Clone();
			pts.Copy(Info.pts);
			lastPts.Copy(Info.lastPts);
		}
		else
		{
			pFtr = NULL;
		}
	}
	DrawingInfo &operator = (const DrawingInfo& Info)
	{
		if(Info.pFtr)
		{
			pFtr = Info.pFtr->Clone();
			pts.Copy(Info.pts);
			lastPts.Copy(Info.lastPts);
		}	
		else
		{
			pFtr = NULL;
		}
		return *this;
	}
	~DrawingInfo()
	{
		if (pFtr)
		{
			delete pFtr;
		}
		pts.RemoveAll();
		lastPts.RemoveAll();
	}
	int GetLastPts(PT_3D *&pt)
	{
		int num = lastPts.GetSize();
		if (num<1)
		{
			if (pts.GetSize()<1)
			{
				return 0;
			}

			lastPts.Add(pts.GetAt(pts.GetSize()-1));
			pt = lastPts.GetData();
			return 1;
		}

		pt = lastPts.GetData();
		return num;
	}
	CGeometry * GetDrawGeo()
	{
		if (pts.GetSize()<=1)
		{
			return NULL;
		}
		CArray<PT_3DEX,PT_3DEX> pts0;
		pts0.Copy(pts);

		pts0.RemoveAt(pts0.GetSize()-1);
		// 解决采集两点后捕捉不到首点的问题
		if (pts0.GetSize() == 1 && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
		{
			pts0.Add(pts0[0]);
		}
		
		pFtr->GetGeometry()->CreateShape(pts0.GetData(),pts0.GetSize());
		return pFtr->GetGeometry();
	}
	CFeature * GetDrawFtr()
	{		
		return pFtr;
	}

	CFeature *pFtr;
	CArray<PT_3DEX,PT_3DEX> pts;
	CArray<PT_3D,PT_3D> lastPts;
};

class CEditor;

//过程对象；可以重用的对象
class EXPORT_EDITBASE CProcedure:public CObject
{
	DECLARE_DYNCREATE(CProcedure)
public:
	//消息类别
	enum MSGTYPE
	{
		msgNone = 0,
		msgPtClick = 1,
		msgPtDblClick = 2,
		msgPtMove = 3,
		msgPtReset = 4,
		msgBack = 5,
		msgEnd = 6
	};
	CProcedure();
	virtual ~CProcedure();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtDblClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);

	virtual void Init(CEditor* pEditor);		
	virtual void Start();
	virtual void Abort();
	virtual void Back();
	virtual void BatchBack();

	//获取过程参数；结果中应该包括过程的所有参数；
	virtual void GetParams(CValueTable& tab);

	//设置的参数不必包含过程的全部参数，只需要包含需要修改的参数；
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	virtual BOOL Keyin(LPCTSTR text);
	
	//nMsgType 就是 MSGTYPE，当Editor执行该类消息的相关接口时，
	//首先判断在该消息下，是否需要子过程响应，
	//如果返回非空子过程的指针，就调用该子过程的相关调用，
	//如果返回空指针，就调用父过程的相关接口
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	//仅当 GetActiveSonProc(msgEnd)!=NULL 时，OnSonEnd 才会被调用
	virtual void OnSonEnd(CProcedure *pProc);
	
	virtual int  GetState();
	int  GetExitCode();
	void	SetSettingsModifyFlag();

	//填充需要界面显示的过程参数
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);

protected:
	void GotoState(int state);

	//根据子过程和当前命令状态，返回合适的命令状态
	int GetStateWithSonProcedure(CProcedure *pSon);

	virtual void Finish();

	//装载和保存 过程参数
	virtual void UpdateParams(BOOL bSave);

	//请求界面输出提示信息
	virtual void PromptString(LPCTSTR str);

protected:
	BOOL m_bSettingModified;
	TCHAR m_strRegPath[256];

	int m_nState;
	int m_nExitCode;

	CEditor *m_pEditor;

};


class EXPORT_EDITBASE CDrawPointProcedure : public CProcedure
{
public:
	CDrawPointProcedure();
	virtual ~CDrawPointProcedure();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Start();
	
	CGeoPoint *m_pGeoPoint;
};

class EXPORT_EDITBASE CDrawDirPointProcedure : public CProcedure
{
public:
	CDrawDirPointProcedure();
	virtual ~CDrawDirPointProcedure();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	BOOL m_bDrawDir;
	CGeoDirPoint *m_pGeoPoint;
};

class EXPORT_EDITBASE CDrawCurveProcedure : public CProcedure
{
public:
	CDrawCurveProcedure();
	virtual ~CDrawCurveProcedure();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Start();
	virtual void Abort();
	virtual void Finish();
	virtual void Back();
	virtual void BatchBack();
	void PutStreamPts();

	// 改变线型前将之前线置为const
	void ChangePencod(int pencode);
	void ReverseLine();
	void TempLine();

	void TrackLine();

	void StartBuildPt();
	void EndBuildPt();

	CGeoCurve *m_pGeoCurve;
	CGeoCurve *m_pGeoMoveCurve;

	BOOL m_bRectify;
	double m_fAutoCloseToler, m_fMaxEndDis;
	BOOL m_bDoSmooth;
	
	int m_nEndSnapped;
	BOOL m_bOpenIfSnapped;
	BOOL m_bFastDisplayCurve;

	BOOL m_bClosed;
	// 当前点属性
	int m_nCurPenCode;
	float m_fCurNodeWid;
	short m_nCurNodeType;

	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CStreamCompress5_PT_3DEX m_compress;

	COLORREF m_layCol;

	BOOL m_bChgPenCode;
	CArray<PT_3DEX,PT_3DEX> m_arrAllPts;
	CUIntArray m_arrIndex;

	//追踪
	CArray<PT_3DEX,PT_3DEX> m_trackPts;//移动过程中添加的点
	BOOL        m_bTrack;
	PT_3D		m_curPt;
	FTR_HANDLE  m_trackHandle; 
	int			m_nLastTrackIndex;//最后一次跟踪的线段序号
	PT_3DEX     m_nLastTrackPt;//最后一次跟踪的点
	int			m_TrackPart;//跟踪的地物的部分
	BOOL        m_bSnap2D;

	// 多点构建点，提高精度
	BOOL        m_bMultiPtBuildPt;
	double      m_fTolerBuildPt;
	CArray<PT_3DEX,PT_3DEX> m_arrBuildPt;
	
};

class EXPORT_EDITBASE CDrawDCurveProcedure : public CDrawCurveProcedure
{
public:
	CDrawDCurveProcedure();
	virtual ~CDrawDCurveProcedure();
	virtual void PtReset(PT_3D &pt);
	virtual void Start();
	virtual void Abort();
	virtual void Finish();
	virtual void Back();
	virtual void BatchBack();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	void TrackLine();
	CArray<PT_3DEX,PT_3DEX> m_arrBasePts;
	int m_bBaseLine;

	// 0:一般双线  1:平行双线
	int m_nMode;

protected:
	void FinishDCurve();
};


class EXPORT_EDITBASE CDrawParallelProcedure : public CProcedure
{
public:
	CDrawParallelProcedure();
	virtual ~CDrawParallelProcedure();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	virtual void Finish();
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Start();
	virtual void Abort();
	virtual void Back();
	void PutStreamPts();
	void ReverseLine();
	void TempLine();
	void TrackLine();
	
	CGeoParallel *m_pGeoPara;	

	float m_lfSepcialWidth;
	BOOL m_bClosed;

	//当前点属性
	int m_nCurPenCode;
	float m_fCurNodeWid;
	short m_nCurNodeType;

	BOOL m_bDrawWidth;
	BOOL m_bMouseWidth;
	BOOL m_bByCenterLine;
	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CStreamCompress5_PT_3DEX m_compress; 


	BOOL m_bRectify, m_bBreakup;
	double m_fAutoCloseToler, m_fMaxEndDis;
	BOOL m_bDoSmooth;
	
	int m_nEndSnapped;
	BOOL m_bOpenIfSnapped;
	BOOL m_bFastDisplayCurve;

	COLORREF m_layCol;

	BOOL m_bCenterlineMode;
	CString m_strRetLayer;

	// 高程差模式
	BOOL m_bDHeight;
	PT_3D m_ptLast;

	//追踪
	CArray<PT_3DEX,PT_3DEX> m_trackPts;//移动过程中添加的点
	BOOL        m_bTrack;
	PT_3D		m_curPt;
	FTR_HANDLE  m_trackHandle; 
	int			m_nLastTrackIndex;//最后一次跟踪的线段序号
	PT_3DEX     m_nLastTrackPt;//最后一次跟踪的点
	int			m_TrackPart;//跟踪的地物的部分
	BOOL        m_bSnap2D;
};


class EXPORT_EDITBASE CDrawSurfaceProcedure : public CProcedure
{
public:
	CDrawSurfaceProcedure();
	virtual ~CDrawSurfaceProcedure();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void PtReset(PT_3D &pt);
	virtual void GetParams(CValueTable& tab);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void Start();	
	virtual void Back();
	virtual void BatchBack();
	void PutStreamPts();
	void Abort();
	virtual void Finish();
	void TrackLine();
	CGeoSurface *m_pGeoSurface;
	
	//当前点属性
	int m_nCurPenCode;
	float m_fCurNodeWid;
	short m_nCurNodeType;

	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CStreamCompress5_PT_3DEX m_compress;

	BOOL m_bRectify;

	double m_lfToler;
	bool m_bIsRef;				 //标志在结束时是否需要共线引用，为PtReset（）中的进一步处理做准备

	int m_nDelPtNum;			 //为实现动态的显示效果而定义的用于记录需释放的尾端点数
	
	CArray<PT_3DEX,PT_3DEX> m_arrRefPts;


	COLORREF m_layCol;

	//追踪
	CArray<PT_3DEX,PT_3DEX> m_trackPts;//移动过程中添加的点
	BOOL        m_bTrack;
	PT_3D		m_curPt;
	FTR_HANDLE  m_trackHandle; 
	int			m_nLastTrackIndex;//最后一次跟踪的线段序号
	PT_3DEX     m_nLastTrackPt;//最后一次跟踪的点
	int			m_TrackPart;//跟踪的地物的部分
	BOOL        m_bSnap2D;
};

//命令对象；具体的操作主体；不具备可重用性；
class EXPORT_EDITBASE CCommand : public CProcedure
{
	DECLARE_DYNCREATE(CCommand)
public:
	CCommand();
	virtual ~CCommand();	
	virtual CString Name()=0;
	virtual CString AccelStr()=0;	

	virtual void PtReset(PT_3D &pt);
	virtual void Start();
	virtual void Abort();
	virtual void Finish();
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	void RefreshParams();
	virtual DrawingInfo GetCurDrawingInfo() { return DrawingInfo();}
protected:


protected:
	
};

typedef CCommand* (*LPPROC_CMDCREATE)();

class EXPORT_EDITBASE CEditCommand : public CCommand
{
	DECLARE_DYNCREATE(CEditCommand)
public:
	CEditCommand();
	virtual ~CEditCommand();
	virtual void Abort();
	virtual void Start();
	virtual void PtReset(PT_3D &pt);
	static BOOL CanGetSelObjs(int flag, BOOL bMultiSel=TRUE);
protected:
	virtual void Finish();
	void EditStepOne();
protected:
	int m_nStep;
};

//状态激活的命令对象；具有状态激活的能力；
class EXPORT_EDITBASE CActiveStateCommand : public CEditCommand
{
public:
	enum
	{
		stateClick = 0,
		stateDblClick = 1
	};
	DECLARE_DYNCREATE(CActiveStateCommand)
public:
	virtual BOOL HitTest(PT_3D pt, int state)const=0;
};


//放置特征的命令对象；
class EXPORT_EDITBASE CDrawCommand : public CCommand
{
	DECLARE_DYNCREATE(CDrawCommand)
public:
	CDrawCommand();
	virtual void Abort();
	virtual void Start();
	virtual CGeometry *GetCurDrawingObj() {return NULL;}
	virtual CFeature* CreateFeature(){
		return new CFeature;
	}
	virtual DrawingInfo GetCurDrawingInfo() { return DrawingInfo();}
	virtual int  GetCurPenCode(){ return penLine; }
	virtual BOOL AddObject(CPFeature pFtr, int layid = -1);
	virtual void RefreshPropertiesPanel(){}
	BOOL IsReadCmdParas() { return m_bReadCmdParas; };
	void EnableReadCmdParas(BOOL bReadCmdParas) { m_bReadCmdParas = bReadCmdParas; };
protected:
	virtual void Finish();

	CFeature *m_pFtr;

	// 读取与命令相关的参数取代与层相关
	BOOL m_bReadCmdParas;
};


class EXPORT_EDITBASE CDrawPointCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawPointCommand)
public:
	CDrawPointCommand();
	static CCommand *Create(){
		return new CDrawPointCommand;
	}
	virtual CString AccelStr(){
		return _T("Point");
	}
	virtual ~CDrawPointCommand();
	virtual CGeometry *GetCurDrawingObj();
	virtual CString Name();
	virtual void OnSonEnd(CProcedure *son);
	virtual void Start();	
	virtual void Abort();
	virtual int  GetState();
	virtual int  GetCurPenCode();

	virtual CProcedure *GetActiveSonProc(int MsgType){
		return m_pDrawProc;
	}

protected:
	virtual void Finish();
protected:
	CDrawPointProcedure *m_pDrawProc;
};



class EXPORT_EDITBASE CDrawCurveCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawCurveCommand)
public:
	void GetParams(CValueTable& tab);
	CDrawCurveCommand();
	static CCommand *Create(){
		return new CDrawCurveCommand;
	}
	virtual CString AccelStr(){
		return _T("Pline");
	}
	virtual ~CDrawCurveCommand();
	virtual CGeometry *GetCurDrawingObj();
	virtual CString Name();
	virtual void Back();
	virtual void OnSonEnd(CProcedure *son);
	virtual void Start();	
	virtual void Abort();
	virtual int  GetState();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual int  GetCurPenCode();
	virtual CProcedure *GetActiveSonProc(int MsgType){
		return m_pDrawProc;
	}
	virtual BOOL Keyin(LPCTSTR text);
protected:
	virtual void Finish();
	virtual void UpdateParams(BOOL bSave);
	
protected:
	CDrawCurveProcedure *m_pDrawProc;
};


class EXPORT_EDITBASE CDrawSurfaceCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CDrawSurfaceCommand)
public:
	CDrawSurfaceCommand();
	static CCommand *Create(){
		return new CDrawSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("Polygon");
	}
	virtual ~CDrawSurfaceCommand();
	virtual CGeometry *GetCurDrawingObj();
	virtual CString Name();
	virtual void OnSonEnd(CProcedure *son);
	virtual void Start();	
	virtual void Abort();
	virtual int  GetState();
	virtual void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual int  GetCurPenCode();
	virtual CProcedure *GetActiveSonProc(int MsgType){
		return m_pDrawProc;
	}
	virtual BOOL Keyin(LPCTSTR text);
protected:
	virtual void Finish();
	virtual void UpdateParams(BOOL bSave);
	
protected:
	CDrawSurfaceProcedure *m_pDrawProc;
};


class EXPORT_EDITBASE CEditVertexCommand : public CActiveStateCommand
{
	DECLARE_DYNCREATE(CEditVertexCommand)
public:
	CEditVertexCommand();
	virtual ~CEditVertexCommand();
	static CCommand *Create(){
		return new CEditVertexCommand;
	}
	virtual CString AccelStr(){
		return _T("ModifyVertex");
	}
	virtual CString Name();

	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);	
	virtual void Start();
	virtual void Abort();
	virtual void Finish();

	virtual BOOL HitTest(PT_3D pt, int state)const;

	virtual void OutputVertexInfo(PT_KEYCTRL pt, CGeometry *pGeo);

protected:	

	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
	
	BOOL  m_bGetPointZ;

	CArray<PT_KEYCTRL,PT_KEYCTRL> m_arrKeyPts;
	CArray<CPFeature,CPFeature>   m_arrFtrs;
	CArray<BOOL,BOOL>			  m_arrPtsRepeat;
};


class EXPORT_EDITBASE CInsertVertexCommand : public CActiveStateCommand
{
	DECLARE_DYNCREATE(CInsertVertexCommand)
public:
	CInsertVertexCommand();
	virtual ~CInsertVertexCommand();
	static CCommand *Create(){
		return new CInsertVertexCommand;
	}
	virtual CString AccelStr(){
		return _T("InsertVertex");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);	
	virtual void Start();
	virtual void Abort();
	
	virtual BOOL HitTest(PT_3D pt, int state)const;
	
protected:

	CArray<CPFeature,CPFeature>   m_arrFtrs;
	CArray<int,int>	m_arrPtIndex;
	BOOL  m_bGetPointZ;
};


class EXPORT_EDITBASE CDeleteVertexCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDeleteVertexCommand)
public:
	CDeleteVertexCommand();
	virtual ~CDeleteVertexCommand();
	static CCommand *Create(){
		return new CDeleteVertexCommand;
	}
	virtual CString AccelStr(){
		return _T("DeleteVertex");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	virtual void Abort();

	static BOOL DeleteVertex(CEditor *pEditor, CFeature *pFtr, int ptIndex, CUndoBatchAction &batchUndo, LPCTSTR undoName);
	
protected:

	CArray<CPFeature,CPFeature>   m_arrFtrs;
	CArray<int,int>	m_arrPtIndex;
	CArray<PT_3DEX,PT_3DEX>	m_arrPts;
	CArray<BOOL,BOOL>	m_arrPtsRepeat;
};



class EXPORT_EDITBASE CMoveCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CMoveCommand)
public:
	CMoveCommand();
	static CCommand *Create(){
		return new CMoveCommand;
	}
	virtual CString AccelStr(){
		return _T("Move");
	}
	virtual ~CMoveCommand();
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D &pt);
	virtual void Start();
	virtual void Abort();

protected:
	PT_3D m_ptMoveStart;
	CArray<FTR_HANDLE,FTR_HANDLE> m_arrObjs;
};

#define CMDPLANE_CONTOURINTER "ContourInter"
#define CMDPLANE_CHECKPCODE   "CheckPCode"
#define CMDPLANE_PLMINDZ      "PLMinDx"
#define CMDPLANE_INDENSITY	  "Indensity"



MyNameSpaceEnd

#endif // !defined(AFX_EBCOMMAND_H__EAE098E6_E73D_4732_9BC5_5057B6FACB4B__INCLUDED_)
