// FullScreenAssist.cpp: implementation of the CFullScreenAssist class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "FullScreenAssist.h"
#include "ExMessage.h"
//#include "RegPath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFullScreenAssist::CFullScreenAssist():m_oldMainRect(0,0,0,0)
{
	m_pFrame = NULL;
	m_bFullScreen = FALSE;
	m_hSaveMenu   = NULL;
	memset(m_wndState,0,sizeof(m_wndState));
}

CFullScreenAssist::~CFullScreenAssist()
{
	if( IsFullScreen() && m_pFrame )
		::DestroyMenu(m_hSaveMenu);
}



void CFullScreenAssist::FullScreen(BOOL bFull)
{
	CFrameWnd *pMainFrame, *pChild;

	if( !m_pFrame )return;
	if( m_pFrame->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) )
	{
		pMainFrame = (CFrameWnd*)AfxGetMainWnd();
		pChild=m_pFrame;
	}
	else if( m_pFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		pMainFrame = m_pFrame;
		pChild     = m_pFrame;
	}
	else return;

	if( !pMainFrame || !pChild )return;

	if( bFull )
	{
		if( m_bFullScreen )return;

		CControlBar *pctrl;	
		int i = 0;		
		// hide all bars
		POSITION pos = pMainFrame->m_listControlBars.GetHeadPosition();
		while (pos != NULL)
		{
			pctrl = (CControlBar*)pMainFrame->m_listControlBars.GetNext(pos);		
			m_wndState[i++] = (pctrl->IsWindowVisible()!=0);		
			if( i>=sizeof(m_wndState)/sizeof(m_wndState[0]) )break;
		}

		pos = pMainFrame->m_listControlBars.GetHeadPosition();
		while (pos != NULL)
		{
			pctrl = (CControlBar*)pMainFrame->m_listControlBars.GetNext(pos);			
			pctrl->ShowWindow(SW_HIDE);		
			if( i>=sizeof(m_wndState)/sizeof(m_wndState[0]) )break;
		}

		// hide menu	
		m_hSaveMenu = ::GetMenu(pMainFrame->m_hWnd);
		::SetMenu(pMainFrame->m_hWnd,NULL);		

		// now save the old positions of the main and child windows
		pMainFrame->GetWindowRect(&m_oldMainRect);
		// remove the caption of the mainWnd:
		LONG style=::GetWindowLong(pMainFrame->m_hWnd,GWL_STYLE);
		style&=~WS_CAPTION;
		::SetWindowLong(pMainFrame->m_hWnd,GWL_STYLE,style);

		CRect rcWnd;
		pMainFrame->GetWindowRect(&rcWnd);
		pMainFrame->SetWindowPos(NULL,rcWnd.left,rcWnd.top,
			rcWnd.Width(),rcWnd.Height(),SWP_FRAMECHANGED);

		//change window size
		RECT rDesktop;
 		::GetWindowRect(::GetDesktopWindow(), &rDesktop);

		BOOL bMainMaximize = (style & WS_MAXIMIZE)?TRUE:FALSE;
		m_wndState[i++] = bMainMaximize;

		if( !bMainMaximize )
		{
			pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
		}	

		// adjust child frame
		if( pChild && pChild!=pMainFrame)
		{
			style=::GetWindowLong(pChild->m_hWnd,GWL_STYLE);
			m_wndState[i++]=(style & WS_MAXIMIZE)?TRUE:FALSE;

			style=::GetWindowLong(pChild->m_hWnd,GWL_STYLE);
			style&=~WS_CAPTION;
			::SetWindowLong(pChild->m_hWnd,GWL_STYLE,style);
			
			pChild->ShowWindow(SW_SHOWMAXIMIZED);
		}

		pMainFrame->RecalcLayout();
		m_bFullScreen=TRUE;
	}
	else
	{
		if( !m_bFullScreen )return;

		LONG style;
		// restore all kinds of bars
		CControlBar *pctrl;	
		int i=0;		
		POSITION pos = pMainFrame->m_listControlBars.GetHeadPosition();
		while (pos != NULL)
		{
			pctrl = (CControlBar*)pMainFrame->m_listControlBars.GetNext(pos);			
			if( m_wndState[i++] )pctrl->ShowWindow(SW_SHOW);	
			if( i>=sizeof(m_wndState)/sizeof(m_wndState[0]) )break;
		}
	
		// restore menu
		::SetMenu(pMainFrame->m_hWnd, m_hSaveMenu);
		m_hSaveMenu = NULL;	
		
		// restore the title of the main frame
		style=::GetWindowLong(pMainFrame->m_hWnd,GWL_STYLE);
		style|=WS_CAPTION;
		::SetWindowLong(pMainFrame->m_hWnd,GWL_STYLE,style);

		CRect rcWnd;
		pMainFrame->GetWindowRect(&rcWnd);
		pMainFrame->SetWindowPos(NULL,rcWnd.left,rcWnd.top,
			rcWnd.Width(),rcWnd.Height(),SWP_FRAMECHANGED);

		BOOL bMainMaximize = m_wndState[i++];
		if( !bMainMaximize )
		{
			pMainFrame->ShowWindow(SW_NORMAL);
		}

		// childs during Full Screen Mode:
		if(pChild && pChild!=pMainFrame )
		{
			style=::GetWindowLong(pChild->m_hWnd,GWL_STYLE);
			style|=WS_CAPTION;
			::SetWindowLong(pChild->m_hWnd,GWL_STYLE,style);
			if(m_wndState[i++])
			{
				pChild->ShowWindow(SW_SHOWMAXIMIZED);
				//pMainFrame->MDIMaximize(pChild);
			}
			else 
			{
				pChild->ShowWindow(SW_NORMAL);
				//pMainFrame->MDIRestore(pChild);
			}
		}

		pMainFrame->RecalcLayout();
		m_bFullScreen=FALSE;
	}

}

void CFullScreenAssist::OnFullscreen() 
{
	FullScreen(!m_bFullScreen);
}

void CFullScreenAssist::Attach(CFrameWnd *pWnd)
{
	m_pFrame = pWnd;
}


#if 0
CFramePlaceAssist::CFramePlaceAssist()
{
	m_pFrame = NULL;
	m_bPlaceReg = TRUE;
}


CFramePlaceAssist::~CFramePlaceAssist()
{
}


void CFramePlaceAssist::Attach(CFrameWnd *pWnd, const char *regPath, BOOL bPlaceReg)
{
	m_pFrame = pWnd;
	if( regPath )m_strPath = regPath;
	m_bPlaceReg = bPlaceReg;

	if (!m_bPlaceReg)
	{
		m_regDisk.SetRegistry(FALSE,FALSE);
		CString strSectionPath = ::UIFGetRegPath (m_strPath);
		m_regDisk.Open(strSectionPath);
	}

}


void CFramePlaceAssist::SavePlace()
{
	if( !m_pFrame ||m_strPath.GetLength()<=0 )return;

	CWinApp *pApp = AfxGetApp();
	if( !pApp )return;

	//窗口范围
	CRect rect;
	m_pFrame->GetWindowRect(&rect);
	CWnd *pParent = m_pFrame->GetParent();
	if( pParent )pParent->ScreenToClient(&rect);
	if (!m_bPlaceReg)
		m_regDisk.Write("WindowRect",(BYTE*)&rect,sizeof(rect));
	else
		pApp->WriteProfileBinary(m_strPath,"WindowRect",(BYTE*)&rect,sizeof(rect));

	//窗口位置
	WINDOWPLACEMENT wndp;
	m_pFrame->GetWindowPlacement( &wndp );
	if (!m_bPlaceReg)
		m_regDisk.Write("Placement",(BYTE*)&wndp,sizeof(wndp));
	else
		pApp->WriteProfileBinary(m_strPath,"Placement",(BYTE*)&wndp,sizeof(wndp));

	//窗口自定义状态
	CUSTOM_STATE state;
	memset(&state,0,sizeof(state));
	m_pFrame->SendMessage(FCCM_GETCUSTOMSTATE,0,(LPARAM)&state);
	if( state.dataLen>0 && state.dataLen<=sizeof(state.data) )
	{
		if (!m_bPlaceReg)
			m_regDisk.Write("CustomState",state.data,state.dataLen);
		else
			pApp->WriteProfileBinary(m_strPath,"CustomState",state.data,state.dataLen);
	}
}


void CFramePlaceAssist::LoadPlace()
{
	if( !m_pFrame ||m_strPath.GetLength()<=0 )return;
	
	CWinApp *pApp = AfxGetApp();
	if( !pApp )return;
	
	BOOL bReadSucceed = FALSE;
	
	BOOL bGetRect = FALSE;
	//窗口范围
	CRect *prect = NULL, rect(0,0,0,0);
	UINT n;
	if (!m_bPlaceReg)
	{
		bReadSucceed = m_regDisk.Read("WindowRect",(LPBYTE*)&prect,&n);
	}
	else
	{
		bReadSucceed = pApp->GetProfileBinary(m_strPath,"WindowRect",(LPBYTE*)&prect,&n);
	}

	if( bReadSucceed/*n==sizeof(CRect)*/ )
	{
		if( prect->Width()>0 && prect->Height()>0 )
		{
			m_pFrame->SetWindowPos(NULL,prect->left,prect->top,prect->Width(),prect->Height(),
				SWP_NOZORDER|SWP_NOACTIVATE);
		}

		rect = *prect;
		bGetRect = TRUE;

		delete[] (BYTE*)prect;
	}
	
	//窗口位置
	WINDOWPLACEMENT *wndp=NULL;
	if (!m_bPlaceReg)
	{
		bReadSucceed = m_regDisk.Read("Placement",(LPBYTE*)&wndp,&n);
	}
	else
	{
		bReadSucceed = pApp->GetProfileBinary(m_strPath,"Placement",(LPBYTE*)&wndp,&n);
	}

	if( bReadSucceed /*n==sizeof(WINDOWPLACEMENT)*/ )
	{
		if( wndp )
		{
			BOOL bSet = FALSE;
			switch( wndp->showCmd )
			{
			case SW_HIDE:
			case SW_MINIMIZE:
			case SW_SHOWMAXIMIZED:
			case SW_SHOWMINIMIZED:
			case SW_SHOWMINNOACTIVE:
				bSet = TRUE;
				break;
			}
			if( bSet && bGetRect && rect.Width()>0 && rect.Height()>0 )
			{
				wndp->ptMaxPosition = rect.TopLeft();
				wndp->ptMinPosition = rect.TopLeft();

				//纠正分屏立体处理的不正确现象
				if( m_pFrame->GetParent()==NULL )
					wndp->rcNormalPosition = rect;
			}
			m_pFrame->SetWindowPlacement(wndp);
			delete[] (BYTE*)wndp;
		}
	}

	//框架自定义状态
	CUSTOM_STATE state;
	memset(&state,0,sizeof(state));
	BYTE *data;
	if (!m_bPlaceReg)
	{
		bReadSucceed = m_regDisk.Read("CustomState",&data,&n);
	}
	else
	{
		bReadSucceed = pApp->GetProfileBinary(m_strPath,"CustomState",&data,&n);
	}
	
	if( bReadSucceed/*n>0 && n<=sizeof(state.data) */)
	{
		state.dataLen = n;
		memcpy(state.data,data,n);
		m_pFrame->SendMessage(FCCM_SETCUSTOMSTATE,0,(LPARAM)&state);		
	}

	if (bReadSucceed)
	{
		delete[] data;
	}

	m_pFrame->RecalcLayout(TRUE);
}


void LoadViewPlace(CView *pView, BOOL bFromReg)
{
	if( !pView )return;

	//CWinApp *pApp = AfxGetApp();
	HKEY hkey;
	char path[256];

	//调入布局
	strcpy(path,"Layout\\MDIPlace\\");
	strcat(path,pView->GetRuntimeClass()->m_lpszClassName);
	CFramePlaceAssist place;
	place.Attach(pView->GetParentFrame(),path,bFromReg);
	place.LoadPlace();

	/*sprintf(path,"Software\\%s\\%s\\Layout",pApp->m_pszRegistryKey,pApp->m_pszAppName);
	if( ::RegOpenKeyEx(HKEY_CURRENT_USER,path,0,KEY_READ,&hkey)==ERROR_SUCCESS )
	{
		::RegCloseKey(hkey);
		
		//调入布局
		strcpy(path,"Layout\\MDIPlace\\");
		strcat(path,pView->GetRuntimeClass()->m_lpszClassName);
		CFramePlaceAssist place;
		place.Attach(pView->GetParentFrame(),path,bFromReg);
		place.LoadPlace();
	}*/
}
#endif