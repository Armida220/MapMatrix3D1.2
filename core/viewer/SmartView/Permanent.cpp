// Tree8Search.cpp: implementation of the CTree8Search class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Permanent.h"
#include "Resource.h"

#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MyNameSpaceBegin

IMPLEMENT_DYNCREATE(CPermanent, CObject)

CArray<CLASSREG,CLASSREG> CPermanent::m_arrPerRegs;

int CPermanent::GetClassType()const
{
	return CLS_PERMANENT;
}


BOOL CPermanent::WriteTo(CValueTable& tab)const
{
	CVariantEx var;
	var = (_variant_t)(long)GetClassType();
	tab.AddValue(FIELDNAME_CLSTYPE,&var,IDS_FIELDNAME_CLSTYPE);
	return TRUE;
}


BOOL CPermanent::ReadFrom(CValueTable& tab ,int idx)
{
// 	const CVariantEx *var;
// 	if( tab.GetValue(idx,FIELDNAME_CLSTYPE,var) )
// 	{
// 		m_bClosed = (bool)(_variant_t)*var;
// 	}
	return TRUE;
}


BOOL CPermanent::RegisterPermanentObject(int nClsType, LPPROC_PERCREATE lpProc, LPCTSTR name)
{
	if( !lpProc )return FALSE;
	
	CLASSREG item;

	CStringA name_a = ConvertTCharToChar(name);
	
	item.nClsType = nClsType;
	item.lpProc = lpProc;
	if(name)strcpy(item.name, name_a);
	else memset(item.name,0,128);
	
	m_arrPerRegs.Add(item);
	
	return TRUE;
}


CPermanent *CPermanent::CreatePermanentObject(int nClsType)
{
	CLASSREG item;
	for( int i=m_arrPerRegs.GetSize()-1; i>=0; i--)
	{
		item = m_arrPerRegs[i];
		if( item.nClsType==nClsType )
		{
			return (*item.lpProc)();
		}
	}
	return NULL;
}

CString CPermanent::GetPermanentName(int nClsType)
{
	CLASSREG item;
	for( int i=m_arrPerRegs.GetSize()-1; i>=0; i--)
	{
		item = m_arrPerRegs[i];
		if( item.nClsType==nClsType )
		{
			return CString(item.name);
		}
	}
	return CString();
}

MyNameSpaceEnd
