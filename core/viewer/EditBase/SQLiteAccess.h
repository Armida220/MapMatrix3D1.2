// SQLiteAccess.h: interface for the CSQLiteAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SQLITEACCESS_H__B9E08DE3_E6FC_4578_91E2_463124B26C8A__INCLUDED_)
#define AFX_SQLITEACCESS_H__B9E08DE3_E6FC_4578_91E2_463124B26C8A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"
#include "CppSQLite3.h"
#include "DataSourceEx.h"
#include "SymbolBase.h"

#define SQLITE_FDB_EDITION     _T("FeatureOne4.1_0001")

#define FIELDNAME_XATRRDEL     _T("XAtrrDel")
#define FIELDNAME_SUMMARYTITLE        _T("Title")
#define FIELDNAME_SUMMARYTOPIC        _T("Topic")
#define FIELDNAME_SUMMARYAUTHOR       _T("Author")
#define FIELDNAME_SUMMARYKEYWORD      _T("KeyWord")
#define FIELDNAME_SUMMARYREMARK       _T("Remark")
#define FIELDNAME_SUMMARYLINK         _T("Link")
#define FIELDNAME_STATEDITOR		  _T("Editor")
#define FIELDNAME_STATEDITNUM         _T("EditNum")
#define FIELDNAME_STATEDITTIME        _T("EditTime")
#define FIELDNAME_USERDEFINEFIELD1    _T("UserDefineField1")
#define FIELDNAME_USERDEFINEFIELD2    _T("UserDefineField2")
#define FIELDNAME_USERDEFINEFIELD3    _T("UserDefineField3")
#define FIELDNAME_USERDEFINEFIELD4    _T("UserDefineField4")
#define FIELDNAME_USERDEFINEFIELD5    _T("UserDefineField5")
#define FIELDNAME_USERDEFINEFIELD6    _T("UserDefineField6")
#define FIELDNAME_USERDEFINEFIELD7    _T("UserDefineField7")
#define FIELDNAME_USERDEFINEFIELD8    _T("UserDefineField8")
#define FIELDNAME_USERDEFINEFIELD9    _T("UserDefineField9")
#define FIELDNAME_USERDEFINEFIELD10   _T("UserDefineField10")
/*
 *	SQLite 数据库中数据的存储格式描述

  1、WorkSpace 表，存储工作区信息
  ID INT (主键，编号) | Scale INT (比例尺) | BOUND void* (工作区范围) 

  2、DLGVectorLayer 表，存储用户的矢量图层信息
  ID INT (主键，编号) | Name char* (名称) |  Visible BOOL (显示) | Locked BOOL (锁定) |
  Inherent INT (是否本地图层), DisplayOrder INT (显示顺序), Symbolized INT (是否符号化)

  3、基本特征表（4个：点、线、面、文字），存储矢量对象；
  点表：Point
  FTRID char[32] (主键，字符化的GUID) | ClassType INT (特征类型) | GeoClass INT (几何类型) | Color INT (颜色) | LayerID INT (所属层ID)  | Shape void* (图形数据) | 
  Angle double (点的方向角) | Length FLOAT (长度)| Width FLOAT (宽度)| SymbolName char[32] (符号名)
  线表：Line
  FTRID char[32] (主键，字符化的GUID) | ClassType INT (特征类型) | GeoClass INT (几何类型) | Color INT (颜色) | LayerID INT (所属层ID)  | Shape void* (图形数据)
  面表：Surface
  FTRID char[32] (主键，字符化的GUID) | ClassType INT (特征类型) | GeoClass INT (几何类型) | Color INT (颜色) | LayerID INT (所属层ID)  | Shape void* (图形数据)
  注记表：Text
  FTRID char[32] (主键，字符化的GUID) | ClassType INT (特征类型) | GeoClass INT (几何类型) | Color INT (颜色) | LayerID INT (所属层ID)  | Shape void* (图形数据) |
  Content VARCHAR (文本内容) |  Font  char[32] | CharWidth float (字符宽度) | CharHeight float (字符高度) | 
  CharIntv float (字符间距) | LineIntv float (行间距) |  AlignType int (对齐方式) |
  Incline int (倾斜/耸肩类型) | InclineAngle float (倾斜角度) | PlaceType int (排列方式)
 
 */


MyNameSpaceBegin

class CScheme;

class EXPORT_EDITBASE CSQLiteAccess : public CAccessModify
{
public:
	enum
	{
		modeRead = 0,
		modeWrite = 1,
		modeModify = 2
	};
	enum
	{
		version40 = 0,
		version41 = 1
	};
	virtual BOOL  Compress();
	virtual BOOL  Repair();
	CSQLiteAccess();
	virtual ~CSQLiteAccess();

	virtual int GetAccessType(){ return AccessType::SQLiteAccess; }
	
	CppSQLite3DB *GetSqliteDb();
	CppSQLite3Query *GetSqliteFtrQuery();
	CppSQLite3Statement *GetSqliteStm();
	virtual BOOL Attach(LPCTSTR fileName);
	virtual BOOL Close();

	BOOL CreateFileSys(long Scale, CScheme *ps=NULL);
	BOOL IsValid();

	virtual void ClearAttrTables();

	BOOL OpenNew(LPCTSTR fileName);
	virtual BOOL OpenRead(LPCTSTR filename);
	virtual BOOL OpenWrite(LPCTSTR filename);
	virtual BOOL CloseRead();
	virtual BOOL CloseWrite();

	virtual void ReadSpecialData(CDataSourceEx *pDS);
	virtual void WriteSpecialData(CDataSourceEx *pDS);

	virtual ObjectGroup* GetFirstObjectGroup();
	virtual ObjectGroup* GetNextObjectGroup();

	virtual FtrLayerGroup* GetFirstFtrLayerGroup();
	virtual FtrLayerGroup* GetNextFtrLayerGroup();
	
	//读取数据源信息
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS);
	
	//读取层
	virtual CFtrLayer *GetFirstFtrLayer(long &idx);
	virtual CFtrLayer *GetNextFtrLayer(long &idx);
	virtual int GetFtrLayerCount();
	CFtrLayer *GetFtrLayerByID(long id);
	CFtrLayer *GetFtrLayerByName(LPCTSTR layerName);
	
	//读取地物
	virtual CFeature *GetFirstFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual CFeature *GetNextFeature(long &idx, CValueTable *exAttriTab = NULL);
	virtual int GetFeatureCount();
	virtual int GetCurFtrLayID();
	virtual int GetCurFtrGrpID();

	//读取扩展属性信息
	virtual BOOL ReadXAttribute(CFeature *pFtr, CValueTable& tab);
	
	//更新数据源信息
	virtual void WriteDataSourceInfo(CDataSourceEx *pDS);
	
	//更新层
	virtual BOOL SaveFtrLayer(CFtrLayer *pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer *pLayer);

	//更新编组
	virtual BOOL SaveObjectGroup(ObjectGroup *ftr);
	virtual BOOL DelObjectGroup(ObjectGroup *ftr);

	//更新层组
	virtual BOOL SaveFtrLayerGroup(FtrLayerGroup *pGroup);
	virtual BOOL DelFtrLayerGroup(FtrLayerGroup *pGroup);
	
	BOOL DelAllFeature();
	//更新地物
	virtual BOOL SetCurFtrLayID(int id);
	virtual BOOL SetCurFtrGrpID(int id);
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL);
	virtual BOOL DelFeature(CFeature *pFt);
	virtual BOOL DelXAttributes(CFeature *pFtr);
	virtual BOOL RestoreXAttributes(CFeature *pFtr);
	virtual BOOL RestoreFeature(CFeature *pFt);
	//更新扩展属性信息
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0);

	//更新可选属性（可有可无的属性）
	//创建可选属性；可以创建多次，只会累加，不会删除；sqlite也不支持删除属性列；
	virtual BOOL CreateOptAttributes(int nGeoCls, CStringArray& fields, CStringArray& field_types);  
	virtual BOOL DelOptAttributes(CFeature *pFtr);
	virtual BOOL RestoreOptAttributes(CFeature *pFtr);
	virtual BOOL SaveOptAttributes(CFeature *pFtr, CValueTable& tab,int idx=0);
	virtual BOOL ReadOptAttributes(CFeature *pFtr, CValueTable& tab);
	virtual BOOL GetOptAttributeDef(int nGeoCls, CStringArray& fields, CStringArray& field_types);

	virtual void BatchUpdateBegin();
	virtual void BatchUpdateEnd();

	void SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type);
	BOOL DelDataSettings(LPCTSTR field);
	BOOL ReadDataSettings(LPCTSTR field, CString &name, CString& value, CString &type);

	BOOL ReadFilePreViewImage(LPCTSTR name, BYTE *&img, int &len);
	BOOL DelFilePreViewImage(LPCTSTR name);
	void SaveFilePreViewImage(LPCTSTR name, BYTE *img, int len);

	//读写存储在FDB中的单元
	BOOL GetFirstCellDef(CellDef& def);
	BOOL GetNextCellDef(CellDef& def);
	BOOL SaveCellDef(CellDef& def);
	BOOL DelCellDef(LPCTSTR name);
	BOOL DelAllCellDefs();
	BOOL IsFileVersionLatest();
	CString GetFileVersion();

protected:

	void DeleteTables(CScheme *ps);

	void CreateWorkspaceTable();

	//存储需要保存到矢量文件中的设置，例如，参考文件的信息就应该存在这里（目前通过附属文件来实现，不合理）
	void CreateDataSettingsTable();
	
	void CreateVectorLayerTable();
	void CreatePointTable();
	void CreateLineTable();
	void CreateSurfaceTable();
	void CreateTextTable();
	void CreateObjectGroupTable();
	void CreateLayerItems(CScheme *ps);
	void CreateXAttributeTables(CScheme *ps);
	void CreateFtrLayerGroupTable();

	void CreateFilePreViewImage();
	void CreateCellDefTable();

	CFeature *GetFirstPoint();
	CFeature *GetNextPoint();
	CFeature *ReadPoint();
	void SavePoint(CFeature *pFtr);

	CFeature *GetFirstLine();
	CFeature *GetNextLine();
	CFeature *ReadLine();
	void SaveLine(CFeature *pFtr);

	CFeature *GetFirstSurface();
	CFeature *GetNextSurface();
	CFeature *ReadSurface();
	void SaveSurface(CFeature *pFtr);

	CFeature *GetFirstText();
	CFeature *GetNextText();
	CFeature *ReadText();
	void SaveText(CFeature *pFtr);
	
protected:
	CFtrLayer *ReadFtrLayer();
	ObjectGroup* ReadObjectGroup();
	FtrLayerGroup* ReadFtrLayerGroup();
	BOOL ReadCellDef(CellDef& def);

	void TryCommit();
	
protected:
	void CreateVersionInfoTable();
	CppSQLite3DB m_db;
	BOOL m_bValid;
	BOOL m_bFileVersionLatest;
	BOOL m_bHaveCheckedVersion;
	int  m_nFileVersion;

	CppSQLite3Statement m_stm;

	//为了获得FtrLayer而暂存的层数据查询对象
	CppSQLite3Query m_layerQuery;

	//为了获得Feature而暂存的特征数据查询对象
	CppSQLite3Query m_ftrQuery;
	int m_nCurLayID;

	int m_nAccessMode;

	BOOL m_bBeginTraction;
	int m_nWriteCount;

private:
	void CreateDefaultLayer();
};


EXPORT_EDITBASE CString EncodeString(const char *p);
EXPORT_EDITBASE CString DecodeString(const char *p);


MyNameSpaceEnd

#endif // !defined(AFX_SQLITEACCESS_H__B9E08DE3_E6FC_4578_91E2_463124B26C8A__INCLUDED_)
