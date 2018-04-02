// GrTrim.cpp: implementation of the CGrTrim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "featurecollector.h"
#include "GrTrim.h"
#include "SmartViewFunctions.h"
#include "float.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGrTrim::CGrTrim()
{
	m_pPolyPts = NULL;
	m_nPolyPtsNum = 0;

	m_pTrimedLines = NULL;
	m_nTrimedLinesNum = m_nTrimedLinesMemLen = 0;

	m_pBlocks = NULL;
	m_nBlocksNum = m_nBlocksMemLen = 0;

	m_bSimplyTrim = FALSE;

	m_bIntersectHeight = FALSE;

	m_pCurBlock = NULL;
}

CGrTrim::~CGrTrim()
{
	if( m_pPolyPts )delete[] m_pPolyPts;
	if( m_pTrimedLines )delete[] m_pTrimedLines;
	if( m_pBlocks )delete[] m_pBlocks;
}


void CGrTrim::InitTrimPolygon(PT_3D *pts, int num, double wid)
{
	if( !pts || num<=0 || wid<=0 )return;
	if( m_pPolyPts )delete[] m_pPolyPts;
	m_nPolyPtsNum = 0;
	m_arrSubPolyPtsNum.RemoveAll();

	//拷贝数据
	m_pPolyPts = new PT_3D[num];
	if( !m_pPolyPts )return;

	memcpy(m_pPolyPts,pts,sizeof(PT_3D)*num);
	m_nPolyPtsNum = num;
	m_arrSubPolyPtsNum.Add(m_nPolyPtsNum);

	//-----------------分块
	//重置
	ClearBlocks();

	//计算分块起点、终点
	double ymin=0, ymax=0, xlim1, xlim2;
	PT_3D *tpt = pts;
	for( int i=0; i<num; i++,tpt++)
	{
		if( i==0 ){ ymin = ymax = tpt->y; xlim1 = xlim2 = tpt->x; }
		else
		{
			if( ymin>tpt->y )ymin = tpt->y;
			if( ymax<tpt->y )ymax = tpt->y;
			if( xlim1>tpt->x )xlim1 = tpt->x;
			if( xlim2<tpt->x )xlim2 = tpt->x;
		}
	}

	xlim1 -= 10;  xlim2 += 10;

	if( (ymax-ymin)/wid>m_nPolyPtsNum )
	{
		return;
	}

	//裁减分块线，并依次生成矩形块
	double y = ymin+1e-6, xmin,xmax;
	PT_3D pt0(0,0,0), pt1(0,0,0);
	TRIM_LINE *pTrim1=NULL, *pTrim2=NULL;
	int nTrim1=0, nTrim2=0;
	for( ; y<ymax; y+=wid )
	{
		//裁减
		pt0.y = y; pt0.x = xlim1; pt1.y = y; pt1.x = xlim2; 
		if( !TrimLine(&pt0, &pt1) )continue;
		
		//得到裁减数据
		if( !pTrim1 )
		{
			if( m_nTrimedLinesNum<=0 )continue;
			pTrim1 = new TRIM_LINE[m_nTrimedLinesNum];
			if( !pTrim1 )continue;
			memcpy(pTrim1,m_pTrimedLines,sizeof(TRIM_LINE)*m_nTrimedLinesNum);
			nTrim1 = m_nTrimedLinesNum;
			continue;
		}
		else
		{
			if( pTrim2 )
			{
				delete[] pTrim1;
				pTrim1 = pTrim2;
				nTrim1 = nTrim2;
				pTrim2 = NULL;
				nTrim2 = 0;
			}
			
			if( m_nTrimedLinesNum<=0 )continue;
			pTrim2 = new TRIM_LINE[m_nTrimedLinesNum];
			if( !pTrim2 )continue;

			memcpy(pTrim2,m_pTrimedLines,sizeof(TRIM_LINE)*m_nTrimedLinesNum);
			nTrim2 = m_nTrimedLinesNum;
		}

		//和前段裁减数据比较，生成矩形块
		for( i=0; i<nTrim1; i++)
		{
			for( int j=0; j<nTrim2; j++)
			{
				if( pTrim1[i].pt1.x<=pTrim2[j].pt0.x ||
					pTrim1[i].pt0.x>=pTrim2[j].pt1.x )
					continue;

				xmin = pTrim1[i].pt0.x>pTrim2[j].pt0.x?pTrim1[i].pt0.x:pTrim2[j].pt0.x;
				xmax = pTrim1[i].pt1.x<pTrim2[j].pt1.x?pTrim1[i].pt1.x:pTrim2[j].pt1.x;
				AddBlock(xmin,xmax,y-wid,y);
			}
		}
	}

	if( pTrim1 )delete[] pTrim1;
	if( pTrim2 )delete[] pTrim2;

}

void CGrTrim::InitTrimPolygons(PT_3DEX *pts, int num, double wid)
{
	if( !pts || num<=0 || wid<=0 )return;
	if( m_pPolyPts )delete[] m_pPolyPts;
	m_nPolyPtsNum = 0;
	m_arrSubPolyPtsNum.RemoveAll();

	//拷贝数据
	m_pPolyPts = new PT_3D[num];
	if( !m_pPolyPts )return;

	int pos0 = 0;
	for(int i=0; i<num; i++)
	{
		m_pPolyPts[i] = pts[i];
		if(i!=0 && pts[i].pencode==penMove)
		{
			m_arrSubPolyPtsNum.Add(i-pos0);
			pos0 = i;
		}
	}	
	m_arrSubPolyPtsNum.Add(i-pos0);
	m_nPolyPtsNum = num;

	//-----------------分块
	//重置
	ClearBlocks();

	//计算分块起点、终点
	double ymin=0, ymax=0, xlim1, xlim2;
	PT_3DEX *tpt = pts;
	for( i=0; i<num; i++,tpt++)
	{
		if( i==0 ){ ymin = ymax = tpt->y; xlim1 = xlim2 = tpt->x; }
		else
		{
			if( ymin>tpt->y )ymin = tpt->y;
			if( ymax<tpt->y )ymax = tpt->y;
			if( xlim1>tpt->x )xlim1 = tpt->x;
			if( xlim2<tpt->x )xlim2 = tpt->x;
		}
	}

	xlim1 -= 10;  xlim2 += 10;

	//划分的块的数目超过顶点数目，分块优化就没有多大意义了
	if( (ymax-ymin)/wid>m_nPolyPtsNum )
	{
		return;
	}

	//裁减分块线，并依次生成矩形块
	double y = ymin+1e-6, xmin,xmax;
	PT_3D pt0(0,0,0), pt1(0,0,0);
	TRIM_LINE *pTrim1=NULL, *pTrim2=NULL;
	int nTrim1=0, nTrim2=0;
	for( ; y<ymax; y+=wid )
	{
		//裁减
		pt0.y = y; pt0.x = xlim1; pt1.y = y; pt1.x = xlim2; 
		if( !TrimLine(&pt0, &pt1) )continue;
		
		//得到裁减数据
		if( !pTrim1 )
		{
			if( m_nTrimedLinesNum<=0 )continue;
			pTrim1 = new TRIM_LINE[m_nTrimedLinesNum];
			if( !pTrim1 )continue;
			memcpy(pTrim1,m_pTrimedLines,sizeof(TRIM_LINE)*m_nTrimedLinesNum);
			nTrim1 = m_nTrimedLinesNum;
			continue;
		}
		else
		{
			if( pTrim2 )
			{
				delete[] pTrim1;
				pTrim1 = pTrim2;
				nTrim1 = nTrim2;
				pTrim2 = NULL;
				nTrim2 = 0;
			}
			
			if( m_nTrimedLinesNum<=0 )continue;
			pTrim2 = new TRIM_LINE[m_nTrimedLinesNum];
			if( !pTrim2 )continue;

			memcpy(pTrim2,m_pTrimedLines,sizeof(TRIM_LINE)*m_nTrimedLinesNum);
			nTrim2 = m_nTrimedLinesNum;
		}

		//和前段裁减数据比较，生成矩形块
		for( i=0; i<nTrim1; i++)
		{
			for( int j=0; j<nTrim2; j++)
			{
				if( pTrim1[i].pt1.x<=pTrim2[j].pt0.x ||
					pTrim1[i].pt0.x>=pTrim2[j].pt1.x )
					continue;

				xmin = pTrim1[i].pt0.x>pTrim2[j].pt0.x?pTrim1[i].pt0.x:pTrim2[j].pt0.x;
				xmax = pTrim1[i].pt1.x<pTrim2[j].pt1.x?pTrim1[i].pt1.x:pTrim2[j].pt1.x;
				AddBlock(xmin,xmax,y-wid,y);
			}
		}
	}

	if( pTrim1 )delete[] pTrim1;
	if( pTrim2 )delete[] pTrim2;

}


int CGrTrim::GetSubPolygonStartPos(int idx)
{
	if(idx<0 || idx>=m_arrSubPolyPtsNum.GetSize())
		return -1;

	int pos = 0;
	for(int i=0; i<idx; i++)
	{
		pos += m_arrSubPolyPtsNum[i];
	}

	return pos;
}


int CGrTrim::GetSubPolygonCount()
{
	return m_arrSubPolyPtsNum.GetSize();
}

BOOL CGrTrim::Trim(const GrBuffer *buf, GrBuffer& ret)
{
	if( !buf || m_nPolyPtsNum<=0 || !m_pPolyPts )
		return FALSE;
	
	Envelope e = buf->GetEnvelope();
	
	if( bPtInPolygon(&PT_3D(e.m_xl,e.m_yl,0)) && 
		bPtInPolygon(&PT_3D(e.m_xl,e.m_yh,0)) && 
		bPtInPolygon(&PT_3D(e.m_xh,e.m_yl,0)) && 
		bPtInPolygon(&PT_3D(e.m_xh,e.m_yh,0)) )
	{
		ret.CopyFrom(buf);
		return TRUE;
	}
	
	if( m_bSimplyTrim )return FALSE;

	if( !buf || m_nPolyPtsNum<=0 || !m_pPolyPts )
		return FALSE;

	
	GrBuffer buf1;
	const_cast<GrBuffer*>(buf)->GetAllGraph(&buf1);
	const_cast<GrBuffer*>(buf)->CopyFrom(&buf1);
	//裁减
	Graph *pu = const_cast<Graph*>(buf->HeadGraph());
	
	while(pu) 
	{
		//优先判断是否在矩形块中
		e = GetEnvelopeOfGraph(pu);
		if( bPtInBlocks(e.m_xl,e.m_yl) && 
 			bPtInBlocks(e.m_xl,e.m_yh) && 
 			bPtInBlocks(e.m_xh,e.m_yl) && 
			bPtInBlocks(e.m_xh,e.m_yh) )
		{
			ret.InsertGraph(pu);		
		}
		else if( IsGrLineString(pu) )
		{
			GrVertex *pts = NULL, *pt1, *pt2;
			// 对线作裁剪
			int npt = const_cast<GrBuffer*>(buf)->GetGraphPts(pu, pts);
			for( int i=1; i<npt; i++, pts++)
			{
				pt2 = pts+1;
				if( (pt2->code&GRBUFFER_PTCODE_LINETO)==0 )continue;
				pt1 = pt2-1;
				
				//优先判断是否在矩形块中
				if( bPtInBlocks(pt1->x,pt1->y) && 
					bPtInBlocks(pt2->x,pt2->y) )
				{
					GrLineString *pLine = (GrLineString*)pu;
					ret.BeginLineString(pLine->color, pLine->width, pLine->bGrdWid, pLine->style);
					ret.MoveTo(pt1);
					ret.LineTo(pt2);
					ret.End();
				}

				//裁减
				else
				{
					TrimLine(pt1, pt2);
					for( int j=0; j<m_nTrimedLinesNum; j++)
					{
						GrLineString *pLine = (GrLineString*)pu;
						ret.BeginLineString(pLine->color, pLine->width, pLine->bGrdWid, pLine->style);
						ret.MoveTo(&m_pTrimedLines[j].pt0);
						ret.LineTo(&m_pTrimedLines[j].pt1);
						ret.End();
					}
				}
			}			
		}
		else if( IsGrPointString(pu) )
		{
			GrPointString *ps = (GrPointString*)pu;
			
			ret.BeginPointString(ps->color,ps->kx,ps->ky,ps->bGrdSize,ps->cell,ps->width,ps->bFlat,ps->bUseSelfcolor,ps->bUseBackColor);
			
			GrVertex *pts = ps->ptlist.pts;
			for(int i=0; i<ps->ptlist.nuse; i++,pts++)
			{
				if( bPtInBlocks(pts->x,pts->y) )
					ret.PointString(pts,pts->CodeToFloat());
			}
			
			ret.End();
		}
		else if( IsGrPointStringEx(pu) )
		{
			GrPointStringEx *ps = (GrPointStringEx*)pu;
			
			ret.BeginPointStringEx(ps->color,ps->kx,ps->ky,ps->bGrdSize,ps->cell,ps->width,ps->bFlat,ps->bUseSelfcolor,ps->bUseBackColor);
			
			GrVertex *pts = ps->ptlist.pts;
			for(int i=0; i<ps->ptlist.nuse; i++,pts++)
			{
				if( bPtInBlocks(pts->x,pts->y) )
					ret.PointStringEx(pts,ps->attrList.pts[i].color,ps->attrList.pts[i].isMarked);
			}
			
			ret.End();
		}

		pu = pu->next;
	}

	ret.KickoffSamePoints();

	return TRUE;
}



BOOL CGrTrim::Trim_notBlockOptimized(const GrBuffer *buf, GrBuffer& ret)
{
	if( !buf || m_nPolyPtsNum<=0 || !m_pPolyPts )
		return FALSE;
			
	if( m_bSimplyTrim )return FALSE;
	
	if( !buf || m_nPolyPtsNum<=0 || !m_pPolyPts )
		return FALSE;	
	
	GrBuffer buf1;
	const_cast<GrBuffer*>(buf)->GetAllGraph(&buf1);
	//裁减
	Graph *pu =buf1.HeadGraph();
	
	while(pu) 
	{
		if (IsGrLineString(pu))
		{
			GrVertex *pts = NULL, *pt1, *pt2;
			// 对线作裁剪
			int npt = buf1.GetGraphPts(pu, pts);

			GrLineString *pLine = (GrLineString*)pu;
			ret.BeginLineString(pLine->color, pLine->width, pLine->bGrdWid, pLine->style);

			for( int i=1; i<npt; i++, pts++)
			{
				pt2 = pts+1;
				if( (pt2->code&GRBUFFER_PTCODE_LINETO)==0 )continue;
				pt1 = pt2-1;
				
				TrimLine_notTypeRet(pt1, pt2);
				for( int j=0; j<m_nTrimedLinesNum; j++)
				{
					ret.MoveTo(&m_pTrimedLines[j].pt0);
					ret.LineTo(&m_pTrimedLines[j].pt1);					
				}
			}

			ret.End();			
		}
		else
		{
			Envelope e = GetEnvelopeOfGraph(pu);
			if( bEnvelopeInPolygon(e)==1 )
			{
				ret.InsertGraph(pu,FALSE);
			}
		}
		
		pu = pu->next;
	}
	
	ret.KickoffSamePoints();
	
	return TRUE;
	
}

CGrTrim::IS_RET::IS_RET()
{
	t = 0;
}


CGrTrim::IS_RET::IS_RET(PT_3D a, double b)
{
	pt = a;
	t = b;
}

BOOL CGrTrim::TrimLine_notTypeRet(const PT_3D *pt0, const PT_3D *pt1)
{
	if( !pt0 || !pt1 || m_nPolyPtsNum<=0 || !m_pPolyPts )return FALSE;
	
	ClearTrimedLines();	
	
	//节省内存: 相交的点数不会超过4个
	CArray<IS_RET,IS_RET> rets;
	rets.Add(IS_RET(*pt0,0));
	
	PT_3D ret;
	double t,t1;
	int i,j,i1;
	//依次求出相交信息，type0 记录目标线段是否头尾正好都在区域边界线上
	int nSubPolygon = GetSubPolygonCount();
	for( j=0; j<nSubPolygon; j++)
	{
		int nPolyPtsNum = m_arrSubPolyPtsNum[j];
		int start = GetSubPolygonStartPos(j);
		for( i=0; i<nPolyPtsNum; i++ )
		{
			PT_3D *pPolyPts = m_pPolyPts + start;
			i1 = (i+1)%nPolyPtsNum;
			if( !Intersect(pt0,pt1,pPolyPts+i,pPolyPts+i1,&ret,&t,&t1) )
				continue;
			
			if (m_bIntersectHeight)
			{
				ret.z = (pPolyPts+i)->z + t1*((pPolyPts+i1)->z-(pPolyPts+i)->z);
			}
			//if( fabs(t)<1e-6 || fabs(1-t)<1e-6 )continue;

			rets.Add(IS_RET(ret,t));
		}		
	}
	
	rets.Add(IS_RET(*pt1,1.0));
	
	//对相交点做排序（冒泡法），以此可以方便地构成裁减线段
	//这里只取了中间的点(0<t<1)，首尾点(t=0,t=1)位置正确，不需要排序
	for( i=1; i<rets.GetSize()-1; i++)
	{
		//找出最小者
		int bChange = 0;
		for( int j=rets.GetSize()-2; j>i; j--)
		{
			//交换
			if( rets[j].t<rets[j-1].t )
			{ 
				ret = rets[j].pt; t =  rets[j].t;
				rets[j].pt = rets[j-1].pt; rets[j].t =  rets[j-1].t;
				rets[j-1].pt = ret; rets[j-1].t = t;

				bChange = 1;
			}
		}

		if( bChange==0 )
		{
			break;
		}
	}
	
	ret.x = (rets[0].pt.x+rets[1].pt.x)/2; ret.y = (rets[0].pt.y+rets[1].pt.y)/2; ret.z = 0;
	
	//判断起点在polygon中的方位
	BOOL bStartIn = bPtInPolygon(&ret);
		
	for( i=bStartIn?0:1; i<rets.GetSize()-1; i+=2 )
	{		
		AddTrimedLine(&rets[i].pt,&rets[i+1].pt,0);
	}
	
	return TRUE;
}

BOOL CGrTrim::TrimLine(const PT_3D *pt0, const PT_3D *pt1)
{
	if( !pt0 || !pt1 || m_nPolyPtsNum<=0 || !m_pPolyPts )return FALSE;

	ClearTrimedLines();

	CArray<IS_RET,IS_RET> rets;
	rets.Add(IS_RET(*pt0,0));

	BOOL bEndOn1 = FALSE, bEndOn2 = FALSE;

	PT_3D ret;
	double t,t1;
	int i,j,i1;
	//依次求出相交信息，type0 记录目标线段是否头尾正好都在区域边界线上
	int nSubPolygon = GetSubPolygonCount();
	for( j=0; j<nSubPolygon; j++)
	{
		int nPolyPtsNum = m_arrSubPolyPtsNum[j];
		int start = GetSubPolygonStartPos(j);
		for( i=0; i<nPolyPtsNum; i++ )
		{
			PT_3D *pPolyPts = m_pPolyPts + start;
			i1 = (i+1)%nPolyPtsNum;
			if( !Intersect(pt0,pt1,pPolyPts+i,pPolyPts+i1,&ret,&t,&t1) )
				continue;
			
			if (m_bIntersectHeight)
			{
				ret.z = (pPolyPts+i)->z + t1*((pPolyPts+i1)->z-(pPolyPts+i)->z);
			}		
			
			if( fabs(t)<1e-10 ) 
			{
				bEndOn1 = TRUE;
				continue;
			}
			else if ( fabs(1-t)<1e-10 )
			{
				bEndOn2 = TRUE;
				continue;
			}
			//if( fabs(t)<1e-6 || fabs(1-t)<1e-6 )continue;

			rets.Add(IS_RET(ret,t));
		}		
	}	
	
	rets.Add(IS_RET(*pt1,1.0));
	
	//对相交点做排序（冒泡法），以此可以方便地构成裁减线段
	//这里只取了中间的点(0<t<1)，首尾点(t=0,t=1)位置正确，不需要排序
	for( i=1; i<rets.GetSize()-1; i++)
	{
		//找出最小者
		int bChange = 0;
		for( int j=rets.GetSize()-2; j>i; j--)
		{
			//交换
			if( rets[j].t<rets[j-1].t )
			{ 
				ret = rets[j].pt; t =  rets[j].t;
				rets[j].pt = rets[j-1].pt; rets[j].t =  rets[j-1].t;
				rets[j-1].pt = ret; rets[j-1].t = t;
				
				bChange = 1;
			}
		}
		
		if( bChange==0 )
		{
			break;
		}
	}

	ret.x = (rets[0].pt.x+rets[1].pt.x)/2; ret.y = (rets[0].pt.y+rets[1].pt.y)/2; ret.z = 0;

	//判断起点在polygon中的方位
	BOOL bStartIn = bPtInPolygon(&ret);

	//判断首尾端点在polygon中的方位
	BOOL bPtIn0 = bPtInPolygon(pt0)&&!bEndOn1, bPtIn1 = bPtInPolygon(pt1)&&!bEndOn2;

	for( i=(bStartIn||bEndOn1)?0:1; i<rets.GetSize()-1; i+=2 )
	{
		int type = 0;

		if( fabs(rets[i].t)<1e-10 )
			type |= (bPtIn0?TL_ENDIN1:TL_ENDON1);
		else if( rets[i].t>0 && rets[i].t<1 )
			type |= TL_MID1;
		
		if( fabs(1-rets[i+1].t)<1e-10 )
			type |= (bPtIn1?TL_ENDIN2:TL_ENDON2);
		else if( rets[i+1].t>0 && rets[i+1].t<1 )
			type |= TL_MID2;

		AddTrimedLine(&rets[i].pt,&rets[i+1].pt,type);
	}

	if (i == rets.GetSize()-1 && bEndOn2)
	{
		int type = 0;
		type |= TL_ENDON1;
		type |= TL_ENDON2;
		
		AddTrimedLine(&rets[i].pt,&rets[i].pt,type);
	}

	return TRUE;
}

static int sign_compare(double x, double toler)
{
	if (fabs(x)<toler)
	{
		return 0;
	}
	else 
		return  x>0?1:-1;
}

static int  static_IsPtInRegion(PT_3D pt0, PT_3D *pPts,int nNum, double toler )
{
	int    i,j ,  sum=0;   
    double    xmin,   ymin,   xmax,   ymax;   
    double   x;   

	PT_3D* pPt = &pt0;
	
	//检验传入参数的合法性
  	if (pPts==NULL||pPt==NULL||nNum<3)
	{
		return -2;
	}	
    for(i=0;   i<nNum;   i++)   
    {
		//j=(i+1)%nNum;
		if (i==nNum-1)
		{
			j=0;
		}
		else
			j=(i+1);		
		xmin=min(pPts[i].x,pPts[j].x);   
		xmax=max(pPts[i].x,pPts[j].x);   
		ymin=min(pPts[i].y,pPts[j].y);   
		ymax=max(pPts[i].y,pPts[j].y); 
		
		//水平射线不可能与轮廓线段i->j有交点 
		if(pPt->y>ymax+toler||pPt->y<ymin-toler||pPt->x>xmax+toler)   
			continue;   

		//可能经过线段i->j某个顶点
		if(fabs(pPt->y-pPts[i].y)<toler||fabs(pPt->y-pPts[j].y)<toler)
		{   
			//线段平行 
			if(fabs(pPts[i].y-pPts[j].y)<toler)//此处不能用ymin，ymax  
			{
				// 检验点在线段端点上
				if (fabs(pPt->x-xmin)<toler||fabs(pPt->x-xmax)<toler)
				{
					return 0;
				}
				// 检验点在线段上
				else if (pPt->x>xmin&&pPt->x<xmax)
				{
					return 1;
				}
				// 其他情况直接进入下次循环
				else
					continue;
			}
			// 线段不平行
			else 
			{   
				// 经过i对应的点
				if (fabs(pPt->y-pPts[i].y)<toler)
				{
					// 检验点与i对应的点重合
					if (fabs(pPt->x-pPts[i].x)<toler)
					{
						return 0;
					}
					// 水平射线过i点
					else if(pPt->x<pPts[i].x)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,toler)+sign_compare(pPts[j].y-pPt->y,toler);
						continue;
					}
					else					
						continue;
				}
				// 经过j对应的点
				else 
				{
					if (fabs(pPt->x-pPts[j].x)<toler)
					{
						return 0;
					}
					else if(pPt->x<pPts[j].x)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,toler)+sign_compare(pPts[j].y-pPt->y,toler);
						continue;
					}
					else					
						continue;
				}				  
			}   			   
		}
		//不过i->j端点，且可能与i->j相交
	    else
		{
			//此判断应该可以不要
			if ( pPt->y>ymin&&pPt->y<ymax)
			{
				// 首先排除必然相交的情况
				if (pPt->x<xmin-toler)
				{
					sum+=sign_compare(pPt->y-pPts[i].y,toler)+sign_compare(pPts[j].y-pPt->y,toler);
					continue;
				}
				// 无法直接判断的情况最后经过计算判断
				else
				{
					x=(double)(pPt->y-pPts[i].y)/(pPts[j].y-pPts[i].y)*(pPts[j].x-pPts[i].x)+pPts[i].x;
					
					//检验点在线段上
					if(fabs(x-pPt->x)<toler)
						return (1);

					//检验点与线段相交
					if(x>pPt->x)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,toler)+sign_compare(pPts[j].y-pPt->y,toler);
						continue;
					}
				}
			}
		}
    }  
    if((sum%4)==0)   return(-1);   //   p在多边形外   
    else        return(2);     //   p在多边形内 
}


BOOL CGrTrim::bPtInPolygon(const PT_3D *pt)
{
	if( bPtInBlocks(pt->x,pt->y) )
		return TRUE;

	int count = 0;
	int nSubPolygon = GetSubPolygonCount();
	for( int j=0; j<nSubPolygon; j++)
	{
		int nPolyPtsNum = m_arrSubPolyPtsNum[j];
		int start = GetSubPolygonStartPos(j);
		PT_3D *pPolyPts = m_pPolyPts + start;
		if(static_IsPtInRegion(*pt, pPolyPts, nPolyPtsNum,1e-10)==2)
		{
			count++;
		}
	}

	return ((count%2)==1);
}

BOOL CGrTrim::bPtInPolygon(const PT_3D *pt,double radius)
{
	if( bPtInBlocks(pt->x,pt->y,radius) )
		return TRUE;

	int count = 0;
	int nSubPolygon = GetSubPolygonCount();
	for( int j=0; j<nSubPolygon; j++)
	{
		int nPolyPtsNum = m_arrSubPolyPtsNum[j];
		int start = GetSubPolygonStartPos(j);
		PT_3D *pPolyPts = m_pPolyPts + start;
		if(static_IsPtInRegion(*pt, pPolyPts, nPolyPtsNum,1e-10)==2)
		{
			count++;
		}
	}

	return ((count%2)==1);	
}


int CGrTrim::bEnvelopeInPolygon(const Envelope& e)
{
	if( m_nPolyPtsNum<=0 || !m_pPolyPts )
		return 0;
	
	int num = 0;

	if( bPtInPolygon(&PT_3D(e.m_xl,e.m_yl,0)) )
		num++;

	if( bPtInPolygon(&PT_3D(e.m_xl,e.m_yh,0)) )
		num++;

	if( num>0 && num<2 )
		return 2;

	if( bPtInPolygon(&PT_3D(e.m_xh,e.m_yl,0)) )
		num++;

	if( num>0 && num<3 )
		return 2;

	if( bPtInPolygon(&PT_3D(e.m_xh,e.m_yh,0)) )
		num++;
	
	if( num==4 )
	{
		return 1;
	}
	if( num==0 )
		return 0;

	return 2;
}

BOOL CGrTrim::Intersect(const PT_3D *pt0, const PT_3D *pt1, const PT_3D *pt2, const PT_3D *pt3,
			   PT_3D *ret, double *t, double *t23)
{
	//为了优化，先判断一下
	double xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;
	
	if( pt0->x<pt1->x ){ xmin1 = pt0->x; xmax1 = pt1->x; }
	else { xmin1 = pt1->x; xmax1 = pt0->x; }
	if( pt0->y<pt1->y ){ ymin1 = pt0->y; ymax1 = pt1->y; }
	else { ymin1 = pt1->y; ymax1 = pt0->y; }

	if( pt2->x<pt3->x ){ xmin2 = pt2->x; xmax2 = pt3->x; }
	else { xmin2 = pt3->x; xmax2 = pt2->x; }
	if( pt2->y<pt3->y ){ ymin2 = pt2->y; ymax2 = pt3->y; }
	else { ymin2 = pt3->y; ymax2 = pt2->y; }

	if( xmax1<xmin2 || xmax2<xmin1 || 
		ymax1<ymin2 || ymax2<ymin1 )
		return FALSE;

	//求交
	double vector1x = pt1->x-pt0->x, vector1y = pt1->y-pt0->y;
	double vector2x = pt3->x-pt2->x, vector2y = pt3->y-pt2->y;
	
	double delta = vector1x*vector2y-vector1y*vector2x;
	if( delta<1e-10 && delta>-1e-10 )return FALSE;
	
	double t1 = ( (pt2->x-pt0->x)*vector2y-(pt2->y-pt0->y)*vector2x )/delta;
	if( t1<0.0 || t1>1.0 )return FALSE; 
	double t2 = ( (pt2->x-pt0->x)*vector1y-(pt2->y-pt0->y)*vector1x )/delta;
	if( t2<0.0 || t2>1.0 )return FALSE; 
	ret->x = pt0->x + t1*vector1x;
	ret->y = pt0->y + t1*vector1y;
	ret->z = pt0->z + t1*(pt1->z-pt0->z);
	*t = t1;
	if (t23)
	{
		*t23 = t2;
	}
	return TRUE;
}

int	 CGrTrim::GetTrimedLinesNum()
{
	return m_nTrimedLinesNum;
}


int CGrTrim::GetTrimedLine(int idx, PT_3D *pt0, PT_3D *pt1)
{
	if( idx>=0 && idx<m_nTrimedLinesNum )
	{
		*pt0 = m_pTrimedLines[idx].pt0;
		*pt1 = m_pTrimedLines[idx].pt1;
		return m_pTrimedLines[idx].type;
	}
	return 0;
}

void CGrTrim::AddTrimedLine(PT_3D *pt0, PT_3D *pt1, int type)
{
	if( !m_pTrimedLines || m_nTrimedLinesNum>=m_nTrimedLinesMemLen )
	{
		int newlen = m_nTrimedLinesMemLen+8;
		TRIM_LINE *pLines = new TRIM_LINE[newlen];
		if( !pLines )return;

		if( m_pTrimedLines )
		{
			memcpy(pLines,m_pTrimedLines, sizeof(TRIM_LINE)*m_nTrimedLinesNum);
			delete[] m_pTrimedLines;
		}
		m_pTrimedLines = pLines;
		m_nTrimedLinesMemLen = newlen;
	}

	m_pTrimedLines[m_nTrimedLinesNum].pt0 = *pt0;
	m_pTrimedLines[m_nTrimedLinesNum].pt1 = *pt1;
	m_pTrimedLines[m_nTrimedLinesNum].type= type;
	m_nTrimedLinesNum++;
}


void CGrTrim::ClearTrimedLines()
{
	m_nTrimedLinesNum = 0;
}


void CGrTrim::AddBlock(double x0, double x1, double y0, double y1)
{
	TRIM_BLOCK *pBlocks;
	if( !m_pBlocks || m_nBlocksNum>=m_nBlocksMemLen )
	{
		int newlen = m_nBlocksMemLen+8;
		pBlocks = new TRIM_BLOCK[newlen];
		if( !pBlocks )return;
		
		if( m_pBlocks )
		{
			memcpy(pBlocks,m_pBlocks, sizeof(TRIM_BLOCK)*m_nBlocksNum);
			delete[] m_pBlocks;
		}
		m_pBlocks = pBlocks;
		m_nBlocksMemLen = newlen;
	}
	
	pBlocks = m_pBlocks+m_nBlocksNum;
	pBlocks->x0 = x0;  pBlocks->x1 = x1;
	pBlocks->y0 = y0;  pBlocks->y1 = y1;
	m_nBlocksNum++;
}


void CGrTrim::ClearBlocks()
{
	m_pCurBlock = NULL;
	m_nBlocksNum = 0;
}


static BOOL bPtInBlock(TRIM_BLOCK *pBlocks, int nBlockNum, TRIM_BLOCK *pCurBlock, double x, double y, double radius)
{
	int i = pCurBlock-pBlocks;
	if(i==0)
	{
		if( x>=pCurBlock->x0-radius && x<=pCurBlock->x1+radius &&
			y>=pCurBlock->y0-radius && y<=pCurBlock->y1 )
		{
			return TRUE;
		}			
	}
	if(i==(nBlockNum-1))
	{
		if( x>=pCurBlock->x0-radius && x<=pCurBlock->x1+radius &&
			y>=pCurBlock->y0 && y<=pCurBlock->y1+radius )
		{
			return TRUE;
		}			
	}
	if(i!=0 && i!=(nBlockNum-1) )
	{
		if( x>=pCurBlock->x0-radius && x<=pCurBlock->x1+radius &&
			y>=pCurBlock->y0 && y<=pCurBlock->y1 )
		{
			return TRUE;
		}			
	}
	return FALSE;
}

BOOL CGrTrim::bPtInBlocks(double x, double y, double radius)
{
	if( m_pCurBlock!=NULL )
	{
		int nBlock = m_pCurBlock-m_pBlocks;
		if( nBlock>=0 && nBlock<m_nBlocksNum )
		{
			if(bPtInBlock(m_pBlocks,m_nBlocksNum,m_pCurBlock,x,y,radius))
			{
				return TRUE;
			}
		}

		if( nBlock>0 && nBlock<m_nBlocksNum )
		{
			TRIM_BLOCK *pBlock1 = m_pCurBlock-1;
			if(bPtInBlock(m_pBlocks,m_nBlocksNum,pBlock1,x,y,radius))
			{
				m_pCurBlock = pBlock1;
				return TRUE;
			}
		}
		if( nBlock>=0 && nBlock<m_nBlocksNum-1 )
		{
			TRIM_BLOCK *pBlock1 = m_pCurBlock+1;
			if(bPtInBlock(m_pBlocks,m_nBlocksNum,pBlock1,x,y,radius))
			{
				m_pCurBlock = pBlock1;
				return TRUE;
			}
		}
	}

	TRIM_BLOCK *pBlock = m_pBlocks;	

	for( int i=0; i<m_nBlocksNum; i++,pBlock++ )
	{
		if(bPtInBlock(m_pBlocks,m_nBlocksNum,pBlock,x,y,radius))
		{
			m_pCurBlock = pBlock;
			return TRUE;			
		}
	}
	
	return FALSE;
}




BOOL CGrTrim::bPtInBlocks(double x, double y)
{
	TRIM_BLOCK *pBlocks = m_pBlocks;	

	if( m_pCurBlock!=NULL )
	{
		int nBlock = m_pCurBlock-m_pBlocks;
		if( nBlock>=0 && nBlock<m_nBlocksNum )
		{
			if( x>=m_pCurBlock->x0 && x<=m_pCurBlock->x1 &&
				y>=m_pCurBlock->y0 && y<=m_pCurBlock->y1 )
				return TRUE;
		}

		if( nBlock>0 && nBlock<m_nBlocksNum )
		{
			TRIM_BLOCK *pBlock1 = m_pCurBlock-1;
			if( x>=pBlock1->x0 && x<=pBlock1->x1 &&
				y>=pBlock1->y0 && y<=pBlock1->y1 )
			{
				m_pCurBlock = pBlock1;
				return TRUE;
			}
		}
		if( nBlock>=0 && nBlock<m_nBlocksNum-1 )
		{
			TRIM_BLOCK *pBlock1 = m_pCurBlock+1;
			if( x>=pBlock1->x0 && x<=pBlock1->x1 &&
				y>=pBlock1->y0 && y<=pBlock1->y1 )
			{
				m_pCurBlock = pBlock1;
				return TRUE;
			}
		}
	}

	for( int i=0; i<m_nBlocksNum; i++,pBlocks++ )
	{
		if( x>=pBlocks->x0 && x<=pBlocks->x1 &&
			y>=pBlocks->y0 && y<=pBlocks->y1 )
		{
			m_pCurBlock = pBlocks;
			return TRUE;
		}
	}
	
	return FALSE;
}
