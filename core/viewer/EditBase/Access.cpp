// Access.cpp: implementation of the CAccess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Access.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

//test
CPermanent*  CAccessRead::CreateObject(CValueTable& tab)
{
	const CVariantEx *var;
	if( !tab.GetValue(0,FIELDNAME_CLSTYPE,var) )
		return NULL;

	int nObjType = (long)(_variant_t)*var;
	
	CPermanent *pObj = CPermanent::CreatePermanentObject(nObjType);
	if( !pObj )return NULL;
	
	pObj->ReadFrom(tab);
	return pObj;
}


BOOL CAccessRead::ReadOptAttributes(CFeature *pFtr, CValueTable& tab)
{
	return FALSE;
}

BOOL CAccessRead::GetOptAttributeDef(int nGeoCls, CStringArray& fields, CStringArray& field_types)
{
	return FALSE;
}

BOOL CAccessWrite::CreateOptAttributes(int nGeoCls, CStringArray& fields, CStringArray& field_types)
{
	return FALSE;
} 

BOOL CAccessWrite::DelOptAttributes(CFeature *pFtr)
{
	return FALSE;
}


BOOL CAccessWrite::RestoreOptAttributes(CFeature *pFtr)
{
	return FALSE;
}

BOOL CAccessWrite::SaveOptAttributes(CFeature *pFtr, CValueTable& tab,int idx)
{
	return FALSE;
}


MyNameSpaceEnd