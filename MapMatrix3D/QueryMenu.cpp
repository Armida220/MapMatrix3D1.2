// QueryMenu.cpp: implementation of the CQueryMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "QueryMenu.h"
#include "Markup.h"
#include <string.h>
#include "StereoView.h"
// #include "StereoView_EP.H"
#include "VectorView_EX.h"
#include "VectorView_new.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void LoadCondCfg(LPCTSTR fileName, CPtrArray& arrMenus)
{
	CMarkup xmlFile;
	if( !xmlFile.Load(fileName) )return;

	if( !xmlFile.FindElem("Menu") )return;
	xmlFile.IntoElem();

	CondMenu *pMenu = NULL;
	CONDSEL cond;
	CString strName, strData;
	while( xmlFile.FindElem("MenuItem") )
	{
		pMenu = new CondMenu;
		if( !pMenu )continue;

		xmlFile.IntoElem();
		
		while( xmlFile.FindElem(_T("Condition")) )
		{
			xmlFile.IntoElem();
			memset(&cond,0,sizeof(cond));

			while( xmlFile.FindElem(NULL) )
			{
				strName = xmlFile.GetTagName();
				strData = xmlFile.GetData();
				if( strName.CompareNoCase(_T("CondType"))==0 )
				{
					char *pBuf = strData.GetBuffer(strData.GetLength()), *pStart, *pStop, *pMax;
					
					pStart = pBuf; pStop = pStart; pMax = pBuf+strData.GetLength();
					
					int num = 0;
					while (num < 3)
					{
						int value = strtol(pStart,&pStop,10);
						if( pStop>=pMax || pStop==pStart )break;
						cond.condtype[num] = value;
						pStart = pStop;
						num++;

					}

					strData.ReleaseBuffer();
				
				}
				else if( strName.CompareNoCase(_T("Field"))==0 )
				{
					if( strData.GetLength()<sizeof(cond.field) )
					{
						strcpy(cond.field,strData);
					}
					else
					{
						memcpy(cond.field,(LPCTSTR)strData,sizeof(cond.field)-1);
					}
				}
				else if( strName.CompareNoCase(_T("FIdx"))==0 )
				{
//					cond.fidx = atoi(strData);
				}
				else if( strName.CompareNoCase(_T("Value"))==0 )
				{
					if( strData.GetLength()<sizeof(cond.value) )
					{
						strcpy(cond.value,strData);
					}
					else
					{
						memcpy(cond.value,(LPCTSTR)strData,sizeof(cond.value)-1);
					}
				}
				else if( strName.CompareNoCase(_T("OP"))==0 )
				{
					cond.op = atoi(strData);
				}
// 				else if( strName.CompareNoCase(_T("CondType"))==0 )
// 				{
// 					cond.condtype = atoi(strData);
// 				}
			}

			pMenu->arrConds.Add(cond);
			xmlFile.OutOfElem();
		}

		if( xmlFile.FindElem(_T("Name"),TRUE) )
		{
			pMenu->name = xmlFile.GetData();
		}

		xmlFile.OutOfElem();

		if( pMenu->name.IsEmpty() || pMenu->arrConds.GetSize()<=0 )
		{
			delete pMenu;
		}
		else
		{
			arrMenus.Add(pMenu);
		}
	}
}


void SaveCondCfg(LPCTSTR fileName, CPtrArray& arrMenus)
{
	CMarkup xmlFile;	
	CONDSEL cond;

	xmlFile.AddElem("Menu");
	xmlFile.IntoElem();

	int num = arrMenus.GetSize();
	for( int i=0; i<num; i++)
	{
		CondMenu *p = (CondMenu*)arrMenus.GetAt(i);
		if( !p )continue;

		xmlFile.AddElem("MenuItem");
		xmlFile.IntoElem();
		xmlFile.AddElem("Name",(LPCTSTR)p->name);

		for( int j=0; j<p->arrConds.GetSize(); j++)
		{
			xmlFile.AddElem("Condition");
			xmlFile.IntoElem();

			cond = p->arrConds.GetAt(j);

			CString str;
			for (int k=0; k<3; k++)
			{
				if (cond.condtype[k])
					str += '1';
				else
					str += '0';
				str += " ";
			}
			
			xmlFile.AddElem("CondType",str);
			xmlFile.AddElem("Field",cond.field);
//			xmlFile.AddElem("FIdx",cond.fidx);
			xmlFile.AddElem("Value",cond.value);
			xmlFile.AddElem("OP",cond.op);
//			xmlFile.AddElem("CondType",cond.condtype);
			
			xmlFile.OutOfElem();
		}

		xmlFile.OutOfElem();
	}

	xmlFile.OutOfElem();
	xmlFile.Save(fileName);
}



HMENU FindMenuByID(HMENU hMenu, UINT id)
{
	if( !hMenu )return NULL;
	for( int i=0; i<::GetMenuItemCount(hMenu); i++)
	{
		int nid = ::GetMenuItemID(hMenu,i);
		if( nid==id )return hMenu;
		if( nid==-1 )
		{
			HMENU ret = FindMenuByID(::GetSubMenu(hMenu,i),id);
			if( ret )return ret;
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CQueryMenu::CQueryMenu()
{
	m_hMenu = NULL;
}

CQueryMenu::~CQueryMenu()
{
	if( m_hMenu )::DestroyMenu(m_hMenu);
	ClearCondData();
}


BOOL CQueryMenu::IsLoaded()
{
	if( m_hMenu && m_arrMenus.GetSize()>0 )
		return TRUE;
	return FALSE;
}

void CQueryMenu::SetDoc(CDocument *pDoc)
{
	if( m_hMenu )::DestroyMenu(m_hMenu);
	m_hMenu = NULL;
	
	CWnd *pWnd = AfxGetMainWnd();
	CView *pView = NULL;

	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMDIFrameWndEx *pMain = (CMDIFrameWndEx*)pWnd;
		CMDIChildWnd *pChild= pMain->MDIGetActive();

		CFrameWnd *pParent = NULL;

		POSITION pos = pDoc->GetFirstViewPosition();
		while( pos )
		{
			pView = pDoc->GetNextView(pos);
			pParent = (pView==NULL?NULL:pView->GetParentFrame());
			if( pParent && pParent->GetSafeHwnd()==pChild->GetSafeHwnd() )
				break;
		}
	}
	else if( pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
	{
		CFrameWnd *pMain = (CFrameWnd*)pWnd;
		pView = pMain->GetActiveView();
	}

	if( !pView )return;
/*	if( pView->IsKindOf(RUNTIME_CLASS(CVectorView_EP))||
		pView->IsKindOf(RUNTIME_CLASS(CEPSectionView))||
		pView->IsKindOf(RUNTIME_CLASS(CEPRoadView)) )
	{
		m_hMenu = ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_DOCTYPE_EPDB_MDB));
	}
	else if( pView->IsKindOf(RUNTIME_CLASS(CStereoView_EP)) )
	{
		m_hMenu = ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_DOCTYPE_STEREO_EP));
	}
	else */
	if( pView->IsKindOf(RUNTIME_CLASS(CVectorView_new)) )
	{
		if( pView->IsKindOf(RUNTIME_CLASS(CVectorCellView_new)) )
		{
			m_hMenu = ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_CELLEDITBATYPE));
		}
		else
			m_hMenu = ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_EDITBATYPE));
	}	
	else if( pView->IsKindOf(RUNTIME_CLASS(CStereoView)) )
	{
		m_hMenu = ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_EDITBATYPE_STEREO));
	}

}


void CQueryMenu::Load(LPCTSTR filename)
{
	ClearCondData();

	//读取配置文件	
	//获得配置路径
	char path[_MAX_PATH]={0};

	if( filename==NULL )
	{
		GetModuleFileName(AfxGetApp()->m_hInstance,path,_MAX_PATH);
		*(strrchr(path,'\\')) = 0x00;
		*(strrchr(path,'\\')) = 0x00;
		strcat(path,"\\Config\\QueryMenu.xml");
	}
	else
	{
		GetModuleFileName(AfxGetApp()->m_hInstance,path,_MAX_PATH);
		*(strrchr(path,'\\')) = 0x00;
		*(strrchr(path,'\\')) = 0x00;
		strcat(path,"\\Config\\");
		strcat(path,filename);
	}

	LoadCondCfg(path,m_arrMenus);
}


void CQueryMenu::Save(LPCTSTR filename)
{
	//获得配置路径
	char path[_MAX_PATH]={0};

	if( filename==NULL )
	{
		GetModuleFileName(AfxGetApp()->m_hInstance,path,_MAX_PATH);
		*(strrchr(path,'\\')) = 0x00;
		*(strrchr(path,'\\')) = 0x00;
		strcat(path,"\\Config\\QueryMenu.xml");
	}
	else
	{
		GetModuleFileName(AfxGetApp()->m_hInstance,path,_MAX_PATH);
		*(strrchr(path,'\\')) = 0x00;
		*(strrchr(path,'\\')) = 0x00;
		strcat(path,"\\Config\\");
		strcat(path,filename);
	}	

	SaveCondCfg(path,m_arrMenus);
}


void CQueryMenu::UpdateWindow()
{
	if( m_hMenu==NULL )return;
	HMENU hMenu = FindMenuByID(m_hMenu,ID_CONFIG_QUERY);
	
	//删除原来的
	while(1)
	{
		int num = ::GetMenuItemCount(hMenu);
		for( int i=0; i<num; i++)
		{
			UINT id = ::GetMenuItemID(hMenu,i);
			if( id>=ID_CONFIG_QUERY_BEGIN && id<=ID_CONFIG_QUERY_END )
			{
				::RemoveMenu(hMenu,i,MF_BYPOSITION);
				break;
			}
		}
		if( i>=num )break;
	}

	//得到插入位置
	int pos = 0, count = ::GetMenuItemCount(hMenu);
	for( int i=0; i<count; i++)
	{
		if( ::GetMenuItemID(hMenu,i)==ID_CONFIG_QUERY )break;
	}

	pos = i+1;

	//增加现在的
	int num = m_arrMenus.GetSize();
	for( i=0; i<num; i++)
	{
		CondMenu *p = (CondMenu*)m_arrMenus.GetAt(i);
		if( p )
		{
			::InsertMenu(hMenu,pos+i,MF_BYPOSITION|MF_STRING|MF_ENABLED,ID_CONFIG_QUERY_BEGIN+i,p->name);
		}
	}

	return;
}


void CQueryMenu::GetQuery(UINT id, CArray<CONDSEL,CONDSEL>& arr)
{
	if( id>=ID_CONFIG_QUERY_BEGIN && id<=ID_CONFIG_QUERY_END && 
		(int)id-ID_CONFIG_QUERY_BEGIN<m_arrMenus.GetSize() )
	{
		CondMenu *p = (CondMenu*)m_arrMenus.GetAt((int)id-ID_CONFIG_QUERY_BEGIN);
		if ( p )
		{
			arr.Copy(p->arrConds);
		}
	}
}


void CQueryMenu::ClearCondData()
{
	CondMenu *p = NULL;
	for( int i=0; i<m_arrMenus.GetSize(); i++)
	{
		p = (CondMenu*)m_arrMenus.GetAt(i);
		if( p )delete p;
	}

	m_arrMenus.RemoveAll();
}

void CQueryMenu::CopyCondData(CQueryMenu& menu)
{
	ClearCondData();
	CondMenu *p = NULL;
	for( int i=0; i<menu.m_arrMenus.GetSize(); i++)
	{
		p = (CondMenu*)menu.m_arrMenus.GetAt(i);
		if( p )
		{
			CondMenu *pNew = new CondMenu;
			if( pNew )
			{
				pNew->name = p->name;
				pNew->arrConds.Copy(p->arrConds);
				m_arrMenus.Add(pNew);
			}
		}
	}
}
