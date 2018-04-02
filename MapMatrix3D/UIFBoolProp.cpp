// UIFBoolProp.cpp: implementation of the CUIFBoolProp class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h "

#include "UIFBoolProp.h"
#include "DlgEditText.h"
#include "DlgCheckList.h"

#include "DlgUsedFonts.h"
#include "DlgSymbolizeFlag.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define  TEXT_MARGIN 4


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUIFBoolProp, CUIFProp)
CUIFBoolProp::CUIFBoolProp(const CString& strName, BOOL is, LPCTSTR lpszDescr, DWORD_PTR dwData) :
	CUIFPropEx (strName, _variant_t(), lpszDescr, dwData)
{
	m_varValue = is?VARIANT_TRUE:VARIANT_FALSE;
}

CUIFBoolProp::~CUIFBoolProp()
{
	
}

BOOL CUIFBoolProp::OnEdit(LPPOINT /*lptClick*/)
{
	if( !m_bAllowEdit )
		return FALSE;

 	ASSERT_VALID (this);
 	ASSERT_VALID (m_pWndList);

//	SetMultiValueState(FALSE,NULL);
	VARIANT_BOOL val = m_varValue.boolVal;
	val = val?VARIANT_FALSE:VARIANT_TRUE;
    m_varValue = val;
	m_varValue.vt=VT_BOOL;

	m_pWndList->RedrawWindow (m_Rect);	
	m_pWndList->OnPropertyChanged(this);
	return FALSE;
 
}
void CUIFBoolProp::OnDrawValue(CDC *pDC, CRect rect)
{
	CDC cdMem;
	m_pWndInPlace = NULL;
	CBitmap bitmap;
	BITMAP bm;
	if (m_varValue.boolVal == VARIANT_TRUE)
	{
        bitmap.LoadBitmap(IDB_BITMAP1);
		bitmap.GetBitmap(&bm);
	}
	else 
	{
		bitmap.LoadBitmap(IDB_BITMAP2);
		bitmap.GetBitmap(&bm);
	}

	CRect rectEdit = rect;
	rectEdit.left = m_pWndList->GetListRect().left + 
		m_pWndList->GetPropertyColumnWidth()  + 1;
	rectEdit.DeflateRect(1,1);
	cdMem.CreateCompatibleDC(pDC);
	CBitmap* oldBitmap = cdMem.SelectObject(&bitmap);
	pDC->StretchBlt(rectEdit.left,rectEdit.top+1,rectEdit.Height(),rectEdit.Height(),&cdMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	cdMem.SelectObject(oldBitmap);
}



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUIFButtonProp, CUIFProp)
CUIFButtonProp::CUIFButtonProp(const CString& strName, LPCTSTR lpszDescr, DWORD_PTR dwData) :
CUIFPropEx (strName, _variant_t(), lpszDescr, dwData)
{
}

CUIFButtonProp::~CUIFButtonProp()
{
	
	
}

BOOL CUIFButtonProp::OnEdit(LPPOINT /*lptClick*/)
{
//	SetMultiValueState(FALSE,NULL);
	m_pWndList->OnPropertyChanged(this);
	return FALSE;
	
}

void CUIFButtonProp::OnDrawName(CDC* pDC, CRect rect)
{

}

void CUIFButtonProp::OnDrawValue(CDC *pDC, CRect rect)
{
	CString strVal = m_strName;
	
	rect.DeflateRect (4, 0);
	
	pDC->DrawText (strVal, rect, 
		DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	
	pDC->Draw3dRect(&rect,0,RGB(128,128,128));	
}





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUIFFontNameProp, CUIFPropEx)


static BOOL FindStrInStrArr(const CStringArray *pArr,LPCSTR str)
{
	if(!pArr) return FALSE;
	for (int i=pArr->GetSize()-1;i>=0;i--)
	{
		if(strcmp(LPCSTR((*pArr)[i]),LPCSTR(str))==0)
			return TRUE;
	}
	if(i<0)
		return FALSE;
	return TRUE;
}


int CALLBACK EnumFontFamProc2(LPENUMLOGFONT lpelf,LPNEWTEXTMETRIC lpntm,DWORD nFontType,LONG_PTR lparam)
{
	
	CStringArray* pArr = (CStringArray*) lparam;
	if(!pArr) return 0;
    if (nFontType==TRUETYPE_FONTTYPE )
	{	
		if(!FindStrInStrArr(pArr,lpelf->elfLogFont.lfFaceName))
			pArr->Add(lpelf->elfLogFont.lfFaceName);
	}
	return 1;
}

void GetAllFontNames(CStringArray& arr)
{
	LOGFONT logfont;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName,"");
	HDC hDC = ::CreateCompatibleDC(NULL);
	
	::EnumFontFamiliesEx(hDC,&logfont,
		(FONTENUMPROC)EnumFontFamProc2,(LPARAM)&arr,0);
	::DeleteDC(hDC);
}


CUIFFontNameProp::CUIFFontNameProp(const CString& strName, const CString& varValue, 
	LPCTSTR lpszDescr, DWORD_PTR dwData)
			  :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
			  NULL,NULL,NULL)
{
	m_bClickOther = FALSE;
}

CUIFFontNameProp::~CUIFFontNameProp()
{
}


void CUIFFontNameProp::LoadFontNames()
{
	m_arrFont.RemoveAll();

	GetUsedTextStyles()->GetTextStyleNames(m_arrFont);

	m_arrFont.Add(StrFromResID(IDS_OTHERS_DOTDOT));

	for (int i=0;i<m_arrFont.GetSize();i++)
	{
		AddOption(m_arrFont[i]);
	}
}


BOOL CUIFFontNameProp::OnUpdateValue()
{
	return CUIFProp::OnUpdateValue();
}

void CUIFFontNameProp::OnSelectCombo ()
{
	if( m_pWndCombo )
	{
		int iSelIndex = m_pWndCombo->GetCurSel ();
		if (iSelIndex >= 0)
		{
			CString str;
			m_pWndCombo->GetLBText (iSelIndex, str);

			if( str.CompareNoCase(StrFromResID(IDS_OTHERS_DOTDOT))==0 )
			{
				m_bClickOther = TRUE;
				return;
			}
		}		
	}
	CUIFPropEx::OnSelectCombo();
}

void CUIFFontNameProp::OnCloseCombo()
{
	CUIFPropEx::OnCloseCombo();
	if (m_bClickOther)
	{
		CDlgUsedFonts dlg;
		if (dlg.DoModal() == IDOK && !dlg.m_strResult.IsEmpty())
		{
			RemoveAllOptions();
			LoadFontNames();
			SetValue((COleVariant)(dlg.m_strResult));
			m_pWndList->OnPropertyChanged(this);
		}
	}
	m_bClickOther = FALSE;
}

#define PROP_HAS_LIST	0x0001
#define PROP_HAS_BUTTON	0x0002
#define PROP_HAS_SPIN	0x0004


IMPLEMENT_DYNAMIC(CUIFMultiEditProp,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFMultiEditProp::CUIFMultiEditProp(const CString& strName, const CString& varValue, 
             LPCTSTR lpszDescr, DWORD_PTR dwData)
			  :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
			  NULL,NULL,NULL)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CUIFMultiEditProp::~CUIFMultiEditProp()
{

}


void CUIFMultiEditProp::OnClickButton(CPoint point)
{
	CDlgEditText dlg;
	dlg.m_strEdit = (LPCTSTR)(_bstr_t)GetValue();
	
	if( dlg.DoModal()!=IDOK )
		return;

//	SetMultiValueState(FALSE,NULL);

	m_varValue = (LPCTSTR)dlg.m_strEdit;
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText (dlg.m_strEdit);
	}
	Redraw ();
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		m_pWndList->SetFocus ();
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUIFArrowProp, CUIFPropEx)
CUIFArrowProp::CUIFArrowProp(const CString& strName, _variant_t value, LPCTSTR lpszDescr, DWORD_PTR dwData) :
CUIFPropEx(strName,value,lpszDescr,dwData)
{
}

CUIFArrowProp::~CUIFArrowProp()
{
	
	
}

BOOL CUIFArrowProp::OnEdit(LPPOINT lptClick)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);


	ArrowArea area = GetClickArea(lptClick);
	if (area == ClickLeft || area == ClickRight)
	{
		long index = (long)(_variant_t)m_varValue;
		if (area == ClickLeft)
		{
			index--;
		}
		else
		{
			index++;
		}

		m_varValue = index;
//		SetMultiValueState(FALSE,NULL);

		m_pWndList->RedrawWindow (m_Rect);	
		m_pWndList->OnPropertyChanged(this);

		return FALSE;
	}
	
	m_pWndInPlace = NULL;
	
	CRect rectEdit = m_Rect;
	rectEdit.DeflateRect (0, 2);
	rectEdit.left = m_pWndList->GetListRect().left + m_pWndList->GetLeftColumnWidth() + TEXT_MARGIN + 1;
	
	if (HasButton ())
	{
		m_rectButton = m_Rect;
		m_rectButton.left = m_rectButton.right - m_rectButton.Height () + 3;
		m_rectButton.top ++;
		
		rectEdit.right = m_rectButton.left;
	}

	rectEdit.right -= 32;
	
	BOOL bDefaultFormat = FALSE;
	m_pWndInPlace = CreateInPlaceEdit (rectEdit, bDefaultFormat);
	
	if (m_pWndInPlace != NULL)
	{
		if (bDefaultFormat)
		{
			m_pWndInPlace->SetWindowText (FormatProperty ());
		}
		
		if (m_dwFlags & PROP_HAS_LIST)
		{
			CRect rectCombo = m_Rect;
			rectCombo.left = rectEdit.left - 4;
			
			m_pWndCombo = CreateCombo (m_pWndList, rectCombo);
			ASSERT_VALID (m_pWndCombo);
			
			m_pWndCombo->SetFont (m_pWndList->GetFont ());
			
			//-----------------------------------------------------------------------
			// Synchronize bottom edge of the combobox with the property bottom edge:
			//-----------------------------------------------------------------------
			m_pWndCombo->GetWindowRect (rectCombo);
			m_pWndList->ScreenToClient (&rectCombo);
			
			int dy = rectCombo.Height () - m_Rect.Height ();
			
			m_pWndCombo->SetWindowPos (NULL, rectCombo.left,
				rectCombo.top - dy + 1, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			
			if (m_varValue.vt == VT_BOOL)
			{
				m_lstOptions.AddTail (/*m_pWndList->m_strTrue*/"Yes");
				m_lstOptions.AddTail (/*m_pWndList->m_strFalse*/"No");
			}
			
			for (POSITION pos = m_lstOptions.GetHeadPosition (); pos != NULL;)
			{
				m_pWndCombo->AddString (m_lstOptions.GetNext (pos));
			}
		}
		
		m_pWndInPlace->SetFont (m_pWndList->GetFont ());
		m_pWndInPlace->SetFocus ();
		
		if (!m_bAllowEdit)
		{
			m_pWndInPlace->HideCaret ();
		}
		
		m_bInPlaceEdit = TRUE;
		return TRUE;
	}
	
	return FALSE;
	
}

void CUIFArrowProp::OnDrawValue(CDC *pDC, CRect rect)
{
	CDC cdMem;
	CBitmap bitmap;
	BITMAP bm;
	bitmap.LoadBitmap(IDB_BITMAP_LRARROW);
	bitmap.GetBitmap(&bm);

	m_bitmapSize.cx = bm.bmWidth;
	m_bitmapSize.cy = bm.bmHeight;
	
	int left1 = m_pWndList->GetListRect().left - m_pWndList->GetPropertyColumnWidth()  + 1;
	int left2 = m_pWndList->GetListRect().right - bm.bmWidth;
	CRect rectEdit = rect;
	rectEdit.left = left1>left2?left1:left2;
	rectEdit.DeflateRect(1,0);
	cdMem.CreateCompatibleDC(pDC);
	CBitmap* oldBitmap = cdMem.SelectObject(&bitmap);
	pDC->StretchBlt(rectEdit.left,rectEdit.top+1,bm.bmWidth,rectEdit.Height(),&cdMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	cdMem.SelectObject(oldBitmap);

	CRect rectNum = rect;
	rectNum.left = m_pWndList->GetListRect().left + 
		m_pWndList->GetPropertyColumnWidth()  + 1;
	rectNum.DeflateRect(0,0,32,0);
	CUIFPropEx::OnDrawValue(pDC,rectNum);
	/*CString strVal = m_strName;
	
	rect.DeflateRect (TEXT_MARGIN, 0);
	
	pDC->DrawText (strVal, rect, 
		DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS);
	
	pDC->Draw3dRect(&rect,0,RGB(128,128,128));	*/
}

ArrowArea CUIFArrowProp::GetClickArea(LPPOINT lptClick) const
{
	if (lptClick == NULL) return ClickEdit;

	CPoint point = *lptClick;
	if (point.x < m_pWndList->GetListRect().right - m_bitmapSize.cx)
	{
		return ClickEdit;
	}
	else if (point.x < m_pWndList->GetListRect().right - m_bitmapSize.cx / 2)
	{
		return ClickLeft;
	}
	else if (point.x < m_pWndList->GetListRect().right)
	{
		return ClickRight;
	}

	return ClickEdit;
	
}

BOOL CUIFArrowProp::OnSetCursor () const
{
	CPoint point;
	
	::GetCursorPos (&point);
	ScreenToClient (m_pWndList->m_hWnd,&point);

	ArrowArea area = GetClickArea(&point);
	if (area == ClickLeft || area == ClickRight)
	{
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_ARROW));
		return TRUE;
	}

	switch (m_varValue.vt)
	{
	case VT_BSTR:
    case VT_R4:
    case VT_R8:
    case VT_I2:
	case VT_I4:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_IBEAM));
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CUIFNodeTypeProp, CUIFProp)
CUIFNodeTypeProp::CUIFNodeTypeProp(const CString& strName, short type, LPCTSTR lpszDescr, DWORD_PTR dwData) :
CUIFPropEx (strName, _variant_t(type), lpszDescr, dwData)
{
	m_varValue = type;
}
CUIFNodeTypeProp::~CUIFNodeTypeProp()
{
	
	
}

BOOL CUIFNodeTypeProp::OnEdit(LPPOINT /*lptClick*/)
{
	ASSERT_VALID (this);
	ASSERT_VALID (m_pWndList);
	
	short type = (short)(_variant_t)m_varValue;
	if (type == ptSpecial)
	{
		type = ptNone;
	}
	else
	{
		type = ptSpecial;
	}

    m_varValue = type;

//	SetMultiValueState(FALSE,NULL);

	m_pWndList->RedrawWindow (m_Rect);	
	m_pWndList->OnPropertyChanged(this);
	return FALSE;
	
}
void CUIFNodeTypeProp::OnDrawValue(CDC *pDC, CRect rect)
{
	CDC cdMem;
	m_pWndInPlace = NULL;
	CBitmap bitmap;
	BITMAP bm;
	if((short)(_variant_t)m_varValue==ptSpecial)
	{
        bitmap.LoadBitmap(IDB_BITMAP_SELSPECIALNODE);
		bitmap.GetBitmap(&bm);
	}
	else 
	{
		bitmap.LoadBitmap(IDB_BITMAP_UNSELSPECIALNODE);
		bitmap.GetBitmap(&bm);
	}
	
	CRect rectEdit = rect;
	rectEdit.left = m_pWndList->GetListRect().left + 
		m_pWndList->GetPropertyColumnWidth()  + 1;
	rectEdit.DeflateRect(1,1);
	cdMem.CreateCompatibleDC(pDC);
	CBitmap* oldBitmap = cdMem.SelectObject(&bitmap);
	pDC->StretchBlt(rectEdit.left,rectEdit.top+1,bm.bmWidth,rectEdit.Height(),&cdMem,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY);
	cdMem.SelectObject(oldBitmap);
}

BOOL CUIFNodeTypeProp::OnSetCursor () const
{
	CPoint point;
	
	::GetCursorPos (&point);
	ScreenToClient (m_pWndList->m_hWnd,&point);
	
	if (point.x > m_pWndList->GetListRect().left + m_pWndList->GetPropertyColumnWidth())
	{
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_ARROW));
		return TRUE;
	}
	
	switch (m_varValue.vt)
	{
	case VT_BSTR:
    case VT_R4:
    case VT_R8:
    case VT_I2:
	case VT_I4:
		SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_IBEAM));
		return TRUE;
	}
	return FALSE;
}


IMPLEMENT_DYNAMIC(CUIFCheckListProp,CUIFPropEx)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFCheckListProp::CUIFCheckListProp(const CString& strName, const CString& varValue, 
LPCTSTR lpszDescr, DWORD_PTR dwData)
									 :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
									 NULL,NULL,NULL)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CUIFCheckListProp::~CUIFCheckListProp()
{
	
}


void CUIFCheckListProp::SetList(CStringArray& arrNames, CArray<int,int>* arrFlags)
{
	m_arrNames.Copy(arrNames);
	if( arrFlags )
		m_arrFlags.Copy(*arrFlags);
	else
	{
		m_arrFlags.SetSize(arrNames.GetSize());
		memset(m_arrFlags.GetData(),0,sizeof(int)*m_arrFlags.GetSize());
	}

	UpdateTextValue();
}


void CUIFCheckListProp::SetValue(CArray<int,int>& arrFlags)
{
	if( m_arrFlags.GetSize()!=arrFlags.GetSize() )
		return;

	m_arrFlags.Copy(arrFlags);

	UpdateTextValue();
}


void CUIFCheckListProp::OptionsToCheckList()
{
	int nCount = m_lstOptions.GetCount();
	CStringArray arrNames;

	POSITION pos = m_lstOptions.GetHeadPosition();
	while( pos )
	{
		CString str = m_lstOptions.GetNext(pos);
		arrNames.Add(str);
	}

	RemoveAllOptions();

	m_dwFlags = PROP_HAS_BUTTON;

	SetList(arrNames,NULL);
}


void CUIFCheckListProp::UpdateTextValue()
{
	int nsz = m_arrFlags.GetSize();
	if( nsz<=0 )
		return;

	CString value;
	for( int i=0; i<nsz; i++)
	{
		if( m_arrFlags[i]==1 )
		{
			if( !value.IsEmpty() )
				value += ",";
			value += m_arrNames[i];
		}
	}
	
	m_varValue = (LPCTSTR)value;
}

void CUIFCheckListProp::OnClickButton(CPoint point)
{
	CDlgCheckList dlg;

	dlg.m_arrStrList.Copy(m_arrNames);
	dlg.m_arrFlags.Copy(m_arrFlags);
	
	if( dlg.DoModal()!=IDOK )
		return;

//	SetMultiValueState(FALSE,NULL);

	m_arrFlags.Copy(dlg.m_arrFlags);
	UpdateTextValue();

	Redraw ();
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText((LPCTSTR)(_bstr_t)m_varValue);
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		m_pWndList->SetFocus ();
	}
}


BOOL CUIFCheckListProp::StringFindItem(LPCTSTR t0, LPCTSTR t1, BOOL bJustLike)
{
	if( t0==NULL || t1==NULL )
		return TRUE;
	int len1 = strlen(t1);
	int len = strlen(t0);

	if( len==0 && len1==0 )
		return TRUE;

	if( len==0 )
		return FALSE;

	char *str = new char[len+1], *p0, *p1;
	strcpy(str,t0);

	p0 = str;

	BOOL bok = FALSE;

	do
	{
		p1 = strchr(p0,',');
		if( p1 )*p1 = 0;

		if( bJustLike )
		{
			if( strstr(t1,p0)!=NULL )
			{
				bok = TRUE;
				break;
			}
		}
		else if( stricmp(t1,p0)==0 )
		{
			bok = TRUE;
			break;
		}
		p0 = p1+1;

	}while( p1!=NULL );

	delete[] str;

	return bok;
}


BOOL CUIFCheckListProp::GetCheck(LPCTSTR value, LPCTSTR name)
{
	if( value==NULL || name==NULL )
		return FALSE;

	CString s1 = value, s2 = name;

	return StringFindItem(s1,s2);
}


IMPLEMENT_DYNAMIC(CUIFFilePropEx, CMFCPropertyGridFileProperty)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFFilePropEx::CUIFFilePropEx(const CString& strName, const CString& strFolderName, DWORD_PTR dwData, LPCTSTR lpszDescr)
:CMFCPropertyGridFileProperty(strName,strFolderName,dwData,lpszDescr)
{
}

CUIFFilePropEx::CUIFFilePropEx(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt,
	DWORD dwFlags, LPCTSTR lpszFilter, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CMFCPropertyGridFileProperty(strName,bOpenFileDialog,strFileName,lpszDefExt,dwFlags,lpszFilter,lpszDescr,dwData)
{
}

CUIFFilePropEx::~CUIFFilePropEx()
{
}

void CUIFFilePropEx::OnClickButton(CPoint /*point*/)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	m_bButtonIsDown = TRUE;
	Redraw();

	CString strPath = m_varValue;
	BOOL bUpdate = FALSE;

	if (m_bIsFolder)
	{
		if (afxShellManager == NULL)
		{
			CWinAppEx* pApp = DYNAMIC_DOWNCAST(CWinAppEx, AfxGetApp());
			if (pApp != NULL)
			{
				pApp->InitShellManager();
			}
		}

		if (afxShellManager == NULL)
		{
			ASSERT(FALSE);
		}
		else
		{
			bUpdate = afxShellManager->BrowseForFolder(strPath, m_pWndList, strPath, NULL, BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | 0x40);
		}
	}
	else
	{
		CFileDialog dlg(m_bOpenFileDialog, m_strDefExt, strPath, m_dwFileOpenFlags, m_strFilter, m_pWndList);
		if (dlg.DoModal() == IDOK)
		{
			bUpdate = TRUE;
			strPath = dlg.GetPathName();
		}
	}

	if (bUpdate)
	{
		if (m_pWndInPlace != NULL)
		{
			m_pWndInPlace->SetWindowText(strPath);
		}

		m_varValue = (LPCTSTR)strPath;
	}

	m_bButtonIsDown = FALSE;
	Redraw();

	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetFocus();
	}
	else
	{
		m_pWndList->SetFocus();
	}
}


IMPLEMENT_DYNAMIC(CUIFLongNamePropEx,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFLongNamePropEx::CUIFLongNamePropEx(const CString& strName, const CString& varValue, 
LPCTSTR lpszDescr, DWORD_PTR dwData)
									 :CUIFPropEx(strName,(_variant_t)(LPCTSTR)varValue,lpszDescr,dwData,
									 NULL,NULL,NULL)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CUIFLongNamePropEx::~CUIFLongNamePropEx()
{
	
}


void CUIFLongNamePropEx::OnClickButton(CPoint point)
{
	CMenu menu;

	if( (m_dwFlags&PROP_HAS_LIST) && m_lstOptions.GetCount()>0  )
	{
		CString val = (LPCTSTR)(_bstr_t)GetValue();
		menu.CreatePopupMenu();

		CStringArray names;
		POSITION pos = m_lstOptions.GetHeadPosition();
		while( pos!=NULL )
		{
			names.Add(m_lstOptions.GetNext(pos));
		}

		for( int i=0; i<names.GetSize(); i++)
		{
			menu.AppendMenu(MF_STRING,i+1,names[i]);
		}

		for( i=0; i<names.GetSize(); i++)
		{
			if( val.CompareNoCase(names[i])==0 )
				menu.CheckMenuRadioItem(1,names.GetSize(),i+1,MF_BYCOMMAND);
		}

		CPoint pt;
		::GetCursorPos(&pt);

		//m_pWndList->ScreenToClient(&pt);
		
		int cmd = menu.TrackPopupMenu(TPM_RIGHTALIGN|TPM_RETURNCMD,pt.x,pt.y,m_pWndList,NULL);

		if( cmd>0 && cmd<=m_lstOptions.GetCount() )
		{
//			SetMultiValueState(FALSE,NULL);

			if (m_pWndInPlace != NULL)
			{
				m_pWndInPlace->SetWindowText (names[cmd-1]);
			}

			m_varValue = (LPCTSTR)names[cmd-1];
			
			Redraw ();
			
			if (m_pWndInPlace != NULL)
			{
				m_pWndInPlace->SetFocus ();
			}
			else
			{
				m_pWndList->SetFocus ();
			}
		}
		
	}
}





IMPLEMENT_DYNAMIC(CUIFSymbolizeFlagProp,CUIFPropEx)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CUIFSymbolizeFlagProp::CUIFSymbolizeFlagProp(const CString& strName, long varValue, 
LPCTSTR lpszDescr, DWORD_PTR dwData)
									   :CUIFPropEx(strName,(_variant_t)varValue,lpszDescr,dwData,
									   NULL,NULL,NULL)
{
	m_dwFlags = PROP_HAS_BUTTON;
}

CUIFSymbolizeFlagProp::~CUIFSymbolizeFlagProp()
{
	
}


void CUIFSymbolizeFlagProp::OnClickButton(CPoint point)
{
	CDlgSymbolizeFlag dlg;

	dlg.SetSymbolizeFlag((long)(_variant_t)GetValue());

	if( dlg.DoModal()!=IDOK )
		return;

//	SetMultiValueState(FALSE,NULL);
	
	m_varValue = dlg.GetSymbolizeFlag();
	
	Redraw ();
	
	if (m_pWndInPlace != NULL)
	{
		m_pWndInPlace->SetWindowText((LPCTSTR)FormatProperty());
		m_pWndInPlace->SetFocus ();
	}
	else
	{
		m_pWndList->SetFocus ();
	}
}


CString CUIFSymbolizeFlagProp::FormatProperty()
{
	CDlgSymbolizeFlag dlg;
	
	dlg.SetSymbolizeFlag((long)(_variant_t)GetValue());

	return dlg.GetDisplayText();
}


BOOL CUIFSymbolizeFlagProp::OnUpdateValue ()
{
	if( m_pWndInPlace==NULL )
		return FALSE;

	if( !::IsWindow (m_pWndInPlace->GetSafeHwnd ()) )
		return FALSE;
	
	CString strText;
	m_pWndInPlace->GetWindowText (strText);
	
	BOOL bIsChanged = FormatProperty () != strText;

	if (bIsChanged)
	{
//		SetMultiValueState(FALSE,NULL);
		m_pWndList->OnPropertyChanged (this);
	}
	
	return TRUE;
}