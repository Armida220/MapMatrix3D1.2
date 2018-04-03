#include <stdafx.h>
#include "math.h"
#include "container.h"
#define _FABS(x) ((x)>=0?(x):(-(x)))
void initLList(LinkList L)
{
	L=NULL;	
}
int ArrayAddBase(void **ppts, int *len, int *num, void *add, int size, int grow)
{
	if( (*num+1)>=*len )
	{
		void *newpts = malloc(size*(*len+grow));
		if( !newpts )return 1;
		if( *ppts!=NULL )
		{
			memcpy(newpts,*ppts,size*(*len));
			free(*ppts);
		}
		*ppts = newpts;
		*len += grow;
	}
	memcpy((unsigned char*)(*ppts)+size*(*num),add,size);
	*num = *num + 1;
	return 0;
}

int ArrayAdd3dPt(PT_3D **ppts, int *len, int *num, PT_3D *pt)
{
	return ArrayAddBase((void**)ppts,len,num,pt,sizeof(PT_3D),256);
}
double DistanceOfPoints(PT_3D *pt1, PT_3D *pt2, int b3d)//两点之间的距离
{
	if( b3d==0 )
		return sqrt((pt1->x-pt2->x)*(pt1->x-pt2->x)+(pt1->y-pt2->y)*(pt1->y-pt2->y));
	else
		return sqrt((pt1->x-pt2->x)*(pt1->x-pt2->x)+(pt1->y-pt2->y)*(pt1->y-pt2->y)+(pt1->z-pt2->z)*(pt1->z-pt2->z));
}
void addLnode(LinkList *pL,PT_3D *start,PT_3D *newpt)
{
	LNode *p=*pL;
	LNode 	*q=NULL;
	LNode *newNode=(LNode *)malloc(sizeof(LNode));
	newNode->dis=DistanceOfPoints(start,newpt,1);
	newNode->next=NULL;
	newNode->pt3d=*newpt;
	if (*pL==NULL)
	{		
		*pL=newNode;
	}
	else
	{
		q=(*pL)->next;
		if (newNode->dis>(*pL)->dis)
		{
			newNode->next=*pL;
			*pL=newNode;
		}
		else{
			for (;q!=NULL;p=p->next,q=q->next)
			{
				if (newNode->dis>q->dis)
				{
					break;					
				}
			}
			p->next=newNode;
			newNode->next=q;
			
			
		}		
	}	
}
void destroyList(LinkList *L)
{
	LNode *p=NULL;
	while(*L!=NULL)
		{
			p=(*L)->next;			
			free(*L);
			*L=p;			
		}
	*L=NULL;

}
int getLineIntersectLine(double x0, double y0,double z0, double vx0, double vy0,double vz0,
					    double x1, double y1,double z1, double vx1, double vy1,double vz1,
						double *x, double *y,double *z)
{	
	if (((x1-x0)*vy0-(y1-y0)*vx0)*((x1+vx1-x0)*vy0-(y1+vy1-y0)*vx0)<0&&
		((x0-x1)*vy1-(y0-y1)*vx1)*((x0+vx0-x1)*vy1-(y0+vy0-y1)*vx1)<0)
	{
		double delta = vx0*vy1-vy0*vx1;
		double t= ( (x1-x0)*vy1-(y1-y0)*vx1 )/delta;
		if(x)*x = x0 + t*vx0;
		
	    if(y)*y = y0 + t*vy0;
		if (z)*z=z0+t*vz0;
		return 1;
	}
	else 
		return 0;
}
//返回值：1表示在首条线段（P0P1）的中间有交点,2表示在首条线段的首端点，3表尾端点，0表示无交点
//flag用于标志z高程取值方式，是在首条线段上还是在第二条线段上
int GLineIntersectLineSegInLimit1(double x0, double y0, double z0, double x1, double y1, double z1,
								  double x2, double y2, double z2, double x3, double y3, double z3,
								  double *x, double *y, double *z, double toler, bool flag,double*t)
{
	double vector1x = x1-x0, vector1y = y1-y0;
	double vector2x = x3-x2, vector2y = y3-y2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	int nmark=0;
	if( delta<1e-10 && delta>-1e-10 )return nmark;
	double t1 = ( (x2-x0)*vector2y-(y2-y0)*vector2x )/delta;
	double xr = x0 + t1*vector1x, yr = y0 + t1*vector1y;	
	double t2 = ( (x2-x0)*vector1y-(y2-y0)*vector1x )/delta;
	if( t2<0 || t2>1 )
	{
		if( _FABS(xr-x2)<=toler && _FABS(yr-y2)<=toler );
		else if( _FABS(xr-x3)<=toler && _FABS(yr-y3)<=toler );
		else return nmark;
	}
	
	if( t1<=0 || t1>=1 )
	{
		if( _FABS(xr-x0)<=toler && _FABS(yr-y0)<=toler ) nmark=2;
		else if( _FABS(xr-x1)<=toler && _FABS(yr-y1)<=toler )nmark=3;
		else return nmark;
	}
	if(nmark==0)
	    nmark=1;
	if(x)*x = xr;
	if(y)*y = yr;
	if(t)*t = t1;
	if (flag)
	{
		if (z)*z=z0+t1*(z1-z0);
	}
	else
		if (z)*z=z2+t2*(z3-z2);	
	return nmark;
}

void insertPoint(PT_3D **ptP,int *len,int *num,PT_3D *pnt,int index)
{
	int i;
	ArrayAdd3dPt(ptP, len, num, pnt);
	
	for (i=*num-2;i>=index;i--)
	{
		(*ptP)[i+1]=(*ptP)[i];
	}
	(*ptP)[i+1]=*pnt;
}
