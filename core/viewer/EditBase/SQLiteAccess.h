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
 *	SQLite ���ݿ������ݵĴ洢��ʽ����

  1��WorkSpace ���洢��������Ϣ
  ID INT (���������) | Scale INT (������) | BOUND void* (��������Χ) 

  2��DLGVectorLayer ���洢�û���ʸ��ͼ����Ϣ
  ID INT (���������) | Name char* (����) |  Visible BOOL (��ʾ) | Locked BOOL (����) |
  Inherent INT (�Ƿ񱾵�ͼ��), DisplayOrder INT (��ʾ˳��), Symbolized INT (�Ƿ���Ż�)

  3������������4�����㡢�ߡ��桢���֣����洢ʸ������
  ���Point
  FTRID char[32] (�������ַ�����GUID) | ClassType INT (��������) | GeoClass INT (��������) | Color INT (��ɫ) | LayerID INT (������ID)  | Shape void* (ͼ������) | 
  Angle double (��ķ����) | Length FLOAT (����)| Width FLOAT (���)| SymbolName char[32] (������)
  �߱�Line
  FTRID char[32] (�������ַ�����GUID) | ClassType INT (��������) | GeoClass INT (��������) | Color INT (��ɫ) | LayerID INT (������ID)  | Shape void* (ͼ������)
  ���Surface
  FTRID char[32] (�������ַ�����GUID) | ClassType INT (��������) | GeoClass INT (��������) | Color INT (��ɫ) | LayerID INT (������ID)  | Shape void* (ͼ������)
  ע�Ǳ�Text
  FTRID char[32] (�������ַ�����GUID) | ClassType INT (��������) | GeoClass INT (��������) | Color INT (��ɫ) | LayerID INT (������ID)  | Shape void* (ͼ������) |
  Content VARCHAR (�ı�����) |  Font  char[32] | CharWidth float (�ַ����) | CharHeight float (�ַ��߶�) | 
  CharIntv float (�ַ����) | LineIntv float (�м��) |  AlignType int (���뷽ʽ) |
  Incline int (��б/�ʼ�����) | InclineAngle float (��б�Ƕ�) | PlaceType int (���з�ʽ)
 
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
	
	//��ȡ����Դ��Ϣ
	virtual BOOL ReadDataSourceInfo(CDataSourceEx *pDS);
	
	//��ȡ��
	virtual CFtrLayer *GetFirstFtrLayer(long &idx);
	virtual CFtrLayer *GetNextFtrLayer(long &idx);
	virtual int GetFtrLayerCount();
	CFtrLayer *GetFtrLayerByID(long id);
	CFtrLayer *GetFtrLayerByName(LPCTSTR layerName);
	
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
	virtual BOOL SaveFtrLayer(CFtrLayer *pLayer);
	virtual BOOL DelFtrLayer(CFtrLayer *pLayer);

	//���±���
	virtual BOOL SaveObjectGroup(ObjectGroup *ftr);
	virtual BOOL DelObjectGroup(ObjectGroup *ftr);

	//���²���
	virtual BOOL SaveFtrLayerGroup(FtrLayerGroup *pGroup);
	virtual BOOL DelFtrLayerGroup(FtrLayerGroup *pGroup);
	
	BOOL DelAllFeature();
	//���µ���
	virtual BOOL SetCurFtrLayID(int id);
	virtual BOOL SetCurFtrGrpID(int id);
	virtual BOOL SaveFeature(CFeature *pFt, CValueTable *exAttriTab = NULL);
	virtual BOOL DelFeature(CFeature *pFt);
	virtual BOOL DelXAttributes(CFeature *pFtr);
	virtual BOOL RestoreXAttributes(CFeature *pFtr);
	virtual BOOL RestoreFeature(CFeature *pFt);
	//������չ������Ϣ
	virtual BOOL SaveXAttribute(CFeature *pFtr, CValueTable& tab,int idx=0);

	//���¿�ѡ���ԣ����п��޵����ԣ�
	//������ѡ���ԣ����Դ�����Σ�ֻ���ۼӣ�����ɾ����sqliteҲ��֧��ɾ�������У�
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

	//��д�洢��FDB�еĵ�Ԫ
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

	//�洢��Ҫ���浽ʸ���ļ��е����ã����磬�ο��ļ�����Ϣ��Ӧ�ô������Ŀǰͨ�������ļ���ʵ�֣�������
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

	//Ϊ�˻��FtrLayer���ݴ�Ĳ����ݲ�ѯ����
	CppSQLite3Query m_layerQuery;

	//Ϊ�˻��Feature���ݴ���������ݲ�ѯ����
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
