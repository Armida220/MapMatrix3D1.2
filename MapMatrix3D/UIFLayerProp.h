// UIFLayerProp.h: interface for the CUIFLayerProp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIFLAYERPROP_H__46344B80_35CF_4CE4_A75F_1BCA00899748__INCLUDED_)
#define AFX_UIFLAYERPROP_H__46344B80_35CF_4CE4_A75F_1BCA00899748__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UIFPropEx.h"


class CDlgDataSource;
class CUIFLayerProp : public CUIFPropEx  
{
	DECLARE_DYNAMIC(CUIFLayerProp)

	friend class CMFCPropertyGridCtrl;
public:
	
	void SetLayersOption( CDlgDataSource *pDS);
	CUIFLayerProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFLayerProp();
	void EnableMutiSelMode(BOOL bMutiSel);
	void EnableNotEmpty(BOOL bEnable);
	void EnableLocalLayers(BOOL bEnable);
	void EnableNotLocalLayers(BOOL bEnable);

	void EnablePoint(BOOL bEnable);
	void EnableLine(BOOL bEnable);
	void EnableArea(BOOL bEnable);
	void EnableText(BOOL bEnable);

	virtual BOOL OnUpdateValue();
	virtual void OnClickButton(CPoint point);
protected:
	CDlgDataSource *m_pDS;
	BOOL m_bMutiSel;
	BOOL m_bNotEmpty;
	BOOL m_bLocalLayers;
	BOOL m_bNotLocalLayers;

	BOOL m_bPoint;
	BOOL m_bLine;
	BOOL m_bArea;
	BOOL m_bText;
};



class CUIFMultiColorProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFMultiColorProp)
		
public:
	CUIFMultiColorProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFMultiColorProp();
	
	virtual void OnClickButton(CPoint point);

	CArray<COLORREF,COLORREF> m_colors;
};



class CUIFCellLinetypeProp : public CUIFPropEx
{
	DECLARE_DYNAMIC(CUIFCellLinetypeProp)
		
	friend class CMFCPropertyGridCtrl;
public:
	
	void SetSymbolClass( int nScale, int nSymType);
	CUIFCellLinetypeProp(const CString& strName, const CString& varValue, 
		LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0);
	virtual ~CUIFCellLinetypeProp();
	virtual void OnClickButton(CPoint point);

protected:
	int m_nScale, m_nSymType;
};

#endif // !defined(AFX_UIFLAYERPROP_H__46344B80_35CF_4CE4_A75F_1BCA00899748__INCLUDED_)
