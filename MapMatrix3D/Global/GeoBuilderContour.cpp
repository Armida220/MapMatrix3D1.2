// GeoBuilderContour.cpp: implementation of the CGeoBuilderContour class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Triangle2.h"
#include "GeoBuilderContour.h" 
#include "editbase.h"
#include <stdlib.h>         
#include <malloc.h>
#include "Resource.h"
#include "FArray.hpp"
#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeoBuilderContour::CGeoBuilderContour()
{
	cntpSum = 0;
	cnt = NULL;
	IndexFlag=0; 
	memset(&in,0,sizeof(in));
	memset(&mid,0,sizeof(in));
	memset(&vorout,0,sizeof(vorout));
	memset(&out,0,sizeof(out)); 
    m_pContour=new GeoLine[10000];
    iLineNum=0;
    m_lfDemInterval=-100;
}

CGeoBuilderContour::~CGeoBuilderContour()
{
    if(iLineNum>0)
	{
	 for(int i=0;i<iLineNum;i++)
	 {
	  delete[] m_pContour[i].gp;
	 }
	 delete[] m_pContour;
	 m_pContour=NULL;
	}
	if(m_pContour!=NULL)
		delete[] m_pContour;
    FreeTriangle();
}
    
void CGeoBuilderContour::FreeTriangle()
{
	/* Free all allocated arrays, including those allocated by Triangle. */
try
{
	if( in.pointlist)	{free(in.pointlist);		in.pointlist 	 = NULL; }
	if( in.pointattributelist) 	{free(in.pointattributelist);	in.pointattributelist= NULL; }
	if( in.pointmarkerlist) 	{free(in.pointmarkerlist);	in.pointmarkerlist= NULL; }
 	if( in.trianglelist) 	{free(in.trianglelist);		in.trianglelist 	 = NULL; }
 	if( in.triangleattributelist) 	{free(in.triangleattributelist);		in.triangleattributelist  = NULL; }
 	if( in.neighborlist) 	{free(in.neighborlist);	in.neighborlist = NULL; } 
	if( in.segmentlist) 	{free(in.segmentlist);		in.segmentlist  = NULL; }
	if( in.segmentmarkerlist) 	{free(in.segmentmarkerlist);		in.segmentmarkerlist  = NULL; }
    if( in.edgelist) 	{free(in.edgelist);		in.edgelist  = NULL; }
    if( in.normlist) 	{free(in.normlist);		in.normlist  = NULL; }
    if(in.edgemarkerlist) {free(in.edgemarkerlist);		in.edgemarkerlist  = NULL; }

 	if( mid.pointlist)	{free(mid.pointlist);		mid.pointlist 	 = NULL; }
	if( mid.pointattributelist) 	{free(mid.pointattributelist);	mid.pointattributelist= NULL; }
	if( mid.pointmarkerlist) 	{free(mid.pointmarkerlist);	mid.pointmarkerlist= NULL; }
 	if( mid.trianglelist) 	{free(mid.trianglelist);		mid.trianglelist 	 = NULL; }
 	if( mid.triangleattributelist) 	{free(mid.triangleattributelist);		mid.triangleattributelist  = NULL; }
 	if( mid.neighborlist) 	{free(mid.neighborlist);	mid.neighborlist = NULL; } 
	if( mid.segmentlist) 	{free(mid.segmentlist);		mid.segmentlist  = NULL; }
	if( mid.segmentmarkerlist) 	{free(mid.segmentmarkerlist);		mid.segmentmarkerlist  = NULL; }
	if( mid.edgelist) 	{free(mid.edgelist);		mid.edgelist  = NULL; }
    if( mid.normlist) 	{free(mid.normlist);		mid.normlist  = NULL; }
    if(mid.edgemarkerlist) {free(mid.edgemarkerlist);	mid.edgemarkerlist  = NULL; }


	if( vorout.pointlist)	{free(vorout.pointlist);		vorout.pointlist 	 = NULL; }
	if( vorout.pointattributelist) 	{free(vorout.pointattributelist);	vorout.pointattributelist= NULL; }
	if( vorout.pointmarkerlist) 	{free(vorout.pointmarkerlist);	vorout.pointmarkerlist= NULL; }
 	if( vorout.trianglelist) 	{free(vorout.trianglelist);		vorout.trianglelist 	 = NULL; }
 	if( vorout.triangleattributelist) 	{free(vorout.triangleattributelist);		vorout.triangleattributelist  = NULL; }
 	if( vorout.neighborlist) 	{free(vorout.neighborlist);	vorout.neighborlist = NULL; } 
	if( vorout.segmentlist) 	{free(vorout.segmentlist);		vorout.segmentlist  = NULL; }
	if( vorout.segmentmarkerlist) 	{free(vorout.segmentmarkerlist);		vorout.segmentmarkerlist  = NULL; }
    if( vorout.edgelist) 	{free(vorout.edgelist);		vorout.edgelist  = NULL; }
    if( vorout.normlist) 	{free(vorout.normlist);		vorout.normlist  = NULL; }
    if(vorout.edgemarkerlist) {free(vorout.edgemarkerlist);		vorout.edgemarkerlist  = NULL; }

}
catch(...)
{
;
}
}
bool CGeoBuilderContour::GetLine(int iNum,GeoLine * &gl)
	{
      if(iNum>iLineNum-1)
		  return false;
	  gl=m_pContour+iNum;
	  return true;
	}
int CGeoBuilderContour::GetContourNumber()
	{
		return iLineNum;
	}
void CGeoBuilderContour::RoatateToDEM(double *x,double *y,int *ir,int *jc)
{
	float dx,dy;

//	dx = *x - DEM->x0;
//	dy = *y - DEM->y0 ;

//	*x = DEM->cosA * dx + DEM->sinA * dy;
//	*y =-DEM->sinA * dx + DEM->cosA * dy;
}

void CGeoBuilderContour::AddDEMPointToTriangle(float dx,float dy,float z)
{
	in.pointlist[xyi] = dx;	 xyi++;
	in.pointlist[xyi] = dy;	 xyi++;
	in.pointattributelist[ai] = z;
	in.pointmarkerlist[ai] = 0;	ai++;
}

void CGeoBuilderContour::AddXyzToTriangle(GeoPoint2 *xyz,int n)
{
	int i,j,k;
	int i1,j1,i2,j2,ir,jc;
	int i1c,j1c,i2c,j2c;
	float x1,y1,x2,y2;
	double x1c,y1c,x2c,y2c;
	float dx,dy,dz,z;

//	dx=xyz->x  ;	
//	dy=xyz->y  ;
	x1   =xyz->x;//( DEM->cosA * dx + DEM->sinA * dy) / DEM->dx;
	y1   =xyz->y;//(-DEM->sinA * dx + DEM->cosA * dy) / DEM->dy;
	for( k=1; k<n; k++,xyz++)
	{
	//	dx = xyz[1].x - DEM->x0;	
	//	dy = xyz[1].y - DEM->y0;
		x2 = xyz[1].x;//( DEM->cosA * dx + DEM->sinA * dy) / DEM->dx;
		y2 = xyz[1].y;//(-DEM->sinA * dx + DEM->cosA * dy) / DEM->dy;

		x1c = x1; y1c = y1;
		x2c = x2; y2c = y2;

		if( pointChk(&DEMClip,x1,y1) == POINTIN )
		{ 
			AddDEMPointToTriangle( x1,y1, xyz->z);
		}
 

		i1 = y1c;	j1 = x1c;
		i2 = y2c;	j2 = x2c;

		dx = x2 - x1;
		dy = y2 - y1;
		dz = xyz[1].z - xyz->z;
		if( j1 < j2 ) 
		{
			for( j=j1+1; j<=j2; j++) 
				AddDEMPointToTriangle(j, y1 + dy/dx*(j-x1),  xyz->z + dz/dx*(j-x1) );
		}
		else if( j2 < j1 ) 
		{
			for( j=j2+1; j<=j1; j++) 
				AddDEMPointToTriangle(j, y1 + dy/dx*(j-x1),  xyz->z + dz/dx*(j-x1) );
		}

		if( i1 < i2 ) {
			for( i=i1+1; i<=i2; i++) 
				AddDEMPointToTriangle(x1 + dx/dy*(i-y1), i, xyz->z + dz/dy*(i-y1) );
		}
		else if( i2 < i1 ) {
			for( i=i2+1; i<=i1; i++) 
				AddDEMPointToTriangle(x1 + dx/dy*(i-y1), i, xyz->z + dz/dy*(i-y1) );
		}
		x1 = x2; y1 = y2;
	}

	if( pointChk(&DEMClip,x1,y1) == POINTIN ) 
		AddDEMPointToTriangle( x1,y1, xyz->z);

} 
int CGeoBuilderContour::Init(double *x ,double * y,double * z,int iNum,float interval )  
{
	if(iNum<=3)  //not enough points
       return 0;
	if(iLineNum>0)
	{
	 for(int i=0;i<iLineNum;i++)
	 {
	  delete[] m_pContour[i].gp;
	 }
//	 delete[] m_pContour;
	}
    FreeTriangle();
    iLineNum=0; 
	int i,j;
	int	nx,ny,n;
	//double x,y,dx,dy,*z,*z1; 

//	SetClipWin(&DEMClip, jxOff,iyOff,jxOff+subNx-1,iyOff+subNy-1);

	mid.numberofedges = 0;
	in.numberofpointattributes = 1;

//	int nFormat=m_demObj.GetDemVersion(m_strDemName);
	long totalNum=iNum;//record the dem or dtm point number,not include the feature point number!!!!!!!
 
	{
	    //m_demObj.GetPtNum();
		in.numberofpoints=totalNum;
		//malloc memory for feature points
	 

		in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
		in.pointattributelist = (REAL *)malloc(in.numberofpoints*in.numberofpointattributes*sizeof(REAL));
	//	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));

		xyi = ai = 0;
		MinZ = 999999999;	MaxZ = -999999999;
		/**************** Add DEM sub grid **********************/
		double *px,*py,*pz;
	    //add point here

		for(long i=0;i<totalNum;i++)
		{
			in.pointlist[i*2]=x[i];
			in.pointlist[i*2+1]=y[i];
			in.pointattributelist[i]=z[i];
		//	in.pointmarkerlist[i]=0;
			if (in.pointattributelist[i]!=-9999.9f&&in.pointattributelist[i]!=-99999.0f) 
			{
				if( MinZ > in.pointattributelist[i] ) MinZ = in.pointattributelist[i];
				else if( MaxZ < in.pointattributelist[i] ) MaxZ = in.pointattributelist[i];
			}
		}
 
		in.numberofsegments = 0;
		in.numberofholes = 0;
		in.numberofregions = 0;
		//in.regionList = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
	}
 
///////////////////////////////////////////////////////////////////////////////////
/*	FILE * fp=fopen("e:\\temp.txt","wt");
	for(int arrayIndex=0;arrayIndex<m_demObj.m_FeatureArray.GetSize();arrayIndex++)
	{
		if(m_demObj.m_FeatureArray[arrayIndex].m_nCode!=0||m_demObj.m_FeatureArray[arrayIndex].m_nPtNum==0)
			continue;

	//	fprintf(fp,"0\n");
	//	fprintf(fp,"%d\n",m_demObj.m_FeatureArray[arrayIndex].m_nPtNum);

		for(int ptIndex=0;ptIndex<m_demObj.m_FeatureArray[arrayIndex].m_nPtNum;ptIndex++,totalNum++)
		{
			fprintf(fp,"%f %f %f\n",m_demObj.m_FeatureArray[arrayIndex].m_pX[ptIndex],m_demObj.m_FeatureArray[arrayIndex].m_pY[ptIndex],m_demObj.m_FeatureArray[arrayIndex].m_pZ[ptIndex]);
		}
	}
	fclose(fp);
*/
///////////////////////////////////////////////////////////////////////////////////

/*	printf("Input point set:\n\n");
	report(&in, 1, 0, 0, 0, 0, 0);
*/
	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */

	mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
	/* Not needed if -N switch used or number of point attributes is zero: */
	mid.pointattributelist = (REAL *) NULL;
	mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */

	mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
	/* Not needed if -E switch used or number of triangle attributes is zero: */
	mid.triangleattributelist = (REAL *) NULL;
	mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */

	/* Needed only if segments are output (-p or -c) and -P not used: */
	mid.segmentlist = (int *) NULL;
	/* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid.segmentmarkerlist = (int *) NULL;
	mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
	/* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	/* Triangulate the points.  Switches are chosen to read and write a  */
	/*   PSLG (p), preserve the convex hull (c), number everything from  */
	/*   zero (z), assign a regional attribute to each element (A), and  */
	/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
	/*   neighbor list (n).                                              */

	memset(&mid,0,sizeof(mid));
	memset(&vorout,0,sizeof(vorout));
	try
	{
	triangulate("pczAevnQ", &in, &mid, &vorout);
	}
    catch(...)
	{
	  return false;
	}
  	return TriContour(NULL,interval); 
}
int CGeoBuilderContour::Init(float *x ,float * y,float * z,int iNum,float interval )  
{	if(iNum<=3)  //not enough points
       return 0;
	if(iLineNum>0)
	{
	 for(int i=0;i<iLineNum;i++)
	 {
	  delete[] m_pContour[i].gp;
	 }
//	 delete[] m_pContour;
	}
    FreeTriangle();
    iLineNum=0; 
	int i,j;
	int	nx,ny,n;
	//double x,y,dx,dy,*z,*z1; 

//	SetClipWin(&DEMClip, jxOff,iyOff,jxOff+subNx-1,iyOff+subNy-1);

	mid.numberofedges = 0;
	in.numberofpointattributes = 1;

//	int nFormat=m_demObj.GetDemVersion(m_strDemName);
	long totalNum=iNum;//record the dem or dtm point number,not include the feature point number!!!!!!!
 
	{
	    //m_demObj.GetPtNum();
		in.numberofpoints=totalNum;
		//malloc memory for feature points
	 

		in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
		in.pointattributelist = (REAL *)malloc(in.numberofpoints*in.numberofpointattributes*sizeof(REAL));
	//	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));

		xyi = ai = 0;
		MinZ = 999999999;	MaxZ = -999999999;
		/**************** Add DEM sub grid **********************/
		double *px,*py,*pz;
	    //add point here

		for(long i=0;i<totalNum;i++)
		{
			in.pointlist[i*2]=x[i];
			in.pointlist[i*2+1]=y[i];
			in.pointattributelist[i]=z[i];
		//	in.pointmarkerlist[i]=0;
			if (in.pointattributelist[i]!=-9999.9f&&in.pointattributelist[i]!=-99999.0f) 
			{
				if( MinZ > in.pointattributelist[i] ) MinZ = in.pointattributelist[i];
				else if( MaxZ < in.pointattributelist[i] ) MaxZ = in.pointattributelist[i];
			}
		}
 
		in.numberofsegments = 0;
		in.numberofholes = 0;
		in.numberofregions = 0;
		//in.regionList = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
	}
 
///////////////////////////////////////////////////////////////////////////////////
/*	FILE * fp=fopen("e:\\temp.txt","wt");
	for(int arrayIndex=0;arrayIndex<m_demObj.m_FeatureArray.GetSize();arrayIndex++)
	{
		if(m_demObj.m_FeatureArray[arrayIndex].m_nCode!=0||m_demObj.m_FeatureArray[arrayIndex].m_nPtNum==0)
			continue;

	//	fprintf(fp,"0\n");
	//	fprintf(fp,"%d\n",m_demObj.m_FeatureArray[arrayIndex].m_nPtNum);

		for(int ptIndex=0;ptIndex<m_demObj.m_FeatureArray[arrayIndex].m_nPtNum;ptIndex++,totalNum++)
		{
			fprintf(fp,"%f %f %f\n",m_demObj.m_FeatureArray[arrayIndex].m_pX[ptIndex],m_demObj.m_FeatureArray[arrayIndex].m_pY[ptIndex],m_demObj.m_FeatureArray[arrayIndex].m_pZ[ptIndex]);
		}
	}
	fclose(fp);
*/
///////////////////////////////////////////////////////////////////////////////////

/*	printf("Input point set:\n\n");
	report(&in, 1, 0, 0, 0, 0, 0);
*/
	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */

	mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
	/* Not needed if -N switch used or number of point attributes is zero: */
	mid.pointattributelist = (REAL *) NULL;
	mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */

	mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
	/* Not needed if -E switch used or number of triangle attributes is zero: */
	mid.triangleattributelist = (REAL *) NULL;
	mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */

	/* Needed only if segments are output (-p or -c) and -P not used: */
	mid.segmentlist = (int *) NULL;
	/* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid.segmentmarkerlist = (int *) NULL;
	mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
	/* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	/* Triangulate the points.  Switches are chosen to read and write a  */
	/*   PSLG (p), preserve the convex hull (c), number everything from  */
	/*   zero (z), assign a regional attribute to each element (A), and  */
	/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
	/*   neighbor list (n).                                              */

	memset(&mid,0,sizeof(mid));
	memset(&vorout,0,sizeof(vorout));
	try
	{
	triangulate("pczAevnQ", &in, &mid, &vorout);
	}
    catch(...)
	{
	  return false;
	}
  	return TriContour(NULL,interval); 

}
int CGeoBuilderContour::Init(GeoPoint * gp,int iNum,float interval )  
{ 
	if(interval<=0)
		return 0;
	if(iNum<=3)  //not enough points
       return 0;
	if(iLineNum>0)
	{
	 for(int i=0;i<iLineNum;i++)
	 {
	  delete[] m_pContour[i].gp;
	 }
//	 delete[] m_pContour;
	}
    FreeTriangle();
    iLineNum=0; 
	int i,j;
	int	nx,ny,n;
	double x,y,dx,dy,*z,*z1; 

//	SetClipWin(&DEMClip, jxOff,iyOff,jxOff+subNx-1,iyOff+subNy-1);

	mid.numberofedges = 0;
	in.numberofpointattributes = 1;

//	int nFormat=m_demObj.GetDemVersion(m_strDemName);
	long totalNum=iNum;//record the dem or dtm point number,not include the feature point number!!!!!!!
 
	{
	    //m_demObj.GetPtNum();
		in.numberofpoints=totalNum;
		//malloc memory for feature points
	 

		in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
		in.pointattributelist = (REAL *)malloc(in.numberofpoints*in.numberofpointattributes*sizeof(REAL));
	//	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));

		xyi = ai = 0;
		MinZ = 999999999;	MaxZ = -999999999;
		/**************** Add DEM sub grid **********************/
		double *px,*py,*pz;
	    //add point here
        int iMaxIndex=0;
		for(long i=0;i<totalNum;i++)
		{
			in.pointlist[i*2]=gp[i].x;
			in.pointlist[i*2+1]=gp[i].y;
			in.pointattributelist[i]=gp[i].z;
		//	in.pointmarkerlist[i]=0;
			if (gp[i].z>-9999) 
			{
				if( MinZ > gp[i].z ) MinZ = gp[i].z; 
			    if( MaxZ < gp[i].z ) 
				{
					MaxZ = gp[i].z;
				    iMaxIndex=i;
				}
			 
			}
		}
	 	if( (MaxZ-MinZ)/interval>10000000) //more than 10,00,0000
		 	return 0;
      //  CString strMsg;
	//	strMsg.Format(" max z= %lf min z=%lf ,max index=%d",double(MaxZ),double(MinZ),iMaxIndex); 
	//	Output(strMsg);
	//	strMsg.Format(" x=%f y=%f z=%f ", gp[iMaxIndex].x,gp[iMaxIndex].y,gp[iMaxIndex].z);
	//	Output(strMsg);
		in.numberofsegments = 0;
		in.numberofholes = 0;
		in.numberofregions = 0;
		//in.regionList = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
	}
		mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
	/* Not needed if -N switch used or number of point attributes is zero: */
	mid.pointattributelist = (REAL *) NULL;
	mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */

	mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
	/* Not needed if -E switch used or number of triangle attributes is zero: */
	mid.triangleattributelist = (REAL *) NULL;
	mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */

	/* Needed only if segments are output (-p or -c) and -P not used: */
	mid.segmentlist = (int *) NULL;
	/* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid.segmentmarkerlist = (int *) NULL;
	mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
	/* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	/* Triangulate the points.  Switches are chosen to read and write a  */
	/*   PSLG (p), preserve the convex hull (c), number everything from  */
	/*   zero (z), assign a regional attribute to each element (A), and  */
	/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
	/*   neighbor list (n).                                              */

	memset(&mid,0,sizeof(mid));
	memset(&vorout,0,sizeof(vorout));
	try
	{
	triangulate("pczAevnQ", &in, &mid, &vorout);
	}
    catch(...)
	{
	  return false;
	}

 	int iCntNum=0;
	try
	{
	 iCntNum=TriContour(NULL,interval);
	}
	catch(...)
	{
		return 0;
	}
	return iCntNum;

}
int CGeoBuilderContour::Init(GeoPoint2 * gp,int iNum,float interval )  
{ 
	if(interval<=0)
		return 0;
	if(iNum<=3)  //not enough points
       return 0;
	if(iLineNum>0)
	{
	 for(int i=0;i<iLineNum;i++)
	 {
	  delete[] m_pContour[i].gp;
	 }
//	 delete[] m_pContour;
	}
    FreeTriangle();
    iLineNum=0; 
	int i,j;
	int	nx,ny,n;
	double x,y,dx,dy,*z,*z1; 

//	SetClipWin(&DEMClip, jxOff,iyOff,jxOff+subNx-1,iyOff+subNy-1);

	mid.numberofedges = 0;
	in.numberofpointattributes = 1;

//	int nFormat=m_demObj.GetDemVersion(m_strDemName);
	long totalNum=iNum;//record the dem or dtm point number,not include the feature point number!!!!!!!
 
	{
	    //m_demObj.GetPtNum();
		in.numberofpoints=totalNum;
		//malloc memory for feature points
	 

		in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
		in.pointattributelist = (REAL *)malloc(in.numberofpoints*in.numberofpointattributes*sizeof(REAL));
	//	in.pointmarkerlist = (int *) malloc(in.numberofpoints * sizeof(int));

		xyi = ai = 0;
		MinZ = 999999999;	MaxZ = -999999999;
		/**************** Add DEM sub grid **********************/
		double *px,*py,*pz;
	    //add point here
        int iMaxIndex=0;
		for(long i=0;i<totalNum;i++)
		{
			in.pointlist[i*2]=gp[i].x;
			in.pointlist[i*2+1]=gp[i].y;
			in.pointattributelist[i]=gp[i].z;
		//	in.pointmarkerlist[i]=0;
			if (gp[i].z>-9999) 
			{
				if( MinZ > gp[i].z ) MinZ = gp[i].z; 
			    if( MaxZ < gp[i].z ) 
				{
					MaxZ = gp[i].z;
				    iMaxIndex=i;
				}
			 
			}
		}
	 	if( (MaxZ-MinZ)/interval>10000000) //more than 10,00,0000
		 	return 0;
      //  CString strMsg;
	//	strMsg.Format(" max z= %lf min z=%lf ,max index=%d",double(MaxZ),double(MinZ),iMaxIndex); 
	//	Output(strMsg);
	//	strMsg.Format(" x=%f y=%f z=%f ", gp[iMaxIndex].x,gp[iMaxIndex].y,gp[iMaxIndex].z);
	//	Output(strMsg);
		in.numberofsegments = 0;
		in.numberofholes = 0;
		in.numberofregions = 0;
		//in.regionList = (REAL *) malloc(in.numberofregions * 4 * sizeof(REAL));
	}
 
///////////////////////////////////////////////////////////////////////////////////
/*	FILE * fp=fopen("e:\\temp.txt","wt");
	for(int arrayIndex=0;arrayIndex<m_demObj.m_FeatureArray.GetSize();arrayIndex++)
	{
		if(m_demObj.m_FeatureArray[arrayIndex].m_nCode!=0||m_demObj.m_FeatureArray[arrayIndex].m_nPtNum==0)
			continue;

	//	fprintf(fp,"0\n");
	//	fprintf(fp,"%d\n",m_demObj.m_FeatureArray[arrayIndex].m_nPtNum);

		for(int ptIndex=0;ptIndex<m_demObj.m_FeatureArray[arrayIndex].m_nPtNum;ptIndex++,totalNum++)
		{
			fprintf(fp,"%f %f %f\n",m_demObj.m_FeatureArray[arrayIndex].m_pX[ptIndex],m_demObj.m_FeatureArray[arrayIndex].m_pY[ptIndex],m_demObj.m_FeatureArray[arrayIndex].m_pZ[ptIndex]);
		}
	}
	fclose(fp);
*/
///////////////////////////////////////////////////////////////////////////////////

/*	printf("Input point set:\n\n");
	report(&in, 1, 0, 0, 0, 0, 0);
*/
	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */

	mid.pointlist = (REAL *) NULL;            /* Not needed if -N switch used. */
	/* Not needed if -N switch used or number of point attributes is zero: */
	mid.pointattributelist = (REAL *) NULL;
	mid.pointmarkerlist = (int *) NULL; /* Not needed if -N or -B switch used. */

	mid.trianglelist = (int *) NULL;          /* Not needed if -E switch used. */
	/* Not needed if -E switch used or number of triangle attributes is zero: */
	mid.triangleattributelist = (REAL *) NULL;
	mid.neighborlist = (int *) NULL;         /* Needed only if -n switch used. */

	/* Needed only if segments are output (-p or -c) and -P not used: */
	mid.segmentlist = (int *) NULL;
	/* Needed only if segments are output (-p or -c) and -P and -B not used: */
	mid.segmentmarkerlist = (int *) NULL;
	mid.edgelist = (int *) NULL;             /* Needed only if -e switch used. */
	mid.edgemarkerlist = (int *) NULL;   /* Needed if -e used and -B not used. */

	vorout.pointlist = (REAL *) NULL;        /* Needed only if -v switch used. */
	/* Needed only if -v switch used and number of attributes is not zero: */
	vorout.pointattributelist = (REAL *) NULL;
	vorout.edgelist = (int *) NULL;          /* Needed only if -v switch used. */
	vorout.normlist = (REAL *) NULL;         /* Needed only if -v switch used. */

	/* Triangulate the points.  Switches are chosen to read and write a  */
	/*   PSLG (p), preserve the convex hull (c), number everything from  */
	/*   zero (z), assign a regional attribute to each element (A), and  */
	/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
	/*   neighbor list (n).                                              */

	memset(&mid,0,sizeof(mid));
	memset(&vorout,0,sizeof(vorout));
	try
	{
	triangulate("pczAevnQ", &in, &mid, &vorout);
	}
    catch(...)
	{
	  return false;
	}
  	int iCntNum=0;
	try
	{
	 iCntNum=TriContour(NULL,interval);
	}
	catch(...)
	{
		return 0;
	}
	return iCntNum;
	 

}

void CGeoBuilderContour::DrawTriangle()
{ 
}

int CGeoBuilderContour::GetNextTriEdge(float *x1,float *y1,float *z1,float *x2,float *y2,float *z2,int *edgeNum)
{
	int i,j,j2;
	struct triangulateio *io = &mid;

	if( *edgeNum >=  io->numberofedges ) return 0;

	i = *edgeNum;

	j=io->edgelist[i*2];
	j2=j+j;
	*x1 = io->pointlist[j2];
	*y1 = io->pointlist[j2+1];
	*z1 = io->pointattributelist[j];

	j=io->edgelist[i*2+1];
	j2=j+j;
	*x2 = io->pointlist[j2];
	*y2 = io->pointlist[j2+1];
	*z2 = io->pointattributelist[j];

	(*edgeNum)++;

	return 1;
}

/****************************** TriContour *****************************/
int CGeoBuilderContour::GetNextTriangle(float *x,float *y,float *z,int *triNum)
{
	int i,j,j3;
	struct triangulateio *io = &mid;

	if( *triNum >=  io->numberoftriangles ) return 0;
	
	for( ; *triNum < io->numberoftriangles; (*triNum)++)
		if( triMarks[*triNum] != 0 ) {
			j3 = *triNum * io->numberofcorners;

			j = io->trianglelist[j3];
			x[0] = io->pointlist[j*2];
			y[0] = io->pointlist[j*2+1];
			z[0] = io->pointattributelist[j];

			j = io->trianglelist[j3+1];
			x[1] = io->pointlist[j*2];
			y[1] = io->pointlist[j*2+1];
			z[1] = io->pointattributelist[j];

			j = io->trianglelist[j3+2];
			x[2] = io->pointlist[j*2];
			y[2] = io->pointlist[j*2+1];
			z[2] = io->pointattributelist[j];

			(*triNum)++;
			return 1;
		}

	return 0;
}

void CGeoBuilderContour::SetCntMarks(float z)
{
	int i,j,j3,n;
	float z1,z2,z3;
	struct triangulateio *io = &mid;

	j3 = 0;
	for( i=0; i< io->numberoftriangles; i++, j3 += io->numberofcorners)
	{
		n = 0;
		j = io->trianglelist[j3];
		z1 = io->pointattributelist[j];
		j = io->trianglelist[j3+1];
		z2 = io->pointattributelist[j];
		j = io->trianglelist[j3+2];
		z3 = io->pointattributelist[j];

		if( (z > z1 && z < z2) ||  (z > z2 && z < z1 ) )
			n++;
		if( (z > z2 && z < z3) ||  (z > z3 && z < z2 ) )
			n++;
		if( (z > z3 && z < z1) ||  (z > z1 && z < z3 ) )
			n++;
		if( z1 == -9999.9f || z2 == -9999.9f || z3 == -9999.9f )
			n = 0;  
//????? Why is n not equal to 3.

//每个三角形只能被一根等高线穿过 zyong
		switch( n ) {
		case 2:
			triMarks[i] = 1;
			break;
		case 1:
		case 3:
			printf("error : SetCntMarks n = %d\n",n);
		default:
			triMarks[i] = 0;
		}

	}
}

void CGeoBuilderContour::ReverseCnt()
{
	int i,j;
	struct POINT2Df	tempPnt;

	for( i=0, j = cntpSum-1; i < j; i++,j--)
	{
		tempPnt = cnt[i];
		cnt[i] = cnt[j];
		cnt[j] = tempPnt;
	}
}

int CGeoBuilderContour::TraceACnt(float z, int nextTri,CRgn& rgn)
{
	int reversed=0;
	int leaveEdge[2],enterEdge[2],firstEnterEdge[2];
	int firstTri,curTri;
	int i,j,j1,j2,j3,k;
	float x1,y1,z1,x2,y2,z2,x3,y3,z3;
	double dzm ;
	struct triangulateio *io = &mid;
	                                                                
	cntpSum = 0;
	firstTri = nextTri;

	i = firstTri*io->numberofcorners;
	j1 = io->trianglelist[i];
	j2 = io->trianglelist[i+1];
	j3 = io->trianglelist[i+2];
  	if(!IsValidTriangle(j1,j2,j3,rgn))
	{ 
  	  return 0;
	}
	curTri = nextTri;
	triMarks[curTri] = 0;

	x1 = io->pointlist[j1*2];
	y1 = io->pointlist[j1*2+1];
	z1 = io->pointattributelist[j1];

	x2 = io->pointlist[j2*2];
	y2 = io->pointlist[j2*2+1];
	z2 = io->pointattributelist[j2];

	x3 = io->pointlist[j3*2];
	y3 = io->pointlist[j3*2+1];
	z3 = io->pointattributelist[j3];

	if( (z > z1 && z < z2) || (z > z2 && z < z1 ) ) {	/* first triangle */
		dzm= (z -z1)/(z2-z1);
		cnt[cntpSum].x = x1 + (x2-x1)*dzm;
		cnt[cntpSum].y = y1 + (y2-y1)*dzm;
		cntpSum++;
			
		if( cntpSum == 1 ) {
			enterEdge[0] = j1;
			enterEdge[1] = j2;
		}
		else {
			leaveEdge[0] = j1;
			leaveEdge[1] = j2;
		}
	}
	if( (z > z2 && z < z3) || (z > z3 && z < z2 ) ) {
		dzm= (z -z2)/(z3-z2);
		cnt[cntpSum].x = x2 + (x3-x2)*dzm;
		cnt[cntpSum].y = y2 + (y3-y2)*dzm;
		cntpSum++;

		if( cntpSum == 1 ) {
			enterEdge[0] = j2;
			enterEdge[1] = j3;
		}
		else {
			leaveEdge[0] = j2;
			leaveEdge[1] = j3;
		}
	}
	if( (z > z3 && z < z1) || (z > z1 && z < z3 ) ) {
		dzm = (z -z3)/(z1-z3);
		cnt[cntpSum].x = x3 + (x1-x3)*dzm;
		cnt[cntpSum].y = y3 + (y1-y3)*dzm;
		cntpSum++;

		leaveEdge[0] = j3;
		leaveEdge[1] = j1;
	}

/**************************************/

	for( ; ; )
	{
leave_L:
		/* search next triangle */
		for( k=0; k<3; k++)
		{
			nextTri = io->neighborlist[curTri*3+k];

			if( nextTri == -1 ) continue;

			if(  triMarks[nextTri] == 1) {
				i = nextTri*io->numberofcorners;

				j1=j2=j3=-1;
				for( j=0; j<3; j++)
				{
					if( io->trianglelist[i+j] == leaveEdge[0] )
						j1 = leaveEdge[0];
					else if( io->trianglelist[i+j] == leaveEdge[1] )
						j2 = leaveEdge[1];
					else	j3 = io->trianglelist[i+j];
				}
				
				if( j1 != -1 && j2 != -1 && j3 != -1 && IsValidTriangle(j1,j2,j3,rgn)) 	goto enter_L;
			}
		}


		if( reversed ) return cntpSum;

		/* if close */
		for( i=0; i<3; i++)
			if( io->neighborlist[curTri*3+i] == firstTri )
				return cntpSum;


				reversed = 1;
				ReverseCnt();
				curTri = firstTri;
				leaveEdge[0] = enterEdge[0];
				leaveEdge[1] = enterEdge[1];
				goto leave_L;

/*		for( i=0; i<3; i++)
		{
			if( io->neighborlist[curTri*3+i] == -1 ) {
			}
		}

		printf("error : not reaching boundary, curTri = %d \n", curTri);
		for( i=0; i<3; i++)
			printf("neighbor, %d %d\n", io->neighborlist[curTri*3+i],triMarks[io->neighborlist[curTri*3+i]]);
		printf("\n");
		return cntpSum;
*/

enter_L:
		curTri = nextTri;
		triMarks[curTri] = 0;

		x1 = io->pointlist[j1*2];
		y1 = io->pointlist[j1*2+1];
		z1 = io->pointattributelist[j1];

		x2 = io->pointlist[j2*2];
		y2 = io->pointlist[j2*2+1];
		z2 = io->pointattributelist[j2];

		x3 = io->pointlist[j3*2];
		y3 = io->pointlist[j3*2+1];
		z3 = io->pointattributelist[j3];

		if( (z > z2 && z < z3) || (z > z3 && z < z2 ) ) {
		//	if( fabs(x2-x3) != 1 || fabs(y2-y3) != 1 ) {
				dzm= (z -z2)/(z3-z2);
				cnt[cntpSum].x = x2 + (x3-x2)*dzm;
				cnt[cntpSum].y = y2 + (y3-y2)*dzm;
				cntpSum++;
		//	}

			leaveEdge[0] = j2;
			leaveEdge[1] = j3;
		}
		else if( (z > z3 && z < z1) || (z > z1 && z < z3 ) ) {
		//	if( fabs(x1-x3) != 1 || fabs(y1-y3) != 1 ) {
	    		dzm = (z -z3)/(z1-z3);
				cnt[cntpSum].x = x3 + (x1-x3)*dzm;
				cnt[cntpSum].y = y3 + (y1-y3)*dzm;
				cntpSum++;
		//	}

			leaveEdge[0] = j3;
			leaveEdge[1] = j1;
		}

	}
	return cntpSum;
}

 bool CGeoBuilderContour::IsValidTriangle(int j1,int j2,int j3,CRgn& rgn)
{
 
//	 return true;
	////
	double x[4];
	double y[4];
	x[0] = mid.pointlist[j1*2];
	y[0] = mid.pointlist[j1*2+1];
	x[1] = mid.pointlist[j2*2];
	y[1] = mid.pointlist[j2*2+1];
	x[2] = mid.pointlist[j3*2];
	y[2] = mid.pointlist[j3*2+1];
	x[3] = x[0];
	y[3] = y[0];

	double dis01=(x[0]-x[1])*(x[0]-x[1])+(y[0]-y[1])*(y[0]-y[1]);
	double dis12=(x[1]-x[2])*(x[1]-x[2])+(y[1]-y[2])*(y[1]-y[2]);
	double dis02=(x[0]-x[2])*(x[0]-x[2])+(y[0]-y[2])*(y[0]-y[2]);
	dis01=sqrt(dis01);
	dis12=sqrt(dis12);
	dis02=sqrt(dis02);
   if(m_lfDemInterval<0)
   {
     m_lfDemInterval=max(dis01,max(dis12,dis02));
   }
   else
    { 
	 if(dis01>2*m_lfDemInterval||dis12>2*m_lfDemInterval||dis02>2*m_lfDemInterval)
		 return false;
	 else
		 return true;
	} 
 
	bool bRet=true;
	for(int i=0;i<3;i++)
	{
		POINT pt;
		double midX,midY;
		midX=(x[i]+x[i+1])/2.0;

		if(fabs(x[i+1]-x[i])<1e-5)
		{
			for(int j=min(y[i],y[i+1])+1;j<max(y[i],y[i+1]);j++)
			{
				pt.x=midX;
				pt.y=j;
				if(!rgn.PtInRegion(pt))
				{
					bRet=false;
					break;
				}
			}
		}
		else
		{
			double k=(y[i+1]-y[i])/(x[i+1]-x[i]);
			double b=y[i]-k*x[i];
			
			for(int j=min(x[i],x[i+1])+1;j<max(x[i],x[i+1]);j++)
			{
				pt.x=j;
				pt.y=k*pt.x+b;
				if(!rgn.PtInRegion(pt))
				{
					bRet=false;
					break;
				}
			}
		}
	}


	return bRet;
}




int CGeoBuilderContour::TriContour(FILE *fvec, float idz)
{ 
	CRgn rgn;
	m_lfInterval=idz;
	//结束
	int i,edgeNum,n;
	float z;//,idz5;
	struct triangulateio *io = &mid;
	triMarks = (int *)malloc( (io->numberoftriangles+10)*sizeof(int));

	z = MinZ - fmod(MinZ, (double)idz); 
	if( cnt == NULL )
		cnt = (struct POINT2Df *)malloc( 102400*sizeof(struct POINT2Df));
	cntpSum = 0; 
  
	if(MinZ<MaxZ)
	{
		GProgressStart((MaxZ - MinZ)/idz);
    }
		
	double tmp;
   
 	for( ; z < MaxZ; z += idz)
	{
 
	    GProgressStep(); 

		if( fmod(z, idz5) == 0.0 )
			IndexFlag = 1;
		else	IndexFlag = 0;

 		for( i=0; i< io->numberofpoints; i++)
		{
			tmp = io->pointattributelist[i];

			if( z == io->pointattributelist[i] )
				io->pointattributelist[i] += 0.001;
		}

		SetCntMarks(z);
		for( i=0; i< io->numberoftriangles; i++)
		{
			if( triMarks[i] ) 
			{
				cntpSum=TraceACnt(z,i,rgn);
				TriDrawContour(fvec, z);
			}
		} 
	} 
    GProgressEnd(); 
	free( triMarks );
	free(cnt);	cnt=NULL; 
	return 1;
}
typedef struct taGeoPoint23D
{
	double x;
	double y;
	double z;
} POINT3D;

int numPoints=1;
int m_curp=0;
double m_Limit=0.2;
int StepCompress(POINT3D linePnts[],int bend)
{
	int			i=0,imax=0,packSum=0;
	double		dmax=0,d=0;
	double		A=0,B=0,C=0,D=0;
	POINT3D	*p1,*p2;

	if( numPoints - m_curp < 2 ) goto RET;

	p1 = &linePnts[m_curp];	p2=&linePnts[numPoints -1];

	A = p2->y - p1->y;
	B = p1->x - p2->x;
	C = p1->y * p2->x - p2->y * p1->x;
	D=A*A+B*B;
	if( D <= m_Limit*m_Limit ) goto RET;
	
	D = sqrt(D);

	dmax=0;
	for ( i=m_curp+1; i<numPoints-1; i++)
	{
		d = fabs( A * linePnts[i].x + B * linePnts[i].y + C );
		if( d > dmax )	{
			dmax = d;
			imax = i;
		}
	}

	if( bend==0 )
	{
		if( dmax/D <= m_Limit )	goto RET;
	}
	else
	{
		imax = numPoints-2;
	}
	
	packSum = imax - m_curp - 1;

	if( packSum > 0 )  {
		if( packSum > 2 )
		{
			m_curp++;
			linePnts[m_curp] = linePnts[m_curp + packSum/2];
			packSum--;
		}
		memcpy(&linePnts[m_curp+1],&linePnts[imax],(numPoints-imax)*sizeof(POINT3D));
		numPoints = (short)(numPoints - packSum);
	}
	m_curp++;

RET:
	return m_curp+1;
}

void CGeoBuilderContour::TriDrawContour(FILE *fvec, float iz)
{
	if(cntpSum==0)
		return;//add by zyong
	int i,color;
	float dx,dy;
	struct POINT2Df *xy=cnt;
	struct GeoPoint2 *xyz0,*xyz; 
	xy = cnt; 
	m_curp=0;
	numPoints=cntpSum;  
    m_pContour[iLineNum].iLineType=IndexFlag;
    m_pContour[iLineNum].gp=new GeoPoint[numPoints];
    m_pContour[iLineNum].iPtNum=numPoints;  
    //等高线的光滑处理


	for(i= 0; i<numPoints ; i++)
	{
      m_pContour[iLineNum].gp[i].x=(xy+i)->x;
      m_pContour[iLineNum].gp[i].y=(xy+i)->y;
	  m_pContour[iLineNum].gp[i].z=iz; 
	} 
 
	iLineNum++;
}
   
int CGeoBuilderContour::pointChk(struct clipWIN *DEMClip, double x, double y)
{
	if( (x >= DEMClip->left && x <= DEMClip->right) && 
		(y >= DEMClip->bottom && y <= DEMClip->top) )
		return POINTIN;
	else
		return POINTOUT;
}

int CGeoBuilderContour::LineClip(struct clipWIN *DEMClip, double *x1, double *y1, double *x2, double *y2)
{
	int retv1, retv2;
	double k;

	retv1 = pointChk(DEMClip, *x1, *y1);
	retv2 = pointChk(DEMClip, *x2, *y2);
	if( retv1 == POINTOUT && retv2 == POINTOUT )
		return LINEOUT;

	if( retv1 == POINTOUT )
	{
		Clipping(DEMClip, x2, y2, x1, y1);
		return 1;
	}
	else
	{
		Clipping(DEMClip, x1, y1, x2, y2);
		return 1;
	}

}

int CGeoBuilderContour::Clipping(struct clipWIN *DEMClip, 
					   double *xin, double *yin, double *xout, double *yout)
{
	double k, x, y;

	if( *xin == *xout && *yin > *yout )
	{
		*yout = DEMClip->bottom;
		return 1;
	}
	if( *xin == *xout && *yin < *yout )
	{
		*yout = DEMClip->top;
		return 1;
	}
	if( *yin == *yout && *xin > *xout )
	{
		*xout = DEMClip->left;
		return 1;
	}
	if( *yin == *yout && *xin < *xout )
	{
		*xout = DEMClip->right;
		return 1;
	}

	k = ( *yout - *yin ) / ( *xout - *xin );
	if( *xout <= *xin && *yout <= *yin )
	{
		y = k * (DEMClip->left - *xin) + *yin;
		x = (DEMClip->bottom - *yin) / k + *xin;

		if( y >= DEMClip->bottom && y<= DEMClip->top )
		{
			*yout = y;
		    *xout = DEMClip->left;
		    return 1;
		}
	    else
		{
			*xout = x;
		    *yout = DEMClip->bottom;
		    return 1;
		}
	}
	if( *xout <= *xin && *yout >= *yin )
	{
		y = k * (DEMClip->left - *xin) + *yin;
		x = (DEMClip->top - *yin) / k + *xin;

		if( y >= DEMClip->bottom && y<= DEMClip->top )
		{
			*yout = y;		
			*xout = DEMClip->left;
		    return 1;
		}
	    else
		{
			*xout = x;
		    *yout = DEMClip->top;
		    return 1;
		}
	}
	if( *xout >= *xin && *yout >= *yin )
	{
		y = k * (DEMClip->right - *xin) + *yin;
		x = (DEMClip->top - *yin) / k + *xin;

		if( y >= DEMClip->bottom && y<= DEMClip->top )
		{
			*yout = y;		
			*xout = DEMClip->right;
		    return 1;
		}
	    else
		{
			*xout = x;
		    *yout = DEMClip->top;
		    return 1;
		}
	}
	if( *xout >= *xin && *yout <= *yin )
	{
		y = k * (DEMClip->right - *xin) + *yin;
		x = (DEMClip->bottom - *yin) / k + *xin;

		if( y >= DEMClip->bottom && y<= DEMClip->top )
		{
			*yout = y;		
			*xout = DEMClip->right;
		    return 1;
		}
	    else
		{
			*xout = x;
		    *yout = DEMClip->bottom;
		    return 1;
		}
	}

	return 1;

}

void CGeoBuilderContour::SetClipWin(struct clipWIN *DEMClip, int left, int bottom, int right, int top)
{
	DEMClip->left = left;
	DEMClip->right = right;
	DEMClip->bottom = bottom;
	DEMClip->top = top;
}
 
struct POI
{
  CString strLayerName;
  CString strPOIName;
  double lfCoordX;
  double lfCoordY; 
}; 
void CGeoBuilderContour::SaveTXT2DXF(CString strDXF)
{ 

    CDxfDrawing m_pDxfDrawing;
    m_pDxfDrawing.Create();
	CString strPOI=strDXF+".dxf";
   	if(access(strPOI,2)!=2)
	{ 
		FILE * fp=fopen(strPOI,"wb");
		if(fp==NULL)
		{
			CString strMsg;
		//	strMsg.Format( IDSF_FILE_WRITE_OPEN_FAILED, strPOI );
			AfxMessageBox( strMsg );
			return  ;
		}
		else
		{
		 fclose(fp);
		} 
	}
	//extract lines from TXT 
    FILE * fp_poi=fopen(strDXF,"rt");
	char chTmp[512];
	CFArray<POI> arrayPOI;
    LAYER layer_first; 
    OBJHANDLE hlt = 0;
	{
		LTYPE ltype;
		memset(&ltype,0,sizeof(ltype));
		strcpy(ltype.Name, "Continuous");
		strcpy(ltype.DescriptiveText, "Solid line");
		hlt = m_pDxfDrawing.AddLinetype(&ltype);
	} 
	layer_first.StandardFlags = 0;
	layer_first.Color = 1; 
	layer_first.LineWeight = 0;
	layer_first.PlotFlag = FALSE;
	layer_first.PlotStyleObjhandle = 0;   
//	sprintf(layer_count.Name,"%s","8110"); 
//	m_pDxfDrawing.AddLayer(&layer_first); 
    POI m_POI; 
	int iNum=0;
    //EmitInstalledMessage();
	GProgressStart(100);
	int iCurNum=0;
	while(!feof(fp_poi))
	{
      fscanf(fp_poi,"%s",chTmp);
	  if(CString(chTmp).GetLength()==4)
	  {
		  iCurNum++;
		  if(iCurNum>10000)
		  {
			  GProgressStart(100);
			  iCurNum=0;
		  }
          if(iCurNum%100==0)
		  {
			  GProgressStep();
		  }

		  m_POI.strLayerName=chTmp;
          
		  fscanf(fp_poi,"%s%s",chTmp,chTmp); 
		  m_POI.strPOIName=chTmp;
		  fscanf(fp_poi,"%s",chTmp);
		  do
		  {
		    if(fscanf(fp_poi,"%s",chTmp)!=1) 
                goto EXIT;

		  }
		  while(strlen(chTmp)!=5); 
		  m_POI.lfCoordX=atof(chTmp);
              
          do
		  {
		    if(fscanf(fp_poi,"%s",chTmp)!=1) 
                goto EXIT;
		  }
		  while(strlen(chTmp)!=4);  
		  m_POI.lfCoordY=atof(chTmp);   
		 
		  {
		      m_pDxfDrawing.Text(m_POI.strPOIName,m_POI.lfCoordX*15,m_POI.lfCoordY*15,10,10); 
		  }
	  }
	} 
EXIT:
	m_pDxfDrawing.SaveDXFFile(strPOI);
	fclose(fp_poi);
	GProgressEnd();

} 
bool CGeoBuilderContour::SaveContour(CString strContour)
{ 
	//m_nRunning ++;
 
    CDxfDrawing m_pDxfDrawing;
    m_pDxfDrawing.Create();
 
    GProgressEnd();
	//EmitInstalledMessage();
	if(access(strContour,2)!=2)
	{
		FILE * fp=fopen(strContour,"wb");
		if(fp==NULL)
		{
			CString strMsg;
			//strMsg.Format( IDSF_FILE_WRITE_OPEN_FAILED, strContour );
			AfxMessageBox( strMsg );
			//m_nRunning --;
			return false;
		}
		else
		{
		 fclose(fp);
		}
	}

	CString strMsg;
	strMsg="start tracing...";
 
	GOutPut( strMsg );//"Begin to tracing contour line...");
	GProgressStart(GetContourNumber());
        LAYER layer_first;
	LAYER layer_count;
    OBJHANDLE hlt = 0;
	{
		LTYPE ltype;
		memset(&ltype,0,sizeof(ltype));
		strcpy(ltype.Name, "Continuous");
		strcpy(ltype.DescriptiveText, "Solid line");
		hlt = m_pDxfDrawing.AddLinetype(&ltype);
	} 
	layer_first.StandardFlags = 0;
	layer_first.Color = 1; 
	layer_first.LineWeight = 0;
	layer_first.PlotFlag = FALSE;
	layer_first.PlotStyleObjhandle = 0; 
	layer_first.LineTypeObjhandle = hlt; 
    layer_count=layer_first;
	 
	sprintf(layer_first.Name,"%s","8120");
	sprintf(layer_count.Name,"%s","8110"); 
	m_pDxfDrawing.AddLayer(&layer_first); 
    m_pDxfDrawing.AddLayer(&layer_count); 
    
 
    
	for(int i=0;i<GetContourNumber();i++)
	{
		GProgressStep();
		GeoLine * glTmp;
		 GetLine(i,glTmp);
		 PENTVERTEX pts; 
		 int num=glTmp->iPtNum;
		 if( num>0 )
		 {
			pts = new ENTVERTEX[num];
			if( pts )
			{
				ZeroMemory(pts,sizeof(ENTVERTEX)*num);
				for( int i=0; i<num; i++ )
				{
					pts[i].Point.x=glTmp->gp[i].x;
					pts[i].Point.y=glTmp->gp[i].y;
					pts[i].Point.z=glTmp->gp[i].z;
				} 
				if(num>0)
				{
				 if(float(int(pts[0].Point.z/(5*m_lfInterval))*5*m_lfInterval)
					 ==float(pts[0].Point.z))
                   m_pDxfDrawing.SetLayer("8120");
				 else
                   m_pDxfDrawing.SetLayer("8110");
				 m_pDxfDrawing.PolyLine(pts,num,8);
				}
				delete[] pts;
			}
		 }
	} 
	GProgressEnd();
	bool rv=m_pDxfDrawing.SaveDXFFile(strContour);

	if(rv==true)
	{
	 strMsg="success";//.Format( IDS_CONTOUR_OUTPUT_SUCCEED, strContour );//"Successfully output contour :%s",strContour);
	 GOutPut(strMsg);
	}
	else
	{ 
	 strMsg="failed";//strMsg.Format( IDS_CONTOUR_OUTPUT_FAILED, strContour );//"Failed output contour :%s",strContour);
	 GOutPut(strMsg);
	}

	//m_nRunning --;
	return rv;
}

/*
typedef struct GeoCalc_SP {
	double x, y,z;
	double xp,yp;
	double hp;
}SPParam;
 void CLineBase::Curve(PT_3D* pts, int nPts)
{
	
	typedef struct  
	{ 
		double	a,b,c; 
	}	ABC;
	
	SPParam* Sp=NULL, *Sp0=NULL;

	double	dx,dy,ds,cosn,sinn; 
	double	cos1,sin1,cos2,sin2; 
	double 	sinh1,d1,d2; 
	int 	i,n1,n2; 
 	ABC		*o,*o0; 
	double SpLen, Sigma;
	PT_3D   pt;

	if( nPts==0 )return;
	if( nPts==1 || nPts==2 )
	{
		m_pBuf->MoveTo(pts);
		m_pBuf->LineTo(pts+nPts-1);
		return;
	}
 
	Sp  = Sp0  = new SPParam[nPts+2]; 
	memset(Sp,0,sizeof(SPParam)*(nPts+2));

	int sum = nPts;
	double oldx =Sp->x = pts[0].x; 
	double oldy =Sp->y = pts[0].y;
	Sp->z = pts[0].z;
	Sp++;
	
	//将控制点复制到辅助数据结构中
	for (i=1;i<nPts;i++) 
	{ 
		if(oldx != pts[i].x || oldy != pts[i].y)
		{
			oldx = Sp->x = pts[i].x;	oldy = Sp->y = pts[i].y;
			Sp->z = pts[i].z; 
			Sp++; 
		}
		else
			sum --;
	
	} 

	//开始求解曲线参数
	Sp = Sp0;
	o0 = o = new ABC[sum+2];  
 
	n1 = sum-1;	n2 = sum-2; 
 
	dx = Sp[1].x - Sp->x;	dy = Sp[1].y - Sp->y; 
	ds = sqrt(dx*dx+dy*dy); 

	//if( fabs(ds) < Ellide )	ds = 3.4E-37;		
	
	cos1 = dx / ds;		sin1  = dy / ds; 
	
	Sp->xp = 0.0;		Sp->yp = 0.0; 
	Sp->hp = ds;	SpLen  = ds; 
 
	cosn=cos1,	sinn=sin1; 
	for( i=1; i<n1; i++) 
	{ 
		Sp++; 
		dx = Sp[1].x - Sp->x;	dy = Sp[1].y - Sp->y; 
		ds =  sqrt(dx*dx+dy*dy) ; 
		
		//if( fabs(ds) < Ellide )		ds = 3.4E-37;	
		
		cos2 = dx / ds;		sin2  = dy / ds; 
				
				
		Sp->xp = cos2 - cos1;	Sp->yp= sin2 - sin1; 
		Sp->hp = ds;			SpLen  += ds; 
		cos1=cos2;				sin1=sin2; 
	}	Sp++; 
 
	//if( fabs(SpLen) < Ellide) SpLen = 3.4E-37;	

	Sigma = 2*n1 / SpLen;  
	Sp	  = Sp0; 
	for(d1=0, i=0; i<n1; i++,d1=d2) 
	{ 
		ds   = Sigma * Sp->hp; 

		//if( fabs(ds)< Ellide)	ds = 3.4E-37;	
		
		sinh1= Sigma /  sinh(ds); 
		
		//if( fabs(Sp->hp) < Ellide)	Sp->hp = 3.4E-37;

		d2   =  ( exp(ds) + exp(-ds) )*0.5 * sinh1 - 1 / Sp->hp ; 
		o->b = d1+d2; 
		o->c = 1 / Sp->hp - sinh1; 
		o[1].a = o->c; 
		o++;	Sp++; 
	}	o->b = d1; 
 
	if( Sp->x != Sp0->x || Sp->y != Sp0->y ) 
	{ 
		Sp->xp =0;	Sp->yp = 0; 
 
		Sp=Sp0;	o=o0; 
		for( i=0; i<n1; i++) 
		{ 
			if( o->b == 0.0 ) o->b = 3.4E-37;

			o->c /= o->b; 
			Sp->xp /= o->b;	Sp->yp /= o->b; 
			o[1].b -= o[1].a*o->c; 
			Sp[1].xp -= o[1].a * Sp->xp; 
			Sp[1].yp -= o[1].a * Sp->yp; 
			Sp++;	o++; 
		} 
		if( o->b == 0.0 ) o->b = 3.4E-37; 

		Sp->xp /= o->b;	Sp->yp /= o->b; 
		for( i=n2; i>0; i--) 
		{ 
			Sp--;	o--; 
			Sp->xp  -= o->c * Sp[1].xp; 
			Sp->yp  -= o->c * Sp[1].yp; 
		} 
	} 
	else	
	{	// is close 
		double *an,*cn; 

		an = new double[sum];; 
		cn = new double[sum];
		memset(an,0,sum*sizeof(double)); 
		memset(cn,0,sum*sizeof(double)); 
 
		an[0] = cn[0] = o0->a = o->a; 
		o0->b += o->b; 
		Sp0->xp = Sp->xp = cosn - cos1; 
		Sp0->yp = Sp->yp = sinn - sin1; 
 
		Sp = Sp0;	o = o0; 
		for( i=0; i<n2; i++) 
		{ 
			if( o->b == 0.0 ) o->b = 3.4E-37;
			
			o->c /= o->b;	*an  /= o->b; 
			Sp->xp /= o->b;	Sp->yp /= o->b; 
			o[1].b -= o[1].a * o->c; 
			an[1]  -= o[1].a * *an; 
			cn[1] -= *cn * o->c; 

			Sp[1].xp -= o[1].a * Sp->xp; 
			Sp[1].yp -= o[1].a * Sp->yp; 
 
			o0[n2].b -= *cn * *an; 
			Sp0[n2].xp -= *cn * Sp->xp; 
			Sp0[n2].yp -= *cn * Sp->yp; 
			Sp++;	o++;	an++;	cn++; 
		} 
		//if( fabs(o->b) < Ellide) o->b = 3.4E-37; 

		Sp->xp /= o->b;	Sp->yp /= o->b; 
		for( i=0; i<n2; i++) 
		{ 
			Sp--;	o--;	an--;	cn--; 
			Sp->xp  -= o->c * Sp[1].xp + *an * Sp0[n2].xp; 
			Sp->yp  -= o->c * Sp[1].yp + *an * Sp0[n2].yp; 
		} 
		delete[] an; delete[] cn;
 
		Sp0[n1].xp = Sp0->xp; 
		Sp0[n1].yp = Sp0->yp; 
	} 

	delete[] o0;

	//绘制曲线
	double  dh; 
	double  xx,yy,xxp,yyp,xxp1,yyp1,hpj; 
	double  sinh2,sinh3,xso,yso,zso; 
	//double  cosh1,cosh2,r; 
	int    j; 
	int	   density = 10; // Point insert density
	
	//if( density < 1 ) density = 1;
	
	Sp = Sp0;

	for( i=0 ; i<sum-1; i++)
	{
		xxp = Sp->x - Sp->xp;	xxp1= Sp[1].x - Sp[1].xp; 
		yyp = Sp->y - Sp->yp;	yyp1= Sp[1].y - Sp[1].yp; 
		
		sinh3 = sinh( Sigma* Sp->hp ); 
		
		dh = (Sp[1].z - Sp->z) / Sp->hp; 
		
		for( j=0; j<Sp->hp; j++) 
		{
			zso = Sp->z+dh*j; 
			
			hpj   = Sp->hp - j; 
			sinh1 =  sinh( Sigma* hpj ); 
			sinh2 =  sinh( Sigma* j ); 
			
			xx=xso = (Sp->xp * sinh1 + Sp[1].xp * sinh2 ) / sinh3 + 
				( xxp * hpj + xxp1 * j )/ Sp->hp; 
			yy=yso = (Sp->yp * sinh1 + Sp[1].yp * sinh2 ) / sinh3 + 
				( yyp * hpj + yyp1 * j )/ Sp->hp; 

			if( i==0 && j==0 ) 
			{
				pt.x = xso;
				pt.y = yso;
				pt.z = zso;
				m_pBuf->MoveTo(&pt);
				continue; 
			}		
			if( j==0 || (j+1)>Sp->hp || (j%density)==0 ) 
			{
				pt.x = xso;
				pt.y = yso;
				pt.z = zso;
				m_pBuf->LineTo(&pt); 
			}
		}
		Sp++;
	}

	//确保完整
	COPY_3DPT(pt,pts[nPts-1]);
	m_pBuf->LineTo(&pt); 

	delete [] Sp0;
}
*/