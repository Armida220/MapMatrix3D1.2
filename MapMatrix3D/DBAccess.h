// DBAccess.h: interface for the CDBAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBACCESS_H__6F2508AC_86E9_438B_9C54_6182187289BD__INCLUDED_)
#define AFX_DBACCESS_H__6F2508AC_86E9_438B_9C54_6182187289BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"
#include "DBVisit.h"
#include "DlgDataSource.h"


class CDBAccess : public CAccessModify
{
public:
	CDBAccess();
	virtual ~CDBAccess();
	
	virtual BOOL Attach(LPCTSTR fileName);
	virtual BOOL Close();

	virtual int GetFtrLayerCount();
	virtual int GetFeatureCount();
	
	virtual BOOL OpenRead(LPCTSTR filename);
	virtual BOOL OpenWrite(LPCTSTR filename);
	virtual BOOL CloseRead();
	virtual BOOL CloseWrite();
	void SetRefDataSource(CDataSourceEx *pDS);
	//读取数据源信息
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS);
	
	CFtrLayer *GetFtrLayerByID(long id);
	//读取层
	virtual CFtrLayer *GetFirstFtrLayer(long &idx);
	virtual CFtrLayer *GetNextFtrLayer(long &idx);
	
	//读取地物
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual int GetCurFtrLayID();
	virtual int GetCurFtrGrpID();
	
	//读取扩展属性信息
	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab);
	//更新数据源信息
	virtual void WriteDataSourceInfo(CDataSourceEx *pDS);
	
	virtual BOOL DelXAttributes(CFeature *pFtr);
	virtual BOOL RestoreXAttributes(CFeature *pFtr);

	//更新层
	virtual BOOL SaveFtrLayer(CFtrLayer *pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer *pLayer);
	
	//更新地物
	virtual BOOL SetCurFtrLayID(int id);
	virtual BOOL SetCurFtrGrpID(int id);
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL);
	virtual BOOL DelFeature(CFeature *pFt);

	//更新扩展属性信息
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0);
protected:
	CFtrLayer* FtrLayerFromDpObject(CDpUserDefinedGroupMgr *pMgr, CDpUserDefinedGroupObject *pObj);
	CDpUserDefinedGroupObject* FtrLayerToDpObject(CDpUserDefinedGroupMgr *pMgr, CFtrLayer *pLayer);
	CFeature* FeatureFromDpFeature(CDpFeatureLayer *pLayer, CDpFeature *pDpFtr);
	CDpFeature* FeatureToDpFeature(CDpFeatureLayer *pLayer, CFeature *pFtr);

	CDpFeatureLayer* FindOwnerDpLayer(CFeature *pFtr);
	
protected:
	CDpDatabase		*m_pDpDatabase; // 需要释放	
	CDpWorkspace	*m_pDpWks; // 需要释放

	CPtrArray		m_listDpLayers;
	int				m_nCurIndexLayer;
	int				m_nCurLayer, m_nCurFtr;
	CDpFeatureClass *m_pFtrCls;
	int				m_nCurLayId, m_nCurGrpId;
	int				m_nLayIdIdx, m_nGrpIdIdx;

	CDpUserDefinedGroupMgr *m_pLayerMgr;
	CDpUserDefinedGroupMgr *m_pGroupMgr;
	CDpRecordset	*m_pLayerRecords;
	CDpRecordset	*m_pGroupRecords;

	CDataSourceEx *m_pDlgDataSource;
public:
	CString m_strMapName;
};


#endif // !defined(AFX_DBACCESS_H__6F2508AC_86E9_438B_9C54_6182187289BD__INCLUDED_)
