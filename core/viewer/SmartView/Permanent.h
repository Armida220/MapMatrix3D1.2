// Permanent.h: interface for the CFeature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_PERMANENT_HEAD_INCLUDE_)
#define _PERMANENT_HEAD_INCLUDE_

#include "ValueTable.h"
#include "PermanentType.h"


#define FIELDNAME_CLSTYPE		_T("ClassType")

MyNameSpaceBegin



class CPermanent;
typedef CPermanent* (*LPPROC_PERCREATE)();

struct CLASSREG
{
	int nClsType;
	LPPROC_PERCREATE lpProc;
	char name[128];
};


class EXPORT_SMARTVIEW CPermanent : public CObject
{
	DECLARE_DYNCREATE(CPermanent)
public:
	CPermanent(){}
	virtual ~CPermanent(){}
	
	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);
	
	virtual int  GetClassType()const;
	static BOOL  RegisterPermanentObject(int nClsType, LPPROC_PERCREATE lpProc, LPCTSTR name = NULL);
	static CPermanent *CreatePermanentObject(int nClsType);
	static CString GetPermanentName(int nClsType);
	static CArray<CLASSREG,CLASSREG> m_arrPerRegs;
};

MyNameSpaceEnd

#endif
