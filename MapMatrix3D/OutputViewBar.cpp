// OutputViewBar.cpp: implementation of the COutputViewBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "OutputViewBar.h"
#include "SmartViewFunctions.h"
#include "EditBaseDoc.h"
#include "DlgInputCommand.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IDC_LIST_NODENAME              51515

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// COutputEdit

COutputEdit::COutputEdit()
{
	
}

COutputEdit::~COutputEdit()
{
}


BEGIN_MESSAGE_MAP(COutputEdit, CEdit)
	//{{AFX_MSG_MAP(COutputEdit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OUTPUT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_OUTPUT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_COPY, OnUpdateEditCopy)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputEdit message handlers

void COutputEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu menu;
    menu.LoadMenu(IDR_POPUP_OUTPUT);

    CMenu* pSumMenu = menu.GetSubMenu(0);

    if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
    {
	   CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

	   if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
         return;

	   ((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
      UpdateDialogControls(this, FALSE);
    }
}

void COutputEdit::OnEditClear() 
{
	SetSel(0, -1);
	Clear();
}

void COutputEdit::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

void COutputEdit::OnEditCopy() 
{
	// TODO: Add your command handler code here
	Copy();
}

void COutputEdit::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	int nStart,nEnd;
	GetSel( nStart,nEnd );
	pCmdUI->Enable( nEnd>nStart );
}


void COutputEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	const MSG *pMsg = GetCurrentMessage();
	GetParent()->SendMessage(WM_KEYONE,pMsg->wParam,pMsg->lParam);
}

void COutputEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (GetKeyState(VK_CONTROL) < 0)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		const MSG *pMsg = GetCurrentMessage();
		GetParent()->SendMessage(WM_CHARONE,pMsg->wParam,pMsg->lParam);
	}	
}

void COutputEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
}

BOOL COutputEdit::IsEmpty()
{
	int nEnd,nLine;
	nLine = GetLineCount();
	nEnd = LineIndex(nLine-1);
	nEnd += LineLength(nEnd);
	return (nEnd==0);
}


void COutputEdit::Output_ReplaceLastLine(LPCTSTR strText)
{
	if( !strText || strlen(strText)<=0)
		return;
	int num = GetLimitText();
	SetSel(0, -1);
	DWORD dwSel = GetSel();
	
	if((HIWORD(dwSel)-LOWORD(dwSel))>=num/2)
	{
		SetSel(0,num/2);
		Clear();
	}
	int nLine;
	nLine = GetLineCount();
	int nEnd0 = LineIndex(nLine-1);
	int nEnd1 = nEnd0 + LineLength(nEnd0);
	
	SetSel(nEnd0, nEnd1);
	ReplaceSel(strText);
}

void COutputEdit::Output(LPCTSTR strText)
{
	if( !strText || strlen(strText)<=0)
		return;
	int num = GetLimitText();
	SetSel(0, -1);
	DWORD dwSel = GetSel();

	if((HIWORD(dwSel)-LOWORD(dwSel))>=num/2)
	{
		SetSel(0,num/2);
		Clear();
	}
	int nEnd,nLine;
	nLine = GetLineCount();
	nEnd = LineIndex(nLine-1);
	nEnd += LineLength(nEnd);
	
	SetSel(nEnd, nEnd);
	ReplaceSel(strText);
}



/////////////////////////////////////////////////////////////////////////////
// CInputEdit

CInputEdit::CInputEdit()
{
	m_bLButtonDown = FALSE;
	m_nHistPos = 0;
	m_nInputType = InputNone;
}

CInputEdit::~CInputEdit()
{
}


BEGIN_MESSAGE_MAP(CInputEdit, CEdit)
	//{{AFX_MSG_MAP(CInputEdit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OUTPUT_CLEAR, OnEditClearall)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_CLEAR, OnUpdateEditClearall)
	ON_COMMAND(ID_OUTPUT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_OUTPUT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_OUTPUT_PASTE, OnUpdateEditPaste)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputEdit message handlers

void CInputEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	CMenu menu;
    menu.LoadMenu(IDR_POPUP_OUTPUT);
	
    CMenu* pSumMenu = menu.GetSubMenu(0);
	
    if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
    {
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;
		
		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;
		
		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu (pPopupMenu);
		UpdateDialogControls(this, FALSE);
    }
}

void CInputEdit::OnEditClearall() 
{
	SetText(OUTSTR_NULL);
}

void CInputEdit::OnUpdateEditClearall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(GetSel()!=NULL)
	{
		pCmdUI->Enable(TRUE);
	}
	else pCmdUI->Enable(false);
}

void CInputEdit::OnEditCopy() 
{
	// TODO: Add your command handler code here
	Copy();
}

void CInputEdit::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	int nStart,nEnd;
	GetSel( nStart,nEnd );
	pCmdUI->Enable( nEnd>nStart );
}


void CInputEdit::OnEditPaste() 
{
	// TODO: Add your command handler code here
	Paste();
}

void CInputEdit::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	OpenClipboard();
	
	HANDLE hMem=::GetClipboardData(CF_TEXT);
	char *pBuf = (char*)::GlobalLock(hMem);
	pCmdUI->Enable(pBuf!=NULL);
	::GlobalUnlock(hMem);

	CloseClipboard();
}


void CInputEdit::SetText(LPCTSTR strText)
{
	SetSel(m_strPrompt.GetLength(),-1);
	ReplaceSel(strText, FALSE);
}

CString CInputEdit::GetText()
{
	int i, nLineCount = GetLineCount();
	CString strText;
	TCHAR strLine[1024];
	
	for( i=0; i<nLineCount; i++)
	{
		int nlen = LineLength(i);
		GetLine(i, strLine, 1024);
		strLine[nlen] = '\0';
		
		strText += strLine;

		if( i==0 )
			strText.Delete(0,min(strText.GetLength(),m_strPrompt.GetLength()));
		else 
			strText += OUTSTR_RETURN;
	}

	return strText;
}

CString CInputEdit::GetPrompt()
{
	return m_strPrompt;
}

void CInputEdit::SetLastPrompt()
{
	SetPrompt(m_strLastPrompt);
}

void CInputEdit::SetPrompt(LPCTSTR strText)
{
	m_strLastPrompt = m_strPrompt;

	CString str(strText);
// 	if (str.GetLength()>100)
// 	{		
// 		str = str.Left(99);
// 		str += _T("...");
// 	}

	CString text = GetText();
	m_strPrompt = str;

	m_strPrompt.Remove(_T('\n'));
	m_strPrompt.Remove(_T('\r'));

	text = m_strPrompt + text;
	SetSel(0,-1);
	ReplaceSel(text, FALSE);
}


void CInputEdit::Return()
{
	CString text = GetText();	
	if( m_strHistory.GetSize()>=100 )
		m_strHistory.RemoveAt(0);
	m_strHistory.Add(text);
	m_nHistPos = m_strHistory.GetSize();

	GetParent()->SendMessage(WM_KEYINSTRING,0,(LPARAM)text.GetBuffer(0));
	text.ReleaseBuffer();
	SetText(OUTSTR_NULL);
}


void CInputEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) 
	{
	case VK_SPACE:
	case VK_RETURN:
		m_nInputType = inputReturn;
		Return();
		return;
	case VK_BACK:
		{
			m_nInputType = InputBack;

			int nStart, nEnd;
			GetSel(nStart,nEnd);
			if( nStart==nEnd && (nStart<=m_strPrompt.GetLength()||nEnd<=m_strPrompt.GetLength()) )
				return;
		}
		break;
	default:
		m_nInputType = InputChar;
	}
	CEdit::OnChar(nChar,nRepCnt,nFlags);
}

void CInputEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_DELETE:
		{
			int nStart, nEnd;
			GetSel(nStart,nEnd);
			if( nStart>=m_strPrompt.GetLength() && nEnd>=m_strPrompt.GetLength() )
				CEdit::OnKeyDown(nChar,nRepCnt,nFlags);
		}		
		break;
	case VK_UP:
		m_nHistPos--;
		if( m_nHistPos<0 )m_nHistPos = 0;
		else if( m_nHistPos<m_strHistory.GetSize() )
		{
			SetText(m_strHistory.GetAt(m_nHistPos));
		}
		break;
	case VK_DOWN:
		m_nHistPos++;
		if( m_nHistPos>=m_strHistory.GetSize() )
		{
			m_nHistPos = m_strHistory.GetSize();
			SetText(OUTSTR_NULL);
		}
		else if( m_nHistPos>=0 )
		{
			SetText(m_strHistory.GetAt(m_nHistPos));
		}
		break;
	case VK_LEFT:
		int nStart, nEnd;
		GetSel(nStart,nEnd);
		if( nStart>m_strPrompt.GetLength() && nEnd>m_strPrompt.GetLength() )
			CEdit::OnKeyDown(nChar,nRepCnt,nFlags);
		break;
	case VK_RIGHT:
		CEdit::OnKeyDown(nChar,nRepCnt,nFlags);
		break;
	default:;
	}	
}


void CInputEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( m_bLButtonDown )return;
	
	SetFocus();

	int index = CharFromPos(point);
	if( index<m_strPrompt.GetLength() )
		index = m_strPrompt.GetLength();

	SetSel(index,index);
	m_bLButtonDown = TRUE;
	m_ptLButtonDown = point;
}

void CInputEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !m_bLButtonDown )return;

	SetFocus();

	int index0 = CharFromPos(m_ptLButtonDown);
	if( index0<m_strPrompt.GetLength() )
		index0 = m_strPrompt.GetLength();

	int index1 = CharFromPos(point);
	if( index1<m_strPrompt.GetLength() )
		index1 = m_strPrompt.GetLength();

	SetSel(index0,index1);

	m_bLButtonDown = FALSE;
}

void CInputEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( (nFlags&MK_LBUTTON) && m_bLButtonDown )
	{
		int index0 = CharFromPos(m_ptLButtonDown);
// 		if( index0<m_strPrompt.GetLength() )
// 			index0 = m_strPrompt.GetLength();
		
		int index1 = CharFromPos(point);
// 		if( index1<m_strPrompt.GetLength() )
// 			index1 = m_strPrompt.GetLength();
// 		
		SetSel(index0,index1);
	}
}

void CInputEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int index0 = m_strPrompt.GetLength();
	int index1 = index0 + GetText().GetLength();

	SetSel(index0,index1);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COutputViewBar::COutputViewBar()
{
	m_pParam = NULL;
	m_nInputState = inputNone;
	m_bChgFromSet = FALSE;
	m_pWndSearchRsltList = NULL;
	m_bChangePrompt = FALSE;
}

COutputViewBar::~COutputViewBar()
{
	if( m_pParam )delete m_pParam;
	if (m_pWndSearchRsltList) 
	{
		delete m_pWndSearchRsltList;
		m_pWndSearchRsltList = NULL;
	}
}



BEGIN_MESSAGE_MAP(COutputViewBar, CDockablePane)
	//{{AFX_MSG_MAP(COutputViewBar)
	ON_EN_CHANGE(IDC_OUTBAR_INPUTEDIT, OnChangeEditFcode)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_NCPAINT()
	ON_WM_MOVE()
	ON_WM_MOVING()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_CHARONE,OnCharOne)
	ON_MESSAGE(WM_KEYONE,OnKeyOne)
	ON_MESSAGE(WM_KEYINSTRING,OnKeyinString)
	ON_MESSAGE(WM_COMMANDSELCHANGE,OnSearchListSelChange)
END_MESSAGE_MAP()

#include "MainFrm.h"
int COutputViewBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pWndSearchRsltList = new CDlgInputCommand();
	if (m_pWndSearchRsltList == NULL)
	{
		return -1;
	}

	if (!m_pWndSearchRsltList->Create(IDD_INPUTCOMMAND, this))
	{
		return -1;
	}

	LOGFONT logfont;
	AfxGetMainWnd()->GetFont()->GetLogFont(&logfont);
	m_Font.CreateFontIndirect(&logfont);
	CRect rectClient (0, 0, lpCreateStruct->cx, lpCreateStruct->cy-20);
	
	m_wndOutEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP,
		CRect(0, 0,1,1), 
		this, 1);
	m_wndOutEdit.SetFont(&m_Font);	
	m_wndOutEdit.ModifyStyleEx(0, WS_EX_CLIENTEDGE, 0);
	m_wndOutEdit.SetMargins(4,4);
	
	m_wndInEdit.Create(ES_MULTILINE | ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
		CRect(0, 0,1,1), 
		this, IDC_OUTBAR_INPUTEDIT);

	m_wndInEdit.SetFont(&m_Font);

	m_wndInEdit.SetMargins(4,4);

	m_pWndSearchRsltList->SetParent(AfxGetMainWnd());
	m_pWndSearchRsltList->SetProcessWnd(this);
	m_pWndSearchRsltList->ShowWindow(SW_HIDE);

// 	if(!m_pWndSearchRsltList->Create(WS_CHILD|WS_VISIBLE|LBS_NOTIFY|WS_VSCROLL|WS_BORDER|WS_HSCROLL, CRect(0, 0,1,1), gpMainWnd->GetDesktopWindow(), IDC_LIST_NODENAME))
// 	{
// 		TRACE0("Failed to Create List\n");
// 		return -1;
// 	}
// 	
// 	m_pWndSearchRsltList->ShowWindow(SW_HIDE);
// 
// 	m_pWndSearchRsltList->SetFont(&m_Font,TRUE);

	m_keyfilepath = GetConfigPath(TRUE) + "\\CustomizeKey.txt";
	LoadCustomizeKey();

	return 0;	
}


void COutputViewBar::OnSize(UINT nType, int cx, int cy) 
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout ();
}



void COutputViewBar::AdjustLayout ()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	
	CRect rectClient,rect;
	GetClientRect (rectClient);
	
	m_wndOutEdit.SetWindowPos (NULL,
		rectClient.left, 
		rectClient.top, 
		rectClient.Width (),
		rectClient.Height () - 20,
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_wndInEdit.SetWindowPos (NULL,
		rectClient.left, 
		rectClient.top + rectClient.Height () - 20, 
		rectClient.Width (),
		20,
		SWP_NOACTIVATE | SWP_NOZORDER);

	ClientToScreen(&rectClient);

	if (m_pWndSearchRsltList)
	{
		m_pWndSearchRsltList->SetWindowPos(NULL, rectClient.left + 30, rectClient.top + rectClient.Height() - 420, 300, 400, SWP_NOACTIVATE);
		m_pWndSearchRsltList->AdjustSize();
	}
}

void COutputViewBar::OnChangeEditFcode()
{
	if (m_bChgFromSet )  return;

	CString strInput;
	wchar_t input[256];
	strInput = m_wndInEdit.GetText();
	
	int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;	
	wstring wstr;
	ConvertCharToWstring((LPCTSTR)strInput,wstr,codepage);			
	wcscpy(input,(wchar_t*)wstr.c_str());
	
	int nInLen = wcslen(input);
	if( nInLen<=0 )
	{
		m_pWndSearchRsltList->ResetContent();
		m_pWndSearchRsltList->ShowWindow(SW_HIDE);
		return;
	}	
	
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )return;
	
	CString strInput0 = strInput;
	strInput0.MakeLower();
	
	BOOL bAllUsed = (strInput0.CompareNoCase("?")==0);
	if( !bAllUsed && m_pParam )
	{
		return;

		//如果快捷键有效，就不弹出命令提示
		for( int i=0; i<m_pParam->m_arrItems.GetSize(); i++)
		{
			int c = m_pParam->m_arrItems[i].accelKey;
			if( (c>='a' && c<='z') || (c>='A' && c<='Z') )
				break;
		}
		if( i<m_pParam->m_arrItems.GetSize() )
			return;
	}

	m_pWndSearchRsltList->ShowWindow(SW_SHOWNOACTIVATE);
	m_pWndSearchRsltList->ResetContent();

    SortItemArray sortArr;
    //自定义快捷命令也加入到列表
	int i=0, j=0;
    for( i=0; i<m_arrkeys.GetSize(); i++)
    {
        CString cskey = m_arrkeys[i].Cskey;
        cskey.MakeLower();
        
        int find = cskey.Find(strInput0);
        int id = GetCustomizeKeyCommand(m_arrkeys[i].Accel);
        
        if( find>=0 && id>0)
        {
            SortItem temp;
            temp.text.Format("%s (%s) ----%s", m_arrkeys[i].Name, m_arrkeys[i].Accel, m_arrkeys[i].Cskey);
            temp.cmdid = id;
            temp.sortStr = cskey.Right(cskey.GetLength()-find);
            temp.findIndex = find;
            sortArr.Add(temp);	
        }
	}

	int nreg ;
	const CMDREG *pRegs = pDoc->GetCmdRegs(nreg);

	for( i=0; i<nreg; i++)
	{
        BOOL bExist = FALSE;
        for(j=0; j<sortArr.GetSize(); j++)
        {
            if(sortArr[j].cmdid == pRegs[i].id)
            {
                bExist = TRUE;
                break;
            }
        }
        if(bExist) continue;

		CString accel = pRegs[i].strAccel, name = pRegs[i].strName;
		accel.MakeLower();
		name.MakeLower();

		int find1 = accel.Find(strInput0);
		int find2 = name.Find(strInput0);

		if( bAllUsed ||  find1>= 0 ||  find2>= 0)
		{
			SortItem temp;
			temp.text.Format("%s (%s)", pRegs[i].strName, pRegs[i].strAccel);
			temp.cmdid = pRegs[i].id;
            for(j=0; j<m_arrkeys.GetSize(); j++)
            {
                if(m_arrkeys[j].Accel == pRegs[i].strAccel)
                {
                    temp.text = temp.text + " ----" + m_arrkeys[j].Cskey;
					break;
                }
            }
			if(bAllUsed)
			{
				temp.sortStr = accel;
				temp.findIndex = -1;
			}
			if(find1 >= 0)
			{
				temp.sortStr = accel.Right(accel.GetLength()-find1);
				temp.findIndex = find1;
			}
			if(find2 >= 0)
			{
				temp.sortStr = name.Right(name.GetLength()-find2);
				temp.findIndex = find2;
			}

			sortArr.Add(temp);	
		}
	}
	SortSearchRsltList(sortArr);
	for(i=0; i<sortArr.GetSize(); i++)
	{
		int index = m_pWndSearchRsltList->AddString(sortArr[i].text);
		m_pWndSearchRsltList->SetItemData(index,sortArr[i].cmdid);
	}

	m_pWndSearchRsltList->AdjustSize();
	if (m_pWndSearchRsltList->GetCount() > 0)
	{
		m_pWndSearchRsltList->SetCurSel(0);
		
		if (m_wndInEdit.GetInputType() == CInputEdit::InputChar)
		{
			m_bChgFromSet = TRUE;
			
			CString strPrompt = m_wndInEdit.GetPrompt();
			int startSel = strInput.GetLength() + strPrompt.GetLength();
// 			m_wndInEdit.SetText(fstAccel);
// 			m_wndInEdit.SetSel(startSel,-1);
			
			m_bChgFromSet = FALSE;
		}
		
	}
	

}


BOOL COutputViewBar::SendCmdKeyin(LPCTSTR str)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( pDoc )
	{
		CCommand *pCurCmd = pDoc->GetCurrentCommand();
		if(pCurCmd )
		{
			return pDoc->SendCmdKeyin(str);
		}
	}

	return FALSE;
}

BOOL COutputViewBar::Keyin(LPCTSTR str, int flag)
{
	CString text = m_wndInEdit.GetPrompt();	
	
	if( m_pParam )
	{
		CUIParam::ParamItem item;
		CValueTable tab;
		tab.BeginAddValueItem();
		if( MatchParam(str,item,tab) )
		{
			tab.EndAddValueItem();
			if( !m_wndOutEdit.IsEmpty() )
				m_wndOutEdit.Output(OUTSTR_RETURN);
			m_wndOutEdit.Output(text+str);
			
			UpdateParam(tab);
			return TRUE;
		}
		else
		{
			CString text2 = m_wndInEdit.GetText();
			if( text2.GetLength()<=0 )
			{
				if( (BYTE)str[0]=='/' ) // "."键
				{
					m_bChangePrompt = !m_bChangePrompt;
					if(m_bChangePrompt)
						ShowParamsWithoutKey(m_pParam);
					else
						ShowKeyParams(m_pParam);
					return TRUE;
				}
			}
		}

		//由于尚未完善，暂不启用
		if (0 && flag == 0)
		{
			m_wndInEdit.SetFocus();
			char *c = (LPTSTR)(LPCTSTR)(str);
			m_wndInEdit.SendMessage(WM_CHAR,WPARAM(*c),0);

			CUIParam::ParamItem item;
			CValueTable tab;
			InputState nInputState;
			if( MatchAccelKeyParam(str,item,tab,&nInputState) )
			{
				if (nInputState == inputKeyEnough)
				{
					if( !m_wndOutEdit.IsEmpty() )
						m_wndOutEdit.Output(OUTSTR_RETURN);
					m_wndOutEdit.Output(text+str);
					
					UpdateParam(tab);

					m_wndInEdit.SetText(OUTSTR_NULL);
				}
			}
		}
		// Enter
		else if (flag == 1)
		{
			CString strInput = m_wndInEdit.GetText();
			if( SendCmdKeyin(strInput) )
			{
				if( !m_wndOutEdit.IsEmpty() )
					m_wndOutEdit.Output(OUTSTR_RETURN);
				m_wndOutEdit.Output(text+strInput);
			}
			return TRUE;
			// 获取输入数据
			if (m_nInputState == inputKeyGetValue)
			{
				m_wndInEdit.SetLastPrompt();

				CValueTable tab;
				tab.BeginAddValueItem();
				if (MatchOneParam(m_CurParamItem,str,tab))
				{
					tab.EndAddValueItem();
					if( !m_wndOutEdit.IsEmpty() )
						m_wndOutEdit.Output(OUTSTR_RETURN);
					m_wndOutEdit.Output(text+str);
					
					UpdateParam(tab);
				}

				m_nInputState = inputNone;
			}
			else
			{
				CUIParam::ParamItem item;
				CValueTable tab;
				if( MatchAccelKeyParam(str,item,tab,&m_nInputState) )
				{
					if (m_nInputState == inputKeyEnough)
					{
						if( !m_wndOutEdit.IsEmpty() )
							m_wndOutEdit.Output(OUTSTR_RETURN);
						m_wndOutEdit.Output(text+str);
						
						UpdateParam(tab);
						
						m_nInputState = inputNone;
					}
					// 提示输入
					else if (m_nInputState == inputKeyRequestValue)
					{
						CString strText;
						strText += item.title;
						strText += _T(": ");
						m_wndInEdit.SetPrompt(strText);
						
						m_CurParamItem = item;

						m_nInputState = inputKeyGetValue;
					}
				}
				// 同时改变多个属性
				else
				{
					CUIParam::ParamItem item0;
					CValueTable tab0;
					tab0.BeginAddValueItem();

					BOOL bProcessed = FALSE;
					CString strValues(str); 
					// 第一个字符;为标志
					//if (strValues.Find(';',0) == 0)
					{
						int nIndex = 0;
						while (nIndex < strValues.GetLength())
						{
							int nfirst = nIndex;
							nIndex = strValues.Find(';', nfirst);
							if (nIndex < 0)
							{
								nIndex = strValues.GetLength();
							}
							
							CString strKeyValue = strValues.Mid(nfirst,nIndex-nfirst);
							strKeyValue.Replace(" ",NULL);
							strKeyValue.Replace("="," ");

							char str0[256], str1[1024];
							InputState nInputState;
							if (sscanf((LPTSTR)(LPCTSTR)strKeyValue,"%s %s",str0,str1) == 2 && MatchAccelKeyParam(str0,item0,tab0,&nInputState))
							{
								if (m_nInputState == inputKeyEnough || nInputState == inputKeyRequestValue)
								{	
									if (nInputState == inputKeyRequestValue)
									{
										MatchOneParam(item0,str1,tab0);
									}
									bProcessed = TRUE;
								}
							}

							nIndex++;
							
						}
					}

					tab0.EndAddValueItem();

					if (bProcessed)
					{
						if( !m_wndOutEdit.IsEmpty() )
							m_wndOutEdit.Output(OUTSTR_RETURN);
						m_wndOutEdit.Output(text+str);

						UpdateParam(tab0);
					}
				}
			}
		}

	}
	
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( !pDoc )return FALSE;
	
	int nreg ;
	const CMDREG *pRegs = pDoc->GetCmdRegs(nreg);
	for( int i=0; i<nreg; i++)
	{
		if( strnicmp(pRegs[i].strAccel,str,strlen(pRegs[i].strAccel))==0 )
			break;
	}
	
	if( i<nreg )
	{
		AfxGetMainWnd()->PostMessage(WM_COMMAND,pRegs[i].id,0);
		return TRUE;
	}
	else
	{
		if (stricmp(str,"group") == 0)
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_TOOL_FTRSORGANIZE);
			return TRUE;
		}
		else
		{
		}
	}

	return FALSE;
}


void COutputViewBar::LoadParams(const CUIParam *param)
{
}


void COutputViewBar::ShowParamsWithoutKey(const CUIParam *param)
{
	if( !m_pParam )m_pParam = new CUIParam;
	if( !m_pParam )return;
	
	if( m_pParam!=param )
		m_pParam->CopyFrom(param);

	CString strText;

	int nsz = param->m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CUIParam::ParamItem item = param->m_arrItems.GetAt(i);
		if (!item.bShow)
			continue;

		if( item.type==CUIParam::HotKeyType )
			continue;

		if( item.type==CUIParam::NoneType )
			continue;

		if( item.type==CUIParam::PointType )
			continue;

		if( !strText.IsEmpty() )strText += "; ";
		strText += ToText(item);
	}

	if( strText.IsEmpty() )
		strText = StrFromResID(IDS_OUTPUT_CMDPROMPT);

	strText += _T(": ");
	m_wndInEdit.SetPrompt(strText);
}


void COutputViewBar::ShowKeyParams(const CUIParam *param)
{
	if( !m_pParam )m_pParam = new CUIParam;
	if( !m_pParam )return;
	
	if( m_pParam!=param )
	{
		m_pParam->CopyFrom(param);
		m_bChangePrompt = FALSE;
	}
	
	CString strText;
	
	int nsz = param->m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CUIParam::ParamItem item = param->m_arrItems.GetAt(i);

		// AccelKey
		CString accelKey = ParamAccelKeyToText(item);
		if (!accelKey.IsEmpty())
		{
			if( !strText.IsEmpty() )strText += "; ";
			strText += accelKey;
		}

		if( item.type!=CUIParam::HotKeyType )
			continue;

		// HotKey
		if( !strText.IsEmpty() )strText += "; ";
		strText += ToText(item);
	}
	
	if( strText.IsEmpty() )
		strText = StrFromResID(IDS_OUTPUT_CMDPROMPT);
	
	strText += _T(": ");
	m_wndInEdit.SetPrompt(strText);
}

CString COutputViewBar::ParamAccelKeyToText(const CUIParam::ParamItem& item)
{
	char key = item.accelKey;
	if( (key>='a' && key<='z') || (key>='A' && key<='Z') )
	{
		CString strText, strData;
		strText += item.title;

		strData.Format("[%c]",key);
		strText += strData;
		return strText;
	}

	return CString();
	
}

CString COutputViewBar::ToText(const CUIParam::ParamItem& item)
{
	CString strText, strData;
	strText += item.title;	

	switch(item.type) 
	{
	case CUIParam::NoneType:
		break;
	case CUIParam::IntType:
		strData.Format("%d",item.data.nValue);
		break;
	case CUIParam::BoolType:
		strData += _T(" * ");
		strData += StrFromResID(item.data.bValue?IDS_YES:IDS_NO);
		//strData += _T("(Y/N)");
		break;
	case CUIParam::FloatType:
		strData.Format("%.4f",item.data.fValue);
		strData.TrimRight(_T('0'));
		break;
	case CUIParam::DoubleType:
		strData.Format("%.6f",item.data.lfValue);
		strData.TrimRight(_T('0'));
		break;
	case CUIParam::StringType:
		strData = *item.data.pStrValue;
		break;
	case CUIParam::OptionType:
		{
			int nsz = item.data.pOptions->GetSize();
			if(nsz>0)
			{
				bool isAccelExist = FALSE;
				CUIParam::Option opt;
				opt = item.data.pOptions->GetAt(0);
				if (opt.accelkey!=' ')
				{	
					isAccelExist = TRUE;					
				}
				if(isAccelExist)
				{
					for( int i=0; i<nsz; i++)
					{
						
						opt = item.data.pOptions->GetAt(i);					
						
						if( i!=0 )strData += _T('/');
						if( opt.flag==1 )strData += _T(" * ");
						strData += opt.desc;
						strData += _T('(');
						strData += (char)opt.accelkey;
						strData += _T(')');
					}
				}
				else
				{
					for( int i=0; i<nsz; i++)
					{
						
						opt = item.data.pOptions->GetAt(i);							
						if( opt.flag==1 )
						{
							strData += _T(" * ");
							strData += opt.desc;
						}					
					}
				}

			}
			
		}
		break;
	case CUIParam::PointType:
		break;
	case CUIParam::HotKeyType:
		if(item.data.chHotKey=='\r')
			strData = "[Enter]";
		else
			strData.Format("[%c]",item.data.chHotKey);
		return strText+strData;

		break;
	default:;
	}

	if( !strData.IsEmpty() )
	{
		strText += _T(":");
		strText += strData;
	}

	return strText;
}


void COutputViewBar::ClearParams()
{
	if( m_pParam )delete m_pParam;
	m_pParam = NULL;

	CString strText = StrFromResID(IDS_OUTPUT_CMDPROMPT);
	
	strText += _T(": ");
	m_wndInEdit.SetText("");
	m_wndInEdit.SetPrompt(strText);
}


void COutputViewBar::OutputMsg(LPCTSTR msg, BOOL bReplaceLastLine)
{
	if( bReplaceLastLine )
	{
		if( !m_wndOutEdit.IsEmpty() )
			m_wndOutEdit.Output_ReplaceLastLine(msg);
		else
			m_wndOutEdit.Output(msg);
	}
	else
	{
		if( !m_wndOutEdit.IsEmpty() )
			m_wndOutEdit.Output(OUTSTR_RETURN);
		m_wndOutEdit.Output(msg);
	}
}


void COutputViewBar::UpdateParam(CValueTable& tab)
{
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if( pDoc )
	{
		pDoc->SetCurCmdParams(tab);
	}
}

BOOL COutputViewBar::MatchParam(LPCTSTR input, CUIParam::ParamItem& item, CValueTable& tab)
{
	if( !m_pParam || input==NULL || strlen(input)<=0 )return FALSE;

	int nsz = m_pParam->m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CUIParam::ParamItem it = m_pParam->m_arrItems.GetAt(i);
		if( MatchOneParam(it,input,tab) )
		{
			item = it;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL COutputViewBar::MatchAccelKeyParam(LPCTSTR input, CUIParam::ParamItem& item, CValueTable& tab, InputState *state)
{
	if( !m_pParam || input==NULL )
	{
		if (state)
		{
			*state = inputNone;
		}
		return FALSE;
	}
	
	if (strlen(input) != 1)
	{
		if (state)
		{
			*state = inputNeedMoreData;
		}
		return FALSE;
	}

	char key = input[0];
	_strlwr(&key);

	BOOL bRet = FALSE;

	int nsz = m_pParam->m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CUIParam::ParamItem it = m_pParam->m_arrItems.GetAt(i);

		char key1 = it.accelKey;
		_strlwr(&key1);

		if (key == key1)
		{
			item = it;
			bRet = TRUE;
			if (it.type == CUIParam::BoolType)
			{
				_variant_t ret = !it.data.bValue;
				tab.BeginAddValueItem();
				tab.AddValue(item.field,&CVariantEx(ret));
				tab.EndAddValueItem();

				if (state)
				{
					*state = inputKeyEnough;
				}
			}
			else
			{
				if (state)
				{
					*state = inputKeyRequestValue;
				}
			}
			
			break;
		}

	}
	return bRet;
}

BOOL COutputViewBar::MatchOneParam(const CUIParam::ParamItem& item, LPCTSTR input, CValueTable& tab)
{
	BOOL bRet = FALSE;
	char line[1024], *stop;
	strncpy(line,input,sizeof(line)-1);
	_strlwr(line);

	_variant_t ret;
	//tab.BeginAddValueItem();
	
	switch(item.type) 
	{
		/*
	case CUIParam::NoneType:
		break;
	case CUIParam::IntType:
		ret = strtol(line,&stop,10);
		if( strlen(stop)==0 )bRet = TRUE;
		if( bRet )tab.AddValue(item.field,&CVariantEx(ret));
		break;
	case CUIParam::BoolType:
		if( strlen(line)==1 )
		{
			bRet = FALSE;
		}
		if( bRet )tab.AddValue(item.field,&CVariantEx(ret));
		break;
	case CUIParam::FloatType:
		ret = (float)strtod(line,&stop);
		if( strlen(stop)==0 )bRet = TRUE;
		if( bRet )tab.AddValue(item.field,&CVariantEx(ret));
		break;
	case CUIParam::DoubleType:
		ret = (double)strtod(line,&stop);
		if( strlen(stop)==0 )bRet = TRUE;
		if( bRet )tab.AddValue(item.field,&CVariantEx(ret));
		break;
	case CUIParam::StringType:
		ret = line;
		bRet = TRUE;
		tab.AddValue(item.field,&CVariantEx(ret));
		break;
		*/
	case CUIParam::PointType:
		{
			PT_3D point;
			COPY_3DPT(point,item.data.point);
			char *buf = line;
			point.x = strtod(buf,&stop);
			if( strlen(stop)>1 && stop[0]==',' )
			{
				buf = stop+1;
				point.y = strtod(buf,&stop);
				if( stop>buf )
				{
					bRet = TRUE;
					if( strlen(stop)>1 && stop[0]==',' )
					{
						buf = stop+1;
						double z = strtod(buf,&stop);
						if( stop>buf && strlen(stop)==0 )
						{
							point.z = z;
						}
					}
				}
			}
			if( bRet )
			{
				PT_3DEX expt;
				COPY_3DPT(expt,point);
				expt.pencode = penNone;
				CArray<PT_3DEX,PT_3DEX> arrPts;
				arrPts.Add(expt);

				CVariantEx t;
				t.SetAsShape(arrPts);
				tab.AddValue(item.field,&t);
			}
		}
		break;
	case CUIParam::OptionType:
		if( strlen(line)==1 )
		{
			CString str;
			int nsz = item.data.pOptions->GetSize();
			for( int i=0; i<nsz; i++)
			{
				str = (char)item.data.pOptions->GetAt(i).accelkey;
				if( str.CompareNoCase(line)==0 )
				{
					bRet = TRUE;
					ret = (_variant_t)(long)item.data.pOptions->GetAt(i).value;
					break;
				}
			}
		}
		if( bRet )tab.AddValue(item.field,&CVariantEx(ret));
		break;
	case CUIParam::HotKeyType:
		if( strlen(line)==1 )
		{		
			CString str(item.data.chHotKey);
			if( str.CompareNoCase(line)==0 )
			{
				bRet = TRUE;
				ret = (_variant_t)(long)item.data.chHotKey;				
			}			
		}
		if( bRet )tab.AddValue(item.field,&CVariantEx(ret));
		break;
	default:;
	}

	//tab.EndAddValueItem();

	return bRet;
}


BOOL COutputViewBar::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message==WM_KEYDOWN )
	{
		if (pMsg->wParam== VK_DOWN)
		{
			int nIndex = m_pWndSearchRsltList->GetCurSel();
			m_pWndSearchRsltList->SetCurSel(nIndex+1);
			
			return TRUE;
		}
		else if(pMsg->wParam==VK_UP)
		{
			int nIndex = m_pWndSearchRsltList->GetCurSel();
			m_pWndSearchRsltList->SetCurSel(nIndex-1);
			
			return TRUE;
		}
		else if( pMsg->wParam==VK_ESCAPE )
		{
			HWND hFocus = ::GetFocus();

			if( m_wndInEdit.GetText().IsEmpty() )
			{
				CDlgDoc *pDoc = GetActiveDlgDoc();
				if( pDoc )
				{
					pDoc->CancelCurrentCommand();
					pDoc->StartDefaultSelect();
				}
			}
			else
			{
				m_wndInEdit.SetText("");
				
				if (m_pWndSearchRsltList)
				{
					m_pWndSearchRsltList->ResetContent();
					m_pWndSearchRsltList->ShowWindow(SW_HIDE);
				}
			}
			return TRUE;
		}
		else if( pMsg->wParam==VK_RETURN || pMsg->wParam==VK_SPACE )
		{
			if (m_pWndSearchRsltList )
			{
				DWORD commandID = 0;
				int index = m_pWndSearchRsltList->GetCurSel();
				if (index >= 0 && index < m_pWndSearchRsltList->GetCount())
				{
					 commandID = m_pWndSearchRsltList->GetItemData(index);
					 m_wndInEdit.SetText("");
					 m_pWndSearchRsltList->ResetContent();
					 m_pWndSearchRsltList->ShowWindow(SW_HIDE);
					 AfxGetMainWnd()->PostMessage(WM_COMMAND,commandID,0);
					return TRUE;
				}
			}
		}

		CDockablePane::IsDialogMessage(pMsg);
		return TRUE;
	}

	return CDockablePane::PreTranslateMessage(pMsg);
}

void COutputViewBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	CDockablePane::OnChar(nChar, nRepCnt, nFlags);
}

void COutputViewBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDockablePane::OnKeyDown(nChar, nRepCnt, nFlags);
}


LRESULT COutputViewBar::OnCharOne(WPARAM nChar, LPARAM nFlags)
{
	if(m_pParam)
	{
		if( !Keyin( LPCTSTR((CString)(char)nChar),0) )
		{
			const MSG *pMsg = GetCurrentMessage();
			m_wndInEdit.SendMessage(WM_CHAR,pMsg->wParam,pMsg->lParam);
			m_wndInEdit.SetFocus();			
		}
	}
	else
	{
		const MSG *pMsg = GetCurrentMessage();
		m_wndInEdit.SendMessage(WM_CHAR,pMsg->wParam,pMsg->lParam);
		m_wndInEdit.SetFocus();
	}

	return 0;
}

LRESULT COutputViewBar::OnKeyOne(WPARAM nChar, LPARAM nFlags)
{
	const MSG *pMsg = GetCurrentMessage();
	m_wndInEdit.SendMessage(WM_KEYDOWN,pMsg->wParam,pMsg->lParam);

	return 0;
}


LRESULT COutputViewBar::OnKeyinString(WPARAM flag, LPARAM text)
{
	Keyin((LPCTSTR)text,1);

	return 0;
}

void COutputViewBar::OnNcPaint()
{
	CDockablePane::OnNcPaint();
}

void COutputViewBar::OnMove(int x, int y)
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	
	CRect rectClient,rect;
	GetClientRect (rectClient);

	ClientToScreen(&rectClient);

	if (m_pWndSearchRsltList)
	{
		m_pWndSearchRsltList->SetWindowPos (NULL, rectClient.left+30,rectClient.top + rectClient.Height ()-420,300,400, SWP_NOACTIVATE);
		m_pWndSearchRsltList->AdjustSize();
	}
	CDockablePane::OnMove(x,y);
}

void COutputViewBar::OnMoving(UINT nSide, LPRECT lpRect)
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	
	CRect rectClient,rect;
	GetClientRect (rectClient);
	
	ClientToScreen(&rectClient);
	
	if (m_pWndSearchRsltList)
	{
		m_pWndSearchRsltList->SetWindowPos (NULL, rectClient.left+30,rectClient.top + rectClient.Height ()-420,300,400, SWP_NOACTIVATE);
		m_pWndSearchRsltList->AdjustSize();
	}
	CDockablePane::OnMoving(nSide,lpRect);
}

LRESULT COutputViewBar::OnSearchListSelChange(WPARAM wParam, LPARAM lParam)
{
	if (m_pWndSearchRsltList)
	{
		m_wndInEdit.SetText("");
		m_pWndSearchRsltList->ResetContent();
		m_pWndSearchRsltList->ShowWindow(SW_HIDE);
		AfxGetMainWnd()->PostMessage(WM_COMMAND,wParam,0);
	}

	return 0;
}

void COutputViewBar::LoadCustomizeKey()
{
	FILE *fp = fopen(m_keyfilepath, "rt");
	if(!fp) return;

	m_arrkeys.RemoveAll();

	char line[1024];
	char szName[1024];
	char szAccel[1024];
	char szCskey[1024];//自定义
	int id=0;
	while(!feof(fp))
	{
		memset(line,0,sizeof(line));
		memset(szName,0,sizeof(szName));
		memset(szAccel,0,sizeof(szAccel));
		memset(szCskey,0,sizeof(szCskey));
		fgets(line,sizeof(line)-1,fp);
		if(strlen(line)<=0)  continue;
		
		char *temp = strchr(line, '>');
		if(temp==NULL)
			continue;

		int length = temp-line;
		strncpy(szName, line+1, length-1);
		
		if (sscanf(temp+1,"%s%s", szAccel, szCskey)>0)
		{
			CustomizeKey temp;
			temp.Name = szName;
			temp.Accel = szAccel;
			temp.Cskey = szCskey;

			if(temp.Cskey.GetLength()>0)
				m_arrkeys.Add(temp);
		}
	}
	fclose(fp);
	fp=NULL;
}

int COutputViewBar::GetCustomizeKeyCommand(CString str)
{
	int nreg =0;
	CDlgDoc *pDoc = GetActiveDlgDoc();
	if(!pDoc) return 0;
	const CMDREG *pRegs = pDoc->GetCmdRegs(nreg);
	for(int i=0; i<nreg; i++)
	{
		CString Accel1 = pRegs[i].strAccel;

		if(Accel1 == str)
		{
			return pRegs[i].id;
		}
	}
		
	return 0;
}

void COutputViewBar::SortSearchRsltList(SortItemArray& arr)
{
	if(arr.GetSize()<2) return;
	for(int i=0; i<arr.GetSize()-1; i++)
	{
		for(int j=0; j<arr.GetSize()-1; j++)
		{
			if(arr[j].findIndex < arr[j+1].findIndex)
			{
				continue;
			}
			else if(arr[j].findIndex == arr[j+1].findIndex && arr[j].sortStr>arr[j+1].sortStr)
			{
				SortItem temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
			else if(arr[j].findIndex > arr[j+1].findIndex)
			{
				SortItem temp = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = temp;
			}
		}
	}
}