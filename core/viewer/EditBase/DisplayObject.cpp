// DisplayObject.cpp: implementation of the CDisplayObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DisplayObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CStepDisplay::CStepDisplay()
{
}


CStepDisplay::~CStepDisplay()
{
}

int CStepDisplay::GetObjectType()
{
	return DISPLAYTYPE_NONE;
}



CTransformStepDisplay::CTransformStepDisplay()
{
}


CTransformStepDisplay::~CTransformStepDisplay()
{
}

int CTransformStepDisplay::GetObjectType()
{
	return DISPLAYTYPE_TRANSFORMSTEP;
}


CStepDisplay *CTransformStepDisplay::Clone()
{
	CTransformStepDisplay *pObj = new CTransformStepDisplay;
	if( !pObj )return NULL;

	pObj->arrObjs.Copy(arrObjs);
	memcpy(pObj->m,m,sizeof(m));
	pObj->m_nStep = 0;

	return pObj;
}


GrBuffer *CTransformStepDisplay::GetStepDisplay()
{
	if( m_nStep>=0 && m_nStep<arrObjs.GetSize() )
	{
		CFeature *pFtr = arrObjs.GetAt(m_nStep++);
		if( !pFtr )return NULL;

		CGeometry *pGeo = pFtr->GetGeometry();
		if( !pGeo )return NULL;

		m_buf.DeleteAll();
		pGeo->Draw(&m_buf);

		return &m_buf;
	}

	return NULL;
}
