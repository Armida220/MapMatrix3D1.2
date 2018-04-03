// GeoDirPoint.h: interface for the CGeoDirPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEODIRPOINT_H__5A9BF753_3C16_401C_A68A_1453BA55C335__INCLUDED_)
#define AFX_GEODIRPOINT_H__5A9BF753_3C16_401C_A68A_1453BA55C335__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GeoPoint.h"

MyNameSpaceBegin



class EXPORT_EDITBASE CGeoDirPoint : public CGeoPoint  
{
	DECLARE_DYNCREATE(CGeoDirPoint)
public:
	CGeoDirPoint();
	virtual ~CGeoDirPoint();
	virtual int GetClassType()const;
	virtual CGeometry* Clone()const;
	virtual BOOL CopyFrom(const CGeometry *pObj);	
	virtual BOOL CreateShape(const PT_3DEX *pts, int npt);
	virtual void Draw(GrBuffer *pBuf, float fDrawScale=1.0);

	virtual Envelope GetEnvelope();
	
	virtual int GetCtrlPointSum()const;
	virtual PT_3DEX GetCtrlPoint(int num)const;
	virtual BOOL SetCtrlPoint(int num, PT_3D pt);
	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab ,int idx = 0 );
	
	static float GetDefaultDirLen();

protected:
	
private:
	// 角度单位:度
	//double m_lfAngle;

};
MyNameSpaceEnd
#endif // !defined(AFX_GEODIRPOINT_H__5A9BF753_3C16_401C_A68A_1453BA55C335__INCLUDED_)
