// GeoParallel.cpp: implementation of the CGeoParallel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeoParallel.h"
#include "SmartViewFunctions.h"
#include "resource.h"
#include "Functions_temp.h"
MyNameSpaceBegin
IMPLEMENT_DYNCREATE(CGeoParallel, CGeoCurve)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoParallel::CGeoParallel()
{
	m_lfWidth = 0;
	m_lfDHeight = 0;
}

CGeoParallel::~CGeoParallel()
{

}

Envelope CGeoParallel::GetEnvelope()
{
	Envelope e;
	e = m_evlp;
	float w = fabs(m_lfWidth);
	e.m_xh += w;
	e.m_xl -= w;
	e.m_yh += w;
	e.m_yl -= w;
	return e;
}
BOOL CGeoParallel::WriteTo(CValueTable& tab)const
{
	CGeoCurve::WriteTo(tab);
	
	CVariantEx var;
	var = (_variant_t)(double)m_lfWidth;
	tab.AddValue(FIELDNAME_GEOCURVE_WIDTH,&var,IDS_FIELDNAME_GEOCURVE_WIDTH);
	var = (_variant_t)(double)m_lfDHeight;
	tab.AddValue(FIELDNAME_GEOCURVE_DHEIGHT,&var,IDS_FIELDNAME_GEOCURVE_DHEIGHT);
	return TRUE;
}

BOOL CGeoParallel::ReadFrom(CValueTable& tab,int idx)
{
	CGeoCurve::ReadFrom(tab,idx);
	
	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOCURVE_WIDTH,var) )
	{
		m_lfWidth = (double)(_variant_t)*var;
	}
	if( tab.GetValue(idx,FIELDNAME_GEOCURVE_DHEIGHT,var) )
	{
		m_lfDHeight = (double)(_variant_t)*var;
	}
	
	return TRUE;
}



double CGeoParallel::GetWidth()
{
	return m_lfWidth;
}

void CGeoParallel::SetWidth(double wid)
{
	m_lfWidth = wid;

}

void CGeoParallel::GetSamePointPos(CArray<int, int> &pos) const
{
	pos.RemoveAll();
	
	CArray<PT_3DEX,PT_3DEX> arr;
	GetShape(arr);

	int num = arr.GetSize();

	int index = 0;
	pos.Add(0);

	for (int i=1; i<num; i++)
	{
		if (_FABS(arr[i].x-arr[i-1].x)<1e-4 && 
			_FABS(arr[i].y-arr[i-1].y)<1e-4 &&
			_FABS(arr[i].z-arr[i-1].z)<1e-4);
		else
		{
			index = i;
		}

		pos.Add(index);
	}
}

CGeoCurve *CGeoParallel::GetCenterlineCurve()
{
	CArray<PT_3DEX,PT_3DEX> arr;

	if( fabs(m_lfWidth)<GraphAPI::GetDisTolerance() )
		GetShape(arr);
	else
		GetParallelShape(arr,m_lfWidth/2);

	CGeoCurve *pGeo = new CGeoCurve;
	if (!pGeo) return NULL;

	pGeo->m_fLineWidth = m_fLineWidth;
	pGeo->m_fLinetypeScale = m_fLinetypeScale;
	pGeo->m_fLinewidthScale = m_fLinewidthScale;
	pGeo->SetColor(GetColor());

	if (pGeo->CreateShape(arr.GetData(),arr.GetSize())) return pGeo;

	delete pGeo;
	pGeo = NULL;

	return NULL;

}

void CGeoParallel::GetParallelShape(CArray<PT_3DEX,PT_3DEX>& ptex, double wid)const
{

	if (fabs(wid) <GraphAPI::GetDisTolerance())
	{
		wid = m_lfWidth;
	}

	if (fabs(wid) <GraphAPI::GetDisTolerance()) return;

	CArray<PT_3DEX,PT_3DEX> arr;
	GetShape(arr);

	int num = GraphAPI::GKickoffSamePoints(arr.GetData(),arr.GetSize());
	arr.SetSize(num);
	
	if(num < 2)return;

	CArray<PT_3D,PT_3D> pts,ptsRet;
	pts.SetSize(num);
	ptsRet.SetSize(num);

	for (int i=0;i<num;i++)
	{
		COPY_3DPT(pts[i],arr[i]);
	}

	if( GraphAPI::GGetParallelLine(pts.GetData(),num,wid,ptsRet.GetData()) )
	{
		double fkz = 0;
		if (fabs(m_lfWidth) > GraphAPI::GetDisTolerance())
		{
			fkz = wid/m_lfWidth;
		}

		int size = num;

		ptex.SetSize(size);
		for (i=0; i<size; i++)
		{
			ptex[i].x = ptsRet[i].x;
			ptex[i].y = ptsRet[i].y;
			ptex[i].z = ptsRet[i].z + fkz*m_lfDHeight;

			ptex[i].pencode = arr[i].pencode;
			
		}
	}
}

int CGeoParallel::GetCtrlPointSum()const
{ 
	return 1; 
}

int CGeoParallel::GetClassType()const
{
	return CLS_GEOPARALLEL;
}

PT_3DEX CGeoParallel::GetCtrlPoint(int i)const
{
	const CShapeLine *pSL = ((CGeoParallel*)this)->GetShape();
	if( !pSL )return PT_3DEX();
	
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!pSL->GetPts(arr))return PT_3DEX();
	int sum = arr.GetSize();

	sum = GraphAPI::GKickoffSamePoints(arr.GetData(),sum);

	if( sum<=1 )return PT_3DEX();
	if( i<0 || i>=GetCtrlPointSum() )return PT_3DEX();

	if( IsClosed() )
	{
		PT_3DEX  t;
		PT_3D pts[3],ret[3];
	
		t = arr.GetAt(sum-2);
		COPY_3DPT(pts[0],t);
		t = arr.GetAt(sum-1);
		COPY_3DPT(pts[1],t);
		t = arr.GetAt(1);
		COPY_3DPT(pts[2],t);
		
		if( !GraphAPI::GGetParallelLine(pts,3,m_lfWidth,ret) )
			return PT_3DEX();

		ret[1].z += m_lfDHeight;
		return PT_3DEX(ret[1],penNone,0,ctrlType_Width);	
	}
	
	PT_3DEX t;
	PT_3D pts[2],ret[2];

	t = arr.GetAt(sum-2);
	COPY_3DPT(pts[0],t);
	t = arr.GetAt(sum-1);
	COPY_3DPT(pts[1],t);
	
	if( !GraphAPI::GGetParallelLine(pts,2,m_lfWidth,ret) )
		return PT_3DEX();
	
	ret[1].z += m_lfDHeight;
	return PT_3DEX(ret[1],penNone,0,ctrlType_Width);	
}

BOOL CGeoParallel::SetCtrlPoint(int i, PT_3D pt)
{ 
	const CShapeLine *pSL = ((CGeoParallel*)this)->GetShape();
	if( !pSL )return FALSE;
	
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!pSL->GetPts(arr))return FALSE;
	
	int sum = arr.GetSize();

	sum = GraphAPI::GKickoffSamePoints(arr.GetData(),sum);

	if( sum<=1 )return FALSE;
	if( i<0 || i>=GetCtrlPointSum() )return FALSE;
	
	PT_3DEX t;
	PT_3D pts[3];
	
	t = arr.GetAt(sum-2);
	COPY_3DPT(pts[0],t);
	t = arr.GetAt(sum-1);
	COPY_3DPT(pts[1],t);
	
	double wid = GraphAPI::GGetNearestDisOfPtToLine(pts[0].x,pts[0].y,
		pts[1].x,pts[1].y,pt.x,pt.y);
	
	pts[2] = pt;

	m_lfDHeight = pts[2].z - pts[1].z;
	
	bool bClockwise = (GraphAPI::GIsClockwise(pts,3)==1);
	if( bClockwise )wid =-wid;
	
	SetWidth( wid );
	return TRUE;
}



BOOL CGeoParallel::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoParallel)) )
	{
		CGeoParallel *pPara = (CGeoParallel*)pObj;
		m_lfWidth = pPara->m_lfWidth;
		m_lfDHeight = pPara->m_lfDHeight;
	}	

	return CGeoCurve::CopyFrom(pObj);
}


CGeometry* CGeoParallel::Clone()const
{
	CGeoParallel* pNew = new CGeoParallel;
	if( pNew )
	{
		pNew->CopyFrom(this);
	}
	return pNew;
}

BOOL CGeoParallel::GetCenter(PT_3D *ipt, PT_3D *opt)
{
	// 即平行线中线的中点位置
	CArray<PT_3DEX,PT_3DEX> basepts;
	m_shape.GetPts(basepts);
	int num = basepts.GetSize();
			
	PT_3D *pts = new PT_3D[num];
	if (!pts)return FALSE;
				
	for (int i=0;i<num;i++)
	{
		COPY_3DPT(pts[i],basepts[i]);
	}
				
	PT_3D *offpts = new PT_3D[num];
	if( !offpts )return FALSE;
				
	if ( !GraphAPI::GGetParallelLine(pts,num,GetWidth()/2,offpts) ) return FALSE;

	int index = GraphAPI::GGetMiddlePt(offpts,num,opt);

	if (opt)
	{
		opt->z += m_lfDHeight/2;
	}
						
	delete []pts;
	delete []offpts;
	
	return TRUE;
}

void CGeoParallel::Draw(GrBuffer *pBuf, float fDrawScale)
{	
	CGeoCurve::Draw(pBuf,fDrawScale);
	
	CArray<PT_3DEX,PT_3DEX> arr,Ret;
	if(!m_shape.GetPts(arr))
		return;	
	int num = arr.GetSize();
	if( num<=0 )return;	
		
// 	PT_3D *pts = new PT_3D[num];
// 	PT_3D *pRet = new PT_3D[num];
//	if( pts && pRet )
	{
	//	for( int i=0; i<num; i++)COPY_3DPT(pts[i],(arr[i]));
		//COPY_3DPT(pts[i],arr[0]);
		num = GraphAPI::GKickoffSamePoints(arr.GetData(),num);
		
		arr.SetSize(num);
		arr.FreeExtra();
		num = arr.GetSize();
		Ret.SetSize(num);
		if( GraphAPI::GGetParallelLine(arr.GetData(),arr.GetSize(),m_lfWidth,Ret.GetData()) )
		{
			BOOL bGrdSize = (m_fLineWidth>0);
			GrBuffer buf;
			buf.BeginLineString(m_nColor,bGrdSize?m_fLinewidthScale*m_fLineWidth*fDrawScale:m_fLinewidthScale,bGrdSize);
			
			for(int i=num -1; i>=0; i--)
			{
				PT_3DEX pt = Ret[i];
				pt.z += m_lfDHeight;
				if( i==(num-1) )buf.MoveTo(&pt);
				else buf.LineTo(&pt);
			}
		
			buf.End();
			pBuf->AddBuffer(&buf);					
		}
	}	
// 	if( pts )delete[] pts;
// 	if( pRet )delete[] pRet;
	

	pBuf->RefreshEnvelope();
	
}

BOOL CGeoParallel::Separate(CGeometry *&pGeo1, CGeometry *&pGeo2)
{
	pGeo1 = pGeo2 = NULL;	

	CArray<PT_3DEX,PT_3DEX> arr;
	GetShape(arr);

	int num = GraphAPI::GKickoffSamePoints(arr.GetData(),arr.GetSize());
	arr.SetSize(num);

	if( num<2 )return FALSE;
	
	PT_3D  *pts,*ptsRet;

	pts = new PT_3D[num];
	if (!pts)
	{
		return FALSE;
	}

	for (int i=0;i<num;i++)
	{
		COPY_3DPT(pts[i],arr[i]);
	}	

	ptsRet = new PT_3D[num];
	if( !ptsRet )
	{	
		delete []pts;
		return FALSE;
	}	
	
	if( GraphAPI::GGetParallelLine(pts,num,m_lfWidth,ptsRet) )
	{
		pGeo1 = new CGeoCurve;
		if(!pGeo1) return FALSE;
		CValueTable tab;
		tab.BeginAddValueItem();
		WriteTo(tab);
		tab.EndAddValueItem();
		pGeo1->ReadFrom(tab);

		pGeo2 = pGeo1->Clone();	
		if(!pGeo2) return FALSE;

		if( pGeo1 )
		{			
			pGeo1->CreateShape(arr.GetData(),arr.GetSize());				
		}
		
		if( pGeo2 )
		{
			int size = num;

			CArray<PT_3DEX,PT_3DEX> arr2;
			arr2.Copy(arr);

			//第二条线反转
			for(int j=size-1; j>=0; j--)
			{				
				arr2[j] = arr[size-1-j];				
				COPY_3DPT(arr2[j],(ptsRet[size-1-j]));
				arr2[j].z += m_lfDHeight;
				
			}
/*
			for(int j=size-1; j>=0; j--)
			{				
				COPY_3DPT(arr2[j],(ptsRet[j]));
				arr2[j].z += m_lfDHeight;
				
			}
*/
			pGeo2->CreateShape(arr2.GetData(),arr2.GetSize());
		}
	}	
	
	delete []pts;
	delete []ptsRet;	
	return (pGeo1!=NULL && pGeo2!=NULL);
	
}


BOOL CGeoParallel::SeparateNoReverse(CGeometry *&pGeo1, CGeometry *&pGeo2)
{
	pGeo1 = pGeo2 = NULL;	

	CArray<PT_3DEX,PT_3DEX> arr;
	GetShape(arr);

	int num = GraphAPI::GKickoffSamePoints(arr.GetData(),arr.GetSize());
	arr.SetSize(num);

	if( num<2 )return FALSE;
	
	PT_3D  *pts,*ptsRet;

	pts = new PT_3D[num];
	if (!pts)
	{
		return FALSE;
	}

	for (int i=0;i<num;i++)
	{
		COPY_3DPT(pts[i],arr[i]);
	}	

	ptsRet = new PT_3D[num];
	if( !ptsRet )
	{	
		delete []pts;
		return FALSE;
	}	
	
	if( GraphAPI::GGetParallelLine(pts,num,m_lfWidth,ptsRet) )
	{	
		pGeo1 = new CGeoCurve;
		if(!pGeo1) return FALSE;
		CValueTable tab;
		tab.BeginAddValueItem();
		WriteTo(tab);
		tab.EndAddValueItem();
		pGeo1->ReadFrom(tab);

		pGeo2 = pGeo1->Clone();	
		if(!pGeo2) return FALSE;

		if( pGeo1 )
		{			
			pGeo1->CreateShape(arr.GetData(),arr.GetSize());				
		}
		
		if( pGeo2 )
		{
			int size = num;

			CArray<PT_3DEX,PT_3DEX> arr2;
			arr2.Copy(arr);

			for(int j=size-1; j>=0; j--)
			{				
				COPY_3DPT(arr2[j],(ptsRet[j]));
				arr2[j].z += m_lfDHeight;
				
			}
			pGeo2->CreateShape(arr2.GetData(),arr2.GetSize());
		}
	}	
	
	delete []pts;
	delete []ptsRet;	
	return (pGeo1!=NULL && pGeo2!=NULL);	
}


//长度
double CGeoParallel::GetLength()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);
	
	return GraphAPI::GGetAllLen2D(pts.GetData(),pts.GetSize());
}


//面积
double CGeoParallel::GetArea()const
{
	CArray<PT_3DEX,PT_3DEX> pts, pts2;
	GetShape(pts);

	GetParallelShape(pts2,0);

	for( int i=pts2.GetSize()-1; i>=0; i--)
	{
		pts.Add(pts2[i]);
	}
	
	return GraphAPI::GGetPolygonArea(pts.GetData(),pts.GetSize());
}

void CGeoParallel::GetAllShape(CArray<PT_3DEX,PT_3DEX>& pts)
{
	if (fabs(m_lfWidth) <GraphAPI::GetDisTolerance())
		return;
	
	CArray<PT_3DEX,PT_3DEX> arr, arr1;
	GetShape(arr);
	
	int num = GraphAPI::GKickoffSamePoints(arr.GetData(),arr.GetSize());
	arr.SetSize(num);
	
	if(num < 2)return;

	arr1.SetSize(num);

	if( GraphAPI::GGetParallelLine(arr.GetData(),num,m_lfWidth,arr1.GetData()) )
	{
		for (int i=0; i<num; i++)
		{
			arr1[i].z = arr[i].z + m_lfDHeight;
			arr1[i].pencode = arr[i].pencode;
		}
		pts.Copy(arr);
		arr1[0].pencode=penMove;
		pts.Append(arr1);
	}
}

MyNameSpaceEnd


