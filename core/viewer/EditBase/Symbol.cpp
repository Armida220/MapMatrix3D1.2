// EBSymbol.cpp: implementation of the CSymbol class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "Symbol.h"
#include "GrBuffer.h"
#include "Markup.h"
#include "Functions_temp.h"
#include "GrTrim.h"
#include "GeoSurface.h"
#include "GeoParallel.h"
#include "GeoDirPoint.h"
#include "textfile.h"
#include "GeoText.h"
#include "SmartViewFunctions.h"
#include "Functions_temp.h"
//#include "RegDef.h "
#include "RegDef2.h"
#include "PlotText.h "
#include "matrix.h"
#include "CADSymFile.h"
#include "FillSurfaceWithPoint.h"
#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define XMLTAG_MaxToothLen		_T("MaxToothLen")
#define XMLTAG_Interval			_T("Interval")
#define XMLTAG_LineWidth		_T("LineWidth")
#define XMLTAG_ADJUSTXOFFSET	_T("AdjustXOffset")
#define XMLTAG_ToothLen			_T("ToothLen")
#define XMLTAG_Fasten			_T("Fasten")
#define XMLTAG_PointSize		_T("PointSize")
#define XMLTAG_PointInterval	_T("PointInterval")

#define XMLTAG_RANDOMANGLE		_T("RandomAngle")

#define XMLTAG_SINGLELINE		_T("SingleLine")
#define XMLTAG_STICKUPLEN		_T("StickupLen")
#define XMLTAG_SIDELINEWIDTH	_T("SideLineWidth")

#define XMLTAG_DASHALIGN		_T("DashAlign")
#define XMLTAG_CELLALIGN		_T("CellAlign")

#define XMLTAG_FILLED			_T("Filled")
#define XMLTAG_INSIDEBASELINE	_T("InsideBaseLine")
#define XMLTAG_KXMIN			_T("KXMIN")
#define XMLTAG_KXMAX			_T("KXMAX")

extern bool GIsPtOnLine(PT_2D pt, PT_2D *pts);
/*******************************************************************************
函数名   : <GGetPtIndexLine>
功能     : 获取基线上任一点在基线上点的索引 
参数     :  
           [IN] PT_3DEX *pts, int num : 折线串
           [IN] PT_3DEX pt : 被求点
		   [OUT] PT_3DEX *dpt  :折线串上满足条件的点
		   [OUT] int *nIndex: q点在折线串的线段索引
           ...
返回值   : 第一个点的索引

*******************************************************************************/
static int GGetPtIndexLine(const PT_3DEX *pts, int num, PT_3DEX pt, PT_3DEX *dpt)
{
	// 首先检查pt是否为pts上的点
	for (int i=0 ;i<num-1; i++)
	{
		PT_2D pt2d[2];
		for (int j=0; j<2; j++)
		{
			COPY_2DPT(pt2d[j],*(pts+i+j));
		}

		// 与第一点重合
		double dis1 = GraphAPI::GGet2DDisOf2P(PT_2D(pt.x,pt.y),pt2d[0]);
		if (dis1 < 1e-4)
		{
			if (dpt) *dpt = pts[i];
			return i;
		}
		
		// 与第二点重合
		double dis2 = GraphAPI::GGet2DDisOf2P(PT_2D(pt.x,pt.y),pt2d[1]);		
		if (dis2 < 1e-4)
		{
			if (i == num-1)
			{
				if (dpt) *dpt = pts[i];
				return i;
			}
			else 
			{
				if (dpt) *dpt = pts[i+1];
				return i+1;	
			}
		}

		double dis = GraphAPI::GGet2DDisOf2P(pt2d[0],pt2d[1]);
		
		// 在线段上
		if (dis1+dis2-dis < 1e-4)
		{
			return i;
		}
	}


 	int nIndex;

	double pencode=DBL_MAX,td;
	PT_3DEX tq, cq;
	for (i=0 ;i<num-1; i++)
	{	
		td = GraphAPI::GGetNearestDisOfPtToLine(pts[i].x,pts[i].y,pts[i+1].x,
			pts[i+1].y,pt.x,pt.y,&(tq.x),&(tq.y),false);
		if (td <= pencode/*GraphAPI::GetDisTolerance()*/)
		{
			pencode = td;
			nIndex = i;
			if (dpt)
			{
				*dpt = pts[i];
			}
			
		}

	}

	// 纠正在基线X坐标范围外的点
	Envelope e;
	e.CreateFromPts(pts,num,sizeof(PT_3DEX));
	if (pt.x < e.m_xl || pt.x > e.m_xh)
	{
		if (nIndex == 0)
		{
			nIndex = -1;
		}
		else if (nIndex == num-1)
		{
			nIndex = num;
		}
	}

	return nIndex;

}


// 获取周期点在原线上索引
template<class T>
static void  GetCycleLine(const T *pts,int nSum, const double dCycle,double dXOff,CArray<T,T> &dpts, CUIntArray &arrindex)
{
	if(pts==NULL || nSum<1 || dCycle<=0)
		return;
	dpts.RemoveAll();
	
	// 对dXOff取余
	if (dXOff > dCycle)
	{
		dXOff = dXOff - ceil(dXOff/dCycle)*dCycle;
	}
	
	//计算每一线段的长度,存储到pLen中
	float *pLen = new float[nSum];
	
	float  *p = pLen;
	double xold, yold, zold;
	
	const T *pt;
	pt = pts;
	int nFlag = nSum;
	while( nFlag-- )
	{
		// 复杂面
		if (p==pLen || pt->pencode==penMove) *p = 0.0;
		else
		{
			*(p) = *(p-1) + sqrt( (pt->x-xold)*(pt->x-xold) +
				(pt->y-yold)*(pt->y-yold)/* +
				(pt->z-zold)*(pt->z-zold) */);
		}
		xold = pt->x;
		yold = pt->y;
		//zold = pt->z;
		
		p++;
		pt++;
	}

	if (pLen[nSum - 1] / dCycle > 1e+4)
	{
		delete[]pLen;
		return;
	}

	//根据周期计算每一个周期点
	float ratio;
	float dis = dXOff;
	
	int i = 1;
	while(i < nSum)
	{
		if(dis < 0)
		{
			dis += dCycle;
		}
		else if(dis <= pLen[i] && pLen[i]>pLen[i-1] )
		{
			arrindex.Add(i-1);

			ratio = (dis-pLen[i-1]) / (pLen[i]-pLen[i-1]);
			T t3d;
			t3d.x = pts[i-1].x + ratio * (pts[i].x - pts[i-1].x);
			t3d.y = pts[i-1].y + ratio * (pts[i].y - pts[i-1].y);
			t3d.z = pts[i-1].z + ratio * (pts[i].z - pts[i-1].z);
			dpts.Add(t3d);
			dis += dCycle;
			if (dis > pLen[i])
			{
				i++;

				// 复杂面
				if (i < nSum && pts[i].pencode == penMove)
				{
					dis = 0;
					i++;
				}

				
			}
		}
		else
		{
			// 复杂面
			if (pts[i].pencode == penMove)
			{
				dis = 0;
			}

			i++;
		}
	}
	if(pLen)
		delete []pLen;
	
	
}

//GetCycleLine的针对图元对齐的优化版本，点数总是为两个，len是两点距离
template<class T>
static void  GetCycleLine_FastLoop(const PT_3DEX *pts,double len, const double dCycle,double dXOff, int index0, CArray<T,T> &dpts, CUIntArray &arrindex)
{
	if(pts==NULL || dCycle<=0)
		return;
	
	// 复杂面
	if (pts[1].pencode == penMove)
	{
		return;
	}

	if (len / dCycle > 1e+4)
	{
		return;
	}
	
	// 对dXOff取余
	dXOff = dXOff - ceil(dXOff/dCycle)*dCycle;

	//根据周期计算每一个周期点
	float ratio;
	float dis = dXOff;
	
	int i = 1;
	while(1)
	{
		dis += dCycle;

		if(dis>=0 && dis < len)
		{
			ratio = dis/len;
			T t3d;
			t3d.x = pts[0].x + ratio * (pts[1].x - pts[0].x);
			t3d.y = pts[0].y + ratio * (pts[1].y - pts[0].y);
			t3d.z = pts[0].z + ratio * (pts[1].z - pts[0].z);

			dpts.Add(t3d);
			arrindex.Add(index0);
		}
		else
		{
			break;
		}
	}		
}


template<class T>
static void  GetXOffLine(const T *pts, int nSum, const double dXOff,CArray<T,T> &dpts, CUIntArray &arrindex, CArray<float,float> &arrRatio)
{
	
	if(pts==NULL || nSum<1)
		return;
	dpts.RemoveAll();
	arrRatio.RemoveAll();
	
	//dXOff等于0时
	if(fabs(dXOff) <= GraphAPI::GetDisTolerance())
	{
		for(int i=0; i<nSum; i++)
		{
			dpts.Add(*(pts+i));
			if (i == nSum-1)
			{
				arrindex.Add(i-1);
			}
			else
				arrindex.Add(i);
		}
		return;
	}

	//根据周期计算每一个周期点
	double *pLen = new double[nSum];
	double dis;
	const T *pt, *pt1;

	pt = pts;
	//首先计算各个线段长度
	for( int i=0; i<nSum-1; i++, pt++)
	{
		pt1 = pt+1;

		dis = sqrt( (pt->x-pt1->x)*(pt->x-pt1->x) +
			(pt->y-pt1->y)*(pt->y-pt1->y) +
			(pt->z-pt1->z)*(pt->z-pt1->z) );

		if( pt1->pencode==penMove )
			pLen[i] = 0;
		else
			pLen[i] = dis;
	}

	pLen[nSum-1] = 0;

	double curLen = dXOff, dis0 = 0, ratio;

	//顶点移动之后所在原线串上的顶点序号
	int nOldIndex = 0;

	//计算各个顶点挪动后的位置信息
	for(i=0; i < nSum; i++)
	{
		if( pts[i].pencode==penMove )
		{
			curLen = dXOff;
			nOldIndex = i;
		}

		//跳过一些点，直到当前点落在某个线段上
		while( curLen>pLen[nOldIndex] && nOldIndex<nSum-1 )
		{
			curLen -= pLen[nOldIndex];
			nOldIndex++;
		}

		if( curLen<0 )
		{
			curLen += pLen[i];
		}
		else if( curLen>=0 && curLen<pLen[nOldIndex] && nOldIndex<nSum-1 )
		{
			arrindex.Add(nOldIndex);
			
			ratio = curLen / pLen[nOldIndex];

			pt = pts + nOldIndex;
			pt1 = pt+1;

			T t3d;
			t3d.x = pt->x + ratio * (pt1->x - pt->x);
			t3d.y = pt->y + ratio * (pt1->y - pt->y);
			t3d.z = pt->z + ratio * (pt1->z - pt->z);
			dpts.Add(t3d);
			
			arrRatio.Add(ratio);

			curLen += pLen[i];
		}
	}

/*	
	//计算每一线段的长度,存储到pLen中
	float *pLen = new float[nSum];
	
	float  *p = pLen;
	double xold, yold, zold;
	
	const T *pt;
	pt = pts;
	int nFlag = nSum;
	while( nFlag-- )
	{
		// 复杂面
		if (p==pLen || pt->pencode==penMove) *p = 0.0;
		else
		{
			*(p) = *(p-1) + sqrt( (pt->x-xold)*(pt->x-xold) +
				(pt->y-yold)*(pt->y-yold) +
				(pt->z-zold)*(pt->z-zold) );
		}
		xold = pt->x;
		yold = pt->y;
		zold = pt->z;
		
		p++;
		pt++;
	}
	//根据周期计算每一个周期点
	float ratio;
	float dis;
	int i,j;
	float *pLenIndex = pLen;
	for(i=0; i < nSum; i++)
	{
		j = 1;

		// 复杂面
		if (i != 0 && pts[i].pencode == penMove)
		{
			j = i + 1;
		}

		dis = pLenIndex[i] + dXOff;
		
		if(dis < 0)
			continue;
		
		for(; j<nSum; j++)
		{
			
			if(dis <= pLenIndex[j])
			{
				arrindex.Add(j-1);

				ratio = (dis-pLen[j-1]) / (pLen[j]-pLen[j-1]);
				T t3d;
				t3d.x = pts[j-1].x + ratio * (pts[j].x - pts[j-1].x);
				t3d.y = pts[j-1].y + ratio * (pts[j].y - pts[j-1].y);
				t3d.z = pts[j-1].z + ratio * (pts[j].z - pts[j-1].z);
				dpts.Add(t3d);

				arrRatio.Add(ratio);

				break;
			}
			
		}
		
	}
*/
	if(pLen)
		delete []pLen;	
	
}

//计算线串的长度数组
template<class D>
static void CalcLineStringLens(const PT_3DEX *pts, int npt, D *pLen)
{
	D *p = pLen;
	D xold, yold, zold;
	
	const PT_3DEX *pt = pts;
	
	for( int i=0; i<npt; i++)
	{
		// 复杂面
		if (p==pLen || pt->pencode==penMove) *p = 0.0;
		else
		{
			*(p) = *(p-1) + sqrt( (pt->x-xold)*(pt->x-xold) + (pt->y-yold)*(pt->y-yold) );
		}
		xold = pt->x;
		yold = pt->y;
		zold = pt->z;
		
		p++;
		pt++;
	}
}

// 求fpt,lpt线段上的点绕pt逆时针旋转ang角度后长度为wid的点
PT_3D GetRotatePt(PT_3D fpt, PT_3D lpt, PT_3D pt, double dWidth, double ang)
{
	// dWidth为0则返回
	if (fabs(dWidth) < 1e-4 || fabs(ang) < 1e-4)
	{
		return pt;
	}
	
	PT_3D dpt;
	
	double dx = pt.x - fpt.x, dy = pt.y - fpt.y;
	double r = sqrt(dx*dx + dy*dy); 
	
	// 与某点重合就与下一个基线上的点作用
	if (r <= 1e-4)  
	{
		dx = lpt.x - pt.x;
		dy = lpt.y - pt.y;
		r = sqrt(dx*dx + dy*dy); 
		if (r <= 1e-4) 
		{
			return pt;
		}
		else
		{
			double a = (dx*sin(ang)+dy*cos(ang))/r, b = (dx*cos(ang)-dy*sin(ang))/r;
			dpt.x = pt.x + dWidth*b;
			dpt.y = pt.y + dWidth*a;
			dpt.z = pt.z;
			
		}
		
	}
	else
	{
		double a = (dx*sin(ang)+dy*cos(ang))/r, b = (dx*cos(ang)-dy*sin(ang))/r;
		dpt.x = pt.x + dWidth*b;
		dpt.y = pt.y + dWidth*a;
		dpt.z = pt.z;
		
	}
	
	return dpt;
}



// 求fpt,lpt线段上的点绕pt逆时针旋转 PI/2 角度后长度为wid的点
PT_3D GetRotate90Pt(PT_3D fpt, PT_3D lpt, PT_3D pt, double dWidth)
{
	// dWidth为0则返回
	if (fabs(dWidth) < 1e-4)
	{
		return pt;
	}
	
	PT_3D dpt;
	
	double dx = pt.x - fpt.x, dy = pt.y - fpt.y;
	double r = sqrt(dx*dx + dy*dy); 
	
	// 与某点重合就与下一个基线上的点作用
	if (r <= 1e-4)  
	{
		dx = lpt.x - pt.x;
		dy = lpt.y - pt.y;
		r = sqrt(dx*dx + dy*dy); 
		if (r <= 1e-4) 
		{
			return pt;
		}
		else
		{
			double a = dx/r, b = -dy/r;
			dpt.x = pt.x + dWidth*b;
			dpt.y = pt.y + dWidth*a;
			dpt.z = pt.z;
			
		}
		
	}
	else
	{
		double a = dx/r, b = -dy/r;
		dpt.x = pt.x + dWidth*b;
		dpt.y = pt.y + dWidth*a;
		dpt.z = pt.z;
		
	}
	
	return dpt;
}


// pts为三点坐标, 获取角平分线上长度为len的点
template<class T>
static BOOL GetAngBisectorPt(T *pts, float len, T *ret, int num)
{
	if (!pts || !ret || num < 1) FALSE;
	
	double ax, ay, az, bx, by, bz;
	
	ax = pts[0].x - pts[1].x;
	ay = pts[0].y - pts[1].y;
	az = pts[0].z - pts[1].z;
	
	bx = pts[2].x - pts[1].x;
	by = pts[2].y - pts[1].y;
	bz = pts[2].z - pts[1].z;
	az = bz = 0;
	
	double alen = sqrt(ax*ax + ay*ay + az*az);
	double blen = sqrt(bx*bx + by*by + bz*bz);

	if( alen<GraphAPI::GetDisTolerance() || blen<GraphAPI::GetDisTolerance() )
	{
		*ret = pts[1];
		return TRUE;
	}
	
	// 角平分线
	double cx, cy, cz;
	cx = ax/alen + bx/blen;
	cy = ay/alen + by/blen;
	cz = az/alen + bz/blen;
	
	// 单位化
	double clen = sqrt(cx*cx + cy*cy + cz*cz);
	if( clen<GraphAPI::GetDisTolerance())
	{
		*ret = pts[1];
		return TRUE;
	}
	cx /= clen;
	cy /= clen;
	cz /= clen;
	
	BOOL bClockWise = GraphAPI::GIsClockwise(pts,3);
	
	if (!bClockWise)
	{
		cx = -cx;
		cy = -cy;
		cz = -cz;
	}
	
	ret[0].x = len*cx + pts[1].x;
	ret[0].y = len*cy + pts[1].y;
	ret[0].z = len*cz + pts[1].z;
	
	if (num == 2)
	{
		ret[1].x = -len*cx + pts[1].x;
		ret[1].y = -len*cy + pts[1].y;
		ret[1].z = -len*cz + pts[1].z;
	}
	
	return TRUE;
}




template<class T>
static void  GGetXOffLinePt(const T *pts, int nSum, T pt, int type, int &index, const double dXOff,T &dpts)
{	
	//dXOff等于0时
	if(pts==NULL || nSum<1 || fabs(dXOff) <= GraphAPI::GetDisTolerance())
	{
		dpts = pt;
		if (type == CCell::PlaceType::Tail && nSum >= 2)
			index = nSum - 2;
		return;
	}
	
	// 首点
	if (type == CCell::PlaceType::Head)
	{
		if (dXOff < 0)
		{
			index = 0;
			int flag = nSum;
			while (--flag)
			{
				double dis = GraphAPI::GGetDisOf2P3D(pts[0].x,pts[0].y,pts[0].z,
					pts[nSum-flag].x,pts[nSum-flag].y,pts[nSum-flag].z);
				if (dis > GraphAPI::GetDisTolerance())
				{
					double ratio = dis/(dis-dXOff);
					double dx = (pts[nSum-flag].x-pts[0].x)/ratio;
					double dy = (pts[nSum-flag].y-pts[0].y)/ratio;
					double dz = (pts[nSum-flag].z-pts[0].z)/ratio;
				
					dpts.x = pts[nSum-flag].x - dx;
					dpts.y = pts[nSum-flag].y - dy;
					dpts.z = pts[nSum-flag].z - dz;	
					break;
				}
			}

			if (flag == 0)
			{
				dpts = pt;
			}
		}
		else
		{
			double len = 0.0, len0;
			double xold, yold, zold;
			
			const T *pt0;
			pt0 = pts;
			xold = pt0->x;
			yold = pt0->y;
			zold = pt0->z;

			int nFlag = 1;
			pt0++;
			while (nFlag < nSum)
			{
				len0 = sqrt( (pt0->x-xold)*(pt0->x-xold) +
						(pt0->y-yold)*(pt0->y-yold) +
						(pt0->z-zold)*(pt0->z-zold) );

				
				if (len+len0 >= dXOff)
				{
					break;
				}
				
				len += len0;
				
				xold = pt0->x;
				yold = pt0->y;
				zold = pt0->z;

				pt0++;
				nFlag++;
			}

			// 最后一点之后
			if (nFlag == nSum)
			{
				index = nSum-2;
				int flag = nSum;
				while (--flag)
				{
					double dis = GraphAPI::GGetDisOf2P3D(pts[nSum-1].x,pts[nSum-1].y,pts[nSum-1].z,
						pts[flag-1].x,pts[flag-1].y,pts[flag-1].z);
					if (dis > GraphAPI::GetDisTolerance())
					{
						double ratio = dis/(dis+dXOff-len);
						double dx = (pts[nSum-1].x-pts[flag-1].x)/ratio;
						double dy = (pts[nSum-1].y-pts[flag-1].y)/ratio;
						double dz = (pts[nSum-1].z-pts[flag-1].z)/ratio;
				
						dpts.x = pts[flag-1].x + dx;
						dpts.y = pts[flag-1].y + dy;
						dpts.z = pts[flag-1].z + dz;	
						break;
					}
				}

				if (flag == 0)
				{
					dpts = pt;
				}
			}
			else
			{
				index = nFlag-1;
				double ratio = (dXOff-len)/len0;
				double dx = (pts[nFlag].x-pts[nFlag-1].x)*ratio;
				double dy = (pts[nFlag].y-pts[nFlag-1].y)*ratio;
				double dz = (pts[nFlag].z-pts[nFlag-1].z)*ratio;
				
				dpts.x = pts[nFlag-1].x + dx;
				dpts.y = pts[nFlag-1].y + dy;
				dpts.z = pts[nFlag-1].z + dz;	
				
			}
		}
	}
	// 尾点
	else if (type == CCell::PlaceType::Tail)
	{
		if (dXOff < 0)
		{
			double len = 0.0, len0;
			double xold, yold, zold;
			
			const T *pt0;
			pt0 = pts+nSum-1;
			xold = pt0->x;
			yold = pt0->y;
			zold = pt0->z;
			
			int nFlag = nSum-2;
			pt0--;
			while (nFlag >= 0)
			{
				len0 = sqrt( (pt0->x-xold)*(pt0->x-xold) +
					(pt0->y-yold)*(pt0->y-yold) +
					(pt0->z-zold)*(pt0->z-zold) );
				
				if (len+len0 >= -dXOff)
				{
					break;
				}
				len += len0;
				
				xold = pt0->x;
				yold = pt0->y;
				zold = pt0->z;
				
				pt0--;
				nFlag--;
			}
			
			// 第一点之前
			if (nFlag == -1)
			{
				index = 0;
				int flag = nSum;
				while (--flag)
				{
					double dis = GraphAPI::GGetDisOf2P3D(pts[0].x,pts[0].y,pts[0].z,pts[nSum-flag].x,pts[nSum-flag].y,pts[nSum-flag].z);
					if (dis > GraphAPI::GetDisTolerance())
					{
						double ratio = dis/(dis-dXOff-len);
						double dx = (pts[nSum-flag].x-pts[0].x)/ratio;
						double dy = (pts[nSum-flag].y-pts[0].y)/ratio;
						double dz = (pts[nSum-flag].z-pts[0].z)/ratio;
						
						dpts.x = pts[nSum-flag].x - dx;
						dpts.y = pts[nSum-flag].y - dy;
						dpts.z = pts[nSum-flag].z - dz;	
						break;
					}
				}

				if (flag == 0)
				{
					dpts = pt;
				}
			}
			else
			{
				index = nFlag;
				double ratio = (len0+len+dXOff)/len0;
				double dx = (pts[nFlag+1].x-pts[nFlag].x)*ratio;
				double dy = (pts[nFlag+1].y-pts[nFlag].y)*ratio;
				double dz = (pts[nFlag+1].z-pts[nFlag].z)*ratio;
				
				dpts.x = pts[nFlag].x + dx;
				dpts.y = pts[nFlag].y + dy;
				dpts.z = pts[nFlag].z + dz;	
				
			}
		}
		else
		{
			index = nSum-2;
			int flag = nSum;
			while (--flag)
			{
				double dis = GraphAPI::GGetDisOf2P3D(pts[nSum-1].x,pts[nSum-1].y,pts[nSum-1].z,
					pts[flag-1].x,pts[flag-1].y,pts[flag-1].z);
				if (dis > GraphAPI::GetDisTolerance())
				{
					double ratio = dis/(dis+dXOff);
					double dx = (pts[nSum-1].x-pts[flag-1].x)/ratio;
					double dy = (pts[nSum-1].y-pts[flag-1].y)/ratio;
					double dz = (pts[nSum-1].z-pts[flag-1].z)/ratio;
				
					dpts.x = pts[flag-1].x + dx;
					dpts.y = pts[flag-1].y + dy;
					dpts.z = pts[flag-1].z + dz;	
					break;
				}

				if (flag == 0)
				{
					dpts = pt;
				}
			}
		}
	}
	// 中点
	else if (type == CCell::PlaceType::Middle)
	{
		double dXOff1 = dXOff;
		if (dXOff < 0)
		{
			double len = 0.0, len0;
			double xold, yold, zold;
			
			const T *pt0;
			pt0 = pts+index+1;
			xold = pt0->x;
			yold = pt0->y;
			zold = pt0->z;
			
			dXOff1 -= sqrt( (pt.x-xold)*(pt.x-xold) + (pt.y-yold)*(pt.y-yold) +
					(pt.z-zold)*(pt.z-zold) );

			int nFlag = index;
			pt0--;
			while (nFlag >= 0)
			{
				len0 = sqrt( (pt0->x-xold)*(pt0->x-xold) +
					(pt0->y-yold)*(pt0->y-yold) +
					(pt0->z-zold)*(pt0->z-zold) );
				
				if (len+len0 >= -dXOff1)
				{
					break;
				}
				len += len0;
				
				xold = pt0->x;
				yold = pt0->y;
				zold = pt0->z;
				
				pt0--;
				nFlag--;
			}
			
			// 第一点之前
			if (nFlag == -1)
			{
				index = 0;
				int flag = nSum;
				while (--flag)
				{
					double dis = GraphAPI::GGetDisOf2P3D(pts[0].x,pts[0].y,pts[0].z,
						pts[nSum-flag].x,pts[nSum-flag].y,pts[nSum-flag].z);
					if (dis > GraphAPI::GetDisTolerance())
					{
						double ratio = dis/(dis-dXOff1-len);
						double dx = (pts[nSum-flag].x-pts[0].x)/ratio;
						double dy = (pts[nSum-flag].y-pts[0].y)/ratio;
						double dz = (pts[nSum-flag].z-pts[0].z)/ratio;
						
						dpts.x = pts[nSum-flag].x - dx;
						dpts.y = pts[nSum-flag].y - dy;
						dpts.z = pts[nSum-flag].z - dz;	
						break;
					}
				}

				if (flag == 0)
				{
					dpts = pt;
				}
			}
			else
			{
				index = nFlag;
				double ratio = (len0+len+dXOff1)/len0;
				double dx = (pts[nFlag+1].x-pts[nFlag].x)*ratio;
				double dy = (pts[nFlag+1].y-pts[nFlag].y)*ratio;
				double dz = (pts[nFlag+1].z-pts[nFlag].z)*ratio;
				
				dpts.x = pts[nFlag].x + dx;
				dpts.y = pts[nFlag].y + dy;
				dpts.z = pts[nFlag].z + dz;	
				
			}
		}
		else
		{
			double len = 0.0, len0;
			double xold, yold, zold;
			
			const T *pt0;
			pt0 = pts+index;
			xold = pt0->x;
			yold = pt0->y;
			zold = pt0->z;
			
			dXOff1 += sqrt( (pt.x-xold)*(pt.x-xold) + (pt.y-yold)*(pt.y-yold) +
				(pt.z-zold)*(pt.z-zold) );
			
			int nFlag = index;
			pt0++;
			while (nFlag < nSum)
			{
				len0 = sqrt( (pt0->x-xold)*(pt0->x-xold) +
					(pt0->y-yold)*(pt0->y-yold) +
					(pt0->z-zold)*(pt0->z-zold) );
				
				if (len+len0 >= dXOff1)
				{
					break;
				}
				len += len0;
				
				xold = pt0->x;
				yold = pt0->y;
				zold = pt0->z;
				
				pt0++;
				nFlag++;
			}
			
			// 最后点之后
			if (nFlag == nSum)
			{
				index = nSum-2;
				int flag = nSum;
				while (--flag)
				{
					double dis = GraphAPI::GGetDisOf2P3D(pts[nSum-1].x,pts[nSum-1].y,pts[nSum-1].z,pts[flag-1].x,pts[flag-1].y,pts[flag-1].z);
					if (dis > GraphAPI::GetDisTolerance())
					{
						double ratio = dis/(dis+dXOff1-len);
						double dx = (pts[nSum-1].x-pts[flag-1].x)/ratio;
						double dy = (pts[nSum-1].y-pts[flag-1].y)/ratio;
						double dz = (pts[nSum-1].z-pts[flag-1].z)/ratio;
				
						dpts.x = pts[flag-1].x + dx;
						dpts.y = pts[flag-1].y + dy;
						dpts.z = pts[flag-1].z + dz;
						break;
					}
				}

				if (flag == 0)
				{
					dpts = pt;
				}
			}
			else
			{
				index = nFlag;
				double ratio = (dXOff1-len)/len0;
				double dx = (pts[nFlag+1].x-pts[nFlag].x)*ratio;
				double dy = (pts[nFlag+1].y-pts[nFlag].y)*ratio;
				double dz = (pts[nFlag+1].z-pts[nFlag].z)*ratio;
				
				dpts.x = pts[nFlag].x + dx;
				dpts.y = pts[nFlag].y + dy;
				dpts.z = pts[nFlag].z + dz;	
				
			}
		}
	}

}


template<class T>
void ReversePoints(T *pts, int npt)
{
	for (int i=0; i<npt/2; i++)
	{
		T pt = pts[i];
		pts[i] = pts[npt-1-i];
		pts[npt-1-i] = pt;
	}
}

void Pt3dToPt2d(CArray<PT_3DEX,PT_3DEX> &pts3,CArray<PT_2D,PT_2D> &pts2)
{
	pts2.RemoveAll();
	pts2.SetSize(pts3.GetSize());
	
	for(int i=0; i<pts3.GetSize(); i++)
	{
		pts2[i].x = pts3[i].x;
		pts2[i].y = pts3[i].y;
	}
}

static void SetColors(const CFeature *pFt, CGeoArray& arrPGeos)
{
	COLORREF clr = pFt->GetGeometry()->GetColor();
	for(int i=0; i<arrPGeos.GetSize(); i++)
	{
		arrPGeos[i]->SetColor(clr);
	}	
}

MyNameSpaceBegin


// pt为线pts上的垂点，dWidth为与垂点的距离
int GGetPtFromLineWidth(PT_3DEX *pts, int num, PT_3DEX pt, double dWidth, PT_3DEX *dpt, BOOL bNextNode)
{
	if (!pts || !dpt) return -1;

	if (num < 2)
	{
		*dpt = pt;
		return -1;
	}

	PT_3DEX ptdp;
	int index = GGetPtIndexLine(pts, num, pt, &ptdp);
	// 支持首点和尾点外的点，首点前的与首点作用，尾点后的与尾点作用
	if (index < 0)
	{
		double dx = pts[0].x - pt.x, dy = pts[0].y - pt.y;
		double r = sqrt(dx*dx + dy*dy); 
		
		double a = dx/r, b = dy/r;
		dpt->x = pt.x - dWidth*b;
		dpt->y = pt.y + dWidth*a;
		dpt->z = pt.z;

		return -1;
	}
	else if (index > num-1)
	{
		double dx = pt.x - pts[num-1].x, dy = pt.y - pts[num-1].y;
		double r = sqrt(dx*dx + dy*dy);
		
		double a = dx/r, b = dy/r;
		dpt->x = pt.x - dWidth*b;
		dpt->y = pt.y + dWidth*a;
		dpt->z = pt.z;
		
		return num;
	}

	// dWidth为0则返回
	if (fabs(dWidth) < GraphAPI::GetDisTolerance())
	{
		*dpt = pt;
		return index;
	}

	double dx = pt.x - pts[index].x, dy = pt.y - pts[index].y;
	double r = sqrt(dx*dx + dy*dy); 

	// 与某点重合
	if (r <= GraphAPI::GetDisTolerance())  
	{
		// 与下一个基线上的点作用
		if (bNextNode)
		{
			// 与最后点重合
			if (index >= num-1)
			{
				dx = pt.x - pts[index-1].x;
				dy = pt.y - pts[index-1].y;
				r = sqrt(dx*dx + dy*dy);
			}
			else
			{
				dx = pts[index+1].x - pt.x;
				dy = pts[index+1].y - pt.y;
				r = sqrt(dx*dx + dy*dy); 			
				
			}		

		}
		// 与上一个基线上的点作用
		else
		{
			// 与首点重合
			if (index <= 0)
			{
				dx = pts[index].x - pt.x;
				dy = pts[index].y - pt.y;
				r = sqrt(dx*dx + dy*dy);
			}
			else
			{
				dx = pt.x - pts[index-1].x;
				dy = pt.y - pts[index-1].y;
				r = sqrt(dx*dx + dy*dy); 			
				
			}		
		}
		
		
	}
	
	if (r <= GraphAPI::GetDisTolerance()) 
	{
		*dpt = pt;
	}
	else
	{
		double a = dx/r, b = dy/r;
		dpt->x = pt.x - dWidth*b;
		dpt->y = pt.y + dWidth*a;
		dpt->z = pt.z;
	}

	return index;
}


static void GetBasePtsAndWidth(CGeometry *pGeo, CArray<PT_3DEX,PT_3DEX>& basepts, float& fwid)
{
	fwid = 1;

	int nGeoType = pGeo->GetClassType();	
	
	if (nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEODCURVE || nGeoType == CLS_GEOPARALLEL)
	{
		if (nGeoType == CLS_GEODCURVE)
		{
			((CGeoDCurve*)pGeo)->GetBaseShape(basepts);
			if (((CGeoDCurve*)pGeo)->GetWidth() < 0)
			{
				fwid = -1;
			}
		}
		else
		{
			const CShapeLine *pShape = ((CGeoCurve*)pGeo)->GetShape();
			if (pShape)
			{
				pShape->GetPts(basepts);
			}
			
			if (nGeoType == CLS_GEOPARALLEL && ((CGeoParallel*)pGeo)->GetWidth()<0)
			{
				fwid = -1;
			}
		}
		
	}
	else if (nGeoType == CLS_GEOSURFACE || nGeoType == CLS_GEOMULTISURFACE)
	{
		const CShapeLine *pShape = ((CGeoSurface*)pGeo)->GetShape();
		if (pShape)
		{
			pShape->GetPts(basepts);
		}
	}
	else
	{
		pGeo->GetShape(basepts);
	}
}


static void GetMultiSurfaceBasePts(CGeometry *pGeo, CArray<PT_3DEX,PT_3DEX>& basepts)
{
	int nGeoType = pGeo->GetClassType();
	if (nGeoType == CLS_GEOMULTISURFACE)
	{
		CGeoMultiSurface* pGeo2 = ((CGeoMultiSurface*)pGeo);
		int nSurface = pGeo2->GetSurfaceNum();
		for(int i=0; i<nSurface; i++)
		{
			CArray<PT_3DEX,PT_3DEX> arrPts;
			pGeo2->GetSurface(i,arrPts);

			CGeoSurface t;
			t.CreateShape(arrPts.GetData(),arrPts.GetSize());

			const CShapeLine *pShape = t.GetShape();

			arrPts.RemoveAll();
			pShape->GetPts(arrPts);

			int nSize = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
			arrPts.SetSize(nSize);

			for(int j=0; j<arrPts.GetSize(); j++)
			{
				arrPts[j].pencode = penLine;
			}
			arrPts[0].pencode = penMove;

			basepts.Append(arrPts);
		}
	}
}


DWORD g_nScale = 2000;
double g_lfDrawScale = 1.0;
double g_lfAnnoScale = 1.0;

void SetDataSourceScale(DWORD nScale)
{
	g_nScale = nScale;
}

void SetSymbolDrawScale(float fDrawScale)
{
	g_lfDrawScale = fDrawScale;
}

void SetSymbolAnnoScale(float fAnnoScale)
{
	g_lfAnnoScale = fAnnoScale;
}

//符号化时，GetSymbolDrawScale() 会频繁调用，每次读取注册表会很慢，所以把“绘制比例”保存为全局变量
float GetSymbolDrawScale()
{
	return (g_nScale/1000.0)*g_lfDrawScale;
}

float GetSymbolAnnoScale()
{
	return g_lfAnnoScale;
}


float CSymbol::m_tolerance = 0.0f;
BOOL CSymbol::m_bBreakCell = TRUE;
BOOL CSymbol::m_bBreakLinetype = TRUE;


// 获取节点线宽点
template<class T>
static bool GetLineWidthPts(T pts[2], double dWidth, T dpt[2], BOOL bFirstPt)
{
	if (fabs(dWidth)<1e-4) return false;

	double dx = pts[0].x - pts[1].x, dy = pts[0].y - pts[1].y;
	double r = sqrt(dx*dx + dy*dy); 

	if (r < 1e-4)
	{
		if (bFirstPt)
		{
			dpt[0]=dpt[1]=pts[0];
		}
		else
		{
			dpt[0]=dpt[1]=pts[1];
		}
		
		return false;
	}

	T pt;
	if (bFirstPt)
	{
		pt = pts[0];
		dx = -dx;
		dy = -dy;
	}
	else
	{
		pt = pts[1];
		dWidth = -dWidth;
	}

	double a = dx/r, b = dy/r;
	dpt[0].x = pt.x - dWidth*b;
	dpt[0].y = pt.y + dWidth*a;
	dpt[0].z = pt.z;

	dWidth = -dWidth;

	dpt[1].x = pt.x - dWidth*b;
	dpt[1].y = pt.y + dWidth*a;
	dpt[1].z = pt.z;

	return true;
}

BOOL FillLineWidth(PT_3DEX *pts, int nSum, float fFtrWidth, float ftrLinewidthScale, GrBuffer *pBuf, COLORREF clr, float fDrawScale)
{
	if (!pts || nSum <= 0 || !pBuf) return FALSE;

	// 检测是否需要使用变宽线（当线串中有节点宽度与整体宽度不一致时使用变宽线填充）
	BOOL bSameWid = TRUE;
	for (int i=0; i<nSum; i++)
	{
		if (fabs(pts[i].wid) > 1e-6 && (pts[i].wid-fFtrWidth) > 1e-4)
		{
			bSameWid = FALSE;
			break;
		}
	}

	if (bSameWid)
	{
		return FALSE;
	}

	// arr1, arr2 分别存储线两边的线宽点
	CArray<PT_3D,PT_3D> arr1, arr2;

	PT_3DEX *p = pts;

	PT_3DEX line[2], ret[2];

	BOOL bStart = TRUE;
	for (i=0; i<nSum; i++,p++)
	{
		float wid = pts[i].wid*ftrLinewidthScale*fDrawScale/2;

		if (fabs(wid) < 1e-8)
		{
			arr1.Add(pts[i]);
			arr2.Add(pts[i]);
		}
		else if (i == 0 || bStart)
		{	
			memcpy(line,p,sizeof(PT_3DEX)*2);
			GetLineWidthPts(line,wid,ret,TRUE);
			arr1.Add(ret[0]);
			arr2.Add(ret[1]);
		}
		else if (i == nSum-1)
		{
			memcpy(line,p-1,sizeof(PT_3DEX)*2);
			GetLineWidthPts(line,wid,ret,FALSE);

			// 闭合，最后一段单独画，以免面有空洞
			if (fabs(pts[0].x-pts[i].x) + fabs(pts[0].y-pts[i].y) + fabs(pts[0].z-pts[i].z) < 1e-4)
			{
				int num1 = arr1.GetSize();
				int num2 = arr2.GetSize();
				
				pBuf->BeginPolygon(clr,POLYGON_FILLTYPE_COLOR);
				pBuf->MoveTo(&arr2[num2-1]);
				pBuf->LineTo(&ret[0]);
				pBuf->LineTo(&ret[1]);
				pBuf->LineTo(&arr1[num1-1]);
				pBuf->End();
			}
			else
			{				
				arr1.Add(ret[0]);
				arr2.Add(ret[1]);
			}
			
		}
		else
		{
			memcpy(line,p-1,sizeof(PT_3DEX)*2);
			GetLineWidthPts(line,wid,ret,FALSE);

			double x, y;
			if (GraphAPI::GGetPtInAngleBisector2D((*(p)).x,(*(p)).y,(*(p-1)).x,(*(p-1)).y,(*(p+1)).x,(*(p+1)).y,&x,&y))
			{
				PT_3DEX ret1[2];

				int size = arr1.GetSize();
				double vx0 = ret[0].x - arr1[size-1].x, vx1 = x - (*p).x;
				double vy0 = ret[0].y - arr1[size-1].y, vy1 = y - (*p).y;
				double t;
				if (GraphAPI::GGetLineIntersectLine(arr1[size-1].x,arr1[size-1].y,vx0,vy0,(*p).x,(*p).y,vx1,vy1,&ret1[0].x,&ret1[0].y,&t))
				{
					ret1[0].z = arr1[size-1].z + t * (ret[0].z - arr1[size-1].z);
				}
				else
				{
					ret1[0] = ret[0];
				}

				vx0 = ret[1].x - arr2[size-1].x, vx1 = x - (*p).x;
				vy0 = ret[1].y - arr2[size-1].y, vy1 = y - (*p).y;
				if (GraphAPI::GGetLineIntersectLine(arr2[size-1].x,arr2[size-1].y,vx0,vy0,(*p).x,(*p).y,vx1,vy1,&ret1[1].x,&ret1[1].y,&t))
				{
					ret1[1].z = arr2[size-1].z + t * (ret[1].z - arr2[size-1].z);
				}
				else
				{
					ret1[1] = ret[1];
				}

				arr1.Add(ret1[0]);
 				arr2.Add(ret1[1]);
			}
			else
			{
				arr1.Add(ret[0]);
 				arr2.Add(ret[1]);
			}
			
		}

		bStart = FALSE;
	}

	CArray<PT_3D,PT_3D> arr;
	arr.Append(arr1);
	for (i=arr2.GetSize()-1; i>=0; i--)
	{
		arr.Add(arr2[i]);
	}

	int size = arr.GetSize();
	if (size < 3)  return FALSE;

	pBuf->BeginPolygon(clr,POLYGON_FILLTYPE_COLOR);
	pBuf->MoveTo(&arr[0]);
	for (i=1; i<size; i++)
	{
		pBuf->LineTo(&arr[i]);
	}
	pBuf->End();

	return TRUE;
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSymbol::CSymbol()
{
	m_nType = SYMTYPE_NONE;
}

CSymbol::~CSymbol()
{

}

BOOL CSymbol::IsNeedAttrWhenDraw()
{
	return FALSE;
}


void CSymbol::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	return;
}


void CSymbol::Draw(const CFeature *pFt, GrBuffer *pBuf, CValueTable &tab, float fDrawScale, float fRotAngle, float fViewScale)
{
	return;
}


BOOL CSymbol::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	return FALSE;
}


BOOL CSymbol::ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	return Explode(pFt,fDrawScale,arrPGeos);
}


BOOL CSymbol::Explode(const CFeature *pFt, CValueTable& tab, float fDrawScale, CGeoArray& arrPGeos)
{
	return Explode(pFt,fDrawScale,arrPGeos);
}


BOOL CSymbol::ExplodeSimple(const CFeature *pFt, CValueTable& tab, float fDrawScale, CGeoArray& arrPGeos)
{
	return ExplodeSimple(pFt,fDrawScale,arrPGeos);
}


// 小数四舍五入  
//  
int Round(const float a)  
{  
    return static_cast<int>(a + 0.5);  
}  

/*******************************************************************************

函数名   : <CreateCoverPolygon>
功能     : 压盖地物
参数     :  
           [IN] int nCoverType	  : 压盖类型
           [IN] const Envelope &e : 压盖区域
		   [IN] PT_3D pt          :	压盖中心
           [OUT] GrBuffer *pBuf   : 压盖区域存储
返回值   : 无

*******************************************************************************/
void CreateCoverPolygon(int nCoverType, Envelope e, PT_3D pt, GrBuffer *pBuf)
{
	if (!pBuf || nCoverType == 0) return;

	pBuf->BeginPolygon(0,POLYGON_FILLTYPE_COLOR,0,NULL,0,FALSE,TRUE);

	// 矩形压盖
	if (nCoverType == COVERTYPE_RECT)
	{		
		pBuf->MoveTo(&PT_3D(e.m_xl,e.m_yl,pt.z));
		pBuf->LineTo(&PT_3D(e.m_xh,e.m_yl,pt.z));
		pBuf->LineTo(&PT_3D(e.m_xh,e.m_yh,pt.z));
		pBuf->LineTo(&PT_3D(e.m_xl,e.m_yh,pt.z));
		pBuf->LineTo(&PT_3D(e.m_xl,e.m_yl,pt.z));				
	}
	// 椭圆形压盖
	else if (nCoverType == COVERTYPE_CIRCLE)
	{
		PT_3D cenpt;
		cenpt.x = (e.m_xl+e.m_xh)/2;
		cenpt.y = (e.m_yl+e.m_yh)/2;
		cenpt.z = pt.z;

		float r = (e.Width()>e.Height()?e.Width():e.Height())/2;
		float rx = e.Width()/2, ry = e.Height()/2;

		float delta_theta = 0.25;
		
		for( float angle = 0; angle < 2*PI; angle += delta_theta )
		{
			PT_3D tmp;
			tmp.x = cenpt.x + rx*cos(angle);
			tmp.y = cenpt.y + ry*sin(angle);
			tmp.z = cenpt.z;

			if (angle == 0)
			{
				pBuf->MoveTo(&tmp);
			}
			else
			{
				pBuf->LineTo(&tmp);
			}
		}
				
	}

	pBuf->End();
	
}


void CreateCoverPolygon(int nCoverType, PT_3D bounds[4], PT_3D pt, GrBuffer *pBuf)
{
	if (!pBuf || nCoverType == 0) return;
	
	// 矩形压盖
	if (nCoverType == COVERTYPE_RECT)
	{	
		pBuf->BeginPolygon(0,POLYGON_FILLTYPE_COLOR,0,NULL,0,FALSE,TRUE);
		pBuf->MoveTo(&PT_3D(bounds[0].x,bounds[0].y,pt.z));
		pBuf->LineTo(&PT_3D(bounds[1].x,bounds[1].y,pt.z));
		pBuf->LineTo(&PT_3D(bounds[2].x,bounds[2].y,pt.z));
		pBuf->LineTo(&PT_3D(bounds[3].x,bounds[3].y,pt.z));
		pBuf->LineTo(&PT_3D(bounds[0].x,bounds[0].y,pt.z));	
		pBuf->End();
	}
	// 椭圆形压盖
	else if (nCoverType == COVERTYPE_CIRCLE)
	{
		PT_3D cenpt;
		cenpt.x = (bounds[0].x+bounds[2].x)/2;
		cenpt.y = (bounds[0].y+bounds[2].y)/2;
		cenpt.z = pt.z;
		
		float rx = GraphAPI::GGet2DDisOf2P(bounds[0],bounds[1])/2, ry = GraphAPI::GGet2DDisOf2P(bounds[1],bounds[2])/2;
		
		float delta_theta = 0.25;
		
		
		GrBuffer buf;
		buf.BeginPolygon(0,POLYGON_FILLTYPE_COLOR,0,NULL,0,FALSE,TRUE);
		for( float angle = 0; angle < 2*PI; angle += delta_theta )
		{
			PT_3D tmp;
			tmp.x = cenpt.x + rx*cos(angle);
			tmp.y = cenpt.y + ry*sin(angle);
			tmp.z = cenpt.z;
			
			if (angle == 0)
			{
				buf.MoveTo(&tmp);
			}
			else
			{
				buf.LineTo(&tmp);
			}
		}
		buf.End();

		float ang = GraphAPI::GGetAngle(bounds[0].x,bounds[0].y,bounds[1].x,bounds[1].y);

		PT_3D ptdir;
		ptdir.x = ptdir.y = 0;
		ptdir.z = 1000.0;
		buf.Rotate(ang,&cenpt,&ptdir);

		pBuf->AddBuffer(&buf);
		
	}
	

	
}

static void DrawCellToBuf(GrBuffer *pBuf, int nCellIndex, CellDef *cell, PT_3D *pt, double kx, double ky, double angle, COLORREF color, float fWidth, 
						  BOOL bBreakCell, BOOL bUseCover, int nCoverType, float fExtendDis)
{
	GrBuffer cellbuf;

	if (bBreakCell)
	{		
		cellbuf.AddBuffer(cell->m_pgr,pt->z);
		
		double m1[16];
		
		Matrix44FromTransform(m1,
			kx,ky,
			angle,
			pt->x,pt->y,0);
		
		cellbuf.Transform(m1);
		cellbuf.ZoomPointSize(kx,ky);
		
		cellbuf.SetAllColor(color);
		cellbuf.SetAllLineWidthOrWidthScale(TRUE,fWidth);
	}
	else
	{
		//优化绘制
		if( cell->m_bCircular )
			angle = 0.0;

		cellbuf.Point(color,pt,kx,ky,angle,TRUE,nCellIndex,fWidth);
	}

	if( bUseCover )
	{
		GrBuffer grCoverBuf;
		
		Envelope e = cellbuf.GetEnvelope();
		e.Inflate(fExtendDis,fExtendDis,0);
		
		CreateCoverPolygon(nCoverType,e,*pt,&grCoverBuf);
		
		pBuf->AddBuffer(&grCoverBuf);
	}

	pBuf->AddBuffer(&cellbuf);	
}

static BOOL IsGrLineStringSimilar(const Graph *p1, const Graph *p2)
{
	if( IsGrLineString(p1) && IsGrLineString(p2) )
	{
		const GrLineString *p3 = (const GrLineString*)p1;
		const GrLineString *p4 = (const GrLineString*)p2;

		if( p3->style==p4->style && p3->width==p4->width && (p3->bGrdWid==p3->bGrdWid) &&
			p3->color==p4->color && p3->bUseSelfcolor==p4->bUseSelfcolor && p3->bUseBackColor==p4->bUseBackColor )
			return TRUE;
	}

	return FALSE;
}


void ConvertGrBufferToGeos(GrBuffer *pBuf, float fDrawScale, CGeoArray &arr)
{
	if (!pBuf) return;

	const Graph *pGr = pBuf->HeadGraph(); 

	if (!pGr) return;

	char symname[256] = {0};

	CCellDefLib *pCellLib = GetCellDefLib();
	CBaseLineTypeLib *pLTLib = GetBaseLineTypeLib();
	
	while(pGr)
	{
		if (IsGrPoint(pGr))
		{
			const GrPoint *cgr = (const GrPoint*)pGr;			
			PT_3D pt = cgr->pt;

			CGeoPoint *pGeo = NULL;
			
// 			if( cgr->cell>0 )
// 				pGeo = (CGeoPoint*)CPermanent::CreatePermanentObject(CLS_GEODIRPOINT);
// 			else
				pGeo = (CGeoPoint*)CPermanent::CreatePermanentObject(CLS_GEOPOINT);

			if (!pGeo) 
			{
				delete pGeo;
				continue;
			}
			pGeo->CreateShape(&PT_3DEX(pt,penLine),1);
			pGeo->SetColor(cgr->color);

			if( cgr->cell>0 )
			{
				CellDef def = pCellLib->GetCellDef(cgr->cell-1);
				if( def.m_pgr )
				{
					sprintf(symname,"@%s",def.m_name);
					pGeo->SetSymbolName(symname);
				}
			}
			else
			{
				if( cgr->bFlat )
					pGeo->EnableFlated(TRUE);
				else
					pGeo->EnableFlated(FALSE);

				if( cgr->bGrdSize )
					pGeo->EnableGrdWid(TRUE);
				else
					pGeo->EnableGrdWid(FALSE);
			}

			//Geometry 的绘制会考虑 fDrawScale，所以这里需要将 fDrawScale 的影响消除
			if( cgr->bGrdSize )
			{
				pGeo->m_fKx = cgr->kx/fDrawScale;
				pGeo->m_fKy = cgr->ky/fDrawScale;
				pGeo->m_fWidth = cgr->width/fDrawScale;
			}

			pGeo->m_lfAngle = cgr->angle*180/PI;

			arr.Add(pGeo);
		}
		else if( IsGrPointString(pGr)||IsGrPointStringEx(pGr) )
		{
			const GrPointString *cgr = (const GrPointString*)pGr;			
			
			for( int i=0; i<cgr->ptlist.nuse; i++)
			{
				CGeoPoint *pGeo = (CGeoPoint*)CPermanent::CreatePermanentObject(CLS_GEOPOINT);
				if (!pGeo) 
				{
					delete pGeo;
					continue;
				}

				PT_3D pt = cgr->ptlist.pts[i];

				pGeo->CreateShape(&PT_3DEX(pt,penLine),1);
				if( IsGrPointStringEx(pGr) )
				{
					const GrPointStringEx *cgr2 = (const GrPointStringEx*)pGr;			
					pGeo->SetColor(cgr2->attrList.pts[i].color);
				}
				else
				{
					pGeo->SetColor(cgr->color);
				}
				
				if( cgr->cell>0 )
				{
					CellDef def = pCellLib->GetCellDef(cgr->cell-1);
					if( def.m_pgr )
					{
						sprintf(symname,"@%s",def.m_name);
						pGeo->SetSymbolName(symname);
					}
				}
				else
				{
					if( cgr->bFlat )
						pGeo->EnableFlated(TRUE);
					else
						pGeo->EnableFlated(FALSE);
					
					if( cgr->bGrdSize )
						pGeo->EnableGrdWid(TRUE);
					else
						pGeo->EnableGrdWid(FALSE);
				}
				
				pGeo->m_fKx = cgr->kx/fDrawScale;
				pGeo->m_fKy = cgr->ky/fDrawScale;
				pGeo->m_fWidth = cgr->width/fDrawScale;
				pGeo->m_lfAngle = cgr->ptlist.pts[i].CodeToFloat()*180/PI;
				
				arr.Add(pGeo);
			}

		}
		else if( IsGrLineString(pGr) )
		{
			GrVertexList *ptlist;

			GrLineString *pGr0 = (GrLineString*)pGr;

			//取出连续的 GrLineString 点串，一起处理（pmc）
			// 连续的 GrLineString 点串可能存在不一样的线型，分开处理（hxb）
			CArray<GrVertex,GrVertex> arrVertexs0, arrVertexs1;

			while( pGr )
			{
				//如果其他属性也相同，才连续处理
				if( IsGrLineString(pGr) && IsGrLineStringSimilar(pGr0,pGr) )
				{					
					ptlist = &((GrLineString*)pGr)->ptlist;

					arrVertexs1.SetSize(ptlist->nuse);
					memcpy(arrVertexs1.GetData(),ptlist->pts,ptlist->nuse*sizeof(GrVertex));

					arrVertexs0.Append(arrVertexs1);

					pGr = pGr->next;
				}
				else
				{
					break;
				}
			}

			//处理点串
			
			CArray<PT_3DEX,PT_3DEX> arrpts;
			
			int begin = 0, sum = arrVertexs0.GetSize();
			GrVertex *pts = arrVertexs0.GetData();

			for( int i=0; i<sum; i++)
			{
				BOOL bCreateCurve = FALSE;

				if( i==(sum-1) )
					bCreateCurve = TRUE;

				//很近的两点，就跳过
				else if( i>0 && GraphAPI::GGet2DDisOf2P(pts[i-1],pts[i])<1e-6 )
					continue;

				else if( IsGrPtCodeMoveTo(pts+i) )
					bCreateCurve = TRUE;

				if( bCreateCurve )
				{
					int npt = i-begin;

					if( i==(sum-1) && IsGrPtCodeLineTo(pts+i) )
						npt++;

					if( npt>=2 )
					{
						arrpts.SetSize(npt);
						
						for (int j=0; j<npt; j++)
						{
							COPY_3DPT(arrpts[j],pts[begin+j]);
							arrpts[j].pencode = penLine;
						}

						npt = GraphAPI::GKickoffSamePoints(arrpts.GetData(),arrpts.GetSize());	
						if( npt>=2 )
						{
							CGeoCurve *pGeo = (CGeoCurve*)CPermanent::CreatePermanentObject(CLS_GEOCURVE);
							if (pGeo && pGeo->CreateShape(arrpts.GetData(),npt))
							{
								pGeo->SetColor(pGr0->color);
								
								if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
								{
									if (pGr0->bGrdWid)
									{
										((CGeoCurve*)pGeo)->m_fLineWidth = pGr0->width/fDrawScale;
									}
									else
									{
										((CGeoCurve*)pGeo)->m_fLinewidthScale = pGr0->width;
									}
								}

								if( pGr0->style>0 )
								{
									BaseLineType lt = pLTLib->GetBaseLineType(pGr0->style-1);
									{
										sprintf(symname,"@%s",lt.m_name);
										pGeo->SetSymbolName(symname);
									}
								}
								else
									pGeo->SetSymbolName("*");

								arr.Add(pGeo);			
							}
						}

					}

					begin = i;
				}
			}
			
			// pGr 已经执行了 next 操作，直接跳转到主循环
			continue;
			
		}
		//处理为线
		else if( IsGrPolygon(pGr) )
		{
			GrVertexList *ptlist;
			GrPolygon *pp = (GrPolygon*)pGr;

			ptlist = &pp->ptlist;
			
			ptlist->nuse = GraphAPI::GKickoffSamePoints(ptlist->pts,ptlist->nuse);

			GrVertex *ptNodes = ptlist->pts;
			int ptnum = ptlist->nuse;
			
			CArray<PT_3DEX,PT_3DEX> arrpts;
			arrpts.SetSize(ptnum);

			for (int i=0; i<ptnum; i++)
			{
				COPY_3DPT(arrpts[i],ptNodes[i]);
				arrpts[i].pencode = penLine;
			}

			CGeoSurface *pGeo = (CGeoSurface*)CPermanent::CreatePermanentObject(CLS_GEOSURFACE);
			if (!pGeo)
			{
				pGr = pGr->next;
				continue;
			}
			if (pGeo && pGeo->CreateShape(arrpts.GetData(),arrpts.GetSize()))
			{
				pGeo->SetColor(pGr->color);

				pGeo->SetSymbolName("*");

				if( pp->filltype==POLYGON_FILLTYPE_COLOR )
				{
					pGeo->EnableFillColor(TRUE,pGr->color);
				}
				arr.Add(pGeo);			
			}
			else
			{
				delete pGeo;
			}
			
		}
		else if (IsGrText(pGr))
		{
			GrText *pText = (GrText*)pGr;
			
			CGeoText *pGeo = (CGeoText*)CPermanent::CreatePermanentObject(CLS_GEOTEXT);
			if (!pGeo)
			{
				pGr = pGr->next;
				continue;
			}

			pGeo->CreateShape(&PT_3DEX(pText->pt.x,pText->pt.y,pText->pt.z,penLine),1);
			
			pGeo->SetColor(pText->color);
			pGeo->SetText(pText->text);
			
			TEXT_SETTINGS0 settings;
			settings.fHeight = pText->settings.fHeight/fDrawScale;
			settings.fWidScale = pText->settings.fWidScale;
			settings.fCharIntervalScale = pText->settings.fCharIntervalScale;
			settings.fLineSpacingScale = pText->settings.fLineSpacingScale;
			settings.fTextAngle = pText->settings.fTextAngle;
			settings.fCharAngle = pText->settings.fCharAngle;			
			settings.nAlignment = pText->settings.nAlignment;
			settings.nInclineType = pText->settings.nInclineType;
			settings.fInclineAngle = pText->settings.fInclineAngle;	
			settings.nPlaceType = singPt;
			_tcscpy(settings.strFontName,pText->settings.tcFaceName);		
			pGeo->SetSettings(&settings);
			pGeo->SetSymbolName("*");

			arr.Add(pGeo);

		}

		pGr = pGr->next;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCell::CCell()
{
	m_nType = SYMTYPE_CELL;
	m_dx=m_dy=m_angle=m_fWidth=0; 
	m_kx = m_ky = 1;
	m_nPlaceType = 0;
	m_nFillType = FillType::BaseLine;
	m_nCenterLineMode = cenNULL;
	m_bDirWithFirstLine = FALSE;
	m_nCoverType = COVERTYPE_NONE;
	m_fExtendDis = 0;
}

CCell::~CCell()
{

}

void CCell::CopyFrom(CSymbol *pSymbol)
{
	CCell *pCel = (CCell*)pSymbol;
	m_angle = pCel->m_angle;
	m_nPlaceType = pCel->m_nPlaceType;
	m_nFillType = pCel->m_nFillType;
	m_dx = pCel->m_dx;
	m_dy = pCel->m_dy;
	m_kx = pCel->m_kx;
	m_ky = pCel->m_ky;
	m_fWidth = pCel->m_fWidth;
	m_nCoverType = pCel->m_nCoverType;
	m_fExtendDis = pCel->m_fExtendDis;
	m_nCenterLineMode = pCel->m_nCenterLineMode;
	m_bDirWithFirstLine = pCel->m_bDirWithFirstLine;
	m_strCellDefName = pCel->m_strCellDefName;

}

void CCell::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if (nIndex == 0)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}

	CellDef cell = cellLib->GetCellDef(nIndex-1);
	
	BOOL bBreakCell  = m_bBreakCell;

	CGeometry *pGeo = pFt->GetGeometry();

	PT_3DEX pt = pGeo->GetDataPoint(0);	

	float   ftrKx, ftrKy, ftrRotateAngle, ftrWidth, fExtendDis;
	int     nCoverType;

	ftrKx = ftrKy = 1;
	ftrRotateAngle = ftrWidth = fExtendDis = 0;
	nCoverType = 0;

	//考虑到符号之间互相压盖的问题，我们在符号解释中，不再使用对象属性中的压盖
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
	{
		CGeoPoint *pPoint = (CGeoPoint*)pGeo;
		ftrKx = pPoint->m_fKx;
		ftrKy = pPoint->m_fKy;
		ftrRotateAngle = pPoint->m_lfAngle*PI/180;
		ftrWidth = pPoint->m_fWidth; 
		//nCoverType = pPoint->m_nCoverType;
		//fExtendDis = pPoint->m_fExtendDis*fDrawScale*fViewScale;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
	{
		//nCoverType = ((CGeoText*)pGeo)->m_nCoverType;
		//fExtendDis = ((CGeoText*)pGeo)->m_fExtendDis*fDrawScale*fViewScale;
	}

	double fCellExtendDis = m_fExtendDis*fDrawScale*fViewScale;
	if( fExtendDis<fCellExtendDis )
		fExtendDis = fCellExtendDis;

	ftrKx = m_kx*fDrawScale*fViewScale*ftrKx;
	ftrKy = m_ky*fDrawScale*fViewScale*ftrKy;

	double dx = m_dx*fDrawScale*fViewScale;
	double dy = m_dy*fDrawScale*fViewScale;

	float fWidth = (m_fWidth+ftrWidth)*fDrawScale*fViewScale;
	if( fWidth==0.0f )
		fWidth = -fDrawScale*fViewScale;

	int nGeoType = pGeo->GetClassType();

	BOOL bUseCover = (nCoverType > 0 || m_nCoverType>0 );

	if( nCoverType==0 && m_nCoverType!=0 )
		nCoverType = m_nCoverType;
	
	GrBuffer grbuf, grbufCover;

	int nBasePtSum = 0;	 // 母线的点数

	float fwid = 1;       // 平行线宽度方向

	CArray<PT_3DEX,PT_3DEX> basepts, ptspar;

	switch(nGeoType)
	{
	case CLS_GEOPOINT:
	case CLS_GEOSURFACEPOINT:
		{
			fRotAngle = m_angle+ftrRotateAngle-fRotAngle;

			DrawCellToBuf(pBuf,nIndex,&cell,&PT_3D(pt.x+dx,pt.y+dy,pt.z),
				ftrKx,ftrKy,fRotAngle,pGeo->GetColor(),fWidth,bBreakCell,bUseCover,nCoverType,fExtendDis);

			if(nGeoType==CLS_GEOSURFACEPOINT)
			{
				pFt->Draw(pBuf,fDrawScale);
			}

			break;
		}
	case CLS_GEODIRPOINT:
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
	case CLS_GEOPARALLEL:
		{
			GetBasePtsAndWidth(pGeo,basepts,fwid);

			int ptsum = GraphAPI::GKickoffSamePoints(basepts.GetData(),basepts.GetSize());
			basepts.SetSize(ptsum);
			
			nBasePtSum = basepts.GetSize();

			if(nBasePtSum < 1)  
				return;
			
			if (nBasePtSum == 1)			
			{
				fRotAngle = m_angle+ftrRotateAngle-fRotAngle;

				DrawCellToBuf(pBuf,nIndex,&cell,&PT_3D(pt.x+dx,pt.y+dy,pt.z),
					ftrKx,ftrKy,fRotAngle,pGeo->GetColor(),fWidth,bBreakCell,bUseCover,nCoverType,fExtendDis);
				return;
			}

			// 过滤掉配置在辅助线但无辅助线的图元
			if (m_nFillType != BaseLine && nGeoType != CLS_GEOPARALLEL && nGeoType != CLS_GEODCURVE)
			{
				return;
			}

			//获得辅助线

			CArray<PT_3DEX,PT_3DEX> ptspar;
			if (nGeoType == CLS_GEOPARALLEL && (m_nFillType != BaseLine || m_nPlaceType == CCell::Center))
			{
				ptspar.Copy(basepts);
				
				double fAidWid = 0;
				if (m_nFillType == AidLine || m_nPlaceType == CCell::Center)
				{
					fAidWid = ((CGeoParallel*)pGeo)->GetWidth();
				}
				else if (m_nFillType == MidLine)
				{
					fAidWid = ((CGeoParallel*)pGeo)->GetWidth()/2;
				}

				if ( !GraphAPI::GGetParallelLine(basepts.GetData(),nBasePtSum,fAidWid,ptspar.GetData()) ) return;

				double dz = ((CGeoParallel*)pGeo)->GetDHeight();
				for (int i=0; i<ptspar.GetSize(); i++)
				{
					ptspar[i].z += dz;
				}

			}
			else if (nGeoType == CLS_GEODCURVE && (m_nFillType != BaseLine || m_nPlaceType == CCell::Center))
			{
				((CGeoDCurve*)pGeo)->GetAssistShape(ptspar);
				int ptsum2 = GraphAPI::GKickoffSamePoints(ptspar.GetData(),ptspar.GetSize());
				ptspar.SetSize(ptsum2);

				ReversePoints(ptspar.GetData(),ptspar.GetSize());
			}

			if( ptspar.GetSize()<=0 && (m_nFillType == AidLine || m_nFillType == MidLine) )
				return;

			//计算定位点 pt，所在序号 index，以及角度 angle
			double angle = 0;

			if (m_nFillType == BaseLine || ( (nGeoType == CLS_GEOPARALLEL || nGeoType == CLS_GEODCURVE) && (m_nFillType == AidLine || m_nFillType == MidLine)) )
			{
				CArray<PT_3DEX,PT_3DEX> retPts;

				if (m_nFillType == BaseLine)
				{
					retPts.Copy(basepts);
				}
				else
				{
					retPts.Copy(ptspar);
				}				
				
				int index = 0;
				if(m_nPlaceType == CCell::Head)
				{
					pt = retPts[0];
					index = 0;
				}
				else if(m_nPlaceType == CCell::Tail)
				{
					pt = retPts[retPts.GetSize()-1];
					index = retPts.GetSize() - 1;
				}
				else if(m_nPlaceType == CCell::Middle)
				{
					index = GraphAPI::GGetMiddlePt(retPts.GetData(),retPts.GetSize(),&pt);
				}
				else if (m_nPlaceType == CCell::Center)
				{
					PT_3DEX ptex0 = basepts.GetAt(0);
					pGeo->GetCenter(&ptex0,&pt);
					pt.x += dx;
					pt.y += dy;
					index = 0;
				}
				
				if (m_nPlaceType != CCell::Center)
				{
					// 对pt沿母线方向和母线的垂直方向分别平移m_dx,m_dy
					PT_3DEX dpt;
					GGetXOffLinePt(retPts.GetData(),retPts.GetSize(),pt,m_nPlaceType,index,dx,dpt);
					GGetPtFromLineWidth(retPts.GetData(),retPts.GetSize(),dpt,dy*fwid,&dpt,TRUE);
					pt = dpt;
					
				}	

				if (m_nPlaceType != CCell::Center || m_bDirWithFirstLine)
				{
					angle = GraphAPI::GGetAngle(retPts[index].x,retPts[index].y,retPts[index+1].x,retPts[index+1].y);
				}

			}	

			double resAngle = m_angle+ftrRotateAngle+angle-fRotAngle;
			if (fwid<0 && m_nPlaceType != CCell::Center)  
				resAngle = (PI-m_angle-ftrRotateAngle)+angle-fRotAngle;				

			// 中心点方向随线段
			PT_3D cenPts1[2];
			BOOL bCenline1 = FALSE;
			if (m_nPlaceType == CCell::Center && m_nCenterLineMode != CCell::cenNULL )
			{
				if (m_nCenterLineMode == CCell::cenHead2Tail)
				{
					cenPts1[0] = basepts[0];
					cenPts1[1] = basepts[basepts.GetSize()-1];
					bCenline1 = TRUE;
				}
				else if (m_nCenterLineMode == CCell::cenMid2Mid && (nGeoType == CLS_GEOPARALLEL || nGeoType == CLS_GEODCURVE) )
				{
					PT_3DEX mids[2];
					GraphAPI::GGetMiddlePt(basepts.GetData(),basepts.GetSize(),&mids[0]);
					GraphAPI::GGetMiddlePt(ptspar.GetData(),ptspar.GetSize(),&mids[1]);
					cenPts1[0] = mids[0];
					cenPts1[1] = mids[1];
					bCenline1 = TRUE;
				}
			}

			// 显示图元的部分不画线
			PT_3D cenPts2[2];
			if( bCenline1 && nIndex>0 && nIndex<=cellLib->GetCellDefCount() )
			{
				CellDef cell = cellLib->GetCellDef(nIndex-1);

				GrBuffer buf;
				buf.AddBuffer(cell.m_pgr,pt.z);
				
				Envelope e = buf.GetEnvelope();

				// 需要空出的线长
				float akx, aky;
				akx = 0.5*e.Width()*ftrKx;
				aky = 0.5*e.Height()*ftrKy;
				float fEmptyLen = akx>aky?akx:aky;

				float fHalfCenlineLen = GraphAPI::GGet3DDisOf2P(cenPts1[0],cenPts1[1])*0.5;

				if (fabs(fHalfCenlineLen) < 1e-6)
				{
					goto zerolen;
				}

				PT_3D cenPt;
				GraphAPI::GGetMiddlePt(cenPts1,2,&cenPt);	
				
				// 向量法计算空出线的两端点
				float ax, ay;
				ax = (cenPts1[0].x - cenPt.x)/fHalfCenlineLen;
				ay = (cenPts1[0].y - cenPt.y)/fHalfCenlineLen;

				cenPts2[0].x = ax*fEmptyLen + cenPt.x;
				cenPts2[0].y = ay*fEmptyLen + cenPt.y;
				cenPts2[0].z = cenPt.z;

				ax = (cenPts1[1].x - cenPt.x)/fHalfCenlineLen;
				ay = (cenPts1[1].y - cenPt.y)/fHalfCenlineLen;
				
				cenPts2[1].x = ax*fEmptyLen + cenPt.x;
				cenPts2[1].y = ay*fEmptyLen + cenPt.y;
				cenPts2[1].z = cenPt.z;

				pBuf->BeginLineString(pGeo->GetColor(),0);
				if (GraphAPI::GGet3DDisOf2P(cenPts1[0],cenPts2[0]) < GraphAPI::GGet3DDisOf2P(cenPts1[0],cenPts2[1]))
				{
					pBuf->MoveTo(&cenPts1[0]);
					pBuf->LineTo(&cenPts2[0]);
					pBuf->MoveTo(&cenPts1[1]);
					pBuf->LineTo(&cenPts2[1]);
				}
				else
				{
					pBuf->MoveTo(&cenPts1[0]);
					pBuf->LineTo(&cenPts2[1]);
					pBuf->MoveTo(&cenPts1[1]);
					pBuf->LineTo(&cenPts2[0]);
				}								

				pBuf->End();

			}
zerolen:
			//绘制图元			

			DrawCellToBuf(pBuf,nIndex,&cell,&pt,
					ftrKx,ftrKy,resAngle,pGeo->GetColor(),fWidth,bBreakCell,bUseCover,nCoverType,fExtendDis);

			
			break;
		
		}
	default:
		{
			break;
		}
	}
	

}


BOOL CCell::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,TRUE);
	int nSave = m_nCoverType;

	m_nCoverType = 0;

	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);

	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);

	m_nCoverType = nSave;

	return TRUE;
}


BOOL CCell::ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,FALSE);
	int nSave = m_nCoverType;
	
	m_nCoverType = 0;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	m_nCoverType = nSave;
	
	return TRUE;
}


BOOL CCell::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();

		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			if( xmlFile.FindElem(XMLTAG_DX) )
			{
				data = xmlFile.GetData();
				m_dx = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_DY) )
			{
				data = xmlFile.GetData();
				m_dy = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_ANGLE) )
			{
				data = xmlFile.GetData();
				m_angle = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_KX) )
			{
				data = xmlFile.GetData();
				m_kx = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_KY) )
			{
				data = xmlFile.GetData();
				m_ky = _ttof(data);
			}

			
			if( xmlFile.FindElem(XMLTAG_PLACETYPE) )
			{
				data = xmlFile.GetData();
				m_nPlaceType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_FILLTYPE) )
			{
				data = xmlFile.GetData();
				m_nFillType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_CENLINEMODE) )
			{
				data = xmlFile.GetData();
				m_nCenterLineMode = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_DIRWITHFIRSTLINE) )
			{
				data = xmlFile.GetData();
				m_bDirWithFirstLine = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_COVERTYPE) )
			{
				data = xmlFile.GetData();
				m_nCoverType = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_EXTENDDIS) )
			{
				data = xmlFile.GetData();
				m_fExtendDis = _ttof(data);
			}		

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_CELLDEFNAME) )
			{
				data = xmlFile.GetData();
				m_strCellDefName = data;
			}

			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
}

void CCell::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_dx);
			xmlFile.AddElem(XMLTAG_DX,data);

			data.Format(_T("%f"),m_dy);
			xmlFile.AddElem(XMLTAG_DY,data);

			data.Format(_T("%f"),m_angle);
			xmlFile.AddElem(XMLTAG_ANGLE,data);

			data.Format(_T("%f"),m_kx);
			xmlFile.AddElem(XMLTAG_KX,data);
			
			data.Format(_T("%f"),m_ky);
			xmlFile.AddElem(XMLTAG_KY,data);

			data.Format(_T("%d"),m_nPlaceType);
			xmlFile.AddElem(XMLTAG_PLACETYPE,data);

			data.Format(_T("%d"),m_nFillType);
			xmlFile.AddElem(XMLTAG_FILLTYPE,data);

			data.Format(_T("%d"),m_nCenterLineMode);
			xmlFile.AddElem(XMLTAG_CENLINEMODE,data);

			data.Format(_T("%d"),m_bDirWithFirstLine);
			xmlFile.AddElem(XMLTAG_DIRWITHFIRSTLINE,data);

			data.Format(_T("%d"),m_nCoverType);
			xmlFile.AddElem(XMLTAG_COVERTYPE,data);

			data.Format(_T("%f"),m_fExtendDis);
			xmlFile.AddElem(XMLTAG_EXTENDDIS,data);

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);

			xmlFile.AddElem(XMLTAG_CELLDEFNAME,m_strCellDefName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();

}

BOOL CCell::IsSupportGeoType(int nGeoType)
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDashLinetype::CDashLinetype()
{
	m_nType = SYMTYPE_DASHLINETYPE;

	m_bUseSelfcolor = FALSE;
	m_nColor = 0;
	m_fBaseOffset = 0;
	m_fXOffset0 = 0;
	m_fIndent = 0;
	m_fWidth = 0;
	m_nPlaceType = CDashLinetype::BaseLine;
	m_nIndentType = IndentType::Node;

	m_bBreakLinetype = TRUE;

	m_bAdjustXOffset = FALSE;

	m_bDashAlign = TRUE;
}

CDashLinetype::~CDashLinetype()
{

}	

void CDashLinetype::CopyFrom(CSymbol *pSymbol)
{
	CDashLinetype *pDash = (CDashLinetype*)pSymbol;
	m_fBaseOffset = pDash->m_fBaseOffset;
	m_fWidth = pDash->m_fWidth;
	m_nPlaceType = pDash->m_nPlaceType;
	m_fXOffset0 = pDash->m_fXOffset0;
	m_fIndent = pDash->m_fIndent;
	m_nIndentType = pDash->m_nIndentType;
	m_bUseSelfcolor = pDash->m_bUseSelfcolor;
	m_nColor = pDash->m_nColor;
	m_strBaseLinetypeName = pDash->m_strBaseLinetypeName;
	m_bAdjustXOffset = pDash->m_bAdjustXOffset;
	m_bDashAlign = pDash->m_bDashAlign;
}

BOOL CDashLinetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_BASEOFF) )
			{
				data = xmlFile.GetData();
				m_fBaseOffset = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_PLACETYPE) )
			{
				data = xmlFile.GetData();
				m_nPlaceType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
			{
				data = xmlFile.GetData();
				m_bUseSelfcolor = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_COLOR) )
			{
				data = xmlFile.GetData();
				m_nColor = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_INDENTTYPE) )
			{
				data = xmlFile.GetData();
				m_nIndentType = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_INDENT) )
			{
				data = xmlFile.GetData();
				m_fIndent = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_STARTOFF) )
			{
				data = xmlFile.GetData();
				m_fXOffset0 = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}

			if( xmlFile.FindElem(XMLTAG_ADJUSTXOFFSET) )
			{
				data = xmlFile.GetData();
				m_bAdjustXOffset = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_DASHALIGN) )
			{
				data = xmlFile.GetData();
				m_bDashAlign = _ttoi(data);
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CDashLinetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fBaseOffset);
			xmlFile.AddElem(XMLTAG_BASEOFF,data);
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			data.Format(_T("%d"),m_nPlaceType);
			xmlFile.AddElem(XMLTAG_PLACETYPE,data);

			data.Format(_T("%d"),m_bUseSelfcolor);
			xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);

			data.Format(_T("%d"),m_nColor);
			xmlFile.AddElem(XMLTAG_COLOR,data);

			data.Format(_T("%d"),m_nIndentType);
			xmlFile.AddElem(XMLTAG_INDENTTYPE,data);			

			data.Format(_T("%f"),m_fIndent);
			xmlFile.AddElem(XMLTAG_INDENT,data);

			data.Format(_T("%f"),m_fXOffset0);
			xmlFile.AddElem(XMLTAG_STARTOFF,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);

			data.Format(_T("%d"),m_bAdjustXOffset);
			xmlFile.AddElem(XMLTAG_ADJUSTXOFFSET,data);

			data.Format(_T("%d"),m_bDashAlign);
			xmlFile.AddElem(XMLTAG_DASHALIGN,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

}

BOOL CDashLinetype::GetAssistPtsWithHeight(PT_3DEX *pts, int nSum, float fAssistWid, CArray<PT_3DEX,PT_3DEX> &arrRet, CGeoParallel *pGeo)
{
	if (!pts || nSum <=0) return FALSE;

	// 辅助线
	if (fabs(fAssistWid) > GraphAPI::GetDisTolerance())
	{	
		nSum = GraphAPI::GKickoffSamePoints(pts,nSum);
		
		arrRet.SetSize(nSum);
		
		GraphAPI::GGetParallelLine(pts,nSum,fAssistWid,arrRet.GetData());

		for (int i=0; i<nSum; i++)
		{
			arrRet[i].pencode = pts[i].pencode;
		}		
		
		if (pGeo)
		{
			double wid = pGeo->GetWidth();
			double fHeight = pGeo->GetDHeight();
			if (fabs(wid) > GraphAPI::GetDisTolerance() && fabs(fHeight) > GraphAPI::GetDisTolerance())
			{
				double fKz = fabs(fAssistWid/wid);
				for (int i=0; i<nSum; i++)
				{
					arrRet[i].z += fKz*fHeight;
				}
			}
			
		}
		
		
	}

	return TRUE;
}


//用虚线显示线串时，有时结尾的地方可能为虚的部分，为了改善显示效果，可以把虚线沿着线串挪动一定长度，使得头尾都是实线部分；
//下面的函数就是计算这个挪动的长度，并尽量让头尾两段实线等长
//对于同一个线串，可以有多种挪动长度满足要求，也可能无解；
//对于前者，我们计算一个合理值即可，对于后者，返回值为-1；
float CalcAdjustLenOfDashLine(int dwStyleCount, const float *lpStyle, double lines_alllen, double len_min)
{
	//这里对 dwStyleCount 和 lpStyle 不再作有效性判断了
	//将dwStyleCount 处理为偶数
	dwStyleCount = ((dwStyleCount>>1)<<1);

	double dash_alllen = 0;

	//正值化
	CArray<float,float> arrStyles;
	arrStyles.SetSize(dwStyleCount);
	float *pStyles = arrStyles.GetData();
	for( int i=0; i<dwStyleCount; i++)
	{
		pStyles[i] = fabs(lpStyle[i]);
		dash_alllen += pStyles[i];
	}

	len_min = fabs(len_min);

	//计算碎部长度
	double left_len = lines_alllen-floor(lines_alllen/dash_alllen)*dash_alllen;

	//首先判断是否存在某个实线长度大于left_len，那么这个地方就可以成为挪动后的位置；如果存在的话就计算最佳的一段
	double len_max = -1, len_t = -1;
	int k = -1;

	for( i=0; i<dwStyleCount; i+=2)
	{
		if( left_len<=pStyles[i] && pStyles[i]>len_min )
		{
			//len_t是前推距离
			len_t = (pStyles[i]+left_len)*0.5;

			if( len_t>len_max )
			{
				len_max = len_t;
				k = i;
			}
		}		
	}

	if( k>=0 )
	{
		double len0 = 0;
		for( i=0; i<k; k++)
		{
			len0 += pStyles[i];
		}
		return (len0+pStyles[k]-len_max);
	}

	//再判断是否可以跨过了一个（或者多个）虚线部分，同时又不超过前后两段实线部分，那么这个地方也可以成为挪动后的位置
	//最多跨越 dwStyleCount/2 个虚线部分，j是虚线部分的数目
	int k1 = -1, k2 = -1;
	for( int j=1; j<=(dwStyleCount/2); j++)
	{
		int j2 = j+j;
		//从起点往后推，直到所有可能跨越j个虚线部分的情况都尝试到
		for( i=1; i<dwStyleCount; i+=j2 )
		{
			//计算从i-1到i-1+j2的长度（这中间跨越了j个虚线部分）
			double len0 = 0, len1 = 0; //len0包括前后两段实线长，len1不包括前后两段
			double style_0 = 0, style_j2 = 0;
			for( int m=0; m<j2+1; m++)
			{
				int i1 = (m+i-1)%dwStyleCount;
				len0 += pStyles[i1];
				if( m!=0 && m!=j2 )
				{
					len1 += pStyles[i1];
				}
				if( m==0 )
					style_0 = pStyles[i1];
				else if( m==j2 )
					style_j2 = pStyles[i1];
			}
			//碎部长度大于虚线长度并且小于加上前后实线的长度
			if( left_len<=len0 && left_len>len1+len_min )
			{
				//len_t是前推距离
				len_t = (left_len-len1)*0.5;

				//平分的长度大于前后的实线长度，就以实线长度为前推距离
				if( len_t>style_0 && style_0>=len_min )
					len_t = style_0;
				else if( len_t>style_j2 && style_j2>=len_min )
					len_t = style_j2;
				
				if( len_t>len_max )
				{
					len_max = len_t;
					k1 = j;
					k2 = i-1;
				}
			}		
		}

		if( k2>=0 )
		{
			double len0 = 0;
			for( i=0; i<k2; i++)
			{
				len0 += pStyles[i];
			}
			return (len0+pStyles[k2]-len_max);
		}
	}

	return -1;
}

//放置虚线串，更小的函数，受到 DashLine 调用
static void PlaceDashLineString(PT_3DEX *pts, int nSum, double *pLen, double xoff, int dwStyleCount,const float *lpStyle, float fDrawScale, GrBuffer *pBuf)
{
	// 隐藏线
	int i, j;
	double alllen = 0;
	for (i=0; i<dwStyleCount; i++)
	{
		alllen += (lpStyle[i]);
	}
	
	if (alllen < 0.2*fDrawScale ) return;
	
	double dis0 = xoff - ceil(xoff/alllen)*alllen;
	double dis = dis0;
	
	GrBuffer& buf = *pBuf;
	
	buf.MoveTo(&pts[0]);
	
	BOOL bAddStyleLen = FALSE;
	
	j = 0;
	for (i=1; i<nSum; i++)
	{
		// 复杂面
		if (pts[i].pencode == penMove)
		{
			buf.MoveTo(&pts[i]);	
			dis = dis0;
			j = 0;
			bAddStyleLen = FALSE;
			continue;
		}

		if(pLen[i]>alllen*10000)
		{
			buf.LineTo(&pts[i]);
			bAddStyleLen = TRUE;
			continue;
		}
		
		//循环处理线型，直到下一个顶点
		while( 1 )
		{
			if( !bAddStyleLen )
				dis += lpStyle[j];			
			
			if (dis <= pLen[i])
			{
				double ratio = (dis-pLen[i-1]) / (pLen[i]-pLen[i-1]);
				
				if( ratio>0 )
				{
					PT_3D t3d;
					t3d.x = pts[i-1].x + ratio * (pts[i].x - pts[i-1].x);
					t3d.y = pts[i-1].y + ratio * (pts[i].y - pts[i-1].y);
					t3d.z = pts[i-1].z + ratio * (pts[i].z - pts[i-1].z);
					
					if (j%2 == 0) 
						buf.LineTo(&t3d);
					else 
						buf.MoveTo(&t3d);
				}
				
				j = (++j)%dwStyleCount;
				
				bAddStyleLen = FALSE;
			}
			else
			{				
				if (j%2 == 0) buf.LineTo(&pts[i]);
				
				bAddStyleLen = TRUE;
				
				break;	
			}
		}		
	}
}

//PlaceDashLineString 的专门针对虚线对齐中循环处理的优化版本，pts中总是只有两个点，len为两点的距离
static void PlaceDashLineString_FastLoop(PT_3DEX *pts, double len, double xoff, int dwStyleCount,const float *lpStyle, float dash_alllen, float fDrawScale, GrBuffer *pBuf)
{
	// 边界起点
	if (pts[1].pencode == penMove)
	{
		return;
	}
	
	double dis = xoff - ceil(xoff/dash_alllen)*dash_alllen;
	
	GrBuffer& buf = *pBuf;
	
	buf.MoveTo(&pts[0]);
	
	if(len>dash_alllen*10000)
	{
		buf.LineTo(&pts[1]);
		return;
	}
	
	int j = 0;
		
	//循环处理线型，直到下一个顶点
	while( 1 )
	{
		dis += lpStyle[j];			
		
		if (dis <= len)
		{
			double ratio = dis / len;
			
			if( ratio>0 )
			{
				PT_3D t3d;
				t3d.x = pts[0].x + ratio * (pts[1].x - pts[0].x);
				t3d.y = pts[0].y + ratio * (pts[1].y - pts[0].y);
				t3d.z = pts[0].z + ratio * (pts[1].z - pts[0].z);
				
				if (j%2 == 0) 
					buf.LineTo(&t3d);
				else 
					buf.MoveTo(&t3d);
			}
			
			j = (++j)%dwStyleCount;
		}
		else
		{				
			if (j%2 == 0) buf.LineTo(&pts[1]);
			
			break;	
		}
	}
}


//设置平行线的辅助线的高程
static void SetParallelDHeight(PT_3DEX *pts, int nSum, float fAssistWid, CGeometry *pGeo)
{
	if (pGeo && pGeo->GetClassType() == CLS_GEOPARALLEL)
	{
		double wid = ((CGeoParallel*)pGeo)->GetWidth();
		double fHeight = ((CGeoParallel*)pGeo)->GetDHeight();
		if (fabs(wid) > GraphAPI::GetDisTolerance() && fabs(fHeight) > GraphAPI::GetDisTolerance())
		{
			double fKz = fabs(fAssistWid/wid);
			for (int i=0; i<nSum; i++)
			{
				pts[i].z += fKz*fHeight;
			}
		}		
	}
}

// 将pts用虚线格式表达，用于符号打散
// bAdjustDash，自动调整虚线，使得尽量头尾是实线，中间是虚线
static void DashLine(PT_3DEX *pts, int nSum, GrBuffer *pBuf, COLORREF clr, float width, float fViewDrawScale, BOOL bGrdWid, float fDrawScale, BOOL bUseSelfcolor, double xoff, int dwStyleCount,const float *lpStyle, int nStyleIndex, BOOL bBreakLinetype, BOOL bAdjustDash=FALSE)
{
	if (!pts || nSum<=1 || !pBuf)  return;
	
	// 隐藏线
	int i, j;
	float alllen = 0, hidelen = 0;
	for (i=0; i<dwStyleCount; i++)
	{
		alllen += (lpStyle[i]);
		if( (i%2)==1 )
		{
			hidelen += (lpStyle[i]);
		}
	}
	
	if (alllen<0.0001*fDrawScale ) return;
	
	// 实线
	if (dwStyleCount <= 1 || hidelen < 0.0001*fDrawScale || !bBreakLinetype )
	{
		float wid = width;

		PT_3DEX *pRet = pts;

		if( (dwStyleCount <= 1 || hidelen < 0.0001*fDrawScale ) )
			nStyleIndex = 0;

		pBuf->BeginLineString(clr,wid,bGrdWid,nStyleIndex,fDrawScale,bUseSelfcolor);

		for (int i=0; i<nSum; i++)
		{
			if (i==0||(pRet[i].pencode==penMove))
			{
				pBuf->MoveTo(&pRet[i]);				
			}				
			else
				pBuf->LineTo(&pRet[i]);
			
		}

		pBuf->End();

		return;
	}

	// 计算线串的长度
	double *pLen = new double[nSum];
	
	CalcLineStringLens(pts,nSum,pLen);

	if( xoff==0 && bAdjustDash )
	{
		double dis = CalcAdjustLenOfDashLine(dwStyleCount,lpStyle,pLen[nSum-1],0.1*fDrawScale);
		if( dis>0 )
			xoff = -dis;
	}

	//绘制虚线串
	pBuf->BeginLineString(clr,width,bGrdWid,0,fDrawScale,bUseSelfcolor);

	PlaceDashLineString(pts,nSum,pLen,xoff,dwStyleCount,lpStyle,fDrawScale,pBuf);

	pBuf->End();
	
	delete []pLen;
}


void CDashLinetype::DashLine(PT_3DEX *pts, int nSum, GrBuffer *pBuf, COLORREF clr, float width, float fViewDrawScale, BOOL bGrdWid, float fDrawScale, float xoff, int dwStyleCount,const float *lpStyle, int nStyleIndex, BOOL bBreakLinetype,BOOL bAdjustXOffset,float fAssistWid, CGeometry *pGeo)
{
	if (!pts || nSum<=1 || !pBuf)  return;
	
	// 隐藏线
	int i;
	float alllen = 0, hidelen = 0;
	for (i=0; i<dwStyleCount; i++)
	{
		alllen += (lpStyle[i]);
		if( (i%2)==1 )
		{
			hidelen += (lpStyle[i]);
		}
	}

	if (alllen < 0.0001*fDrawScale ) return;
	
	// 实线
	if (dwStyleCount <= 1 || hidelen < 0.0001*fDrawScale || !bBreakLinetype )
	{
		if( dwStyleCount <= 1 || hidelen < 0.0001*fDrawScale )
			nStyleIndex = 0;

		float wid = width;

		PT_3DEX *pRet = pts;
		// 辅助线
		if (fabs(fAssistWid) > GraphAPI::GetDisTolerance())
		{				
			PT_3DEX *pRet1 = new PT_3DEX[nSum];
			for ( i=0; i<nSum; i++)
			{
				COPY_3DPT(pRet1[i],pts[i]);
				pRet1[i].pencode = pts[i].pencode;
			}		
			
			GraphAPI::GGetParallelLine(pts,nSum,fAssistWid,pRet1);

			SetParallelDHeight(pRet1,nSum,fAssistWid,pGeo);

			nSum = GraphAPI::GKickoffSamePoints(pRet1,nSum);

			pRet = pRet1;			
		}

		pBuf->BeginLineString(clr,wid,bGrdWid,nStyleIndex,fDrawScale,m_bUseSelfcolor);

		for ( i=0; i<nSum; i++)
		{
			if (i==0||(pRet[i].pencode==penMove))
			{
				pBuf->MoveTo(&pRet[i]);				
			}				
			else
				pBuf->LineTo(&pRet[i]);
			
		}

		pBuf->End();

		if (pRet != pts)
		{
			delete []pRet;
		}
		return;
	}
	
	//有宽度偏移的线
	if( fabs(fAssistWid)>GraphAPI::GetDisTolerance() && m_bDashAlign )
	{		
		PT_3DEX *pRet = new PT_3DEX[nSum];
		for ( i=0; i<nSum; i++)
		{
			COPY_3DPT(pRet[i],pts[i]);
			pRet[i].pencode = pts[i].pencode;
		}		
		
		GraphAPI::GGetParallelLine(pts,nSum,fAssistWid,pRet);

		SetParallelDHeight(pRet,nSum,fAssistWid,pGeo);
		
		// 计算线串的长度数组
		double *pLen1 = new double[nSum];
		double *pLen2 = new double[nSum];
		double *pLen3 = new double[nSum];

		CalcLineStringLens(pts,nSum,pLen1);
		CalcLineStringLens(pRet,nSum,pLen2);

		//计算pRet的点投影到pts上的垂足与pts上当前点的距离，这个距离便于后面计算各个线段的dash的起点偏移
		pLen3[0] = 0;
		for( i=1; i<nSum; i++)
		{
			//当前垂足偏移 = 母线的当前线段长度 - 辅助线当前线段长度 - 上一个垂足偏移
			pLen3[i] = (pLen1[i]-pLen1[i-1]) - (pLen2[i]-pLen2[i-1]) - pLen3[i-1];
		}
		
		double xoff2 = xoff;
		
		GrBuffer buf;

		// 逐个线段绘制，与母线保持对齐
		buf.BeginLineString(clr,width,bGrdWid,0,fDrawScale,m_bUseSelfcolor);
		
		for( i=0; i<nSum-1; i++)
		{			
			PlaceDashLineString_FastLoop(pRet+i,pLen2[i+1]-pLen2[i],xoff2,dwStyleCount,lpStyle,alllen,fDrawScale,&buf);
			
			xoff2 = -(pLen1[i+1] + pLen3[i+1]);
		}
		
		buf.End();

		buf.KickoffSamePoints();
		
		pBuf->AddBuffer(&buf);
		
		delete[] pRet;
		delete[] pLen1;
		delete[] pLen2;
		delete[] pLen3;

		return;
	}
	//母线
	else
	{
		GrBuffer buf;
		double *pLen = new double[nSum];

		PT_3DEX *pts_t = pts;
		if( fabs(fAssistWid)>GraphAPI::GetDisTolerance() )
		{
			PT_3DEX *pRet = new PT_3DEX[nSum];
			for ( int i=0; i<nSum; i++)
			{
				COPY_3DPT(pRet[i],pts[i]);
				pRet[i].pencode = pts[i].pencode;
			}

			GraphAPI::GGetParallelLine(pts,nSum,fAssistWid,pRet);

			pts_t = pRet;
		}

		CalcLineStringLens(pts_t,nSum,pLen);

		if( xoff==0.0f && bAdjustXOffset )
		{
			float dis = CalcAdjustLenOfDashLine(dwStyleCount,lpStyle,pLen[nSum-1],0.1*fDrawScale);
			if( dis>0 )
				xoff = -dis;
		}

		//绘制虚线串
		buf.BeginLineString(clr,width,bGrdWid,0,fDrawScale,m_bUseSelfcolor);

		PlaceDashLineString(pts_t,nSum,pLen,xoff,dwStyleCount,lpStyle,fDrawScale,&buf);

		buf.End();	

		buf.KickoffSamePoints();

		pBuf->AddBuffer(&buf);

		delete []pLen;	

		if( pts_t!=pts )
		{
			delete[] pts_t;
		}
	}
	
}


// 获取基线上指定长度的点
template<class T>
static int GGetPtOfLine(const T *pts,const int &nSum, double len, T* opt) 
{
	if(pts==NULL || nSum<1 || opt==NULL || len < 0)
		return -1;
	
	if (nSum == 1)
	{ 
		if (fabs(len) < 1e-4)
		{
			*opt = pts[0];
			return 0;
		}
		else
		{
			return -1;
		}
		
	}
	
	//计算每一线段的长度,存储到pDis中
	double *pDis = new double[nSum];
	if (!pDis) return -1;
	
	double  *p = pDis;
	double xold, yold, zold;
	
	const T *pt = pts;
	int nFlag = nSum;
	while( nFlag-- )
	{
		if (p==pDis) *p = 0.0;
		else
		{
			*(p) = *(p-1) + sqrt( (pt->x-xold)*(pt->x-xold) +
				(pt->y-yold)*(pt->y-yold) /*+
				(pt->z-zold)*(pt->z-zold) */);
		}
		xold = pt->x;
		yold = pt->y;
		zold = pt->z;
		
		p++;
		pt++;
	}
	
	double dLen =  pDis[nSum-1];
	if (dLen < 1e-4 || len > dLen) 
	{
		delete []pDis;
		*opt = pts[1];
		return -1;
	}
	
	//double dMid = dLen/2;
	
	double dis = 0.0;
	for (int i=1; i < nSum; i++)
	{
		if(len <= pDis[i])
		{
			double ratio = (len-pDis[i-1]) / (pDis[i]-pDis[i-1]);
			opt->x = pts[i-1].x + ratio * (pts[i].x - pts[i-1].x);
			opt->y = pts[i-1].y + ratio * (pts[i].y - pts[i-1].y);
			opt->z = pts[i-1].z + ratio * (pts[i].z - pts[i-1].z);
			delete []pDis;
			return i-1;
		}
	}
	delete []pDis;
	return -1;
}

// 获取基线上指定长度的点
template<class T>
static int GGetPtOfLine(T pt0, T pt1, double len, T* opt)
{
	double len0 = GraphAPI::GGet2DDisOf2P(pt0,pt1);
	if( len0<GraphAPI::g_lfDisTolerance )
	{
		*opt = pt0;
		opt->x += len;
		return 0;
	}

	double ratio = len/len0;
	opt->x = pt0.x + ratio * (pt1.x - pt0.x);
	opt->y = pt0.y + ratio * (pt1.y - pt0.y);
	opt->z = pt0.z + ratio * (pt1.z - pt0.z);
	return 0;
}

void CDashLinetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	// 线型库找不到线此线型
	if (nIndex == 0)
	{
		CGeometry *pGeo = pFt->GetGeometry();
		int nGeoType = pGeo->GetClassType();
		if(nGeoType==CLS_GEOSURFACE || nGeoType==CLS_GEOMULTISURFACE )
		{
			if(pGeo->IsHideSymbolizeSurfaceBound())
				return;
			
			if(pGeo->IsHideSymbolizeSurfaceInnerBound() && nGeoType==CLS_GEOMULTISURFACE )
			{
				CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
				int nGeo = pMGeo->GetSurfaceNum();
				if(nGeo>0)
				{						
					CGeoSurface *pNewGeo = new CGeoSurface();
					pNewGeo->CopyFrom(pGeo);
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pMGeo->GetSurface(0,arrPts);
					pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					pNewGeo->Draw(pBuf,fDrawScale);
					delete pNewGeo;
				}
				return;
			}			
		}

		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid, ftrXoff = 0;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
		ftrXoff = pCurve->m_fLinetypeXoff;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		ftrWid = ((CGeoSurface*)pGeo)->m_fLineWidth;
	}

	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	int symflagEnum = pGeo->GetSymbolizeFlagEnum();

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	int nGeoType = pGeo->GetClassType();

	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOPARALLEL:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
		{
			int nGeo = 1;

			CArray<CGeometry*,CGeometry*> arrGeos;
			arrGeos.Add(pGeo);
			if( nGeoType==CLS_GEOMULTISURFACE )
			{
				arrGeos.RemoveAll();				

				CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
				nGeo = pMGeo->GetSurfaceNum();
				for( int m=0; m<nGeo; m++)
				{
					if(pMGeo->IsHideSymbolizeSurfaceBound())
						continue;
					
					if(pMGeo->IsHideSymbolizeSurfaceInnerBound() && m>0)
						continue;

					CGeoSurface *pNewGeo = new CGeoSurface();
					pNewGeo->CopyFrom(pGeo);

					CArray<PT_3DEX,PT_3DEX> arrPts;
					pMGeo->GetSurface(m,arrPts);
					pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					arrGeos.Add(pNewGeo);
				}
				nGeo = arrGeos.GetSize();
			}
			else if( nGeoType==CLS_GEOSURFACE )
			{
				if(pGeo->IsHideSymbolizeSurfaceBound())
					nGeo = 0;
			}

			for(int m=0; m<nGeo; m++)
			{
				pGeo = arrGeos[m];

				// 平行线宽度方向
				float fwid = 1;
				CArray<PT_3DEX,PT_3DEX> basepts;
				GetBasePtsAndWidth(pGeo,basepts,fwid);

				SimpleCompressPoints(basepts,CSymbol::m_tolerance);

				if (basepts.GetSize() < 1)
					break;

				int  nStyle = 0;
				float styles[8] = {0}, style_allLen = 0;

				BaseLineType linetype = baselineLib->GetBaseLineType(nIndex-1);		

				nStyle = linetype.m_nNum;
				for( int i=0; i<linetype.m_nNum; i++)
				{
					styles[i] = fabs(linetype.m_fLens[i])*fDrawScale*ftrLinetypeScale;	
					style_allLen += styles[i];
				}

				DWORD nColor = pGeo->GetColor();
				if (m_bUseSelfcolor)
				{
					nColor = m_nColor;
				}

				GrBuffer grbuf;

				{
					float fAssistWid = 0;
					CArray<PT_3DEX,PT_3DEX> offpts;

					if (m_nPlaceType == BaseLine)
					{
						fAssistWid = m_fBaseOffset*fwid*ftrLinetypeScale*fDrawScale*fViewScale;
						offpts.Copy(basepts);
					}
					else if (nGeoType == CLS_GEODCURVE && m_nPlaceType == AidLine)
					{
						fAssistWid = m_fBaseOffset*fwid*ftrLinetypeScale*fDrawScale*fViewScale;
						((CGeoDCurve*)pGeo)->GetAssistShape(offpts);
						ReversePoints(offpts.GetData(),offpts.GetSize());
					}
					else if (nGeoType==CLS_GEOPARALLEL && (m_nPlaceType == AidLine || m_nPlaceType == MidLine))
					{
						offpts.Copy(basepts);
						
						if (m_nPlaceType == AidLine)
						{
							fAssistWid = m_fBaseOffset*fwid*ftrLinetypeScale*fDrawScale*fViewScale + ((CGeoParallel*)pGeo)->GetWidth();
						}
						else if (m_nPlaceType == MidLine)
						{
							fAssistWid = m_fBaseOffset*fwid*ftrLinetypeScale*fDrawScale*fViewScale + ((CGeoParallel*)pGeo)->GetWidth()/2;
						}
					}

					float xoff = m_fXOffset0*ftrLinetypeScale*fDrawScale*fViewScale + ftrXoff;
					float indent = fabs(m_fIndent)*ftrLinetypeScale*fDrawScale*fViewScale;
					BOOL bXOff = (fabs(xoff)>1e-4);
					BOOL bIndent = (fabs(indent)>1e-4);

					//偏移和缩进
					if (bXOff || bIndent)
					{			
						if (bIndent)
						{
							if (m_nIndentType == Node )
							{
								CArray<float,float> arrRatio1, arrRatio2;
								CArray<PT_3DEX,PT_3DEX> dpt1, dpt2;
								CUIntArray arrIndex1, arrIndex2;
								GetXOffLine(offpts.GetData(),offpts.GetSize(),indent,dpt1,arrIndex1,arrRatio1);
								GetXOffLine(offpts.GetData(),offpts.GetSize(),-indent,dpt2,arrIndex2,arrRatio2);
								
								if (arrIndex1.GetSize() <= 0 || arrIndex2.GetSize() <= 0)
									break;
								
								for (int i=0; i<arrIndex1.GetSize()&&i<arrIndex2.GetSize(); i++)
								{
									PT_3DEX arrPts[2];
									if (arrIndex1[i] == arrIndex2[i])
									{
										arrPts[0] = dpt1[i];
										arrPts[1] = dpt2[i];

										//没有首点电杆，首点不缩进
										if( (symflagEnum==SYMFLAG_ENUM_NODIANGAN1||symflagEnum==SYMFLAG_ENUM_NODIANGAN3) && i==0 )
										{
											arrPts[0] = offpts[0];
										}
										//没有尾点电杆，尾点不缩进
										if( (symflagEnum==SYMFLAG_ENUM_NODIANGAN2||symflagEnum==SYMFLAG_ENUM_NODIANGAN3) && i==(offpts.GetSize()-2) )
										{
											arrPts[1] = offpts[offpts.GetSize()-1];
										}

										DashLine(arrPts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale*fViewScale,0,nStyle,styles,nIndex,bBreakLinetype,m_bAdjustXOffset,fAssistWid,pGeo);
									}
								}
							}
							else if (m_nIndentType==HeadTail)
							{
								CArray<PT_3DEX,PT_3DEX> dpt0;
								dpt0.Copy(offpts);

								double len = GraphAPI::GGetAllLen2D(offpts.GetData(),offpts.GetSize());

								PT_3DEX pt1, pt2;
								int index1 = -1, index2 = -1;	
								index1 = GGetPtOfLine(offpts.GetData(),offpts.GetSize(),indent,&pt1);
								index2 = GGetPtOfLine(offpts.GetData(),offpts.GetSize(),len-indent,&pt2);

								if (index1 < 0 || index2 < 0)
										break;

								dpt0[0] = pt1;
								dpt0[offpts.GetSize()-1] = pt2;

								DashLine(dpt0.GetData(),dpt0.GetSize(),&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale*fViewScale,0,nStyle,styles,nIndex,bBreakLinetype,m_bAdjustXOffset,fAssistWid,pGeo);
								
							}
							else if (m_nIndentType == Mid)
							{
								double len = GraphAPI::GGetAllLen2D(offpts.GetData(),offpts.GetSize());
								float fIndex = fabs(indent);
								if (len/2 - fIndex > 0)
								{
									PT_3DEX pt1, pt2;
									int index1 = -1, index2 = -1;								
									index1 = GGetPtOfLine(offpts.GetData(),offpts.GetSize(),len/2-fIndex,&pt1);
									index2 = GGetPtOfLine(offpts.GetData(),offpts.GetSize(),len/2+fIndex,&pt2);

									if (index1 < 0 || index2 < 0)
										break;

									CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
									for (int i=0; i<offpts.GetSize(); i++)
									{
										if (i < index1)
										{
											dpt0.Add(offpts[i]);
										}
										else if (i == index1)
										{
											dpt0.Add(offpts[i]);
											dpt0.Add(pt1);
											dpt1.Add(pt2);
										}
										else if (i > index2)
										{
											dpt1.Add(offpts[i]);
										}
									}

									DashLine(dpt0.GetData(),dpt0.GetSize(),&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale*fViewScale,0,nStyle,styles,nIndex,bBreakLinetype,m_bAdjustXOffset,fAssistWid,pGeo);
									DashLine(dpt1.GetData(),dpt1.GetSize(),&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale*fViewScale,0,nStyle,styles,nIndex,bBreakLinetype,m_bAdjustXOffset,fAssistWid,pGeo);
									
								}	
							}
							
						}
						else if (bXOff)
						{
							CArray<float,float> arrRatio0;
							CArray<PT_3DEX,PT_3DEX> dpt0;
							CUIntArray arrIndex0;

							//这里约定：当 xoff<0 时，虚线向前挪动xoff，但是 0 之前的部分才能显示
							//当xoff>0时，意义不变，表示虚线的绘制起点为xoff

							if( xoff>=0 )
							{
								GetXOffLine(offpts.GetData(),offpts.GetSize(),xoff,dpt0,arrIndex0,arrRatio0);
								
								if (arrIndex0.GetSize() <= 0)
									break;
								
								int index = arrIndex0[0];
								
								for (int i=offpts.GetSize()-1; i>=0; i--)
								{
									if (i <= index)
									{
										offpts.RemoveAt(i);
									}
								}
								
								offpts.InsertAt(0,dpt0[0]);
								
								xoff = 0;
							}						
							
							DashLine(offpts.GetData(),offpts.GetSize(),&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale*fViewScale,xoff,nStyle,styles,nIndex,bBreakLinetype,m_bAdjustXOffset,fAssistWid,pGeo);
						}
					}
					else
					{
						DashLine(offpts.GetData(),offpts.GetSize(),&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale*fViewScale,0,nStyle,styles,nIndex,bBreakLinetype,m_bAdjustXOffset,fAssistWid,pGeo);
					}
				}			
				
				pBuf->AddBuffer(&grbuf);
			}
			
			if( nGeoType==CLS_GEOMULTISURFACE )
			{
				for( int m=0; m<nGeo; m++)
				{
					delete arrGeos[m];
				}
			}
		}

		break;

	default:
		{
			pFt->Draw(pBuf,fDrawScale);
			break;
		}
	}
}


BOOL CDashLinetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,TRUE);

	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;

	if( nIndex==0 )
		return FALSE;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}


BOOL CDashLinetype::ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	if( nIndex==0 )
		return FALSE;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}


BOOL CDashLinetype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT || nGeoType == CLS_GEODIRPOINT)
		return FALSE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCellLinetype::CCellLinetype()
{
	m_nType = SYMTYPE_CELLLINETYPE;

	m_fXOffset = m_fYOffset = m_fAngle = m_fWidth = 0;	
	m_fkx = m_fky = 1;
	m_nPlaceType = CCellLinetype::BaseLine;
	m_fBaseXOffset = m_fBaseYOffset = 0;
	m_nPlacePos = CCellLinetype::Cycle;
	m_fCycle = 0;

	m_bCellAlign = FALSE;

	m_bInsideBaseLine = FALSE;
}

CCellLinetype::~CCellLinetype()
{

}



void CCellLinetype::CopyFrom(CSymbol *pSymbol)
{
    CCellLinetype *pCellLine = (CCellLinetype*)pSymbol;
	m_fAngle = pCellLine->m_fAngle;
	m_fBaseXOffset = pCellLine->m_fBaseXOffset;
	m_fBaseYOffset = pCellLine->m_fBaseYOffset;
	m_fCycle = pCellLine->m_fCycle;
	m_fkx = pCellLine->m_fkx;
	m_fky = pCellLine->m_fky;
	m_fXOffset = pCellLine->m_fXOffset;
	m_fYOffset = pCellLine->m_fYOffset;
	m_nPlaceType = pCellLine->m_nPlaceType;
	m_nPlacePos = pCellLine->m_nPlacePos;
	m_fWidth = pCellLine->m_fWidth;
	m_strCellDefName = pCellLine->m_strCellDefName;
	m_bCellAlign = pCellLine->m_bCellAlign;
	m_bInsideBaseLine = pCellLine->m_bInsideBaseLine;
}

BOOL CCellLinetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_ANGLE) )
			{
				data = xmlFile.GetData();
				m_fAngle = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASEXOFF) )
			{
				data = xmlFile.GetData();
				m_fBaseXOffset = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_BASEYOFF) )
			{
				data = xmlFile.GetData();
				m_fBaseYOffset = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_CYCLE) )
			{
				data = xmlFile.GetData();
				m_fCycle = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_KX) )
			{
				data = xmlFile.GetData();
				m_fkx = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_KY) )
			{
				data = xmlFile.GetData();
				m_fky = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_XOFFSET) )
			{
				data = xmlFile.GetData();
				m_fXOffset = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_YOFFSET) )
			{
				data = xmlFile.GetData();
				m_fYOffset = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_PLACEPOS) )
			{
				data = xmlFile.GetData();
				m_nPlacePos = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_PLACETYPE) )
			{
				data = xmlFile.GetData();
				m_nPlaceType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_CELLDEFNAME) )
			{
				data = xmlFile.GetData();
				m_strCellDefName = data;
			}

			if( xmlFile.FindElem(XMLTAG_CELLALIGN) )
			{
				data = xmlFile.GetData();
				m_bCellAlign = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_INSIDEBASELINE) )
			{
				data = xmlFile.GetData();
				m_bInsideBaseLine = _ttoi(data);
			}

			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CCellLinetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fAngle);
			xmlFile.AddElem(XMLTAG_ANGLE,data);
			
			data.Format(_T("%f"),m_fBaseXOffset);
			xmlFile.AddElem(XMLTAG_BASEXOFF,data);

			data.Format(_T("%f"),m_fBaseYOffset);
			xmlFile.AddElem(XMLTAG_BASEYOFF,data);
			
			data.Format(_T("%f"),m_fCycle);
			xmlFile.AddElem(XMLTAG_CYCLE,data);

			data.Format(_T("%f"),m_fkx);
			xmlFile.AddElem(XMLTAG_KX,data);

			data.Format(_T("%f"),m_fky);
			xmlFile.AddElem(XMLTAG_KY,data);

			data.Format(_T("%f"),m_fXOffset);
			xmlFile.AddElem(XMLTAG_XOFFSET,data);
			
			data.Format(_T("%f"),m_fYOffset);
			xmlFile.AddElem(XMLTAG_YOFFSET,data);

			data.Format(_T("%d"),m_nPlacePos);
			xmlFile.AddElem(XMLTAG_PLACEPOS,data);
			
			data.Format(_T("%d"),m_nPlaceType);
			xmlFile.AddElem(XMLTAG_PLACETYPE,data);

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			xmlFile.AddElem(XMLTAG_CELLDEFNAME,m_strCellDefName);

			data.Format(_T("%d"),m_bCellAlign);
			xmlFile.AddElem(XMLTAG_CELLALIGN,data);

			data.Format(_T("%d"),m_bInsideBaseLine);
			xmlFile.AddElem(XMLTAG_INSIDEBASELINE,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

	

}

void CCellLinetype::TransformLine(CArray<PT_3DEX,PT_3DEX> &pts,CArray<PT_3DEX,PT_3DEX> &dpts, CUIntArray &arrindex, float cycle, float fbaseXoff, float fbaseYoff, 
								  float fAssitWid, float fDrawScale)
{
	CArray<float,float> arrRatio;

	CArray<PT_3DEX,PT_3DEX> dpt1;
	if(m_nPlacePos == PlacePos::Cycle)
	{
		if( m_bCellAlign )
		{
			int npt = pts.GetSize();
			float *pLen1 = new float[npt];
			float *pLen2 = new float[npt];
			float *pLen3 = new float[npt];

			PT_3DEX *pt3ds = pts.GetData();
			PT_3DEX *pRets = new PT_3DEX[npt];
			for (int i=0; i<npt; i++)
			{
				COPY_3DPT(pRets[i],pt3ds[i]);
				pRets[i].pencode = pt3ds[i].pencode;
			}

			GraphAPI::GGetParallelLine(pt3ds,npt,fAssitWid+fbaseYoff,pRets);

			CalcLineStringLens(pt3ds,npt,pLen1);
			CalcLineStringLens(pRets,npt,pLen2);

			//计算pRet的点投影到pt3ds上的垂足与pt3ds上当前点的距离，这个距离便于后面计算各个线段的cycle的起点偏移
			pLen3[0] = 0;
			for( i=1; i<npt; i++)
			{
				//当前垂足偏移 = 母线的当前线段长度 - 辅助线当前线段长度 - 上一个垂足偏移
				pLen3[i] = (pLen1[i]-pLen1[i-1]) - (pLen2[i]-pLen2[i-1]) - pLen3[i-1];
			}

			float xoff = fbaseXoff;

			for( i=0; i<npt-1; i++)
			{
				GetCycleLine_FastLoop(pRets+i,pLen2[i+1]-pLen2[i],cycle,xoff,i,dpts,arrindex);

				xoff = fbaseXoff - (pLen1[i+1]+pLen3[i+1]);
			}

			delete[] pLen1;
			delete[] pLen2;
			delete[] pLen3;
			delete[] pRets;

			return;
		}
		else
		{
			GetCycleLine(pts.GetData(),pts.GetSize(),cycle,fbaseXoff,dpt1,arrindex);
		}
	}
	else if(m_nPlacePos == PlacePos::Vertex)
	{		
		GetXOffLine(pts.GetData(),pts.GetSize(),fbaseXoff,dpt1,arrindex,arrRatio);
	}
	else   // VertexDouble
	{
		//仅做x左右偏移
		if(fabs(fbaseXoff) <= GraphAPI::GetDisTolerance())//当xoff=0
		{
			int nPt = pts.GetSize();
			for(int i=0; i<nPt-1; i++)
			{
				dpt1.Add(pts[i]);
				arrindex.Add(i);
			}
			for(i=1; i<nPt; i++)
			{
				dpt1.Add(pts[i]);
				arrindex.Add(i-1);
			}
		}
		else
		{
			CArray<PT_3DEX,PT_3DEX> ldpts,rdpts;
			GetXOffLine(pts.GetData(),pts.GetSize(),fbaseXoff,ldpts,arrindex,arrRatio);
			GetXOffLine(pts.GetData(),pts.GetSize(),-fbaseXoff,rdpts,arrindex,arrRatio);

			dpt1.Append(ldpts);
			dpt1.Append(rdpts);		
		}
	}

	double dWidth = fbaseYoff;
	if( fabs(dWidth)<1e-4 )
	{
		dpts.Copy(dpt1);
	}
	else
	{
	
	// 沿线垂直平移
	for (int i=0; i<dpt1.GetSize(); i++)
	{
		int index = arrindex[i];
		PT_3D dpt = GetRotate90Pt(pts[index],pts[index+1],dpt1[i],dWidth);
		dpts.Add(PT_3DEX(dpt,penNone));
/*		PT_3DEX pt;
		int index = GGetPtFromLineWidth(pts.GetData(),pts.GetSize(),dpt1.GetAt(i),m_fBaseYOffset*fDrawScale,&pt);
		arrindex.Add(index);
		dpts.Add(pt);
*/	}
	
	}

}


static void CalcAngles(CArray<PT_3DEX,PT_3DEX>& basePts, CArray<double,double>& angles)
{
	int nsize = basePts.GetSize();
	PT_3DEX *pts = basePts.GetData();	

	angles.SetSize(nsize);
	double *angs = angles.GetData();
	for( int i=0; i<nsize-1; i++,pts++)
	{
		angs[i] = GraphAPI::GGetAngle(pts[0].x,pts[0].y,pts[1].x,pts[1].y);
	}
	if( nsize>0 )
		angs[nsize-1] = 0;
}

void CCellLinetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if (nIndex == 0)
	{
		CGeometry *pGeo = pFt->GetGeometry();
		int nGeoType = pGeo->GetClassType();
		if(nGeoType==CLS_GEOSURFACE || nGeoType==CLS_GEOMULTISURFACE )
		{
			if(pGeo->IsHideSymbolizeSurfaceBound())
				return;

			if(pGeo->IsHideSymbolizeSurfaceInnerBound() && nGeoType==CLS_GEOMULTISURFACE )
			{
				CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
				int nGeo = pMGeo->GetSurfaceNum();
				if(nGeo>0)
				{						
					CGeoSurface *pNewGeo = new CGeoSurface();
					pNewGeo->CopyFrom(pGeo);
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pMGeo->GetSurface(0,arrPts);
					pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					pNewGeo->Draw(pBuf,fDrawScale);
					delete pNewGeo;
				}
				return;
			}			
		}
		
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CellDef cell = cellLib->GetCellDef(nIndex-1);
	
	BOOL bBreakCell  = m_bBreakCell;

	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrXoff = 0;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrXoff = pCurve->m_fLinetypeXoff;
	}	

	float fWidth = m_fWidth*fDrawScale*fViewScale;
	if( fWidth==0.0f )
		fWidth = -fDrawScale*fViewScale;

	int symflagEnum = pGeo->GetSymbolizeFlagEnum();

	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOPARALLEL:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:

		{
			int nGeo = 1;
			
			CArray<CGeometry*,CGeometry*> arrGeos;
			arrGeos.Add(pGeo);
			if( nGeoType==CLS_GEOMULTISURFACE )
			{
				arrGeos.RemoveAll();
				
				CGeoMultiSurface *pMGeo = (CGeoMultiSurface*)pGeo;
				nGeo = pMGeo->GetSurfaceNum();
				for( int m=0; m<nGeo; m++)
				{
					if(pMGeo->IsHideSymbolizeSurfaceBound())
						continue;

					if(pMGeo->IsHideSymbolizeSurfaceInnerBound() && m>0)
						continue;

					CGeoSurface *pNewGeo = new CGeoSurface();
					pNewGeo->CopyFrom(pGeo);
					
					CArray<PT_3DEX,PT_3DEX> arrPts;
					pMGeo->GetSurface(m,arrPts);
					pNewGeo->CreateShape(arrPts.GetData(),arrPts.GetSize());
					arrGeos.Add(pNewGeo);
				}
				nGeo = arrGeos.GetSize();
			}
			else if( nGeoType==CLS_GEOSURFACE )
			{
				if(pGeo->IsHideSymbolizeSurfaceBound())
					nGeo = 0;
			}
			
			for(int m=0; m<nGeo; m++)
			{
				pGeo = arrGeos[m];
		
				// 平行线宽度方向
				float fwid = 1;
				CArray<PT_3DEX,PT_3DEX> basepts;
				GetBasePtsAndWidth(pGeo,basepts,fwid);

				SimpleCompressPoints(basepts,CSymbol::m_tolerance);			

				if (basepts.GetSize() < 1)
					break;

				BOOL bClose = FALSE;
				if (basepts.GetSize() >= 3)
				{
					PT_3DEX pt0,pt1;
					
					pt0 = basepts.GetAt(0); pt1 = basepts.GetAt(basepts.GetSize()-1);
					if( fabs(pt0.x-pt1.x)<1e-4 && fabs(pt0.y-pt1.y)<1e-4 && fabs(pt0.z-pt1.z)<1e-4 )
						bClose = TRUE;
				}
		
				CArray<PT_3DEX,PT_3DEX> dpts0;	
				
				float fWidAid = 0;
				if (m_nPlaceType == BaseLine)
				{
					dpts0.Copy(basepts);
				}
				else if (nGeoType==CLS_GEOPARALLEL && (m_nPlaceType == AidLine || m_nPlaceType == MidLine))
				{
					dpts0.Copy(basepts);
					
					if (m_nPlaceType == AidLine)
					{
						fWidAid = ((CGeoParallel*)pGeo)->GetWidth();
					}
					else
					{
						fWidAid = ((CGeoParallel*)pGeo)->GetWidth()/2;
					}

					//需要作图元对齐的话，应该使用母线作为基线，这样以保持和母线对齐；否则，就应该使用辅助线作为基线；
					if( !m_bCellAlign )
					{
						if ( !GraphAPI::GGetParallelLine(basepts.GetData(),basepts.GetSize(),fWidAid,dpts0.GetData()) ) return;
						
						for (int i=0; i<dpts0.GetSize(); i++)
						{
							dpts0[i].z += ((CGeoParallel*)pGeo)->GetDHeight();
						}

						fWidAid = 0;
					}
			
				}
				else if (nGeoType==CLS_GEODCURVE && (m_nPlaceType == AidLine || m_nPlaceType == MidLine))
				{
					((CGeoDCurve*)pGeo)->GetAssistShape(dpts0);
					ReversePoints(dpts0.GetData(),dpts0.GetSize());
				}

				if(dpts0.GetSize()>1)
				{					
					GrBuffer grbuf;

					CArray<PT_3DEX,PT_3DEX> dpts;
					CUIntArray arrIndex;
					TransformLine(dpts0, dpts, arrIndex, m_fCycle*ftrLinetypeScale*fDrawScale*fViewScale,
						m_fBaseXOffset*fDrawScale*fViewScale+ftrXoff,
						m_fBaseYOffset*fwid*fDrawScale*fViewScale,fWidAid);
					
					double ftrKx = m_fkx*ftrLinetypeScale*fDrawScale*fViewScale;
					double ftrKy = m_fky*ftrLinetypeScale*fDrawScale*fViewScale;
					
					double dx = m_fXOffset*fDrawScale*fViewScale;
					double dy = m_fYOffset*fDrawScale*fViewScale;
					
					if (m_nPlacePos == PlacePos::VertexDouble)
					{					
						//优化绘制结果
						BOOL bUsePointString = FALSE;
						if( cell.m_bCircular && !bBreakCell )
						{
							bUsePointString = TRUE;
													
							grbuf.BeginPointString(pGeo->GetColor(),ftrKx,ftrKy,TRUE,nIndex,fWidth);
						}

						// 偏移为0
						//if (fabs(m_fBaseXOffset) <= GraphAPI::GetDisTolerance())
						{
							CArray<double,double> angles;
							CalcAngles(dpts0,angles);

							for(int i=0; i<dpts.GetSize(); i++)
							{
								int index = arrIndex.GetAt(i);
								double angle = angles[index];
								
								double resAngle = 0;
								// 正方向
								if (i <= dpts.GetSize()/2-1)
								{
									resAngle = m_fAngle+angle-fRotAngle;
									if (fwid < 0)  
										resAngle = (PI-m_fAngle)+angle-fRotAngle;

								}
								// 反方向
								else if (i >= dpts.GetSize()/2)
								{
									if ( 1 || i == dpts.GetSize()-1)
									{
										resAngle = m_fAngle+angle+PI-fRotAngle;
										if (fwid < 0)  
											resAngle = (PI-m_fAngle)+angle+PI-fRotAngle;

									}
									else
									{
										double tmpangle = angles[index-1];
										resAngle = m_fAngle+tmpangle+PI-fRotAngle;
										if (fwid < 0)  resAngle = (PI-m_fAngle)+tmpangle+PI-fRotAngle;
								
									}
								}
								else
									continue;

								if( bUsePointString )
								{
									grbuf.PointString(&PT_3D(dpts[i].x+dx,dpts[i].y+dy,dpts[i].z));
								}
								else
									DrawCellToBuf(&grbuf,nIndex,&cell,&PT_3D(dpts[i].x+dx,dpts[i].y+dy,dpts[i].z),
										ftrKx,ftrKy,resAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,0,0);

							}
							
						}
						
						if( bUsePointString )
							grbuf.End();
					}
					else
					{
						GrBuffer cellbuf0;
						cellbuf0.AddBuffer(cell.m_pgr,0);

						int nsum_cell = cellbuf0.GetLinePts(NULL);
						BOOL bUseFastVertexs = FALSE;
						BOOL bUseSinglePoint = FALSE;
						//优化绘制结果
						BOOL bUsePointString = FALSE;

						CArray<GrVertex,GrVertex> pts_cell, pts_cell0;

						if( cell.m_evlp.Width()*ftrKx<CSymbol::m_tolerance &&
							cell.m_evlp.Height()*ftrKy<CSymbol::m_tolerance )
						{
							bUseSinglePoint = TRUE;
						}

						if( cell.m_bCircular && !bBreakCell )
						{
							bUsePointString = TRUE;
									
							if( bUseSinglePoint )
								grbuf.BeginPointString(pGeo->GetColor(),0,0,FALSE,0,0);
							else
								grbuf.BeginPointString(pGeo->GetColor(),ftrKx,ftrKy,TRUE,nIndex,fWidth);
						}	
						//直接提取图形点，作变换，添加到结果中；这样速度和内存效率都更高
						else if( cellbuf0.IsOnlyType(GRAPH_TYPE_LINESTRING) && bBreakCell )
						{
							bUseFastVertexs = TRUE;

							cellbuf0.SetAllColor(pGeo->GetColor());
							cellbuf0.SetAllLineWidthOrWidthScale(TRUE,fWidth);

							pts_cell0.SetSize(nsum_cell);
							cellbuf0.GetVertexPts(pts_cell0.GetData());

							grbuf.BeginLineString(pGeo->GetColor(),fWidth,TRUE);
						}

						CArray<double,double> angles;
						CalcAngles(dpts0,angles);

						double out_len = 0;
						if( m_bInsideBaseLine )
						{
							GrBuffer cellbuf1;
							cellbuf1.CopyFrom(&cellbuf0);
							
							cellbuf1.Zoom(ftrKx,ftrKy);
							cellbuf1.Rotate(-m_fAngle-fRotAngle);
							out_len = cellbuf1.GetEnvelope().m_xh;
						}

						for(int i=0; i<dpts.GetSize(); i++)
						{
							int index = arrIndex.GetAt(i);
							double angle = angles[index];

							if( m_nPlacePos == PlacePos::Vertex )
							{
								//没有首点电杆
								if( (symflagEnum==SYMFLAG_ENUM_NODIANGAN1||symflagEnum==SYMFLAG_ENUM_NODIANGAN3) && i==0 )
								{
									continue;
								}
								//没有尾点电杆
								if( (symflagEnum==SYMFLAG_ENUM_NODIANGAN2||symflagEnum==SYMFLAG_ENUM_NODIANGAN3) && i==(basepts.GetSize()-1) )
								{
									continue;
								}
							}
							
							// 控制最后一个点的方向
							if (m_nPlacePos == PlacePos::Vertex && bClose && i == basepts.GetSize()-1)  
								break;
							
							if (m_nPlacePos == PlacePos::Vertex && i == basepts.GetSize()-1)
							{
								double resAngle = -m_fAngle+angle-fRotAngle;
								if (fwid < 0)  resAngle = (PI+m_fAngle)+angle-fRotAngle;

								{
									if( bUseFastVertexs )
									{
										pts_cell.Copy(pts_cell0);
										
										double m1[16];

										Matrix44FromTransform(m1,
											ftrKx,ftrKy,
											resAngle,
											dpts[i].x+dx,dpts[i].y+dy,dpts[i].z);

										
										GraphAPI::TransformPointsWith44Matrix(m1,pts_cell.GetData(),pts_cell.GetSize());

										grbuf.Lines(pts_cell.GetData(),pts_cell.GetSize());
									}
									else if( bUsePointString )
									{
										grbuf.PointString(&PT_3D(dpts[i].x+dx,dpts[i].y+dy,dpts[i].z));
									}
									else if( bUseSinglePoint )
									{
										grbuf.Point(pGeo->GetColor(),&dpts[i],0,0,0,TRUE,0,1);
									}
									else
									{
										DrawCellToBuf(&grbuf,nIndex,&cell,&PT_3D(dpts[i].x+dx,dpts[i].y+dy,dpts[i].z),
											ftrKx,ftrKy,resAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,0,0);

									}
								}
								
							}					
							else
							{
								//最后一个点是否超出了母线
								if( m_bInsideBaseLine && GraphAPI::GGet2DDisOf2P(dpts[i],basepts[basepts.GetSize()-1])<out_len )
									continue;

								double resAngle = m_fAngle+angle-fRotAngle;
								if (fwid < 0)  resAngle = (PI-m_fAngle)+angle-fRotAngle;

								{
									if( bUseFastVertexs )
									{
										pts_cell.Copy(pts_cell0);
										double m1[16];

										Matrix44FromTransform(m1,
											ftrKx,ftrKy,
											resAngle,
											dpts[i].x+dx,dpts[i].y+dy,dpts[i].z);

										
										GraphAPI::TransformPointsWith44Matrix(m1,pts_cell.GetData(),pts_cell.GetSize());

										grbuf.Lines(pts_cell.GetData(),pts_cell.GetSize());
									}
									else if( bUsePointString )
									{
										grbuf.PointString(&PT_3D(dpts[i].x+dx,dpts[i].y+dy,dpts[i].z));
									}
									else if( bUseSinglePoint )
									{
										grbuf.Point(pGeo->GetColor(),&dpts[i],0,0,0,TRUE,0,1);
									}
									else
									{
										DrawCellToBuf(&grbuf,nIndex,&cell,&PT_3D(dpts[i].x+dx,dpts[i].y+dy,dpts[i].z),
											ftrKx,ftrKy,resAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,0,0);
									}
								}
								
							}
						}
						if( bUsePointString || bUseFastVertexs )
						{
							grbuf.End();
						}
					}

					if (bBreakCell)
					{
						grbuf.SetAllColor(pGeo->GetColor());
					}	
					else
					{
						grbuf.Move(dx,dy);				
					}
					
					pBuf->AddBuffer(&grbuf);
					
				}
			}
			
			if( nGeoType==CLS_GEOMULTISURFACE )
			{
				for( int m=0; m<nGeo; m++)
				{
					delete arrGeos[m];
				}
			}
		}

		break;
		
	default:
		{
			break;
		}
	}
}



BOOL CCellLinetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,FALSE);
	
	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if( nIndex==0 )
		return FALSE;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}


void CCellLinetype::ConvertToDashLinetype(CDashLinetype *pLT)
{
	pLT->m_fWidth = m_fWidth;
	pLT->m_fBaseOffset = m_fBaseYOffset;
	pLT->m_strBaseLinetypeName = "";

	if( m_nPlacePos==Vertex || m_nPlacePos==VertexDouble )
		pLT->m_fBaseOffset = 0;
}


BOOL CCellLinetype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT)
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CColorHatch::CColorHatch()
{
	m_nType = SYMTYPE_COLORHATCH;
	m_bUseSelfcolor = FALSE;
	m_nColor = RGB(255,255,255);
	m_fTransparence = 0;
}

CColorHatch::~CColorHatch()
{

}

void CColorHatch::CopyFrom(CSymbol *pSymbol)
{
	CColorHatch *pColorHatch = (CColorHatch*)pSymbol;
	m_nColor = pColorHatch->m_nColor;
	m_bUseSelfcolor = pColorHatch->m_bUseSelfcolor;
	m_fTransparence = pColorHatch->m_fTransparence;
}

BOOL CColorHatch::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_TRANSPARENCE) )
			{
				data = xmlFile.GetData();
				m_fTransparence = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_USESELFCOLOR) )
			{
				data = xmlFile.GetData();
				m_bUseSelfcolor = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_COLOR) )
			{
				data = xmlFile.GetData();
				m_nColor = _ttoi(data);
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CColorHatch::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fTransparence);
			xmlFile.AddElem(XMLTAG_TRANSPARENCE,data);

			data.Format(_T("%d"),m_bUseSelfcolor);
			xmlFile.AddElem(XMLTAG_USESELFCOLOR,data);
			
			data.Format(_T("%d"),m_nColor);
			xmlFile.AddElem(XMLTAG_COLOR,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	

void CColorHatch::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
		{
			break;
		}
	case CLS_GEODCURVE:
	case CLS_GEOPARALLEL:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
		{
			CArray<PT_3DEX,PT_3DEX> ptsex;
			if (nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEOPARALLEL)
			{
				const CShapeLine *pShape = ((CGeoCurve*)pGeo)->GetShape();
				if (pShape)
				{
					pShape->GetPts(ptsex);
				}

				if (nGeoType == CLS_GEOPARALLEL)
				{
					CGeoParallel *pParallel = (CGeoParallel*)pGeo;
					CArray<PT_3DEX,PT_3DEX> ptex;
					pParallel->GetParallelShape(ptex);
					for (int i=ptex.GetSize()-1; i>=0; i--)
					{
						ptsex.Add(ptex[i]);
					}					
					
				}
			}
			else if (nGeoType == CLS_GEODCURVE)
			{
				((CGeoDCurve*)pGeo)->GetOrderShape(ptsex);
			}
			else if (nGeoType == CLS_GEOSURFACE || nGeoType == CLS_GEOMULTISURFACE)
			{
				const CShapeLine *pShape = ((CGeoSurface*)pGeo)->GetShape();
				if (pShape)
				{
					pShape->GetPts(ptsex);
				}
			}
			else
			{
				pGeo->GetShape(ptsex);
			}

//			if( nGeoType==CLS_GEOCURVE || nGeoType == CLS_GEOSURFACE )
			{
				if(!((CGeoCurveBase*)pGeo)->IsClosed())
					break;
			}

			if(ptsex.GetSize() < 1)
				break;

			DWORD nColor = pGeo->GetColor();
			if (m_bUseSelfcolor)
			{
				nColor = m_nColor;
			}

			GrBuffer grbuf;

			grbuf.BeginPolygon(nColor,POLYGON_FILLTYPE_COLOR,m_fTransparence*2.55,NULL,0,m_bUseSelfcolor);
			for(int i=0; i<ptsex.GetSize(); i++)
			{
				if(i == 0 || (ptsex[i].pencode == penMove && nGeoType != CLS_GEODCURVE))
				{
					if (i != 0)
					{
						//grbuf.End();						
					}
					
					grbuf.MoveTo(&ptsex[i]);
				}
				else
					grbuf.LineTo(&ptsex[i]);
			}
			
			grbuf.End();
			
			pBuf->AddBuffer(&grbuf);
			
			break;
			
		}
		
	default:
		{
			break;
		}
	}

}



BOOL CColorHatch::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{	
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);	
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);

	return TRUE;
}

BOOL CColorHatch::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT)
		return FALSE;
	return TRUE;
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CConditionColorHatch::CConditionColorHatch()
{
	m_nType = SYMTYPE_COLORHATCH_COND;
	m_nDefColor = RGB(255,255,255);
	m_fDefTransparence = 0;
}

CConditionColorHatch::~CConditionColorHatch()
{

}


BOOL CConditionColorHatch::IsNeedAttrWhenDraw()
{
	return TRUE;
}



void CConditionColorHatch::CopyFrom(CSymbol *pSymbol)
{
	CConditionColorHatch *pColorHatch = (CConditionColorHatch*)pSymbol;
	m_nDefColor = pColorHatch->m_nDefColor;
	m_fDefTransparence = pColorHatch->m_fDefTransparence;
	m_strFieldName = pColorHatch->m_strFieldName;

	m_Conditions.Copy(pColorHatch->m_Conditions);
}

BOOL CConditionColorHatch::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_COLOR) )
			{
				data = xmlFile.GetData();
				m_nDefColor = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_TRANSPARENCE) )
			{
				data = xmlFile.GetData();
				m_fDefTransparence = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_FIELD) )
			{
				data = xmlFile.GetData();
				m_strFieldName = data;
			}

			if( xmlFile.FindElem(_T("Conditions")) )
			{
				xmlFile.IntoElem();

				while(xmlFile.FindElem(_T("Condition")))
				{
					xmlFile.IntoElem();

					Condition item;
					memset(&item,0,sizeof(item));

					if( xmlFile.FindElem(_T("Value")) )
					{
						data = xmlFile.GetData();
						_tcsncpy(item.value,data,sizeof(item.value)-1);
					}					
					if( xmlFile.FindElem(XMLTAG_COLOR) )
					{
						data = xmlFile.GetData();
						item.color = _ttoi(data);
					}

					if( xmlFile.FindElem(XMLTAG_TRANSPARENCE) )
					{
						data = xmlFile.GetData();
						item.fTransparence = _ttof(data);
					}

					m_Conditions.Add(item);

					xmlFile.OutOfElem();
				}

				xmlFile.OutOfElem();
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CConditionColorHatch::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			data.Format(_T("%d"), m_nDefColor);
			xmlFile.AddElem(XMLTAG_COLOR,data);
			
			data.Format(_T("%f"), m_fDefTransparence);
			xmlFile.AddElem(XMLTAG_TRANSPARENCE,data);

			xmlFile.AddElem(XMLTAG_FIELD,m_strFieldName);			
			if( m_Conditions.GetSize()>0 )
			{
				xmlFile.AddElem(_T("Conditions"));
				xmlFile.IntoElem();
				for( int i=0; i<m_Conditions.GetSize(); i++)
				{
					Condition item = m_Conditions[i];

					xmlFile.AddElem(_T("Condition"));
					xmlFile.IntoElem();
					xmlFile.AddElem(_T("Value"), item.value);

					data.Format(_T("%d"), item.color);
					xmlFile.AddElem(XMLTAG_COLOR,data);
					
					data.Format(_T("%f"), item.fTransparence);
					xmlFile.AddElem(XMLTAG_TRANSPARENCE,data);
					xmlFile.OutOfElem();
				}
				xmlFile.OutOfElem();
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	


void CConditionColorHatch::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
	case CLS_GEOPARALLEL:
		{
			CArray<PT_3DEX,PT_3DEX> ptsex;
			if (nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEOPARALLEL)
			{
				const CShapeLine *pShape = ((CGeoCurve*)pGeo)->GetShape();
				if (pShape)
				{
					pShape->GetPts(ptsex);
				}

				if (nGeoType == CLS_GEOPARALLEL)
				{
					CGeoParallel *pParallel = (CGeoParallel*)pGeo;
					CArray<PT_3DEX,PT_3DEX> ptex;
					pParallel->GetParallelShape(ptex);
					for (int i=ptex.GetSize()-1; i>=0; i--)
					{
						ptsex.Add(ptex[i]);
					}					
					
				}
			}
			else if (nGeoType == CLS_GEODCURVE)
			{
				((CGeoDCurve*)pGeo)->GetOrderShape(ptsex);
			}
			else if (nGeoType == CLS_GEOSURFACE || nGeoType == CLS_GEOMULTISURFACE)
			{
				const CShapeLine *pShape = ((CGeoSurface*)pGeo)->GetShape();
				if (pShape)
				{
					pShape->GetPts(ptsex);
				}
			}
			else
			{
				pGeo->GetShape(ptsex);
			}

			if(ptsex.GetSize() < 1)
				break;

			DWORD nColor = m_nDefColor;

			GrBuffer grbuf;

			grbuf.BeginPolygon(nColor,POLYGON_FILLTYPE_COLOR,m_fDefTransparence*255,NULL,0,FALSE);
			for(int i=0; i<ptsex.GetSize(); i++)
			{
				if(i == 0 || (ptsex[i].pencode == penMove && nGeoType != CLS_GEODCURVE))
				{
					if (i != 0)
					{
						//grbuf.End();						
					}
					
					grbuf.MoveTo(&ptsex[i]);
				}
				else
					grbuf.LineTo(&ptsex[i]);
			}
			
			grbuf.End();
			
			pBuf->AddBuffer(&grbuf);
			
			break;
			
		}
		
	default:
		{
			break;
		}
	}

}


static BOOL CompareVariantAndString(const _variant_t& var, const char *text)
{
	if(text==NULL)
		return FALSE;

	BOOL bNotEmpty = (_tcslen(text)!=0);
	
	CString text2;
	switch(var.vt) 
	{
	case VT_UI1:
		text2.Format(_T("%i"),(int)var.bVal);
		break;
	case VT_UI2:
		text2.Format(_T("%i"), (int)var.uiVal);
		break;
	case VT_UI4:
		text2.Format(_T("%i"), (int)var.ulVal);
		break;
	case VT_UINT:
		text2.Format(_T("%i"), (int)var.uintVal);
		break;
	case VT_I1:
		text2.Format(_T("%c"), var.cVal);
		break;
	case VT_I2:
		text2.Format(_T("%i"), (int)var.iVal);
		break;	
	case VT_I4:
		text2.Format(_T("%i"), (long)var);
		break;
	case VT_INT:
		text2.Format(_T("%i"), (long)var);
		break;
	case VT_BOOL:
		text2.Format(_T("%i"), (bool)var);
		break;
	case VT_R4:
		{
			float v = _ttof(text);
			return (bNotEmpty && v==(float)var);
		}
		break;
	case VT_R8:
		{
			double v = _ttof(text);
			return (bNotEmpty && v==(double)var);
		}
		break;
	case VT_BSTR:
		text2.Format(_T("%s"), (LPCTSTR)(_bstr_t)var);
		break;
	default:
		break;
	}

	return (text2.Compare(text)==0);
}

void CConditionColorHatch::Draw(const CFeature *pFt, GrBuffer *pBuf, CValueTable &tab, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	const CVariantEx *pVar = NULL;
	if(!tab.GetValue(0,m_strFieldName,pVar))
	{
		Draw(pFt,pBuf,fDrawScale,fRotAngle,fViewScale);
		return;
	}

	for(int i=0; i<m_Conditions.GetSize(); i++)
	{
		Condition item = m_Conditions[i];
		if(CompareVariantAndString(pVar->m_variant,item.value))
		{
			DWORD saveColor = m_nDefColor;
			float saveTransp = m_fDefTransparence;

			m_nDefColor = item.color;
			m_fDefTransparence = item.fTransparence;

			Draw(pFt,pBuf,fDrawScale,fRotAngle,fViewScale);

			m_nDefColor = saveColor;
			m_fDefTransparence = saveTransp;

			return;
		}
	}

	Draw(pFt,pBuf,fDrawScale,fRotAngle,fViewScale);
}

BOOL CConditionColorHatch::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{	
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);	
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);

	return TRUE;
}


BOOL CConditionColorHatch::Explode(const CFeature *pFt, CValueTable &tab, float fDrawScale, CGeoArray& arrPGeos)
{	
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);	
	
	GrBuffer buf;
	Draw(pFt,&buf,tab,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);

	return TRUE;
}

BOOL CConditionColorHatch::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT)
		return FALSE;
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCellHatch::CCellHatch()
{
	m_nType = SYMTYPE_CELLHATCH;

	m_fdx = m_fdy = m_fddx = m_fddy = m_fAngle = m_fWidth = 0;	
	m_fxoff = m_fyoff = 0;
	m_fkx = m_fky = 1;
	m_bAccurateCutCell = 0;
	m_bRandomAngle = 0;
}

CCellHatch::~CCellHatch()
{
	
}

void CCellHatch::CopyFrom(CSymbol *pSymbol)
{
	CCellHatch *pCellHatch = (CCellHatch*)pSymbol;
	m_fAngle = pCellHatch->m_fAngle;
	m_fddx = pCellHatch->m_fddx;
	m_fddy = pCellHatch->m_fddy;
	m_fdx = pCellHatch->m_fdx;
	m_fdy = pCellHatch->m_fdy;
	m_fkx = pCellHatch->m_fkx;
	m_fky = pCellHatch->m_fky;
	m_fWidth = pCellHatch->m_fWidth;
	m_fxoff = pCellHatch->m_fxoff;
	m_fyoff = pCellHatch->m_fyoff;
	m_strCellDefName = pCellHatch->m_strCellDefName;
	m_bAccurateCutCell = pCellHatch->m_bAccurateCutCell;
	m_bRandomAngle = pCellHatch->m_bRandomAngle;
}

BOOL CCellHatch::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_KX) )
			{
				data = xmlFile.GetData();
				m_fkx = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_KY) )
			{
				data = xmlFile.GetData();
				m_fky = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_DX) )
			{
				data = xmlFile.GetData();
				m_fdx = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_DY) )
			{
				data = xmlFile.GetData();
				m_fdy = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_ANGLE) )
			{
				data = xmlFile.GetData();
				m_fAngle = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_DDX) )
			{
				data = xmlFile.GetData();
				m_fddx = _ttof(data);
			}	
			
			if( xmlFile.FindElem(XMLTAG_DDY) )
			{
				data = xmlFile.GetData();
				m_fddy = _ttof(data);
			}	
			
			if( xmlFile.FindElem(XMLTAG_XOFFSET) )
			{
				data = xmlFile.GetData();
				m_fxoff = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_YOFFSET) )
			{
				data = xmlFile.GetData();
				m_fyoff = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_CELLDEFNAME) )
			{
				data = xmlFile.GetData();
				m_strCellDefName = data;
			}

			if( xmlFile.FindElem(XMLTAG_ACCURATE_CUTCELL) )
			{
				data = xmlFile.GetData();
				m_bAccurateCutCell = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_RANDOMANGLE) )
			{
				data = xmlFile.GetData();
				m_bRandomAngle = _ttoi(data);
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CCellHatch::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			data.Format(_T("%f"),m_fkx);
			xmlFile.AddElem(XMLTAG_KX,data);
			
			data.Format(_T("%f"),m_fky);
			xmlFile.AddElem(XMLTAG_KY,data);

			data.Format(_T("%f"), m_fdx);
			xmlFile.AddElem(XMLTAG_DX,data);
			
			data.Format(_T("%f"),m_fdy);
			xmlFile.AddElem(XMLTAG_DY,data);
			
			data.Format(_T("%f"),m_fAngle);
			xmlFile.AddElem(XMLTAG_ANGLE,data);
			
			data.Format(_T("%f"),m_fddx);
			xmlFile.AddElem(XMLTAG_DDX,data);

			data.Format(_T("%f"),m_fddy);
			xmlFile.AddElem(XMLTAG_DDY,data);

			data.Format(_T("%f"),m_fxoff);
			xmlFile.AddElem(XMLTAG_XOFFSET,data);

			data.Format(_T("%f"),m_fyoff);
			xmlFile.AddElem(XMLTAG_YOFFSET,data);

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);

			xmlFile.AddElem(XMLTAG_CELLDEFNAME,m_strCellDefName);

			data.Format(_T("%d"),m_bAccurateCutCell);
			xmlFile.AddElem(XMLTAG_ACCURATE_CUTCELL,data);

			data.Format(_T("%d"),m_bRandomAngle);
			xmlFile.AddElem(XMLTAG_RANDOMANGLE,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	

double Cofactor(double* matrix[],int jie,int row,int column);
double AlCo(double* matrix[],int jie,int row,int column) 
{ 
	double result;
	if((row+column)%2 == 0)
		result = Cofactor(matrix,jie,row,column); 
	else result=(-1)*Cofactor(matrix,jie,row,column);
	return result; 
}

double Determinant(double* matrix[],int n) 
{ 
	double result=0,temp; 
	int i; 
	if(n==1) 
		result=(*matrix[0]); 
	else 
	{ 
		for(i=0;i<n;i++) 
		{ 
			temp=AlCo(matrix,n,n-1,i); 
			result+=(*(matrix[n-1]+i))*temp; 
		} 
	} 
	return result; 
} 

#define MAX 3
double Cofactor(double* matrix[],int jie,int row,int column) 
{ 
	double result; 
	int i,j; 
	double* smallmatr[MAX-1]; 
	for(i=0;i<jie-1;i++) 
		smallmatr[i]= new double[jie - 1];
	for(i=0;i<row;i++) 
		for(j=0;j<column;j++) 
			*(smallmatr[i]+j)=*(matrix[i]+j); 
		for(i=row;i<jie-1;i++) 
			for(j=0;j<column;j++) 
				*(smallmatr[i]+j)=*(matrix[i+1]+j); 
			for(i=0;i<row;i++) 
				for(j=column;j<jie-1;j++) 
					*(smallmatr[i]+j)=*(matrix[i]+j+1); 
				for(i=row;i<jie-1;i++) 
					for(j=column;j<jie-1;j++) 
						*(smallmatr[i]+j)=*(matrix[i+1]+j+1); 
					result = Determinant(smallmatr,jie-1);
					for(i=0;i<jie-1;i++)
						delete[] smallmatr[i];
					return result;  
}

void Inverse(double *matrix1[],double *matrix2[],int n,double d)
{
	int i,j;
	for(i=0;i<n;i++)
		matrix2[i]=(double *)malloc(n*sizeof(double));
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			*(matrix2[j]+i)=(AlCo(matrix1,n,i,j)/d);
}

// Ax + By + Cz + 1 = 0
BOOL SolvePlaneEquation(PT_3D *pts, int num, double *A, double *B, double *C)
{
	if (num < 3 || A == NULL || B == NULL || C == NULL) return FALSE;

	double *array,Y[3];
	ZeroMemory(Y,sizeof(Y));
	*A = *B = *C = 0;
	array = new double[num*3];
	for (int i=0; i<num; i++)
	{
		array[i*3] = pts[i].x;
		array[i*3+1] = pts[i].y;
		array[i*3+2] = pts[i].z;
	}
	double *Matrix[3],*IMatrix[3];
	for (i=0; i<3; i++)
	{
		Matrix[i]  = new double[3];
		IMatrix[i] = new double[3];
	}
	for (i=0; i<3; i++)
	{
		ZeroMemory(Matrix[i],sizeof(double)*3);
	}
	for (int j = 0 ;j < 3; j++)
	{
		for (int i = 0; i < num; i++)
		{
			*(Matrix[0] + j) += array[i*3] * array[i*3+j];
			*(Matrix[1] + j) += array[i*3+1] * array[i*3+j];
			*(Matrix[2] + j) += array[i*3+2] * array[i*3+j];
			Y[j] -= array[i*3+j];
		}
	}
	double d = Determinant(Matrix,3);
	if (abs(d) < 1e-8)
	{
		return FALSE;
	}
	Inverse(Matrix,IMatrix,3,d);
	for (i = 0;i < 3;i++)
	{
		*A += *(IMatrix[0] + i)*Y[i];
		*B += *(IMatrix[1] + i)*Y[i];
		*C += *(IMatrix[2] + i)*Y[i];
	}

	for (i = 0; i < 3; i++)
	{
		delete[] Matrix[i];
		delete[] IMatrix[i];
	}

	delete []array;

	return TRUE;
}


//与SolvePlaneEquation的区别：由于本函数主要用于校正z坐标，所以将方程式作了调整
// z = Ax + By + C
BOOL SolvePlaneEquation2(PT_3D *pts, int num, double* a,double* b,double* c)
{
	if (num <= 0)
		return FALSE;

	double M[9];
	double RM[9];
	double A[3];
	ZeroMemory(M,sizeof(double)*9);
	ZeroMemory(RM,sizeof(double)*9);
	ZeroMemory(A,sizeof(double)*3);
	int i;

	//用首点取差值，可以将a、b参数计算的更精确，避免浮点误差。
	//当pts的坐标中有大数时，不这样做的话，误差较大（可以达到几米）。
	double x0 = pts[0].x, y0 = pts[0].y;
	
	for(i = 0;i<num;i++)
	{
		PT_3D point = pts[i];	
		point.x -= x0;
		point.y -= y0;
		M[0] += point.x * point.x;
		M[1] += point.x * point.y;
		M[2] += point.x;
		
		M[4] += point.y * point.y;
		M[5] += point.y;
		
		
		A[0] += point.x*point.z;
		A[1] += point.y*point.z;
		A[2] += point.z;
	}	

	M[3] = M[1];
	M[6] = M[2];
	M[7] = M[5];
	M[8] = num;	
	
	//如果矩阵不可逆，
	if (!matrix_reverse(M,3,RM))
	{
		return FALSE;
	}

	*a = RM[0]*A[0] + RM[1]*A[1] + RM[2]*A[2];
	*b = RM[3]*A[0] + RM[4]*A[1] + RM[5]*A[2];
	*c = RM[6] * A[0] + RM[7] * A[1] + RM[8] * A[2] - *a * x0 - *b *y0;
	
	return TRUE;
}

//对复杂面进行处理，占据一块区域的为一组
static int GetMultiSurfacePart(CGeometry* pGeo, CGeoArray& arr)
{
	if(!pGeo->IsKindOf(RUNTIME_CLASS(CGeoMultiSurface)))
	{
		arr.Add(pGeo);
		return 1;
	}

	CGeoMultiSurface* pMS = (CGeoMultiSurface*)pGeo;
	int num = pMS->GetSurfaceNum();
	int *pos = new int[num];
	memset(pos, -1, num*sizeof(pos));
	CArray<PT_3DEX,PT_3DEX> pts1, pts2;
	for(int i=0; i<num; i++)
	{
		if(pos[i] != -1) continue;
		pts1.RemoveAll();
		pMS->GetSurface(i, pts1);
		pos[i] = i;
		for(int j=i+1; j<num; j++)
		{
			if(pos[j] != -1) continue;
			pts2.RemoveAll();
			pMS->GetSurface(j, pts2);
			if( 2==GraphAPI::GIsPtInRegion(pts2[0], pts1.GetData(), pts1.GetSize()) )
			{
				pos[j] = i;
			}
			if( 2==GraphAPI::GIsPtInRegion(pts1[0], pts2.GetData(), pts2.GetSize()) )
			{
				pos[i] = j;
			}
		}
	}

	for(i=0; i<num; i++)
	{
		if(pos[i]!=i) continue;
		pts1.RemoveAll();
		pMS->GetSurface(i, pts1);
		int ptsum = pts1.GetSize();
		for(int j=0; j<num; j++)
		{
			if(i==j) continue;
			if(pos[j]!=i) continue;
			pts2.RemoveAll();
			pMS->GetSurface(j, pts2);
			pts1.Append(pts2);
		}

		if(ptsum==pts1.GetSize())
		{
			CGeoSurface* pSurface = new CGeoSurface();
			pSurface->CreateShape(pts1.GetData(), pts1.GetSize());
			arr.Add(pSurface);
		}
		else if(ptsum<pts1.GetSize())
		{
			CGeoMultiSurface* pMS = new CGeoMultiSurface();
			pMS->CreateShape(pts1.GetData(), pts1.GetSize());
			arr.Add(pMS);
		}
	}

	delete[] pos;
	return arr.GetSize();
}


BOOL CCellHatch::m_bAccuTrimSurface = TRUE;
BOOL CCellHatch::m_bkeepOneCell = FALSE;

void CCellHatch::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakCell  = m_bBreakCell;

	BOOL bAccuTrimSurface = m_bAccuTrimSurface;

	float fWidth = m_fWidth*fDrawScale*fViewScale;

	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrCellScale, ftrCellAngle, ftrIntvScale, ftrXStartOff, ftrYStartOff, fRandomAngle;
	
	ftrCellScale = ftrIntvScale = 1;
	ftrCellAngle = ftrXStartOff = ftrYStartOff = 0;
	fRandomAngle = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		CGeoSurface *pSur = (CGeoSurface*)pGeo;
		ftrCellScale = pSur->m_fCellScale;
		ftrCellAngle = pSur->m_fCellAngle;
		ftrIntvScale = pSur->m_fIntvScale;
		ftrXStartOff = pSur->m_fXStartOff;
		ftrYStartOff = pSur->m_fYStartOff;
	}

	float xoff = m_fxoff*fDrawScale*fViewScale;
	float yoff = m_fyoff*fDrawScale*fViewScale;

	ftrXStartOff += xoff;
	ftrYStartOff += yoff;

	//以地物的GUID生成一个随机数的种子，来生成随机角度
	if( m_bRandomAngle )
	{
		OUID id = pFt->GetID();
		double v = sqrt((double)abs(id.v[0]*id.v[1]*id.v[2]*id.v[3]) + 2);
		v = (long)((v - floor(v))*1e+6);
		if( v<2 )v = 100;
		srand((long)v);
	}
	
	int nGeoType = pGeo->GetClassType();
	CGeometry* pGeo1 = NULL;
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
	case CLS_GEOPARALLEL:
		{
			CCellDefLib *cellLib = GetCellDefLib();
			int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;

			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				break;
			}

			CellDef cell = cellLib->GetCellDef(nIndex-1);
				
			float kx = m_fkx*ftrCellScale*fDrawScale*fViewScale;
			float ky = m_fky*ftrCellScale*fDrawScale*fViewScale;

			GrBuffer buf0;
			Envelope e0, e1;

			//计算图元外包
			{
				buf0.AddBuffer(cell.m_pgr,0);
				
				double m1[16];
				
				Matrix44FromTransform(m1,
					kx,ky,m_fAngle-fRotAngle+ftrCellAngle,
					0,0,0);
				
				buf0.Transform(m1);
				buf0.ZoomPointSize(kx,ky);
				
				e0 = buf0.GetEnvelope();
			}

			//外扩，小于0.1mm时，以0.1mm计算
			double ext = e0.Width()+e0.Height();
			if(ext<(0.1*ftrCellScale*fDrawScale*fViewScale))ext = 0.1*ftrCellScale*fDrawScale*fViewScale;

			//以前的算法很慢
			if(0)
			{
				//有包含关系的子面分成一个块
				CGeoArray arr;
				int num = GetMultiSurfacePart(pGeo, arr);
				for(int n=0; n<num; n++)
				{
					pGeo1 = arr[n];
					int nGeoType1 = pGeo1->GetClassType();
					float fwid = -1;
					CArray<PT_3DEX,PT_3DEX> basepts;
					if(nGeoType1==CLS_GEOMULTISURFACE)
					{
						GetMultiSurfaceBasePts(pGeo1,basepts);
					}
					else
					{
						GetBasePtsAndWidth(pGeo1,basepts,fwid);
					}

					int sum = basepts.GetSize();

					if (nGeoType == CLS_GEOPARALLEL)
					{
						CGeoParallel *pParallel = (CGeoParallel*)pGeo1;
						CArray<PT_3DEX,PT_3DEX> ptspar;
						pParallel->GetParallelShape(ptspar);

						for (int i=ptspar.GetSize()-1; i>=0; i--)
						{
							basepts.Add(ptspar.GetAt(i));
						}					
					}
				
					if(sum < 3) continue;

					Envelope evlp;
					PT_3D *ptstmp = new PT_3D[basepts.GetSize()];
					for(int i=0; i<basepts.GetSize(); i++)
					{
						COPY_3DPT(ptstmp[i],basepts[i]);
					}
					evlp.CreateFromPts(ptstmp,sum);

					//间距倍率很大时，会导致后面的 GrTrim::InitTrimPolygon 会很耗时
					if( ext>evlp.Width()+evlp.Height() )
						ext = evlp.Width()+evlp.Height();

					evlp.Inflate(ext,ext,0);

					// 求解平面方程
					double A, B, C;
					BOOL bSolved = SolvePlaneEquation2(ptstmp,basepts.GetSize(),&A,&B,&C);

					// 面中心点
					PT_3D	ptcen;
					PT_3DEX ptex0 = basepts.GetAt(0);
					((CGeoCurveBase*)pGeo1)->CGeoCurveBase::GetCenter(&ptex0,&ptcen);

					float fddxscale = m_fddx*fDrawScale*fViewScale, fddyscale = m_fddy*fDrawScale*fViewScale;
					float fdxscale = m_fdx*ftrIntvScale*fDrawScale*fViewScale, fdyscale = m_fdy*ftrIntvScale*fDrawScale*fViewScale;

					CGrTrim	grTrim;

					if(nGeoType==CLS_GEOMULTISURFACE)
						grTrim.InitTrimPolygons(basepts.GetData(),basepts.GetSize(),ext/2);
					else
						grTrim.InitTrimPolygon(ptstmp,sum,ext/2);
					
					float fDdx = fddxscale;
					if(fabs(fdxscale)>1e-4 && fDdx>=fdxscale)
						fDdx = fDdx -floor(fDdx/fdxscale)*fdxscale;

					float fDdy = fddyscale;
					if(fabs(fdyscale)>1e-4 && fDdy>=fdyscale)
						fDdy = fDdy -floor(fDdy/fdyscale)*fdyscale;

					// 填充考虑中心点，先计算开始填充的位置
					double fXstart = ptcen.x, fYstart = ptcen.y;
					i = 0;
					fYstart += (ftrYStartOff+fDdy);
					if (fabs(fdyscale) > 1e-4)
					{
						i = ceil((fYstart-evlp.m_yl)/fdyscale);
						fYstart -= fdyscale*i;
					}

					// 第i行ddx值
					float fddxt = 0;
					if (fabs(fdxscale) > 1e-4)
					{
						fddxt = fDdx*i -floor(fDdx*i/fdxscale)*fdxscale;
					}
					
					fXstart = ptcen.x + ftrXStartOff - fddxt;
					if (fabs(fdxscale) > 1e-4)
					{
						i = ceil((fXstart-evlp.m_xl)/fdxscale);
						fXstart -= fdxscale*i;
					}

					// 计算个数，若大于1000则不用精确裁切
					int xsum, ysum;
					xsum = fabs(fdxscale)>1e-4?(evlp.m_xh-fXstart)/fdxscale:1;
					ysum = fabs(fdyscale)>1e-4?(evlp.m_yh-fYstart)/fdyscale:1;
					if (xsum*ysum > 1000 || !bAccuTrimSurface || !m_bAccurateCutCell)
					{
						grTrim.m_bSimplyTrim = TRUE;
					}
					
					GrBuffer buf, ret;

					//记录跳过的点，以便在需要精确裁剪图元时，绘制这部分图元
					CArray<PT_3D,PT_3D> arrSkipPts;

					{
						pBuf->BeginPointString(pGeo->GetColor(),kx,ky,TRUE,nIndex,fWidth);

						for(double y=fYstart; y<=evlp.m_yh; y+=fdyscale)
						{				
							for(double x=fXstart; x<=evlp.m_xh; x+=fdxscale)
							{
								PT_3D pt;
								pt.x = x;
								pt.y = y;
								if (bSolved && fabs(C) > GraphAPI::GetZTolerance())
								{
									pt.z = A*pt.x+B*pt.y+C;
								}
								else
								{	pt.z = ptcen.z;
								}

								if( grTrim.bPtInPolygon(&pt,ext) )
								{
									e1 = e0;
									e1.Offset(pt.x,pt.y,pt.z);

									int bPtInFlag = grTrim.bEnvelopeInPolygon(e1);

									if( m_bRandomAngle )
										fRandomAngle = (rand()%50)*PI/25;

									if( bPtInFlag==1 )
									{
										pBuf->PointString(&pt,m_fAngle+ftrCellAngle-fRotAngle+fRandomAngle);
									}
									else if( bPtInFlag==2 )
									{
										arrSkipPts.Add(pt);
									}
									else
									{
										//当图元是极扁平时，有可能导致图元穿透了地物边界，而应该绘制它，但bEnvelopeInPolygon()==0，程序跳过了；
										//这是一个漏洞，不过这种情况太少见，而且处理起来会很费计算量，所以暂不处理。
										if( xsum*ysum<1000 )
										{
											arrSkipPts.Add(pt);
										}
									}

								}
								//m_fdx==0
								if (fabs(fdxscale) < 1e-4)
									break;						
							}

							fXstart += fDdx;
							if(fXstart-evlp.m_xl > fdxscale)
								fXstart -= fdxscale; 
							//m_fdy==0
							if (fabs(fdyscale) < 1e-4)
								break;
						}

						pBuf->End();
					}

					//精确裁剪图元
					if( m_bAccurateCutCell || bAccuTrimSurface)
					{
						grTrim.m_bSimplyTrim = FALSE;
						
						int nSkipPts = arrSkipPts.GetSize();
						
						for(int k=0; k<nSkipPts; k++)
						{				
							PT_3D pt = arrSkipPts[k];
							
							GrBuffer buf, ret;							
							{							
								buf.AddBuffer(cell.m_pgr,pt.z);

								if( m_bRandomAngle )
									fRandomAngle = (rand()%50)*PI/25;
								
								double m1[16],m2[16],m3[16];
								Matrix44FromZoom(kx,ky,1,m1);
								Matrix44FromRotate(&PT_3D(),&PT_3D(0,0,1),m_fAngle-fRotAngle+ftrCellAngle+fRandomAngle,m2);
								matrix_multiply(m2,m1,4,m3);
								Matrix44FromMove(pt.x,pt.y,0,m2);
								matrix_multiply(m2,m3,4,m1);
								
								buf.Transform(m1);
								
								buf.SetAllColor(pGeo->GetColor());								
							}

							grTrim.Trim_notBlockOptimized(&buf,ret);
							pBuf->AddBuffer(&ret);
						}				
 					}

					GrBuffer buf1;
					pBuf->GetAllGraph(&buf1);
					Graph *pu =buf1.HeadGraph();
					if(!pu)
					{
						BOOL bkeepOneCell = m_bkeepOneCell;
						if(bkeepOneCell)//至少保留一个图元
						{
							DrawCellToBuf(pBuf,nIndex,&cell,&PT_3D(ptcen.x,ptcen.y,ptcen.z),
								kx,ky,m_fAngle-fRotAngle+ftrCellAngle+fRandomAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,0,0);
						}
					}

					delete []ptstmp;
				}

				for(n=0; n<num; n++)
				{
					if(pGeo!=arr[n])
					{
						delete arr[n];
					}
				}
			}
			else
			{
				pGeo1 = pGeo;
				int nGeoType1 = pGeo1->GetClassType();
				float fwid = -1;
				CArray<PT_3DEX,PT_3DEX> basepts;
				pGeo1->GetShape(basepts);

				int sum = basepts.GetSize();
			
				if(sum < 3) break;

				Envelope evlp;
				PT_3D *ptstmp = new PT_3D[basepts.GetSize()];
				for(int i=0; i<basepts.GetSize(); i++)
				{
					COPY_3DPT(ptstmp[i],basepts[i]);
				}
				evlp.CreateFromPts(ptstmp,sum);

				//间距倍率很大时，会导致后面的 GrTrim::InitTrimPolygon 会很耗时
				if( ext>evlp.Width()+evlp.Height() )
					ext = evlp.Width()+evlp.Height();

				evlp.Inflate(ext,ext,0);

				// 求解平面方程
				double A, B, C;
				BOOL bSolved = SolvePlaneEquation2(ptstmp,basepts.GetSize(),&A,&B,&C);

				// 面中心点
				PT_3D	ptcen;
				PT_3DEX ptex0 = basepts.GetAt(0);
				pGeo1->GetCenter(&ptex0, &ptcen);

				float fddxscale = m_fddx*fDrawScale*fViewScale, fddyscale = m_fddy*fDrawScale*fViewScale;
				float fdxscale = m_fdx*ftrIntvScale*fDrawScale*fViewScale, fdyscale = m_fdy*ftrIntvScale*fDrawScale*fViewScale;

				// 填充考虑中心点，先计算开始填充的位置
				double fXstart = ptcen.x+ftrXStartOff, fYstart = ptcen.y+ftrYStartOff;

				// 计算个数，若大于100000就不再填充了
				int xsum, ysum;
				xsum = fabs(fdxscale)>1e-4?(evlp.Width())/fdxscale:1;
				ysum = fabs(fdyscale)>1e-4?(evlp.Height())/fdyscale:1;
				if (xsum*ysum > 100000 )
				{
					pFt->Draw(pBuf,fDrawScale);
					delete []ptstmp;
					break;
				}
			
				//
				std::vector<fillregion::vector2d> res;
				fillregion::FillSurfaceWithPointPrecise(pGeo1,fillregion::vector2d(fXstart,fYstart),fdxscale,fdyscale,fddxscale,ext/2,ext/2,res);

				pBuf->BeginPointString(pGeo->GetColor(),kx,ky,TRUE,nIndex,fWidth);
				for(int k=0; k<res.size(); ++k)
				{
					PT_3DEX pt;
					pt.x = res[k].x;
					pt.y = res[k].y;
					pt.z = pt.x*A + pt.y*B + C;
					pBuf->PointString(&pt,m_fAngle+ftrCellAngle-fRotAngle+fRandomAngle);
				}
				pBuf->End();
				//

				BOOL bkeepOneCell = m_bkeepOneCell;
				Graph *pu =pBuf->HeadGraph();
				if(!pu)
				{
					if(bkeepOneCell)//至少保留一个图元
					{
						DrawCellToBuf(pBuf,nIndex,&cell,&PT_3D(ptcen.x,ptcen.y,ptcen.z),
							kx,ky,m_fAngle-fRotAngle+ftrCellAngle+fRandomAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,0,0);
					}
				}

				delete []ptstmp;
			}

			break;
			
		}
	case CLS_GEOPOINT:
	case CLS_GEODIRPOINT:
	case CLS_GEOSURFACEPOINT:
		{
			CCellDefLib *cellLib = GetCellDefLib();
			int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}
			CellDef cell = cellLib->GetCellDef(nIndex-1);

			CGeometry *pGeo = pFt->GetGeometry();
			PT_3DEX pt = pGeo->GetDataPoint(0);	
			
			float   ftrKx, ftrKy, ftrRotateAngle, ftrWidth, fExtendDis;
			int     nCoverType;
			
			ftrKx = ftrKy = 1;
			ftrRotateAngle = ftrWidth = fExtendDis = 0;
			nCoverType = 0;

			if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
			{
				CGeoPoint *pPoint = (CGeoPoint*)pGeo;
				ftrKx = pPoint->m_fKx;
				ftrKy = pPoint->m_fKy;
				ftrRotateAngle = pPoint->m_lfAngle*PI/180;
				ftrWidth = pPoint->m_fWidth; 
			}

			ftrKx = m_fkx*fDrawScale*fViewScale*ftrKx;
			ftrKy = m_fky*fDrawScale*fViewScale*ftrKy;

			double dx = 0;
			double dy = 0;

			float fWidth = (m_fWidth+ftrWidth)*fDrawScale*fViewScale;
			if( fWidth==0.0f )
				fWidth = -fDrawScale*fViewScale;
			
			fRotAngle = m_fAngle+ftrRotateAngle-fRotAngle;
			
			DrawCellToBuf(pBuf,nIndex,&cell,&PT_3D(pt.x+dx,pt.y+dy,pt.z),
				ftrKx,ftrKy,fRotAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,nCoverType,fExtendDis);

			if(nGeoType==CLS_GEOSURFACEPOINT)
			{
				pFt->Draw(pBuf,fDrawScale);
			}
			
		}
		break;
	default:
		{
			break;
		}
	}
	
}



BOOL CCellHatch::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,FALSE);
	
	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if( nIndex==0 )
		return FALSE;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}


BOOL CCellHatch::IsSupportGeoType(int nGeoType)
{
	return TRUE;
}

BOOL CAnnotation::m_bUpwardText = FALSE;
BOOL CAnnotation::m_bBreakText = FALSE;

BOOL CAnnotation::m_bAnnotToText = FALSE;
BOOL CAnnotation::m_bNotDisplayAnnot = FALSE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAnnotation::CAnnotation()
{
	m_nType = SYMTYPE_ANNOTATION;

	m_fXOff = m_fYOff = 0;
	m_nAnnoType = CAnnotation::Text;
	m_nDigit = 0;
	m_nPlaceType = CAnnotation::Head;
	_tcscpy(m_strText,_T("abc"));
	_tcscpy(m_strField,_T(""));
	m_textSettings.nAlignment = TAH_LEFT;
	m_nCoverType = COVERTYPE_NONE;
	m_fExtendDis = 0;	
}

CAnnotation::~CAnnotation()
{
	
}


BOOL CAnnotation::IsNeedAttrWhenDraw()
{
	if(m_nAnnoType==CAnnotation::Attribute)
		return TRUE;

	return FALSE;
}


void CAnnotation::CopyFrom(CSymbol *pSymbol)
{
  	CAnnotation *pAnno = (CAnnotation*)pSymbol;

	m_bUpwardText = pAnno->m_bUpwardText;
	m_fExtendDis = pAnno->m_fExtendDis;
	m_fXOff = pAnno->m_fXOff;
	m_fYOff = pAnno->m_fYOff;
	m_nAnnoType = pAnno->m_nAnnoType;
	m_nCoverType = pAnno->m_nCoverType;
	m_nDigit = pAnno->m_nDigit;
	m_nPlaceType = pAnno->m_nPlaceType;
	_tcscpy(m_strText,pAnno->m_strText);
	_tcscpy(m_strField,pAnno->m_strField);

	m_textSettings = pAnno->m_textSettings;
}


BOOL CAnnotation::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_XOFFSET) )
			{
				data = xmlFile.GetData();
				m_fXOff = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_YOFFSET) )
			{
				data = xmlFile.GetData();
				m_fYOff = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_ANNOTYPE) )
			{
				data = xmlFile.GetData();
				m_nAnnoType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_DIGIT) )
			{
				data = xmlFile.GetData();
				m_nDigit = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_PLACETYPE) )
			{
				data = xmlFile.GetData();
				m_nPlaceType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_TEXT) )
			{
				data = xmlFile.GetData();
				_tcscpy(m_strText,data);
			}

			if( xmlFile.FindElem(XMLTAG_FIELD) )
			{
				data = xmlFile.GetData();
				_tcscpy(m_strField,data);
			}

			if( xmlFile.FindElem(XMLTAG_COVERTYPE) )
			{
				data = xmlFile.GetData();
				m_nCoverType = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_EXTENDDIS) )
			{
				data = xmlFile.GetData();
				m_fExtendDis = _ttof(data);
			}	
			
			if( xmlFile.FindElem(XMLTAG_TEXTFONT) )
			{
				xmlFile.IntoElem();	
				
				if( xmlFile.FindElem(XMLTAG_TEXTHEIGHT) )
				{
					data = xmlFile.GetData();
					m_textSettings.fHeight = _ttof(data);
				}
				if( xmlFile.FindElem(XMLTAG_TEXTWIDTHSCALE) )
				{
					data = xmlFile.GetData();
					m_textSettings.fWidScale = _ttof(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTINTVSCALE) )
				{
					data = xmlFile.GetData();
					m_textSettings.fCharIntervalScale = _ttof(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTLNSPCESCALE) )
				{
					data = xmlFile.GetData();
					m_textSettings.fLineSpacingScale = _ttof(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTTEXTANGLE) )
				{
					data = xmlFile.GetData();
					m_textSettings.fTextAngle = _ttof(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTCHARANGLE) )
				{
					data = xmlFile.GetData();
					m_textSettings.fCharAngle = _ttof(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTALIGN) )
				{
					data = xmlFile.GetData();
					m_textSettings.nAlignment = _ttoi(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTINCLINETYPE) )
				{
					data = xmlFile.GetData();
					m_textSettings.nInclineType = _ttoi(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTINCLINEANGLE) )
				{
					data = xmlFile.GetData();
					m_textSettings.fInclineAngle = _ttof(data);
				}
				
				if( xmlFile.FindElem(XMLTAG_TEXTFONTNAME) )
				{
					data = xmlFile.GetData();
					_tcscpy(m_textSettings.tcFaceName,data);
				}

				xmlFile.OutOfElem();
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	
	return TRUE;
	
}

void CAnnotation::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fXOff);
			xmlFile.AddElem(XMLTAG_XOFFSET,data);
			
			data.Format(_T("%f"),m_fYOff);
			xmlFile.AddElem(XMLTAG_YOFFSET,data);

			data.Format(_T("%d"),m_nAnnoType);
			xmlFile.AddElem(XMLTAG_ANNOTYPE,data);

			data.Format(_T("%d"),m_nDigit);
			xmlFile.AddElem(XMLTAG_DIGIT,data);

			data.Format(_T("%d"),m_nPlaceType);
			xmlFile.AddElem(XMLTAG_PLACETYPE,data);

			xmlFile.AddElem(XMLTAG_TEXT,m_strText);

			xmlFile.AddElem(XMLTAG_FIELD,m_strField);

			data.Format(_T("%d"),m_nCoverType);
			xmlFile.AddElem(XMLTAG_COVERTYPE,data);
			
			data.Format(_T("%f"),m_fExtendDis);
			xmlFile.AddElem(XMLTAG_EXTENDDIS,data);
 
			if( xmlFile.AddElem(XMLTAG_TEXTFONT) )
			{
				xmlFile.IntoElem();			

				data.Format(_T("%f"),m_textSettings.fHeight);
				xmlFile.AddElem(XMLTAG_TEXTHEIGHT,data);

				data.Format(_T("%lf"),m_textSettings.fWidScale);
				xmlFile.AddElem(XMLTAG_TEXTWIDTHSCALE,data);			
 
				data.Format(_T("%lf"),m_textSettings.fCharIntervalScale);
				xmlFile.AddElem(XMLTAG_TEXTINTVSCALE,data);
 				
				data.Format(_T("%lf"),m_textSettings.fLineSpacingScale);
				xmlFile.AddElem(XMLTAG_TEXTLNSPCESCALE,data);
				
				data.Format(_T("%lf"),m_textSettings.fTextAngle);
				xmlFile.AddElem(XMLTAG_TEXTTEXTANGLE,data);
				
				data.Format(_T("%lf"),m_textSettings.fCharAngle);
				xmlFile.AddElem(XMLTAG_TEXTCHARANGLE,data);
 				
				data.Format(_T("%d"),m_textSettings.nAlignment);
				xmlFile.AddElem(XMLTAG_TEXTALIGN,data);

				data.Format(_T("%d"),m_textSettings.nInclineType);
				xmlFile.AddElem(XMLTAG_TEXTINCLINETYPE,data);
 
				data.Format(_T("%lf"),m_textSettings.fInclineAngle);
				xmlFile.AddElem(XMLTAG_TEXTINCLINEANGLE,data);
 				
				xmlFile.AddElem(XMLTAG_TEXTFONTNAME,m_textSettings.tcFaceName);
 
				xmlFile.OutOfElem();
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	

BOOL CAnnotation::ExtractGeoText(const CFeature *pFtr, CPtrArray &geoText, CValueTable &tab, float fDrawScale, float fRotAngle)
{
	if( !pFtr )return FALSE;
	CGeometry *pObj = pFtr->GetGeometry();
	if (!pObj)  return FALSE;
	
	CArray<PT_3DEX,PT_3DEX> pts;
	pObj->GetShape(pts);
	int ptsize = pts.GetSize();
	if (ptsize < 1) return FALSE;

	if (pObj->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		if( !((CGeoCurveBase*)pObj)->IsClosed() && m_nPlaceType==CAnnotation::ByClosedFtrCenter )
			return FALSE;
	}

	CString lpszFormat = _T("%0.f");
	CString tmp;
	tmp.Format(_T("%i"),m_nDigit);
	lpszFormat.Insert(3,tmp);

	CString strGeoText;
	switch(m_nAnnoType)
	{
	case CAnnotation::Text:
		{
			strGeoText.Format(_T("%s"), m_strText);
			break;
		}
	case CAnnotation::Height:
		{
			PT_3DEX pt = pts.GetAt(0);
			strGeoText.Format(lpszFormat, pt.z);
			break;
		}
	case CAnnotation::DHeight:
		{
			double z = 0;
			if (ptsize >= 2)
			{
				z = pts.GetAt(0).z - pts.GetAt(1).z;
			}
			strGeoText.Format(lpszFormat, z);
			break;
		}
	case CAnnotation::Attribute:
		{
			const CVariantEx *var; 
			if (tab.GetValue(0,m_strField,var))
			{
				strGeoText.Empty();
				switch( var->GetType() ) 
				{			
				case VT_I1:
				case VT_UI1:
					strGeoText.Format(_T("%d"),(BYTE)(_variant_t)*var);
					break;
				case VT_I2:
				case VT_UI2:
					strGeoText.Format(_T("%d"),(short)(_variant_t)*var);
					break;
				case VT_I4:
				case VT_UI4:
				case VT_INT:
				case VT_UINT:
					strGeoText.Format(_T("%d"),(long)(_variant_t)*var);
					break;
				case VT_BSTR:
					strGeoText.Format(_T("%s"),(LPCTSTR)(_bstr_t)(_variant_t)*var);
					break;
				case VT_BOOL:
					strGeoText.Format(_T("%d"),((bool)(_variant_t)*var)?1:0);
					break;
				case VT_R4:
					strGeoText.Format(lpszFormat,(float)(_variant_t)*var);
					break;
				case VT_R8:
					strGeoText.Format(lpszFormat,(double)(_variant_t)*var);
					break;
				default:;
				}
			}
			break;
		}
	default:
		return FALSE;
	}

	//放置位置	
	TEXT_SETTINGS0 settings;
	settings.fHeight = m_textSettings.fHeight;
	settings.fWidScale = m_textSettings.fWidScale;
	settings.fCharIntervalScale = m_textSettings.fCharIntervalScale;
	settings.fLineSpacingScale = m_textSettings.fLineSpacingScale;
	settings.fTextAngle = m_textSettings.fTextAngle;
	settings.fCharAngle = m_textSettings.fCharAngle;
	settings.nAlignment = m_textSettings.nAlignment;
	settings.nInclineType = m_textSettings.nInclineType;
	settings.fInclineAngle = m_textSettings.fInclineAngle;
	_tcscpy(settings.strFontName,m_textSettings.tcFaceName);
	CArray<PT_3DEX, PT_3DEX> placept;	
	switch(m_nPlaceType)
	{
	case CAnnotation::Head:
		{
			settings.nPlaceType = singPt;
			placept.Add(pts[0]);
			break;
		}
	case CAnnotation::Tail:
		{
			settings.nPlaceType = singPt;
			placept.Add(pts[ptsize-1]);
			break;
		}
	case CAnnotation::Middle:
		{
			settings.nPlaceType = singPt;
			placept.Add(pts[ptsize/2]);
			break;
		}
	case CAnnotation::ByClosedFtrCenter:
	case CAnnotation::Center:
		{
			settings.nPlaceType = singPt;
			{
				PT_3DEX pt;
				GraphAPI::GGetCenter(pts.GetData(),pts.GetSize(),&pt);
				placept.Add(pt);
// 				int xsum, ysum, zsum;
// 				xsum = ysum = zsum = 0;
// 				for(int i=0; i<ptsize; i++)
// 				{
// 					xsum += pts[i].x;
// 					ysum += pts[i].y;
// 					zsum += pts[i].z;
// 				}
// 				PT_3DEX pt;
// 				pt.x = xsum / ptsize;
// 				pt.y = ysum / ptsize;
// 				pt.z = zsum / ptsize;
// 				placept.Add(pt);
				
			}
			break;
		}
	case CAnnotation::PerVertex:
		{
			settings.nPlaceType = mutiPt;
			placept.Copy(pts);			
			break;
		}
	case CAnnotation::ByLineHoriz:
		{
			settings.nPlaceType = byLineH;
			placept.Copy(pts);	
			break;
		}
	case CAnnotation::ByLineHorizEven:
		{
			settings.nPlaceType = byLineGridH;
			placept.Copy(pts);	
			break;
		}
	case CAnnotation::ByLineVert:
		{
			settings.nPlaceType = byLineV;
			placept.Copy(pts);	
			break;
		}
	case CAnnotation::ByLineVertEven:
		{
			settings.nPlaceType = byLineGridV;
			placept.Copy(pts);	
			break;
		}
	default:
		return FALSE;
		
	}

	double dx = m_fXOff*fDrawScale,dy = m_fYOff*fDrawScale;
	
	if (m_bUpwardText)
	{
		double cosa = cos(fRotAngle);
		double sina = sin(fRotAngle);
		dx = m_fXOff*fDrawScale*cosa-m_fYOff*fDrawScale*sina;
		dy = m_fXOff*fDrawScale*sina+m_fYOff*fDrawScale*cosa;		
	}

	for (int i=0; i<placept.GetSize(); i++)
	{
		placept[i].pencode = penMove;
		placept[i].x += dx; placept[i].y += dy;
		CGeoText *tmp = new CGeoText;
		if (!tmp) continue;

		tmp->CreateShape(&placept[i],1);
		tmp->SetSettings(&settings);
		tmp->SetText(strGeoText);
		tmp->m_nCoverType = m_nCoverType;
		tmp->m_fExtendDis = m_fExtendDis;
		tmp->SetSymbolName("*");
		geoText.Add(tmp);
	}		
	
	return TRUE;
}


BOOL CAnnotation::Explode(const CFeature *pFt, CValueTable &tab, float fDrawScale, CGeoArray& arrPGeos)
{	
	CPtrArray arrPTexts;
	ExtractGeoText(pFt,arrPTexts,tab,fDrawScale);

	for( int i=0; i<arrPTexts.GetSize(); i++)
	{
		arrPGeos.Add((CGeometry*)arrPTexts[i]);
	}

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



BOOL CAnnotation::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{	
	CValueTable tab;

	return Explode(pFt,tab,fDrawScale,arrPGeos);
	
	return TRUE;
}


void CAnnotation::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	CValueTable tab;
	tab.BeginAddValueItem();
	((CFeature*)pFt)->WriteTo(tab);
	tab.EndAddValueItem();

	Draw(pFt,pBuf,tab,fDrawScale,fRotAngle,fViewScale);
}

void CAnnotation::Draw(const CFeature *pFt, GrBuffer *pBuf, CValueTable &tab, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	BOOL bAnnotToText = m_bAnnotToText;
	BOOL bNotDisplay  = m_bNotDisplayAnnot;
	BOOL bBreakText  = m_bBreakText;

	if( bNotDisplay )return ;

	if( bAnnotToText && (m_nType==3) )return ;
	CGeometry *pGeo = pFt->GetGeometry();

	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		if( !((CGeoCurveBase*)pGeo)->IsClosed() && m_nPlaceType==CAnnotation::ByClosedFtrCenter )
			return;
	}

	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)) && m_nAnnoType==CAnnotation::LayerConfig)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	

	CArray<PT_3DEX,PT_3DEX> pts;
	pGeo->GetShape(pts);
	int ptsize = pts.GetSize();

	CString lpszFormat = _T("%0.f");
	CString tmp;
	tmp.Format(_T("%i"),m_nDigit);
	lpszFormat.Insert(3,tmp);

	CString strGeoText;
	switch(m_nAnnoType)
	{
	case CAnnotation::Text:
		{
			strGeoText.Format(_T("%s"), m_strText);
			break;
		}
	case CAnnotation::Height:
		{
			PT_3DEX pt = pts.GetAt(0);
			strGeoText.Format(lpszFormat, pt.z);
			break;
		}
	case CAnnotation::DHeight:
		{
			double z = 0;
			if (ptsize >= 2)
			{
				z = pts.GetAt(0).z - pts.GetAt(1).z;
			}
			strGeoText.Format(lpszFormat, z);
			break;
		}
	case CAnnotation::Attribute:
		{
			const CVariantEx *var; 
			if (tab.GetValue(0,m_strField,var))
			{
				strGeoText.Empty();
				switch( var->GetType() ) 
				{			
				case VT_I1:
				case VT_UI1:
					strGeoText.Format(_T("%d"),(BYTE)(_variant_t)*var);
					break;
				case VT_I2:
				case VT_UI2:
					strGeoText.Format(_T("%d"),(short)(_variant_t)*var);
					break;
				case VT_I4:
				case VT_UI4:
				case VT_INT:
				case VT_UINT:
					strGeoText.Format(_T("%d"),(long)(_variant_t)*var);
					break;
				case VT_BSTR:
					strGeoText.Format(_T("%s"),(LPCTSTR)(_bstr_t)(_variant_t)*var);
					break;
				case VT_BOOL:
					strGeoText.Format(_T("%d"),((bool)(_variant_t)*var)?1:0);
					break;
				case VT_R4:
					strGeoText.Format(lpszFormat,(float)(_variant_t)*var);
					break;
				case VT_R8:
					strGeoText.Format(lpszFormat,(double)(_variant_t)*var);
					break;
				default:;
				}
			}
			
			break;
		}
	case CAnnotation::Length:
		{
			if(!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				return;
			double len = ((CGeoCurveBase*)pGeo)->GetLength();
			strGeoText.Format(lpszFormat, len);
			break;
		}
	case CAnnotation::Area:
		{
			if(!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
				return;
			double area = ((CGeoCurveBase*)pGeo)->GetArea();
			strGeoText.Format(lpszFormat, area);
			break;
		}
	default:
		break;
	}
	double dx = m_fXOff*fDrawScale*fViewScale,dy = m_fYOff*fDrawScale*fViewScale;

	if (m_bUpwardText)
	{
		double cosa = cos(fRotAngle);
		double sina = sin(fRotAngle);
		dx = m_fXOff*fDrawScale*fViewScale*cosa-m_fYOff*fDrawScale*fViewScale*sina;
		dy = m_fXOff*fDrawScale*fViewScale*sina+m_fYOff*fDrawScale*fViewScale*cosa;		
	}

	TEXT_SETTINGS0 settings;
	settings.fHeight = m_textSettings.fHeight*fDrawScale*fViewScale;
	settings.fWidScale = m_textSettings.fWidScale;
	settings.fCharIntervalScale = m_textSettings.fCharIntervalScale;
	settings.fLineSpacingScale = m_textSettings.fLineSpacingScale;
	settings.fTextAngle = m_textSettings.fTextAngle;
	settings.fCharAngle = m_textSettings.fCharAngle;
	settings.nAlignment = m_textSettings.nAlignment;
	settings.nInclineType = m_textSettings.nInclineType;
	settings.fInclineAngle = m_textSettings.fInclineAngle;
	_tcscpy(settings.strFontName,m_textSettings.tcFaceName);

	long color = pGeo->GetColor();

	int     nCoverType = 0;
	float   fExtendDis = 0;
	
	//考虑到符号之间互相压盖的问题，我们在符号解释中，不再使用对象属性中的压盖
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoPoint)))
	{
		//nCoverType = ((CGeoPoint*)pGeo)->m_nCoverType;
		//fExtendDis = ((CGeoPoint*)pGeo)->m_fExtendDis*fDrawScale*fViewScale;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoText)))
	{
		//nCoverType = ((CGeoText*)pGeo)->m_nCoverType;
		//fExtendDis = ((CGeoText*)pGeo)->m_fExtendDis*fDrawScale*fViewScale;
	}

	if( nCoverType==0 && m_nCoverType!=0 )
		nCoverType = m_nCoverType;

	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOPOINT:
	case CLS_GEODIRPOINT:
		{
			PT_3DEX pt = pGeo->GetDataPoint(0);

			pt.x = pt.x+dx;
			pt.y = pt.y+dy;

			GrBuffer grbuf;

			// 压盖
			if (nCoverType > 0 || m_nCoverType > 0)
			{
				GrBuffer grbuf1, grCoverBuf;
				
				TextSettings setPar;
				memcpy(&setPar,&m_textSettings,sizeof(TextSettings));
				setPar.fHeight = setPar.fHeight*fDrawScale*fViewScale;		
				
				grbuf1.Text(color,&pt,strGeoText,&setPar,TRUE,!m_bUpwardText);
				
				Envelope e = grbuf1.GetEnvelope();

				if( fExtendDis<m_fExtendDis )
					fExtendDis = m_fExtendDis;

				if (nCoverType > 0 || m_nCoverType>0 )
				{
					Envelope ee(e.m_xl-fExtendDis,e.m_xh+fExtendDis,e.m_yl-fExtendDis,e.m_yh+fExtendDis);
					CreateCoverPolygon(nCoverType,ee,pt,&grCoverBuf);
				}			

				pBuf->AddBuffer(&grCoverBuf);
			}

			if (bBreakText)
			{
				CPlotTextEx plot;
				plot.SetSettings(&settings);	

				CShapeLine shape;
				CLinearizer line;
				line.SetShapeBuf(&shape);
				line.Linearize(&pt,1,FALSE);				
				shape.LineEnd();
				
				plot.SetShape(&shape);
				plot.SetText(strGeoText);
				plot.GetOutLineBuf(&grbuf);
			}
			else
			{
				TextSettings setPar;
				memcpy(&setPar,&m_textSettings,sizeof(TextSettings));
				setPar.fHeight = setPar.fHeight*fDrawScale*fViewScale;		
				
				grbuf.Text(color,&pt,strGeoText,&setPar,TRUE,!m_bUpwardText);
			}
			
			grbuf.SetAllColor(color);			

			pBuf->AddBuffer(&grbuf);

			break;
		}
	case CLS_GEOCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
	case CLS_GEOPARALLEL:
		{
			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);
			if(pts.GetSize() < 1)
				break;

			if (nGeoType == CLS_GEOPARALLEL)
			{
				CGeoParallel *pParallel = (CGeoParallel*)pGeo;
				CArray<PT_3DEX,PT_3DEX> ptex;
				pParallel->GetParallelShape(ptex);
				pts.Append(ptex);
				
			}

			int ptsize = pts.GetSize();
			PT_3DEX pt;

			CArray<PT_3DEX,PT_3DEX> arr;

			GrBuffer grbuf;

			switch(m_nPlaceType)
			{
			case CAnnotation::Head:
				{
					pt = pts[0];
					break;
				}
			case CAnnotation::Tail:
				{
					pt = pts[ptsize-1];
					break;
				}
			case CAnnotation::Middle:
				{				
					int index = GraphAPI::GGetMiddlePt(pts.GetData(),pts.GetSize(),&pt);
					pt.pencode = pts[0].pencode;
					break;
				}
			case CAnnotation::ByClosedFtrCenter:
			case CAnnotation::Center:
				{
					PT_3DEX ptex0 = pts.GetAt(0);
					pGeo->GetCenter(&ptex0,&pt);
					pt.pencode = ptex0.pencode;
					break;
				}
			case CAnnotation::PerVertex:
				{
					arr.Copy(pts);
					settings.nPlaceType = mutiPt;
					break;
				}
			case CAnnotation::ByLineHoriz:
				{
					arr.Copy(pts);
					settings.nPlaceType = byLineH;
					break;
				}
			case CAnnotation::ByLineHorizEven:
				{
					arr.Copy(pts);
					settings.nPlaceType = byLineGridH;
					break;
				}
			case CAnnotation::ByLineVert:
				{
					arr.Copy(pts);
					settings.nPlaceType = byLineV;
					break;
				}
			case CAnnotation::ByLineVertEven:
				{
					arr.Copy(pts);
					settings.nPlaceType = byLineGridV;
					break;
				}
			default:
				break;
			}

			if (m_nPlaceType == CAnnotation::Head || m_nPlaceType == CAnnotation::Tail 
					|| m_nPlaceType == CAnnotation::Middle || m_nPlaceType == CAnnotation::Center || m_nPlaceType == CAnnotation::ByClosedFtrCenter)
			{
				pt.x += dx;
				pt.y += dy;
			}

			if (bBreakText)
			{
				CPlotTextEx plot;
				plot.SetSettings(&settings);	
				
				CShapeLine shape;
				CLinearizer line;
				line.SetShapeBuf(&shape);
				if (m_nPlaceType == CAnnotation::Head || m_nPlaceType == CAnnotation::Tail 
					|| m_nPlaceType == CAnnotation::Middle || m_nPlaceType == CAnnotation::Center || m_nPlaceType == CAnnotation::ByClosedFtrCenter)
				{
					line.Linearize(&pt,1,FALSE);
				}
				else
				{
					line.Linearize(arr.GetData(),arr.GetSize(),FALSE);
				}
								
				shape.LineEnd();
				
				plot.SetShape(&shape);
				plot.SetText(strGeoText);
				plot.GetOutLineBuf(&grbuf);
			}
			else
			{
				if (m_nPlaceType == CAnnotation::Head || m_nPlaceType == CAnnotation::Tail 
					|| m_nPlaceType == CAnnotation::Middle || m_nPlaceType == CAnnotation::Center || m_nPlaceType == CAnnotation::ByClosedFtrCenter)
				{
					TextSettings setPar;
					memcpy(&setPar,&m_textSettings,sizeof(TextSettings));
					setPar.fHeight = setPar.fHeight*fDrawScale*fViewScale;
					grbuf.Text(color,&pt,strGeoText,&setPar,TRUE,!m_bUpwardText);
				}
 				else
				{
					CGeoText *pText = new CGeoText;
					pText->CreateShape(arr.GetData(), arr.GetSize());
					pText->SetSettings(&settings);
					pText->SetText(strGeoText);
					pText->m_nCoverType = m_nCoverType;
					pText->m_fExtendDis = m_fExtendDis;
					pText->SetSymbolName("*");
					pText->Draw(&grbuf, fDrawScale);
					delete pText;
				}
			}
			
			grbuf.SetAllColor(pGeo->GetColor());
			
//			grbuf.Move(m_fXOff*fDrawScale*fViewScale,m_fYOff*fDrawScale*fViewScale);
			
//			grbuf.End();
			
			pBuf->AddBuffer(&grbuf);
			
			break;
		
		}
	default:
		{
			break;
		}
	}
	
}

BOOL CAnnotation::IsSupportGeoType(int nGeoType)
{
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleLinetype::CScaleLinetype()
{
	m_nType = SYMTYPE_SCALELINETYPE;

	m_nPlaceType = 0; 
	m_lfCycle = 0;
	m_lfXOffset0 = m_lfYOffset0 = 0;
	m_lfYOffsetType0 = m_lfYOffsetType1 = 0;
	m_lfXOffset1 = m_lfYOffset1 = 0;
	m_lfWidth = 0;
	m_bAverageDraw = FALSE;
}

CScaleLinetype::~CScaleLinetype()
{
	
}

void CScaleLinetype::CopyFrom(CSymbol *pSymbol)
{
	CScaleLinetype *pScale = (CScaleLinetype*)pSymbol;
	m_lfCycle = pScale->m_lfCycle;
	m_lfWidth = pScale->m_lfWidth;
	m_lfXOffset0 = pScale->m_lfXOffset0;
	m_lfXOffset1 = pScale->m_lfXOffset1;
	m_lfYOffset0 = pScale->m_lfYOffset0;
	m_lfYOffset1 = pScale->m_lfYOffset1;
	m_nPlaceType = pScale->m_nPlaceType;
	m_lfYOffsetType0 = pScale->m_lfYOffsetType0;
	m_lfYOffsetType1 = pScale->m_lfYOffsetType1;
	m_bAverageDraw = pScale->m_bAverageDraw;
	m_strBaseLinetypeName = pScale->m_strBaseLinetypeName;
}

BOOL CScaleLinetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_CYCLE) )
			{
				data = xmlFile.GetData();
				m_lfCycle = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_lfWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_FIRXOFFSET) )
			{
				data = xmlFile.GetData();
				m_lfXOffset0 = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_ENDXOFFSET) )
			{
				data = xmlFile.GetData();
				m_lfXOffset1 = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_FIRYOFFSET) )
			{
				data = xmlFile.GetData();
				m_lfYOffset0 = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_ENDYOFFSET) )
			{
				data = xmlFile.GetData();
				m_lfYOffset1 = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_PLACETYPE) )
			{
				data = xmlFile.GetData();
				m_nPlaceType = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_FIRYOFFTYPE) )
			{
				data = xmlFile.GetData();
				m_lfYOffsetType0 = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_ENDYOFFTYPE) )
			{
				data = xmlFile.GetData();
				m_lfYOffsetType1 = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_AVERAGEDRAW) )
			{
				data = xmlFile.GetData();
				m_bAverageDraw = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}


			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleLinetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_lfCycle);
			xmlFile.AddElem(XMLTAG_CYCLE,data);
			
			data.Format(_T("%f"),m_lfWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);

			data.Format(_T("%f"),m_lfXOffset0);
			xmlFile.AddElem(XMLTAG_FIRXOFFSET,data);

			data.Format(_T("%f"),m_lfXOffset1);
			xmlFile.AddElem(XMLTAG_ENDXOFFSET,data);

			data.Format(_T("%f"),m_lfYOffset0);
			xmlFile.AddElem(XMLTAG_FIRYOFFSET,data);

			data.Format(_T("%f"),m_lfYOffset1);
			xmlFile.AddElem(XMLTAG_ENDYOFFSET,data);

			data.Format(_T("%d"),m_nPlaceType);
			xmlFile.AddElem(XMLTAG_PLACETYPE,data);
			
			data.Format(_T("%f"),m_lfYOffsetType0);
			xmlFile.AddElem(XMLTAG_FIRYOFFTYPE,data);
			
			data.Format(_T("%f"),m_lfYOffsetType1);
			xmlFile.AddElem(XMLTAG_ENDYOFFTYPE,data);

			data.Format(_T("%d"),m_bAverageDraw);
			xmlFile.AddElem(XMLTAG_AVERAGEDRAW,data);

			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);

			
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	

//起点前
template<class T>
void GGetCycleLine(const T *pts,int nSum, const double dCycle, const double dXOff,CArray<T,T> &dpts)
{
	if(pts==NULL || nSum<1 || dCycle<=0)
		return;
	dpts.RemoveAll();
	
	//计算每一线段的长度,存储到pLen中
	float *pLen = new float[nSum];
	
	float  *p = pLen;
	double xold, yold, zold;
	
	const T *pt;
	pt = pts;
	int nFlag = nSum;
	while( nFlag-- )
	{
		if (p==pLen) *p = 0.0;
		else
		{
			*(p) = *(p-1) + sqrt( (pt->x-xold)*(pt->x-xold) +
				(pt->y-yold)*(pt->y-yold) +
				(pt->z-zold)*(pt->z-zold) );
		}
		xold = pt->x;
		yold = pt->y;
		zold = pt->z;
		
		p++;
		pt++;
	}
	//根据周期计算每一个周期点
	float ratio;
	float dis = dXOff;
	if(dis == 0)
	{
		dpts.Add(pts[0]);
	}
	dis += dCycle;
	
	int i = 1;
	BOOL bFlag = FALSE;
	while(i < nSum)
	{
		if(dis < 0)
		{
			dis += dCycle;
			//起点前
// 			dpts.Add(pts[0]);
// 			bFlag = TRUE;
		}
		else if(dis <= pLen[i] && pLen[i]>pLen[i-1] )
		{
			
			ratio = (dis-pLen[i-1]) / (pLen[i]-pLen[i-1]);
			T t3d;
			t3d.x = pts[i-1].x + ratio * (pts[i].x - pts[i-1].x);
			t3d.y = pts[i-1].y + ratio * (pts[i].y - pts[i-1].y);
			t3d.z = pts[i-1].z + ratio * (pts[i].z - pts[i-1].z);
			dpts.Add(t3d);
			dis += dCycle;
			if(dis > pLen[i])
				i++;
		}
		else
			i++;
	}
	//起点后
// 	if (!bFlag)
// 		dpts.Add(pts[nSum-1]);

	if(pLen)
		delete []pLen;
	
	
}

double CScaleLinetype::LayAlongLine(PT_3DEX *pts, int num, PT_3DEX *dpt, int dptnum, PT_3DEX *ratatept)
{
	PT_2D *pts2d = new PT_2D[num];
	if (!pts2d)  return 0;

	for (int j=0; j<num; j++)//获取点数组
	{
		COPY_2DPT(pts2d[j],pts[j]);
	}
		
	int index;
	PT_2D center2d, pt2ddp;
	COPY_2DPT(center2d,*ratatept);	
	GraphAPI::GGetNearstDisPToPointSet2D(pts2d,num,center2d,pt2ddp,&index);
	if (index < 0)  return 0;
				
	double angle = GraphAPI::GGetAngle(pts[index].x,pts[index].y,pts[index+1].x,pts[index+1].y);	
				
	double matrix[16];
	Matrix44FromRotate(ratatept,angle,matrix);
	GraphAPI::TransformPointsWith44Matrix(matrix,dpt,dptnum);

	if (pts2d)
	{
		delete []pts2d;
	}

	return angle;
	
}

template<class T>
static BOOL GIsLineIntersectLineSeg(T pt0, T pt1,const T *pts, int num, T &result)
{
	if (pts == NULL || num < 2)
	{
		return FALSE;
	}

	double dx = pt1.x - pt0.x;
	double dy = pt1.y - pt0.y;

	T pt2 = pt0;
	pt2.x += dx*10000;
	pt2.y += dy*10000;

	T pt3, pt4;
	for (int i=1; i<num; i++)
	{
		pt3 = pts[i-1];
		pt4 = pts[i];

		if ( GraphAPI::GGetLineIntersectLineSeg(pt0.x,pt0.y,pt2.x,pt2.y,pt3.x,pt3.y,pt4.x,pt4.y,&result.x,&result.y,NULL) )
			return TRUE;
	
	}

	return FALSE;
	
}

typedef struct tag_PtIntersect
{
	tag_PtIntersect()
	{
		idx = -1;
		lfRatio1 = -1;
		lfRatio2 = -1;
	}
	int idx;
	PT_3D pt1,pt2;
	double lfRatio1, lfRatio2;
}PtIntersect;

void CScaleLinetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	BOOL bBreakLinetype = m_bBreakLinetype;

	CGeometry *pGeo = pFt->GetGeometry();
	int nGeoType = pGeo->GetClassType();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid, ftrXoff = 0;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
		ftrXoff = pCurve->m_fLinetypeXoff;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		ftrWid = ((CGeoSurface*)pGeo)->m_fLineWidth;
	}

	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		CGeoCurveBase *pCurve = (CGeoCurveBase*)pGeo;
		if( pCurve->IsClosed() && (m_nPlaceType==3||m_nPlaceType==4||m_nPlaceType==5) )
		{
			return;
		}
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_lfWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	if (nGeoType != CLS_GEOPARALLEL && nGeoType != CLS_GEODCURVE)
	{
	}
	else
	{
		// 比例线型的比例宽度
		double dScaleWidth = 0;
		
		CArray<PT_3DEX,PT_3DEX> pts, assistPts;

		if (nGeoType == CLS_GEOPARALLEL)
		{
			CGeoParallel *pParallel = (CGeoParallel*)pGeo;
			dScaleWidth = pParallel->GetWidth();

			const CShapeLine *pShape =  pParallel->GetShape();
			pShape->GetPts(pts);

			assistPts.SetSize(pts.GetSize());
			GraphAPI::GGetParallelLine(pts.GetData(),pts.GetSize(),dScaleWidth,assistPts.GetData());
		}
		else if (nGeoType == CLS_GEODCURVE)
		{
			CGeoDCurve *pDCurve = (CGeoDCurve*)pGeo;
			dScaleWidth = pDCurve->GetWidth();
			
			pDCurve->GetBaseShape(pts);
			pDCurve->GetAssistShape(assistPts);
			ReversePoints(assistPts.GetData(),assistPts.GetSize());

			// 暂支持固定周期
			if (m_nPlaceType == 1 || m_nPlaceType == 2 || assistPts.GetSize() < 2)
			{
				return;
			}
		}		
		
		int npsum = pts.GetSize();

		if (npsum < 1) return;

		CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
		int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;

		// 线型库找不到线此线型
		if (nIndex == 0)
		{
			return;
		}

		int  nStyle = 0;
		float styles[8] = {0};
		
		if( nIndex>0 && nIndex<=baselineLib->GetBaseLineTypeCount() )
		{
			BaseLineType linetype = baselineLib->GetBaseLineType(nIndex-1);		
			
			nStyle = linetype.m_nNum;
			for( int i=0; i<linetype.m_nNum; i++)
			{
				styles[i] = fabs(linetype.m_fLens[i])*fDrawScale*ftrLinetypeScale;
			}
			
		}
		
		DWORD nColor = pGeo->GetColor();

		double xoff0 = m_lfXOffset0*fDrawScale*fViewScale + ftrXoff;
		
		double yoff0 = m_lfYOffset0*fDrawScale*fViewScale*(dScaleWidth>0?1:-1) + dScaleWidth*m_lfYOffsetType0;
		
		double xoff1 = m_lfXOffset1*fDrawScale*fViewScale + ftrXoff;
		
		double yoff1 = m_lfYOffset1*fDrawScale*fViewScale*(dScaleWidth>0?1:-1) + dScaleWidth*m_lfYOffsetType1;
		
		if (m_nPlaceType==0 || m_nPlaceType==1)
		{
			double dcycle = m_lfCycle*fDrawScale*fViewScale;
			if (m_nPlaceType == 1)	dcycle *= fabs(dScaleWidth);
			
			dcycle = fabs(dcycle);
			if (dcycle < 1e-4)	return;

			GrBuffer grbuf;


			// 1,双线改善, 首点均匀过渡到尾点
			// 2,对平行线从母线拉到辅助线的情况也均匀过渡
			if (m_bAverageDraw && fabs(fabs(xoff0)-fabs(xoff1)) < 1e-4)
			{
				int nlastIndex0 = -1, nlastIndex1 = -1, i, j;
				for ( i=0; i<pts.GetSize(); i++)
				{
					if (pts[i].type == ptSpecial)
					{
						for ( j=nlastIndex1+1; j<assistPts.GetSize(); j++)
						{
							if (assistPts[j].type == ptSpecial)
							{
								// 对应特征点处理
								CArray<PT_3DEX,PT_3DEX> basepts1, asspts1;
								int istart = nlastIndex0>=0?nlastIndex0:0, k;
								for ( k=istart; k<=i; k++)
								{
									basepts1.Add(pts[k]);
								}

								istart = nlastIndex1>=0?nlastIndex1:0;
								for ( k=istart; k<=j; k++)
								{
									asspts1.Add(assistPts[k]);
								}

								int npsum1 = basepts1.GetSize();

								// 将母线和辅助线分成等距的线段，然后连接对应的点
								double len0, len1;
								len0 = GraphAPI::GGetAllLen3D(basepts1.GetData(),basepts1.GetSize())-fabs(xoff0);
								len1 = GraphAPI::GGetAllLen3D(asspts1.GetData(),asspts1.GetSize())-fabs(xoff1);
								
								int numLine = ceil(len0/dcycle);
								
								double dcycle0, dcycle1;
								
								// 对周期作调整，保证最后连接线与前面间距一样
								dcycle0 = /*dcycle;//*/len0/numLine;
								dcycle1 = len1/numLine;
								
								CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
								
								GraphAPI::GGetCycleLine(basepts1.GetData(),basepts1.GetSize(),dcycle0,fabs(xoff0),dpt0,0);
								GraphAPI::GGetCycleLine(asspts1.GetData(),asspts1.GetSize(),dcycle1,fabs(xoff1),dpt1,0);
								
								if (ceil(len0/dcycle) == floor(len0/dcycle)+1 && fabs(xoff0) <= 1e-4)
								{
									int assPtSize = asspts1.GetSize();
									dpt0.Add(basepts1[npsum1-1]);
									dpt1.Add(asspts1[assPtSize-1]);
								}			
								
								int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
								for (int i=0; i<nmin; i++)
								{
									PT_3DEX tmppts[2];
									tmppts[0] = dpt0[i];
									tmppts[1] = dpt1[i];
									double a = fabs(m_lfYOffsetType1-1);
									if (a > 1e-4 && a < 1)
									{
										PT_3DEX pt;
										double len = GraphAPI::GGet2DDisOf2P(tmppts[0],tmppts[1]);
										GGetPtOfLine(tmppts,2,len*fabs(m_lfYOffsetType1),&pt);
										tmppts[1] = pt;
									}
									DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
									
								}

								nlastIndex1 = j;

								break;
							}
						}

						if (j > assistPts.GetSize()-1)
						{
							break;
						}

						nlastIndex0 = i;
					}
					
				}

				// 整体无效
				if (nlastIndex0 < 0 || nlastIndex1 < 0)
				{
					// 将母线和辅助线分成等距的线段，然后连接对应的点
					double len0, len1;
					len0 = GraphAPI::GGetAllLen3D(pts.GetData(),pts.GetSize())-fabs(xoff0);
					len1 = GraphAPI::GGetAllLen3D(assistPts.GetData(),assistPts.GetSize())-fabs(xoff1);

					// 对周期作调整，保证最后连接线与前面间距一样
					double dcycle0 = dcycle;
					double dcycle1 = len1*dcycle/len0;
					
					CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
					
					GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dcycle0,fabs(xoff0),dpt0,0);
					GraphAPI::GGetCycleLine(assistPts.GetData(),assistPts.GetSize(),dcycle1,fabs(xoff1),dpt1,0);
					
					//if (ceil(len0/dcycle) == floor(len0/dcycle)+1 && fabs(xoff0) <= 1e-4)
					{
						int assPtSize = assistPts.GetSize();
						dpt0.Add(pts[npsum-1]);
						dpt1.Add(assistPts[assPtSize-1]);
					}			
					
					int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
					for (int i=0; i<nmin; i++)
					{
						PT_3DEX tmppts[2];
						tmppts[0] = dpt0[i];
						tmppts[1] = dpt1[i];
						double a = fabs(m_lfYOffsetType1-1);
						if (a > 1e-4 && a < 1)
						{
							PT_3DEX pt;
							double len = GraphAPI::GGet2DDisOf2P(tmppts[0],tmppts[1]);
							GGetPtOfLine(tmppts,2,len*fabs(m_lfYOffsetType1),&pt);
							tmppts[1] = pt;
						}
						DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
						
					}
				}
				// 最后部分处理
				else
				{
					// 对应特征点处理
					CArray<PT_3DEX,PT_3DEX> basepts1, asspts1;
					int istart = nlastIndex0>=0?nlastIndex0:0, k;
					for ( k=istart; k<pts.GetSize(); k++)
					{
						basepts1.Add(pts[k]);
					}
					
					istart = nlastIndex1>=0?nlastIndex1:0;
					for ( k=istart; k<assistPts.GetSize(); k++)
					{
						asspts1.Add(assistPts[k]);
					}
					
					int npsum1 = basepts1.GetSize();
					
					// 将母线和辅助线分成等距的线段，然后连接对应的点
					double len0, len1;
					len0 = GraphAPI::GGetAllLen3D(basepts1.GetData(),basepts1.GetSize())-fabs(xoff0);
					len1 = GraphAPI::GGetAllLen3D(asspts1.GetData(),asspts1.GetSize())-fabs(xoff1);
					
					// 对周期作调整，保证最后连接线与前面间距一样
					double dcycle0 = dcycle;
					double dcycle1 = len1*dcycle/len0;
					
					CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
					
					GraphAPI::GGetCycleLine(basepts1.GetData(),basepts1.GetSize(),dcycle0,fabs(xoff0),dpt0,0);
					GraphAPI::GGetCycleLine(asspts1.GetData(),asspts1.GetSize(),dcycle1,fabs(xoff1),dpt1,0);
					
					if (ceil(len0/dcycle) == floor(len0/dcycle)+1 && fabs(xoff0) <= 1e-4)
					{
						int assPtSize = asspts1.GetSize();
						dpt0.Add(basepts1[npsum1-1]);
						dpt1.Add(asspts1[assPtSize-1]);
					}			
					
					int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
					for (int i=0; i<nmin; i++)
					{
						PT_3DEX tmppts[2];
						tmppts[0] = dpt0[i];
						tmppts[1] = dpt1[i];
						double a = fabs(m_lfYOffsetType1-1);
						if (a > 1e-4 && a < 1)
						{
							PT_3DEX pt;
							double len = GraphAPI::GGet2DDisOf2P(tmppts[0],tmppts[1]);
							GGetPtOfLine(tmppts,2,len*fabs(m_lfYOffsetType1),&pt);
							tmppts[1] = pt;
						}
						DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
						
					}
				}

			}
			else if (!m_bAverageDraw)
			{
				// 对于从基线拉到辅助线的情况，依比例线形的方向垂直于基线
				if (fabs(fabs(m_lfXOffset0)-fabs(m_lfXOffset1)) < 1e-4 && (fabs(m_lfYOffsetType0) > 1e-4 || fabs(m_lfYOffsetType1) > 1e-4) && fabs(m_lfYOffset0) < 1e-4 && fabs(m_lfYOffset1) < 1e-4)
				{
					CArray<PtIntersect,PtIntersect> arrIntersectPts;

					CUIntArray arrIdx;
					CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
					GetCycleLine(pts.GetData(),pts.GetSize(),dcycle,xoff0,dpt0,arrIdx);
					
					// 垂直于母线与辅助线有交点的直线
					int i=0;
					int num0 = dpt0.GetSize();
					for (i=0; i<num0; i++)
					{
						PT_3DEX ptIntersect, ptAnother;
						double fRatio;
						GGetPtFromLineWidth(pts.GetData(), pts.GetSize(),dpt0.GetAt(i),yoff1, &ptAnother);
						BOOL bIntersect = GraphAPI::GGetFirstPointOfRayIntersect(assistPts.GetData(),assistPts.GetSize(),dpt0.GetAt(i),ptAnother,&ptIntersect,&fRatio,NULL);
						if(!bIntersect) continue;
						int indexNest = -1;
						PT_3DEX ptIntersectNest;
						GraphAPI::GGetNearstDisPToPointSet2D(assistPts.GetData(),assistPts.GetSize(),dpt0.GetAt(i),ptIntersectNest,&indexNest);
					
						PtIntersect pt0;

						pt0.idx = i;
						pt0.pt1 = dpt0[i];
						

						pt0.lfRatio1 = arrIdx[i];

// 						if (!bIntersect || GraphAPI::GGet2DDisOf2P(dpt0[i],ptIntersect) > 1.5*GraphAPI::GGet2DDisOf2P(dpt0[i],ptIntersectNest))
// 						{
// 							pt0.pt2 = ptIntersectNest;
// 							pt0.lfRatio2 = indexNest;
// 						}
// 						else
						{
							pt0.pt2 = ptIntersect;
							pt0.lfRatio2 = fRatio;
						}
						
						arrIntersectPts.Add(pt0);
					}

					// 删除arrLine中相互有交点的直线以及不合理的直线（间距太大或太小）
/*					CArray<PtIntersect,PtIntersect> tmpIntersectPts;
					int intersectNum = arrIntersectPts.GetSize();					
					for (i=0; i<intersectNum; i++)
					{
						if (i == 0)
						{
							tmpIntersectPts.Add(arrIntersectPts[0]);
							continue;
						}

						for (int j=i; j<intersectNum; j++)
						{
							if (GraphAPI::GGetLineIntersectLineSeg(arrIntersectPts[i].pt1.x,arrIntersectPts[i].pt1.y,
								arrIntersectPts[i].pt2.x,arrIntersectPts[i].pt2.y,
								arrIntersectPts[j].pt1.x,arrIntersectPts[j].pt1.y,
								arrIntersectPts[j].pt2.x,arrIntersectPts[j].pt2.y,NULL,NULL,NULL))
							{
								break;
							}
						}

						if (j >= intersectNum)
						{
							BOOL bAdd = TRUE;
							int tmpSize = tmpIntersectPts.GetSize();
							if (tmpSize > 0)
							{
								int start = tmpIntersectPts[tmpSize-1].lfRatio2;
								int end = arrIntersectPts[i].lfRatio2;

								CArray<PT_3DEX,PT_3DEX> pts2;
								pts2.Add(PT_3DEX(tmpIntersectPts[tmpSize-1].pt2,penLine));
								for (j=start+1; j<=end; j++)
								{
									pts2.Add(assistPts[j]);
								}
								pts2.Add(PT_3DEX(arrIntersectPts[i].pt2,penLine));

								double len0;
								len0 = GraphAPI::GGetAllLen3D(pts2.GetData(),pts2.GetSize());

								int numLine = arrIntersectPts[i].idx-tmpIntersectPts[tmpSize-1].idx;

								double dcycle0 = len0/numLine;
								if (dcycle0 < 0.5*dcycle || dcycle0 > 2*dcycle)
								{
									bAdd = FALSE;
								}
								
							}

							if (bAdd)
							{
								tmpIntersectPts.Add(arrIntersectPts[i]);
							}
							else if (tmpSize > 1)
							{
								tmpIntersectPts.RemoveAt(tmpSize-1);
							}
							
						}
					}

					arrIntersectPts.Copy(tmpIntersectPts);

					// 将空缺的线补齐，均匀过渡
					int intersectNum = arrIntersectPts.GetSize();
					for (i=0; i<intersectNum; i++)
					{
						int s0 = 0, s1 = 0, e0 = 0, e1 = 0;
						if (i == intersectNum-1)
						{
							if (arrIntersectPts[i].idx != num0-1)
							{ 
								s0 = arrIntersectPts[i].lfRatio1;
								e0 = npsum - 1;
								s1 = arrIntersectPts[i].lfRatio2;
								e1 = assistPts.GetSize() - 1;
							}
							else continue;
							
						}
						else if (arrIntersectPts[i+1].idx-arrIntersectPts[i].idx > 1)
						{
							s0 = arrIntersectPts[i].lfRatio1;
							e0 = arrIntersectPts[i+1].lfRatio1;
							s1 = arrIntersectPts[i].lfRatio2;
							e1 = arrIntersectPts[i+1].lfRatio2;
						}

						if (e0 - s0 > 0)
						{
							CArray<PT_3DEX,PT_3DEX> pts1, pts2;
							pts1.Add(dpt0[arrIntersectPts[i].idx]);
							for (int j=s0+1; j<=e0; j++)
							{
								pts1.Add(pts[j]);
							}
							if (i == intersectNum-1)
							{
								pts1.Add(pts[npsum-1]);
							}
							else
							{
								pts1.Add(dpt0[arrIntersectPts[i+1].idx]);
							}

							pts2.Add(PT_3DEX(arrIntersectPts[i].pt2,penLine));
							for (j=s1+1; j<=e1; j++)
							{
								pts2.Add(assistPts[j]);
							}
							if (i == intersectNum-1)
							{
								pts2.Add(assistPts[assistPts.GetSize()-1]);
							}
							else
							{
								pts2.Add(PT_3DEX(arrIntersectPts[i+1].pt2,penLine));
							}

							// 将母线和辅助线分成等距的线段，然后连接对应的点
							double len0, len1;
							len0 = GraphAPI::GGetAllLen3D(pts1.GetData(),pts1.GetSize());
							len1 = GraphAPI::GGetAllLen3D(pts2.GetData(),pts2.GetSize());
							
							int numLine = 0;
							if (i == intersectNum-1)
							{
								numLine = num0 - arrIntersectPts[i].idx;
							}
							else
							{
								numLine = arrIntersectPts[i+1].idx-arrIntersectPts[i].idx;
							}
							
							double dcycle0, dcycle1;							
							dcycle0 = len0/numLine-1e-6;
							dcycle1 = len1/numLine-1e-6;
							
							CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
							
							GraphAPI::GGetCycleLine(pts1.GetData(),pts1.GetSize(),dcycle0,0,dpt0,0);
							GraphAPI::GGetCycleLine(pts2.GetData(),pts2.GetSize(),dcycle1,0,dpt1,0);

							int numDpt0 = dpt0.GetSize();
							if( dpt1.GetSize()<numDpt0 )
								continue;

							if (i != intersectNum-1)
							{
								numDpt0--;
							}

							for (j=1; j<numDpt0; j++)
							{
								PtIntersect pt0;
								pt0.pt1 = dpt0[j];
								pt0.pt2 = dpt1[j];
								arrIntersectPts.Add(pt0);
							}

						}
					}*/

					int intersectNum = arrIntersectPts.GetSize();
					for (i=0; i<intersectNum; i++)
					{
						PtIntersect pt0 = arrIntersectPts[i];

						long coltmp = nColor;
						if (pt0.lfRatio1 >= 0)
						{
							//coltmp = RGB(0,255,0);
						}
						else
						{
							int aa = 10;
						}

						PT_3DEX line[2];

						PT_3DEX tmpline[2];
						tmpline[0] = PT_3DEX(pt0.pt1,penMove);
						tmpline[1] = PT_3DEX(pt0.pt2,penLine);
						
						GGetPtFromLineWidth(pts.GetData(), pts.GetSize(),tmpline[0],yoff0, &line[0]);
						
						PT_3DEX pt;
						double len = GraphAPI::GGet2DDisOf2P(tmpline[0],tmpline[1]);
						double tmpyoff = m_lfYOffset1*fDrawScale*fViewScale*(dScaleWidth>0?1:-1);
						double len1 = fabs(tmpyoff)+fabs(len*m_lfYOffsetType1);
						tmpyoff = len1 > len ? len : len1;
						GGetPtOfLine(tmpline,2,tmpyoff,&pt);
						line[1] = pt;

						DashLine(line,2,&grbuf,coltmp,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
					}
				}
				else
				{
					CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;

					GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dcycle,xoff0,dpt0,0);
					GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dcycle,xoff1,dpt1,0);
					
					int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
					for (int i=0; i<nmin; i++)
					{						
						PT_3DEX dp0, dp1;
						GGetPtFromLineWidth(pts.GetData(), pts.GetSize(),dpt0.GetAt(i),yoff0, &dp0);
						GGetPtFromLineWidth(pts.GetData(), pts.GetSize(),dpt1.GetAt(i),yoff1, &dp1);
						
						PT_3DEX tmppts[2];
						tmppts[0] = dp0;
						tmppts[1] = dp1;
						GraphAPI::GGetPtZOfLine(&dpt0[i],&dpt1[i],&tmppts[0]);
						GraphAPI::GGetPtZOfLine(&dpt0[i],&dpt1[i],&tmppts[1]);
						
						DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
					}					
				}				
			}
			
			pBuf->AddBuffer(&grbuf);	
			
		}
		else if (m_nPlaceType == 2)
		{
			GrBuffer grbuf;
			//grbuf.BeginLineString(pGeo->GetColor(),m_lfWidth*fDrawScale*fViewScale,TRUE,nIndex,fDrawScale*fViewScale);
			
			for (int i=0; i<npsum; i++)
			{
				PT_3DEX tmppts[2];
				tmppts[0] = tmppts[1] = pts.GetAt(i);

				tmppts[0].x += xoff0;
				tmppts[0].y += yoff0;
				tmppts[0].pencode = penMove;
				tmppts[1].x += xoff1;
				tmppts[1].y += yoff1;
				tmppts[1].pencode = penLine;
				LayAlongLine(pts.GetData(),pts.GetSize(),tmppts,2,&pts.GetAt(i));

				GraphAPI::GetHeightFromSurface(pts[0],pts[1],assistPts[0],tmppts,2);
				
				DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
			}
			
			pBuf->AddBuffer(&grbuf);
		}
		else
		{
			if( m_nPlaceType==3 || m_nPlaceType==5 )
			{
				GrBuffer grbuf;

				PT_3DEX tmppts[2];
				if (nGeoType == CLS_GEODCURVE)
				{
					// 直接连接母线和辅助线的首点
					if (1 || m_nPlaceType == 5)
					{
						tmppts[0] = pts[0];
						tmppts[1] = PT_3DEX(assistPts[0],penLine);
					}
					else 
					{
						// 辅助线首点
						if (fabs(m_lfYOffsetType0-1) < 1e-4)
						{
							tmppts[0] = tmppts[1] = assistPts[0];
							tmppts[1].x += xoff1;
							tmppts[1].y += (yoff1-dScaleWidth*m_lfYOffsetType0);
						
							LayAlongLine(assistPts.GetData(),assistPts.GetSize(),tmppts,2,&assistPts[0]);
						}
						else
						{
							tmppts[0] = tmppts[1] = pts[0];
							tmppts[0].x += xoff0;
							tmppts[0].y += yoff0;
							tmppts[1].x += xoff1;
							tmppts[1].y += yoff1;
							
							LayAlongLine(pts.GetData(),pts.GetSize(),tmppts,2,&pts[0]);
						}
					}
				}
				else
				{
					tmppts[0] = tmppts[1] = pts.GetAt(0);
					tmppts[0].x += xoff0;
					tmppts[0].y += yoff0;
					tmppts[1].x += xoff1;
					tmppts[1].y += yoff1;

					LayAlongLine(pts.GetData(),pts.GetSize(),tmppts,2,&pts.GetAt(0));
				}			

				tmppts[0].pencode = penMove;
				tmppts[1].pencode = penLine;
				
				GraphAPI::GetHeightFromSurface(pts[0],pts[1],assistPts[0],tmppts,2);

				DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
				
				pBuf->AddBuffer(&grbuf);

			}
			
			if( m_nPlaceType==4 || m_nPlaceType==5 )
			{
				GrBuffer grbuf;
			//	grbuf.BeginLineString(pGeo->GetColor(),m_lfWidth*fDrawScale*fViewScale,TRUE,nIndex,fDrawScale*fViewScale);

				PT_3DEX tmppts[2];
				if (nGeoType == CLS_GEODCURVE)
				{
					if (1 || m_nPlaceType == 5)
					{
						tmppts[0] = pts[npsum-1];
						tmppts[1] = assistPts[assistPts.GetSize()-1];
					}
					else 
					{
						if (fabs(m_lfYOffsetType1-1) < 1e-4)
						{
							PT_3DEX pt3d = assistPts[assistPts.GetSize()-1];
							tmppts[0] = tmppts[1] = pt3d;
							tmppts[1].x += xoff1;
							tmppts[1].y += (yoff1-dScaleWidth*m_lfYOffsetType1);
						
							LayAlongLine(assistPts.GetData(),assistPts.GetSize(),tmppts,2,&pt3d);
						}
						else
						{
							tmppts[0] = tmppts[1] = pts[npsum-1];
							tmppts[0].x += xoff0;
							tmppts[0].y += yoff0;
							tmppts[1].x += xoff1;
							tmppts[1].y += yoff1;
							
							LayAlongLine(pts.GetData(),pts.GetSize(),tmppts,2,&pts[npsum-1]);

						}
					}
				}
				else
				{
					tmppts[0] = tmppts[1] = pts.GetAt(npsum-1);
					tmppts[0].x += xoff0;
					tmppts[0].y += yoff0;
					tmppts[1].x += xoff1;
					tmppts[1].y += yoff1;

					LayAlongLine(pts.GetData(),pts.GetSize(),tmppts,2,&pts.GetAt(npsum-1));
				}
				
				tmppts[0].pencode = penMove;
				tmppts[1].pencode = penLine;

				GraphAPI::GetHeightFromSurface(pts[0],pts[1],assistPts[0],tmppts,2);

				DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);

			//	grbuf.MoveTo(&tmppts[0]);
 			//	grbuf.LineTo(&tmppts[1]);
								
			//	grbuf.End();
				
				pBuf->AddBuffer(&grbuf);
			}

			if( m_nPlaceType==6 )
			{
				GrBuffer grbuf;
				
				PT_3DEX tmppts[2];
				GraphAPI::GGetMiddlePt(pts.GetData(),pts.GetSize(),&tmppts[0]);
				GraphAPI::GGetMiddlePt(assistPts.GetData(),assistPts.GetSize(),&tmppts[1]);			
				
				GraphAPI::GetHeightFromSurface(pts[0],pts[1],assistPts[0],tmppts,2);
				
				DashLine(tmppts,2,&grbuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
				
				pBuf->AddBuffer(&grbuf);
				
			}
		}		
	}

}



BOOL CScaleLinetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,TRUE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



BOOL CScaleLinetype::ExplodeSimple(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CScaleLinetype::IsSupportGeoType(int nGeoType)
{
	if (nGeoType == CLS_GEOPARALLEL || nGeoType == CLS_GEODCURVE)
		return TRUE;
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineHatch::CLineHatch()
{
	m_nType = SYMTYPE_LINEHATCH;

	m_fAngle = m_fIntv = m_fWidth = 0;	
}

CLineHatch::~CLineHatch()
{
	
}

void CLineHatch::CopyFrom(CSymbol *pSymbol)
{
	CLineHatch *pLineHatch = (CLineHatch*)pSymbol;
	m_fAngle = pLineHatch->m_fAngle;
	m_fIntv = pLineHatch->m_fIntv;
	m_fWidth = pLineHatch->m_fWidth;
	m_strBaseLinetypeName = pLineHatch->m_strBaseLinetypeName;
}

BOOL CLineHatch::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_ANGLE) )
			{
				data = xmlFile.GetData();
				m_fAngle = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_INTV) )
			{
				data = xmlFile.GetData();
				m_fIntv = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CLineHatch::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			data.Format(_T("%f"),m_fAngle);
			xmlFile.AddElem(XMLTAG_ANGLE,data);

			data.Format(_T("%f"), m_fIntv);
			xmlFile.AddElem(XMLTAG_INTV,data);

			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	



void CLineHatch::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;

	CGeometry *pGeo = pFt->GetGeometry();
	
	int nGeoType = pGeo->GetClassType();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid, ftrIntervalScale = 1.0f, ftrAngle = 0;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		ftrWid = ((CGeoSurface*)pGeo)->m_fLineWidth;
		ftrIntervalScale = ((CGeoSurface*)pGeo)->m_fIntvScale;
		ftrAngle = ((CGeoSurface*)pGeo)->m_fCellAngle;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
	case CLS_GEOPARALLEL:
		{
			CArray<PT_3DEX,PT_3DEX> arr;

			if (nGeoType == CLS_GEODCURVE)
			{
				((CGeoDCurve*)pGeo)->GetOrderShape(arr);
			}
			else			
				pGeo->GetShape(arr);

			if (nGeoType == CLS_GEOPARALLEL)
			{
				CArray<PT_3DEX,PT_3DEX> ptspar;
				((CGeoParallel*)pGeo)->GetParallelShape(ptspar);
				
				for (int i=ptspar.GetSize()-1; i>=0; i--)
				{
					arr.Add(ptspar.GetAt(i));
				}				
			}
			
			int polysnum = arr.GetSize();
			
			if (polysnum < 3 || fabs(m_fIntv) < 1e-4)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}
			
			CArray<PT_3D,PT_3D> polys;
			polys.SetSize(polysnum);
			for(int j=0; j<polysnum; j++)
			{
				COPY_3DPT(polys[j],arr[j]);
			}

			GrBuffer buf;
			pFt->Draw(&buf,fDrawScale);
			Envelope evlp = buf.GetEnvelope();

			double interwid = evlp.Width()/10;
			//if( wid<0.1 )wid = 1;

			// 求解平面方程
			double A, B, C;
			BOOL bSolved = SolvePlaneEquation2(polys.GetData(),polysnum,&A,&B,&C);
			
			CGrTrim trim;
			trim.InitTrimPolygon(polys.GetData(),polysnum,interwid);
			trim.m_bIntersectHeight = TRUE;
			
			buf.DeleteAll();

			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}

			int  nStyle = 0;
			float styles[8] = {0};
			
			if( nIndex>0 && nIndex<=baselineLib->GetBaseLineTypeCount() )
			{
				BaseLineType linetype = baselineLib->GetBaseLineType(nIndex-1);		
				
				nStyle = linetype.m_nNum;
				for( int i=0; i<linetype.m_nNum; i++)
				{
					styles[i] = fabs(linetype.m_fLens[i])*fDrawScale*ftrLinetypeScale;
				}				
			}

			DWORD nColor = pGeo->GetColor();
			//buf.BeginLineString(pGeo->GetColor(),m_fWidth,TRUE,nIndex,fDrawScale*fViewScale);

			float fAngle = m_fAngle + ftrAngle;

			while (fAngle < 0)
			{
				fAngle += PI;
			}
			
			while (fAngle > PI)
			{
				fAngle -= PI;
			}

			PT_3DEX pts[2];

			double fIntv = m_fIntv*fDrawScale*fViewScale*ftrIntervalScale;
			// 横
			if ( fabs(fAngle) < GraphAPI::GetDisTolerance() || fabs(fAngle-PI) < GraphAPI::GetDisTolerance())
			{
				PT_3D pt0(evlp.m_xl,0,0) , pt1(evlp.m_xh,0,0);
				double y = evlp.m_yl + fIntv;
				for ( ; evlp.m_yh-y>1e-4; y+=fIntv)
				{
					pt0.y = pt1.y = y;
					pts[0] = PT_3DEX(pt0,penMove);
					pts[1] = PT_3DEX(pt1,penLine);
					pts[0].z = pts[0].x*A + pts[0].y*B + C;
					pts[1].z = pts[1].x*A + pts[1].y*B + C;
					DashLine(pts,2,&buf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,FALSE);
				}
			}
			// 竖
			else if ( fabs(fAngle-PI/2) < GraphAPI::GetDisTolerance() )
			{
				PT_3D pt0(0,evlp.m_yl,0) , pt1(0,evlp.m_yh,0);
				double x = evlp.m_xl + fIntv;
				for ( ; evlp.m_xh-x>1e-4; x+=fIntv)
				{
					pt0.x = pt1.x = x;

					pts[0] = PT_3DEX(pt0,penMove);
					pts[1] = PT_3DEX(pt1,penLine);
					pts[0].z = pts[0].x*A + pts[0].y*B + C;
					pts[1].z = pts[1].x*A + pts[1].y*B + C;
					DashLine(pts,2,&buf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,FALSE);
				}
			}
			// 左斜
			else if (fAngle > 0 && fAngle < PI/2)
			{
				// 先计算要填满矩形区域第一条线和最后条线的间距，
				// y向下移动的最大距离
				double fInterMax = evlp.Width()*tan(fAngle) + evlp.Height();
				
				// 每次y的移动距离
				double fSideInter = fIntv/cos(fAngle);
				
				// 移动次数
				int num = fInterMax/fSideInter;
				
				PT_3D pt0, pt1;
				pt0.x = evlp.m_xl;
				pt1.x = evlp.m_xh;

				// 起始点y的坐标
				double y0 = evlp.m_yh - fSideInter;
				double y1 = evlp.m_yh + (evlp.Width()*tan(fAngle)-fSideInter);
				
				for (int j=0; j<num; j++)
				{
					pt0.y = y0 - fSideInter*j;
					pt1.y = y1 - fSideInter*j; 
					
					pts[0] = PT_3DEX(pt0,penMove);
					pts[1] = PT_3DEX(pt1,penLine);
					pts[0].z = pts[0].x*A + pts[0].y*B + C;
					pts[1].z = pts[1].x*A + pts[1].y*B + C;
					DashLine(pts,2,&buf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,FALSE);
				}				
			}
			// 右斜
			else if (fAngle > PI/2 && fAngle < PI)
			{
				// 先计算要填满矩形区域第一条线和最后条线的间距，
				// y向下移动的最大距离
				double fInterMax = evlp.Width()*tan(PI-fAngle) + evlp.Height();
				
				// 每次y的移动距离
				double fSideInter = fIntv/(-cos(fAngle));
				
				// 移动次数
				int num = fInterMax/fSideInter;
				
				PT_3D pt0, pt1;
				pt0.x = evlp.m_xl;
				pt1.x = evlp.m_xh;

				// 起始点y的坐标
				double y0 = evlp.m_yl + fSideInter;
				double y1 = evlp.m_yl - (evlp.Width()*tan(PI-fAngle)-fSideInter);
				
				for (int j=0; j<num; j++)
				{
					pt0.y = y0 + fSideInter*j;
					pt1.y = y1 + fSideInter*j; 
						
					pts[0] = PT_3DEX(pt0,penMove);
					pts[1] = PT_3DEX(pt1,penLine);
					pts[0].z = pts[0].x*A + pts[0].y*B + C;
					pts[1].z = pts[1].x*A + pts[1].y*B + C;
					DashLine(pts,2,&buf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,FALSE);				
				}
			}

			//buf.End();

			trim.Trim(&buf,*pBuf);

			//去除单点（对多面对象，不去除的话显示有点问题）
			if( nGeoType==CLS_GEOMULTISURFACE )
			{
				Graph* pGr = pBuf->HeadGraph();
				CArray<Graph*,Graph*> grs;
				while( pGr )
				{
					if( IsGrLineString(pGr) )
					{
						GrLineString *pLS = (GrLineString*)pGr;
						if( pLS->ptlist.nuse==2 )
						{
							if( GraphAPI::GGet2DDisOf2P(pLS->ptlist.pts[0],pLS->ptlist.pts[1])<1e-10 )
							{
								grs.Add(pGr);
							}
						}					
					}
					pGr = pGr->next;
				}
				
				for( int m=0; m<grs.GetSize(); m++)
				{
					pBuf->DeleteGraph(grs[m]);
				}
			}

			break;			
		}
		
	default:
		{
			break;
		}
	}
	
}




BOOL CLineHatch::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CLineHatch::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT || nGeoType == CLS_GEODIRPOINT)
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDiagonal::CDiagonal()
{
	m_nType = SYMTYPE_DIAGONAL;
	m_nDiagonalType = 0;
	m_fWidth = 0;	
}

CDiagonal::~CDiagonal()
{
	
}

void CDiagonal::CopyFrom(CSymbol *pSymbol)
{
	CDiagonal *pDiagonal = (CDiagonal*)pSymbol;
	m_fWidth = pDiagonal->m_fWidth;
	m_nDiagonalType = pDiagonal->m_nDiagonalType;
	m_strBaseLinetypeName = pDiagonal->m_strBaseLinetypeName;
}

BOOL CDiagonal::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if ( xmlFile.FindElem(XMLTAG_DIAGONALTYPE))
			{
				data = xmlFile.GetData();
				m_nDiagonalType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CDiagonal::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);

			data.Format(_T("%d"),m_nDiagonalType);
			xmlFile.AddElem(XMLTAG_DIAGONALTYPE,data);

			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	



void CDiagonal::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;

	CGeometry *pGeo = pFt->GetGeometry();
	
	int nGeoType = pGeo->GetClassType();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		ftrWid = ((CGeoSurface*)pGeo)->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
	//case CLS_GEOTEXT:
	case CLS_GEOPARALLEL:
		{

			CArray<PT_3DEX,PT_3DEX> basepts;
			if (nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEOPARALLEL)
			{
				const CShapeLine *pShape = ((CGeoCurve*)pGeo)->GetShape();
				if (pShape)
				{
					pShape->GetPts(basepts);
				}
			}
			else if (nGeoType == CLS_GEODCURVE)
			{
				((CGeoDCurve*)pGeo)->GetOrderShape(basepts);
			}
			else if (nGeoType == CLS_GEOSURFACE || nGeoType == CLS_GEOMULTISURFACE)
			{
				const CShapeLine *pShape = ((CGeoSurface*)pGeo)->GetShape();
				if (pShape)
				{
					pShape->GetPts(basepts);
				}
			}
			else
			{
				pGeo->GetShape(basepts);
			}

			if (nGeoType == CLS_GEOPARALLEL)
			{
				CGeoParallel *pParallel = (CGeoParallel*)pGeo;
				CArray<PT_3DEX,PT_3DEX> ptspar;
				pParallel->GetParallelShape(ptspar);

				for (int i=ptspar.GetSize()-1; i>=0; i--)
				{
					basepts.Add(ptspar.GetAt(i));
				}
				
			}

			int sum = GraphAPI::GKickoffSamePoints(basepts.GetData(),basepts.GetSize());
		
			if(sum < 3) return;

			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}

			int  nStyle = 0;
			float styles[8] = {0};
			
			if( nIndex>0 && nIndex<=baselineLib->GetBaseLineTypeCount() )
			{
				BaseLineType linetype = baselineLib->GetBaseLineType(nIndex-1);		
				
				nStyle = linetype.m_nNum;
				for( int i=0; i<linetype.m_nNum; i++)
				{
					styles[i] = fabs(linetype.m_fLens[i])*fDrawScale*ftrLinetypeScale;
				}
				
			}

			if (GraphAPI::GGet3DDisOf2P(basepts[0],basepts[sum-1]) < 1e-4)
			{
				--sum;
			}

			DWORD nColor = pGeo->GetColor();

			PT_3DEX ptex;
			for (int i=0; i<sum; i++)
			{
				CArray<PT_3DEX,PT_3DEX> pts;
				ptex = basepts[i];
				ptex.pencode = penMove;
				pts.Add(ptex);
				for (int j=i+2; j<sum; j+=2)
				{
					ptex = basepts[j];
					ptex.pencode = penLine;
					pts.Add(ptex);
				}
				DashLine(pts.GetData(),pts.GetSize(),pBuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype);

			}	
			
			// 入地口紧靠杆位指向第一条基线，与第二条线平行，比第二条线稍短（暂取4/5长度）
			if (m_nDiagonalType == 1)
			{
				BOOL bClock = GraphAPI::GIsClockwise(basepts.GetData(),basepts.GetSize());
				// 箭头到第二条线距离
				double dis = 0.8/1.414*fDrawScale*fViewScale;
				if (!bClock)
					dis = -dis;

				PT_3DEX retLine[2], retLine0[2];				
				GraphAPI::GGetParallelLine(basepts.GetData()+1,2,dis,retLine0);

				retLine[0].x = retLine0[0].x + (retLine0[1].x - retLine0[0].x) / 10;
				retLine[0].y = retLine0[0].y + (retLine0[1].y - retLine0[0].y) / 10;
				retLine[0].z = retLine0[0].z + (retLine0[1].z - retLine0[0].z) / 10;

				retLine[1].x = retLine0[1].x + (retLine0[0].x - retLine0[1].x) / 10;
				retLine[1].y = retLine0[1].y + (retLine0[0].y - retLine0[1].y) / 10;
				retLine[1].z = retLine0[1].z + (retLine0[0].z - retLine0[1].z) / 10;

				DashLine(retLine,2,pBuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype);
				// 箭头上的两个点
				PT_3D retPt1, retPt2;
				retPt1 = GetRotatePt(retLine[0],retLine[1],retLine[0],0.6*fDrawScale*fViewScale,PI/4);
				retPt2 = GetRotatePt(retLine[0],retLine[1],retLine[0],0.6*fDrawScale*fViewScale,-PI/4);

				retLine[1] = PT_3DEX(retPt1,penLine);
				DashLine(retLine,2,pBuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype);

				retLine[1] = PT_3DEX(retPt2,penLine);
				DashLine(retLine,2,pBuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype);

			}
		
			break;
			
		}
		
	default:
		{
			break;
		}
	}
	
}





BOOL CDiagonal::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CDiagonal::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT || nGeoType == CLS_GEODIRPOINT)
		return FALSE;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CParaLinetype::CParaLinetype()
{
	m_nType = SYMTYPE_PARATYPE;

	m_fWidth = 0;
	m_fIntv = 1;	
}

CParaLinetype::~CParaLinetype()
{
	
}

void CParaLinetype::CopyFrom(CSymbol *pSymbol)
{
	CParaLinetype *pPara = (CParaLinetype*)pSymbol;
	m_fWidth = pPara->m_fWidth;
	m_fIntv = pPara->m_fIntv;
	m_strBaseLinetypeName = pPara->m_strBaseLinetypeName;
}

BOOL CParaLinetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_INTV) )
			{
				data = xmlFile.GetData();
				m_fIntv = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CParaLinetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);

			data.Format(_T("%f"),m_fIntv);
			xmlFile.AddElem(XMLTAG_INTV,data);

			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();
}	



void CParaLinetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOPARALLEL:
		{

			CArray<PT_3DEX,PT_3DEX> basepts;
			
			CGeoParallel *pParallel = (CGeoParallel*)pGeo;			
			pParallel->GetShape(basepts);

			double wid = pParallel->GetWidth();

			int sum = basepts.GetSize();
		
			if(sum < 2) return;

			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0 || fabs(wid)<1e-4 || fabs(m_fIntv)<1e-4)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}

			double intv = m_fIntv*wid/fabs(wid)*fDrawScale*fViewScale;
			double intvs = 0;

			double fDHeight = pParallel->GetDHeight();

			CArray<PT_3DEX,PT_3DEX> ptspar;
			ptspar.SetSize(basepts.GetSize());			

			while (fabs(intvs) < fabs(wid+intv))
			{	
				//保证最后一条台阶也画上
				float offset = intvs;
				if( fabs(offset)>fabs(wid) )
					offset = wid;

				pBuf->BeginLineString(pGeo->GetColor(),m_fWidth*fDrawScale*fViewScale,TRUE,nIndex,fDrawScale*fViewScale);

				GraphAPI::GGetParallelLine(basepts.GetData(),basepts.GetSize(),offset,ptspar.GetData());
			
				double fkz = fabs(offset/wid);
				for (int i=0; i<ptspar.GetSize(); i++)
				{
					PT_3DEX pt = ptspar[i];
					pt.z += fDHeight*fkz;
					if (i == 0)
					{
						pBuf->MoveTo(&pt);
					}
					else
					{
						pBuf->LineTo(&pt);
					}
					
				}

				intvs += intv;

				pBuf->End();
			}

			//pFt->Draw(pBuf);

			break;
			
		}
		
	default:
		{
			break;
		}
	}
	
}

BOOL CParaLinetype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPARALLEL)
		return TRUE;
	return FALSE;
}



BOOL CParaLinetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAngBisectortype::CAngBisectortype()
{
	m_nType = SYMTYPE_ANGBISECTORTYPE;
	
	m_fWidth = 0;
	m_fLength = 1;	
}

CAngBisectortype::~CAngBisectortype()
{
	
}

void CAngBisectortype::CopyFrom(CSymbol *pSymbol)
{
	CAngBisectortype *pAng = (CAngBisectortype*)pSymbol;
	m_fWidth = pAng->m_fWidth;
	m_fLength = pAng->m_fLength;
	m_strBaseLinetypeName = pAng->m_strBaseLinetypeName;
}

BOOL CAngBisectortype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_ANGLENTGH) )
			{
				data = xmlFile.GetData();
				m_fLength = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CAngBisectortype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			data.Format(_T("%f"),m_fLength);
			xmlFile.AddElem(XMLTAG_ANGLENTGH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}


void CAngBisectortype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();
		
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)))
	{
		CGeoCurveBase *pCurve = (CGeoCurveBase*)pGeo;
		ftrWid = pCurve->m_fLineWidth;		
	}

	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;		
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOPARALLEL:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
		{
			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				break;
			}
			
			CArray<PT_3DEX,PT_3DEX> pts;
		    if( nGeoType == CLS_GEODCURVE)
			{
				((CGeoDCurve*)pGeo)->GetOrderShape(pts);
			}
			else if(nGeoType == CLS_GEOPARALLEL)
			{
				pGeo->GetShape(pts);

				CArray<PT_3DEX,PT_3DEX> arr;
				((CGeoParallel*)pGeo)->GetParallelShape(arr);

				for (int i=arr.GetSize()-1; i>=0; i--)
				{
					pts.Add(arr[i]);
				}
				
			}
			else
			{
				pGeo->GetShape(pts);
			}

			/*int ptsum = GraphAPI::GKickoffSamePoints(pts.GetData(),pts.GetSize());
			pts.SetSize(ptsum);*/

			int ptsum = pts.GetSize();
			
			if (ptsum < 3)
				break;
			
			BOOL bClosed = FALSE;
			if (ptsum >=3 )
			{
				PT_3DEX pt0,pt1;
				
				pt0 = pts[0]; pt1 = pts[ptsum-1];
				if( fabs(pt0.x-pt1.x)<GraphAPI::GetDisTolerance() && 
					fabs(pt0.y-pt1.y)<GraphAPI::GetDisTolerance() )
					bClosed = TRUE;
			}

			float len = m_fLength*fDrawScale*fViewScale;
			PT_3DEX ret;
			CArray<PT_3DEX,PT_3DEX> arr;

			pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);

			if( bClosed )
			{
				arr.Add(pts[ptsum-2]);
				arr.Add(pts[0]);
				arr.Add(pts[1]);
				
				GetAngBisectorPt(arr.GetData(),len,&ret,1);
				pBuf->MoveTo(&pts[0]);
				pBuf->LineTo(&ret);
			}

			// 中间点
			for (int i=1; i<ptsum-1; i++)
			{
				arr.RemoveAll();
				arr.Add(pts[i-1]);
				arr.Add(pts[i]);
				arr.Add(pts[i+1]);
				
				GetAngBisectorPt(arr.GetData(),len,&ret,1);
				pBuf->MoveTo(&pts[i]);
				pBuf->LineTo(&ret);
			}

			pBuf->End();

			break;
			
		}
		
	default:
		{
			break;
		}
	}

}




BOOL CAngBisectortype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}


BOOL CAngBisectortype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPOINT || nGeoType == CLS_GEODIRPOINT)
		return FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleArctype::CScaleArctype()
{
	m_nType = SYMTYPE_SCALEARCTYPE;
	
	m_fWidth = 0;
	m_fArcLength = 1;	
	m_nPlaceType = 2;
}

CScaleArctype::~CScaleArctype()
{
	
}

void CScaleArctype::CopyFrom(CSymbol *pSymbol)
{
	CScaleArctype *pArc = (CScaleArctype*)pSymbol;
	m_fWidth = pArc->m_fWidth;
	m_fArcLength = pArc->m_fArcLength;
	m_nPlaceType = pArc->m_nPlaceType;
	m_strBaseLinetypeName = pArc->m_strBaseLinetypeName;
}

BOOL CScaleArctype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_PLACETYPE) )
			{
				data = xmlFile.GetData();
				m_nPlaceType = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_ARCLENTGH) )
			{
				data = xmlFile.GetData();
				m_fArcLength = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleArctype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			data.Format(_T("%i"),m_nPlaceType);
			xmlFile.AddElem(XMLTAG_PLACETYPE,data);
			
			data.Format(_T("%f"),m_fArcLength);
			xmlFile.AddElem(XMLTAG_ARCLENTGH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

template<class T>
void static GetArcPts(const T &cen, double r, double startAng, double endAng, CArray<T,T> &ret)
{
	float delta_theta = (endAng-startAng)/10;
	
	for( float angle = startAng; angle < endAng+1e-4; angle += delta_theta )
	{
		T tmp;
		tmp.x = cen.x + r*cos(angle);
		tmp.y = cen.y + r*sin(angle);
		tmp.z = cen.z;
		ret.Add(tmp);
	}
}

// 获取等边三角形的第三点
template<class T>
T GetThirdPt(T pt0, T pt1, bool dir)
{
	double dx, dy, h, len;
	dx = pt1.x - pt0.x;
	dy = pt1.y - pt0.y;
	len = sqrt((pt1.y-pt0.y)*(pt1.y-pt0.y)+(pt1.x-pt0.x)*(pt1.x-pt0.x));
	h = sqrt(3)*len/2;

	T cpt;
	cpt.x = (pt0.x+pt1.x)/2;
	cpt.y = (pt0.y+pt1.y)/2;

	double tdx, tdy;
	tdx = -h*dy/len;
	tdy = h*dx/len;
	
	T pt;

	if (dir)
	{
		pt.x = cpt.x-tdx;
		pt.y = cpt.y-tdy;
		//		point.z = cpt.z;
	}
	else
	{
		pt.x = cpt.x+tdx;
		pt.y = cpt.y+tdy;
		//		point.z = cpt.z;
	}

	return pt;
}

void CScaleArctype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		ftrWid = ((CGeoSurface*)pGeo)->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOPARALLEL:
		{
			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				break;
			}

			CGeoParallel *pPar = ((CGeoParallel*)pGeo);
			if (!pPar) break;
			
			CArray<PT_3DEX,PT_3DEX> pts, arr;
			pGeo->GetShape(pts);
			pPar->GetParallelShape(arr);

			int ptsum = GraphAPI::GKickoffSamePoints(pts.GetData(),pts.GetSize());
			pts.SetSize(ptsum);
			
			if (ptsum < 2)
				break;

			// 露出弧长
			float len = m_fArcLength*fDrawScale*fViewScale;
			// 半径
			float r = fabs(pPar->GetWidth());

			float baseAng = GraphAPI::GGetAngle(pts[0].x,pts[0].y,pts[1].x,pts[1].y);
			// 露出弧长对应的角度
			float arcAng = len/r;

			CArray<PT_3DEX,PT_3DEX> ret;
			if (m_nPlaceType == 0 || m_nPlaceType == 2)
			{
				PT_3DEX cen = GetThirdPt(pts[0],arr[0],true);			
						
				GetArcPts(cen,r,baseAng-PI/6-arcAng,baseAng+PI/6+arcAng,ret);
				pBuf->BeginLineString(pGeo->GetColor(),wid,TRUE,nIndex,fDrawScale*fViewScale);

				GraphAPI::GetHeightFromSurface(pts[0],pts[1],arr[0],ret.GetData(),ret.GetSize());
				
				for (int i=0; i<ret.GetSize(); i++)
				{
					if (i == 0)
					{
						pBuf->MoveTo(&ret[0]);
					}
					else
					{
						pBuf->LineTo(&ret[i]);
					}
				}				
				
				pBuf->End();

			}

			ret.RemoveAll();
			if (m_nPlaceType == 1 || m_nPlaceType == 2)
			{
				PT_3DEX cen = GetThirdPt(pts[ptsum-1],arr[ptsum-1],false);			
				
				GetArcPts(cen,r,baseAng+PI-PI/6-arcAng,baseAng+PI+PI/6+arcAng,ret);
				pBuf->BeginLineString(pGeo->GetColor(),wid,TRUE,nIndex,fDrawScale*fViewScale);
				
				GraphAPI::GetHeightFromSurface(pts[0],pts[1],arr[0],ret.GetData(),ret.GetSize());

				for (int i=0; i<ret.GetSize(); i++)
				{
					if (i == 0)
					{
						pBuf->MoveTo(&ret[0]);
					}
					else
					{
						pBuf->LineTo(&ret[i]);
					}
				}				
				
				pBuf->End();
				
			}

			break;
			
		}
		
	default:
		{
			break;
		}
	}

}





BOOL CScaleArctype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CScaleArctype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPARALLEL)
		return TRUE;
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleTurnplatetype::CScaleTurnplatetype()
{
	m_nType = SYMTYPE_SCALETURNPLATETYPE;
	
	m_fWidth = 0.4;
	m_fRoadWidth = 1.435;
	m_bSingleLine = FALSE;
	m_fStickupLen = 0.6;
	m_fSideLineWidth = 0.2;
}

CScaleTurnplatetype::~CScaleTurnplatetype()
{
	
}

void CScaleTurnplatetype::CopyFrom(CSymbol *pSymbol)
{
	CScaleTurnplatetype *pArc = (CScaleTurnplatetype*)pSymbol;
	m_fWidth = pArc->m_fWidth;
	m_fRoadWidth = pArc->m_fRoadWidth;
	m_strBaseLinetypeName = pArc->m_strBaseLinetypeName;
	m_bSingleLine = pArc->m_bSingleLine;
	m_fStickupLen = pArc->m_fStickupLen;
}

BOOL CScaleTurnplatetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_ROADWIDTH) )
			{
				data = xmlFile.GetData();
				m_fRoadWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			if( xmlFile.FindElem(XMLTAG_SINGLELINE) )
			{
				data = xmlFile.GetData();
				m_bSingleLine = _ttoi(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_STICKUPLEN) )
			{
				data = xmlFile.GetData();
				m_fStickupLen = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_STICKUPLEN) )
			{
				data = xmlFile.GetData();
				m_fSideLineWidth = _ttof(data);
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleTurnplatetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			data.Format(_T("%f"),m_fRoadWidth);
			xmlFile.AddElem(XMLTAG_ROADWIDTH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);

			data.Format(_T("%d"),(int)m_bSingleLine);
			xmlFile.AddElem(XMLTAG_SINGLELINE,data);

			data.Format(_T("%f"),m_fStickupLen);
			xmlFile.AddElem(XMLTAG_STICKUPLEN,data);

			data.Format(_T("%f"),m_fSideLineWidth);
			xmlFile.AddElem(XMLTAG_SIDELINEWIDTH,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

void CScaleTurnplatetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)))
	{
		ftrWid = ((CGeoSurface*)pGeo)->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:0;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEOSURFACE:
		{
			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				break;
			}

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);

			int size = pts.GetSize();

			if (size < 3)
			{
				break;
			}

			if (!((pts[0].pencode == pen3PArc && pts[1].pencode == pen3PArc && pts[2].pencode == pen3PArc) || (pts[0].pencode == penArc && pts[1].pencode == penArc && pts[2].pencode == penArc)))
			{
				break;
			}

			// 圆心，半径
			double r;
			PT_3DEX cen, cen2;
			if (!CalcArcParamFrom3P(pts.GetData(),&cen,&r,NULL,NULL)) break;
			pGeo->GetCenter(NULL,&cen2);

			cen.z = cen2.z;

			// 上下平移宽度
			double height = fabs(m_fRoadWidth)*fDrawScale*fViewScale/2;
			double fwid2 = fabs(m_fWidth)*fDrawScale*fViewScale;

			PT_3DEX firPt = pts[0];
			// 圆心角
			double a = asin(height/r), baseAng = GraphAPI::GGetAngle(firPt.x,firPt.y,cen.x,cen.y);
			PT_3DEX line[4];
			line[0].x = cen.x + r*cos(baseAng+a);
			line[0].y = cen.y + r*sin(baseAng+a);
			line[0].z = cen.z;
			line[1].x = cen.x + r*cos(baseAng+PI-a);
			line[1].y = cen.y + r*sin(baseAng+PI-a);
			line[1].z = cen.z;
			line[2].x = cen.x + r*cos(baseAng-a);
			line[2].y = cen.y + r*sin(baseAng-a);
			line[2].z = cen.z;
			line[3].x = cen.x + r*cos(baseAng+PI+a);
			line[3].y = cen.y + r*sin(baseAng+PI+a);
			line[3].z = cen.z;

			GraphAPI::GetHeightFromSurface(pts[0],pts[1],pts[2],line,4);

			float fSideLineWidth = 0;			
			if( !m_bSingleLine )
				fSideLineWidth = m_fSideLineWidth*fDrawScale*fViewScale;

			pBuf->BeginLineString(pGeo->GetColor(),fSideLineWidth,TRUE,nIndex,fDrawScale*fViewScale);
			pBuf->MoveTo(&line[0]);
			pBuf->LineTo(&line[1]);
			pBuf->End();

			pBuf->BeginLineString(pGeo->GetColor(),fSideLineWidth,TRUE,nIndex,fDrawScale*fViewScale);
			pBuf->MoveTo(&line[2]);
			pBuf->LineTo(&line[3]);
			pBuf->End();

			double fStickup = fabs(m_fStickupLen)*fDrawScale*fViewScale;

			if( !m_bSingleLine )
			{
				// 在1/4和3/4处绘制两个枕木
				PT_3D line0[2], line1[2];
				PT_3D pt;
				
				pt.x = line[0].x + (line[1].x - line[0].x)/4;
				pt.y = line[0].y + (line[1].y - line[0].y)/4;
				pt.z = line[0].z + (line[1].z - line[0].z)/4;
				line0[0] = GetRotate90Pt(line[0],line[1],pt,-fStickup);
				pt.x = line[0].x + (line[1].x - line[0].x)*3/4;
				pt.y = line[0].y + (line[1].y - line[0].y)*3/4;
				pt.z = line[0].z + (line[1].z - line[0].z)*3/4;
				line0[1] = GetRotate90Pt(line[0],line[1],pt,-fStickup);
				
				pt.x = line[2].x + (line[3].x - line[2].x)/4;
				pt.y = line[2].y + (line[3].y - line[2].y)/4;
				pt.z = line[2].z + (line[3].z - line[2].z)/4;
				line1[0] = GetRotate90Pt(line[2],line[3],pt,fStickup);
				pt.x = line[2].x + (line[3].x - line[2].x)*3/4;
				pt.y = line[2].y + (line[3].y - line[2].y)*3/4;
				pt.z = line[2].z + (line[3].z - line[2].z)*3/4;
				line1[1] = GetRotate90Pt(line[2],line[3],pt,fStickup);
				
				pBuf->BeginLineString(pGeo->GetColor(),fwid2,TRUE,nIndex,fDrawScale*fViewScale);
				pBuf->MoveTo(&line0[0]);
				pBuf->LineTo(&line1[0]);
				pBuf->End();
				
				pBuf->BeginLineString(pGeo->GetColor(),fwid2,TRUE,nIndex,fDrawScale*fViewScale);
				pBuf->MoveTo(&line0[1]);
				pBuf->LineTo(&line1[1]);
				pBuf->End();
			}
			else
			{
				line[0].x = cen.x + r*cos(baseAng);
				line[0].y = cen.y + r*sin(baseAng);
				line[1].x = cen.x;
				line[1].y = cen.y;
				
				GraphAPI::GetHeightFromSurface(pts[0],pts[1],pts[2],line,2);

				//绘制一段有宽度的线
				pBuf->BeginLineString(pGeo->GetColor(),height*2,TRUE,0,fDrawScale*fViewScale);
				pBuf->MoveTo(&line[0]);
				pBuf->LineTo(&line[1]);
				pBuf->End();
			}

			break;
			
		}
		
	default:
		{
			pFt->Draw(pBuf,fDrawScale);
			break;
		}
	}

}

BOOL CScaleTurnplatetype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOCURVE)
		return TRUE;
	return FALSE;
}




BOOL CScaleTurnplatetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleCranetype::CScaleCranetype()
{
	m_nType = SYMTYPE_SCALECRANETYPE;
	
	m_fWidth = 0;
}

CScaleCranetype::~CScaleCranetype()
{
	
}

void CScaleCranetype::CopyFrom(CSymbol *pSymbol)
{
	CScaleCranetype *pScale = (CScaleCranetype*)pSymbol;
	m_fWidth = pScale->m_fWidth;
	m_strBaseLinetypeName = pScale->m_strBaseLinetypeName;
}

BOOL CScaleCranetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleCranetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

// 获取pts上距离首点dLen的点
template<class T>
static bool  GetLengthPt(const T *pts, int nSum, const double *pDis, int ptnum, T *dpts, float *pos)
{
	
	if(pts==NULL || pDis == NULL || dpts == NULL || nSum<1 || ptnum < 1)
		return false;
	
	//计算每一线段的长度,存储到pLen中
	float *pLen = new float[nSum];
	
	float  *p = pLen;
	double xold, yold, zold;
	
	const T *pt;
	pt = pts;
	int nFlag = nSum;
	while( nFlag-- )
	{
		// 复杂面
		if (p==pLen || pt->pencode==penMove) *p = 0.0;
		else
		{
			*(p) = *(p-1) + sqrt( (pt->x-xold)*(pt->x-xold) +
				(pt->y-yold)*(pt->y-yold) +
				(pt->z-zold)*(pt->z-zold) );
		}
		xold = pt->x;
		yold = pt->y;
		zold = pt->z;
		
		p++;
		pt++;
	}

	for (int i=0; i<ptnum; i++)
	{
		double len = pDis[i];
		if (len < 0 || len > pLen[nSum-1])
		{
			continue;
		}
	
		for(int j=0; j < nSum; j++)
		{			
			if (len > pLen[j]) continue;

			float ratio = (len-pLen[j-1]) / (pLen[j]-pLen[j-1]);
			
			T t3d;
			t3d.x = pts[j-1].x + ratio * (pts[j].x - pts[j-1].x);
			t3d.y = pts[j-1].y + ratio * (pts[j].y - pts[j-1].y);
			t3d.z = pts[j-1].z + ratio * (pts[j].z - pts[j-1].z);
			
			dpts[i] = t3d;

			if (pos)
			{
				pos[i] = (j-1) + ratio;
			}
			
			break;
			
		}
	}

	
	if(pLen)
		delete []pLen;	

	return true;
	
}

void CScaleCranetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOPARALLEL:
		{
			CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
			int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
			
			// 线型库找不到线此线型
			if (nIndex == 0)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}
			
			int  nStyle = 0;
			float styles[8] = {0};
			
			if( nIndex>0 && nIndex<=baselineLib->GetBaseLineTypeCount() )
			{
				BaseLineType linetype = baselineLib->GetBaseLineType(nIndex-1);		
				
				nStyle = linetype.m_nNum;
				for( int i=0; i<linetype.m_nNum; i++)
				{
					styles[i] = fabs(linetype.m_fLens[i])*fDrawScale*ftrLinetypeScale;
				}
				
			}
			
			DWORD nColor = pGeo->GetColor();

			CArray<PT_3DEX,PT_3DEX> pts;
			pGeo->GetShape(pts);

			int size = pts.GetSize();

			if (size < 2)
			{
				break;
			}

			CGeoParallel *pPar = (CGeoParallel*)pGeo;

			double fWidth = pPar->GetWidth();
			double fAllLen = GraphAPI::GGetAllLen2D(pts.GetData(),pts.GetSize());
			
			double off = fabs(fWidth)/2 / sqrt(3);

			// 偏移
			PT_3DEX dpts[4];
			double len[2];
			len[0] = fAllLen/2 - off;
			len[1] = fAllLen/2 + off;

			// 地物太小，只显示基线
			if (len[0] < 0 || len[1] > fAllLen)
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}

			// 基线上的两点
			GetLengthPt(pts.GetData(),pts.GetSize(),len,2,dpts,NULL);

			// 辅助线上的两点
			CArray<PT_3DEX,PT_3DEX> arr;
			pPar->GetParallelShape(arr);
			if( arr.GetSize()<=0 )
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}
			GetLengthPt(arr.GetData(),arr.GetSize(),len,2,dpts+2,NULL);

			// 两测竖线（虚线）
			PT_3DEX tmppts[2];
			tmppts[0] = dpts[0];
			tmppts[1] = dpts[2];
			DashLine(tmppts,2,pBuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);
			
			tmppts[0] = dpts[1];
			tmppts[1] = dpts[3];
			DashLine(tmppts,2,pBuf,nColor,wid,fDrawScale*fViewScale,gGrdWid,fDrawScale*ftrLinetypeScale,FALSE,0,nStyle,styles,nIndex,bBreakLinetype,TRUE);

			// 叉叉（实线）
			pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
			pBuf->MoveTo(&dpts[0]);
			pBuf->LineTo(&dpts[3]);
			pBuf->End();	
			
			pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
			pBuf->MoveTo(&dpts[1]);
			pBuf->LineTo(&dpts[2]);
			pBuf->End();

			// 四个点上放1毫米竖齿（实线）
			/*PT_3D line[2];
			line[0] = GetRotate90Pt(pts[0],pts[1],pts[0],0.5*fDrawScale*fViewScale);
			line[1] = GetRotate90Pt(pts[0],pts[1],pts[0],-0.5*fDrawScale*fViewScale);

			GrBuffer buf;
			buf.BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
			buf.MoveTo(&line[0]);
			buf.LineTo(&line[1]);
			buf.End();	

			pBuf->AddBuffer(&buf);

			double matrix[16];
			Matrix44FromMove(arr[0].x-pts[0].x,arr[0].y-pts[0].y,arr[0].z-pts[0].z,matrix);
			buf.Transform(matrix);
			pBuf->AddBuffer(&buf);

			line[0] = GetRotate90Pt(pts[size-2],pts[size-1],pts[size-1],0.5*fDrawScale*fViewScale);
			line[1] = GetRotate90Pt(pts[size-2],pts[size-1],pts[size-1],-0.5*fDrawScale*fViewScale);
			
			GrBuffer buf1;
			buf1.BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
			buf1.MoveTo(&line[0]);
			buf1.LineTo(&line[1]);
			buf1.End();	
			
			pBuf->AddBuffer(&buf1);
			
			Matrix44FromMove(arr[size-1].x-pts[size-1].x,arr[size-1].y-pts[size-1].y,arr[size-1].z-pts[size-1].z,matrix);
			buf1.Transform(matrix);
			pBuf->AddBuffer(&buf1);*/

			

			// 基线和辅助线（实线）
			pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
			pBuf->MoveTo(&pts[0]);
			pBuf->LineTo(&pts[size-1]);
			pBuf->End();	

			pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
			pBuf->MoveTo(&arr[0]);
			pBuf->LineTo(&arr[size-1]);
			pBuf->End();

			break;
			
		}
		
	default:
		{
			pFt->Draw(pBuf,fDrawScale);
			break;
		}
	}

}

BOOL CScaleCranetype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPARALLEL)
		return TRUE;
	return FALSE;
}




BOOL CScaleCranetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleFunneltype::CScaleFunneltype()
{
	m_nType = SYMTYPE_SCALEFUNNELTYPE;
	
	m_fWidth = 0;
}

CScaleFunneltype::~CScaleFunneltype()
{
	
}

void CScaleFunneltype::CopyFrom(CSymbol *pSymbol)
{
	CScaleFunneltype *pScale = (CScaleFunneltype*)pSymbol;
	m_fWidth = pScale->m_fWidth;
	m_nFunnelType = pScale->m_nFunnelType;
	m_strBaseLinetypeName = pScale->m_strBaseLinetypeName;
}

BOOL CScaleFunneltype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_FUNNELTYPE) )
			{
				data = xmlFile.GetData();
				m_nFunnelType = _ttoi(data);
			}

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleFunneltype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%d"),m_nFunnelType);
			xmlFile.AddElem(XMLTAG_FUNNELTYPE,data);

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

void CScaleFunneltype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	if( Draw_core(pFt,pBuf,fDrawScale,fRotAngle,fViewScale) )
		return;

	pFt->Draw(pBuf,fDrawScale);
	return;
}


void CScaleFunneltype::GetShortLine(PT_3D pt1, PT_3D pt2, PT_3D pt3, double dis, double len, BOOL bClockwise, PT_3D& pt4, PT_3D& pt5)
{
	double dx = pt3.x-pt2.x, dy = pt3.y-pt2.y, dz = pt3.z-pt2.z;
	double dis1 = sqrt(dx*dx+dy*dy);
	if( dis1<1e-6 )
	{
		dx = 1.0;
		dy = 0;
		dz = 0;
	}
	else
	{
		dx /= dis1;
		dy /= dis1;
		dz /= dis1;
	}
	
	pt4.x = pt1.x + dx * dis;
	pt4.y = pt1.y + dy * dis;
	pt4.z = pt1.z + dz * dis;
	
	double dx1, dy1;
	if( bClockwise )
	{
		dx1 = dy;
		dy1 = -dx;
	}
	else
	{
		dx1 = -dy;
		dy1 = dx;
	}
	
	pt5.x = pt4.x + dx1 * len;
	pt5.y = pt4.y + dy1 * len;
	pt5.z = pt4.z;
	
}

BOOL CScaleFunneltype::Draw_core(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	// 线型库找不到线此线型
	if(nIndex == 0)
	{
		return FALSE;
	}
	
	int nGeoType = pGeo->GetClassType();

	//小比例尺的漏斗
	if( m_nFunnelType==0 && nGeoType==CLS_GEOPARALLEL )
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);

		int size = pts.GetSize();

		if( size<2 )
		{
			return FALSE;
		}

		CGeoParallel *pPar = (CGeoParallel*)pGeo;

		double fWidth = pPar->GetWidth();
		double fAllLen = GraphAPI::GGetAllLen2D(pts.GetData(),pts.GetSize());
		
		double off = 0.5*fDrawScale*fViewScale;

		// 偏移
		PT_3DEX dpts[4];
		double len[2];
		len[0] = fAllLen/2 - off;
		len[1] = fAllLen/2 + off;

		// 地物太小，只显示基线
		if (len[0] < 0 || len[1] > fAllLen)
		{
			return FALSE;
		}

		// 辅助线上1*0.6（毫米）的矩形
		float pos[2];
		CArray<PT_3DEX,PT_3DEX> arr;
		pPar->GetParallelShape(arr);
		GetLengthPt(arr.GetData(),arr.GetSize(),len,2,dpts,pos);

		int index0, index1;
		index0 = int(pos[0]);
		index1 = int(pos[1]);

		dpts[2] = PT_3DEX(GetRotate90Pt(arr[index0],arr[index0+1],dpts[0],0.6*fDrawScale*fViewScale),penNone);
		dpts[3] = PT_3DEX(GetRotate90Pt(arr[index1],arr[index1+1],dpts[1],0.6*fDrawScale*fViewScale),penNone);
		
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
		pBuf->MoveTo(&dpts[0]);
		pBuf->LineTo(&dpts[2]);
		pBuf->LineTo(&dpts[3]);
		pBuf->LineTo(&dpts[1]);
		pBuf->End();	

		// 基线首尾点到矩形实线
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
		pBuf->MoveTo(&pts[0]);
		pBuf->LineTo(&dpts[2]);
		pBuf->MoveTo(&pts[size-1]);
		pBuf->LineTo(&dpts[3]);
		pBuf->End();	
		
		// 基线和辅助线（实线）
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);
		pBuf->MoveTo(&pts[0]);
		pBuf->LineTo(&pts[size-1]);
		pBuf->MoveTo(&arr[0]);
		pBuf->LineTo(&arr[size-1]);
		pBuf->MoveTo(&pts[0]);
		pBuf->LineTo(&arr[0]);
		pBuf->MoveTo(&pts[size-1]);
		pBuf->LineTo(&arr[size-1]);
		pBuf->End();

		// 四个0.6*0.6 黑块
		len[0] = 0.6*fDrawScale*fViewScale;
		len[1] = fAllLen - 0.6*fDrawScale*fViewScale;
		
		// 地物太小，只显示基线
		if (len[0] < 0 || len[1] > fAllLen)
		{
			return FALSE;
		}

		GetLengthPt(pts.GetData(),pts.GetSize(),len,2,dpts,NULL);
		GetLengthPt(arr.GetData(),arr.GetSize(),len,2,dpts+2,NULL);

		PT_3DEX dpts2[4];
		double fAllLen2 = GraphAPI::GGet2DDisOf2P(pts[0],arr[0]);
		len[0] = 0.6*fDrawScale*fViewScale;
		len[1] = fAllLen2 - 0.6*fDrawScale*fViewScale;
		
		// 修正
		if (len[0] < 0)
		{
			len[0] = 0;
		}
		
		if (len[1] > fAllLen)
		{
			len[1] = fAllLen-1e-4;
		}

		PT_3DEX pts2[2];

		pts2[0] = pts[0];
		pts2[1] = arr[0];
		GetLengthPt(pts2,2,len,2,dpts2,NULL);

		pts2[0] = pts[size-1];
		pts2[1] = arr[size-1];
		GetLengthPt(pts2,2,len,2,dpts2+2,NULL);

		// 四个面填充
		PT_3D mid, pt;

		// 1
		mid.x = (dpts[0].x+dpts2[0].x)/2;
		mid.y = (dpts[0].y+dpts2[0].y)/2;
		mid.z = (dpts[0].z+dpts2[0].z)/2;
		pt.x = 2*mid.x - pts[0].x;
		pt.y = 2*mid.y - pts[0].y;
		pt.z = 2*mid.z - pts[0].z;
		
		pBuf->BeginPolygon(pGeo->GetColor());
		pBuf->MoveTo(&pts[0]);
		pBuf->LineTo(&dpts[0]);
		pBuf->LineTo(&pt);
		pBuf->LineTo(&dpts2[0]);
		pBuf->End();	

		// 2
		mid.x = (dpts[1].x+dpts2[2].x)/2;
		mid.y = (dpts[1].y+dpts2[2].y)/2;
		mid.z = (dpts[1].z+dpts2[2].z)/2;
		pt.x = 2*mid.x - pts[size-1].x;
		pt.y = 2*mid.y - pts[size-1].y;
		pt.z = 2*mid.z - pts[size-1].z;
		
		pBuf->BeginPolygon(pGeo->GetColor());
		pBuf->MoveTo(&pts[size-1]);
		pBuf->LineTo(&dpts[1]);
		pBuf->LineTo(&pt);
		pBuf->LineTo(&dpts2[2]);
		pBuf->End();

		// 3
		mid.x = (dpts[2].x+dpts2[1].x)/2;
		mid.y = (dpts[2].y+dpts2[1].y)/2;
		mid.z = (dpts[2].z+dpts2[1].z)/2;
		pt.x = 2*mid.x - arr[0].x;
		pt.y = 2*mid.y - arr[0].y;
		pt.z = 2*mid.z - arr[0].z;
		
		pBuf->BeginPolygon(pGeo->GetColor());
		pBuf->MoveTo(&arr[0]);
		pBuf->LineTo(&dpts[2]);
		pBuf->LineTo(&pt);
		pBuf->LineTo(&dpts2[1]);
		pBuf->End();

		// 4
		mid.x = (dpts[3].x+dpts2[3].x)/2;
		mid.y = (dpts[3].y+dpts2[3].y)/2;
		mid.z = (dpts[3].z+dpts2[3].z)/2;
		pt.x = 2*mid.x - arr[size-1].x;
		pt.y = 2*mid.y - arr[size-1].y;
		pt.z = 2*mid.z - arr[size-1].z;
		
		pBuf->BeginPolygon(pGeo->GetColor());
		pBuf->MoveTo(&arr[size-1]);
		pBuf->LineTo(&dpts[3]);
		pBuf->LineTo(&pt);
		pBuf->LineTo(&dpts2[3]);
		pBuf->End();

		// 外围线
		pFt->Draw(pBuf,fDrawScale);
			
	}
	//斗在坑内
	else if( m_nFunnelType==1 && (nGeoType==CLS_GEOCURVE || nGeoType==CLS_GEOSURFACE ) )
	{
		CArray<PT_3DEX,PT_3DEX> arrPts;
		pGeo->GetShape(arrPts);
		
		if( arrPts.GetSize()<4 )
		{
			return FALSE;
		}

		BOOL bClockwise_BaseLine = GraphAPI::GIsClockwise(arrPts.GetData(),arrPts.GetSize());
		
		CGeoCurveBase *pCurve = (CGeoCurveBase*)pGeo;

		int npt = arrPts.GetSize();
		int npt1 = npt;
		if( pCurve->IsClosed() )
		{
			npt1 = npt-1;
		}
		else
		{
			return FALSE;
		}

		//是否存在圆弧
		BOOL bHaveArc = FALSE;
		for( int i=0; i<arrPts.GetSize(); i++)
		{
			if( arrPts[i].pencode==penArc || arrPts[i].pencode==pen3PArc )
			{
				bHaveArc = TRUE;
				break;
			}
		}

		npt = arrPts.GetSize();
		if( bHaveArc )
		{
			pCurve->GetShape()->GetPts(arrPts);
			
			npt = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
			arrPts.SetSize(npt);
		}

		double len = 1.0 * fDrawScale*fViewScale;

		CArray<PT_3DEX,PT_3DEX> arrFindPts;

		PT_3D pt1, pt2;
		
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);

		//取等分点
		if( bHaveArc )
		{
			GraphAPI::CLengthCalculator2D<PT_3DEX> lc;
			lc.Init(arrPts.GetData(),npt);

			double all_len = lc.GetAllLen();
			int index;
			double t;
			PT_3DEX pt;

			pt = lc.GetLenPt(0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(0,&index,&t);
			arrFindPts.Add(arrPts[index]);
			arrFindPts.Add(arrPts[index+1]);

			pt = lc.GetLenPt(all_len-0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(0,&index,&t);
			arrFindPts.Add(arrPts[index+1]);
			arrFindPts.Add(arrPts[index]);

			pt = lc.GetLenPt(all_len*1.0/4+0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(all_len*1.0/4,&index,&t);
			arrFindPts.Add(arrPts[index]);
			arrFindPts.Add(arrPts[index+1]);

			pt = lc.GetLenPt(all_len*1.0/4-0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(all_len*1.0/4,&index,&t);
			arrFindPts.Add(arrPts[index+1]);
			arrFindPts.Add(arrPts[index]);

			pt = lc.GetLenPt(all_len*2.0/4+0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(all_len*2.0/4,&index,&t);
			arrFindPts.Add(arrPts[index]);
			arrFindPts.Add(arrPts[index+1]);

			pt = lc.GetLenPt(all_len*2.0/4-0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(all_len*2.0/4,&index,&t);
			arrFindPts.Add(arrPts[index+1]);
			arrFindPts.Add(arrPts[index]);

			pt = lc.GetLenPt(all_len*3.0/4+0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(all_len*3.0/4,&index,&t);
			arrFindPts.Add(arrPts[index]);
			arrFindPts.Add(arrPts[index+1]);

			pt = lc.GetLenPt(all_len*3.0/4-0.5*len,&index,&t);
			arrFindPts.Add(pt);
			lc.GetLenPt(all_len*3.0/4,&index,&t);
			arrFindPts.Add(arrPts[index+1]);
			arrFindPts.Add(arrPts[index]);

			//在上述点上画短线
			BOOL bCloseWise = bClockwise_BaseLine;
			for( i=0; i<24; i+=3)
			{
				GetShortLine(arrFindPts[i],arrFindPts[i+1],arrFindPts[i+2],0,len,bCloseWise,pt1,pt2);
				pBuf->MoveTo(&pt1);	
				pBuf->LineTo(&pt2);

				bCloseWise = !bCloseWise;
			}
		}
		else
		{
			//取四个线段的中点
			for( int i=0; i<npt1; i++)
			{
				arrFindPts.Add(GraphAPI::GGetMiddlePt(arrPts[i],arrPts[i+1]));
				arrFindPts.Add(arrPts[i]);
				arrFindPts.Add(arrPts[i+1]);
			}

			//在上述点的两侧画短线
			for( i=0; i<npt1*3; i+=3)
			{
				GetShortLine(arrFindPts[i],arrFindPts[i+1],arrFindPts[i+2],0.5*len,len,bClockwise_BaseLine,pt1,pt2);
				pBuf->MoveTo(&pt1);	
				pBuf->LineTo(&pt2);
				
				GetShortLine(arrFindPts[i],arrFindPts[i+2],arrFindPts[i+1],0.5*len,len,!bClockwise_BaseLine,pt1,pt2);
				pBuf->MoveTo(&pt1);	
				pBuf->LineTo(&pt2);
			}
		}

		pBuf->End();

		// 中间漏斗										
		float delta_theta = 0.25;
		
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);

		PT_3D cen;
		pGeo->GetCenter(&arrPts[0],&cen);
		
		// 直径1mm
		double smallr = 0.5*fDrawScale*fViewScale;
		for( float angle = 0; angle < 2*PI; angle += delta_theta )
		{
			PT_3D tmp;
			tmp.x = cen.x + smallr*cos(angle);
			tmp.y = cen.y + smallr*sin(angle);
			tmp.z = cen.z;
			
			if (angle == 0)
			{
				pBuf->MoveTo(&tmp);
			}
			else
			{
				pBuf->LineTo(&tmp);
			}
		}
		
		pBuf->End();
		
		//绘制边界
		pFt->Draw(pBuf,fDrawScale);
	}
	else if (m_nFunnelType == 3)
	{
		CArray<PT_3DEX,PT_3DEX> pts;
		pGeo->GetShape(pts);
		
		if( pts.GetSize()<4 )
		{
			return FALSE;
		}

		if (!((pts[0].pencode == pen3PArc && pts[1].pencode == pen3PArc && pts[2].pencode == pen3PArc) || (pts[0].pencode == penArc && pts[1].pencode == penArc && pts[2].pencode == penArc)))
		{
			return FALSE;
		}
		
		// 圆心，半径
		double r;
		PT_3DEX cen;
		if (!CalcArcParamFrom3P(pts.GetData(),&cen,&r,NULL,NULL))return FALSE;

		PT_3D cen2;
		pGeo->GetCenter(NULL,&cen2);
		cen.z = cen2.z;
		
		// 中间漏斗										
		float delta_theta = 0.25;
		
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);

		// 直径1mm
		double smallr = 0.5*fDrawScale*fViewScale;
		for( float angle = 0; angle < 2*PI; angle += delta_theta )
		{
			PT_3D tmp;
			tmp.x = cen.x + smallr*cos(angle);
			tmp.y = cen.y + smallr*sin(angle);
			tmp.z = cen.z;
			
			if (angle == 0)
			{
				pBuf->MoveTo(&tmp);
			}
			else
			{
				pBuf->LineTo(&tmp);
			}
		}

		pBuf->End();

		// 当中叉，小圆到大圆四段线组成，
		PT_3DEX line[8];
		line[0].x = cen.x + smallr*cos(PI/4);
		line[0].y = cen.y + smallr*sin(PI/4);
		line[0].z = cen.z;
		line[1].x = cen.x + r*cos(PI/4);
		line[1].y = cen.y + r*sin(PI/4);
		line[1].z = cen.z;

		line[2].x = cen.x + smallr*cos(PI*3/4);
		line[2].y = cen.y + smallr*sin(PI*3/4);
		line[2].z = cen.z;
		line[3].x = cen.x + r*cos(PI*3/4);
		line[3].y = cen.y + r*sin(PI*3/4);
		line[3].z = cen.z;

		line[4].x = cen.x + smallr*cos(-PI/4);
		line[4].y = cen.y + smallr*sin(-PI/4);
		line[4].z = cen.z;
		line[5].x = cen.x + r*cos(-PI/4);
		line[5].y = cen.y + r*sin(-PI/4);
		line[5].z = cen.z;

		line[6].x = cen.x + smallr*cos(-PI*3/4);
		line[6].y = cen.y + smallr*sin(-PI*3/4);
		line[6].z = cen.z;
		line[7].x = cen.x + r*cos(-PI*3/4);
		line[7].y = cen.y + r*sin(-PI*3/4);
		line[7].z = cen.z;
		
		GraphAPI::GetHeightFromSurface(pts[0],pts[1],pts[2],line,8);
		
		for (int i=0; i<8; i++)
		{
			if (i%2 == 0)
			{							
				pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
				pBuf->MoveTo(&line[i]);
			}
			else
			{
				pBuf->LineTo(&line[i]);
				pBuf->End();
			}
		}
		
		// 外围线
		pFt->Draw(pBuf,fDrawScale);
	}

	return TRUE;
}

BOOL CScaleFunneltype::IsSupportGeoType(int nGeoType)
{
	if(nGeoType == CLS_GEOPARALLEL || nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEOSURFACE)
		return TRUE;
	return FALSE;
}


BOOL CScaleFunneltype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcSymbol_LT::CProcSymbol_LT()
{
	m_nType = SYMTYPE_SCALE_LiangChang;
	
	m_fWidth = 0;
}

CProcSymbol_LT::~CProcSymbol_LT()
{
	
}

void CProcSymbol_LT::CopyFrom(CSymbol *pSymbol)
{
	CProcSymbol_LT *pScale = (CProcSymbol_LT*)pSymbol;
	m_fWidth = pScale->m_fWidth;
	m_strBaseLinetypeName = pScale->m_strBaseLinetypeName;
}

BOOL CProcSymbol_LT::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CProcSymbol_LT::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}


void CProcSymbol_LT::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{

}

BOOL CProcSymbol_LT::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOCURVE || nGeoType==CLS_GEOSURFACE )
		return TRUE;
	return FALSE;
}




BOOL CProcSymbol_LT::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



void CScaleLiangCang::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();

	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}

	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;

	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	Envelope e = pGeo->GetEnvelope();

	//计算中心位置
	PT_3D center, dpt;
	center.x = e.CenterX(); center.y = e.CenterY(); center.z = e.CenterZ();
	double radius = (e.Width()>e.Height()?e.Width():e.Height());

	//超出1毫米
	radius = radius*0.5 + 1.0 * fDrawScale;

	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	//三个方向
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&center);

	dpt.x = 0; dpt.y = radius;
	pBuf->LineTo(&PT_3D(center.x+dpt.x,center.y+dpt.y,center.z));

	pBuf->MoveTo(&center);
	dpt.x = radius*cos(7*PI/6); dpt.y = radius*sin(7*PI/6);
	pBuf->LineTo(&PT_3D(center.x+dpt.x,center.y+dpt.y,center.z));

	pBuf->MoveTo(&center);
	dpt.x = radius*cos(11*PI/6); dpt.y = radius*sin(11*PI/6);
	pBuf->LineTo(&PT_3D(center.x+dpt.x,center.y+dpt.y,center.z));

	pBuf->End();

	pFt->Draw(pBuf,fDrawScale);

}



void CScaleYouGuan::CopyFrom(CSymbol *pSymbol)
{
	CScaleYouGuan *pScale = (CScaleYouGuan*)pSymbol;
	m_fWidth = pScale->m_fWidth;
	m_strBaseLinetypeName = pScale->m_strBaseLinetypeName;
	m_bFilled = pScale->m_bFilled;
}

BOOL CScaleYouGuan::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}

			if( xmlFile.FindElem(XMLTAG_FILLED) )
			{
				data = xmlFile.GetData();
				m_bFilled = _ttoi(data);
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleYouGuan::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);

			data.Format(_T("%d"),m_bFilled);
			xmlFile.AddElem(XMLTAG_FILLED,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}


void CScaleYouGuan::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	CGeoCurveBase *pCurve = (CGeoCurveBase*)pGeo;
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pCurve->GetShape()->GetPts(arrPts);
	int npt = GraphAPI::GKickoffSame2DPoints(arrPts.GetData(),arrPts.GetSize());
	arrPts.SetSize(npt);

	Envelope e = pGeo->GetEnvelope();
	
	//计算中心位置
	PT_3D center;
	pCurve->GetCenter(NULL,&center);		
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;

	PT_3DEX ret1, ret2;
	double t1,t2;
	GraphAPI::GGetFirstPointOfRayIntersect(arrPts.GetData(),npt,
		PT_3DEX(center,penLine),PT_3DEX(e.m_xl-10,center.y,center.z,penLine),
		&ret1,&t1,NULL);

	GraphAPI::GGetFirstPointOfRayIntersect(arrPts.GetData(),npt,
		PT_3DEX(center,penLine),PT_3DEX(e.m_xh+10,center.y,center.z,penLine),
		&ret2,&t2,NULL);
	
	if( !m_bFilled )
	{
		//绘制直径
		pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
		pBuf->MoveTo(&PT_3D(e.m_xl,center.y,center.z));
		
		pBuf->LineTo(&PT_3D(e.m_xh,center.y,center.z));
		
		pBuf->End();
	}
	else
	{
		int index = ceil(t1);
		int index2 = floor(t2);

		if( (index-t1)==0.0 )
		{
			index++;
		}

		int step = 1;
		//顺序取点
		if( arrPts[index].y>center.y )
		{			
		}
		//逆序取点
		else
		{
			step = -1;
			index = floor(t1);
			index2 = ceil(t2);
		}

		pBuf->BeginPolygon(pGeo->GetColor());

		pBuf->MoveTo(&ret1);

		for( int i=index; ; )
		{
			pBuf->LineTo(&arrPts[i]);
			i = (i+step+npt)%npt;

			if( i==index2 )
				break;
		}

		pBuf->LineTo(&arrPts[i]);
		pBuf->LineTo(&ret2);
		pBuf->LineTo(&ret1);

		pBuf->End();

	}
	
	pFt->Draw(pBuf,fDrawScale);
}


void CScaleJianFangWu::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);

	if( arrPts.GetSize()<4 )
	{
		//pFt->Draw(pBuf,fDrawScale);
		return;
	}
		
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	PT_3DEX line[2];
	line[0] = arrPts[0];
	line[1] = arrPts[arrPts.GetSize()/2];
	int index = 0;
	double ang = GraphAPI::GGetAngle(line[1].x,line[1].y,line[0].x,line[0].y);
	if ((ang > PI/2 && ang < PI) || (ang > PI*3/2 && ang < 2*PI))
	{
		index = 1;
	}

	//绘制一条对角线
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);

	pBuf->MoveTo(&arrPts[index]);
	
	pBuf->LineTo(&arrPts[index+arrPts.GetSize()/2]);
	
	pBuf->End();
	
	//pFt->Draw(pBuf,fDrawScale);
}

void CScaleDiShangYaoDong::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	
	if( arrPts.GetSize()<=2 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	// 三点有效，前两点是窑洞外的两点，第三点到前两点组成线的距离表示窑洞的高
	// 外围线
	PT_3DEX pt;
	double hei = GraphAPI::GGetNearestDisOfPtToLine(arrPts[0].x,arrPts[0].y,arrPts[1].x,arrPts[1].y,arrPts[2].x,arrPts[2].y,&pt.x,&pt.y);

	bool bClockwise = (GraphAPI::GIsClockwise(arrPts.GetData(),3)==1);
	if( bClockwise )hei = -hei;

	PT_3DEX line[2];
	GraphAPI::GGetParallelLine(arrPts.GetData(),2,hei,line);
	GraphAPI::GetHeightFromSurface(arrPts[0],arrPts[1],arrPts[2],line,2);
	
	double radius = GraphAPI::GGet2DDisOf2P(line[0],line[1])/2;

	PT_3DEX ptMid;
	float baseAng = GraphAPI::GGetAngle(line[1].x,line[1].y,line[0].x,line[0].y);
	GraphAPI::GGetMiddlePt(line,2,&ptMid);
	CArray<PT_3DEX,PT_3DEX> arrArc;
	if (bClockwise)
	{
		GetArcPts(ptMid,radius,baseAng,baseAng+PI,arrArc);
	}
	else
	{
		GetArcPts(ptMid,radius,baseAng-PI,baseAng,arrArc);
	}
	
	GraphAPI::GetHeightFromSurface(arrPts[0],arrPts[1],arrPts[2],arrArc.GetData(),arrArc.GetSize());

	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&arrPts[0]);	
	pBuf->LineTo(&line[0]);
	if (bClockwise)
	{
		for (int i=0; i<arrArc.GetSize(); i++)
		{
			pBuf->LineTo(&arrArc[i]);
		}
	}
	else
	{
		for (int i=arrArc.GetSize()-1; i>=0; i--)
		{
			pBuf->LineTo(&arrArc[i]);
		}
	}
	
	pBuf->LineTo(&line[1]);
	pBuf->LineTo(&arrPts[1]);
	pBuf->End();
	
	// 当中修饰直线
	PT_3DEX line0[2];
	line0[0] = ptMid;

	PT_3DEX ptMid0;
	GraphAPI::GGetMiddlePt(arrPts.GetData(),2,&ptMid0);

	double ang0 = PI/2;
	if (bClockwise)  ang0 = -PI/2;
	PT_3D ret0 = GetRotatePt(arrPts[0],arrPts[1],ptMid0,fabs(hei)*2/3,ang0);	
	line0[0] = PT_3DEX(ret0,penLine);

	GraphAPI::GetHeightFromSurface(arrPts[0],arrPts[1],arrPts[2],line0,2);
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&ptMid0);	
	pBuf->LineTo(&ret0);
	pBuf->End();

}

BOOL CScaleTongCheShuiZha::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOPARALLEL || nGeoType==CLS_GEODCURVE )
		return TRUE;
	return FALSE;
}


// dir==0，沿着idx0向前，dir==1，沿着idx0向后
static void GetIntersect_345(PT_3D pt0, PT_3D dpt, PT_3DEX *pts, int npt, int idx0, PT_3D pt_start, int dir, int &idx_ret, PT_3D& pt_ret)
{
	Envelope e;
	e = CreateEnvelopeFromPts(pts,npt);
	double r = (e.Width()+e.Height()) * 2;
	double len = sqrt(dpt.x*dpt.x+dpt.y*dpt.y);
	if( len<1e-4 )len = 0;
	else len = 1.0/len;

	PT_3D pt1(pt0.x+dpt.x*r*len, pt0.y+dpt.y*r*len, pt0.z);

	PT_3D pt = pt_start, pt_r;
	double t;

	//沿着idx0向前
	if( dir==0 )
	{
		for( int i=idx0; i>=0; i--)
		{
			if( GraphAPI::GGetLineIntersectLineSeg(pts[i].x,pts[i].y,pt.x,pt.y,pt0.x,pt0.y,pt1.x,pt1.y,&pt_r.x,&pt_r.y,&t) )
			{
				pt_ret = pt_r;
				pt_ret.z = pts[i].z + (pt.z-pts[i].z)*t;
				
				idx_ret = i;
				return;
			}
			
			pt = pts[i];
		}
	}
	//沿着idx0向后
	else
	{
		for( int i=idx0+1; i<npt; i++)
		{
			if( GraphAPI::GGetLineIntersectLineSeg(pt.x,pt.y,pts[i].x,pts[i].y,pt0.x,pt0.y,pt1.x,pt1.y,&pt_r.x,&pt_r.y,&t) )
			{
				pt_ret = pt_r;
				pt_ret.z = pt.z + (pts[i].z-pt.z)*t;
				
				idx_ret = i-1;
				return;
			}
			
			pt = pts[i];
		}
	}

	//没有找到合适的点，就取端点
	if( dir==0 )
	{
		pt_ret = pts[0];
		idx_ret = 0;
	}
	else
	{
		pt_ret = pts[npt-1];
		idx_ret = npt-1;
	}

}


//绘制水闸符号，pts0是母线，pts1是辅助线，算法如下：
//1、求pts0的中点A
//2、求pts1的中点B
//3、延长AB到C，使得AB/BC = 4
//4、设pts0总长度为len，将A沿着线pts0向左右取点D、E，是的路径长度AD=AE=len/8
//5、角DCE切割辅助线，计算交点
//6、绘制水闸符号

static void DrawShuiZha(PT_3DEX *pts0, int npt0, PT_3DEX *pts1, int npt1, GrBuffer *pBuf, float fDrawScale)
{
	GraphAPI::CLengthCalculator2D<PT_3DEX> lc;

	//1、求pts0的中点A
	PT_3DEX a, b, c;
	lc.Init(pts0,npt0);
	double len0 = lc.GetAllLen();
	int idx0 = -1;
	a = lc.GetLenPt(len0*0.5,&idx0,NULL);

	//2、求pts1的中点B
	lc.Init(pts1,npt1);
	double len1 = lc.GetAllLen();
	int idx1 = -1;
	b = lc.GetLenPt(len1*0.5,&idx1,NULL);

	if( idx0<0 || idx1<0 )
		return;

	//3、延长AB到AC，使得AB/BC = 4
	double dis = GraphAPI::GGet2DDisOf2P(a,b);
	double dis1 = dis*0.25;
	if( dis<1e-6 )
	{
		c.x = b.x;
		c.y = b.y;
		c.z = b.z;
	}
	else
	{
		c.x = b.x + fDrawScale*dis1 * (b.x-a.x)/dis;
		c.y = b.y + fDrawScale*dis1 * (b.y-a.y)/dis;
		c.z = b.z;
	}
	//4、设pts0总长度为len，将A沿着线pts0向左右取点D、E，是的路径长度AD=AE=len/8
	PT_3DEX d, e;
	int idx_ret1, idx_ret2, idx_ret3, idx_ret4;

	lc.Init(pts0,npt0);
	d = lc.GetLenPt(len0*3/8,&idx_ret1,NULL);
	e = lc.GetLenPt(len0*5/8,&idx_ret2,NULL);

	PT_3DEX test[3] = {a,b,pts0[0]};
	if( GraphAPI::GIsClockwise(test,3)==1 )
	{
		//PT_3DEX t = d;
		//d = e;
		//e = t;
	}

	//5、角DCE分别切割母线、辅助线，计算交点
	
	PT_3D pt_ret1, pt_ret2, pt_ret3, pt_ret4;
	pt_ret1 = d;
	pt_ret2 = e;

	PT_3D d1(d.x-c.x,d.y-c.y,d.z-c.z), e1(e.x-c.x,e.y-c.y,e.z-c.z);

	GetIntersect_345(c,d1,pts1,npt1,idx1,b,0,idx_ret3,pt_ret3);
	GetIntersect_345(c,e1,pts1,npt1,idx1,b,1,idx_ret4,pt_ret4);

	//6、绘制水闸符号
	//母线部分
	pBuf->MoveTo(&pts0[0]);
	for( int i=1; i<=idx_ret1; i++)
	{
		pBuf->LineTo(&pts0[i]);
	}
	pBuf->LineTo(&pt_ret1);

	pBuf->LineTo(&c);

	pBuf->LineTo(&pt_ret2);

	for( i=idx_ret2+1; i<npt0; i++)
	{
		pBuf->LineTo(&pts0[i]);
	}

	//辅助线部分
	pBuf->MoveTo(&pts1[0]);
	for( i=1; i<=idx_ret3; i++)
	{
		pBuf->LineTo(&pts1[i]);
	}
	pBuf->LineTo(&pt_ret3);	
	
	pBuf->MoveTo(&pt_ret4);
	
	for( i=idx_ret4+1; i<npt1; i++)
	{
		pBuf->LineTo(&pts1[i]);
	}
}

//获取邻近的点，dir==0向前，dir==1向后
static int GetNearbyPt_345(PT_3DEX *pts0, int npt0, int index0, int dir)
{
	int i1 = -1;
	if( dir==0 )
	{
		for( int i=index0-1; i>=0; i--)
		{
			if( GraphAPI::GGet2DDisOf2P(pts0[i],pts0[index0])>1e-4 )
			{
				i1 = i;
				break;
			}
		}
	}
	else
	{
		for( int i=index0+1; i<npt0; i++)
		{
			if( GraphAPI::GGet2DDisOf2P(pts0[i],pts0[index0])>1e-4 )
			{
				i1 = i;
				break;
			}
		}
	}

	return i1;
}


//绘制首尾两端的齿
static void DrawChi(PT_3DEX *pts0, int npt0, PT_3DEX *pts1, int npt1, GrBuffer *pBuf, float fDrawScale)
{
	//判断母线辅助线的位置关系
	int i1 = GetNearbyPt_345(pts0,npt0,0,1);
	if( i1<0 )
		return;

	PT_3DEX test[3] = {pts0[0],pts0[i1],pts1[0]};
	BOOL bLeft = (GraphAPI::GIsClockwise(test,3)==0?TRUE:FALSE);

	double r = fDrawScale * 1.6;  //1.6 mm
	//母线部分
	if( 1 )
	{
		double rot = -0.75*PI;
		if( !bLeft )
			rot = 0.75*PI;

		//起点的齿
		if( 1 )
		{
			double ang = GraphAPI::GGetAngle(pts0[0].x,pts0[0].y,pts0[i1].x,pts0[i1].y);
			
			PT_3D pt0 = pts0[0];
			PT_3D pt1;
			pt1.x = pts0[0].x + r * cos(ang+rot);
			pt1.y = pts0[0].y + r * sin(ang+rot);
			pt1.z = pt0.z;
			
			pBuf->MoveTo(&pt0);
			pBuf->LineTo(&pt1);
		}

		//终点的齿
		if( 1 )
		{
			i1 = GetNearbyPt_345(pts0,npt0,npt0-1,0);
			if( i1<0 )
				return;

			double ang = GraphAPI::GGetAngle(pts0[npt0-1].x,pts0[npt0-1].y,pts0[i1].x,pts0[i1].y);
			
			PT_3D pt0 = pts0[npt0-1];
			PT_3D pt1;
			pt1.x = pts0[npt0-1].x + r * cos(ang-rot);
			pt1.y = pts0[npt0-1].y + r * sin(ang-rot);
			pt1.z = pt0.z;
			
			pBuf->MoveTo(&pt0);
			pBuf->LineTo(&pt1);
		}
	}

	//辅助线部分
	if( 1 )
	{
		double rot = 0.75*PI;
		if( !bLeft )
			rot = -0.75*PI;
		
		//起点的齿
		if( 1 )
		{
			i1 = GetNearbyPt_345(pts1,npt1,0,1);
			if( i1<0 )
				return;

			double ang = GraphAPI::GGetAngle(pts1[0].x,pts1[0].y,pts1[i1].x,pts1[i1].y);
			
			PT_3D pt0 = pts1[0];
			PT_3D pt1;
			pt1.x = pts1[0].x + r * cos(ang+rot);
			pt1.y = pts1[0].y + r * sin(ang+rot);
			pt1.z = pt0.z;
			
			pBuf->MoveTo(&pt0);
			pBuf->LineTo(&pt1);
		}
		
		//终点的齿
		if( 1 )
		{
			i1 = GetNearbyPt_345(pts1,npt1,npt1-1,0);
			if( i1<0 )
				return;
			
			double ang = GraphAPI::GGetAngle(pts1[npt1-1].x,pts1[npt1-1].y,pts1[i1].x,pts1[i1].y);
			
			PT_3D pt0 = pts1[npt1-1];
			PT_3D pt1;
			pt1.x = pts1[npt1-1].x + r * cos(ang-rot);
			pt1.y = pts1[npt1-1].y + r * sin(ang-rot);
			pt1.z = pt0.z;
			
			pBuf->MoveTo(&pt0);
			pBuf->LineTo(&pt1);
		}
	}
}


void CScaleTongCheShuiZha::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOPARALLEL && pGeo->GetClassType()!=CLS_GEODCURVE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}

	//获取两边的线
	CArray<PT_3DEX,PT_3DEX> arrPts0, arrPts1;
	if( pGeo->GetClassType()==CLS_GEOPARALLEL )
	{
		CGeoParallel *pCurve = ((CGeoParallel*)pGeo);
		pCurve->GetShape(arrPts0);
		pCurve->GetParallelShape(arrPts1);
	}
	else
	{
		CGeoDCurve *pCurve = ((CGeoDCurve*)pGeo);
		pCurve->GetBaseShape(arrPts0);
		pCurve->GetAssistShape(arrPts1);
		ReversePoints(arrPts1.GetData(),arrPts1.GetSize());
	}

	if( arrPts0.GetSize()<2 || arrPts1.GetSize()<2 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}	
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	//绘制水闸部分
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	
	DrawShuiZha(arrPts0.GetData(),arrPts0.GetSize(),arrPts1.GetData(),arrPts1.GetSize(),pBuf,fDrawScale*fViewScale);
	
	DrawChi(arrPts0.GetData(),arrPts0.GetSize(),arrPts1.GetData(),arrPts1.GetSize(),pBuf,fDrawScale*fViewScale);

	pBuf->End();
}

BOOL CScaleBuTongCheShuiZha::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOPARALLEL || nGeoType==CLS_GEODCURVE )
		return TRUE;
	return FALSE;
}


void CScaleBuTongCheShuiZha::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOPARALLEL && pGeo->GetClassType()!=CLS_GEODCURVE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	//获取两边的线
	CArray<PT_3DEX,PT_3DEX> arrPts0, arrPts1;
	if( pGeo->GetClassType()==CLS_GEOPARALLEL )
	{
		CGeoParallel *pCurve = ((CGeoParallel*)pGeo);
		pCurve->GetShape(arrPts0);
		pCurve->GetParallelShape(arrPts1);
	}
	else
	{
		CGeoDCurve *pCurve = ((CGeoDCurve*)pGeo);
		pCurve->GetBaseShape(arrPts0);
		pCurve->GetAssistShape(arrPts1);	
		ReversePoints(arrPts1.GetData(),arrPts1.GetSize());
	}
	
	if( arrPts0.GetSize()<2 || arrPts1.GetSize()<2 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}	
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	//绘制水闸部分
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	
	DrawShuiZha(arrPts0.GetData(),arrPts0.GetSize(),arrPts1.GetData(),arrPts1.GetSize(),pBuf,fDrawScale*fViewScale);

	pBuf->End();
}

void CScaleChuanSongDai::CopyFrom(CSymbol *pSymbol)
{
	CScaleChuanSongDai *pScale = (CScaleChuanSongDai*)pSymbol;
	m_nCarryType = pScale->m_nCarryType;
	m_fWidth = pScale->m_fWidth;
	m_strBaseLinetypeName = pScale->m_strBaseLinetypeName;
}

BOOL CScaleChuanSongDai::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strBaseLinetypeName = data;
			}

			if( xmlFile.FindElem(XMLTAG_CARRYTYPE) )
			{
				data = xmlFile.GetData();
				m_nCarryType = _ttoi(data);
			}
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
	
}

void CScaleChuanSongDai::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strBaseLinetypeName);

			data.Format(_T("%d"),m_nCarryType);
			xmlFile.AddElem(XMLTAG_CARRYTYPE,data);
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

BOOL CScaleChuanSongDai::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOPARALLEL  )
		return TRUE;
	return FALSE;
}


void CScaleChuanSongDai::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOPARALLEL  )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	//获取两边的线
	CArray<PT_3DEX,PT_3DEX> arrPts0, arrPts1;
	CGeoParallel *pCurve = ((CGeoParallel*)pGeo);
	pCurve->GetShape(arrPts0);
	pCurve->GetParallelShape(arrPts1);
	
	if( arrPts0.GetSize()<2 || arrPts1.GetSize()<2 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}	
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;
	
	double fParaWid = fabs(pCurve->GetWidth());

	// 地上从辅助线开始,地下传送带从基线开始
	CArray<PT_3DEX,PT_3DEX> *pArr0 = &arrPts1, *pArr1 = &arrPts0;
	if (m_nCarryType == 1)
	{
		pArr0 = &arrPts0;
		pArr1 = &arrPts1;
	}

	CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
	
	GraphAPI::GGetCycleLine(pArr0->GetData(),pArr0->GetSize(),fParaWid*2,0,dpt0,0);
	GraphAPI::GGetCycleLine(pArr1->GetData(),pArr1->GetSize(),fParaWid*2,fParaWid,dpt1,0);

	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	
	int num0 = dpt0.GetSize();
	int num1 = dpt1.GetSize();

	if (num0 == num1)
	{
		pBuf->MoveTo(&dpt0[0]);
		pBuf->LineTo(&dpt1[0]);
		
		for (int i=1; i<num0; i++)
		{
			if (i < dpt1.GetSize())
			{				
				pBuf->LineTo(&dpt0[i]);
				pBuf->LineTo(&dpt1[i]);
			}
			
			if (i == num0-1)
			{
				PT_3DEX pt0 = pArr0->GetAt(pArr0->GetSize()-1);
				PT_3DEX pt1 = pArr1->GetAt(pArr1->GetSize()-1);
				double dis = GraphAPI::GGet2DDisOf2P(dpt1[i],pt1);
				double ratio = dis/GraphAPI::GGet2DDisOf2P(pt0,pt1);
				
				PT_3DEX ret;
				ret.x = pt1.x + (pt0.x - pt1.x) * ratio;
				ret.y = pt1.y + (pt0.y - pt1.y) * ratio;
				ret.z = pt1.z + (pt0.z - pt1.z) * ratio;
				
				pBuf->LineTo(&ret);
			}
			
		}
	}
	else if (num0 > num1)
	{
		pBuf->MoveTo(&dpt0[0]);

		for (int i=0; i<num0-1; i++)
		{
			if (i < dpt1.GetSize())
			{				
				pBuf->LineTo(&dpt1[i]);
				pBuf->LineTo(&dpt0[i+1]);
			}
			
			if (i == num0-2)
			{
				PT_3DEX pt0 = pArr0->GetAt(pArr0->GetSize()-1);
				PT_3DEX pt1 = pArr1->GetAt(pArr1->GetSize()-1);
				double dis = GraphAPI::GGet2DDisOf2P(dpt0[i+1],pt0);
				double ratio = dis/GraphAPI::GGet2DDisOf2P(pt0,pt1);
				
				PT_3DEX ret;
				ret.x = pt0.x + (pt1.x - pt0.x) * ratio;
				ret.y = pt0.y + (pt1.y - pt0.y) * ratio;
				ret.z = pt0.z + (pt1.z - pt0.z) * ratio;

				pBuf->LineTo(&ret);
			}
			
		}
	}

	
	
	pBuf->End();
}
BOOL CScaleChuRuKou::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOCURVE || nGeoType==CLS_GEOSURFACE )
		return TRUE;
	return FALSE;
}


void CScaleChuRuKou::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}	

	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	
	if( arrPts.GetSize()<4 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;

	//绘制A字形
	int npt = arrPts.GetSize();
	int npt1 = npt;
	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		if( ((CGeoCurve*)pGeo)->IsClosed() )
		{
			if( npt<=4 )
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}

			npt1 = npt-1;
		}
		else
		{
			if( npt<4 )
			{
				pFt->Draw(pBuf,fDrawScale);
				return;
			}
		}
	}
	else
	{
		if( npt<4 )
		{
			pFt->Draw(pBuf,fDrawScale);
			return;
		}
	}
			
	PT_3DEX pt0 = arrPts[0];
	PT_3DEX pt1 = arrPts[npt1-1];
	PT_3DEX pt2 = arrPts[npt1/2-1];
	PT_3DEX pt3 = arrPts[npt1/2];
	PT_3DEX pt4;
	pt4.x = (pt2.x+pt3.x)*0.5; pt4.y = (pt2.y+pt3.y)*0.5; pt4.z = (pt2.z+pt3.z)*0.5;

	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&pt0);
	
	pBuf->LineTo(&pt4);
	pBuf->LineTo(&pt1);
	
	pt2.x = (pt0.x+pt4.x)*0.5; pt2.y = (pt0.y+pt4.y)*0.5; pt2.z = (pt0.z+pt4.z)*0.5;
	pt3.x = (pt1.x+pt4.x)*0.5; pt3.y = (pt1.y+pt4.y)*0.5; pt3.z = (pt1.z+pt4.z)*0.5;
	
	pBuf->MoveTo(&pt2);
	pBuf->LineTo(&pt3);

	pBuf->End();
	
	//绘制边界
	pFt->Draw(pBuf,fDrawScale);
}


BOOL CScaleWenShi::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOCURVE || nGeoType==CLS_GEOSURFACE )
		return TRUE;
	return FALSE;
}


void CScaleWenShi::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}	

	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	
	if( arrPts.GetSize()<4 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strBaseLinetypeName) + 1;

	CGeoCurveBase *pCurve = (CGeoCurveBase*)pGeo;

	//绘制A字形
	int npt = arrPts.GetSize();
	int npt1 = npt;
	if( pCurve->IsClosed() )
	{
		if( npt<=4 )
		{
			pFt->Draw(pBuf,fDrawScale);
			return;
		}
		npt1 = npt-1;
	}
			
	PT_3DEX pt0 = arrPts[0];
	PT_3DEX pt1 = arrPts[1];
	PT_3DEX pt2 = arrPts[npt1-2];
	PT_3DEX pt3 = arrPts[npt1-1];
	PT_3DEX pt4, pt5;
	pt4.x = (pt0.x+pt1.x)*0.5; pt4.y = (pt0.y+pt1.y)*0.5; pt4.z = (pt0.z+pt1.z)*0.5;
	pt5.x = (pt2.x+pt3.x)*0.5; pt5.y = (pt2.y+pt3.y)*0.5; pt5.z = (pt2.z+pt3.z)*0.5;

	//计算pt0 pt1 pt2的角平分线pt0-pt6, pt1 pt2 pt3 的角平分线pt2-pt7
	PT_3DEX pt6, pt7;
	double angle = (GraphAPI::GGetAngle(pt1.x,pt1.y,pt2.x,pt2.y)+GraphAPI::GGetAngle(pt1.x,pt1.y,pt0.x,pt0.y))*0.5;
	pt6.x = pt1.x+100*cos(angle);
	pt6.y = pt1.y+100*sin(angle);

	angle = (GraphAPI::GGetAngle(pt2.x,pt2.y,pt3.x,pt3.y)+GraphAPI::GGetAngle(pt2.x,pt2.y,pt1.x,pt1.y))*0.5;
	pt7.x = pt2.x+100*cos(angle);
	pt7.y = pt2.y+100*sin(angle);

	//计算平分线与中线的交点pt8,pt9
	PT_3DEX pt8, pt9;
	double t;
	GraphAPI::GGetLineIntersectLine(pt4.x,pt4.y,pt5.x-pt4.x,pt5.y-pt4.y,
		pt1.x,pt1.y,pt6.x-pt1.x,pt6.y-pt1.y,&pt8.x,&pt8.y,&t);
	pt8.z = pt4.z + (pt5.z-pt4.z)*t;
	GraphAPI::GGetLineIntersectLine(pt4.x,pt4.y,pt5.x-pt4.x,pt5.y-pt4.y,
		pt2.x,pt2.y,pt7.x-pt2.x,pt7.y-pt2.y,&pt9.x,&pt9.y,&t);
	pt9.z = pt4.z + (pt5.z-pt4.z)*t;

	//绘制连线
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&pt0);	
	pBuf->LineTo(&pt8);
	pBuf->LineTo(&pt1);	
	
	pBuf->MoveTo(&pt3);
	pBuf->LineTo(&pt9);
	pBuf->LineTo(&pt2);

	pBuf->MoveTo(&pt8);
	pBuf->LineTo(&pt9);

	pBuf->End();
	
	//绘制边界
	pFt->Draw(pBuf,fDrawScale);
}


CScaleCell::CScaleCell()
{
	m_nType = SYMTYPE_SCALE_Cell;

	m_fX1 = m_fX2 = m_fY1 = 0;
	m_fWidth = 0;
}


CScaleCell::~CScaleCell()
{
	
}


BOOL CScaleCell::IsSupportGeoType(int nGeoType)
{
	if( nGeoType==CLS_GEOCURVE )
		return TRUE;
	return FALSE;
}


void CScaleCell::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if (nIndex == 0)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}

	CellDef cell = cellLib->GetCellDef(nIndex-1);
	
	CGeometry *pGeo = pFt->GetGeometry();

	PT_3DEX pt = pGeo->GetDataPoint(0);	

	float   ftrKx, ftrKy, ftrRotateAngle, ftrWidth, fExtendDis;
	int     nCoverType;

	ftrKx = ftrKy = 1;
	ftrRotateAngle = ftrWidth = fExtendDis = 0;
	nCoverType = 0;

	if (!pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CArray<PT_3DEX,PT_3DEX> basepts;
	pGeo->GetShape(basepts);

	if( basepts.GetSize()<2 || m_fX2-m_fX1<1e-4 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}

	double dis1 = GraphAPI::GGet2DDisOf2P(basepts[0],basepts[1]);
	double dis2 = 0;
	
	if( basepts.GetSize()>=3 )
		dis2 = GraphAPI::GGetDisofPtToLine(basepts[0].x,basepts[0].y,basepts[1].x,basepts[1].y,basepts[2].x,basepts[2].y);

	if( GraphAPI::GIsClockwise(basepts.GetData(),basepts.GetSize())==1 )
	{
		dis2 = -dis2;
	}

	float fkx = dis1/((m_fX2-m_fX1)*fViewScale);
	float fky = fkx;

	if( fabs(dis2)>1e-4 && fabs(m_fY1)>1e-4 )
		fky = dis2/(m_fY1*fViewScale);

	double ang = GraphAPI::GGetAngle(basepts[0].x,basepts[0].y,basepts[1].x,basepts[1].y);

	GrBuffer buf;
	buf.AddBuffer(cell.m_pgr,0);
	buf.SetAllColor(pGeo->GetColor());

	float fWidth = m_fWidth*fDrawScale*fViewScale;
	if( m_fWidth==0.0f )
		fWidth = -fDrawScale*fViewScale;

	buf.SetAllLineWidthOrWidthScale(TRUE,fWidth);

	buf.Zoom(fkx,fky,1.0);
	buf.ZoomPointSize(fkx,fky);
	buf.Rotate(ang);

	PT_3D pt0;
	pt0.x = basepts[0].x + (basepts[1].x-basepts[0].x)*(0-m_fX1)/(m_fX2-m_fX1);
	pt0.y = basepts[0].y + (basepts[1].y-basepts[0].y)*(0-m_fX1)/(m_fX2-m_fX1);
	pt0.z = basepts[0].z + (basepts[1].z-basepts[0].z)*(0-m_fX1)/(m_fX2-m_fX1);

	buf.Move(pt0.x,pt0.y,pt0.z);

	pBuf->AddBuffer(&buf);
}



BOOL CScaleCell::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,TRUE);
	
	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if( nIndex==0 )
		return FALSE;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



void CScaleCell::CopyFrom(CSymbol *pSymbol)
{
    CScaleCell *pScaleCell = (CScaleCell*)pSymbol;
	m_fX1 = pScaleCell->m_fX1;
	m_fX2 = pScaleCell->m_fX2;
	m_fY1 = pScaleCell->m_fY1;
	m_fWidth = pScaleCell->m_fWidth;
	m_strCellDefName = pScaleCell->m_strCellDefName;
	
}



BOOL CScaleCell::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_CELLDEFNAME) )
			{
				data = xmlFile.GetData();
				m_strCellDefName = data;
			}
			
			if( xmlFile.FindElem(XMLTAG_DX) )
			{
				data = xmlFile.GetData();
				m_fX1 = _ttof(data);
			}
						
			if( xmlFile.FindElem(XMLTAG_DX2) )
			{
				data = xmlFile.GetData();
				m_fX2 = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_DY) )
			{
				data = xmlFile.GetData();
				m_fY1 = _ttof(data);
			}			

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}

			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CScaleCell::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			xmlFile.AddElem(XMLTAG_CELLDEFNAME,m_strCellDefName);
			
			data.Format(_T("%f"),m_fX1);
			xmlFile.AddElem(XMLTAG_DX,data);			
			
			data.Format(_T("%f"),m_fX2);
			xmlFile.AddElem(XMLTAG_DX2,data);

			data.Format(_T("%f"),m_fY1);
			xmlFile.AddElem(XMLTAG_DY,data);

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);			
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleCellLinetype::CScaleCellLinetype()
{
	m_nType = SYMTYPE_SCALE_CellLinetype;

	m_fWidth = 0;	
	m_fkx = 1;
	m_fBaseXOffset = m_fBaseYOffset = 0;
	m_fCycle = 0;
	m_fAssistYOffset = 0;
}

CScaleCellLinetype::~CScaleCellLinetype()
{

}



void CScaleCellLinetype::CopyFrom(CSymbol *pSymbol)
{
    CScaleCellLinetype *pCellLine = (CScaleCellLinetype*)pSymbol;
	m_fBaseXOffset = pCellLine->m_fBaseXOffset;
	m_fBaseYOffset = pCellLine->m_fBaseYOffset;
	m_fCycle = pCellLine->m_fCycle;
	m_fkx = pCellLine->m_fkx;
	m_fWidth = pCellLine->m_fWidth;
	m_strCellDefName = pCellLine->m_strCellDefName;
	
}

BOOL CScaleCellLinetype::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_CELLDEFNAME) )
			{
				data = xmlFile.GetData();
				m_strCellDefName = data;
			}
			
			if( xmlFile.FindElem(XMLTAG_KX) )
			{
				data = xmlFile.GetData();
				m_fkx = _ttof(data);
			}
						
			if( xmlFile.FindElem(XMLTAG_BASEXOFF) )
			{
				data = xmlFile.GetData();
				m_fBaseXOffset = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_BASEYOFF) )
			{
				data = xmlFile.GetData();
				m_fBaseYOffset = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_CYCLE) )
			{
				data = xmlFile.GetData();
				m_fCycle = _ttof(data);
			}			

			if( xmlFile.FindElem(XMLTAG_WIDTH) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}


			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CScaleCellLinetype::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			xmlFile.AddElem(XMLTAG_CELLDEFNAME,m_strCellDefName);
			
			data.Format(_T("%f"),m_fkx);
			xmlFile.AddElem(XMLTAG_KX,data);			
			
			data.Format(_T("%f"),m_fBaseXOffset);
			xmlFile.AddElem(XMLTAG_BASEXOFF,data);

			data.Format(_T("%f"),m_fBaseYOffset);
			xmlFile.AddElem(XMLTAG_BASEYOFF,data);
			
			data.Format(_T("%f"),m_fCycle);
			xmlFile.AddElem(XMLTAG_CYCLE,data);

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_WIDTH,data);
			
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

}


void CScaleCellLinetype::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;

	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if (nIndex == 0)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CellDef cell = cellLib->GetCellDef(nIndex-1);
	Envelope e_cell = cell.m_evlp;
	
	BOOL bBreakCell  = m_bBreakCell;

	CGeometry *pGeo = pFt->GetGeometry();

	if( !IsSupportGeoType(pGeo->GetClassType()) || e_cell.Height()<1e-2 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrXoff;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	
	CGeoCurve *pCurve = (CGeoCurve*)pGeo;
	ftrLinetypeScale = pCurve->m_fLinetypeScale;
	ftrLinewidthScale = pCurve->m_fLinewidthScale;
	ftrXoff = pCurve->m_fLinetypeXoff;

	float fWidth = m_fWidth*fDrawScale*fViewScale;

	// 比例线型的比例宽度
	double dScaleWidth = 0;

	CArray<PT_3DEX,PT_3DEX> pts, assistPts, assistPts2;
	int nGeoType = pGeo->GetClassType();

	if (nGeoType == CLS_GEOPARALLEL)
	{
		CGeoParallel *pParallel = (CGeoParallel*)pGeo;
		dScaleWidth = pParallel->GetWidth();
		
		const CShapeLine *pShape =  pParallel->GetShape();
		pShape->GetPts(pts);

		pParallel->GetParallelShape(assistPts,0);

	}
	else if (nGeoType == CLS_GEODCURVE)
	{
		CGeoDCurve *pDCurve = (CGeoDCurve*)pGeo;
		dScaleWidth = pDCurve->GetWidth();
		
		pDCurve->GetBaseShape(pts);
		pDCurve->GetAssistShape(assistPts);	
		ReversePoints(assistPts.GetData(),assistPts.GetSize());
	}	

	int npsum = pts.GetSize();
	
	if( npsum<2 || assistPts.GetSize()<2 ) return;
	
	double xoff0 = m_fBaseXOffset*fDrawScale*fViewScale + ftrXoff;
	
	double yoff0 = m_fBaseYOffset*fDrawScale*fViewScale*(dScaleWidth>0?1:-1);
		
	double yoff1 = m_fAssistYOffset*fDrawScale*fViewScale*(dScaleWidth>0?1:-1);

	double dcycle = m_fCycle*fDrawScale*fViewScale;
	dcycle = fabs(dcycle);

	CArray<PT_3DEX,PT_3DEX> dpt0;
	CArray<int,int> arrIndexs;
	GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dcycle,xoff0,dpt0,&arrIndexs);

	assistPts2.Copy(assistPts);

	if ( !GraphAPI::GGetParallelLine(assistPts.GetData(),assistPts.GetSize(),yoff1,assistPts2.GetData()) ) 
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	int nmin = dpt0.GetSize();
	for (int i=0; i<nmin; i++)
	{
		PT_3DEX pt0, pt1, pt2;
		int idx = arrIndexs[i];
		double dx = pts[idx+1].x-pts[idx].x, dy = pts[idx+1].y-pts[idx].y;
		double dx1 = -dy, dy1 = dx;

		if( dScaleWidth<0 )
		{
			dx1 = dy; dy1 = -dx;
		}

		//计算基于dpt0，方向为(dx1,dy2)的射线与对边assistPts2的相交点
		pt0 = dpt0[i];
		pt1 = pt0;
		pt1.x += dx1, pt1.y += dy1;

		if( !GraphAPI::GGetFirstPointOfRayIntersect(assistPts2.GetData(),assistPts2.GetSize(),pt0,pt1,&pt2,NULL,NULL) )
		{
			continue;
		}

		//对线段pt0-pt2，从头部去掉 yoff0 的长度，从尾部去掉yoff1的长度，中间部分就是图元拉伸后的长度
		double len = GraphAPI::GGet2DDisOf2P(pt0,pt2);
		if( len<1e-4 )
			continue;

		double len1 = len-yoff0-yoff1;
		if( len1<1e-4 )
			continue;
		
		pt1.x = pt0.x + yoff0/len * (pt2.x-pt0.x);
		pt1.y = pt0.y + yoff0/len * (pt2.y-pt0.y);
		pt1.z = pt0.z + yoff0/len * (pt2.z-pt0.z);

		double ky = len1/e_cell.Height();

		double ftrKx = m_fkx*fDrawScale*fViewScale;
		double angle = GraphAPI::GGetAngle(0,0,dx1,dy1)-PI/2;

		DrawCellToBuf(pBuf,nIndex,&cell,&pt1,ftrKx,ky,angle,pGeo->GetColor(),m_fWidth,m_bBreakCell,FALSE,0,0);
	
 	}
	
}



BOOL CScaleCellLinetype::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,FALSE);
	
	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if( nIndex==0 )
		return FALSE;
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



BOOL CScaleCellLinetype::IsSupportGeoType(int nGeoType)
{
	if( nGeoType == CLS_GEOPARALLEL || nGeoType==CLS_GEODCURVE )
		return TRUE;
	return FALSE;
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleOldDouya::CScaleOldDouya()
{
	m_nType = SYMTYPE_SCALE_OldDouya;

	m_fMaxToothLen = 2.0f;	
	m_fInterval = 1.0f;
	m_fWidth = 0;
	m_fCycle = 3.0f;
}

CScaleOldDouya::~CScaleOldDouya()
{

}



void CScaleOldDouya::CopyFrom(CSymbol *pSymbol)
{
    CScaleOldDouya *p = (CScaleOldDouya*)pSymbol;
	m_fCycle = p->m_fCycle;
	m_fMaxToothLen = p->m_fMaxToothLen;
	m_fInterval = p->m_fInterval;
	m_fWidth = p->m_fWidth;	
}



BOOL CScaleOldDouya::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			if( xmlFile.FindElem(XMLTAG_CYCLE) )
			{
				data = xmlFile.GetData();
				m_fCycle = _ttof(data);
			}			
			if( xmlFile.FindElem(XMLTAG_MaxToothLen) )
			{
				data = xmlFile.GetData();
				m_fMaxToothLen = _ttof(data);
			}
			
			if( xmlFile.FindElem(XMLTAG_Interval) )
			{
				data = xmlFile.GetData();
				m_fInterval = _ttof(data);
			}
						
			if( xmlFile.FindElem(XMLTAG_LineWidth) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CScaleOldDouya::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();	
			
			data.Format(_T("%f"),m_fCycle);
			xmlFile.AddElem(XMLTAG_CYCLE,data);

			data.Format(_T("%f"),m_fMaxToothLen);
			xmlFile.AddElem(XMLTAG_MaxToothLen,data);
			
			data.Format(_T("%f"),m_fInterval);
			xmlFile.AddElem(XMLTAG_Interval,data);			
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_LineWidth,data);			
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

}


void CScaleOldDouya::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();

	if( !IsSupportGeoType(pGeo->GetClassType()) )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	float   ftrLinetypeScale, ftrLinewidthScale;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	
	CGeoCurve *pCurve = (CGeoCurve*)pGeo;
	ftrLinetypeScale = pCurve->m_fLinetypeScale;
	ftrLinewidthScale = pCurve->m_fLinewidthScale;

	float fWidth = m_fWidth*fDrawScale*fViewScale;

	// 比例线型的比例宽度
	double dScaleWidth = 0;

	CArray<PT_3DEX,PT_3DEX> pts, assistPts, assistPts2;
	int nGeoType = pGeo->GetClassType();

	if (nGeoType == CLS_GEOPARALLEL)
	{
		CGeoParallel *pParallel = (CGeoParallel*)pGeo;
		dScaleWidth = pParallel->GetWidth();
		
		const CShapeLine *pShape =  pParallel->GetShape();
		pShape->GetPts(pts);

		pParallel->GetParallelShape(assistPts,0);

	}
	else if (nGeoType == CLS_GEODCURVE)
	{
		CGeoDCurve *pDCurve = (CGeoDCurve*)pGeo;
		dScaleWidth = pDCurve->GetWidth();
		
		pDCurve->GetBaseShape(pts);
		pDCurve->GetAssistShape(assistPts);		
		ReversePoints(assistPts.GetData(),assistPts.GetSize());
	}	

	int npsum = GraphAPI::GKickoffSame2DPoints(pts.GetData(),pts.GetSize());
	pts.SetSize(npsum);
	
	if( npsum<2 || assistPts.GetSize()<2 ) return;	

	double dcycle = m_fCycle*fDrawScale*fViewScale;
	dcycle = fabs(dcycle);

	double xoff0 = dcycle*1.0;

	double fInterval = m_fInterval*fDrawScale*fViewScale;
	double fMaxTooth = m_fMaxToothLen*fDrawScale*fViewScale;

	CArray<PT_3DEX,PT_3DEX> dpt0;
	CArray<int,int> arrIndexs;
	GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dcycle,xoff0,dpt0,&arrIndexs);

	assistPts2.Copy(assistPts);

	pBuf->BeginLineString(pGeo->GetColor(),fWidth,TRUE,0,fDrawScale*fViewScale);
	
	int nmin = dpt0.GetSize();
	for (int i=0; i<nmin; i++)
	{
		PT_3DEX pt0, pt1, pt2;
		int idx = arrIndexs[i];
		double dx = pts[idx+1].x-pts[idx].x, dy = pts[idx+1].y-pts[idx].y;
		double dx1 = -dy, dy1 = dx;
		double dis = sqrt(dx*dx + dy*dy);
		if( dis<1e-6 )
			continue;

		if( dScaleWidth<0 )
		{
			dx1 = dy; dy1 = -dx;
		}

		//计算基于dpt0，方向为(dx1,dy1)的射线与对边assistPts2的相交点
		pt0 = dpt0[i];
		pt1 = pt0;
		pt1.x += dx1, pt1.y += dy1;

		if( !GraphAPI::GGetFirstPointOfRayIntersect(assistPts2.GetData(),assistPts2.GetSize(),pt0,pt1,&pt2,NULL,NULL) )
		{
			continue;
		}

		//在pt0-pt2上画齿，齿的方向与线的前进方向平行且相反
		pBuf->MoveTo(&pt0);
		pBuf->LineTo(&pt2);

		dx /= dis, dy /= dis;
		dx1 /= dis, dy1 /= dis;

		dx = -dx;  dy = -dy;

		double dz = pt2.z-pt0.z;
		double len = GraphAPI::GGet2DDisOf2P(pt0,pt2);
		double tooth = fMaxTooth;
		double cur_len = fInterval;
		for( int j=1; cur_len<len; cur_len+=fInterval, j++ )
		{
			pt1 = pt0;
			pt1.x += cur_len*dx1;
			pt1.y += cur_len*dy1;
			pt1.z += cur_len/len * dz;

			//用函数 y = 1/x (1<=x<=N)来表示齿长度的递减规律
			tooth = fMaxTooth/j;

			//控制长度
			if( j>4 )
				tooth = fMaxTooth/4;

			if( j==2 )
				tooth = fMaxTooth*0.67;

			pt2 = pt1;
			pt2.x += tooth*dx;
			pt2.y += tooth*dy;

			pBuf->MoveTo(&pt1);
			pBuf->LineTo(&pt2);
		}
 	}
	
	pBuf->End();

}



BOOL CScaleOldDouya::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



BOOL CScaleOldDouya::IsSupportGeoType(int nGeoType)
{
	if( nGeoType == CLS_GEOPARALLEL || nGeoType==CLS_GEODCURVE )
		return TRUE;
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScaleXiepo::CScaleXiepo()
{
	m_nType = SYMTYPE_SCALE_Xiepo;

	m_fToothLen = 1.0f;	
	m_fInterval = 2.0f;
	m_fWidth = 0;
	m_bFasten = FALSE;
	m_fPointSize = 0.05f;
	m_fPointInterval = 0.8f;
	m_fBaseYOffset = 0.0f;
	m_bAverageDraw = TRUE;
}

CScaleXiepo::~CScaleXiepo()
{

}



void CScaleXiepo::CopyFrom(CSymbol *pSymbol)
{
    CScaleXiepo *p = (CScaleXiepo*)pSymbol;
	m_fToothLen = p->m_fToothLen;
	m_fInterval = p->m_fInterval;
	m_fWidth = p->m_fWidth;	
	m_bFasten = p->m_bFasten;
	m_fPointSize = p->m_fPointSize;
	m_fPointInterval = p->m_fPointInterval;
	m_bAverageDraw = p->m_bAverageDraw;
	m_fBaseYOffset = p->m_fBaseYOffset;
}



BOOL CScaleXiepo::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

			if( xmlFile.FindElem(XMLTAG_Interval) )
			{
				data = xmlFile.GetData();
				m_fInterval = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_ToothLen) )
			{
				data = xmlFile.GetData();
				m_fToothLen = _ttof(data);
			}			
						
			if( xmlFile.FindElem(XMLTAG_LineWidth) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}			
			
			if( xmlFile.FindElem(XMLTAG_Fasten) )
			{
				data = xmlFile.GetData();
				m_bFasten = _ttoi(data);
			}
			if( xmlFile.FindElem(XMLTAG_PointSize) )
			{
				data = xmlFile.GetData();
				m_fPointSize = _ttof(data);
			}
			if( xmlFile.FindElem(XMLTAG_PointInterval) )
			{
				data = xmlFile.GetData();
				m_fPointInterval = _ttof(data);
			}
			if( xmlFile.FindElem(XMLTAG_AVERAGEDRAW) )
			{
				data = xmlFile.GetData();
				m_bAverageDraw = _ttoi(data);
			}
			if( xmlFile.FindElem(XMLTAG_BASEYOFF) )
			{
				data = xmlFile.GetData();
				m_fBaseYOffset = _ttof(data);
			}
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CScaleXiepo::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();	
			
			data.Format(_T("%f"),m_fInterval);
			xmlFile.AddElem(XMLTAG_Interval,data);			
			
			data.Format(_T("%f"),m_fToothLen);
			xmlFile.AddElem(XMLTAG_ToothLen,data);
			
			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_LineWidth,data);		
			
			data.Format(_T("%d"),m_bFasten);
			xmlFile.AddElem(XMLTAG_Fasten,data);	

			data.Format(_T("%f"),m_fPointSize);
			xmlFile.AddElem(XMLTAG_PointSize,data);	

			data.Format(_T("%f"),m_fPointInterval);
			xmlFile.AddElem(XMLTAG_PointInterval,data);	

			data.Format(_T("%d"),m_bAverageDraw);
			xmlFile.AddElem(XMLTAG_AVERAGEDRAW,data);

			data.Format(_T("%f"),m_fBaseYOffset);
			xmlFile.AddElem(XMLTAG_BASEYOFF,data);	
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

}


void CScaleXiepo::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CGeometry *pGeo = pFt->GetGeometry();

	if( !IsSupportGeoType(pGeo->GetClassType()) )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	float   ftrLinewidthScale = 1, ftrWid;
		
	CGeoCurve *pCurve = (CGeoCurve*)pGeo;
	ftrLinewidthScale = pCurve->m_fLinewidthScale;
	ftrWid = pCurve->m_fLineWidth;
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}

	// 比例线型的比例宽度
	double dScaleWidth = 0;

	CArray<PT_3DEX,PT_3DEX> pts, assistPts;
	int nGeoType = pGeo->GetClassType();

	if (nGeoType == CLS_GEOPARALLEL)
	{
		CGeoParallel *pParallel = (CGeoParallel*)pGeo;
		dScaleWidth = pParallel->GetWidth();
		
		const CShapeLine *pShape =  pParallel->GetShape();
		pShape->GetPts(pts);

		pParallel->GetParallelShape(assistPts,0);

	}
	else if (nGeoType == CLS_GEODCURVE)
	{
		CGeoDCurve *pDCurve = (CGeoDCurve*)pGeo;
		dScaleWidth = pDCurve->GetWidth();
		
		pDCurve->GetBaseShape(pts);
		pDCurve->GetAssistShape(assistPts);		
		ReversePoints(assistPts.GetData(),assistPts.GetSize());
	}	

	if( m_fBaseYOffset!=0.0f )
	{
		float fBaseYOffset = m_fBaseYOffset*fDrawScale*fViewScale;
		if( dScaleWidth<0 )
		{
			fBaseYOffset = -fBaseYOffset;
		}

		CArray<PT_3DEX,PT_3DEX> pts2;
		pts2.Copy(pts);
		GraphAPI::GGetParallelLine(pts2.GetData(),pts2.GetSize(),fBaseYOffset,pts.GetData());
	}

	int npsum = GraphAPI::GKickoffSame2DPoints(pts.GetData(),pts.GetSize());
	pts.SetSize(npsum);
	
	if( npsum<2 || assistPts.GetSize()<2 ) return;

	double fInterval = m_fInterval*fDrawScale*fViewScale;
	double fToothLen = m_fToothLen*fDrawScale*fViewScale;
	fInterval = fabs(fInterval);
	double fPointInterval = m_fPointInterval*fDrawScale*fViewScale;

	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,0,fDrawScale*fViewScale);

	CArray<PT_3D,PT_3D> arrPoints;
	{
		double dcycle = fInterval;
		
		dcycle = fabs(dcycle);
		if (dcycle < 1e-4)	return;

		DWORD nColor = pGeo->GetColor();

		double xoff0 = dcycle;
		
		double yoff0 = 0;
		
		double xoff1 = dcycle;
		
		double yoff1 = dScaleWidth;

		//长短齿交替
		int nToothNumber = 0;
		
		// 1,双线改善, 首点均匀过渡到尾点
		// 2,对平行线从母线拉到辅助线的情况也均匀过渡
		if (m_bAverageDraw )
		{
			int nlastIndex0 = -1, nlastIndex1 = -1, i, j;
			for ( i=0; i<pts.GetSize(); i++)
			{
				if (pts[i].type == ptSpecial)
				{
					for ( j=nlastIndex1+1; j<assistPts.GetSize(); j++)
					{
						if (assistPts[j].type == ptSpecial)
						{
							// 对应特征点处理
							CArray<PT_3DEX,PT_3DEX> basepts1, asspts1;
							int istart = nlastIndex0>=0?nlastIndex0:0, k;
							for ( k=istart; k<=i; k++)
							{
								basepts1.Add(pts[k]);
							}

							istart = nlastIndex1>=0?nlastIndex1:0;
							for ( k=istart; k<=j; k++)
							{
								asspts1.Add(assistPts[k]);
							}

							double xoff10 = xoff0, xoff11 = xoff1;
							if( istart>0 )
								xoff10 = 0, xoff11 = 0;

							int npsum1 = basepts1.GetSize();

							// 将母线和辅助线分成等距的线段，然后连接对应的点
							double len0, len1;
							len0 = GraphAPI::GGetAllLen3D(basepts1.GetData(),basepts1.GetSize())-fabs(xoff10);
							len1 = GraphAPI::GGetAllLen3D(asspts1.GetData(),asspts1.GetSize())-fabs(xoff11);
							
							int numLine = floor(len0/dcycle+0.5);
							if( istart>0 )
								numLine = (numLine/2)*2;
							else
								numLine = (numLine/2)*2+1;
							
							double dcycle0, dcycle1;
							
							// 对周期作调整，保证最后连接线与前面间距一样
							dcycle0 = len0/numLine;
							dcycle1 = len1/numLine;
							
							CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
							
							GraphAPI::GGetCycleLine(basepts1.GetData(),basepts1.GetSize(),dcycle0,fabs(xoff10),dpt0,0);
							GraphAPI::GGetCycleLine(asspts1.GetData(),asspts1.GetSize(),dcycle1,fabs(xoff11),dpt1,0);
							
// 							if (ceil(len0/dcycle) == floor(len0/dcycle)+1 && fabs(xoff0) <= 1e-4)
// 							{
// 								int assPtSize = asspts1.GetSize();
// 								dpt0.Add(basepts1[npsum1-1]);
// 								dpt1.Add(asspts1[assPtSize-1]);
// 							}			
							
							int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
							if( nmin>numLine )nmin = numLine;

							for (int i=0; i<nmin; i++)
							{
								//短齿
								if( (nToothNumber%2)==0 )
								{
									if( fToothLen>0 )
									{
										PT_3DEX expt;
										GGetPtOfLine(dpt0[i],dpt1[i],fToothLen,&expt);
										
										pBuf->MoveTo(&dpt0[i]);
										pBuf->LineTo(&expt);
										
										if( m_bFasten )
										{
											GGetPtOfLine(dpt0[i],dpt1[i],fToothLen+fPointInterval,&expt);
											arrPoints.Add(expt);
										}
									}
								}
								//长齿
								else
								{
									pBuf->MoveTo(&dpt0[i]);
									pBuf->LineTo(&dpt1[i]);	
								}								
								nToothNumber++;
							}

							nlastIndex1 = j;

							break;
						}
					}

					if (j > assistPts.GetSize()-1)
					{
						break;
					}

					nlastIndex0 = i;
				}
				
			}

			// 整体无效
			if (nlastIndex0 < 0 || nlastIndex1 < 0)
			{
				// 将母线和辅助线分成等距的线段，然后连接对应的点
				double len0, len1;
				len0 = GraphAPI::GGetAllLen3D(pts.GetData(),pts.GetSize())-fabs(xoff0);
				len1 = GraphAPI::GGetAllLen3D(assistPts.GetData(),assistPts.GetSize())-fabs(xoff1);
				
				int numLine = floor(len0/dcycle+0.5);
				numLine = (numLine/2)*2 + 1;
				
				double dcycle0, dcycle1;
				
				// 对周期作调整，保证最后连接线与前面间距一样
				dcycle0 = len0/numLine;
				dcycle1 = len1/numLine;
				
				CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
				
				GraphAPI::GGetCycleLine(pts.GetData(),pts.GetSize(),dcycle0,fabs(xoff0),dpt0,0);
				GraphAPI::GGetCycleLine(assistPts.GetData(),assistPts.GetSize(),dcycle1,fabs(xoff1),dpt1,0);
				
				//if (ceil(len0/dcycle) == floor(len0/dcycle)+1 && fabs(xoff0) <= 1e-4)
// 				{
// 					int assPtSize = assistPts.GetSize();
// 					dpt0.Add(pts[npsum-1]);
// 					dpt1.Add(assistPts[assPtSize-1]);
// 				}			
				
				int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
				if( nmin>numLine )nmin = numLine;

				for (int i=0; i<nmin; i++)
				{
					//短齿
					if( (nToothNumber%2)==0 )
					{
						if( fToothLen>0 )
						{
							PT_3DEX expt;
							GGetPtOfLine(dpt0[i],dpt1[i],fToothLen,&expt);
							
							pBuf->MoveTo(&dpt0[i]);
							pBuf->LineTo(&expt);
							
							if( m_bFasten )
							{
								GGetPtOfLine(dpt0[i],dpt1[i],fToothLen+fPointInterval,&expt);
								arrPoints.Add(expt);
							}
						}

					}
					//长齿
					else
					{
						pBuf->MoveTo(&dpt0[i]);
						pBuf->LineTo(&dpt1[i]);	
					}
					
					nToothNumber++;
				}
			}
			// 最后部分处理
			else
			{
				// 对应特征点处理
				CArray<PT_3DEX,PT_3DEX> basepts1, asspts1;
				int istart = nlastIndex0>=0?nlastIndex0:0, k;
				for ( k=istart; k<pts.GetSize(); k++)
				{
					basepts1.Add(pts[k]);
				}
				
				istart = nlastIndex1>=0?nlastIndex1:0;
				for ( k=istart; k<assistPts.GetSize(); k++)
				{
					asspts1.Add(assistPts[k]);
				}
				
				int npsum1 = basepts1.GetSize();
				
				// 将母线和辅助线分成等距的线段，然后连接对应的点
				double len0, len1;
				len0 = GraphAPI::GGetAllLen3D(basepts1.GetData(),basepts1.GetSize());
				len1 = GraphAPI::GGetAllLen3D(asspts1.GetData(),asspts1.GetSize());
				
				int numLine = floor(len0/dcycle + 0.5);
				numLine = (numLine/2)*2;
				
				double dcycle0, dcycle1;
				
				// 对周期作调整，保证最后连接线与前面间距一样
				dcycle0 = len0/numLine;
				dcycle1 = len1/numLine;
				
				CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
				
				GraphAPI::GGetCycleLine(basepts1.GetData(),basepts1.GetSize(),dcycle0,0,dpt0,0);
				GraphAPI::GGetCycleLine(asspts1.GetData(),asspts1.GetSize(),dcycle1,0,dpt1,0);
				
				if (ceil(len0/dcycle) == floor(len0/dcycle)+1 && fabs(xoff0) <= 1e-4)
				{
					int assPtSize = asspts1.GetSize();
					dpt0.Add(basepts1[npsum1-1]);
					dpt1.Add(asspts1[assPtSize-1]);
				}			
				
				int nmin = dpt0.GetSize()<dpt1.GetSize()?dpt0.GetSize():dpt1.GetSize();
				if( nmin>numLine )nmin = numLine;
				for (int i=0; i<nmin; i++)
				{
					//短齿
					if( (nToothNumber%2)==0 )
					{
						if( fToothLen>0 )
						{
							PT_3DEX expt;
							GGetPtOfLine(dpt0[i],dpt1[i],fToothLen,&expt);
							
							pBuf->MoveTo(&dpt0[i]);
							pBuf->LineTo(&expt);
							
							if( m_bFasten )
							{
								GGetPtOfLine(dpt0[i],dpt1[i],fToothLen+fPointInterval,&expt);
								arrPoints.Add(expt);
							}
						}
					}
					//长齿
					else
					{
						pBuf->MoveTo(&dpt0[i]);
						pBuf->LineTo(&dpt1[i]);	
					}
					
					nToothNumber++;					
				}
			}

		}
		else
		{
			// 对于从基线拉到辅助线的情况，依比例线形的方向垂直于基线
			{
				CArray<PtIntersect,PtIntersect> arrIntersectPts;

				CUIntArray arrIdx;
				CArray<PT_3DEX,PT_3DEX> dpt0, dpt1;
				GetCycleLine(pts.GetData(),pts.GetSize(),dcycle,xoff0,dpt0,arrIdx);
				
				// 垂直于母线与辅助线有交点的直线
				int num0 = dpt0.GetSize();
				for (int i=0; i<num0; i++) 
				{
					PT_3DEX ptIntersect, ptAnother;
					double fRatio;
					GGetPtFromLineWidth(pts.GetData(), pts.GetSize(),dpt0.GetAt(i),yoff1, &ptAnother);
					BOOL bIntersect = GraphAPI::GGetFirstPointOfRayIntersect(assistPts.GetData(),assistPts.GetSize(),dpt0.GetAt(i),ptAnother,&ptIntersect,&fRatio,NULL);
					if(!bIntersect) continue;
					int indexNest = -1;
					PT_3DEX ptIntersectNest;
					GraphAPI::GGetNearstDisPToPointSet2D(assistPts.GetData(),assistPts.GetSize(),dpt0.GetAt(i),ptIntersectNest,&indexNest);
				
					PtIntersect pt0;

					pt0.idx = i;
					pt0.pt1 = dpt0[i];					

					pt0.lfRatio1 = arrIdx[i];

					if (!bIntersect /*|| GraphAPI::GGet2DDisOf2P(dpt0[i],ptIntersect) > 1.5*GraphAPI::GGet2DDisOf2P(dpt0[i],ptIntersectNest)*/ ) 
					{
						pt0.pt2 = ptIntersectNest;
						pt0.lfRatio2 = indexNest;
					}
					else
					{
						pt0.pt2 = ptIntersect;
						pt0.lfRatio2 = fRatio;
					}
					
					arrIntersectPts.Add(pt0);
				}

				// 删除arrLine中相互有交点的直线以及不合理的直线（间距太大或太小）
/*				int intersectNum = arrIntersectPts.GetSize();
				CArray<int,int> arrBadFlags;
				arrBadFlags.SetSize(intersectNum);

				for (i=0; i<intersectNum; i++)
				{
					arrBadFlags[i] = 0;

					//跳过起点和短齿
					if( (i%2)==0 )
					{
						continue;
					}

					for (int j=0; j<i; j++)
					{
						if( (j%2)==0 )
							continue;
						if( arrBadFlags[j]==1 )
							continue;

						if (GraphAPI::GGetLineIntersectLineSeg(arrIntersectPts[i].pt1.x,arrIntersectPts[i].pt1.y,
							arrIntersectPts[i].pt2.x,arrIntersectPts[i].pt2.y,
							arrIntersectPts[j].pt1.x,arrIntersectPts[j].pt1.y,
							arrIntersectPts[j].pt2.x,arrIntersectPts[j].pt2.y,NULL,NULL,NULL))
						{
							arrBadFlags[i] = 1;
							break;
						}
					}
				}

				// 调整错误的地方，使其均匀过渡
				intersectNum = arrIntersectPts.GetSize();
				for (i=1; i<intersectNum; i++)
				{
					if( (i%2)==0 )
						continue;

					if( arrBadFlags[i]==0 )
						continue;

					for( int j=i+1; j<intersectNum; j++)
					{
						if( (j%2)==0 )
							continue;

						if( arrBadFlags[j]!=1 )
							break;
					}

					int k = j;

					double s0 = 0, s1 = 0, e0 = 0, e1 = 0;
					if (k==intersectNum)
					{
						if (arrIntersectPts[i].idx != num0-1)
						{ 
							s0 = arrIntersectPts[i].lfRatio1;
							e0 = npsum - 1;
							s1 = arrIntersectPts[i].lfRatio2;
							e1 = assistPts.GetSize() - 1;
						}
						else continue;
						
					}
					else
					{
						s0 = arrIntersectPts[i].lfRatio1;
						e0 = arrIntersectPts[k].lfRatio1;
						s1 = arrIntersectPts[i].lfRatio2;
						e1 = arrIntersectPts[k].lfRatio2;
					}

					{
						CArray<PT_3DEX,PT_3DEX> pts2;

						pts2.Add(PT_3DEX(arrIntersectPts[i].pt2,penLine));
						for (j=s1+1; j<=e1; j++)
						{
							pts2.Add(assistPts[j]);
						}
						if (k==intersectNum)
						{
							pts2.Add(assistPts[assistPts.GetSize()-1]);
						}
						else
						{
							pts2.Add(PT_3DEX(arrIntersectPts[k].pt2,penLine));
						}

						// 将母线和辅助线分成等距的线段，然后连接对应的点
						double len1;
						len1 = GraphAPI::GGetAllLen3D(pts2.GetData(),pts2.GetSize());
						
						int numLine = 1 + k-i;
						
						double dcycle1;	
						dcycle1 = len1/numLine;
						
						CArray<PT_3DEX,PT_3DEX> dpt1;
						
						GraphAPI::GGetCycleLine(pts2.GetData(),pts2.GetSize(),dcycle1,0,dpt1,0);

						for (j=0; j<numLine-1; j++)
						{
							if( ((i+j)%2)==0 )continue;

							//arrIntersectPts[i+j].pt2 = dpt1[j];
						}

					}
				}*/

				int intersectNum = arrIntersectPts.GetSize();
				for (i=0; i<intersectNum; i++)
				{
					PtIntersect pt0 = arrIntersectPts[i];

					//短齿
					if( (nToothNumber%2)==0 )
					{
						if( fToothLen>0 )
						{
							PT_3D expt;
							GGetPtOfLine(pt0.pt1,pt0.pt2,fToothLen,&expt);
							
							pBuf->MoveTo(&pt0.pt1);
							pBuf->LineTo(&expt);
							
							if( m_bFasten )
							{
								GGetPtOfLine(pt0.pt1,pt0.pt2,fToothLen+fPointInterval,&expt);
								arrPoints.Add(expt);
							}
						}

					}
					//长齿
					else
					{
						pBuf->MoveTo(&pt0.pt1);
						pBuf->LineTo(&pt0.pt2);	
					}

					nToothNumber++;	

				}
			}				
		}	
	}

	pBuf->End();

	if( m_bFasten && arrPoints.GetSize()>0 )
	{
		double kx = m_fPointSize*fDrawScale*fViewScale;
		pBuf->BeginPointString(pGeo->GetColor(),kx,kx,TRUE,0,0);
		for( int i=0; i<arrPoints.GetSize(); i++)
		{
			pBuf->PointString(&arrPoints[i],0);
		}
		pBuf->End();
	}

}



BOOL CScaleXiepo::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



BOOL CScaleXiepo::IsSupportGeoType(int nGeoType)
{
	if( nGeoType == CLS_GEOPARALLEL || nGeoType==CLS_GEODCURVE )
		return TRUE;
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTidalWaterSymbol::CTidalWaterSymbol()
{
	m_nType = SYMTYPE_TIDALWATER;

	m_fInterval = 2.0f;
	m_fkxMin = 1.0f;
	m_fkxMax = 1.0f;

	m_fWidth = 0;
}

CTidalWaterSymbol::~CTidalWaterSymbol()
{

}



void CTidalWaterSymbol::CopyFrom(CSymbol *pSymbol)
{
    CTidalWaterSymbol *p = (CTidalWaterSymbol*)pSymbol;
	m_strCellDefName = p->m_strCellDefName;
	m_fInterval = p->m_fInterval;
	m_fkxMin = p->m_fkxMin;	
	m_fkxMax = p->m_fkxMax;
	m_fWidth = p->m_fWidth;
}



BOOL CTidalWaterSymbol::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if( xmlFile.FindElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();
			
			if( xmlFile.FindElem(XMLTAG_CELLDEFNAME) )
			{
				data = xmlFile.GetData();
				m_strCellDefName = data;
			}
			
			if( xmlFile.FindElem(XMLTAG_Interval) )
			{
				data = xmlFile.GetData();
				m_fInterval = _ttof(data);
			}

			if( xmlFile.FindElem(XMLTAG_KXMIN) )
			{
				data = xmlFile.GetData();
				m_fkxMin = _ttof(data);
			}			
						
			if( xmlFile.FindElem(XMLTAG_KXMAX) )
			{
				data = xmlFile.GetData();
				m_fkxMax = _ttof(data);
			}	
			
			if( xmlFile.FindElem(XMLTAG_LineWidth) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}	

			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;

}

void CTidalWaterSymbol::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();	

			xmlFile.AddElem(XMLTAG_CELLDEFNAME,m_strCellDefName);
			
			data.Format(_T("%f"),m_fInterval);
			xmlFile.AddElem(XMLTAG_Interval,data);				
			
			data.Format(_T("%f"),m_fkxMin);
			xmlFile.AddElem(XMLTAG_KXMIN,data);	
			
			data.Format(_T("%f"),m_fkxMax);
			xmlFile.AddElem(XMLTAG_KXMAX,data);	

			data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_LineWidth,data);	
			
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}

	strXML = xmlFile.GetDoc();

}


void CTidalWaterSymbol::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if (pFt==NULL || pBuf==NULL) return;
	
	CCellDefLib *cellLib = GetCellDefLib();
	int nIndex = cellLib->GetCellDefIndex(m_strCellDefName) + 1;
	
	if (nIndex == 0)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CellDef cell = cellLib->GetCellDef(nIndex-1);
	
	BOOL bBreakCell  = m_bBreakCell;

	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrXoff = 0;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	
	if (pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)))
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrXoff = pCurve->m_fLinetypeXoff;
	}	

	float fWidth = m_fWidth*fDrawScale*fViewScale;
	if( fWidth==0.0f )
		fWidth = -fDrawScale*fViewScale;

	int nGeoType = pGeo->GetClassType();
	switch(nGeoType)
	{
	case CLS_GEOCURVE:
	case CLS_GEODCURVE:
	case CLS_GEOPARALLEL:
	case CLS_GEOSURFACE:
	case CLS_GEOMULTISURFACE:
		{
			// 平行线宽度方向
			float fwid = 1;
			CArray<PT_3DEX,PT_3DEX> basepts;
			GetBasePtsAndWidth(pGeo,basepts,fwid);

			SimpleCompressPoints(basepts,CSymbol::m_tolerance);			

			if (basepts.GetSize() < 1)
				break;
	
			CArray<PT_3DEX,PT_3DEX> dpts0;				
			dpts0.Copy(basepts);

			{				
				GrBuffer grbuf;

				CArray<PT_3DEX,PT_3DEX> dpts;
				CUIntArray arrIndex;

				GetCycleLine(dpts0.GetData(),dpts0.GetSize(),m_fInterval*ftrLinetypeScale*fDrawScale*fViewScale,ftrXoff,dpts,arrIndex);

				if( dpts.GetSize()<=0 )
					break;
				
				CArray<double,double> angles;
				CalcAngles(dpts0,angles);
				
				double ftrKxMax = m_fkxMax*ftrLinetypeScale*fDrawScale*fViewScale;
				double ftrKxMin = m_fkxMin*ftrLinetypeScale*fDrawScale*fViewScale;
				
				double step = (ftrKxMax-ftrKxMin)/dpts.GetSize();
				double kx = ftrKxMax;
				
				for(int i=0; i<dpts.GetSize(); i++)
				{
					int index = arrIndex.GetAt(i);
					double angle = angles[index];						
					
					double resAngle = angle-fRotAngle;
					if (fwid < 0)  
						resAngle = PI+angle-fRotAngle;
					
					DrawCellToBuf(&grbuf,nIndex,&cell,&PT_3D(dpts[i].x,dpts[i].y,dpts[i].z),
						kx,kx,resAngle,pGeo->GetColor(),fWidth,bBreakCell,FALSE,0,0);

					kx -= step;
				}

				if (bBreakCell)
				{
					grbuf.SetAllColor(pGeo->GetColor());
				}
				
				pBuf->AddBuffer(&grbuf);
				
			}
			
			break;
			
		}
		
	default:
		{
			pFt->Draw(pBuf,fDrawScale);
			break;
		}
	}
}



BOOL CTidalWaterSymbol::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}



BOOL CTidalWaterSymbol::IsSupportGeoType(int nGeoType)
{
	if( nGeoType == CLS_GEOCURVE || nGeoType==CLS_GEOSURFACE )
		return TRUE;
	return FALSE;
}



/////////////////////////////////////////////////////////////
////   CCulvertSurface1Symbol
///////////////////////////////////////////////////////////////
void CCulvertSurface1Symbol::CopyFrom(CSymbol *pSymbol)
{
    CCulvertSurface1Symbol *p = (CCulvertSurface1Symbol*)(pSymbol);
	if( p )
	{
		m_strLinetypeName = p->m_strLinetypeName;
		m_fWidth = p->m_fWidth;
		m_nType = p->m_nType;
		m_strName = p->m_strName;
	}
	CSymbol::CopyFrom(pSymbol);
}

BOOL CCulvertSurface1Symbol::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
    CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	
	
	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CCulvertSurface1Symbol::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if(xmlFile.FindElem(XMLTAG_SYMBOL))
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

            if( xmlFile.FindElem(XMLTAG_NAME) )
			{
				data = xmlFile.GetData();
				m_strName = data;
			}

			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strLinetypeName = data;
			}

			if( xmlFile.FindElem(XMLTAG_LineWidth) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}

			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
}

void CCulvertSurface1Symbol::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();	
			
			xmlFile.AddElem(XMLTAG_NAME, m_strName);

			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strLinetypeName);

            data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_LineWidth,data);	
				
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

BOOL CCulvertSurface1Symbol::IsSupportGeoType(int nGeoType)
{
	if(nGeoType==CLS_GEOSURFACE || nGeoType == CLS_GEOCURVE)
		return TRUE;
	return FALSE;
}

void CCulvertSurface1Symbol::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
    if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}	

	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if(pGeo->GetClassType()==CLS_GEOSURFACE)
	{
		CGeoSurface *pCurve = (CGeoSurface*)pGeo;
		ftrWid = pCurve->m_fLineWidth;		
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	
	if( arrPts.GetSize()<5 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();

	int count_first = ceil(double((arrPts.GetSize()-3))/2.0 - 0.1);
	int count_second = floor(double((arrPts.GetSize()-3))/2.0 + 0.1);
    
	int i=0;

	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strLinetypeName) + 1;
    pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&arrPts[i]);
	for(i=1; i<=count_first; ++i)
	{
		pBuf->LineTo(&arrPts[i]);
	}
	pBuf->End();

	nIndex = baselineLib->GetBaseLineTypeIndex(m_strLinetypeName) + 1;
    pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&arrPts[i]);
	++i;
	for(; i<=count_first+1+count_second; ++i)
	{
		pBuf->LineTo(&arrPts[i]);
	}
	pBuf->End();
}

/////////////////////////////////////////////////////////////
////  CCulvertSurface2Symbol
///////////////////////////////////////////////////////////////
void CCulvertSurface2Symbol::CopyFrom(CSymbol *pSymbol)
{
    CCulvertSurface2Symbol *p = (CCulvertSurface2Symbol*)(pSymbol);
	if( p )
	{
		m_strLinetypeName = p->m_strLinetypeName;
		m_fWidth = p->m_fWidth;
		m_nType = p->m_nType;
		m_strName = p->m_strName;
	}
	CSymbol::CopyFrom(pSymbol);
}

BOOL CCulvertSurface2Symbol::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
    CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakLinetype,FALSE);
	
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	
	
	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CCulvertSurface2Symbol::Load(CString& strXML)
{
	CMarkup xmlFile;
	xmlFile.SetDoc(strXML);
	CString data;
	if(xmlFile.FindElem(XMLTAG_SYMBOL))
	{
		xmlFile.IntoElem();
		
		if( xmlFile.FindElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();

            if( xmlFile.FindElem(XMLTAG_NAME) )
			{
				data = xmlFile.GetData();
				m_strName = data;
			}

			if( xmlFile.FindElem(XMLTAG_BASELINENAME) )
			{
				data = xmlFile.GetData();
				m_strLinetypeName = data;
			}

			if( xmlFile.FindElem(XMLTAG_LineWidth) )
			{
				data = xmlFile.GetData();
				m_fWidth = _ttof(data);
			}

			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	return TRUE;
}

void CCulvertSurface2Symbol::Save(CString& strXML)
{
	CMarkup xmlFile;
	CString data;
	if( xmlFile.AddElem(XMLTAG_SYMBOL) )
	{
		xmlFile.IntoElem();
		
		data.Format(_T("%d"),m_nType);
		xmlFile.AddElem(XMLTAG_TYPE,data);
		
		if( xmlFile.AddElem(XMLTAG_DATA) )
		{
			xmlFile.IntoElem();	
			
			xmlFile.AddElem(XMLTAG_NAME, m_strName);

			xmlFile.AddElem(XMLTAG_BASELINENAME,m_strLinetypeName);

            data.Format(_T("%f"),m_fWidth);
			xmlFile.AddElem(XMLTAG_LineWidth,data);	
				
			xmlFile.OutOfElem();
		}
		xmlFile.OutOfElem();
	}
	
	strXML = xmlFile.GetDoc();
}

BOOL CCulvertSurface2Symbol::IsSupportGeoType(int nGeoType)
{
	if(nGeoType==CLS_GEOSURFACE || nGeoType == CLS_GEOCURVE)
		return TRUE;
	return FALSE;
}

void CCulvertSurface2Symbol::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
    if (pFt==NULL || pBuf==NULL) return;
	
	BOOL bBreakLinetype = m_bBreakLinetype;
	
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrLinetypeScale, ftrLinewidthScale, ftrWid;
	
	ftrLinetypeScale = ftrLinewidthScale = 1;
	ftrWid = 0;
	
	if( pGeo->GetClassType()!=CLS_GEOCURVE && pGeo->GetClassType()!=CLS_GEOSURFACE)
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}	

	if( pGeo->GetClassType()==CLS_GEOCURVE )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinetypeScale = pCurve->m_fLinetypeScale;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if(pGeo->GetClassType()==CLS_GEOSURFACE)
	{
		CGeoSurface *pCurve = (CGeoSurface*)pGeo;
		ftrWid = pCurve->m_fLineWidth;		
	}
	
	float fMapWid = fabs(ftrWid+1)<1e-4?m_fWidth:ftrWid;
	
	float wid = 0;
	BOOL gGrdWid = TRUE;
	if (fabs(fMapWid) < 1e-4)
	{
		gGrdWid = FALSE;
		wid = ftrLinewidthScale;
	}
	else 
	{
		gGrdWid = TRUE;
		wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
	}
	
	CArray<PT_3DEX,PT_3DEX> arrPts;
	pGeo->GetShape(arrPts);
	
	if( arrPts.GetSize()<5 )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	CBaseLineTypeLib *baselineLib = GetBaseLineTypeLib();

	int count_first = ceil(double((arrPts.GetSize()-3))/2.0 - 0.1);
	int nIndex = baselineLib->GetBaseLineTypeIndex(m_strLinetypeName) + 1;
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&arrPts[count_first]);
	pBuf->LineTo(&arrPts[count_first+1]);
	pBuf->End();

	int count_second = floor(double((arrPts.GetSize()-3))/2.0 + 0.1);
	count_second += count_first+1;
	nIndex = baselineLib->GetBaseLineTypeIndex(m_strLinetypeName) + 1;
	pBuf->BeginLineString(pGeo->GetColor(),wid,gGrdWid,nIndex,fDrawScale*fViewScale);
	pBuf->MoveTo(&arrPts[count_second]);
	pBuf->LineTo(&arrPts[count_second+1]);
	pBuf->End();
}

////////////////////////////////////////////////////////////////////////////
/////
//////////////////////////////////////////////////////////////////////////
CCADSymbol::CCADSymbol()
{
	m_nType = SYMTYPE_CAD;
	m_pCADSymLib = NULL;
	m_nCADSymType = CCADSymbol::typeNone;
}


CCADSymbol::~CCADSymbol()
{
	
}



void CCADSymbol::CopyFrom(CSymbol *pSymbol)
{
	CCADSymbol *p = (CCADSymbol*)(pSymbol);
	if( p )
	{
		m_pCADSymLib = p->m_pCADSymLib;
		m_nCADSymType = p->m_nCADSymType;
		m_nType = p->m_nType;
		m_strName = p->m_strName;
	}
	CSymbol::CopyFrom(pSymbol);
}



BOOL CCADSymbol::Explode(const CFeature *pFt, float fDrawScale, CGeoArray& arrPGeos)
{
	CSymbolFlag sf0(CSymbolFlag::Tolerance,0.0);
	CSymbolFlag sf(CSymbolFlag::BreakCell,FALSE);
		
	GrBuffer buf;
	Draw(pFt,&buf,fDrawScale,0,1);
	
	ConvertGrBufferToGeos(&buf,fDrawScale,arrPGeos);	

	if(!GIsSymbolSelfColor(this))
		SetColors(pFt,arrPGeos);
	
	return TRUE;
}

BOOL CCADSymbol::Load(CString& strXML)
{
	return TRUE;
}


void CCADSymbol::Save(CString& strXML)
{
	return;
}

void CCADSymbol::Draw(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	if( m_pCADSymLib==NULL )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}
	
	if( !IsSupportGeoType(pFt->GetGeometry()->GetClassType()) )
	{
		pFt->Draw(pBuf,fDrawScale);
		return;
	}

	if( m_nCADSymType==typeLin )
	{
		DrawLin(pFt,pBuf,fDrawScale,fRotAngle,fViewScale);
	}
	else if( m_nCADSymType==typeHat )
	{
		DrawPat(pFt,pBuf,fDrawScale,fRotAngle,fViewScale);
	}

	return;
}


void CCADSymbol::DrawLin(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	CGeometry *pGeo = pFt->GetGeometry();
	
	float   ftrSymScale = 1.0f, ftrLinewidthScale = 1.0f, ftrWid = 0.0f;

	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrLinewidthScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
		ftrSymScale = pCurve->m_fLinetypeScale;
	}
	
	float fMapWid = ftrWid;
	
	float wid = 0;
	wid = fMapWid*ftrLinewidthScale*fDrawScale*fViewScale;
		
	CArray<PT_3DEX,PT_3DEX> pts, assistPts;
	int nGeoType = pGeo->GetClassType();
	
	if (nGeoType == CLS_GEOPARALLEL)
	{
		CGeoParallel *pParallel = (CGeoParallel*)pGeo;
		
		const CShapeLine *pShape =  pParallel->GetShape();
		pShape->GetPts(pts);
		
		pParallel->GetParallelShape(assistPts,0);
		
	}
	else if (nGeoType == CLS_GEODCURVE)
	{
		CGeoDCurve *pDCurve = (CGeoDCurve*)pGeo;
		
		pDCurve->GetBaseShape(pts);
		pDCurve->GetAssistShape(assistPts);		
		ReversePoints(assistPts.GetData(),assistPts.GetSize());
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		((CGeoCurveBase*)pGeo)->GetShape()->GetPts(pts);
	}
	
	int npsum = GraphAPI::GKickoffSame2DPoints(pts.GetData(),pts.GetSize());
	pts.SetSize(npsum);
	
	if( npsum<2 ) return;

	CCADLinFile *pLinFile = m_pCADSymLib->GetLin();
	const CAD_LTDEF *pDef = pLinFile->GetItem(m_strName);
	if( !pDef )return;

	LinToGrBuffer(pDef,m_pCADSymLib,pts,wid,ftrSymScale,pBuf);

	if( assistPts.GetSize()>=2 )
	{
		npsum = GraphAPI::GKickoffSame2DPoints(assistPts.GetData(),assistPts.GetSize());
		assistPts.SetSize(npsum);
		
		LinToGrBuffer(pDef,m_pCADSymLib,assistPts,wid,ftrLinewidthScale,pBuf);
	}

	pBuf->SetAllColor(pGeo->GetColor());
}


void CCADSymbol::DrawPat(const CFeature *pFt, GrBuffer *pBuf, float fDrawScale, float fRotAngle, float fViewScale)
{
	CGeometry *pGeo = pFt->GetGeometry();
	
	float ftrSymScale = 1.0f, ftrWid = 0.0f;
	double xoff = 0, yoff = 0;
	
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurve)) )
	{
		CGeoCurve *pCurve = (CGeoCurve*)pGeo;
		ftrSymScale = pCurve->m_fLinewidthScale;
		ftrWid = pCurve->m_fLineWidth;
	}
	else if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoSurface)) )
	{
		CGeoSurface *pSurface = (CGeoSurface*)pGeo;
		ftrSymScale = pSurface->m_fIntvScale;
		ftrWid = pSurface->m_fLineWidth;
		xoff = pSurface->m_fXStartOff;
		yoff = pSurface->m_fYStartOff;
	}
	
	float fMapWid = ftrWid;
	
	float wid = 0;
	wid = fMapWid*ftrSymScale*fDrawScale*fViewScale;
	
	CArray<PT_3DEX,PT_3DEX> pts, assistPts;
	int nGeoType = pGeo->GetClassType();
	
	if( pGeo->IsKindOf(RUNTIME_CLASS(CGeoCurveBase)) )
	{
		((CGeoCurveBase*)pGeo)->GetShape()->GetPts(pts);
	}
	
	int npsum = GraphAPI::GKickoffSame2DPoints(pts.GetData(),pts.GetSize());
	pts.SetSize(npsum);
	
	if( npsum<2 ) return;
	
	CCADPatFile *pPatFile = m_pCADSymLib->GetPat();
	const CAD_HATCHDEF *pDef = pPatFile->GetItem(m_strName);
	if( !pDef )return;

	PatToGrBuffer(pDef,pts,wid,ftrSymScale,xoff,yoff,pBuf);

	pBuf->SetAllColor(pGeo->GetColor());
}

BOOL CCADSymbol::IsSupportGeoType(int nGeoType)
{
	if( m_nCADSymType==typeLin )
	{
		if( nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEOSURFACE  || nGeoType == CLS_GEOMULTISURFACE || 
			nGeoType == CLS_GEOPARALLEL || nGeoType==CLS_GEODCURVE )
			return TRUE;
	}
	else if( m_nCADSymType==typeHat )
	{
		if( nGeoType == CLS_GEOCURVE || nGeoType == CLS_GEOSURFACE  || nGeoType == CLS_GEOMULTISURFACE )
			return TRUE;
	}

	return FALSE;
}


void CCADSymbol::SetCADSymLib(CCADSymbolLib *p)
{
	m_pCADSymLib = p;
}


CCADSymbolLib *CCADSymbol::GetCADSymLib()
{
	return m_pCADSymLib;
}

void CCADSymbol::SetCADSymType(int type)
{
	m_nCADSymType = type;
}


int CCADSymbol::GetCADSymType()
{
	return m_nCADSymType;
}


BOOL GIsSymbolSelfColor(CSymbol *pSym)
{
	if( !pSym )
		return FALSE;

	int nType = pSym->GetType();

	if( nType==SYMTYPE_COLORHATCH )
		return ((CColorHatch*)pSym)->m_bUseSelfcolor;

	if( nType==SYMTYPE_DASHLINETYPE )
		return ((CDashLinetype*)pSym)->m_bUseSelfcolor;

	return FALSE;
}


CSymbol *GCreateSymbol(int type)
{
	switch (type)
	{
	case SYMTYPE_CELL:
		{
			return new CCell();
		}
	case SYMTYPE_DASHLINETYPE:
		{
			return new CDashLinetype();
		}
	case SYMTYPE_CELLLINETYPE:
		{
			return new CCellLinetype();
		}
	case SYMTYPE_COLORHATCH:
		{
			return new CColorHatch();
		}
	case SYMTYPE_COLORHATCH_COND:
		{
			return new CConditionColorHatch();
		}
	case SYMTYPE_CELLHATCH:
		{
			return new CCellHatch();
		}
	case SYMTYPE_ANNOTATION:
		{
			return new CAnnotation();
		}
	case SYMTYPE_SCALELINETYPE:
		{
			return new CScaleLinetype();
		}
	case SYMTYPE_LINEHATCH:
		{
			return new CLineHatch();
		}
	case SYMTYPE_DIAGONAL:
		{
			return new CDiagonal();
		}
	case SYMTYPE_PARATYPE:
		{
			return new CParaLinetype();
		}
	case SYMTYPE_ANGBISECTORTYPE:
		{
			return new CAngBisectortype();
		}
	case SYMTYPE_SCALEARCTYPE:
		{
			return new CScaleArctype();
		}
	case SYMTYPE_SCALETURNPLATETYPE:
		{
			return new CScaleTurnplatetype();
		}
	case SYMTYPE_SCALECRANETYPE:
		{
			return new CScaleCranetype();
		}
	case SYMTYPE_SCALEFUNNELTYPE:
		{
			return new CScaleFunneltype();
		}
	case SYMTYPE_SCALE_LiangChang:
		{
			return new CScaleLiangCang();
		}
	case SYMTYPE_SCALE_YouGuan:
		{
			return new CScaleYouGuan();
		}
	case SYMTYPE_SCALE_JianFangWu:
		{
			return new CScaleJianFangWu();
		}
	case SYMTYPE_SCALE_TongCheShuiZha:
		{
			return new CScaleTongCheShuiZha();
		}
	case SYMTYPE_SCALE_BuTongCheShuiZha:
		{
			return new CScaleBuTongCheShuiZha();
		}
	case SYMTYPE_SCALE_DiShangYaoDong:
		{
			return new CScaleDiShangYaoDong();
		}
	case SYMTYPE_SCALE_CellLinetype:
		{
			return new CScaleCellLinetype();
		}
	case SYMTYPE_SCALE_ChuRuKou:
		{
			return new CScaleChuRuKou();
		}
	case SYMTYPE_SCALE_WenShi:
		{
			return new CScaleWenShi();
		}
	case SYMTYPE_SCALE_ChuanSongDai:
		{
			return new CScaleChuanSongDai();
		}
	case SYMTYPE_SCALE_Cell:
		{
			return new CScaleCell();
		}
	case SYMTYPE_TIDALWATER:
		{
			return new CTidalWaterSymbol();
		}
	case SYMTYPE_CULVERTSURFACE1:
		{
			return new CCulvertSurface1Symbol();
		}
	case SYMTYPE_CULVERTSURFACE2:
		{
			return new CCulvertSurface2Symbol();
		}
	case SYMTYPE_SCALE_OldDouya:
		{
			return new CScaleOldDouya();
		}
	case SYMTYPE_SCALE_Xiepo:
		{
			return new CScaleXiepo();
		}
	case SYMTYPE_CAD:
		{
			return new CCADSymbol();
		}
	default:
		return NULL;
	}
	
	return NULL;
}


void UpdateGlobalSymbolizeParams()
{
	CWinApp *pApp = AfxGetApp();
	if (pApp)
	{
		CAnnotation::m_bAnnotToText = pApp->GetProfileInt(REGPATH_USER, REGITEM_ANNOTTOTEXT, FALSE);
		CAnnotation::m_bNotDisplayAnnot = pApp->GetProfileInt(REGPATH_SYMBOL, REGITEM_NOTDISPLAYANNOT, FALSE);
		CAnnotation::m_bUpwardText = pApp->GetProfileInt(REGPATH_SYMBOL, REGITEM_ANNOTATIONUPWARD, FALSE);

		CCellHatch::m_bAccuTrimSurface = pApp->GetProfileInt(REGPATH_USER, REGITEM_TRIMSURFACE, TRUE);
		CCellHatch::m_bkeepOneCell = pApp->GetProfileInt(REGPATH_USER, REGITEM_KEEPONECELL, FALSE);
	}
}


MyNameSpaceEnd