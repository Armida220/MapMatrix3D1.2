// UIFBoolProp.h: interface for the CUIFBoolProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFBOOLPROP_H__79E1FE3E_3C65_4A91_849D_2E6C681F5DD9__INCLUDED_)
#define AFX_UIFBOOLPROP_H__79E1FE3E_3C65_4A91_849D_2E6C681F5DD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIFPropEx.h"

class CUIFBoolProp : public CUIFPropEx  
{
	friend class CMFCPropertyGridCtrl;
	DECLARE_DYNAMIC(CUIFBoolProp)

public:
	void OnDrawValue (CDC* pDC, CRect rect);
	CUIFBoolProp(const CString& strName, BOOL is, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFBoolProp();
	BOOL OnEdit(LPPOINT lptClick);
private:

/*	BOOL m_bIs;*/
};


class CUIFButtonProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFButtonProp)
		
public:
	void OnDrawName(CDC* pDC, CRect rect);
	void OnDrawValue (CDC* pDC, CRect rect);
	CUIFButtonProp(const CString& strName, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFButtonProp();
	BOOL OnEdit(LPPOINT lptClick);
};


class CUIFMultiEditProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFMultiEditProp)
		
public:
	CUIFMultiEditProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFMultiEditProp();

	virtual void OnClickButton(CPoint point);
};



class CUIFFontNameProp : public CUIFPropEx  
{
	DECLARE_DYNAMIC(CUIFFontNameProp)

public:	
	CUIFFontNameProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFFontNameProp();
	virtual BOOL OnUpdateValue();
	virtual void OnSelectCombo();
	virtual void OnCloseCombo();
	void LoadFontNames();

protected:
	CStringArray m_arrFont;
	BOOL m_bClickOther;
};

typedef enum ArrowArea
{
	ClickEdit  = 0,
	ClickLeft  = 1,
	ClickRight = 2
} ArrowArea ;

class CUIFArrowProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFArrowProp)
	friend class CMFCPropertyGridCtrl;
public:
	void OnDrawValue (CDC* pDC, CRect rect);
	CUIFArrowProp(const CString& strName, _variant_t value, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFArrowProp();
	BOOL OnEdit(LPPOINT lptClick);
	ArrowArea GetClickArea(LPPOINT lptClick) const;
	virtual BOOL OnSetCursor () const;
protected:
	CSize  m_bitmapSize;
};

class CUIFNodeTypeProp : public CUIFPropEx  
{
	friend class CMFCPropertyGridCtrl;
	DECLARE_DYNAMIC(CUIFNodeTypeProp)
		
public:
	void OnDrawValue (CDC* pDC, CRect rect);
	CUIFNodeTypeProp(const CString& strName, short type, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFNodeTypeProp();
	BOOL OnEdit(LPPOINT lptClick);
	virtual BOOL OnSetCursor () const;
private:
	
};

class CUIFCheckListProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFCheckListProp)
		
public:	
	CUIFCheckListProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFCheckListProp();

	void SetList(CStringArray& arrNames, CArray<int,int>* arrFlags);
	void SetValue(CArray<int,int>& arrFlags);
	void OptionsToCheckList();
	
	virtual void OnClickButton(CPoint point);

	static BOOL StringFindItem(LPCTSTR t0, LPCTSTR t1, BOOL bJustLike=FALSE);
	static BOOL GetCheck(LPCTSTR value, LPCTSTR name);
	
protected:
	void UpdateTextValue();

	CStringArray m_arrNames;
	CArray<int,int> m_arrFlags;
};

class CUIFFilePropEx : public CMFCPropertyGridFileProperty
{
	DECLARE_DYNAMIC(CUIFFilePropEx)
public:
	CUIFFilePropEx(const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = NULL);
	CUIFFilePropEx(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFFilePropEx();

public:
	virtual void OnClickButton(CPoint);
};

class CUIFLongNamePropEx : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFLongNamePropEx)
		
public:
	CUIFLongNamePropEx(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFLongNamePropEx();
	
	virtual void OnClickButton(CPoint point);
};


class CUIFSymbolizeFlagProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFSymbolizeFlagProp)
		
public:
	CUIFSymbolizeFlagProp(const CString& strName, long varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFSymbolizeFlagProp();
	
	virtual CString FormatProperty();
	virtual void OnClickButton(CPoint point);
	virtual BOOL OnUpdateValue();
};


#endif // !defined(AFX_UIFBOOLPROP_H__79E1FE3E_3C65_4A91_849D_2E6C681F5DD9__INCLUDED_)
