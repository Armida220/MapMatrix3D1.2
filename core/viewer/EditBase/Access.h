// Access.h: interface for the CAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACCESS_H__6B8586D5_E33C_475F_9832_0FFDF37FF871__INCLUDED_)
#define AFX_ACCESS_H__6B8586D5_E33C_475F_9832_0FFDF37FF871__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"

MyNameSpaceBegin

BOOL  VariantToTextA(const CVariantEx& v, CStringA &text);

class CFtrLayer;
class CDataSourceEx;
class CScheme;
struct CellDef;
struct ObjectGroup;
struct FtrLayerGroup;

enum AccessType
{
	SQLiteAccess = 0,
	UVSModify = 1
};

class EXPORT_EDITBASE CAccessRead  
{
public:
	CAccessRead(){};
	virtual ~CAccessRead(){};

	virtual BOOL IsFileVersionLatest() { return TRUE; }

	virtual BOOL OpenRead(LPCTSTR fileName)=0;
	virtual BOOL CloseRead()=0;

	//读取数据源信息
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS)=0;

	//读取层
	virtual CFtrLayer *GetFirstFtrLayer(long &idx)=0;
	virtual CFtrLayer *GetNextFtrLayer(long &idx)=0;
	virtual int GetFtrLayerCount() {return 0;};

	//读取地物
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL)=0;
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL)=0;
	virtual int GetFeatureCount() {return 0;};
	virtual int GetCurFtrLayID()=0;
	virtual int GetCurFtrGrpID()=0;

	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab)=0;
	CString GetReadName(){
		return m_strReadName;
	}
	virtual BOOL GetOptAttributeDef(int nGeoCls, CStringArray& fields, CStringArray& field_types);  
	virtual BOOL ReadOptAttributes(CFeature *pFtr, CValueTable& tab);

	// 读取数据源特定信息
	virtual void ReadSpecialData(CDataSourceEx *pDS) {}

	static CPermanent* CreateObject(CValueTable& tab);

protected:
	CString m_strReadName;
};

class EXPORT_EDITBASE CAccessWrite  
{
public:
	CAccessWrite(){};
	virtual ~CAccessWrite(){};

	virtual BOOL OpenWrite(LPCTSTR fileName)=0;
	virtual BOOL CloseWrite()=0;

	//更新数据源信息
	virtual void WriteDataSourceInfo(CDataSourceEx *pDS)=0;

	//更新层
	virtual BOOL SaveFtrLayer(CFtrLayer *pLayer)=0;

	//更新地物
	virtual BOOL SetCurFtrLayID(int id)=0;
	virtual BOOL SetCurFtrGrpID(int id)=0;
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL)=0;

	virtual void BatchUpdateBegin(){};
	virtual void BatchUpdateEnd(){};

	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0)=0;
	CString GetWriteName(){
		return m_strWriteName;
	}
	virtual BOOL DelXAttributes(CFeature *pFtr)=0;
	virtual BOOL RestoreXAttributes(CFeature *pFtr)=0;

	virtual BOOL CreateOptAttributes(int nGeoCls, CStringArray& fields, CStringArray& field_types);  
	virtual BOOL DelOptAttributes(CFeature *pFtr);
	virtual BOOL RestoreOptAttributes(CFeature *pFtr);
	virtual BOOL SaveOptAttributes(CFeature *pFtr, CValueTable& tab,int idx=0);

	// 保存数据源特定信息
	virtual void WriteSpecialData(CDataSourceEx *pDS) {}
protected:
		
protected:
	CString m_strWriteName;
};


class EXPORT_EDITBASE CAccessModify : public CAccessRead, public CAccessWrite
{
public:
	CAccessModify(){};
	virtual ~CAccessModify(){};

	virtual int GetAccessType(){ return -1; }

	virtual BOOL Attach(LPCTSTR fileName)=0;
	virtual BOOL Close()=0;

	virtual BOOL DelFtrLayer(CFtrLayer *pLayer)=0;
	virtual BOOL DelFeature(CFeature *pFt)=0;
	
	virtual BOOL Compress(){return TRUE;};

	//////////////////////////////
	virtual void SetDataSource(CDataSourceEx * _pds) {pds=_pds;}
	virtual void ClearAttrTables(){}
	
	virtual ObjectGroup* GetFirstObjectGroup() {return NULL;}
	virtual ObjectGroup* GetNextObjectGroup() {return NULL;}
	
	virtual FtrLayerGroup* GetFirstFtrLayerGroup() {return NULL;}
	virtual FtrLayerGroup* GetNextFtrLayerGroup() {return NULL;}
	
	virtual void SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type){}
	virtual BOOL DelDataSettings(LPCTSTR field) {return FALSE;}
	virtual BOOL ReadDataSettings(LPCTSTR field, CString &name, CString& value, CString &type){return FALSE;}
	
	virtual BOOL ReadFilePreViewImage(LPCTSTR name, BYTE *&img, int &len){return FALSE;}
	virtual BOOL DelFilePreViewImage(LPCTSTR name){return FALSE;}
	virtual void SaveFilePreViewImage(LPCTSTR name, BYTE *img, int len){}
	
	//更新编组
	virtual BOOL SaveObjectGroup(ObjectGroup *ftr) {return FALSE;}
	virtual BOOL DelObjectGroup(ObjectGroup *ftr) {return FALSE;}
	
	//更新层组
	virtual BOOL SaveFtrLayerGroup(FtrLayerGroup *pGroup) {return FALSE;}
	virtual BOOL DelFtrLayerGroup(FtrLayerGroup *pGroup) {return FALSE;}
	
	//读写存储在FDB中的单元
	virtual BOOL GetFirstCellDef(CellDef& def){return FALSE;}
	virtual BOOL GetNextCellDef(CellDef& def){return FALSE;}
	virtual BOOL SaveCellDef(CellDef& def){return FALSE;}
	virtual BOOL DelCellDef(LPCTSTR name){return FALSE;}
	virtual BOOL DelAllCellDefs(){return FALSE;}
	
	//更新地物
	virtual BOOL RestoreFeature(CFeature *pFt) {return FALSE;}
    
protected:
	CDataSourceEx *pds;
};

MyNameSpaceEnd

#endif // !defined(AFX_ACCESS_H__6B8586D5_E33C_475F_9832_0FFDF37FF871__INCLUDED_)
