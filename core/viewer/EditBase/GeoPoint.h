// EBGeoPoint.h: interface for the CGeoPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBGEOPOINT_H__BB0D19FF_9157_416F_82DB_2B1A59F361AE__INCLUDED_)
#define AFX_EBGEOPOINT_H__BB0D19FF_9157_416F_82DB_2B1A59F361AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

MyNameSpaceBegin

#define FIELDNAME_GEOPOINT_ANGLE		_T("Angle")
#define FIELDNAME_GEOPOINT_KX			_T("Kx")
#define FIELDNAME_GEOPOINT_KY			_T("Ky")
#define FIELDNAME_GEOPOINT_WIDTH		_T("Width")
#define FIELDNAME_GEOPOINT_COVERTYPE	_T("CoverType")
#define FIELDNAME_GEOPOINT_EXTENDDIS	_T("ExtendDis")

class EXPORT_EDITBASE CGeoPoint : public CGeometry  
{
	DECLARE_DYNCREATE(CGeoPoint)
public:
	CGeoPoint();
	virtual ~CGeoPoint();

	virtual Envelope GetEnvelope();
	virtual int GetClassType()const;
	virtual int GetDataPointSum()const;
	
	virtual BOOL SetDataPoint(int i,PT_3DEX pt);
	virtual PT_3DEX GetDataPoint(int i)const;
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	virtual void GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const;
	
	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);
	
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);

	void EnableFlated(BOOL bFlat);
	BOOL IsFlatedPoint();

	void EnableGrdWid(BOOL bGrdWid);
	BOOL IsGrdWid();
	
	PT_3DEX GetCtrlPoint2(int num)const;
	BOOL SetCtrlPoint2(int num, PT_3D pt, BOOL bSynchXY);

	double GetDirection()const;
	void SetDirection(double angle);
	
protected:
	PT_3DEX m_pt;
public:
	// 横向比例系数
	float   m_fKx;
	// 纵向比例系数
	float   m_fKy;
	// 线宽
	float   m_fWidth;
	// 压盖类型: 0, 无压盖 1,矩形压盖 2,圆形压盖
	int     m_nCoverType;
	// 压盖外扩距离
	float   m_fExtendDis;

	// 角度单位:度
	double m_lfAngle;
};



class EXPORT_EDITBASE CGeoSurfacePoint : public CGeoPoint  
{
	DECLARE_DYNCREATE(CGeoSurfacePoint)
public:
	CGeoSurfacePoint();
	virtual ~CGeoSurfacePoint();
	virtual int GetClassType()const;
	virtual CGeometry* Clone()const;
	
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
};

MyNameSpaceEnd

#endif // !defined(AFX_EBGEOPOINT_H__BB0D19FF_9157_416F_82DB_2B1A59F361AE__INCLUDED_)
