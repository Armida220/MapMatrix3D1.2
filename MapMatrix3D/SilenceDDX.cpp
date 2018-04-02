// DlgWorkSpaceBound.cpp : implementation file
//

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void DDX_Text_Silence(CDataExchange* pDX, int nIDC, double& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atof(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}

void DDX_Text_Silence(CDataExchange* pDX, int nIDC, float& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atof(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}


void DDX_Text_Silence(CDataExchange* pDX, int nIDC, int& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atoi(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}


void DDX_Text_Silence(CDataExchange* pDX, int nIDC, UINT& value)
{
	if( pDX->m_bSaveAndValidate )
	{
		HWND hCtrl = ::GetDlgItem(pDX->m_pDlgWnd->GetSafeHwnd(),nIDC);
		CWnd *pWnd = CWnd::FromHandle(hCtrl);
		if( pWnd )
		{
			CString text;
			pWnd->GetWindowText(text);
			value = atoi(text);
		}
	}
	else
	{
		DDX_Text(pDX,nIDC,value);
	}
}
