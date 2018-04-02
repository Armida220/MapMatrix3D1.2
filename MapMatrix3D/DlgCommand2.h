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



//生成坐标标注线
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
	float   m_fInterval;	//格网间距，单位cm，缺省 10 cm
	int		m_nDirection;   //方向：0水平，1垂直；
	int		m_nPosition;	//位置：0左（上），1右（下）
	
	//内外图廓线
	static CArray<PT_3DEX,PT_3DEX> m_arrBound1;
	static CArray<PT_3DEX,PT_3DEX> m_arrBound2;
	
};


//生成坐标注记
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

	int m_nAlign;  //对齐方式：0左对齐，1居中，2右对齐

	float m_fDX, m_fDY;  //横向间距，纵向间距；单位 mm

	CString m_strFontName;  //字体名
	float m_fFontSize;		//字体大小
	float m_fWidthScale;	//字宽倍率

	float m_fCoordUnit;		//坐标单位(1米，1000米，等等)
	int m_nDecimal;			//小数尾数
	BOOL m_bUseNEMark;		//是否带有NE标志
};


//设置文字角度与直线一致
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


//线线悬挂处理
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
	
	//被处理的图层
	CString m_strLayer0;

	//参考图层
	CString m_strLayer1;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
};



//点线悬挂处理
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
	
	//点的图层
	CString m_strLayer0;
	
	//线的图层
	CString m_strLayer1;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;

	//修改方式
	int m_nModifyType; //typeMovePt, typeMoveLine
};

//处理自相交
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
		PT_3D pt;	 //交点坐标
		int ptIdx;   //交点的位置（排序时用）
		double t;    //交点所在线段的比例位置，排序用
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


//拓扑预处理
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
		PT_3D pt;	 //交点坐标
		int ptIdx;   //交点的位置（排序时用）
		double t;    //交点所在线段的比例位置，排序用
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
		PT_3D pt1;	 //重叠点的坐标
		PT_3D pt2;	 //重叠点的坐标
		int ptIdx1;   //重叠点的位置（排序时用）
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
	
	//被处理的图层
	CString m_strLayer0;
	
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
};



//手工匹配节点
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



//全图自动匹配节点
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
	//采用消除悬挂点的方式进行处理
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
//删除拓扑面
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


//通过面域点刷面的图层和属性
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


//通过面域点刷面的图层和属性(选择集)
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
//拓扑构面
class CTopoBuildSurfaceCommand : public CEditCommand  
{
	DECLARE_DYNCREATE(CTopoBuildSurfaceCommand)
public:
	friend class CTopoDelSurfaceCommand;
	friend class CTopoUnionSurfaceCommand;
public:
	//弧的状态枚举；
	enum TState{HANDLED,CLOSED,VISITED,UNVISITED,INVALID};//INVALID断开的弧
	//
	struct TConnectItemBase
	{
		int index;//连接弧的弧序号
		BOOL StoE;//连接弧的方向是否由起点到终点
	};
	struct TConnectItem:public TConnectItemBase
	{
		double degree;//角度
		TConnectItem *next;
		int visited;//记录正在追踪的起始弧序号
		TConnectItem()
		{
			index=-1;
			StoE=TRUE;
			degree=0;
			visited=-1;
			next=NULL;
		}
	};
	//端点数据结构；
	struct TEndPoint
	{
		PT_3DEX data;
		TConnectItem *connects;
		TConnectItem *next_arc;//追踪结束后确定的那条弧
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
	//弧的数据结构；
	struct TArcItem
	{
		int geo_index;
		int point_count;  //弧的点数量
		TEndPoint sp;     //起始点；
		double s_degree;  //已起始为首点计算的弧的角度；
		TEndPoint ep;     //尾点;
		double e_degree;  //已尾点计算的弧的角度；
		TState state;     //该弧当前的状态；
		CArray<int,int> *face_index_list;   //由该弧所构成的面的列表;
		int FeatureFlag;//保存当前ftr的appflag,使用完后还原
		BOOL SEVisited;//起点->终点是否遍历过
		BOOL ESVisited;//终点->起点是否遍历过
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
    //面的数据结构；
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
	//需要处理的图层
	CString m_HandleLayer;
	//存放面的图层名；
	CString m_StoreLayer;
	//存放面的图层的 id; 
	int m_Layer_ID;
	//标记是否删除已有面
	BOOL m_DeleteExistFace;
	//浮点精度
	double m_lfToler;
	//常量180/PI
	double m_pr;
	BOOL m_bFillAtt;
	//有面域点时才生成面
	BOOL m_bBuildWithSurfacePt;
	//删除面域点
	BOOL m_bDelSurfacePt;
private:
	//失量列表；
    CGeoArray geo_list;
	//和geo_list同步，保存地物指针
	CFtrArray ftr_list;
	//所需要处理的层列表；
	CFtrLayerArray in_layer_list;
    //弧列表；
	CArray<TArcItem,TArcItem> arc_list;
    //所构建的面列表；
	CArray<TFaceItem,TFaceItem> face_list;
	//颜色数组；
	DWORD ColorList[10];
private:
	//获取需要处理的线地物的列表，返回地物的个数；
	ULONG32 get_geo_data();
	//创建弧列表；
	void create_arc_list();
	//处理自闭合构成面的弧；
	void find_closed_arc();
	//根据起始弧追踪一个唯一的面；
	void face_trace();
	//创建一个面；
	void create_face(int start_index, BOOL IsEP);//IsEP：构面的是ep还是sp
	//给面上色；
	void set_color();
	//保存构面结果；
	void save_data();
	//清理内存数据；
	void clear();

	void FindSameEdgePart(int *buf, int num, int& start, int& end);
	void GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts);
	CGeoSurface * MergeSurface(CGeoSurface * pObj1, CGeoSurface * pObj2);
	void CreateSurfaces(CFtrArray& ftrs1, CFtrArray& ftrs2);
	BOOL IsInside(CGeometry *pObj_inside, CGeometry *pObj_outside);
	BOOL IsOverlapped(CPFeature f1, CPFeature f2);
	void ToNewSurface(FTR_HANDLE handle, CUndoFtrs& undo);
};

//非打断拓扑构面
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
		PT_3D pt;	 //交点坐标
		int ptIdx;   //交点的位置（排序时用）
		double t;    //交点所在线段的比例位置，排序用
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
		PT_3D pt1;	 //重叠点的坐标
		PT_3D pt2;	 //重叠点的坐标
		int ptIdx1;   //重叠点的位置（排序时用）
		int ptIdx2;
	};
	//弧的状态枚举；
	enum TState{HANDLED,CLOSED,VISITED,UNVISITED,INVALID};//INVALID断开的弧
	//
	struct TConnectItemBase
	{
		int index;//连接弧的弧序号
		BOOL StoE;//连接弧的方向是否由起点到终点
	};
	struct TConnectItem:public TConnectItemBase
	{
		double degree;//角度
		TConnectItem *next;
		int visited;//记录正在追踪的起始弧序号
		TConnectItem()
		{
			index=-1;
			StoE=TRUE;
			degree=0;
			visited=-1;
			next=NULL;
		}
	};
	//端点数据结构；
	struct TEndPoint
	{
		PT_3DEX data;
		TConnectItem *connects;
		TConnectItem *next_arc;//追踪结束后确定的那条弧
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
	//弧的数据结构；
	struct TArcItem
	{
		int geo_index;
		int point_count;  //弧的点数量
		TEndPoint sp;     //起始点；
		double s_degree;  //已起始为首点计算的弧的角度；
		TEndPoint ep;     //尾点;
		double e_degree;  //已尾点计算的弧的角度；
		TState state;     //该弧当前的状态；
		CArray<int,int> *face_index_list;   //由该弧所构成的面的列表;
		int FeatureFlag;//保存当前ftr的appflag,使用完后还原
		BOOL SEVisited;//起点->终点是否遍历过
		BOOL ESVisited;//终点->起点是否遍历过
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
    //面的数据结构；
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

	//重叠地物的层码，包括自身
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

	//用于计数
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
				else if(values[i]>value)//排序
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
	void BuildSurface(CFtrArray& inArr, CFtrArray& outArr, BOOL bProgress=FALSE);//传入的CFeature*对象应该是单独的对象
protected:
	int CopyDataSource(CFtrLayerArray& layers0, CFtrLayerArray& layers1);//返回拷贝的地物数量
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
	//失量列表；
    CGeoArray geo_list;
	//和geo_list同步，保存地物指针
	CFtrArray ftr_list;
	//已处理的弧的数量；
	//int handled_count;
	//所需要处理的层列表；
	CFtrLayerArray in_layer_list;
    //弧列表；
	CArray<TArcItem,TArcItem> arc_list;
    //所构建的面列表；
	CArray<TFaceItem,TFaceItem> face_list;
	CArray<LayidItem,LayidItem> layid_list;
	//颜色数组；
	DWORD ColorList[10];
protected:
	//获取需要处理的线地物的列表，返回地物的个数；
	ULONG32 get_geo_data();
	//创建弧列表；
	void create_arc_list();
	//处理自闭合构成面的弧；
	void find_closed_arc();
	//根据起始弧追踪一个唯一的面；
	void face_trace();
	//创建一个面；
	void create_face(int start_index, BOOL IsEP);//IsEP：构面的是ep还是sp
	//给面上色；
	void set_color();
	//保存构面结果；
	void save_data();
	//清理内存数据；
	void clear();

	void FindSameEdgePart(int *buf, int num, int& start, int& end);
	void GetObjectPart(CGeometry* pObj, int start, int end, CArray<PT_3DEX,PT_3DEX>& arrPts);
	CGeoSurface * MergeSurface(CGeoSurface * pObj1, CGeoSurface * pObj2);
	void CreateSurfaces(CFtrArray& ftrs1, CFtrArray& ftrs2);
	BOOL IsInside(CGeometry *pObj_inside, CGeometry *pObj_outside);
	CPFeature ToNewSurface(FTR_HANDLE handle, CUndoFtrs& undo);
	
protected:
	//需要处理的图层
	CString m_HandleLayer;
	//存放面的图层名；
	CString m_StoreLayer;
	CString m_ReplaceLayer;
	//存放面的图层的 id; 
	int m_Layer_ID;
	//标记是否删除已有面
	BOOL m_DeleteExistFace;
	//浮点精度；
	double PRECISION;
	double m_pr;

	CDataSourceEx *m_pTempDS;//临时数据源
	CDataQueryEx *m_pDQ;
	double m_lfToler;
	BOOL m_bSnap3D;
	BOOL m_bAddPt;
	BOOL m_bFillAtt;
	BOOL m_bBuildWithSurfacePt;
	BOOL m_bDelSurfacePt;//构面成功是否删除面域点

	BOOL m_bProgress;//是否显示进度条

public:
	BOOL m_bCreateMultiSurface; //是否创建复杂面
};

//单点构面
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
//拓扑面替换边线
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


//点的扩展属性复制到面
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
	CString m_strLayer;		//面层名
	CString m_strLayer2;	//点层名
protected:
	BOOL CopyXAttr(CAttributesSource* pXDS, CFeature* pSrc, CFeature* pDes);
};


//将GB码转为CC码
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
	BOOL UpdateXAttr(CValueTable& tab,std::map<string,string>& refTable);			//检查扩展属性是否有gb和cc
};


//将CC码转为GB码
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
	BOOL UpdateXAttr(CValueTable& tab,std::map<string,string>& refTable);			//检查扩展属性是否有gb和cc
};

//面的非公共部分生成地类界
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
		PT_3D pt;	 //交点坐标
		int ptIdx;   //交点的位置（排序时用）
		double t;    //交点所在线段的比例位置，排序用
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
		PT_3D pt1;	 //重叠点的坐标
		PT_3D pt2;	 //重叠点的坐标
		int ptIdx1;   //重叠点的位置（排序时用）
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

//河流增加流向
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
	int m_nOperation;//操作方式：0全图，1选择
	CString m_RiverLayer;
	CString m_dirpointLayer;
	double m_deta;//纵向间隔cm
	double m_offset;//横向偏移cm
	CDataSourceEx *m_pDS;

	double m_d;//纵向间隔
	double m_off;//很想偏移

	CFeature *m_pFtrTempl;//有向点模板
	int m_layid;
};

#include "editbasedoc.h"
//面缝隙检查
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
	int *m_flags;//-1表示非面缝隙；flags大于0时，相同的表示一组
	void MergeTins();
	int m_nOperation;//操作方式：0全图不加边线 1全图+工作区边线 2手绘边线
public:
	double m_lfArea;
	BOOL m_bAddCheckResult;//检查命令调用时为FALSE
};

//面的公共部分生成地类界
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

//面的边界生成地类界
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

//河流高程渐变
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
	BOOL m_bZInc;//高程是否递增
};

//区域面域点构面
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