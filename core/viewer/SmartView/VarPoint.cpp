// VarPoint.cpp: implementation of the CVarPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VarPoint.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVarPoint::CVarPoint()
{
	memset(&m_pt,0,sizeof(m_pt));
	m_nType = typeInvalid;
}

CVarPoint::~CVarPoint()
{

}


CVarPoint::CVarPoint(const CVarPoint& varSrc)
{
	m_pt = varSrc.m_pt;
	m_nType = varSrc.m_nType;
}


CVarPoint::CVarPoint(double x, double y)
{
	m_nType = type2D;
	m_pt.m_c[0] = x;
	m_pt.m_c[1] = y;
}


CVarPoint::CVarPoint(double x, double y, double z)
{
	m_nType = type3D;
	m_pt.m_c[0] = x;
	m_pt.m_c[1] = y;
	m_pt.m_c[2] = z;
}


CVarPoint::CVarPoint(double xl, double yl, double xr, double yr)
{
	m_nType = type4D;
	m_pt.m_c[0] = xl;
	m_pt.m_c[1] = yl;
	m_pt.m_c[2] = xr;
	m_pt.m_c[3] = yr;
}

CVarPoint::CVarPoint( const PT_2D varSrc )
{
	m_nType = type2D;
	m_pt.m_c[0] = varSrc.x;
	m_pt.m_c[1] = varSrc.y;
}


CVarPoint::CVarPoint( const PT_3D varSrc )
{
	m_nType = type3D;
	m_pt.m_c[0] = varSrc.x;
	m_pt.m_c[1] = varSrc.y;
	m_pt.m_c[2] = varSrc.z;
}


CVarPoint::CVarPoint( const PT_4D varSrc )
{
	m_nType = type4D;
	m_pt.m_c[0] = varSrc.x;
	m_pt.m_c[1] = varSrc.y;
	m_pt.m_c[2] = varSrc.z;
	m_pt.m_c[3] = varSrc.yr;
}


CVarPoint::CVarPoint( const GR_PT_2D varSrc )
{
	m_nType = type2D;
	m_pt.m_c[0] = varSrc.x;
	m_pt.m_c[1] = varSrc.y;
}

CVarPoint::operator PT_2D() const
{
	return PT_2D(m_pt.m_c[0],m_pt.m_c[1]);
}

CVarPoint::operator PT_3D() const
{
	return PT_3D(m_pt.m_c[0],m_pt.m_c[1],m_pt.m_c[2]);
}

CVarPoint::operator PT_4D() const
{
	return PT_4D(m_pt.m_c[0],m_pt.m_c[1],m_pt.m_c[2],m_pt.m_c[3]);
}

CVarPoint::operator GR_PT_2D() const
{
	return GR_PT_2D(m_pt.m_c[0],m_pt.m_c[1]);
}


void CVarPoint::CopyValue(const CVarPoint& varSrc)
{
	if( varSrc.m_nType==type2D )
	{
		m_pt.m_c[0] = varSrc.m_pt.m_c[0];
		m_pt.m_c[1] = varSrc.m_pt.m_c[1];
	}
	else if( varSrc.m_nType==type3D )
	{
		m_pt.m_c[0] = varSrc.m_pt.m_c[0];
		m_pt.m_c[1] = varSrc.m_pt.m_c[1];
		m_pt.m_c[2] = varSrc.m_pt.m_c[2];
	}
	else if( varSrc.m_nType==type4D )
	{
		m_pt.m_c[0] = varSrc.m_pt.m_c[0];
		m_pt.m_c[1] = varSrc.m_pt.m_c[1];
		m_pt.m_c[2] = varSrc.m_pt.m_c[2];
		m_pt.m_c[3] = varSrc.m_pt.m_c[3];
	}
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVarPointArray::CVarPointArray()
{
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;

	m_npt = 0;
}

CVarPointArray::~CVarPointArray()
{
	Clear();
}

CVarPointArray::CVarPointArray(PT_2D *pts, int npt)
{
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;
	
	m_npt = 0;

	Copy(pts,npt);
}

CVarPointArray::CVarPointArray(PT_3D *pts, int npt)
{
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;
	
	m_npt = 0;

	Copy(pts,npt);
}

CVarPointArray::CVarPointArray(PT_4D *pts, int npt)
{
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;
	
	m_npt = 0;

	Copy(pts,npt);
}

CVarPointArray::CVarPointArray(GR_PT_2D *pts, int npt)
{
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;
	
	m_npt = 0;

	Copy(pts,npt);
}

CVarPointArray::CVarPointArray(CVarPoint *pts, int npt)
{
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;
	
	m_npt = 0;

	Copy(pts,npt);
}


void CVarPointArray::Clear()
{
	if( m_pts )delete[] m_pts;
	m_nType = CVarPoint::typeInvalid;
	m_pts = NULL;
	
	m_npt = 0;
}


BOOL CVarPointArray::AllocMem(int npt)
{
	if( npt<=0 )return FALSE;

	m_pts = new CVarPoint::PointData[npt];
	if( !m_pts )return FALSE;

	m_npt = npt;
	return TRUE;
}

void CVarPointArray::Copy(CVarPoint *pts, int npt)
{
	Clear();
	
	if( !AllocMem(npt) )
		return;

	m_nType = pts[0].m_nType;
	
	for( int i=0; i<npt; i++)
	{
		m_pts[i] = pts[i].m_pt;
	}
}

void CVarPointArray::Copy(const CVarPointArray& arr)
{
	Clear();
	if( arr.m_pts!=NULL )
	{
		if( !AllocMem(arr.m_npt) )
			return;
		memcpy(m_pts,arr.m_pts,sizeof(CVarPoint::PointData)*arr.m_npt);
		m_npt = arr.m_npt;

		m_nType = arr.m_nType;
	}
}

void CVarPointArray::Copy(const PT_2D *pts, int npt)
{
	Clear();

	if( !AllocMem(npt) )
		return;

	m_nType = CVarPoint::type2D;

	for( int i=0; i<npt; i++)
	{
		m_pts[i] = CVarPoint(pts[i]).m_pt;
	}
}

void CVarPointArray::Copy(const PT_3D *pts, int npt)
{
	Clear();
	
	if( !AllocMem(npt) )
		return;
	
	m_nType = CVarPoint::type3D;
	
	for( int i=0; i<npt; i++)
	{
		m_pts[i] = CVarPoint(pts[i]).m_pt;
	}
}

void CVarPointArray::Copy(const PT_4D *pts, int npt)
{
	Clear();
	
	if( !AllocMem(npt) )
		return;
	
	m_nType = CVarPoint::type4D;
	
	for( int i=0; i<npt; i++)
	{
		m_pts[i] = CVarPoint(pts[i]).m_pt;
	}
}

void CVarPointArray::Copy(const GR_PT_2D *pts, int npt)
{
	Clear();
	
	if( !AllocMem(npt) )
		return;
	
	m_nType = CVarPoint::type2D;
	
	for( int i=0; i<npt; i++)
	{
		m_pts[i] = CVarPoint(pts[i]).m_pt;
	}
}


CVarPoint CVarPointArray::GetAt( int nIndex ) const
{
	if( m_pts!=NULL )
	{
		CVarPoint pt;
		pt.m_nType = m_nType;
		pt.m_pt = m_pts[nIndex];
		return pt;
	}

	return CVarPoint();
}

void CVarPointArray::SetAt( int nIndex, const CVarPoint& pt )
{
	if( m_pts!=NULL )
	{
		m_pts[nIndex] = pt.m_pt;
		m_nType = pt.m_nType;
	}
}


MyNameSpaceEnd