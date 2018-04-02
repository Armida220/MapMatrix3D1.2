// Graph2dSearch.cpp: implementation of the CGraph2dSearch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Graph2dSearch.h"
#include "GrBuffer2d.h"
#include "CoordSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGraph2dSearch::CGraph2dSearch():m_arrObjs(256)
{
	m_fDrawCellKX = m_fDrawCellKY = 1.0;
	m_fDrawCellAngle = 0;
	m_arrFoundObjs.SetSize(0,10240);
}

CGraph2dSearch::~CGraph2dSearch()
{
	DelAll();
}


void CGraph2dSearch::AddObj(LONG_PTR id, CArray<Graph2d*, Graph2d*>& arr)
{
	int insert = 0;
	if( FindObj(id,insert) )
	{
		return;
	}

	ObjData *p = CreateObj(id,arr);
	m_arrObjs.InsertAt(insert,p);
	
}


void CGraph2dSearch::DelObj(LONG_PTR id)
{
	ClearMarks();

	int insert = 0;
	if( !FindObj(id,insert) )
	{
		return;
	}
	ObjData *p = m_arrObjs.GetAt(insert);
	FreeObj(p);
	m_arrObjs.RemoveAt(insert);
}


void CGraph2dSearch::DelAll()
{
	m_arrFoundObjs.RemoveAll();

	for( int i=0; i<m_arrObjs.GetSize(); i++)
	{
		FreeObj(m_arrObjs.GetAt(i));
	}
	m_arrObjs.RemoveAll();
}



void CGraph2dSearch::MarkObjsInRect(double xl, double xh, double yl, double yh, CCoordSys *pCS)
{
	if( m_arrObjs.GetSize()<=0 )
		return;

	float x1,x2,y1,y2;
	Envelope e;

	ClearMarks();

	if( pCS!=NULL )
	{
		PT_4D pts[4];
		pts[0].x = xl; pts[0].y = yl; 
		pts[1].x = xh; pts[1].y = yl; 
		pts[2].x = xh; pts[2].y = yh; 
		pts[3].x = xl; pts[3].y = yh; 
		
		pCS->ClientToGround(pts,4);
		Envelope e;
		e.CreateFromPts(pts,4,sizeof(PT_4D));
		xl = e.m_xl; xh = e.m_xh;
		yl = e.m_yl; yh = e.m_yh;
	}
	
	for( int i=0; i<m_arrObjs.GetSize(); i++)
	{
		ObjData *p = m_arrObjs.GetAt(i);
		float xoff, yoff;
		p->list[0]->owner->GetOrigin(xoff,yoff);

		x1 = xl-xoff; x2 = xh-xoff;
		y1 = yl-yoff; y2 = yh-yoff;

		while( p!=NULL )
		{
			//不相交
			if( p->xh<x1 || p->xl>x2 || p->yh<y1 || p->yl>y2 )
			{
			}
			//完全包含
			else if( p->xl>=x1 && p->xh<=x2 && p->yl>=y1 && p->yh<=y2 )
			{
				for( int j=0; j<p->nused; j++)
				{
					p->list[j]->needdraw = 1;
				}
				m_arrFoundObjs.Add(p);
			}
			//相交
			else
			{
				for( int j=0; j<p->nused; j++)
				{
					e = GetEnvelopeOfGraph2d(p->list[j],m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY);

					//不相交
					if( e.m_xh<x1 || e.m_xl>x2 || e.m_yh<y1 || e.m_yl>y2 )
					{
					}
					else
					{
						p->list[j]->needdraw = 1;
					}
				}
				m_arrFoundObjs.Add(p);
			}
			p = p->next;
		}
	}
}



void CGraph2dSearch::ClearMarks()
{
	int nSize = m_arrFoundObjs.GetSize();
	for( int i=0; i<nSize; i++)
	{
		ObjData *p = m_arrFoundObjs.GetAt(i);
		for( int j=0; j<p->nused; j++)
		{
			p->list[j]->needdraw = 0;
		}
	}
	m_arrFoundObjs.RemoveAll();
}


CGraph2dSearch::ObjData *CGraph2dSearch::CreateObj(LONG_PTR id, CArray<Graph2d*, Graph2d*>& arr)
{
	ObjData *pObj = new ObjData;
	if( !pObj )return NULL;

	ObjData *pTail = pObj;

	int nSize = arr.GetSize(), nUsed = 0;
	Graph2d **pBuf = arr.GetData();
	Graph2d **p = pBuf;

	for( int i=0; i<nSize; i++,p++ )
	{
		p[0]->use_needdraw = 1;	
	}

	p = pBuf;

	for( i=0; i<nSize; i+=ObjDataBlockSize )
	{
		nUsed = (nSize-i)>ObjDataBlockSize?ObjDataBlockSize:(nSize-i);

		pTail->id = id;
		memcpy(pTail->list,p,sizeof(Graph2d*)*nUsed);
		pTail->nused = nUsed;

		Envelope e;
		for( int j=0; j<nUsed; j++)
		{
			e.Union(&GetEnvelopeOfGraph2d(p[j],m_fDrawCellAngle,m_fDrawCellKX,m_fDrawCellKY));
		}
		pTail->xl = e.m_xl; pTail->xh = e.m_xh;
		pTail->yl = e.m_yl; pTail->yh = e.m_yh;

		if( (nSize-i)>ObjDataBlockSize )
		{
			ObjData *pNew = new ObjData;
			pTail->next = pNew;
			pTail = pNew;
		}

		p += nUsed;
	}
	return pObj;
}

BOOL CGraph2dSearch::FindObj(LONG_PTR id, int& insert_idx)
{
	int findidx = -1, bnew = 0;
	if( m_arrObjs.GetSize()<=0 ){ findidx = 0; bnew = 1; }
	else
	{
		int i0 = 0, i1 = m_arrObjs.GetSize()-1, i2;
		while(findidx==-1) 
		{
			if( id<=m_arrObjs.GetAt(i0)->id )
			{
				bnew = (id==m_arrObjs.GetAt(i0)->id?0:1);
				findidx = i0;
				break;
			}
			else if( id>=m_arrObjs.GetAt(i1)->id )
			{
				bnew = (id==m_arrObjs.GetAt(i1)->id?0:1);
				findidx = (id==m_arrObjs.GetAt(i1)->id?i1:(i1+1));
				break;
			}
			
			i2 = (i0+i1)/2;
			if( i2==i0 )
			{
				bnew = (id==m_arrObjs.GetAt(i0)->id?0:1);
				findidx = (id==m_arrObjs.GetAt(i0)->id?i0:(i0+1));
				break;
			}
			
			if( id<m_arrObjs.GetAt(i2)->id )
				i1 = i2;
			else if( id>m_arrObjs.GetAt(i2)->id )
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


void CGraph2dSearch::FreeObj(ObjData *p)
{
	ObjData *p2;
	while( p!=NULL )
	{
		p2 = p;
		p = p->next;
		delete p2;
	}
}

MyNameSpaceEnd