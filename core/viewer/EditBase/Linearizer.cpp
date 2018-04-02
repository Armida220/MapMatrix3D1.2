// Linearizer.cpp: implementation of the CLinearizer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <float.h>
#include "Linearizer.h"
#include "SmartViewFunctions.h"
#include "CoordSys.h"
#include "SmartViewBaseType.h"
#include "Symbol.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin



class CSinhTool
{
public:
	CSinhTool()
	{

	}
	~CSinhTool()
	{

	}
	void Set_h(double h)
	{
		sinh_h = sinh(h);
		cosh_h = cosh(h);
	}

	void Set_dh(double dh)
	{
		sinh_cur_dh = sinh_dh = sinh(dh);
		cosh_cur_dh = cosh_dh = cosh(dh);
	}

	// 根据 sinh(x+y) = sinh(x) * cosh(y) + cosh(x) * sinh(y); cosh(x+y) = cosh(x) * cosh(y) + sinh(x) * sinh(y)

	void Add_dh()
	{
		double sh,ch;
		sh = sinh_dh*cosh_cur_dh + cosh_dh*sinh_cur_dh;
		ch = sinh_dh*sinh_cur_dh + cosh_dh*cosh_cur_dh;

		sinh_cur_dh = sh;
		cosh_cur_dh = ch;
	}

	// 根据 sinh(x+y) = sinh(x) * cosh(y) + cosh(x) * sinh(y); cosh(x+y) = cosh(x) * cosh(y) + sinh(x) * sinh(y)

	double Sinh_h_sub_dh()
	{
		return (sinh_h*cosh_cur_dh - cosh_h*sinh_cur_dh);
	}

	double Sinh_dh()
	{
		return sinh_cur_dh;
	}

private:
	double sinh_dh, cosh_dh;
	double sinh_h, cosh_h;
	double sinh_cur_dh, cosh_cur_dh;
};


CShapeLine::CShapeLine()
{
	m_pHead = NULL;
	m_pTail = NULL;
}


CShapeLine::~CShapeLine()
{
	Clear();
}


void CShapeLine::Clear()
{
	ShapeLineUnit *p = (ShapeLineUnit*)m_pHead, *p2;
		
	while( p!=NULL )
	{
		p2 = p->next;
		if( p->pts )delete[] p->pts;
		delete p;
		p = p2;
	}
	
	m_pHead = NULL;
	m_pTail = NULL;
}


#define SHAPLINE_LEN	256

BOOL CShapeLine::ReallocLineUnit()
{
	if( m_pHead==NULL )
	{
		ShapeLineUnit *pNew = new ShapeLineUnit;
		if( pNew==NULL )return FALSE;
		
		memset(pNew,0,sizeof(ShapeLineUnit));
		
		pNew->pts = new PT_3DEX[SHAPLINE_LEN];
		if( !pNew->pts )
		{
			delete pNew;
			return FALSE;
		}
		m_pHead = pNew;
		m_pTail = pNew;
		return TRUE;
	}

	if( m_pTail->nuse<SHAPLINE_LEN )
		return TRUE;

	ShapeLineUnit *pNew = new ShapeLineUnit;
	if( !pNew )return NULL;

	memset(pNew,0,sizeof(ShapeLineUnit));

	pNew->pts = new PT_3DEX[SHAPLINE_LEN];
	if( !pNew->pts )
	{
		delete pNew;
		return NULL;
	}

	pNew->pts[0] = m_pTail->pts[m_pTail->nuse-1];
	pNew->pts[0].pencode = penNone;
	pNew->nuse = 1;

	m_pTail->next = pNew;
	m_pTail = pNew;

	return TRUE;
}

BOOL CShapeLine::AddShapeLine(const CShapeLine *pShape)
{
	if(!pShape)
		return FALSE;
	if (!m_pHead)
	{
		return CopyFrom(pShape);
	}
	CPtrArray arr;
	if(!pShape->GetShapeLineUnit(arr)) return FALSE;
	int nsz = arr.GetSize();
	if(nsz<=0)return FALSE;
	
	for (int i=0;i<nsz;i++)
	{
		ShapeLineUnit *pUnit = (ShapeLineUnit*)arr.GetAt(i);
		if (!pUnit) continue;
		ShapeLineUnit *pNew = new ShapeLineUnit;
		if (pNew == NULL) return FALSE;
		pNew->pts = new PT_3DEX[SHAPLINE_LEN];
		if (!pNew->pts)
		{
			delete pNew;
			return FALSE;
		}
		
		memcpy(pNew->pts,pUnit->pts,pUnit->nuse*sizeof(PT_3DEX));
		pNew->nuse = pUnit->nuse;
		pNew->evlp = pUnit->evlp;
		pNew->next = NULL;	
		
		
		m_pTail->next = pNew;
		m_pTail = pNew;		
	}
	return TRUE;
}

BOOL CShapeLine::CopyFrom(const CShapeLine *pShape)
{
	Clear();

	if (!pShape)  return FALSE;

	CPtrArray arr;
	if (pShape->GetShapeLineUnit(arr))
	{
		for (int i=0; i<arr.GetSize(); i++)
		{
			ShapeLineUnit *pUnit = (ShapeLineUnit*)arr.GetAt(i);
			if (!pUnit) continue;
			ShapeLineUnit *pNew = new ShapeLineUnit;
			if (pNew == NULL) return FALSE;
			pNew->pts = new PT_3DEX[SHAPLINE_LEN];
			if (!pNew->pts)
			{
				delete pNew;
				return FALSE;
			}

			memcpy(pNew->pts,pUnit->pts,pUnit->nuse*sizeof(PT_3DEX));
			pNew->nuse = pUnit->nuse;
			pNew->evlp = pUnit->evlp;
			pNew->next = NULL;

			if (i == 0)
			{
				m_pHead = pNew;
				m_pTail = pNew;
			}
			else
			{
				m_pTail->next = pNew;
				m_pTail = pNew;
			}

		}
		return TRUE;
	}

	return FALSE;

}

void CShapeLine::LinePt(PT_3D pt, int pencode, float wid, int type)
{
	if( !ReallocLineUnit() )
		return;
		
	m_pTail->pts[m_pTail->nuse] = PT_3DEX(pt,pencode,wid,type);
	m_pTail->nuse++;

	return;
}


void CShapeLine::LinePts(const PT_3DEX *pts, int num)
{
	int i, cpy_len;
	for( i=0; i<num; )
	{
		if( !ReallocLineUnit() )
			return;
		
		cpy_len = num-i;
		if( cpy_len>(SHAPLINE_LEN-m_pTail->nuse) )
			cpy_len = SHAPLINE_LEN-m_pTail->nuse;

		memcpy(m_pTail->pts+m_pTail->nuse,pts+i,sizeof(PT_3DEX)*cpy_len);
		m_pTail->nuse += cpy_len;
		i += cpy_len;
	}
	
	return;
}

//设置最后一个点的cd码
void CShapeLine::SetLastPtcd(int pencode)
{
	if( m_pTail==NULL )
		return;

	if( m_pTail->nuse<=0 )
		return;

	m_pTail->pts[m_pTail->nuse-1].pencode = pencode;
}


void CShapeLine::SetFirstPtcd(int pencode)
{
	if( m_pHead==NULL )
		return;
	
	if( m_pHead->nuse<=0 )
		return;
	
	m_pHead->pts[0].pencode = pencode;
}


void CShapeLine::LineEnd()
{
	if( m_pTail==NULL )
		return;

	// calculate all envelopes
	ShapeLineUnit *pu = m_pHead, *pp = m_pHead;
	while( pu )
	{
		pu->evlp.CreateFromPts(pu->pts,pu->nuse,sizeof(PT_3DEX),3);
		pp = pu;
		pu = pu->next;
	}

	// tack back redundant space
	if( pp->nuse<SHAPLINE_LEN )
	{	
		PT_3DEX *pts = new PT_3DEX[pp->nuse];
		if( !pts )
		{
			return;
		}
		
		memcpy(pts,pp->pts,pp->nuse*sizeof(PT_3DEX));
		delete[] pp->pts;

		pp->pts = pts;
	}
}

Envelope CShapeLine::GetEnvelope()const
{
	if( m_pHead==NULL )
		return Envelope();
	ShapeLineUnit *pu = m_pHead;
	Envelope e;
	while( pu )
	{
		e.Union(&(pu->evlp),3);
		pu = pu->next;
	}
	return e;
}

BOOL CShapeLine::GetPts(CArray<PT_3DEX,PT_3DEX> &pts)const
{
	pts.RemoveAll();

	if( m_pHead==NULL )
		return FALSE;
	
	int size = 0;
	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		size += pu->nuse;
		pu = pu->next;
	}

	pts.SetSize(size);
	PT_3DEX *buf = pts.GetData();
	PT_3DEX *pos = buf;

	pu = m_pHead;
	while( pu )
	{
		memcpy(pos,pu->pts,sizeof(PT_3DEX)*pu->nuse);
		pos += pu->nuse;
		pu = pu->next;
	}

	return TRUE;
}

int  CShapeLine::GetPtsCount()const
{
	if( m_pHead==NULL )
		return 0;
	int size = 0;
	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		size += pu->nuse;
		pu = pu->next;
	}
	return size;
}

int  CShapeLine::GetKeyPtsCount()const
{
	if( m_pHead==NULL )
		return 0;
	int size = 0;
	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		for( int i=0; i<pu->nuse; i++)
		{
			if( pu->pts[i].pencode!=penNone )
			{
				size++;
			}
		}
		
		pu = pu->next;
	}
	return size;
}

BOOL CShapeLine::GetKeyPts(CArray<PT_3DEX,PT_3DEX> &pts)const
{
	pts.RemoveAll();

	if( m_pHead==NULL )
		return FALSE;
	
	int size = 0;
	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		size += pu->nuse;
		pu = pu->next;
	}
	
	pts.SetSize(size);
	PT_3DEX *buf = pts.GetData();
	PT_3DEX *pos = buf;
	
	pu = m_pHead;
	while( pu )
	{
		for( int i=0; i<pu->nuse; i++)
		{
			if( pu->pts[i].pencode!=penNone )
			{
				*pos = pu->pts[i];
				pos++;
			}
		}

		pu = pu->next;
	}

	pts.SetSize(pos-buf);
	pts.FreeExtra();
	
	return TRUE;
}


PT_3DEX CShapeLine::GetPt(int idx)const
{
	if( m_pHead==NULL )
		return PT_3DEX();
	
	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		if( idx<pu->nuse )
			return pu->pts[idx];
		
		idx -= pu->nuse;
		pu = pu->next;
	}

	return PT_3DEX();
}


BOOL CShapeLine::GetShapeLineUnit(CPtrArray& arr)const
{
	if( m_pHead==NULL )
		return FALSE;

	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		arr.Add(pu);
		pu = pu->next;
	}
	return TRUE;
}


inline double SimpleDis(double x, double y){
	if( x<0 )x = -x;
	if( y<0 )y = -y;
	if( x>y )return (x+0.4*y);
	return (y+0.4*x);
}

double Fast_GNearestDisOfPtToLine(double x0, double y0, double x1, double y1,
							 double xt, double yt, 
							 double *xr, double *yr,
							 bool bFootCanOut)
{
	double x,y;
	GraphAPI::GGetPerpendicular(x0,y0,x1,y1,xt,yt,&x,&y,NULL);
	
	bool bOut = false;
	double d1 = _FABS(x0-x)+_FABS(y0-y);
	double d2 = _FABS(x1-x)+_FABS(y1-y);
	double d3 = _FABS(x0-x1)+_FABS(y0-y1);
	if( d1>d3 || d2>d3 )bOut = true;
	
	if( !bOut||bFootCanOut )
	{
		if(xr)*xr=x;  if(yr)*yr=y; 
		return SimpleDis((xt-x),(yt-y));
	}
	
	d1 = SimpleDis((xt-x0),(yt-y0));
	d2 = SimpleDis((xt-x1),(yt-y1));
	
	if( d1<d2 ){ if(xr)*xr=x0;  if(yr)*yr=y0; return (d1); }
	else { if(xr)*xr=x1;  if(yr)*yr=y1;  return (d2); }
}

BOOL CShapeLine::ShapeLineUnit::FindNearestPt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis)const
{
	PT_3D pt1,pt2,line[2];
	*mindis = -1;
		
	if (pCS==NULL||pCS->GetCoordSysType()==COORDSYS_44MATRIX)
	{
		PT_3D pt = sch_pt;

		Envelope sch_evlp0 = sch_evlp;
		sch_evlp.TransformCltToGrd(pCS);
		if( nuse==1 )
		{
			pCS->GroundToClient(&pts[0],&pt2);
			*mindis = DIST_3DPT(sch_pt,pt2);
			*pRet = pts[0];
		}
		else
		{
			double dis = -1, lfk;
			int k = -1;
			
			PT_3DEX *p = pts;
			
			pCS->GroundToClient(&p[0],&line[1]);
			p++;
			
			for( int i=1; i<nuse; i++,p++)
			{
				line[0] = line[1];
				pCS->GroundToClient(&pts[i],&line[1]);

				if( min(pts[i-1].x,pts[i].x)>sch_evlp.m_xh || max(pts[i-1].x,pts[i].x)<sch_evlp.m_xl || 
					min(pts[i-1].y,pts[i].y)>sch_evlp.m_yh || max(pts[i-1].y,pts[i].y)<sch_evlp.m_yl )
						continue;

// 				line[0] = line[1];
// 				pCS->GroundToClient(&pts[i],&line[1]);
				

				dis = Fast_GNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
					pt.x,pt.y,&pt2.x,&pt2.y,FALSE);
			
				

				if( *mindis<0 || *mindis>dis )
				{
					*mindis = dis;
					
					k = i;
					
					if( mindis==0 )
					{
						double r1 = fabs(line[1].x-line[0].x)+fabs(line[1].y-line[0].y);
						double r2 = fabs(pt2.x-line[0].x)+fabs(pt2.y-line[0].y);
						if( r1<1e-10 )lfk = 0;
						else lfk = r2/r1;
						p = pts;
						pt1.x = p[k-1].x + lfk*(p[k].x-p[k-1].x);
						pt1.y = p[k-1].y + lfk*(p[k].y-p[k-1].y);
						pt1.z = p[k-1].z + lfk*(p[k].z-p[k-1].z);
	
						*pRet = pt1;
						
						goto Find_Over;
					}
				}
			}
			
			if( k>=0 )
			{

				double r1 = fabs(pts[k].x-pts[k-1].x)+fabs(pts[k].y-pts[k-1].y);
				double r2 = fabs(pt2.x-pts[k-1].x)+fabs(pt2.y-pts[k-1].y);
				if( r1<1e-10 )lfk = 0;
				else lfk = r2/r1;
				p = pts;
				pt1.x = p[k-1].x + lfk*(p[k].x-p[k-1].x);
				pt1.y = p[k-1].y + lfk*(p[k].y-p[k-1].y);
				pt1.z = p[k-1].z + lfk*(p[k].z-p[k-1].z);
		
				*pRet = pt1;
			}
			else
			{
				return FALSE;
			}
		}

	}
	else
	{
		if( nuse==1 )
		{
			pCS->GroundToClient(&pts[0],&pt2);
			*mindis = DIST_3DPT(sch_pt,pt2);
			*pRet = pts[0];
		}
		else
		{
			double dis = -1, lfk;
			int k = -1;
			
			PT_3DEX *p = pts;
			
			pCS->GroundToClient(&p[0],&line[1]);
			p++;
			for( int i=1; i<nuse; i++,p++)
			{
				line[0] = line[1];
				pCS->GroundToClient(&pts[i],&line[1]);
				
				dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
					sch_pt.x,sch_pt.y,&pt2.x,&pt2.y,FALSE);
				
				if( *mindis<0 || *mindis>dis )
				{
					*mindis = dis;
					double r1 = fabs(line[1].x-line[0].x)+fabs(line[1].y-line[0].y);
					double r2 = fabs(pt2.x-line[0].x)+fabs(pt2.y-line[0].y);
					if( r1<1e-10 )lfk = 0;
					else lfk = r2/r1;
					k = i;
					
					if( mindis==0 )
					{
						p = pts;
						pt1.x = p[k-1].x + lfk*(p[k].x-p[k-1].x);
						pt1.y = p[k-1].y + lfk*(p[k].y-p[k-1].y);
						pt1.z = p[k-1].z + lfk*(p[k].z-p[k-1].z);
						*pRet = pt1;
						
						goto Find_Over;
					}
				}
			}
			
			if( k>=0 )
			{
				p = pts;
				pt1.x = p[k-1].x + lfk*(p[k].x-p[k-1].x);
				pt1.y = p[k-1].y + lfk*(p[k].y-p[k-1].y);
				pt1.z = p[k-1].z + lfk*(p[k].z-p[k-1].z);
				*pRet = pt1;
			}
			else
			{
				return FALSE;
			}
		}

	}
	
Find_Over:
	return TRUE;
}

int CShapeLine::FindNearestKeyPt(PT_3D pt)const
{
	if (m_pHead == NULL)
	{
		return -1;
	}
	CArray<PT_3DEX,PT_3DEX> pts;
	if (!GetPts(pts))
	{
		return -1;
	}

	//找最近的图形点
	PT_3DEX rpt;
	double x,y,z;
	double dis,min=-1;
	int baseindex = -1;
	
	for( int i=0; i<pts.GetSize()-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
			pts[i+1].x,pts[i+1].y,pts[i+1].z,pt.x,pt.y,pt.z,&x,&y,&z,false);
		
		if( min<0 || dis<min )
		{ 
			min = dis; rpt.x=x; rpt.y=y; rpt.z=z; 
			
			if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y) > 1e-10 )
				baseindex = i;
			//就是第二个点时，序号增加1
			else 
				baseindex = i+1;
		}
	}
	
	if(min<0 || baseindex<0)return -1;


	CArray<int,int> arrKeyPts;
	GetKeyPosOfBaseLines(arrKeyPts);

	for (i=0; i<arrKeyPts.GetSize(); i++)
	{
		if (baseindex < arrKeyPts[i])
		{
			baseindex = i-1;
			break;
		}
	}

	if(i >= arrKeyPts.GetSize())
	{
		baseindex = arrKeyPts.GetSize()-1;
	}

	return baseindex;
	
}


int CShapeLine::FindPosofBaseLines(PT_3D pt)const
{
	if (m_pHead == NULL)
	{
		return -1;
	}
	const PT_3DEX *pts;
	PT_3DEX rpt;
	double mindis = -1;
	double x,y,z;
	int i1=-1;
	const ShapeLineUnit *pList = m_pHead;
	if (pList->nuse==1)
	{
		return 0;
	}
	else
	{
		double dis = -1;
		while (pList!=NULL)
		{
			pts = pList->pts;
			for (int i=0;i<pList->nuse-1;i++)
			{
				//求出当前线段的最近距离
				dis = GraphAPI::GGetNearestDisOfPtToLine3D(pts[i].x,pts[i].y,pts[i].z,
					pts[i+1].x,pts[i+1].y,pts[i+1].z,pt.x,pt.y,pt.z,&x,&y,&z,false);
				if( mindis<0 || dis<mindis )
				{ 
					mindis = dis; rpt.x=x; rpt.y=y; rpt.z=z; 
					
					if( _FABS(x-pts[i+1].x)+_FABS(y-pts[i+1].y)>1e-10 )
						i1=i;
					//就是第二个点时，序号增加1
					else i1=i+1;
				}
			}
			pList = pList->next;

		}
	}
	if( mindis<0 )return -1;
	
	//插入找到的点
	return i1;
}

BOOL CShapeLine::bIntersect(PT_3D pt1, PT_3D pt2) const
{
	if( m_pHead==NULL )
	{
		return FALSE;
	}

	Envelope e = GetEnvelope();
	if( !e.bIntersect(&pt1,&pt2) )return FALSE;

	BOOL bIntersect = FALSE;	
	const ShapeLineUnit *pList = m_pHead;
	while (pList)
	{
		if( pList->evlp.bIntersect(&pt1,&pt2) )
		{
			bIntersect = TRUE;
			break;
		}

		pList = pList->next;
	}

	return bIntersect;	
}

BOOL CShapeLine::FindNearestLine(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, 
								 PT_3D *pRet1, PT_3D *pRet2, double *pMindis, int *pKeyPos)const
{
	if( m_pHead==NULL )
	{
		return FALSE;
	}
	
	PT_3D pt2,line[2],ret1,ret2;
	double mindis = -1;
	const PT_3DEX *pts;
	int nKeyPosi = -1, nKeyPos = -1;
	
	const ShapeLineUnit *pList = m_pHead;
	if( pList->nuse==1 )
	{
		return FALSE;
	}
	else
	{
		double dis = -1;
		while( pList!=NULL )
		{
			
			pts = pList->pts;

			if (pts[0].pencode != penNone)
			{
				nKeyPosi++;
			}

//			if( pts[0].pencode!=penNone )nKeyPosi = 0;
			
			pCS->GroundToClient(&pts[0],&line[1]);
			pts++;
			for( int i=1; i<pList->nuse; i++,pts++)
			{
				line[0] = line[1];
				pCS->GroundToClient(&pts[0],&line[1]);
				
				dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
					sch_pt.x,sch_pt.y,&pt2.x,&pt2.y,FALSE);
				
				if( mindis<0 || mindis>dis )
				{
					mindis = dis;
					ret1 = *(pts-1);
					ret2 = *(pts);
					nKeyPos = nKeyPosi;
					
					if( mindis==0 )
					{
						goto Find_Over;
					}
				}

				if( pts[0].pencode!=penNone )nKeyPosi++;
			}

			pList = pList->next;			
		}
	}
	
Find_Over:

	if( pRet1 )*pRet1 = ret1;
	if( pRet2 )*pRet2 = ret1;
	if( pMindis )*pMindis = mindis;
	if( pKeyPos )*pKeyPos = nKeyPos;

	return TRUE;
}

BOOL CShapeLine::GetKeyPosOfBaseLines(CArray<int,int> &arr)const
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	if(!GetPts(arrPts)) return FALSE;
	int size = arrPts.GetSize();
	for (int i=0;i<size;i++)
	{
		if (arrPts[i].pencode!=penNone)
		{
			arr.Add(i);
		}
	}
	return TRUE;
}


int CShapeLine::GetKeyPos(int basept_index)const
{
	CArray<PT_3DEX,PT_3DEX> arrPts;
	if(!GetPts(arrPts)) return -1;
	int size = arrPts.GetSize(), keypos = -1;
	for (int i=0; i<=basept_index && i<size; i++)
	{
		if (arrPts[i].pencode!=penNone)
		{
			keypos++;
		}
	}
	return keypos;
}


void CShapeLine::GetConstGrBuffer(GrBuffer *pBuf, bool bClosed)const
{
	
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!GetKeyPts(arr)) return;
	int size = arr.GetSize();
	if( size<2 )
	{	
		return;
	}
	//得到节点在图形点中的对应序号
	CArray<int,int> arrPos;
	if(!GetKeyPosOfBaseLines(arrPos)) return;
	int pos = 0;
	int pencode = arr[size-1].pencode;
	arr.RemoveAll();
	if(!GetPts(arr)) return;
	int num = arr.GetUpperBound();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	if ( pencode==penArc || pencode==pen3PArc)
	{
		if (bClosed)
		{
			if (size<5)
			{
				if (size == 4)
				{
					CArray<PT_3D,PT_3D> pts, pts1;
					
					for (int i=0;i<size-2;i++)
					{
						GetPartShape(pts1,i);
						pts.Append(pts1);
					}
					
					//					GetPts(arrPts);
					pBuf->BeginLineString(0,0,0);
					pBuf->Lines(pts.GetData(),pts.GetSize(),sizeof(PT_3D));	
					pBuf->End();
				}
				return;
			}
			pos = size - 3;

			// 三点弧少加一段
			if (pencode == pen3PArc && size%2 != 0)
			{
				pos--;
			}
		}
		else
		{
			if (size<4)
			{
				if (size == 3)
				{
					GetPts(arrPts);
					pBuf->BeginLineString(0,0,0);
					pBuf->Lines(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX));	
					pBuf->End();
				}
				return;
			}
			pos = size - 2;

			// 三点弧少加一段
			if (pencode == pen3PArc && size%2 == 0)
			{
				pos--;
			}
		}
		
	}
	else if ( pencode == penSpline )
	{
		if(size<4)
		{			
			return;
		}
		pos = size - 4;	
		if (bClosed)
		{
			pos--;
		}
	}
	else
	{
		if (size < 2)
		{
			return;
		}
		pos = size-2;
		if (bClosed && pos > 0)
		{
			pos--;
		}
	}
	
// 	int num2 = num;
// 	if( pos<size-1 )num2 = arrPos[pos+1]+1;
 	
// 	for( int i=arrPos[pos]; i<num2; i++) arrPts.Add(arr[i]);
	for( int i=0; i<arrPos[pos+1]+1; i++)arrPts.Add(arr[i]);
	pBuf->BeginLineString(0,0,0);
	pBuf->Lines(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX));	
	pBuf->End();

}


BOOL CShapeLine::GetPartShape(CArray<PT_3D,PT_3D> &pts,int idx)const
{
	pts.RemoveAll();
	if (idx<0) return FALSE;
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!GetKeyPts(arr)) return FALSE;
	int keyNum = arr.GetSize();
	if (idx>=keyNum)
	{
		return FALSE;
	}
	CArray<int ,int> keyPos;
	if(!GetKeyPosOfBaseLines(keyPos)) return FALSE;
	
	CArray<PT_3DEX,PT_3DEX> arr0;
	if(!GetPts(arr0)) return FALSE;
	if (keyPos[idx]<arr0.GetSize()-1)
	{
		pts.Add(arr0[keyPos[idx]]);
		for (int i=keyPos[idx]+1;i<arr0.GetSize();i++)
		{
			pts.Add(arr0[i]);
			if(arr0[i].pencode!=penNone)
				break;
		}
		return TRUE;
	}
	return FALSE;	
}
//使用此函数的规范：（主要是针对编辑节点操作中产生的动态线）
//面：bClosed始终为TRUE，varPt为关键点索引，-1表示尾部关键点（面的尾部关键点和第一点不是重合的）
//闭合线：bClosed始终为TRUE，varPt为关键点索引，-1表示尾部关键点（面的尾部关键点和第一点是重合的）
//非闭合线：bClose为FALSE,varPt为关键点索引，-1表示尾部关键点
void CShapeLine::GetVariantGrBuffer(GrBuffer *pBuf, bool bClosed, int varPt, bool bEditOrInsert)const
{
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!GetKeyPts(arr)) return;
	int size = arr.GetSize();
	if (bClosed&&size<=3)
	{
		;
	}
	if( size<2 )
	{	
		return;
	}	
	
	//最后一个点
	if( varPt==-1 )
	{
		varPt = size-1;
	}
	else if( varPt<0 || varPt>size )
	{	
		return;
	}
	CArray<PT_3D,PT_3D> pts1,pts2;
 	int pencode = arr[varPt].pencode;

	if (pencode == penMove)
	{
		if (varPt<size-1&&varPt>0)
		{
			int linesum = size - 1;
			if (bEditOrInsert)
			{
				if (arr[varPt+1].pencode == penMove)
					linesum = varPt;
				else
					linesum = varPt+1;
			}
			pts1.RemoveAll();
			for (int i=varPt;i<linesum;i++)
			{
				GetPartShape(pts2,i);
				pts1.Append(pts2);
			}
			pBuf->BeginLineString(0,0,0);
			pBuf->Lines(pts1.GetData(),pts1.GetSize());
			pBuf->End();
			return;
		}
	}
	else if (pencode == penLine)
	{
		// move closed
		if (varPt<size-1&&varPt>0)
		{
			int linesum = size - 1;
			if (bEditOrInsert)
			{
				if (arr[varPt+1].pencode == penMove)
					linesum = varPt;
				else
					linesum = varPt+1;
			}
			pts1.RemoveAll();
			for (int i=varPt-1;i<linesum;i++)
			{
				GetPartShape(pts2,i);
				pts1.Append(pts2);
			}
			pBuf->BeginLineString(0,0,0);
			pBuf->Lines(pts1.GetData(),pts1.GetSize());
			pBuf->End();
			return;
		}
		
		if (varPt==0)
		{
			if (bClosed)
			{
				if(!GetPartShape(pts1,size-1))
				{
					GetPartShape(pts1,size-2);
				}
				GetPartShape(pts2,0);
				pBuf->BeginLineString(0,0,0);
				pts1.Append(pts2);
				pBuf->Lines(pts1.GetData(),pts1.GetSize());
				pBuf->End();
				return;
			}
			else
			{
				GetPartShape(pts2,0);
				pBuf->BeginLineString(0,0,0);
				//	pts1.Append(pts2);
				pBuf->Lines(pts2.GetData(),pts2.GetSize());
				pBuf->End();
			}
		}
		// move(not closed),click(closed)
		if (varPt==size-1)
		{
			if (bClosed)
			{
				GetPartShape(pts2,size-2);
				pBuf->BeginLineString(0,0,0);
				//				pts2.Append(pts1);
				pBuf->Lines(pts2.GetData(),pts2.GetSize());
				pBuf->End();
				return;				
			}
			else
			{
				GetPartShape(pts2,size-2);
				pBuf->BeginLineString(0,0,0);
				//	pts1.Append(pts2);
				pBuf->Lines(pts2.GetData(),pts2.GetSize());
				pBuf->End();
				return;
			}
		}
		return;
	}
	else if(pencode == penSpline)
	{
		CArray<PT_3D,PT_3D> pts3;

		int index = varPt-3;

		if (index < 0)
		{
			index = 0;
		}

		for (int i=index; i<=varPt; i++)
		{
			if (arr[i].pencode == penMove)
			{
				break;
			}
		}

		if (i <= varPt)
		{
			index = i;
		}
		else if (i > varPt)
		{
			if (i < size && arr[i].pencode == penMove)
			{
				varPt -= 1;
			}
			else
				index = varPt-3;
		}
		
		
		for ( i=index;i<=varPt;i++)
		{
			GetPartShape(pts3,i);
			pts1.Append(pts3);
		}
// 		for (;i<varPt+3;i++)
// 		{
// 			GetPartShape(pts3,i);
// 			pts1.Append(pts3);
// 		}
		pBuf->BeginLineString(0,0,0);
		//	pts1.Append(pts2);
		pBuf->Lines(pts1.GetData(),pts1.GetSize());
		pBuf->End();

	}
	else if (pencode == penArc || pencode == penStream || pencode == pen3PArc)
	{
		if (varPt < 3 && !bEditOrInsert)
		{
			CArray<PT_3DEX,PT_3DEX> pts;
			GetPts(pts);
			pBuf->BeginLineString(0,0,0);
			pBuf->Lines(pts.GetData(),pts.GetSize(),sizeof(PT_3DEX));
			pBuf->End();
			return;
		}
		else
		{
			if (varPt<(size-1)&&varPt>0)
			{
				if (pencode == pen3PArc && (size%2)==0)
				{
					GetPartShape(pts1,varPt-2);
				}
				GetPartShape(pts2,varPt-1);
				pts1.Append(pts2);
				pts2.RemoveAll();
				
				GetPartShape(pts2,varPt);
				pts1.Append(pts2);

				pBuf->BeginLineString(0,0,0);
				pBuf->Lines(pts1.GetData(),pts1.GetSize());
				pBuf->End();
				return;
			}
			if (varPt==0)
			{
				if (bClosed)
				{
					if(!GetPartShape(pts1,size-1))
					{
						GetPartShape(pts1,size-2);
					}
					GetPartShape(pts2,0);
					pBuf->BeginLineString(0,0,0);
					pts1.Append(pts2);
					pBuf->Lines(pts1.GetData(),pts1.GetSize());
					pBuf->End();
					return;
				}
				else
				{
					GetPartShape(pts2,0);
					pBuf->BeginLineString(0,0,0);
					//	pts1.Append(pts2);
					pBuf->Lines(pts2.GetData(),pts2.GetSize());
					pBuf->End();
				}
			}
			if (varPt==(size-1))
			{
				if (pencode == pen3PArc && (size%2)!=0)
				{
					GetPartShape(pts1,size-3);
				}
				GetPartShape(pts2,size-2);
				pts1.Append(pts2);
				pBuf->BeginLineString(0,0,0);
				pBuf->Lines(pts1.GetData(),pts1.GetSize());
				pBuf->End();
				return;	
			}

		}
	}

	return;

	
}


void CShapeLine::GetAddConstGrBuffer(GrBuffer *pBuf, bool bClosed)const
{
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!GetKeyPts(arr)) return;
	int size = arr.GetSize();
	if( size<2 )
	{	
		return;
	}
	//得到节点在图形点中的对应序号
	CArray<int,int> arrPos;
	if(!GetKeyPosOfBaseLines(arrPos)) return;
	int pos = 0;
	int pencode = arr[size-1].pencode;
	arr.RemoveAll();
	if(!GetPts(arr)) return;
	int num = arr.GetUpperBound();
	CArray<PT_3DEX,PT_3DEX> arrPts;
	if ( pencode==penArc || pencode == pen3PArc)
	{
		if (bClosed)
		{
			if (size<5)
			{
				if (size == 4)
				{
					CArray<PT_3D,PT_3D> pts, pts1;
					
					for (int i=0;i<size-2;i++)
					{
						GetPartShape(pts1,i);
						pts.Append(pts1);
					}

//					GetPts(arrPts);
					pBuf->BeginLineString(0,0,0);
					pBuf->Lines(pts.GetData(),pts.GetSize(),sizeof(PT_3D));	
					pBuf->End();
				}
				return;
			}

			// 三点弧偶数个点加一次常线段（闭合）
			if (pencode == pen3PArc && size%2 != 0)
			{
				return;
			}
			pos = size - 3;
		}
		else
		{
			if (size<4)
			{
				if (size == 3)
				{
					GetPts(arrPts);
					pBuf->BeginLineString(0,0,0);
					pBuf->Lines(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX));	
					pBuf->End();
				}
				return;
			}

			// 三点弧奇数个点加一次常线段（不闭合）
			if (pencode == pen3PArc && size%2 == 0)
			{
				return;
			}
			pos = size - 2;
		}
		
	}
	else if ( pencode == penSpline )
	{
		if(size<4)
		{			
			return;
		}
		pos = size - 4;	
		if (bClosed)
		{
			pos--;
		}
	}
	else
	{
		pos = size-2;
		if (bClosed)
		{
			pos--;
		}
		
	}
	
	int num2 = num;
	if( pos<size-1 )num2 = arrPos[pos+1]+1;
	if( pos<0 )pos = 0;
	// 三点弧多加一段
	if (pencode == pen3PArc)
	{
		pos--;
	}
	for( int i=arrPos[pos]; i<num2; i++) arrPts.Add(arr[i]);
	pBuf->BeginLineString(0,0,0);
	pBuf->Lines(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX));	
	pBuf->End();
}


BOOL CShapeLine::FindNearestPt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *pMindis)const
{
	if( m_pHead==NULL )
	{
		return FALSE;
	}

	PT_3D pt1,pt2,line[2],ret;
	double mindis = -1;
	const PT_3DEX *pts, *mpts = NULL;
	
	const ShapeLineUnit *pList = m_pHead;
	if (pCS->GetCoordSysType()==COORDSYS_44MATRIX)
	{
		double rm[16],m[16];
		pCS->GetRMatrix(rm);

		pCS->GetMatrix(m);
		PT_3D pt;
	
		pt.x = sch_pt.x*rm[0] + sch_pt.y*rm[1] + sch_pt.z*rm[2] + rm[3];
		pt.y = sch_pt.x*rm[4] + sch_pt.y*rm[5] + sch_pt.z*rm[6] + rm[7];
		pt.z = sch_pt.x*rm[8] + sch_pt.y*rm[9] + sch_pt.z*rm[10] + rm[11];

		sch_evlp.TransformCltToGrd(pCS);
		if( pList->nuse==1 )
		{
			pts = pList->pts;
//			pCS->GroundToClient(&pts[0],&pt2);
			mindis = DIST_3DPT(pt,pts[0]);
			ret = pts[0];
		}
		else
		{
			double dis = -1, lfk;
			int k = -1;
			while( pList!=NULL )
			{
				pts = pList->pts;
				
				line[1] = pts[0];
			//	pCS->GroundToClient(&pts[0],&line[1]);
				pts++;
				for( int i=1; i<pList->nuse; i++,pts++)
				{
					line[0] = line[1];
					line[1] = pts[0];
			//		pCS->GroundToClient(&pts[0],&line[1]);
					if(!sch_evlp.bPtIn(&line[0])&&!sch_evlp.bPtIn(&line[1]))
						continue;

					dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
						pt.x,pt.y,&pt2.x,&pt2.y,FALSE);
					
					if( mindis<0 || mindis>dis )
					{
						mindis = dis;
						double r1 = fabs(line[1].x-line[0].x)+fabs(line[1].y-line[0].y);
						double r2 = fabs(pt2.x-line[0].x)+fabs(pt2.y-line[0].y);
						if( r1<1e-10 )lfk = 0;
						else lfk = r2/r1;
						k = i;
						
						mpts = pList->pts;
						
						if( mindis==0 )
						{
							pts = pList->pts;
							pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
							pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
							pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);
							ret.x = pt1.x*m[0] + pt1.y*m[1] + pt1.z*m[2] + m[3];
							ret.y = pt1.x*m[4] + pt1.y*m[5] + pt1.z*m[6] + m[7];
							ret.z = pt1.x*m[8] + pt1.y*m[9] + pt1.z*m[10] + m[11];
							
							
							goto Find_Over;
						}
					}
				}
				
				pList = pList->next;
			}
			
			if( k>=0 )
			{
				pts = mpts;
				pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
				pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
				pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);

				ret.x = pt1.x*m[0] + pt1.y*m[1] + pt1.z*m[2] + m[3];
				ret.y = pt1.x*m[4] + pt1.y*m[5] + pt1.z*m[6] + m[7];
				ret.z = pt1.x*m[8] + pt1.y*m[9] + pt1.z*m[10] + m[11];
			//	ret = pt1;
			}
		}
	}
	else
	{
		if( pList->nuse==1 )
		{
			pts = pList->pts;
			pCS->GroundToClient(&pts[0],&pt2);
			mindis = DIST_3DPT(sch_pt,pt2);
			ret = pts[0];
		}
		else
		{
			double dis = -1, lfk;
			int k = -1;
			while( pList!=NULL )
			{
				pts = pList->pts;
				
				pCS->GroundToClient(&pts[0],&line[1]);
				pts++;
				for( int i=1; i<pList->nuse; i++,pts++)
				{
					line[0] = line[1];
					pCS->GroundToClient(&pts[0],&line[1]);
					
					dis = GraphAPI::GGetNearestDisOfPtToLine(line[0].x,line[0].y,line[1].x,line[1].y,
						sch_pt.x,sch_pt.y,&pt2.x,&pt2.y,FALSE);
					
					if( mindis<0 || mindis>dis )
					{
						mindis = dis;
						double r1 = fabs(line[1].x-line[0].x)+fabs(line[1].y-line[0].y);
						double r2 = fabs(pt2.x-line[0].x)+fabs(pt2.y-line[0].y);
						if( r1<1e-10 )lfk = 0;
						else lfk = r2/r1;
						k = i;
						
						mpts = pList->pts;
						
						if( mindis==0 )
						{
							pts = pList->pts;
							pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
							pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
							pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);
							ret = pt1;
							
							goto Find_Over;
						}
					}
				}
				
				pList = pList->next;
			}
			
			if( k>=0 )
			{
				pts = mpts;
				pt1.x = pts[k-1].x + lfk*(pts[k].x-pts[k-1].x);
				pt1.y = pts[k-1].y + lfk*(pts[k].y-pts[k-1].y);
				pt1.z = pts[k-1].z + lfk*(pts[k].z-pts[k-1].z);
				ret = pt1;
			}
		}

	}
	
Find_Over:

	if( pMindis )*pMindis = mindis;
	if( pRet )*pRet = ret;

	return TRUE;
}

double CShapeLine::GetLength(BOOL b2D)const
{	
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!GetPts(arr))return 0;
	int num = arr.GetSize();
	if( num<=1 )return 0;
	
	//找最近的图形点
	double dis = 0;
	
	for( int i=0; i<num-1; i++)
	{
		if( b2D )
			dis = dis + sqrt( (arr[i].x-arr[i+1].x)*(arr[i].x-arr[i+1].x) +
			(arr[i].y-arr[i+1].y)*(arr[i].y-arr[i+1].y) );
		else
			dis = dis + sqrt( (arr[i].x-arr[i+1].x)*(arr[i].x-arr[i+1].x) +
				(arr[i].y-arr[i+1].y)*(arr[i].y-arr[i+1].y) + 
				(arr[i].z-arr[i+1].z)*(arr[i].z-arr[i+1].z) );
	}
	return dis;
}


double CShapeLine::GetLength(PT_3D *testPt, BOOL bFirst, BOOL b2D)const
{
	double allLen = GetLength(b2D);
	if( !testPt )return allLen;
	
	CArray<PT_3DEX,PT_3DEX> arr;
	if(!GetPts(arr))return 0;
	int num = arr.GetSize();
	if( num<=1 )return 0;
	
	//找最近的图形点
	PT_3DEX rpt1;
	double x,y,z;
	double dis,min1=-1;
	int i1=-1/*, imin, imax*/;
	
	for( int i=0; i<num-1; i++)
	{
		//求出当前线段的最近距离
		dis = GraphAPI::GGetNearestDisOfPtToLine3D(arr[i].x,arr[i].y,arr[i].z,
			arr[i+1].x,arr[i+1].y,arr[i+1].z,
			testPt->x,testPt->y,testPt->z,&x,&y,&z,false,b2D==TRUE);
		
		if( min1<0 || dis<min1 )
		{ 
			min1 = dis; rpt1.x=x; rpt1.y=y; rpt1.z=z; 
			
			if( _FABS(x-arr[i+1].x)+_FABS(y-arr[i+1].y)>1e-10 )
				i1=i;
			//就是第二个点时，序号增加1
			else i1=i+1;
		}
	}
	
	if( min1<0 )
	{		
		return 0;
	}

	//算第一段的距离和第二段距离
	double dis1=0,dis2=0;
	if( i1>=num-1 )
	{
		dis1 = allLen; dis2 = allLen-dis1;
	}
	else
	{
		if( i1<=num/2 )
		{
			for( i=0; i<i1; i++)
			{
				if( b2D )
					dis1 = dis1 + sqrt( (arr[i].x-arr[i+1].x)*(arr[i].x-arr[i+1].x) +
			                              (arr[i].y-arr[i+1].y)*(arr[i].y-arr[i+1].y) );
				else
					dis1 = dis1 + sqrt( (arr[i].x-arr[i+1].x)*(arr[i].x-arr[i+1].x) +
							(arr[i].y-arr[i+1].y)*(arr[i].y-arr[i+1].y) + 
								(arr[i].z-arr[i+1].z)*(arr[i].z-arr[i+1].z) );
			}

			if( b2D )
				dis1 = dis1 + sqrt( (arr[i1].x-rpt1.x)*(arr[i1].x-rpt1.x) +
					(arr[i1].y-rpt1.y)*(arr[i1].y-rpt1.y) );
			else
				dis1 = dis1 + sqrt( (arr[i1].x-rpt1.x)*(arr[i1].x-rpt1.x) +
					(arr[i1].y-rpt1.y)*(arr[i1].y-rpt1.y)  + 
					(arr[i1].z-rpt1.z)*(arr[i1].z-rpt1.z) );

			dis2 = allLen-dis1;
		}
		else
		{
			for( i=i1+1; i<num-1; i++)
			{
				if( b2D )
					dis2 = dis2 + sqrt( (arr[i].x-arr[i+1].x)*(arr[i].x-arr[i+1].x) +
			                 (arr[i].y-arr[i+1].y)*(arr[i].y-arr[i+1].y) );
				else
					dis2 = dis2 + sqrt( (arr[i].x-arr[i+1].x)*(arr[i].x-arr[i+1].x) +
					          (arr[i].y-arr[i+1].y)*(arr[i].y-arr[i+1].y) + 
				              (arr[i].z-arr[i+1].z)*(arr[i].z-arr[i+1].z) );
			}
			if( b2D )
				dis2 = dis2 + sqrt( (arr[i1+1].x-rpt1.x)*(arr[i1+1].x-rpt1.x) +
					(arr[i1+1].y-rpt1.y)*(arr[i1+1].y-rpt1.y) );
			else
				dis2 = dis2 +  sqrt( (arr[i1+1].x-rpt1.x)*(arr[i1+1].x-rpt1.x) +
				    (arr[i1+1].y-rpt1.y)*(arr[i1+1].y-rpt1.y)  + 
					(arr[i1+1].z-rpt1.z)*(arr[i1+1].z-rpt1.z) );

			dis1 = allLen-dis2;
		}
	}

	if( bFirst )return dis1;
	return dis2;
}



BOOL CShapeLine::ToGrBuffer(GrBuffer *buf)const
{
	if( m_pHead==NULL )
		return FALSE;

//	CArray<PT_3DEX,PT_3DEX> arrPts;
//	GetPts(arrPts);
//	SimpleCompressPoints(arrPts,CSymbol::m_tolerance);
// 	buf->Lines(arrPts.GetData(),arrPts.GetSize(),sizeof(PT_3DEX));
// 	
// 	return TRUE;


	int i = 0;
	ShapeLineUnit *pu = m_pHead;
	while( pu )
	{
		GrVertex* pt = new GrVertex[pu->nuse];
		if (!pt)
		{
			continue;
		}		
		for (i=0;i<pu->nuse;i++)
		{
			COPY_3DPT((pt[i]),((pu->pts)[i]));
			if ((pu==m_pHead && i==0)||(pu->pts)[i].pencode==penMove)
			{				
				pt[i].code = GRBUFFER_PTCODE_MOVETO;
			}
			else
			{
				pt[i].code = GRBUFFER_PTCODE_LINETO;
			}
		}
		buf->Lines(pt,pu->nuse);
		pu = pu->next;
		delete []pt;
	}	
	return TRUE;
}




float CLinearizer::m_fPrecision = 0.05f;
double CLinearizer::m_st = 0.03;

CLinearizerPrecisionChange::CLinearizerPrecisionChange(float newValue)
{
	m_fPrecisionSaved = CLinearizer::m_fPrecision;
	CLinearizer::m_fPrecision = newValue;
}

CLinearizerPrecisionChange::~CLinearizerPrecisionChange()
{
	CLinearizer::m_fPrecision = m_fPrecisionSaved;
}

CLinearizerStChange::CLinearizerStChange(double newValue)
{
	m_lfStSaved = CLinearizer::m_st;
	CLinearizer::m_st = newValue;
}

CLinearizerStChange::~CLinearizerStChange()
{
	CLinearizer::m_st = m_lfStSaved;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLinearizer::CLinearizer()
{
	m_pBuf = NULL;
}

CLinearizer::~CLinearizer()
{

}

void CLinearizer::SetShapeBuf(CShapeLine *pBuf)
{
	m_pBuf = pBuf;
}

void CLinearizer::LineString(const PT_3DEX* pts, int nPts)
{
	if( pts==NULL || m_pBuf==NULL || nPts<=0 )return;

	
	m_pBuf->LinePt(pts[0],penNone,pts[0].wid,pts[0].type);
	m_pBuf->LinePts(pts+1,nPts-1);
/*
	for( int i=1; i<nPts; i++)
	{	
		m_pBuf->LinePt(pts[i],pts[i].pencode);
	}
*/
}

BOOL CLinearizer::Arc(const PT_3D *ptc, double radius, double sAngle, double eAngle, double dz, bool bclockwise)
{
	double ang;
	double st=0;
	PT_3D  pt;
	
	if( _FABS(radius)<1e-10 || _FABS(radius)<=_FABS(m_fPrecision) )
	{
		return FALSE;
	}

	//弧高小于0.05m
	if( radius<=1 )
	{
		st = 2*acos(1-_FABS(m_fPrecision)/radius);
		if( st>PI/15 ) st = PI/15;
		if( st<PI/50 ) st = PI/50;
	}
	else
	{
		st = 2*acos(1-_FABS(m_fPrecision)/radius);
		if (st<CLinearizer::m_st)st = CLinearizer::m_st;
	}

	if( bclockwise )st = -st;
	
	//顺时针
	if( bclockwise )
	{
		if( sAngle<eAngle )sAngle += 2*PI;
		for( ang=sAngle; ang>eAngle; ang +=st ) 
		{ 
			pt.x = ptc->x + radius*cos(ang); 
			pt.y = ptc->y + radius*sin(ang); 
			pt.z = ptc->z + dz*(sAngle-ang)/(sAngle-eAngle);

			m_pBuf->LinePt(pt,penNone);
		} 

		pt.x = ptc->x + radius*cos(eAngle); 
		pt.y = ptc->y + radius*sin(eAngle); 
		pt.z = ptc->z + dz;

		m_pBuf->LinePt(pt,penNone);
	}
	//逆时针
	else
	{
		if( sAngle>eAngle )sAngle -= 2*PI;
		for( ang=sAngle; ang<eAngle; ang +=st ) 
		{ 
			pt.x = ptc->x + radius*cos(ang); 
			pt.y = ptc->y + radius*sin(ang); 
			pt.z = ptc->z + dz*(ang-sAngle)/(eAngle-sAngle);

			m_pBuf->LinePt(pt,penNone);
		}

		pt.x = ptc->x + radius*cos(eAngle); 
		pt.y = ptc->y + radius*sin(eAngle); 
		pt.z = ptc->z + dz;

		m_pBuf->LinePt(pt,penNone);
	}

	return TRUE;
}

BOOL CLinearizer::Arc(const PT_3D *ptc, double radius, double sAngle, double mAngle, double eAngle, double dz, bool bclockwise)
{
	if( _FABS(radius)<1e-10 || _FABS(radius)<=_FABS(m_fPrecision) )
	{
		return FALSE;
	}

	double k1 = (mAngle-sAngle)/(eAngle-sAngle);

	if( mAngle!=sAngle )
	{
		Arc(ptc,radius,sAngle,mAngle,k1*dz,bclockwise);
	}

	if( mAngle!=eAngle )
	{
		Arc(ptc,radius,mAngle,eAngle,(1-k1)*dz,bclockwise);
	}

	return TRUE;
}




void CLinearizer::Arc(const PT_3D *spt, const PT_3D *mpt, const PT_3D *ept)
{
	PT_2D pts[3];
	PT_2D ptc;
	double angle[3];
	double r;

	pts[0].x = spt->x;
	pts[0].y = spt->y;

	pts[1].x = mpt->x;
	pts[1].y = mpt->y;

	pts[2].x = ept->x;
	pts[2].y = ept->y;

	_clearfp();
	int bclockwise = 0;

	if( !CalcArcParamFrom3P( pts, &ptc, &r,angle, &bclockwise ) ) 
	{
		m_pBuf->LinePt( *spt,penArc );
		m_pBuf->LinePt( *mpt,penArc );
		m_pBuf->LinePt( *ept,penArc );
	}
	else
	{
		PT_3D pt3dc;
		pt3dc.x = ptc.x; pt3dc.y = ptc.y; pt3dc.z = spt->z;

		Arc(&pt3dc,r,angle[0],angle[1],angle[2],ept->z-spt->z,bclockwise);
		
	}
}



void CLinearizer::Spline(const PT_3DEX* pts, int nPts)
{
	struct TEMP_PARAM
	{
		//坐标
		double x,y,z;
		//弦长
		double h,s;
		//节点关系式方程组系数
		double a,b,c,dx,dy;
		//节点重合数
		int repeat;

		CArray<PT_3DEX,PT_3DEX> *ppts;
	};
	
	if( nPts==0 )return;
	if( nPts==1 )
	{
		m_pBuf->LinePt(*pts,penNone);
		return;
	}
	if( nPts==2 )
	{
		m_pBuf->LinePt(pts[0],penNone);
		m_pBuf->LinePt(pts[1],pts[1].pencode);
		return;
	}	
	
	TEMP_PARAM *p0 = new TEMP_PARAM[nPts+2], *p;

	int i, nPts2;

	memset(p0,0,sizeof(TEMP_PARAM)*(nPts+2));
	for( i=0; i<nPts+2; i++)
	{
		p0[i].ppts = new CArray<PT_3DEX,PT_3DEX>();
	}

	//张力系数
	double omic = 0;
	
	{		
		p = p0;
		p->x = pts[0].x; 
		p->y = pts[0].y;
		p->z = pts[0].z;

		double oldx = p->x, oldy = p->y;
		p++;
		//计算弦长
		for (i=1, nPts2=1; i<nPts; i++)
		{ 
			if( !GraphAPI::GIsEqual2DPoint(&PT_3DEX(oldx,oldy,0,0),pts+i) )
			{
				p->x = pts[i].x;	
				p->y = pts[i].y;
				p->z = pts[i].z; 
				(p-1)->h = GraphAPI::GGetDisOf2P2D((p-1)->x,(p-1)->y,p->x,p->y);
				p->s = (p-1)->s + (p-1)->h;

				oldx = p->x, oldy = p->y;
				p++; 
				nPts2++;
			}
			else
			{
				(p-1)->repeat +=1;
				(p-1)->ppts->Add(pts[i]);
			}	
		}
		
		omic = 2*(nPts2-1)/p0[nPts2-1].s;  
		
		//计算曲线参数
		p = p0; 
		
		double ch0, ch1, sh0, sh1;
		ch0 = cosh(omic*p->h), sh0 = sinh(omic*p->h);
		
		p->a = 0;
		p->b = omic*ch0/sh0 - 1/p->h;
		p->c = 1/p->h - omic/sh0;		
		p->dx = p->dy = 0;

		p++;
		
		for( i=1; i<nPts2-1; i++,p++) 
		{
			ch1 = cosh(omic*p->h), sh1 = sinh(omic*p->h);
			
			p->a = 1/(p-1)->h - omic/sh0;
			
			p->b = omic*ch0/sh0 - 1/(p-1)->h + omic*ch1/sh1 - 1/p->h;
			
			p->c = 1/p->h - omic/sh1;
			
			p->dx = ((p+1)->x-p->x)/p->h - (p->x-(p-1)->x)/(p-1)->h;	
			p->dy = ((p+1)->y-p->y)/p->h - (p->y-(p-1)->y)/(p-1)->h;	
			
			ch0 = ch1; sh0 = sh1;
		}
				
		p->a = 1/(p-1)->h - omic/sh0;
		p->b = omic*ch0/sh0 - 1/(p-1)->h;
		p->c = 0;
		p->dx = p->dy = 0;
		
		p = p0; 
		
		if( !GraphAPI::GIsEqual2DPoint(&PT_2D(p0->x,p0->y),&PT_2D(p0[nPts2-1].x,p0[nPts2-1].y)) ) 
		{
			p[nPts2-1].dx =0;	
			p[nPts2-1].dy = 0; 
			
			//依次消元
			for( i=0; i<nPts2-1; i++,p++) 
			{ 
				if( fabs(p->b)<1e-10 )p->b = 1e-10;
				
				p->c /= p->b; 
				p->dx /= p->b;	
				p->dy /= p->b; 
				p[1].b -= p[1].a*p->c; 
				p[1].dx -= p[1].a * p->dx; 
				p[1].dy -= p[1].a * p->dy; 
			}
			
			//得解
			if( fabs(p->b)<1e-10 ) p->b = 1e-10;
			p->dx /= p->b;	p->dy /= p->b; 
			
			p = p0 + nPts2-2;
			for( i=nPts2-2; i>0; i--, p--) 
			{
				p->dx -= p->c * p[1].dx; 
				p->dy -= p->c * p[1].dy; 
			} 
		} 
		else	
		{
			p0->a = p0[nPts2-1].a; 
			p0->b = p0->b + p0[nPts2-1].b; 
			p0->dx = p0[nPts2-1].dx = ((p+1)->x-p->x)/p->h - (p->x-(p+nPts2-2)->x)/(p+nPts2-2)->h;
			p0->dy = p0[nPts2-1].dy = ((p+1)->y-p->y)/p->h - (p->y-(p+nPts2-2)->y)/(p+nPts2-2)->h;

			double *pa0 = new double[nPts2];
			double *pc0 = new double[nPts2];
			memset(pa0,0,nPts2*sizeof(double)); 
			memset(pc0,0,nPts2*sizeof(double)); 

			pa0[0] = pc0[0] = p0->a;
			
			double *pa = pa0, *pc = pc0;
			
			for( i=0; i<nPts2-2; i++,p++,pa++,pc++) 
			{ 
				if( fabs(p->b)<1e-10 )p->b = 1e-10;
				
				//归一化当前方程
				p->c /= p->b;	
				p->dx /= p->b;	
				p->dy /= p->b; 
				pa[0] /= p->b; 

				//消下一个方程的a系数
				p[1].b -= p[1].a * p->c; 
				p[1].dx -= p[1].a * p->dx; 
				p[1].dy -= p[1].a * p->dy; 
				pa[1] -= p[1].a * pa[0]; 
				
				//消最后一个方程的c系数（实际上是向右移动）
				p0[nPts2-2].b -= pc[0] * pa[0]; 
				p0[nPts2-2].dx -= pc[0] * p->dx; 
				p0[nPts2-2].dy -= pc[0] * p->dy; 
				pc[1] -= pc[0] * p->c; 
			}
			
			p->dx /= p->b;	p->dy /= p->b; 
			
			p = p0 + nPts2-3;
			pa = pa0 + nPts2-3;
			pc = pc0 + nPts2-3;
			
			for( i=nPts2-2; i>0; i--,p--,pa--) 
			{
				p->dx -= p->c * p[1].dx + pa[0] * p0[nPts2-2].dx; 
				p->dy -= p->c * p[1].dy + pa[0] * p0[nPts2-2].dy; 
			}
			
			p0[nPts2-1].dx = p0->dx; 
			p0[nPts2-1].dy = p0->dy; 

			delete[] pa0; 
			delete[] pc0;
		}
	}
	//绘制曲线
	{
		float dh0 = m_fPrecision*50;
		dh0 = m_fPrecision;
		
		p = p0;
		
		for( i=0 ; i<nPts2-1; i++,p++)
		{
			for(int l=0;l<=p->repeat;l++)
			{
				if (i == 0 && l==0)
				{
					m_pBuf->LinePt(PT_3D(p->x,p->y,p->z),penNone);
				}
				else if( l==0 )
					m_pBuf->LinePt(PT_3D(p->x,p->y,p->z),penSpline);
				else
					m_pBuf->LinePt(p->ppts->GetAt(l-1),penSpline);
			}
			double sh = sinh(omic*p->h), x, y, z;
			double dh = dh0;
			
			if( p->h/dh>50 )
			{
				dh = p->h/50;
			}

			SimpleStreamCompress<PT_3D> comp;
			comp.BeginCompress(m_fPrecision);
			PT_3D tpt;
			double t1, t2;

			// 根据 sinh(x+y) = sinh(x) * cosh(y) + cosh(x) * sinh(y); cosh(x+y) = cosh(x) * cosh(y) + sinh(x) * sinh(y)
			// 优化 t1 = sinh(omic*(p->h-j)); t2 = sinh(omic*j);

			CSinhTool sinh_t;
			sinh_t.Set_h( (omic*p->h) );
			sinh_t.Set_dh( (omic*dh) );

			// dh_div_h -> dh/p->h, j_div_h -> j/p->h
			double dh_div_h = dh/p->h, j_div_h = dh/p->h;
			
			for( double j=dh; j<p->h; j+=dh, sinh_t.Add_dh(), j_div_h += dh_div_h ) 
			{
				//t1 = sinh(omic*(p->h-j)); t2 = sinh(omic*j);
				t1 = sinh_t.Sinh_h_sub_dh(); t2 = sinh_t.Sinh_dh();

				x = 1/sh* ( p->dx*t1 + p[1].dx*t2 ) +
					(p->x-p->dx)*(1.0-j_div_h) + (p[1].x-p[1].dx)*j_div_h;
				
				y = 1/sh* ( p->dy*t1 + p[1].dy*t2 ) +
					(p->y-p->dy)*(1.0-j_div_h) + (p[1].y-p[1].dy)*j_div_h;
				
				z = p[0].z+(p[1].z-p[0].z)*j_div_h; 

				if( comp.AddStreamPt(PT_3D(x,y,z))==2 )
				{
					comp.GetLastCompPt(tpt);
					m_pBuf->LinePt(tpt,penNone); 
				}
				
			}

			if( comp.GetCurCompPt(tpt)>=1 && ((i+1)>=nPts2-1 || !(fabs(tpt.x-(p+1)->x)<1e-6 && fabs(tpt.y-(p+1)->y)<1e-6)) )
			{
				m_pBuf->LinePt(tpt,penNone);				 
			}
			comp.EndCompress();

		}
		
		//确保完整
		for(int l=0;l<=p->repeat;l++)
		{
			if( l==0 )
				m_pBuf->LinePt(PT_3D(p->x,p->y,p->z),penSpline);
			else
				m_pBuf->LinePt(p->ppts->GetAt(l-1),penSpline);
		}
	}

	for( i=0; i<nPts+2; i++)
	{
		delete p0[i].ppts;
	}
	
	delete [] p0;
}	


void CLinearizer::Linearize(const PT_3DEX* expts, int nPts, BOOL bClosed)
{
	if( !m_pBuf || nPts<=0 )return;

	if( nPts==1 )
	{ 
		//绘制点
		m_pBuf->LinePt(expts[0],expts[0].pencode);
		return;
	}

	CArray<PT_3DEX,PT_3DEX> arrPts;

	if( bClosed && nPts>=3 )
	{
		arrPts.SetSize(nPts+1);
		memcpy(arrPts.GetData(),expts,sizeof(PT_3DEX)*nPts);
		PT_3DEX *pts = arrPts.GetData();

		pts[nPts] = pts[0];
		pts[nPts].pencode = pts[nPts-1].pencode;
		nPts++;

		expts = pts;
	}

	CArray<int,int> arrCdChgPos;

	for( int i=1; i<nPts; i++) 
	{
		if( expts[i].pencode!=expts[i-1].pencode )
		{
			arrCdChgPos.Add(i);
		}
	}

	arrCdChgPos.Add(nPts);

	int nCodeNum = arrCdChgPos.GetSize();
	int *pChgPos = arrCdChgPos.GetData();

	double vx0 = 0, vy0 = 0;
	BOOL bUseVX0 = FALSE;

	//共有多少个子对象
	for( int j=0; j<nCodeNum; j++) 
	{
		int i0, i1;
		if( j==0 )
		{
			i0 = 0; i1 = pChgPos[j]; 
		}
		else
		{
			i0 = pChgPos[j-1]-1; i1 = pChgPos[j]; 
		}
		
		switch( expts[i1-1].pencode )
		{
		case penLine: 
		case penStream:
			{
				LineString(expts+i0, i1-i0);
				bUseVX0 = FALSE;
			}
			break; 
		case penSpline: 
			{
				Spline(expts+i0,i1-i0);
				bUseVX0 = FALSE;
			}
			break; 
		case penArc: 
			{
				PT_3DEX cpt; 
				double r,vx,vy,ang[3];
				
				int bclockwise = 0;
				double oldang;
				if( i0==0 )
				{
					if( (i1-i0)<3 ) 
					{
						LineString(expts+i0,(i1-i0));
						break;
					}

					if( CalcArcParamFrom3P(expts+i0,&cpt,&r,ang,&bclockwise) )
					{
						cpt.z = expts[i0].z;

						m_pBuf->LinePt(expts[i0],/*expts[i0].pencode*/penNone);
						Arc(&cpt,r,ang[0],ang[1],expts[i0+1].z-expts[i0].z,bclockwise);

						m_pBuf->LinePt(expts[i0+1],expts[i0+1].pencode);
						
						cpt.z = expts[i0+1].z;
						Arc(&cpt,r,ang[1],ang[2],expts[i0+2].z-expts[i0+1].z,bclockwise);

						m_pBuf->LinePt(expts[i0+2],expts[i0+2].pencode);

						vx = (expts[i0+2].x-cpt.x); vy = (expts[i0+2].y-cpt.y); 
						oldang = ang[2];
					}
					else
					{
						LineString(expts+i0,3);
						vx = (expts[i0+2].y-expts[i0].y); vy = (expts[i0].x-expts[i0+2].x); 

						oldang = GraphAPI::GGetAngle(expts[i0].x,expts[i0].y,expts[i0+2].x,expts[i0+2].y)-PI/2;
						if( oldang<0 )oldang += 2*PI;
					}

					i = i0+2;

				} 
				else
				{
					i = i0; 
					if(bUseVX0)
					{
						vx = vy0; 
						vy = -vx0; 						
					}
					else
					{
						vx = expts[i0].y-expts[i0-1].y; 
						vy = expts[i0-1].x-expts[i0].x; 						
					}

					oldang = GraphAPI::GGetAngle(expts[i0-1].x,expts[i0-1].y,expts[i0].x,expts[i0].y)-PI/2;
					if( oldang<0 )oldang += 2*PI;
				}
				
				for( ; i<i1-1; i++) 
				{ 
					if( CalcArcParamFrom1Tan2P(vx,vy,expts+i,&cpt,&r,ang) )
					{
						if( _FABS(ang[0]-oldang)>PI/2 )bclockwise = 1-bclockwise;
						oldang = ang[1];
						
						cpt.z = expts[i].z;
						Arc(&cpt,r,ang[0],ang[1],expts[i+1].z-expts[i].z,bclockwise);
						
						m_pBuf->LinePt(expts[i+1],expts[i+1].pencode);

						vx = (expts[i+1].x-cpt.x); vy = (expts[i+1].y-cpt.y); 
					}
					else
					{
						m_pBuf->LinePt(expts[i+1],expts[i+1].pencode);
					}

				} 
			}
			break;
		case pen3PArc: 
			{
				double r, ang[3];
				int bclockwise = 0;
				PT_3DEX cpt;
				while (i0+2 < i1)
				{					
					if( CalcArcParamFrom3P(expts+i0,&cpt,&r,ang,&bclockwise) )
					{
						cpt.z = expts[i0].z;
						
						m_pBuf->LinePt(expts[i0],/*expts[i0].pencode*/penNone);
						Arc(&cpt,r,ang[0],ang[1],expts[i0+1].z-expts[i0].z,bclockwise);
						
						m_pBuf->LinePt(expts[i0+1],expts[i0+1].pencode);
						cpt.z = expts[i0+1].z;
						Arc(&cpt,r,ang[1],ang[2],expts[i0+2].z-expts[i0+1].z,bclockwise);
						
						m_pBuf->LinePt(expts[i0+2],expts[i0+2].pencode);

						bUseVX0 = TRUE;
						vx0 =  expts[i0+2].y - cpt.y;
						vy0 = -expts[i0+2].x + cpt.x;
					}
					else
					{						
						LineString(expts+i0,3);
						bUseVX0 = FALSE;
					}

					i0 += 2;
				}

				if( (i1-i0)==2 )
				{
					LineString(expts+i0,2);
					bUseVX0 = FALSE;
				}
			}
			break; 
		case penMove: 
			{
				LineString(expts+i0,i1-i0);
				bUseVX0 = FALSE;
			}
			break;
		} 
	}

	if( bClosed && nPts>=3 )
	{
		m_pBuf->SetLastPtcd(penNone);
	}

	m_pBuf->SetFirstPtcd(expts[0].pencode);
}

int CLinearizer::GetTangency(PT_3D *ipt, PT_3DEX* pts, int nPts,BOOL bClosed, int nPos,PT_3D opt[2])
{
	if( !ipt || !pts || !opt || nPos>=nPts || nPts<0 )
		return 0;

	if( nPts<=2 )return 0;
	
	if( pts[nPos+1].pencode!=penArc )
		return 0;

	//获得圆弧段的起点
	int start = nPos;
	for( ; start>=0; start-- )
	{
		if( pts[start].pencode!=penArc )
			break;
	}
	if( start<0 )start = 0;

	int curpos;
	int bclockwise = 0;
	double oldang,vx,vy,r,ang[3];
	PT_3DEX c;

	if( nPos>=nPts-1 )
		return 0;

	int nret = 0;

	//起点处有直线段，就从直线中计算连续条件参数
	if( start>0 )
	{
		vx=pts[start].y-pts[start-1].y; 
		vy=pts[start-1].x-pts[start].x; 
		oldang = GraphAPI::GGetAngle(pts[start-1].x,pts[start-1].y,pts[start].x,pts[start].y)-PI/2;
		if( oldang<0 )oldang += 2*PI;

		curpos = start;
	}
	else
	{
		//先计算由前三个点构成的圆弧段的尾部连续条件参数
		if( CalcArcParamFrom3P(pts+start,&c,&r,ang,&bclockwise) )
		{
			vx= (pts[start+2].x-c.x); vy= (pts[start+2].y-c.y); 
			oldang = ang[2];
		}
		else
		{
			vx= (pts[start+2].y-pts[start].y); vy= (pts[start].x-pts[start+2].x); 
			oldang = GraphAPI::GGetAngle(pts[start].x,pts[start].y,pts[start+2].x,pts[start+2].y)-PI/2;
			if( oldang<0 )oldang += 2*PI;
		}

		curpos = start+2;

		//要求的切点就在这三个点中
		if( curpos>nPos )
		{
			if( r/sqrt((ipt->x-c.x)*(ipt->x-c.x)+(ipt->y-c.y)*(ipt->y-c.y))>=1.0 )
				return 0;

			//anga是切点与目标点和圆心连线之间的交角
			double anga = acos( r/sqrt((ipt->x-c.x)*(ipt->x-c.x)
				+(ipt->y-c.y)*(ipt->y-c.y)) );
			
			//angc是目标点和圆心连线的角度
			double angc = GraphAPI::GGetAngle(c.x,c.y,ipt->x,ipt->y);
			
			//angt是切点和圆心连线的角度
			double angt = angc-anga;
			if( angt<0 )angt += 2*PI;

			//当切点落在之指定的关键点之间时，角度的旋转方向是一致的，
			//否则，就是落在外面，这样的切点不符合要求，
			if( bclockwise==(GraphAPI::GIsClockwise(ang[0],angt,ang[2])?1:0) )
			{
				opt[nret].x = c.x + cos(angt)*r;
				opt[nret].y = c.y + sin(angt)*r;
				opt[nret].z = pts[nPos].z;
				nret++;
			}

			angt = angc+anga;
			if( angt>=2*PI )angt -= 2*PI;

			//当切点落在之指定的关键点之间时，角度的旋转方向是一致的，
			//否则，就是落在外面，这样的切点不符合要求，
			if( bclockwise==(GraphAPI::GIsClockwise(ang[0],angt,ang[2])?1:0) )
			{
				opt[nret].x = c.x + cos(angt)*r;
				opt[nret].y = c.y + sin(angt)*r;
				opt[nret].z = pts[nPos].z;
				nret++;
			}

			return nret;
		}
	}

	if( curpos<=nPos )
	{
		//计算完前面所有的圆弧段，直到当前点所在的圆弧段为止
		for( int i=curpos; i<=nPos; i++) 
		{			
			if( CalcArcParamFrom1Tan2P(vx,vy,pts+i,&c,&r,ang) )
			{
				if( _FABS(ang[0]-oldang)>PI/2 )bclockwise = 1-bclockwise;
				oldang = ang[1];
				
				vx = (pts[i+1].x-c.x); vy = (pts[i+1].y-c.y); 
			}
		}

		if( r/sqrt((ipt->x-c.x)*(ipt->x-c.x)+(ipt->y-c.y)*(ipt->y-c.y))>=1.0 )
			return 0;

		double anga = acos( r/sqrt((ipt->x-c.x)*(ipt->x-c.x)
			+(ipt->y-c.y)*(ipt->y-c.y)) );
		double angc = GraphAPI::GGetAngle(c.x,c.y,ipt->x,ipt->y);
		double angt = angc-anga;
		if( angt<0 )angt += 2*PI;
		
		//当切点落在之指定的关键点之间时，角度的旋转方向是一致的，
		//否则，就是落在外面，这样的切点不符合要求，
		if( bclockwise==(GraphAPI::GIsClockwise(ang[0],angt,ang[1])?1:0) )
		{
			opt[nret].x = c.x + cos(angt)*r;
			opt[nret].y = c.y + sin(angt)*r;
			opt[nret].z = pts[nPos].z;
			nret++;
		}
		
		angt = angc+anga;
		if( angt>=2*PI )angt -= 2*PI;
		
		//当切点落在之指定的关键点之间时，角度的旋转方向是一致的，
		//否则，就是落在外面，这样的切点不符合要求，
		if( bclockwise==(GraphAPI::GIsClockwise(ang[0],angt,ang[1])?1:0) )
		{
			opt[nret].x = c.x + cos(angt)*r;
			opt[nret].y = c.y + sin(angt)*r;
			opt[nret].z = pts[nPos].z;
			nret++;
		}
	}

	return nret;
}

BOOL CLinearizer::GetCenter(PT_3DEX* pts,int nPts,BOOL bClosed, int nPos,PT_3D *opt)
{
	if( !pts || !opt || nPos>=nPts || nPts<0 )
		return FALSE;

	if( nPts<=2 )return FALSE;
	
	if( pts[nPos+1].pencode!=penArc )
		return FALSE;

	//获得圆弧段的起点
	int start = nPos;
	for( ; start>=0; start-- )
	{
		if( pts[start].pencode!=penArc )
			break;
	}
	if( start<0 )start = 0;

	int curpos;
	int bclockwise = 0;
	double oldang,vx,vy,r,ang[3];
	PT_3DEX c;

	if( nPos>=nPts-1 )
		return FALSE;

	//起点处有直线段，就从直线中计算连续条件参数
	if( start>0 )
	{
		vx = pts[start].y-pts[start-1].y; 
		vy = pts[start-1].x-pts[start].x; 
		oldang = GraphAPI::GGetAngle(pts[start-1].x,pts[start-1].y,pts[start].x,pts[start].y)-PI/2;
		if( oldang<0 )oldang += 2*PI;

		curpos = start;
	}
	else
	{
		//先计算由前三个点构成的圆弧段的尾部连续条件参数
		
		if( CalcArcParamFrom3P(pts+start,&c,&r,ang,&bclockwise) )
		{
			vx = (pts[start+2].x-c.x); vy = (pts[start+2].y-c.y); 
			oldang = ang[2];
		}
		else
		{
			vx = (pts[start+2].y-pts[start].y); vy = (pts[start].x-pts[start+2].x); 
			oldang = GraphAPI::GGetAngle(pts[start].x,pts[start].y,pts[start+2].x,pts[start+2].y)-PI/2;
			if( oldang<0 )oldang += 2*PI;
		}

		curpos = start+2;

		//要求的切点就在这三个点中
		if( curpos>nPos )
		{
			opt->x = c.x;	opt->y = c.y;	opt->z = pts[nPos].z;
			return TRUE;
		}
	}

	if( curpos<=nPos )
	{
		//计算完前面所有的圆弧段，直到当前点所在的圆弧段为止
		for( int i=curpos; i<=nPos; i++) 
		{
			if( CalcArcParamFrom1Tan2P(vx,vy,pts+i,&c,&r,ang) )
			{
				if( _FABS(ang[0]-oldang)>PI/2 )bclockwise = 1-bclockwise;
				oldang = ang[1];
				
				vx= (pts[i+1].x-c.x); vy= (pts[i+1].y-c.y); 
			}
		}

		opt->x = c.x;	opt->y = c.y;	opt->z = pts[nPos].z;
	}

	return TRUE;
}


int CLinearizer::FindKeyPosOfBaseLine(PT_3D sch_pt, const CShapeLine *pBaseLine, CCoordSys *pCS, int *state)
{
	Envelope e;
	e.CreateMaxEnvelope();

	PT_3D ptret1, ptret2;
	double dis;
	int nKeyPos;
	if( !pBaseLine->FindNearestLine(sch_pt,e,pCS,&ptret1,&ptret2,&dis,&nKeyPos) )
		return -1;

	CArray<PT_3DEX,PT_3DEX> arrPts;
	pBaseLine->GetKeyPts(arrPts);

	PT_3D pt1 = arrPts.GetAt(nKeyPos), pt2 = arrPts.GetAt(nKeyPos+1);
	pCS->GroundToClient(&pt1,&ptret1);
	pCS->GroundToClient(&pt2,&ptret2);
	pt1 = ptret1;
	pt2 = ptret2;

	*state = GraphAPI::GGetPerpPointPosinLine(pt1.x,pt1.y,pt2.x,pt2.y,sch_pt.x,sch_pt.y);

	return nKeyPos;
	return 0;
}


MyNameSpaceEnd