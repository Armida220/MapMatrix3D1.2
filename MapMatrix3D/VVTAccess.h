// VVTAccess.h: interface for the CVVTAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VVTACCESS_H__041D7BFB_F594_4215_A590_FF11937C79BF__INCLUDED_)
#define AFX_VVTACCESS_H__041D7BFB_F594_4215_A590_FF11937C79BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"

class CDlgDataSource;
class CVVTAccess : public CAccessRead  
{
public:
	CVVTAccess();
	virtual ~CVVTAccess();
	
	virtual BOOL OpenRead(LPCTSTR fileName);
	virtual BOOL CloseRead();
	
	//��ȡ����Դ��Ϣ
	virtual BOOL ReadDataSourceInfo(CDlgDataSource *pDS);
	
	//��ȡ��
	virtual CFtrLayer *GetFirstFtrLayer(long &idx);
	virtual CFtrLayer *GetNextFtrLayer(long &idx);
	
	//��ȡ����
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual int GetCurFtrLayID();
	virtual int GetCurFtrGrpID();
protected:
	FILE *m_fp;
	CDlgDataSource *m_pDS;
	//int m_nOpenMode;
	int m_nCurFtrLayID;
	int m_nCurFtrGrpID;

};

#endif // !defined(AFX_VVTACCESS_H__041D7BFB_F594_4215_A590_FF11937C79BF__INCLUDED_)
