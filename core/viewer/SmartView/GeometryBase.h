// GeometryBase.h: interface for the CGeometryBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GEOMETRYBASE_H__B2B907D4_C0DB_4BE5_9AB9_5CBCB5A9A948__INCLUDED_)
#define AFX_GEOMETRYBASE_H__B2B907D4_C0DB_4BE5_9AB9_5CBCB5A9A948__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Permanent.h"

MyNameSpaceBegin

class EXPORT_SMARTVIEW CGeometryBase : public CPermanent
{
public:
	CGeometryBase();
	virtual ~CGeometryBase();

	virtual int GetDataPointSum()const;	
	virtual void GetShape(CArray<PT_3DEX,PT_3DEX>& arrPts)const=0;

	virtual int GetCtrlPointSum()const;
	virtual PT_3DEX GetCtrlPoint(int i)const;

	virtual Envelope GetEnvelope();

	virtual long GetColor();

};

MyNameSpaceEnd

#endif // !defined(AFX_GEOMETRYBASE_H__B2B907D4_C0DB_4BE5_9AB9_5CBCB5A9A948__INCLUDED_)
