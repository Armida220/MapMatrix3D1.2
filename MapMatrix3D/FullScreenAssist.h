// FullScreenAssist.h: interface for the CFullScreenAssist class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FULLSCREENASSIST_H__6344B467_6C29_4BBB_B006_FA5777166850__INCLUDED_)
#define AFX_FULLSCREENASSIST_H__6344B467_6C29_4BBB_B006_FA5777166850__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFullScreenAssist  
{
public:
	void Attach(CFrameWnd *pWnd);
	CFullScreenAssist();
	virtual ~CFullScreenAssist();
	void OnFullscreen();
	void FullScreen(BOOL bFull);
	inline BOOL IsFullScreen(){ return m_bFullScreen; }
private:
	CFrameWnd *m_pFrame;
	BOOL m_bFullScreen;
	BOOL m_wndState[100];
	HMENU  m_hSaveMenu;
	CRect  m_oldMainRect;
};


struct CUSTOM_STATE
{
	int dataLen;
	BYTE data[4096];
};

/*
class CFramePlaceAssist
{
public:
	CFramePlaceAssist();
	virtual ~CFramePlaceAssist();
	void Attach(CFrameWnd *pWnd, const char *regPath, BOOL bPlaceReg=TRUE);
	void SavePlace();
	void LoadPlace();

private:
	CFrameWnd *m_pFrame;
	CString m_strPath;
	// 是否保存到注册表
	BOOL m_bPlaceReg;
	CUIFRegistry m_regDisk;
};*/

void LoadViewPlace(CView *pView, BOOL bFromReg=TRUE);

#endif // !defined(AFX_FULLSCREENASSIST_H__6344B467_6C29_4BBB_B006_FA5777166850__INCLUDED_)
