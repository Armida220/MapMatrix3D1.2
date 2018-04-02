#include <stdafx.h>
#include "SmartViewFunctions.h"
#include <float.h>
#include <math.h>
#include "matrix.h"
#include "TextRender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _ttof _tstof


extern BOOL LoadMyString(CString& str, UINT uID);

namespace  GraphAPI
{
	double EXPORT_SMARTVIEW g_lfDisTolerance = 1e-4;
	double EXPORT_SMARTVIEW g_lfPraTolerance = 1e-10;
	double EXPORT_SMARTVIEW g_lfAngTolerance = 1e-10;
	double EXPORT_SMARTVIEW g_lfZTolerance = 1e-4;
	double EXPORT_SMARTVIEW g_lfAreaTolerance = 1e-4;
}


void WriteProfileDouble(LPCTSTR regpath, LPCTSTR regitem, double v)
{
	CString str;
	str.Format(_T("%.12lf"),v);
	if( AfxGetApp() )AfxGetApp()->WriteProfileString(regpath,regitem,str);
}

double GetProfileDouble(LPCTSTR regpath, LPCTSTR regitem, double vdef)
{
	if( !AfxGetApp() )
		return vdef;
	CString str = AfxGetApp()->GetProfileString(regpath,regitem,NULL);
	if( str.GetLength()<=0 )return vdef;
	return _ttof(str);
}

extern AFX_EXTENSION_MODULE SmartViewDLL;

BOOL LoadMyString(CString &str, UINT uID)
{
	if( !SmartViewDLL.hResource )return FALSE;
	TCHAR tmpstr[256];
	
	::LoadString(SmartViewDLL.hResource,uID,tmpstr,sizeof(tmpstr)-1);
	str = tmpstr;
	return TRUE;
}

CString StrFromResID(UINT id)
{
	CString str;
	str.LoadString(id);
	return str;
}



void Matrix33FromZoom(double kx, double ky, double matrix[9])
{
	matrix_toIdentity(matrix,3);
	matrix[0] = kx;
	matrix[4] = ky;
}

void Matrix33FromMove(double dx, double dy, double matrix[9])
{
	matrix_toIdentity(matrix,3);
	matrix[2] = dx;
	matrix[5] = dy;
}

void Matrix33FromRotate(const PT_2D *pt, double seita, double matrix[9])
{
	matrix_toIdentity(matrix,3);
	
	double cosq = cos(seita);double sinq = sin(seita);
	
	//平移矩阵
	double mt[9]={1.0,0.0,-pt->x,  0.0,1.0,-pt->y,  0.0,0.0,1.0};
	//绕z轴旋转q角的旋转矩阵
	double mrzq[9]={cosq,-sinq,0.0,  sinq,cosq,0.0,  0.0,0.0,1.0  };
	
	//以上各个矩阵的逆矩阵
	//平移矩阵
	double mtr[9]={1.0,0.0,pt->x,  0.0,1.0,pt->y,  0.0,0.0,1.0};
	//用于相乘的临时矩阵
	double mr1[9],mr2[9];
	
	matrix_multiply(mtr,mrzq,3,mr1);
	matrix_multiply(mr1,mt,3,mr2);
	
	memcpy(matrix,mr2,sizeof(mr2));
}


void Matrix44FromZoom(double kx, double ky, double kz, double matrix[16])
{
	matrix_toIdentity(matrix,4);
	matrix[0] = kx;
	matrix[5] = ky;
	matrix[10] = kz;
}

void Matrix44FromMove(double dx, double dy, double dz, double matrix[16])
{
	matrix_toIdentity(matrix,4);
	matrix[3] = dx;
	matrix[7] = dy;
	matrix[11] = dz;
}

void Matrix44FromRotate(const PT_3D *pt, double seita, double matrix[16])
{
	double cosq = cos(seita);
	double sinq = sin(seita);

	//绕z轴旋转q角的旋转矩阵
	double mrzq[16]={cosq,-sinq,0.0,0.0,  sinq,cosq,0.0,0.0,  0.0,0.0,1.0,0.0,  0.0,0.0,0.0,1.0};

	if( pt==NULL )
	{
		memcpy(matrix,mrzq,sizeof(mrzq));
		return;
	}	

	//平移矩阵
	double mt[16]={1.0,0.0,0.0,-pt->x,  0.0,1.0,0.0,-pt->y,  0.0,0.0,1.0,0,  0.0,0.0,0.0,1.0};
	
	//以上各个矩阵的逆矩阵
	//平移矩阵
	double mtr[16]={1.0,0.0,0.0,pt->x,  0.0,1.0,0.0,pt->y,  0.0,0.0,1.0,0,  0.0,0.0,0.0,1.0};
	//用于相乘的临时矩阵
	double mr1[16],mr2[16];
	
	matrix_multiply(mtr,mrzq,4,mr1);
	matrix_multiply(mr1,mt,4,mr2);
	
	memcpy(matrix,mr2,sizeof(mr2));
}


void Matrix44FromRotate(const PT_3D *pt, const PT_3D *dpt, double seita, double matrix[16])
{
	matrix_toIdentity(matrix,4);
	
	double ca = sqrt(dpt->y*dpt->y+dpt->z*dpt->z);
	double cb = sqrt(dpt->x*dpt->x+dpt->y*dpt->y+dpt->z*dpt->z);
	if( fabs(ca)<1e-8 && fabs(cb)<1e-8 )return;
	
	double cosa,sina,cosb,sinb;
	
	if( fabs(ca)<1e-8 ){ cosa = 1; sina = 0; }
	else { cosa = dpt->z/ca; sina = dpt->y/ca; }
	
	if( fabs(cb)<1e-8 ){ cosb = 1; sinb = 0; }
	else { cosb = ca/cb; sinb = dpt->x/cb; }
	
	double cosq = cos(seita);double sinq = sin(seita);
	
	//平移矩阵
	double mt[16]={1.0,0.0,0.0,-pt->x,  0.0,1.0,0.0,-pt->y,  0.0,0.0,1.0,-pt->z,  0.0,0.0,0.0,1.0};
	//绕x轴旋转a角的旋转矩阵
	double mrxa[16]={1.0,0.0,0.0,0.0,  0.0,cosa,-sina,0.0,  0.0,sina,cosa,0.0,  0.0,0.0,0.0,1.0};
	//绕y轴旋转b角的旋转矩阵
	double mryb[16]={cosb,0.0,sinb,0.0,  0.0,1.0,0.0,0.0,  -sinb,0.0,cosb,0.0,  0.0,0.0,0.0,1.0};
	//绕z轴旋转q角的旋转矩阵
	double mrzq[16]={cosq,-sinq,0.0,0.0,  sinq,cosq,0.0,0.0,  0.0,0.0,1.0,0.0,  0.0,0.0,0.0,1.0};
	
	//以上各个矩阵的逆矩阵
	//平移矩阵
	double mtr[16]={1.0,0.0,0.0,pt->x,  0.0,1.0,0.0,pt->y,  0.0,0.0,1.0,pt->z,  0.0,0.0,0.0,1.0};
	//绕x轴旋转a角的旋转矩阵
	double mrxar[16]={1.0,0.0,0.0,0.0,  0.0,cosa,sina,0.0,  0.0,-sina,cosa,0.0,  0.0,0.0,0.0,1.0};
	//绕y轴旋转b角的旋转矩阵
	double mrybr[16]={cosb,0.0,-sinb,0.0,  0.0,1.0,0.0,0.0,  sinb,0.0,cosb,0.0,  0.0,0.0,0.0,1.0};
	//用于相乘的临时矩阵
	double mr1[16],mr2[16];
	
	matrix_multiply(mtr,mrxar,4,mr1);
	matrix_multiply(mr1,mrybr,4,mr2);
	matrix_multiply(mr2,mrzq,4,mr1);
	matrix_multiply(mr1,mryb,4,mr2);
	matrix_multiply(mr2,mrxa,4,mr1);
	matrix_multiply(mr1,mt,4,mr2);
	
	memcpy(matrix,mr2,sizeof(mr2));
}

//更加快速的函数
void Matrix44FromTransform(double m[16], float kx, float ky, float angle, double x, double y, double z)
{
	double cosa = cos(angle), sina = sin(angle);
	
	m[0] = kx*cosa; m[1] = -sina*ky; m[2] = 0; m[3] = x;
	m[4] = kx*sina; m[5] = cosa*ky; m[6] = 0; m[7] = y;
	m[8] = 0; m[9] = 0; m[10] = 1; m[11] = z;
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}

long FixPreciError(double x)
{
	long n = floor(x);
	double e = x-n;
	
	if( e>=0.5 )
	{
		n += 1;
	}
	
	return n;
}


//

bool GraphAPI::GIsClockwise(double lfAng0, double lfAng1, double lfAng2)
{
	if( lfAng0<=lfAng2 )
	{
		if( lfAng0<=lfAng1 && lfAng1<=lfAng2 )return false;
		if( lfAng1<lfAng0 )return true;
		if( lfAng1>lfAng2 )return true;
	}
	else
	{
		if( lfAng0>=lfAng1 && lfAng1>=lfAng2 )return true;
		if( lfAng1<lfAng2 )return false;
		if( lfAng1>lfAng0 )return false;
	}
	return false;
}



bool GraphAPI::GCalTanP2ArcPara(double lfVx0,double lfVy0,double lfX1,double lfY1,double lfX2,double lfY2,
				   double *plfXc,double *plfYc,double *plfR,double *plfAng1,double *plfAng2)
{
	double vx1=lfX2-lfX1, vy1=lfY2-lfY1;
	double x1=(lfX2+lfX1)/2, y1=(lfY2+lfY1)/2;
	if( !GGetLineIntersectLine( lfX1, lfY1, lfVx0, lfVy0,
		x1,y1,-vy1,vx1,plfXc,plfYc,NULL) )
		return false;
	
	if(plfR)*plfR = sqrt((*plfXc-lfX1)*(*plfXc-lfX1)+(*plfYc-lfY1)*(*plfYc-lfY1));
	if(plfAng1)*plfAng1 = GGetAngle(*plfXc,*plfYc,lfX1,lfY1);
	if(plfAng2)*plfAng2 = GGetAngle(*plfXc,*plfYc,lfX2,lfY2);
	
	return true; 
}


bool GraphAPI::GGetLineIntersectLine(double lfX0, double lfY0, double lfVx0, double lfVy0,
								  double lfX1, double lfY1, double lfVx1, double lfVy1,
								double *plfX, double *plfY ,double *plfT)
{
	double delta = lfVx0*lfVy1-lfVy0*lfVx1;
	if ((fabs(lfVx0)+fabs(lfVy0))*(fabs(lfVx1)+fabs(lfVy1))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(lfVx0)+fabs(lfVy0))*(fabs(lfVx1)+fabs(lfVy1)));
	if( unitdelta<g_lfPraTolerance && delta>-g_lfPraTolerance )return false;
	
	double t1 = ( (lfX1-lfX0)*lfVy1-(lfY1-lfY0)*lfVx1 )/delta;
	if(plfX)*plfX = lfX0 + t1*lfVx0;
	if(plfY)*plfY = lfY0 + t1*lfVy0;
	if(plfT)*plfT = t1;	
	return true;
}

bool GraphAPI::GGetLineIntersectLineSeg0(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY,double *plfT0, double *plfT1)
{
	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = lfX3-lfX2, vector2y = lfY3-lfY2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return false;
	double t1 = ( (lfX2-lfX0)*vector2y-(lfY2-lfY0)*vector2x )/delta;
	double xr = lfX0 + t1*vector1x, yr = lfY0 + t1*vector1y;
	if( t1<0 || t1>1 )
	{
		if( fabs(xr-lfX0)<=g_lfDisTolerance && fabs(yr-lfY0)<=g_lfDisTolerance );
		else if( fabs(xr-lfX1)<=g_lfDisTolerance && fabs(yr-lfY1)<=g_lfDisTolerance );
		else return false;
	}
	double t2 = ( (lfX2-lfX0)*vector1y-(lfY2-lfY0)*vector1x )/delta;
// 	if( t2<0 || t2>1 )
// 	{
// 		if( fabs(xr-lfX2)<=g_lfDisTolerance && fabs(yr-lfY2)<=g_lfDisTolerance );
// 		else if( fabs(xr-lfX3)<=g_lfDisTolerance && fabs(yr-lfY3)<=g_lfDisTolerance );
// 		else return false;
// 	}
	if(plfX)*plfX = xr;
	if(plfY)*plfY = yr;
	if(plfT0)*plfT0 = t1;
	if(plfT1)*plfT1 = t2;
	return true;
}

bool GraphAPI::GGetLineIntersectLineSeg(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY,double *plfT)
{
	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = lfX3-lfX2, vector2y = lfY3-lfY2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return false;
	double t1 = ( (lfX2-lfX0)*vector2y-(lfY2-lfY0)*vector2x )/delta;
	double xr = lfX0 + t1*vector1x, yr = lfY0 + t1*vector1y;
	if( t1<0 || t1>1 )
	{
		if( fabs(xr-lfX0)<=g_lfDisTolerance && fabs(yr-lfY0)<=g_lfDisTolerance );
		else if( fabs(xr-lfX1)<=g_lfDisTolerance && fabs(yr-lfY1)<=g_lfDisTolerance );
		else return false;
	}
	double t2 = ( (lfX2-lfX0)*vector1y-(lfY2-lfY0)*vector1x )/delta;
	if( t2<0 || t2>1 )
	{
		if( fabs(xr-lfX2)<=g_lfDisTolerance && fabs(yr-lfY2)<=g_lfDisTolerance );
		else if( fabs(xr-lfX3)<=g_lfDisTolerance && fabs(yr-lfY3)<=g_lfDisTolerance );
		else return false;
	}
	if(plfX)*plfX = xr;
	if(plfY)*plfY = yr;
	if(plfT)*plfT = t1;
	return true;
}

bool GraphAPI::GGetLineIntersectLineSeg(double lfX0,double lfY0, double lfX1, double lfY1,double lfX2,double lfY2,double lfX3,double lfY3, double *plfX, double *plfY,double *plfT0, double *plfT1)
{
	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = lfX3-lfX2, vector2y = lfY3-lfY2;
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return false;
	double t1 = ( (lfX2-lfX0)*vector2y-(lfY2-lfY0)*vector2x )/delta;
	double xr = lfX0 + t1*vector1x, yr = lfY0 + t1*vector1y;
	if( t1<0 || t1>1 )
	{
		if( fabs(xr-lfX0)<=g_lfDisTolerance && fabs(yr-lfY0)<=g_lfDisTolerance );
		else if( fabs(xr-lfX1)<=g_lfDisTolerance && fabs(yr-lfY1)<=g_lfDisTolerance );
		else return false;
	}
	double t2 = ( (lfX2-lfX0)*vector1y-(lfY2-lfY0)*vector1x )/delta;
	if( t2<0 || t2>1 )
	{
		if( fabs(xr-lfX2)<=g_lfDisTolerance && fabs(yr-lfY2)<=g_lfDisTolerance );
		else if( fabs(xr-lfX3)<=g_lfDisTolerance && fabs(yr-lfY3)<=g_lfDisTolerance );
		else return false;
	}
	if(plfX)*plfX = xr;
	if(plfY)*plfY = yr;
	if(plfT0)*plfT0 = t1;
	if(plfT1)*plfT1 = t2;
	return true;
}

void  GraphAPI::GGetPerpendicular(double lfX0, double lfY0, double lfX1, double lfY1,
					 double lfXt, double lfYt, double *plfX, double *plfY,double *plfT)
{
	if (plfX==NULL||plfY==NULL)
	{
		return;
	}
	double dx,dy,t;
	
	dx = lfX1-lfX0; dy = lfY1-lfY0;
	if( fabs(dx)<g_lfDisTolerance && fabs(dy)<g_lfDisTolerance )
	{
		*plfX = lfX0; *plfY = lfY0;
		if (plfT) *plfT = 0;
		return;
	}
	
	t = (dx*(lfXt-lfX0 )+dy*(lfYt-lfY0))/(dx*dx+dy*dy);
	if (plfT) *plfT = t;	
	*plfX= lfX0 + dx*t;	*plfY= lfY0 + dy*t;
}


void  GraphAPI::GGetPerpendicular3D(double lfX0, double lfY0, double lfZ0, double lfX1, double lfY1, double lfZ1,
					   double lfXt, double lfYt, double lfZt, double *plX, double *plY, double *plZ)
{
	if (plX==NULL||plY==NULL||plZ==NULL)
	{
		return;
	}
	double dx=lfX1-lfX0, dy=lfY1-lfY0, dz=lfZ1-lfZ0;
	double k = dx*dx + dy*dy;
	if( fabs(k)<g_lfDisTolerance )
	{
		*plX = lfX0; *plY = lfY0; *plZ = lfZ0;
		return;
	}
	
	double t = (dx*(lfXt-lfX0 )+dy*(lfYt-lfY0))/k;
	*plX= lfX0 + dx*t;	*plY= lfY0 + dy*t; *plZ= lfZ0 + dz*t;
}


double GraphAPI::GGetDisofPtToLine(double x0, double y0, double x1, double y1,
					  double xt, double yt)
{
	double x,y;
	GGetPerpendicular(x0,y0,x1,y1,xt,yt,&x,&y,NULL);
	return 	 sqrt((xt-x)*(xt-x)+(yt-y)*(yt-y));
}


double GraphAPI::GGetNearestDisOfPtToLine(double lfX0, double lfY0, double lfX1, double lfY1,
							 double lfXt, double lfYt, 
							 double *plfXr, double *plfYr,
							 bool bFootCanOut)
{
	double x,y;
	GGetPerpendicular(lfX0,lfY0,lfX1,lfY1,lfXt,lfYt,&x,&y,NULL);
	
	bool bOut = false;
	double d1 = fabs(lfX0-x)+fabs(lfY0-y);
	double d2 = fabs(lfX1-x)+fabs(lfY1-y);
	double d3 = fabs(lfX0-lfX1)+fabs(lfY0-lfY1);
	if( d1>d3 || d2>d3 )bOut = true;
	
	if( !bOut||bFootCanOut )
	{
		if(plfXr)*plfXr=x; 
		if(plfYr)*plfYr=y; 
		return sqrt((lfXt-x)*(lfXt-x)+(lfYt-y)*(lfYt-y));
	}
	
	d1 = (lfXt-lfX0)*(lfXt-lfX0)+(lfYt-lfY0)*(lfYt-lfY0);
	d2 = (lfXt-lfX1)*(lfXt-lfX1)+(lfYt-lfY1)*(lfYt-lfY1);
	
	if( d1<d2 )
	{ 
		if(plfXr)*plfXr=lfX0;  
		if(plfYr)*plfYr=lfY0; 
		return sqrt(d1); 
	}
	else 
	{ 
		if(plfXr)
			*plfXr=lfX1;  
		if(plfYr)
			*plfYr=lfY1;  
		return sqrt(d2);
	}
}


double GraphAPI::GGetNearestDisOfPtToLine3D(double lfX0, double lfY0, double lfZ0, double lfX1, double lfY1, double lfZ1,
							   double lfXt, double lfYt, double lfZt, 
							   double *plfXr, double *plfYr, double *plfZr,
							   bool bFootCanOut, bool b2DDis )
{
	double x,y,z;
	GGetPerpendicular(lfX0,lfY0,lfX1,lfY1,lfXt,lfYt,&x,&y,NULL);
	
	GGetPtZOfLine(lfX0,lfY0,lfZ0,lfX1,lfY1,lfZ1,&x,&y,&z);
	
	bool bOut = false;
	double d1 = fabs(lfX0-x)+fabs(lfY0-y)+fabs(lfZ0-z);
	double d2 = fabs(lfX1-x)+fabs(lfY1-y)+fabs(lfZ1-z);
	double d3 = fabs(lfX0-lfX1)+fabs(lfY0-lfY1)+fabs(lfZ0-lfZ1);
	if( d1>d3 || d2>d3 )bOut = true;
	
	if( !bOut||bFootCanOut )
	{
		if(plfXr)*plfXr=x;  if(plfYr)*plfYr=y;	if(plfZr)*plfZr=z;
		if( b2DDis )return sqrt((lfXt-x)*(lfXt-x)+(lfYt-y)*(lfYt-y));
		return sqrt((lfXt-x)*(lfXt-x)+(lfYt-y)*(lfYt-y)+(lfZt-z)*(lfZt-z));
	}
	
	if( b2DDis )
	{
		d1 = (lfXt-lfX0)*(lfXt-lfX0)+(lfYt-lfY0)*(lfYt-lfY0);
		d2 = (lfXt-lfX1)*(lfXt-lfX1)+(lfYt-lfY1)*(lfYt-lfY1);
	}
	else
	{
		d1 = (lfXt-lfX0)*(lfXt-lfX0)+(lfYt-lfY0)*(lfYt-lfY0)+(lfZt-lfZ0)*(lfZt-lfZ0);
		d2 = (lfXt-lfX1)*(lfXt-lfX1)+(lfYt-lfY1)*(lfYt-lfY1)+(lfZt-lfZ1)*(lfZt-lfZ1);
	}
	
	if( d1<d2 )
	{ 
		if(plfXr)*plfXr=lfX0;  
		if(plfYr)*plfYr=lfY0; 
		if(plfZr)*plfZr=lfZ0; 
		return sqrt(d1); 
	}
	else 
	{ 
		if(plfXr)*plfXr=lfX1; 
		if(plfYr)*plfYr=lfY1;  
		if(plfZr)*plfZr=lfZ1; 
		return sqrt(d2); 
	}
}


int GraphAPI::GGetPerpPointPosinLine(double lfX0, double lfY0, double lfX1, double lfY1, double lfX, double lfY)
{
	double vector1x = lfX1-lfX0, vector1y = lfY1-lfY0;
	double vector2x = -vector1y, vector2y = vector1x;
	
	double delta = vector1x*vector2y-vector1y*vector2x;
	if ((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(vector1x)+fabs(vector1y))*(fabs(vector2x)+fabs(vector2y)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return 0;
	
	double t1 = ( (lfX-lfX0)*vector2y-(lfY-lfY0)*vector2x )/delta;
	if( t1<0 )return -1;
	else if( t1>1 )return 1;
	return 0;
}


void GraphAPI::GGetPtZOfLine(double lfX0,double lfY0,double lfZ0,double lfX1,double lfY1, double lfZ1, double *plfX,double *plfY,double *plfZ)
{
	if (plfX==NULL||plfY==NULL||plfZ==NULL)
	{
		return;
	}
	if( fabs(lfX1-lfX0)<g_lfDisTolerance && fabs(lfY1-lfY0)<g_lfDisTolerance )
		*plfZ = lfZ0;
	else if( fabs(lfX1-lfX0)>fabs(lfY1-lfY0) )
	{
		*plfZ = (*plfX-lfX0)/(lfX1-lfX0)*(lfZ1-lfZ0) + lfZ0;
	}
	else
	{
		*plfZ = (*plfY-lfY0)/(lfY1-lfY0)*(lfZ1-lfZ0) + lfZ0;
	}
}


void GraphAPI::GGetRightAnglePoint(double lfX1,double lfY1,double lfX2,double lfY2,double lfX3,double lfY3,double *plfX,double *plfY)
{
	if (plfX==NULL||plfY==NULL)
	{
		return;
	}
	if( fabs(lfX1-lfX2)<g_lfDisTolerance && fabs(lfY1-lfY2)<g_lfDisTolerance )
	{
		*plfX = lfX3; *plfY = lfY3;
		return;
	}
	//水平方向向量和垂直方向向量的plfX,plfY分量
	double hvectorx, hvectory, vvectorx, vvectory;
	hvectorx = lfX2-lfX1; hvectory = lfY2-lfY1;
	vvectorx = lfY1-lfY2; vvectory = lfX2-lfX1;
	
	//过第二点的垂线和过第三点的平行线的交点即为所求

	double delta = vvectory*hvectorx-vvectorx*hvectory;
	
	if ((fabs(hvectorx)+fabs(hvectory))*(fabs(vvectorx)+fabs(vvectory))<=1e-15)
	{
		return ;
	}
	double unitdelta = delta/((fabs(hvectorx)+fabs(hvectory))*(fabs(vvectorx)+fabs(vvectory)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return;
	
	double t = ( (lfX2-lfX3)*hvectory-(lfY2-lfY3)*hvectorx )/delta;
	
	*plfX = lfX2 + t*vvectorx;
	*plfY = lfY2 + t*vvectory;
}


double GraphAPI::GGetAngle(double lfX1,double lfY1, double lfX2, double lfY2)
{
	double vectorx = lfX2-lfX1, vectory = lfY2-lfY1;
	double angle;
	if( vectorx<g_lfDisTolerance && vectorx>-g_lfDisTolerance )
	{
		if( vectory<g_lfDisTolerance && vectory>-g_lfDisTolerance )
			return 0;
		if( vectory>0 )return PI/2;
		return PI*3/2;
	}
	angle=atan(vectory/vectorx);
	if( lfX2>=lfX1 && lfY2>=lfY1 );//0-90
	else if( lfX2< lfX1 && lfY2>=lfY1 )angle=angle+PI;//90-180
	else if( lfX2< lfX1 && lfY2< lfY1 )angle=angle+PI;//180-270
	else if( lfX2>=lfX1 && lfY2< lfY1 )angle=2*PI+angle;//270-360
	return angle;
}

//求解方程组
bool GraphAPI::SolveEquation(double *plfA,double *plfB,int nN)
{
	double *r = new double[nN*nN];
	double *v = new double[nN];
	bool ret = false;
	if( r && v )
	{
		if( matrix_reverse(plfA,nN,r) )
		{
			matrix_multiply_byvector(r,nN,nN,plfB,v);
			memcpy(plfB,v,sizeof(double)*nN);
			ret = true;
		}
	}
	
	if( r )delete[] r;
	if( v )delete[] v;
	return ret;
}






double GraphAPI::GGetTriangleArea(double lfX0,double lfY0,double lfX1,double lfY1,double lfX2,double lfY2)
{
	double v1x = lfX1-lfX0, v1y = lfY1-lfY0;
	double v2x = lfX2-lfX1, v2y = lfY2-lfY1;
	return fabs((v1x*v2y-v1y*v2x)/2);
}

bool GraphAPI::GGetRay1IntersectRay2(double  lfX0,double lfY0,double lfX1,double lfY1,double lfX2,double lfY2,double lfX3,double lfY3,double *plfXRet, double *plfYRet)
{
	double dx1=lfX1-lfX0;
	double dy1=lfY1-lfY0;
	double dx2=lfX3-lfX2;
	double dy2=lfY3-lfY2;
	double delta = dx1*dy2-dy1*dx2;

	if ((fabs(dx1)+fabs(dy1))*(fabs(dx2)+fabs(dy2))<=1e-15)
	{
		return false;
	}
	double unitdelta = delta/((fabs(dx1)+fabs(dy1))*(fabs(dx2)+fabs(dy2)));
	if( unitdelta<g_lfPraTolerance && unitdelta>-g_lfPraTolerance )return false;
	
	double t1 = ( (lfX2-lfX0)*dy2-(lfY2-lfY0)*dx2 )/delta;
	if( t1<=0  )return false; 
	double t2 = ( (lfX2-lfX0)*dy1-(lfY2-lfY0)*dx1 )/delta;
	if( t2<=0  )return false; 
	if(plfYRet&&plfXRet)
	{
		*plfXRet = lfX0 + t1*dx1;
		*plfYRet = lfY0 + t1*dy1;
	}
	return true;
}


bool GraphAPI::GGetPtInAngleBisector2D(double  lfXorg,double lfYorg,double lfX1,double lfY1,double lfX2,double lfY2,double *plfXRet,double *plfYRet)
{
	if (plfYRet==NULL||plfXRet==NULL)
	{
		return false;
	}
	double vectorx1=lfX1-lfXorg;
	double vectory1=lfY1-lfYorg;
	double vectorx2=lfX2-lfXorg;
	double vectory2=lfY2-lfYorg;
	double  dis1=sqrt(vectorx1*vectorx1+vectory1*vectory1);
	double  dis2=sqrt(vectorx2*vectorx2+vectory2*vectory2);
    *plfXRet=vectorx1/dis1+vectorx2/dis2+lfXorg;
	*plfYRet=vectory1/dis1+vectory2/dis2+lfYorg;
	return true;  	
}



void GRotate2DPT(double *px, double *py, int sum, double x0, double y0, double angle)
{
	float dx,dy,sina,cosa;
	
	sina = sin(angle);
	cosa = cos(angle);
	
	for (int i=0; i<sum; i++)
	{
		dx = px[i] - x0;
		dy = py[i] - y0;
		
		px[i] = dx * cosa - dy * sina  + x0;
		py[i] = dy * cosa + dx * sina  + y0;
	}
}


//用n个点求出仿射变换系数
void CalcAffineParams(double *x1,double *y1,double *d1,double *d2,int n,double *r1,double *r2)
{
	double aa[9],bb[9],a[3],b[3],r[3];
	
	memset(aa,0,9*sizeof(double));
	memset(a,0,3*sizeof(double));
	memset(b,0,3*sizeof(double));
	
	for( int i=0; i<n; i++)
	{
		aa[0]+= *x1 * *x1;	aa[1] += *x1 * *y1;	aa[2] += *x1;
		aa[3]+= *x1 * *y1;	aa[4] += *y1 * *y1;	aa[5] += *y1;
		aa[6]+= *x1;	aa[7] += *y1;	aa[8] += 1;
		
		a[0] += *x1 * *d1;	a[1] += *y1 * *d1;	a[2] += *d1;
		b[0] += *x1 * *d2;	b[1] += *y1 * *d2;	b[2] += *d2;
		
		x1++;	y1++;	d1++;	d2++;
	}
	
	matrix_reverse(aa,3,bb);
	matrix_multiply_byvector(bb,3,3,a,r);
	memcpy(r1,r,3*sizeof(double));
	matrix_multiply_byvector(bb,3,3,b,r);
	memcpy(r2,r,3*sizeof(double));
}



//用n个点求出仿射变换系数 含有 z 系数
void CalcAffineParams(double *x1,double *y1,double *z1,double *x2,double *y2,double *z2, int n,double *r1,double *r2, double *r3)
{
	double aa[16],raa[16],d1[4],d2[4],d3[4],r[4];
	
	memset(aa,0,sizeof(aa));
	memset(d1,0,sizeof(d1));
	memset(d2,0,sizeof(d2));
	memset(d3,0,sizeof(d2));
	
	for( int i=0; i<n; i++)
	{
		aa[0]+= *x1 * *x1;	aa[1] += *x1 * *y1;	aa[2] += *z1 * *x1; aa[3] += *x1;
		aa[4]+= *x1 * *y1;	aa[5] += *y1 * *y1;	aa[6] += *z1 * *y1; aa[7] += *y1;
		aa[8]+= *x1 * *z1;	aa[9] += *y1 * *z1;	aa[10] += *z1 * *z1; aa[11] += *z1;
		aa[12]+= *x1;	aa[13] += *y1;	aa[14] += *z1; aa[15] += 1;
		
		d1[0] += *x1 * *x2;	d1[1] += *y1 * *x2;	d1[2] += *z1 * *x2;  d1[3] += *x2;
		d2[0] += *x1 * *y2;	d2[1] += *y1 * *y2;	d2[2] += *z1 * *y2;  d2[3] += *y2;
		d3[0] += *x1 * *z2;	d3[1] += *y1 * *z2;	d3[2] += *z1 * *z2;  d3[3] += *z2;
		
		x1++; y1++; z1++; x2++;	y2++; z2++;
	}

	matrix_toIdentity(raa,4);
	
	matrix_reverse(aa,4,raa);
	matrix_multiply_byvector(raa,4,4,d1,r);
	memcpy(r1,r,4*sizeof(double));

	matrix_multiply_byvector(raa,4,4,d2,r);
	memcpy(r2,r,4*sizeof(double));

	matrix_multiply_byvector(raa,4,4,d3,r);
	memcpy(r3,r,4*sizeof(double));
}

void ConvertCharToWstring(const char* from, wstring &to, UINT codepage)
{
	to = L"";
	int wlen = MultiByteToWideChar( codepage,
		0,
		from,
		-1,
		NULL,
		0);
	ASSERT(wlen);
	if(wlen == 0) 
		return;
	wchar_t* wbuffer = new wchar_t[wlen+2];
    MultiByteToWideChar(codepage,
		0,
		from,
		-1,
		wbuffer,
		wlen);
	to = wbuffer;
	delete [] wbuffer;	
}

void ConvertWcharToString(const wchar_t* from, string &to, 
										 UINT codepage, bool* datalost, char unknownchar)
{
	to = "";
	int alen = 	WideCharToMultiByte(codepage,
		0,
		from,
		-1, 
		NULL,
		0,
		NULL,
		NULL);
	ASSERT(alen);
	if(alen == 0) 
		return;
	char* abuffer = new char[alen+2]; 
	BOOL UsedDefault=FALSE;
	WideCharToMultiByte(	codepage,
		0,
		from,
		-1,
		abuffer,
		alen, 
		(unknownchar != 0 ? &unknownchar : NULL),
		(datalost != NULL ? &UsedDefault : NULL)
		);
	if( datalost != NULL && UsedDefault != FALSE)
		*datalost = true;
	to = abuffer;
	delete [] abuffer;
}


CStringA ConvertWCharToChar(const wchar_t* text)
{
	string to;
	if( text )ConvertWcharToString(text,to,CP_ACP,NULL,0);

	return CStringA(to.c_str());
}

CStringW ConvertCharToWChar(const char* text)
{
	wstring to;
	if( text )ConvertCharToWstring(text,to,CP_ACP);

	return CStringW(to.c_str());
}

CString ConvertCharToTChar(const char* text)
{
#ifdef _UNICODE
	return ConvertCharToWChar(text);
#else
	return CStringA(text);
#endif
}

CStringA ConvertTCharToChar(LPCTSTR text)
{
#ifdef _UNICODE
	return ConvertWCharToChar(text);
#else
	return CStringA(text);
#endif
}

CDebugTime::CDebugTime()
{
	m_time0 = m_time1 = GetTickCount();
}


CDebugTime::~CDebugTime()
{

}


void CDebugTime::PrintfTime(LPCTSTR title)
{
	DWORD time1 = GetTickCount();
	int dtime = time1-m_time1;

	CString strMsg;
	if( title )
		strMsg.Format(_T("%s: %d ms\n"),title,dtime);
	else
		strMsg.Format(_T("%d ms\n"),title,dtime);

	OutputDebugString(strMsg);

	m_time1 = time1;
}

//