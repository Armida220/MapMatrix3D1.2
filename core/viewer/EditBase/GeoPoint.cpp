// EBGeoPoint.cpp: implementation of the CGeoPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
//#include "EditBase.h"
#include "GeoPoint.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define PTFLAG_FLAT			0x01
#define PTFLAG_GRDWID		0x02


MyNameSpaceBegin

static float PT_LEN = 1;
static float DIR_LEN = 2;

extern float GetSymbolDrawScale();

IMPLEMENT_DYNCREATE(CGeoPoint, CPermanent)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoPoint::CGeoPoint()
{
	m_pt.x = 0; m_pt.y = 0; m_pt.z = 0;
	m_fKx = m_fKy = 1;
	m_lfAngle = m_fWidth = 0;
	m_nCoverType = COVERTYPE_NONE;
	m_fExtendDis = 0;
}

CGeoPoint::~CGeoPoint()
{

}

Envelope CGeoPoint::GetEnvelope()
{
	Envelope e;
	e.CreateFromPts(&m_pt,1,sizeof(PT_3DEX),3);
	if( IsGrdWid() )e.Inflate(m_fKx*0.5,m_fKy*0.5,0);
	return e;
}
int  CGeoPoint::GetClassType()const
{
	return CLS_GEOPOINT;
}

int CGeoPoint::GetDataPointSum()const
{
	return 1;
}


BOOL CGeoPoint::SetDataPoint(int i,PT_3DEX pt)
{
	if( i==0 )
	{
		m_pt = pt;
		return TRUE;
	}
	return FALSE;
}


PT_3DEX CGeoPoint::GetDataPoint(int i)const
{
	if( i==0 )
	{
		return m_pt;
	}
	return PT_3DEX();
}


BOOL CGeoPoint::CreateShape(const PT_3DEX *pts, int npt)
{
	if (npt < 1)  return FALSE;
	
	m_pt = pts[0];

	return TRUE;
}

void CGeoPoint::GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	pts.RemoveAll();
	pts.Add(m_pt);
}


CGeometry* CGeoPoint::Clone()const
{
	CGeoPoint *pNew = new CGeoPoint;
	pNew->CopyFrom(this);
	return pNew;
}


BOOL CGeoPoint::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoPoint)) )
	{
		CGeoPoint *pPoint = ((CGeoPoint*)pObj);
		m_fKx = pPoint->m_fKx;
		m_fKy = pPoint->m_fKy;
		m_fWidth = pPoint->m_fWidth;
		m_lfAngle = pPoint->m_lfAngle;
		m_nCoverType = pPoint->m_nCoverType;
		m_fExtendDis = pPoint->m_fExtendDis;
		m_pt = pPoint->m_pt;
	}	

	return CGeometry::CopyFrom(pObj);
}

void CGeoPoint::EnableFlated(BOOL bFlat)
{
	int flag = 0;
	if( m_symname[0]=='%' )
	{
		flag = atol(m_symname+1);
	}
	if( bFlat )
	{
		flag |= PTFLAG_FLAT;
		sprintf(m_symname,"%%%d",flag);
	}
	else
	{
		flag &= (~PTFLAG_FLAT);
		sprintf(m_symname,"%%%d",flag);
	}
}


BOOL CGeoPoint::IsFlatedPoint()
{
	int flag = 0;
	if( m_symname[0]=='%' )
	{
		flag = atol(m_symname+1);
	}
	if( flag&PTFLAG_FLAT )
		return TRUE;

	return FALSE;
}



void CGeoPoint::EnableGrdWid(BOOL bGrdWid)
{
	int flag = 0;
	if( m_symname[0]=='%' )
	{
		flag = atol(m_symname+1);
	}
	if( bGrdWid )
	{
		flag |= PTFLAG_GRDWID;
		sprintf(m_symname,"%%%d",flag);
	}
	else
	{
		flag &= (~PTFLAG_GRDWID);
		sprintf(m_symname,"%%%d",flag);
	}
}


BOOL CGeoPoint::IsGrdWid()
{
	int flag = 0;
	if( m_symname[0]=='%' )
	{
		flag = atol(m_symname+1);
	}
	if( flag&PTFLAG_GRDWID )
		return TRUE;
	
	return FALSE;
}

void CGeoPoint::Draw(GrBuffer *pBuf, float fDrawScale)
{
	if( IsGrdWid() )
		pBuf->Point(m_nColor,&m_pt,m_fKx*fDrawScale,m_fKy*fDrawScale,0,IsGrdWid(),0,m_fWidth*fDrawScale,IsFlatedPoint());
	else if( m_symname[0]==0 )
		pBuf->Point(m_nColor,&m_pt,m_fKx,m_fKy,0,IsGrdWid(),0,m_fWidth*fDrawScale,IsFlatedPoint());
	else
		pBuf->Point(m_nColor,&m_pt,1,1,0,FALSE,0,m_fWidth*fDrawScale,FALSE);

}

BOOL CGeoPoint::WriteTo(CValueTable& tab)const
{
	CGeometry::WriteTo(tab);

	CVariantEx var;
	var = (_variant_t)m_fKx;
	tab.AddValue(FIELDNAME_GEOPOINT_KX,&var,IDS_FIELDNAME_GEOKX);

	var = (_variant_t)m_fKy;
	tab.AddValue(FIELDNAME_GEOPOINT_KY,&var,IDS_FIELDNAME_GEOKY);

	var = (_variant_t)m_lfAngle;
	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&var,IDS_FIELDNAME_DIRPOINT_ANGLE);

	var = (_variant_t)(long)m_nCoverType;
	tab.AddValue(FIELDNAME_GEOPOINT_COVERTYPE,&var,IDS_FIELDNAME_GEOCOVERTYPE);

	var = (_variant_t)m_fExtendDis;
	tab.AddValue(FIELDNAME_GEOPOINT_EXTENDDIS,&var,IDS_FIELDNAME_GEOEXTENDDIS);
	
	var = (_variant_t)m_fWidth;
	tab.AddValue(FIELDNAME_GEOPOINT_WIDTH,&var,IDS_FIELDNAME_GEOWIDTH);
	
	return TRUE;
}

BOOL CGeoPoint::ReadFrom(CValueTable& tab,int idx)
{
	CGeometry::ReadFrom(tab,idx);

	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_KX,var) )
	{
		m_fKx = (_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_KY,var) )
	{
		m_fKy = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_ANGLE,var) )
	{
		m_lfAngle = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_COVERTYPE,var) )
	{
		m_nCoverType = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_EXTENDDIS,var) )
	{
		m_fExtendDis = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_WIDTH,var) )
	{
		m_fWidth = (_variant_t)*var;
	}
	
	return TRUE;
}



//控制比例的两个控制点 单独使用，所以拆分为新的函数
PT_3DEX CGeoPoint::GetCtrlPoint2(int num)const
{
	PT_3DEX pt(0,0,0,penNone,0,ctrlType_Dir);

	float fSymLen = DIR_LEN * GetSymbolDrawScale();

	if( num==0 )
	{		
		PT_3DEX t;
		t = GetDataPoint(0);
		double angle = m_lfAngle*PI/180.0;
		pt.x = t.x+cos(angle)*fSymLen;
		pt.y = t.y+sin(angle)*fSymLen;
		pt.z = t.z;
		
		return pt; 
	}
	//scale x
	else if( num==1 )
	{
		fSymLen = -fSymLen*0.5*m_fKx;
		
		PT_3DEX t;
		t = GetDataPoint(0);
		double angle = m_lfAngle*PI/180.0;
		pt.x = t.x+cos(angle)*fSymLen;
		pt.y = t.y+sin(angle)*fSymLen;
		pt.z = t.z;
		pt.type = ctrlType_Scale;
		
		return pt; 
	}
	//scale y
	else if( num==2 )
	{
		fSymLen = fSymLen*0.5*m_fKy;
		
		PT_3DEX t;
		t = GetDataPoint(0);
		double angle = (m_lfAngle+90)*PI/180.0;
		pt.x = t.x+cos(angle)*fSymLen;
		pt.y = t.y+sin(angle)*fSymLen;
		pt.z = t.z;
		
		pt.type = ctrlType_Scale;
		
		return pt; 
	}
	return PT_3DEX();
}


//控制比例的两个控制点 单独使用，所以拆分为新的函数
BOOL CGeoPoint::SetCtrlPoint2(int num, PT_3D pt, BOOL bSynchXY)
{
	float fSymLen = DIR_LEN * GetSymbolDrawScale();

	if( num==0 )
	{
		PT_3DEX t;
		t = GetDataPoint(0);
		
		double ang = GraphAPI::GGetAngle(t.x,t.y,pt.x,pt.y);
		
		m_lfAngle = (ang*180.0/PI);
	}
	else if( num==1 )
	{
		PT_3DEX t;
		t = GetDataPoint(0);
		
		double angle = m_lfAngle*PI/180.0;
		
		PT_3D t2, ret;
		t2.x = t.x+cos(angle)*fSymLen;
		t2.y = t.y+sin(angle)*fSymLen;
		t2.z = t.z;
		
		GraphAPI::GGetPerpendicular(t.x,t.y,t2.x,t2.y,pt.x,pt.y,&ret.x,&ret.y,NULL);
		
		double dis = GraphAPI::GGet2DDisOf2P(ret,(PT_3D)t);
		fSymLen *= 0.5;

		float oldkx = m_fKx;
		
		m_fKx = dis/fSymLen;
		if( bSynchXY )
			m_fKy = m_fKy * m_fKx/oldkx;
	}
	else if( num==2 )
	{
		PT_3DEX t;
		t = GetDataPoint(0);
		
		double angle = (m_lfAngle+90)*PI/180.0;
		
		PT_3D t2, ret;
		t2.x = t.x+cos(angle)*fSymLen;
		t2.y = t.y+sin(angle)*fSymLen;
		t2.z = t.z;
		
		GraphAPI::GGetPerpendicular(t.x,t.y,t2.x,t2.y,pt.x,pt.y,&ret.x,&ret.y,NULL);
		
		double dis = GraphAPI::GGet2DDisOf2P(ret,(PT_3D)t);
		fSymLen *= 0.5;

		float oldky = m_fKy;
		
		m_fKy = dis/fSymLen;

		if( bSynchXY )
			m_fKx = m_fKx * m_fKy/oldky;
	}
	
	return TRUE;
}

double CGeoPoint::GetDirection()const
{
	return m_lfAngle;
}

void CGeoPoint::SetDirection(double angle)
{
	m_lfAngle = angle;
}




IMPLEMENT_DYNCREATE(CGeoSurfacePoint, CGeoPoint)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoSurfacePoint::CGeoSurfacePoint()
{
}

CGeoSurfacePoint::~CGeoSurfacePoint()
{
	
}

int  CGeoSurfacePoint::GetClassType()const
{
	return CLS_GEOSURFACEPOINT;
}


CGeometry* CGeoSurfacePoint::Clone()const
{
	CGeoSurfacePoint *pNew = new CGeoSurfacePoint();
	pNew->CopyFrom(this);
	return pNew;
}


void CGeoSurfacePoint::Draw(GrBuffer *pBuf, float fDrawScale)
{
	pBuf->Point(RGB(255,0,0),&m_pt,5,5,0,FALSE,0,0,FALSE);	
	pBuf->Point(RGB(0,255,0),&m_pt,3,3,0,FALSE,0,0,FALSE);	
	pBuf->Point(RGB(255,0,0),&m_pt,1,1,0,FALSE,0,0,FALSE);	
}

MyNameSpaceEnd