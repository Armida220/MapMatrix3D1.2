// JBFormatData.h: interface for the CJBFormatData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JBFORMATDATA_H__C313AFAE_BB56_406E_AC07_F75680A6E9BE__INCLUDED_)
#define AFX_JBFORMATDATA_H__C313AFAE_BB56_406E_AC07_F75680A6E9BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValueTable.h"

#define JBFIELD_MAPLBX 			"����ͼ���ǵ������"
#define JBFIELD_MAPLBY 			"����ͼ���ǵ�������"
#define JBFIELD_MAPRBX 			"����ͼ���ǵ������"
#define JBFIELD_MAPRBY 			"����ͼ���ǵ�������"
#define JBFIELD_MAPRTX 			"����ͼ���ǵ������"
#define JBFIELD_MAPRTY 			"����ͼ���ǵ�������"
#define JBFIELD_MAPLTX 			"����ͼ���ǵ������"
#define JBFIELD_MAPLTY 			"����ͼ���ǵ�������"

#define JBFIELD_LAYNAME			"����"

#define JBFIELD_LONGRAIUS		"���򳤰뾶"
#define JBFIELD_FLATTENING		"�������"
#define JBFIELD_JIZHUN			"��ػ�׼"

#define JBFIELD_MAPNAME			"ͼ��"
#define JBFIELD_MAPNUMBER		"ͼ��"
#define JBFIELD_CONTOURINTERV	"ͼ���ȸ߾�"
#define JBFIELD_MAPSCALE		"��ͼ�����߷�ĸ"
#define JBFIELD_LONGRANGE		"ͼ���ǵ㾭�ȷ�Χ"
#define JBFIELD_LATRANGE		"ͼ���ǵ�γ�ȷ�Χ"
#define JBFIELD_DIM				"����ά��"

#define JBFIELD_CENTERLONG		"���뾭��"
#define JBFIELD_STRIPTYPE		"�ִ���ʽ"
#define JBFIELD_STRIPNUM		"��˹ͶӰ����"
#define JBFIELD_CENTERLONG0		"ԭͼ���뾭��"
#define JBFIELD_STRIPTYPE0		"ԭͼ�ִ���ʽ"

#define JBFIELD_ORIGINX			"���ԭ�������"
#define JBFIELD_ORIGINY			"���ԭ��������"

#define JBFIELD_MAPSCALE0		"ԭͼ�����߷�ĸ"
#define JBFIELD_CONTOURINTERVAL	"ͼ���ȸ߾�"
#define JBFIELD_COORDINATESCALE	"����Ŵ�ϵ��"
#define JBFIELD_CONTOURINTERVAL0 "ԭͼ�ȸ߾�"

//��ʽ������
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

//��ʽ���ݶ�д
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
