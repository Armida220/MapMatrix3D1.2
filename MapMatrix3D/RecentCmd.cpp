// RecentCmd.cpp: implementation of the CRecentCmd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "RecentCmd.h"
#include  <io.h>
#include "RegDef2.h "

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecentCmd::CRecentCmd()
{
	m_nMaxNum = 20;
}

CRecentCmd::~CRecentCmd()
{

}

int CRecentCmd::GetCmdsCount()
{
	return m_arrCmdItems.GetSize();
}

CRecentCmd::CmdItem CRecentCmd::GetCmdAt(int idx)
{
	ASSERT(idx>=0||idx<m_arrCmdItems.GetSize());
	return m_arrCmdItems.GetAt(idx);
}

void CRecentCmd::AddCmdItem(int id, LPCTSTR name)
{
	m_nMaxNum  = AfxGetApp()->GetProfileInt(REGPATH_USER,REGITEM_RECENTCMD,20);
	CmdItem item;
	int nsize = m_arrCmdItems.GetSize();
	for( int i=0; i<nsize; i++)
	{
		item = m_arrCmdItems.GetAt(i);
		if( item.id==id )
		{
			break;
		}
	}
	
	if( i<nsize )
	{
		m_arrCmdItems.RemoveAt(i);
		strcpy(item.name,name);
		m_arrCmdItems.InsertAt(0,item);
		if( nsize>m_nMaxNum )
		{
			int idx = m_arrCmdItems.GetSize()-1;
			for (int j=0;j<nsize-m_nMaxNum+1;j++)
			{
				m_arrCmdItems.RemoveAt(idx-j);
			}
		}
//		m_wndList.DeleteString(i);
//		m_wndList.InsertString(0,name);
	}
	else
	{
		item.id = id;
		strcpy(item.name,name);
		
		m_arrCmdItems.InsertAt(0,item);	
		if( nsize>m_nMaxNum )
		{
			int idx = m_arrCmdItems.GetSize()-1;
			for (int j=0;j<nsize-m_nMaxNum+1;j++)
			{
				m_arrCmdItems.RemoveAt(idx-j);
			}
//			m_wndList.DeleteString(idx);
		}
	}
	
	Save();
}

CString CRecentCmd::GetConfigFile()
{
	//获取历史文件目录
	TCHAR module[_MAX_PATH]={0};	
	GetModuleFileName(NULL,module,_MAX_PATH);
	CString dir = module;
	int pos = dir.ReverseFind(_T('\\'));
	if( pos>0 )
	{
		dir = dir.Left(pos);
		pos = dir.ReverseFind(_T('\\'));		
	}
	
	if( pos>0 )
	{
		dir = dir.Left(pos+1) + _T("History");
	}
	else
	{
		dir = _T("History");
	}
	
	//查看目录是否存在
	if(_taccess(dir,0)!=0)
		::CreateDirectory(dir,NULL);
// 	HANDLE hDir = ::CreateFile( dir,
// 		GENERIC_READ | GENERIC_WRITE,
// 		FILE_SHARE_READ | FILE_SHARE_WRITE,
// 		NULL,
// 		OPEN_EXISTING,
// 		FILE_FLAG_BACKUP_SEMANTICS,
// 		NULL
// 		);
// 	
// 	if( hDir!=INVALID_HANDLE_VALUE )
// 	{
// 		::CloseHandle(hDir);
// 	}
// 	else
// 	{
// 		::CreateDirectory(dir,NULL);
// 	}
	
	dir += _T("\\");	
	
	CString logFile = dir + _T("RecentCommand.txt");
	return logFile;
}

void CRecentCmd::Load()
{
//	m_wndList.ResetContent();
	m_arrCmdItems.RemoveAll();
	
	CString filename = GetConfigFile();
	FILE *fp = fopen(filename,"r");
	if( !fp )return;
	
	char line[1024];
	CmdItem item;
	
	while( !feof(fp) )
	{
		memset(&item,0,sizeof(item));
	//	fscanf( stream, "%s", s );  //改写

		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		if( strlen(line)<=0 )continue;
		sscanf(line,"%d", &item.id);
		
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		if( strlen(line)<=0 )continue;
		
		sscanf(line, "%s", item.name);
		m_arrCmdItems.Add(item);
	}
	
	fclose(fp);
}

void CRecentCmd::Save()
{
	CString filename = GetConfigFile();
	FILE *fp = fopen(filename,"w");
	if( !fp )return;
	
	char line[1024];
	CmdItem item;
	
	for( int i=0; i<m_arrCmdItems.GetSize(); i++)
	{
		item = m_arrCmdItems.GetAt(i);
		
		memset(line,0,sizeof(line));
		fprintf(fp,"%d\n",item.id);
		fprintf(fp,"%s\n",item.name);
	}
	
	fclose(fp);
}

CRecentCmd::CmdItem CRecentCmd::FindItem(const char *name)
{
	ASSERT(name!=NULL);
	for (int i=0;i<m_arrCmdItems.GetSize();i++)
	{
		if(strcmp(m_arrCmdItems[i].name,name)==0)
			return m_arrCmdItems.GetAt(i);
	}
	return CmdItem();	
}

CString CRecentCmd::LastCommandName()
{
	if(m_arrCmdItems.GetSize()>1)		
	{
		CmdItem item = m_arrCmdItems.GetAt(1);
		return CString(item.name);
	}
	return CString();
}

void CRecentCmd::ActiveLastRecentCmd()
{
	if( m_arrCmdItems.GetSize()>1 )
	{
		CmdItem item = m_arrCmdItems.GetAt(1);
		AfxGetMainWnd()->SendMessage(WM_COMMAND,item.id);
	}
}