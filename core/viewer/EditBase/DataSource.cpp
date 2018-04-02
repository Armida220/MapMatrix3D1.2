// EBDataSource.cpp: implementation of the CDataSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "DataSource.h"
#include "SmartViewFunctions.h"
#include "CoordWnd.h"
#include "Symbol.h"
#include "GeoPoint.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "GeoText.h"
#include "GeoParallel.h "
#include "GeoDirPoint.h "

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataQuery::CDataQuery():m_ObjLinkMaps(256)
{
	m_ObjLinkMaps.InitHashTable(999983/*A prime number*/);
}

CDataQuery::~CDataQuery()
{
	ClearAllSearchableUnits();
}

BOOL CDataSource::ClearAll(BOOL delete_org_data)
{
	if (!delete_org_data)
	{
		m_pDataQuery->ClearAllSearchableUnits();
		//
		m_arrObjs.RemoveAll();
		//
		return TRUE;
	}
	//
	CFeature* pFt;

	DWORD iStartTime = GetTickCount();

	if (m_pDataQuery)
	{
		ObjLinkInfo info;
		INDEX idx = GetFirstIndex();

		while (!idx.IsNULL())
		{
			pFt = GetNextObjByIndex(&idx);

			m_pDataQuery->DelSearchableUnit(pFt, TRUE);
		}
	}
	DWORD iEndTime = GetTickCount();
	iEndTime -= iStartTime;

	for (int i = m_arrObjs.GetSize() - 1; i >= 0; i--)
	{
		pFt = (CFeature*)m_arrObjs.GetAt(i);
		delete pFt;
	}

	m_pDataQuery->ClearAllSearchableUnits();
	//
	m_arrObjs.RemoveAll();

	return TRUE;
}

void CDataQuery::AddSearcher(CSearcher *ps)
{
	if( ps==NULL )
		return;
	else
	{
		for( int i=m_arrPSearchers.GetSize()-1; i>=0; i--)
		{
			if( ps==m_arrPSearchers.GetAt(i) )
			{
				return;
			}
		}
		m_arrPSearchers.Add(ps);
	}
}

void CDataQuery::RemoveSearcher(CSearcher *ps)
{
	if( ps==NULL )
		m_arrPSearchers.RemoveAll();
	else
	{
		for( int i=m_arrPSearchers.GetSize()-1; i>=0; i--)
		{
			if( ps==m_arrPSearchers.GetAt(i) )
			{
				m_arrPSearchers.RemoveAt(i);
				return;
			}
		}
	}
}

void CDataQuery::SetActiveSearcher(CSearcher *ps)
{
	for( int i=m_arrPSearchers.GetSize()-1; i>=0; i--)
	{
		if( ps==m_arrPSearchers.GetAt(i) )
		{
			m_arrPSearchers.RemoveAt(i);
			m_arrPSearchers.InsertAt(0,ps);
			return;
		}
	}
}

CSearcher *CDataQuery::GetActiveSearcher()
{
	if( m_arrPSearchers.GetSize()<=0 )return NULL;
	return m_arrPSearchers.GetAt(0);
}

void CDataQuery::AddSearchableUnit(CPFeature pFt, SearchableUnit **pus, int nUnit)
{
	if( nUnit<=0 )return;

	//find pFt
	ObjLinkInfo info;
	if( m_ObjLinkMaps.Lookup(pFt,info) )
	{
		// merge existing Searchable Units
		if( info.pus!=NULL )
		{
			SearchableUnit **pNew = new SearchableUnit*[info.nUnit+nUnit];
			if( pNew==NULL )
				return;

			memcpy(pNew,info.pus,sizeof(SearchableUnit*)*info.nUnit);
			memcpy(pNew+info.nUnit,pus,sizeof(SearchableUnit*)*nUnit);

			delete[] info.pus;
			info.pus = pNew;
			info.nUnit += nUnit;
		}
		// new Searchable Units
		else
		{
			SearchableUnit **pNew = new SearchableUnit*[nUnit];
			if( pNew==NULL )
				return;
			
			memcpy(pNew,pus,sizeof(SearchableUnit*)*nUnit);
			
			info.pus = pNew;
			info.nUnit = nUnit;
		}

		m_ObjLinkMaps.SetAt(pFt,info);
	}
	// new Searchable Units
	else
	{
		SearchableUnit **pNew = new SearchableUnit*[nUnit];
		if( pNew==NULL )
			return;
		
		memcpy(pNew,pus,sizeof(SearchableUnit*)*nUnit);
		
		info.pus = pNew;
		info.nUnit = nUnit;
		m_ObjLinkMaps.SetAt(pFt,info);
	}

	// add Searchable Units to searchers
	int size = m_arrPSearchers.GetSize();
	for( int i=0; i<size; i++)
	{
		for( int j=0; j<nUnit; j++)
		{
			m_arrPSearchers.GetAt(i)->AddObj((LONG_PTR)pFt,pus[j]);
		}
	}
}


void CDataQuery::DelSearchableUnit(CPFeature pFt, BOOL bDelInfo)
{
	// remove Searchable Units from searchers
	int size = m_arrPSearchers.GetSize();
	for( int i=0; i<size; i++)
	{
		m_arrPSearchers.GetAt(i)->DelObj((LONG_PTR)pFt);
	}

	// clear Searchable Units
	ObjLinkInfo info;
	if( m_ObjLinkMaps.Lookup(pFt,info) )
	{
		ClearSearchableUnit(info.pus,info.nUnit);
		if( info.pus )delete[] info.pus;
		
		if( bDelInfo )
		{
			m_ObjLinkMaps.RemoveKey(pFt);
		}
		else
		{
			info.nUnit = 0;
			info.pus = NULL;
			m_ObjLinkMaps.SetAt(pFt,info);
		}
	}
}


void CDataQuery::ClearAllSearchableUnits()
{
	// remove Searchable Units from searchers
	int size = m_arrPSearchers.GetSize();
	for( int i=0; i<size; i++)
	{
		m_arrPSearchers.GetAt(i)->DelAll();
	}

	// clear Searchable Units
	CPFeature pFt;
	ObjLinkInfo info;
	POSITION pos = m_ObjLinkMaps.GetStartPosition();
	while( pos!=NULL )
	{
		m_ObjLinkMaps.GetNextAssoc(pos,pFt,info);

		ClearSearchableUnit(info.pus,info.nUnit);
		if( info.pus )delete[] info.pus;
	}

	m_ObjLinkMaps.RemoveAll();
}


POSITION CDataQuery::GetFirstObjPos()const
{
	return m_ObjLinkMaps.GetStartPosition();
}

CFeature* CDataQuery::GetNextObjByPos(POSITION &pos)const
{
	if( pos==NULL )return NULL;

	CPFeature pFt;
	ObjLinkInfo info;
	m_ObjLinkMaps.GetNextAssoc(pos,pFt,info);

	if( pFt!=NULL && pFt->IsDeleted() )
		return NULL;

	return pFt;
}


int CDataQuery::GetObjectCount()const
{
	return m_ObjLinkMaps.GetCount();
}


BOOL CDataQuery::IsManagedObj(CPFeature pObj)const
{
	ObjLinkInfo info;
	if( m_ObjLinkMaps.Lookup(pObj,info) )
	{
		return TRUE;
	}

	return FALSE;
}


int CDataQuery::FindObject(const SearchRange *sr)
{
	int nfind = 0;
	CSearcher *psearch = GetActiveSearcher();

	psearch->FindObj(sr);
	
	const LONG_PTR *ids = psearch->GetFoundIds(nfind);
	if (nfind==0)
	{		
		m_arrFoundHandles.SetSize(0);
		m_arrFoundObjDis.SetSize(0);
	}
	if( nfind>0 )
	{
		m_arrFoundHandles.SetSize(nfind);
		memcpy(m_arrFoundHandles.GetData(),ids,nfind*sizeof(__int64));

		int nfind2 = 0;
		const double *dis_buf = psearch->GetFoundDis(nfind2);
		if(nfind2>0)
		{
			if(nfind2!=nfind)
			{
				int a = 1;
			}
			m_arrFoundObjDis.SetSize(nfind);
			memcpy(m_arrFoundObjDis.GetData(),dis_buf,nfind*sizeof(double));
		}
	}

	return nfind;
}


BOOL CDataQuery::Intersect(CPFeature pFtr, Envelope e,CCoordSys *pCS)
{
	CSearcher *psearch = GetActiveSearcher();

	return psearch->Intersect((LONG_PTR)pFtr, e, pCS);
}

void CDataQuery::RefillObjects(CSearcher* ps)
{
	if( ps )
	{
		ps->DelAll();
	}
	else
	{
		int size = m_arrPSearchers.GetSize();
		for( int j=0; j<size; j++)
		{
			m_arrPSearchers.GetAt(j)->DelAll();
		}
	}


	// clear Searchable Units
	CPFeature pFt;
	ObjLinkInfo info;
	POSITION pos = m_ObjLinkMaps.GetStartPosition();
	while( pos!=NULL )
	{
		m_ObjLinkMaps.GetNextAssoc(pos,pFt,info);

		if( ps==NULL )
		{
			int size = m_arrPSearchers.GetSize();
			for(int j=0; j<size; j++)
			{
				for( int i=0; i<info.nUnit; i++)
				{
					m_arrPSearchers.GetAt(j)->AddObj((LONG_PTR)pFt, info.pus[i]);
				}
			}
		}
		else
		{
			for( int i=0; i<info.nUnit; i++)
			{
				ps->AddObj((LONG_PTR)pFt,info.pus[i]);
			}
		}
	}
}


BOOL CDataQuery::IsObjInFoundObjs(CPFeature pFt)const
{
	int num = m_arrFoundHandles.GetSize();
	const CPFeature *buf = m_arrFoundHandles.GetData();
	for( int i=0; i<num; i++)
	{
		if( buf[i]==pFt )return TRUE;
	}
	return FALSE;
}

int CDataQuery::FindObjectInRect(Envelope sch_evlp, CCoordSys *pCS, BOOL bEntirelyInc, BOOL forSymbolized)
{
	SearchRect sr(sch_evlp,pCS,bEntirelyInc,forSymbolized);
	return FindObject(&sr);
}

int CDataQuery::FindObjectInPolygon(PT_3D *sch_pts, int num, CCoordSys *pCS, BOOL forSymbolized)
{
	SearchPolygon sr(sch_pts,num,pCS,forSymbolized);
	return FindObject(&sr);
}

CPFeature CDataQuery::FindNearestObject(PT_3D sch_pt, double r, CCoordSys *pCS, BOOL forSymbolized)
{
	SearchNearest sr(sch_pt,r,pCS,forSymbolized);
	if( FindObject(&sr)>=1 )
	{
		return m_arrFoundHandles.GetAt(0);
	}

	return NULL;
}

CPFeature CDataQuery::FindPtNearestObject(PT_3D sch_pt, CCoordSys *pCS, BOOL forSymbolized)
{
	SearchNearestObj sr(sch_pt,pCS,forSymbolized);
	if( FindObject(&sr)>=1 )
	{
		return m_arrFoundHandles.GetAt(0);
	}
	
	return NULL;
}

int CDataQuery::FindIntersectObject(PT_3D *sch_pts, int num, Envelope e, CCoordSys *pCS, BOOL forSymbolized)
{
	SearchIntersectObj sr(sch_pts,num,e,pCS,forSymbolized);
	return FindObject(&sr);
}


const CPFeature *CDataQuery::GetFoundHandles(int &num)const
{
	num = m_arrFoundHandles.GetSize();
	return m_arrFoundHandles.GetData();
}

const double *CDataQuery::GetFoundDis(int &num)const
{
	num = m_arrFoundObjDis.GetSize();
	return m_arrFoundObjDis.GetData();
}


BOOL CDataQuery::SetObjectInfo(CPFeature pFt, int idx, LONG_PTR info)
{
	//find pFt
	ObjLinkInfo link;

	if( idx>=0 && idx<sizeof(link.hParent)/sizeof(link.hParent[0]) )
	{
		if( m_ObjLinkMaps.Lookup(pFt,link) )
		{
			link.hParent[idx] = info;

			m_ObjLinkMaps.SetAt(pFt,link);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CDataQuery::GetObjectInfo(CPFeature pFt, int idx, LONG_PTR &info)const
{
	//find pFt
	ObjLinkInfo link;
	
	if( idx>=0 && idx<sizeof(link.hParent)/sizeof(link.hParent[0]) )
	{
		if( m_ObjLinkMaps.Lookup(pFt,link) )
		{
			info = link.hParent[idx];
			return TRUE;
		}
	}
	
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDataSource::CDataSource(CDataQuery *pDQ)
{
	ASSERT(pDQ);
	m_pDataQuery = pDQ;
	m_bSurfaceInsideSelect = TRUE;
}

CDataSource::~CDataSource()
{
	CFeature* pFt;

	DWORD iStartTime = GetTickCount();	

	if(m_pDataQuery)
	{
		ObjLinkInfo info;
		INDEX idx = GetFirstIndex();
		
		while( !idx.IsNULL() )
		{
			pFt = GetNextObjByIndex(&idx);
			
			m_pDataQuery->DelSearchableUnit(pFt,TRUE);
		}
	}
	DWORD iEndTime = GetTickCount();
	iEndTime -= iStartTime;
	
	for( int i=m_arrObjs.GetSize()-1; i>=0; i--)
	{
		pFt = (CFeature*)m_arrObjs.GetAt(i);
		delete pFt;
	}
}


INDEX CDataSource::GetFirstIndex()const
{
	if( m_arrObjs.GetSize()>0 )return INDEX(0);
	return INDEX();
}

CFeature* CDataSource::GetNextObjByIndex(INDEX *idx)const
{
	CFeature* pFt = NULL;
	while( pFt==NULL )
	{
		pFt = GetObjByIndex(idx);
		if( pFt!=NULL&&pFt->IsDeleted() )
			pFt = NULL;	

		INDEX_0(idx) = INDEX_0(idx)+1;
		if( INDEX_0(idx)>=m_arrObjs.GetSize() )
		{
			*idx = INDEX();
			break;
		}
	}
	return pFt;
}

CFeature* CDataSource::GetObjByIndex(const INDEX *idx)const
{
	if( INDEX_0(idx)>=0 && INDEX_0(idx)<m_arrObjs.GetSize() )
	{
		CFeature* pFt = ((CFeature*)m_arrObjs.GetAt(INDEX_0(idx)));
		if( pFt->IsDeleted() )pFt = NULL;
		return pFt;
	}
	return NULL;
}


void CDataSource::CreateSearchableUnit(CFeature *pObj, CPtrArray& arrPUnits)
{
#ifdef TRIAL_VERSION
	return;
#else

	CGeometry *pGeo = pObj->GetGeometry();
	if( !pGeo )return;

	if ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDirPoint)))
	{
		SearchableGrBuf *p = new SearchableGrBuf;
		if (!p) return;
		pGeo->Draw(p->CreateBuf());
		p->RefreshEnvelope();
		arrPUnits.Add(p);
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		SearchablePoint *p = new SearchablePoint;
		if (!p) return;
		p->p = pGeo;

		arrPUnits.Add(p);
	}
	else if ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)))
	{
		SearchableGrBuf *p = new SearchableGrBuf;
		if (!p) return;
		pGeo->Draw(p->CreateBuf());
		p->RefreshEnvelope();
		arrPUnits.Add(p);
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		CGeoCurve *pRealObj = (CGeoCurve*)pGeo;
		const CShapeLine *pShape = pRealObj->GetShape();

		CPtrArray arr;
		pShape->GetShapeLineUnit(arr);
		int size = arr.GetSize();

		for( int i=0; i<size; i++)
		{
			SearchableLine *p = new SearchableLine;
			if (!p) return;
			p->p = arr.GetAt(i);

			arrPUnits.Add(p);
		}
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CGeoSurface *pRealObj = (CGeoSurface*)pGeo;
		if( (pRealObj->IsFillColor() || m_bSurfaceInsideSelect) )
		{
			SearchableGrBuf *p = new SearchableGrBuf;
			if (!p) return;
			pRealObj->DrawForSearch(p->CreateBuf(),TRUE);
			p->RefreshEnvelope();
			arrPUnits.Add(p);		
		}
		else
		{
			const CShapeLine *pShape = pRealObj->GetShape();
			
			CPtrArray arr;
			pShape->GetShapeLineUnit(arr);
			int size = arr.GetSize();
			
			for( int i=0; i<size; i++)
			{
				SearchableLine *p = new SearchableLine;
				if (!p) return;
				p->p = arr.GetAt(i);
				
				arrPUnits.Add(p);
			}			
		}
	}	
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		SearchableGrBuf *p = new SearchableGrBuf;
		if (!p) return;
 		((CGeoText*)pGeo)->CreateSearchReg(p->CreateBuf());
		p->RefreshEnvelope();
		arrPUnits.Add(p);
// 		CGeoText *pRealObj = (CGeoText*)pGeo;
// 		const CShapeLine *pShape = pRealObj->GetShape();
// 		
// 		CPtrArray arr;
// 		pShape->GetShapeLineUnit(arr);
// 		int size = arr.GetSize();
// 		
// 		for( int i=0; i<size; i++)
// 		{
// 			SearchableLine *p = new SearchableLine;
// 			if (!p) return;
// 			p->p = arr.GetAt(i);
// 			
// 			arrPUnits.Add(p);
// 		}
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoDemPoint)))
	{
		CGeoDemPoint *pRealObj = (CGeoDemPoint*)pGeo;
		SearchableObj *pS = new SearchableObj;
		if(!pS) return;
		SearchableMultiPoint *p = new SearchableMultiPoint;
		if (!p) 
		{
			delete pS;
			return;
		}
		p->m_pObj = pRealObj;
		
		pS->p = p;
		
		arrPUnits.Add(((SearchableUnit*)pS));		
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)))
	{
		CGeoMultiPoint *pRealObj = (CGeoMultiPoint*)pGeo;
		SearchableObj *pS = new SearchableObj;
		if(!pS) return;
		SearchableMultiPoint *p = new SearchableMultiPoint;
		if (!p) 
		{
			delete pS;
			return;
		}
		p->m_pObj = pRealObj;
		
		pS->p = p;
		
		arrPUnits.Add(((SearchableUnit*)pS));		
	}
	else
	{
		SearchableGrBuf *p = new SearchableGrBuf;
		if (!p) return;
		pGeo->Draw(p->CreateBuf());
		p->RefreshEnvelope();
		arrPUnits.Add(p);
	}
#endif
}


BOOL CDataSource::AddObject(CFeature *pFt)
{
	pFt->SetToDeleted(FALSE);

	m_arrObjs.Add(pFt);
	
	CGeometry *pGeo = pFt->GetGeometry();
	if(pGeo && m_pDataQuery)
	{
		CPtrArray arrPUnits;
		CreateSearchableUnit(pFt, arrPUnits);
		m_pDataQuery->AddSearchableUnit(pFt,(SearchableUnit**)arrPUnits.GetData(),arrPUnits.GetSize());

		m_pDataQuery->SetObjectInfo(pFt, 0, (LONG_PTR)this);
	}
	
	return TRUE;
}

BOOL CDataSource::DeleteObject(CPFeature pFt)
{
	if(m_pDataQuery)
	{
		LONG_PTR info;
		if(	!m_pDataQuery->GetObjectInfo(pFt,0,info) )
			return FALSE;
		
		if (info != (LONG_PTR)this)
			return FALSE;
		
		m_pDataQuery->DelSearchableUnit(pFt,FALSE);
	}

	pFt->SetToDeleted(TRUE);
	
	return TRUE;
}

BOOL CDataSource::RestoreObject(CPFeature pFt)
{
	if(m_pDataQuery)
	{
		LONG_PTR info;
		if(	!m_pDataQuery->GetObjectInfo(pFt,0,info) )
			return FALSE;
		
		if (info != (LONG_PTR)this)
			return FALSE;
		
		CGeometry *pGeo = pFt->GetGeometry();
		if( pGeo )
		{
			CPtrArray arrPUnits;
			CreateSearchableUnit(pFt, arrPUnits);
			m_pDataQuery->AddSearchableUnit(pFt,(SearchableUnit**)arrPUnits.GetData(),arrPUnits.GetSize());
		}
	}


	pFt->SetToDeleted(FALSE);
	
	return TRUE;
}


BOOL CDataSource::ClearObject(CPFeature pFt)
{
	LONG_PTR info;
	if(m_pDataQuery)
	{
		if(	!m_pDataQuery->GetObjectInfo(pFt,0,info) )
			return FALSE;
		
		if (info != (LONG_PTR)this)
			return FALSE;
		
		m_pDataQuery->DelSearchableUnit(pFt,TRUE);

	}

	int size = m_arrObjs.GetSize();
	for( int i=0; i<size; i++)
	{
		if( m_arrObjs.GetAt(i)==pFt )
		{
			delete pFt;
			m_arrObjs.RemoveAt(i);
			break;
		}
	}
	
	return TRUE;
}


void CDataSource::GetSymbol(CFeature *pObj, CPtrArray& arr)
{
	return;
}

BOOL CDataSource::IsSurfaceInsideSelect()
{
	return m_bSurfaceInsideSelect;
}

MyNameSpaceEnd