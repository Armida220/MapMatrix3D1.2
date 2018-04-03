// Scheme.h: interface for the CScheme class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCHEME_H__576AEC74_4378_4A2C_91DA_20EF3899FB7E__INCLUDED_)
#define AFX_SCHEME_H__576AEC74_4378_4A2C_91DA_20EF3899FB7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"
#include "UIParam.h"
#include "Symbol.h"
#include <algorithm>//hcw,2012.4.5
#include <functional>//hcw,2012.4.5


#define XMLTAG_ISMUST          _T("IsMust")
#define XMLTAG_DIGITNUM        _T("DigitNum")
#define XMLTAG_VALUERANGE      _T("ValueRange")


MyNameSpaceBegin

//自定义字段类型,用于表示属性表的字段类型
enum DpCustomFieldType
{
	DP_CFT_NULL			= 0,			
	DP_CFT_SMALLINT		= 1,			//SQL_C_SHORT
	DP_CFT_BYTE			= 11,			//SQL_C_SHORT
	DP_CFT_INTEGER		= 2,			//SQL_C_LONG
	DP_CFT_FLOAT		= 3,			//SQL_C_DOUBLE
	DP_CFT_DOUBLE		= 31,			//SQL_C_DOUBLE
	DP_CFT_VARCHAR		= 4,			//SQL_C_CHAR
	DP_CFT_DATE			= 5,			//SQL_C_TYPE_DATE
	DP_CFT_BLOB			= 6,			//SQL_C_LONGVARBINARY
	DP_CFT_BOOL			= 7,			//SQL_C_SHORT
	DP_CFT_COLOR		= 8,			//SQL_C_LONG
	DP_CFT_ENUM			= 9				//SQL_C_LONG
};


struct LAYGROUP
{
	TCHAR GroupName[_MAX_FNAME];
};

struct EXPORT_EDITBASE IDX
{
	IDX()
	{
		code = -1;
		groupidx = -1;
		memset(FeatureName,0,_MAX_FNAME);
		memset(strAccel,0,_MAX_FNAME);
	}
//	long index;			 //在Scheme中m_arrPLayDefines中的下标,-1表示还没有建立,需新建
	__int64 code;			 // －1表示无效值
	long groupidx;
	
	TCHAR FeatureName[_MAX_FNAME];
	TCHAR strAccel[_MAX_FNAME];

};


struct EXPORT_EDITBASE USERIDX
{
	void operator=(const USERIDX &user)
	{
		m_aGroup.Copy(user.m_aGroup);
		m_aIdx.Copy(user.m_aIdx);
		
	}
	void Clear()
	{
		m_aGroup.RemoveAll();
		m_aIdx.RemoveAll();
	}
	CArray<LAYGROUP,LAYGROUP&> m_aGroup;
	CArray<IDX,IDX&> m_aIdx;
};


//方案的描述
/*

  作业方案的目标
  a、能创建作业中使用的矢量图层，删除不需要的矢量图层；
  b、能为每个图层定义任意的属性；在该图层下采集的地物都具备这些属性；
  c、能定义图层的所有属性的缺省值；在该图层下采集的地物都缺省具有这些属性值；
  d、图层必须具备颜色、锁定、显示的基本属性；
  e、图层的符号可以配置；
  
  方案对象的特点
  a、方案对象是运行时永久存在的，它是应用程序业务逻辑的核心之一；
	 数据源、应用程序都需要在运行时知道它；
  b、通过它可以获得图层列表、各个图层的特征类型、特征的属性缺省值、特征的动态属性，
     从而允许用户有效采集该图层的地物；
  c、数据的访问类CAccess根据方案对象建立合适的文件存储结构，
     用这种显式的、弱耦合的依赖代替原来的未知的、强绑定的依赖；
	
*/

struct EXPORT_EDITBASE XDefine
{
	XDefine();
	const XDefine& operator=(XDefine def);
	
	//字段名（存储用的名）
	TCHAR field[32];
	
	//别名（显示给用户看的名称）
	TCHAR name[32];
	
	//缺省值
	TCHAR defvalue[64];
	
	//值类型
	int  valuetype;
	
	//值长度
	int  valuelen;

	//是否必填字段（不能为空），0，不必，1，必须
	int  isMust;

	//小数点位数，>0 才有意义
	int  nDigit;

	//取值范围，枚举值用逗号表示，范围值用~表示
	TCHAR valueRange[128];
};

// 方案中层的定义
class EXPORT_EDITBASE CSchemeLayerDefine
{
public:
	//交换两个属性位置
	BOOL SwapXDefine(int ndef1,int ndef2);
	
	CSchemeLayerDefine();
	virtual ~CSchemeLayerDefine();
	void Copy( CSchemeLayerDefine& schemeLayerDefine);//hcw,2012.5.3,for 拷贝CSchemeLayerDefine对象。
	void CopyXDefines(CSchemeLayerDefine& schemeLayerDefine, BOOL bOverrided=TRUE); //hcw,2012.9.10,bOverrided = TRUE,直接覆盖,bOverrided = FALSE,追加到末尾。
	void Clear();

	CFeature* CreateDefaultFeature(int cls=CLS_NULL);

	void ReadFrom(CString &xmlString);
	void WriteTo(CString &xmlString);
	
	//获得层名
	CString GetLayerName() { return m_strLayName; };
	void SetLayerName(LPCTSTR name) { m_strLayName = name; };
	
	//获得入库层名
	CString GetDBLayerName() { return m_strDBLayName; };
	void SetDBLayerName(LPCTSTR name) { m_strDBLayName = name; };

	CString GetAccel() { return m_strAccel; };
	void SetAccel(LPCTSTR name) { m_strAccel = name; };

	//获取组名
	CString GetGroupName() { return	m_strGroupName; };
	void SetGroupName(LPCTSTR name) { m_strGroupName = name; };

	//获取层号
	__int64 GetLayerCode() { return m_nCode; };
	void SetLayerCode(__int64 code) { m_nCode = code;};

	//获取采集几何体类型
	int GetGeoClass() { return m_nGeoClass; };
	void SetGeoClass(int ncls) { m_nGeoClass = ncls; };

	//获取入库几何体类型
	int GetDbGeoClass() { return m_nDbGeoClass; };
	void SetDbGeoClass(int ncls) { m_nDbGeoClass = ncls; };

	BOOL IsSupportGeoClass(int ncls);
	BOOL DelXDefines(); //hcw,2012.9.18
	CAnnotation* GetHeightAnnotation();

	
	BOOL GetSupportClsName(CStringArray &name);
	void SetSupportClsName(const CStringArray &name);

	long GetColor() { return m_nColor; };
	void SetColor(long lColor) { m_nColor = lColor; };

	// 获取固有属性缺省值(由缺省几何类型决定)
	BOOL GetBasicAttributeDefaultValues(CValueTable& tab);

	float GetDefaultLineWidth();

	// 获得固有属性的缺省值
	BOOL GetDefaultValues(CValueTable& tab);
	BOOL SetDefaultValues(CValueTable& tab);

	//获得扩展属性类名称
	CString GetXClassName();
	void SetXClassName(LPCTSTR name);

	//获得扩展属性的定义
	const XDefine* GetXDefines(int &size);
	BOOL AddXDefine(XDefine item);
	BOOL DelXDefine(int idx);
	XDefine GetXDefine(int idx);
	BOOL SetXDefine(int idx, XDefine item);
	int FindXDefine(LPCTSTR field);
	int FindXDefineByName(LPCTSTR name);

	//获得扩展属性的缺省值
	BOOL GetXDefaultValues(CValueTable& tab);
	BOOL SetXDefaultValues(CValueTable& tab);

	//获得符号配置项；一个层可以指定多个符号；
	BOOL AddSymbol(CSymbol* pItem);
	BOOL DelSymbol(int idx);
	int GetSymbolCount();
	CSymbol* GetSymbol(int idx);
	void GetSymbols(CArray<CSymbol*,CSymbol*> &sys);
	void SetSymbols(CArray<CSymbol*,CSymbol*>& sys); //hcw,2012.2.26,重新设置层中符号。
	void SymbolToTop(int idx);
	void SymbolToTail(int idx);

	BOOL IsFromCADSymbol();
	BOOL CreateFromCADSymbol(CSymbol *pSym);

protected:
	BOOL m_bFromCADSymbol;
	CString m_strGroupName;
	__int64 m_nCode;
	CString m_strLayName;
	CString m_strAccel;
	CStringArray m_strSupportClsName;
	CString m_strDBLayName;
	// 采集几何体类型
	int m_nGeoClass;
	// 入库几何体类型
	int m_nDbGeoClass;
	long m_nColor;

	CValueTable m_defValueTable;

	CArray<XDefine,XDefine> m_arrXDefines;
	CArray<CSymbol*,CSymbol*> m_arrPCfgs;
};

class EXPORT_EDITBASE CScheme  
{
public:
	BOOL GetXAttributesAlias(CString LayerName,CValueTable &tab);

	CScheme();
	virtual ~CScheme();
	void Clear();
	CScheme &operator=(const CScheme& scheme);//hcw,2012.5.17,重载"="操作符。

	//写入到一个 XML 字符串流中
	BOOL WriteTo(CString &strXml)const;
	//从一个 XML 字符串流中读出
	BOOL ReadFrom(CString &strXml);

	BOOL Load(LPCTSTR strPath);
	BOOL Save(LPCTSTR strPath=NULL)const;

	BOOL LoadCADSymols();
	BOOL ImportCADSymbols(LPCTSTR strPath);
	void SaveCADSymbols();
	void DeleteCADSymbols();

	BOOL AddLayerDefine(CSchemeLayerDefine *pItem, BOOL bSpecial=FALSE);
	BOOL InsertLayerDefineAt(int nIndex, CSchemeLayerDefine *pItem, BOOL bSpecial=FALSE);
	BOOL DelLayerDefine(int idx, BOOL freeMemory=TRUE, BOOL bSpecial=FALSE);
	
	int GetLayerDefineCount(BOOL bSpecial=FALSE)const;
	int GetLayerDefineIndex(LPCTSTR layname, BOOL bSpecial=FALSE);
	int GetFirstLayerDefineIndex(LPCTSTR strGroupName, BOOL bSpecial=FALSE); //hcw,2012.7.11,重载,获取某组下的第一个层信息。
	CSchemeLayerDefine *GetLayerDefine(int idx, BOOL bSpecial=FALSE);
	CSchemeLayerDefine *GetLayerDefine(LPCTSTR layname, BOOL byCode=FALSE, __int64 code=0, BOOL bSpecial=FALSE);
	
	BOOL GetLayerDefinePreviewGrBuffer(LPCTSTR layname, GrBuffer *pBuf, BOOL bSpecial=FALSE);
	//byCode=TRUE通过code寻找,否则通过name
	BOOL FindLayerIdx(BOOL byCode, __int64 &code, CString& name);
	inline void SetPath(CString strPath){ m_strSchemePath = strPath; };
	BOOL AddToRecent(__int64 code, CString strLayerName);
	USERIDX *GetRecentIdx();

	void SetMaxRecentNum(int num);
	int  GetMaxRecentNum();

	void SetRecentIdx( USERIDX& userIDX);//hcw,2012.4.5,设置m_UserRecent;
	void SortLayerDefinebyCode(CString strGroupName=_T(""));//hcw,2012.7.13,根据组名按照层码重新排序。

	float GetLayerDefineLineWidth(LPCTSTR layname);

	//获取入库图层的所有扩展字段定义
	int GetDBLayerXdefines(CString dblayname, CArray<const XDefine*, const XDefine*>& xdefs);

protected:
	void ClearCADSymbols();
	void CreateCADSymbols();
	CString GetCADSymbolPath();

protected:
	CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> m_arrPLayDefines;
	CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> m_arrSpecialGroup;
	CString m_strSchemePath;
	USERIDX m_UserRecent;

	CCADSymbolLib *m_pCADSymbolLib;

	//为了查找更快速而缓存的层定义项
	CSchemeLayerDefine* m_pRecentLD;

	int m_nMaxRecentNum;
public:
	// 保存组的先后顺序
	CStringArray m_strSortedGroupName;
};

MyNameSpaceEnd

#endif // !defined(AFX_SCHEME_H__576AEC74_4378_4A2C_91DA_20EF3899FB7E__INCLUDED_)
