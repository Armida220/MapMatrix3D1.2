#ifndef FUNCTIONS_123i124j1239423040239402349_
#define FUNCTIONS_123i124j1239423040239402349_

#ifndef EXPORT_SMARTVIEW
#define EXPORT_SMARTVIEW
#endif

#define _FABS(x)	((x)>=0?(x):(-(x)))
#define PI			3.14159265


//xx

#include <math.h>
#include "FLOAT.H "
#include "SmartViewBaseType.h"
#include <string>
using namespace std;

#define _FABS(x) ((x)>=0?(x):(-(x)))
#define PI			3.14159265



namespace  GraphAPI
{

extern double EXPORT_SMARTVIEW g_lfDisTolerance;

extern double EXPORT_SMARTVIEW g_lfPraTolerance;

extern double EXPORT_SMARTVIEW g_lfAngTolerance;

extern double EXPORT_SMARTVIEW g_lfZTolerance;

extern double EXPORT_SMARTVIEW g_lfAreaTolerance;



inline double EXPORT_SMARTVIEW GetDisTolerance()
{
	return g_lfDisTolerance;		
}

inline void EXPORT_SMARTVIEW SetDisTolerance(double lfTol)
{
	g_lfDisTolerance = lfTol;
}

inline double EXPORT_SMARTVIEW GetPralTolerance()
{
	return g_lfPraTolerance;		
}

inline void EXPORT_SMARTVIEW SetPralTolerance(double lfTol)
{
	g_lfPraTolerance = lfTol;
}

inline double EXPORT_SMARTVIEW GetAngTolerance()
{
	return g_lfAngTolerance;		
}

inline void EXPORT_SMARTVIEW SetAngTolerance(double lfTol)
{
	g_lfAngTolerance = lfTol;
}

inline double EXPORT_SMARTVIEW GetZTolerance()
{
	return g_lfZTolerance;		
}

inline void EXPORT_SMARTVIEW SetZTolerance(double lfTol)
{
	g_lfZTolerance = lfTol;
}

inline double EXPORT_SMARTVIEW GetAreaTolerance()
{
	return g_lfAreaTolerance;		
}

inline void EXPORT_SMARTVIEW SetAreaTolerance(double lfTol)
{
	g_lfAreaTolerance = lfTol;
}

bool EXPORT_SMARTVIEW SolveEquation(double *plfA,double *plfB,int nN);

/******************************************************************************
函数名   : <GGetCycleLine>
功能     : 以pts的首点为起点,在折线pts方向上以dCycle为周期找到pts上所有的点存在dpts
参数     : 基线pts,基线点数nSum,周期dCycle,dpts存储找到的周期点,pIndexs记录dpts所在线段的序号
返回值   : 若成功返回 TRUE,否则返回FALSE
******************************************************************************/

template<class T>
void  GGetCycleLine(const T *pts,int nSum, const double dCycle,double dXOff,CArray<T,T> &dpts, CArray<int,int> *pIndexs)
{
	if(pts==NULL || nSum<1 || dCycle<=0)
		return;
	dpts.RemoveAll();

	// 对dXOff取余
// 	if (dXOff > dCycle)
// 	{
// 		dXOff = dXOff - (dXOff/dCycle)*dCycle;
// 	}

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
// 	if(dis == 0)
// 	{
// 		dpts.Add(pts[0]);
// 	}
//	dis += dCycle;

	int i = 1;
	while(i < nSum)
	{
		if(dis < 0)
		{
			dis += dCycle;
		}
		else if(dis <= pLen[i])
		{
			
			ratio = (dis-pLen[i-1]) / (pLen[i]-pLen[i-1]);
			T t3d;
			t3d.x = pts[i-1].x + ratio * (pts[i].x - pts[i-1].x);
			t3d.y = pts[i-1].y + ratio * (pts[i].y - pts[i-1].y);
			t3d.z = pts[i-1].z + ratio * (pts[i].z - pts[i-1].z);
			dpts.Add(t3d);
			if( pIndexs )pIndexs->Add(i-1);
			dis += dCycle;
			if (dis > pLen[i])
				i++;
		}
		else
			i++;
	}
	if(pLen)
		delete pLen;
	

}

// 三点共面,获取第三个点高程值
template<class T>
void GetHeightFromSurface(const T &pt0, const T &pt1,const T &pt2, T *ret, int sum)
{	
	// 点法式
	double ax, ay, az, bx, by, bz;
	ax = pt1.x - pt0.x;
	ay = pt1.y - pt0.y;
	az = pt1.z - pt0.z;
	
	bx = pt2.x - pt0.x;
	by = pt2.y - pt0.y;
	bz = pt2.z - pt0.z;
	
	// 判断两向量是否平行
	double alen = sqrt(ax*ax + ay*ay + az*az);
	double blen = sqrt(bx*bx + by*by + bz*bz);
	double c = ax*bx + ay*by + az*bz;
	if ( alen < 1e-6 || blen < 1e-6 || (fabs(c/(alen*blen)) >= 1-1e-6 && fabs(c/(alen*blen)) <= 1+1e-6) )
	{
		for (int i=0; i<sum; i++)
		{
			(ret+i)->z = (pt0.z + pt1.z + pt2.z)/3;
		}
		
		return;
	}
	
	
	double fA, fB, fC;
	fA = ay*bz - az*by;
	fB = bx*az - ax*bz;
	fC = ax*by - bx*ay;
	
	if (fabs(fC) < 1e-6)
	{
		for (int i=0; i<sum; i++)
		{
			(ret+i)->z = (pt0.z + pt1.z + pt2.z)/3;
		}
	}
	else
	{
		for (int i=0; i<sum; i++)
		{
			(ret+i)->z = -( fA*((ret+i)->x-pt0.x) + fB*((ret+i)->y-pt0.y))/fC + pt0.z;
		}
		
	}	
	
}

// 获取线的中点,返回离中点最近的点
template<class T>
int GGetMiddlePt(const T *pts,const int &nSum, T* opt) 
{
	if(pts==NULL || nSum<1 || opt==NULL)
		return -1;

	if (nSum == 1)
	{
		*opt = pts[0];
		return 0;
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
				(pt->y-yold)*(pt->y-yold) +
				(pt->z-zold)*(pt->z-zold) );
		}
		xold = pt->x;
		yold = pt->y;
		zold = pt->z;
		
		p++;
		pt++;
	}

	double dLen =  pDis[nSum-1];
	if (dLen < GetDisTolerance()) 
	{
		*opt = pts[0];
		delete []pDis;
		return -1;
	}

	double dMid = dLen/2;
	
	double dis = 0.0;
	for (int i=1; i < nSum; i++)
	{
		if(dMid <= pDis[i])
		{
			double ratio = (dMid-pDis[i-1]) / (pDis[i]-pDis[i-1]);
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


/******************************************************************************
函数名   : <GGetXOffLine>
功能     : 将折线baseline上的指定的点列pts沿折线方向移动dXOff得到折线dpts
参数     : 基线pts,基线点数nSum,要移动的距离dXOff,dpts存储移动后的点
返回值   : 若成功返回 TRUE,否则返回FALSE
******************************************************************************/
template<class T>
void  GGetXOffLine(const T *pts, int nSum, const double dXOff,CArray<T,T> &dpts)
{

	if(pts==NULL || nSum<1)
		return;
	dpts.RemoveAll();

	//dXOff等于0时
	if(fabs(dXOff) <= GraphAPI::GetDisTolerance())
	{
		for(int i=0; i<nSum; i++)
			dpts.Add(*(pts+i));
		return;
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
	float dis;
	int i,j;
	float *pLenIndex = pLen;
	for(i=0; i < nSum; i++)
	{
		dis = pLenIndex[i] + dXOff;
		
		if(dis < 0)
			continue;
		
		for(j=1; j<nSum; j++)
		{
			
			if(dis <= pLenIndex[j])
			{
				
				ratio = (dis-pLen[j-1]) / (pLen[j]-pLen[j-1]);
				T t3d;
				t3d.x = pts[j-1].x + ratio * (pts[j].x - pts[j-1].x);
				t3d.y = pts[j-1].y + ratio * (pts[j].y - pts[j-1].y);
				t3d.z = pts[j-1].z + ratio * (pts[j].z - pts[j-1].z);
				dpts.Add(t3d);
				break;
			}
			
		}
		
	}
	if(pLen)
		delete pLen;	

}

/*******************************************************************************
函数名   : <GRotate2DPT>
功能     : 以pt点为原点，将pts点数组逆时针旋转angle弧度，结果仍然保存在pts数组中 
参数     :  
           [IN] pt : 作为旋转的原点
           [IN] lfAngle : 旋转的弧度
           [IN|OUT] pts: 执行旋转的点数组，结果仍保存其中
		   [IN] nSum : pts的数组大小
返回值   : 
*******************************************************************************/
template<class T>
void  GRotate2DPT(const T &pt, double lfAngle, T *pts, int nSum)
{
	double dx,dy,sina,cosa;
	
	sina = sin(lfAngle);
	cosa = cos(lfAngle);
	
	for (int i=0; i<nSum; i++)
	{
		dx = pts[i].x - pt.x;
		dy = pts[i].y - pt.y;
		
		pts[i].x = dx * cosa - dy * sina  + pt.x;
		pts[i].y = dy * cosa + dx * sina  + pt.y;
	}
}


/*******************************************************************************
函数名   : <GIsClockwise>
功能     : 判定三条射线 （分别具有弧度lfAng0,lfAng1,lfAng2）的旋转方向
参数     :   
返回值   : 如果顺时针则放回TRUE，否则FALSE

*******************************************************************************/
bool EXPORT_SMARTVIEW GIsClockwise(double lfAng0, double lfAng1, double lfAng2);

/*******************************************************************************
函数名   : <GIsClockwise>
功能     : 判断多边形的旋转方向 
参数     :           
返回值   : 判定失败返回-1，顺时针返回true，逆时针返回false
*******************************************************************************/
template<class T> //已测试
int  GIsClockwise(T *pts, int nNum)
{
	if( nNum<=2 )return -1;

	double *angs = new double[nNum+1];
	int ang_num = 0;	
	int i1;

	for( int i=0; i<nNum; i++ )
	{
		i1 = ((i+1)%nNum);
		if( fabs(pts[i].x-pts[i1].x)<g_lfDisTolerance && 
			fabs(pts[i].y-pts[i1].y)<g_lfDisTolerance )
			continue;

		angs[ang_num++] = GGetAngle(pts[i].x,pts[i].y,pts[i1].x,pts[i1].y);
	}

	if( fabs(pts[0].x-pts[nNum-1].x)>=g_lfDisTolerance || 
		fabs(pts[0].y-pts[nNum-1].y)>=g_lfDisTolerance )
	{
		angs[ang_num++] = GGetAngle(pts[nNum-1].x,pts[nNum-1].y,pts[0].x,pts[0].y);
	}
		
	double all_ang = 0, delta;
	for(i=0; i<ang_num; i++)
	{
		i1 = ((i+1)%ang_num);

		delta = angs[i1]-angs[i];

		if( delta>PI )delta -= 2*PI;
		else if( delta<-PI )delta += 2*PI;
		else if( (delta>PI-g_lfAngTolerance&&delta<PI+g_lfAngTolerance) || (delta>-PI-g_lfAngTolerance&&delta<-PI+g_lfAngTolerance) )
			delta = 0;
		
		all_ang += delta;		
	}

	delete[] angs;
	
	if( all_ang>PI )return 0;
	else if( all_ang<-PI )return 1;
	return -1;
}

/*******************************************************************************
函数名   : <GCalP3ArcPara>
功能     : 计算3点构成圆弧参数(中心点，半径，三个点与圆心连线的方向角度
参数     :  
           [IN] pts : 三点坐标
           [OUT] c : 中心坐标
           [OUT] lfAng : 三个点与圆心连线的方向角度
		   [OUT] lfR : 圆半径
返回值   : bool 是否顺时针 (注: 可能为负值)
备注     :
     three point circle to certer and radius  
Equation: 
x*x + y*y + D*x + E*y + F =0; 
Center: 
(xc,yc)=(-D/2,-E/2) 
Radius: 
sqrt( D*D + E*E - 4*F ) / 2 
= sqrt( xc*xc + yc*yc + (x0*x0+y0*y0) + (-2*xc*x0-2*yc*y0) ) 
= sqrt( xc*(xc-2*x0) + yc*(yc-2*y0) + r0 ) 
*******************************************************************************/
template<class T>
bool  GCalP3ArcPara(T* pts,T* c,double *plfR,double *lfAng, int *bclockwise)
{
	int		i; 
	double	da[2]; 
	double	dx0,dy0,dx1,dy1; 
	double	r0,r1,r2,det,dx,dy;
	double	xoff,yoff; 
	
	xoff = pts[1].x;
	yoff = pts[1].y;
	for( i=0; i<3; i++)
	{
		pts[i].x -= xoff;
		pts[i].y -= yoff;
	}
	
	dx0=pts[0].x - pts[1].x;	dx1=pts[1].x-pts[2].x; 
	dy0=pts[0].y - pts[1].y;	dy1=pts[1].y-pts[2].y; 
	
	r0 = pts[0].x*pts[0].x + pts[0].y*pts[0].y; 
	r1 = pts[1].x*pts[1].x + pts[1].y*pts[1].y; 
	r2 = pts[2].x*pts[2].x + pts[2].y*pts[2].y; 
	
	det = dx0*dy1 - dx1*dy0; 
	if( fabs(det)<1e-10 )return false;
	
	c->x = ( dy1*(r0-r1) - dy0*(r1-r2) )/(2*det); 
	c->y = (-dx1*(r0-r1) + dx0*(r1-r2) )/(2*det); 
	
	r2 = sqrt( c->x*(c->x-2*pts[0].x) + c->y*(c->y-2*pts[0].y)+r0 ); 
	
	for( i=0; i<3; i++) 
	{
		lfAng[i] = GGetAngle(c->x,c->y,pts[i].x,pts[i].y);
	} 
	
	c->x += xoff;	c->y += yoff;
	for( i=0; i<3; i++)
	{
		pts[i].x += xoff;
		pts[i].y += yoff;
	}
	
	if( bclockwise )
	{
		*bclockwise = GIsClockwise(lfAng[0],lfAng[1],lfAng[2]);
	}	
	
	*plfR = r2;
	return true;
}


/*******************************************************************************
函数名   : <GCalTanP2ArcPara>
功能     : 已知：两点，以及其中一点与圆心的连线的方向向量，计算圆的参数
参数     :  
           [IN] double lfVx0,double lfVy0 : 方向向量
           [IN] double lfX1,double lfY1,double lfX2,double lfY2 : 圆上的两点
           [OUT] double *plfXc,double *plfYc : 圆心坐标
		   [OUT] double *plfR :圆半径
		   [OUT] double *plfAng1,double *plfAng2: 圆心与分别与圆上两端连线的方向角的弧度
返回值   : 成功，则返回true；否则，返回false
备注     : A line & two point to certer and radius 
Equation: 
(x0-xc)^2 + (y0-yc)^2 = (x1-xc)^2 + (y1-yc)^2 
xc = x0 + b*t		tangent line(a,b) 
yc = y0 - a*t 
==>	dx=x1-x0,dy=y1-y0 
t = (dx*dx+dy*dy)/2/(b*dx-a*dy) 
Center: 
(xc,yc)= 
Radius: 
r = (x0-xc)^2+(y0-yc)^2 = abs(t) 
*******************************************************************************/
bool EXPORT_SMARTVIEW GCalTanP2ArcPara(double lfVx0,double lfVy0,double lfX1,double lfY1,double lfX2,double lfY2,
				   double *plfXc,double *plfYc,double *plfR,double *plfAng1,double *plfAng2);

/*******************************************************************************
函数名   : <GLineIntersectLine>
功能     : 求两条直线的交点 
参数     :  
	[IN]  lfX0,  lfY0,  lfVx0,  lfVy0 : 表征第一条直线，lfX0,  lfY0,为其上某点，lfVx0,  lfVy0为方向向量
	[IN]  lfX1,  lfY1,  lfVx1,  lfVy1 : 表征第二条直线，lfX1,  lfY1,为其上某点，lfVx1,  lfVy1为方向向量
           
    [OUT]	plfX, plfY :传出交点
返回值   : if two lines is the same or the parallax lines , return false
  else return true	
备注     :
P3	
|
P0-----P-----P1
|
P2
*******************************************************************************/
bool EXPORT_SMARTVIEW GGetLineIntersectLine(double lfX0, double lfY0, double lfVx0, double lfVy0,
						double lfX1, double lfY1, double lfVx1, double lfVy1,
						double *plfX, double *plfY,double *plfT);  //已测试


/*******************************************************************************
/******************************************************************************
函数名   : <GGetLineIntersectLineSeg0>
功能     : 求直线和线段的交点 
参数     :  
           [IN] double lfX0,double lfY0, double lfX1, double lfY1 : 表征线段
           [IN] double lfX2,double lfY2, double lfX3, double lfY3 : 表征直线
           ...
返回值   : 如果有交点，则返回true；否则，返回false。
*******************************************************************************/
bool EXPORT_SMARTVIEW GGetLineIntersectLineSeg0(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY,double *plfT0, double *plfT1);


/******************************************************************************
函数名   : <GGetLineIntersectLineSeg>
功能     : 求两线段的交点 
参数     :  
           [IN] double lfX0,double lfY0, double lfX1, double lfY1 : 第一条线段的首尾点
           [IN] double lfX2,double lfY2, double lfX3, double lfY3 : 第二条线段的首尾点
           ...
返回值   : 如果有交点，则返回true；否则，返回false。

******************************************************************************/
bool EXPORT_SMARTVIEW GGetLineIntersectLineSeg(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY,double *plfT);
bool EXPORT_SMARTVIEW GGetLineIntersectLineSeg(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY,double *plfT0, double *plfT1);
//已测试
/*******************************************************************************
函数名   : <GGetPerpendicular>
功能     : 求点在直线上的投影点
参数     :  
           [IN] double lfX0, double lfY0, double lfX1, double lfY1 : 直线上的两点
           [IN] double lfXt, double lfYt : 需要投影的点
		   [OUT] double *plfX, double *plfY : 投影点
          
返回值   : 
备注     :
	P0-----P-----P1
		   |
		   P2
根据P0，P1, 求 P2 在 P0_P1 上的垂点P, 
*******************************************************************************/
void  EXPORT_SMARTVIEW GGetPerpendicular(double lfX0, double lfY0, double lfX1, double lfY1,
					double lfXt, double lfYt, double *plfX, double *plfY,double *plfT);

/*******************************************************************************
函数名   : <GGetPerpendicular3D>
功能     : 求点在直线上的投影点
参数     :  
           [IN] double lfX0, double lfY0, double lfZ0, double lfX1, double lfY1, double lfZ1 : 直线上的两点
           [IN] double lfXt, double lfYt, double lfZt : 需要投影的点
		   [OUT] double *plfX, double *plfY, double *plfZ : 投影点
          
返回值   : 
*******************************************************************************/
void  EXPORT_SMARTVIEW GGetPerpendicular3D(double lfX0, double lfY0, double lfZ0, double lfX1, double lfY1, double lfZ1,
					   double lfXt, double lfYt, double lfZt, double *plfX, double *plfY, double *plfZ);

/*******************************************************************************
函数名   : <GGetDisofPtToLine>
功能     : 求点在直线上的投影点
参数     :  
           [IN] double lfX0, double lfY0, double lfX1, double lfY1 : 直线上的两点
           [IN] double lfXt, double lfYt :距离点		  
          
返回值   : 点到直线的距离
*******************************************************************************/
double EXPORT_SMARTVIEW GGetDisofPtToLine(double x0, double y0, double x1, double y1,
					  double xt, double yt);


/*******************************************************************************
函数名   : <GGetNearestDisOfPtToLine>
功能     : 求点到2D线段的最短距离 
参数     :  
           [IN] double lfX0, double lfY0, double lfX1, double lfY1 : 线段的两个端点
           [IN] double lfXt, double lfYt : 要求距离的点
		   [OUT] double *plfXr, double *plfYr :最短距离的另一端点
           [IN] bool bFootCanOut : 是否允许点在直线外
返回值   : 返回最短距离

*******************************************************************************/
double EXPORT_SMARTVIEW GGetNearestDisOfPtToLine(double lfX0, double lfY0, double lfX1, double lfY1,
								double lfXt, double lfYt, 
								double *plfXr=NULL, double *plfYr=NULL,
								bool bFootCanOut=TRUE);
//以测试
/*******************************************************************************
函数名   : <GGetNearestDisOfPtToLine3D>
功能     : 在立体空间上求点到线段的距离 
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : <函数返回值的意义（如果有的话）>

*******************************************************************************/
double EXPORT_SMARTVIEW GGetNearestDisOfPtToLine3D(double lfX0, double lfY0, double lfZ0, double lfX1, double lfY1, double lfZ1,
							   double lfXt, double lfYt, double lfZt, 
							   double *xr=NULL, double *yr=NULL, double *zr=NULL,
							 bool bFootCanOut=true, bool b2DDis=true);

/*******************************************************************************
函数名   : <GGetPerpPointPosinLine>
功能     : 求点到一条线段的垂点在此线段上的位置
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   :在线段的前向延长线上时，返回-1；线段上时，返回0；后向延长线上时，返回1； 

*******************************************************************************/
int EXPORT_SMARTVIEW GGetPerpPointPosinLine(double lfX0, double lfY0, double lfX1, double lfY1, double lfX, double lfY);

/*******************************************************************************
函数名   : <GGetPtZOfLine>
功能     : <函数实现功能> 
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : Z坐标

*******************************************************************************/
void EXPORT_SMARTVIEW GGetPtZOfLine(double lfX0,double lfY0,double lfZ0,double lfX1,double lfY1, double lfZ1, double *plfX,double *plfY,double *plfZ);


template<class T>
void GGetPtZOfLine(const T *pt0, const T *pt1, T *pt2)
{
	GGetPtZOfLine(pt0->x,pt0->y,pt0->z,pt1->x,pt1->y,pt1->z,&pt2->x,&pt2->y,&pt2->z);
}

/*******************************************************************************
函数名   : <GGetParallelLine>
功能     : <函数实现功能> 
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : <函数返回值的意义（如果有的话）>
备注     :
GGetParallelLine
(pts[0])   (pRet[0])
\             \
\    width    \
\ ----------  \
\             \
\             \
\             \
\             \
(pts[sum-1]) (pRet[sum-1])
*******************************************************************************/
template<class T>                              //已测试
bool GGetParallelLine(T* pts,int nSum,double lfWidth,T* pRet)
{
	if( fabs(lfWidth)<g_lfDisTolerance||nSum<=1 )
	{
		memcpy(pRet,pts,sizeof(T)*nSum);
		return true;
	}
	
	double vx0, vy0, vx1, vy1, vx00, vy00, x, y;
	T pt0, pt00, pt1;
	
	int j, i0, i1, i10;
	
	//计算第一个点
	i0 = 0;
	
	//找到下一个不相同的点
	for( j=i0+1; j<nSum; j++)
	{
		if( !GIsEqual2DPoint(pts+i0,pts+j) )
			break;
	}
	i1 = i10 = j;

	if (j >= nSum)
	{
		memcpy(pRet, pts, sizeof(T)*nSum);
		return true;
	}
	
	//计算平行射线的起点和方向
	vx00 = vx0 = pts[i1].x-pts[i0].x; vy00 = vy0 = pts[i1].y-pts[i0].y;
	double dis = sqrt(vx0*vx0 + vy0*vy0);
	double kx = vx0/dis, ky = vy0/dis;
	
	pt0.x = pts[i0].x - ky*lfWidth;  
	pt0.y = pts[i0].y + kx*lfWidth;
	pt0.z = pts[i0].z;
	
	//备份首点以在闭合时使用
	pt00 = pt0;  
	
	//依次计算各个平行点
	i0 = i1;
	for( ; i1<nSum; )
	{
		//找到下一个不相同的点
		for( j=i0+1; j<nSum; j++)
		{
			if( !GIsEqual2DPoint(pts+i0,pts+j) )
				break;
		}
		
		//计算平行射线的起点和方向
		i1 = j;
		if( j<nSum )
		{
			vx1 = pts[i1].x-pts[i0].x; vy1 = pts[i1].y-pts[i0].y;
			
			dis = sqrt(vx1*vx1 + vy1*vy1);
			kx = vx1/dis, ky = vy1/dis; 
		}
		
		pt1.x = pts[i0].x - ky*lfWidth;  
		pt1.y = pts[i0].y + kx*lfWidth;
		pt1.z = pts[i0].z;

		if( j>=nSum )break;
		
		//和上一段平行射线求交
		if( !GGetLineIntersectLine(pt0.x,pt0.y,vx0,vy0,pt1.x,pt1.y,vx1,vy1,&x,&y,NULL) )
		{
			x = pt1.x; y = pt1.y;
		}
		
		//保存结果
		for( j=i0; j<i1; j++)
		{
			pRet[j].x = x; pRet[j].y = y; pRet[j].z = pts[j].z;
		}
		
		//设置循环参数
		i0 = i1;
		vx0 = vx1, vy0 = vy1;
		pt0 = pt1;
	}
	
	//设置首尾点的结果
	//闭合情况
	if( GIsEqual2DPoint(pts,pts+nSum-1) )
	{
		//和上一段平行射线求交
		if( !GGetLineIntersectLine(pt00.x,pt00.y,vx00,vy00,pt1.x,pt1.y,vx1,vy1,&x,&y,NULL) )
		{
			x = pts[0].x; y = pts[0].y;
		}
		
		//尾点结果
		for( j=i0; j<i1; j++)
		{
			pRet[j].x = x; pRet[j].y = y; pRet[j].z = pts[j].z;
		}
		
		//首点结果
		for( j=0; j<i10; j++)
		{
			pRet[j].x = x; pRet[j].y = y; pRet[j].z = pts[j].z;
		}
	}
	else
	{
		//和上一段平行射线求交
		x = pt1.x, y = pt1.y;
		
		//尾点结果
		for( j=i0; j<i1; j++)
		{
			pRet[j].x = x; pRet[j].y = y; pRet[j].z = pts[j].z;
		}
		
		//首点结果
		for( j=0; j<i10; j++)
		{
			pRet[j] = pt00;
		}
	}
	return true;
}


/***************************************************************************
P1-----------P2
             |
             |
      P3-----P
根据P1，P2，P3，求P, 其中，P1_P2 垂直于 P2_P; P1_P2 平行于 P3_P
****************************************************************************/
void EXPORT_SMARTVIEW GGetRightAnglePoint(double lfX1,double lfY1,double lfX2,double lfY2,double lfX3,double lfY3,double *plfX,double *plfY);



/*double GDistance(PT_2D pt0, PT_2D pt1);*/
/*******************************************************************************
函数名   : <GGetAngle>
功能     : 得到向量的方向角 
参数     :  
           [IN] double lfX1, double lfY1 : 首点
           [IN] double lfX2, double lfY2 : 尾点
           ...
返回值   : 返回方向角
备注     : 返回角度为弧度
*******************************************************************************/
double EXPORT_SMARTVIEW GGetAngle(double lfX1,double lfY1, double lfX2, double lfY2);

/*******************************************************************************
函数名   : <GRectifyPoints>
功能     : 直角化 
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : <函数返回值的意义（如果有的话）>

*******************************************************************************/
template<class T>
bool  GRectifyPoints(T *pts, int npt, float toler, bool bLockStart, bool bLockEnd)
{
	if( npt<=2 )return false;

	//条件数目
	int nConditionNum = npt - 2;
	
	BOOL bClosed = TRUE;
	if( GIsEqual2DPoint(pts,pts+npt-1) )
		npt--;
	else
		bClosed = FALSE;

	//变量数
	int nVarNum = npt*2;
	
	//准备平差
	double *pa0 = new double[nConditionNum*nVarNum];
	double *pata = new double[nConditionNum*nConditionNum];
	double *pv0  = new double[nVarNum];
	double *pw0  = new double[nConditionNum];

	bool bRet = false;

	//执行 15 次
	for( int i=0; i<15; i++)
	{
		double *pa = pa0, *pw = pw0, *pv = pv0;
		T *pts2 = pts;
		
		memset(pa,0,nConditionNum*nVarNum*sizeof(double));
		memset(pata,0,nConditionNum*nConditionNum*sizeof(double));
		memset(pv,0,nVarNum*sizeof(double));

		//计算系数矩阵和值向量
		double dx1 = pts2->x - pts2[1].x;
		double dy1 = pts2->y - pts2[1].y;
		int j = 0;
		for ( j=0; j<nConditionNum; j++)
		{
			double dx2 = pts2[2].x - pts2[1].x;
			double dy2 = pts2[2].y - pts2[1].y;

			*pw++ = dx1*dx2 + dy1*dy2;
			*pa++ = dx2; *pa++ = dy2;
			*pa++ = -dx1-dx2; *pa++ = -dy1-dy2;

			if( bClosed && (j==nConditionNum-1) )
				pa -= nVarNum;
			
			*pa++ = dx1; *pa++ = dy1;
			dx1 = -dx2;	dy1 = -dy2;
			pa += (nVarNum-4);
			pts2++;
		}

		//闭合条件的处理
		if( bClosed )
		{
			if( bLockStart )
			{
				pa0[0] = 0; pa0[1] = 0;
				pa0[(nConditionNum-1)*nVarNum+0] = 0;
				pa0[(nConditionNum-1)*nVarNum+1] = 0;
			}
			if( bLockEnd )
			{
				pa0[(nConditionNum-1)*nVarNum-2] = 0;
				pa0[(nConditionNum-1)*nVarNum-1] = 0;				
				pa0[nConditionNum*nVarNum-2] = 0;
				pa0[nConditionNum*nVarNum-1] = 0;
			}
		}
		else
		{
			if( bLockStart )
			{
				pa0[0] = 0; pa0[1] = 0;
			}
			if( bLockEnd )
			{
				pa0[nConditionNum*nVarNum-2] = 0;
				pa0[nConditionNum*nVarNum-1] = 0;	
			}
		}

		//计算 AT*A 矩阵
		int m = 0;
		for( m=0; m<nConditionNum; m++)
		{
			for( int n=0; n<nConditionNum; n++)
			{
				pata[m*nConditionNum + n] = 0;
				for (int l=0; l<nVarNum; l++)
				{
					pata[m*nConditionNum + n] += pa0[m*nVarNum+l]*pa0[n*nVarNum+l];
				}
			}
		}

		//求解方程
		SolveEquation(pata,pw0,nConditionNum);

		//计算改正数
		for( m=0; m<nVarNum; m++)
		{
			for( int n=0; n<nConditionNum; n++)
			{
				pv0[m] += pa0[n*nVarNum+m] * pw0[n];
			}
		}

		//计算最大偏差
		double lfmax = 0;
		for( j=0; j<npt; j++)
		{
			if( j==0 || lfmax<fabs(pv0[j]) )lfmax = fabs(pv0[j]);
		}
		if( lfmax>toler )
		{
			bRet = false;
			break;
		}

		//保存结果
		pv = pv0;
		for( j=0; j<npt; j++)
		{
			pts[j].x -= pv[0];
			pts[j].y -= pv[1];
			pv += 2;
		}
		if( bClosed )	
		{
			pts[npt].x = pts[0].x;
			pts[npt].y = pts[0].y;
		}

		//偏差较小 即可终止循环
		if( lfmax<toler*0.5 )
		{
			bRet = true;
			break;
		}
	}

	delete[] pa0;
	delete[] pata;
	delete[] pv0;
	delete[] pw0;

	return bRet;
}

/*******************************************************************************
函数名   : <GGetTriangleArea>
功能     : 得到三角形面积(二维) 
参数     :            
           ...
返回值   : 三角形的面积

*******************************************************************************/

double EXPORT_SMARTVIEW GGetTriangleArea(double lfX0,double lfY0,double lfX1,double lfY1,double lfX2,double lfY2);

/*******************************************************************************
函数名   : <GGetPolygonArea>
功能     : 得到多边形的面积
参数     :             
           ...
返回值   : 多边形的面积
*******************************************************************************/
template<class T>
double  GGetPolygonArea(T *pts, int nNum)
{
	if( nNum<3 )return 0;
	
	double area = 0;
	for( int i=0; i<nNum-1; i++)
	{
		area += ((pts[i].y+pts[i+1].y)*(pts[i+1].x-pts[i].x)*0.5);
	}
	area += ((pts[nNum-1].y+pts[0].y)*(pts[0].x-pts[nNum-1].x)*0.5);
	area = fabs(area);
	
	return area;
}


template<class T>
BOOL GGetCenter2D(T *pts, int num, T *opt)
{
	if( num<=0 )return FALSE;
	
	T t;
	if( num==1 )
	{
		t = pts[0];
		COPY_2DPT((*opt),t);
		return TRUE;
	}
	else if( num==2 )
	{
		t = pts[0];
		COPY_2DPT((*opt),t);
		t = pts[1];
		opt->x = (opt->x+t.x)/2;
		opt->y = (opt->y+t.y)/2;
		return TRUE;
	}
	
	T pts0[3], cpt, cpt0;

	cpt.x = cpt.y = 0;
	cpt0.x = cpt0.y = 0;

	t = pts[0]; 
	COPY_2DPT(pts0[0],t);
	t = pts[1]; 
	COPY_2DPT(pts0[1],t);
	double area = 0, sarea = 0;
	for( int i=2; i<num; i++ )
	{
		t = pts[i]; 
		COPY_2DPT(pts0[2],t);
		
		cpt.x = (pts0[0].x+pts0[1].x+pts0[2].x)/3;
		cpt.y = (pts0[0].y+pts0[1].y+pts0[2].y)/3;
		
		double v1x = pts0[1].x-pts0[0].x, v1y = pts0[1].y-pts0[0].y;
		double v2x = pts0[2].x-pts0[1].x, v2y = pts0[2].y-pts0[1].y;
		
		area = (v1x*v2y-v1y*v2x)/2;
		sarea += area;
		cpt0.x += (cpt.x*area); cpt0.y += (cpt.y*area);
		
		COPY_2DPT(pts0[1],pts0[2]);
	}
	
	if( fabs(sarea)<=1e-10 )*opt = pts0[0];
	else 
	{
		cpt0.x /= sarea; cpt0.y /= sarea;
	}
	*opt = cpt0;
	
	return TRUE;
	
}

template<class T>
BOOL GGetCenter(T *pts, int num, T *opt)
{
	if( num<=0 )return FALSE;
	
	T t;
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
	
	T pts0[3], cpt, cpt0;
	cpt.x = cpt.y = cpt.z = 0;
	cpt0.x = cpt0.y = cpt0.z = 0;

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
		
		double v1x = pts0[1].x-pts0[0].x, v1y = pts0[1].y-pts0[0].y;
		double v2x = pts0[2].x-pts0[1].x, v2y = pts0[2].y-pts0[1].y;

		area = (v1x*v2y-v1y*v2x)/2;
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

/*******************************************************************************
函数名   : <GIsPtInRegion>
功能     : 判断点是否在多边形内 
参数     :  
	T pt, 点坐标；
	T* pts, 多边形边界点的坐标，不含闭合点；
	nNum, 多边形边界点的数目, nNum>=3；
           ...
返回值   : -2, 参数无效, -1, 点在多边形外, 0, 点和多边形的点重叠, 1, 点在多边形上, 2, 点在多边形内;
备注     :
参数  :
算法思想：用一个变量sum记录"净穿越"射线的边数.	顺序遍历多边形的所有边,设
		当前遍历到的边的顶点	为P1、P2（P1在前P2在后）, 则将D的值累加上：
		sgn(P1.y-P.y)+sgn(P.y-P2.y)，到最后看D的值是否为零，为零则在多边形
		外，否则在其内。不是对所有边进行累加D的操作，只是那些与从P引出的向
		右水平射线相交的。

*******************************************************************************/
//符号函数：sgn(x)= 1,x>0 ; -1,x<0; 0,x=0.
inline int sign_compare(double x, double toler)
{
	if (fabs(x)<toler)
	{
		return 0;
	}
	else 
		return  x>0?1:-1;
}

template<class T>
int   GIsPtInRegion(T pt0, T *pPts,int nNum )
{
	int    i,j ,  sum=0;   
    double    xmin,   ymin,   xmax,   ymax;   
    double   x;   

	T* pPt = &pt0;
	
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
		if(pPt->y>ymax+g_lfDisTolerance||pPt->y<ymin-g_lfDisTolerance||pPt->x>xmax+g_lfDisTolerance)   
			continue;   

		//可能经过线段i->j某个顶点
		if(fabs(pPt->y-pPts[i].y)<g_lfDisTolerance && fabs(pPt->y-pPts[j].y)<g_lfDisTolerance)
		{
			//线段平行 
			if(1)//此处不能用ymin，ymax  
			{
				// 检验点在线段端点上
				if (fabs(pPt->x-xmin)<g_lfDisTolerance||fabs(pPt->x-xmax)<g_lfDisTolerance)
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
		}
		else if(fabs(pPt->y-pPts[i].y)<g_lfDisTolerance||fabs(pPt->y-pPts[j].y)<g_lfDisTolerance)
		{   
			//线段平行 
			if(fabs(pPts[i].y-pPts[j].y)<g_lfDisTolerance)//此处不能用ymin，ymax  
			{
				// 检验点在线段端点上
				if (fabs(pPt->x-xmin)<g_lfDisTolerance||fabs(pPt->x-xmax)<g_lfDisTolerance)
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
				if (fabs(pPt->y-pPts[i].y)<g_lfDisTolerance)
				{
					// 检验点与i对应的点重合
					if (fabs(pPt->x-pPts[i].x)<g_lfDisTolerance)
					{
						return 0;
					}
					// 水平射线过i点
					else if(pPt->x<pPts[i].x)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,g_lfDisTolerance)+sign_compare(pPts[j].y-pPt->y,g_lfDisTolerance);
						continue;
					}
					else					
						continue;
				}
				// 经过j对应的点
				else 
				{
					if (fabs(pPt->x-pPts[j].x)<g_lfDisTolerance)
					{
						return 0;
					}
					else if(pPt->x<pPts[j].x)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,g_lfDisTolerance)+sign_compare(pPts[j].y-pPt->y,g_lfDisTolerance);
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
				if (pPt->x<xmin-g_lfDisTolerance)
				{
					sum+=sign_compare(pPt->y-pPts[i].y,g_lfDisTolerance)+sign_compare(pPts[j].y-pPt->y,g_lfDisTolerance);
					continue;
				}
				// 无法直接判断的情况最后经过计算判断
				else
				{
					x=(double)(pPt->y-pPts[i].y)/(pPts[j].y-pPts[i].y)*(pPts[j].x-pPts[i].x)+pPts[i].x;
					
					//检验点在线段上
					if(fabs(x-pPt->x)<g_lfDisTolerance)
						return (1);

					//检验点与线段相交
					if(x>pPt->x)
					{
						sum+=sign_compare(pPt->y-pPts[i].y,g_lfDisTolerance)+sign_compare(pPts[j].y-pPt->y,g_lfDisTolerance);
						continue;
					}
				}
			}
		}
    }  
    if(sum==0)   return(-1);   //   p在多边形外   
    else        return(2);     //   p在多边形内 
}


//求差积
template<class T>
inline double GGetMultiply(const T &s1,const T &s2,const T &p)
{
	return((s2.x-s1.x)*(p.y-s1.y)-(s2.y-s1.y)*(p.x-s1.x));
} 

//求p点到直线s1s2的距离
template<class T>
double  GGetPtoLDist2D(const T &s1,const T &s2,const T &p)
{
	if (GIsEqual2DPoint(&s1,&s2))
	{
		return GGet2DDisOf2P(s1,p);
	}
	return fabs(GGetMultiply(s1,s2,p))/sqrt((s2.x-s1.x)*(s2.x-s1.x)+(s2.y-s1.y)*(s2.y-s1.y));
} 

/*******************************************************************************
函数名   : <GGetIncludedAngle>
功能     : 得到两向量的夹角
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : 夹角的弧度

*******************************************************************************/
template<class T>
double  GGetIncludedAngle(T s1,T s2,T p1,T p2)
{
	//求两向量的点积
	double vx1 = s2.x-s1.x, vy1 = s2.y-s1.y;
	double vx2 = p2.x-p1.x, vy2 = p2.y-p1.y;

	double v = vx1*vx2 + vy1*vy2;
	double dis = GraphAPI::GGet2DDisOf2P(s1,s2)*GraphAPI::GGet2DDisOf2P(p1,p2);
	if( dis<1e-10 )
		return 0;

	double cosfi = v/dis;

	//可能存在浮点误差
	if( cosfi<=-1.0 )
		return PI;
	else if( cosfi>=1.0 )
		return 0;

	return acos(cosfi);
} 


static bool GGetIntersectLineSegParam(double x0,double y0, double x1, double y1,double x2,double y2,double z2,double x3,double y3, double z3,double *x, double *y,double *z, double *t,double *t3)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2,vector2z=z3-z2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return false;       //平行无交点
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xx = x0 + t1*vector1x;
	double yy = y0 + t1*vector1y;
	if (fabs(xx-x1)<g_lfDisTolerance&&
		fabs(yy-y1)<g_lfDisTolerance)
	{
		t1=1;
	}
	else if( t1>1 )return false; 
	if ((fabs(xx-x0)<g_lfDisTolerance&&
		fabs(yy-y0)<g_lfDisTolerance)||t1<0)
	{
		return false;
	}

	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	if ((fabs(xx-x3)>g_lfDisTolerance&&fabs(yy-y3)>g_lfDisTolerance&&t2>1)||
		((fabs(xx-x2)<g_lfDisTolerance&&fabs(yy-y2)<g_lfDisTolerance)||t2<0))
	{
		return false;
	}
	if(x)*x = xx;
	if(y)*y = yy;
	if(z)*z = z2 + t2*vector2z;
	if(t)*t = t1;
	if (t3)
	{
		*t3=t2;
	}
	return true;
}


template<class T>
bool  GGetFirstPointOfIntersect(T *pts,int nNum,T pt1,T pt2,T *pRet,double *plfMintt)
{	
	int i/*,index*/;
	double mint=1+1e-6,t,st0/*,st1*/;
	T tem,ret1;
	for (i=0;i<nNum-1;i++)
	{
		if(max(pts[i].x,pts[i+1].x)<min(pt1.x,pt2.x)||
			min(pts[i].x,pts[i+1].x)>max(pt1.x,pt2.x)||
			max(pts[i].y,pts[i+1].y)<min(pt1.y,pt2.y)||
			min(pts[i].y,pts[i+1].y)>max(pt1.y,pt2.y))
		{ 
			continue;
		}
		if(GGetIntersectLineSegParam(pt1.x,pt1.y,pt2.x,pt2.y,pts[i].x,pts[i].y,pts[i].z,pts[i+1].x,pts[i+1].y,pts[i+1].z,&(tem.x),&(tem.y),&(tem.z),&t,&st0))
		{
			if (t<=mint)
			{
				if (st0<0.5)
				{
					if((fabs(pts[i].x-tem.x)+fabs(pts[i].y-tem.y))<g_lfDisTolerance)
					{					
						if (i!=0 && (GGetMultiply(pt1,pt2,pts[i-1])*GGetMultiply(pt1,pt2,pts[i+1])<0))
						{
							COPY_3DPT(ret1,pts[i]);
								mint=t;
							continue;
						}
						else
							continue;
					}
				}
				else
				{
					if((fabs(pts[i+1].x-tem.x)+fabs(pts[i+1].y-tem.y))<g_lfDisTolerance)
					{
						if (i+2!=nNum && (GGetMultiply(pt1,pt2,pts[i])*GGetMultiply(pt1,pt2,pts[i+2])<0))
						{						
							COPY_3DPT(ret1,pts[i+1]);
								mint=t;
							continue;
						}
						else
							continue;
					}					
				}
				mint=t;
				ret1=tem;
			}			
		}
	}
	if (mint>1)
	{
		return false;
	}
	else
	{
		if(pRet)*pRet=ret1;
		if(plfMintt)*plfMintt=mint;
		return true;		
	}	
}



/*******************************************************************************
函数名   : <GetPointsOfIntersect>
功能     : 获取弯曲线段和一直线段的所有交点
参数     :  
[IN] T *pts,int nNum : 线串
[IN] T pt1,T pt2 : 射线
[OUT] T *pRet : 交点
[OUT] int nRetNum :交点个数

  返回值   : ture/false
  cjc 2012-11-22 
*******************************************************************************/
template<class T>
bool GGetPointsOfIntersect(T *pts,int nNum,T pt1,T pt2,T *pRet,int *nRetNum)
{
	int i,j=0;//j用于记录相交点的个数
	double t,st0/*,st1*/;
	T tem;
	
	for (i=0;i<nNum-1;i++)
	{
		if(max(pts[i].x,pts[i+1].x)<min(pt1.x,pt2.x)-g_lfDisTolerance||
			min(pts[i].x,pts[i+1].x)>max(pt1.x,pt2.x)+g_lfDisTolerance||
			max(pts[i].y,pts[i+1].y)<min(pt1.y,pt2.y)-g_lfDisTolerance||
			min(pts[i].y,pts[i+1].y)>max(pt1.y,pt2.y)+g_lfDisTolerance)
		{ 
			continue;
		}
		if(GGetIntersectLineSegParam(pt1.x,pt1.y,pt2.x,pt2.y,pts[i].x,pts[i].y,pts[i].z,pts[i+1].x,pts[i+1].y,pts[i+1].z,&(tem.x),&(tem.y),&(tem.z),&t,&st0))
		{
			if(j<nNum-1)
			{
				pRet[j] = tem;
				j++;
			}
		}
	}
	
	if (j>0)
	{
		*nRetNum =j;
		return true;
	}
	else
		return false;	
	
}



/*******************************************************************************
函数名   : <GGetFirstPointOfRayIntersect>
功能     : 射线和线串的交点
参数     :  
           [IN] T *pts,int nNum : 线串
           [IN] T pt1,T pt2 : 射线
		   [OUT] T *pRet : 交点
          
返回值   : 

*******************************************************************************/
template<class T>
bool  GGetFirstPointOfRayIntersect(T *pts,int nNum,T pt1,T pt2,T *pRet,double *plfMinst,double *plfMintt)
{	
	int i,index;
	double mint=-1,t,minst0=-1,st0;
	T tem,ret1;
	for (i=0;i<nNum-1;i++)
	{
		if(GGetLineIntersectLineSeg0(pts[i].x,pts[i].y,pts[i+1].x,pts[i+1].y,pt1.x,pt1.y,pt2.x,pt2.y,&(tem.x),&(tem.y),&st0,&t))
		{
			if ((t >= 0 && t <= mint) || mint < 0)
			{
				index = i;
				mint=t;
				minst0=st0;
				ret1=tem;
				ret1.z = pts[i].z + st0 * (pts[i+1].z - pts[i].z);
			}			
		}
	}
	if (mint < 0)
	{
		return false;
	}
	else
	{
		if(pRet)*pRet=ret1;
		if(plfMinst)*plfMinst=minst0+index;
		if(plfMintt)*plfMintt=mint;
		return true;		
	}	
}

/*******************************************************************************
函数名   : <GIsPtInRange>
功能     : 判断点pt是否在以pt1和pt2为对角线构成的矩形内 
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : 点在举行内，则返回true；否则，false。

*******************************************************************************/
template<class T>
bool  GIsPtInRange(T pt1,T pt2,T pt,bool bIs3D = false)
{
	if (bIs3D)
	{
		if (pt.x>=min(pt1.x,pt2.x)-g_lfDisTolerance&&
			pt.x<=max(pt1.x,pt2.x)+g_lfDisTolerance&&
			pt.y>=min(pt1.y,pt2.y)-g_lfDisTolerance&&
			pt.y<=max(pt1.y,pt2.y)+g_lfDisTolerance&&
			pt.z>=min(pt1.z,pt2.z)-g_lfDisTolerance&&
			pt.z<=max(pt1.z,pt2.z)+g_lfDisTolerance)	
			return true;

		if( GGet2DDisOf2P(pt,pt1)<g_lfDisTolerance )
			return true;

		if( GGet2DDisOf2P(pt,pt2)<g_lfDisTolerance )
			return true;

		return false;
	}
	else
	{
		if (pt.x>=min(pt1.x,pt2.x)-g_lfDisTolerance&&
			pt.x<=max(pt1.x,pt2.x)+g_lfDisTolerance&&
			pt.y>=min(pt1.y,pt2.y)-g_lfDisTolerance&&
			pt.y<=max(pt1.y,pt2.y)+g_lfDisTolerance)		
			return true;

		if( GGet2DDisOf2P(pt,pt1)<g_lfDisTolerance )
			return true;

		if( GGet2DDisOf2P(pt,pt2)<g_lfDisTolerance )
			return true;			
		
		return false;
	}
}

// 点是否在线上
template<class T>
bool  GIsPtInLine(T pt1,T pt2,T pt)
{
	if ( GGetDisofPtToLine(pt1.x,pt1.y,pt2.x,pt2.y,pt.x,pt.y)<g_lfDisTolerance && GIsPtInRange(pt1,pt2,pt)) 
		return true;
	
	return false;
}

/*******************************************************************************
函数名   : <GKickoffSamePoints>
功能     : 剔除点序列中的重复点
参数     :  
          [IN|OUT] T *pts:  点序列
		  [IN] int nNum :点数
返回值   : 处理后的点数目
*******************************************************************************/
template<class T>
int  GKickoffSamePoints(T *pts, int nNum)
{
	int i, pos = 0;
	T *pt = pts+1;
	if( nNum<=1 )return nNum;
	for( i=1; i<nNum; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<g_lfDisTolerance && 
			fabs(pts[pos].y-pt->y)<g_lfDisTolerance && 
			fabs(pts[pos].z-pt->z)<g_lfDisTolerance )
			continue;
		if( i!=(pos+1) )pts[pos+1] = *pt;
		pos++;
	}
	return (pos+1);
}


template<class T>
int  GKickoffSame2DPoints(T *pts, int nNum)
{
	int i, pos = 0;
	T *pt = pts+1;
	if( nNum<=1 )return nNum;
	for( i=1; i<nNum; i++,pt++)
	{
		if( fabs(pts[pos].x-pt->x)<g_lfDisTolerance && 
			fabs(pts[pos].y-pt->y)<g_lfDisTolerance )
			continue;
		if( i!=(pos+1) )pts[pos+1] = *pt;
		pos++;
	}
	return (pos+1);
}



template<class T>
int  GKickoffSameData(T *arr, int nNum)
{
	int i, pos = 0;
	T *p = arr+1;
	if( nNum<=1 )return nNum;
	for( i=1; i<nNum; i++,p++)
	{
		if( arr[pos]==*p )
			continue;
		if( i!=(pos+1) )arr[pos+1] = *p;
		pos++;
	}
	return (pos+1);
}


/*******************************************************************************
函数名   : <GIsEqualPoint>
功能     : 判断两点是否重合
参数     :     
           
返回值   : 重合则返回true；否则，false。

*******************************************************************************/
template<class T>
bool  GIsEqual2DPoint(const T *pt1, const T *pt2)
{
	if( fabs(pt1->x-pt2->x)<g_lfDisTolerance && fabs(pt1->y-pt2->y)<g_lfDisTolerance )
		return true;
	return false;
}

template<class T>
bool  GIsEqual3DPoint(const T *pt1, const T *pt2)
{	
	if( fabs(pt1->x-pt2->x)<g_lfDisTolerance && fabs(pt1->y-pt2->y)<g_lfDisTolerance && fabs(pt1->z-pt2->z)<g_lfDisTolerance )
		return true;
	return false;
}

inline bool EXPORT_SMARTVIEW GIsEqualPoint2D(double lfX0,double lfY0,double lfX1,double lfY1)
{
	if( fabs(lfX1-lfX0)<g_lfDisTolerance && fabs(lfY1-lfY0)<g_lfDisTolerance )
		return true;
	return false;
}
inline bool EXPORT_SMARTVIEW GIsEqualPoint3D(double lfX0,double lfY0,double lfZ0, double lfX1,double lfY1,double lfZ1)
{
	if( fabs(lfX1-lfX0)<g_lfDisTolerance && fabs(lfY1- lfY0)<g_lfDisTolerance && fabs(lfZ1-lfZ0)<g_lfDisTolerance )
		return true;
	return false;
}

/*******************************************************************************
函数名   : <GGetDisOf2P>
功能     : 求两点的距离 
参数     :  
          
返回值   : 两点的距离
*******************************************************************************/
template<class T>
double  GGet2DDisOf2P(const T &p1,const T &p2) // 返回两点之间欧氏距离
{	
	return( sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y) ) );
} 

template<class T>
double  GGet3DDisOf2P(const T &p1,const T &p2) // 返回两点之间欧氏距离
{
	return( sqrt( (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z) ) );
} 

inline double EXPORT_SMARTVIEW GGetDisOf2P2D(double lfX0, double lfY0,double lfX1,double lfY1)
{
	return sqrt((lfX1-lfX0)*(lfX1-lfX0)+(lfY1-lfY0)*(lfY1-lfY0));
}

inline double EXPORT_SMARTVIEW GGetDisOf2P3D(double lfX0, double lfY0, double lfZ0, double lfX1,double lfY1, double lfZ1)
{
	return sqrt((lfX1-lfX0)*(lfX1-lfX0)+(lfY1-lfY0)*(lfY1-lfY0)+(lfZ1-lfZ0)*(lfZ1-lfZ0));
}

// 两向量的点积
template<class T>
inline double  GGetDotMultiply(const T &p1,const T &p2,const T &p0)
{
	return ((p1.x-p0.x)*(p2.x-p0.x)+(p1.y-p0.y)*(p2.y-p0.y));
}

/*******************************************************************************
函数名   : <GGetNearstDisPToPointSet2D>
功能     : 得到点到折线串的距离的最小值， 
参数     :  
           [IN] T pointset[],int nCount : 折线串
           [IN] const T &p : 被求点
		   [OUT] T &q  :折线串上满足条件的点
		   [OUT] int *nIndex: q点在折线串的线段索引
           ...
返回值   : 最小距离

*******************************************************************************/
template<class T>
double  GGetNearstDisPToPointSet2D(T pointset[],int nCount,const T &p,T &q,int *nIndex)
{
	int i;
	double pencode=DBL_MAX,td;
	T tq,cq;
	tq=pointset[0];
	for(i=0;i<nCount-1;i++)
	{			
		td = GGetNearestDisOfPtToLine(pointset[i].x,pointset[i].y,pointset[i+1].x,
			pointset[i+1].y,p.x,p.y,&(tq.x),&(tq.y),false);
		if(td<pencode)
		{			
			if(nIndex) *nIndex = i;
			pencode=td;
			cq=tq;
		}
	}
	q=cq;
	return pencode;
} 

/*******************************************************************************
函数名   : <GGetAllLen2D>
功能     : <函数实现功能> 
参数     :  
           [IN|OUT] 参数1 : 参数说明
           [IN|OUT] 参数2 : 参数说明
           ...
返回值   : 折线串的长度

*******************************************************************************/
template<class T>
double  GGetAllLen2D(T *pointset, int nCount)
{
	double len=0;
	for (int i=0;i<nCount-1;i++)
	{
		len+=GGetDisOf2P2D(pointset[i].x,pointset[i].y,pointset[i+1].x,pointset[i+1].y);
	}
	return len;
}

template<class T>
double  GGetAllLen3D(T *pointset, int nCount)
{
	double len=0;
	for (int i=0;i<nCount-1;i++)
	{
		len+=GGetDisOf2P3D(pointset[i].x,pointset[i].y,pointset[i].z,pointset[i+1].x,pointset[i+1].y,pointset[i+1].z);
	}
	return len;
}


//点序列的长度计算工具
template<class T>
class CLengthCalculator2D
{
public:
	CArray<double,double> m_arrLens;
	CArray<T,T> m_arrPts;

	CLengthCalculator2D(){}
	~CLengthCalculator2D(){}
	void Init(T *pointset, int nCount)
	{
		m_arrLens.SetSize(nCount);
		m_arrPts.SetSize(nCount);
		if( nCount>0 )
		{
			m_arrLens[0] = 0;
			m_arrPts[0] = pointset[0];
			for (int i=1;i<nCount;i++)
			{
				m_arrLens[i] = m_arrLens[i-1] + GGet2DDisOf2P(pointset[i-1],pointset[i]);
				m_arrPts[i] = pointset[i];
			}
		}
	}
	double GetAllLen()
	{
		int npt = m_arrLens.GetSize();
		if( npt>0 )
			return m_arrLens[npt-1];
		return 0;
	}
	T GetLenPt(double len, int *pIndex, double *t)
	{
		int npt = m_arrLens.GetSize();
		for (int i=0;i<npt-1;i++)
		{
			if( len>=m_arrLens[i] && len<m_arrLens[i+1] )
			{
				T pt0, pt1, pt;
				pt0 = m_arrPts[i];
				pt1 = m_arrPts[i+1];
				double k = (len-m_arrLens[i])/(m_arrLens[i+1]-m_arrLens[i]);
				pt.x = pt0.x + k * (pt1.x-pt0.x);
				pt.y = pt0.y + k * (pt1.y-pt0.y);
				pt.z = pt0.z + k * (pt1.z-pt0.z);

				if( pIndex )*pIndex = i;
				if( t )*t = k;

				return pt;
			}
		}

		if( pIndex )*pIndex = -1;
		if( t )*t = 0;

		return T();
	}
	double GetLen(int index, double t, T *pRet)
	{
		int npt = m_arrLens.GetSize();

		if( index<0 || index>npt )
			return 0;

		if( index==npt && t==0.0 )
		{
			index = npt-1;
			t = 1.0;
		}
		else
		{
			return 0;
		}

		double dis = 0;
		for (int i=0; i<npt-1 && i<index; i++)
		{
			dis += m_arrLens[i];
		}

		dis += m_arrLens[i]*t;

		if( pRet )
		{
			T pt0, pt1, pt;
			pt0 = m_arrPts[i];
			pt1 = m_arrPts[i+1];
			
			pt = pt0;
			pt.x = pt0.x + t * (pt1.x-pt0.x);
			pt.y = pt0.y + t * (pt1.y-pt0.y);
			pt.z = pt0.z + t * (pt1.z-pt0.z);

			*pRet = pt;
		}

		return dis;
	}
};


template<class T>
T GGetMiddlePt(T pt1, T pt2)
{
	T pt3 = pt1;
	pt3.x = (pt1.x+pt2.x)*0.5;
	pt3.y = (pt1.y+pt2.y)*0.5;
	pt3.z = (pt1.z+pt2.z)*0.5;
	return pt3;
}


template<class T>
void TransformPointsWith44Matrix(const double *matrix, T *pts, int num)
{
	double vo[4]={ 0,0,0,1.0 };
	
	for( int i=0; i<num; i++)
	{
		vo[0] = pts->x, vo[1] = pts->y, vo[2] = pts->z;
		pts->x = vo[0]*matrix[0] + vo[1]*matrix[1] + vo[2]*matrix[2] + matrix[3];
		pts->y = vo[0]*matrix[4] + vo[1]*matrix[5] + vo[2]*matrix[6] + matrix[7];
		pts->z = vo[0]*matrix[8] + vo[1]*matrix[9] + vo[2]*matrix[10] + matrix[11];
		
		pts++;
	}
}

template<class T>
void TransformPointsWith33Matrix(const double *matrix, T *pts, int num)
{
	double vo[3]={ 0,0,1.0 };
	
	for( int i=0; i<num; i++)
	{
		vo[0] = pts->x, vo[1] = pts->y;
		pts->x = vo[0]*matrix[0] + vo[1]*matrix[1] + matrix[2];
		pts->y = vo[0]*matrix[3] + vo[1]*matrix[4] + matrix[5];

		pts++;
	}
}

//将点列反向
template<class T>
void ReversePoints(T *pts, int npt)
{
	int npt2 = npt/2;
	for (int i=0; i<npt2; i++)
	{
		T pt = pts[i];
		pts[i] = pts[npt-1-i];
		pts[npt-1-i] = pt;
	}
}

// 计算平面点集的凸壳
// hull_isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles
template<class T>
double hull_isLeft( T P0, T P1, T P2 )
{
    return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}
//===================================================================


template<class T>
class _DummyForTemplateFuncAddress
{
public:
	_DummyForTemplateFuncAddress(){

	}
	~_DummyForTemplateFuncAddress(){

	}
	static int compare_func(const void *p1, const void *p2)
	{
		const T *t1 = (const T*)p1;
		const T *t2 = (const T*)p2;
		
		if( t1->x<t2->x )
			return -1;
		if( t1->x>t2->x )
			return 1;
		
		if( t1->y<t2->y )
			return -1;
		if( t1->y>t2->y )
			return 1;
		
		return 0;
	}
};


template<class T>
void GSortXY( T* P, int n)
{
	_DummyForTemplateFuncAddress<T> t;
	qsort(P,n,sizeof(T),t.compare_func);
}

// GGetHull2D(): Andrew's monotone chain 2D convex hull algorithm
//     Input:  P[] = an array of 2D points
//                   presorted by increasing x- and y-coordinates
//             n = the number of points in P[]
//     Output: H[] = an array of the convex hull vertices (max is n)
//     Return: the number of points in H[]
template<class T>
int GGetHull2D( T* P, int n, T* H )
{
    // the output array H[] will be used as the stack
    int    bot=0, top=(-1);  // indices for bottom and top of the stack
    int    i;                // array scan index

	GSortXY(P,n);
	
    // Get the indices of points with min x-coord and min|max y-coord
    int minmin = 0, minmax;
    double xmin = P[0].x;
    for (i=1; i<n; i++)
        if (P[i].x != xmin) break;
		minmax = i-1;
		if (minmax == n-1) {       // degenerate case: all x-coords == xmin
			H[++top] = P[minmin];
			if (P[minmax].y != P[minmin].y) // a nontrivial segment
				H[++top] = P[minmax];
			H[++top] = P[minmin];           // add polygon endpoint
			return top+1;
		}
		
		// Get the indices of points with max x-coord and min|max y-coord
		int maxmin, maxmax = n-1;
		float xmax = P[n-1].x;
		for (i=n-2; i>=0; i--)
			if (P[i].x != xmax) break;
			maxmin = i+1;
			
			// Compute the lower hull on the stack H
			H[++top] = P[minmin];      // push minmin point onto stack
			i = minmax;
			while (++i <= maxmin)
			{
				// the lower line joins P[minmin] with P[maxmin]
				if (hull_isLeft( P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
					continue;          // ignore P[i] above or on the lower line
				
				while (top > 0)        // there are at least 2 points on the stack
				{
					// test if P[i] is left of the line at the stack top
					if (hull_isLeft( H[top-1], H[top], P[i]) > 0)
						break;         // P[i] is a new hull vertex
					else
						top--;         // pop top point off stack
				}
				H[++top] = P[i];       // push P[i] onto stack
			}
			
			// Next, compute the upper hull on the stack H above the bottom hull
			if (maxmax != maxmin)      // if distinct xmax points
				H[++top] = P[maxmax];  // push maxmax point onto stack
			bot = top;                 // the bottom point of the upper hull stack
			i = maxmin;
			while (--i >= minmax)
			{
				// the upper line joins P[maxmax] with P[minmax]
				if (hull_isLeft( P[maxmax], P[minmax], P[i]) >= 0 && i > minmax)
					continue;          // ignore P[i] below or on the upper line
				
				while (top > bot)    // at least 2 points on the upper stack
				{
					// test if P[i] is left of the line at the stack top
					if (hull_isLeft( H[top-1], H[top], P[i]) > 0)
						break;         // P[i] is a new hull vertex
					else
						top--;         // pop top point off stack
				}
				H[++top] = P[i];       // push P[i] onto stack
			}
			if (minmax != minmin)
				H[++top] = P[minmin];  // push joining endpoint onto stack
			
			return top+1;
}

/**********************************************
输入参数：
返回值：如果以有向线段起点为起点，沿线段方向的射线没有交点，则返回false,否则返回true,并把交点保存在传入参数*ret中
***************************************************/
bool EXPORT_SMARTVIEW GGetRay1IntersectRay2(double  lfX0,double lfY0,double lfX1,double lfY1,
						   double lfX2,double lfY2,double lfX3,double lfY3,double *plfXRet, double *plfYRet);

/****************************************
利用向量运算，求角平分线上的某一点
**********************************/
bool EXPORT_SMARTVIEW GGetPtInAngleBisector2D(double  lfXorg,double lfYorg,double lfX1,double lfY1,double lfX2,double lfY2,double *plfXRet,double *plfYRet);

};
class EXPORT_SMARTVIEW CTempTolerance//已测试
{
public:
	enum
	{
		flagDistance = 0,
		flagParallel = 1,
		flagAngle = 2
	};
    CTempTolerance(double lfTolerance,int nFlag=flagDistance):m_nFlag(nFlag)
    {
		if(nFlag==flagDistance)
		{
			m_lfSave = GraphAPI::GetDisTolerance();
			GraphAPI::SetDisTolerance(lfTolerance);
		}
		else if (nFlag==flagParallel)
		{
			m_lfSave = GraphAPI::GetPralTolerance();
			GraphAPI::SetPralTolerance(lfTolerance);
		}
		else if (nFlag==flagAngle)
		{
			m_lfSave = GraphAPI::GetAngTolerance();
			GraphAPI::SetAngTolerance(lfTolerance);
		}
	}
	~CTempTolerance()
	{
		if(m_nFlag==0)
			GraphAPI::SetDisTolerance(m_lfSave); 
		else if (m_nFlag==1)
			GraphAPI::SetPralTolerance(m_lfSave);
		else if (m_nFlag==2)
			GraphAPI::SetAngTolerance(m_lfSave);
	}
	
private:
	double m_lfSave;
	int m_nFlag;
};


//xx

void EXPORT_SMARTVIEW WriteProfileDouble(LPCTSTR regpath, LPCTSTR regitem, double v);

double EXPORT_SMARTVIEW GetProfileDouble(LPCTSTR regpath, LPCTSTR regitem, double vdef);

CString EXPORT_SMARTVIEW StrFromResID(UINT id);

// 点(x,y)以(x0,y0)为原点逆时针旋转angle角，单位为弧度
void EXPORT_SMARTVIEW GRotate2DPT(double *px, double *py, int sum, double x0, double y0, double angle);

void EXPORT_SMARTVIEW CalcAffineParams(double *x1,double *y1,double *d1,double *d2,int n,double *r1,double *r2);
void EXPORT_SMARTVIEW CalcAffineParams(double *x1,double *y1,double *z1,double *x2,double *y2,double *z2, int n,double *r1,double *r2, double *r3);

void EXPORT_SMARTVIEW Matrix44FromZoom(double kx, double ky, double kz, double matrix[16]);
void EXPORT_SMARTVIEW Matrix44FromMove(double dx, double dy, double dz, double matrix[16]);
void EXPORT_SMARTVIEW Matrix44FromRotate(const PT_3D *pt, const PT_3D *dpt, double seita, double matrix[16]);
void EXPORT_SMARTVIEW Matrix44FromRotate(const PT_3D *pt, double seita, double matrix[16]);

//依次变换：缩放、旋转、平移
void EXPORT_SMARTVIEW Matrix44FromTransform(double m[16], float kx, float ky, float angle, double x, double y, double z);

void EXPORT_SMARTVIEW Matrix33FromZoom(double kx, double ky, double matrix[9]);
void EXPORT_SMARTVIEW Matrix33FromMove(double dx, double dy, double matrix[9]);
void EXPORT_SMARTVIEW Matrix33FromRotate(const PT_2D *pt, double seita, double matrix[9]);

long EXPORT_SMARTVIEW FixPreciError(double x);

void EXPORT_SMARTVIEW ConvertCharToWstring(const char* from, wstring &to, UINT codepage);
void EXPORT_SMARTVIEW ConvertWcharToString(const wchar_t* from, string &to, 
										 UINT codepage, bool* datalost, char unknownchar);

CStringA EXPORT_SMARTVIEW ConvertWCharToChar(const wchar_t* text);
CStringW EXPORT_SMARTVIEW ConvertCharToWChar(const char* text);

CString EXPORT_SMARTVIEW ConvertCharToTChar(const char* text);
CStringA EXPORT_SMARTVIEW ConvertTCharToChar(LPCTSTR text);

#define PIXEL(x)	FixPreciError((x)-0.5)

void EXPORT_SMARTVIEW qsort2(
			void *base,
			unsigned num,
			unsigned width,
			void *param,
			int (__cdecl *comp)(const void *, const void *, void *param)
    );

class EXPORT_SMARTVIEW CDebugTime
{
public:
	CDebugTime();
	~CDebugTime();
	void PrintfTime(LPCTSTR title);

	DWORD m_time0, m_time1;
};

#endif // FUNCTIONS_123i124j1239423040239402349_