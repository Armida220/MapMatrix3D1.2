// JBFormatData.h: interface for the CJBFormatData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JBFORMATDATA_H__C313AFAE_BB56_406E_AC07_F75680A6E9BE__INCLUDED_)
#define AFX_JBFORMATDATA_H__C313AFAE_BB56_406E_AC07_F75680A6E9BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueTable.h"

#define JBFIELD_MAPLBX 			"西南图廓角点横坐标"
#define JBFIELD_MAPLBY 			"西南图廓角点纵坐标"
#define JBFIELD_MAPRBX 			"东南图廓角点横坐标"
#define JBFIELD_MAPRBY 			"东南图廓角点纵坐标"
#define JBFIELD_MAPRTX 			"东北图廓角点横坐标"
#define JBFIELD_MAPRTY 			"东北图廓角点纵坐标"
#define JBFIELD_MAPLTX 			"西北图廓角点横坐标"
#define JBFIELD_MAPLTY 			"西北图廓角点纵坐标"

#define JBFIELD_LAYNAME			"层名"

#define JBFIELD_LONGRAIUS		"椭球长半径"
#define JBFIELD_FLATTENING		"椭球扁率"
#define JBFIELD_JIZHUN			"大地基准"

#define JBFIELD_MAPNAME			"图名"
#define JBFIELD_MAPNUMBER		"图号"
#define JBFIELD_CONTOURINTERV	"图幅等高距"
#define JBFIELD_MAPSCALE		"地图比例尺分母"
#define JBFIELD_LONGRANGE		"图廓角点经度范围"
#define JBFIELD_LATRANGE		"图廓角点纬度范围"
#define JBFIELD_DIM				"坐标维数"

#define JBFIELD_CENTERLONG		"中央经线"
#define JBFIELD_STRIPTYPE		"分带方式"
#define JBFIELD_STRIPNUM		"高斯投影带号"
#define JBFIELD_CENTERLONG0		"原图中央经线"
#define JBFIELD_STRIPTYPE0		"原图分带方式"

#define JBFIELD_ORIGINX			"相对原点横坐标"
#define JBFIELD_ORIGINY			"相对原点纵坐标"

#define JBFIELD_MAPSCALE0		"原图比例尺分母"
#define JBFIELD_CONTOURINTERVAL	"图幅等高距"
#define JBFIELD_COORDINATESCALE	"坐标放大系数"
#define JBFIELD_CONTOURINTERVAL0 "原图等高距"

//格式描述项
struct JBFormatDefineItem
{
	char name[64];
	char type[64];
	char format[64];
	char desc[256];

	void ConvertStringToValue(LPCTSTR text, _variant_t& var)const;
	void ConvertValueToString(_variant_t& var, CString& text)const;
	void NormalizeFormat();
	BOOL IsHaveEnumValues()const;
	void GetEnumValues(CStringArray* arrPNames, CStringArray *arrPValues=NULL)const;
	void CreateDefaultValue(_variant_t& var)const;
	CString ConvertEnumNameToValue(LPCTSTR text)const;
	CString ConvertEnumValueToName(LPCTSTR text)const;
};

//格式数据读写
class CJBFormatData
{
public:
	CJBFormatData();
	~CJBFormatData();
	
	BOOL LoadDefines(CString& text);
	BOOL LoadValues(int index, CString& text, CValueTable& tab);
	BOOL SaveValues(int index, CValueTable& tab, CString& text);
	
public:
	JBFormatDefineItem FindDefine(LPCTSTR field);
	
	CString m_strFmtName;
	CArray<JBFormatDefineItem,JBFormatDefineItem> m_FmtItems;
};



typedef struct JBFormatDefineItem JBMetaDefineItem;


class CDlgDoc;
class CJBMetaData
{
public:
	CJBMetaData();
	~CJBMetaData();
	
	BOOL LoadDefines();
	BOOL LoadDefines(CString& text);
	BOOL SetMapValue(LPCTSTR field, LPCTSTR value);
	BOOL SetLayValue(int idx, LPCTSTR field, LPCTSTR value);
	BOOL LoadValues(LPCTSTR fileName);
	BOOL SaveValues(LPCTSTR fileName);
	void CreateLayerValues(int num);

	void Clear();

	void ReloadValues();
	void SetValuesFromDoc(CDlgDoc *pDoc);
	JBFormatDefineItem FindMapDefine(LPCTSTR field);
	void RefreshBigNumber();

protected:
	void CreateDefaultValues();

public:
	CArray<JBMetaDefineItem,JBMetaDefineItem> m_mapItems;
	CArray<JBMetaDefineItem,JBMetaDefineItem> m_layItems;
	
	CString m_strDefinePath;
	CString m_strValuePath;
	CValueTable m_mapValues;
	CValueTable m_layValues;
	CString m_strLayNumField;
};


#endif // !defined(AFX_JBFORMATDATA_H__C313AFAE_BB56_406E_AC07_F75680A6E9BE__INCLUDED_)
