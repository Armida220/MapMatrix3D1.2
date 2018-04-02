// VarPoint.h: interface for the CVarPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VARPOINT_H__D12FDC94_C763_4528_9FC5_4F1D1705F8DC__INCLUDED_)
#define AFX_VARPOINT_H__D12FDC94_C763_4528_9FC5_4F1D1705F8DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "GrBuffer.h"
#include "GrBuffer2d.h"

MyNameSpaceBegin

//通用的点，仿照 _variant_t
class EXPORT_SMARTVIEW CVarPoint  
{
public:
	struct PointData
	{
		double m_c[4];
	};
	enum
	{
		typeInvalid = 0,
		type2D = 1,
		type3D = 2,
		type4D = 3
	};
	CVarPoint();
	CVarPoint(const CVarPoint& varSrc);
	CVarPoint(double x, double y);
	CVarPoint(double x, double y, double z);
	CVarPoint(double xl, double yl, double xr, double yr);

	CVarPoint( const PT_2D varSrc );
	CVarPoint( const PT_3D varSrc );
	CVarPoint( const PT_4D varSrc );
	CVarPoint( const GR_PT_2D varSrc );

	virtual ~CVarPoint();
	operator PT_2D() const;
	operator PT_3D() const;
	operator PT_4D() const;
	operator GR_PT_2D() const;

	void CopyValue(const CVarPoint& varSrc);

	int m_nType;
	PointData m_pt;
};

class EXPORT_SMARTVIEW CVarPointArray
{
public:
	enum
	{
		typeInvalid = 0,
		typePT_2D = 1,
		typePT_3D = 2,
		typePT_4D = 3,
		typeGR_PT_2D = 4,
		typeVarPoint = 5
	};
	CVarPointArray();
	CVarPointArray(PT_2D *pts, int npt);
	CVarPointArray(PT_3D *pts, int npt);
	CVarPointArray(PT_4D *pts, int npt);
	CVarPointArray(GR_PT_2D *pts, int npt);
	CVarPointArray(CVarPoint *pts, int npt);
	virtual ~CVarPointArray();

	void Copy(const CVarPointArray& arr);
	void Copy(const PT_2D *pts, int npt);
	void Copy(const PT_3D *pts, int npt);
	void Copy(const PT_4D *pts, int npt);
	void Copy(const GR_PT_2D *pts, int npt);
	void Copy(CVarPoint *pts, int npt);

	CVarPoint GetAt( int nIndex ) const;
	void SetAt( int nIndex, const CVarPoint& pt );
	int GetSize(){
		return m_npt;
	}

	int m_nType;
	int m_npt;

private:
	void Clear();
	BOOL AllocMem(int npt);

private:
	CVarPoint::PointData *m_pts;
};


MyNameSpaceEnd

#endif // !defined(AFX_VARPOINT_H__D12FDC94_C763_4528_9FC5_4F1D1705F8DC__INCLUDED_)
