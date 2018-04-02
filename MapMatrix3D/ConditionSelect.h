// ConditionSelect.h : APIs use for condition select
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONDITIONSELECT_H__35D1FE7C_0B36_4003_B651_C81E994E65E8__INCLUDED_)
#define AFX_CONDITIONSELECT_H__35D1FE7C_0B36_4003_B651_C81E994E65E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "DlgConditionSelect.h"
//#include "GeoBaseType.h"
#include "DlgDataSource.H"
//#include "Search.h"

class CDlgDoc;

#define OP_EQUAL				0
#define OP_UNEQUAL				1
#define OP_GREATER				2
#define OP_LESS					3

// 0, 无效结果; 1, 小于; 2, 等于; 4, 大于;
#define COMP_ERROR					0
#define COMP_LESS					1
#define COMP_EQUAL					2
#define COMP_GREATER				4

#define COMP_CURFALSE				0	
#define COMP_CURTRUE				1
			

//－－－－－－－－－－－－－－－比较结果的详细数据
//属性信息结构
struct CP_ATTR
{
	int index; //属性所在的序号
	_variant_t value; //属性值
};

//一个节点的信息
struct CP_POINT
{
	int ptidx; //节点号
	CArray<CP_ATTR,CP_ATTR> arrAttrs; //节点的属性信息
	CP_POINT& operator=(CP_POINT& info)
	{
		ptidx = info.ptidx;
		arrAttrs.Copy(info.arrAttrs);
		return *this;
	}
};


//一个地物的查询结果数据
struct CP_RESULT
{
	CFeature *m_pFtr;
	CArray<CP_ATTR,CP_ATTR> arrAttrs; //满足条件的属性表
	CPtrArray arrPoints; //满足条件的节点,存放CP_POINT的指针

	CP_RESULT& operator=(CP_RESULT& r)
	{
		m_pFtr = r.m_pFtr;
		arrAttrs.Copy(r.arrAttrs);
		int np = r.arrPoints.GetSize(), i;
		CP_POINT *p, *t;
		for( i=0; i<np; i++)
		{
			p = new CP_POINT;
			t = (CP_POINT*)r.arrPoints.GetAt(i);
			if( p && t )
			{
				*p = *t;
				arrPoints.Add(p);
			}
		}
		return *this;
	}

	void Clear()
	{
		int np = arrPoints.GetSize();
		for( int j=0; j<np; j++)
		{
			CP_POINT *p = (CP_POINT*)arrPoints.GetAt(j);
			if( p )delete p;
		}
		arrAttrs.RemoveAll();
		arrPoints.RemoveAll();
	}
};

class CCPResultList
{
public:
	CCPResultList();
	CCPResultList(CStringArray& nameList);
	~CCPResultList();
	void Copy(CCPResultList& list);
	void CombineList(CCPResultList& list);
	void SubtractList(CCPResultList& list);
	void Clear();

	void StartRecord();
	void AbortRecord();
	void FinishRecord();

	void SetCurFtr(CFeature *pFt);
	void SetCurAttr(int idx);
	void AddAttrResult(_variant_t value);
	void AddPointResult(int ptidx, _variant_t value);
	CFeature* GetCurFtr(){ return curResult.m_pFtr; }

	CStringArray attrNameList;
	CPtrArray resultList;
private:
	void Combine();
	void AddAttrResult(CP_ATTR *attr);
	void AddPointResult(CP_POINT* pt);

	CP_RESULT curResult;
	int nCurAttr;//, nCombineType;
};

typedef BOOL (*PFUNCOMP)(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较地物任意属性
BOOL CompField(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r); 

//比较节点 x 坐标
BOOL CompX(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较节点 y 坐标
BOOL CompY(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较节点 z 坐标
BOOL CompZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较节点 z 是否为高程步距的整数倍
BOOL CompIZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较相邻节点间高差
BOOL CompDZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较相邻节点间距
BOOL CompPerDis(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较折线尖角
BOOL CompPerAngle(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较首尾节点距离
BOOL CompEndDis(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//比较节点线型
BOOL CompLineType(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//顺/逆时针方向
BOOL CompClockwise(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//长度
BOOL CompLen(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//面积
BOOL CompArea(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//自相交
BOOL CompIntersectSelf(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//悬挂点
BOOL CompSuspend(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//水流反向 by shy
BOOL StreamReverse(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//公共点
BOOL CompSamePoint(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//公共边
BOOL CompSameLine(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//边相交
BOOL CompIntersectLine(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r);

//条件比较结构
struct CONDCOMP 
{
	UINT strid;
	int	 id;
	PFUNCOMP pfun;
};

typedef BOOL (*PFUN_CHECKOBJ)(CFeature *pFtr, CFtrLayer *pLayer);
class CSearchMultiObj
{
public:
	CSearchMultiObj();
	virtual ~CSearchMultiObj();

	void Init(CDlgDataSource *pDataSouce,CCoordSys *pCoordSys);

	//计算分割的格网的原点、宽度、数目；
	void Prepare(int intensity=1);

	//计算分割的格网的原点、宽度、数目；
	void PrepareWithRadius(double r);

	//清空临时数据
	void Clear();

	//查找所有 pt 落在其基线上的地物，但不包括 pFtr0
	int FindObjectInRect(CFeature *pFtr0, PT_3D *pt, Envelope evlp, BOOL bForPoint=TRUE);

	//查找所有与线段pt0,pt1相交的地物，但不包括 pFtr0
	int FindIntersectInRect(CFeature *pFtr0, PT_3D pt0, PT_3D pt1);

	int GetFoundObjectHandles(FTR_HANDLE *pFtrHandle);

	inline void GetGridSize(double& dx, double& dy){
		dx = m_lfDX; dy = m_lfDY;
	}
	inline void SetCheckFunc(PFUN_CHECKOBJ pfun){
		m_pFuncCheckObj = pfun;
	}
	inline void SetDataSource(CDlgDataSource *pDataSource){
		m_pDataSource = pDataSource;
	}
	inline CDlgDataSource* GetDataSource(){
		return m_pDataSource;
	}
	inline PFUN_CHECKOBJ GetCheckFunc()const{
		return m_pFuncCheckObj;
	}
	inline CCoordSys* GetCoorSys(){
		return m_pCoorSys;
	}

private:
	//按照格网将所有的地物分类到格网中
	void CalcGridForPoint();
	void CalcGridForLine();
	double m_lfOX, m_lfOY;
	double m_lfDX, m_lfDY;
	int m_nx, m_ny;
	CPtrArray m_arrObjs;
	BOOL m_bCalcGrid;

	CDlgDataSource *m_pDataSource;

	CArray<FTR_HANDLE,FTR_HANDLE> m_foundHandles;

	CCoordSys *m_pCoorSys;

	PFUN_CHECKOBJ m_pFuncCheckObj;
};


class CLinesSearch
{
public:
	CLinesSearch();
	~CLinesSearch();
	void Init(const PT_3D *pts, int num);
	int *FindNearLines(PT_3D pt0, PT_3D pt1, int &num);
private:
	double m_lfOX, m_lfOY;
	double m_lfDX, m_lfDY;
	int m_nx, m_ny;
	CArray<int,int> *m_pArrIndex;
	CArray<int,int> m_arrFound;
};


extern CSearchMultiObj gSSP_forPoint, gSSP_forLine;

#endif // !defined(AFX_CONDITIONSELECT_H__35D1FE7C_0B36_4003_B651_C81E994E65E8__INCLUDED_)