//本文件定义搜索对象时使用的基本搜索结构

#include "SmartViewBaseType.h"
#include "CoordSys.h"
#include "GrBuffer.h "

#include "MemoryPool.h"

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_EDITBASE_SEARCHSTRUCT_INCLUDED
#define _INC_EDITBASE_SEARCHSTRUCT_INCLUDED

// SearchRange 的类型定义
#define EBSR_NONE					0
#define EBSR_RECT					1
#define EBSR_RADIUS					2
#define EBSR_NEARESTOBJ				3
#define EBSR_INTERSECTOBJ			4
#define EBSR_POLYGON				5
#define EBSR_RECT_SIMPLE			6

// SearchUnit 的类型定义
#define EBSU_NONE					0
#define EBSU_POINT					1
#define EBSU_LINE					2
#define EBSU_GRBUF					3
#define EBSU_OBJECT					-1

// Searcher 的类型定义
#define EBSE_NONE					0
#define EBSE_TREE8					1

MyNameSpaceBegin

//广义上的搜索范围的定义，例如，点半径搜索，矩形搜索，多边形搜索，直线段搜索
struct SearchRange
{
	int type;
	int forSymbolized;
	CCoordSys *pCS;
};

//矩形搜索
struct SearchRect : public SearchRange
{
	SearchRect()
	{
		type = EBSR_RECT;
		pCS = NULL;
		bEntireInclude = FALSE;
		forSymbolized = TRUE;
	}
	SearchRect(Envelope e, CCoordSys *p, BOOL bEntInc = FALSE, BOOL symbolized = TRUE){
		type = EBSR_RECT;
		evlp = e;
		pCS = p;
		bEntireInclude = bEntInc;
		forSymbolized = symbolized;
	}
	Envelope evlp;
	BOOL bEntireInclude;
};


//矩形搜索
struct SearchRectSimple : public SearchRect
{
	SearchRectSimple()
	{
		type = EBSR_RECT_SIMPLE;
		pCS = NULL;
		bEntireInclude = FALSE;
		forSymbolized = TRUE;
	}
	SearchRectSimple(Envelope e, CCoordSys *p, BOOL bEntInc = FALSE, BOOL symbolized = TRUE){
		type = EBSR_RECT_SIMPLE;
		evlp = e;
		pCS = p;
		bEntireInclude = bEntInc;
		forSymbolized = symbolized;
	}
};


//点半径搜索
struct SearchNearest : public SearchRange
{
	SearchNearest()
	{
		type = EBSR_RADIUS;
		r = 0;
		pCS = NULL;
		forSymbolized = TRUE;
	}

	SearchNearest(PT_3D t, double lfr, CCoordSys *p, BOOL symbolized = TRUE){
		type = EBSR_RADIUS;
		pt = t;	r = lfr;
		pCS = p;
		forSymbolized = symbolized;
	}
	PT_3D pt;
	double r;
};

struct SearchNearestObj : public SearchRange
{
	SearchNearestObj()
	{
		type = EBSR_NEARESTOBJ;
		pCS = NULL;
		forSymbolized = TRUE;
	}

	SearchNearestObj(PT_3D t, CCoordSys *p, BOOL symbolized = TRUE){
		type = EBSR_NEARESTOBJ;
		pt = t;	
		pCS = p;
		forSymbolized = symbolized;
	}
	PT_3D pt;
};

struct SearchIntersectObj : public SearchRange
{
	SearchIntersectObj()
	{
		type = EBSR_INTERSECTOBJ;
		pts = NULL;	num = 0;
		pCS = NULL;
		forSymbolized = FALSE;
	}

	SearchIntersectObj(PT_3D *arr,int len, Envelope e, CCoordSys *p, BOOL symbolized = TRUE){
		type = EBSR_INTERSECTOBJ;
		pts = arr;	num = len;
		pCS = p;
		forSymbolized = symbolized;
		evlp = e;
	}
	PT_3D *pts; 
	int num;
	Envelope evlp;
};

struct SearchPolygon : public SearchRange
{
	SearchPolygon()
	{
		type = EBSR_POLYGON;
		pts = NULL;	num = 0;
		pCS = NULL;
		forSymbolized = FALSE;
	}

	SearchPolygon(PT_3D *arr,int len, CCoordSys *p, BOOL symbolized = TRUE){
		type = EBSR_POLYGON;
		pts = arr;	num = len;
		pCS = p;
		forSymbolized = symbolized;
	}
	PT_3D *pts; 
	int num;
};

//可搜索的数据单元
struct SearchableUnit
{
	long type;
};

//点单元：数据量小，就特殊处理
struct EXPORT_EDITBASE SearchablePoint : public SearchableUnit
{
	SearchablePoint(void *t=NULL){
		p = t;
		type = EBSU_POINT;
	}
	//CGeoPoint* 对象
	void *p;

	DECLARE_MEMPOOL(SearchablePoint);
};


//线单元：为了减少内存，增加执行速度，仍然特殊处理
struct EXPORT_EDITBASE SearchableLine : public SearchableUnit
{
	SearchableLine(void *t=NULL){
		p = t;
		type = EBSU_LINE;
	}
	//CShapeLine::ShapeLineUnit* 对象
	void *p;

	DECLARE_MEMPOOL(SearchableLine);
};

//图形单元：支持更复杂的图形地物，主要用于文字和平行线
struct EXPORT_EDITBASE SearchableGrBuf : public SearchableUnit
{
	SearchableGrBuf(){
		p = NULL;
		type = EBSU_GRBUF;
	}
	~SearchableGrBuf(){
	}
	GrBuffer* CreateBuf(){
		ClearBuf();
		p = new GrBuffer();
		return p;
	}
	void ClearBuf(){
		if( p )delete p;
		p = NULL;
	}
	void RefreshEnvelope(){
		if( p!=NULL )
			e = p->GetEnvelope();
	}
	//GrBuffer* 对象
	GrBuffer *p;
	Envelope e;

	DECLARE_MEMPOOL(SearchableGrBuf);
};


//一般单元，可用于扩展
struct EXPORT_EDITBASE SearchableAgentObj
{
	SearchableAgentObj();
	~SearchableAgentObj();

	//获得矩形外包
	virtual Envelope GetEnvelope()const;

	virtual double GetMinDistance(PT_3D sch_pt, CCoordSys* pCS, PT_3D *pret)const;

	//在某个搜索范围内查找，当 s r是SearchNearest类型时，distance 返回距离，
	virtual BOOL FindObj(const SearchRange *sr, double *distance=NULL)const;
};

struct EXPORT_EDITBASE SearchableObj : public SearchableUnit
{
	SearchableObj();
	~SearchableObj();
    SearchableAgentObj *p;

	DECLARE_MEMPOOL(SearchableObj);
};

//获得可搜索对象的外包
Envelope EXPORT_EDITBASE GetEnvelopeOfSearchableUnit(const SearchableUnit *pu);

#define IsSearchablePoint(p)		((p)!=NULL&&(p)->type==EBSU_POINT)
#define IsSearchableLine(p)			((p)!=NULL&&(p)->type==EBSU_LINE)
#define IsSearchableObj(p)			((p)!=NULL&&(p)->type==EBSU_OBJECT)
#define IsSearchableGrBuf(p)		((p)!=NULL&&(p)->type==EBSU_GRBUF)

void ClearSearchableUnit(SearchableUnit **pus, int nUnit=1);

class EXPORT_EDITBASE CSearcherFilter
{
public:
	CSearcherFilter(){};
	virtual ~CSearcherFilter(){};
};

typedef BOOL (CSearcherFilter::*PFilterFunc)(LONG_PTR id);
//搜索器的基类定义
class EXPORT_EDITBASE CSearcher 
{
public:
	CSearcher(){
		m_pFilter = NULL;
		m_pFilterFunc = NULL;

		m_pFilter1 = NULL;
		m_pFilterFunc1 = NULL;
	};
	virtual ~CSearcher(){};

	virtual int GetType(){
		return EBSE_NONE;
	}
	
	// add/delete objects
	virtual void AddObj(LONG_PTR id, SearchableUnit *pu)=0;
	virtual void DelObj(LONG_PTR id) = 0;
	virtual void DelAll()=0;
	
	// search
	virtual int  FindObj(const SearchRange* sr)=0;
	virtual BOOL Intersect(LONG_PTR id, Envelope e, CCoordSys *pCS) = 0;
	
	// get results of search
	virtual const LONG_PTR* GetFoundIds(int& num)const=0;
	virtual const double* GetFoundDis(int& num)const=0;
	virtual int  GetObjSum()const=0;
	virtual void SetHitSurfaceInside(BOOL bEnable);
public:
	CSearcherFilter *m_pFilter;
	PFilterFunc m_pFilterFunc;

	CSearcherFilter *m_pFilter1;
	PFilterFunc m_pFilterFunc1;
};

MyNameSpaceEnd

#endif /* _INC_EDITBASE_SEARCHSTRUCT_INCLUDED */
