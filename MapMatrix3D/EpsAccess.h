// EpsAccess.h: interface for the CEpsAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EPSACCESS_H__BFCFF767_1163_4FA3_9FF5_DF5387A39B4E__INCLUDED_)
#define AFX_EPSACCESS_H__BFCFF767_1163_4FA3_9FF5_DF5387A39B4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef _WIN64
#include <afxDao.h>
#include "Access.h"
#include "ListFile.h "
#include "DlgDataSource.h"

class CEpsAccess;
class LCodeListManager
{
public:
	LCodeListManager();
	~LCodeListManager();
	BOOL LoadFile(LPCTSTR fileName);
	void Clear();
	BOOL SetDataSourse(CEpsAccess *pAccess, CDlgDataSource *pDS);
//	BOOL SetImportDataSourse( CDlgDataSource *pDS);
	long LocLayNameToExternCode(LPCTSTR name);
	CString ExternCodeToLocLayName(long code);
	int GetParaOutMode(LPCTSTR name);
private:
	CDaoDatabase *m_pExternDaoDb;
	CDaoRecordset *m_pDaoSmallLayerRs;
	CDaoRecordset *m_pDaoNotesRs;
	CMultiListFile m_listFile;
	BOOL m_bHasListFile;
	
//	CString m_tableName;
	CDlgDataSource *m_pLocalDb;
};

//class CScheme;
class CEpsAccess : public CAccessModify  
{
	friend LCodeListManager;
public:
	CEpsAccess();
	virtual ~CEpsAccess();

//	void AddDataField();//暂时接口

	void UpdateLayerName(int nFtrType = CLS_GEOPOINT);
	virtual BOOL  Compress();	
	BOOL InitImport();
	BOOL InitExport();
	void  Clear();	
	virtual BOOL Attach(LPCTSTR fileName);
	virtual BOOL Close();
//	void SetScheme(CScheme * pSheme);
	BOOL CreateFile(LPCTSTR fileName, LPCTSTR templateFile = NULL);
	BOOL IsValid();

	void ClearAttrTables();

	virtual BOOL OpenRead(LPCTSTR filename);
	void SetTemplatePath(LPCTSTR templateFile) {  m_strTemplatePath = templateFile; }
	virtual BOOL OpenWrite(LPCTSTR filename);
	virtual BOOL CloseRead();
	virtual BOOL CloseWrite();
	
	void SetDataSource(CDlgDataSource *pDS);
	//读取数据源信息
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS);
	
	void OpenBigLayerTable(CDaoRecordset *pDaoRs = NULL);
	void CloseBigLayerTable(CDaoRecordset *pDaoRs = NULL);
	void OpenSmallLayerTable(CDaoRecordset *pDaoRs = NULL);
	void CloseSmallLayerTable(CDaoRecordset *pDaoRs = NULL);
	void OpenSymbolsTable(CDaoRecordset *pDaoRs = NULL);
	void CloseSymbolsTable(CDaoRecordset *pDaoRs = NULL);
	void OpenNotesTmpTable(CDaoRecordset *pDaoRs = NULL);
	void CloseNotesTmpTable(CDaoRecordset *pDaoRs = NULL);

	void OpenTable(LPCTSTR tableName, CDaoRecordset *pDaoRs = NULL);
	void CloseTable(CDaoRecordset *pDaoRs = NULL);

	//遍历取得
	long GetFirstFtrLayerCode();
	long GetNextFtrLayerCode(); 
	//读取层
	virtual CFtrLayer *GetFirstFtrLayer(long &idx);
	virtual CFtrLayer *GetNextFtrLayer(long &idx);
	virtual int GetFtrLayerCount();
	
	//读取地物
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual int GetFeatureCount();
	virtual int GetCurFtrLayID();
	virtual int GetCurFtrGrpID();


	//更新数据源信息
	virtual void WriteDataSourceInfo(CDataSourceEx *pDS);
	
	//更新层
	virtual BOOL SaveFtrLayer(CFtrLayer *pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer *pLayer);
	
	//更新地物
	virtual BOOL SetCurFtrLayID(int id);
	virtual BOOL SetCurFtrGrpID(int id);
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL);
	virtual BOOL DelFeature(CFeature *pFt);
	virtual BOOL DelXAttributes(CFeature *pFtr);
	virtual BOOL RestoreXAttributes(CFeature *pFtr);
	//更新扩展属性信息
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0);
	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab){return TRUE;}

	virtual void BatchUpdateBegin();
	virtual void BatchUpdateEnd();

protected:
	static BOOL MatchDataType(const CDaoFieldInfo *info, const CVariantEx *pVar, COleVariant *pOleVar);
	BOOL TryUseRefTemplateDatabase(LPCTSTR dbName, LPCTSTR tableName);
	BOOL OpenRefTemplateDatabase();
	BOOL CloseRefTemplateDatabase();

// 	void CreateWorkspaceTable();
// 	void CreateVectorLayerTable();
// 	void CreatePointTable();
// 	void CreateLineTable();
// 	void CreateSurfaceTable();
// 	void CreateTextTable();
// 	void CreateLayerItems(CScheme *ps);
// 	void CreateXAttributeTables(CScheme *ps);
// 	void CreateSymbolsNameTable();

	CFeature *GetFirstPoint(CValueTable *exAttriTab = NULL);
	CFeature *GetNextPoint(CValueTable *exAttriTab = NULL);
	CFeature *ReadPoint(CValueTable *exAttriTab = NULL);
	void SavePoint(CFeature *pFtr, CValueTable *exAttriTab = NULL);

	CFeature *GetFirstLine(CValueTable *exAttriTab = NULL);
	CFeature *GetNextLine(CValueTable *exAttriTab = NULL);
	CFeature *ReadLine(CValueTable *exAttriTab = NULL);
	void SaveLine(CFeature *pFtr, CValueTable *exAttriTab = NULL);

	CFeature *GetFirstSurface(CValueTable *exAttriTab = NULL);
	CFeature *GetNextSurface(CValueTable *exAttriTab = NULL);
	CFeature *ReadSurface(CValueTable *exAttriTab = NULL);
	void SaveSurface(CFeature *pFtr, CValueTable *exAttriTab = NULL);

	CFeature *GetFirstText(CValueTable *exAttriTab = NULL);
	CFeature *GetNextText(CValueTable *exAttriTab = NULL);
	CFeature *ReadText(CValueTable *exAttriTab = NULL);
	void SaveText(CFeature *pFtr, CValueTable *exAttriTab = NULL);
public:
// 	void SetCodelistMager(LCodeListManager *pCodeList)
// 	{
// 		m_pCodeList = pCodeList ;
// 	}
	BOOL IsUseRefDb()
	{
		return m_bUseRefTmpDb;
	}
	CDaoDatabase *GetDaoDataSource()
	{
		return m_pDaoDb;
	}
	CDaoDatabase *GetDaoRefDataSource()
	{
		return m_pDaoRefDb;
	}

	long GetCurCode()
	{
		return m_lTmpCode;
	}
	int GetCurType()
	{
		return m_nTmpType;
	}
	
protected:
	CFtrLayer *ReadFtrLayer();
	
protected:

//	CDaoWorkspace	*m_pDaoWs; //使用缺省的
	CDaoDatabase	*m_pDaoDb;
	CDaoDatabase    *m_pDaoRefDb;
	
//	CDaoTableDef	*m_pDaoTd;
//	CDaoQueryDef	*m_pDaoQd;
	BOOL m_bValid;
	
	//为了获得FtrLayer而暂存的层数据查询对象
	CDaoRecordset	*m_pDaoLayerRs;
	
	//为了获得Feature而暂存的特征数据查询对象
	CDaoRecordset	*m_pDaoFtrRs;

	//临时记录记录查询对象
	CDaoRecordset  *m_pDaoTmpRs;

	int m_nCurLayID;

	BOOL m_bUseRefTmpDb;

//	LCodeListManager *m_pCodeList;
//	CScheme *m_pScheme;

public:
	CDlgDataSource *m_pDataSource;
	long m_lObjID;
	long m_lScale;
	long m_lTmpCode;
	int  m_nTmpType;
	CString m_strLayerName;
	CString m_strByname;
	void CreateDefaultLayer();
public:
	enum ParaOutMode
	{
		modeSeparate = 0,
		modeSurface = 1,
		modeSingle  = 2,
		modeClock   =3
	};
	CString m_strSmallLayer;
	CString m_strBigLayer;
	CString m_strSymbolsTable;
	CString m_strNotesTable;
	CString m_strTemplatePath;
	int m_nParaOutMode;
	CString m_strSchemeLayerName;
};
#endif
#endif // !defined(AFX_EPSACCESS_H__BFCFF767_1163_4FA3_9FF5_DF5387A39B4E__INCLUDED_)
