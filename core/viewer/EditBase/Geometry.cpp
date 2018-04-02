// EBGeometry.cpp: implementation of the CGeometry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "resource.h"
#include "Geometry.h"
#include "Matrix.h"
#include "SmartViewFunctions.h"
#include "CoordSys.h"
#include "Linearizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


FlagBit::FlagBit()
{
	m_nValue = 0;
}

FlagBit::FlagBit(long a)
{
	m_nValue = a;
}

FlagBit::~FlagBit()
{
}


FlagBit::operator long()
{
	return m_nValue;
}

void FlagBit::SetBit(int nBit, BOOL bEnable)
{
	if( nBit>=0 && nBit<24 )
	{
		nBit += 8;
		if( bEnable )
		{
			m_nValue = (m_nValue|(1<<nBit));
		}
		else
		{
			m_nValue = (m_nValue&(~(1<<nBit)));
		}
	}
}

BOOL FlagBit::GetBit(int nBit)const
{
	if( nBit>=0 && nBit<24 )
	{
		nBit += 8;
		return ((m_nValue&(1<<nBit))!=0);
	}
	return FALSE;
}

void FlagBit::SetEnumValue(BYTE value)
{
	m_nValue = ((m_nValue>>8)<<8) + value;
}


BYTE FlagBit::GetEnumValue()const
{
	return (BYTE)(m_nValue&0xff);
}
 

IMPLEMENT_DYNAMIC(CGeometry,CPermanent)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometry::CGeometry()
{
	m_nColor = COLOUR_BYLAYER;
	memset(m_symname,0x00,32);

	m_nSymbolizeFlag = 0;

	FlagBit bit(m_nSymbolizeFlag);
	bit.SetBit(SYMFLAG_TEXT,TRUE);

	m_nSymbolizeFlag = (long)bit;
}

CGeometry::~CGeometry()
{

}

int  CGeometry::GetClassType()const
{
	return CLS_GEOMETRY;
}


PT_3DEX CGeometry::GetDataPoint(int i)const
{
	return PT_3DEX();
}

BOOL CGeometry::SetDataPoint(int i, PT_3DEX pt)
{
	return TRUE;
}

int CGeometry::GetDataPointSum()const
{
	return 0;
}

BOOL CGeometry::SetCtrlPoint(int i,PT_3D pt)
{
	return TRUE;
}

PT_3DEX CGeometry::GetCtrlPoint(int i)const
{
	PT_3DEX pt(0,0,0,penNone);
	return pt;
}

int CGeometry::GetCtrlPointSum()const
{
	return 0;
}


BOOL CGeometry::CheckValid()const
{
	return TRUE;
}


BOOL CGeometry::CopyFrom(const CGeometry *pObj)
{
	m_nColor = pObj->m_nColor;
	m_nSymbolizeFlag = pObj->m_nSymbolizeFlag;
	strncpy(m_symname,pObj->m_symname,32);
	m_nSymbolizeFlag = pObj->m_nSymbolizeFlag;
		
	return TRUE;
}


void CGeometry::Transform(double matrix[16])
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	GetShape(arrPts);
	
	PT_3DEX *pts = arrPts.GetData();
	int npt = arrPts.GetSize();
	
	double vo[4]={ 0,0,0,1.0 };
	double vr[4];

	for( int i=npt-1; i>=0; i--)
	{
		vo[0] = pts[i].x, vo[1] = pts[i].y, vo[2] = pts[i].z;
		matrix_multiply_byvector(matrix,4,4,vo,vr);
		pts[i].x = vr[0], pts[i].y = vr[1], pts[i].z = vr[2];
	}
	
	CreateShape(pts,npt);
}

void CGeometry::ReversePoints()
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	GetShape(arrPts);

	int npt = arrPts.GetSize();
	if( npt>1 && arrPts[0].pencode==penMove )
	{
		arrPts[0].pencode = arrPts[1].pencode;
	}

	for (int i=0; i<npt/2; i++)
	{
		PT_3DEX pt = arrPts[i];
		arrPts[i] = arrPts[npt-1-i];
		arrPts[npt-1-i] = pt;
	}

	CreateShape(arrPts.GetData(),arrPts.GetSize());
}

BOOL CGeometry::WriteTo(CValueTable& tab)const
{
	CVariantEx var;
	var = (_variant_t)(long)GetClassType();
	tab.AddValue(FIELDNAME_GEOCLASS,&var,IDS_FIELDNAME_GEOCLASS);

 	var = (_variant_t)(const char*)GetSymbolName();
 	tab.AddValue(FIELDNAME_SYMBOLNAME,&var,IDS_FIELDNAME_GEOSYMBOLNAME);

	var = (_variant_t)(long)m_nColor;
	tab.AddValue(FIELDNAME_GEOCOLOR,&var,IDS_FIELDNAME_COLOR);

	var = (_variant_t)(long)m_nSymbolizeFlag;
	tab.AddValue(FIELDNAME_SYMBOLIZEFLAG,&var,IDS_FIELDNAME_SYMBOLIZEFLAG);
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	GetShape(arrPts);
	var.SetAsShape(arrPts);
	tab.AddValue(FIELDNAME_SHAPE,&var,IDS_FIELDNAME_SHAPE);
	
	return TRUE;
}

BOOL CGeometry::ReadFrom(CValueTable& tab,int idx)
{
	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOCOLOR,var) )
	{
		m_nColor = (long)(_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_SYMBOLNAME,var) )
	{
		CString str = (LPCTSTR)(_bstr_t)(_variant_t)*var;
		str = str.Left(31);
		strcpy(m_symname,str);
	}

	if( tab.GetValue(idx,FIELDNAME_SYMBOLIZEFLAG,var) )
	{
		m_nSymbolizeFlag = (long)(_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_SHAPE,var) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		var->GetShape(arrPts);
		return CreateShape(arrPts.GetData(),arrPts.GetSize());
	}

	return TRUE;
}

void CGeometry::SetColor(long color)
{
	m_nColor = color;
}


long CGeometry::GetColor()
{
	return m_nColor;
}

void CGeometry::SetSymbolName(const char* name)
{
	if (!name)
	{
		m_symname[0] = '\0';
		return;
	}

	strncpy(m_symname,name,31); 
	m_symname[31] = '\0';
}

void CGeometry::SetSymbolizeFlagBit(int nBit, BOOL bEnable)
{
	FlagBit bit(m_nSymbolizeFlag);
	bit.SetBit(nBit,bEnable);
	
	m_nSymbolizeFlag = (long)bit;
}


BOOL CGeometry::GetSymbolizeFlagBit(int nBit)const
{
	FlagBit bit(m_nSymbolizeFlag);
	return bit.GetBit(nBit);
}

void CGeometry::SetSymbolizeFlagEnum(BYTE value)
{
	FlagBit bit(m_nSymbolizeFlag);
	bit.SetEnumValue(value);
	
	m_nSymbolizeFlag = (long)bit;
}


BYTE CGeometry::GetSymbolizeFlagEnum()const
{
	FlagBit bit(m_nSymbolizeFlag);
	return bit.GetEnumValue();
}

long CGeometry::GetSymbolizeFlag()const
{
	return m_nSymbolizeFlag;
}


void CGeometry::SetSymbolizeFlag(long flag)
{
	m_nSymbolizeFlag = flag;
}

BOOL CGeometry::IsSymbolizeText()const
{
	return GetSymbolizeFlagBit(SYMFLAG_TEXT);
}

BOOL CGeometry::IsHideSymbolizeSurfaceBound()const
{
	return GetSymbolizeFlagBit(SYMFLAG_HIDE_SURFACE_BOUND);
}

BOOL CGeometry::IsHideSymbolizeSurfaceInnerBound()const
{
	return GetSymbolizeFlagBit(SYMFLAG_HIDE_SURFACE_INNER_BOUND);
}	

void CGeometry::EnableSymbolizeText(BOOL bSymbolized)
{
	SetSymbolizeFlagBit(SYMFLAG_TEXT,bSymbolized);
}

const char* CGeometry::GetSymbolName() const
{
	return m_symname;
}

int CGeometry::FindNearestKeyLine(PT_3D sch_pt, double r, CCoordSys *pCS)
{
	PT_3DEX kpt0,kpt1,kpt;
	int num = GetDataPointSum(), k=-1;
	if( num<=1 )return -1;
	
	kpt = GetDataPoint(0);
	pCS->GroundToClient(&kpt,&kpt1);
	
	double d = r, m=-1, t;
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);
	for( int i=1; i<num; i++)
	{
		kpt0 = kpt1;
		kpt = pts[i];
		pCS->GroundToClient(&kpt,&kpt1);
		
		t = GraphAPI::GGetNearestDisOfPtToLine(kpt0.x,kpt0.y,
			kpt1.x,kpt1.y,sch_pt.x,sch_pt.y,NULL,NULL,false);
		
		if( t<d && (k<0||m>t) )
		{
			k = i-1;
			m = t;
		}
	}
	return k;
}



PT_KEYCTRL CGeometry::FindNearestKeyCtrlPt(PT_3D sch_pt, double r, CCoordSys *pCS, int flag)
{
	PT_KEYCTRL ret;
	double d = r*r, m=-1, t;
	PT_3D *pt = &sch_pt;
	if( flag&1 )
	{
		PT_3DEX kpt, kpt1;
		CArray<PT_3DEX,PT_3DEX> pts;
		GetShape(pts);	
		int num = pts.GetSize(), k=-1;		
//		kpt1 = GetDataPoint(0);
		for( int i=0; i<num; i++)
		{
			kpt1 = pts[i];
			pCS->GroundToClient(&kpt1,&kpt);
			t = (kpt.x-pt->x)*(kpt.x-pt->x) + (kpt.y-pt->y)*(kpt.y-pt->y);
			if( t<d && (k<0||m>t) )
			{
				k = i;
				m = t;
			}
		}		
		if( k>=0 )
		{
			ret.type = PT_KEYCTRL::typeKey;
			ret.index= k;
		}
	}
	if( flag&2 )
	{
		PT_3D cpt,cpt1;
		int num = GetCtrlPointSum(), k=-1;
		double m2 = -1;
		for( int i=0; i<num; i++)
		{
			cpt1 = GetCtrlPoint(i);
			pCS->GroundToClient(&cpt1,&cpt);
			t = (cpt.x-pt->x)*(cpt.x-pt->x) + (cpt.y-pt->y)*(cpt.y-pt->y);
			if( t<d && (k<0||m>t) )
			{
				k = i;
				m2 = t;
			}
		}
		
		if( k>=0 && (m2<m||m<0) )
		{
			ret.type = PT_KEYCTRL::typeCtrl;
			ret.index= k;
		}
	}
	return ret;
}


// search a line segment in an object
BOOL CGeometry::FindNearestBaseLine(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *ret1, PT_3D *ret2, double *mindis)
{
	PT_3DEX kpt0,kpt1,kpt, ret;
	int num = GetDataPointSum(), k=-1;
	if( num<=1 )return FALSE;
	Envelope e;	
	double m=-1, t;
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);
	kpt = pts.GetAt(0);
	pCS->GroundToClient(&kpt,&kpt1);
	for( int i=1; i<num; i++)
	{
		kpt0 = kpt1;
		kpt = pts[i];
		pCS->GroundToClient(&kpt,&kpt1);
		
		t = GraphAPI::GGetNearestDisOfPtToLine(kpt0.x,kpt0.y,
			kpt1.x,kpt1.y,sch_pt.x,sch_pt.y,&ret.x,&ret.y,false);

		if( !sch_evlp.bPtIn(&ret) )
			continue;
		
		if( (k<0||m>t) )
		{
			k = i-1;
			m = t;
		}
	}

	if( k>=0 )
	{
		if( ret1 )*ret1 = pts.GetAt(k);
		if( ret2 )*ret2 = pts.GetAt(k+1);
		if( mindis )*mindis = m;
	}

	return (k>=0);
}

// search a point in an object
BOOL CGeometry::FindNearestBasePt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis)
{
	PT_3DEX kpt0,kpt1,kpt, ret, mret;
	int num = GetDataPointSum(), k=-1;
	if( num<1 )return FALSE;
	if (num==1)
	{
		kpt = GetDataPoint(0);
		pCS->GroundToClient(&kpt,&kpt1);
		if( !sch_evlp.bPtIn(&kpt1) )
			return FALSE;
		if( pRet )*pRet = kpt;		
		if( mindis )*mindis = sqrt((kpt1.x-sch_pt.x)*(kpt1.x-sch_pt.x)+(kpt1.y-sch_pt.y)*(kpt1.y-sch_pt.y));
		return TRUE;
	}
	
	kpt = GetDataPoint(0);
	pCS->GroundToClient(&kpt,&kpt1);

	double m=-1, t, lfk;
	CArray<PT_3DEX,PT_3DEX> pts;
	const CShapeLine *pShp = GetShape();
	if( pShp )
		pShp->GetPts(pts);
	else
		GetShape(pts);

	num = pts.GetSize();
	for( int i=1; i<num; i++)
	{
		kpt0 = kpt1;
		kpt = pts[i];
		pCS->GroundToClient(&kpt,&kpt1);
		
		t = GraphAPI::GGetNearestDisOfPtToLine(kpt0.x,kpt0.y,
			kpt1.x,kpt1.y,sch_pt.x,sch_pt.y,&ret.x,&ret.y,false);
		
		if( !sch_evlp.bPtIn(&ret) )
			continue;
		
		if( (k<0||m>t) )
		{
			k = i-1;
			m = t;
		}
	}
	
	if( k>=0 )
	{
		kpt = pts[k];
		pCS->GroundToClient(&kpt,&kpt0);

		kpt = pts[k+1];
		pCS->GroundToClient(&kpt,&kpt1);

		t = GraphAPI::GGetNearestDisOfPtToLine(kpt0.x,kpt0.y,
			kpt1.x,kpt1.y,sch_pt.x,sch_pt.y,&ret.x,&ret.y,false);

		double r1 = fabs(kpt1.x-kpt0.x)+fabs(kpt1.y-kpt0.y);
		double r2 = fabs(ret.x-kpt0.x)+fabs(ret.y-kpt0.y);
		if( r1<1e-10 )lfk = 0;
		else lfk = r2/r1;

		kpt0 = pts[k];
		kpt1 = pts[k+1];
		
		ret.x = kpt0.x + lfk*(kpt1.x-kpt0.x);
		ret.y = kpt0.y + lfk*(kpt1.y-kpt0.y);
		ret.z = kpt0.z + lfk*(kpt1.z-kpt0.z);
		if( pRet )*pRet = ret;

		if( mindis )*mindis = m;
	}
	
	return (k>=0);
}

double GetTriangleArea(PT_3D pts[3])
{
	double v1x = pts[1].x-pts[0].x, v1y = pts[1].y-pts[0].y;
	double v2x = pts[2].x-pts[1].x, v2y = pts[2].y-pts[1].y;
	return (v1x*v2y-v1y*v2x)/2;
}

BOOL CGeometry::GetCenter(PT_3D *ipt, PT_3D *opt)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);
	int num = pts.GetSize();
	if( num<=0 )return FALSE;
	
	PT_3DEX t;
	if( num==1 )
	{
		t = pts[0];
		COPY_3DPT((*opt),t);
		return TRUE;
	}
	else if( num==2 )
	{
		t = pts[0];
		COPY_3DPT((*opt),t);
		t = pts[1];
		opt->x = (opt->x+t.x)/2;
		opt->y = (opt->y+t.y)/2;
		opt->z = (opt->z+t.z)/2;
		return TRUE;
	}
	
	PT_3D pts0[3], cpt, cpt0;
	t = pts[0]; 
	COPY_3DPT(pts0[0],t);
	t = pts[1]; 
	COPY_3DPT(pts0[1],t);
	double area = 0, sarea = 0;
	for( int i=2; i<num; i++ )
	{
		t = pts[i]; 
		COPY_3DPT(pts0[2],t);
		
		cpt.x = (pts0[0].x+pts0[1].x+pts0[2].x)/3;
		cpt.y = (pts0[0].y+pts0[1].y+pts0[2].y)/3;
		cpt.z = (pts0[0].z+pts0[1].z+pts0[2].z)/3;

		area = GetTriangleArea(pts0);
		sarea += area;
		cpt0.x += (cpt.x*area); cpt0.y += (cpt.y*area); cpt0.z += (cpt.z*area);

		COPY_3DPT(pts0[1],pts0[2]);
	}
	
	if( fabs(sarea)<=1e-10 )*opt = pts0[0];
	else 
	{
		cpt0.x /= sarea; cpt0.y /= sarea; cpt0.z /= sarea;
	}
	*opt = cpt0;
	
	return TRUE;
	
}

int CGeometry::bPtIn(const PT_3D *pt)
{
	return 0;
}


MyNameSpaceEnd
