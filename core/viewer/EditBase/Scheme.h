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

//�Զ����ֶ�����,���ڱ�ʾ���Ա���ֶ�����
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
//	long index;			 //��Scheme��m_arrPLayDefines�е��±�,-1��ʾ��û�н���,���½�
	__int64 code;			 // ��1��ʾ��Чֵ
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


//����������
/*

  ��ҵ������Ŀ��
  a���ܴ�����ҵ��ʹ�õ�ʸ��ͼ�㣬ɾ������Ҫ��ʸ��ͼ�㣻
  b����Ϊÿ��ͼ�㶨����������ԣ��ڸ�ͼ���²ɼ��ĵ��ﶼ�߱���Щ���ԣ�
  c���ܶ���ͼ����������Ե�ȱʡֵ���ڸ�ͼ���²ɼ��ĵ��ﶼȱʡ������Щ����ֵ��
  d��ͼ�����߱���ɫ����������ʾ�Ļ������ԣ�
  e��ͼ��ķ��ſ������ã�
  
  ����������ص�
  a����������������ʱ���ô��ڵģ�����Ӧ�ó���ҵ���߼��ĺ���֮һ��
	 ����Դ��Ӧ�ó�����Ҫ������ʱ֪������
  b��ͨ�������Ի��ͼ���б�����ͼ����������͡�����������ȱʡֵ�������Ķ�̬���ԣ�
     �Ӷ������û���Ч�ɼ���ͼ��ĵ��
  c�����ݵķ�����CAccess���ݷ������������ʵ��ļ��洢�ṹ��
     ��������ʽ�ġ�����ϵ���������ԭ����δ֪�ġ�ǿ�󶨵�������
	
*/

struct EXPORT_EDITBASE XDefine
{
	XDefine();
	const XDefine& operator=(XDefine def);
	
	//�ֶ������洢�õ�����
	TCHAR field[32];
	
	//��������ʾ���û��������ƣ�
	TCHAR name[32];
	
	//ȱʡֵ
	TCHAR defvalue[64];
	
	//ֵ����
	int  valuetype;
	
	//ֵ����
	int  valuelen;

	//�Ƿ�����ֶΣ�����Ϊ�գ���0�����أ�1������
	int  isMust;

	//С����λ����>0 ��������
	int  nDigit;

	//ȡֵ��Χ��ö��ֵ�ö��ű�ʾ����Χֵ��~��ʾ
	TCHAR valueRange[128];
};

// �����в�Ķ���
class EXPORT_EDITBASE CSchemeLayerDefine
{
public:
	//������������λ��
	BOOL SwapXDefine(int ndef1,int ndef2);
	
	CSchemeLayerDefine();
	virtual ~CSchemeLayerDefine();
	void Copy( CSchemeLayerDefine& schemeLayerDefine);//hcw,2012.5.3,for ����CSchemeLayerDefine����
	void CopyXDefines(CSchemeLayerDefine& schemeLayerDefine, BOOL bOverrided=TRUE); //hcw,2012.9.10,bOverrided = TRUE,ֱ�Ӹ���,bOverrided = FALSE,׷�ӵ�ĩβ��
	void Clear();

	CFeature* CreateDefaultFeature(int cls=CLS_NULL);

	void ReadFrom(CString &xmlString);
	void WriteTo(CString &xmlString);
	
	//��ò���
	CString GetLayerName() { return m_strLayName; };
	void SetLayerName(LPCTSTR name) { m_strLayName = name; };
	
	//���������
	CString GetDBLayerName() { return m_strDBLayName; };
	void SetDBLayerName(LPCTSTR name) { m_strDBLayName = name; };

	CString GetAccel() { return m_strAccel; };
	void SetAccel(LPCTSTR name) { m_strAccel = name; };

	//��ȡ����
	CString GetGroupName() { return	m_strGroupName; };
	void SetGroupName(LPCTSTR name) { m_strGroupName = name; };

	//��ȡ���
	__int64 GetLayerCode() { return m_nCode; };
	void SetLayerCode(__int64 code) { m_nCode = code;};

	//��ȡ�ɼ�����������
	int GetGeoClass() { return m_nGeoClass; };
	void SetGeoClass(int ncls) { m_nGeoClass = ncls; };

	//��ȡ��⼸��������
	int GetDbGeoClass() { return m_nDbGeoClass; };
	void SetDbGeoClass(int ncls) { m_nDbGeoClass = ncls; };

	BOOL IsSupportGeoClass(int ncls);
	BOOL DelXDefines(); //hcw,2012.9.18
	CAnnotation* GetHeightAnnotation();

	
	BOOL GetSupportClsName(CStringArray &name);
	void SetSupportClsName(const CStringArray &name);

	long GetColor() { return m_nColor; };
	void SetColor(long lColor) { m_nColor = lColor; };

	// ��ȡ��������ȱʡֵ(��ȱʡ�������;���)
	BOOL GetBasicAttributeDefaultValues(CValueTable& tab);

	float GetDefaultLineWidth();

	// ��ù������Ե�ȱʡֵ
	BOOL GetDefaultValues(CValueTable& tab);
	BOOL SetDefaultValues(CValueTable& tab);

	//�����չ����������
	CString GetXClassName();
	void SetXClassName(LPCTSTR name);

	//�����չ���ԵĶ���
	const XDefine* GetXDefines(int &size);
	BOOL AddXDefine(XDefine item);
	BOOL DelXDefine(int idx);
	XDefine GetXDefine(int idx);
	BOOL SetXDefine(int idx, XDefine item);
	int FindXDefine(LPCTSTR field);
	int FindXDefineByName(LPCTSTR name);

	//�����չ���Ե�ȱʡֵ
	BOOL GetXDefaultValues(CValueTable& tab);
	BOOL SetXDefaultValues(CValueTable& tab);

	//��÷��������һ�������ָ��������ţ�
	BOOL AddSymbol(CSymbol* pItem);
	BOOL DelSymbol(int idx);
	int GetSymbolCount();
	CSymbol* GetSymbol(int idx);
	void GetSymbols(CArray<CSymbol*,CSymbol*> &sys);
	void SetSymbols(CArray<CSymbol*,CSymbol*>& sys); //hcw,2012.2.26,�������ò��з��š�
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
	// �ɼ�����������
	int m_nGeoClass;
	// ��⼸��������
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
	CScheme &operator=(const CScheme& scheme);//hcw,2012.5.17,����"="��������

	//д�뵽һ�� XML �ַ�������
	BOOL WriteTo(CString &strXml)const;
	//��һ�� XML �ַ������ж���
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
	int GetFirstLayerDefineIndex(LPCTSTR strGroupName, BOOL bSpecial=FALSE); //hcw,2012.7.11,����,��ȡĳ���µĵ�һ������Ϣ��
	CSchemeLayerDefine *GetLayerDefine(int idx, BOOL bSpecial=FALSE);
	CSchemeLayerDefine *GetLayerDefine(LPCTSTR layname, BOOL byCode=FALSE, __int64 code=0, BOOL bSpecial=FALSE);
	
	BOOL GetLayerDefinePreviewGrBuffer(LPCTSTR layname, GrBuffer *pBuf, BOOL bSpecial=FALSE);
	//byCode=TRUEͨ��codeѰ��,����ͨ��name
	BOOL FindLayerIdx(BOOL byCode, __int64 &code, CString& name);
	inline void SetPath(CString strPath){ m_strSchemePath = strPath; };
	BOOL AddToRecent(__int64 code, CString strLayerName);
	USERIDX *GetRecentIdx();

	void SetMaxRecentNum(int num);
	int  GetMaxRecentNum();

	void SetRecentIdx( USERIDX& userIDX);//hcw,2012.4.5,����m_UserRecent;
	void SortLayerDefinebyCode(CString strGroupName=_T(""));//hcw,2012.7.13,�����������ղ�����������

	float GetLayerDefineLineWidth(LPCTSTR layname);

	//��ȡ���ͼ���������չ�ֶζ���
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

	//Ϊ�˲��Ҹ����ٶ�����Ĳ㶨����
	CSchemeLayerDefine* m_pRecentLD;

	int m_nMaxRecentNum;
public:
	// ��������Ⱥ�˳��
	CStringArray m_strSortedGroupName;
};

MyNameSpaceEnd

#endif // !defined(AFX_SCHEME_H__576AEC74_4378_4A2C_91DA_20EF3899FB7E__INCLUDED_)
