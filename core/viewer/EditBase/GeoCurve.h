// EBGeoCurve.h: interface for the CGeoCurve class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBGEOCURVE_H__9C2973BC_E6E2_4967_A486_85E18397C01C__INCLUDED_)
#define AFX_EBGEOCURVE_H__9C2973BC_E6E2_4967_A486_85E18397C01C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"
#include "Linearizer.h"

MyNameSpaceBegin

#define FIELDNAME_GEOCURVE_CLOSED		_T("Closed")
#define FIELDNAME_GEOCURVE_WIDTH        _T("Width")
#define FIELDNAME_GEOCURVE_DHEIGHT      _T("DHeight")
#define FIELDNAME_GEOCURVE_LINETYPESCALE	_T("LinetypeScale")
#define FIELDNAME_GEOCURVE_LINEWIDTHSCALE	_T("LinewidthScale")
#define FIELDNAME_GEOCURVE_LINEWIDTH        _T("LineWidth")
#define FIELDNAME_GEOCURVE_LINETYPEXOFF		_T("LinetypeXoff")

class EXPORT_EDITBASE CGeoCurveBase : public CGeometry  
{
	DECLARE_DYNCREATE(CGeoCurveBase)
public:
	CGeoCurveBase();
	virtual ~CGeoCurveBase();
	
	virtual Envelope GetEnvelope();
	virtual int GetClassType()const;
	virtual int GetDataPointSum()const;
	
	virtual BOOL SetDataPoint(int i,PT_3DEX pt);
	virtual PT_3DEX GetDataPoint(int i)const;
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	virtual void GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const;
	
	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);
	
	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab ,int idx = 0 );
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
	
	// special functions for CGeoCurve
	const CShapeLine *GetShape();
	virtual BOOL IsClosed()const;
	virtual BOOL EnableClose(BOOL bClose);
	void EnableFillColor(BOOL bFill, COLORREF clr);
	BOOL IsFillColor()const;
	COLORREF GetFillColor()const;
	DWORD GetTransparency();//获取透明度
	void SetTransparency(DWORD val);

	virtual float GetLineWidth() const{
		return m_fLineWidth;
	}
	virtual void  SetLineWidth(float fLineWidth){
		m_fLineWidth = fLineWidth;
	}
	
	void GetBreak(PT_3D pt1, PT_3D pt2, CGeometry*& pObj1, CGeometry*& pObj2, CGeometry*& pObj3, BOOL bClose=FALSE);

	BOOL GetTangency(PT_3D *ipt0, PT_3D *ipt1,PT_3D* opt);
	virtual BOOL GetCenter(PT_3D *ipt,PT_3D* opt);
	
	//长度
	virtual double GetLength()const;
	
	//面积
	virtual double GetArea()const;
	
	virtual CGeometry *Linearize();
	
	// search a line segment in an object
	virtual BOOL FindNearestBaseLine(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *ret1, PT_3D *ret2, double *mindis);
	
	// search a point in an object
	virtual BOOL FindNearestBasePt(PT_3D sch_pt, Envelope sch_evlp, CCoordSys *pCS, PT_3D *pRet, double *mindis);

	virtual int bPtIn(const PT_3D *pt);
	
protected:
	CShapeLine m_shape;
public:
	Envelope m_evlp;

	float   m_fLineWidth;
	
};

class EXPORT_EDITBASE CGeoCurve : public CGeoCurveBase  
{
	DECLARE_DYNCREATE(CGeoCurve)
public:
	CGeoCurve();
	virtual ~CGeoCurve();

	virtual int GetClassType()const;


	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab ,int idx = 0 );
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);

protected:
public:
	// 线型比例
	float      m_fLinetypeScale;
	// 线宽比例
	float      m_fLinewidthScale;
	//线型沿线方向的偏移
	float	m_fLinetypeXoff;
};

class EXPORT_EDITBASE CGeoDCurve : public CGeoCurve  
{
	DECLARE_DYNCREATE(CGeoDCurve)
public:
	CGeoDCurve();
	virtual ~CGeoDCurve();

	virtual BOOL GetCenter(PT_3D *ipt,PT_3D* opt);

	BOOL Separate(CGeometry *&pGeo1, CGeometry *&pGeo2);

	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);

	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);

	virtual BOOL IsClosed()const;
	virtual BOOL EnableClose(BOOL bClose);
	
	void GetBaseShape(CArray<PT_3DEX,PT_3DEX>& pts)const;
	void GetAssistShape(CArray<PT_3DEX,PT_3DEX>& pts)const;
	BOOL GetOrderShape(CArray<PT_3DEX,PT_3DEX>& pts)const;

	double GetWidth();
	//长度
	virtual double GetLength()const;
	
	//面积
	virtual double GetArea()const;	
	
	int GetClassType()const;

protected:

};

#define FIELDNAME_GEOMULTIPOINT_LNEXCODE		_T("LnExCode")
class EXPORT_EDITBASE CGeoMultiPoint : public CGeometry
{
	DECLARE_DYNCREATE(CGeoMultiPoint)
public:
	CGeoMultiPoint();
	virtual ~CGeoMultiPoint();

	Envelope GetEnvelope();
	virtual int GetClassType()const;
	virtual int GetDataPointSum()const;
	
	virtual BOOL SetDataPoint(int i,PT_3DEX pt);
	virtual PT_3DEX GetDataPoint(int i)const;

	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	virtual void GetShape(CArray<PT_3DEX,PT_3DEX>& pts)const;
	virtual const CShapeLine *GetShape();
	
	int  DeletePart(Envelope e, BOOL bInside);
	BOOL IsInEnvelope(Envelope e);

	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
	
protected:
//	void DoPermanentDataExchange(CPermanentDataExchange *pPDX);
	
protected:
	Envelope m_evlp;
	CArray<PT_3DEX, PT_3DEX> m_PointList;
};

struct EXPORT_EDITBASE SearchableMultiPoint : public SearchableAgentObj
{
	SearchableMultiPoint();
	virtual ~SearchableMultiPoint();
	
	//获得矩形外包
	virtual Envelope GetEnvelope()const;
	
	virtual double GetMinDistance(PT_3D sch_pt, CCoordSys* pCS, PT_3D *pret)const;
	
	//在某个搜索范围内查找，当 s r是SearchNearest类型时，distance 返回距离，
	virtual BOOL FindObj(const SearchRange *sr, double *distance=NULL)const;
	
	CGeoMultiPoint *m_pObj;
};


class EXPORT_EDITBASE CGeoDemPoint : public CGeoMultiPoint
{
	DECLARE_DYNCREATE(CGeoDemPoint)
public:
	double CalcuDemDot();
	CGeoDemPoint();
	virtual ~CGeoDemPoint();
	virtual CGeometry* Clone()const;
	virtual int GetClassType()const;
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
};

MyNameSpaceEnd

#endif // !defined(AFX_EBGEOCURVE_H__9C2973BC_E6E2_4967_A486_85E18397C01C__INCLUDED_)
