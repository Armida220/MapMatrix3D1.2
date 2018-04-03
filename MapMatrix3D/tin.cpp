#include "stdafx.h"


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <math.h>

#include "triangle2.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void free2(void *p)
{
	if( p!=NULL )
		free(p);
}

#define free free2


namespace MyTriangle{

double toler = 1e-6;

int compare_func( const void *arg1, const void *arg2 )
{
	MYPT_3D *p1 = (MYPT_3D*)arg1;
	MYPT_3D *p2 = (MYPT_3D*)arg2;

	if( p1->x-p2->x>toler )
		return 1;
	if( p1->x-p2->x<-toler )
		return -1;
	if( p1->y-p2->y>toler )
		return 1;
	if( p1->y-p2->y<-toler )
		return -1;
	return 0;
}

	
void sortPT3D(MYPT_3D *pts, int npt)
{
	qsort(pts,npt,sizeof(MYPT_3D),compare_func);
}


double getDis(MYPT_3D pt1, MYPT_3D pt2)
{
	return sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x) + (pt1.y-pt2.y)*(pt1.y-pt2.y));
}


void createTIN(MYPT_3D *pts, int npt, MYTIN **tins, int *num, MYPT_3D **pts2, int *npt2)
{
	if (pts == NULL)
	{
		*num = 0;
		*npt2 = 0;
		return;
	}
	toler = GraphAPI::g_lfDisTolerance;
	
	//--------------预处理，去除重复点
	//排序顶点
	MYPT_3D *pts3 = new MYPT_3D[npt];
	memcpy(pts3,pts,npt*sizeof(MYPT_3D));
	
	for( int i=0; i<npt; i++)
	{
		pts3[i].iold = i;
	}
	
	sortPT3D(pts3,npt);
	
	//合并重复点
	int pos = 0, j;
	
	pts[pts3[pos].iold].isort = pos;
	
	for( i=1; i<npt; i++)
	{
		if( fabs(pts3[pos].x-pts3[i].x)<toler && 
			fabs(pts3[pos].y-pts3[i].y)<toler )
		{
			pts[pts3[i].iold].isort = pos;
		}
		else
		{
			pos++;
			pts3[pos] = pts3[i];
			
			pts[pts3[i].iold].isort = pos;
			
		}
	}

	int npt_new = pos+1;

	struct triangulateio in, mid, vorout;
	
	/* Define input points. */

	memset(&in,0,sizeof(mid));
	
	in.numberofpoints = npt_new;
	in.numberofpointattributes = 1;
	in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));
	in.pointattributelist = (REAL *) malloc(in.numberofpoints * sizeof(REAL));
	
	for( i=0, j=0; i<npt_new; i++, j+=2)
	{
		in.pointlist[j] = pts3[i].x;
		in.pointlist[j+1] = pts3[i].y;
		in.pointattributelist[i] = pts3[i].z;	
	}

	int nseg = 0;
	for( i=0; i<npt; i++)
	{
		if( pts[i].type==1 && i<(npt-1) && pts[i+1].type==1 && 
			pts[i].isort!=pts[i+1].isort && pts[i].id==pts[i+1].id )
			nseg++;
	}

	in.pointmarkerlist = NULL;
	
	in.numberofsegments = nseg;
	in.segmentlist = (int*) malloc(in.numberofsegments * 2 * sizeof(int));

	nseg = 0;

	for( i=0; i<npt; i++)
	{
		if( pts[i].type==1 && i<(npt-1) && pts[i+1].type==1 && 
			pts[i].isort!=pts[i+1].isort && pts[i].id==pts[i+1].id )
		{
			in.segmentlist[nseg+nseg] = pts[i].isort;
			in.segmentlist[nseg+nseg+1] = pts[i+1].isort;
			nseg++;
		}
	}

	in.numberofholes = 0;
	in.numberofregions = 0;

	in.regionlist = NULL;
		
	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */

	memset(&mid,0,sizeof(mid));
	
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

	memset(&vorout,0,sizeof(vorout));
	

	triangulate("pczAevn", &in, &mid, &vorout);

	if( mid.numberoftriangles>0 )
	{
		MYTIN *buf = new MYTIN[mid.numberoftriangles];
		
		for (i = 0; i<mid.numberoftriangles; i++) 
		{
			buf[i].i1 = mid.trianglelist[i*3];
			buf[i].i2 = mid.trianglelist[i*3+1];
			buf[i].i3 = mid.trianglelist[i*3+2];

// 			if( buf[i].i1>=in.numberofpoints || buf[i].i2>=in.numberofpoints || buf[i].i3>=in.numberofpoints )
// 			{
// 				int aa=1;
// 			}
		}

		*tins = buf;
		*num = mid.numberoftriangles;
	}			

	if( pts2 )
	{
		MYPT_3D *ptst = new MYPT_3D[mid.numberofpoints];

		for (i = 0; i<mid.numberofpoints; i++) 
		{
			ptst[i].x = mid.pointlist[i+i];
			ptst[i].y = mid.pointlist[i+i+1];
			ptst[i].z = mid.pointattributelist[i];

			if( i<npt_new )
			{
				ptst[i].iold = pts3[i].iold;
			}
			else
			{
				ptst[i].iold = -1;
			}
		}

		*pts2 = ptst;
		*npt2 = mid.numberofpoints;
	}

	delete[] pts3;

	free(in.pointlist);
	free(in.pointattributelist);
	free(in.pointmarkerlist);
	free(in.segmentlist);
	free(in.regionlist);
	free(mid.pointlist);
	free(mid.pointattributelist);
	free(mid.pointmarkerlist);
	free(mid.trianglelist);
	free(mid.triangleattributelist);
	free(mid.trianglearealist);
	free(mid.neighborlist);
	free(mid.segmentlist);
	free(mid.segmentmarkerlist);
	free(mid.edgelist);
	free(mid.edgemarkerlist);
	free(vorout.pointlist);
	free(vorout.pointattributelist);
	free(vorout.edgelist);
	free(vorout.normlist);
	
}

}

