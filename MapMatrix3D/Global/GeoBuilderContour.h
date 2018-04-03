// GeoBuilderContour.h: interface for the CGeoBuilderContour class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOBUILDERCONTOUR_H__21C152D2_48A9_4CBF_A449_D872CB6538EC__INCLUDED_)
#define AFX_GEOBUILDERCONTOUR_H__21C152D2_48A9_4CBF_A449_D872CB6538EC__INCLUDED_

/*
contour tracing class..
2005/1/14 by ZergLee
*/
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Base.h"
#include "triangle2.h"
#include "CADLIB.h"
using namespace MyTriangle;
#ifdef SINGLE
#define REAL float
#else
#define REAL double
#endif

#define   POINTIN    1
#define   POINTOUT   0
#define   LINEIN     1
#define   LINEOUT    0
#define   pNoValue   -9999.9f

typedef struct clipWIN {
	int left, right, bottom, top;
}; 
typedef struct POINT2Df {
	double x, y;
};


class CGeoBuilderContour  
{ 
public:
	bool SaveContour(CString strContour);
	CGeoBuilderContour();
	virtual ~CGeoBuilderContour();
 	float m_lfDemInterval;
public:
	int Init(GeoPoint2 * gp,int iNum,float interval);
	int Init(float *x, float * y,float * z,int iNum,float interval ); 
   int  Init(double *x ,double * y,double * z,int iNum,float interval )  ;
   int   Init(GeoPoint * gp,int iNum,float interval )  ;

   bool GetLine(int iNum,GeoLine * &gl); 
   int  GetContourNumber();
private:
// 
	void SaveTXT2DXF(CString strDXF);
	GeoLine * m_pContour;
	int iLineNum;
    float idz5;
	struct triangulateio in, mid, out, vorout;
    double MinZ,MaxZ;
    float m_lfInterval;
    int xyi,ai;
    int jxOff, iyOff, SubNx, SubNy;
     struct clipWIN DEMClip;
 	int *triMarks;
     int cntpSum;
     struct POINT2Df *cnt;
     int IndexFlag; 
    void FreeTriangle();
 	void RoatateToDEM(double *x,double *y,int *ir,int *jc);
 	void AddDEMPointToTriangle(float dx,float dy,float z);
 	void AddXyzToTriangle( GeoPoint2 *xyz,int n);
 	void DrawTriangle();
 	int GetNextTriEdge(float *x1,float *y1,float *z1,float *x2,float *y2,float *z2,int *edgeNum);
	
	/****************************** TriContour *****************************/
 	int GetNextTriangle(float *x,float *y,float *z,int *triNum);
 	void SetCntMarks(float z);
 	void ReverseCnt();
 	int TraceACnt(float z, int nextTri,CRgn& rgn);
 	int TriContour(FILE *fvec, float idz);
 	void TriDrawContour(FILE *fvec, float iz);
	//##ModelId=3A569B860271
     int pointChk(struct clipWIN *DEMClip, double x, double y);
 	int LineClip(struct clipWIN *DEMClip, double *x1, double *y1, double *x2, double *y2);
 	void SetClipWin(struct clipWIN *DEMClip, int left, int bottom, int right, int top);
 	int Clipping(struct clipWIN *DEMClip, double *xin, double *yin, double *xout, double *yout);
 	bool IsValidTriangle(int j1,int j2,int j3,CRgn& rgn);
};

#endif // !defined(AFX_GEOBUILDERCONTOUR_H__21C152D2_48A9_4CBF_A449_D872CB6538EC__INCLUDED_)
