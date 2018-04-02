// UVSModify.h: interface for the CUVSModify class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UVSMODIFY_H__6AD196C1_6E15_4AF6_B84C_9D0E4131FFC7__INCLUDED_)
#define AFX_UVSMODIFY_H__6AD196C1_6E15_4AF6_B84C_9D0E4131FFC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Access.h"
//#include "UVSCashe.h"
#include <VECTOR>
#include <LIST>
#include <MAP>
#include <string>
#include <STDLIB.H>
#include <AFXMT.H>
using std::vector;
using std::list;
using std::map;
using std::string;

#define XATTR_INTEGER  1
#define XATTR_FLOAT    2
#define XATTR_BLOB     4
#define XATTR_NULL     5
#ifdef XATTR_TEXT
# undef XATTR_TEXT
#else
# define XATTR_TEXT     3
#endif
#define XATTR_TEXT     3

namespace uvsclient
{
	struct IUVSClient;
	struct WorkSpace;
	struct IRecordSet;
	struct MapSheet;
	struct VectorLayer;
	struct VectorGroup;
	struct DatasourceConfig;
	struct PointFTR;
	struct LineFTR;
	struct PolygonFTR;
	struct TextFTR;
	struct attributefielddefineSet;
}

namespace uvscashe
{
	class CUVSCashe;
	class BasicOperate;
	class OPStartTransaction;
	class OPEndTransaction;
	class OPAddFeature;
	class OPDeleteFeature;
	class OPAddvectorLayer;
	class OPDeletevectorLayer;
	class OPAddXAttribute;
	class OPDeleteXAttribute;
	class OPAddvectorgroup;
	class OPDeletevectorgroup;
}


class EXPORT_EDITBASE CUVSModify : public CAccessModify  
{
	friend class uvscashe::BasicOperate;
	friend class uvscashe::OPStartTransaction;
	friend class uvscashe::OPEndTransaction;
	friend class uvscashe::OPAddFeature;
	friend class uvscashe::OPDeleteFeature;
	friend class uvscashe::OPAddvectorLayer;
	friend class uvscashe::OPDeletevectorLayer;
	friend class uvscashe::OPAddXAttribute;
	friend class uvscashe::OPDeleteXAttribute;
	friend class uvscashe::OPAddvectorgroup;
	friend class uvscashe::OPDeletevectorgroup;
	friend class uvscashe::CUVSCashe;
public:
	CUVSModify();
	virtual ~CUVSModify();
	virtual int GetAccessType(){ return AccessType::UVSModify; }
public:
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
	
	virtual BOOL Attach(LPCTSTR fileName);
	virtual BOOL Close();

	virtual BOOL CreateFileSys(long Scale, CScheme *ps=NULL);
	virtual BOOL IsValid();

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

	virtual void SaveDataSettings(LPCTSTR field, LPCTSTR name, LPCTSTR value, LPCTSTR type);
	virtual BOOL DelDataSettings(LPCTSTR field);
	virtual BOOL ReadDataSettings(LPCTSTR field, CString &name, CString& value, CString &type);

	virtual BOOL ReadFilePreViewImage(LPCTSTR name, BYTE *&img, int &len);
	virtual BOOL DelFilePreViewImage(LPCTSTR name);
	virtual void SaveFilePreViewImage(LPCTSTR name, BYTE *img, int len);

	//读写存储在FDB中的单元
	virtual BOOL GetFirstCellDef(CellDef& def);
	virtual BOOL GetNextCellDef(CellDef& def);
	virtual BOOL SaveCellDef(CellDef& def);
	virtual BOOL DelCellDef(LPCTSTR name);
	virtual BOOL DelAllCellDefs();
	virtual BOOL IsFileVersionLatest();
	virtual CString GetFileVersion();
protected:
    void ReadFtrLayer();
	void ReadObjGroup();
	//
	void valuetableFromVectorLayer(CValueTable & tab,uvsclient::VectorLayer & temp_layer);
	void valuetableFromPointFTR(CValueTable & tab,uvsclient::PointFTR & point_ftr);
	void valuetableFromLineFTR(CValueTable & tab,uvsclient::LineFTR & line_ftr);
	void valuetableFromPolygonFTR(CValueTable & tab,uvsclient::PolygonFTR & polygon_ftr);
	void valuetableFromTextFTR(CValueTable & tab,uvsclient::TextFTR & text_ftr);
	//
    void vectorlayerFromValueTable(CValueTable & tab,uvsclient::VectorLayer & temp_layer);
	void pointftrFromValueTable(CValueTable & tab,uvsclient::PointFTR & point_ftr);
	void lineftrFromValueTable(CValueTable & tab,uvsclient::LineFTR & line_ftr);
	void polygonftrFromValueTable(CValueTable & tab,uvsclient::PolygonFTR & polygon_ftr);
	void textftrFromValueTable(CValueTable & tab,uvsclient::TextFTR & text_ftr);
public:
	static BOOL ConnectUVSServer(LPCTSTR ip, int port);
	static BOOL OpenDB(LPCTSTR db_name, LPCTSTR user_name, LPCTSTR password,
		                         CString & identity_id,CString & identity_id2); 
	static void UpdateImmediately(BOOL _update_immediately);
	static BOOL is_updateimmediately();
	static BOOL CanExport();
	static void SetDelayCount(int delay_count);
	static int  GetDelayCount();
	static void ReleaseUVSServer();
	static void EnumerateDBList(CStringArray& db_list);
	static void EnumerateWorkspace(CStringArray& workspace_list);
	static BOOL SetCurrentWorkspace(LPCTSTR workspace_name);
	static CString GetCurrentWorkspace();
	static BOOL IsDBConnected();
	static void EnumerateMapsheet(CStringArray& mapsheet_list, CArray<double,double>& bounds);
	static BOOL AddMapsheet(LPCTSTR name,double lbx=0,double lby=0,double rbx=1000,double rby=0,
										 double rtx=1000, double rty=1000,double ltx=0,double lty=1000,
										 float zmin=-1000,float zmax=1000);
	static CString GetWorkspaceScale();
	static bool IsMapsheetExist(LPCTSTR map_name);
private:
	string toHexString(const unsigned char* input, const int datasize)
	{
		string output;
		char ch[3];
		
		for (int i = 0; i < datasize; ++i)
		{
			sprintf(ch, "%02x", input[i]);
			output += ch;
		}
		return output;
	}
	
	unsigned char * toHexArray(string hex_str)
	{
		int length = hex_str.length();
		if (length % 2 != 0)
		{
			return NULL;
		}
		unsigned char * buffer = new unsigned char[length / 2];
		int k = 0;
		int t = 0;
		char ch[2];
		while (k < length)
		{
			ch[0] = hex_str[k];
			++k;
			ch[1] = hex_str[k];
			unsigned long temp = strtoul(ch, 0, 16);
			buffer[t] = temp;
			++k;
			++t;
		}
		return (unsigned char *)buffer;
	}

private:
	static uvsclient::IUVSClient * puvs;
	static BOOL update_immediately;
	static uvsclient::WorkSpace * current_workspace;
	static BOOL can_export_fdb;
	//
	uvsclient::MapSheet * current_mapsheet;
	vector<uvsclient::VectorLayer> vector_layer;
	vector<uvsclient::VectorGroup> vector_group;
	uvsclient::IRecordSet * record_set;
	map<CStringA,uvsclient::attributefielddefineSet*> layer_xattribute;
	CMutex mutex_layer_xattribute;

    list<CFeature*> feature_list;
	int fetch_size;
	bool point_loaded;
	bool line_loaded;
	bool polygon_loaded;
	bool text_loaded;
	int read_layer_index;
	int read_group_index;
	int m_nCurLayID;
};

#endif // !defined(AFX_UVSMODIFY_H__6AD196C1_6E15_4AF6_B84C_9D0E4131FFC7__INCLUDED_)
