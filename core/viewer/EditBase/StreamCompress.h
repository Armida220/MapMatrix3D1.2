// StreamCompress.h: interface for the CStreamCompress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STREAMCOMPRESS_H__4C6A948A_007A_4BE8_B95B_C2DAF19B9DEE__INCLUDED_)
#define AFX_STREAMCOMPRESS_H__4C6A948A_007A_4BE8_B95B_C2DAF19B9DEE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SmartViewFunctions.h"
#include "SmartViewBaseType.h"

// template<class Type>
// class CStreamCompress
// {
// public:
// 	CStreamCompress();
// 	virtual ~CStreamCompress();
// 	
// 	void BeginCompress(double limit=-1);
// 	void EndCompress();
// 	int  AddStreamPt(Type pt);
// 	BOOL GetLastCompPt(Type &pt);
// 	BOOL GetCurCompPt(Type &pt);
// 
// 	inline void SetLimit(double limit){ m_lfLimit = limit;	}
// 	inline double GetLimit(){ return m_lfLimit;	}
// 	
// private:
// 	double m_lfLimit;
// 	Type m_ptCurComp;
// 	Type m_ptLastComp;
// 	int m_nCurPt;
// 
// 	double m_lfMinX, m_lfMinY, m_lfMaxX, m_lfMaxY;
// };
// 
// 
// template<class Type>
// CStreamCompress<Type>::CStreamCompress()
// {
// 	m_nCurPt = -1;
// 	m_lfLimit= -1;
// 	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;
// }
// 
// template<class Type>
// CStreamCompress<Type>::~CStreamCompress()
// {
// 	
// }
// 
// template<class Type>
// void CStreamCompress<Type>::BeginCompress(double limit)
// {
// 	if( limit>0 )m_lfLimit = limit;
// 	m_nCurPt = 0;
// 	m_lfMinX = m_lfMaxX = m_lfMinY = m_lfMaxY = 0;
// }
// 
// 
// template<class Type>
// void CStreamCompress<Type>::EndCompress()
// {
// 	m_nCurPt = -1;
// }
// 
// template<class Type>
// BOOL CStreamCompress<Type>::GetLastCompPt(Type &pt)
// {
// 	if( m_nCurPt>=2 )
// 	{
// 		pt = m_ptLastComp;
// 		return TRUE;
// 	}
// 	return FALSE;
// }
// 
// template<class Type>
// BOOL CStreamCompress<Type>::GetCurCompPt(Type &pt)
// {
// 	if( m_nCurPt>=1 )
// 	{
// 		pt = m_ptCurComp;
// 		return TRUE;
// 	}
// 	return FALSE;
// }
// 
// template<class Type>
// int  CStreamCompress<Type>::AddStreamPt(Type pt)
// {
// 	if( m_nCurPt<0 )
// 		return 0;
// 	
// 	if( m_nCurPt==0 )
// 	{
// 		m_ptCurComp = pt;
// 		m_nCurPt++;
// 		return 1;
// 	}
// 	
// 	if( m_nCurPt==1 )
// 	{
// 		m_ptLastComp = m_ptCurComp;
// 		m_ptCurComp = pt;
// 		m_lfMaxX = sqrt((m_ptLastComp.x-m_ptCurComp.x)*(m_ptLastComp.x-m_ptCurComp.x)+(m_ptLastComp.y-m_ptCurComp.y)*(m_ptLastComp.y-m_ptCurComp.y));
// 		m_nCurPt++;
// 		return 2;
// 	}
// 
// 	m_nCurPt++;
//  
// 	//求 m_ptCurComp 在 m_ptLastComp-pt 直线上的垂足
// 	double x,y,t;
// 	{
// 		double dx,dy;
// 		
// 		dx = pt.x-m_ptLastComp.x; dy = pt.y-m_ptLastComp.y;
// 		if( _FABS(dx)<1e-10 && _FABS(dy)<1e-10 )
// 		{
// 			x = m_ptLastComp.x; y = m_ptLastComp.y;
// 			t = 0;
// 		}
// 		else
// 		{
// 			t = (dx*(m_ptCurComp.x-m_ptLastComp.x )+dy*(m_ptCurComp.y-m_ptLastComp.y))/(dx*dx+dy*dy);
// 			x= m_ptLastComp.x + dx*t;	y= m_ptLastComp.y + dy*t;
// 		}
// 	}
// 
// 	//判断 m_ptCurComp 在m_ptLastComp-pt 直线上的方位(是向左还是向右)
// 	PT_3D pts[3];
// 	pts[0].x = m_ptLastComp.x;  pts[0].y = m_ptLastComp.y; pts[0].z = 0;
// 	pts[1].x = m_ptCurComp.x; pts[1].y = m_ptCurComp.y; pts[1].z = 0; 
// 	pts[2].x = pt.x;  pts[2].y = pt.y; pts[2].z = 0; 
// 	
// 	bool bclockwise = GIsClockwise(pts,3)==1;
// 
// 	//计算各个方向的偏距
// 	double disy = sqrt((x-m_ptCurComp.x)*(x-m_ptCurComp.x)+(y-m_ptCurComp.y)*(y-m_ptCurComp.y));
// 	double disx = sqrt((x-pt.x)*(x-pt.x)+(y-pt.y)*(y-pt.y));
// 
// 	if( t>=1.0 )
// 	{
// 		m_lfMaxX += disx;
// 		if( m_lfMinX>=disx )m_lfMinX -= disx;
// 		else m_lfMinX = 0;
// 	}
// 	else
// 	{
// 		m_lfMinX += disx;
// 		if( m_lfMaxX>=disx )m_lfMaxX -= disx;
// 		else m_lfMaxX = 0;
// 	}
// 
// 	if( bclockwise )
// 	{
// 		m_lfMaxY += disy;
// 		if( m_lfMinY>=disy )m_lfMinY -= disy;
// 		else m_lfMinY = 0;
// 	}
// 	else
// 	{
// 		m_lfMinY += disy;	
// 		if( m_lfMaxY>=disy )m_lfMaxY -= disy;
// 		else m_lfMaxY = 0;
// 	}
// 
// 	double minX = m_lfMaxX<m_lfMinX?m_lfMaxX:m_lfMinX;
// 	if( minX>=m_lfLimit || m_lfMinY>=m_lfLimit || m_lfMaxY>=m_lfLimit )
// 	{
// 		m_ptLastComp = m_ptCurComp;
// 		m_ptCurComp = pt;
// 		m_lfMaxX = sqrt((m_ptLastComp.x-m_ptCurComp.x)*(m_ptLastComp.x-m_ptCurComp.x)+(m_ptLastComp.y-m_ptCurComp.y)*(m_ptLastComp.y-m_ptCurComp.y));
// 		m_lfMinX = m_lfMinY = m_lfMaxY = 0;
// 		return 2;
// 	}
// 
// 	m_ptCurComp = pt;
// 	return 1;
// }

template<class Type>
class CStreamCompress
{
public:
	CStreamCompress();
	virtual ~CStreamCompress();

	void BeginCompress(Type *pts, int num, double limit=0.1, bool resetStart=true);
	int StepCompress();
	int EndCompress();
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}
private:
	int StepCompress(bool bEnd);
public:
	Type *m_pPts;

	int m_nNum;
	int m_nCurPt;
	double m_lfLimit;

	double m_lfMinLen;
	double m_lfMinHei;
	double m_lfMaxAng;
	double m_lfMinArea;
};


template<class Type>
CStreamCompress<Type>::CStreamCompress()
{
	m_lfMinLen = 1.0;
	m_lfMinHei = 1.0;
	m_lfMaxAng = 1.0;
	m_lfMinArea = 1.0;

	m_pPts = NULL;

	m_nNum = 0;
	m_nCurPt = 0;
	m_lfLimit = 0;
}

template<class Type>
CStreamCompress<Type>::~CStreamCompress()
{
	
}

template<class Type>
void CStreamCompress<Type>::BeginCompress(Type *pts, int num, double limit, bool resetStart)
{
	if( resetStart )m_nCurPt = 0;
	m_pPts = pts;
	m_nNum = num;
	m_lfLimit = limit;
}

template<class Type>
int CStreamCompress<Type>::StepCompress()
{
	return StepCompress(false);
}

template<class Type>
int CStreamCompress<Type>::StepCompress(bool bEnd)
{	
	int			i=0,imax=0,packSum=0;
	double		dmax=0,d=0;
	double		A=0,B=0,C=0,D=0;
	Type		*p1,*p2;
	
	if( !m_pPts || m_nNum<=2 || m_nCurPt<0 )return 0;
	
	if( m_nNum - m_nCurPt < 2 ) goto RET;
	
	p1 = m_pPts+m_nCurPt;	p2=m_pPts+m_nNum-1;
	
	A = p2->y - p1->y;
	B = p1->x - p2->x;
	C = p1->y * p2->x - p2->y * p1->x;
	D=A*A+B*B;
	if( D <= m_lfLimit*m_lfLimit ) goto RET;
	
	D = sqrt(D);
	
	dmax=0;
	for ( i=m_nCurPt+1; i<m_nNum-1; i++)
	{
		d = fabs( A * m_pPts[i].x + B * m_pPts[i].y + C );
		if( d > dmax )	{
			dmax = d;
			imax = i;
		}
	}
	
	if( bEnd==false )
	{
		if( dmax/D <= m_lfLimit )	goto RET;
	}
	else
	{
		imax = m_nNum-2;
	}
	
	packSum = imax - m_nCurPt - 1;
	
	if( packSum > 0 )  
	{
		if( packSum > 2 )
		{
			m_nCurPt++;
			m_pPts[m_nCurPt] = m_pPts[m_nCurPt + packSum/2];
			packSum--;
		}
		memcpy(m_pPts+m_nCurPt+1,m_pPts+imax,(m_nNum-imax)*sizeof(Type));
		m_nNum = short(m_nNum - packSum);
	}
	m_nCurPt++;
	
RET:
	return m_nCurPt+1;
}

template<class Type>
int CStreamCompress<Type>::EndCompress()
{
	return StepCompress(true);
}


MyNameSpaceBegin

// 
//管道压缩，
class EXPORT_EDITBASE CStreamCompress2_PT_3D
{
public:
	CStreamCompress2_PT_3D();
	virtual ~CStreamCompress2_PT_3D();
	
	void BeginCompress(double limit=-1);
	void EndCompress();
	int  AddStreamPt(PT_3D pt);
	BOOL GetPts(PT_3D *&pts, int &npt);
	BOOL GetCurCompPt(PT_3D &pt);
	
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}
	
protected:
	void Compress();

public:
	double m_lfMaxLen;
private:
	double m_lfLimit;
	int m_nCurPt;
	
	CArray<PT_3D,PT_3D> m_arrOldPts;
	CArray<PT_3D,PT_3D> m_arrNewPts;
};


//近似管道压缩算法，应该更快速一些，同时增加了一个角度的处理，希望能够在转弯半径很小的地方，
//保留更多的点，避免打折
//该算法被淘汰
class EXPORT_EDITBASE CStreamCompress3_PT_3DEX
{
public:
	CStreamCompress3_PT_3DEX();
	virtual ~CStreamCompress3_PT_3DEX();
	
	void BeginCompress(double limit=-1);
	void EndCompress();
	int  AddStreamPt(PT_3D pt);
	BOOL GetLastCompPt(PT_3D *&pts, int &npt);
	BOOL GetCurCompPt(PT_3D &pt);
	
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}
	
public:
	double m_lfAngLimit;

private:
	double m_lfLimit;
	PT_3D m_ptCurComp;
	PT_3D m_ptLastComp;
	int m_nCurPt;
	
	double m_lfMinX, m_lfMinY, m_lfMaxX, m_lfMaxY;

	CArray<PT_3D,PT_3D> m_arrOldPts;
	CArray<PT_3D,PT_3D> m_arrNewPts;
};
// 
// 
//包装了 SP 的压缩算法
class EXPORT_EDITBASE CStreamCompress4_PT_3DEX
{
public:
	CStreamCompress4_PT_3DEX();
	virtual ~CStreamCompress4_PT_3DEX();
	
	void BeginCompress(double limit=-1);
	void EndCompress();
	int  AddStreamPt(PT_3D pt);
	BOOL GetPts(PT_3DEX *&pts, int &npt);
	BOOL GetCurCompPt(PT_3D &pt);
	
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}
	
private:
	double m_lfLimit;
	
	CArray<MyNameSpaceName::PT_3DEX,MyNameSpaceName::PT_3DEX> m_arrPts;
	CStreamCompress<MyNameSpaceName::PT_3DEX> m_compress;
};

// 在黑龙江那边进一步改进的压缩算法

class EXPORT_EDITBASE CStreamCompress5_PT_3DEX
{
public:
	CStreamCompress5_PT_3DEX();
	virtual ~CStreamCompress5_PT_3DEX();
	
	void BeginCompress(double limit=-1);
	void EndCompress();
	int  AddStreamPt(PT_3D pt);
	BOOL GetPts(PT_3DEX *&pts, int &npt);
	BOOL GetCurCompPt(PT_3D &pt);
	
	inline void SetLimit(double limit){ m_lfLimit = limit;	}
	inline double GetLimit(){ return m_lfLimit;	}

protected:
	void Compress();

public:
	double m_lfScale, m_lfMaxLen;
		
private:
	double m_lfLimit;
	int m_nCurPt;
		
	CArray<PT_3DEX,PT_3DEX> m_arrOldPts;
	CArray<PT_3DEX,PT_3DEX> m_arrNewPts;
};


MyNameSpaceEnd

#endif // !defined(AFX_STREAMCOMPRESS_H__4C6A948A_007A_4BE8_B95B_C2DAF19B9DEE__INCLUDED_)
