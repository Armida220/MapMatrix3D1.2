#include "stdafx.h"
#include "License.h"
#include "ExMessage.h"
#include "SmartViewFunctions.h"
#include "vsEncrypt.h"

#include "sqlite3.h"
#pragma comment( lib,"sqlite.lib" )

BOOL CheckLicense(int idx)
{
	return true;// (1 == sqlite3_checklicense(idx));
}

BOOL ExecuteLicense(int idx, int fun, int msg_id, LPVOID in, DWORD nIn, LPVOID out, DWORD nOut, LPDWORD pnret)
{
	return TRUE;
}
void ClearLicense(int idx)
{

}

void InitLicense()
{
#ifndef _LICEDU
	ExecuteLicense(0, LIC_INIT, 0, 0, 0, 0, 0);
#endif
}

double Modulus3(double *m)
{
	return (m[0]*(m[4]*m[8]-m[5]*m[7])-m[1]*(m[3]*m[8]-m[5]*m[6])+m[2]*(m[3]*m[7]-m[4]*m[6]));
}


void Matrix_reverse31(double *m, double *r)
{		
	double mm = m[0]*m[4]*m[8]-m[0]*m[5]*m[7]-m[1]*m[3]*m[8]+m[1]*m[5]*m[6]+m[2]*m[3]*m[7]-m[2]*m[4]*m[6];
	mm = 1/mm;
	r[0] = (m[4]*m[8]-m[5]*m[7])*mm, r[1] =-(m[1]*m[8]-m[2]*m[7])*mm, r[2] = (m[1]*m[5]-m[2]*m[4])*mm;
	r[3] =-(m[3]*m[8]-m[5]*m[6])*mm, r[4] = (m[0]*m[8]-m[2]*m[6])*mm, r[5] =-(m[0]*m[5]-m[2]*m[3])*mm;
	r[6] = (m[3]*m[7]-m[4]*m[6])*mm, r[7] =-(m[0]*m[7]-m[1]*m[6])*mm, r[8] = (m[0]*m[4]-m[1]*m[3])*mm;	
}

static void _matrix_multiply(const double *m1, const double *m2, int n, double *r)
{
	int i,j,k;
	double v;
	for( i=0; i<n; i++ )
	{
		for( j=0; j<n; j++ )
		{
			v = 0.0;
			for( k=0; k<n; k++)
			{
				v += (m1[i*n+k]*m2[k*n+j]);
			}
			
			r[i*n+j] = v;
		}
	}
	
	return;
}


BOOL CreateMatrix3(double *m)
{
	double v = ::GetTickCount();
	while( 1 )
	{
		double d = 1+sqrt(v);
		v = d*1.5;
		d = d-(int)d;
		if( d<1e-2 )continue;

		double buf[4];
		
		for( int i=0; i<4; i++)
		{
			d = d*100;
			buf[i] = sqrt(d)*3.6*3.1416/180;
			d = d-(int)d;
		}

		double m1[9] = {
			cos(buf[0]),sin(buf[0]),0,
			-sin(buf[0]),cos(buf[0]),0,
			0,0,1
		};

		double m2[9] = {
			cos(buf[1]),0,sin(buf[1]),
			0,1,0,
			-sin(buf[1]),0,cos(buf[1])				
		};

		double m3[9] = {			
			1,0,0,
			0,cos(buf[2]),sin(buf[2]),
			0,-sin(buf[2]),cos(buf[2])				
		};

		double m4[9];

		_matrix_multiply(m1,m2,3,m4);
		_matrix_multiply(m4,m3,3,m);

		if( buf[3]<0.1 )
			buf[3] = sqrt(buf[3] + 3.0);

		for( i=0; i<9; i++)
		{
			m[i] *= buf[3];
		}

		break;
	}
	
	return TRUE;
}
