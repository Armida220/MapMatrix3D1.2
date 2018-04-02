#if !defined(AFX_DLGREFDATAFILE_H__849E364D_152D_4905_B0A6_8C6DD9003471__INCLUDED_)
#define AFX_DLGREFDATAFILE_H__849E364D_152D_4905_B0A6_8C6DD9003471__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRefDataFile.h : header file
//
#include "PropList0.h"
#include "editbasedoc.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgRefDataFile dialog
class CDlgDoc;
class CDlgRefDataFile : public CDialog
{
	// Construction
public:
	CDlgRefDataFile(CWnd* pParent = NULL);   // standard constructor
	
	// Dialog Data
	//{{AFX_DATA(CDlgRefDataFile)
	enum { IDD = IDD_REFDATAFILE_MANAGER };
	CButton	m_btnDel;
	//}}AFX_DATA
	CLVLPropList0	m_wndPropListCtrl;
	CArray<dataParam,dataParam&> m_arrDataParams;

	void ModifyDataSourceAttri(CLVLPropItem0 *pItem, CLVLPropColumn0* Pc ,_variant_t exData);
	void OnSelChange();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRefDataFile)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
public:
	void AddRefDataFile()
	{
       OnButtonNew();
	   OnOK();
	}
	void AddRefDataFileUVS()
	{
		OnButtonNewDB();
		OnOK();
	}
	void DelRefDataFile(CString file_name)
	{
       for(int i=0; i<m_wndPropListCtrl.GetItemCount(); ++i)
	   {
            CLVLPropItem0* pitem = m_wndPropListCtrl.GetPropItem(i);
			//
			CString item_title = (LPCTSTR)(_bstr_t)pitem->GetValue(0);
			if(file_name == item_title)
			{
				m_wndPropListCtrl.SelectItem(pitem);
				break;
			}
	   }
	   //
	   OnButtonDel();
	   OnOK();
	}
	void ActiveRefDataFile(CString file_name)
	{
        int old_active_index = -1;
		int new_active_index = -1;
		for(int i=0; i<m_wndPropListCtrl.GetItemCount(); ++i)
		{
            CLVLPropItem0* pitem = m_wndPropListCtrl.GetPropItem(i);
			//
			if((bool)pitem->GetValue(1))
			{
				old_active_index = i;
			}
			//
			CString item_title = (LPCTSTR)(_bstr_t)pitem->GetValue(0);
			if(file_name == item_title)
			{
				/*if (pitem->GetData()==0)
				{
					AfxMessageBox(IDS_UVS_NO_ACTIVE);
					return;
				}*/
				new_active_index = i;
			}
		}
		//
		if(old_active_index==-1 || new_active_index == -1)
			return;
		//
		CLVLPropItem0* polditem = m_wndPropListCtrl.GetPropItem(old_active_index);
		polditem->SetValue((_variant_t)(bool)false,1);
		CLVLPropItem0* pnewitem = m_wndPropListCtrl.GetPropItem(new_active_index);
		pnewitem->SetValue((_variant_t)(bool)true,1);
		//
		OnOK();		
	}

protected:
	
	// Generated message map functions
	//{{AFX_MSG(CDlgRefDataFile)
	afx_msg void OnButtonNew();
	afx_msg void OnButtonNewDB();
	afx_msg void OnButtonDel();
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:	
	CDlgDoc*		m_pDlgDoc;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGREFDATAFILE_H__849E364D_152D_4905_B0A6_8C6DD9003471__INCLUDED_)
