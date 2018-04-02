#include "stdafx.h"
#include <memory.h>
#include "matrix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _FABS(x) ((x)>=0?(x):(-(x)))

//矩阵相乘
void matrix_multiply(const double *m1, const double *m2, int n, double *r)
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

//矩阵乘向量
void matrix_multiply_byvector(const double *mt, int m, int n, double *v, double *r)
{
	int i,k;
	double t;
	for( i=0; i<n; i++ )
	{
		t = 0.0;
		for( k=0; k<m; k++)
		{
			t += (mt[i*m+k]*v[k]);
		}

		r[i] = t;
	}
	return;
}

//置矩阵为单位矩阵
void matrix_toIdentity(double *m, int n)
{
	int i, N = n*n;
	memset(m,0,sizeof(double)*N);
	for( i=0; i<N; i+=(n+1) )
	{
		m[i] = 1.0;
	}

	return;
}

//判断矩阵是否为单位矩阵
bool matrix_isIdentity(const double *m, int n)
{
	int i,j;
	for( i=0; i<n; i++ )
	{
		for( j=0; j<n; j++ )
		{
			if( i==j && _FABS((m[i*n+j]-1))>1e-8 )return false;
			else if( i!=j && _FABS((m[i*n+j]))>1e-8 )return false;
		}
	}

	return true;
}

//求逆矩阵（采用初等变换，高斯列主消元法）
bool matrix_reverse(const double *m, int n, double *r)
{
	int i,j,k,p; 
	int *flag; 
	double l; 
	double *tm;

	flag = new int[n];
	tm = new double[n*n];
	if( !flag || !tm )
	{
		if( tm )delete[] tm;
		if( flag )delete[] flag;
		return false;
	}

	memset(flag,0,sizeof(int)*n);
	memcpy(tm,m,sizeof(double)*n*n);

	//将r置为单位方阵
	matrix_toIdentity(r,n);
	
	//按列循环
	for(i=0;i<n;i++)
	{
		//求出i列绝对值最大数
		p=-1; 
		for(j=0;j<n;j++) 
		{
			if(!flag[j] && _FABS((tm[j*n+i]))>0.0)
			{
				if(p==-1) p=j; 
				else if( _FABS((tm[p*n+i]))<_FABS((tm[j*n+i])) ) p=j; 
			}
		}   
		
		if( p==-1 ) 
			return false;
		if( _FABS((tm[p*n+i]))<=0.0 )
			return false;

		//flag[p]为零，表示p行还没有主元，为1+i，表示此行的主元是在i列上
		flag[p]=1+i; 
		
		//逐行消去第i列系数元(不含拥有列主元的那一行)
		for(j=0;j<n;j++) 
		{
			if(p!=j)
			{ 
				l=tm[j*n+i]/tm[p*n+i]; 
				for(k=i;k<n;k++)
				{
					tm[j*n+k]-=(tm[p*n+k]*l);
				}

				for(k=0;k<n;k++)
				{				
					r[j*n+k]-=(r[p*n+k]*l); 
				}
			}
		}
		
		//将拥有列主元的那一行的系数归一化
		l=1.0/tm[p*n+i]; 
		for(k=i;k<n;k++)
		{
			tm[p*n+k]=(tm[p*n+k]*l);
		}

		for(k=0;k<n;k++)
		{
			r[p*n+k]=(r[p*n+k]*l); 
		}		
	}

	//置换各行，使 tm 矩阵对角化(为单位矩阵)
	for(i=0; i<n; i++)
	{	
		memcpy( tm+(flag[i]-1)*n, r+i*n, sizeof(double)*n);
	}

	memcpy(r,tm,sizeof(double)*n*n);

	delete[] tm;
	delete[] flag;

	return true;
}



double matrix_modulus(const double *m, int n)
{
	if( n<0 )
	{
		return 0;
	}
	if( n==1 )
	{
		return m[0];
	}
	double v = 0;
	double *buf = new double[(n-1)*(n-1)];

	if( !buf )
		return 0;
	
	const double *p1;
	double *p2;
	for(int i=0; i<n; i++)
	{
		//取出去掉0行i列后的n-1维子矩阵，放到buf中
		p2 = buf;
		for( int j=0; j<n-1; j++)
		{
			p1 = m + (j+1)*n;
			for( int k=0; k<n; k++, p1++)
			{
				if( k!=i )*p2++ = *p1;
			}
		}
		//求模
		if( (i%2)==0 )
			v = v + m[i]*matrix_modulus(buf,n-1);
		else
			v = v - m[i]*matrix_modulus(buf,n-1);
	}

	delete[] buf;

	return v;
}