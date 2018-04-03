// GeoDirPoint.cpp: implementation of the CGeoDirPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "resource.h"
#include "GeoDirPoint.h"
//#include "RegDef.h "
#include "RegDef2.h"
#include "SmartViewFunctions.h"
MyNameSpaceBegin
///////////////////////////////////////////////////
//////// CGeoDirPoint
///////////////////////////////////////////////////
float PT_LEN = 1;
float DIR_LEN = 1;

extern float GetSymbolDrawScale();
// float GetDefaultSymbolDrawScale()
// {
// 	float fDrawScale = 1.0;
// 	BOOL bDefault = AfxGetApp()->GetProfileInt(REGPATH_SYMBOL,REGPATH_USEDEFAULT,1);
// 	if( bDefault )
// 		fDrawScale = 1.0;
// 	else
// 		fDrawScale = GetProfileDouble(REGPATH_SYMBOL,REGPATH_SYMBOLDRAWSCAL2,1.0);
	
// 	return fDrawScale;
// }

IMPLEMENT_DYNCREATE(CGeoDirPoint, CGeoPoint)

CGeoDirPoint::CGeoDirPoint()
{
	m_lfAngle = 0.0;	
}

CGeoDirPoint::~CGeoDirPoint()
{
}

float CGeoDirPoint::GetDefaultDirLen()
{
	return DIR_LEN;
}

int  CGeoDirPoint::GetClassType()const
{
	return CLS_GEODIRPOINT;
}
BOOL CGeoDirPoint::CopyFrom(const CGeometry *pObj)
{
	return CGeoPoint::CopyFrom(pObj);	
}

CGeometry* CGeoDirPoint::Clone()const
{
	CGeoDirPoint* pNew = new CGeoDirPoint;
	if( pNew )
	{
		pNew->CopyFrom(this);
	}
	return pNew;
}

BOOL CGeoDirPoint::CreateShape(const PT_3DEX *pts, int npt)
{
	if (npt > 1)
	{
		BOOL bResult = CGeoPoint::CreateShape(pts,1);
		SetCtrlPoint(0,*(pts+1));
		return bResult;
	}
	else
		return CGeoPoint::CreateShape(pts,npt);
}

Envelope CGeoDirPoint::GetEnvelope()
{
	float fSymLen = DIR_LEN * GetSymbolDrawScale();

	PT_3DEX pts[2];
	pts[0] = m_pt;
	double angle = m_lfAngle*PI/180.0;
	pts[1].x = m_pt.x+cos(angle)*fSymLen;
	pts[1].y = m_pt.y+sin(angle)*fSymLen;
	pts[1].z = m_pt.z;

	Envelope e;
	e.CreateFromPts(pts,2,sizeof(PT_3DEX),3);
	return e;
}

int CGeoDirPoint::GetCtrlPointSum()const
{
	return 1; 
}

PT_3DEX CGeoDirPoint::GetCtrlPoint(int num)const
{
	PT_3DEX pt(0,0,0,penNone,0,ctrlType_Dir);

	if( num<0 || num>=GetCtrlPointSum() )return PT_3DEX();

	float fSymLen = DIR_LEN * GetSymbolDrawScale();

	if( num==0 )
	{		
		PT_3DEX t;
		t = GetDataPoint(0);
		double angle = GetDirection()*PI/180.0;
		pt.x = t.x+cos(angle)*fSymLen;
		pt.y = t.y+sin(angle)*fSymLen;
		pt.z = t.z;
		
		return pt; 
	}

	return pt;
}



BOOL CGeoDirPoint::SetCtrlPoint(int num, PT_3D pt)
{ 
	if( num<0 || num>=GetCtrlPointSum() )return FALSE;

	float fSymLen = DIR_LEN * GetSymbolDrawScale();
	
	if( num==0 )
	{
		PT_3DEX t;
		t = GetDataPoint(0);
		
		double ang = GraphAPI::GGetAngle(t.x,t.y,pt.x,pt.y);
		
		SetDirection(ang*180.0/PI);
	}

	return TRUE;
}



BOOL CGeoDirPoint::WriteTo(CValueTable& tab)const
{
	CGeoPoint::WriteTo(tab);
	
// 	CVariantEx var;
// 	var = (_variant_t)(double)m_lfAngle;
// 	tab.AddValue(FIELDNAME_GEOPOINT_ANGLE,&var,IDS_FIELDNAME_DIRPOINT_ANGLE);
	
	return TRUE;
}

BOOL CGeoDirPoint::ReadFrom(CValueTable& tab,int idx)
{
	CGeoPoint::ReadFrom(tab,idx);
	
// 	const CVariantEx *var;
// 	if( tab.GetValue(idx,FIELDNAME_GEOPOINT_ANGLE,var) )
// 	{
// 		m_lfAngle = (double)(_variant_t)*var;
// 	}
	
	return TRUE;
}

void CGeoDirPoint::Draw(GrBuffer *pBuf, float fDrawScale)
{
	if (pBuf)
	{
		PT_3D pt0,pt1;
		
	
	//	pBuf->(m_nColor);
		pBuf->BeginLineString(m_nColor,0,0);
		pBuf->MoveTo(&m_pt);

		double offlen = DIR_LEN * fDrawScale;//GetSymbolDrawScale();
		
		double ang = GetDirection()*PI/180;
		double cosa= cos(ang), sina= sin(ang);
		pt1.x = m_pt.x+cosa*offlen; pt1.y = m_pt.y+sina*offlen; pt1.z = m_pt.z;
		pBuf->LineTo(&pt1);
		
		pt0 = pt1; offlen = DIR_LEN*0.2*fDrawScale;/*GetSymbolDrawScale();*/ ang -= PI;
		
		cosa= cos(ang-PI/6), sina= sin(ang-PI/6);
		pt1.x = pt0.x+cosa*offlen; pt1.y = pt0.y+sina*offlen; pt1.z = pt0.z;

		pBuf->MoveTo(&pt1);
		pBuf->LineTo(&pt0);
		
		cosa= cos(ang+PI/6), sina= sin(ang+PI/6);
		pt1.x = pt0.x+cosa*offlen; pt1.y = pt0.y+sina*offlen; pt1.z = pt0.z;
		pBuf->MoveTo(&pt1);
		pBuf->LineTo(&pt0);
		pBuf->End();
	}
	
}

MyNameSpaceEnd