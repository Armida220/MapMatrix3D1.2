// EBDataSource.h: interface for the CDataSource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBDATASOURCE_H__7B8126E0_50A4_42F0_A346_8D17D76A42FA__INCLUDED_)
#define AFX_EBDATASOURCE_H__7B8126E0_50A4_42F0_A346_8D17D76A42FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"
#include "Envelope.h"
#include "Tree8Search.h"

MyNameSpaceBegin

class CDataSource;
class CSymbol;


//数据的查询类
//它与 CDataSource 相对应，如果使用者派生了新的 CDataSource，可以根据需要也派生 CDataQuery
class EXPORT_EDITBASE CDataQuery : public CSearcherFilter
{
	friend class CDataSource;
public:
	// used to save some infos of objects for getting faster its GrBuffer or parents.
	// use it only internally.
	struct _ObjLinkInfo
	{
		_ObjLinkInfo(){
			hParent[0] = hParent[1] = hParent[2] = hParent[3] = -1;
		}
		// handles of parents of object
		LONG_PTR hParent[4];

		// 记录对象的搜索对象
		SearchableUnit **pus;
		int nUnit;
	};
public:	
	
	CDataQuery();
	virtual ~CDataQuery();

	//manage searchers
	virtual void AddSearcher(CSearcher *ps);
	virtual void RemoveSearcher(CSearcher *ps=NULL);
	virtual void SetActiveSearcher(CSearcher *ps);
	virtual void RefillObjects(CSearcher* ps);

	// Check
	virtual BOOL IsManagedObj(CPFeature pObj)const;
	
	// Iterator
	virtual POSITION GetFirstObjPos()const;	
	virtual CFeature* GetNextObjByPos(POSITION &pos)const;
	virtual int GetObjectCount()const;
	
	// Search objects by searchers
	virtual int FindObject(const SearchRange *sr);
	CPFeature FindNearestObject(PT_3D sch_pt, double r, CCoordSys *pCS, BOOL forSymbolized=TRUE);
	virtual BOOL Intersect(CPFeature pFtr, Envelope e,CCoordSys *pCS);
	
	// sch_evlp为客户坐标
	int FindObjectInRect(Envelope sch_evlp, CCoordSys *pCS, BOOL bEntirelyInc=FALSE, BOOL forSymbolized=TRUE);
	CPFeature FindPtNearestObject(PT_3D sch_pt, CCoordSys *pCS, BOOL forSymbolized=TRUE);
	int FindIntersectObject(PT_3D *sch_pts, int num, Envelope e, CCoordSys *pCS, BOOL forSymbolized=TRUE);
	int FindObjectInPolygon(PT_3D *sch_pts, int num, CCoordSys *pCS, BOOL forSymbolized=TRUE);

	virtual BOOL IsObjInFoundObjs(CPFeature pObj)const;
	virtual const CPFeature *GetFoundHandles(int &num)const;
	virtual const double *GetFoundDis(int &num)const;

public:	
	// add searchable objects to searchers
	virtual void AddSearchableUnit(CPFeature pFt, SearchableUnit **pus, int nUnit);
	virtual void DelSearchableUnit(CPFeature pFt, BOOL bDelInfo);
	virtual void ClearAllSearchableUnits();

	virtual BOOL SetObjectInfo(CPFeature pFt, int idx, LONG_PTR info);
	virtual BOOL GetObjectInfo(CPFeature pFt, int idx, LONG_PTR &info)const;

	//get searchers
	virtual CSearcher *GetActiveSearcher();
	
public:	
	CMap<CPFeature,CPFeature,_ObjLinkInfo,_ObjLinkInfo> m_ObjLinkMaps;

	CArray<CSearcher*,CSearcher*> m_arrPSearchers;
	
	CArray<CPFeature,CPFeature> m_arrFoundHandles;
	CArray<double,double> m_arrFoundObjDis;
};

typedef CDataQuery::_ObjLinkInfo ObjLinkInfo; 

//数据的管理(增加/删除等等)
class EXPORT_EDITBASE CDataSource
{
public:
	// Construction/Destruction
	CDataSource(CDataQuery* pDQ);
	virtual ~CDataSource();
	virtual BOOL ClearAll(BOOL delete_org_data = false);
	
	// Iterator implement
	// 这几个接口区别于 CDataQuery 的 GetFirstPos 系列接口在于：
	// 这几个接口是遍历本数据源的数据，而 CDataQuery 可能跨多个数据源
	virtual CFeature * GetObjByIndex(const INDEX *idx)const;
	virtual INDEX GetFirstIndex()const;	
	virtual CFeature* GetNextObjByIndex(INDEX *idx)const;

	// Manage objects
	virtual BOOL RestoreObject(CPFeature pFt);
	virtual BOOL DeleteObject(CPFeature pFt);
	virtual BOOL AddObject(CFeature *pFt);
	virtual BOOL ClearObject(CPFeature pFt);

	virtual void GetSymbol(CFeature *pObj, CPtrArray& arr);
	BOOL IsSurfaceInsideSelect();
protected:
	virtual void CreateSearchableUnit(CFeature *pObj, CPtrArray& arrPUnits);

protected:
	CFtrArray m_arrObjs;
	CDataQuery *m_pDataQuery;

	//面内部是否可以选中面
	BOOL m_bSurfaceInsideSelect;
};

MyNameSpaceEnd

#endif // !defined(AFX_EBDATASOURCE_H__7B8126E0_50A4_42F0_A346_8D17D76A42FA__INCLUDED_)
