// DummyDialog.h: interface for the CDummyDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUMMYDIALOG_H__03ABFC85_1E9A_4531_B442_E6D7C6B28D04__INCLUDED_)
#define AFX_DUMMYDIALOG_H__03ABFC85_1E9A_4531_B442_E6D7C6B28D04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_THREADMSG			(WM_USER+1)


typedef LRESULT (CObject::*PFUNCALLBACK)(WPARAM wParam, LPARAM lParam);

class EXPORT_SMARTVIEW CDummyDialog : public CDialog
{
public:
	CDummyDialog(CObject *pObj,PFUNCALLBACK func){
		m_pObj = pObj;
		m_pFunc = func;
	}
	virtual ~CDummyDialog(){
	}
	
	LRESULT OnThreadMsg(WPARAM wParam, LPARAM lParam)
	{
		if( m_pObj && m_pFunc )
			return (m_pObj->*m_pFunc)(wParam,lParam);
		return 1;
	}

	BOOL Create2(HINSTANCE hInst, UINT nIDTemplate, CWnd* pParentWnd = NULL);

	DECLARE_MESSAGE_MAP()
		
private:	
	CObject *m_pObj;
	PFUNCALLBACK m_pFunc;
};

#endif // !defined(AFX_DUMMYDIALOG_H__03ABFC85_1E9A_4531_B442_E6D7C6B28D04__INCLUDED_)
