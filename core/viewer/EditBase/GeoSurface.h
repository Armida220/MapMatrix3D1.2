// EBGeoSurface.h: interface for the CGeoSurface class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBGEOSURFACE_H__BE82D668_CF02_4BD3_BD9C_731FC329AFE8__INCLUDED_)
#define AFX_EBGEOSURFACE_H__BE82D668_CF02_4BD3_BD9C_731FC329AFE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Linearizer.h"
#include "GeoCurve.h"

MyNameSpaceBegin

#define FIELDNAME_GEOSURFACE_CELLANGLE		_T("CellAngle")
#define FIELDNAME_GEOSURFACE_CELLSCALE		_T("CellScale")
#define FIELDNAME_GEOSURFACE_INTVSCALE		_T("IntvScale")
#define FIELDNAME_GEOSURFACE_XSTARTOFF		_T("XStartOff")
#define FIELDNAME_GEOSURFACE_YSTARTOFF		_T("YStartOff")
#define FIELDNAME_GEOSURFACE_LINEWIDTH		_T("LineWidth")

class EXPORT_EDITBASE CGeoSurface : public CGeoCurveBase  
{
	DECLARE_DYNCREATE(CGeoSurface)
public:
	CGeoSurface();
	virtual ~CGeoSurface();

	virtual int GetClassType()const;
	
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	
	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);
	
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);
	virtual CGeometry *Linearize();
	virtual int bPtIn(const PT_3D *pt);
	virtual BOOL GetCenter(PT_3D *ipt,PT_3D* opt);

	virtual void DrawForSearch(GrBuffer *pBuf, BOOL bFilled);

protected:
public:
	// ͼԪ����
	float      m_fCellScale;
	// ͼԪ�Ƕ�
	float      m_fCellAngle;
	// �������
	float      m_fIntvScale;
	// ˮƽ���ƫ��
	float      m_fXStartOff;
	// ��ֱ���ƫ��
	float      m_fYStartOff;
};


class EXPORT_EDITBASE CGeoMultiSurface : public CGeoSurface
{
	DECLARE_DYNCREATE(CGeoMultiSurface)
public:
	CGeoMultiSurface();
	virtual ~CGeoMultiSurface();
	virtual int GetClassType()const;
	virtual CGeometry* Clone()const;
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	virtual BOOL SetDataPoint(int i,PT_3DEX pt);
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);
	void AddSurface(CArray<PT_3DEX,PT_3DEX>& arr);
	int  GetSurfaceNum()const;
	void GetSurface(int idx, CArray<PT_3DEX,PT_3DEX>& arr)const;
	void CopyFromSurface(const CGeometry *pObj);
	void CopyToSurface(CGeometry *pObj);
	virtual BOOL IsClosed()const;

	//���ñպϵ�����꣬��� bIsClosedPt ΪTRUE����ʾ�õ��Ǳպϵ㣬�޸�ʱ�����޸�������
	void SetClosedPt(int index, PT_3D pt, BOOL bIsClosedPt);

	//����
	virtual double GetLength()const;
	
	//���
	virtual double GetArea()const;

	//���Ƿ����ڲ�
	virtual int bPtIn(const PT_3D *pt);

	virtual BOOL GetCenter(PT_3D *ipt,PT_3D* opt);

	//�淶�����������˳ʱ�룬�ڱ�����ʱ��
	void NormalizeDirection();

protected:
	
public:
};



MyNameSpaceEnd

#endif // !defined(AFX_EBGEOSURFACE_H__BE82D668_CF02_4BD3_BD9C_731FC329AFE8__INCLUDED_)
