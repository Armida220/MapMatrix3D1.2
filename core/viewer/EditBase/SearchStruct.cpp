
#include "stdafx.h"
#include "Envelope.h"
#include "SearchStruct.h"
#include "SmartViewFunctions.h"
#include "GeoPoint.h"
#include "Linearizer.h"

MyNameSpaceBegin

IMPLEMENT_MEMPOOL(SearchablePoint);
IMPLEMENT_MEMPOOL(SearchableLine);
IMPLEMENT_MEMPOOL(SearchableGrBuf);
IMPLEMENT_MEMPOOL(SearchableObj);

Envelope GetEnvelopeOfSearchableUnit(const SearchableUnit *pu)
{
	if( pu==NULL )return Envelope();
	if( IsSearchablePoint(pu) )
	{
		return ((CGeoPoint*)((const SearchablePoint*)pu)->p)->GetEnvelope();
	}
	if( IsSearchableLine(pu) )
	{
		return ((const CShapeLine::ShapeLineUnit*)((const SearchableLine*)pu)->p)->evlp;
	}
	if( IsSearchableObj(pu) )
	{
		return ((const SearchableObj*)pu)->p->GetEnvelope();
	}
	else if ( IsSearchableGrBuf(pu))
	{
		return ((const SearchableGrBuf*)pu)->e;
	}
	return Envelope();
}


void ClearSearchableUnit(SearchableUnit **pus, int nUnit)
{
	if( pus!=NULL )
	{
		for( int i=0; i<nUnit; i++)
		{
			if( IsSearchableObj(pus[i]) )
			{
				delete (SearchableObj*)pus[i];
			}
			else if( IsSearchablePoint(pus[i]) )
			{
				delete (SearchablePoint*)pus[i];
			}
			else if( IsSearchableLine(pus[i]) )
			{
				delete (SearchableLine*)pus[i];
			}
			else if ( IsSearchableGrBuf(pus[i]))
			{
				((SearchableGrBuf*)pus[i])->ClearBuf();
				delete (SearchableGrBuf*)pus[i];
			}
		}
	}
}


double FindMinimumDistance(PT_3D sch_pt, Envelope sch_evlp, const SearchableUnit *pu, CCoordSys *pCS, PT_3D *pRet, double& ret_real_mindis)
{
	ret_real_mindis = 1e+100;
	if( pu==NULL )return ret_real_mindis;

	PT_3D pt1,pt2,line[2];

	if( IsSearchablePoint(pu) )
	{
		PT_4D pt = (PT_3D)((const CGeoPoint*)((const SearchablePoint*)pu)->p)->GetDataPoint(0);
		pCS->GroundToClient(&pt,1);
		
		ret_real_mindis = DIST_3DPT(sch_pt,pt);
		*pRet = pt;
	}
	if( IsSearchableLine(pu) )
	{
		const CShapeLine::ShapeLineUnit* p = (const CShapeLine::ShapeLineUnit*)(((const SearchableLine*)pu)->p);
		if( p->FindNearestPt(sch_pt,sch_evlp,pCS,pRet,&ret_real_mindis) )
		{
			return ret_real_mindis;
		}
		else
		{
			return ret_real_mindis;
		}
	}
	if( IsSearchableObj(pu) )
	{
		return (ret_real_mindis = ((const SearchableObj*)pu)->p->GetMinDistance(sch_pt,pCS,pRet));
	}
	if (IsSearchableGrBuf(pu))
	{		
		return FindMinimumDistance(sch_pt, (const GrBuffer *)(((const SearchableGrBuf*)pu)->p), pCS, pRet, &ret_real_mindis);
	}

	return ret_real_mindis;
}


SearchableAgentObj::SearchableAgentObj()
{
	
}


SearchableAgentObj::~SearchableAgentObj()
{
	
}

Envelope SearchableAgentObj::GetEnvelope()const
{
	return Envelope();
}

BOOL SearchableAgentObj::FindObj(const SearchRange *sr, double *distance)const
{
	return FALSE;
}

double SearchableAgentObj::GetMinDistance(PT_3D sch_pt, CCoordSys* pCS, PT_3D *pret)const
{
	return 1e+100;
}

SearchableObj::SearchableObj()
{
	type = EBSU_OBJECT;
	p = NULL;
}

SearchableObj::~SearchableObj()
{
	if( p!=NULL )delete p;
}


void CSearcher::SetHitSurfaceInside(BOOL bEnable)
{
	
}

MyNameSpaceEnd

