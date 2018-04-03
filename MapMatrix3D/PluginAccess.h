// PluginAccess.h: interface for the CPluginAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLUGINACCESS_H__88FAB421_5B99_48AD_B089_5C5BF4154401__INCLUDED_)
#define AFX_PLUGINACCESS_H__88FAB421_5B99_48AD_B089_5C5BF4154401__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"
#include "AccessPlugin.h"
#include "Scheme.h"
#include "DlgDataSource.h"

class CPluginAccess : public CAccessModify 
{
public:
	CPluginAccess();
	virtual ~CPluginAccess();

	BOOL   Init(CAccessPlugin* pAccessPlugin,BOOL bUseSqliteEx = TRUE);
	virtual BOOL  Compress();
	virtual BOOL Attach(LPCTSTR fileName);
	virtual BOOL Close();
	
	BOOL CreateFileSys(long Scale,CScheme *ps=NULL);
	BOOL IsValid();
	
	void ClearAttrTables();
	
	virtual BOOL OpenRead(LPCTSTR filename);
	virtual BOOL OpenWrite(LPCTSTR filename);
	virtual BOOL CloseRead();
	virtual BOOL CloseWrite();
	// 	
	//��ȡ����Դ��Ϣ
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS);
	
	//��ȡ��
	virtual CFtrLayer* GetFirstFtrLayer(long &idx);
	virtual CFtrLayer* GetNextFtrLayer(long &idx);
	virtual int GetFtrLayerCount();
	
	//��ȡ����
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual int GetFeatureCount();
	virtual int GetCurFtrLayID();
	virtual int GetCurFtrGrpID();
	
	//��ȡ��չ������Ϣ
	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab);
	
	//��������Դ��Ϣ
	virtual void WriteDataSourceInfo(CDataSourceEx *pDS);
	
	//���²�
	virtual BOOL SaveFtrLayer(CFtrLayer* pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer* pLayer);
	
	//���µ���
	virtual BOOL SetCurFtrLayID(int id);
	virtual BOOL SetCurFtrGrpID(int id);
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL);
	virtual BOOL DelFeature(CFeature *pFt);
	virtual BOOL DelXAttributes(CFeature *pFtr);
	virtual BOOL RestoreXAttributes(CFeature *pFtr);
	//������չ������Ϣ
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0);
	
	virtual void BatchUpdateBegin();
	virtual void BatchUpdateEnd();
protected:
	BOOL			m_bUseSqliteEx;
	CSQLiteAccess *m_pSqliteAccess;
	CAccessPlugin *m_pAccessPlugin;
	int				m_nCurLayID;
};

#endif // !defined(AFX_PLUGINACCESS_H__88FAB421_5B99_48AD_B089_5C5BF4154401__INCLUDED_)
