// Copyright (C) 2004 - 2005 Gibuilder Group

#include "SmartViewBaseType.h"
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_ENVELOPE_29DA30CE064A_INCLUDED
#define _INC_ENVELOPE_29DA30CE064A_INCLUDED


MyNameSpaceBegin

class CCoordSys;


class EXPORT_SMARTVIEW Envelope 
{
public:
	Envelope();
	Envelope(double xl, double xh, double yl, double yh, double zl=0, double zh=0);

	void CreateMaxEnvelope();
	void CreateFromPts(const PT_3D *pts, int ptsnum, int sizeofpt=sizeof(PT_3D), int dims=2);
	void CreateFromPts(const PT_2D *pts, int ptsnum, int sizeofpt=sizeof(PT_2D));
	void CreateFromPtAndRadius(PT_3D pt, double r, int dims=2);

	//与另外一个矩形框求交
	void Intersect(const Envelope* pEnlp, int dims=2);

	//与另外一个矩形框合并
	void Union(const Envelope *pEnlp, int dims=2);

	//旋转并生成新的矩形外包
	void Transform(const double *m);

	void Transform2d(const double *m);

	void TransformGrdToClt(CCoordSys *pCS, int flag);

	void TransformCltToGrd(CCoordSys *pCS);

	//判断一个点是否在矩形框内
	BOOL bPtIn(const PT_3D *pt, int dims=2)const;

	//判断是否包含另外一个对象pEnlp
	BOOL bEnvelopeIn(const Envelope *pEnlp, int dims=2) const;

	//判断是否与一条线段相交
	BOOL bIntersect(const PT_3D *pt1, const PT_3D *pt2, int dims=2)const;

	//判断是否与另一个矩形框相交
	BOOL bIntersect(const Envelope *pEnlp, int dims=2)const;

	//判断是否与一个多边形相交
	BOOL bIntersect(const PT_3D *pts, int n, int dims=2)const;

	//判断是否为空
	inline BOOL IsEmpty(int dims=2) const {
		if( dims>=3 )return ( (m_xl>m_xh)||(m_yl>m_yh)||(m_zl>m_zh) ); 
		else return ( (m_xl>m_xh)||(m_yl>m_yh) ); 
	}
	inline double Width() const {
		return (m_xh>m_xl?(m_xh-m_xl):0);
	}
	inline double Height() const {
		return (m_yh>m_yl?(m_yh-m_yl):0);
	}
	inline double Depth() const {
		return (m_zh>m_zl?(m_zh-m_zl):0);
	}
	inline double CenterX() const {
		return ((m_xh+m_xl)*0.5);
	}
	inline double CenterY() const {
		return ((m_yh+m_yl)*0.5);
	}
	inline double CenterZ() const {
		return ((m_zh+m_zl)*0.5);
	}

	inline void Offset(double dx, double dy, double dz){
		m_xl += dx; m_xh += dx;
		m_yl += dy; m_yh += dy;
		m_zl += dz; m_zh += dz;
	}

	inline void Inflate(double dx, double dy, double dz){
		m_xl -= dx; m_xh += dx;
		m_yl -= dy; m_yh += dy;
		m_zl -= dz; m_zh += dz;
	}

public:
	double m_xl;
	double m_xh;

	double m_yl;
	double m_yh;

	double m_zl;
	double m_zh;

};

template<class T>
Envelope CreateEnvelopeFromPts(const T* pts, int ptsnum, int dims=2)
{
	Envelope e;
	for( int i=0; i<ptsnum; i++)
	{
		if( i==0 )
		{
			e.m_xl = pts->x;
			e.m_xh = pts->x;
			
			e.m_yl = pts->y;
			e.m_yh = pts->y;
		}
		else
		{
			e.m_xl = (e.m_xl<pts->x?e.m_xl:pts->x);
			e.m_xh = (e.m_xh>pts->x?e.m_xh:pts->x);
			
			e.m_yl = (e.m_yl<pts->y?e.m_yl:pts->y);
			e.m_yh = (e.m_yh>pts->y?e.m_yh:pts->y);
		}
		
		if( dims==3 )
		{
			if( i==0 )
			{
				e.m_zl = pts->z;
				e.m_zh = pts->z;
			}
			else
			{
				e.m_zl = (e.m_zl<pts->z?e.m_zl:pts->z);
				e.m_zh = (e.m_zh>pts->z?e.m_zh:pts->z);
			}
		}
		
		pts++;
	}
	return e;
}

MyNameSpaceEnd

#endif /* _INC_ENVELOPE_40DA40CA029A_INCLUDED */
