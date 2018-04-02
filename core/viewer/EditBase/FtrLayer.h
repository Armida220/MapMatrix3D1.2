// FtrLayer.h: interface for the CFtrLayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FTRLAYER_H__82C73E3D_D76D_40A4_BEFB_CFF3EE095E24__INCLUDED_)
#define AFX_FTRLAYER_H__82C73E3D_D76D_40A4_BEFB_CFF3EE095E24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Feature.h"
#include "FArray.hpp"

#define FILTERMODE_NULL				    0x00000000L
#define FILTERMODE_DELETED				0x00000001L
#define FILTERMODE_INVISIBLE			0x00000002L
#define CLS_USERLAYER				100


#define FIELDNAME_LAYERID			_T("ID")
#define FIELDNAME_LAYERCODE			_T("CODE")
#define FIELDNAME_LAYERNAME			_T("LAYERNAME")
#define FIELDNAME_LAYMAPNAME		_T("MAPNAME")
#define FIELDNAME_LAYGROUPNAME		_T("GROUPNAME")
#define FIELDNAME_LAYVISIBLE		_T("VISIBLE")
#define FIELDNAME_LAYLOCKED			_T("LOCKED")
#define FIELDNAME_LAYATTR			_T("ATTR")
#define FIELDNAME_LAYFTRTABLE		_T("FTRTABLE")
#define FIELDNAME_LAYCOLOR			_T("COLOR")
#define FIELDNAME_LAYUSED			_T("LAYERUSED")
#define FIELDNAME_LAYINHERENT		_T("LAYERINHERENT")
#define FIELDNAME_LAYERDISPLAYORDER	_T("DISPLAYORDER")
#define FIELDNAME_LAYERSYMBOLIZED  	_T("SYMBOLIZED")
#define FIELDNAME_LAYWORKSPACENAME  _T("WORKSPACENAME")

MyNameSpaceBegin


struct EXPORT_EDITBASE UpdateDispyParam
{
	enum
	{
		//原来定义成 UPDATE_COLOR，很容易跟#define宏冲突
		typeNONE = -1,
		typeCOLOR = 0,
		typeVISIBLE = 1
	};
	UpdateDispyParam()
	{
		type = typeNONE;
		handle = 0;
		memset(&data,0,sizeof(data));
	}
	int type;
	LONG_PTR handle;
	union 
	{
		bool bVisible;
		long lColor;
	}data;			
};


class EXPORT_EDITBASE CFtrLayer : public CPermanent
{
public:
	CFtrLayer();
	CFtrLayer(int id);
	void CopyFrom(const CFtrLayer * rv);
	virtual ~CFtrLayer();
	int GetClassType()const;
	static CPermanent* CreateObject(){ return new CFtrLayer; };

	// 控制地物显示顺序（编号无限增大的问题）
	BOOL DisplayTop(CFeature *pFtr);
	BOOL DisplayBottom(CFeature *pFtr);
	double GetFtrsMaxMinDisplayOrder(BOOL bMax);
	void GetAllFtrsByDisplayOrder(CFtrArray &arr);
	void ResetFtrsDisplayOrder();

	BOOL GetSymbols(CPtrArray &arr);
	void SetSymbols(const CPtrArray &arr);

	void SetInherentFlag(BOOL bInherent);
	BOOL IsInherent();
	BOOL IsEmpty();
	int GetID()const;
	LPCTSTR GetstrID();
	LPCTSTR GetXAttributeName();
	LPCTSTR GetName()const;
	LPCTSTR GetMapName()const;
	void SetName(LPCTSTR name);
	void SetID(int nId);
	void SetstrID(LPCTSTR strid);
	void SetXAttributeName(LPCTSTR xname);

	LPCTSTR GetGroupName()const { return m_strGroupName; }
	void SetGroupName(LPCTSTR name) { m_strGroupName = name; }

	void EnableLocked(BOOL bLocked);
	void EnableVisible(BOOL bVisible);
	BOOL IsVisible()const;	
	BOOL IsLocked()const;
	BOOL IsDeleted()const;
	void SetDeleted(BOOL bDeleted);

	CFeature* CreateDefaultFeature(int nScale,int clsid = CLS_NULL);
	COLORREF GetColor()const;
	void SetColor(COLORREF color);

	int GetDisplayOrder()const{
		return m_nDisplayOrder;
	}
	void SetDisplayOrder(int nOrder){
		m_nDisplayOrder = nOrder;
	}

	int IsSymbolized()const{
		return m_bSymbolized;
	}
	void EnableSymbolized(BOOL bSymbolized){
		m_bSymbolized = bSymbolized;
	}

	BOOL IsHaveExtraAttr()const{
		return m_bHaveExtraAttr;
	}

	BOOL IsAllowQuery()const;

	void SetHaveExtraAttr(BOOL bHave){
		m_bHaveExtraAttr = bHave;
	}

	int GetObjectType()const;
	
	BOOL AddObject(CFeature* pFt);
	BOOL DeleteObject(CFeature* pFt);
	int GetObjectCount();
	CFeature* GetObject(int i, int filterMode = FILTERMODE_DELETED|FILTERMODE_INVISIBLE);

	virtual BOOL WriteTo(CValueTable& tab)const;
	virtual BOOL ReadFrom(CValueTable& tab,int idx = 0);
	
public:
	int GetValidObjsCount();
	int GetEditableObjsCount();
	void SetAllowQuery(BOOL bQuery);
	void SetMapName(LPCTSTR mapName);
	static const long m_nClassType;
protected:
	BOOL FindObj(FTR_HANDLE handle, int& insert_idx);
protected:
//	CFArray<CFeature*> m_arrObjs;
	CArray<CFeature*,CFeature*> m_arrObjs;

	CArray<CFeature*,CFeature*> m_arrObjsByDisplayOrder;
	BOOL m_bDisplayOrderModified;

	int m_nID;
	CString m_strID;
	CString m_xattributeName;
	CString m_strName;
	CString m_strMapName;
	CString m_strGroupName;
	COLORREF  m_nColor;
	BOOL m_bLocked;
	BOOL m_bVisible;
	BOOL m_bDeleted;
	int	 m_nDisplayOrder;
	BOOL m_bIsInherent;

	BOOL m_bSymbolized;

	BOOL m_bHaveExtraAttr;

	//便于控制查询
	BOOL m_bAllowQuery;

	// 符号
	CPtrArray m_arrSymbols;
};

typedef CArray<CFtrLayer*,CFtrLayer*> CFtrLayerArray;

MyNameSpaceEnd

#endif // !defined(AFX_FTRLAYER_H__82C73E3D_D76D_40A4_BEFB_CFF3EE095E24__INCLUDED_)
