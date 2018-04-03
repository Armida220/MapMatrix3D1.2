// RecentCmd.h: interface for the CRecentCmd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RECENTCMD_H__A3AA89C3_17E8_4C90_B958_E90353E4202B__INCLUDED_)
#define AFX_RECENTCMD_H__A3AA89C3_17E8_4C90_B958_E90353E4202B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRecentCmd  
{
public:
	
	struct CmdItem
	{
		CmdItem(){
			id = 0;
			strcpy(name, "");
		}
		char name[256];
		int id;
	};
	CRecentCmd();
	virtual ~CRecentCmd();
	int GetCmdsCount();
	CmdItem GetCmdAt(int idx);
	void AddCmdItem(int id, LPCTSTR name);
	void Load();
	void Save();
	CmdItem FindItem(const char* name);
	CString LastCommandName();
	void ActiveLastRecentCmd();
protected:
	CString GetConfigFile();

	int m_nMaxNum;
	CArray<CmdItem,CmdItem> m_arrCmdItems;

};

#endif // !defined(AFX_RECENTCMD_H__A3AA89C3_17E8_4C90_B958_E90353E4202B__INCLUDED_)
