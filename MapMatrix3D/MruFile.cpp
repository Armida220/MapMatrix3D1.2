// MruFile.cpp: implementation of the CMruFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MruFile.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMruFile::CMruFile()
{
	m_IniFileName = _T("");
	for( int i=0; i<MAXNUM; i++ )
	{
		m_PathName[i] = _T("");
		m_nIndex[i] = _T("");
	}
		m_CurNum = 0;
}

CMruFile::~CMruFile()
{

}

void CMruFile::SetMenuPtr(CMenu *pMruMenu)
{
	m_pMruMenu = pMruMenu;
}

CMenu * CMruFile::GetMenuPtr()
{
	return m_pMruMenu;
}

//读取ini文件中的最近文件
void CMruFile::ReadMru()
{
	m_CurNum = ::GetPrivateProfileInt(_T("Mru File"), _T("FileNum"), 0, m_IniFileName);  //读取当前文件数
	CString no;
	for( int i=0; i<m_CurNum; i++)
	{
		no.Format( "%d", i+1 );   //求项名
		::GetPrivateProfileString(_T("Mru File"), no, "", m_PathName[i].GetBuffer(MAX_PATH),
			MAX_PATH, m_IniFileName);   //读取路径名
		m_PathName[i].ReleaseBuffer();

		no.Format("%d_index",i+1);	//求索引
		::GetPrivateProfileString(_T("Mru File"), no, "", m_nIndex[i].GetBuffer(MAX_PATH),
			MAX_PATH, m_IniFileName);   //读取路径名
		m_nIndex[i].ReleaseBuffer();
		
	}
	SetMruMenu();   //修改MRU菜单
}

//最近文件写入ini文件
void CMruFile::WriteMru()
{
	CString no;
	no.Format( "%d", m_CurNum );
	::WritePrivateProfileString( _T("Mru File"), _T("FileNum"), no, m_IniFileName );  //写当前文件数
	for( int i=0; i<MAXNUM; i++)
	{
		no.Format( "%d", i+1 );
		::WritePrivateProfileString(_T("Mru File"), no, m_PathName[i], m_IniFileName); //写路径名
		no.Format("%d_index",i+1);
		::WritePrivateProfileString(_T("Mru File"), no, m_nIndex[i], m_IniFileName); //写索引
	}
}

//添加最近文件（nPathName-添加的文件路径名）
//设计思路：当nPathName在m_PathName[]列表中已存在，须把它提升到表头；若它不存在，则添加到表头，如果表已满，删除表尾的元素。
void CMruFile::AddMru(CString nPathName,CString nIndex)
{
	int i;
	CString str1, str2;
	CString str3,str4;
	if( m_CurNum )
	{
		if( nPathName.CompareNoCase(m_PathName[0])==0 )  //如果nPathName已在表头，结束
			return;
	}
	str1 = nPathName;
	str3 = nIndex;
	i = 0;
	while( i<m_CurNum && nPathName.CompareNoCase(m_PathName[i])!=0 )  //在表中查找，不相等的元素下移
	{
		str2 = m_PathName[i];
		m_PathName[i] = str1;
		str1 = str2;
		str4 = m_nIndex[i];
		m_nIndex[i] = str3;
		str3 = str4;
		i++;
	}
	if( i<m_CurNum )
	{
		m_PathName[i] = str1;  //nPathName已存在
		m_nIndex[i] = str3;
	}
	else if( m_CurNum<MAXNUM )
	{
		m_PathName[m_CurNum] =str1;  //表未满
		m_nIndex[m_CurNum] = str3;
		m_CurNum++;
	}

	SetMruMenu();   //修改MRU菜单
	WriteMru();     //最近文件写入ini文件
}

void CMruFile::DelMru(CString nPathName,int nindex)
{
	if (!m_pMruMenu||nindex<0||nindex>MAXNUM||nindex>m_CurNum)
	{
		return;
	}
	int i = 0;
	if (0 != nPathName.Compare(m_PathName[nindex]))
	{
		return;
	}
	m_PathName[nindex].Empty();
	m_nIndex[nindex].Empty();
	for (i = nindex;i<m_CurNum-1;i++)
	{
		m_PathName[i] = m_PathName[i+1];
		m_nIndex[i] = m_nIndex[i+1];
	}
	m_CurNum = m_CurNum-1;
	SetMruMenu();   //修改MRU菜单
	WriteMru();     //最近文件写入ini文件
}


//清除最近文件
void CMruFile::ClearMru()
{
	m_CurNum = 0;
	for( int i=0; i<MAXNUM; i++)
	{	
		m_PathName[i].Empty();
		m_nIndex[i].Empty();
	}	
	SetMruMenu();   //修改MRU菜单
	WriteMru();
}

//修改最近文件菜单
void CMruFile::SetMruMenu()
{
// 	CMenu *pMenu = AfxGetMainWnd()->GetMenu();		//主菜单指针
// 	CMenu *pFileMenu = pMenu->GetSubMenu(0);		//“文件”菜单指针
// 	CMenu *pMruMenu = pFileMenu->GetSubMenu(5);		//“最近文件”菜单指针
// 	SetMenuPtr(pMruMenu);
	if (!m_pMruMenu)
	{
		return;
	}
	m_pMruMenu->RemoveMenu( ID_MRU1, MF_BYCOMMAND );	//删除各菜单项
	m_pMruMenu->RemoveMenu( ID_MRU2, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU3, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU4, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU5, MF_BYCOMMAND );	//删除各菜单项
	m_pMruMenu->RemoveMenu( ID_MRU6, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU7, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU8, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU9, MF_BYCOMMAND );	//删除各菜单项
	m_pMruMenu->RemoveMenu( ID_MRU10, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU11, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU12, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU13, MF_BYCOMMAND );	//删除各菜单项
	m_pMruMenu->RemoveMenu( ID_MRU14, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU15, MF_BYCOMMAND );
	m_pMruMenu->RemoveMenu( ID_MRU16, MF_BYCOMMAND );
	if( m_CurNum>0 )								//重新插入各菜单项
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU1, _T("&1 ")+m_PathName[0] );
	if( m_CurNum>1 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU2, _T("&2 ")+m_PathName[1] );
	if( m_CurNum>2 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU3,_T("&3 ")+ m_PathName[2] );
	if( m_CurNum>3 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU4, _T("&4 ")+m_PathName[3] );
	if( m_CurNum>4 )								//重新插入各菜单项
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU5, _T("&5 ")+m_PathName[4] );
	if( m_CurNum>5 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU6, _T("&6 ")+m_PathName[5] );
	if( m_CurNum>6 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU7, _T("&7 ")+m_PathName[6] );
	if( m_CurNum>7 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU8, _T("&8 ")+m_PathName[7] );
	if( m_CurNum>8 )								//重新插入各菜单项
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU9, _T("&9 ")+m_PathName[8] );
	if( m_CurNum>9 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU10, _T("1&0 ")+m_PathName[9] );
	if( m_CurNum>10 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU11, _T("1&1 ")+m_PathName[10] );
	if( m_CurNum>11 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU12, _T("1&2 ")+m_PathName[11] );
	if( m_CurNum>12 )								//重新插入各菜单项
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU13, _T("1&3 ")+m_PathName[12] );
	if( m_CurNum>13 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU14, _T("1&4 ")+m_PathName[13] );
	if( m_CurNum>14 )
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU15,_T("1&5 ")+m_PathName[14] );
	if( m_CurNum>15)
		m_pMruMenu->InsertMenu( ID_MRU_CLR, MF_BYCOMMAND,
		ID_MRU16, _T("1&6 ")+m_PathName[15] );
}

