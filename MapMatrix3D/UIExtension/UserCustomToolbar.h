// UserCustomToolbar.h: interface for the CUserCustomToolbar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERCUSTOMTOOLBAR_H__B7C2D5F6_0297_4D92_A898_2F1F797DB933__INCLUDED_)
#define AFX_USERCUSTOMTOOLBAR_H__B7C2D5F6_0297_4D92_A898_2F1F797DB933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CUserCustomToolbar : public CMFCToolBar  
{
public:
	struct BtnParam
	{
		BtnParam()
		{
			id=0;
			ZeroMemory(name, 256);
			ZeroMemory(key, 256);
			ZeroMemory(cmd, 256);
		}
		int id;
		TCHAR name[256];
		char key[256];
		char cmd[256];
	};
	CUserCustomToolbar();
	virtual ~CUserCustomToolbar();
	static void LoadFromPulgins(CWnd *pParentWnd, const CArray<PlugItem,PlugItem> &arrItem, CPtrArray &arr);
//	static void LoadFromConfig(CWnd *pParentWnd, LPCTSTR cfgPath, CPtrArray &arr);
	CString GetCmdString(int nId);
	HACCEL	GetAccelerator();
	int GetButtonsCount();
	UINT GetButtonsID(int idx);
	void CreateButtons(int num, const CUserCustomToolbar::BtnParam *params);
	void CreateToolbar(CWnd *pParentWnd);
	virtual BOOL LoadState (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);
	virtual BOOL SaveState (LPCTSTR lpszProfileName = NULL, int nIndex = -1, UINT uiID = (UINT) -1);

protected:	
	virtual BOOL OnUserToolTip(CMFCToolBarButton* pButton, CString& strTTText) const;

	BOOL LoadBitmap(LPCTSTR bmpFile);
	BOOL LoadBitmap(UINT uiResID,HINSTANCE hinstRes);
protected:
	CArray<BtnParam,BtnParam> m_arrBtnParams;
	
	HBITMAP m_hToolbarBmp;
	
	HACCEL m_hAccel;
DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_USERCUSTOMTOOLBAR_H__B7C2D5F6_0297_4D92_A898_2F1F797DB933__INCLUDED_)
