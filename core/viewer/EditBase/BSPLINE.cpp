// BSPLINE.cpp
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

/*
Subroutine to generate a B-spline open knot vector with multiplicity
equal to the order at the ends.

  c            = order of the basis function
  n            = the number of defining polygon vertices
  nplus2       = index of x() for the first occurence of the maximum knot vector value
  nplusc       = maximum value of the knot vector -- $n + c$
  x()          = array containing the knot vector
*/

static void knot(int n, int c, int *x)
{
	int nplusc,nplus2,i;
	
	nplusc = n + c;
	
	x[0] = 0;
	for (i = 1; i < nplusc; i++){
		if ( (i >= c) && (i <=n) )
			x[i] = x[i-1] + 1;
		else
			x[i] = x[i-1];
	}
}


static void rbasis(int c, double t, int npts, int *x, double *h, double *r)
{
	int nplusc;
	int i,j,k;
	double d,e;
	double sum;
	double *temp;
	
	nplusc = npts + c;

	temp = new double[nplusc];
	
	/* calculate the first order nonrational basis functions n[i]	*/
	
	for (i = 0; i< nplusc-1; i++){
		if (( t >= x[i]) && (t < x[i+1]))
			temp[i] = 1;
		else
			temp[i] = 0;
	}
	
	/* calculate the higher order nonrational basis functions */
	
	for (k = 2; k<=c; k++)
	{
		for (i = 0; i < nplusc-k; i++)
		{
			if (temp[i] != 0)    /* if the lower order basis function is zero skip the calculation */
				d = ((t-x[i])*temp[i])/(x[i+k-1]-x[i]);
			else
				d = 0;
			
			if (temp[i+1] != 0)     /* if the lower order basis function is zero skip the calculation */
				e = ((x[i+k]-t)*temp[i+1])/(x[i+k]-x[i+1]);
			else
				e = 0;
			
			temp[i] = d + e;
		}
	}
	
	if (t == (float)x[nplusc-1]){		/*    pick up last point	*/
		temp[npts-1] = 1;
	}

	/* calculate sum for denominator of rational basis functions */
	
	sum = 0.;
	for (i = 0; i <npts; i++){
		sum = sum + temp[i]*h[i];
	}
	
	/* form rational basis functions and put in r vector */
	
	for (i = 0; i <npts; i++){
		if (sum != 0){
			r[i] = (temp[i]*h[i])/(sum);}
		else
			r[i] = 0;
	}

	delete[] temp;
}


/*  Subroutine to generate a rational B-spline curve using an uniform open knot vector

	C code for An Introduction to NURBS
	by David F. Rogers. Copyright (C) 2000 David F. Rogers,
	All rights reserved.
	
	Name: rbspline.c
	Language: C
	Subroutines called: knot.c, rbasis.c, fmtmul.c
	Book reference: Chapter 4, Alg. p. 297

    pts         = array containing the defining polygon vertices
                  b[1] contains the x-component of the vertex
                  b[2] contains the y-component of the vertex
                  b[3] contains the z-component of the vertex
	h[]			= array containing the homogeneous weighting factors 
    k           = order of the B-spline basis function
    nbasis      = array containing the basis functions for a single value of t
    nplusc      = number of knot values
    npts        = number of defining polygon vertices
    pts_out		= array containing the curve points
                  p[1] contains the x-component of the point
                  p[2] contains the y-component of the point
                  p[3] contains the z-component of the point
    npts2          = number of points to be calculated on the curve
    t           = parameter value 0 <= t <= npts - k + 1
    x[]         = array containing the knot vector
*/

void rbspline(const PT_3DEX *pts, int npts, int k, double *h, int npts2, PT_3D *pts_out)
{
	int i,j,jcount;
	int i1;
	int *x;		/* allows for 20 data points with basis function of order 5 */
	int nplusc;

	double step;
	double t;
	double *nbasis;
	double temp;

	double *h1 = NULL;

	if( h==NULL )
	{
		h1 = new double[npts];
		for(i = 0; i < npts; i++){
			h1[i] = 1.0;
		}
		h = h1;
	}

	nplusc = npts + k;

/*  zero and redimension the knot vector and the basis array */

	nbasis = new double[npts];
	for(i = 0; i < npts; i++){
		 nbasis[i] = 0.;
	}

	x = new int[nplusc];
	for(i = 0; i < nplusc; i++){
		 x[i] = 0.;
		}

/* generate the uniform open knot vector */

	knot(npts,k,x);

/*    calculate the points on the rational B-spline curve */

	t = 0;
	step = ((float)x[nplusc-1])/((float)(npts2-1));

	for (i1 = 0; i1< npts2; i1++)
	{
		if ((float)x[nplusc-1] - t < 5e-6){
			t = (float)x[nplusc-1];
		}

	    rbasis(k,t,npts,x,h,nbasis);      /* generate the basis function for this value of t */

		pts_out[i1].x = 0;
		pts_out[i1].y = 0;
		pts_out[i1].z = 0;
		for (i = 0; i <npts; i++) /* Do local matrix multiplication */
		{
			if( nbasis[i]==0.0 )continue;

			temp = nbasis[i]*pts[i].x;
			pts_out[i1].x = pts_out[i1].x + temp;

			temp = nbasis[i]*pts[i].y;
			pts_out[i1].y = pts_out[i1].y + temp;

			temp = nbasis[i]*pts[i].z;
			pts_out[i1].z = pts_out[i1].z + temp;
		}

		t = t + step;
	}

	delete[] nbasis;
	delete[] x;
	if( h1 )delete[] h1;
}

