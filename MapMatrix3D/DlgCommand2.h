// DlgCommand2.h: interface for the CCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLGCOMMAND2_H_INCLUDED_)
#define AFX_DLGCOMMAND2_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Command.h"
#include "GeoCurve.h"
#include "EditDef.h"
#include "UndoAction.h"

#include <fstream>
#include <string>
#include <map>

class CDlgDoc;
typedef CArray<FTR_HANDLE,FTR_HANDLE> CIDArray;



//���������ע��
class CCreateCoordNoteLineCommand : public CDrawCommand
{
	DECLARE_DYNCREATE(CCreateCoordNoteLineCommand)
public:
	struct IntersectPoint
	{
		PT_3D pt;
		double t;
	};

	CCreateCoordNoteLineCommand();
	static CCommand *Create(){
		return new CCreateCoordNoteLineCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateCoordNoteLine");
	}
	virtual ~CCreateCoordNoteLineCommand();
	virtual CString Name();	
	virtual void Start();
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void PtDblClick(PT_3D &pt, int flag);
	virtual void PtMove(PT_3D& pt);
	virtual void Abort();
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual void GetParams(CValueTable& tab);
	virtual DrawingInfo GetCurDrawingInfo();
	virtual int  GetCurPenCode();
	
	virtual void PtReset(PT_3D &pt);
	
protected:
	virtual void Finish();

	int IsContainEachother(CArray<PT_3DEX,PT_3DEX>& arr1, CArray<PT_3DEX,PT_3DEX>& arr2);
	BOOL CreateCoordNoteLine(PT_3D curPt, PT_3D pts[2]);
	
	BOOL 	m_bSelectBound;
	float   m_fInterval;	//������࣬��λcm��ȱʡ 10 cm
	int		m_nDirection;   //����0ˮƽ��1��ֱ��
	int		m_nPosition;	//λ�ã�0���ϣ���1�ң��£�
	
	//����ͼ����
	static CArray<PT_3DEX,PT_3DEX> m_arrBound1;
	static CArray<PT_3DEX,PT_3DEX> m_arrBound2;
	
};


//��������ע��
class CCreateCoordNoteCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CCreateCoordNoteCommand)
public:
	CCreateCoordNoteCommand();
	virtual ~CCreateCoordNoteCommand();
	static CCommand* Create(){
		return new CCreateCoordNoteCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateCoordNote");
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

	int m_nAlign;  //���뷽ʽ��0����룬1���У�2�Ҷ���

	float m_fDX, m_fDY;  //�����࣬�����ࣻ��λ mm

	CString m_strFontName;  //������
	float m_fFontSize;		//�����С
	float m_fWidthScale;	//�ֿ���

	float m_fCoordUnit;		//���굥λ(1�ף�1000�ף��ȵ�)
	int m_nDecimal;			//С��β��
	BOOL m_bUseNEMark;		//�Ƿ����NE��־
};


//�������ֽǶ���ֱ��һ��
class CSetTextAngleFromLineCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSetTextAngleFromLineCommand)
public:	
	CSetTextAngleFromLineCommand();
	virtual ~CSetTextAngleFromLineCommand();
	static CCommand* Create(){
		return new CSetTextAngleFromLineCommand;
	}
	virtual CString AccelStr(){
		return _T("SetTextAngleFromLine");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void Abort();
	void Start();

protected:

	FTR_HANDLE m_hTextObj;
	
};



class CExtendOrTrimCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CExtendOrTrimCommand)
public:	
	CExtendOrTrimCommand();
	virtual ~CExtendOrTrimCommand();
	static CCommand* Create(){
		return new CExtendOrTrimCommand;
	}
	virtual CString AccelStr(){
		return _T("ExtendOrTrim");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
protected:
	BOOL ProcessOne(CFeature *pFtr, CFeature *pFtrRef, PT_3D ptClick, double& dis, BOOL bTest, CUndoBatchAction *pUndo);
	void GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z);

	PT_3D m_ptClick1;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;

	CFtrArray m_arrRefFtrs; 
};




class CExtendOrTrimAllCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CExtendOrTrimAllCommand)
public:	
	CExtendOrTrimAllCommand();
	virtual ~CExtendOrTrimAllCommand();
	static CCommand* Create(){
		return new CExtendOrTrimAllCommand;
	}
	virtual CString AccelStr(){
		return _T("ExtendOrTrimAll");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
private:
	double CalcExtendDistance(CPFeature f1, CPFeature f2, int nPtIndex);
	void ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction *pUndo);
	void GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z);
	
	CString m_strLayer0;
	CString m_strLayer1;

	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
};


//�������Ҵ���
class CLLSuspendProcess1Command : public CEditCommand  
{
	DECLARE_DYNCREATE(CLLSuspendProcess1Command)
public:	
	CLLSuspendProcess1Command();
	virtual ~CLLSuspendProcess1Command();
	static CCommand* Create(){
		return new CLLSuspendProcess1Command;
	}
	virtual CString AccelStr(){
		return _T("LLSuspendProcess1");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
private:
	double CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex);
	void ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction *pUndo);
	
	//�������ͼ��
	CString m_strLayer0;

	//�ο�ͼ��
	CString m_strLayer1;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
};



//�������Ҵ���
class CPLSuspendProcessCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CPLSuspendProcessCommand)
public:	
	enum
	{
		typeMovePt = 0,
		typeMoveLine = 1
	};
	CPLSuspendProcessCommand();
	virtual ~CPLSuspendProcessCommand();
	static CCommand* Create(){
		return new CPLSuspendProcessCommand;
	}
	virtual CString AccelStr(){
		return _T("PLSuspendProcess");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
private:
	double CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex);
	void ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction *pUndo);
	
	//���ͼ��
	CString m_strLayer0;
	
	//�ߵ�ͼ��
	CString m_strLayer1;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;

	//�޸ķ�ʽ
	int m_nModifyType; //typeMovePt, typeMoveLine
};

//�������ཻ
class   CDealSelfIntersectionCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CDealSelfIntersectionCommand)
public:
	struct IntersectItem
	{
		IntersectItem()
		{
			pGeo = NULL;
			ptIdx = -1;
			t = 0;
		}
		CGeometry *pGeo;
		PT_3D pt;	 //��������
		int ptIdx;   //�����λ�ã�����ʱ�ã�
		double t;    //���������߶εı���λ�ã�������
	};
	typedef CArray<IntersectItem, IntersectItem> IntersectItemArray;
public:
	CDealSelfIntersectionCommand();
	virtual ~CDealSelfIntersectionCommand();
	virtual CString Name();
	virtual CString AccelStr(){ return _T("DealSelfIntersection"); };
	static CCommand* Create();
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

	void Abort();
	void Start();
	virtual void Finish();
	BOOL CutLines_self(CGeometry *pGeo, CGeoArray& arrGeos);
	BOOL DeleteCommonLine(CGeoArray& arrGeos);
	void GetIntersects_self(CGeometry *pGeo1, IntersectItemArray &arr);
	BOOL IsOverlapped(CGeometry *pGeo1, CGeometry *pGeo2);
	virtual void PtClick(PT_3D &pt, int flag);

protected:
	CString m_HandleLayer;
	double m_lfToler;
};


//����Ԥ����
class CTopoPreTreatCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTopoPreTreatCommand)
public:	
	typedef CShapeLine::ShapeLineUnit ShapeUnit;

	struct IntersectItem
	{
		IntersectItem()
		{
			pFtr = NULL;
			ptIdx = -1;
			t = 0;
		}
		CFeature *pFtr;	
		PT_3D pt;	 //��������
		int ptIdx;   //�����λ�ã�����ʱ�ã�
		double t;    //���������߶εı���λ�ã�������
	};

	struct SnapItem
	{
		SnapItem()
		{
			pFtr1 = NULL;
			pFtr2 = NULL;
			ptIdx1 = -1;
			ptIdx2 = -1;
		}
		CFeature *pFtr1;
		CFeature *pFtr2;	
		PT_3D pt1;	 //�ص��������
		PT_3D pt2;	 //�ص��������
		int ptIdx1;   //�ص����λ�ã�����ʱ�ã�
		int ptIdx2;
	};

	typedef CArray<IntersectItem,IntersectItem> IntersectItemArray;
	typedef CArray<SnapItem,SnapItem> SnapItemArray;

	CTopoPreTreatCommand();
	virtual ~CTopoPreTreatCommand();
	static CCommand* Create(){
		return new CTopoPreTreatCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoPreTreat");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
protected:
	BOOL Linearize(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);
	BOOL ProcessSuspend(CFtrLayerArray& layers0, CUndoBatchAction* pUndop);
	BOOL CutLines(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);
	BOOL CutLines_self(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);
	BOOL SnapVertexes(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);
	BOOL DeleteCommonLine(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);
	void CreateSurfacePoints(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);

	void GetIntersects(CFeature* f1,CFeature* f2,IntersectItemArray &arr);
	void GetIntersects_self(CFeature* f1, IntersectItemArray &arr);
	void GetSnapVertexes(CFeature* f1,CFeature* f2,Envelope evlp, SnapItemArray &arr);

	double CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex);
	void ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex, CUndoBatchAction* pUndo);
	void GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z);

	BOOL IsOverlapped(CPFeature f1, CPFeature f2);
	
	//�������ͼ��
	CString m_strLayer0;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
};



//�ֹ�ƥ��ڵ�
class CMatchVertexsCommand : public CTopoPreTreatCommand  
{
	DECLARE_DYNCREATE(CMatchVertexsCommand)
public:
	CMatchVertexsCommand();
	virtual ~CMatchVertexsCommand();
	virtual CString Name();
	
	static CCommand* Create(){
		return new CMatchVertexsCommand;
	}
	virtual CString AccelStr(){
		return _T("MatchVertexs");
	}
	
	void PtMove(PT_3D &pt);
	void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
    virtual void GetParams(CValueTable& tab);
	
protected:
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
	
};



//ȫͼ�Զ�ƥ��ڵ�
class CMatchAllVertexsCommand : public CTopoPreTreatCommand  
{
	DECLARE_DYNCREATE(CMatchAllVertexsCommand)
public:
	CMatchAllVertexsCommand();
	virtual ~CMatchAllVertexsCommand();
	virtual CString Name();
	
	static CCommand* Create(){
		return new CMatchAllVertexsCommand;
	}
	virtual CString AccelStr(){
		return _T("MatchAllVertexs");
	}
	
	void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
    virtual void GetParams(CValueTable& tab);
	
protected:
	//�����������ҵ�ķ�ʽ���д���
	BOOL SnapVertexes2_One(CFeature *pFtr, double lfSusToler, double lfTolerZ, BOOL bCrossLay, BOOL bModifyZ, CUndoBatchAction* pUndo);
	void SnapVertexes2(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);

	void AddVectexUndo(CUndoBatchAction* pUndo, CFeature *pFtr, int ptidx, PT_3DEX ptOld, PT_3DEX ptNew, BOOL bRepeat);
};

class CModifyCutLinesCommand : public CTopoPreTreatCommand  
{
	DECLARE_DYNCREATE(CModifyCutLinesCommand)
public:	
	CModifyCutLinesCommand();
	virtual ~CModifyCutLinesCommand();
	static CCommand *Create(){
		return new CModifyCutLinesCommand;
	}
	virtual CString AccelStr(){
		return _T("CutIntersectLines");
	}
	virtual CString Name();
	
	virtual void PtClick(PT_3D &pt, int flag);
	virtual void Start();
	
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	virtual	void GetParams(CValueTable& tab);
protected:
	CString m_handleLayers;	

	BOOL CutLines1(CFtrLayerArray& layers0, CUndoBatchAction* pUndo);
};


// 
//ɾ��������
class CTopoDelSurfaceCommand : public CEditCommand  
{
   DECLARE_DYNCREATE(CTopoDelSurfaceCommand)
public:
    CTopoDelSurfaceCommand();
	virtual ~CTopoDelSurfaceCommand();
	static CCommand* Create(){
		return new CTopoDelSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoDelSurface");
	}
	virtual CString Name();
	void Abort();
	void Start();
	virtual void Finish();
	virtual void PtClick(PT_3D &pt, int flag);
protected:
	BOOL delete_surface();
};


//ͨ�������ˢ���ͼ�������
class CFillSurfaceATTFromPtCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CFillSurfaceATTFromPtCommand)
public:
	CFillSurfaceATTFromPtCommand();
	virtual ~CFillSurfaceATTFromPtCommand();
	static CCommand* Create(){
		return new CFillSurfaceATTFromPtCommand;
	}
	virtual CString AccelStr(){
		return _T("FillSurfaceATTFromPt");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
protected:
	int FillAttribute(CFtrArray& arrPtFtr, CFeature *pFtr, CUndoFtrs *undo);

public:
	CString m_HandleLayer;
	BOOL m_bDelSurfacePt;
protected:
	CDataSourceEx *m_pDS;
};


//ͨ�������ˢ���ͼ�������(ѡ��)
class CFillSurfaceATTFromPtSelCommand : public CFillSurfaceATTFromPtCommand  
{
	DECLARE_DYNCREATE(CFillSurfaceATTFromPtSelCommand)
public:
	CFillSurfaceATTFromPtSelCommand();
	virtual ~CFillSurfaceATTFromPtSelCommand();
	static CCommand* Create(){
		return new CFillSurfaceATTFromPtSelCommand;
	}
	virtual CString AccelStr(){
		return _T("FillSurfaceATTFromPtSel");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);

	void Start();
};


#include <math.h>
//���˹���
class CTopoBuildSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTopoBuildSurfaceCommand)
public:
	friend class CTopoDelSurfaceCommand;
	friend class CTopoUnionSurfaceCommand;
public:
	//����״̬ö�٣�
	enum TState{HANDLED,CLOSED,VISITED,UNVISITED,INVALID};//INVALID�Ͽ��Ļ�
	//
	struct TConnectItemBase
	{
		int index;//���ӻ��Ļ����
		BOOL StoE;//���ӻ��ķ����Ƿ�����㵽�յ�
	};
	struct TConnectItem:public TConnectItemBase
	{
		double degree;//�Ƕ�
		TConnectItem *next;
		int visited;//��¼����׷�ٵ���ʼ�����
		TConnectItem()
		{
			index=-1;
			StoE=TRUE;
			degree=0;
			visited=-1;
			next=NULL;
		}
	};
	//�˵����ݽṹ��
	struct TEndPoint
	{
		PT_3DEX data;
		TConnectItem *connects;
		TConnectItem *next_arc;//׷�ٽ�����ȷ����������
		TEndPoint()
		{
			connects=NULL;
			next_arc=NULL;
		}
		inline bool operator==(const TEndPoint &rp)
		{
			if(fabs(data.x-rp.data.x)<GraphAPI::g_lfDisTolerance && fabs(data.y-rp.data.y)<GraphAPI::g_lfDisTolerance )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		inline void add_con_item(TConnectItem * p)
		{
			TConnectItem * s=connects;
			while(s->next!=0 && s->next->degree<=p->degree)
			{
				s=s->next;
			}
			p->next=s->next;
			s->next=p;
		}
	};
	//�������ݽṹ��
	struct TArcItem
	{
		int geo_index;
		int point_count;  //���ĵ�����
		TEndPoint sp;     //��ʼ�㣻
		double s_degree;  //����ʼΪ�׵����Ļ��ĽǶȣ�
		TEndPoint ep;     //β��;
		double e_degree;  //��β�����Ļ��ĽǶȣ�
		TState state;     //�û���ǰ��״̬��
		CArray<int,int> *face_index_list;   //�ɸû������ɵ�����б�;
		int FeatureFlag;//���浱ǰftr��appflag,ʹ�����ԭ
		BOOL SEVisited;//���->�յ��Ƿ������
		BOOL ESVisited;//�յ�->����Ƿ������
        TArcItem()
		{
			point_count = 0;
			state=UNVISITED;
			geo_index=-1;
			face_index_list=NULL;
			FeatureFlag = 0;
			SEVisited = FALSE;
			ESVisited = FALSE;
		}
	};
    //������ݽṹ��
	struct TFaceItem
	{
	   int  color_index;
	   int  layer_id;
       CArray<TConnectItemBase,TConnectItemBase> *arc_item_list;
	   PT_3DEX *datas;
	   int point_count;
	   TFaceItem()
	   {
		   datas=NULL;
		   point_count=0;
	   }
	};

	inline double get_degree(PT_3DEX ps,PT_3DEX pe)
	{
		double dx=pe.x-ps.x;
		double dy=pe.y-ps.y;
		if(fabs(dx)<m_lfToler && dy>0)
		{
			return 90;
		}
		else if(fabs(dx)<m_lfToler && dy<0)
		{
			return 270;
		}
		else if(fabs(dy)<m_lfToler && dx<0)
		{
			return 180;
		}
		else if(fabs(dy)<m_lfToler && dx>0)
		{
			return 0;
		}
		else if(dy>=0)
		{
			return atan2(dy,dx)*m_pr;
		}
		else if(dy<0)
		{
			return atan2(dy,dx)*m_pr+360;
		}
		return 0;
	}
	//
public:
    CTopoBuildSurfaceCommand();
	virtual ~CTopoBuildSurfaceCommand();
	static CCommand* Create(){
		return new CTopoBuildSurfaceCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoBuildSurface");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
protected:
	BOOL build_surface();
private:
	//��Ҫ�����ͼ��
	CString m_HandleLayer;
	//������ͼ������
	CString m_StoreLayer;
	//������ͼ��� id; 
	int m_Layer_ID;
	//����Ƿ�ɾ��������
	BOOL m_DeleteExistFace;
	//���㾫��
	double m_lfToler;
	//����180/PI
	double m_pr;
	BOOL m_bFillAtt;
	//�������ʱ��������
	BOOL m_bBuildWithSurfacePt;
	//ɾ�������
	BOOL m_bDelSurfacePt;
private:
	//ʧ���б�
    CGeoArray geo_list;
	//��geo_listͬ�����������ָ��
	CFtrArray ftr_list;
	//����Ҫ����Ĳ��б�
	CFtrLayerArray in_layer_list;
    //���б�
	CArray<TArcItem,TArcItem> arc_list;
    //�����������б�
	CArray<TFaceItem,TFaceItem> face_list;
	//��ɫ���飻
	DWORD ColorList[10];
private:
	//��ȡ��Ҫ������ߵ�����б����ص���ĸ�����
	ULONG32 get_geo_data();
	//�������б�
	void create_arc_list();
	//�����ԱպϹ�����Ļ���
	void find_closed_arc();
	//������ʼ��׷��һ��Ψһ���棻
	void face_trace();
	//����һ���棻
	void create_face(int start_index, BOOL IsEP);//IsEP���������ep����sp
	//������ɫ��
	void set_color();
	//���湹������
	void save_data();
	//�����ڴ����ݣ�
	void clear();

	void FindSameEdgePart(int *buf, int num, int& start, int& end);
	void GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts);
	CGeoSurface * MergeSurface(CGeoSurface * pObj1, CGeoSurface * pObj2);
	void CreateSurfaces(CFtrArray& ftrs1, CFtrArray& ftrs2);
	BOOL IsInside(CGeometry *pObj_inside, CGeometry *pObj_outside);
	BOOL IsOverlapped(CPFeature f1, CPFeature f2);
	void ToNewSurface(FTR_HANDLE handle, CUndoFtrs& undo);
};

//�Ǵ�����˹���
class CTopoSurfaceNoBreakCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTopoSurfaceNoBreakCommand)
public:
	typedef CShapeLine::ShapeLineUnit ShapeUnit;

	struct IntersectItem
	{
		IntersectItem()
		{
			pFtr = NULL;
			ptIdx = -1;
			t = 0;
		}
		CFeature *pFtr;	
		PT_3D pt;	 //��������
		int ptIdx;   //�����λ�ã�����ʱ�ã�
		double t;    //���������߶εı���λ�ã�������
	};

	struct SnapItem
	{
		SnapItem()
		{
			pFtr1 = NULL;
			pFtr2 = NULL;
			ptIdx1 = -1;
			ptIdx2 = -1;
		}
		CFeature *pFtr1;
		CFeature *pFtr2;	
		PT_3D pt1;	 //�ص��������
		PT_3D pt2;	 //�ص��������
		int ptIdx1;   //�ص����λ�ã�����ʱ�ã�
		int ptIdx2;
	};
	//����״̬ö�٣�
	enum TState{HANDLED,CLOSED,VISITED,UNVISITED,INVALID};//INVALID�Ͽ��Ļ�
	//
	struct TConnectItemBase
	{
		int index;//���ӻ��Ļ����
		BOOL StoE;//���ӻ��ķ����Ƿ�����㵽�յ�
	};
	struct TConnectItem:public TConnectItemBase
	{
		double degree;//�Ƕ�
		TConnectItem *next;
		int visited;//��¼����׷�ٵ���ʼ�����
		TConnectItem()
		{
			index=-1;
			StoE=TRUE;
			degree=0;
			visited=-1;
			next=NULL;
		}
	};
	//�˵����ݽṹ��
	struct TEndPoint
	{
		PT_3DEX data;
		TConnectItem *connects;
		TConnectItem *next_arc;//׷�ٽ�����ȷ����������
		TEndPoint()
		{
			connects=NULL;
			next_arc=NULL;
		}
		inline bool operator==(const TEndPoint &rp)
		{
			if(fabs(data.x-rp.data.x)<GraphAPI::g_lfDisTolerance && fabs(data.y-rp.data.y)<GraphAPI::g_lfDisTolerance )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		inline void add_con_item(TConnectItem * p)
		{
			TConnectItem * s=connects;
			while(s->next!=0 && s->next->degree<=p->degree)
			{
				s=s->next;
			}
			p->next=s->next;
			s->next=p;
		}
	};
	//�������ݽṹ��
	struct TArcItem
	{
		int geo_index;
		int point_count;  //���ĵ�����
		TEndPoint sp;     //��ʼ�㣻
		double s_degree;  //����ʼΪ�׵����Ļ��ĽǶȣ�
		TEndPoint ep;     //β��;
		double e_degree;  //��β�����Ļ��ĽǶȣ�
		TState state;     //�û���ǰ��״̬��
		CArray<int,int> *face_index_list;   //�ɸû������ɵ�����б�;
		int FeatureFlag;//���浱ǰftr��appflag,ʹ�����ԭ
		BOOL SEVisited;//���->�յ��Ƿ������
		BOOL ESVisited;//�յ�->����Ƿ������
        TArcItem()
		{
			point_count = 0;
			state=UNVISITED;
			geo_index=-1;
			face_index_list=NULL;
			FeatureFlag = 0;
			SEVisited = FALSE;
			ESVisited = FALSE;
		}
	};
    //������ݽṹ��
	struct TFaceItem
	{
	   int  color_index;
	   int  layer_id;
       CArray<TConnectItemBase,TConnectItemBase> *arc_item_list;
	   PT_3DEX *datas;
	   int point_count;
	   TFaceItem()
	   {
		   datas=NULL;
		   point_count=0;
	   }
	};

	//�ص�����Ĳ��룬��������
	struct LayidItem
	{
		CFeature *pFtr;
		CArray<int,int> *layids;
		LayidItem()
		{
			pFtr=NULL;
		}
		void add_id(int newid)
		{
			for(int i=0; i<layids->GetSize(); i++)
			{
				if(newid == layids->GetAt(i))
					break;
			}
			if(i>=layids->GetSize())
				layids->Add(newid);
		}
		void copy_item(LayidItem& item)
		{
			for(int i=0; i<item.layids->GetSize(); i++)
			{
				add_id(item.layids->GetAt(i));
			}
		}
	};

	//���ڼ���
	struct CountItem
	{
		CArray<int,int> values;
		CArray<int,int> counts;
		void addValue(int value)
		{
			for(int i=0; i<values.GetSize(); i++)
			{
				if(values[i]==value)
				{
					counts[i]++;
					break;
				}
				else if(values[i]>value)//����
				{
					values.InsertAt(i, value);
					counts.InsertAt(i, 1);
					break;
				}
			}
			if(i>=values.GetSize())
			{
				values.Add(value);
				counts.Add(1);
			}
		}
		int getValue(int count)
		{
			int count0 = 0;
			for(int j=0; j<values.GetSize(); j++)
			{
				if(values[j]==0)
				{
					count0 = counts[j];
					break;
				}
			}
			for(int i=values.GetSize()-1; i>=0; i--)
			{
				if(counts[i]+count0>=count)
				{
					return values[i];
				}
			}
			return 0;
		}
	};

	inline double get_degree(PT_3DEX ps,PT_3DEX pe)
	{
		double dx=pe.x-ps.x;
		double dy=pe.y-ps.y;
		if(fabs(dx)<PRECISION && dy>0)
		{
			return 90;
		}
		else if(fabs(dx)<PRECISION && dy<0)
		{
			return 270;
		}
		else if(fabs(dy)<PRECISION && dx<0)
		{
			return 180;
		}
		else if(fabs(dy)<PRECISION && dx>0)
		{
			return 0;
		}
		else if(dy>=0)
		{
			return atan2(dy,dx)*m_pr;
		}
		else if(dy<0)
		{
			return atan2(dy,dx)*m_pr+360;
		}
		return 0;
	}
public:
	typedef CArray<IntersectItem,IntersectItem> IntersectItemArray;
	typedef CArray<SnapItem,SnapItem> SnapItemArray;

	CTopoSurfaceNoBreakCommand();
	virtual ~CTopoSurfaceNoBreakCommand();
	static CCommand* Create(){
		return new CTopoSurfaceNoBreakCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoSurfaceNoBreak");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
public:
	void BuildSurface(CFtrArray& inArr, CFtrArray& outArr, BOOL bProgress=FALSE);//�����CFeature*����Ӧ���ǵ����Ķ���
protected:
	int CopyDataSource(CFtrLayerArray& layers0, CFtrLayerArray& layers1);//���ؿ����ĵ�������
	void SurfaceToCurve(CFtrLayerArray& layers1);
	void ReplaceLittleSurface(CUndoFtrs& undo);

	BOOL Linearize(CFtrLayerArray& layers0);
	BOOL ProcessSuspend(CFtrLayerArray& layers0);
	BOOL CutLines(CFtrLayerArray& layers0);
	BOOL CutLines_self(CFtrLayerArray& layers0);
	BOOL SnapVertexes(CFtrLayerArray& layers0);
	BOOL DeleteCommonLine(CFtrLayerArray& layers0);
	void GetIntersects(CFeature* f1,CFeature* f2,IntersectItemArray &arr);
	void GetIntersects_self(CFeature* f1, IntersectItemArray &arr);
	void GetSnapVertexes(CFeature* f1,CFeature* f2,Envelope evlp, SnapItemArray &arr);
	double CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex);
	void ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex);
	void GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z);
	BOOL IsOverlapped(CPFeature f1, CPFeature f2);

protected:
	BOOL build_surface();
protected:
	//ʧ���б�
    CGeoArray geo_list;
	//��geo_listͬ�����������ָ��
	CFtrArray ftr_list;
	//�Ѵ���Ļ���������
	//int handled_count;
	//����Ҫ����Ĳ��б�
	CFtrLayerArray in_layer_list;
    //���б�
	CArray<TArcItem,TArcItem> arc_list;
    //�����������б�
	CArray<TFaceItem,TFaceItem> face_list;
	CArray<LayidItem,LayidItem> layid_list;
	//��ɫ���飻
	DWORD ColorList[10];
protected:
	//��ȡ��Ҫ������ߵ�����б����ص���ĸ�����
	ULONG32 get_geo_data();
	//�������б�
	void create_arc_list();
	//�����ԱպϹ�����Ļ���
	void find_closed_arc();
	//������ʼ��׷��һ��Ψһ���棻
	void face_trace();
	//����һ���棻
	void create_face(int start_index, BOOL IsEP);//IsEP���������ep����sp
	//������ɫ��
	void set_color();
	//���湹������
	void save_data();
	//�����ڴ����ݣ�
	void clear();

	void FindSameEdgePart(int *buf, int num, int& start, int& end);
	void GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts);
	CGeoSurface * MergeSurface(CGeoSurface * pObj1, CGeoSurface * pObj2);
	void CreateSurfaces(CFtrArray& ftrs1, CFtrArray& ftrs2);
	BOOL IsInside(CGeometry *pObj_inside, CGeometry *pObj_outside);
	CPFeature ToNewSurface(FTR_HANDLE handle, CUndoFtrs& undo);
	
protected:
	//��Ҫ�����ͼ��
	CString m_HandleLayer;
	//������ͼ������
	CString m_StoreLayer;
	CString m_ReplaceLayer;
	//������ͼ��� id; 
	int m_Layer_ID;
	//����Ƿ�ɾ��������
	BOOL m_DeleteExistFace;
	//���㾫�ȣ�
	double PRECISION;
	double m_pr;

	CDataSourceEx *m_pTempDS;//��ʱ����Դ
	CDataQueryEx *m_pDQ;
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
	BOOL m_bFillAtt;
	BOOL m_bBuildWithSurfacePt;
	BOOL m_bDelSurfacePt;//����ɹ��Ƿ�ɾ�������

	BOOL m_bProgress;//�Ƿ���ʾ������

public:
	BOOL m_bCreateMultiSurface; //�Ƿ񴴽�������
};

//���㹹��
class CTopoSurfaceFromPtCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTopoSurfaceFromPtCommand)
public:
	CTopoSurfaceFromPtCommand();
	virtual ~CTopoSurfaceFromPtCommand();
	static CCommand* Create(){
		return new CTopoSurfaceFromPtCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoSurfaceFromPt");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();

protected:
	CString m_StoreLayer;
};


// 
//�������滻����
class CTopoReplaceLinesCommand : public CEditCommand  
{
   DECLARE_DYNCREATE(CTopoReplaceLinesCommand)
public:
    CTopoReplaceLinesCommand();
	virtual ~CTopoReplaceLinesCommand();
	static CCommand* Create(){
		return new CTopoReplaceLinesCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoReplaceLines");
	}
	virtual CString Name();
	void Abort();
	void Start();
	virtual void Finish();
	virtual void PtClick(PT_3D &pt, int flag);
protected:
	BOOL replace_lines();
};

class CCreateFigureCommand : CDrawCommand
{
	DECLARE_DYNCREATE(CCreateFigureCommand)
public:
	CCreateFigureCommand();
	virtual ~CCreateFigureCommand();
	static CCommand* Create(){
		return new CCreateFigureCommand;
	}
	virtual CString AccelStr(){
		return _T("CreateFigure");
	}
	virtual CString Name();
	void Start();
	virtual void PtClick(PT_3D &pt, int flag);
};


//�����չ���Ը��Ƶ���
class CCopyXAttrOfPointCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCopyXAttrOfPointCommand)
public:
	CCopyXAttrOfPointCommand();
	virtual ~CCopyXAttrOfPointCommand();
	static CCommand* Create(){
		return new CCopyXAttrOfPointCommand;
	}
	virtual CString AccelStr(){
		return _T("CopyXAttrOfPointToSurface");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();

private:
	CString m_strLayer;		//�����
	CString m_strLayer2;	//�����
protected:
	BOOL CopyXAttr(CAttributesSource* pXDS, CFeature* pSrc, CFeature* pDes);
};


//��GB��תΪCC��
class CTransformGB2CCCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTransformGB2CCCommand)
public:
	CTransformGB2CCCommand();
	virtual ~CTransformGB2CCCommand();
	static CCommand* Create(){
		return new CTransformGB2CCCommand;
	}
	virtual CString AccelStr(){
		return _T("TransformGB2CC");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();

private:
	CString m_strLayer;
	BOOL UpdateXAttr(CValueTable& tab,std::map<string,string>& refTable);			//�����չ�����Ƿ���gb��cc
};


//��CC��תΪGB��
class CTransformCC2GBCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTransformCC2GBCommand)
public:
	CTransformCC2GBCommand();
	virtual ~CTransformCC2GBCommand();
	static CCommand* Create(){
		return new CTransformCC2GBCommand;
	}
	virtual CString AccelStr(){
		return _T("TransformCC2GB");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();

private:
	CString m_strLayer;
	BOOL UpdateXAttr(CValueTable& tab,std::map<string,string>& refTable);			//�����չ�����Ƿ���gb��cc
};

//��ķǹ����������ɵ����
class CSurNoOverPartCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CSurNoOverPartCommand)
public:
	typedef CShapeLine::ShapeLineUnit ShapeUnit;

	struct IntersectItem
	{
		IntersectItem()
		{
			pFtr = NULL;
			ptIdx = -1;
			t = 0;
		}
		CFeature *pFtr;	
		PT_3D pt;	 //��������
		int ptIdx;   //�����λ�ã�����ʱ�ã�
		double t;    //���������߶εı���λ�ã�������
	};

	struct SnapItem
	{
		SnapItem()
		{
			pFtr1 = NULL;
			pFtr2 = NULL;
			ptIdx1 = -1;
			ptIdx2 = -1;
		}
		CFeature *pFtr1;
		CFeature *pFtr2;	
		PT_3D pt1;	 //�ص��������
		PT_3D pt2;	 //�ص��������
		int ptIdx1;   //�ص����λ�ã�����ʱ�ã�
		int ptIdx2;
	};

public:
	typedef CArray<IntersectItem,IntersectItem> IntersectItemArray;
	typedef CArray<SnapItem,SnapItem> SnapItemArray;

	CSurNoOverPartCommand();
	virtual ~CSurNoOverPartCommand();
	static CCommand* Create(){
		return new CSurNoOverPartCommand;
	}
	virtual CString AccelStr(){
		return _T("SurNoOverPartToArea");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();

	void GetNoOverPart();

protected:
	int CopyDataSource(CFtrArray& ftrs, CFtrLayerArray& arr);
		
	BOOL Linearize(CFtrLayerArray& layers0);
	BOOL ProcessSuspend(CFtrLayerArray& layers0);
	BOOL CutLines(CFtrLayerArray& layers0);
	BOOL CutLines_self(CFtrLayerArray& layers0);
	BOOL SnapVertexes(CFtrLayerArray& layers0);
	BOOL DeleteCommonLine(CFtrLayerArray& layers0);
	void GetIntersects(CFeature* f1,CFeature* f2,IntersectItemArray &arr);
	void GetIntersects_self(CFeature* f1, IntersectItemArray &arr);
	void GetSnapVertexes(CFeature* f1,CFeature* f2,Envelope evlp, SnapItemArray &arr);
	double CalcSnapDistance(CPFeature f1, CPFeature f2, int nPtIndex);
	void ProcessOneFeature(CPFeature f1, CPFeature f2, int nPtIndex);
	void GetIntersectPoints(PT_3DEX pt0, PT_3DEX pt1, FTR_HANDLE id, CArray<double,double> &ret1, CArray<double,double> &ret2, CArray<double,double> &ret_z);
	BOOL IsOverlapped(CPFeature f1, CPFeature f2);

protected:
	CString m_targetLayer;
	CDataSourceEx *m_pDS;
	CDataSourceEx *m_pTempDS;
	CDataQueryEx *m_pDQ;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
	double m_lfToler;
};

//������������
class CRiverAddDirectCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CRiverAddDirectCommand)
public:
	CRiverAddDirectCommand();
	virtual ~CRiverAddDirectCommand();
	static CCommand* Create(){
		return new CRiverAddDirectCommand;
	}
	virtual CString AccelStr(){
		return _T("RiverAddDirect");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
protected:
	void AddDirPoint(CFeature *pFtr, CUndoFtrs& undo);
	BOOL prepare();
	
protected:
	int m_nOperation;//������ʽ��0ȫͼ��1ѡ��
	CString m_RiverLayer;
	CString m_dirpointLayer;
	double m_deta;//������cm
	double m_offset;//����ƫ��cm
	CDataSourceEx *m_pDS;

	double m_d;//������
	double m_off;//����ƫ��

	CFeature *m_pFtrTempl;//�����ģ��
	int m_layid;
};

#include "editbasedoc.h"
//���϶���
class CCheckSurfaceGapCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CCheckSurfaceGapCommand)
public:
	struct item
	{
		item()
		{
			pFtr=NULL;
		}
		CFeature *pFtr;
		Envelope e;
	};
public:
	CCheckSurfaceGapCommand();
	virtual ~CCheckSurfaceGapCommand();
	static CCommand* Create(){
		return new CCheckSurfaceGapCommand;
	}
	virtual CString AccelStr(){
		return _T("CheckSurfaceGap");
	}
	virtual CString Name();
	void PtMove(PT_3D &pt);
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
private:
	PT_3D m_pt0;
	PT_3D m_pt1;
	CMYTinObj m_tin;
	int *m_flags;//-1��ʾ�����϶��flags����0ʱ����ͬ�ı�ʾһ��
	void MergeTins();
	int m_nOperation;//������ʽ��0ȫͼ���ӱ��� 1ȫͼ+���������� 2�ֻ����
public:
	double m_lfArea;
	BOOL m_bAddCheckResult;//����������ʱΪFALSE
};

//��Ĺ����������ɵ����
class CSurOverPartCommand : public CSurNoOverPartCommand  
{
	DECLARE_DYNCREATE(CSurOverPartCommand)	
public:
	
	CSurOverPartCommand();
	virtual ~CSurOverPartCommand();
	static CCommand* Create(){
		return new CSurOverPartCommand;
	}
	virtual CString AccelStr(){
		return _T("SurOverPartToArea");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
	
	void GetCommonLine(CFtrLayerArray& layers0, CGeoArray& geos);
	void GetOverPart(CFeature* pFtr1,CFeature* pFtr2);
	
protected:
	void MergeCurves(CGeoArray &arr);
};

//��ı߽����ɵ����
class CSurfaceToBoundCommand : public CSurNoOverPartCommand
{
	DECLARE_DYNCREATE(CSurfaceToBoundCommand)
public:

	CSurfaceToBoundCommand();
	virtual ~CSurfaceToBoundCommand();
	static CCommand* Create(){
		return new CSurfaceToBoundCommand;
	}
	virtual CString AccelStr(){
		return _T("SurfaceToBound");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);

	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab, BOOL bInit = FALSE);

	void Abort();
	void Start();
	virtual void Finish();

protected:
	CString m_excludelayers;
	CString m_handleLayers;
};

//�����߳̽���
class CRiverZChangeCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CRiverZChangeCommand)
public:
	CRiverZChangeCommand();
	virtual ~CRiverZChangeCommand();
	static CCommand* Create(){
		return new CRiverZChangeCommand;
	}
	virtual CString AccelStr(){
		return _T("RiverZChange");
	}
	virtual CString Name();
	virtual void PtClick(PT_3D &pt, int flag);
	
	void GetParams(CValueTable& tab);
	virtual void FillShowParams(CUIParam* param, BOOL bForLoad = FALSE);
	virtual void SetParams(CValueTable& tab,BOOL bInit=FALSE);
	
	void Abort();
	void Start();
	virtual void Finish();
protected:
	BOOL ZChange(CArray<PT_3DEX,PT_3DEX>& pts, CArray<PT_3DEX,PT_3DEX>& newPts);
	
protected:
	CString m_RiverLayer;
	BOOL m_bZInc;//�߳��Ƿ����
};

//��������㹹��
class CTopoSurfaceRegionCommand : public CEditCommand
{
	DECLARE_DYNCREATE(CTopoSurfaceRegionCommand)
public:
	CTopoSurfaceRegionCommand();
	virtual ~CTopoSurfaceRegionCommand();
	static CCommand* Create(){
		return new CTopoSurfaceRegionCommand;
	}
	virtual CString AccelStr(){
		return _T("TopoSurfaceRegion");
	}
	virtual CString Name();

	virtual void PtMove(PT_3D& pt);
	virtual void PtClick(PT_3D &pt, int flag);
	void Abort();
	void Start();

protected:
	PT_3D m_ptDragStart;
	PT_3D m_ptDragEnd;
};

#endif