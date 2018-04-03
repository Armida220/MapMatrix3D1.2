// EBGeoCurve.cpp: implementation of the CGeoCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
//#include "EditBase.h"
#include "GeoCurve.h"
#include "Linearizer.h"
#include "SmartViewFunctions.h"
#include "FLOAT.H "
#include "RegDef2.h"
#include "SearchStruct.h"
#include "Symbol.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin


IMPLEMENT_DYNCREATE(CGeoCurveBase, CPermanent)

extern float GetSymbolDrawScale();

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoCurveBase::CGeoCurveBase()
{
	m_fLineWidth = -1;
}

CGeoCurveBase::~CGeoCurveBase()
{

}

int  CGeoCurveBase::GetClassType()const
{
	return CLS_GEOCURVE;
}

int CGeoCurveBase::GetDataPointSum()const
{	
	return m_shape.GetKeyPtsCount();
}


BOOL CGeoCurveBase::SetDataPoint(int i,PT_3DEX pt)
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);

	int nSum = arrPts.GetSize();
	
	if( i>=0 && i<nSum )
	{
		arrPts.SetAt(i,pt);
		CreateShape(arrPts.GetData(),nSum);
		return TRUE;
	}
	return FALSE;
}


PT_3DEX CGeoCurveBase::GetDataPoint(int i)const
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);

	int nSum = arrPts.GetSize();
	
	if( i>=0 && i<nSum )
	{
		return arrPts.GetAt(i);
	}
	return PT_3DEX();
}


BOOL CGeoCurveBase::CreateShape(const PT_3DEX *pts, int npt)
{
	m_shape.Clear();

	if( npt<=1 )
		return FALSE;	

	CLinearizer line;
	line.SetShapeBuf(&m_shape);
	line.Linearize(pts,npt,FALSE);

	m_shape.LineEnd();

	m_evlp = m_shape.GetEnvelope();

	return TRUE;
}

void CGeoCurveBase::GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	m_shape.GetKeyPts(pts);
}



void CGeoCurveBase::EnableFillColor(BOOL bFill, COLORREF clr)
{
	if( bFill )
	{
		DWORD val = GetTransparency()<<24;
		if(clr==COLOUR_BYLAYER)
		{
			if(m_nColor==COLOUR_BYLAYER)
			{
				sprintf(m_symname,"%%%%");
			}
			else
			{
				val += m_nColor;
				sprintf(m_symname,"%%%u%%",val);
			}
		}
		else
		{
			val += clr;
			sprintf(m_symname,"%%%u%%",val);
		}
	}
	else
		memset(m_symname,0,sizeof(m_symname));
}

BOOL CGeoCurveBase::IsFillColor()const
{
	return (m_symname[0]=='%');
}

COLORREF CGeoCurveBase::GetFillColor()const
{
	if( m_symname[0]=='%' )
	{
		if( m_symname[1]=='%' )
		{
			return -1;
		}
		DWORD clr = m_nColor;
		if( sscanf(m_symname,"%%%u%%",&clr)>0 )
		{
			clr = clr&0xffffff;
			return clr;
		}
		else
		{
			return m_nColor;
		}
	}
	
	return -1;
}

//透明度
DWORD CGeoCurveBase::GetTransparency()
{
	if( IsFillColor() )
	{
		DWORD clr = 0;
		if( sscanf(m_symname,"%%%u%%",&clr)>0 )
		{
			clr = clr>>24;
			return clr;
		}
	}
	
	return 0;
}

void CGeoCurveBase::SetTransparency(DWORD val)
{
	if( IsFillColor() )
	{
		DWORD val1 = val<<24;
		val1 += GetFillColor();
		sprintf(m_symname,"%%%u%%",val1);
	}
}

//长度
double CGeoCurveBase::GetLength()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);

	return GraphAPI::GGetAllLen2D(pts.GetData(),pts.GetSize());
}


//面积
double CGeoCurveBase::GetArea()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	
	return GraphAPI::GGetPolygonArea(pts.GetData(),pts.GetSize());
}


void CGeoCurveBase::GetBreak(PT_3D pt1, PT_3D pt2, CGeometry*& pObj1, CGeometry*& pObj2, CGeometry*& pObj3, BOOL bClose)
{	
	int size = m_shape.GetKeyPtsCount();
	if( size<=1 )return;

	CArray<int,int> arrKeyPos; //关键点在图形点中的索引数组
	if(!m_shape.GetKeyPosOfBaseLines(arrKeyPos))
		return;
	CArray<PT_3DEX,PT_3DEX> pts,pts1;
	if(!m_shape.GetKeyPts(pts1)) //关键点数组
		return;
	
	int num;
	CArray<PT_3DEX,PT_3DEX> arrPts;//图形点数组
	m_shape.GetPts(arrPts);
	//找最近的图形点
	PT_3DEX rpt1,rpt2;
	double x,y,z;
	double dis,min1=-1,min2=-1;
	int i1=-1, i2=-1, imin, imax;
	num = arrPts.GetSize();
	for( int i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(arrPts[i].x,arrPts[i].y,arrPts[i].z,
			arrPts[i+1].x,arrPts[i+1].y,arrPts[i+1].z,pt1.x,pt1.y,pt1.z,&x,&y,&z,false);

		if( min1<0 || dis<min1 )
		{ 
			min1 = dis; rpt1.x=x; rpt1.y=y; rpt1.z=z; 
			
			if( _FABS(x-arrPts[i+1].x)+_FABS(y-arrPts[i+1].y)>1e-10 )
				i1=i;
			//就是第二个点时，序号增加1
			else i1=i+1;
		}

		dis = GraphAPI::GGetNearestDisOfPtToLine3D(arrPts[i].x,arrPts[i].y,arrPts[i].z,
			arrPts[i+1].x,arrPts[i+1].y,arrPts[i+1].z,pt2.x,pt2.y,pt2.z,&x,&y,&z,false);

		if( min2<0 || dis<min2 )
		{ 
			min2 = dis; rpt2.x=x; rpt2.y=y; rpt2.z=z; 
			if( _FABS(x-arrPts[i+1].x)+_FABS(y-arrPts[i+1].y)>1e-10 )
				i2=i;
			//就是第二个点时，序号增加1
			else i2=i+1;
		}
	}
	
	if( min1<0 || min2<0 )
	{	
		return;
	}

	BOOL bSamePoint = FALSE;

	{	

		//调整 i1 和 i2，确保 i1 在前，i2 在后
		if( i1>i2 )
		{
			int nt; nt = i1; i1 = i2; i2 = nt;
			PT_3DEX tpt = rpt1; rpt1 = rpt2; rpt2 = tpt; 
		}

		//(虚拟)插入两个最近的图形点 rpt1 和 rpt2; 
		double d1 = _FABS(arrPts[i1].x-rpt1.x)+_FABS(arrPts[i1].y-rpt1.y)+_FABS(arrPts[i1].z-rpt1.z);
		double d2 = _FABS(arrPts[i2].x-rpt2.x)+_FABS(arrPts[i2].y-rpt2.y)+_FABS(arrPts[i2].z-rpt2.z);

		//调整 rpt1 和 rpt2，确保 rpt1 在前，rpt2 在后
		if( i1==i2 )
		{
			if( fabs(d1-d2)<1e-10 )bSamePoint = TRUE;
			if( d1>d2 ){ PT_3DEX tpt = rpt1; rpt1 = rpt2; rpt2 = tpt; }
		}

		if( d1>1e-10 )
		{
			arrPts.InsertAt(i1+1,rpt1);
			for( i=0; i<size; i++)if( arrKeyPos[i]>i1 )arrKeyPos[i] = arrKeyPos[i]+1;
			if( i1<=i2 )i2++;
			i1++; 
		}
		if( d2>1e-10 )
		{
			arrPts.InsertAt(i2+1,rpt2);
			for( i=0; i<size; i++)if( arrKeyPos[i]>i2 )arrKeyPos[i] = arrKeyPos[i]+1;
			if( i2<i1 )i1++;
			i2++; 
		}
 	}

	//imin到imax一段的点是需要被删除的??
	if( i1<=i2 ){ imin = i1; imax = i2;	}
	else { imin = i2; imax = i1;  PT_3DEX tpt = rpt1; rpt1 = rpt2; rpt2 = tpt; }

	pObj1 = NULL;  pObj2 = NULL;  pObj3 = NULL;
	
	//生成第一段线对象
	if( arrKeyPos[0]<imin )
	{
		CGeoCurveBase *pObj = (CGeoCurveBase*)Clone();
		
		if( pObj )
		{		
			for( i=0; i<size; i++)
			{
				if( arrKeyPos[i]<imin )
				{
					pts.Add(pts1[i]);
				}
				else break;
			}

			i--;

			if( i>=0 )
			{
				//补充前部的线型碎片（圆弧、样条）
				if( imin-arrKeyPos[i]>=2 )
				{
					PT_3DEX pt;
					//求出有效长度
					int actlen = 0;
					for( int j=arrKeyPos[i]; j<imin; j++)
					{
						if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10 )
						{
							actlen++;
						}
					}

					//加入有效半长的那个点
					if( actlen>=2 )
					{
						int tlen = 0;
						for( j=arrKeyPos[i]; j<imin; j++)
						{
							if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10)
							{
								tlen++;
							}
							if( tlen>=actlen/2 )
							{
								COPY_3DPT(pt,(arrPts[j+1]));
								pt.pencode = pts1[i].pencode;
								pts.Add(pt);
								break;
							}
						}
					}
				}
			
				rpt1.pencode = pts1[i].pencode;
				pts.Add(rpt1);
			}
			if(!pObj->CreateShape(pts.GetData(),pts.GetSize())) return;
			if( pObj->GetDataPointSum()>1 )
			{
				pObj1 = pObj;
			}
			else
			{
				delete pObj;
			}

			//首点有可能删除不掉
			if( pObj1 && pObj1->GetDataPointSum()==2 )
			{
				PT_3DEX expt1, expt2, expt3;
				expt1 = pObj1->GetDataPoint(0);
				expt2 = pObj1->GetDataPoint(1);

				if( fabs(expt1.x-expt2.x)<1e-6 && fabs(expt1.y-expt2.y)<1e-6 )
				{
					delete pObj1;
					pObj1 = NULL;
				}
			}
		}
	}
	pts.RemoveAll();
	//生成第二段线对象
	if( !bSamePoint )
	{
		CGeoCurveBase *pObj = (CGeoCurveBase*)Clone();
		if( pObj )
		{			
			int start = -1, end = -1;
			for( i=0; i<size; i++)
			{
				if( arrKeyPos[i]>=imin && arrKeyPos[i]<=imax )
				{
					if( start<0 )start = i;
					pts.Add(pts1[i]/*arrPts[i]*/);
				}
				
				if( arrKeyPos[i]>imax && end<0 )
				{
					end = i-1;
				}
			}

			if( start>=0 )
			{
				//补充前部的线型碎片（圆弧、样条）
				if( arrKeyPos[start]-imin>=2 )
				{
					PT_3DEX pt;
					//求出有效长度
					int actlen = 0;
					for( int j=imin; j<arrKeyPos[start]; j++)
					{
						if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10 )
						{
							actlen++;
						}
					}
					
					//插入有效半长的那个点
					if( actlen>=2 )
					{
						int tlen = 0;
						for( j=imin; j<arrKeyPos[start]; j++)
						{
							if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10 )
							{
								tlen++;
							}
							if( tlen>=actlen/2 )
							{
								COPY_3DPT(pt,(arrPts[j+1]));
								pt.pencode = pts1[start].pencode;
								pts.InsertAt(0,pt);
								break;
							}
						}
					}
				}

				if( arrKeyPos[start]-imin>0 )
				{
					rpt1.pencode = pts1[start].pencode;
					pts.InsertAt(0,rpt1);
				}
			}

			if( end>=0 )
			{
				int startPos = arrKeyPos[end];
				if( start<0 )
				{
					rpt1.pencode = pts1[end].pencode;
					pts.Add(rpt1);
					startPos = imin;
				}

				//补充后部的线型碎片（圆弧、样条）
				if( imax-startPos>=2 )
				{
					PT_3DEX pt;
					//求出有效长度
					int actlen = 0;
					for( int j=startPos; j<imax; j++)
					{
						if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10 )
						{
							actlen++;
						}
					}
					
					//加入有效半长的那个点
					if( actlen>=2 )
					{
						int tlen = 0;
						for( j=startPos; j<imax; j++)
						{
							if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10)
							{
								tlen++;
							}
							if( tlen>=actlen/2 )
							{
								COPY_3DPT(pt,(arrPts[j+1]));
								pt.pencode = pts1[end].pencode;
								pts.Add(pt);
								break;
							}
						}
					}
				}
				
				if( imax-startPos>=0 )
				{
					rpt2.pencode = pts1[end].pencode;
					pts.Add(rpt2);
				}
			}
			
			if(!pObj->CreateShape(pts.GetData(),pts.GetSize())) return;
			if( pObj->GetDataPointSum()>1 )
			{
				pObj2 = pObj;
			}
			else
			{
				delete pObj;
			}
		}
	}
	pts.RemoveAll();
	//生成第三段线对象
	if( arrKeyPos[size-1]>imax )
	{
		CGeoCurveBase *pObj = (CGeoCurveBase*)Clone();
		if( pObj )
		{		
			int start = -1;
			for( i=0; i<size; i++)
			{
				if( arrKeyPos[i]>imax )
				{
					pts.Add(pts1[i]);
					if( start<0 )start = i;
				}
			}

			if( start>=0 )
			{
				//补充后部的线型碎片（圆弧、样条）
				if( arrKeyPos[start]-imax>=2 )
				{
					PT_3DEX pt;
					//求出有效长度
					int actlen = 0;
					for( int j=imax; j<arrKeyPos[start]; j++)
					{
						if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10 )
						{
							actlen++;
						}
					}
					
					//插入有效半长的那个点
					if( actlen>=2 )
					{
						int tlen = 0;
						for( j=imax; j<arrKeyPos[start]; j++)
						{
							if(	_FABS(arrPts[j].x-arrPts[j+1].x)+_FABS(arrPts[j].y-arrPts[j+1].y)+_FABS(arrPts[j].z-arrPts[j+1].z)>1e-10 )
							{
								tlen++;
							}
							if( tlen>=actlen/2 )
							{
								COPY_3DPT(pt,(arrPts[j+1]));
								pt.pencode = pts1[start].pencode;
								pts.InsertAt(0,pt);
								break;
							}
						}
					}
				}

				rpt2.pencode = pts1[start].pencode;
				pts.InsertAt(0,rpt2);
			}

			if(!pObj->CreateShape(pts.GetData(),pts.GetSize())) return;
			if( pObj->GetDataPointSum()>1 )
			{
				pObj3 = pObj;
			}
			else
			{
				delete pObj;
			}

			//首点有可能删除不掉
			if( pObj3 && pObj3->GetDataPointSum()==2 )
			{
				PT_3DEX expt1, expt2, expt3;
				expt1 = pObj3->GetDataPoint(0);
				expt2 = pObj3->GetDataPoint(1);
				
				if( fabs(expt1.x-expt2.x)<1e-6 && fabs(expt1.y-expt2.y)<1e-6 )
				{
					delete pObj3;
					pObj3 = NULL;
				}
			}
		}
	}

	// 闭合则将两个地物合为一个
	if (!bSamePoint && bClose && IsClosed() && pObj1 && pObj3)
	{
		CArray<PT_3DEX,PT_3DEX> pts1, pts3;
		pObj1->GetShape(pts1);
		pts1.ElementAt(0).pencode = penLine;
		pObj3->GetShape(pts3);

		//去除首尾重复点
		pts3.RemoveAt(pts3.GetSize()-1);

		pts3.Append(pts1);

		delete pObj3;
		pObj3 = NULL;

		if (!pObj1->CreateShape(pts3.GetData(),pts3.GetSize()))
		{
			delete pObj1;
			pObj1 = NULL;
		}
	}
	return;
}


CGeometry* CGeoCurveBase::Clone()const
{
	CGeoCurveBase *pNew = new CGeoCurveBase();
	if(pNew)
	   pNew->CopyFrom(this);
	return pNew;
}


BOOL CGeoCurveBase::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		CGeoCurveBase *pCurve = (CGeoCurveBase*)pObj;
		const CShapeLine *pShape = pCurve->GetShape();
		m_shape.CopyFrom(pShape);
		
		m_fLineWidth = pCurve->m_fLineWidth;
		m_evlp = pCurve->m_evlp;
	}
	
	return CGeometry::CopyFrom(pObj);
}

BOOL CGeoCurveBase::WriteTo(CValueTable& tab)const
{
	CGeometry::WriteTo(tab);

	CVariantEx var;	
	var = (_variant_t)m_fLineWidth;
	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTH,&var,IDS_FIELDNAME_GEOWIDTH);
	
	return TRUE;
}

BOOL CGeoCurveBase::ReadFrom(CValueTable& tab,int idx)
{
	CGeometry::ReadFrom(tab,idx);

	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOCURVE_LINEWIDTH,var) )
	{
		m_fLineWidth = (_variant_t)*var;
	}
	
	return TRUE;
}

void CGeoCurveBase::Draw(GrBuffer *pBuf, float fDrawScale)
{	
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);

	if( IsFillColor() )
	{
		pBuf->BeginPolygon(GetFillColor());
		m_shape.ToGrBuffer(pBuf);	
		pBuf->End();		
	}
	else
	{		
		pBuf->BeginLineString(m_nColor,0,TRUE);
		m_shape.ToGrBuffer(pBuf);	
		pBuf->End();
	}
}

CGeometry *CGeoCurveBase::Linearize()
{
	CGeoCurveBase *pCurve = (CGeoCurveBase*)Clone();
	if(!pCurve)
		return NULL;
	
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetPts(pts);
	for (int i=0;i<pts.GetSize();i++)
	{
		if( pts[i].pencode!=penMove )
			pts[i].pencode = penLine;
	}
	int num = GraphAPI::GKickoffSamePoints(pts.GetData(),pts.GetSize());

	pCurve->CreateShape(pts.GetData(),num);

	return pCurve;

}

Envelope CGeoCurveBase::GetEnvelope()
{
	return m_evlp;
}

const CShapeLine *CGeoCurveBase::GetShape()
{
	return &m_shape;
}

BOOL CGeoCurveBase::IsClosed()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int num = pts.GetSize();
	if (num>=3)
	{
		PT_3DEX pt0,pt1;
		
		pt0 = pts[0]; pt1 = pts[num-1];
		if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 /*&& fabs(pt0.z-pt1.z)<1e-4*/ )
			return TRUE;
	}
	return FALSE;
}

// search a line segment in an object
BOOL CGeoCurveBase::FindNearestBaseLine(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *ret1, PT_3D *ret2, double *mindis)
{
	CArray<PT_3DEX,PT_3DEX>  pts;
	m_shape.GetPts(pts);

	PT_3DEX kpt0,kpt1,kpt, ret;
	int num = pts.GetSize(), k=-1;
	if( num<=1 )return -1;
	
	kpt = pts.GetAt(0);
	pCS->GroundToClient(&kpt,&kpt1);

	Envelope e;
	
	double m=-1, t;
	for( int i=1; i<num; i++)
	{
		kpt0 = kpt1;
		kpt = pts[i];

		if(kpt.pencode==penMove)
			continue;

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
BOOL CGeoCurveBase::FindNearestBasePt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis)
{
	CArray<PT_3DEX,PT_3DEX>  pts;
	m_shape.GetPts(pts);

	PT_3DEX kpt0,kpt1,kpt, ret, mret;
	int num = pts.GetSize(), k=-1;
	if( num<1 )return FALSE;
	if (num==1)
	{
		kpt = pts.GetAt(0);
		pCS->GroundToClient(&kpt,&kpt1);
		if( !sch_evlp.bPtIn(&kpt1) )
			return FALSE;
		if( pRet )*pRet = kpt;		
		if( mindis )*mindis = sqrt((kpt.x-sch_pt.x)*(kpt.x-sch_pt.x)+(kpt.y-sch_pt.y)*(kpt.y-sch_pt.y));
		return TRUE;
	}
	
	kpt = pts.GetAt(0);
	pCS->GroundToClient(&kpt,&kpt1);

	double m=-1, t, lfk;
	for( int i=1; i<num; i++)
	{
		kpt0 = kpt1;
		kpt = pts[i];
		pCS->GroundToClient(&kpt,&kpt1);
		if(kpt.pencode==penMove)
			continue;

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

BOOL CGeoCurveBase::EnableClose(BOOL bClose)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	if (pts.GetSize()<3)
	{
		return FALSE;
	}

	if( IsClosed() )
	{
		if( !bClose )
			pts.RemoveAt(pts.GetUpperBound());
	}
	else
	{
		if( bClose )
			pts.Add(pts[0]);
	}

	CreateShape(pts.GetData(),pts.GetSize());

	return TRUE;
}

BOOL CGeoCurveBase::GetTangency(PT_3D *ipt0, PT_3D *ipt1, PT_3D* opt)
{
	CArray<PT_3DEX,PT_3DEX> arrpts;
	if(!m_shape.GetKeyPts(arrpts)) 
		return FALSE;
	int keynum =arrpts.GetSize();

	if( keynum<=2 )return FALSE;

	for( int i=0; i<keynum; i++ )
	{
		if( arrpts[i].pencode==penArc )
			break;
	}
	if( i>=keynum )return FALSE;
	
	//得到节点在图形点中的对应序号
	CArray<int,int> KeyPos;
	m_shape.GetKeyPosOfBaseLines(KeyPos);


	CArray<PT_3DEX,PT_3DEX> arrpts0;
	if(!m_shape.GetPts(arrpts0)) return FALSE;
	int num = arrpts0.GetSize();	
	if( num<=1 )
	{
		return FALSE;
	}
// 	
// 	GrNode *pts = new GrNode[num+2];
// 	if( !pts )
// 	{
// 		return FALSE;
// 	}
// 	
// 	pBuf->GetPts(pts);
	
	//找最近的图形点的相邻关键点
	double x,y,z;
	double dis,min=-1;
	int idx=-1;
	
	for( i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(arrpts0[i].x,arrpts0[i].y,arrpts0[i].z,
			arrpts0[i+1].x,arrpts0[i+1].y,arrpts0[i+1].z,ipt1->x,ipt1->y,ipt1->z,&x,&y,&z,false);
		
		if( min<0 || dis<min )
		{ 
			min = dis; 			
			if( _FABS(x-arrpts0[i+1].x)+_FABS(y-arrpts0[i+1].y)>1e-10 )
				idx=i;
			//就是第二个点时，序号增加1
			else idx=i+1;
		}
	}
	
	for( i=0; i<keynum; i++)
	{
		if( KeyPos[i]>idx )
		{
			idx = i-1;
			break;
		}
	}
	
	if( min<0 )return FALSE;
	if( idx>=keynum-1 )idx = keynum-2;
	
	PT_3D retPts[2];
	int retNum = CLinearizer::GetTangency(ipt0,arrpts.GetData(),keynum,FALSE,idx,retPts);
	if( retNum<=0 )return FALSE;
	
	if( retNum==1 )
	{
		COPY_3DPT((*opt),retPts[0]);
	}
	else
	{
		double d0 = (ipt1->x-retPts[0].x)*(ipt1->x-retPts[0].x) +
			(ipt1->y-retPts[0].y)*(ipt1->y-retPts[0].y);
		double d1 = (ipt1->x-retPts[1].x)*(ipt1->x-retPts[1].x) +
			(ipt1->y-retPts[1].y)*(ipt1->y-retPts[1].y);
		
		if( d0<=d1 )COPY_3DPT((*opt),retPts[0]);
			else COPY_3DPT((*opt),retPts[1]);
			
	}
	
	return TRUE;
}

BOOL CGeoCurveBase::GetCenter(PT_3D *ipt, PT_3D *opt)
{
	CArray<PT_3DEX,PT_3DEX> arrpts;
	if(!m_shape.GetKeyPts(arrpts)) 
		return FALSE;
	int keynum =arrpts.GetSize();
	if( keynum<=2 )return CGeometry::GetCenter(ipt,opt);
	for( int i=0; i<keynum; i++ )
	{
		if( arrpts[i].pencode==penArc )
			break;
	}
	if( i>=keynum )return CGeometry::GetCenter(ipt,opt);
	
	//得到节点在图形点中的对应序号
	CArray<PT_3DEX,PT_3DEX> arrpts0;
	if(!m_shape.GetPts(arrpts0)) return FALSE;
	int num = arrpts0.GetSize();
	if( num<=1 || ipt == NULL)
	{
		PT_3DEX retPt;
		BOOL ret = GraphAPI::GGetCenter(arrpts0.GetData(),arrpts0.GetSize(),&retPt);
		COPY_3DPT((*opt),retPt);
		return ret;
		//return CGeometry::GetCenter(ipt,opt);
	}
	
	//找最近的图形点的相邻关键点
	double x,y,z;
	double dis,min=-1;
	int idx=-1;
	
	for( i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(arrpts0[i].x,arrpts0[i].y,arrpts0[i].z,
			arrpts0[i+1].x,arrpts0[i+1].y,arrpts0[i+1].z,ipt->x,ipt->y,ipt->z,&x,&y,&z,false);
		
		if( min<0 || dis<min )
		{ 
			min = dis; 			
			if( _FABS(x-arrpts0[i+1].x)+_FABS(y-arrpts0[i+1].y)>1e-10 )
				idx=i;
			//就是第二个点时，序号增加1
			else idx=i+1;
		}
	}

	while (1)
	{
		if (arrpts0[idx].pencode==penNone)
		{
			idx--;
		}
		else
			break;
	}
	
	if( min<0 )return CGeometry::GetCenter(ipt,opt);
	if( idx>=keynum-1 )idx = keynum-2;
	
	PT_3D retPts;
	if( CLinearizer::GetCenter(arrpts.GetData(),keynum,FALSE,idx,&retPts) )
	{
		COPY_3DPT((*opt),retPts);
	        return TRUE;
        }

	return CGeometry::GetCenter(ipt,opt);

}

int CGeoCurveBase::bPtIn(const PT_3D *pt)
{
	if(IsClosed())
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		m_shape.GetKeyPts(pts);
		PT_3DEX ptex(*pt, penNone);
		int ret = GraphAPI::GIsPtInRegion(ptex, pts.GetData(), pts.GetSize());
		if( ret==2 )
		{
			return 2;
		}
		else if(ret==1 || ret==0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}


IMPLEMENT_DYNCREATE(CGeoCurve, CGeoCurveBase)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoCurve::CGeoCurve()
{
	m_fLinetypeScale = m_fLinewidthScale = 1;
	m_fLineWidth = -1;
	m_fLinetypeXoff = 0;
}

CGeoCurve::~CGeoCurve()
{

}

int  CGeoCurve::GetClassType()const
{
	return CLS_GEOCURVE;
}


CGeometry* CGeoCurve::Clone()const
{
	CGeoCurve *pNew = new CGeoCurve();
	if(pNew)
	   pNew->CopyFrom(this);
	return pNew;
}


BOOL CGeoCurve::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pObj;
		m_fLinetypeScale = pCurve->m_fLinetypeScale;
		m_fLinewidthScale = pCurve->m_fLinewidthScale;
		m_fLinetypeXoff = pCurve->m_fLinetypeXoff;
	}
	
	return CGeoCurveBase::CopyFrom(pObj);
}

BOOL CGeoCurve::WriteTo(CValueTable& tab)const
{
	CGeoCurveBase::WriteTo(tab);

	CVariantEx var;
	var = (_variant_t)m_fLinetypeScale;
	tab.AddValue(FIELDNAME_GEOCURVE_LINETYPESCALE,&var,IDS_FIELDNAME_GEOLINETYPESCALE);
	
	var = (_variant_t)m_fLinewidthScale;
	tab.AddValue(FIELDNAME_GEOCURVE_LINEWIDTHSCALE,&var,IDS_FIELDNAME_GEOLINEWIDTHSCALE);

	var = (_variant_t)m_fLinetypeXoff;
	tab.AddValue(FIELDNAME_GEOCURVE_LINETYPEXOFF,&var,IDS_FIELDNAME_LINETYPEXOFF);
	
	return TRUE;
}

BOOL CGeoCurve::ReadFrom(CValueTable& tab,int idx)
{
	CGeoCurveBase::ReadFrom(tab,idx);

	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOCURVE_LINETYPESCALE,var) )
	{
		m_fLinetypeScale = (_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOCURVE_LINEWIDTHSCALE,var) )
	{
		m_fLinewidthScale = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOCURVE_LINETYPEXOFF,var) )
	{
		m_fLinetypeXoff = (_variant_t)*var;
	}
	
	return TRUE;
}

void CGeoCurve::Draw(GrBuffer *pBuf, float fDrawScale)
{	
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);

	if(IsFillColor() && IsClosed() )
	{
		pBuf->BeginPolygon(GetFillColor());
		m_shape.ToGrBuffer(pBuf);	
		pBuf->End();		
	}
	else
	{		
		BOOL bFillWid = FillLineWidth(pts.GetData(),pts.GetSize(),m_fLineWidth*fDrawScale,m_fLinewidthScale,pBuf,m_nColor,fDrawScale);
		
		if (!bFillWid)
		{
			BOOL bGrdSize = (m_fLineWidth>0);
			pBuf->BeginLineString(m_nColor,bGrdSize?m_fLinewidthScale*m_fLineWidth*fDrawScale:m_fLinewidthScale,bGrdSize);
			m_shape.ToGrBuffer(pBuf);	
			pBuf->End();
		}	
	}
}

IMPLEMENT_DYNCREATE(CGeoDCurve, CGeoCurve)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoDCurve::CGeoDCurve()
{
	
}

CGeoDCurve::~CGeoDCurve()
{
	
}

int  CGeoDCurve::GetClassType()const
{
	return CLS_GEODCURVE;
}

double CGeoDCurve::GetWidth()
{
	const CShapeLine *pSL = GetShape();
	if( !pSL )return 0;
	
	CArray<PT_3DEX,PT_3DEX> arr, arr1;
	GetBaseShape(arr);
	GetAssistShape(arr1);
	
	int sum = arr.GetSize();
	
	sum = GraphAPI::GKickoffSamePoints(arr.GetData(),sum);
	
	if( sum<=1 || arr1.GetSize() <= 1) return 0;

	PT_3DEX pt;
	GraphAPI::GGetMiddlePt(arr1.GetData(),arr1.GetSize(),&pt);
	
	PT_3DEX ret;
	int index = -1;
	double wid = GraphAPI::GGetNearstDisPToPointSet2D(arr.GetData(),arr.GetSize(),pt,ret,&index);
	
	PT_3DEX pts[3];
	if (index == -1)
	{
		pts[0] = arr[0];
		pts[1] = arr[sum-1];
	}
	else if (index == arr.GetSize()-1)
	{
		pts[0] = arr[index-1];
		pts[1] = arr[index];
	}
	else
	{
		pts[0] = arr[index];
		pts[1] = arr[index+1];
	}
	
	pts[2] = pt;

	// 双线的方向
// 	CArray<PT_3DEX,PT_3DEX> arrPts;
// 	GetOrderShape(arrPts);	
	bool bClockwise = (GraphAPI::GIsClockwise(pts,3)==1);
	if( bClockwise )
		wid =-wid;
	
	return wid;
}

BOOL CGeoDCurve::GetOrderShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	pts.RemoveAll();
	
	CArray<PT_3DEX,PT_3DEX>  arr, arrAssist;
	m_shape.GetPts(arr);
	
	int num = arr.GetSize();
	if (num < 2) return FALSE;

	for (int i=0; i<num; i++)
	{
		if (i !=0 && arr[i].pencode == penMove)
		{
			break;
		}
		
		pts.Add(arr[i]);
	}

	for (int j=i; j<num; j++)
	{
		arrAssist.Add(arr[j]);
	}

	if (arrAssist.GetSize() <= 0)
	{
		return FALSE;
	}

	PT_3DEX pt = pts[0], pt1 = arrAssist[0], pt2 = arrAssist[arrAssist.GetSize()-1];
	if (GraphAPI::GGet3DDisOf2P(pt,pt1) > GraphAPI::GGet3DDisOf2P(pt,pt2))
	{
		pts.Append(arrAssist);
	}
	else
	{
		for (i=arrAssist.GetSize()-1; i>=0; i--)
		{
			pts.Add(arrAssist[i]);
		}

	}

	return TRUE;

}

extern double GetTriangleArea(PT_3D pts[3]);

BOOL CGeoDCurve::GetCenter(PT_3D *ipt, PT_3D *opt)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	GetOrderShape(pts);
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

void CGeoDCurve::GetBaseShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	pts.RemoveAll();
	
	CArray<PT_3DEX,PT_3DEX>  arr;
	m_shape.GetPts(arr);
	
	int num = arr.GetSize();
	if (num < 2) return;
	
	for (int i=0; i<num; i++)
	{
		if (i !=0 && arr[i].pencode == penMove)
		{
			break;
		}

		pts.Add(arr[i]);
	}
}

void CGeoDCurve::GetAssistShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	pts.RemoveAll();

	CArray<PT_3DEX,PT_3DEX>  arr;
	m_shape.GetPts(arr);

	int num = arr.GetSize();
	if (num < 2) return;

	for (int i=1; i<num; i++)
	{
		if (arr[i].pencode == penMove)
		{
			for (int j=i; j<num; j++)
			{
				pts.Add(arr[j]);
			}
			break;
		}
	}
}

CGeometry* CGeoDCurve::Clone()const
{
	CGeoDCurve *pNew = new CGeoDCurve;
	if(pNew)
		pNew->CopyFrom(this);
	return pNew;
}

BOOL CGeoDCurve::Separate(CGeometry *&pGeo1, CGeometry *&pGeo2)
{
	pGeo1 = pGeo2 = NULL;	

	CArray<PT_3DEX,PT_3DEX> arr1, arr2;
	GetBaseShape(arr1);
	GetAssistShape(arr2);

	if (arr1.GetSize() < 2 )
	{
		return FALSE;
	}

	pGeo1 = new CGeoCurve;
	if(!pGeo1) return FALSE;
	CValueTable tab;
	tab.BeginAddValueItem();
	WriteTo(tab);
	tab.EndAddValueItem();
	pGeo1->ReadFrom(tab);
	
	if( pGeo1 )
	{
		pGeo1->CreateShape(arr1.GetData(),arr1.GetSize());				
	}	

	if( arr2.GetSize()>=2 )
	{
		pGeo2 = pGeo1->Clone();	
		
		if( pGeo2 )
		{
			arr2[0].pencode = arr1[0].pencode;		
			pGeo2->CreateShape(arr2.GetData(),arr2.GetSize());
		}
	}
	
	return TRUE;
}

BOOL CGeoDCurve::CopyFrom(const CGeometry *pObj)
{
	return CGeoCurve::CopyFrom(pObj);
}

BOOL CGeoDCurve::CreateShape(const PT_3DEX *pts, int npt)
{
	m_shape.Clear();

	if (npt<2)
	{
		return FALSE;
	}
	m_shape.Clear();
	CLinearizer line;
	CShapeLine shape;	
	int start = 0;
	BOOL ret = FALSE;
	for (int i=0;i<npt;i++)
	{		
		if(pts[i].pencode==penMove)
		{
			if(i==0)continue;
			if(i-start<2)
			{
				start = i;
				continue;
			}
			else
			{	
				shape.Clear();
				line.SetShapeBuf(&shape);
				line.Linearize(pts+start,i-start,FALSE);
				shape.LineEnd();
				m_shape.AddShapeLine(&shape);
				start = i;
				ret = TRUE;
			}
		}
		if (i==npt-1)
		{
			if(npt-start<2)
				continue;
			else
			{
				shape.Clear();
				line.SetShapeBuf(&shape);
				line.Linearize(pts+start,npt-start,FALSE);
				shape.LineEnd();
				m_shape.AddShapeLine(&shape);				
				ret = TRUE;
			}
		}
	}

	m_evlp = m_shape.GetEnvelope();

	return ret;
}

BOOL CGeoDCurve::EnableClose(BOOL bClose)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);

	int num = pts.GetSize();
	if (num < 4) return FALSE;
	
	int index = -1;
	for (int i=0; i<num; i++)
	{
		if (i !=0 && pts[i].pencode == penMove)
		{
			index = i;
			break;
		}		
	}

	if( index<1 )
		return FALSE;

	BOOL bClosed1 = TRUE, bClosed2 = TRUE;
	PT_3DEX pt0,pt1;				
	pt0 = pts[0]; pt1 = pts[index-1];
	if( fabs(pt0.x-pt1.x)>1e-4 || fabs(pt0.y-pt1.y)>1e-4 || fabs(pt0.z-pt1.z)>1e-4 )
		bClosed1 = FALSE;
	
	pt0 = pts[index]; pt1 = pts[num-1];
	if( fabs(pt0.x-pt1.x)>1e-4 || fabs(pt0.y-pt1.y)>1e-4 || fabs(pt0.z-pt1.z)>1e-4 )
		bClosed2 = FALSE;

	if (bClose)
	{
		if (!bClosed2)
		{
			PT_3DEX pt = pts[index];
			pt.pencode = pts[num-1].pencode;
			pts.Add(pt);
		}

		if (!bClosed1)
		{
			PT_3DEX pt = pts[0];
			pt.pencode = pts[index-1].pencode;
			pts.InsertAt(index,pt);
		}


	}
	else
	{
		if (bClosed2)
		{
			pts.RemoveAt(num-1);
		}
		
		if (bClosed1)
		{
			pts.RemoveAt(index-1);
		}
	}

	CreateShape(pts.GetData(),pts.GetSize());
	
	return TRUE;
}

BOOL CGeoDCurve::IsClosed()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);

	int num = pts.GetSize();
	if (num <= 2) return FALSE;

	int index = -1;
	for (int i=0; i<num; i++)
	{
		if (i !=0 && pts[i].pencode == penMove)
		{
			index = i;
			break;
		}		
	}

	if( index<1 )
		return FALSE;

	PT_3DEX pt0,pt1;				
	pt0 = pts[0]; pt1 = pts[index-1];
	if( fabs(pt0.x-pt1.x)>1e-4 || fabs(pt0.y-pt1.y)>1e-4 || fabs(pt0.z-pt1.z)>1e-4 )
		return FALSE;

	pt0 = pts[index]; pt1 = pts[num-1];
	if( fabs(pt0.x-pt1.x)>1e-4 || fabs(pt0.y-pt1.y)>1e-4 || fabs(pt0.z-pt1.z)>1e-4 )
		return FALSE;

	return TRUE;
}

//长度
double CGeoDCurve::GetLength()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	GetBaseShape(pts);
	
	return GraphAPI::GGetAllLen2D(pts.GetData(),pts.GetSize());
}


//面积
double CGeoDCurve::GetArea()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	
	return GraphAPI::GGetPolygonArea(pts.GetData(),pts.GetSize());
}


//////////////////////////////////////////////////////////////////////
// CGeoMultiPoint Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGeoMultiPoint, CGeometry)

CGeoMultiPoint::CGeoMultiPoint()
{
//	m_nClassID = CLS_GEOMULTIPOINT;	
	m_nColor = RGB(0, 255, 0);
	CWinApp *pApp = AfxGetApp();
	if (pApp)
	{
		m_nColor = pApp->GetProfileInt(REGPATH_SYMBOL, _T("MultipointColor"), m_nColor);
	}
}

CGeoMultiPoint::~CGeoMultiPoint()
{
}

// void CGeoMultiPoint::DoPermanentDataExchange(CPermanentDataExchange *pPDX)
// {
// 	CGeometry::DoPermanentDataExchange(pPDX);
// 	//{{AFX_PRESERVABLE_DATA_MAP(CGeoMultiPoint)
// 	pPDX->Group(StrFromResID(IDS_GROUP_LINE));
// 	DPDX_SHAPE(pPDX, FALSE, &m_PointList,GEO_Line);
// 	DPDX_VALUE(pPDX, "LNEXCODE",StrFromResID(IDS_LINEEXCODE),FALSE,&m_nLnExCode,DP_CFT_INTEGER);
// 	//}}AFX_PRESERVABLE_DATA_MAP
// }

//DEL void CGeoMultiPoint::AddPt(PT_3DEX pt)
//DEL {
//DEL 	m_PointList.Add(pt);
//DEL }

BOOL CGeoMultiPoint::SetDataPoint(int i, PT_3DEX  pt)
{
	if( i<0 || i>m_PointList.GetSize() )return FALSE;
	m_PointList.SetAt(i,pt);
	return TRUE;
}

//DEL BOOL CGeoMultiPoint::DeletePt(int i)
//DEL {
//DEL 	if( i>=0 && i<m_PointList.GetSize() )
//DEL 	{
//DEL 		m_PointList.RemoveAt(i);
//DEL 		return TRUE;
//DEL 	}
//DEL 	return FALSE;
//DEL }


PT_3DEX CGeoMultiPoint::GetDataPoint(int i)const
{
	if( i>=0 && i<m_PointList.GetSize() )
	{
		return m_PointList[i];
	}
	return PT_3DEX();
}

int CGeoMultiPoint::GetDataPointSum()const
{
	return m_PointList.GetSize();
}

int CGeoMultiPoint::GetClassType()const
{
	return CLS_GEOMULTIPOINT;
}

/*
void CGeoCurve::Draw(GrBuffer *pBuf)
{
	pBuf->BeginLineString(m_nColor,0,0);
	m_shape.ToGrBuffer(pBuf);
	if( m_bClosed && m_shape.GetPtsCount()>=3 )
	{
		PT_3DEX pt3dex = m_shape.GetPt(0);
		pBuf->LineTo(&pt3dex);
	}
	pBuf->End();
}
*/

Envelope CGeoMultiPoint::GetEnvelope()
{
	return m_evlp;
}

void CGeoMultiPoint::Draw(GrBuffer *pBuf, float fDrawScale)
{
	if (!pBuf) return;
	int num = m_PointList.GetSize();
	if( num<=0 )return;

	m_nColor = RGB(0, 255, 0);
	CWinApp *pApp = AfxGetApp();
	if (pApp)
	{
		m_nColor = pApp->GetProfileInt(REGPATH_SYMBOL, _T("MultipointColor"), RGB(0, 255, 0));
	}

	double ptlen = GetProfileDouble(REGPATH_SYMBOL,_T("MultipointSize"),1.0);
	float fSymLen = ptlen * GetSymbolDrawScale();
	
	pBuf->BeginLineString(m_nColor,m_nColor,fSymLen,fSymLen,TRUE);
	
	PT_3D pt0;
	PT_3DEX *data = m_PointList.GetData();
	
	for( int i=0; i<num; i++)
	{
		COPY_3DPT(pt0,data[i]);
		pBuf->PointString(&pt0,0);
	}

	pBuf->End();

	pBuf->RefreshEnvelope();
}

//DEL const GrBuffer* CGeoMultiPoint::GetBaseLines()
//DEL {
//DEL /*	return &m_grBuffer;
//DEL 
//DEL 	if( m_bNeedDrawBase )
//DEL 	{
//DEL 		m_grBase.DeleteAll();
//DEL 		
//DEL 		CLineBase drawer;
//DEL 		drawer.SetGrBuffer(&m_grBase);
//DEL 		drawer.Linearize(m_PointList.GetData(),m_PointList.GetSize());
//DEL 		
//DEL 		m_grBase.KickoffSamePts();
//DEL 		m_grBase.CutRedundantMem();
//DEL 		m_grBase.RefreshEnvelope();
//DEL 	}
//DEL 	
//DEL 	m_bNeedDrawBase = FALSE;
//DEL 	
//DEL 	return (&m_grBase);
//DEL */
//DEL 	return NULL;
//DEL }


//DEL BOOL CGeoMultiPoint::InsertPt(int num, PT_3DEX pt)
//DEL {
//DEL 	if( num>=0 && num<=m_PointList.GetSize() )
//DEL 	{
//DEL 		m_PointList.InsertAt(num,pt);
//DEL 		return TRUE;
//DEL 	}
//DEL 	return FALSE;
//DEL }



BOOL CGeoMultiPoint::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoMultiPoint)) )
	{
		CGeoMultiPoint *pCurve = (CGeoMultiPoint*)pObj;
		m_PointList.Copy(pCurve->m_PointList);
		m_evlp = pCurve->m_evlp;
	}

	return CGeometry::CopyFrom(pObj);
}


CGeometry* CGeoMultiPoint::Clone()const
{
	CGeometry* pNew = new CGeoMultiPoint;
	if( pNew )
	{
		pNew->CopyFrom(this);
	}
	return pNew;
}

BOOL CGeoMultiPoint::CreateShape(const PT_3DEX *pts, int npt)
{
	m_PointList.RemoveAll();

	if( npt<1 )
		return FALSE;	

	for (int i=0; i<npt; i++,pts++)
	{
		PT_3DEX pt = *pts;
		m_PointList.Add(pt);
	}

	m_evlp = CreateEnvelopeFromPts(m_PointList.GetData(),m_PointList.GetSize(),3);
	
	return TRUE;
}

void CGeoMultiPoint::GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const
{
	pts.RemoveAll();
	pts.Copy(m_PointList);
}

//DEL void CGeoMultiPoint::DeleteAll()
//DEL {
//DEL 	m_PointList.RemoveAll();
//DEL }


BOOL CGeoMultiPoint::IsInEnvelope(Envelope e)
{
	int num = m_PointList.GetSize();
	if( num<=0 )return TRUE;
		
	PT_3D pt0;
	PT_3DEX *data = m_PointList.GetData();
	
	for( int i=0; i<num; i++)
	{
		COPY_3DPT(pt0,data[i]);

		if( !e.bPtIn(&pt0) )
			return FALSE;
	}

	return TRUE;
}

const CShapeLine *CGeoMultiPoint::GetShape()
{
	if( m_PointList.GetSize()<=1 )
		return NULL;
	return NULL;
// 	CShapeLine 
// 	m_shape.Clear();
// 	
// 	CLinearizer line;
// 	line.SetShapeBuf(&m_shape);
// 	line.Linearize(pts,npt);
// 	
// 	m_shape.LineEnd();

}

int CGeoMultiPoint::DeletePart(Envelope e, BOOL bInside)
{
	int num = m_PointList.GetSize();
	if( num<=0 )return 0;
		
	PT_3D pt0;
	PT_3DEX *data = m_PointList.GetData();
	CArray<PT_3DEX, PT_3DEX> pts;
	
	for( int i=0; i<num; i++)
	{
		COPY_3DPT(pt0,data[i]);

		if( !bInside )
		{
			if( e.bPtIn(&pt0) )
			{
				pts.Add(data[i]);
			}
		}
		else
		{
			if( !e.bPtIn(&pt0) )
			{
				pts.Add(data[i]);
			}
		}
	}

	m_PointList.Copy(pts);

	m_evlp = CreateEnvelopeFromPts(m_PointList.GetData(),m_PointList.GetSize(),3);

	return (num-pts.GetSize());

}

SearchableMultiPoint::SearchableMultiPoint()
{
//	type = EBSU_OBJECT;
}


SearchableMultiPoint::~SearchableMultiPoint()
{
	
}

Envelope SearchableMultiPoint::GetEnvelope()const
{
	return m_pObj->GetEnvelope();
}

BOOL SearchableMultiPoint::FindObj(const SearchRange *sr, double *distance)const
{	
	CArray<PT_3DEX,PT_3DEX> pts;
	m_pObj->GetShape(pts);
	Envelope e;
	CCoordSys *p = NULL;
	PT_3D pt;
	if(sr->type==EBSR_RECT)
	{
		const SearchRect *srr = (const SearchRect*)sr;
		e = srr->evlp;
		p = srr->pCS;
		
		if (srr->bEntireInclude)
		{
			int i;
			for (i=0;i<pts.GetSize();i++)
			{
				if( p )p->GroundToClient(&pts[i],&pt);
				else pt = pts[i];
				if (!e.bPtIn(&pt))
				{
					break;
				}
			}
			if(i>=pts.GetSize())
				return TRUE;
		}
		else
		{
			for (int i=0;i<pts.GetSize();i++)
			{
				if( p )p->GroundToClient(&pts[i],&pt);
				else pt = pts[i];
				if (e.bPtIn(&pt))
				{
					return TRUE;
				}
			}		
			
		}
		
	}
	else if( sr->type==EBSR_RECT_SIMPLE )
	{
		const SearchRectSimple *srr = (const SearchRectSimple*)sr;
		e = srr->evlp;
		p = srr->pCS;
		
		if (srr->bEntireInclude)
		{
			int i;
			for (i=0;i<pts.GetSize();i++)
			{
				if( p )p->GroundToClient(&pts[i],&pt);
				else pt = pts[i];
				if (!e.bPtIn(&pt))
				{
					break;
				}
			}
			if(i>=pts.GetSize())
				return TRUE;
		}
		else
		{
			for (int i=0;i<pts.GetSize();i++)
			{
				if( p )p->GroundToClient(&pts[i],&pt);
				else pt = pts[i];
				if (e.bPtIn(&pt))
				{
					return TRUE;
				}
			}		
			
		}
	}
	else if (sr->type==EBSR_RADIUS)
	{
		const SearchNearest *srr = (const SearchNearest*)sr;
		e.CreateFromPtAndRadius(srr->pt,srr->r);
		p = srr->pCS;		
	
		{
			for (int i=0;i<pts.GetSize();i++)
			{
				if( p )p->GroundToClient(&pts[i],&pt);
				else pt = pts[i];

				if (e.bPtIn(&pt))
				{
					return TRUE;
				}
			}		
			
		}
	}
	return FALSE;
}

double SearchableMultiPoint::GetMinDistance(PT_3D sch_pt, CCoordSys* pCS, PT_3D *pret)const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_pObj->GetShape(pts);
	double minDis = DBL_MAX;
	double tem;
	int idx = -1;
	for (int i=0;i<pts.GetSize();i++)
	{		
		tem = DIST_2DPT(sch_pt,(pts[i]));
		if (minDis>tem)
		{
			minDis = tem;
			idx = i;
		}
	}
	if(idx!=-1)
	{
		COPY_3DPT((*pret),(pts[idx]));
	}
	return minDis;
}
//////////////////////////////////////////////////////////////////////
// CGeoDemPoint Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGeoDemPoint, CGeoMultiPoint)

CGeoDemPoint::CGeoDemPoint()
{
}

CGeoDemPoint::~CGeoDemPoint()
{
}

int CGeoDemPoint::GetClassType()const
{
	return CLS_GEODEMPOINT;
}



CGeometry* CGeoDemPoint::Clone()const
{
	CGeometry* pNew = new CGeoDemPoint;
	if( pNew )
	{
		pNew->CopyFrom(this);
	}
	return pNew;
}


void CGeoDemPoint::Draw(GrBuffer *pBuf, float fDrawScale)
{
	BOOL bUseLine = TRUE;
	double ptlen = GetProfileDouble(REGPATH_SYMBOL,_T("MultipointSize"),1.0);
	float fSymLen = ptlen * GetSymbolDrawScale();

	m_nColor = RGB(0,255,0);

	CWinApp *pApp = AfxGetApp();
	if (pApp)
	{
		bUseLine = pApp->GetProfileInt(REGPATH_SYMBOL, _T("DisplayDemLine"), TRUE);
		m_nColor = pApp->GetProfileInt(REGPATH_SYMBOL, _T("MultipointColor"), RGB(0, 255, 0));
	}

	if( bUseLine )
	{
		int num = m_PointList.GetSize();
		if( num<=0 )return;	
		
		Envelope e = CreateEnvelopeFromPts(m_PointList.GetData(),num);
		double len = (e.Width()>e.Height()?e.Width():e.Height());

		len = len * 0.5;
		
		PT_3D pt0,pt1;

		pBuf->BeginPointString(m_nColor,fSymLen,fSymLen,TRUE,0);
		for( int i=0; i<num; i++)
		{
			COPY_3DPT(pt0,m_PointList[i]);

			pBuf->PointString(&pt0,0);
		}
		pBuf->End();

		pBuf->BeginLineString(m_nColor,0);
		pBuf->MoveTo(&m_PointList[0]);
		for( i=0; i<num-1; i++)
		{
			if( GraphAPI::GGet2DDisOf2P(m_PointList[i],m_PointList[i+1])>len )
			{
				pBuf->End();
				pBuf->BeginLineString(m_nColor,0);
				pBuf->MoveTo(&m_PointList[i+1]);
			}
			else
			{
				pBuf->LineTo(&m_PointList[i+1]);
			}
		}
		pBuf->End();
	}
	else
	{
		int num = m_PointList.GetSize();
		if( num<=0 )return;		
		
		PT_3D pt0,pt1;
		
		pBuf->BeginPointString(m_nColor,fSymLen,fSymLen,TRUE,0);

		for( int i=0; i<num; i++)
		{
			COPY_3DPT(pt0,m_PointList[i]);
			
			pBuf->PointString(&pt0,0);		
		}
		pBuf->End();
	}
	
//	CGeometry::Draw(pBuf);
}

double CGeoDemPoint::CalcuDemDot()
{
	double minDis = -1.0,lfTemp;
	for (int i=0;i<m_PointList.GetSize()-1;i++)
	{
		lfTemp = GraphAPI::GGet2DDisOf2P(m_PointList[i],m_PointList[i+1]);
		if (minDis<0||lfTemp<minDis)
		{
			minDis = lfTemp;
		}
	}
	return minDis;		
}

MyNameSpaceEnd
