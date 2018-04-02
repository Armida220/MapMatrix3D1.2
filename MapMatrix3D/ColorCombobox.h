// ColorCombobox.h: interface for the CColorCombobox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORCOMBOBOX_H__55231F21_65AC_4E0A_83E1_704003F37656__INCLUDED_)
#define AFX_COLORCOMBOBOX_H__55231F21_65AC_4E0A_83E1_704003F37656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

#define BLOCK_ZONE 1
#define GRADUAL_ZONE 2

struct ColorZone 
{
	std::vector<COLORREF> colors;
	CString id;
	int zone_type;
	void* pzonebmp;
};

class CColorComboBox  : public CComboBox
{
	// Construction
public:
	CColorComboBox();
	
	// Attributes
public:
	
	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorComboBox)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	void InitColor();
	void GetSelectColorZone(std::vector<COLORREF>& color_zone, int& type, CString& id);
	void SetCurSelByZoneid(CString zone_id);
	virtual ~CColorComboBox();	
	// Generated message map functions
protected:
	virtual void PreSubclassWindow();
	//{{AFX_MSG(CColorComboBox)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
private:
	void FindColorZoneFile() ;
	void GetColorZoneFromFile();
private:
	std::vector<CString> color_file_paths;
	std::vector<ColorZone> color_zones;
	int current_zone_index;
};

#endif // !defined(AFX_COLORCOMBOBOX_H__55231F21_65AC_4E0A_83E1_704003F37656__INCLUDED_)
