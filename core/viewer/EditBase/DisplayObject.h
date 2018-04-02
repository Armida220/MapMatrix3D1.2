// DisplayObject.h: interface for the CDisplayObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DISPLAYOBJECT_H__3205F822_1B55_41D1_9A48_3AEF96BF648B__INCLUDED_)
#define AFX_DISPLAYOBJECT_H__3205F822_1B55_41D1_9A48_3AEF96BF648B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GrBuffer.h"
#include "Feature.h"

#define DISPLAYTYPE_NONE					0
#define DISPLAYTYPE_TRANSFORMSTEP			1


//Á¢¼´ÏÔÊ¾µÄ
class CStepDisplay
{
public:
	CStepDisplay();
	virtual ~CStepDisplay();
	
	virtual int GetObjectType();
	virtual CStepDisplay *Clone()=0;
	
	virtual GrBuffer *GetStepDisplay()=0;
};


class CTransformStepDisplay : public CStepDisplay
{
public:
	CTransformStepDisplay();
	virtual ~CTransformStepDisplay();
	
	virtual int GetObjectType();
	virtual CStepDisplay *Clone();
	virtual GrBuffer *GetStepDisplay();

	CFtrArray arrObjs;
	double m[16];
private:
	int m_nStep;
	GrBuffer m_buf;
};

#endif // !defined(AFX_DISPLAYOBJECT_H__3205F822_1B55_41D1_9A48_3AEF96BF648B__INCLUDED_)
