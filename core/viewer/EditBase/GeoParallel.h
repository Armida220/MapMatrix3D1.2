// GeoParallel.h: interface for the CGeoParallel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOPARALLEL_H__F201BC34_B5FB_4AE5_86EA_A802BDFB82A8__INCLUDED_)
#define AFX_GEOPARALLEL_H__F201BC34_B5FB_4AE5_86EA_A802BDFB82A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoCurve.h"
MyNameSpaceBegin

class EXPORT_EDITBASE CGeoParallel : public CGeoCurve  
{
	DECLARE_DYNCREATE(CGeoParallel)
public:
	BOOL Separate(CGeometry *&pGeo1, CGeometry *&pGeo2);
	BOOL SeparateNoReverse(CGeometry *&pGeo1, CGeometry *&pGeo2);

	CGeoParallel();
	virtual ~CGeoParallel();
	virtual BOOL GetCenter(PT_3D *ipt,PT_3D* opt);

	virtual Envelope GetEnvelope();
	void GetParallelShape(CArray<PT_3DEX,PT_3DEX>& pts, double wid=0)const;
	CGeoCurve *GetCenterlineCurve();
	void GetAllShape(CArray<PT_3DEX,PT_3DEX>& pts);
	
	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);
	virtual int GetClassType()const;
	
	virtual PT_3DEX GetCtrlPoint(int i)const;
	virtual int GetCtrlPointSum()const;
	virtual BOOL SetCtrlPoint(int i,PT_3D pt);
	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab ,int idx = 0 );

	double GetWidth();
	void SetWidth(double wid);

	double GetDHeight() { return m_lfDHeight; }
	//长度
	virtual double GetLength()const;
	
	//面积
	virtual double GetArea()const;
	
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
	
protected:

	void GetSamePointPos(CArray<int, int> &pos) const;

	
private:
	double m_lfWidth;
	// 辅助线与基线高程差
	double m_lfDHeight;
};
MyNameSpaceEnd
#endif // !defined(AFX_GEOPARALLEL_H__F201BC34_B5FB_4AE5_86EA_A802BDFB82A8__INCLUDED_)
