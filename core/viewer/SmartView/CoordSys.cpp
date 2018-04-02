// CoordSys.cpp: implementation of the CCoordSys class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CoordSys.h"
#include "SmartViewFunctions.h"
#include "matrix.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

extern void TransformPt2d(const double *matrix, PT_2D *pts, int num, int sizeofpt=sizeof(PT_2D));


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCoordSys::CCoordSys()
{
	m_nType = COORDSYS_NONE;
	m_hCoordWnd = NULL;
	m_lfGScale = 1.0;
	
	memset(m_matrix,0,sizeof(m_matrix));
	memset(m_rmatrix,0,sizeof(m_rmatrix));

	m_bClientLeft = TRUE;
	m_pFunGC = NULL;
	m_pFunCG = NULL;
}


CCoordSys::~CCoordSys()
{
}


CCoordSys *CCoordSys::CreateObject()
{
	return new CCoordSys();
}

void CCoordSys::DeleteObject(CCoordSys* pObj)
{
	if( pObj )
	{
		delete pObj;
	}
}

void CCoordSys::CopyFrom(CCoordSys *pCS)
{
	m_nType = pCS->m_nType;
	m_hCoordWnd = pCS->m_hCoordWnd;
	memcpy(m_matrix,pCS->m_matrix,sizeof(m_matrix));
	memcpy(m_rmatrix,pCS->m_rmatrix,sizeof(m_rmatrix));
	m_arrPCS.Copy(pCS->m_arrPCS);
	
	m_bClientLeft = pCS->m_bClientLeft;

	m_pFunGC = pCS->m_pFunGC;
	m_pFunCG = pCS->m_pFunCG;
	m_lfGScale = pCS->m_lfGScale;
}

BOOL CCoordSys::Create33Matrix(const double *m)
{
	m_nType = COORDSYS_33MATRIX;
	m_hCoordWnd = NULL;
	
	if( m==NULL )
	{
		matrix_toIdentity(m_matrix,3);
		matrix_toIdentity(m_rmatrix,3);
	}
	else
	{
		memcpy(m_matrix,m,sizeof(double)*9);
		matrix_reverse(m_matrix,3,m_rmatrix);
	}

	return TRUE;
}

BOOL CCoordSys::Create44Matrix(const double *m)
{
	m_nType = COORDSYS_44MATRIX;
	m_hCoordWnd = NULL;
	
	if( m==NULL )
	{
		matrix_toIdentity(m_matrix,4);
		matrix_toIdentity(m_rmatrix,4);
	}
	else
	{
		memcpy(m_matrix,m,sizeof(double)*16);
		matrix_reverse(m_matrix,4,m_rmatrix);
	}
	
	return TRUE;
}


BOOL CCoordSys::CreateCallback(COORDSYS_CALLBACK_PFUN pFunGC, COORDSYS_CALLBACK_PFUN pFunCG)
{
	m_nType = COORDSYS_CALLBACK;
	m_pFunGC = pFunGC;
	m_pFunCG = pFunCG;
	return TRUE;
}


BOOL CCoordSys::CreateCompound(CArray<CCoordSys*,CCoordSys*>& arrItems)
{
	m_nType = COORDSYS_COMPOUND;
	m_arrPCS.Copy(arrItems);
	return TRUE;
}


BOOL CCoordSys::CreateWnd(HWND hWnd)
{
	m_nType = COORDSYS_WND;
	m_hCoordWnd = hWnd;
	
	memset(m_matrix,0,sizeof(m_matrix));
	memset(m_rmatrix,0,sizeof(m_rmatrix));

	return TRUE;
}


BOOL CCoordSys::ClientToGround(PT_4D *pts, int num)
{
	if (this==NULL)
	{
		return TRUE;
	}
	if( m_nType==COORDSYS_WND )
		::SendMessage(m_hCoordWnd,MSGID_CLTTOGND,num,(LPARAM)pts);
	else if( m_nType==COORDSYS_33MATRIX )
		GraphAPI::TransformPointsWith33Matrix(m_rmatrix,pts,num);
	else if( m_nType==COORDSYS_44MATRIX )
		GraphAPI::TransformPointsWith44Matrix(m_rmatrix,pts,num);
	else if( m_nType==COORDSYS_COMPOUND )
	{
		for( int i=m_arrPCS.GetSize()-1; i>=0; i--)
		{
			m_arrPCS.GetAt(i)->ClientToGround(pts,num);
		}
	}
	else if( m_nType==COORDSYS_CUSTOM )
	{
		return ClientToGround0(pts,num);
	}

	return TRUE;
}

BOOL CCoordSys::GroundToClient(PT_4D *pts, int num)
{
	if (this==NULL)
	{
		return TRUE;
	}
	if( m_nType==COORDSYS_WND )
	{
		::SendMessage(m_hCoordWnd,MSGID_GNDTOCLT,num,(LPARAM)pts);
		if( !m_bClientLeft )
		{
			double t;
			for( int i=0; i<num; i++)
			{
				t = pts[i].x; pts[i].x = pts[i].z; pts[i].z = t;
				t = pts[i].y; pts[i].y = pts[i].yr; pts[i].yr = t;
			}
		}
	}
	else if( m_nType==COORDSYS_33MATRIX )
		GraphAPI::TransformPointsWith33Matrix(m_matrix,pts,num);
	else if( m_nType==COORDSYS_44MATRIX )
		GraphAPI::TransformPointsWith44Matrix(m_matrix,pts,num);
	else if( m_nType==COORDSYS_COMPOUND )
	{
		for( int i=0; i<m_arrPCS.GetSize(); i++)
		{
			m_arrPCS.GetAt(i)->GroundToClient(pts,num);
		}
	}
	else if( m_nType==COORDSYS_CUSTOM )
	{
		return GroundToClient0(pts,num);
	}

	return TRUE;
}


BOOL CCoordSys::GroundToClient(const PT_3D *pt1, PT_3D *pt2)
{
	if(this==NULL)
	{
		COPY_3DPT(*pt2,*pt1);
		return TRUE;
	}
	PT_4D pt3 = *pt1;
	BOOL bRet = GroundToClient(&pt3,1);
	*pt2 = pt3;

	return bRet;
}


BOOL CCoordSys::ClientToGround(const PT_3D *pt1, PT_3D *pt2)
{
	if(this==NULL)
	{
		COPY_3DPT(*pt2,*pt1);
		return TRUE;
	}
	PT_4D pt3 = *pt1;
	BOOL bRet = ClientToGround(&pt3,1);
	*pt2 = pt3;
	
	return bRet;
}

Envelope CCoordSys::GrdEvlpFromGrdPtAndCltRadius(PT_4D grdpt, double r)
{
	PT_4D pt0,pts[4];
	pt0 = PT_4D(grdpt);
	
	GroundToClient(&pt0,1);
	
	pts[0].x = pt0.x-r; pts[0].y = pt0.y-r; pts[0].z = pt0.z-r; pts[0].yr = pt0.yr-r;
	pts[1].x = pt0.x+r; pts[1].y = pt0.y-r; pts[1].z = pt0.z+r; pts[1].yr = pt0.yr-r;
	pts[2].x = pt0.x+r; pts[2].y = pt0.y+r; pts[2].z = pt0.z+r; pts[2].yr = pt0.yr+r;
	pts[3].x = pt0.x-r; pts[3].y = pt0.y+r; pts[3].z = pt0.z-r; pts[3].yr = pt0.yr+r;
	
	ClientToGround(pts,4);
	
	Envelope e;
	e.CreateFromPts(pts,sizeof(PT_4D),3);
	return e;
}

Envelope CCoordSys::GrdEvlpFromCltPtAndCltRadius(PT_4D cltpt, double r)
{
	PT_4D t0;
	PT_4D t1[4];
	t0.x = cltpt.x-r; t0.y = cltpt.y-r; t0.z = cltpt.z-r; t0.yr = t0.y;
	t1[0] = t0;
	t0.x = cltpt.x+r; t0.y = cltpt.y-r; t0.z = cltpt.z+r; t0.yr = t0.y;
	t1[1] = t0;
	t0.x = cltpt.x+r; t0.y = cltpt.y+r; t0.z = cltpt.z+r; t0.yr = t0.y;
	t1[2] = t0;
	t0.x = cltpt.x-r; t0.y = cltpt.y+r; t0.z = cltpt.z-r; t0.yr = t0.y;
	t1[3] = t0;
	
	ClientToGround(t1,4);
	
	Envelope e;
	e.CreateFromPts(t1,4,sizeof(PT_4D),3);
	return e;
}

Envelope CCoordSys::GrdEvlpFromCltRect(CRect& rect)
{
	PT_4D t0;
	PT_4D t1[4];
	t0.x = rect.left; t0.y = rect.top; t0.z = rect.left; t0.yr = rect.top;
	t1[0] = t0;
	t0.x = rect.right; t0.y = rect.top; t0.z = rect.right; t0.yr = rect.top;
	t1[1] = t0;
	t0.x = rect.right; t0.y = rect.bottom; t0.z = rect.right; t0.yr = rect.bottom;
	t1[2] = t0;
	t0.x = rect.left; t0.y = rect.bottom; t0.z = rect.left; t0.yr = rect.bottom;
	t1[3] = t0;
	
	ClientToGround(t1,4);
	
	Envelope e;
	e.CreateFromPts(t1,4,sizeof(PT_4D),3);
	return e;
}


BOOL CCoordSys::ClientToGround0(PT_4D *pts, int num)
{
	return FALSE;
}


BOOL CCoordSys::GroundToClient0(PT_4D *pts, int num)
{
	return FALSE;
}


CRect CCoordSys::GrdEvlpToCltRect(Envelope e)
{
	PT_4D t0;
	PT_4D t1[8];
	t0.x = e.m_xl; t0.y = e.m_yl; t0.z = e.m_zl; t0.yr = 0;
	t1[0] = t0;
	t0.x = e.m_xh; t0.y = e.m_yl; t0.z = e.m_zl; t0.yr = 0;
	t1[1] = t0;
	t0.x = e.m_xh; t0.y = e.m_yh; t0.z = e.m_zl; t0.yr = 0;
	t1[2] = t0;
	t0.x = e.m_xl; t0.y = e.m_yh; t0.z = e.m_zl; t0.yr = 0;
	t1[3] = t0;

	t0.x = e.m_xl; t0.y = e.m_yl; t0.z = e.m_zh; t0.yr = 0;
	t1[4] = t0;
	t0.x = e.m_xh; t0.y = e.m_yl; t0.z = e.m_zh; t0.yr = 0;
	t1[5] = t0;
	t0.x = e.m_xh; t0.y = e.m_yh; t0.z = e.m_zh; t0.yr = 0;
	t1[6] = t0;
	t0.x = e.m_xl; t0.y = e.m_yh; t0.z = e.m_zh; t0.yr = 0;
	t1[7] = t0;
	
	GroundToClient(t1,8);

	Envelope e2;
	e2.CreateFromPts(t1,8,sizeof(PT_4D),2);

	return CRect(floor(e2.m_xl),floor(e2.m_yl),ceil(e2.m_xh),ceil(e2.m_yh));
}


double CCoordSys::CalcScale()
{
	/*
	if( m_nType==COORDSYS_33MATRIX )
	{
		m_lfGScale = sqrt(fabs(matrix_modulus(m_matrix,3)));
	}
	else if( m_nType==COORDSYS_44MATRIX )
	{
		m_lfGScale = pow(fabs(matrix_modulus(m_matrix,4)),1.0/3.0);
	}
	else*/
	{
		PT_3D pt0, pt1, pt2, pt3, pt4, pt5;

		pt0.x = 0; pt0.y = 0; pt0.z = 0;
//		pt1.x = 100; pt1.y = 0; pt1.z = 0;
		pt2.x = 0; pt2.y = 100; pt2.z = 0;
		ClientToGround(&pt0,&pt3);
//		ClientToGround(&pt1,&pt4);
		ClientToGround(&pt2,&pt5);

		double dis1 = 0;//DIST_3DPT(pt3,pt4);
		double dis2 = DIST_3DPT(pt3,pt5);
		m_lfGScale = 100/dis2;
	}

	return m_lfGScale;
}

void CCoordSys::GetMatrix(double m[16])
{
	memcpy(m,m_matrix,sizeof(m_matrix));
}

void CCoordSys::GetRMatrix(double m[16])
{
	memcpy(m,m_rmatrix,sizeof(m_rmatrix));
}

void CCoordSys::MultiplyMatrix(double m[16])
{
	double m2[16];
	if( m_nType==COORDSYS_33MATRIX )
	{
		matrix_multiply(m,m_matrix,3,m2);
		Create33Matrix(m2);
	}
	else if( m_nType==COORDSYS_44MATRIX )
	{
		matrix_multiply(m,m_matrix,4,m2);
		Create44Matrix(m2);
	}
}

void CCoordSys::MultiplyRightMatrix(double m[16])
{
	double m2[16];
	if( m_nType==COORDSYS_33MATRIX )
	{
		matrix_multiply(m_matrix,m,3,m2);
		Create33Matrix(m2);
	}
	else if( m_nType==COORDSYS_44MATRIX )
	{
		matrix_multiply(m_matrix,m,4,m2);
		Create44Matrix(m2);
	}
}

MyNameSpaceEnd