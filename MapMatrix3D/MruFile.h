// MruFile.h: interface for the CMruFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MRUFILE_H__671CC55F_A571_4732_B0E9_8E33F33E3AFC__INCLUDED_)
#define AFX_MRUFILE_H__671CC55F_A571_4732_B0E9_8E33F33E3AFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXNUM 16     //最大MRU文件数

class CMruFile  
{
public:
	CMruFile();
	virtual ~CMruFile();

	CString	m_IniFileName;		//ini文件名
	CString	m_PathName[MAXNUM];	//MRU文件路径名
	CString	m_nIndex[MAXNUM];	//路径下绑定的索引
	int		m_CurNum;			//当前最近文件数

	void ReadMru();			//读取ini文件中的最近文件
	void WriteMru();		//最近文件写入ini文件
	void AddMru(CString nPathName,CString nIndex = _T(""));		//添加最近文件
	void ClearMru();		//清除最近文件
	void SetMruMenu();		//修改最近文件菜单
	void SetMenuPtr(CMenu *pMruMenu);		//设置需要修改的菜单指针
	void DelMru(CString nPathName,int nindex);
	CMenu *GetMenuPtr();
	CMenu *m_pMruMenu;
};

#endif // !defined(AFX_MRUFILE_H__671CC55F_A571_4732_B0E9_8E33F33E3AFC__INCLUDED_)
