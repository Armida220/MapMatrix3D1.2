#include "stdafx.h"
#include "CommonCallStation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCommonCallStation gCallStation;

CCommonCallStation::CCommonCallStation()
{
}


CCommonCallStation::~CCommonCallStation()
{
	UnlinkAll();
}


BOOL CCommonCallStation::LinkCallback(UINT nMsg, HWND hwnd)
{
	if( hwnd==0 )return FALSE;

	CCommonCallStation::CALLBACK_ITEM* pItem = FindCallItem(nMsg);
	if( !pItem )
	{
		pItem = new CCommonCallStation::CALLBACK_ITEM;
		if( !pItem )return FALSE;

		pItem->nMsg = nMsg;
		pItem->arrCallLink.Add(hwnd);
		m_arrCallStack.Add(pItem);
	}
	else
	{
		pItem->arrCallLink.Add(hwnd);
	}

	return TRUE;
}


BOOL CCommonCallStation::UnlinkCallback(UINT nMsg, HWND hwnd)
{
	BOOL bRet = FALSE;
	for(int i=m_arrCallStack.GetSize()-1; i>=0; i--)
	{
		CCommonCallStation::CALLBACK_ITEM *pItem = (CCommonCallStation::CALLBACK_ITEM*)m_arrCallStack[i];
		if( !pItem )continue;

		if (nMsg != 0)
		{
			for( int j=pItem->arrCallLink.GetSize()-1; j>=0; j-- )
			{
				if( pItem->nMsg==nMsg && pItem->arrCallLink[j]==hwnd )
				{
					pItem->arrCallLink.RemoveAt(j);
					bRet = TRUE;
				}
			}
		}
		else
		{
			for( int j=pItem->arrCallLink.GetSize()-1; j>=0; j-- )
			{
				if( pItem->arrCallLink[j]==hwnd )
				{
					pItem->arrCallLink.RemoveAt(j);
					bRet = TRUE;
				}
			}
		}
	}
	
	return bRet;
}


void CCommonCallStation::UnlinkAll()
{
	for(int i=m_arrCallStack.GetSize()-1; i>=0; i--)
	{
		CCommonCallStation::CALLBACK_ITEM *pItem = (CCommonCallStation::CALLBACK_ITEM*)m_arrCallStack[i];
		if( pItem )delete pItem;
	}
	
	m_arrCallStack.RemoveAll();
	return;
}


BOOL CCommonCallStation::CallMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	UINT msg = nMsg;
	if (nMsg == WM_COMMAND)
	{
		msg = LOWORD(wParam);
	}

	CCommonCallStation::CALLBACK_ITEM* pItem = FindCallItem(msg);	
	if( !pItem )return FALSE;

	int num = pItem->arrCallLink.GetSize();
	if (num <= 0) return FALSE;
	HWND hwnd = pItem->arrCallLink.GetAt(num-1);

	LRESULT ret;
	if (nMsg == WM_COMMAND)
		ret = SendMessage(hwnd,WM_COMMAND,msg,lParam);	
	else
		ret = SendMessage(hwnd,msg,wParam,lParam);

	return ret;
}

CCommonCallStation::CALLBACK_ITEM* CCommonCallStation::FindCallItem(UINT nMsg)
{
	for(int i=m_arrCallStack.GetSize()-1; i>=0; i--)
	{
		CCommonCallStation::CALLBACK_ITEM *pItem = (CCommonCallStation::CALLBACK_ITEM*)m_arrCallStack[i];
		if( !pItem )continue;
		
		if( pItem->nMsg==nMsg )return pItem;
	}
	return NULL;
}


void CCommonCallStation::BringCallbackToTop(UINT nMsg, HWND hwnd)
{
	for(int i=m_arrCallStack.GetSize()-1; i>=0; i--)
	{
		CCommonCallStation::CALLBACK_ITEM *pItem = (CCommonCallStation::CALLBACK_ITEM*)m_arrCallStack[i];
		if( !pItem )continue;
		
		if( nMsg!=0 )
		{
			for( int j=pItem->arrCallLink.GetSize()-1; j>=0; j-- )
			{
				if( pItem->nMsg==nMsg && pItem->arrCallLink[j]==hwnd )
				{
					HWND link = pItem->arrCallLink.GetAt(j);
					pItem->arrCallLink.RemoveAt(j);
					pItem->arrCallLink.Add(link);
				}
			}
		}
		else
		{
			for( int j=pItem->arrCallLink.GetSize()-1; j>=0; j-- )
			{
				if( pItem->arrCallLink[j]==hwnd )
				{
					HWND link = pItem->arrCallLink.GetAt(j);
					pItem->arrCallLink.RemoveAt(j);
					pItem->arrCallLink.Add(link);
				}
			}
		}
	}

}
