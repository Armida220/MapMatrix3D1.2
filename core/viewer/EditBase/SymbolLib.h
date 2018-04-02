// SymbolLib.h: interface for the CSymbolLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SYMBOLLIB_H__3D009D4A_505B_4CB6_A52C_B6729C61EFF2__INCLUDED_)
#define AFX_SYMBOLLIB_H__3D009D4A_505B_4CB6_A52C_B6729C61EFF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"
#include "Scheme.h"
#include "Expression.h"

//XML tag of CPlaceConfig
#define XMLTAG_ACTION				_T("Action")
#define XMLTAG_PLACEPARAM 			_T("PlaceParam")
#define XMLTAG_LAYERNAME     		_T("LayerName")
#define XMLTAG_COMMANDID            _T("CommandID")
#define XMLTAG_CMDPARAMS			_T("CommandParams")
#define XMLTAG_VIEWPARAMS			_T("ViewParams")

#define XMLTAG_COMMAND				_T("Command")
#define XMLTAG_COMMANDITEM			_T("CmdItem")


//XML tag of ConfigItem
#define XMLTAG_LAYERCONFIG			_T("LayerConfig")
#define XMLTAG_EXPRESSION			_T("Expression")
#define XMLTAG_CONFIGFILE			_T("ConfigFile")
#define XMLTAG_DEFCONFIG			_T("DefaultConfig")


MyNameSpaceBegin


//采集(Place)配置项
class EXPORT_EDITBASE CPlaceConfig
{
public:
	CPlaceConfig();
	~CPlaceConfig();
	
	CPlaceConfig& operator=(const CPlaceConfig &config);
	void Copy(CPlaceConfig const& config);//hcw,2012.7.31.
	BOOL WriteTo(CString &strXml);
	BOOL ReadFrom(CString &strXml);
	
	TCHAR m_strLayName[64];
	int  m_nCommandID;
	TCHAR m_strCmdParams[1024];
	TCHAR m_strViewParams[128];
};

class EXPORT_EDITBASE CCmdXMLParams
{
public:
	CCmdXMLParams();
	void Copy(CCmdXMLParams const& cmdParams); //hcw,2012.7.31
	BOOL WriteTo(CString &strXml);
	BOOL ReadFrom(CString &strXml);
	
	int  m_nCommandID;
	TCHAR m_strCmdParams[1024];
};

//采集(Place)配置库
class EXPORT_EDITBASE CPlaceConfigLib
{
public:
	CPlaceConfigLib();
	virtual ~CPlaceConfigLib();

	BOOL Load(LPCTSTR pathname);
	BOOL Save(LPCTSTR pathname=NULL)const;
    void Copy(CPlaceConfigLib const& config); //hcw,2012.7.31
	CPlaceConfig* GetConfig(LPCTSTR strLayName);
	CPlaceConfig* GetConfig(int i);
	BOOL DeleteConfig(LPCTSTR strLayName);

	CCmdXMLParams* GetCmdXMLParams(int id);
	void AddCmdXMLParams(int id, LPCTSTR params);

	BOOL AddConfig(const CPlaceConfig &config);
//	BOOL SetConfig(LPCTSTR strLayName, CPlaceConfig item);
	int GetConfigCount() const{
		return m_arrItems.GetSize();
	};
	

protected:
	void Clear();

protected:
	CString m_strCfgPath;
	CArray<CPlaceConfig,CPlaceConfig> m_arrItems;

	CArray<CCmdXMLParams,CCmdXMLParams> m_arrCmdParams;
};

struct EXPORT_EDITBASE ConfigLibItem
{
	ConfigLibItem();
	void Create();
	void Clear();
	ConfigLibItem& operator=(const ConfigLibItem &config);
    void Copy(ConfigLibItem &config);//hcw,2012.7.27
	BOOL Load();
	CString GetPath() { return m_strPath; };
	int GetScale()  { return scale; };

	BOOL LoadCellLine(CString path=_T(""));
	BOOL SaveCellLine(CString path=_T(""));

	BOOL SetScalePath(CString strPath);
	BOOL bLoaded;
	CPlaceConfigLib *pPlaceCfgLib;
	CScheme *pScheme;
	CCellDefLib *pCellDefLib;
	CBaseLineTypeLib *pLinetypeLib;
private:
	DWORD scale;
	CString m_strPath;
};

class CDataSourceEx;
//extern BOOL IsDigital(CString strItem);
//符号库管理器
class EXPORT_EDITBASE CConfigLibManager
{
public:
	
public:
	CConfigLibManager();
	virtual ~CConfigLibManager();

	BOOL LoadConfig(LPCTSTR strConfigPath, BOOL bRefConfig=FALSE);
	// 如果有引用方案则删除备份方案
	void ResetConfig();
	void Clear();
	void ReLoadBakItems();//hcw,2012.7.30, for refresh m_arrBakItems;

	// 读入一个比例尺，返回下标
	int LoadScale(CString strPath, BOOL bLoaded=FALSE);
	BOOL SaveScale(int scale=0);
	LPCTSTR GetPath() const {
		return m_strPath;
	}
    void SetPath(CString strPath){ //hcw,2012.6.8,for SchemeMerge
		m_strPath = strPath;
	}
	CPlaceConfigLib *GetPlaceConfigLib(DWORD scale);
	CScheme *GetScheme(DWORD scale);
	CCellDefLib *GetCellDefLib(DWORD scale);
	CBaseLineTypeLib *GetBaseLineTypeLib(DWORD scale);
	BOOL GetSymbol(CDataSourceEx *pSource, CFeature *pFtr, CPtrArray& arr, LPCTSTR strLayerNameAppointed=NULL);

	int GetConfigLibCount() const{
		return m_arrItems.GetSize();
	}
	ConfigLibItem GetConfigLibItem(int i);
	
	ConfigLibItem GetConfigLibItemByScale(int scale);
	DWORD GetScaleByScheme(CScheme *pScheme);
	int AddConfigItem(ConfigLibItem item){
		m_arrItems.Add(item);
		return m_arrItems.GetSize()-1;
	}

	DWORD GetSuitedScale(DWORD scale);
	void Copy(CConfigLibManager * const pCfgLibMan);//hcw,2012.7.27,deep Copy
protected:
	ConfigLibItem FindItem(DWORD scale);	

protected:
	CString m_strPath, m_strBakPath;
	CArray<ConfigLibItem,ConfigLibItem> m_arrItems, m_arrBakItems;
};

EXPORT_EDITBASE CConfigLibManager *GetConfigLibManager();
EXPORT_EDITBASE void SetConfigLibManager(CConfigLibManager *pLib);
EXPORT_EDITBASE BOOL IsDigital(const CString& strText);
EXPORT_EDITBASE BOOL IsValidScaleFile(CString path);

MyNameSpaceEnd



#endif // !defined(AFX_SYMBOLLIB_H__3D009D4A_505B_4CB6_A52C_B6729C61EFF2__INCLUDED_)
