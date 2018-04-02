// Snap.cpp: implementation of the CSnap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Snap.h"
#include "DataSourceEx.h"
#include "FLOAT.H "
#include "SmartViewFunctions.h"
#include "GeoCurve.h "
#include "RegDef.h "
#include "RegDef2.h "
#include "GeoParallel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL SNAPITEM::IsValid()
{
	return ( pFtr || bInPreObj || (nSnapMode&CSnap::modeGrid)!=0 || (nSnapMode&CSnap::modePolar)!=0 );
}

CSnap::CSnap()
{	
//	m_dwFindMode = GETMODE_INC_NORMAL;
	m_bWithSymbol = TRUE;
	m_lfNearestPreSnap = -1.0;
	m_nMode = modeKeyPoint;
	m_bOpen = FALSE;
	m_b2D = FALSE;
	m_bSnapSelf = TRUE;
	m_bOnlyBase = TRUE;
	m_lfRadius = 10;
	m_pDataQuery = NULL;
}

CSnap::~CSnap()
{

}

void CSnap::SetSnapRadius(double r)
{
	m_lfRadius = r;
}

double CSnap::GetSnapRadius()
{
	return m_lfRadius;
}

void CSnap::SetSnapMode(long nMode)
{
	m_nMode = nMode;
}

long CSnap::GetSnapMode()
{
	return m_nMode;
}


SNAP_GRID CSnap::GetGridParams()
{
	return m_gridParm;
}

void CSnap::SetGridParams(SNAP_GRID *param)
{
	if( param )m_gridParm = *param;
}


BOOL CSnap::PreSnap(Envelope e, PT_3D *lastPt, int lastPtNum, PT_3D *curPt,CCoordSys *pCS, CGeometry *pObj, BOOL bReset)
{
	CSetMaxNumResultsOfSearch _t(m_pDataQuery,10);

	if( bReset )
	{
		m_lfNearestPreSnap = -1.0;
		m_preItem = SNAPITEM();
	}
	if( !pObj )return FALSE;

//	m_foundHandles.RemoveAll();
	m_arrSnapItems.RemoveAll();

	if( !m_bOpen )return FALSE;
//	if( m_arrSource.GetSize()<=0 )return FALSE;

	BOOL save = m_bWithSymbol;
	if( m_bOnlyBase )m_bWithSymbol = FALSE;

	PT_3D ret;
	PT_3D tmp;
	pCS->GroundToClient(curPt,&tmp);
	if( !pObj->FindNearestBasePt(tmp,e,pCS,&ret,NULL) )
	{
		m_bWithSymbol = save;
		return FALSE;
	}

	if( (m_nMode&modeIntersect) )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
		m_pDataQuery->FindNearestObject(tmp,r,pCS);
		//插入一个空ID作为当前这个对象的ID，以便DoSnapByObj在做相交处理时，能正确进行
/*		m_pDataQuery->*/
	}

	double min = -1;
	BOOL bRet = DoSnapByObj(e,lastPt,lastPtNum,curPt,pCS,pObj,&min);

	if( bRet && m_arrSnapItems.GetSize()>0 && (m_lfNearestPreSnap<0||m_lfNearestPreSnap>min) )
	{
		m_lfNearestPreSnap = min;
		m_preItem = m_arrSnapItems[0];
		m_preItem.bInPreObj = 1;
	}

	/*m_pDataQuery->c*/

	m_bWithSymbol = save;

	return bRet;
}

BOOL CSnap::DoSnap(Envelope e, PT_3D *lastPt, int lastPtNum, PT_3D *curPt,CCoordSys *pCS)
{
	m_arrSnapItems.RemoveAll();

	if( !m_bOpen )return FALSE;

	CSetMaxNumResultsOfSearch _t(m_pDataQuery,10);

// 	if( (m_nMode&modeGrid) )
// 	{
// 		return DoSnapByGrid(e,curPt,pCS);
// 	}

//	if( m_arrSource.GetSize()<0 )return FALSE;

	BOOL save = m_bWithSymbol;
	if( m_bOnlyBase )m_bWithSymbol = FALSE;

	double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
	PT_3D pt;
	pCS->GroundToClient(curPt,&pt);
	BOOL bOldLocked = m_pDataQuery->SetFilterIncludeLocked(TRUE);
	BOOL bOldRef = m_pDataQuery->SetFilterIncludeRefData(TRUE);
	CFeature *pFtr = m_pDataQuery->FindNearestObject(pt,r,pCS,FALSE);
	m_pDataQuery->SetFilterIncludeLocked(bOldLocked);
	m_pDataQuery->SetFilterIncludeRefData(bOldRef);
	if( !pFtr && !((m_nMode&modePolar) || (m_nMode&modeCenterPoint) || (m_nMode&modeGrid)))
	{
		m_bWithSymbol = save;
		if( m_preItem.bInPreObj && m_lfNearestPreSnap>0 )
		{
			m_arrSnapItems.Add(m_preItem);
			return TRUE;
		}

		return FALSE;		
	}

	double min = -1, dis=0;
	BOOL bRet = FALSE;
	if (pFtr || (m_nMode&modeGrid) || (m_nMode&modeCenterPoint))
	{
		CGeometry *pObj = pFtr!=NULL?pFtr->GetGeometry():NULL;
		bOldLocked = m_pDataQuery->SetFilterIncludeLocked(TRUE);
		bOldRef = m_pDataQuery->SetFilterIncludeRefData(TRUE);

		CGeometry *pObj2;
		CArray<SNAPITEM,SNAPITEM> arrSnapItems;
		int nObj1 = 0, nObj2 = 0;
		const CPFeature *ftr_buf = m_pDataQuery->GetFoundHandles(nObj1);
		const double *dis_buf = m_pDataQuery->GetFoundDis(nObj2);
		CPFeature *ftr_buf2 = new CPFeature[nObj1];
		double *dis_buf2 = new double[nObj2];
		memcpy(ftr_buf2,ftr_buf,sizeof(CPFeature)*nObj1);
		memcpy(dis_buf2,dis_buf,sizeof(double)*nObj2);
		dis_buf = dis_buf2;
		ftr_buf = ftr_buf2;
		if(nObj1==nObj2 && nObj1>1)
		{
			for(int i=0; i<nObj1; i++)
			{			
				if( i==0 || (dis_buf[i]-dis_buf[i-1])<GraphAPI::g_lfDisTolerance)
				{
					pObj2 = ftr_buf[i]->GetGeometry();
					bRet = DoSnapByObj(e,lastPt,lastPtNum,curPt,pCS,pObj2,&dis);	

					if(bRet && (min<0 || dis<min) )
					{
						min = dis;
						arrSnapItems.Copy(m_arrSnapItems);
					}
				}
				else if(i>0 && (dis_buf[i]-dis_buf[i-1])>GraphAPI::g_lfDisTolerance )
				{
					break;
				}
			}

			m_arrSnapItems.Copy(arrSnapItems);
		}
		else
		{
			bRet = DoSnapByObj(e,lastPt,lastPtNum,curPt,pCS,pObj,&min);				
		}

		delete[] ftr_buf2;
		delete[] dis_buf2;
		
		CGeometry *pObj1=NULL;
		pObj2 = NULL;
		if( pObj!=NULL && pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && ((CGeoParallel*)pObj)->Separate(pObj1,pObj2) )
		{
			if( pObj2 )
			{
				CArray<SNAPITEM,SNAPITEM> snapItems;
				snapItems.Copy(m_arrSnapItems);
				
				BOOL bRet2 = FALSE;
				double min2 = -1;
				bRet2 = DoSnapByObj(e,lastPt,lastPtNum,curPt,pCS,pObj2,&min2);	
				
				if( bRet && !bRet2 )
				{
					m_arrSnapItems.Copy(snapItems);
				}
				else if( !bRet && bRet2 )
				{
					bRet = TRUE;
				}
				else if( !bRet && !bRet2 )
				{
				}
				else if( bRet && bRet2 )
				{
					if( min<min2 )
						m_arrSnapItems.Copy(snapItems);
					else
					{
					}
				}
			}
			
			if( pObj1 )delete pObj1;
			if( pObj2 )delete pObj2;			
		}

		m_pDataQuery->SetFilterIncludeLocked(bOldLocked);
		m_pDataQuery->SetFilterIncludeRefData(bOldRef);
	}
	
	if ( (m_nMode&modePolar) )
	{
		for (int i=0; i<lastPtNum; i++)
		{
			double dis = -1;
			if( DoSnapByPolar(e,lastPt+i,curPt,pCS,&dis) )
			{
				bRet = TRUE;
				if( min<0 || min>dis )
				{
					min = dis;
					if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
				}
				else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
			}
		}
		
	}

	if( m_preItem.bInPreObj && m_lfNearestPreSnap>0  && (min<0||min>m_lfNearestPreSnap) )
	{
		m_arrSnapItems.InsertAt(0,m_preItem);
		bRet = TRUE;
	}

	m_bWithSymbol = save;
	
	return bRet;
}


BOOL CSnap::DoSnapByObj(Envelope e, PT_3D *lastPt, int lastPtNum, PT_3D *curPt, CCoordSys *pCS,CGeometry *pObj,double *ret_mindis)
{
	m_arrSnapItems.RemoveAll();

	BOOL bRet = FALSE;
	double dis=-1, min=-1;


	if( (m_nMode&modeGrid) )
	{
		if( DoSnapByGrid(e,curPt,pCS,&dis) )
		{
			bRet = TRUE;
			if( min<0 || min>dis )
			{
				min = dis;
				if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
			}
			else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
		}
		
	}

	if( (m_nMode&modeEndPoint) )
	{
		if( DoSnapByEndPoint(e,curPt,pCS,pObj,&dis) )
		{
			bRet = TRUE;
			if( min<0 || min>dis )
			{
				min = dis;
				if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
			}
			else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
		}
	}

	if( (m_nMode&modeKeyPoint) )
	{
		if( DoSnapByKeyPoint(e,curPt,pCS,pObj,&dis) )
		{
			bRet = TRUE;
			if( min<0 || min>dis )
			{
				min = dis;
				if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
			}
			else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
		}
	}

	if( (m_nMode&modeMidPoint) )
	{
		if( DoSnapByMidPoint(e,curPt,pCS,pObj,&dis) )
		{
			bRet = TRUE;
			if( min<0 || min>dis )
			{
				min = dis;
				if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
			}
			else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
		}
	}

	if( (m_nMode&modeIntersect) )
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		CGeometry *pObj1 = NULL;
		if(num>=2)
			pObj1 = ftr[1]->GetGeometry();
		if( pObj1 )
		{
			BOOL bProcessOK = FALSE;
			CGeometry *pObj2=NULL, *pObj3 = NULL;
			if( pObj1->IsKindOf(RUNTIME_CLASS(CGeoParallel)) && ((CGeoParallel*)pObj1)->Separate(pObj2,pObj3) )
			{
				if( pObj2 && pObj3 )
				{
					double dis1, dis2;
					BOOL bRet1, bRet2;
					bRet1 = DoSnapByIntersect(e,curPt,pCS,pObj,pObj2,&dis1);

					CArray<SNAPITEM,SNAPITEM> snapItems;
					snapItems.Copy(m_arrSnapItems);

					bRet2 = DoSnapByIntersect(e,curPt,pCS,pObj,pObj3,&dis2);

					bRet = TRUE;

					if( bRet1 && !bRet2 )
					{
						dis = dis1;
						m_arrSnapItems.Copy(snapItems);
					}
					else if( !bRet1 && bRet2 )
					{
						dis = dis2;
					}
					else if( !bRet1 && !bRet2 )
					{
						bRet = FALSE;
					}
					else if( bRet1 && bRet2 )
					{
						if( dis1<dis2 )
						{
							dis = dis1;
							m_arrSnapItems.Copy(snapItems);
						}
					}

					if( pObj2 )delete pObj2;
					if( pObj3 )delete pObj3;

					if( min<0 || min>dis )
					{
						min = dis;
						if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
					}
					else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);

					bProcessOK = TRUE;					
				}
			}

			if( !bProcessOK && DoSnapByIntersect(e,curPt,pCS,pObj,pObj1,&dis) )
			{
				bRet = TRUE;
				if( min<0 || min>dis )
				{
					min = dis;
					if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
				}
				else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
			}
		}
	}

	if( (m_nMode&modePerpPoint) )
	{
		for (int i=0; i<lastPtNum; i++)
		{
			if( DoSnapByPerpPoint(e,lastPt+i,curPt,pCS,pObj,&dis) )
			{
				bRet = TRUE;
				if( min<0 || min>dis )
				{
					min = dis;
					if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
				}
				else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
			}
		}
	}
	
	if( (m_nMode&modeCenterPoint) )
	{
		if( DoSnapByCenterPoint(e,curPt,pCS,pObj,&dis) )
		{
			bRet = TRUE;
			if( min<0 || min>dis )
			{
				min = dis;
				if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
			}
			else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
		}
	}

	if( (m_nMode&modeTangPoint) )
	{
		for (int i=0; i<lastPtNum; i++)
		{
			if( DoSnapByTangPoint(e,lastPt+i,curPt,pCS,pObj,&dis) )
			{
				bRet = TRUE;
				if( min<0 || min>dis )
				{
					min = dis;
					if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
				}
				else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
			}
		}
		
	}
	
	if( (m_nMode&modeNearPoint) )
	{
		if( DoSnapByNearPoint(e,curPt,pCS,pObj,&dis) )
		{
			bRet = TRUE;
			if( min<0 || min>dis )
			{
				min = dis;
				if( m_arrSnapItems.GetSize()>1 )m_arrSnapItems.RemoveAt(0);
			}
			else m_arrSnapItems.RemoveAt(m_arrSnapItems.GetSize()-1);
		}
	}

	if( bRet )*ret_mindis = min;

	if( bRet&&m_b2D&&curPt&&m_arrSnapItems.GetSize()>0 )
	{
		int num = m_arrSnapItems.GetSize();
		for( int i=0; i<num; i++ )
		{
			m_arrSnapItems[i].pt.z = curPt->z;
		}
	}

	return bRet;
}


BOOL CSnap::DoSnapByNearPoint(Envelope e, PT_3D *pt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{
	PT_3D tmp;
	pCS->GroundToClient(pt,&tmp);
	CFeature * pFtr =NULL;
	if( !pObj )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;		
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )return FALSE;
		
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];
	}
	
	PT_3D ret;
	if( !pObj->FindNearestBasePt(tmp,e,pCS,&ret,NULL) )
		return FALSE;

	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modeNearPoint;
	item.pt = ret;

	if( ret_dis )
	{
		*ret_dis = 1e+200 * (1 + sqrt( (pt->x-item.pt.x)*(pt->x-item.pt.x) + (pt->y-item.pt.y)*(pt->y-item.pt.y) ));
	}

	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByEndPoint(Envelope e, PT_3D *pt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{	
	PT_3D tmp;
	pCS->GroundToClient(pt,&tmp);
	CFeature *pFtr = NULL;
	if( !pObj )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )return FALSE;
		
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;
		
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];	
	}

	if( pObj->GetDataPointSum()<=0 )
		return FALSE;

	PT_3DEX ret;
	if( pObj->GetDataPointSum()==1 )
	{
		ret = pObj->GetDataPoint(0);
	}
	else
	{
		PT_3DEX pt2;
		ret = pObj->GetDataPoint(0);		
		pt2 = pObj->GetDataPoint(pObj->GetDataPointSum()-1);
		double dis1 = sqrt( (pt->x-ret.x)*(pt->x-ret.x) + (pt->y-ret.y)*(pt->y-ret.y) );
		double dis2 = sqrt( (pt->x-pt2.x)*(pt->x-pt2.x) + (pt->y-pt2.y)*(pt->y-pt2.y) );
		if( dis1>dis2 )ret  = pt2;
	}	
	pCS->GroundToClient(&ret,&tmp);
	if( !e.bPtIn(&tmp) )
		return FALSE;
	
	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modeEndPoint;
	COPY_3DPT(item.pt,ret);
	
	if( ret_dis )
	{
		*ret_dis = sqrt( (pt->x-item.pt.x)*(pt->x-item.pt.x) + (pt->y-item.pt.y)*(pt->y-item.pt.y) );
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByKeyPoint(Envelope e, PT_3D *pt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{	
	PT_3D tmp;
	pCS->GroundToClient(pt,&tmp);
	CFeature *pFtr = NULL;
	if( !pObj )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )return FALSE;
		
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;
		
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];	
	}
	
	double w = e.m_xh-e.m_xl, h = e.m_yh-e.m_yl;
	double r = (w>h?w:h)*0.5;


	PT_KEYCTRL ketpt = pObj->FindNearestKeyCtrlPt(tmp,r,pCS,1);
	if(!ketpt.IsValid())
		return FALSE;

	PT_3DEX ret;	
	ret = pObj->GetDataPoint(ketpt.index);

	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modeKeyPoint;
	COPY_3DPT(item.pt,ret);

	if( ret_dis )
	{
		*ret_dis = sqrt( (pt->x-item.pt.x)*(pt->x-item.pt.x) + (pt->y-item.pt.y)*(pt->y-item.pt.y) );
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByMidPoint(Envelope e, PT_3D *pt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{		
	PT_3D tmp;
	pCS->GroundToClient(pt,&tmp);
	CFeature *pFtr = NULL;
	if( !pObj )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )return FALSE;
		
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;
		
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];	
	}
	
	PT_3D ret1,ret2;
	if( !pObj->FindNearestBaseLine(tmp,e,pCS,&ret1,&ret2,NULL) )
		return FALSE;

	ret1.x = (ret1.x+ret2.x)/2; ret1.y = (ret1.y+ret2.y)/2; ret1.z = (ret1.z+ret2.z)/2; 
	
	pCS->GroundToClient(&ret1,&tmp);
	if( !e.bPtIn(&tmp) )
		return FALSE;

	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modeMidPoint;
	item.pt = ret1;

	if( ret_dis )
	{
		*ret_dis = sqrt( (pt->x-item.pt.x)*(pt->x-item.pt.x) + (pt->y-item.pt.y)*(pt->y-item.pt.y) );
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByCenterPoint(Envelope e, PT_3D *pt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{
	PT_3D tmp;
	pCS->GroundToClient(pt,&tmp);
	CFeature *pFtr = NULL;

	double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;

	if( 1 || !pObj )
	{
		
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )
			return FALSE;
		/*
		int nObj = m_pDataQuery->FindNearestObjectByObjNum(tmp,1,pCS,FALSE,NULL,NULL);		
			
		const CPFeature *ppFtrs = m_pDataQuery->GetFoundHandles(nObj);
		if (nObj <= 0) return FALSE;

		pFtr = ppFtrs[0];
		if( !pFtr )return FALSE;
		*/
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;		
		
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];	
	}
		
	PT_3D ret;
	if( !pObj->GetCenter(pt,&ret) )return FALSE;

	double dis = sqrt( (pt->x-ret.x)*(pt->x-ret.x) + (pt->y-ret.y)*(pt->y-ret.y) );
	//if (dis > r) return FALSE;

	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modeCenterPoint;
	item.pt = ret;

	if( ret_dis )
	{
		*ret_dis = dis;
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByIntersect(Envelope e, PT_3D *pt, CCoordSys *pCS,CGeometry* pObj0, CGeometry* pObj1, double *ret_dis)
{		
	double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;

	PT_3D tmp;
	pCS->GroundToClient(pt,&tmp);
	if( !pObj0 || !pObj1 )
	{
		m_pDataQuery->FindNearestObject(tmp,r,pCS);
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num<2 )return FALSE;

		pObj0 = ftr[0]->GetGeometry();
		if( !pObj0 )return FALSE;
		pObj1 = ftr[1]->GetGeometry();
		if( !pObj1 )return FALSE;
	}
	
	//获得两个地物在 e 范围内的线段
 	const CShapeLine  *pSL0 = pObj0->GetShape();
	if(!pSL0) return FALSE;
	const CShapeLine  *pSL1 = pObj1->GetShape();
	if(!pSL1) return FALSE;

 	int maxLen = pSL0->GetPtsCount()+pSL1->GetPtsCount();
 	if( maxLen<=0 )return FALSE;
 
 	LINE_3D *pLines = new LINE_3D[maxLen];
	if (!pLines)
	{
		return FALSE;
	}
 	int num0=0, num1=0;
	PT_3DEX *pts = NULL;
	CPtrArray arr;
	pSL0->GetShapeLineUnit(arr);
	Envelope e0;
	for( int i=0;i<arr.GetSize();i++ )
	{
		CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[i];
		e0 = pUnit->evlp;
		e0.TransformGrdToClt(pCS,1);
		if( e.bIntersect(&e0) )
		{
			pts = pUnit->pts;
			for( int j=0; j<pUnit->nuse-1; j++,pts++)
			{
				e0.CreateFromPts(pts,2,sizeof(PT_3DEX));
				e0.TransformGrdToClt(pCS,1);
				if( e.bIntersect(&e0) )
				{
					COPY_3DPT((pLines[num0].sp), (pts[0])); 
					COPY_3DPT((pLines[num0].ep), (pts[1]));
					num0++;
				}
			}
		}
		//pUnit = pUnit->next;
	}

	arr.RemoveAll();
	pSL1->GetShapeLineUnit(arr);
	num1 = num0;
	
	for (i=0;i<arr.GetSize();i++)	
	{
		CShapeLine::ShapeLineUnit *pUnit = (CShapeLine::ShapeLineUnit *)arr[i];
		e0 = pUnit->evlp;
		e0.TransformGrdToClt(pCS,1);
		if( e.bIntersect(&e0) )
		{
			pts = pUnit->pts;
			for( int j=0; j<pUnit->nuse-1; j++,pts++)
			{
				e0.CreateFromPts(pts,2,sizeof(PT_3DEX));
				e0.TransformGrdToClt(pCS,1);
				if( e.bIntersect(&e0) )
				{
					COPY_3DPT((pLines[num1].sp), (pts[0])); 
					COPY_3DPT((pLines[num1].ep), (pts[1]));
					num1++;
				}
			}
		}
	//	pUnit = pUnit->next;
	}

	//计算太多，就不再计算了
	if( (num1-num0)*num0>1000 )
		return FALSE;

	//线段之间求交，并求得最近的交点
	PT_3D t,ret;
	double dis,min=-1;
	for( i=0; i<num0; i++)
	{
		for( int j=num0; j<num1; j++)
		{
			if( GraphAPI::GGetLineIntersectLineSeg(
				pLines[i].sp.x,pLines[i].sp.y,pLines[i].ep.x,pLines[i].ep.y,
				pLines[j].sp.x,pLines[j].sp.y,pLines[j].ep.x,pLines[j].ep.y,
				&t.x,&t.y,NULL) )
			{
				dis = (t.x-pt->x)*(t.x-pt->x)+(t.y-pt->y)*(t.y-pt->y);
				if( min<0 || dis<min )
				{
					min = dis;	ret = t;
					GraphAPI::GGetPtZOfLine(pLines[i].sp.x,pLines[i].sp.y,pLines[i].sp.z,
						pLines[i].ep.x,pLines[i].ep.y,pLines[i].ep.z,&ret.x,&ret.y,&ret.z);
				}
			}
		}
	}

	delete[] pLines;

	if( min<0 )return FALSE;

	if( sqrt(min)*pCS->CalcScale()>r )return FALSE;
	
	int num;
	const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
	if( num<2 )return FALSE;
	SNAPITEM item;
	item.pFtr = ftr[0];
	item.nSnapMode = modeIntersect;
	item.pt = ret;

	if( ret_dis )
	{
		*ret_dis = sqrt( (pt->x-item.pt.x)*(pt->x-item.pt.x) + (pt->y-item.pt.y)*(pt->y-item.pt.y) );
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}

BOOL CSnap::DoSnapByPolar(Envelope e, PT_3D *lastPt,PT_3D *curPt,CCoordSys *pCS,double *ret_dis)
{
	if( !lastPt || !curPt )return FALSE;

	//double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
	double r = 10;

	PT_3D cltLastPt, cltCurPt;
	pCS->GroundToClient(lastPt,&cltLastPt);
	pCS->GroundToClient(curPt,&cltCurPt);

	double fdy = fabs(cltCurPt.y-cltLastPt.y), fdx = fabs(cltCurPt.x-cltLastPt.x);
	// 水平
	if (fdy < r)
	{
		PT_3D ptH = *curPt;
		ptH.y = lastPt->y;
		SNAPITEM item;
		item.nSnapMode = modePolar;
		item.pt = ptH;
		
		if( ret_dis )
		{
			*ret_dis = sqrt( (curPt->x-item.pt.x)*(curPt->x-item.pt.x) + (curPt->y-item.pt.y)*(curPt->y-item.pt.y) );
		}
		
		m_arrSnapItems.Add(item);

		return TRUE;
	}
	// 垂直
	else if (fdx < r)
	{
		PT_3D ptV = *curPt;
		ptV.x = lastPt->x;
		SNAPITEM item;
		item.nSnapMode = modePolar;
		item.pt = ptV;
		
		if( ret_dis )
		{
			*ret_dis = sqrt( (curPt->x-item.pt.x)*(curPt->x-item.pt.x) + (curPt->y-item.pt.y)*(curPt->y-item.pt.y) );
		}
		
		m_arrSnapItems.Add(item);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CSnap::DoSnapByPerpPoint(Envelope e, PT_3D *lastPt, PT_3D *curPt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{
	PT_3D tmp;
	pCS->GroundToClient(curPt,&tmp);
	if( !lastPt || !curPt )return FALSE;
	
	CFeature *pFtr = NULL;
	if( !pObj )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )return FALSE;
		
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;
		
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];	
	}
	

	PT_3D ret1,ret2;
	if( !pObj->FindNearestBaseLine(tmp,e,pCS,&ret1,&ret2,NULL) )
		return FALSE;
	
	PT_3D perp;
	GraphAPI::GGetPerpendicular(ret1.x,ret1.y,ret2.x,ret2.y,lastPt->x,lastPt->y,&perp.x,&perp.y,NULL);
	GraphAPI::GGetPtZOfLine(&ret1,&ret2,&perp);
	
	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modePerpPoint;
	item.pt = perp;

	if( ret_dis )
	{
		*ret_dis = sqrt( (curPt->x-item.pt.x)*(curPt->x-item.pt.x) + (curPt->y-item.pt.y)*(curPt->y-item.pt.y) );
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByTangPoint(Envelope e, PT_3D *lastPt, PT_3D *curPt, CCoordSys *pCS,CGeometry* pObj, double *ret_dis)
{
	PT_3D tmp;
	pCS->GroundToClient(curPt,&tmp);
	if( !lastPt || !curPt )return FALSE;
	
	CFeature *pFtr = NULL;
	if( !pObj )
	{
		double r = sqrt(e.Width()*e.Width()+e.Height()*e.Height())/2;
		pFtr = m_pDataQuery->FindNearestObject(tmp,r,pCS);
		if( !pFtr )return FALSE;
		
		pObj = pFtr->GetGeometry();
		if( !pObj )return FALSE;
		
	}
	else
	{
		int num;
		const CPFeature *ftr = m_pDataQuery->GetFoundHandles(num);
		if( num>0 )
			pFtr = ftr[0];	
	}
	
	if( !pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )return FALSE;
	
	SNAPITEM item;
	item.pFtr = pFtr;
	item.nSnapMode = modeTangPoint;
	if( !((CGeoCurve*)pObj)->GetTangency(lastPt,curPt,&item.pt) )return FALSE;

	if( ret_dis )
	{
		*ret_dis = sqrt( (curPt->x-item.pt.x)*(curPt->x-item.pt.x) + (curPt->y-item.pt.y)*(curPt->y-item.pt.y) );
	}
	
	m_arrSnapItems.Add(item);
	return TRUE;
}


BOOL CSnap::DoSnapByGrid(Envelope e, PT_3D *pt,CCoordSys *pCS, double *ret_dis)
{
	if( m_gridParm.nx<=0 || m_gridParm.ny<=0 ||
		m_gridParm.dx<=0 || m_gridParm.dy<=0 )
		return FALSE;

	double fnx = (pt->x-m_gridParm.ox)/m_gridParm.dx;
	double fny = (pt->y-m_gridParm.oy)/m_gridParm.dy;

	PT_3D corners[4];
	int sx = floor(fnx), sy = floor(fny), idx, min=-1;
	double dis, mindis=-1;
	for( int i=0; i<2; i++)
	{
		for( int j=0; j<2; j++)
		{
			idx = i*2+j;
			corners[idx].x = m_gridParm.ox + (sx+i)*m_gridParm.dx;
			corners[idx].y = m_gridParm.oy + (sy+j)*m_gridParm.dy;
			corners[idx].z = pt->z;

			dis = sqrt( (corners[idx].x-pt->x)*(corners[idx].x-pt->x) +
						(corners[idx].y-pt->y)*(corners[idx].y-pt->y) );
			if( mindis<0 || mindis>dis )
			{ 
				mindis = dis;
				min = idx;
			}
		}
	}

	PT_3D snap_pt_clt;
	pCS->GroundToClient(corners+min,&snap_pt_clt);

	if( !e.bPtIn(&snap_pt_clt) )
		return FALSE;

	SNAPITEM item;
	item.nSnapMode = modeGrid;
	item.pt = corners[min];
	m_arrSnapItems.Add(item);

	if( ret_dis )
	{
		*ret_dis = sqrt( (pt->x-item.pt.x)*(pt->x-item.pt.x) + (pt->y-item.pt.y)*(pt->y-item.pt.y) );
	}
	
	return TRUE;
}


int CSnap::GetSnapResult(SNAPITEM *pItems)
{
	if( pItems )
	{
		memcpy( pItems,m_arrSnapItems.GetData(),m_arrSnapItems.GetSize()*sizeof(SNAPITEM) );
	}

	return m_arrSnapItems.GetSize();
}

SNAPITEM CSnap::GetFirstSnapResult()
{
	if( m_arrSnapItems.GetSize()>0 )
		return m_arrSnapItems[0];
	return SNAPITEM();
}


void CSnap::UpdateSettings(BOOL bSave)
{
	if( !bSave )
	{
		m_lfRadius     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_RADIUS,gdef_nSnapSize);
		m_bOpen		   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_ENABLE,TRUE);           
		m_b2D		   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_IS2D,TRUE);      
		m_bSnapSelf	   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SNAPSELF,TRUE); 
		m_bOnlyBase    = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_SNAPBASE,FALSE); 
		BOOL CenterPoint   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_CENTER,0);              
		BOOL Intersect     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_INTERSECT,0);           
		BOOL KeyPoint      = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_KEY,TRUE);	          
		BOOL MidPoint      = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_MID,0);                 
		BOOL NearPoint     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_NEAR,0);                
		BOOL PerpPoint     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_PERP,0);                
		BOOL TangPoint     = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_TANG,0);   
		BOOL EndPoint	   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_END,0);  
		BOOL Polar	   = AfxGetApp()->GetProfileInt(REGPATH_SNAP,REGITEM_POLAR,0);
		
		m_nMode = 0;
		if(PerpPoint)  m_nMode  |= CSnap::modePerpPoint;
		if(CenterPoint)m_nMode  |= CSnap::modeCenterPoint;
		if(TangPoint)  m_nMode  |= CSnap::modeTangPoint;
		if(Intersect)  m_nMode  |= CSnap::modeIntersect;
		if(KeyPoint)   m_nMode  |= CSnap::modeKeyPoint;
		if(MidPoint)   m_nMode  |= CSnap::modeMidPoint;
		if(NearPoint)  m_nMode  |= CSnap::modeNearPoint;
		if(EndPoint)   m_nMode  |= CSnap::modeEndPoint;
		if(Polar)	   m_nMode  |= CSnap::modePolar;
	}
	else
	{
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP, REGITEM_RADIUS,(int)m_lfRadius);
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_ENABLE,m_bOpen);
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_IS2D,	m_b2D);
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_SNAPSELF,	m_bSnapSelf);
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_SNAPBASE,	m_bOnlyBase);
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_CENTER,((m_nMode&modeCenterPoint)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_INTERSECT,((m_nMode&modeIntersect)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_KEY,((m_nMode&modeKeyPoint)!=0));	
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_MID,((m_nMode&modeMidPoint)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_NEAR,((m_nMode&modeNearPoint)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_PERP,((m_nMode&modePerpPoint)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_TANG,((m_nMode&modeTangPoint)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_END,((m_nMode&modeEndPoint)!=0));
		AfxGetApp()->WriteProfileInt(REGPATH_SNAP,REGITEM_POLAR,((m_nMode&modePolar)!=0));
	}
}

void CSnap::Init(CDataQueryEx *pDQ)
{
	ASSERT(pDQ!=NULL);
	m_pDataQuery = pDQ;
}


MyNameSpaceEnd