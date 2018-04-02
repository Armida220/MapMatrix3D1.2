// UIFPropListEx.cpp: implementation of the CUIFPropListEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "UIFPropListEx.h"
#include "exMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CUIFPropListEx, CMFCPropertyGridCtrl)

BEGIN_MESSAGE_MAP(CUIFPropListEx, CMFCPropertyGridCtrl)
//{{AFX_MSG_MAP(CUIFPropListEx)
ON_WM_KEYDOWN()
ON_WM_CHAR()
ON_WM_DESTROY()
//}}AFX_MSG_MAP
ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFPropListEx::CUIFPropListEx()
{

}

CUIFPropListEx::~CUIFPropListEx()
{
	RemoveAllHideProps();
}


BOOL CUIFPropListEx::HidePropertyByData(DWORD_PTR data, BOOL bRedraw)
{
	POSITION pos = m_lstProps.GetHeadPosition(), old;
	CUIFProp *pProp = NULL;
	while( pos )
	{
		old = pos;
		pProp = m_lstProps.GetNext(pos);
		if( pProp && pProp->GetData()==data )break;
		pProp = NULL;
	}

	if( !old || !pProp )return FALSE;

	m_lstProps.RemoveAt(old);

	CUIFPropEx *pPropEx = DYNAMIC_DOWNCAST(CUIFPropEx, pProp);
	if (pPropEx)pPropEx->SetOwnerList_Public(this);
	m_lstHideProps.AddTail(pProp);

	AdjustLayout();

	if( bRedraw && GetSafeHwnd()!=NULL )
	{
		RedrawWindow(&pProp->GetRect());
	}

	return TRUE;
}



BOOL CUIFPropListEx::ShowPropByDataToIndex(DWORD_PTR data, int idx, BOOL bRedraw)
{
	int count = m_lstProps.GetCount();
	
	POSITION pos = m_lstHideProps.GetHeadPosition(), old=NULL;
	CUIFProp *pProp = NULL;
	while( pos )
	{
		old = pos;
		pProp = m_lstHideProps.GetNext(pos);
		if( pProp && pProp->GetData()==data )break;
		pProp = NULL;
	}
	
	if( !old || !pProp )return FALSE;
	
	m_lstHideProps.RemoveAt(old);

	CUIFPropEx *pPropEx = DYNAMIC_DOWNCAST(CUIFPropEx, pProp);
	if (pPropEx)pPropEx->SetOwnerList_Public(this);
	
	if( idx>=count )
		m_lstProps.AddTail(pProp);
	else
		m_lstProps.InsertBefore(m_lstProps.FindIndex(idx<0?0:idx),pProp);
	
	AdjustLayout();
	
	if( bRedraw && GetSafeHwnd()!=NULL )
	{
		RedrawWindow(&pProp->GetRect());
	}

	return TRUE;
}


BOOL CUIFPropListEx::HidePropertyByName(LPCTSTR name, BOOL bRedraw)
{
	POSITION pos = m_lstProps.GetHeadPosition(), old;
	CUIFProp *pProp = NULL;
	while( pos )
	{
		old = pos;
		pProp = m_lstProps.GetNext(pos);
		if( pProp && strcmp(pProp->GetName(),name)==0 )break;
		pProp = NULL;
	}
	
	if( !old || !pProp )return FALSE;
	
	m_lstProps.RemoveAt(old);
	CUIFPropEx *pPropEx = DYNAMIC_DOWNCAST(CUIFPropEx, pProp);
	if (pPropEx)pPropEx->SetOwnerList_Public(this);
	m_lstHideProps.AddTail(pProp);
	
	AdjustLayout();
	
	if( bRedraw && GetSafeHwnd()!=NULL )
	{
		RedrawWindow(&pProp->GetRect());
	}
	
	return TRUE;
}



BOOL CUIFPropListEx::ShowPropByNameToIndex(LPCTSTR name, int idx, BOOL bRedraw)
{
	int count = m_lstProps.GetCount();
	
	POSITION pos = m_lstHideProps.GetHeadPosition(), old=NULL;
	CUIFProp *pProp = NULL;
	while( pos )
	{
		old = pos;
		pProp = m_lstHideProps.GetNext(pos);
		if( pProp && strcmp(pProp->GetName(),name)==0 )break;
		pProp = NULL;
	}
	
	if( !old || !pProp )return FALSE;
	
	m_lstHideProps.RemoveAt(old);
	
	CUIFPropEx *pPropEx = DYNAMIC_DOWNCAST(CUIFPropEx, pProp);
	if (pPropEx)pPropEx->SetOwnerList_Public(this);
	
	if( idx>=count )
		m_lstProps.AddTail(pProp);
	else
		m_lstProps.InsertBefore(m_lstProps.FindIndex(idx<0?0:idx),pProp);
	
	AdjustLayout();
	
	if( bRedraw && GetSafeHwnd()!=NULL )
	{
		RedrawWindow(&pProp->GetRect());
	}
	
	return TRUE;
}

void CUIFPropListEx::RemoveAllHideProps()
{
	CUIFProp *pProp = NULL;
	while( !m_lstHideProps.IsEmpty() )
	{
		pProp = m_lstHideProps.RemoveHead ();
		if( pProp )
		{
			delete pProp;
		}
	}
}


void CUIFPropListEx::AddCanHideProperty(CUIFProp* pProp, BOOL bHide)
{
	if( !bHide )
		AddProperty(pProp);
	else
	{
		CUIFPropEx *pPropEx = DYNAMIC_DOWNCAST(CUIFPropEx, pProp);
		if (pPropEx)
		{
			pPropEx->SetOwnerList_Public(this);
		}
		else
		{
			AddProperty(pProp);
			return;
		}

		m_lstHideProps.AddTail(pProp);
	}
}


void CUIFPropListEx::InsertProperty(CUIFProp* pProp,int idx, BOOL bRedraw)
{
	int count = m_lstProps.GetCount();
	if( idx<0 || idx>m_lstProps.GetCount() )
	{
		return;
	}
	
	CUIFPropEx *pPropEx = DYNAMIC_DOWNCAST(CUIFPropEx, pProp);
	if (pPropEx)pPropEx->SetOwnerList_Public(this);

	if( idx==count )
		m_lstProps.AddTail(pProp);
	else
		m_lstProps.InsertBefore(m_lstProps.FindIndex(idx),pProp);
	AdjustLayout();
	
	if (bRedraw && GetSafeHwnd() != NULL)
	{
		RedrawWindow(&pProp->GetRect());
	}
}


void CUIFPropListEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	MSG msg = *GetCurrentMessage();
	if (msg.wParam == VK_ESCAPE)
	{
		AfxGetMainWnd()->SendMessage(FCCM_TRANSLATEMSG,msg.wParam,(LPARAM)&msg);
	}
}


void CUIFPropListEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void CUIFPropListEx::OnDestroy()
{
	CMFCPropertyGridCtrl::OnDestroy();
	RemoveAllHideProps();
}


BOOL CUIFPropListEx::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message==WM_KEYDOWN )
	{
		if( m_pSel!=NULL && m_pSel->IsInPlaceEditing() && m_pSel->IsEnabled() && m_pSel->IsAllowEdit() )
		{
			if (!CMFCPropertyGridCtrl::PreTranslateMessage(pMsg))
			{
				return PreTranslateInput(pMsg);
			}

			if (pMsg->wParam == VK_ESCAPE)
			{
				AfxGetMainWnd()->SendMessage(FCCM_TRANSLATEMSG,pMsg->wParam,(LPARAM)pMsg);
			}

			return TRUE;
		}
	}

	return CMFCPropertyGridCtrl::PreTranslateMessage(pMsg);
}

CUIFProp* CUIFPropListEx::FindProperty(LPCTSTR name, CUIFProp *pProp)
{
	if (!name || !pProp) return NULL;

	CUIFProp *pFindProp = NULL;

	if (!pProp->IsGroup())
	{
		if (stricmp(pProp->GetName(),name) == 0)
		{
			pFindProp = pProp;
		}
	}
	else
	{
		for (int i=0; i<pProp->GetSubItemsCount(); i++)
		{
			CUIFProp *pSubProp = pProp->GetSubItem(i);
			CUIFProp *tmp = FindProperty(name,pSubProp);
			if (tmp)
			{
				pFindProp = tmp;
				break;
			}
		}
	}

	return pFindProp;
			
}

CUIFProp* CUIFPropListEx::FindProperty(LPCTSTR name)
{
	if (!name) return NULL;

	CUIFProp *pFindProp = NULL;
	
	POSITION pos = m_lstProps.GetHeadPosition();
	CUIFProp *pProp = NULL;
	while( pos )
	{
		pProp = m_lstProps.GetNext(pos);
		CUIFProp *tmp = FindProperty(name,pProp);
		if (tmp)
		{
			pFindProp = tmp;
			break;
		}
	}
	
	return pFindProp;
}

int	CUIFPropListEx::FindProperty(DWORD_PTR data)
{
	int idx = -1;
	
	POSITION pos = m_lstProps.GetHeadPosition();
	CUIFProp *pProp = NULL;
	while( pos )
	{
		idx++;
		pProp = m_lstProps.GetNext(pos);
		if( pProp && pProp->GetData()==data )
			break;
	}

	return idx;
}

void CUIFPropListEx::SaveCurValue()
{
	CUIFProp *pProp = GetCurSel();
	if( pProp && m_pSel->IsInPlaceEditing() && m_pSel->IsEnabled() && m_pSel->IsAllowEdit() )
		pProp->OnUpdateValue();
}

void CUIFPropListEx::UpdateColor (COLORREF color)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pSel);
	
	CMFCPropertyGridColorProperty* pColorProp = DYNAMIC_DOWNCAST(CMFCPropertyGridColorProperty, m_pSel);
	if (pColorProp == NULL)
	{
		ASSERT(FALSE);
		return;
	}

//	pColorProp->SetMultiValueState(FALSE,NULL);
	
	BOOL bChanged = color != pColorProp->GetColor ();
	pColorProp->SetColor (color);
	
//	if (bChanged)
	{
		OnPropertyChanged (pColorProp);
	}
	
	pColorProp->OnUpdateValue ();
}


int CUIFPropListEx::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPropertyGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	COLORREF clrBackground = RGB(50, 50, 50);
	COLORREF clrText = RGB(255, 255, 255);
	COLORREF clrGrpBackgrd = RGB(50, 50, 50);
	COLORREF clrGrpText = RGB(255, 255, 255);
	COLORREF clrDesBackgrd = RGB(50, 50, 50);
	COLORREF clrDescriptionText = RGB(255, 255, 255);
	COLORREF clrLine = RGB(50, 50, 50);
	
	SetCustomColors(clrBackground, clrText, clrGrpBackgrd, clrGrpText, clrDesBackgrd, clrDescriptionText, clrLine);
	
	return 0;
}
