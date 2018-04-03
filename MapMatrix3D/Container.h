#include "SmartViewBaseType.h"
typedef struct Lnode
{
	double dis;//此点至起点的距离
	PT_3D pt3d;
    struct Lnode *next;
	
} LNode,*LinkList;
void initLList(LinkList L);

int ArrayAddBase(void **ppts, int *len, int *num, void *add, int size, int grow);



int ArrayAdd3dPt(PT_3D **ppts, int *len, int *num, PT_3D *pt);

double DistanceOfPoints(PT_3D *pt1, PT_3D *pt2, int b3d);//两点之间的距离

void addLnode(LinkList *pL,PT_3D *start,PT_3D *newpt);

void destroyList(LinkList *L);

int getLineIntersectLine(double x0, double y0,double z0, double vx0, double vy0,double vz0,
						 double x1, double y1,double z1, double vx1, double vy1,double vz1,
						 double *x, double *y,double *z);
int GLineIntersectLineSegInLimit1(double x0, double y0, double z0, double x1, double y1, double z1,
								  double x2, double y2, double z2, double x3, double y3, double z3,
								  double *x, double *y, double *z, double toler, bool flag=false,double*t=NULL);

void insertPoint(PT_3D **ptP,int *len,int *num,PT_3D *pnt,int index);
