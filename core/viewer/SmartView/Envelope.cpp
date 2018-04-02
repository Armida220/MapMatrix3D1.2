// Copyright (C) 2004 - 2005 Gibuilder Group

#include "stdafx.h"
#include "Envelope.h"
#include "GrBuffer.h"
#include "SmartViewFunctions.h"
#include "CoordSys.h"
#include "Float.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

Envelope::Envelope()
{
	m_xl = 0;
	m_xh = -1;
	m_yl = 0;
	m_yh = -1;
	m_zl = 0;
	m_zh = -1;
}


Envelope::Envelope(double xl, double xh, double yl, double yh, double zl, double zh)
{
	m_xl = xl;
	m_xh = xh;
	m_yl = yl;
	m_yh = yh;
	m_zl = zl;
	m_zh = zh;
}


void Envelope::CreateMaxEnvelope()
{
	m_xl = -1e+12;
	m_xh = 1e+12;
	m_yl = -1e+12;
	m_yh = 1e+12;
	m_zl = -1e+12;
	m_zh = 1e+12;
}

void Envelope::CreateFromPts(const PT_3D *pts, int ptsnum, int sizeofpt, int dims)
{
	for( int i=0; i<ptsnum; i++)
	{
		if( i==0 )
		{
			m_xl = pts->x;
			m_xh = pts->x;

			m_yl = pts->y;
			m_yh = pts->y;
		}
		else
		{
			m_xl = (m_xl<pts->x?m_xl:pts->x);
			m_xh = (m_xh>pts->x?m_xh:pts->x);

			m_yl = (m_yl<pts->y?m_yl:pts->y);
			m_yh = (m_yh>pts->y?m_yh:pts->y);
		}

		if( dims==3 )
		{
			if( i==0 )
			{
				m_zl = pts->z;
				m_zh = pts->z;
			}
			else
			{
				m_zl = (m_zl<pts->z?m_zl:pts->z);
				m_zh = (m_zh>pts->z?m_zh:pts->z);
			}
		}

		pts = (const PT_3D*)((const BYTE*)pts + sizeofpt);
	}
}


void Envelope::CreateFromPts(const PT_2D *pts, int ptsnum, int sizeofpt)
{
	for( int i=0; i<ptsnum; i++)
	{
		if( i==0 )
		{
			m_xl = pts->x;
			m_xh = pts->x;
			
			m_yl = pts->y;
			m_yh = pts->y;
		}
		else
		{
			m_xl = (m_xl<pts->x?m_xl:pts->x);
			m_xh = (m_xh>pts->x?m_xh:pts->x);
			
			m_yl = (m_yl<pts->y?m_yl:pts->y);
			m_yh = (m_yh>pts->y?m_yh:pts->y);
		}
		
		pts = (const PT_2D*)((const BYTE*)pts + sizeofpt);
	}
}


void Envelope::CreateFromPtAndRadius(PT_3D pt, double r, int dims)
{
	m_xl = pt.x-r; m_xh = pt.x+r;
	m_yl = pt.y-r; m_yh = pt.y+r;
	if( dims==3 )
	{
		m_zl = pt.z-r; m_zh = pt.z+r;
	}
}

void Envelope::Transform2d(const double *m)
{
	PT_3D pts[8];
	memset(pts,0,sizeof(pts));
	pts[4].x = m_xl, pts[4].y = m_yl;
	pts[5].x = m_xh, pts[5].y = m_yl;
	pts[6].x = m_xh, pts[6].y = m_yh;
	pts[7].x = m_xl, pts[7].y = m_yh;

	CONVERT_PT2D(pts[4],m,pts[0]);
	CONVERT_PT2D(pts[5],m,pts[1]);
	CONVERT_PT2D(pts[6],m,pts[2]);
	CONVERT_PT2D(pts[7],m,pts[3]);

	CreateFromPts(pts,4,sizeof(PT_3D));
}


void Envelope::Transform(const double *m)
{
	PT_3D pts[8];
	pts[4].x = m_xl, pts[4].y = m_yl, pts[4].z = m_zl;
	pts[5].x = m_xh, pts[5].y = m_yl, pts[5].z = m_zl;
	pts[6].x = m_xl, pts[6].y = m_yh, pts[6].z = m_zl;
	pts[7].x = m_xl, pts[7].y = m_yl, pts[7].z = m_zh;

	CONVERT_PT3D(pts[4],m,pts[0]);
	CONVERT_PT3D(pts[5],m,pts[1]);
	CONVERT_PT3D(pts[6],m,pts[2]);
	CONVERT_PT3D(pts[7],m,pts[3]);

	pts[4].x = pts[0].x + pts[1].x-pts[0].x + pts[2].x-pts[0].x + pts[3].x-pts[0].x;
	pts[4].y = pts[0].y + pts[1].y-pts[0].y + pts[2].y-pts[0].y + pts[3].y-pts[0].y;
	pts[4].z = pts[0].z + pts[1].z-pts[0].z + pts[2].z-pts[0].z + pts[3].z-pts[0].z;

	pts[5].x = pts[0].x + pts[1].x-pts[0].x + pts[3].x-pts[0].x;
	pts[5].y = pts[0].y + pts[1].y-pts[0].y + pts[3].y-pts[0].y;
	pts[5].z = pts[0].z + pts[1].z-pts[0].z + pts[3].z-pts[0].z;

	pts[6].x = pts[0].x + pts[2].x-pts[0].x + pts[3].x-pts[0].x;
	pts[6].y = pts[0].y + pts[2].y-pts[0].y + pts[3].y-pts[0].y;
	pts[6].z = pts[0].z + pts[2].z-pts[0].z + pts[3].z-pts[0].z;

	pts[7].x = pts[0].x + pts[1].x-pts[0].x + pts[2].x-pts[0].x;
	pts[7].y = pts[0].y + pts[1].y-pts[0].y + pts[2].y-pts[0].y;
	pts[7].z = pts[0].z + pts[1].z-pts[0].z + pts[2].z-pts[0].z;

	CreateFromPts(pts,8,sizeof(PT_3D),3);
}


void Envelope::TransformGrdToClt(CCoordSys *pCS, int flag)
{
	if( !pCS || IsEmpty(2) )return;
	
	PT_4D pts[8];
	memset(pts,0,sizeof(pts));
	pts[4].x = m_xl, pts[4].y = m_yl, pts[4].z = m_zl;
	pts[5].x = m_xh, pts[5].y = m_yh, pts[5].z = m_zl;
	pts[6].x = m_xl, pts[6].y = m_yh, pts[6].z = m_zh;
	pts[7].x = m_xh, pts[7].y = m_yl, pts[7].z = m_zh;

	pCS->GroundToClient(pts+4,4);
	
	pts[0] = pts[4]; pts[1] = pts[5];
	pts[2] = pts[6]; pts[3] = pts[7];
	
	pts[4].x = pts[0].z; pts[4].y = pts[0].yr;
	pts[5].x = pts[1].z; pts[5].y = pts[1].yr;
	pts[6].x = pts[2].z; pts[6].y = pts[2].yr;
	pts[7].x = pts[3].z; pts[7].y = pts[3].yr;
	
	if( flag==1 )CreateFromPts(pts,4,sizeof(PT_4D),2);
	else if( flag==2 )CreateFromPts(pts+4,4,sizeof(PT_4D),2);
	else if( flag==3 )CreateFromPts(pts,8,sizeof(PT_4D),2);
	
	m_zl = 0; m_zh = -1;
}


void Envelope::TransformCltToGrd(CCoordSys *pCS)
{
	if( !pCS || IsEmpty(2) )return;
	
	PT_4D pts[8];
	memset(pts,0,sizeof(pts));
	pts[4].x = m_xl, pts[4].y = m_yl, pts[4].z = 0;
	pts[5].x = m_xh, pts[5].y = m_yl, pts[5].z = 0;
	pts[6].x = m_xh, pts[6].y = m_yh, pts[6].z = 0;
	pts[7].x = m_xl, pts[7].y = m_yh, pts[7].z = 0;
	
	pCS->ClientToGround(pts+4,4);
	
	CreateFromPts(pts+4,4,sizeof(PT_4D),2);
}


BOOL Envelope::bIntersect(const Envelope *pEnlp, int dims) const
{
	if( !pEnlp )return FALSE;

	//if( pEnlp->bEnvelopeIn(this,dims) || bEnvelopeIn(pEnlp,dims) )
	//	return TRUE;

	double xl = (m_xl>pEnlp->m_xl?m_xl:pEnlp->m_xl);
	double xh = (m_xh<pEnlp->m_xh?m_xh:pEnlp->m_xh);
	if( xl>xh )return FALSE;

	double yl = (m_yl>pEnlp->m_yl?m_yl:pEnlp->m_yl);
	double yh = (m_yh<pEnlp->m_yh?m_yh:pEnlp->m_yh);
	if( yl>yh )return FALSE;

	if( dims>=3 )
	{
		double zl = (m_zl>pEnlp->m_zl?m_zl:pEnlp->m_zl);
		double zh = (m_zh<pEnlp->m_zh?m_zh:pEnlp->m_zh);

		if( zl>zh )return FALSE;
	}
	return TRUE;
}

BOOL Envelope::bIntersect(const PT_3D *pt1, const PT_3D *pt2, int dims) const
{
	if( bPtIn(pt1,dims) || bPtIn(pt2,dims) )
		return TRUE;

	double xl = (pt1->x<pt2->x?pt1->x:pt2->x);
	double xh = (pt1->x>pt2->x?pt1->x:pt2->x);
	double yl = (pt1->y<pt2->y?pt1->y:pt2->y);
	double yh = (pt1->y>pt2->y?pt1->y:pt2->y);
	double zl = (pt1->z<pt2->z?pt1->z:pt2->z);
	double zh = (pt1->z>pt2->z?pt1->z:pt2->z);

	Envelope e(xl,xh,yl,yh,zl,zh);
	if( !bIntersect(&e,dims) )return FALSE;
//	return TRUE;

	return ( GraphAPI::GGetLineIntersectLineSeg(pt1->x,pt1->y,pt2->x,pt2->y,m_xl,m_yl,m_xh,m_yl,NULL,NULL,NULL) ||
		GraphAPI::GGetLineIntersectLineSeg(pt1->x,pt1->y,pt2->x,pt2->y,m_xh,m_yl,m_xh,m_yh,NULL,NULL,NULL) ||
		GraphAPI::GGetLineIntersectLineSeg(pt1->x,pt1->y,pt2->x,pt2->y,m_xh,m_yh,m_xl,m_yh,NULL,NULL,NULL) ||
		GraphAPI::GGetLineIntersectLineSeg(pt1->x,pt1->y,pt2->x,pt2->y,m_xl,m_yh,m_xl,m_yl,NULL,NULL,NULL) );

}


BOOL Envelope::bPtIn(const PT_3D *pt, int dims) const
{
	if( dims==2 )
		return (pt->x>=m_xl && pt->x<=m_xh && 
				pt->y>=m_yl && pt->y<=m_yh);
	else 
		return (pt->x>=m_xl && pt->x<=m_xh && 
				pt->y>=m_yl && pt->y<=m_yh &&
				pt->z>=m_zl && pt->z<=m_zh );
}



BOOL Envelope::bEnvelopeIn(const Envelope *pEnlp, int dims) const
{
	if( dims==2 )
		return (m_xl<=pEnlp->m_xl && m_xh>=pEnlp->m_xh && 
		m_yl<=pEnlp->m_yl && m_yh>=pEnlp->m_yh);
	else 
		return (m_xl<=pEnlp->m_xl && m_xh>=pEnlp->m_xh && 
		m_yl<=pEnlp->m_yl && m_yh>=pEnlp->m_yh &&
		m_zl<=pEnlp->m_zl && m_zh>=pEnlp->m_zh );
}

BOOL Envelope::bIntersect(const PT_3D *pts, int n, int dims) const
{
	if( n<=0 )return FALSE;
	if( n==1 )return bPtIn(pts,dims);
	if( n==2 )return bIntersect(pts,pts+1,dims);

	if( bIntersect(pts,pts+n-1,dims) )return TRUE;
	for( int i=0; i<n-1; i++)
	{
		if( bIntersect(pts+i,pts+i+1,dims) )return TRUE;
	}
	
	return FALSE;
}

void Envelope::Intersect(const Envelope *pEnlp, int dims)
{
	if( !pEnlp )return;

	m_xl = (m_xl>pEnlp->m_xl?m_xl:pEnlp->m_xl);
	m_xh = (m_xh<pEnlp->m_xh?m_xh:pEnlp->m_xh);

	m_yl = (m_yl>pEnlp->m_yl?m_yl:pEnlp->m_yl);
	m_yh = (m_yh<pEnlp->m_yh?m_yh:pEnlp->m_yh);

	if( dims>=3 )
	{
		m_zl= (m_zl>pEnlp->m_zl?m_zl:pEnlp->m_zl);
		m_zh= (m_zh<pEnlp->m_zh?m_zh:pEnlp->m_zh);
	}

	return;
}

void Envelope::Union(const Envelope *pEnlp, int dims)
{
	if( !pEnlp )return;
	if( pEnlp->IsEmpty() )return;
	if( IsEmpty() )
	{
		m_xl = pEnlp->m_xl;
		m_xh = pEnlp->m_xh;
		
		m_yl = pEnlp->m_yl;
		m_yh = pEnlp->m_yh;
		
		if( dims>=3 )
		{
			m_zl= pEnlp->m_zl;
			m_zh= pEnlp->m_zh;
		}
		return;
	}

	m_xl = (m_xl<pEnlp->m_xl?m_xl:pEnlp->m_xl);
	m_xh = (m_xh>pEnlp->m_xh?m_xh:pEnlp->m_xh);

	m_yl = (m_yl<pEnlp->m_yl?m_yl:pEnlp->m_yl);
	m_yh = (m_yh>pEnlp->m_yh?m_yh:pEnlp->m_yh);

	if( dims>=3 )
	{
		m_zl= (m_zl<pEnlp->m_zl?m_zl:pEnlp->m_zl);
		m_zh= (m_zh>pEnlp->m_zh?m_zh:pEnlp->m_zh);
	}

	return;
}

MyNameSpaceEnd

