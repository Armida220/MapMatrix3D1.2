// EBGeoSurface.cpp: implementation of the CGeoSurface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
//#include "EditBase.h"
#include "GeoSurface.h"
#include "SmartViewFunctions.h"
#include "CoordSys.h"
#include "Symbol.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

struct Node{
	int start;//起始点
	int num;//点的数量
};

IMPLEMENT_DYNCREATE(CGeoSurface, CGeoCurveBase)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoSurface::CGeoSurface()
{
	m_fCellScale = m_fIntvScale = 1;
	m_fCellAngle = m_fXStartOff = m_fYStartOff = 0;
	m_fLineWidth = -1;
}

CGeoSurface::~CGeoSurface()
{

}


int  CGeoSurface::GetClassType()const
{
	return CLS_GEOSURFACE;
}


BOOL CGeoSurface::CreateShape(const PT_3DEX *pts, int npt)
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


CGeometry *CGeoSurface::Linearize()
{
	CGeoSurface *pSurface = (CGeoSurface*)Clone();
	if(!pSurface)
		return NULL;
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetPts(pts);
	for (int i=0;i<pts.GetSize();i++)
	{
		if(pts[i].pencode!=penLine&&pts[i].pencode!=penMove)
		{
			pts[i].pencode = penLine;
		}		
	}
	int num = GraphAPI::GKickoffSamePoints(pts.GetData(),pts.GetSize());
	
	pSurface->CreateShape(pts.GetData(),num);
	
	return pSurface;
}

CGeometry* CGeoSurface::Clone()const
{
	CGeoSurface *pNew = new CGeoSurface();
	pNew->CopyFrom(this);
	return pNew;
}


BOOL CGeoSurface::CopyFrom(const CGeometry *pObj)
{
	if( pObj->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CGeoSurface *pSurface = (CGeoSurface*)pObj;
		
		m_fCellAngle = pSurface->m_fCellAngle;
		m_fCellScale = pSurface->m_fCellScale;
		m_fIntvScale = pSurface->m_fIntvScale;
		m_fXStartOff = pSurface->m_fXStartOff;
		m_fYStartOff = pSurface->m_fYStartOff;
		m_fLineWidth = pSurface->m_fLineWidth;
	}
	
	return CGeoCurveBase::CopyFrom(pObj);
}


void CGeoSurface::Draw(GrBuffer *pBuf, float fDrawScale)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);
	if(pts.GetSize()<=0)
		return;
	
	pts.Add(pts[0]);

	BOOL bFillWid = FillLineWidth(pts.GetData(),pts.GetSize(),m_fLineWidth*fDrawScale,1,pBuf,m_nColor,fDrawScale);
	
	if (!bFillWid)
	{
		if( IsFillColor() && IsClosed() )
		{
			COLORREF clr = GetFillColor();
			if(clr==-1)
				clr = GetColor();
			pBuf->BeginPolygon(clr, POLYGON_FILLTYPE_COLOR, GetTransparency()*2.55);
			m_shape.ToGrBuffer(pBuf);	
			pBuf->End();

			BOOL bGrdSize = (m_fLineWidth>0);
			pBuf->BeginLineString(m_nColor,bGrdSize?m_fLineWidth*fDrawScale:m_fLineWidth,bGrdSize);
			m_shape.ToGrBuffer(pBuf);	
			pBuf->End();
		}
		else
		{
			BOOL bGrdSize = (m_fLineWidth>0);
			pBuf->BeginLineString(m_nColor,bGrdSize?m_fLineWidth*fDrawScale:m_fLineWidth,bGrdSize);
			m_shape.ToGrBuffer(pBuf);	
			pBuf->End();
		}
	}
}


void CGeoSurface::DrawForSearch(GrBuffer *pBuf, BOOL bFilled)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	GetShape(pts);
	if(pts.GetSize()<=0)
		return;
	
	pts.Add(pts[0]);
	
	if( bFilled )
	{
		pBuf->BeginPolygon(0, POLYGON_FILLTYPE_COLOR, 0);
		m_shape.ToGrBuffer(pBuf);	
		pBuf->End();
	}
	else
	{
		pBuf->BeginLineString(m_nColor,0,FALSE);
		m_shape.ToGrBuffer(pBuf);	
		pBuf->End();
	}
}


BOOL CGeoSurface::WriteTo(CValueTable& tab)const
{
	CGeoCurveBase::WriteTo(tab);

	CVariantEx var;
	var = (_variant_t)m_fCellAngle;
	tab.AddValue(FIELDNAME_GEOSURFACE_CELLANGLE,&var,IDS_FIELDNAME_GEOCELLANGLE);
	
	var = (_variant_t)m_fCellScale;
	tab.AddValue(FIELDNAME_GEOSURFACE_CELLSCALE,&var,IDS_FIELDNAME_GEOCELLSCALE);

	var = (_variant_t)m_fIntvScale;
	tab.AddValue(FIELDNAME_GEOSURFACE_INTVSCALE,&var,IDS_FIELDNAME_GEOINTVSCALE);

	var = (_variant_t)m_fXStartOff;
	tab.AddValue(FIELDNAME_GEOSURFACE_XSTARTOFF,&var,IDS_FIELDNAME_GEOXSTARTOFF);

	var = (_variant_t)m_fYStartOff;
	tab.AddValue(FIELDNAME_GEOSURFACE_YSTARTOFF,&var,IDS_FIELDNAME_GEOYSTARTOFF);
	
	return TRUE;
}

BOOL CGeoSurface::ReadFrom(CValueTable& tab,int idx)
{
	CGeoCurveBase::ReadFrom(tab,idx);

	const CVariantEx *var;
	if( tab.GetValue(idx,FIELDNAME_GEOSURFACE_CELLANGLE,var) )
	{
		m_fCellAngle = (_variant_t)*var;
	}
	
	if( tab.GetValue(idx,FIELDNAME_GEOSURFACE_CELLSCALE,var) )
	{
		m_fCellScale = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOSURFACE_INTVSCALE,var) )
	{
		m_fIntvScale = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOSURFACE_XSTARTOFF,var) )
	{
		m_fXStartOff = (_variant_t)*var;
	}

	if( tab.GetValue(idx,FIELDNAME_GEOSURFACE_YSTARTOFF,var) )
	{
		m_fYStartOff = (_variant_t)*var;
	}
	
	return TRUE;
}

int CGeoSurface::bPtIn(const PT_3D *pt)
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetPts(pts);
	PT_3DEX pt0(*pt, penNone);
	int ret = GraphAPI::GIsPtInRegion(pt0, pts.GetData(), pts.GetSize());
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

	return FALSE;
}

BOOL CGeoSurface::GetCenter(PT_3D *ipt, PT_3D *opt)
{
	BOOL bOK = CGeoCurveBase::GetCenter(ipt, opt);
	if(bOK && bPtIn(opt))
		return TRUE;

	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetPts(pts);

	PT_3DEX line[2];//对角线
	int pos = 0;
	int nPt = pts.GetSize();
	if(nPt<5)
	{
		return FALSE;
	}

	for(int i=0; i<nPt/2; i++)
	{
		line[0] = pts[i];
		line[1] = pts[i+nPt/2];

		opt->x = line[0].x + (line[1].x-line[0].x)*0.5;
		opt->y = line[0].y + (line[1].y-line[0].y)*0.5;
		opt->z = line[0].z + (line[1].z-line[0].z)*0.5;

		if(bPtIn(opt))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGeoMultiSurface, CGeoSurface)

CGeoMultiSurface::CGeoMultiSurface()
{

}

CGeoMultiSurface::~CGeoMultiSurface()
{
}


int CGeoMultiSurface::GetClassType()const
{
	return CLS_GEOMULTISURFACE;
}

CGeometry* CGeoMultiSurface::Clone()const
{
	CGeoSurface* pNew = new CGeoMultiSurface();
	if( pNew )
	{
		pNew->CopyFrom(this);
	}
	return pNew;
}

BOOL CGeoMultiSurface::CreateShape(const PT_3DEX *pts, int npt)
{
	m_shape.Clear();

	if (npt<=1)
	{
		return FALSE;
	}
	
	CLinearizer line;
	CShapeLine shape;	
	int start = 0;
	BOOL ret = FALSE;
	for (int i=0;i<npt;i++)
	{		
		if(pts[i].pencode==penMove)
		{
			if(i==0)continue;
			if(i-start<=2)
			{
				start = i;
				continue;
			}
			else
			{	
				shape.Clear();
				line.SetShapeBuf(&shape);
				line.Linearize(pts+start,i-start,TRUE);
				shape.LineEnd();
				m_shape.AddShapeLine(&shape);
				start = i;
				ret = TRUE;
			}
		}
		if (i==npt-1)
		{
			if(npt-start<=2)
				continue;
			else
			{
				shape.Clear();
				line.SetShapeBuf(&shape);
				line.Linearize(pts+start,npt-start,TRUE);
				shape.LineEnd();
				m_shape.AddShapeLine(&shape);				
				ret = TRUE;
			}
		}
	}

	m_evlp = m_shape.GetEnvelope();

	return ret;
}

BOOL CGeoMultiSurface::SetDataPoint(int i, PT_3DEX  pt)
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);
	
	int nSum = arrPts.GetSize();
	
	if( i>=0 && i<nSum )
	{
		COPY_3DPT(arrPts[i],pt);

// 		int nPos1=0, nPos2=nSum-1, j;
// 		for( j=i; j>=0; j--)
// 		{
// 			if( arrPts[j].cd==penMOVE )
// 			{
// 				nPos1 = j;
// 				break;
// 			}
// 		}
// 		for( j=i+1; j<nSum; j++)
// 		{
// 			if( arrPts[j].cd==penMOVE )
// 			{
// 				nPos2 = j-1;
// 				break;
// 			}
// 		}
// 		
// 		if( i!=nPos1 && i!=nPos2 )
// 		{
// 			arrPts[i] = pt;
// 		}
// 		else if( i==nPos1 /*|| num==nPos2*/ )
// 		{
// 			COPY_3DPT(arrPts[nPos1],pt);
// 			COPY_3DPT(arrPts[nPos2],pt);
// 		}
		CreateShape(arrPts.GetData(),arrPts.GetSize());
		return TRUE;
	}	
	return FALSE;
}


void CGeoMultiSurface::Draw(GrBuffer *pBuf,float fDrawScale)
{
	if(IsHideSymbolizeSurfaceBound())
	{
		return;
	}

	if(IsHideSymbolizeSurfaceInnerBound())
	{
		int nGeo = GetSurfaceNum();		
		{			
			CGeoSurface *pNewGeo = new CGeoSurface();
			pNewGeo->CopyFrom(this);
			
			CArray<PT_3DEX,PT_3DEX> arrPts;
			GetSurface(0,arrPts);
			pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
			pNewGeo->Draw(pBuf,fDrawScale);
			delete pNewGeo;
		}
		return;
	}		

	CGeoSurface::Draw(pBuf,fDrawScale);
	return;

// 	CLinearizer lb;
// 	lb.SetShapeBuf(&m_shape);
	pBuf->BeginLineString(m_nColor,0);
	//pBuf->BeginPolygon(m_nColor);
	m_shape.ToGrBuffer(pBuf);	
	pBuf->End();
}


void CGeoMultiSurface::AddSurface(CArray<PT_3DEX,PT_3DEX>& arr)
{
	if( arr.GetSize()<=0 )return;
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int nsz = pts.GetSize();
	if( nsz>0 )
	{		
		pts.Append(arr);
		pts[nsz].pencode = penMove;
	}
	else
	{
		pts.Append(arr);
	}
	CreateShape(pts.GetData(),pts.GetSize());
}

int  CGeoMultiSurface::GetSurfaceNum()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int size = pts.GetSize(), nCount = 1;
		for( int i=1; i<size; i++)
	{
		if( pts[i].pencode==penMove )
			nCount++;
	}
	return nCount;
}

void CGeoMultiSurface::GetSurface(int idx, CArray<PT_3DEX,PT_3DEX>& arr)const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int size = pts.GetSize(), nCount = 0;	

	if( idx==0 )
	{
		for( int i=0; i<size; i++)
		{
			if( i!=0 && pts[i].pencode==penMove )
				break;

			arr.Add(pts[i]);
		}
	}
	else
	{
		for( int i=1; i<size; i++)
		{
			if( pts[i].pencode==penMove )
				nCount++;
			if( nCount==idx )break;
		}

		if( nCount==idx )
		{
			arr.Add(pts[i]);
			i++;
			for( ; i<size; i++)
			{
				if( pts[i].pencode==penMove )
					break;
				
				arr.Add(pts[i]);
			}
		}
	}
}

void CGeoMultiSurface::CopyFromSurface(const CGeometry *pObj)
{
	CopyFrom(pObj);
	//m_nClassID = CLS_GEOMULTISURFACE;
}


void CGeoMultiSurface::CopyToSurface(CGeometry *pObj)
{
//	long old = pObj->m_nClassID;
	pObj->CopyFrom(this);
//	pObj->m_nClassID = old;
}

void CGeoMultiSurface::SetClosedPt(int index, PT_3D pt, BOOL bIsClosedPt)
{
	CArray<PT_3DEX,PT_3DEX> arrAllPts;

	if(index<0 || index>=GetDataPointSum() )
		return;

	if(!bIsClosedPt)
	{
		GetShape(arrAllPts);
		PT_3DEX expt = arrAllPts[index];
		COPY_3DPT(expt,pt);

		arrAllPts[index] = expt;
		CreateShape(arrAllPts.GetData(),arrAllPts.GetSize());
		return;
	}

	BOOL bModified = FALSE;
	
	int nGeo = GetSurfaceNum();
	int nPtClosedPos = 0;
	for(int i=0; i<nGeo; i++)
	{
		CArray<PT_3DEX,PT_3DEX> arrSonPts;
		GetSurface(i,arrSonPts);
		
		if(index==nPtClosedPos || index==(nPtClosedPos+arrSonPts.GetSize()-1) )
		{
			PT_3DEX expt = arrSonPts[0];
			COPY_3DPT(expt,pt);
			arrSonPts[0] = expt;

			expt = arrSonPts[arrSonPts.GetSize()-1];
			COPY_3DPT(expt,pt);
			arrSonPts[arrSonPts.GetSize()-1] = expt;

			bModified = TRUE;
		}

		arrAllPts.Append(arrSonPts);
		
		nPtClosedPos += arrSonPts.GetSize();
	}
	if( bModified )
	{
		CreateShape(arrAllPts.GetData(),arrAllPts.GetSize());
	}
}

//长度
double CGeoMultiSurface::GetLength()const
{
	double len = 0;
	int n = GetSurfaceNum();
	for(int i=0; i<n; i++)
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		GetSurface(i,pts);

		len += GraphAPI::GGetAllLen2D(pts.GetData(),pts.GetSize());
	}
	
	return len;
}


//面积
double CGeoMultiSurface::GetArea()const
{
	double area = 0;

	CArray<Node,Node> nodes;
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int i, j, size=pts.GetSize();
	int startpos = 0;
	for( i=1; i<=size; i++)
	{
		if( i==size || pts[i].pencode==penMove )	
		{
			Node item;
			item.start = startpos;
			item.num = i-startpos;
			nodes.Add(item);
			startpos = i;
		}
	}
	
	PT_3DEX *pt3ds = pts.GetData();
	CArray<PT_3DEX,PT_3DEX> newpts;
	
	for(i=0; i<nodes.GetSize(); i++)
	{
		int pos1 = nodes[i].start;
		int num1 = nodes[i].num;
		
		int nSum = 0;//计数在多少个面内，不包含自己
		for(j=0; j<nodes.GetSize(); j++)
		{
			int pos2 = nodes[j].start;
			int num2 = nodes[j].num;
			if(pos1 == pos2) continue;
			
			if( 2==GraphAPI::GIsPtInRegion(pt3ds[pos1], pt3ds+pos2, num2) )
			{
				nSum++;
			}
		}
		
		//插入面时首尾点不变
		if(nSum%2)//内边沿
		{
			area -= GraphAPI::GGetPolygonArea(pt3ds+pos1,num1);
		}
		else//外边沿
		{
			area += GraphAPI::GGetPolygonArea(pt3ds+pos1,num1);
		}
	}
	
	return area;
}

int CGeoMultiSurface::bPtIn(const PT_3D *pt)
{
	CArray<PT_3DEX,PT_3DEX> pts, pts2;
	m_shape.GetKeyPts(pts);
	int size = pts.GetSize();

	int nSum = 0;
	PT_3DEX pt0(*pt, penNone);
	
	for( int i=0; i<size; i++)
	{
		if( i!=0 && pts[i].pencode==penMove )
		{
			int ret = GraphAPI::GIsPtInRegion(pt0, pts2.GetData(), pts2.GetSize());
			if(ret==1 || ret==0)
			{
				return 1;
			}
			else if( ret ==2 )
			{
				nSum++;
			}

			pts2.RemoveAll();
			pts2.Add(pts[i]);
		}
		else
		{
			pts2.Add(pts[i]);
		}		
	}

	//最后一段
	
	int ret = GraphAPI::GIsPtInRegion(pt0, pts2.GetData(), pts2.GetSize());
	if(ret==1)
	{
		return 1;
	}
	else if( ret ==2 )
	{
		nSum++;
	}

	return ((nSum%2)==1?2:0);
}

static int GetLineIntersectCurve_static(PT_3DEX& pt0, PT_3DEX& pt1, PT_3DEX *pts, int num, CArray<PT_3D, PT_3D> &arrIntersectPts)
{
	if (!pts || num < 2) return false;

	PT_3D ret;
	double t;

	for (int j = 0; j < num - 1; j++)
	{
		if ((pts + j + 1)->pencode == penMove) continue;
		//求相交点
		if (!GraphAPI::GGetLineIntersectLineSeg(pt0.x, pt0.y, pt1.x, pt1.y, pts[j].x, pts[j].y,
			pts[j + 1].x, pts[j + 1].y, &ret.x, &ret.y, &t))
		{
			continue;
		}
		else
		{
			//按照 x 大小排序插入
			int size = arrIntersectPts.GetSize();
			for (int k = 0; k < size && ret.x < arrIntersectPts[k].x; k++);
			if (k < size)
				arrIntersectPts.InsertAt(k, ret);
			else
				arrIntersectPts.Add(ret);
		}
	}

	return arrIntersectPts.GetSize();
}

//获取复杂面内部一个位置较为合适的点
BOOL CGeoMultiSurface::GetCenter(PT_3D *ipt, PT_3D *opt)
{
	int n = GetSurfaceNum();
	if (n < 2) return CGeoSurface::GetCenter(ipt, opt);

	PT_3DEX pts[2];
	pts[0].x = m_evlp.m_xl; pts[0].y = (m_evlp.m_yl + m_evlp.m_yh) / 2; pts[0].z = 0;
	pts[1].x = m_evlp.m_xh; pts[1].y = pts[0].y; pts[1].z = 0;

	CArray<PT_3DEX, PT_3DEX> arrPts;
	m_shape.GetKeyPts(arrPts);
	CArray<PT_3D, PT_3D> retPts;
	GetLineIntersectCurve_static(pts[0], pts[1], arrPts.GetData(), arrPts.GetSize(), retPts);

	double maxlen = 0;
	for (int i = 0; i < retPts.GetSize() / 2; i++)
	{
		double len = GraphAPI::GGet2DDisOf2P(retPts[2 * i], retPts[2 * i + 1]);
		if (len > maxlen)
		{
			opt->x = (retPts[2 * i].x + retPts[2 * i + 1].x) / 2;
			opt->y = pts[0].y;
			opt->z = 0;
			maxlen = len;
		}
	}

	return TRUE;
}

void CGeoMultiSurface::NormalizeDirection()
{
	CArray<Node,Node> nodes;
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int i, j, k, size=pts.GetSize();
	int startpos = 0;
	for( i=1; i<=size; i++)
	{
		if( i==size || pts[i].pencode==penMove )	
		{
			Node item;
			item.start = startpos;
			item.num = i-startpos;
			nodes.Add(item);
			startpos = i;
		}
	}

	PT_3DEX *pt3ds = pts.GetData();
	CArray<PT_3DEX,PT_3DEX> newpts;

	for(i=0; i<nodes.GetSize(); i++)
	{
		int pos1 = nodes[i].start;
		int num1 = nodes[i].num;

		int nSum = 0;//计数在多少个面内，不包含自己
		for(j=0; j<nodes.GetSize(); j++)
		{
			int pos2 = nodes[j].start;
			int num2 = nodes[j].num;
			if(pos1 == pos2) continue;

			if( 2==GraphAPI::GIsPtInRegion(pt3ds[pos1], pt3ds+pos2, num2) )
			{
				nSum++;
			}
		}

		//插入面时首尾点不变
		if(nSum%2)//内边沿
		{
			newpts.Add(pt3ds[pos1]);
			if(1==GraphAPI::GIsClockwise(pt3ds+pos1, num1))
			{
				for(k=num1-2; k>=1; k--)
				{
					newpts.Add(pt3ds[pos1+k]);
				}
			}
			else
			{
				for(k=1; k<num1-1; k++)
				{
					newpts.Add(pt3ds[pos1+k]);
				}
			}
			newpts.Add( pt3ds[pos1+num1-1] );
		}
		else//外边沿
		{
			newpts.Add(pt3ds[pos1]);
			if(0==GraphAPI::GIsClockwise(pt3ds+pos1, num1))
			{
				for(k=num1-2; k>=1; k--)
				{
					newpts.Add(pt3ds[pos1+k]);
				}
			}
			else
			{
				for(k=1; k<num1-1; k++)
				{
					newpts.Add(pt3ds[pos1+k]);
				}
			}
			newpts.Add( pt3ds[pos1+num1-1] );
		}
	}

	CreateShape(newpts.GetData(), newpts.GetSize());
}

BOOL CGeoMultiSurface::IsClosed()const
{
	CArray<PT_3DEX,PT_3DEX> pts;
	m_shape.GetKeyPts(pts);
	int num = pts.GetSize();
	if(num<3) return FALSE;

	PT_3DEX pt0,pt1;
	pt0 = pts[0];
	for(int i=0; i<num; i++)
	{
		if( i!=0 && pts[i].pencode==penMove)
		{
			pt1 = pts[i-1];
			if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 /*&& fabs(pt0.z-pt1.z)<1e-4*/ )
				;
			else
				return FALSE;

			pt0 = pts[i];
		}
	}

	pt1 = pts[num-1];
	if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 /*&& fabs(pt0.z-pt1.z)<1e-4*/ )
		return TRUE;
	else
		return FALSE;

	return TRUE;
}

MyNameSpaceEnd