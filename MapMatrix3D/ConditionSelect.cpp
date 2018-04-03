#include "stdafx.h"
#include "SmartViewFunctions.h"
#include "editbasedoc.h"
#include "ConditionSelect.h"
#include "RegDef.h"
#include "RegDef2.h"
#include "DataSource.h"
#include "DlgDataSource.h"
#include "Geometry.h"
#include "EditBase.h"
#include "GeoCurve.h"
#include "GeoText.h"
#include "Functions_temp.h"
#include "SymbolLib.h"


extern void PenCodeToText(int pencode, char* text);
extern CString FirstStrFromResID(UINT id);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CCPResultList::CombineList(CCPResultList& list)
{
	//合并属性表，记录下属性对应关系
	CArray<int,int> idxs;
	int na1 = attrNameList.GetSize(), na2 = list.attrNameList.GetSize();
	idxs.SetSize(na2);
	for( int i=0; i<na2; i++)
	{
		CString str1 = list.attrNameList.GetAt(i);
		for( int j=0; j<na1; j++)
		{
			if( str1.Compare(attrNameList.GetAt(j))==0 )
				break;
		}
		
		if( j<na1 )idxs.SetAt(i,j);
		else
		{
			idxs.SetAt(i,attrNameList.Add(str1));
		}
	}
	
	int nr = list.resultList.GetSize();
	for( i=0; i<nr; i++)
	{
		CP_RESULT *r = (CP_RESULT*)list.resultList.GetAt(i);
		if( !r )continue;
		
		StartRecord();
		SetCurFtr(r->m_pFtr);
		int na = r->arrAttrs.GetSize();
		CP_ATTR *attr = r->arrAttrs.GetData();
		for( int j=0; j<na; j++)
		{
			int save = attr[j].index;
			attr[j].index = idxs[attr[j].index];
			AddAttrResult(attr+j);
			attr[j].index = save;
		}
		
		na = r->arrPoints.GetSize();
		for( j=0; j<na; j++)
		{
			CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
			CP_POINT pt2;
			pt2 = *pt;
			
			int na2 = pt2.arrAttrs.GetSize();
			attr = pt2.arrAttrs.GetData();
			for( int m=0; m<na2; m++)
			{
				attr[m].index = idxs[attr[m].index];
			}
			
			AddPointResult(&pt2);
		}
		
		FinishRecord();
	}
}


CCPResultList::CCPResultList()
{
	curResult.m_pFtr = NULL;
	nCurAttr = -1;
	//	nCombineType = -1;
}

CCPResultList::CCPResultList(CStringArray& nameList)
{
	attrNameList.Copy(nameList);
	curResult.m_pFtr = NULL;
	nCurAttr = -1;
	//	nCombineType = -1;
}

CCPResultList::~CCPResultList()
{
	Clear();
}

void CCPResultList::Clear()
{
	int nr = resultList.GetSize();
	for( int i=0; i<nr; i++)
	{
		CP_RESULT* r = (CP_RESULT*)resultList[i];
		if( !r )continue;
		r->Clear();
		delete r;
	}
	
	curResult.Clear();
	resultList.RemoveAll();
}

void CCPResultList::Copy(CCPResultList& list)
{
	Clear();
	
	attrNameList.Copy(list.attrNameList);
	
	int nr = list.resultList.GetSize();
	for( int i=0; i<nr; i++)
	{
		CP_RESULT *r = (CP_RESULT*)list.resultList.GetAt(i);
		if( !r )continue;
		
		StartRecord();
		SetCurFtr(r->m_pFtr);
		int na = r->arrAttrs.GetSize();
		CP_ATTR *attr = r->arrAttrs.GetData();
		for( int j=0; j<na; j++)
		{
			AddAttrResult(attr+j);
		}
		
		na = r->arrPoints.GetSize();
		for( j=0; j<na; j++)
		{
			CP_POINT *pt = (CP_POINT*)r->arrPoints.GetAt(j);
			AddPointResult(pt);
		}
		
		FinishRecord();
	}
}


void CCPResultList::SubtractList(CCPResultList& list)
{
	int nr = list.resultList.GetSize();
	int nr2 = resultList.GetSize();
	for( int i=nr2-1; i>=0; i--)
	{
		CP_RESULT *r2 = (CP_RESULT*)resultList.GetAt(i);
		if( !r2 )continue;
		
		for( int j=0; j<nr; j++)
		{
			CP_RESULT *r = (CP_RESULT*)list.resultList.GetAt(j);
			if( !r )continue;
			
			if( r->m_pFtr==r2->m_pFtr )break;
		}
		
		if( j<nr )
		{
			r2->Clear();
			delete r2;
			resultList.RemoveAt(i);
		}
	}
}

void CCPResultList::Combine()
{
	if( curResult.arrAttrs.GetSize()<=0 && curResult.arrPoints.GetSize()<=0 )
		return;
	
	int nsz = attrNameList.GetSize(), i, j, max = 0, num;
	CP_ATTR attr;
	
	int *list = new int[nsz];
	if( !list )return;
	memset(list,0,sizeof(int)*nsz);
	
	//合并属性表
	num = curResult.arrAttrs.GetSize();
	for( i=0; i<num; i++)
	{
		attr = curResult.arrAttrs.GetAt(i);
		list[attr.index]++;
	}
	
	for( i=num-1; i>=0; i--)
	{
		attr = curResult.arrAttrs.GetAt(i);
		if( list[attr.index]>0 )list[attr.index] = -1;
		else if( list[attr.index]==-1 )
		{
			curResult.arrAttrs.RemoveAt(i);
		}
	}
	
	//合并每个节点的属性表
	int np = curResult.arrPoints.GetSize();
	for( j=0; j<np; j++)
	{
		memset(list,0,sizeof(int)*nsz);
		
		CP_POINT *pt = (CP_POINT*)curResult.arrPoints.GetAt(j);
		num = pt->arrAttrs.GetSize();
		for( i=0; i<num; i++)
		{
			attr = pt->arrAttrs.GetAt(i);
			list[attr.index]++;
		}
		
		for( i=num-1; i>=0; i--)
		{
			attr = pt->arrAttrs.GetAt(i);
			if( list[attr.index]>0 )list[attr.index] = -1;
			else if( list[attr.index]==-1 )
			{
				pt->arrAttrs.RemoveAt(i);
			}
		}
	}
	delete[] list;
}

void CCPResultList::StartRecord()
{
	//	nCombineType = type;
	curResult.Clear();
}
void CCPResultList::AbortRecord()
{
	curResult.Clear();
}
void CCPResultList::FinishRecord()
{
	Combine();
	
	if( curResult.arrAttrs.GetSize()<=0 && curResult.arrPoints.GetSize()<=0 )
		return;
	
	CP_RESULT *r = new CP_RESULT;
	if( !r )return;
	*r = curResult;
	resultList.Add(r);
	
	curResult.Clear();
}
void CCPResultList::SetCurFtr(CFeature *pFt)
{
	curResult.m_pFtr = pFt;
}
void CCPResultList::SetCurAttr(int idx)
{
	if( idx>=0 && idx<attrNameList.GetSize() )
		nCurAttr = idx;
}
void CCPResultList::AddAttrResult(_variant_t value)
{
	CP_ATTR attr;
	attr.index = nCurAttr;
	attr.value = value;
	curResult.arrAttrs.Add(attr);
}
void CCPResultList::AddAttrResult(CP_ATTR *attr)
{
	curResult.arrAttrs.Add(*attr);
}
void CCPResultList::AddPointResult(int ptidx, _variant_t value)
{
	CP_POINT pt;
	pt.ptidx = ptidx;
	
	CP_ATTR attr;
	attr.index = nCurAttr;
	attr.value = value;
	pt.arrAttrs.Add(attr);
	
	AddPointResult(&pt);
}
void CCPResultList::AddPointResult(CP_POINT* pt)
{
	CP_POINT *p = new CP_POINT, *tpt = NULL;
	if( !p )return;
	*p = *pt;
	
	//由小到大排序插入
	int nsz = curResult.arrPoints.GetSize(), i;
	for( i=0; i<nsz; i++)
	{
		tpt = (CP_POINT*)curResult.arrPoints.GetAt(i);
		if( p->ptidx<=tpt->ptidx )break;
	}
	
	if( i>=nsz )
		curResult.arrPoints.Add(p);
	else if( p->ptidx==tpt->ptidx )
	{
		tpt->arrAttrs.Append(p->arrAttrs);
		delete p;
	}
	else
		curResult.arrPoints.InsertAt(i,p);
}


CSearchMultiObj::CSearchMultiObj()
{
	m_lfOX = m_lfOY = 0;
	m_lfDX = m_lfDY = 1;
	m_nx = m_ny = 0;
	m_bCalcGrid = FALSE;
	
	m_pFuncCheckObj = NULL;
	
	m_pDataSource = NULL;
	
	m_pCoorSys = NULL;
}


CSearchMultiObj::~CSearchMultiObj()
{
	Clear();
}

void CSearchMultiObj::Init(CDlgDataSource *pDataSouce,CCoordSys *pCoordSys)
{
	m_pDataSource = pDataSouce;
	m_pCoorSys = pCoordSys;
}

void CSearchMultiObj::Prepare(int intensity)
{
	
	if( !m_pDataSource )return;
	
	Envelope evlp;
	CFeature *pFtr = NULL;
	
	long nptsum = 0;
	for (int i=0; i<m_pDataSource->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDataSource->GetFtrLayerByIndex(i);
		if (!pLayer|| !pLayer->IsVisible() )
			continue;
		
		int nobj = pLayer->GetObjectCount();
		for (int j=0;j<nobj;j++)
		{
			GrBuffer buf;
			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;
			if( m_pFuncCheckObj && !(*m_pFuncCheckObj)(pFtr,pLayer) )
				continue;
			
			CGeometry *pObj = pFtr->GetGeometry();
			nptsum += pObj->GetDataPointSum();
			
			pFtr->Draw(&buf,m_pDataSource->GetSymbolDrawScale());
			evlp.Union(&buf.GetEnvelope());
		}
		
	}
	
	if( evlp.IsEmpty() || nptsum<=0 )return;
	
	if( intensity>10 )intensity = 10;
	else if( intensity<1 )intensity = 1;
	
	intensity *= 100;
	
	nptsum = sqrt(nptsum>intensity?(nptsum/intensity):1);
	
	m_lfDX = (evlp.m_xh-evlp.m_xl)/nptsum;
	m_lfDY = (evlp.m_yh-evlp.m_yl)/nptsum;
	m_lfOX = evlp.m_xl-m_lfDX/2; m_lfOY = evlp.m_yl-m_lfDY/2;
	m_nx = nptsum+1; m_ny = nptsum+1;
	m_arrObjs.SetSize(m_nx*m_ny);
	memset(m_arrObjs.GetData(),0,sizeof(void*)*m_nx*m_ny);
}


void CSearchMultiObj::PrepareWithRadius(double r)
{
	
	if( !m_pDataSource )return;
	if( r<=0.0 )return;
	
	Envelope evlp;
	CFeature *pFtr = NULL;
	
	long nptsum = 0;
	for (int i=0; i<m_pDataSource->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDataSource->GetFtrLayerByIndex(i);
		if ( !pLayer||!pLayer->IsVisible() )
			continue;
		
		int nobj = pLayer->GetObjectCount();
		for (int j=0;j<nobj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;
			if( m_pFuncCheckObj && !(*m_pFuncCheckObj)(pFtr,pLayer) )
				continue;
			
			CGeometry *pObj = pFtr->GetGeometry();
			nptsum += pObj->GetDataPointSum();
			GrBuffer buf;
			pFtr->Draw(&buf,m_pDataSource->GetSymbolDrawScale());
			evlp.Union(&buf.GetEnvelope());
		}
		
	}
	
	if( evlp.IsEmpty() || nptsum<=0 )return;
	
	m_lfDX = r;
	m_lfDY = r;
	m_lfOX = evlp.m_xl-m_lfDX/2; m_lfOY = evlp.m_yl-m_lfDY/2;
	m_nx = (evlp.m_xh-evlp.m_xl)/m_lfDX+1; m_ny = (evlp.m_yh-evlp.m_yl)/m_lfDY+1;
	m_arrObjs.SetSize(m_nx*m_ny);
	memset(m_arrObjs.GetData(),0,sizeof(void*)*m_nx*m_ny);
}

void CSearchMultiObj::CalcGridForPoint()
{
	if( m_bCalcGrid || !m_pDataSource )return;
	
	//依次遍历所有的地物
	for (int i=0; i<m_pDataSource->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDataSource->GetFtrLayerByIndex(i);
		if ( !pLayer||!pLayer->IsVisible() )
			continue;
		
		int nobj = pLayer->GetObjectCount();
		for (int j=0;j<nobj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;
			if( m_pFuncCheckObj && !(*m_pFuncCheckObj)(pFtr,pLayer) )
				continue;
			
			CGeometry *pObj = pFtr->GetGeometry();
			//遍历所有的节点
			CArray<int,int> idxs;
			CArray<PT_3DEX,PT_3DEX> arr;
			pObj->GetShape(arr);
			int npt = arr.GetSize(), idx, oldx = -1, oldy = -1, nsz;
			PT_3DEX expt;
			for( int k=0; k<npt; k++)
			{
				expt = arr.GetAt(k);
				
				//计算当前节点处在哪个格网中
				int x = floor((expt.x-m_lfOX)/m_lfDX), y = floor((expt.y-m_lfOY)/m_lfDY);
				if( x>=0 && y>=0 && x<m_nx && y<m_ny )
				{
					if( x==oldx && y==oldy )continue;
					idx = y*m_nx+x;
					nsz = idxs.GetSize();
					for( int m=0; m<nsz; m++)
					{
						if( idx==idxs[m] )break;
						if( idx>idxs[m] )
						{
							idxs.InsertAt(m,idx);
							break;
						}
					}
					if( m>=nsz )idxs.Add(idx);
					oldx = x; oldy = y;
				}
			}
			
			//在所覆盖到的格网中注册当前地物
			nsz = idxs.GetSize();
			for( int m=0; m<nsz; m++)
			{
				CPtrArray *p = (CPtrArray*)m_arrObjs.GetAt(idxs[m]);
				if( !p )
				{
					p = new CPtrArray;
					if( !p )continue;
					m_arrObjs.SetAt(idxs[m],p);
				}
				
				p->Add(pLayer);
				p->Add(pFtr);
			}
		}
		
	}
	
	m_bCalcGrid = TRUE;
}

void CSearchMultiObj::CalcGridForLine()
{
	
	if( m_bCalcGrid || !m_pDataSource )return;
	
	
	Graph *pGraph = NULL;
	
	//依次遍历所有的地物
	for (int i=0; i<m_pDataSource->GetFtrLayerCount(); i++)
	{
		CFtrLayer *pLayer = m_pDataSource->GetFtrLayerByIndex(i);
		
		if ( !pLayer||!pLayer->IsVisible() )
			continue;
		
		int nobj = pLayer->GetObjectCount();
		for (int j=0;j<nobj;j++)
		{
			CFeature *pFtr = pLayer->GetObject(j);
			if( !pFtr )continue;
			if( m_pFuncCheckObj && !(*m_pFuncCheckObj)(pFtr,pLayer) )
				continue;
			GrBuffer buf;
			pFtr->Draw(&buf,m_pDataSource->GetSymbolDrawScale());
			
			CGeometry *pObj = pFtr->GetGeometry();
			//遍历所有的基线点
			CArray<int,int> idxs;
			int npt = pObj->GetDataPointSum(), idx, oldx = -1, oldy = -1, nsz;
			PT_3DEX expt;
			
			pGraph = buf.HeadGraph();
			while( pGraph )
			{
				Envelope e = GetEnvelopeOfGraph(pGraph);
				
				PT_3DEX rect[4];
				rect[0].x = e.m_xl;
				rect[0].y = e.m_yl;
				rect[1].x = e.m_xl;
				rect[1].y = e.m_yh;
				rect[2].x = e.m_xh;
				rect[2].y = e.m_yl;
				rect[3].x = e.m_xh;
				rect[3].y = e.m_yh;
				
				int x = floor((rect[0].x-m_lfOX)/m_lfDX), y = floor((rect[0].y-m_lfOY)/m_lfDY);
				
				int nMinX=x,nMaxX=x,nMinY=y,nMaxY=y;
				
				int pts = 1;
				while (pts < 4)
				{
					
					x = floor((rect[pts].x-m_lfOX)/m_lfDX);
					y = floor((rect[pts].y-m_lfOY)/m_lfDY);
					
					if (x < nMinX)  nMinX = x;
					if (x > nMaxX)  nMaxX = x;
					if (y < nMinY)  nMinY = y;
					if (y > nMaxY)  nMaxY = y;

					pts++;
					
				}

				if( nMinX<0 || nMinY<0 || nMaxX>=m_nx || nMaxY>=m_ny )
				{
					pGraph = pGraph->next;
					continue;
				}
				
				for (x=nMinX; x<=nMaxX; x++)
				{
					for (y=nMinY; y<=nMaxY; y++)
					{
						if( x>=0 && y>=0 && x<m_nx && y<m_ny )
						{
							if( x==oldx && y==oldy )continue;
							idx = y*m_nx+x;
							nsz = idxs.GetSize();
							for( int m=0; m<nsz; m++)
							{
								if( idx==idxs[m] )break;
								if( idx>idxs[m] )
								{
									idxs.InsertAt(m,idx);
									break;
								}
							}
							if( m>=nsz )idxs.Add(idx);
							oldx = x; oldy = y;
						}
						
					}
				}
				
				pGraph = pGraph->next;
			}
			
			//在所覆盖到的格网中注册当前地物
			nsz = idxs.GetSize();
			for( int m=0; m<nsz; m++)
			{
				CPtrArray *p = (CPtrArray*)m_arrObjs.GetAt(idxs[m]);
				if( !p )
				{
					p = new CPtrArray;
					if( !p )continue;
					m_arrObjs.SetAt(idxs[m],p);
				}
				
				p->Add(pLayer);
				p->Add(pFtr);
			}
		}
		
	}
	
	m_bCalcGrid = TRUE;
}

void CSearchMultiObj::Clear()
{
	int nsz = m_arrObjs.GetSize();
	for(int i=0; i<nsz; i++)
	{
		CPtrArray *p = (CPtrArray*)m_arrObjs.GetAt(i);
		if( p )delete p;
	}
	m_arrObjs.RemoveAll();
	m_bCalcGrid = FALSE;
}


int CSearchMultiObj::FindObjectInRect(CFeature *pFtr0, PT_3D *pt, Envelope evlp, BOOL bForPoint)
{
	if( bForPoint )CalcGridForPoint();
	else CalcGridForLine();
	
	m_foundHandles.RemoveAll();
	double x = (pt->x-m_lfOX)/m_lfDX, y = (pt->y-m_lfOY)/m_lfDY;
	if( x<0 || y<0 || x>=m_nx || y>=m_ny )return -1;
	
	int nx = floor(x), ny = floor(y);
	CPtrArray *p = (CPtrArray*)m_arrObjs.GetAt(ny*m_nx+nx);
	if( !p )return -1;
	
	CFeature *pFtr = NULL;
	
	Graph *pGraph = NULL;
	GrVertex *pts = NULL;
	
	int nobj = p->GetSize()/2;
	for (int j=0;j<nobj;j++)
	{
		CFtrLayer *pLayer = (CFtrLayer*)p->GetAt(j*2);
		pFtr = (CFeature*)p->GetAt(j*2+1);
		if( !pFtr || pFtr==pFtr0 )continue;
		GrBuffer buf;
		pFtr->Draw(&buf,m_pDataSource->GetSymbolDrawScale());
		if( !evlp.bIntersect(&buf.GetEnvelope()) )continue;
		
		pGraph = buf.HeadGraph();
		while( pGraph )
		{
			if (evlp.bIntersect(&GetEnvelopeOfGraph(pGraph)))
			{
				if (IsGrPoint(pGraph))
				{
					GrPoint *pPoint = (GrPoint*)pGraph;
					if (evlp.bPtIn(&pPoint->pt))
					{
						m_foundHandles.Add(FtrToHandle(pFtr));
						break;
					}
				}
				if(IsGrLineString(pGraph))
				{
					GrLineString *pLine = (GrLineString*)pGraph;
					
					pts = pLine->ptlist.pts;
					for( int k=0; k<pLine->ptlist.nuse-1; k++,pts++)
					{
						if( bForPoint )
						{
							if( IsGrPtCodeLineTo(pts+1) && (evlp.bPtIn(pts)||evlp.bPtIn(&pts[1])) )break;
						}
						else
						{
							if( IsGrPtCodeLineTo(pts+1) && evlp.bIntersect(pts,&pts[1]) )break;
						}
					}
					
					if( k<pLine->ptlist.nuse-1 || (pLine->ptlist.nuse==1 && evlp.bPtIn(pts)) )
					{
						m_foundHandles.Add(FtrToHandle(pFtr));
						break;
					}
					
				}
				else if(IsGrPolygon(pGraph))
				{
					GrPolygon *pPloy = (GrPolygon*)pGraph;
					
					pts = pPloy->ptlist.pts;
					for( int k=0; k<pPloy->ptlist.nuse-1; k++,pts++)
					{
						if( bForPoint )
						{
							if( IsGrPtCodeLineTo(pts+1) && (evlp.bPtIn(pts)||evlp.bPtIn(&pts[1])) )break;
						}
						else
						{
							if( IsGrPtCodeLineTo(pts+1) && evlp.bIntersect(pts,&pts[1]) )break;
						}
					}
					
					if( k<pPloy->ptlist.nuse-1 || (pPloy->ptlist.nuse==1 && evlp.bPtIn(pts)) )
					{
						m_foundHandles.Add(FtrToHandle(pFtr));
						break;
					}
					
				}
				else if(IsGrText(pGraph))
				{
					GrText *pText = (GrText*)pGraph;
					if (evlp.bPtIn(&pText->pt))
					{
						m_foundHandles.Add(FtrToHandle(pFtr));
						break;
					}
				}
				
			}
		
			pGraph = pGraph->next;
		}
	}
	
	return m_foundHandles.GetSize();
}


int CSearchMultiObj::FindIntersectInRect(CFeature *pFtr0, PT_3D pt0, PT_3D pt1)
{
	
	CalcGridForLine();
	m_foundHandles.RemoveAll();
	double x0 = (pt0.x-m_lfOX)/m_lfDX, y0 = (pt0.y-m_lfOY)/m_lfDY;
	if( x0<0 || y0<0 || x0>=m_nx || y0>=m_ny )return -1;
	
	double x1 = (pt1.x-m_lfOX)/m_lfDX, y1 = (pt1.y-m_lfOY)/m_lfDY;
	if( x1<0 || y1<0 || x1>=m_nx || y1>=m_ny )return -1;
	
	int nx0 = floor(x0), ny0 = floor(y0);
	int nx1 = floor(x1), ny1 = floor(y1);
	
	int stepx = nx1>nx0?1:-1, stepy = ny1>ny0?1:-1;
	
	Envelope evlp,evlp2;
	PT_3D pts3d[2];
	pts3d[0] = pt0, pts3d[1] = pt1;
	evlp.CreateFromPts(pts3d,2,sizeof(PT_3D));
	
	while( 1 )
	{
		int ny = ny0;
		while( 1 )
		{
			CPtrArray *p = (CPtrArray*)m_arrObjs.GetAt(ny*m_nx+nx0);
			if( !p )
			{
				if( ny==ny1 )break;
				ny += stepy;
				continue;
			}
			
			CFeature *pFtr = NULL;
			
			Graph *pGraph = NULL;
			
			int nobj = p->GetSize()/2;
			for (int j=0;j<nobj;j++)
			{
				CFtrLayer *pLayer = (CFtrLayer*)p->GetAt(j*2);
				pFtr = (CFeature*)p->GetAt(j*2+1);
				if( !pFtr || pFtr==pFtr0 )continue;
				GrBuffer buf;
				pFtr->Draw(&buf,m_pDataSource->GetSymbolDrawScale());
				if( !evlp.bIntersect(&buf.GetEnvelope()) )continue;
				
				pGraph = buf.HeadGraph();
				while( pGraph )
				{		
					
					if (evlp.bIntersect(&GetEnvelopeOfGraph(pGraph)))
					{
						GrVertexList *pList = NULL;
						if(IsGrLineString(pGraph))
						{
							GrLineString *pLine = (GrLineString*)pGraph;
							
							pList = &pLine->ptlist;
							
						}
						else if(IsGrPolygon(pGraph))
						{
							GrPolygon *pPloy = (GrPolygon*)pGraph;
							
							pList = &pPloy->ptlist;
							
						}
						
						if (pList != NULL)
						{
							GrVertex *pts = pList->pts;
							for( int k=0; k<pList->nuse-1; k++,pts++)
							{
								if (!IsGrPtCodeLineTo(pts+1)) continue;
								COPY_3DPT(pts3d[0],pts[0]);
								COPY_3DPT(pts3d[1],pts[1]);
								evlp2.CreateFromPts(pts3d,2,sizeof(PT_3D));
								if( !evlp.bIntersect(&evlp2) )continue;
								
								if( (fabs(pt0.x-pts[0].x)<1e-4&&fabs(pt0.y-pts[0].y)<1e-4)||
									(fabs(pt1.x-pts[0].x)<1e-4&&fabs(pt1.y-pts[0].y)<1e-4)||
									(fabs(pt0.x-pts[1].x)<1e-4&&fabs(pt0.y-pts[1].y)<1e-4)||
									(fabs(pt1.x-pts[1].x)<1e-4&&fabs(pt1.y-pts[1].y)<1e-4) )
									continue;
								
								if( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt1.x,pt1.y,
									pts[0].x,pts[0].y,pts[1].x,pts[1].y,NULL,NULL,NULL) )break;
							}
							
							if (k < pList->nuse-1)
							{
								m_foundHandles.Add(FtrToHandle(pFtr));
								break;
							}
						}
					}
					
					pGraph = pGraph->next;
				}
			}
			if( ny==ny1 )break;
			ny += stepy;
		}
		if( nx0==nx1 )break;
		nx0 += stepx;
	}
	
	return m_foundHandles.GetSize();
}

int CSearchMultiObj::GetFoundObjectHandles(FTR_HANDLE *pFtrHandle)
{
	if (m_foundHandles.GetSize()<1)
	{
		return 0;
	}
//	pFtr = (CFeature*)m_foundHandles[0];
	if( pFtrHandle )
	{
		memcpy(pFtrHandle,m_foundHandles.GetData(),sizeof(FTR_HANDLE)*m_foundHandles.GetSize());
	}
	return m_foundHandles.GetSize();
}

CLinesSearch::CLinesSearch()
{
	m_pArrIndex = NULL;
	m_nx = m_ny = 0;
	m_lfDX = m_lfDY = m_lfOX = m_lfOY = 0;
}


CLinesSearch::~CLinesSearch()
{
	m_arrFound.RemoveAll();
	if( m_pArrIndex )delete[] m_pArrIndex;
}


void CLinesSearch::Init(const PT_3D *pts, int num)
{
	m_arrFound.RemoveAll();
	if( m_pArrIndex )delete[] m_pArrIndex;
	
	Envelope evlp;
	evlp.CreateFromPts(pts,num);
	
	int nblksum = num/20;
	if( nblksum<=0 )nblksum = 1;
	
	m_lfDX = (evlp.m_xh-evlp.m_xl)/nblksum;
	m_lfDY = (evlp.m_yh-evlp.m_yl)/nblksum;
	m_lfOX = evlp.m_xl-m_lfDX/2; m_lfOY = evlp.m_yl-m_lfDY/2;
	m_nx = nblksum+1; m_ny = nblksum+1;
	m_pArrIndex = new CArray<int,int>[m_nx*m_ny];
	if( m_pArrIndex==NULL )return;
	
	//遍历所有的基线点
	CArray<int,int> *pidxs;
	int idx, oldx = -1, oldy = -1, nsz;
	const PT_3D *pts0 = pts;
	
	for( int k=0; k<num-1; k++,pts++)
	{			
		//计算当前线段覆盖了哪些格网
		int x1 = floor((pts[0].x-m_lfOX)/m_lfDX), y1 = floor((pts[0].y-m_lfOY)/m_lfDY);
		int x2 = floor((pts[1].x-m_lfOX)/m_lfDX), y2 = floor((pts[1].y-m_lfOY)/m_lfDY);
		
		//单个网格
		if( x1==x2 && y1==y2 )
		{
			if( x1==oldx && y1==oldy )continue;				
			idx = y1*m_nx+x1;
			pidxs = m_pArrIndex+idx;
			nsz = pidxs->GetSize();
			for( int m=0; m<nsz; m++)
			{
				if( k==pidxs->GetAt(m) )break;
				if( k>pidxs->GetAt(m) )
				{
					pidxs->InsertAt(m,k);
					break;
				}
			}
			if( m>=nsz )pidxs->Add(k);
		}
		//将可能穿越的网格记录下来
		else
		{
			int stepx = x2>x1?1:-1, stepy = y2>y1?1:-1;
			while( 1 )
			{
				int y = y1;
				while( 1 )
				{
					idx = y*m_nx+x1;
					pidxs = m_pArrIndex+idx;
					nsz = pidxs->GetSize();
					for( int m=0; m<nsz; m++)
					{
						if( k==pidxs->GetAt(m) )break;
						if( k>pidxs->GetAt(m) )
						{
							pidxs->InsertAt(m,k);
							break;
						}
					}
					if( m>=nsz )pidxs->Add(k);
					if( y==y2 )break;
					y += stepy;
				}
				if( x1==x2 )break;
				x1 += stepx;
			}
		}
	}
}


int *CLinesSearch::FindNearLines(PT_3D pt0, PT_3D pt1, int &num)
{
	m_arrFound.RemoveAll();
	num = 0;
	
	double x0 = (pt0.x-m_lfOX)/m_lfDX, y0 = (pt0.y-m_lfOY)/m_lfDY;
	if( x0<0 || y0<0 || x0>=m_nx || y0>=m_ny )return NULL;
	
	double x1 = (pt1.x-m_lfOX)/m_lfDX, y1 = (pt1.y-m_lfOY)/m_lfDY;
	if( x1<0 || y1<0 || x1>=m_nx || y1>=m_ny )return NULL;
	
	int nx0 = floor(x0), ny0 = floor(y0);
	int nx1 = floor(x1), ny1 = floor(y1);
	
	int stepx = nx1>nx0?1:-1, stepy = ny1>ny0?1:-1;
	int nx = nx0, ny = ny0;
	CArray<int,int> *p;
	
	while( 1 )
	{
		nx = nx0;
		while( 1 )
		{
			p = m_pArrIndex+(ny*m_nx+nx);
			if( p->GetSize()!=0 )
			{
				m_arrFound.Append(*p);
			}
			
			if( nx==nx1 )break;
			nx += stepx;
		}
		
		if( ny==ny1 )break;
		ny += stepy;
	}
	
	num = m_arrFound.GetSize();
	return m_arrFound.GetData();
}


//比较地物任意属性
BOOL CompField(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	if (!pDataSource)  return FALSE;

	int ncur = COMP_CURFALSE;

	if (stricmp(strField,FIELDNAME_LAYERNAME) == 0 || stricmp(strField,FIELDNAME_LAYMAPNAME) == 0)
	{
		CFtrLayer *pLayer = pDataSource->GetFtrLayerOfObject(pFtr);
		if (!pLayer) return COMP_CURFALSE;

		_variant_t v((const char*)(LPCTSTR)(strValue));
		if (stricmp(strField,FIELDNAME_LAYERNAME) == 0)
		{
			if (IsDigital(strValue))
			{
				__int64 nCode = _atoi64(strValue);

				CConfigLibManager * pManager = gpCfgLibMan;
				if(!pManager) return FALSE;
				CScheme *pScheme = pManager->GetScheme(pDataSource->GetScale());
				if(!pScheme) return FALSE;
				
				__int64 code = 0;
				CString name = (LPCTSTR)pLayer->GetName();
				
				if(pScheme->FindLayerIdx(FALSE,code,name) && code == nCode)
				{
					r->AddAttrResult(v);
					return COMP_CURTRUE;
				}
			} 
			

			if ((nOp == OP_EQUAL && stricmp(strValue,pLayer->GetName()) == 0) ||
				(nOp == OP_UNEQUAL && stricmp(strValue,pLayer->GetName()) != 0))
			{
				r->AddAttrResult(v);
				return COMP_CURTRUE;
			}
			
		}		
		else if (stricmp(strField,FIELDNAME_LAYMAPNAME) == 0)
		{
			if ((nOp == OP_EQUAL && stricmp(strValue,pLayer->GetMapName()) == 0) ||
				(nOp == OP_UNEQUAL && stricmp(strValue,pLayer->GetMapName()) != 0))
			{
				r->AddAttrResult(v);
				return COMP_CURTRUE;
			}
		}

		return COMP_CURFALSE;
	}
	
	if (stricmp(strField,FIELDNAME_GEOCLASS) == 0)
	{
		if (stricmp(strValue,StrFromResID(IDS_ALLTYPES)) == 0)
			ncur = COMP_CURTRUE;
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_DOT_DOT)) == 0)
		{
			strValue.Format("%d",CLS_GEOPOINT);
		}
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_DOT_VECTORDOT)) == 0)
		{
			strValue.Format("%d",CLS_GEODIRPOINT);
		}
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_LINE_LINE)) == 0)
		{
			strValue.Format("%d",CLS_GEOCURVE);	
		}
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_LINE_DLINE)) == 0)
		{
			strValue.Format("%d",CLS_GEODCURVE);	
		}
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_LINE_PARALLEL)) == 0)
		{
			strValue.Format("%d",CLS_GEOPARALLEL);	
		}
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_FACE_FACE)) == 0)
		{
			strValue.Format("%d",CLS_GEOSURFACE);
		}
		else if (stricmp(strValue,FirstStrFromResID(ID_ELEMENT_TEXT)) == 0)
		{
			strValue.Format("%d",CLS_GEOTEXT);
		}
	}
	
	CValueTable table;
	table.BeginAddValueItem();
	pFtr->WriteTo(table);
	table.EndAddValueItem();
	
	const CVariantEx *pVar; 
	if ( table.GetValue(0,strField,pVar) )
	{
		CVariantEx value, v = *pVar;
		// 颜色值若是ByLayer(-1)，则设为层的颜色
		if (strField.CompareNoCase(FIELDNAME_GEOCOLOR) == 0)
		{
			CVariantEx color = (_variant_t)(long)-1;
			// 获取层的颜色
			if (color == v)
			{
				CFtrLayer *pLayer = pDataSource->GetFtrLayerOfObject(pFtr);
				if (!pLayer)  return FALSE;

				v = (_variant_t)(long)pLayer->GetColor();
			}
		}
		
		if (value.CreateFromString(strValue,v.GetType()))
		{
			if( nOp==OP_EQUAL )
			{
				if( v==value )ncur = COMP_CURTRUE;
			}
			else 
			{
				if( nOp==OP_GREATER )
				{
					if( v>value )ncur = COMP_CURTRUE;
				}
				else if( nOp==OP_LESS )
				{
					if( v<value )ncur = COMP_CURTRUE;
				}
				else if( v!=value )ncur = COMP_CURTRUE;
			}
			
			if (ncur == COMP_CURTRUE)
			{
				r->AddAttrResult(v.m_variant);
				
			}
		}
		
	}
	else
	{
		CAttributesSource *attSource = pDataSource->GetXAttributesSource();
		if (attSource)
		{
			table.DelAll();
			table.BeginAddValueItem();
			attSource->GetXAttributes(pFtr,table);
			table.EndAddValueItem();
			
			if ( table.GetValue(0,strField,pVar) )
			{
				CVariantEx value, v = *pVar;
				if (value.CreateFromString(strValue,v.GetType()))
				{
					if( nOp==OP_EQUAL )
					{
						if( v==value )ncur = COMP_CURTRUE;
					}
					else 
					{
						if( nOp==OP_GREATER )
						{
							if( v>value )ncur = COMP_CURTRUE;
						}
						else if( nOp==OP_LESS )
						{
							if( v<value )ncur = COMP_CURTRUE;
						}
						else if( v!=value )ncur = COMP_CURTRUE;
					}
					
					if (ncur == COMP_CURTRUE)
					{
						r->AddAttrResult(v.m_variant);
						
					}
				}
				
			}
		}
		
		
	}
	
	return ncur;
	
}

//比较节点 x 坐标
BOOL CompX(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt;
	for( int i=0; i<npt; i++)
	{
		expt = pObj->GetDataPoint(i);
		int ntrue = 0;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if( expt.x==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( expt.x>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( expt.x<value )ncur = COMP_CURTRUE;
			}
			else if( expt.x!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,expt.x);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}

//比较节点 y 坐标
BOOL CompY(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt;
	for( int i=0; i<npt; i++)
	{
		expt = pObj->GetDataPoint(i);
		int ntrue = 0;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if( expt.y==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( expt.y>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( expt.y<value )ncur = COMP_CURTRUE;
			}
			else if( expt.y!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,expt.y);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//比较节点 z 坐标
BOOL CompZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt;
	for( int i=0; i<npt; i++)
	{
		expt = pObj->GetDataPoint(i);
		int ntrue = 0;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if (fabs(expt.z - value) <= GraphAPI::GetZTolerance())
				ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( expt.z>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( expt.z<value )ncur = COMP_CURTRUE;
			}
			else if( expt.z!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,expt.z);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//比较节点 Z 坐标于高程步距整数倍的误差
int CompIZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	float step  = GetProfileDouble(REGPATH_CONFIG,REGITEM_HEISTEP,5.0);
	if( step==0 )return FALSE;
	
	double v1, v2;
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt;
	for( int i=0; i<npt; i++)
	{
		expt = pObj->GetDataPoint(i);
		v1 = fabs(expt.z-step*(int)floor(expt.z/step));
		v2 = fabs(expt.z-step*(int)ceil(expt.z/step));
		if( v1>v2 )v1 = v2;
		
		int ntrue = 0;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if( v1==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( v1>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( v1<value )ncur = COMP_CURTRUE;
			}
			else if( v1!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,v1);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//比较相邻节点间高差
int CompDZ(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt1,expt2;
	expt1 = pObj->GetDataPoint(0);
	
	for( int i=1; i<npt; i++)
	{
		expt2 = pObj->GetDataPoint(i);
		double v = expt2.z-expt1.z;
		expt1 = expt2;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if( v==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( v>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( v<value )ncur = COMP_CURTRUE;
			}
			else if( v!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,v);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//比较相邻节点间距
int CompPerDis(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt1,expt2;
	expt1 = pObj->GetDataPoint(0);
	
	for( int i=1; i<npt; i++) 
	{
		expt2 = pObj->GetDataPoint(i);
		double v = sqrt( (expt2.x-expt1.x)*(expt2.x-expt1.x)+(expt2.y-expt1.y)*(expt2.y-expt1.y)+
			(expt2.z-expt1.z)*(expt2.z-expt1.z) );
		expt1 = expt2;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if( v==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( v>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( v<value )ncur = COMP_CURTRUE;
			}
			else if( v!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,v);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//比较折线尖角
int CompPerAngle(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt1,expt2,expt3;
	expt1 = pObj->GetDataPoint(0);
	expt2 = pObj->GetDataPoint(1);
	for( int i=2; i<npt; i++)
	{
		expt3 = pObj->GetDataPoint(i);
		double v = GraphAPI::GGetAngle(expt2.x,expt2.y,expt3.x,expt3.y)-GraphAPI::GGetAngle(expt2.x,expt2.y,expt1.x,expt1.y);
		
		if( v<-PI )v += 2*PI;
		else if( v>PI )v -= 2*PI;
		if( v<0 )v = -v;
		v = v*180/PI;
		
		expt1 = expt2;
		expt2 = expt3;
		
		int ncur = COMP_CURFALSE;
		
		double value = atof(strValue);
		if( nOp==OP_EQUAL )
		{
			if( v==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( v>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( v<value )ncur = COMP_CURTRUE;
			}
			else if( v!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			r->AddPointResult(i,v);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//比较首尾节点距离
int CompEndDis(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt1, expt2;
	expt1 = pObj->GetDataPoint(0);
	expt2 = pObj->GetDataPoint(npt-1);
	
	double v = sqrt( (expt2.x-expt1.x)*(expt2.x-expt1.x)+(expt2.y-expt1.y)*(expt2.y-expt1.y)+
		(expt2.z-expt1.z)*(expt2.z-expt1.z) );
	
	int ncur = COMP_CURFALSE;
	
	double value = atof(strValue);
	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		r->AddAttrResult(v);
		ninfo++;
	}
	
	return (ninfo>0);
}


//比较节点线型
int CompLineType(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	PT_3DEX expt;
	char text[256];
	for( int i=0; i<npt; i++)
	{
		expt = pObj->GetDataPoint(i);
		int v = expt.pencode;
		
		int ncur = COMP_CURFALSE;
		
		int value = atoi(strValue);
		if( nOp==OP_EQUAL )
		{
			if( v==value )ncur = COMP_CURTRUE;
		}
		else 
		{
			if( nOp==OP_GREATER )
			{
				if( v>value )ncur = COMP_CURTRUE;
			}
			else if( nOp==OP_LESS )
			{
				if( v<value )ncur = COMP_CURTRUE;
			}
			else if( v!=value )ncur = COMP_CURTRUE;
		}
		
		
		if (ncur == COMP_CURTRUE)
		{
			PenCodeToText(v,text);
			r->AddPointResult(i,text);
			ninfo++;
		}
		
	}
	
	return (ninfo>0);
}


//顺/逆时针方向
int CompClockwise(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	
	if( npt<3 )return FALSE;
	
	PT_3D *pts = new PT_3D[npt];
	if( !pts )return FALSE;
	
	PT_3DEX expt;
	for( int i=0; i<npt; i++)
	{
		expt = pObj->GetDataPoint(i);
		COPY_3DPT(pts[i],expt);
	}
	
	int v = GraphAPI::GIsClockwise(pts,npt);
	delete[] pts;

	if (v == -1)  return 0;
	
	char text[256];
	if( v )sprintf(text,StrFromResID(IDS_COND_CLOCKWISE));
	else sprintf(text,StrFromResID(IDS_COND_ANTICLOCKWISE));
	
	int ncur = COMP_CURFALSE;
	
	int value = strValue.CompareNoCase(StrFromResID(IDS_COND_CLOCKWISE))==0?1:0;
	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		r->AddAttrResult(text);
		ninfo++;
	}
	
	return (ninfo>0);
}


//长度
int CompLen(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum(), ninfo = 0;
	
	if( npt<2 )return FALSE;
	
	CShapeLine *pSharpLine = NULL;
	
	if(pObj->GetClassType() == CLS_GEOCURVE)
	{
		CGeoCurve *pCurve = (CGeoCurve*)pObj;
		pSharpLine = const_cast<CShapeLine*>(pCurve->GetShape());
	}
	else if(pObj->GetClassType() == CLS_GEOSURFACE)
	{
		CGeoSurface *pSurface = (CGeoSurface*)pObj;
		pSharpLine = const_cast<CShapeLine*>(pSurface->GetShape());
	}
	
	if (pSharpLine == NULL) return FALSE;
	
	double v = pSharpLine->GetLength();
	
	int ncur = COMP_CURFALSE;
	
	double value = atof(strValue);
	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		r->AddAttrResult(v);
		ninfo++;
	}
	
	return (ninfo>0);
	
}

//面积
int CompArea(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	int npt = pObj->GetDataPointSum();
	
	if( npt<3 )return FALSE;
	
	double area = 0;
	{
		GrBuffer buf;
		pFtr->Draw(&buf,pDataSource->GetSymbolDrawScale());
		
		int npt = buf.GetLinePts(NULL);		
		
		if( npt>2 )
		{
			PT_3DEX *pts = new PT_3DEX[npt];
			if( pts )
			{
				buf.GetLinePts(pts,sizeof(PT_3DEX));
				area = GraphAPI::GGetPolygonArea(pts,npt);
// 				PT_3D pt3ds[3];
// 				for( int i=1; i<npt-1; i++)
// 				{
// 					if( pts[i+1].cd&1==GRBUFFER_PTCODE_LINETO )
// 					{
// 						pt3ds[0] = pts[0];
// 						pt3ds[1] = pts[i];
// 						pt3ds[2] = pts[i+1];
// 						area += GGetTriangleArea(pt3ds);
// 					}
// 				}
				delete[] pts;
			}
		}
	}
	
	double v = area;
	
	int  ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	double value = atof(strValue);
	if( nOp==OP_EQUAL )
	{
		if(fabs(v-value) <= GraphAPI::GetAreaTolerance())
			ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		r->AddAttrResult(v);
		ninfo++;
	}
	
	return (ninfo>0);
}

//是否自相交
int CompIntersectSelf(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);
	int npt = arr.GetSize();
	
	if( npt<2 )return FALSE;
	
	GrBuffer buf;
	pFtr->Draw(&buf,pDataSource->GetSymbolDrawScale());
	buf.KickoffSamePoints();
	
	int nsum = buf.GetLinePts(NULL);
	PT_3DEX *pts = new PT_3DEX[nsum];
	if( !pts )return FALSE;
	
	buf.GetLinePts(pts,sizeof(PT_3DEX));
	
	CArray<int,int> idxs;
	
	//非重叠的相交
	for( int i=0; i<nsum-1; i++)
	{
		if( pts[i+1].pencode==GRBUFFER_PTCODE_MOVETO )continue;
		for( int k=i+2; k<nsum-1; k++)
		{
			if( pts[k+1].pencode==GRBUFFER_PTCODE_MOVETO )continue;			
			
			if( GraphAPI::GGetLineIntersectLineSeg(pts[i].x,pts[i].y,pts[i+1].x,pts[i+1].y,
				pts[k].x,pts[k].y,pts[k+1].x,pts[k+1].y,NULL,NULL,NULL) )
			{
				if( (fabs(pts[i].x-pts[k].x)<1e-4&&fabs(pts[i].y-pts[k].y)<1e-4)||
					(fabs(pts[i+1].x-pts[k].x)<1e-4&&fabs(pts[i+1].y-pts[k].y)<1e-4)||
					(fabs(pts[i].x-pts[k+1].x)<1e-4&&fabs(pts[i].y-pts[k+1].y)<1e-4)||
					(fabs(pts[i+1].x-pts[k+1].x)<1e-4&&fabs(pts[i+1].y-pts[k+1].y)<1e-4) )
					continue;
				idxs.Add(i);
				idxs.Add(i+1);
			}
		}
	}
	delete[] pts;
	
	//节点重叠的相交
	PT_3DEX expt;
	CArray<PT_3DEX,PT_3DEX> expts;
	expts.SetSize(npt);
	for( i=0; i<npt; i++)
	{
		expt = arr.GetAt(i);
		expts.SetAt(i,expt);
	}
	
	PT_3DEX *exp = expts.GetData();
	for( i=0; i<npt-1; i++)
	{
		for( int k=i+1; k<npt; k++)
		{
			//不包括闭合点的重叠
			if( !(i==0&&k==nsum-1&&npt>=4) && 
				fabs(exp[i].x-exp[k].x)<1e-4 && fabs(exp[i].y-exp[k].y)<1e-4 )
			{
				int nsz = idxs.GetSize();
				for( int m=0; m<nsz; m++)
				{					
					if( i==idxs[m] )break;
					if( i<idxs[m] )
					{
						idxs.InsertAt(m,i);
						break;
					}
				}
				if( m>=nsz )idxs.Add(i);
			}
		}
	}
	expts.RemoveAll();
	
	int v = (idxs.GetSize()>0?1:0), ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	int value;
	if (strValue.CompareNoCase(StrFromResID(IDS_YES)) == 0)
	{
		value = 1;
	}
	else
		value = 0;

	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		if (v == 1)
		{
			for( i=0; i<idxs.GetSize(); i++)
				r->AddPointResult(idxs[i],(short)1);
			ninfo += idxs.GetSize();
		}
		else
		{
			r->AddAttrResult((short)v);
			ninfo++;
		}
	}
	
	return (ninfo>0);
}


CSearchMultiObj gSSP_forPoint;
CSearchMultiObj gSSP_forLine;

//悬挂点
BOOL CompSuspend(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);

// 	if (pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
// 	{
// 		int size = arr.GetSize();
// 		arr.Add(arr[0]);
// 	}

	int npt = arr.GetSize();
	PT_3DEX expt1, expt2, expt;
	if( npt<2 )return FALSE;
	
	
	expt1 = arr[0];
	expt2 = arr[npt-1];
	
	CArray<int,int> idxs;
	//闭合地物，不是悬挂的
	if( fabs(expt1.x-expt2.x)<1e-4 && fabs(expt1.y-expt2.y)<1e-4 );
	else
	{
		//先假定首尾点都是悬挂点
		idxs.Add(0);
		idxs.Add(npt-1);
		
		PT_3D pt3d,ret;
		Envelope e;
		CArray<FTR_HANDLE,FTR_HANDLE> pFoundedFtrs;
		
		//如果首/尾点落在某个地物基线上，该点就不是悬挂点
		for( int i=0; i<2; i++)
		{
			if( i==0 )COPY_3DPT(pt3d,expt1);
			else COPY_3DPT(pt3d,expt2);
			e.m_xl = pt3d.x-1e-4; e.m_xh = pt3d.x+1e-4;
			e.m_yl = pt3d.y-1e-4; e.m_yh = pt3d.y+1e-4;
			int nfind = gSSP_forPoint.FindObjectInRect(pFtr,&pt3d,e,FALSE);
			if( nfind<1 )continue;
			
			pFoundedFtrs.SetSize(nfind);
			int nFtrSize = gSSP_forPoint.GetFoundObjectHandles(pFoundedFtrs.GetData());
			
			for( int j=0; j<nfind; j++)
			{
				if( HandleToFtr(pFoundedFtrs[j])==r->GetCurFtr() )continue;
				if( idxs.GetSize()<=0 )break;
				
				CGeometry* pObj2 = HandleToFtr(pFoundedFtrs[j])->GetGeometry();
				if( !pObj2 )continue;

				PT_3D cltpt;
				gSSP_forPoint.GetCoorSys()->GroundToClient(&pt3d,&cltpt);
				Envelope clte(e.m_xl,e.m_xh,e.m_yl,e.m_yh,e.m_zl,e.m_zh);
				clte.TransformGrdToClt(gSSP_forPoint.GetCoorSys(),1);

				if( !pObj2->FindNearestBasePt(cltpt,clte,gSSP_forPoint.GetCoorSys(),&ret,NULL) )
					continue;
				
				if( fabs(pt3d.x-ret.x)<1e-4 && fabs(pt3d.y-ret.y)<1e-4 )
				{
					if( i==0 )idxs.RemoveAt(0);
					else idxs.RemoveAt(idxs.GetSize()-1);
				}
			}
		}
	}
	
	int v = (idxs.GetSize()>0?1:0), ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	int value;
	if (strValue.CompareNoCase(StrFromResID(IDS_YES)) == 0)
	{
		value = 1;
	}
	else
		value = 0;

	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		if (v == 1)
		{
			for( int i=0; i<idxs.GetSize(); i++)
				r->AddPointResult(idxs[i],(short)1);
			ninfo += idxs.GetSize();
		}
		else
		{
			r->AddAttrResult((short)v);
			ninfo++;
		}
	}
	
	return (ninfo>0);
}

//水流反向
BOOL StreamReverse(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);

	int npt = arr.GetSize();
	if (npt < 2)  return FALSE;

	CArray<int,int> idxs;
	
	PT_3DEX expt,expt0;
	PT_3D pt3d;
	Envelope e;
	
	CFeature *pFoundedFtrs = NULL;
	expt = arr[0];
	expt0 = arr[npt-1];
	if (expt0.z>expt.z)
	{
		idxs.Add(0);
		idxs.Add(npt-1);
	}
	
	int v = (idxs.GetSize()>0?1:0);
	int  ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	int value;
	if (strValue.CompareNoCase(StrFromResID(IDS_YES)) == 0)
	{
		value = 1;
	}
	else
		value = 0;

	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		if (v == 1)
		{
			for( int i=0; i<idxs.GetSize(); i++)
				r->AddPointResult(idxs[i],(short)1);
			ninfo += idxs.GetSize();
		}
		else
		{
			r->AddAttrResult((short)v);
			ninfo++;
		}
	}
	
	return (ninfo>0);
}

//公共点
BOOL CompSamePoint(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);

	int npt = arr.GetSize(), i, j;
	CArray<int,int> idxs;
	
	PT_3DEX expt;
	PT_3D pt3d;
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> pFoundedFtrs;
	
	for( i=0; i<npt; i++)
	{
		expt = arr[i];
		COPY_3DPT(pt3d,expt);
		e.m_xl = pt3d.x-1e-4; e.m_xh = pt3d.x+1e-4;
		e.m_yl = pt3d.y-1e-4; e.m_yh = pt3d.y+1e-4;
		int nfind = gSSP_forPoint.FindObjectInRect(pFtr,&pt3d,e);
		if( nfind<1 )continue;
		
		pFoundedFtrs.SetSize(nfind);
		gSSP_forPoint.GetFoundObjectHandles(pFoundedFtrs.GetData());
		
		for( j=0; j<nfind; j++)
		{
			if( HandleToFtr(pFoundedFtrs[j])==r->GetCurFtr() )continue;
			
			CGeometry* pObj2 = HandleToFtr(pFoundedFtrs[j])->GetGeometry();
			if( !pObj2 )continue;
			
			PT_3D cltpt;
			gSSP_forPoint.GetCoorSys()->GroundToClient(&pt3d,&cltpt);

			int nkeypt = pObj2->FindNearestKeyCtrlPt(cltpt,1e-4,gSSP_forPoint.GetCoorSys()).index;
			if( nkeypt<0 )continue;
			expt = pObj2->GetDataPoint(nkeypt);
			
			if( fabs(pt3d.x-expt.x)<1e-4 && fabs(pt3d.y-expt.y)<1e-4 )
				idxs.Add(i);
		}
	}
	
	int v = (idxs.GetSize()>0?1:0);
	int  ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	int value;
	if (strValue.CompareNoCase(StrFromResID(IDS_YES)) == 0)
	{
		value = 1;
	}
	else
		value = 0;

	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		if (v == 1)
		{
			for( int i=0; i<idxs.GetSize(); i++)
				r->AddPointResult(idxs[i],(short)1);
			ninfo += idxs.GetSize();
		}
		else
		{
			r->AddAttrResult((short)v);
			ninfo++;
		}
	}
	
	return (ninfo>0);
}

//CompSamePointData
struct CompSPData
{
	CFeature *pFtr;
	int idx;
};


//公共边
BOOL CompSameLine(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);

	int npt = arr.GetSize(),i, j;
	CArray<int,int> idxs;
	CPtrArray arrSame;
	
	//找到全部的公共点
	PT_3DEX expt;
	PT_3D pt3d,ret;
	Envelope e;
	CArray<FTR_HANDLE,FTR_HANDLE> pFoundedFtrs;
	for (i=0; i<npt; i++)
	{
		CArray<CompSPData,CompSPData> *pData = new CArray<CompSPData,CompSPData>;
		arrSame.Add(pData);
		if( !pData )continue;
		
		expt = arr[i];
		COPY_3DPT(pt3d,expt);
		e.m_xl = pt3d.x-1e-4; e.m_xh = pt3d.x+1e-4;
		e.m_yl = pt3d.y-1e-4; e.m_yh = pt3d.y+1e-4;
		int nfind = gSSP_forPoint.FindObjectInRect(pFtr,&pt3d,e,FALSE);
		if( nfind<1 )continue;
		
		pFoundedFtrs.SetSize(nfind);
		gSSP_forPoint.GetFoundObjectHandles(pFoundedFtrs.GetData());
		
		CompSPData d;
		for (j=0; j<nfind; j++)
		{
			if( HandleToFtr(pFoundedFtrs[j])==r->GetCurFtr() )continue;
			
			CGeometry* pObj2 = HandleToFtr(pFoundedFtrs[j])->GetGeometry();
			if( !pObj2 )continue;

			PT_3D cltpt;
			gSSP_forPoint.GetCoorSys()->GroundToClient(&pt3d,&cltpt);
			Envelope clte(e.m_xl,e.m_xh,e.m_yl,e.m_yh,e.m_zl,e.m_zh);
			clte.TransformGrdToClt(gSSP_forPoint.GetCoorSys(),1);

			if( !pObj2->FindNearestBasePt(cltpt,clte,gSSP_forPoint.GetCoorSys(),&ret,NULL) )
				continue;
			
			if( fabs(pt3d.x-ret.x)<1e-4 && fabs(pt3d.y-ret.y)<1e-4 )
			{
				int nkeypt = pObj2->FindNearestKeyCtrlPt(cltpt,1e-4,gSSP_forPoint.GetCoorSys()).index;
				//点就落在关键节点上，序号以2倍表示
				if( nkeypt>=0 )d.idx = nkeypt*2;
				//点落在关键节点之间的线上，序号以2倍余1表示
				else 
				{
					CShapeLine *pShape = NULL;
					if (pObj2->GetClassType() == CLS_GEOCURVE)
					{
						pShape = const_cast<CShapeLine*>(((CGeoCurve*)pObj2)->GetShape());
					}
					else if(pObj2->GetClassType() == CLS_GEOSURFACE)
					{
						pShape = const_cast<CShapeLine*>(((CGeoSurface*)pObj2)->GetShape());
					}
					
					if (!pShape) continue;
					
					nkeypt = pShape->FindNearestKeyPt(ret);
					if (nkeypt < 0) continue;
					d.idx = nkeypt*2+1;
				}
				
				d.pFtr = HandleToFtr(pFoundedFtrs[j]);
				pData->Add(d);
			}
		}
	}
	
	//从公共点中抽取出公共边
	//抽取思路：找到与前后两节点都重叠的同一对象，比较两节点（也是重叠点）
	//是否该对象的基线上的一个线段上
	for( i=0; i<npt-1; i++)
	{
		CArray<CompSPData,CompSPData> *pData = (CArray<CompSPData,CompSPData>*)arrSame.GetAt(i);
		if( !pData )continue;
		
		CArray<CompSPData,CompSPData> *pData2 = (CArray<CompSPData,CompSPData>*)arrSame.GetAt(i+1);
		if( !pData2 )continue;
		
		for( int k=0; k<pData->GetSize(); k++)
		{
			CompSPData d1 = pData->GetAt(k);
			
			for( int m=0; m<pData2->GetSize(); m++)
			{
				CompSPData d2 = pData2->GetAt(m);
				
				//同一对象
				if( d1.pFtr==d2.pFtr && 
					//如果都落在基线段之间，那么对应的位置号必须相等
					( ((d1.idx%2)!=0&&(d2.idx%2)!=0&&d1.idx==d2.idx)||
					//只要任何一个落在基线段的端点上，那么它们取下限的端点位置应该连续
					(((d1.idx%2)==0||(d2.idx%2)==0)&&abs(d1.idx/2-d2.idx/2)<=1) ))
				{
					if( idxs.GetSize()<=0 || idxs.GetAt(idxs.GetSize()-1)!=i )
						idxs.Add(i);
					idxs.Add(i+1);
					goto CompSameLine_Find;
				}
			}
		}
		
CompSameLine_Find:;
	}
	
	int v = (idxs.GetSize()>0?1:0), ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	int value;
	if (strValue.CompareNoCase(StrFromResID(IDS_YES)) == 0)
	{
		value = 1;
	}
	else
		value = 0;

	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		if (v == 1)
		{
			for( int i=0; i<idxs.GetSize(); i++)
				r->AddPointResult(idxs[i],(short)1);
			ninfo += idxs.GetSize();
		}
		else
		{
			r->AddAttrResult((short)v);
			ninfo++;
		}
	}
	
	int nsz = arrSame.GetSize();
	for( i=0; i<nsz; i++)
	{
		CArray<CompSPData,CompSPData> *pData = (CArray<CompSPData,CompSPData>*)arrSame.GetAt(i);
		if( !pData )continue;
		delete pData;
	}
	
	return (ninfo>0);
}


BOOL CompIntersectLine(CDlgDataSource *pDataSource, CFeature *pFtr,CString strField, int nOp, CString strValue, CCPResultList* r)
{
	CGeometry *pObj = pFtr->GetGeometry();
	CArray<PT_3DEX,PT_3DEX> arr;
	pObj->GetShape(arr);

	int npt = arr.GetSize(), i;
	if( npt<2 )return FALSE;
	CArray<int,int> idxs;
	
	PT_3DEX expt;
	PT_3D pt3d[2];
	Envelope e;
	CFeature *pFoundedFtrs = NULL;
	
	for( i=0; i<npt-1; i++)
	{
		expt = arr[i];
		COPY_3DPT(pt3d[0],expt);
		expt = arr[i+1];
		COPY_3DPT(pt3d[1],expt);
		int nfind = gSSP_forLine.FindIntersectInRect(pFtr,pt3d[0],pt3d[1]);
		if( nfind<=0 )continue;
		
		idxs.Add(i);
	}
	
	int v = (idxs.GetSize()>0?1:0);
	int  ninfo = 0;
	int ncur = COMP_CURFALSE;
	
	int value;
	if (strValue.CompareNoCase(StrFromResID(IDS_YES)) == 0)
	{
		value = 1;
	}
	else
		value = 0;

	if( nOp==OP_EQUAL )
	{
		if( v==value )ncur = COMP_CURTRUE;
	}
	else 
	{
		if( nOp==OP_GREATER )
		{
			if( v>value )ncur = COMP_CURTRUE;
		}
		else if( nOp==OP_LESS )
		{
			if( v<value )ncur = COMP_CURTRUE;
		}
		else if( v!=value )ncur = COMP_CURTRUE;
	}
	
	
	if (ncur == COMP_CURTRUE)
	{
		if (v == 1)
		{
			for( int i=0; i<idxs.GetSize(); i++)
				r->AddPointResult(idxs[i],(short)1);
			ninfo += idxs.GetSize();
		}
		else
		{
			r->AddAttrResult((short)v);
			ninfo++;
		}
	}
	
	return (ninfo>0);
}
