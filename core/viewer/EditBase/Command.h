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

// ����״̬
// ��û�г�ʼ��
#define PROCSTATE_NOT_INIT					-1
// ��ʼ���ˣ�����û������
#define PROCSTATE_NOT_START					0
// �Ѿ������ˣ����ȴ��û����루���ݵ�ȣ�
#define PROCSTATE_READY						1
// ���ܵ����û������룬���ڲ�����
#define PROCSTATE_PROCESSING				2
// �����Ѿ��ɹ������
#define PROCSTATE_FINISHED					3
// �����Ѿ�ʧ�ܻ��߱�ȡ����
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
		// ����ɼ������׽�����׵������
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

//���̶��󣻿������õĶ���
class EXPORT_EDITBASE CProcedure:public CObject
{
	DECLARE_DYNCREATE(CProcedure)
public:
	//��Ϣ���
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

	//��ȡ���̲����������Ӧ�ð������̵����в�����
	virtual void GetParams(CValueTable& tab);

	//���õĲ������ذ������̵�ȫ��������ֻ��Ҫ������Ҫ�޸ĵĲ�����
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	virtual BOOL Keyin(LPCTSTR text);
	
	//nMsgType ���� MSGTYPE����Editorִ�и�����Ϣ����ؽӿ�ʱ��
	//�����ж��ڸ���Ϣ�£��Ƿ���Ҫ�ӹ�����Ӧ��
	//������طǿ��ӹ��̵�ָ�룬�͵��ø��ӹ��̵���ص��ã�
	//������ؿ�ָ�룬�͵��ø����̵���ؽӿ�
	virtual CProcedure *GetActiveSonProc(int nMsgType);

	//���� GetActiveSonProc(msgEnd)!=NULL ʱ��OnSonEnd �Żᱻ����
	virtual void OnSonEnd(CProcedure *pProc);
	
	virtual int  GetState();
	int  GetExitCode();
	void	SetSettingsModifyFlag();

	//�����Ҫ������ʾ�Ĺ��̲���
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);

protected:
	void GotoState(int state);

	//�����ӹ��̺͵�ǰ����״̬�����غ��ʵ�����״̬
	int GetStateWithSonProcedure(CProcedure *pSon);

	virtual void Finish();

	//װ�غͱ��� ���̲���
	virtual void UpdateParams(BOOL bSave);

	//������������ʾ��Ϣ
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

	// �ı�����ǰ��֮ǰ����Ϊconst
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
	// ��ǰ������
	int m_nCurPenCode;
	float m_fCurNodeWid;
	short m_nCurNodeType;

	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CStreamCompress5_PT_3DEX m_compress;

	COLORREF m_layCol;

	BOOL m_bChgPenCode;
	CArray<PT_3DEX,PT_3DEX> m_arrAllPts;
	CUIntArray m_arrIndex;

	//׷��
	CArray<PT_3DEX,PT_3DEX> m_trackPts;//�ƶ���������ӵĵ�
	BOOL        m_bTrack;
	PT_3D		m_curPt;
	FTR_HANDLE  m_trackHandle; 
	int			m_nLastTrackIndex;//���һ�θ��ٵ��߶����
	PT_3DEX     m_nLastTrackPt;//���һ�θ��ٵĵ�
	int			m_TrackPart;//���ٵĵ���Ĳ���
	BOOL        m_bSnap2D;

	// ��㹹���㣬��߾���
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

	// 0:һ��˫��  1:ƽ��˫��
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

	//��ǰ������
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

	// �̲߳�ģʽ
	BOOL m_bDHeight;
	PT_3D m_ptLast;

	//׷��
	CArray<PT_3DEX,PT_3DEX> m_trackPts;//�ƶ���������ӵĵ�
	BOOL        m_bTrack;
	PT_3D		m_curPt;
	FTR_HANDLE  m_trackHandle; 
	int			m_nLastTrackIndex;//���һ�θ��ٵ��߶����
	PT_3DEX     m_nLastTrackPt;//���һ�θ��ٵĵ�
	int			m_TrackPart;//���ٵĵ���Ĳ���
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
	
	//��ǰ������
	int m_nCurPenCode;
	float m_fCurNodeWid;
	short m_nCurNodeType;

	CArray<PT_3DEX,PT_3DEX> m_arrPts;
	CStreamCompress5_PT_3DEX m_compress;

	BOOL m_bRectify;

	double m_lfToler;
	bool m_bIsRef;				 //��־�ڽ���ʱ�Ƿ���Ҫ�������ã�ΪPtReset�����еĽ�һ��������׼��

	int m_nDelPtNum;			 //Ϊʵ�ֶ�̬����ʾЧ������������ڼ�¼���ͷŵ�β�˵���
	
	CArray<PT_3DEX,PT_3DEX> m_arrRefPts;


	COLORREF m_layCol;

	//׷��
	CArray<PT_3DEX,PT_3DEX> m_trackPts;//�ƶ���������ӵĵ�
	BOOL        m_bTrack;
	PT_3D		m_curPt;
	FTR_HANDLE  m_trackHandle; 
	int			m_nLastTrackIndex;//���һ�θ��ٵ��߶����
	PT_3DEX     m_nLastTrackPt;//���һ�θ��ٵĵ�
	int			m_TrackPart;//���ٵĵ���Ĳ���
	BOOL        m_bSnap2D;
};

//������󣻾���Ĳ������壻���߱��������ԣ�
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

//״̬�����������󣻾���״̬�����������
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


//�����������������
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

	// ��ȡ��������صĲ���ȡ��������
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
