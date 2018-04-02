// GeometryBase.cpp: implementation of the CGeometryBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GeometryBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGeometryBase::CGeometryBase()
{

}

CGeometryBase::~CGeometryBase()
{

}

int CGeometryBase::GetDataPointSum()const
{
	return 0;
}

int CGeometryBase::GetCtrlPointSum()const
{
	return 0;
}

PT_3DEX CGeometryBase::GetCtrlPoint(int i)const
{
	return PT_3DEX();
}

Envelope CGeometryBase::GetEnvelope()
{
	return Envelope();
}

long CGeometryBase::GetColor()
{
	return 0;
}


MyNameSpaceEnd