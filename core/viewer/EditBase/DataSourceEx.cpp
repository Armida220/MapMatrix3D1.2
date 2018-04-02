// DlgDataSource.cpp: implementation of the CDataSourceEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Access.h"
#include "DataSourceEx.h"
#include "Symbol.h"
#include "Scheme.h "
#include "SymbolLib.h "
#include "Feature.h"
#include "SQLiteAccess.h "
#include "SmartViewFunctions.h"
#include "regdef.h"
#include "regdef2.h"
#include "GeoText.h"
#include "FBHelperFunc.h"
#include "SymbolLib.h"
#include "GeoParallel.h"
#include "GeoCurve.h"
#include "GeoSurface.h"
#include "GeoDirPoint.h"
#include "UVSCashe.h"
#include <MAP>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define M_PDQ		((CDataQueryEx*)m_pDataQuery)

MyNameSpaceBegin

void CreateCoverPolygon(int nCoverType, Envelope e, PT_3D pt, GrBuffer *pBuf);

CDataQueryEx::CDataQueryEx()
{
	m_pFilterDataSource = NULL;
	m_bIncludeLocked = FALSE;
	m_bIncludeInvisible = FALSE;
	m_bIncludeRefData = FALSE;

	m_bUseSymQuery = FALSE;
	m_pBaseQuery = NULL;
	m_pSymQuery = NULL;

	m_bSymFixed = FALSE;

	InitQuery();
}

CDataQueryEx::~CDataQueryEx()
{
	m_searchBase.DelAll();
	m_searchSym.DelAll();
	
	if( m_pBaseQuery )
	{
		delete m_pBaseQuery;
		m_pBaseQuery = NULL;
	}
	
	if( m_pSymQuery )
	{
		delete m_pSymQuery;
		m_pSymQuery = NULL;
	}
}

void CDataQueryEx::InitQuery()
{
	m_pBaseQuery = new CDataQuery();
	if( m_pBaseQuery )
	{
		m_pBaseQuery->AddSearcher(&m_searchBase);
		m_searchBase.m_pFilter = this;
		m_searchBase.m_pFilterFunc = (PFilterFunc)(&CDataQueryEx::FilterFeature);
	}
	
	m_pSymQuery = new CDataQuery();
	
	if( m_pSymQuery )
	{
		m_pSymQuery->AddSearcher(&m_searchSym);
		m_searchSym.m_pFilter = this;
		m_searchSym.m_pFilterFunc = (PFilterFunc)(&CDataQueryEx::FilterFeature);
	}
	
	m_bUseSymQuery = FALSE;
}


BOOL CDataQueryEx::SetFilterIncludeLocked(BOOL bIncluded)
{
	BOOL bOld = m_bIncludeLocked;
	m_bIncludeLocked = bIncluded;
	return bOld;
}

BOOL CDataQueryEx::SetFilterIncludeInvisible(BOOL bIncluded)
{
	BOOL bOld = m_bIncludeInvisible;
	m_bIncludeInvisible = bIncluded;
	return bOld;
}

BOOL CDataQueryEx::SetFilterIncludeRefData(BOOL bIncluded)
{
	BOOL bOld = m_bIncludeRefData;
	m_bIncludeRefData = bIncluded;
	return bOld;
}



BOOL CDataQueryEx::FilterFeature(LONG_PTR id)
{
	CFeature *pFtr = (CFeature*)id;
	if(m_pFilterDataSource) 
	{
		CFtrLayer* pLayer = m_pFilterDataSource->GetFtrLayerOfObject(pFtr);

		// 在当前数据源中找不到地物所在的层就认为是在其它数据源中
		if (!pLayer)
		{
			if(!m_bIncludeRefData)
				return FALSE;
			else 
				return TRUE;
		}

		if (!m_bIncludeInvisible && (!pLayer->IsVisible()||!pFtr->IsVisible()))
			return FALSE;

		if (!m_bIncludeLocked && pLayer->IsLocked() ) return FALSE;

		if( !pLayer->IsAllowQuery() )
			return FALSE;
	}
	return TRUE;
}

int CDataQueryEx::FindNearestObjectByObjNum(PT_3D sch_pt, int nObjNum, CCoordSys *pCS, BOOL forSymbolized, double* pFindRadius, double *pLimitRadius)
{
	CTree8Search *pSearcher = (CTree8Search*)GetActiveSearcher();
	const Tree8Node *Node = pSearcher->FindLeafNodeByPt(sch_pt,pCS);
	int nObj = Node->objs.GetSize(), way = 0;

	double fGScale = (pCS==NULL?1:pCS->CalcScale());

	Envelope e0 = pSearcher->GetRegion();
	Envelope e1 = m_pFilterDataSource->GetBound();
	double r, r_limit;
	static double k_buf[2] = {1.0, 1.0};
	if( nObj<=0 ||
		fabs(Node->e0.Width())>1e+11 ||
		fabs(Node->e0.Height())>1e+11 )
	{
		nObj =  m_pFilterDataSource->GetEditableObjCount();
		if( nObj<=0 )
			return 0;

		double fWidth = e1.Width() * fGScale;

		r = sqrt((double)nObjNum/nObj)*(fWidth/2)*k_buf[0];

		way = 0;
	}
	else
	{
		r = sqrt((double)nObjNum/nObj)*(Node->e0.Width()/2)*k_buf[1];
		way = 1;
	}

	if (r <= 1e-6)
	{
		return 0;
	}

	if( pLimitRadius )
	{
		r_limit = *pLimitRadius;
		if( r_limit>e1.Width() )
			r_limit = e1.Width();
	}
	else
	{
		r_limit = e1.Width();
	}
	
	SearchNearest sr(sch_pt,r,pCS,forSymbolized);	
	int nFind = 0;
	while (  (nFind=FindObject(&sr))<nObjNum  )
	{
		if(sr.r>(r_limit*fGScale))
			break;
		sr.r *=2;
	}

	if( nFind<=0 )
		return 0;

	//动态调整系数
	double k = (sr.r/r)/(nFind/(float)nObjNum);

	k_buf[way] = sqrt(k);

	/*
	CString strMsg;
	strMsg.Format("FindNearestObjectByObjNum %lf %lf %lf %lf\n",k_buf[0],k_buf[1],k,r);
	OutputDebugString(strMsg);
	*/

	if( pFindRadius )*pFindRadius = sr.r / fGScale;

	return m_arrFoundHandles.GetSize();
	
}

CDataQueryEx::FilterFeatureItem CDataQueryEx::SetFilterFeature(FilterFeatureItem item)
{
	FilterFeatureItem oldItem;
	if(m_pBaseQuery)
	{
		oldItem.m_pFilterFunc = m_pBaseQuery->GetActiveSearcher()->m_pFilterFunc1;
		oldItem.m_pFilter = m_pBaseQuery->GetActiveSearcher()->m_pFilter1;
		m_pBaseQuery->GetActiveSearcher()->m_pFilterFunc1 = item.m_pFilterFunc;
		m_pBaseQuery->GetActiveSearcher()->m_pFilter1 = item.m_pFilter;
	}
	if(m_pSymQuery)
	{
		m_pSymQuery->GetActiveSearcher()->m_pFilterFunc1 = item.m_pFilterFunc;
		m_pSymQuery->GetActiveSearcher()->m_pFilter1 = item.m_pFilter;
	}
	return oldItem;
}

void CDataQueryEx::AddSearcher(CSearcher *ps)
{
	m_pBaseQuery->AddSearcher(ps);
	m_pSymQuery->AddSearcher(ps);
}


void CDataQueryEx::RemoveSearcher(CSearcher *ps)
{
	m_pBaseQuery->RemoveSearcher(ps);
	m_pSymQuery->RemoveSearcher(ps);
}


void CDataQueryEx::SetActiveSearcher(CSearcher *ps)
{
	m_pBaseQuery->SetActiveSearcher(ps);
	m_pSymQuery->SetActiveSearcher(ps);
}


void CDataQueryEx::RefillObjects(CSearcher* ps)
{
	m_pBaseQuery->RefillObjects(ps);
	m_pSymQuery->RefillObjects(ps);
}


// Check
BOOL CDataQueryEx::IsManagedObj(CPFeature pObj)const
{
	return m_pBaseQuery->IsManagedObj(pObj);
}


// Iterator
POSITION CDataQueryEx::GetFirstObjPos()const
{
	return m_pBaseQuery->GetFirstObjPos();
}


CFeature* CDataQueryEx::GetNextObjByPos(POSITION &pos)const
{
	return m_pBaseQuery->GetNextObjByPos(pos);
}


int CDataQueryEx::GetObjectCount()const
{
	return m_pBaseQuery->GetObjectCount();
}

#define EBSR_NONE					0
#define EBSR_RECT					1
#define EBSR_RADIUS					2
#define EBSR_NEARESTOBJ				3
#define EBSR_INTERSECTOBJ			4

// Search objects by searchers
int CDataQueryEx::FindObject(const SearchRange *sr)
{
	int nRet = 0;

// 	if( sr->type==EBSR_RECT )
// 	{
// 		nRet = m_pBaseQuery->FindObject(sr);
// 		m_arrFoundHandles.Copy(m_pBaseQuery->m_arrFoundHandles);
// 
// 		if( nRet>0 )return nRet;
// 
// 		if( m_bUseSymQuery )
// 		{
// 			nRet = m_pSymQuery->FindObject(sr);
// 			m_arrFoundHandles.Copy(m_pSymQuery->m_arrFoundHandles);
// 		}
// 	}
// 	else
	{
		nRet = m_pBaseQuery->FindObject(sr);
		m_arrFoundHandles.Copy(m_pBaseQuery->m_arrFoundHandles);
		m_arrFoundObjDis.Copy(m_pBaseQuery->m_arrFoundObjDis);

		if( nRet>0 )return nRet;

		if( m_bUseSymQuery && sr->forSymbolized )
		{
			nRet = m_pSymQuery->FindObject(sr);
			m_arrFoundHandles.Copy(m_pSymQuery->m_arrFoundHandles);
			m_arrFoundObjDis.Copy(m_pSymQuery->m_arrFoundObjDis);

		}
	}

	if( m_bUseSymQuery && m_bSymFixed && sr->forSymbolized )
	{
		//不能循环查找
		m_bSymFixed = FALSE;
		if( sr->type==EBSR_RADIUS )
		{
			SearchNearest sr2 = *(SearchNearest*)sr;
			if( m_arrFoundHandles.GetSize()<=0 )
			{
				Envelope evlp;
				evlp.CreateFromPtAndRadius(sr2.pt,sr2.r);
				
				evlp.TransformCltToGrd(sr2.pCS);

				CSetMaxNumResultsOfSearch maxSearch(this,20);

				double fLimitRadius = sr2.pCS==NULL?1:sr2.pCS->CalcScale();
				fLimitRadius = 20 / fLimitRadius;

				if( FindNearestObjectByObjNum(sr2.pt,5,sr2.pCS,TRUE,NULL,&fLimitRadius)>0 )
				{
					CArray<CPFeature,CPFeature> arrHandles;
					CArray<double,double> arrDis;

					for( int i=0; i<m_arrFoundHandles.GetSize(); i++)
					{
						CPFeature pFtr = m_arrFoundHandles[i];
						
						LONG_PTR info = 0;
						GetObjectInfo(pFtr,0,info);
						
						CDataSourceEx *pDS = (CDataSourceEx*)info;
						if( pDS )
						{
							GrBuffer buf;
							pDS->DrawFeature(pFtr,&buf,TRUE,0);
							
							if( GrBufferIsIntersect(evlp,&buf,FALSE) )
							{
								arrHandles.Add(pFtr);
								arrDis.Add(m_arrFoundObjDis[i]);
							}
						}
					}

					m_arrFoundHandles.Copy(arrHandles);
					m_arrFoundObjDis.Copy(arrDis);
					nRet = m_arrFoundHandles.GetSize();

				}
				if( nRet<=0 )
				{
					m_arrFoundHandles.RemoveAll();
					m_arrFoundObjDis.RemoveAll();
				}
			}
		}
		m_bSymFixed = TRUE;
	}

	return nRet;
}

BOOL CDataQueryEx::SetSymFixed(BOOL bEnable)
{
	BOOL bSymFixed = m_bSymFixed;
	m_bSymFixed = bEnable;

	return bSymFixed;
}

BOOL CDataQueryEx::IsExceedMaxNumOfResult()
{
	CSearcher *ps = GetActiveSearcher();
	if( ps->GetType()==EBSE_TREE8 )
	{
		return ((CTree8Search*)ps)->IsExceedMaxNumOfResult();
	}

	return FALSE;
}


int CDataQueryEx::FindObjectInRect_SimpleAndSym(Envelope sch_evlp, CCoordSys *pCS, BOOL bEntirelyInc, BOOL forSymbolized)
{
	SearchRectSimple sr(sch_evlp,pCS,bEntirelyInc,forSymbolized);

	int nRet1 = m_pBaseQuery->FindObject(&sr);
	m_arrFoundHandles.Copy(m_pBaseQuery->m_arrFoundHandles);

	if( !m_bUseSymQuery || !forSymbolized )
		return nRet1;	
	
	int nRet2 = m_pSymQuery->FindObject(&sr);
	m_arrFoundHandles.Append(m_pSymQuery->m_arrFoundHandles);

	if( nRet1==0 || nRet2==0 )
		return (nRet1+nRet2);

	CArray<CPFeature,CPFeature> arrHandles;
	arrHandles.SetSize(0,1024);

	//排序合并
	int size = nRet1+nRet2;

	CPFeature *ftrs0 = m_arrFoundHandles.GetData();
	CPFeature *ftrs1 = arrHandles.GetData();
	for( int i=0; i<size; i++)
	{
		//二分法插入
		CPFeature ftr = ftrs0[i];

		int findidx = -1, bnew = 0;
		if( i<=0 ){ findidx = 0; bnew = 1; }
		else
		{
			int i0 = 0, i1 = arrHandles.GetSize()-1, i2;
			while(findidx==-1) 
			{
				if( ftr<=ftrs1[i0] )
				{
					bnew = (ftr==ftrs1[i0]?0:1);
					findidx = i0;
					break;
				}
				else if( ftr>=ftrs1[i1] )
				{
					bnew = (ftr==ftrs1[i1]?0:1);
					findidx = (ftr==ftrs1[i1]?i1:(i1+1));
					break;
				}
				
				i2 = (i0+i1)/2;
				if( i2==i0 )
				{
					bnew = (ftr==ftrs1[i0]?0:1);
					findidx = (ftr==ftrs1[i0]?i0:(i0+1));
					break;
				}
				
				if( ftr<ftrs1[i2] )
					i1 = i2;
				else if( ftr>ftrs1[i2] )
					i0 = i2;
				else
				{
					findidx = i2; bnew = 0;
					break;
				}
			}
		}

		if( bnew )
		{
			arrHandles.InsertAt(findidx,ftr);
			ftrs1 = arrHandles.GetData();
		}
	}

	m_arrFoundHandles.Copy(arrHandles);

	return m_arrFoundHandles.GetSize();
}

BOOL CDataQueryEx::IsObjInFoundObjs(CPFeature pObj)const
{
	return CDataQuery::IsObjInFoundObjs(pObj);
}


const CPFeature *CDataQueryEx::GetFoundHandles(int &num)const
{
	return CDataQuery::GetFoundHandles(num);
}


// add searchable objects to searchers
void CDataQueryEx::AddSearchableUnit(CPFeature pFt, SearchableUnit **pus, int nUnit)
{
	m_pBaseQuery->AddSearchableUnit(pFt,pus,nUnit);
}


void CDataQueryEx::AddSearchableUnit_Sym(CPFeature pFt, SearchableUnit **pus, int nUnit)
{
	m_pSymQuery->AddSearchableUnit(pFt,pus,nUnit);
}

void CDataQueryEx::DelSearchableUnit(CPFeature pFt, BOOL bDelInfo)
{
	m_pBaseQuery->DelSearchableUnit(pFt,bDelInfo);
	m_pSymQuery->DelSearchableUnit(pFt,bDelInfo);
}


void CDataQueryEx::ClearAllSearchableUnits()
{
	m_pBaseQuery->ClearAllSearchableUnits();
	m_pSymQuery->ClearAllSearchableUnits();
}


BOOL CDataQueryEx::SetObjectInfo(CPFeature pFt, int idx, LONG_PTR info)
{
	return m_pBaseQuery->SetObjectInfo(pFt,idx,info);
}


BOOL CDataQueryEx::GetObjectInfo(CPFeature pFt, int idx, LONG_PTR &info)const
{
	return m_pBaseQuery->GetObjectInfo(pFt,idx,info);
}


//get searchers
CSearcher *CDataQueryEx::GetActiveSearcher()
{
	return m_pBaseQuery->GetActiveSearcher();
}


CSetMaxNumResultsOfSearch::CSetMaxNumResultsOfSearch(CDataQueryEx *p, int num)
{
	m_pDQ = p;
	if( p==NULL )
		return;

	CSearcher *ps = m_pDQ->GetActiveSearcher();
	if( ps->GetType()==EBSE_TREE8 )
	{
		m_nSaveNum = ((CTree8Search*)ps)->SetMaxNumOfResult(num);
	}
}


CSetMaxNumResultsOfSearch::~CSetMaxNumResultsOfSearch()
{
	if( m_pDQ==NULL )
		return;

	CSearcher *p = m_pDQ->GetActiveSearcher();
	if( p->GetType()==EBSE_TREE8 )
	{
		((CTree8Search*)p)->SetMaxNumOfResult(m_nSaveNum);
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAttributesSource::CAttributesSource()
{
	m_pAccess = NULL;
	m_pDS = NULL;
}


CAttributesSource::~CAttributesSource()
{
	
}

BOOL CAttributesSource::GetXAttributes(CFeature *pFt, CValueTable& tab)
{
	if(!m_pAccess)return FALSE;

	CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFt);
	if( !pLayer )
		return FALSE;

	if( pLayer->IsHaveExtraAttr() )
	{
		m_pAccess->ReadXAttribute(pFt,tab);	 
	}

	m_pAccess->ReadOptAttributes(pFt,tab);

	return TRUE;
}

BOOL CAttributesSource::SetXAttributes(CFeature *pFt, CValueTable& tab,int idx)
{
	if(!m_pAccess) return FALSE;

	CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFt);
	if( !pLayer )
		return FALSE;
	
	if( pLayer->IsHaveExtraAttr() )
	{
		m_pAccess->SaveXAttribute(pFt,tab,idx);
	}
	
	m_pAccess->SaveOptAttributes(pFt,tab,idx);

	return TRUE;
}

BOOL CAttributesSource::CopyXAttributes(CFeature* pSrc, CFeature* pDes)
{
	if (!m_pAccess)  return TRUE;

	if(pSrc->GetID()==pDes->GetID()) return FALSE;	

	CFtrLayer *pLayer1 = m_pDS->GetFtrLayerOfObject(pSrc);
	if( !pLayer1 )
		return FALSE;

	CFtrLayer *pLayer2 = m_pDS->GetFtrLayerOfObject(pDes);
	if( !pLayer2 )
		return FALSE;

	if( pLayer1->IsHaveExtraAttr() && pLayer2->IsHaveExtraAttr() )
	{
		CValueTable tab;
		tab.BeginAddValueItem();
		m_pAccess->ReadXAttribute(pSrc,tab);
		tab.EndAddValueItem();
		m_pAccess->SaveXAttribute(pDes,tab);
	}

	CValueTable tab;
	tab.BeginAddValueItem();
	m_pAccess->ReadOptAttributes(pSrc,tab);
	tab.EndAddValueItem();
	m_pAccess->SaveOptAttributes(pDes,tab);

	return TRUE;
}


BOOL CAttributesSource::DelXAttributes(CFeature *pFt)
{
	if(!m_pAccess)return FALSE;

	CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFt);
	if( !pLayer )
		return FALSE;
	
	if( pLayer->IsHaveExtraAttr() )
	{
		m_pAccess->DelXAttributes(pFt);
	}

	m_pAccess->DelOptAttributes(pFt);

	return TRUE;
}


void CAttributesSource::SetScale(DWORD scale)
{
	m_nScale = scale;
}

BOOL CAttributesSource::RestoreXAttributes(CFeature *pFt)
{
	if(!m_pAccess)return FALSE;

	CFtrLayer *pLayer = m_pDS->GetFtrLayerOfObject(pFt);
	if( !pLayer )
		return FALSE;
	
	if( pLayer->IsHaveExtraAttr() )
	{
		m_pAccess->RestoreXAttributes(pFt);
	}

	m_pAccess->RestoreOptAttributes(pFt);

	return TRUE;
}

bool ObjectGroup::ClearObject(CFeature *pFtr)
{
	int size = ftrs.GetSize();
	for (int i=size-1; i>=0; i--)
	{
		CFeature *pObject = ftrs[i];
		if (pObject == pFtr)
		{
			ftrs.RemoveAt(i);
			break;
		}
	}

	return true;
}

bool ObjectGroup::AddObject(CFeature *pFtr)
{
	ftrs.Add(pFtr);
	return true;
}

bool FtrLayerGroup::DelFtrLayer(CFtrLayer *pFtrLayer)
{
	int size = ftrLayers.GetSize();
	for (int i=size-1; i>=0; i--)
	{
		CFtrLayer *pFtrLayer0 = ftrLayers[i];
		if (pFtrLayer0 == pFtrLayer)
		{
			ftrLayers.RemoveAt(i);
			break;
		}
	}
	
	return true;
}

bool FtrLayerGroup::AddFtrLayer(CFtrLayer *pFtrLayer)
{
	ftrLayers.Add(pFtrLayer);
	return true;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CDataSourceEx::CDataSourceEx() : CDataSource(NULL)
{
	m_nCurLayer = -1;

	memset(m_bounds,0,sizeof(m_bounds));

	m_bounds[1].x = m_bounds[2].x = 1000;
	m_bounds[2].y = m_bounds[2].y = 1000;
	m_nScale = 2000;
	m_lfZmin = -1000;
	m_lfZmax = 1000;
	
	m_bModified = FALSE;
	m_pAccess = NULL;
	m_pAttrSource = NULL;

	m_bShowBound = TRUE;
	m_bMonoMode = FALSE;
	m_monoColor = RGB(125,125,125);
	m_bSymbolizedSelect = TRUE;//AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_SYMBOLIZEDSELECT,TRUE);
	
	m_fDrawScale = 1;

	m_bDisplayOrderModified = TRUE;

	m_bHaveSaved = FALSE;
	m_nTotalModifiedTime = 0;
	m_nStartModifiedTime = 0;
}

CDataSourceEx::CDataSourceEx(CDataQueryEx *pDQ)
: CDataSource(pDQ)
{
	m_nCurLayer = -1;

	memset(m_bounds,0,sizeof(m_bounds));

	m_bounds[1].x = m_bounds[2].x = 1000;
	m_bounds[2].y = m_bounds[2].y = 1000;
	m_nScale = 2000;
	m_lfZmin = -1000;
	m_lfZmax = 1000;
	
	m_bModified = FALSE;
	m_pAccess = NULL;
	m_pAttrSource = new CAttributesSource;
	m_pAttrSource->m_pDS = this;

	m_bShowBound = TRUE;
	m_bMonoMode = FALSE;
	m_monoColor = RGB(125,125,125);
	m_bSymbolizedSelect = TRUE;//AfxGetApp()->GetProfileInt(REGPATH_SELECT,REGITEM_SYMBOLIZEDSELECT,TRUE);
	
	m_fDrawScale = GetProfileDouble(REGPATH_SYMBOL,REGITEM_DRAWSCALE,1);

	m_bDisplayOrderModified = TRUE;

	m_bHaveSaved = FALSE;
	m_nTotalModifiedTime = 0;
	m_nStartModifiedTime = 0;
	
	m_bUVSLocalCopy = FALSE;
}

BOOL CDataSourceEx::ClearAll(BOOL delete_org_data)
{
	if (!delete_org_data)
	{
		m_arrAllFtrsByOrderID.RemoveAll();
		//
		m_arrPFtrLayer.RemoveAll();
		//
		m_arrFtrGroup.RemoveAll();
		//
		m_arrFtrLayerGroup.RemoveAll();
		//
		m_arrAppFlagsBuf.RemoveAll();
		//
		m_arrUsedFtrlayer.RemoveAll();
		//
		m_arrPFtrLayerByDisplayOrder.RemoveAll();
		//
		m_arrUserMap.RemoveAll();
		//
		m_arrAppFlagsBuf.RemoveAll();
		//
		m_arrQueryFlagsBuf.RemoveAll();

		return CDataSource::ClearAll(FALSE);
	}
	//
	int nsz = m_arrPFtrLayer.GetSize();
	for (int i = 0; i < nsz; i++)
	{
		CFtrLayer *pLayer = m_arrPFtrLayer.GetAt(i);
		if (pLayer)
		{
			delete pLayer;
			pLayer = NULL;
		}
	}

	nsz = m_arrFtrGroup.GetSize();
	for (i = 0; i < nsz; i++)
	{
		ObjectGroup *pGroup = m_arrFtrGroup.GetAt(i);
		if (pGroup)
		{
			delete pGroup;
			pGroup = NULL;
		}
	}

	nsz = m_arrFtrLayerGroup.GetSize();
	for (i = 0; i < nsz; i++)
	{
		FtrLayerGroup *pGroup = m_arrFtrLayerGroup.GetAt(i);
		if (pGroup)
		{
			delete pGroup;
			pGroup = NULL;
		}
	}

	for (i = 0; i < m_arrAppFlagsBuf.GetSize(); i++)
	{
		delete m_arrAppFlagsBuf[i];
	}
	//
	m_arrAllFtrsByOrderID.RemoveAll();
	//
	m_arrPFtrLayer.RemoveAll();
	//
	m_arrFtrGroup.RemoveAll();
	//
	m_arrFtrLayerGroup.RemoveAll();
	//
	m_arrAppFlagsBuf.RemoveAll();
	//
	m_arrUsedFtrlayer.RemoveAll();
	//
	m_arrPFtrLayerByDisplayOrder.RemoveAll();
	//
	m_arrUserMap.RemoveAll();
	//
	m_arrAppFlagsBuf.RemoveAll();
	//
	m_arrQueryFlagsBuf.RemoveAll();

	return CDataSource::ClearAll(TRUE);
}

CDataSourceEx::~CDataSourceEx()
{
	///uvs  ///////////////////////////////
	uvscashe::CUVSCashe::WaiteExecuteAll();
	//
	int nsz = m_arrPFtrLayer.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CFtrLayer *pLayer = m_arrPFtrLayer.GetAt(i);
		if(pLayer)
		{
			delete pLayer;
			pLayer = NULL;
		}
	}
	
	nsz = m_arrFtrGroup.GetSize();
	for( i=0; i<nsz; i++)
	{
		ObjectGroup *pGroup = m_arrFtrGroup.GetAt(i);
		if(pGroup)
		{
			delete pGroup;
			pGroup = NULL;
		}
	}

	m_arrFtrGroup.RemoveAll();
	
	nsz = m_arrFtrLayerGroup.GetSize();
	for( i=0; i<nsz; i++)
	{
		FtrLayerGroup *pGroup = m_arrFtrLayerGroup.GetAt(i);
		if(pGroup)
		{
			delete pGroup;
			pGroup = NULL;
		}
	}
	
	m_arrFtrLayerGroup.RemoveAll();

	for( i=0; i<m_arrAppFlagsBuf.GetSize(); i++)
	{
		delete m_arrAppFlagsBuf[i];
	}
	m_arrAppFlagsBuf.RemoveAll();

	if(m_pAccess)
	{
		m_pAccess->Close();
		delete m_pAccess;
		m_pAccess = NULL;
	}
	if (m_pAttrSource)
	{
		delete m_pAttrSource;
	}
}



void CDataSourceEx::CreateSearchableUnit(CFeature *pObj, CPtrArray& arrPUnits)
{
#ifndef TRIAL_VERSION
	CDataSource::CreateSearchableUnit(pObj, arrPUnits);
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

BOOL CDataSourceEx::AddObject(CFeature *pFt)
{
	CFtrLayer *pLayer = GetCurFtrLayer();
	if( !pLayer )return FALSE;

	return AddObject(pFt,pLayer->GetID());
}

BOOL CDataSourceEx::UpdateFtrQuery(CPFeature pFtr)
{
	if (!M_PDQ) return FALSE;

	// 全部更新
	if (pFtr == NULL)
	{
		int nSum = 0;
		for (int j=0; j<GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = GetFtrLayerByIndex(j);
			if( !pLayer || !pLayer->IsVisible() )continue;
			
			nSum += pLayer->GetObjectCount();
		}

		GOutPut(StrFromResID(IDS_TIP_RESET_QUERYENGINE));
		GProgressStart(nSum);

		for (j=0; j<GetFtrLayerCount(); j++)
		{
			CFtrLayer *pLayer = GetFtrLayerByIndex(j);
			if( !pLayer || !pLayer->IsVisible() )continue;
			
			int nObjs = pLayer->GetObjectCount();
			for(int k=0; k<nObjs; k++)
			{			
				GProgressStep();

				CFeature *pFtr = pLayer->GetObject(k);	
				if (!pFtr) continue;
				
				M_PDQ->DelSearchableUnit(pFtr,TRUE);

				CGeometry *pGeo = pFtr->GetGeometry();
				if (pGeo)
				{
					CPtrArray arrPUnits;
					CreateSearchableUnit(pFtr, arrPUnits);
					M_PDQ->AddSearchableUnit(pFtr,(SearchableUnit**)arrPUnits.GetData(),arrPUnits.GetSize());
					
					M_PDQ->SetObjectInfo(pFtr, 0, (LONG_PTR)this);
					M_PDQ->SetObjectInfo(pFtr, 1, (LONG_PTR)pLayer);

					AddSymGrBufToQuery(pFtr);
					
				}
			}
		}

		GProgressEnd();
	}
	else
	{
		CFtrLayer *pLayer = GetFtrLayerOfObject(pFtr);
		M_PDQ->DelSearchableUnit(pFtr,TRUE);

		CGeometry *pGeo = pFtr->GetGeometry();
		if(pGeo)
		{
			CPtrArray arrPUnits;
			CreateSearchableUnit(pFtr, arrPUnits);
			M_PDQ->AddSearchableUnit(pFtr,(SearchableUnit**)arrPUnits.GetData(),arrPUnits.GetSize());
			
			M_PDQ->SetObjectInfo(pFtr, 0, (LONG_PTR)this);
			M_PDQ->SetObjectInfo(pFtr, 1, (LONG_PTR)pLayer);

			AddSymGrBufToQuery(pFtr);

		}
	}

	return TRUE;
}

void CDataSourceEx::AddSymGrBufToQuery(CPFeature pFtr)
{
	if( !m_bSymbolizedSelect )
		return;

	CPtrArray arr;
	GetConfigLibManager()->GetSymbol(this,pFtr,arr);

	//无符号，则不需要增加符号化图形到查询器中
	//因为母线已经在查询器中了
	if (arr.GetSize()<1 && 0) 
	{
		if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
		{
			CGeoSurface *pGeo = (CGeoSurface*)pFtr->GetGeometry();
			if( pGeo->IsFillColor())
			{
				CArray<SearchableUnit*,SearchableUnit*> arrPUnits;

				SearchableGrBuf *p = new SearchableGrBuf;
				if (!p) return;
				pGeo->Draw(p->CreateBuf());
				p->RefreshEnvelope();
				arrPUnits.Add(p);
				
				M_PDQ->AddSearchableUnit_Sym(pFtr,arrPUnits.GetData(),arrPUnits.GetSize());
			}
			else if( m_bSurfaceInsideSelect && pGeo->IsClosed() )
			{
				CArray<SearchableUnit*,SearchableUnit*> arrPUnits;

				SearchableGrBuf *p = new SearchableGrBuf;
				if (!p) return;

				pGeo = (CGeoSurface*)pGeo->Clone();
				pGeo->EnableFillColor(TRUE,RGB(255,0,0));
				pGeo->Draw(p->CreateBuf());
				p->RefreshEnvelope();
				arrPUnits.Add(p);

				delete pGeo;
				
				M_PDQ->AddSearchableUnit_Sym(pFtr,arrPUnits.GetData(),arrPUnits.GetSize());				
			}
		}
		return;
	}
	else
	{
		for (int k=0; k<arr.GetSize(); k++)
		{
			BOOL bIgnore = FALSE;

			CSymbol *pSym = (CSymbol*)arr[k];
			if( pSym->GetType()==SYMTYPE_DASHLINETYPE || pSym->GetType()==SYMTYPE_CELLLINETYPE )
			{
				if( ((CDashLinetype*)pSym)->m_fBaseOffset==0 )
					bIgnore = TRUE;
			}

			if( !bIgnore )break;
		}

		//如果完全为虚线线型，且无基线偏移，也不需要增加符号化图形到查询器中
		//因为母线已经在查询器中了
		if( k>=arr.GetSize() && 0 )
		{
			if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
			{
				CGeoSurface *pGeo = (CGeoSurface*)pFtr->GetGeometry();
				if( pGeo->IsFillColor())
				{
					CArray<SearchableUnit*,SearchableUnit*> arrPUnits;

					SearchableGrBuf *p = new SearchableGrBuf;
					if (!p) return;
					pGeo->Draw(p->CreateBuf());
					p->RefreshEnvelope();
					arrPUnits.Add(p);
					
					M_PDQ->AddSearchableUnit_Sym(pFtr,arrPUnits.GetData(),arrPUnits.GetSize());
				}
				else if( m_bSurfaceInsideSelect && pGeo->IsClosed() )
				{
					CArray<SearchableUnit*,SearchableUnit*> arrPUnits;

					SearchableGrBuf *p = new SearchableGrBuf;
					if (!p) return;

					pGeo = (CGeoSurface*)pGeo->Clone();
					pGeo->EnableFillColor(TRUE,RGB(255,0,0));
					pGeo->Draw(p->CreateBuf());
					p->RefreshEnvelope();
					arrPUnits.Add(p);

					delete pGeo;
					
					M_PDQ->AddSearchableUnit_Sym(pFtr,arrPUnits.GetData(),arrPUnits.GetSize());				
				}
			}
			return;
		}			
	}
	
	GrBuffer buf0, buf;

	//不需要将无基线偏移的虚线线型增加添加到查询器
	for ( int k=0; k<arr.GetSize(); k++)
	{
		CSymbol *pSym = (CSymbol*)arr[k];
		if( pSym->GetType()==SYMTYPE_DASHLINETYPE || pSym->GetType()==SYMTYPE_CELLLINETYPE )
		{
			if( ((CDashLinetype*)pSym)->m_fBaseOffset==0 )
				continue;
		}

		double fDrawScale = ((double)GetScale()/1000)*m_fDrawScale;
// 		double fDrawScale = (double)GetScale()/1000;
// 		if (m_bScaleByUser)
// 		{
// 			fDrawScale = m_fDrawScale;
// 		}				
		
		if( pSym->GetType() == SYMTYPE_CELLHATCH && pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
		{
			CColorHatch ch;
			ch.Draw(pFtr,&buf,fDrawScale);
		}
		// 若为注记
		else if (pSym->GetType() == SYMTYPE_ANNOTATION && !CAnnotation::m_bUpwardText)
		{
			CValueTable tab;
			if (pSym->IsNeedAttrWhenDraw())
			{
				GetAllAttribute(pFtr,tab);
			}			
			((CAnnotation*)arr[k])->Draw(pFtr,&buf,tab,fDrawScale,0,1);
		}
		else
			((CSymbol*)arr[k])->Draw(pFtr,&buf,fDrawScale,0,1);
		
		buf0.AddBuffer(&buf);
		buf.DeleteAll();
	}

	CArray<SearchableUnit*,SearchableUnit*> arrPUnits;

	//点状图元可能会很多，就每50个打一个包
	const Graph *pGr = buf0.HeadGraph();
	int nGraphSum = 0, nGraphLimit = 0;
	while( pGr )
	{
		nGraphSum++;
		pGr = pGr->next;
	}
	
	nGraphLimit = nGraphSum/500;
	if( nGraphLimit<50 )nGraphLimit = 50;
	
	nGraphLimit = 10; 

	pGr = buf0.HeadGraph();
	while( pGr )
	{
		SearchableGrBuf *p = new SearchableGrBuf;
		if( p )
		{
			GrBuffer *pBuf = p->CreateBuf();
			int nGraphs = 0;

			while( pGr )
			{
				BOOL bPackage = FALSE;

				if( IsGrPoint(pGr) || IsGrText(pGr) )
				{
					bPackage = TRUE;
				}
				else if( IsGrLineString(pGr) && ((GrLineString*)pGr)->ptlist.nuse<3 )
				{
					bPackage = TRUE;
				}
				bPackage = TRUE;

				pBuf->InsertGraph(pGr);

				nGraphs++;
				pGr = pGr->next;

				if( bPackage && nGraphs<nGraphLimit )
				{
				}
				else
				{
					break;
				}
			}
			
			p->RefreshEnvelope();
			arrPUnits.Add(p);
		}
	}
	
	M_PDQ->AddSearchableUnit_Sym(pFtr,arrPUnits.GetData(),arrPUnits.GetSize());
}

BOOL CDataSourceEx::AddObject(CFeature *pFt, int layid, BOOL bForLoad)
{
	CFtrLayer *pLayer = GetFtrLayer(layid);
	if( !pLayer )return FALSE;
	
  	if( !CDataSource::AddObject(pFt) )
  		return FALSE;
	
	if(M_PDQ)
		M_PDQ->SetObjectInfo(pFt, 1, (LONG_PTR)pLayer);

	int index;
	if( !bForLoad && (pFt->GetDisplayOrder()<0 || FindOrderIndex(pFt->GetDisplayOrder(),index)))
	{
		int nOrder = GetFtrsMaxMinDisplayOrder(TRUE);
		pFt->SetDisplayOrder(nOrder+1);
		m_arrAllFtrsByOrderID.Add(pFt);
	}
	else 
	{
		FindOrderIndex(pFt->GetDisplayOrder(),index);
		m_arrAllFtrsByOrderID.InsertAt(index,pFt);
	}

	pLayer->AddObject(pFt);

	if( pFt->GetID().IsNull() )
	{
		GUID id;
		::CoCreateGuid(&id);
		pFt->SetID(id);

		if (!bForLoad)
		{
			if( m_pAccess )
			{
				m_pAccess->SetCurFtrLayID(layid);
				m_pAccess->SaveFeature(pFt);
			}

			SetDefaultXAttributes(pFt);

			SetModifiedFlag(TRUE);
		}	

	}
	else
	{
		if (!bForLoad)
		{
			GUID id;
			::CoCreateGuid(&id);
			pFt->SetID(id);

			if( m_pAccess )
			{
				m_pAccess->SetCurFtrLayID(layid);
				m_pAccess->SaveFeature(pFt);
			}
			
			if( m_pAttrSource )
				m_pAttrSource->RestoreXAttributes(pFt);

			SetModifiedFlag(TRUE);
		}
		
	}

	if(M_PDQ)AddSymGrBufToQuery(pFt);

	return TRUE;

}

BOOL CDataSourceEx::AddObject(CFeature *pFt, LPCTSTR layid, BOOL bForLoad)
{
	CFtrLayer *pLayer = GetFtrLayerBystrID(layid);
	if (!pLayer)return FALSE;

	if (!CDataSource::AddObject(pFt))
		return FALSE;

	if (M_PDQ)
		M_PDQ->SetObjectInfo(pFt, 1, (LONG_PTR)pLayer);

	int index;
	if (!bForLoad && (pFt->GetDisplayOrder() < 0 || FindOrderIndex(pFt->GetDisplayOrder(), index)))
	{
		int nOrder = GetFtrsMaxMinDisplayOrder(TRUE);
		pFt->SetDisplayOrder(nOrder + 1);
		m_arrAllFtrsByOrderID.Add(pFt);
	}
	else
	{
		FindOrderIndex(pFt->GetDisplayOrder(), index);
		m_arrAllFtrsByOrderID.InsertAt(index, pFt);
	}

	pLayer->AddObject(pFt);

	if (pFt->GetID().IsNull())
	{
		GUID id;
		::CoCreateGuid(&id);
		pFt->SetID(id);

		if (!bForLoad)
		{
			if (m_pAccess)
			{
				//m_pAccess->SetCurFtrLayID(layid);
				//m_pAccess->SaveFeature(pFt);
			}

			SetDefaultXAttributes(pFt);

			SetModifiedFlag(TRUE);
		}

	}
	else
	{
		if (!bForLoad)
		{
			GUID id;
			::CoCreateGuid(&id);
			pFt->SetID(id);

			if (m_pAccess)
			{
				//m_pAccess->SetCurFtrLayID(layid);
				//m_pAccess->SaveFeature(pFt);
			}

			if (m_pAttrSource)
				m_pAttrSource->RestoreXAttributes(pFt);

			SetModifiedFlag(TRUE);
		}

	}

	if (M_PDQ)AddSymGrBufToQuery(pFt);

	return TRUE;
}

BOOL CDataSourceEx::DeleteObject(CPFeature pFt)
{
	if( !pFt )return FALSE;

	BOOL bRet = CDataSource::DeleteObject(pFt);

	if( m_pAttrSource )
		m_pAttrSource->DelXAttributes(pFt);
	int index;
	if (FindOrderIndex(pFt->GetDisplayOrder(),index) && m_arrAllFtrsByOrderID[index] == pFt)
	{
		m_arrAllFtrsByOrderID.RemoveAt(index);
	}
	else
	{
		int aa = 10;
	}

	if( m_pAccess )
	{
		CFtrLayer *pLayer = GetFtrLayerOfObject(pFt);
		if( pLayer )
		{
			m_pAccess->SetCurFtrLayID(pLayer->GetID());
			m_pAccess->DelFeature(pFt);
		}
	}
	SetModifiedFlag(TRUE);
	return bRet;
}

BOOL CDataSourceEx::DeleteObjectKeepOrder(CPFeature pFt)
{
	if( !pFt )return FALSE;
	
	BOOL bRet = CDataSource::DeleteObject(pFt);
	
	if( m_pAttrSource )
		m_pAttrSource->DelXAttributes(pFt);
	
	int index;
	if (FindOrderIndex(pFt->GetDisplayOrder(),index) && m_arrAllFtrsByOrderID[index] == pFt)
	{
		m_arrAllFtrsByOrderID.RemoveAt(index);
	}

	SetModifiedFlag(TRUE);
	return bRet;
}

BOOL CDataSourceEx::ClearObject(CPFeature pFt)
{
	if(!pFt)
		return FALSE;
	
	if (pFt)
	{
		CUIntArray indexs;
		pFt->GetObjectGroup(indexs);
		int size = indexs.GetSize();
		for (int i=size; i>=0; i--)
		{
			ObjectGroup *pGroup = GetObjectGroup(i);
			if (pGroup)
			{
				pGroup->ClearObject(pFt);
			}
		}
	}

	int index;
	if (FindOrderIndex(pFt->GetDisplayOrder(),index) && m_arrAllFtrsByOrderID[index] == pFt)
	{
		m_arrAllFtrsByOrderID.RemoveAt(index);
	}
	CFtrLayer *pLayer = GetFtrLayerOfObject(pFt);
	if( pLayer )
	{
		if( m_pAccess )
		{
			m_pAccess->SetCurFtrLayID(pLayer->GetID());
			if( pFt )m_pAccess->DelFeature(pFt);
		}

		pLayer->DeleteObject(pFt);
		
	}
	
// 	if( m_pAccess )
// 	{
// 		CFtrLayer *pLayer = GetFtrLayerOfObject(pFt);
// 		if( pLayer )
// 		{
// 			m_pAccess->SetCurFtrLayID(pLayer->GetID());
// 			if( pFt )m_pAccess->DelFeature(pFt);
// 
// 			pLayer->DeleteObject(pFt);
// 		}
// 	}

	BOOL bRet = CDataSource::ClearObject(pFt);

	SetModifiedFlag(TRUE);
	return bRet;
}

BOOL CDataSourceEx::RestoreObject(CPFeature pFt)
{
	if( !CDataSource::RestoreObject(pFt) )
		return FALSE;

	int index;
	if (!FindOrderIndex(pFt->GetDisplayOrder(),index))
		m_arrAllFtrsByOrderID.InsertAt(index,pFt);
	else if( m_arrAllFtrsByOrderID[index]!=pFt )
		m_arrAllFtrsByOrderID.InsertAt(index,pFt);

	CFtrLayer *pLayer = GetFtrLayerOfObject(pFt);
	if( !pLayer )return FALSE;	
	
	if( m_pAccess )
	{
		m_pAccess->SetCurFtrLayID(pLayer->GetID());
		if( pFt )m_pAccess->SaveFeature(pFt);
	}	
	if(m_pAttrSource) m_pAttrSource->RestoreXAttributes(pFt);

	AddSymGrBufToQuery(pFt);

	SetModifiedFlag(TRUE);
	return TRUE;
}


int CDataSourceEx::GetUserMapCount()
{
	return m_arrUserMap.GetSize();
}

UserMap CDataSourceEx::GetUserMap(int i)
{
	return m_arrUserMap.GetAt(i);
}

UserMap CDataSourceEx::GetUserMap(LPCTSTR name)
{
	UserMap *data = m_arrUserMap.GetData();
	int nsz = m_arrUserMap.GetSize();
	for( int i=0; i<nsz; i++)
	{
		if( _tcsicmp(data[i].name,name)==0 )break;
	}
	
	if( i<nsz )return data[i];
	return UserMap();
}

int CDataSourceEx::GetFtrLayerCount()
{
	return m_arrPFtrLayer.GetSize();
}

int CDataSourceEx::GetValidObjCount()
{
	CFtrLayer *pFtrLayer = NULL;
	int nObjs = 0;
	for (int i=m_arrPFtrLayer.GetSize()-1;i>=0;i--)
	{
		pFtrLayer = m_arrPFtrLayer[i];
		if(pFtrLayer==NULL||pFtrLayer->IsDeleted())continue;
		nObjs+=pFtrLayer->GetValidObjsCount();
	}
	return nObjs;
}
int CDataSourceEx::GetEditableObjCount()
{
	CFtrLayer *pFtrLayer = NULL;
	int nObjs = 0;
	for (int i=m_arrPFtrLayer.GetSize()-1;i>=0;i--)
	{
		pFtrLayer = m_arrPFtrLayer[i];
		if(pFtrLayer==NULL||pFtrLayer->IsDeleted()||pFtrLayer->IsLocked()||!pFtrLayer->IsVisible())
			continue;
		nObjs+=pFtrLayer->GetEditableObjsCount();
	}
	return nObjs;
}

int CDataSourceEx::GetCurFtrLayerID()
{
	CFtrLayer *pFtrLayer = GetFtrLayerByIndex(m_nCurLayer);
	if (!pFtrLayer)
		return -1;
	return pFtrLayer->GetID();
}

CString CDataSourceEx::GetFtrLayerNameByNameOrLayerIdx(LPCTSTR name)
{
	if (name == NULL)
		return name;

	__int64 code = _ttoi64(name);
	if( code>0 )
	{
		CScheme *pScheme = GetConfigLibManager()->GetScheme(m_nScale);
		if (pScheme)
		{
			CString str;
			if(pScheme->FindLayerIdx(TRUE, code, str))
				return str;
		}
	}

	return name;
}

__int64 CDataSourceEx::GetFtrLayerCode(LPCTSTR name)
{
	if (name == NULL) return 0;
	
	__int64 code = 0;
	CScheme *pScheme = GetConfigLibManager()->GetScheme(m_nScale);
	if (pScheme)
	{
		CString str = name;
		pScheme->FindLayerIdx(FALSE, code, str);
	}

	return code;
}
/*
//层名或层码
CFtrLayer* CDataSourceEx::GetFtrLayerByNameOrLayerIdx(const char *name)
{
	char *pos = NULL;
	long code = strtol(name,&pos,10);
	if( _tcslen(pos)<=0 )
	{
		CConfigLibManager *pConfigMag = ((CFeatureBaseApp*)AfxGetApp())->GetConfigLibManager();
		CScheme *pScheme = pConfigMag->GetScheme(m_nScale);
		if(pScheme)
			pScheme->FindLayerIdx(TRUE,code,name);		
	}
	CString name2 = name;	
	return GetFtrLayer(LPCTSTR(name2));
}
*/
CFtrLayer* CDataSourceEx::GetFtrLayerByIndex(int i)
{
	if( i>=0 && i<m_arrPFtrLayer.GetSize() )
	{
		CFtrLayer* pLayer = m_arrPFtrLayer.GetAt(i);
		if( !pLayer->IsDeleted() )
			return pLayer;
	}
	return NULL;
}


static int CompFuncSortLayerInDS(const void *e1, const void *e2)
{
	int order1 = (*(CFtrLayer**)e1)->GetDisplayOrder();
	int order2 = (*(CFtrLayer**)e2)->GetDisplayOrder();
	if( order1<order2 )
	{
		return -1;
	}
	else if( order1==order2 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void CDataSourceEx::GetAllFtrLayersByDisplayOrder(CFtrLayerArray& arr)
{
	arr.Copy(m_arrPFtrLayerByDisplayOrder);

	if( m_bDisplayOrderModified )
	{
		arr.Copy(m_arrPFtrLayer);

		CFtrLayer **data = arr.GetData();
		int nCount = arr.GetSize();
		
		qsort(data,nCount,sizeof(CFtrLayer*),CompFuncSortLayerInDS);

		/*
		int minOrder, k, nLay = arr.GetSize();
		for (int i=0; i<nLay; i++)
		{
			k = i;
			for( int j=i+1; j<nLay; j++)
			{
				if( arr[k]->GetDisplayOrder()>arr[j]->GetDisplayOrder() )
				{
					k = j;
				}
			}
			
			if( k!=i )
			{
				CFtrLayer *pLayer = arr[k];
				arr[k] = arr[i];
				arr[i] = pLayer;
			}
		}*/

		m_arrPFtrLayerByDisplayOrder.Copy(arr);
		m_bDisplayOrderModified = FALSE;
	}
}

CFtrLayer* CDataSourceEx::GetFtrLayer(LPCTSTR strLayerName, LPCTSTR mapname, CPtrArray *arr)
{
	mapname = NULL;

	CString layname = GetFtrLayerNameByNameOrLayerIdx(strLayerName);

	if (layname.IsEmpty())
		return NULL;

	int i, j;
	for( i=0; i<2; i++)
	{
		if( i==1 )
		{
			if( _tcsicmp(layname,strLayerName)==0 )
				break;
			layname = strLayerName;
		}

		if (arr == NULL)
		{
			CFtrLayer *pLayer;
			
			for (j=0; j<m_arrUsedFtrlayer.GetSize(); j++)
			{
				int index = m_arrUsedFtrlayer.GetAt(j);
				pLayer = m_arrPFtrLayer.GetAt(index);
				if( pLayer->IsDeleted() )continue;
				
				if( _tcsicmp(pLayer->GetName(),layname)==0 )
				{
					if( mapname==NULL || _tcslen(mapname)<=0)
					{
						if (_tcslen(pLayer->GetMapName()) <= 0)
						{
							return pLayer;
						}						
					}
					else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
						return pLayer;
				}
			}
			
			int nsz = m_arrPFtrLayer.GetSize();
			for (j=0; j<nsz; j++)
			{
				pLayer = m_arrPFtrLayer.GetAt(j);
				if( pLayer->IsDeleted() )continue;
				
				if( _tcsicmp(pLayer->GetName(),layname)==0 )
				{
					if( mapname==NULL || _tcslen(mapname)<=0)
					{
						if (_tcslen(pLayer->GetMapName()) <= 0)
						{
							m_arrUsedFtrlayer.InsertAt(0,j);
							if (m_arrUsedFtrlayer.GetSize() > 10)
							{
								m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
							}

							return pLayer;
						}
					}
					else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
					{
						m_arrUsedFtrlayer.InsertAt(0,j);
						if (m_arrUsedFtrlayer.GetSize() > 10)
						{
							m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
						}

						return pLayer;
					}
				}
			}
		}
		else
		{
			int nsz = m_arrPFtrLayer.GetSize();
			for (j=0; j<nsz; j++)
			{
				CFtrLayer *pLayer = m_arrPFtrLayer.GetAt(j);
				if( pLayer->IsDeleted() )continue;
				
				if (_tcsicmp(pLayer->GetName(),layname) == 0)
				{
					if( mapname==NULL || _tcslen(mapname)<=0)
					{
						arr->Add(pLayer);
					}
					else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
					{
						arr->Add(pLayer);
					}
				}
			}
			
			if (arr->GetSize() > 0)
				return (CFtrLayer*)arr->GetAt(0);
			
		}
	}
	
	
	return NULL;

}

CFtrLayer* CDataSourceEx::GetLocalFtrLayer(LPCTSTR strLayerName)
{
	CString layname = GetFtrLayerNameByNameOrLayerIdx(strLayerName);
	
	if (layname.IsEmpty())  return NULL;

	CFtrLayer *pLayer;
	
	int i, j;
	for( i=0; i<2; i++)
	{
		if( i==1 )
		{
			if( _tcsicmp(layname,strLayerName)==0 )
				break;
			layname = strLayerName;
		}

		for (j=0; j<m_arrUsedFtrlayer.GetSize(); j++)
		{
			int index = m_arrUsedFtrlayer.GetAt(j);
			pLayer = m_arrPFtrLayer.GetAt(index);
			if( pLayer->IsDeleted() || !pLayer->IsInherent())continue;
			
			if( _tcsicmp(pLayer->GetName(),layname)==0 )
			{
				return pLayer;	
			}
		}
		
		int nsz = m_arrPFtrLayer.GetSize();
		for (j=0; j<nsz; j++)
		{
			pLayer = m_arrPFtrLayer.GetAt(j);
			if( pLayer->IsDeleted() || !pLayer->IsInherent() )continue;
			
			if( _tcsicmp(pLayer->GetName(),layname)==0 )
			{
				return pLayer;
			}
		}

		layname = strLayerName;

	}

	return NULL;
}

BOOL convertStringToStrArray(LPCTSTR str, CStringArray &arr)
{
	if (!str)
		return FALSE;
	CString temp(str);
	int startPos = 0, endPos = 0;
	while (endPos >= 0)
	{
		endPos = temp.Find(_T(","), startPos);
		if (endPos > startPos)
			arr.Add(temp.Mid(startPos, endPos - startPos));
		else
			arr.Add(temp.Mid(startPos));
		startPos = endPos + 1;
	}
	return TRUE;
}

void CDataSourceEx::GetFtrLayersByNameOrCode(LPCTSTR strLayerName, CFtrLayerArray& arr)
{
	CStringArray arrstrs;
	convertStringToStrArray(strLayerName, arrstrs);
	for (int i = 0; i < arrstrs.GetSize(); i++)
	{
		CPtrArray arr2;
		GetFtrLayer(arrstrs[i], NULL, &arr2);

		for (int j = 0; j < arr2.GetSize(); j++)
		{
			arr.Add((CFtrLayer*)arr2[j]);
		}
	}
}

void CDataSourceEx::GetFtrLayersByNameOrCode_editable(LPCTSTR strLayerName, CFtrLayerArray& arr)
{
	CString name = strLayerName;
	if (name.IsEmpty())
	{
		for (int i=0; i<m_arrPFtrLayer.GetSize(); i++)
		{
			CFtrLayer *pLayer = m_arrPFtrLayer.GetAt(i);
			if( pLayer->IsDeleted() || !pLayer->IsVisible() || pLayer->IsLocked() )
				continue;
			
			if(pLayer->GetObjectCount()>0)
				arr.Add(pLayer);
		}
		return;
	}
	
	CStringArray arrstrs;
	convertStringToStrArray(strLayerName, arrstrs);
	for (int i = 0; i < arrstrs.GetSize(); i++)
	{
		CPtrArray arr2;
		GetFtrLayer(arrstrs[i], NULL, &arr2);

		for (int j = 0; j < arr2.GetSize(); j++)
		{
			CFtrLayer *pLayer = (CFtrLayer*)arr2[j];
			if (pLayer->IsDeleted() || !pLayer->IsVisible() || pLayer->IsLocked())
				continue;

			if (pLayer->GetObjectCount()>0)
				arr.Add(pLayer);
		}
	}
}

CFtrLayer* CDataSourceEx::GetCurFtrLayer()
{
	return GetFtrLayerByIndex(m_nCurLayer);
}
/*
CFtrLayer* CDataSourceEx::GetFtrLayer(LPCTSTR layname, LPCTSTR mapname)
{
	if( mapname==NULL && layname==NULL )
	{
		return GetFtrLayerByIndex(m_nCurLayer);
	}

	
	CFtrLayer *pLayer;

	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer = m_arrPFtrLayer.GetAt(index);
		if( pLayer->IsDeleted() )continue;
		
		if( layname!=NULL && _tcsicmp(pLayer->GetName(),layname)==0 )
		{
			if( mapname==NULL )
				return pLayer;
			else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
				return pLayer;
		}
	}

	int nsz = m_arrPFtrLayer.GetSize();
	for (i=0; i<nsz; i++)
	{
		pLayer = m_arrPFtrLayer.GetAt(i);
		if( pLayer->IsDeleted() )continue;

		if( layname!=NULL && _tcsicmp(pLayer->GetName(),layname)==0 )
		{
			if( mapname==NULL )
				return pLayer;
			else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
				return pLayer;
		}
	}
	return NULL;
}
*/

CFtrLayer* CDataSourceEx::GetFtrLayer(int layid)
{
	
	CFtrLayer *pLayer;

	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer = m_arrPFtrLayer.GetAt(index);
		if( pLayer->IsDeleted() )continue;
		
		if( pLayer->GetID()==layid )
		{
			return pLayer;
		}
	}

	int nsz = m_arrPFtrLayer.GetSize();
	
	for (i=0; i<nsz; i++)
	{
		pLayer = m_arrPFtrLayer.GetAt(i);
		if( pLayer->IsDeleted() )continue;

		if( pLayer->GetID()==layid )
		{
			m_arrUsedFtrlayer.InsertAt(0,i);
			if (m_arrUsedFtrlayer.GetSize() > 10)
			{
				m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
			}

			return pLayer;
		}
	}
	return NULL;
}

CFtrLayer* CDataSourceEx::GetFtrLayerBystrID(LPCTSTR str_id)
{
	CFtrLayer *pLayer;
	
	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		
		pLayer = m_arrPFtrLayer.GetAt(index);
		
		if( pLayer->IsDeleted() )continue;
		
		if( pLayer->GetstrID()==str_id )
		{
			return pLayer;
		}
	}
	
	int nsz = m_arrPFtrLayer.GetSize();
	
	for (i=0; i<nsz; i++)
	{
		pLayer = m_arrPFtrLayer.GetAt(i);
		
		if( pLayer->IsDeleted() )continue;
		CString temp=pLayer->GetstrID();
		if( temp==CString(str_id) )
		{
			m_arrUsedFtrlayer.InsertAt(0,i);
			if (m_arrUsedFtrlayer.GetSize() > 10)
			{
				m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
			}
			
			
			return pLayer;
		}
	}
	
	return NULL;
}

CFtrLayer* CDataSourceEx::GetDefaultTxtFtrLayer()
{
	return GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMET));
}

CFtrLayer* CDataSourceEx::GetDefaultPntFtrLayer()
{
	return GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEP));
}

CFtrLayer* CDataSourceEx::GetDefaultLineFtrLayer()
{
	return GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMEL));
}

CFtrLayer* CDataSourceEx::GetDefaultPolygonFtrLayer()
{
	return GetLocalFtrLayer(StrFromResID(IDS_DEFLAYER_NAMES));
}

CFtrLayer* CDataSourceEx::GetFtrLayerOfObject(CPFeature pObj)
{
	LONG_PTR info;
	if(!M_PDQ)
		return NULL;

	if( !M_PDQ->GetObjectInfo(pObj,0,info) )
		return NULL;

	if (info != (LONG_PTR)this)
		return NULL;

	if( !M_PDQ->GetObjectInfo(pObj,1,info) )
		return NULL;
	
	return (CFtrLayer*)(info);
}

BOOL CDataSourceEx::RestoreFtrLayerOfObject(CPFeature pFtr,int layid)
{
	if (!m_pAccess)
		return FALSE;
	
	LONG_PTR info;
	if(!M_PDQ)
		return FALSE;
	
	if( !M_PDQ->GetObjectInfo(pFtr,0,info) )
		return FALSE;
	
	if (info != (LONG_PTR)this)
		return FALSE;
	
	if( !M_PDQ->GetObjectInfo(pFtr,1,info) )
		return FALSE;
	
	CFtrLayer *pNewLayer = GetFtrLayer(layid); 
	if( info==0 || pNewLayer==NULL )
		return FALSE;
	
	if (info == (LONG_PTR)pNewLayer)
		return FALSE;
	
	CFtrLayer *pOldLayer = ((CFtrLayer*)info);
	
	CValueTable tab;
	if(m_pAttrSource)
	{		
// 		tab.BeginAddValueItem();
// 		m_pAttrSource->GetXAttributes(pFtr,tab);
// 		tab.EndAddValueItem();
		m_pAttrSource->DelXAttributes(pFtr);
	}
	pOldLayer->DeleteObject(pFtr);
	
	m_pAccess->SetCurFtrLayID(pOldLayer->GetID());
	if( pFtr )m_pAccess->DelFeature(pFtr);
	
	
	pNewLayer->AddObject(pFtr);
	
	M_PDQ->SetObjectInfo(pFtr, 1, (LONG_PTR)pNewLayer);
	
	m_pAccess->SetCurFtrLayID(pNewLayer->GetID());
	if( pFtr )m_pAccess->SaveFeature(pFtr);
	
	if(m_pAttrSource)
	{
//		SetDefaultXAttributes(pFtr);
		m_pAttrSource->RestoreXAttributes(pFtr);
	}
	return TRUE;
}

BOOL CDataSourceEx::SetFtrLayerOfObject(CPFeature pFtr,int layid)
{
	if (!m_pAccess)
		return FALSE;
	
	LONG_PTR info;
	if(!M_PDQ)
		return FALSE;

	if( !M_PDQ->GetObjectInfo(pFtr,0,info) )
		return FALSE;
	
	if (info != (LONG_PTR)this)
		return FALSE;
	
	if( !M_PDQ->GetObjectInfo(pFtr,1,info) )
		return FALSE;

	CFtrLayer *pNewLayer = GetFtrLayer(layid); 
	if( info==0 || pNewLayer==NULL )
		return FALSE;

	if (info == (LONG_PTR)pNewLayer)
		return FALSE;

	CFtrLayer *pOldLayer = ((CFtrLayer*)info);

	CValueTable tab;
	if(m_pAttrSource)
	{		
		tab.BeginAddValueItem();
		m_pAttrSource->GetXAttributes(pFtr,tab);
		tab.EndAddValueItem();
		m_pAttrSource->DelXAttributes(pFtr);
	}
	pOldLayer->DeleteObject(pFtr);
	
	m_pAccess->SetCurFtrLayID(pOldLayer->GetID());
	if( pFtr )m_pAccess->DelFeature(pFtr);
	

	pNewLayer->AddObject(pFtr);

	M_PDQ->SetObjectInfo(pFtr, 1, (LONG_PTR)pNewLayer);

	m_pAccess->SetCurFtrLayID(pNewLayer->GetID());
	if( pFtr )m_pAccess->SaveFeature(pFtr);
	
	if(m_pAttrSource)
	{
		SetDefaultXAttributes(pFtr);
		m_pAttrSource->SetXAttributes(pFtr,tab);
	}
	SetModifiedFlag(TRUE);
	return TRUE;
}
/*
BOOL CDataSourceEx::SetCurFtrLayer(LPCTSTR layname, LPCTSTR mapname)
{
	if( layname==NULL )
		return FALSE;

	int nsz = m_arrPFtrLayer.GetSize();
	CFtrLayer *pLayer;

	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer = m_arrPFtrLayer.GetAt(index);
		if( pLayer->IsDeleted() )continue;
		
		if( _tcsicmp(pLayer->GetName(),layname)==0 )
		{
			if( mapname==NULL )
			{
				m_nCurLayer = index;
				return TRUE;
			}
			else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
			{
				m_nCurLayer = index;
				return TRUE;
			}
		}
	}
	
	for (i=0; i<nsz; i++)
	{
		pLayer = m_arrPFtrLayer.GetAt(i);
		if( pLayer->IsDeleted() )continue;

		if( _tcsicmp(pLayer->GetName(),layname)==0 )
		{
			if( mapname==NULL )
			{
				m_nCurLayer = i;
				m_arrUsedFtrlayer.InsertAt(0,m_nCurLayer);
				if (m_arrUsedFtrlayer.GetSize() > 10)
				{
					m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
				}
				return TRUE;
			}
			else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
			{
				m_nCurLayer = i;
				m_arrUsedFtrlayer.InsertAt(0,m_nCurLayer);
				if (m_arrUsedFtrlayer.GetSize() > 10)
				{
					m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
				}
				return TRUE;
			}
		}
	}
	return FALSE;
}
*/

BOOL CDataSourceEx::SetCurFtrLayer(int layid)
{	
	int nsz = m_arrPFtrLayer.GetSize();
	CFtrLayer *pLayer;
	
	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer = m_arrPFtrLayer.GetAt(index);
		if( pLayer->IsDeleted() )continue;
		
		if( pLayer->GetID()==layid )
		{			
			m_nCurLayer = index;
			m_arrUsedFtrlayer.RemoveAt(i);
			m_arrUsedFtrlayer.InsertAt(0,m_nCurLayer);
			return TRUE;			
		}
	}

	for (i=0; i<nsz; i++)
	{
		pLayer = m_arrPFtrLayer.GetAt(i);
		if( pLayer->IsDeleted() )continue;

		if( pLayer->GetID()==layid )
		{			
			m_nCurLayer = i;
			m_arrUsedFtrlayer.InsertAt(0,m_nCurLayer);
			if (m_arrUsedFtrlayer.GetSize() > 10)
			{
				m_arrUsedFtrlayer.RemoveAt(10,m_arrUsedFtrlayer.GetSize()-10);
			}
			return TRUE;			
		}
	}
	return FALSE;
}


BOOL CDataSourceEx::SetDefaultXAttributes(CFeature *pFtr)
{
	if(!m_pAttrSource) return FALSE;

	CConfigLibManager * pConfig = GetConfigLibManager();
	if(!pConfig) return FALSE;
	CScheme *pScheme = pConfig->GetScheme(m_nScale);
	if(!pScheme)return FALSE;
	CFtrLayer *pFtrLayer = GetFtrLayerOfObject(pFtr);
	if (!pFtrLayer)  return FALSE;
	CSchemeLayerDefine *pLayerDef = pScheme->GetLayerDefine(pFtrLayer->GetName());
	if(!pLayerDef) return FALSE;

	CValueTable tab;
	tab.BeginAddValueItem();
	pLayerDef->GetXDefaultValues(tab);
	tab.EndAddValueItem();	

	m_pAttrSource->SetXAttributes(pFtr,tab);
	return TRUE;
}


//DEL void CDataSourceEx::SetFixedFtrLayers()
//DEL {
//DEL 	CConfigLibManager *pConfig = ((CFeatureBaseApp*)AfxGetApp())->GetConfigLibManager();
//DEL 	if(!pConfig)return;
//DEL 	CScheme *pScheme = pConfig->GetScheme(m_nScale);
//DEL 	if(!pScheme) return;	
//DEL 	CFtrLayer *pLayer;
//DEL 	for (int i=0;i<pScheme->GetLayerDefineCount();i++)
//DEL 	{
//DEL 		CSchemeLayerDefine *pSLD = pScheme->GetLayerDefine(i);		
//DEL 		pLayer = GetFtrLayer(pSLD->GetLayerName());
//DEL 		if(pLayer)
//DEL 			pLayer->SetFixedFlag(TRUE);
//DEL 	}
//DEL }


CFeature* CDataSourceEx::CreateObjByNameOrLayerIdx(LPCTSTR name)
{
	__int64 code = _ttoi64(name);
	if( code>0 )
	{
		CConfigLibManager * pConfi = GetConfigLibManager();
		if(!pConfi) return NULL;
		CScheme *pScheme = pConfi->GetScheme(m_nScale);
		if(!pScheme) return NULL;
		CString name2 = name;
		pScheme->FindLayerIdx(TRUE, code, name2);
	}
	CFtrLayer* pLayer = GetLocalFtrLayer(name);
	if(!pLayer) return NULL;
	return pLayer->CreateDefaultFeature(m_nScale);	
}

CFtrLayer* CDataSourceEx::CreateFtrLayer(LPCTSTR layname)
{
	CFtrLayer *pFtrLayer = new CFtrLayer;
	if (!pFtrLayer) return NULL;
	pFtrLayer->SetName(layname);
	return pFtrLayer;
}

int CDataSourceEx::GetMaxDisplayOrder()
{
	int count = GetFtrLayerCount();
	
	int nMax = -1;
	
	for (int i=0; i<count; i++)
	{
		CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
		if (!pFtrLayer) continue;

		int order = pFtrLayer->GetDisplayOrder();
		if (order > nMax)
			nMax = order;
	}
	
	return nMax;
}

BOOL CDataSourceEx::AddFtrLayer(CFtrLayer* pLayer, BOOL bForLoad)
{
	if( GetFtrLayer(pLayer->GetID()) )
		return FALSE;

	if (pLayer->GetID() == 0)
	{
		int nMax = GetMaxLayerId();
		if (nMax == -1)
			pLayer->SetID(1);
		else
			pLayer->SetID(nMax + 1);
	}

	if (pLayer->GetDisplayOrder() < 0)
	{
		int nOrder = GetMaxDisplayOrder();
		if (nOrder >= 0)
		{
			pLayer->SetDisplayOrder(nOrder+1);
		}
	}

	m_arrPFtrLayer.Add(pLayer);

	if( bForLoad )return TRUE;

	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}

	SetModifiedFlag(TRUE);
	m_bDisplayOrderModified = TRUE;
	return TRUE;
}

void CDataSourceEx::DelFtrLayer(CFtrLayer* pLayer)
{
	if (!pLayer)
	{
		return;
	}
	int nsz = m_arrPFtrLayer.GetSize();
	CFtrLayer *pLayer2;
	BOOL bChangeCurLayer = FALSE;
	BOOL bFind = FALSE;
	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer2 = m_arrPFtrLayer.GetAt(index);
		if( pLayer2==pLayer )
		{
			pLayer2->SetDeleted(TRUE);
			m_arrUsedFtrlayer.RemoveAt(i);
			if (m_nCurLayer==index)
			{
				bChangeCurLayer = TRUE;	
			}
			bFind = TRUE;
			break;
		}
		
	}

	if (!bFind)
	{
		for (i=0; i<nsz; i++)
		{
			pLayer2 = m_arrPFtrLayer.GetAt(i);
			if( pLayer2==pLayer )
			{
				pLayer2->SetDeleted(TRUE);
				if (m_nCurLayer==i)
				{
					bChangeCurLayer = TRUE;	
				}
				break;
			}
		}
	}
	////重新设定当前层
	if (bChangeCurLayer)
	{
		if (m_arrUsedFtrlayer.GetSize()>0)
		{
			m_nCurLayer = m_arrUsedFtrlayer.GetAt(0);
		}
		else
		{
			for (int i=0;i<m_arrPFtrLayer.GetSize();i++)
			{
				pLayer2 = m_arrPFtrLayer.GetAt(i);
				if (pLayer2&&!pLayer2->IsDeleted())
				{
					m_nCurLayer = i;
					break;
				}
			}
		}
	}
	
	if( m_pAccess )
	{
		m_pAccess->DelFtrLayer(pLayer);
	}
	SetModifiedFlag(TRUE);
	m_bDisplayOrderModified = TRUE;
}

void CDataSourceEx::DelFtrLayer(int layid)
{
	int nsz = m_arrPFtrLayer.GetSize();
	CFtrLayer *pLayer;
	
	BOOL bChangeCurLayer = FALSE;
	BOOL nFind = FALSE;
	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer = m_arrPFtrLayer.GetAt(index);
		if (pLayer->GetID() == layid)
		{
			
			pLayer->SetDeleted(TRUE);
			m_arrUsedFtrlayer.RemoveAt(i);
			if(m_nCurLayer == index)
			{
				bChangeCurLayer = TRUE;
			}
			nFind = TRUE;
			break;
			
		}
	}
	
	if (!nFind)
	{
		for (i=0; i<nsz; i++)
		{
			pLayer = m_arrPFtrLayer.GetAt(i);
			if (pLayer->GetID() == layid)
			{
				pLayer->SetDeleted(TRUE);
				if (m_nCurLayer==i)
				{
					bChangeCurLayer = TRUE;
				}
				break;
			}
		}
	}
	//重新设定当前层
	if (bChangeCurLayer)
	{
		if (m_arrUsedFtrlayer.GetSize()>0)
		{
			m_nCurLayer = m_arrUsedFtrlayer.GetAt(0);
		}
		else
		{
			for (int i=0;i<m_arrPFtrLayer.GetSize();i++)
			{
				pLayer = m_arrPFtrLayer.GetAt(i);
				if (pLayer&&!pLayer->IsDeleted())
				{
					m_nCurLayer = i;
					break;
				}
			}
		}
	}
	
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}
	SetModifiedFlag(TRUE);
	m_bDisplayOrderModified = TRUE;
	return;
}
/*
void CDataSourceEx::DelFtrLayer(LPCTSTR layname, LPCTSTR mapname)
{
	if( layname==NULL )
		return;
	
	int nsz = m_arrPFtrLayer.GetSize();
	CFtrLayer *pLayer;

	BOOL bChangeCurLayer = FALSE;
	BOOL nFind = FALSE;
	for (int i=0; i<m_arrUsedFtrlayer.GetSize(); i++)
	{
		int index = m_arrUsedFtrlayer.GetAt(i);
		pLayer = m_arrPFtrLayer.GetAt(index);
		if( _tcsicmp(pLayer->GetName(),layname)==0 )
		{
			if( mapname==NULL )
			{
				pLayer->SetDeleted(TRUE);
				m_arrUsedFtrlayer.RemoveAt(i);
				if (m_nCurLayer==index)
				{
					bChangeCurLayer = TRUE;
				}
				nFind = TRUE;
				break;
			}
			else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
			{
				pLayer->SetDeleted(TRUE);
				m_arrUsedFtrlayer.RemoveAt(i);
				if (m_nCurLayer==index)
				{
					bChangeCurLayer = TRUE;
				}
				nFind = TRUE;
				break;
			}
		}
	}
	
	if (!nFind)
	{
		for (i=0; i<nsz; i++)
		{
			pLayer = m_arrPFtrLayer.GetAt(i);
			if( _tcsicmp(pLayer->GetName(),layname)==0 )
			{
				if( mapname==NULL )
				{
					pLayer->SetDeleted(TRUE);
					if (m_nCurLayer==i)
					{
						bChangeCurLayer = TRUE;
					}
					break;
				}
				else if( _tcsicmp(pLayer->GetMapName(),mapname)==0 )
				{
					pLayer->SetDeleted(TRUE);
					if (m_nCurLayer==i)
					{
						bChangeCurLayer = TRUE;
					}
					break;
				}
			}
		}
	}
	//重新设定当前层
	if (bChangeCurLayer)
	{
		if (m_arrUsedFtrlayer.GetSize()>0)
		{
			m_nCurLayer = m_arrUsedFtrlayer.GetAt(0);
		}
		else
		{
			for (int i=0;i<m_arrPFtrLayer.GetSize();i++)
			{
				pLayer = m_arrPFtrLayer.GetAt(i);
				if (pLayer&&!pLayer->IsDeleted())
				{
					m_nCurLayer = i;
					break;
				}
			}
		}
	}

	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}
	m_bModified = TRUE;
	return;
}
*/

void CDataSourceEx::RestoreLayer(CFtrLayer* pLayer)
{
	int nsz = m_arrPFtrLayer.GetSize();
	CFtrLayer *pLayer2;
	
	for( int i=0; i<nsz; i++)
	{
		pLayer2 = m_arrPFtrLayer.GetAt(i);
		if( pLayer2==pLayer )
		{
			pLayer2->SetDeleted(FALSE);
			break;
		}
	}
	
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}
	SetModifiedFlag(TRUE);
	m_bDisplayOrderModified = TRUE;
}

void CDataSourceEx::SaveAllLayers()
{
	if (m_pAccess)
	{
//		m_pAccess->BatchUpdateBegin();
		
		int nsz = m_arrPFtrLayer.GetSize();
		
		for (int i=0; i<nsz; i++)
		{
			CFtrLayer *pLayer = GetFtrLayerByIndex(i);
			if (!pLayer) continue;
			
			m_pAccess->SaveFtrLayer(pLayer);
			
		}

//		m_pAccess->BatchUpdateEnd();

		SetModifiedFlag(TRUE);
	}

}

void CDataSourceEx::SaveLayer(CFtrLayer *pLayer)
{
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}
	SetModifiedFlag(TRUE);
}


void CDataSourceEx::SaveFeature(CFeature *pFtr)
{
	if( m_pAccess )
	{
		m_pAccess->SaveFeature(pFtr);
	}
	SetModifiedFlag(TRUE);
}

void CDataSourceEx::ResetDisplayOrder()
{
	return;
	CFtrLayerArray arr[5];

	CScheme *pScheme = GetConfigLibManager()->GetScheme(GetScale());
	if (!pScheme)  return;

	int nsz = m_arrPFtrLayer.GetSize();

	for (int i=0; i<nsz; i++)
	{		
		CFtrLayer *pLayer = m_arrPFtrLayer[i];
		if (!pLayer)  
		{
			continue;
		}

		CString strLayerName = pLayer->GetName();
		
		CSchemeLayerDefine *pItem = pScheme->GetLayerDefine(strLayerName);
		if (!pItem)
		{
			arr[4].Add(pLayer);
			continue; 
		}
		
		int nGeoClass = pItem->GetGeoClass();

		if (nGeoClass == CLS_GEOSURFACE)
		{
			arr[0].Add(pLayer);
		}
		else if (nGeoClass == CLS_GEOCURVE || nGeoClass == CLS_GEOPARALLEL)
		{
			arr[1].Add(pLayer);
		}
		else if (nGeoClass == CLS_GEOPOINT || nGeoClass == CLS_GEODIRPOINT || nGeoClass == CLS_GEOSURFACEPOINT)
		{
			arr[2].Add(pLayer);
		}
		else if (nGeoClass == CLS_GEOTEXT)
		{
			arr[3].Add(pLayer);
		}
		else
		{
			arr[4].Add(pLayer);
		}

	}

	int order = 0;
	for (i=0; i<5; i++)
	{
		CFtrLayerArray *arrLayers = &arr[i];
		if (!arrLayers)  continue;

		for (int j=0; j<arrLayers->GetSize(); j++)
		{
			CFtrLayer *pLayer = arrLayers->GetAt(j);
			if (!pLayer) continue;

			pLayer->SetDisplayOrder(order);
			order++;
		}
	}
	
}

void CDataSourceEx::SetBound(const PT_3D *pts ,double zmin, double zmax)
{
	memcpy(m_bounds,pts,sizeof(m_bounds));
	m_lfZmin = zmin;
	m_lfZmax = zmax;
	if( m_pAccess )
		m_pAccess->WriteDataSourceInfo(this);
	SetModifiedFlag(TRUE);
}

void CDataSourceEx::SetScale(DWORD scale)
{
	m_nScale = scale;
	if( m_pAccess )
		m_pAccess->WriteDataSourceInfo(this);
	if(m_pAttrSource)
	{
		m_pAttrSource->SetScale(scale);
	}
	SetModifiedFlag(TRUE);

//	WriteProfileDouble(REGPATH_SYMBOL,REGITEM_DOCDRAWSCALE,(double)m_nScale/1000);
}

BOOL CDataSourceEx::WriteTo(CValueTable& tab)const
{
	tab.AddValue(FIELDNAME_SCALE,&CVariantEx((_variant_t)(long)m_nScale));

	CArray<PT_3DEX,PT_3DEX> arrPts;
	arrPts.SetSize(4);

	PT_3DEX expt;
	expt.pencode = penNone;

	COPY_3DPT(expt,m_bounds[0]);
	arrPts.SetAt(0,expt);

	COPY_3DPT(expt,m_bounds[1]);
	arrPts.SetAt(1,expt);

	COPY_3DPT(expt,m_bounds[2]);
	arrPts.SetAt(2,expt);
	
	COPY_3DPT(expt,m_bounds[3]);
	arrPts.SetAt(3,expt);

	CVariantEx var;
	var.SetAsShape(arrPts);

	tab.AddValue(FIELDNAME_BOUND,&var);

	tab.AddValue(FIELDNAME_ZMIN,&CVariantEx((_variant_t)(double)m_lfZmin));

	tab.AddValue(FIELDNAME_ZMAX,&CVariantEx((_variant_t)(double)m_lfZmax));
	return TRUE;
}


BOOL CDataSourceEx::ReadFrom(CValueTable& tab)
{
	const CVariantEx *var;
	if( tab.GetValue(0, FIELDNAME_SCALE, var) )
	{
		m_nScale = (long)(_variant_t)*var;
	}
	if( tab.GetValue(0, FIELDNAME_BOUND, var) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		if( var->GetShape(arrPts) && arrPts.GetSize()==4 )
		{
			m_bounds[0] = arrPts[0];
			m_bounds[1] = arrPts[1];
			m_bounds[2] = arrPts[2];
			m_bounds[3] = arrPts[3];
		}
	}
	if( tab.GetValue(0, FIELDNAME_ZMIN, var) )
	{
		m_lfZmin = (double)(_variant_t)*var;
	}
	if( tab.GetValue(0, FIELDNAME_ZMAX, var) )
	{
		m_lfZmax = (double)(_variant_t)*var;
	}
	
//	WriteProfileDouble(REGPATH_SYMBOL,REGITEM_DOCDRAWSCALE,(double)m_nScale/1000);

	return TRUE;
}


BOOL CDataSourceEx::SetAccessObject(CAccessModify *pAccess)
{
	if( m_pAccess && m_pAccess!=pAccess )
		delete m_pAccess;
	m_pAccess = pAccess;
	if (m_pAttrSource)
	{
		m_pAttrSource->SetAccessObject(pAccess);
	}
	return TRUE;
}


BOOL CDataSourceEx::SaveAll(CAccessWrite *pAccess)
{
	if( pAccess==NULL )
		pAccess = m_pAccess;
	
	if( pAccess==NULL )
		return FALSE;
	
	pAccess->WriteDataSourceInfo(this);

	pAccess->WriteSpecialData(this);

	int i, j;
	int nlay = GetFtrLayerCount();
	
	CFtrLayer *pLayer;
	for( i=0; i<nlay; i++)
	{
		pLayer = GetFtrLayerByIndex(i);
		if (!pLayer)  continue;

		pAccess->SaveFtrLayer(pLayer);
	}
	
	CFeature *pFt;
	for( i=0; i<nlay; i++)
	{
		pLayer = GetFtrLayerByIndex(i);

		if (!pLayer)  continue;
		
		int nobj = pLayer->GetObjectCount();
		for( j=0; j<nobj; j++)
		{
			pFt = pLayer->GetObject(j);
			if( !pFt )continue;
			
			pAccess->SetCurFtrLayID(pLayer->GetID());
			pAccess->SaveFeature(pFt);
		}

	}

	return TRUE;
}

int CDataSourceEx::GetMaxLayerId()
{
	int nMax = -1;
	if(m_arrPFtrLayer.GetSize() > 0)
		nMax = m_arrPFtrLayer[0]->GetID();
	for(int i=1; i<m_arrPFtrLayer.GetSize(); i++)
	{
		if(m_arrPFtrLayer[i]->GetID() > nMax)
			nMax = m_arrPFtrLayer[i]->GetID();
	}
	return nMax;
}

void CDataSourceEx::LoadLayerSymbolCache()
{
	CScheme *pScheme = GetConfigLibManager()->GetScheme(GetScale());
	if (!pScheme)  return;

	for (int i=0; i<GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
		if (!pFtrLayer) continue;
		CString strLayerName = pFtrLayer->GetName();
		
		CSchemeLayerDefine *define = pScheme->GetLayerDefine(strLayerName);
		
		if (!define) continue;
		
		CPtrArray arr;
		for (int i=0; i<define->GetSymbolCount(); i++)
		{
			CSymbol *pSymbol = define->GetSymbol(i);
			arr.Add(pSymbol);
		}
		pFtrLayer->SetSymbols(arr);
	}	
	
}

void CDataSourceEx::ClearLayerSymbolCache()
{
	for (int i=0; i<GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
		if (!pFtrLayer) continue;
		pFtrLayer->SetSymbols(CPtrArray());
	}
}

void CDataSourceEx::GetAllField(BOOL bIncludeExtraFields, CStringArray &strField,CStringArray &strName)
{
//	strField.RemoveAll();
//	strName.RemoveAll();
	
	//基本属性
	CValueTable table;
	CFeature feature;

	table.BeginAddValueItem();
	feature.CreateGeometry(CLS_GEOPOINT);
	feature.WriteTo(table);

	feature.CreateGeometry(CLS_GEODIRPOINT);
	feature.WriteTo(table);

	feature.CreateGeometry(CLS_GEOCURVE);
	feature.WriteTo(table);

	feature.CreateGeometry(CLS_GEOPARALLEL);
	feature.WriteTo(table);

	feature.CreateGeometry(CLS_GEOSURFACE);
	feature.WriteTo(table);

	feature.CreateGeometry(CLS_GEOTEXT);
	feature.WriteTo(table);

	table.EndAddValueItem();

	for (int i=0; i<table.GetFieldCount(); i++)
	{	
		CString field,name;
		int type;
		table.GetField(i,field,type,name);
		
		strField.Add(field);
		strName.Add(name);
	}

	//扩展属性
	CScheme *pScheme = GetConfigLibManager()->GetScheme(m_nScale);
	if (pScheme && bIncludeExtraFields )
	{
		for (i=0; i<pScheme->GetLayerDefineCount(); i++)
		{
			CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(i);

			if (!pLayer)  continue;

			int size = 0;
			const XDefine *pDefine = pLayer->GetXDefines(size);
			for (int j=0; j<size; j++,pDefine++)
			{
				strField.Add(pDefine->field);
				strName.Add(pDefine->name);
			}
		}

		if( m_pAccess )
		{
			CStringArray fields, types;
			m_pAccess->GetOptAttributeDef(CLS_GEOPOINT,fields,types);
			strField.Append(fields);
			strName.Append(fields);

			m_pAccess->GetOptAttributeDef(CLS_GEOCURVE,fields,types);
			strField.Append(fields);
			strName.Append(fields);
			
			m_pAccess->GetOptAttributeDef(CLS_GEOSURFACE,fields,types);
			strField.Append(fields);
			strName.Append(fields);
			
			m_pAccess->GetOptAttributeDef(CLS_GEOTEXT,fields,types);
			strField.Append(fields);
			strName.Append(fields);			
		}
		
	}

	//整理属性
	for (i=0; i<strField.GetSize(); i++)
	{
		//去掉不需要的属性
		CString field = strField.GetAt(i);
		if (field.CompareNoCase(FIELDNAME_FTRDELETED)==0 || field.CompareNoCase(FIELDNAME_FTRID)==0 ||
				field.CompareNoCase(FIELDNAME_SHAPE)==0 || field.CompareNoCase(FIELDNAME_CLSTYPE)==0 )
		{
			strField.RemoveAt(i);
			strName.RemoveAt(i);
			i--;
			continue;
		}
		//去掉相同的属性
		for (int j=0; j<strField.GetSize(); j++)
		{
			if (j!=i && field.CompareNoCase(strField.GetAt(j))==0)
			{
				strField.RemoveAt(j);
				strName.RemoveAt(j);
				j--;
			}
			
		}
	}

	
}

BOOL CDataSourceEx::IsFeatureValid(CFeature *pFtr)
{
	if (pFtr == NULL) return FALSE;
	
	CGeometry *pGeo = pFtr->GetGeometry();
	if (pGeo == NULL) return FALSE;
	
	int num = pGeo->GetDataPointSum();
	
	if (num < 1 || (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) && num < 2) )
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CDataSourceEx::LoadAll(CAccessRead *pAccess, BOOL bShowProgress)
{
	if( pAccess==NULL )
		pAccess = m_pAccess;

	if( pAccess==NULL )
		return FALSE;

//	m_strName = pAccess->GetReadName();

	pAccess->ReadSpecialData(this);

	long idx;
	int lSum = pAccess->GetFtrLayerCount();
	lSum += pAccess->GetFeatureCount()*2;
	
	if( bShowProgress )
	{
		GOutPut(StrFromResID(IDS_LOAD_FEATURES));
		GProgressStart(lSum);	
	}
	
	CFtrLayer *pLayer = pAccess->GetFirstFtrLayer(idx);
	while( pLayer )
	{
		if( bShowProgress )
			GProgressStep();

		if( !AddFtrLayer(pLayer,TRUE) )
			delete pLayer;
		
		CString strGroupName = pLayer->GetGroupName();
		if (!strGroupName.IsEmpty())
		{
			FtrLayerGroup *pFtrLayerGroup = GetFtrLayerGroupByName(pLayer->GetGroupName());
			if (pFtrLayerGroup)
			{
				pFtrLayerGroup->AddFtrLayer(pLayer);
			}
			else
			{
				FtrLayerGroup *pNewGroup = new FtrLayerGroup;
				if (pNewGroup)
				{
					pNewGroup->Name = pLayer->GetGroupName();
					pNewGroup->AddFtrLayer(pLayer);
					AddFtrLayerGroup(pNewGroup,TRUE);
					SetModifiedFlag(TRUE);
				}
			}
		}

		pLayer = pAccess->GetNextFtrLayer(idx);	

	}

	LoadLayerSymbolCache();

	int id;
	CFeature *pFt = pAccess->GetFirstFeature(idx);

	//试验效果：先读取所有Feature再添加到DataSource，比读一个加一个要快20%；
	CArray<CPFeature,CPFeature> arrFeatures;
	CArray<int,int> ids;

	while (pFt)
	{
		if (bShowProgress)
			GProgressStep();

		id = pAccess->GetCurFtrLayID();

		if (IsFeatureValid(pFt))
		{
			arrFeatures.Add(pFt);
			ids.Add(id);
		}
		else
		{
			delete pFt;
		}

		pFt = pAccess->GetNextFeature(idx);
	}
    
	int num = arrFeatures.GetSize();
	for( int i=0; i<num; i++)
	{
		if( bShowProgress )
			GProgressStep();

		pFt = arrFeatures[i];
		id = ids[i];

		if (!pAccess->IsFileVersionLatest())
		{
			int nOrder = GetFtrsMaxMinDisplayOrder(TRUE);
			pFt->SetDisplayOrder(nOrder+1);
		}

		if( SetCurFtrLayer(id) && AddObject(pFt,ids[i],TRUE) );
		else
		{
			delete pFt;
			continue;
		}

		CUIntArray index;
		pFt->GetObjectGroup(index);
		int size = index.GetSize();
		for (int i=0; i<size; i++)
		{
			for (int j=0; j<m_arrFtrGroup.GetSize(); j++)
			{
				if (index[i] == m_arrFtrGroup[j]->id)
				{
					m_arrFtrGroup[j]->AddObject(pFt);
				}
			}
			
		}
	}

	if( bShowProgress )
	{
		GProgressEnd();
		GOutPut(StrFromResID(IDS_LOAD_FINISH));
	}
	
	return TRUE;
}

BOOL CoverGeometry(CGeometry *pGeo, GrBuffer *pBuf)
{
	int nCoverType = 0;
	float fCoverExtend = 0;
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		nCoverType = ((CGeoPoint*)pGeo)->m_nCoverType;
		fCoverExtend = ((CGeoPoint*)pGeo)->m_fExtendDis;
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) )
	{
		nCoverType = ((CGeoText*)pGeo)->m_nCoverType;
		fCoverExtend = ((CGeoText*)pGeo)->m_fExtendDis;
	}
	
	if( nCoverType!=0 && fCoverExtend>=0.0f )
	{
		GrBuffer buf0;
		
		Envelope e_buf = pBuf->GetEnvelope();
		
		e_buf.Inflate(fCoverExtend,fCoverExtend,0);
		
		CreateCoverPolygon(nCoverType,e_buf,PT_3D(e_buf.CenterX(),e_buf.CenterY(),e_buf.CenterZ()),&buf0);
		
		buf0.AddBuffer(pBuf);
		pBuf->CopyFrom(&buf0);

		return TRUE;
	}

	return FALSE;
}


BOOL CDataSourceEx::DrawFeature(CFeature *pFtr, GrBuffer *pBuf, BOOL bText, float angle, LPCTSTR strLayerNameSpecified)
{
	SetDataSourceScale(m_nScale);

	double fDrawScale = GetSymbolDrawScale();

	GrBuffer buf;
	CPtrArray arr;
	GetConfigLibManager()->GetSymbol(this,pFtr,arr,strLayerNameSpecified);

	if (arr.GetSize() < 1) 
	{
		if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			pFtr->Draw(pBuf, fDrawScale*GetSymbolAnnoScale());
		}
		else
		{
			pFtr->Draw(pBuf,fDrawScale);
		}
		CoverGeometry(pFtr->GetGeometry(),pBuf);
		return FALSE;
	}
	else
	{
		int num = arr.GetSize();
		CGeometry *pGeo = pFtr->GetGeometry();

// 		BOOL bDrawAidLine = FALSE;
// 		// 判断是否需要自动渲染辅助线
// 		if ((pFtr->GetPurpose()&FTR_COLLECT) && (pGeo->GetClassType() == CLS_GEODCURVE || pGeo->GetClassType() == CLS_GEOPARALLEL))
// 		{			
// 			for (int k=0; k<num; k++)
// 			{
// 				CSymbol* pSym = (CSymbol*)arr[k];
// 				if (pSym->GetType() == SYMTYPE_CELL && ((CCell*)pSym)->m_nFillType == CCell::AidLine)
// 				{
// 					break;
// 				}
// 				else if (pSym->GetType() == SYMTYPE_DASHLINETYPE && ((CDashLinetype*)pSym)->m_nPlaceType == CDashLinetype::AidLine)
// 				{
// 					break;
// 				}
// 				else if (pSym->GetType() == SYMTYPE_CELLLINETYPE && ((CCellLinetype*)pSym)->m_nPlaceType == CCellLinetype::AidLine)
// 				{
// 					break;
// 				}
// 			}
// 
// 			if (k > num-1)
// 			{
// 				bDrawAidLine = TRUE;
// 			}
// 		}
		
		// 是否进行了符号化
		BOOL bDrawSym = FALSE, bSymValid = FALSE;
		CValueTable tab;
		for (int k=0; k<num; k++)
		{			
			buf.DeleteAll();

			CSymbol* pSym = (CSymbol*)arr[k];
			double fDrawScale1 = fDrawScale;
			if (!bText)
			{
				if (pSym->GetType()==SYMTYPE_ANNOTATION)
					continue;	
			}
			else
			{
				if (pSym->GetType()==SYMTYPE_ANNOTATION)
					fDrawScale1 *= GetSymbolAnnoScale();
			}

			
			if (pSym->IsNeedAttrWhenDraw())
			{
				if(tab.GetFieldCount()<=0)
					GetAllAttribute(pFtr,tab);
				pSym->Draw(pFtr,&buf,tab,fDrawScale1,angle,1);
				bDrawSym = TRUE;				
			}
			else
			{
				pSym->Draw(pFtr,&buf,fDrawScale1,0,1);
				bDrawSym = TRUE;
			}			

			if(buf.HeadGraph()!=NULL)
				bSymValid = TRUE;
				
			pBuf->AddBuffer(&buf);
		}

		int nClassType = pGeo->GetClassType();

		if (!bDrawSym || (!bSymValid && (nClassType == CLS_GEOSURFACEPOINT || nClassType==CLS_GEOPOINT || nClassType==CLS_GEODIRPOINT ||nClassType==CLS_GEOTEXT)))
		{
			pFtr->Draw(pBuf,fDrawScale);
		}

		//压盖统一处理		
		CoverGeometry(pGeo,pBuf);
		
		// 清理
		for (k=0; k<num; k++)
		{
			CSymbol* pSym = (CSymbol*)arr[k];
			if (pSym)
			{
				delete pSym;
			}
		}
		arr.RemoveAll();

		return TRUE;
	}
}


BOOL CDataSourceEx::DrawFeature_SymbolizePart(CFeature *pFtr, GrBuffer *pBuf, BOOL bText, float angle, LPCTSTR strLayerNameSpecified)
{
	SetDataSourceScale(m_nScale);
	
	double fDrawScale = GetSymbolDrawScale();
	
	GrBuffer buf;
	CPtrArray arr;
	GetConfigLibManager()->GetSymbol(this,pFtr,arr,strLayerNameSpecified);
	
	if (arr.GetSize() < 1) 
	{
		if(pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			pFtr->Draw(pBuf, fDrawScale*GetSymbolAnnoScale());
		}
		else
		{
			pFtr->Draw(pBuf,fDrawScale);
		}
		
		CoverGeometry(pFtr->GetGeometry(),pBuf);
		return FALSE;
	}
	else
	{
		int num = arr.GetSize();
		CGeometry *pGeo = pFtr->GetGeometry();

		//是否只是点符号、虚线符号、图元线型符号，如果是的话，就可以不符号化，否则就符号化
		BOOL bSymbolized = FALSE;
		for (int k=0; k<num; k++)
		{
			CSymbol *pSym = (CSymbol*)arr[k];
			int type = pSym->GetType();
			if( type==SYMTYPE_CELL || type==SYMTYPE_ANNOTATION || type==SYMTYPE_DASHLINETYPE ||
				type==SYMTYPE_CELLLINETYPE || type==SYMTYPE_ANGBISECTORTYPE )
				continue;
			else
			{
				bSymbolized = TRUE;
				break;
			}
		}

		if( !bSymbolized )
		{
			pFtr->Draw(pBuf,fDrawScale);	
			CoverGeometry(pGeo,pBuf);

			for (k=0; k<num; k++)
			{
				CSymbol *pSym = (CSymbol*)arr[k];
				if (pSym)
				{
					delete pSym;
				}
			}
			arr.RemoveAll();

			return FALSE;
		}
		
// 		BOOL bDrawAidLine = FALSE;
// 		// 判断是否需要自动渲染辅助线
// 		if ((pFtr->GetPurpose()&FTR_COLLECT) && (pGeo->GetClassType() == CLS_GEODCURVE || pGeo->GetClassType() == CLS_GEOPARALLEL))
// 		{			
// 			for (k=0; k<num; k++)
// 			{
// 				CSymbol *pSym = (CSymbol*)arr[k];
// 				if (pSym->GetType() == SYMTYPE_CELL && ((CCell*)pSym)->m_nFillType == CCell::AidLine)
// 				{
// 					break;
// 				}
// 				else if (pSym->GetType() == SYMTYPE_DASHLINETYPE && ((CDashLinetype*)pSym)->m_nPlaceType == CDashLinetype::AidLine)
// 				{
// 					break;
// 				}
// 				else if (pSym->GetType() == SYMTYPE_CELLLINETYPE && ((CCellLinetype*)pSym)->m_nPlaceType == CCellLinetype::AidLine)
// 				{
// 					break;
// 				}
// 			}
// 			
// 			if (k > num-1)
// 			{
// 				bDrawAidLine = TRUE;
// 			}
// 		}
		
		// 是否进行了符号化
		BOOL bDrawSym = FALSE;
		CValueTable tab;
		for (k=0; k<num; k++)
		{			
			buf.DeleteAll();

			CSymbol *pSym = (CSymbol*)arr[k];
			double fDrawScale1 = fDrawScale;
			if (!bText)
			{
				if (pSym->GetType()==SYMTYPE_ANNOTATION)
					continue;	
			}
			else
			{
				if (pSym->GetType()==SYMTYPE_ANNOTATION)
					fDrawScale1 *= GetSymbolAnnoScale();
			}
			
			if (pSym->IsNeedAttrWhenDraw())
			{
				if(tab.GetFieldCount()<=0)
					GetAllAttribute(pFtr,tab);

				pSym->Draw(pFtr,&buf,tab,fDrawScale,angle,1);
				bDrawSym = TRUE;				
			}
			else
			{
				pSym->Draw(pFtr,&buf,fDrawScale,0,1);
				bDrawSym = TRUE;
			}			
			
			pBuf->AddBuffer(&buf);
		}
		
		if (!bDrawSym || (pBuf->HeadGraph()==NULL && pGeo->GetClassType() == CLS_GEOSURFACEPOINT))
		{
			pFtr->Draw(pBuf,fDrawScale);
		}

		CoverGeometry(pGeo,pBuf);
		
		// 清理
		for (k=0; k<num; k++)
		{
			CSymbol *pSym = (CSymbol*)arr[k];
			if (pSym)
			{
				delete pSym;
			}
		}
		arr.RemoveAll();
		
		return TRUE;
	}
}

//按照最恰当的方式打散符号
BOOL CDataSourceEx::ExplodeSymbols(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs, LPCTSTR strLayerNameSpecified)
{
	if( !pFtr || !pLayer )return FALSE;
	
	CPtrArray arrPSyms;
	GetConfigLibManager()->GetSymbol(this,pFtr,arrPSyms,strLayerNameSpecified);

	CDataSourceEx *pDS = this;
	double fDrawScale = ((double)pDS->GetScale()/1000)*pDS->GetDrawScaleByUser();

	//无符号时，只有双线对象能打散
	if( arrPSyms.GetSize()<1 )
	{
		CGeometry *pGeo = pFtr->GetGeometry(), *pGeo1, *pGeo2;
		if( ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && ((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2) ) || ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) && ((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2) ))
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
			if( pFtrNew )
			{
				pFtrNew->SetID(OUID());
				pFtrNew->SetGeometry(pGeo1);	
				
				arrPFtrs.Add(pFtrNew);

				CFeature *pFtrNew1 = pFtrNew->Clone();
				if (pFtrNew1)
				{					
					pFtrNew1->SetID(OUID());
					pFtrNew1->SetGeometry(pGeo2);	
					
					arrPFtrs.Add(pFtrNew1);
				}

				return TRUE;
			}
		}	
		
		return FALSE;

		if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
			if( pText->GetText().GetLength()>1 )
			{
				GrBuffer buf;
				double fDrawScale1 = fDrawScale*GetSymbolAnnoScale();
				pText->SeparateText(&buf,fDrawScale1);
				
				// 取出点或线片段，生成地物
				CGeoArray arrPGeos;
				ConvertGrBufferToGeos(&buf,fDrawScale1,arrPGeos);
				
				CString strNewSymName = "*";
				
				for (int i=arrPGeos.GetSize()-1; i>=0; i--)
				{
					CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
					if( !pFtrNew )
					{
						delete arrPGeos[i];
						continue;
					}
					
					pFtrNew->SetID(OUID());
					pFtrNew->SetGeometry(arrPGeos[i]);
					
					arrPGeos[i]->SetSymbolName((LPCSTR)strNewSymName);
					
					arrPFtrs.Add(pFtrNew);
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		
	}	

	// GetSymbol 得到的符号对象 都是可以被打散的；不能打散的话，GetSymbol 不会得到符号对象，
	// 所以，这里不需要考虑 CGeometry::m_symname 是什么状况

	CValueTable tab;

	CGeoArray arrPGeos;

	//如果有多个符号，就不打碎图元和基本线型
	BOOL bExplodeSimple = (arrPSyms.GetSize()>1);

	for( int i=0; i<arrPSyms.GetSize(); i++)
	{
		CSymbol *pSym = (CSymbol*)arrPSyms[i];
		double fDrawScale1 = fDrawScale;

		if(pSym->GetType() == SYMTYPE_ANNOTATION)
		{
			fDrawScale1 *= GetSymbolAnnoScale();
		}

		// 若需要扩展属性
		if (pSym->IsNeedAttrWhenDraw())
		{
			if( tab.GetFieldCount()<=0 )
				pDS->GetAllAttribute(pFtr,tab);
			pSym->Explode(pFtr,tab,fDrawScale1,arrPGeos);	
		}
		else
		{
			if( bExplodeSimple )
				pSym->ExplodeSimple(pFtr,fDrawScale1,arrPGeos);
			else
				pSym->Explode(pFtr,fDrawScale1,arrPGeos);
		}

		BOOL bUseSelfColor = GIsSymbolSelfColor(pSym);
		
		for (int i=arrPGeos.GetSize()-1; i>=0; i--)
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
			if( !pFtrNew )
			{
				delete arrPGeos[i];
				continue;
			}
			
			pFtrNew->SetID(OUID());
			pFtrNew->SetGeometry(arrPGeos[i]);
			
			if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER && !bUseSelfColor )
			{
				arrPGeos[i]->SetColor(COLOUR_BYLAYER);
			}
			
			arrPFtrs.Add(pFtrNew);
		}

		arrPGeos.RemoveAll();
	}	
	
	return TRUE;
}


//打散一般符号，将图元填充和晕线填充提取为面地物，使得用户可以对它们单独控制；
BOOL CDataSourceEx::ExplodeSymbolsExceptSurface(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs, LPCTSTR strLayerNameSpecified)
{
	if( !pFtr || !pLayer )return FALSE;

	CPtrArray arrPSyms;
	GetConfigLibManager()->GetSymbol(this,pFtr,arrPSyms,strLayerNameSpecified);
	
	CDataSourceEx *pDS = this;
	double fDrawScale = ((double)pDS->GetScale()/1000)*pDS->GetDrawScaleByUser();

	//无符号时，只有双线对象能打散
	if( arrPSyms.GetSize()<1 )
	{
		CGeometry *pGeo = pFtr->GetGeometry(), *pGeo1, *pGeo2;
		if( ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && ((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2) ) || ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) && ((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2) ))
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
			if( pFtrNew )
			{
				pFtrNew->SetID(OUID());
				pFtrNew->SetGeometry(pGeo1);	
				
				arrPFtrs.Add(pFtrNew);

				CFeature *pFtrNew1 = pFtrNew->Clone();
				if (pFtrNew1)
				{					
					pFtrNew1->SetID(OUID());
					pFtrNew1->SetGeometry(pGeo2);	
					
					arrPFtrs.Add(pFtrNew1);
				}

				return TRUE;
			}
		}	
		
		return FALSE;		
	}	

	// GetSymbol 得到的符号对象 都是可以被打散的；不能打散的话，GetSymbol 不会得到符号对象，
	// 所以，这里不需要考虑 CGeometry::m_symname 是什么状况

	CValueTable tab;

	CGeoArray arrPGeos;

	//如果有多个符号，就不打碎图元和基本线型
	BOOL bExplodeSimple = (arrPSyms.GetSize()>1);

	for( int i=0; i<arrPSyms.GetSize(); i++)
	{
		CSymbol *pSym = (CSymbol*)arrPSyms[i];

		//图元填充或者晕线填充或者颜色填充
		if( pSym->GetType()==SYMTYPE_CELLHATCH || pSym->GetType()==SYMTYPE_LINEHATCH || pSym->GetType()==SYMTYPE_COLORHATCH )
		{
			CGeoSurface *pGeo = (CGeoSurface*)CPermanent::CreatePermanentObject(CLS_GEOSURFACE);
			if( !pGeo )
				continue;

			CArray<PT_3DEX,PT_3DEX> arrPts;
			pFtr->GetGeometry()->GetShape(arrPts);
			pGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

			char symname[256];

			sprintf(symname,"#%d",i);
			pGeo->SetSymbolName(symname);
			arrPGeos.Add(pGeo);			
		}

		BOOL bUseSelfColor = GIsSymbolSelfColor(pSym);
		
		for (int i=arrPGeos.GetSize()-1; i>=0; i--)
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
			if( !pFtrNew )
			{
				delete arrPGeos[i];
				continue;
			}
			
			pFtrNew->SetID(OUID());
			pFtrNew->SetGeometry(arrPGeos[i]);
			
			if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER && !bUseSelfColor )
			{
				arrPGeos[i]->SetColor(COLOUR_BYLAYER);
			}
			
			arrPFtrs.Add(pFtrNew);
		}

		arrPGeos.RemoveAll();
	}	
	
	return TRUE;
}


//按照最恰当的方式打散符号
BOOL CDataSourceEx::ExplodeSymbols(CFeature *pFtr, CPtrArray& arrPSyms, CFtrLayer *pLayer, CFtrArray& arrPFtrs)
{
	if( !pFtr || !pLayer )return FALSE;
	if( arrPSyms.GetSize()<=0 )return FALSE;
	
	CDataSourceEx *pDS = this;
	double fDrawScale = ((double)pDS->GetScale()/1000)*pDS->GetDrawScaleByUser();

	//无符号时，只有双线对象能打散
	if( arrPSyms.GetSize()<1 )
	{
		CGeometry *pGeo = pFtr->GetGeometry(), *pGeo1, *pGeo2;
		if( ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && ((CGeoParallel*)pGeo)->Separate(pGeo1,pGeo2) ) || ( pGeo->IsKindOf(RUNTIME_CLASS(CGeoDCurve)) && ((CGeoDCurve*)pGeo)->Separate(pGeo1,pGeo2) ))
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale(),CLS_GEOCURVE);
			if( pFtrNew )
			{
				pFtrNew->SetID(OUID());
				pFtrNew->SetGeometry(pGeo1);	
				
				arrPFtrs.Add(pFtrNew);

				CFeature *pFtrNew1 = pFtrNew->Clone();
				if (pFtrNew1)
				{					
					pFtrNew1->SetID(OUID());
					pFtrNew1->SetGeometry(pGeo2);	
					
					arrPFtrs.Add(pFtrNew1);
				}

				return TRUE;
			}
		}	
		
		return FALSE;

		if (pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)))
		{
			CGeoText *pText = (CGeoText*)pFtr->GetGeometry();
			if( pText->GetText().GetLength()>1 )
			{
				GrBuffer buf;
				pText->SeparateText(&buf,fDrawScale);
				
				// 取出点或线片段，生成地物
				CGeoArray arrPGeos;
				ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);
				
				CString strNewSymName = "*";
				
				for (int i=arrPGeos.GetSize()-1; i>=0; i--)
				{
					CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
					if( !pFtrNew )
					{
						delete arrPGeos[i];
						continue;
					}
					
					pFtrNew->SetID(OUID());
					pFtrNew->SetGeometry(arrPGeos[i]);
					
					arrPGeos[i]->SetSymbolName((LPCSTR)strNewSymName);
					
					arrPFtrs.Add(pFtrNew);
				}

				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return FALSE;
		}
		
	}	

	// GetSymbol 得到的符号对象 都是可以被打散的；不能打散的话，GetSymbol 不会得到符号对象，
	// 所以，这里不需要考虑 CGeometry::m_symname 是什么状况

	CValueTable tab;

	CGeoArray arrPGeos;

	//如果有多个符号，就不打碎图元和基本线型
	BOOL bExplodeSimple = (arrPSyms.GetSize()>1);

	for( int i=0; i<arrPSyms.GetSize(); i++)
	{
		CSymbol *pSym = (CSymbol*)arrPSyms[i];

		int fDrawScale1 = fDrawScale;
		if(pSym->GetType() == SYMTYPE_ANNOTATION)
		{
			fDrawScale1 *= GetSymbolAnnoScale();
		}

		if (pSym->IsNeedAttrWhenDraw())
		{
			if( tab.GetFieldCount()<=0 )
				pDS->GetAllAttribute(pFtr,tab);
			pSym->Explode(pFtr,tab,fDrawScale1,arrPGeos);	
		}
		else if( bExplodeSimple)
		{
			pSym->ExplodeSimple(pFtr,fDrawScale1,arrPGeos);
		}
		else
		{
			pSym->Explode(pFtr,fDrawScale1,arrPGeos);
		}

		BOOL bUseSelfColor = GIsSymbolSelfColor(pSym);
		
		for (int i=arrPGeos.GetSize()-1; i>=0; i--)
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
			if( !pFtrNew )
			{
				delete arrPGeos[i];
				continue;
			}
			
			pFtrNew->SetID(OUID());
			pFtrNew->SetGeometry(arrPGeos[i]);
			
			if( pFtr->GetGeometry()->GetColor()==COLOUR_BYLAYER && !bUseSelfColor )
			{
				arrPGeos[i]->SetColor(COLOUR_BYLAYER);
			}
			
			arrPFtrs.Add(pFtrNew);
		}

		arrPGeos.RemoveAll();
	}	
	
	return TRUE;
}


//彻底打散打散符号
BOOL CDataSourceEx::ExplodeSymbolsFinally(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs, LPCTSTR strLayerNameSpecified)
{	
	CFtrArray arrInputs, arrOutputs0, arrOutputs1;
	
	arrInputs.Add(pFtr);
	
	while( arrInputs.GetSize()>0 )
	{
		for( int i=0; i<arrInputs.GetSize(); i++)
		{
			arrOutputs1.RemoveAll();
			if( ExplodeSymbols(arrInputs[i],pLayer,arrOutputs1) )
			{
				arrOutputs0.Append(arrOutputs1);
			}
			else
			{
				arrPFtrs.Add(arrInputs[i]);
			}				
		}
		
		arrInputs.Copy(arrOutputs0);
		
		arrOutputs0.RemoveAll();
	}
	
	for( int i=arrPFtrs.GetSize()-1; i>=0; i--)
	{
		if( arrPFtrs[i]==pFtr )
		{
			arrPFtrs.RemoveAt(i);
		}
	}
	
	return (arrPFtrs.GetSize()>0);
}




//彻底打散打散符号
BOOL CDataSourceEx::ExplodeSymbolsFinally(CFeature *pFtr, CPtrArray& arrPSyms, CFtrLayer *pLayer, CFtrArray& arrPFtrs)
{	
	CFtrArray arrInputs, arrOutputs0, arrOutputs1;
	
	arrInputs.Add(pFtr);
	
	while( arrInputs.GetSize()>0 )
	{
		for( int i=0; i<arrInputs.GetSize(); i++)
		{
			arrOutputs1.RemoveAll();
			if( ExplodeSymbols(arrInputs[i],arrPSyms,pLayer,arrOutputs1) )
			{
				arrOutputs0.Append(arrOutputs1);
			}
			else
			{
				arrPFtrs.Add(arrInputs[i]);
			}				
		}
		
		arrInputs.Copy(arrOutputs0);
		
		arrOutputs0.RemoveAll();
	}
	
	for( int i=arrPFtrs.GetSize()-1; i>=0; i--)
	{
		if( arrPFtrs[i]==pFtr )
		{
			arrPFtrs.RemoveAt(i);
		}
	}
	
	return (arrPFtrs.GetSize()>0);
}


//假定只有直线线型；1，正包含，-1，反包含，0，无包含
static int IsObjIncludeObj(CGeometry *pObj1, CGeometry *pObj2)
{
	CArray<PT_3DEX,PT_3DEX> arrPts1, arrPts2;
	pObj1->GetShape(arrPts1);
	pObj2->GetShape(arrPts2);

	PT_3DEX *pts1 = arrPts1.GetData(), *pts2 = arrPts2.GetData();
	int npts1 = arrPts1.GetSize(), npts2 = arrPts2.GetSize();

	Envelope e1, e2;
	e1 = CreateEnvelopeFromPts(pts1,npts1);
	e2 = CreateEnvelopeFromPts(pts2,npts2);

	if( e1.Width()*e1.Height()>e2.Width()*e2.Height() )
	{
		for( int j=0; j<npts2; j++)
		{
			if( GraphAPI::GIsPtInRegion(pts2[j],pts1,npts1)!=2 )
				break;
		}

		if( j>=npts2 )
			return 1;
	}
	else
	{
		for( int j=0; j<npts1; j++)
		{
			if( GraphAPI::GIsPtInRegion(pts1[j],pts2,npts2)!=2 )
				break;
		}
		
		if( j>=npts1 )
			return -1;
	}

	return 0;
}


BOOL CDataSourceEx::ExplodeText(CFeature *pFtr, CFtrLayer *pLayer, CFtrArray& arrPFtrs)
{
	if( !pFtr || !pLayer )return FALSE;
		
	CDataSourceEx *pDS = this;
	double fDrawScale = ((double)pDS->GetScale()/1000)*pDS->GetDrawScaleByUser()*GetSymbolAnnoScale();
	
	if ( !pFtr->GetGeometry()->IsKindOf(RUNTIME_CLASS(CGeoText)) )
		return FALSE;

	CGeoText *pText = (CGeoText*)pFtr->GetGeometry();

	std::wstring wstr0;

#ifdef _UNICODE
	wstr0 = (LPCTSTR)pText->GetText();
#else
	ConvertCharToWstring(pText->GetText(),wstr0,CP_ACP);
#endif

	size_t len  = wcslen(wstr0.c_str());
	
	if( len<=0 )
		return FALSE;

	//首先分离文字对象
	if( len>1 )
	{
		GrBuffer buf;
		pText->SeparateText(&buf,fDrawScale);
		
		// 取出点或线片段，生成地物
		CGeoArray arrPGeos;
		ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);
		
		CString strNewSymName = "*";
		
		for (int i=arrPGeos.GetSize()-1; i>=0; i--)
		{
			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
			if( !pFtrNew )
			{
				delete arrPGeos[i];
				continue;
			}
			
			pFtrNew->SetID(OUID());
			pFtrNew->SetGeometry(arrPGeos[i]);
			
			arrPGeos[i]->SetSymbolName(strNewSymName);
			
			arrPFtrs.Add(pFtrNew);
		}
		
	}
	else
	{
		GrBuffer buf;
		pText->ExplodeAsGrBuffer(&buf,fDrawScale);

		// 取出点或线片段，生成地物
		CGeoArray arrPGeos0;
		ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos0);

		int color = pText->GetColor();
		char symname[256] = {0};

		if( color==COLOUR_BYLAYER )
			strcpy(symname,"%-1%");
		else
			sprintf(symname,"%%%d%%",color);

		//----- 以下处理：将相互包含的线，合并起来生成多面对象；独立的，生成为单面对象

		//用来找到有包含关系的线地物
		CGeoArray arrPGeos1;

		//依次遍历剩余地物
		while( arrPGeos0.GetSize()>0 )
		{
			arrPGeos1.RemoveAll();

			arrPGeos1.Add(arrPGeos0[0]);

			//找出剩余地物中所有与arrPGeos1中地物有包含关系的地物，存放到arrPGeos1中
			for( int j=1; j<arrPGeos0.GetSize(); j++)
			{
				CGeometry *pGeo1 = arrPGeos0[j];
				if( pGeo1==NULL )
					continue;

				for( int i=0; i<arrPGeos1.GetSize(); i++)
				{
					CGeometry *pGeo2 = arrPGeos1[i];

					int ret = IsObjIncludeObj(pGeo1,pGeo2);
					if( ret!=0 )
					{
						arrPGeos1.Add(pGeo1);
						arrPGeos0[j] = NULL;

						//重新开始循环
						j = 0;
						break;
					}
				}
			}

			arrPGeos0[0] = NULL;

			//清理 arrPGeos0
			for( j=arrPGeos0.GetSize()-1; j>=0; j--)
			{
				if( arrPGeos0[j]==NULL )
					arrPGeos0.RemoveAt(j);
			}

			CFeature *pFtrNew = pLayer->CreateDefaultFeature(pDS->GetScale());
			if( !pFtrNew )
			{
				continue;
			}
			
			pFtrNew->SetID(OUID());

			CGeometry *pNewGeo = NULL;

			//将 arrPGeos1 中的地物打包为多面对象
			if( arrPGeos1.GetSize()>1 )
			{
				pNewGeo = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOMULTISURFACE);

				CGeoMultiSurface *pMS = (CGeoMultiSurface*)pNewGeo;
				CArray<PT_3DEX,PT_3DEX> arrPts;
				for( j=0; j<arrPGeos1.GetSize(); j++)
				{
					arrPts.RemoveAll();
					arrPGeos1[j]->GetShape(arrPts);
					pMS->AddSurface(arrPts);
				}
			}
			else
			{
				pNewGeo = (CGeometry*)CPermanent::CreatePermanentObject(CLS_GEOSURFACE);

				CArray<PT_3DEX,PT_3DEX> arrPts;
				arrPGeos1[0]->GetShape(arrPts);

				pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());

				
			}

			pNewGeo->SetSymbolName(symname);

			pFtrNew->SetGeometry(pNewGeo);
			
			arrPFtrs.Add(pFtrNew);
		}
	}

	return TRUE;

}


BOOL CDataSourceEx::IsModified()
{
	return m_bModified;
}

void CDataSourceEx::SetModifiedFlag(BOOL bModified)
{
	if (bModified != m_bModified)
	{
		UpdateModifiedTime();
	}
	m_bModified = bModified;
}

void CDataSourceEx::SetSavedFlag(BOOL bSaved)
{
	m_bHaveSaved = bSaved;
}

BOOL CDataSourceEx::IsSaved()
{
	return m_bHaveSaved;
}

void CDataSourceEx::UpdateModifiedTime()
{
	if (m_bModified)
	{
			if (m_nStartModifiedTime > 0)
			{				
				DWORD dIntervalTime = (GetTickCount() - m_nStartModifiedTime) / 60000;
				m_nTotalModifiedTime += dIntervalTime;
				m_nStartModifiedTime = 0;
			}
	}
	else
	{
		m_nStartModifiedTime = GetTickCount();
	}
}

DWORD CDataSourceEx::GetTotalModifiedTime()
{
	if (m_nStartModifiedTime > 0)
	{
		DWORD dIntervalTime = (GetTickCount() - m_nStartModifiedTime) / 60000;
		return (m_nTotalModifiedTime + dIntervalTime);
	}

	return m_nTotalModifiedTime;
}

void CDataSourceEx::SetDrawScaleByUser( double fDrawScale)
{
		m_fDrawScale = fDrawScale;
}

double CDataSourceEx::GetDrawScaleByUser()
{
	return m_fDrawScale;
}


double CDataSourceEx::GetSymbolDrawScale()
{
	return m_fDrawScale*m_nScale*0.001;
}

double CDataSourceEx::GetAnnotationScale()
{
	return GetSymbolDrawScale()*GetSymbolAnnoScale();
}



BOOL CDataSourceEx::FindLayerIdx(BOOL byCode, __int64 &code, CString& name)
{	
	CConfigLibManager *pConfigMag = GetConfigLibManager();	
	CScheme *pScheme = pConfigMag->GetScheme(m_nScale);
	if(pScheme)
	{					
		return pScheme->FindLayerIdx(byCode,code,name);
	}
	return FALSE;
}

__int64 CDataSourceEx::GetLayerCodeOfFtrLayer(CFtrLayer *pFtrLayer)
{
	ASSERT(pFtrLayer!=NULL);
	__int64 code = 0;
	CConfigLibManager *pConfigMag = GetConfigLibManager();	
	CScheme *pScheme = pConfigMag->GetScheme(m_nScale);
	if(pScheme)
	{	
		CString name = pFtrLayer->GetName();
		pScheme->FindLayerIdx(FALSE,code,name);
	}
	return code;
}

BOOL CDataSourceEx::GetAllAttribute(CFeature *pFtr, CValueTable &tab)
{
	tab.DelAll();
	tab.BeginAddValueItem();
	// 固有属性
	if (pFtr)
	{			
		pFtr->WriteTo(tab);
	}

	tab.EndAddValueItem();
				
	CFtrLayer *pFtrLayer = GetFtrLayerOfObject(pFtr);
	if (!pFtrLayer) return FALSE;
				
	CScheme *pScheme = GetConfigLibManager()->GetScheme(GetScale());
	if (!pScheme) return FALSE;
				
	CSchemeLayerDefine *pLayer = pScheme->GetLayerDefine(pFtrLayer->GetName());
	if (!pLayer) return FALSE;

	// 扩展属性
	CValueTable xtab;
	xtab.BeginAddValueItem();			
	m_pAttrSource->GetXAttributes(pFtr,xtab);
	xtab.EndAddValueItem();
				
	tab.AddValuesFromTab(xtab);

	return TRUE;
}

void CDataSourceEx::DisplayTop(CFtrLayer *pLayer)
{
	if (!pLayer)  return;
	
	int count = GetFtrLayerCount();
	int order = pLayer->GetDisplayOrder();
	
	for (int i=0; i<count; i++)
	{
		CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
		if (!pFtrLayer) continue;
		
		int order1 = pFtrLayer->GetDisplayOrder();
		
		if (order1 > order)
		{
			pFtrLayer->SetDisplayOrder(--order1);
			if( m_pAccess )
			{
				m_pAccess->SaveFtrLayer(pFtrLayer);
			}
		}
	}
	
	pLayer->SetDisplayOrder(GetMaxDisplayOrder()+1);
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}

	m_bDisplayOrderModified = TRUE;
	
}

void CDataSourceEx::DisplayBottom(CFtrLayer *pLayer)
{
	if (!pLayer)  return;
	
	int order = pLayer->GetDisplayOrder();
	
	for (int i=0; i<GetFtrLayerCount(); i++)
	{
		CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
		if (!pFtrLayer) continue;
		
		int order1 = pFtrLayer->GetDisplayOrder();
		
		if (order1 < order)
		{
			pFtrLayer->SetDisplayOrder(++order1);
			if( m_pAccess )
			{
				m_pAccess->SaveFtrLayer(pFtrLayer);
			}
		}
	}
	
	pLayer->SetDisplayOrder(0);
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pLayer);
	}

	m_bDisplayOrderModified = TRUE;
}

void CDataSourceEx::DisplayInsert(CFtrLayer *pInsert, CFtrLayer *pBefore)
{
	if (!pInsert || !pBefore)  return;

	int count = GetFtrLayerCount();
	int order0 = pInsert->GetDisplayOrder(), order2 = pBefore->GetDisplayOrder();

	if (order0 == order2) return;
	else if (order0 > order2)
	{
		for (int i=0; i<count; i++)
		{
			CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
			if (!pFtrLayer) continue;
			
			int order1 = pFtrLayer->GetDisplayOrder();
			
			if (order1 >= order2 && order1 < order0)			      
			{
				pFtrLayer->SetDisplayOrder(++order1);
				if( m_pAccess )
				{
					m_pAccess->SaveFtrLayer(pFtrLayer);
				}
			}
			
		}

	}
	else
	{
		for (int i=0; i<count; i++)
		{
			CFtrLayer *pFtrLayer = GetFtrLayerByIndex(i);
			if (!pFtrLayer) continue;
			
			int order1 = pFtrLayer->GetDisplayOrder();
			
		    if (order0 < order2 && order1 > order0 && order1 <= order2)
			{
				pFtrLayer->SetDisplayOrder(--order1);
				if( m_pAccess )
				{
					m_pAccess->SaveFtrLayer(pFtrLayer);
				}
			}
		}

		
	}

	pInsert->SetDisplayOrder(order2);
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayer(pInsert);
	}

	m_bDisplayOrderModified = TRUE;

}
CString CDataSourceEx::GetTempFilePath()
{
	CString tempPath = GetName();
	tempPath +=_T(".tmp");
	return tempPath;
}


void Bmp2Mem(HBITMAP hBmp, BYTE *&pBmpMem, int &nLen)
{
	DIBSECTION dibInfo;
	::GetObject(hBmp,sizeof(dibInfo),&dibInfo);
	BITMAPFILEHEADER fileInfo;
	BITMAPINFO		 bmpInfo;
	
	fileInfo.bfType = 0x4d42;
	fileInfo.bfSize = sizeof(fileInfo)+sizeof(bmpInfo)+dibInfo.dsBmih.biSizeImage;
	fileInfo.bfReserved1 = 0;
	fileInfo.bfReserved2 = 0;
	fileInfo.bfOffBits = sizeof(fileInfo)+sizeof(bmpInfo);
	
	memset(&bmpInfo,0,sizeof(bmpInfo));
	memcpy(&bmpInfo.bmiHeader,&dibInfo.dsBmih,sizeof(dibInfo.dsBmih));
	
	int fileHeadSize = sizeof(fileInfo), bmpInfoSize = sizeof(bmpInfo), bmpBitsSize = dibInfo.dsBmih.biSizeImage;

	nLen = fileHeadSize+bmpInfoSize+bmpBitsSize;

	pBmpMem = new BYTE[nLen];
	BYTE *pDest = pBmpMem;
	memcpy(pDest,&fileInfo,fileHeadSize);
	pDest += fileHeadSize;
	memcpy(pDest,&bmpInfo,bmpInfoSize);
	pDest += bmpInfoSize;
	memcpy(pDest,dibInfo.dsBm.bmBits,bmpBitsSize);
}

BOOL CDataSourceEx::AddPreviewImage(LPCTSTR name, HBITMAP hBit)
{
	BYTE *pBitmap;
	int nLen;
	Bmp2Mem(hBit,pBitmap,nLen);
	m_pAccess->SaveFilePreViewImage(name,pBitmap,nLen);

	return TRUE;
}

BOOL CDataSourceEx::DelPreviewImage(LPCTSTR name)
{
	m_pAccess->DelFilePreViewImage(name);
	return TRUE;
}

BOOL CDataSourceEx::SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type)
{
	if (field == NULL || name == NULL || value == NULL) return FALSE;

	m_pAccess->SaveDataSettings(field,name,value,type);

	SetModifiedFlag(TRUE);

	return TRUE;
}

BOOL CDataSourceEx::DelDataSettings(LPCTSTR field)
{
	if (field == NULL) return FALSE;
	
	m_pAccess->DelDataSettings(field);

	SetModifiedFlag(TRUE);

	return TRUE;
}

BOOL CDataSourceEx::GetDataSettings(LPCTSTR field, CString &name, CString &value, CString &type)
{	
	return m_pAccess->ReadDataSettings(field,name,value,type);
}

int CDataSourceEx::GetDataSettingsInt(LPCTSTR field, int def_val)
{
	CString name,value,type;
	if( GetDataSettings(field,name,value,type) )
	{
		return _ttol(value);
	}
	else
	{
		return def_val;
	}
}


float CDataSourceEx::GetDataSettingsFloat(LPCTSTR field, float def_val)
{
	CString name,value,type;
	if( GetDataSettings(field,name,value,type) )
	{
		return _ttof(value);
	}
	else
	{
		return def_val;
	}	
}


double CDataSourceEx::GetDataSettingsDouble(LPCTSTR field, double def_val)
{
	CString name,value,type;
	if( GetDataSettings(field,name,value,type) )
	{
		return _ttof(value);
	}
	else
	{
		return def_val;
	}	
}

CString CDataSourceEx::GetDataSettingsString(LPCTSTR field, LPCTSTR def_val)
{
	CString name,value,type;
	if( GetDataSettings(field,name,value,type) )
	{
		return value;
	}
	else
	{
		return def_val;
	}	
}


void CDataSourceEx::SetDataSettingsInt(LPCTSTR field, int val, LPCTSTR name)
{
	TCHAR name1[256] = {0};
	if( name==NULL )name = name1;
	CString value;
	value.Format(_T("%d"),val);
	SaveDataSettings(field,name,value,_T("int"));
}


void CDataSourceEx::SetDataSettingsFloat(LPCTSTR field, float val, LPCTSTR name)
{
	TCHAR name1[256] = {0};
	if( name==NULL )name = name1;
	CString value;
	value.Format(_T("%f"),val);
	SaveDataSettings(field,name,value,_T("float"));	
}


void CDataSourceEx::SetDataSettingsDouble(LPCTSTR field, double val, LPCTSTR name)
{
	TCHAR name1[256] = {0};
	if( name==NULL )name = name1;
	CString value;
	value.Format(_T("%lf"),val);
	SaveDataSettings(field,name,value,_T("double"));	
}


void CDataSourceEx::SetDataSettingsString(LPCTSTR field, LPCTSTR val, LPCTSTR name)
{
	TCHAR name1[256] = {0};
	if( name==NULL )name = name1;
	SaveDataSettings(field,name,val,_T("string"));	
}

BOOL CDataSourceEx::AddObjectGroup(ObjectGroup *group, BOOL bForLoad)
{
	if( !bForLoad && group->id <= 0)
	{
		group->id = GetMaxObjectGroupID() + 1;
	}

	m_arrFtrGroup.Add(group);
	
	if (!bForLoad)
	{
		m_pAccess->SaveObjectGroup(group);		
		SetModifiedFlag(TRUE);
	}
	

	return TRUE;
}

BOOL CDataSourceEx::DelObjectGroup(ObjectGroup *group)
{
	for (int i=0; i<m_arrFtrGroup.GetSize(); i++)
	{
		ObjectGroup *pGroup = m_arrFtrGroup[i];
		if (pGroup->id == group->id)
		{			
			m_arrFtrGroup.RemoveAt(i);
			if (m_pAccess)
			{
				m_pAccess->DelObjectGroup(group);				
				SetModifiedFlag(TRUE);
			}
			delete pGroup;
			break;
		}
	}
	
	return TRUE;
}

BOOL CDataSourceEx::UpdateObjectGroup(ObjectGroup *group)
{
	if( m_pAccess )
	{
		m_pAccess->SaveObjectGroup(group);
	}
	SetModifiedFlag(TRUE);
	
	return TRUE;
}

int CDataSourceEx::GetMaxObjectGroupID()
{
	int id = 0;
	for (int i=0; i<m_arrFtrGroup.GetSize(); i++)
	{
		if (m_arrFtrGroup[i]->id > id)
		{
			id = m_arrFtrGroup[i]->id;
		}
	}

	return id;
}

int CDataSourceEx::GetMaxFtrLayerGroupID()
{
	int id = 0;
	for (int i=0; i<m_arrFtrLayerGroup.GetSize(); i++)
	{
		if (m_arrFtrLayerGroup[i]->id > id)
		{
			id = m_arrFtrLayerGroup[i]->id;
		}
	}
	
	return id;
}

int CDataSourceEx::GetAssociatesWithObject(CFeature *pFtr, CFtrArray &arr)
{
	CUIntArray indexs;
	pFtr->GetObjectGroup(indexs);
	for (int i=0; i<indexs.GetSize(); i++)
	{
		int index = indexs[i];
		for (int j=0; j<m_arrFtrGroup.GetSize(); j++)
		{
			if (index == m_arrFtrGroup[j]->id)
			{
				if (m_arrFtrGroup[j]->select)
				{
					int size = m_arrFtrGroup[j]->ftrs.GetSize();
					for (int k=0; k<size; k++)
					{
						CFeature *pFtr = m_arrFtrGroup[j]->ftrs[k];
						if (pFtr && !pFtr->IsDeleted() && pFtr->IsVisible())
						{
							arr.Add(pFtr);
						}
					}
				}
				
				break;
			}
		}
	}

	return arr.GetSize();
}

int CDataSourceEx::GetObjectGroupCount() const
{
	return m_arrFtrGroup.GetSize();
}

ObjectGroup* CDataSourceEx::GetObjectGroup(int i)
{
	int size = m_arrFtrGroup.GetSize();
	if (i >= 0 && i < size)
	{
		return m_arrFtrGroup[i];
	}

	return NULL;
}

ObjectGroup* CDataSourceEx::GetObjectGroupByID(UINT id)
{
	int size = m_arrFtrGroup.GetSize();
	for( int i=0; i<size; i++)
	{
		if( m_arrFtrGroup[i]->id==id )
			return m_arrFtrGroup[i];
	}
	
	return NULL;
}


ObjectGroup* CDataSourceEx::CreateNewObjectGroup()
{
	ObjectGroup *pNewGrp = new ObjectGroup();
	pNewGrp->id = GetMaxObjectGroupID()+1;

	for( int j=1; j>0; j++)
	{
		sprintf(pNewGrp->name,"Group_%d",j);

		for (int i=0; i<m_arrFtrGroup.GetSize(); i++)
		{
			if( stricmp(pNewGrp->name,m_arrFtrGroup[i]->name)==0 )
			{
				break;
			}
		}
		if( i>=m_arrFtrGroup.GetSize() )
			break;
	}

	AddObjectGroup(pNewGrp,FALSE);

	return pNewGrp;
}

BOOL CDataSourceEx::AddFtrLayerGroup(FtrLayerGroup *pGroup, BOOL bForLoad)
{	
	if( !bForLoad && pGroup->id <= 0)
	{
		pGroup->id = GetMaxFtrLayerGroupID() + 1;
	}

	m_arrFtrLayerGroup.Add(pGroup);

	if (!bForLoad)
	{
		m_pAccess->SaveFtrLayerGroup(pGroup);	
		SetModifiedFlag(TRUE);
	}
	
	return TRUE;
}

BOOL CDataSourceEx::DelFtrLayerGroup(FtrLayerGroup *pGroup)
{
	for (int i=0; i<m_arrFtrLayerGroup.GetSize(); i++)
	{
		FtrLayerGroup *pGroup0 = m_arrFtrLayerGroup[i];
		if (pGroup0->Name.CompareNoCase(pGroup->Name) == 0)
		{			
			m_arrFtrLayerGroup.RemoveAt(i);
			if (m_pAccess)
			{
				m_pAccess->DelFtrLayerGroup(pGroup);
				SetModifiedFlag(TRUE);
			}
			delete pGroup0;
			break;
		}
	}
	
	return TRUE;
}

BOOL CDataSourceEx::UpdateFtrLayerGroup(FtrLayerGroup *pGroup)
{
	if( m_pAccess )
	{
		m_pAccess->SaveFtrLayerGroup(pGroup);
	}
	SetModifiedFlag(TRUE);
	
	return TRUE;
}

int CDataSourceEx::GetFtrLayerGroupCount() const
{
	return m_arrFtrLayerGroup.GetSize();
}

FtrLayerGroup* CDataSourceEx::GetFtrLayerGroup(int i)
{
	int size = m_arrFtrLayerGroup.GetSize();
	if (i >= 0 && i < size)
	{
		return m_arrFtrLayerGroup[i];
	}
	
	return NULL;
}

FtrLayerGroup* CDataSourceEx::GetFtrLayerGroupByName(CString Name)
{
	int size = m_arrFtrLayerGroup.GetSize();
	for( int i=0; i<size; i++)
	{
		if( m_arrFtrLayerGroup[i]->Name.CompareNoCase(Name) == 0 )
			return m_arrFtrLayerGroup[i];
	}
	
	return NULL;
}


CFeature *CDataSourceEx::GetFeatureByID(OUID id)
{
	for( int i=0; i<m_arrObjs.GetSize(); i++)
	{
		if( m_arrObjs[i]->GetID()==id )
			return m_arrObjs[i];
	}

	return NULL;
}

double CDataSourceEx::GetFtrsMaxMinDisplayOrder(BOOL bMax)
{
	int count = m_arrAllFtrsByOrderID.GetSize();
	
	if (count < 1)
	{
		return 0;
	}

	int index = bMax?count-1:0;
	return m_arrAllFtrsByOrderID[index]->GetDisplayOrder();	
	
}

void CDataSourceEx::GetAllFtrsByDisplayOrder(CFtrArray &arr)
{
	arr.Copy(m_arrAllFtrsByOrderID);
}

extern int CompFuncSortFtrInLayer(const void *e1, const void *e2);

BOOL CDataSourceEx::DisplayTop(CFeature **pSrcFtrs, int nSrcNum, CFeature **pRefFtrs, int nRefNum)
{
	if (pSrcFtrs == NULL || nSrcNum == 0) return FALSE;

	int allSize = m_arrAllFtrsByOrderID.GetSize(), i;

	double maxRefOrder = -1;
	int index;
	if (pRefFtrs == NULL || nRefNum == 0)
	{
		index = allSize-1;
		maxRefOrder = m_arrAllFtrsByOrderID[index]->GetDisplayOrder();
	}
	else
	{
		for (i=0; i<nRefNum; i++)
		{
			double order = pRefFtrs[i]->GetDisplayOrder();
			if (i == 0 || order > maxRefOrder)
			{
				maxRefOrder = order;
			}
		}		
		
		if (!FindOrderIndex(maxRefOrder,index)) return FALSE;
	}	

	// 排序
	CFtrArray arrSrc;
	arrSrc.SetSize(nSrcNum);
	memcpy(arrSrc.GetData(),pSrcFtrs,nSrcNum*sizeof(CFeature*));
	qsort(arrSrc.GetData(),nSrcNum,sizeof(CPFeature),CompFuncSortFtrInLayer);

	// 在最上层地物之上依次加1，否则将index,index+1均分为nSrcNum+1份
	double order0 = 1;
	if (index != allSize-1)
	{
		order0 = (m_arrAllFtrsByOrderID[index+1]->GetDisplayOrder() - m_arrAllFtrsByOrderID[index]->GetDisplayOrder())/(nSrcNum+1);
	}

	for (i=nSrcNum-1; i>=0; i--)
	{
		CFeature *pFtr = arrSrc[i];

		int index0;
		if (FindOrderIndex(pFtr->GetDisplayOrder(),index0) && m_arrAllFtrsByOrderID[index0] == pFtr)
		{
			if (index0 == index) continue;

			m_arrAllFtrsByOrderID.RemoveAt(index0);
			if (index > index0)
			{
				index--;
			}

			pFtr->SetDisplayOrder(maxRefOrder+(i+1)*order0);

			m_arrAllFtrsByOrderID.InsertAt(index+1,pFtr);
		}
	}
	
	return TRUE;
}	

BOOL CDataSourceEx::DisplayBottom(CFeature **pSrcFtrs, int nSrcNum, CFeature **pRefFtrs, int nRefNum)
{
	if (pSrcFtrs == NULL || nSrcNum == 0) return FALSE;
	
	int allSize = m_arrAllFtrsByOrderID.GetSize(), i;

	double minRefOrder = -1;
	int index;
	if (pRefFtrs == NULL || nRefNum == 0)
	{
		index = 0;
		minRefOrder = m_arrAllFtrsByOrderID[index]->GetDisplayOrder();
	}
	else
	{
		for (int i=0; i<nRefNum; i++)
		{
			double order = pRefFtrs[i]->GetDisplayOrder();
			if (i == 0 || order < minRefOrder)
			{
				minRefOrder = order;
			}
		}
		
		if (!FindOrderIndex(minRefOrder,index)) return FALSE;
	}
	
	// 排序
	CFtrArray arrSrc;
	arrSrc.SetSize(nSrcNum);
	memcpy(arrSrc.GetData(),pSrcFtrs,nSrcNum*sizeof(CFeature*));
	qsort(arrSrc.GetData(),nSrcNum,sizeof(CPFeature),CompFuncSortFtrInLayer);
	
	// 在最底层地物之下依次减1，否则将index-1,index均分为nSrcNum+1份
	double order0 = 1;
	if (index != 0)
	{
		order0 = (m_arrAllFtrsByOrderID[index]->GetDisplayOrder() - m_arrAllFtrsByOrderID[index-1]->GetDisplayOrder())/(nSrcNum+1);
	}
	
	for (i=0; i<nSrcNum; i++)
	{
		CFeature *pFtr = arrSrc[i];
		
		int index0;
		if (FindOrderIndex(pFtr->GetDisplayOrder(),index0) && m_arrAllFtrsByOrderID[index0] == pFtr)
		{
			if (index == index0) continue;

			m_arrAllFtrsByOrderID.RemoveAt(index0);
			if (index > index0)
			{
				index--;
			}

			pFtr->SetDisplayOrder(minRefOrder-(nSrcNum-i)*order0);
			m_arrAllFtrsByOrderID.InsertAt(index,pFtr);

			if (index < index0)
			{
				index++;
			}
		}
	}
	
	return TRUE;
}	

BOOL CDataSourceEx::FindOrderIndex(double order, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrAllFtrsByOrderID.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		int i0 = 0, i1 = m_arrAllFtrsByOrderID.GetSize()-1, i2;
		while(findidx==-1) 
		{
			if( order<=m_arrAllFtrsByOrderID.GetAt(i0)->GetDisplayOrder() )
			{
				bool bEqual = (fabs(order-m_arrAllFtrsByOrderID.GetAt(i0)->GetDisplayOrder())<1e-10);
				bnew = (bEqual?0:1);
				findidx = i0;
				break;
			}
			else if( order>=m_arrAllFtrsByOrderID.GetAt(i1)->GetDisplayOrder() )
			{
				bool bEqual = (fabs(order-m_arrAllFtrsByOrderID.GetAt(i1)->GetDisplayOrder())<1e-10);
				bnew = (bEqual?0:1);
				findidx = (bEqual?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bool bEqual = (fabs(order-m_arrAllFtrsByOrderID.GetAt(i0)->GetDisplayOrder())<1e-10);
				bnew = (bEqual?0:1);
				findidx = (bEqual?i0:(i0+1));
				break;
			}
			
			if( order<m_arrAllFtrsByOrderID.GetAt(i2)->GetDisplayOrder() )
				i1 = i2;
			else if( order>m_arrAllFtrsByOrderID.GetAt(i2)->GetDisplayOrder() )
				i0 = i2;
			else
			{
				findidx = i2; bnew = 0;
				break;
			}
		}
	}
	
	insert_idx = findidx;
	return (bnew==0);
}

void CDataSourceEx::SaveAllAppFlags(BOOL bClearZero)
{
	AppFlagArray *pArray = new AppFlagArray();

	int nLayNum = GetFtrLayerCount();
	int nObj = 0;
	for (int i=0;i<nLayNum;i++)
	{
		CFtrLayer* pLayer = GetFtrLayerByIndex(i);
		if (pLayer==NULL)continue;		
		
		nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(pFtr==NULL)continue;

			FtrAppFlag item;
			item.pFtr = pFtr;
			item.flag = pFtr->GetAppFlag();
			pArray->Add(item);

			if( bClearZero )
			{
				pFtr->SetAppFlag(0);
			}
		}
	}

	m_arrAppFlagsBuf.Add(pArray);
}

void CDataSourceEx::SetAllAppFlags(int flag)
{	
	int nLayNum = GetFtrLayerCount();
	int nObj = 0;
	for (int i=0;i<nLayNum;i++)
	{
		CFtrLayer* pLayer = GetFtrLayerByIndex(i);
		if (pLayer==NULL)continue;		
		
		nObj = pLayer->GetObjectCount();
		for (int j=0;j<nObj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if(pFtr==NULL)continue;
			
			pFtr->SetAppFlag(flag);
		}
	}
}


void CDataSourceEx::RestoreAllAppFlags()
{
	if( m_arrAppFlagsBuf.GetSize()<=0 )
		return;

	AppFlagArray *pArray = m_arrAppFlagsBuf[m_arrAppFlagsBuf.GetSize()-1];
	if( pArray==NULL )
		return;
	int nSize = pArray->GetSize();

	for( int i=0; i<nSize; i++)
	{
		FtrAppFlag item = pArray->GetAt(i);
		item.pFtr->SetAppFlag(item.flag);
	}
	
	delete pArray;
	m_arrAppFlagsBuf.RemoveAt(m_arrAppFlagsBuf.GetSize()-1);
}


void CDataSourceEx::SaveAllQueryFlags(BOOL bClear, BOOL bQueryFlag)
{
	QueryFlagArray *pArray = new QueryFlagArray();
	
	int nLayNum = GetFtrLayerCount();
	int nObj = 0;
	for (int i=0;i<nLayNum;i++)
	{
		CFtrLayer* pLayer = GetFtrLayerByIndex(i);
		if (pLayer==NULL)continue;		
			
		FtrLayerQueryFlag item;
		item.pLayer = pLayer;
		item.flag = pLayer->IsAllowQuery();

		pArray->Add(item);
		
		if( bClear )
		{
			pLayer->SetAllowQuery(bQueryFlag);
		}
	}
	
	m_arrQueryFlagsBuf.Add(pArray);
}


void CDataSourceEx::RestoreAllQueryFlags()
{
	if( m_arrQueryFlagsBuf.GetSize()<=0 )
		return;
	
	QueryFlagArray *pArray = m_arrQueryFlagsBuf[m_arrQueryFlagsBuf.GetSize()-1];
	if( pArray==NULL )
		return;
	int nSize = pArray->GetSize();
	
	for( int i=0; i<nSize; i++)
	{
		FtrLayerQueryFlag item = pArray->GetAt(i);
		item.pLayer->SetAllowQuery(item.flag);
	}
	
	delete pArray;
	m_arrQueryFlagsBuf.RemoveAt(m_arrQueryFlagsBuf.GetSize()-1);
}


void CDataSourceEx::SetFtrLayerQueryFlag(LPCTSTR layname, BOOL bAllowQuery)
{
	CPtrArray arrLayers;
	GetFtrLayer(layname,NULL,&arrLayers);

	for( int i=0; i<arrLayers.GetSize(); i++)
	{
		CFtrLayer *pLayer = (CFtrLayer*)arrLayers[i];
		if( pLayer )pLayer->SetAllowQuery(FALSE);
	}
}


CDataSourceEx *CDataSourceEx::CreateObj(CDataQueryEx *pDQ)
{
	return new CDataSourceEx(pDQ);
}

void CDataSourceEx::DestroyObj(CDataSourceEx *pObj)
{
	if( pObj )
		delete pObj;
}

CFtrLayer* CDataSourceEx::GetOrCreateFtrLayer(LPCTSTR strLayerName)
{
	CFtrLayer *pLayer = GetFtrLayer(strLayerName);
	
	if( pLayer==NULL )
	{
		pLayer = new CFtrLayer();
		pLayer->SetName(strLayerName);
		
		AddFtrLayer(pLayer,FALSE);
	}
	
	return pLayer;
}

MyNameSpaceEnd