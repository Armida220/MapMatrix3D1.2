// CheckScheme.h: interface for the CCheckScheme class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHECKSCHEME_H__4DF66FBB_2FD7_4B45_9D6D_1ACE9C730CD0__INCLUDED_)
#define AFX_CHECKSCHEME_H__4DF66FBB_2FD7_4B45_9D6D_1ACE9C730CD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define			XMLTAG_CHECKSCHEME			_T("CheckScheme")
#define         XMLTAG_CHKTASK      		_T("CheckTask")
#define         XMLTAG_CHKTASKCATEGORY		_T("CheckTaskCateGory")
#define         XMLTAG_CHKTASKNAME			_T("CheckTaskName")
#define         XMLTAG_CHKTASKPARAM			_T("CheckTaskParam")
#define			XMLTAG_CHKITEM   			_T("CheckItem")
#define			XMLTAG_CHKITEMNAME			_T("CheckItemName")
#define			XMLTAG_CHKITEMTASKLIST		_T("CheckItemTaskList")
#define			XMLTAG_CHKGROUP	        	_T("CheckGroup")
#define			XMLTAG_CHKGROUPNAME	        _T("CheckGroupName")
#define			XMLTAG_CHKGROUPITEMLIST		_T("CheckGroupItemList")
//检查任务
class CCheckTask 
{
public:
	CCheckTask();
	~CCheckTask();
	BOOL ReadFrom(const CString &strData);
	BOOL WriteTo(CString &strData);
	CString m_checkCategory;
	CString m_checkName;
	CString m_checkParams;
};
//检查项
class CCheckItem
{
public:
	CCheckItem();
	~CCheckItem();
	CCheckItem(const CCheckItem& rhs);
	CCheckItem& operator=(const CCheckItem& rhs);
	BOOL ReadFrom(const CString &strData);
	BOOL WriteTo(CString &strData);
	CString m_itemName;
	CArray<CCheckTask,CCheckTask&> m_taskList;
};
//检查组
struct CCheckGroup
{
public:
	CCheckGroup();
	~CCheckGroup();
	CCheckGroup(const CCheckGroup& rhs);
	CCheckGroup& operator=(const CCheckGroup& rhs);
	BOOL ReadFrom(const CString &strData);
	BOOL WriteTo(CString &strData);
	CString m_groupName;
	CArray<CCheckItem,CCheckItem&> m_itemList;
};
//检查方案
class CCheckScheme  
{
	friend CCheckScheme& GetCheckScheme();
public:	
	~CCheckScheme();
	BOOL Load(LPCTSTR fileName);
	BOOL Save(LPCTSTR fileName = NULL)const;
	inline BOOL IsValid(){
		return m_bIsValid;
	}

	BOOL Import(LPCTSTR chkFileName);
	CCheckGroup*GetCheckGroup(int &nNum);
	void AddCheckGroup( CCheckGroup &group);
	BOOL DelCheckGroup(LPCTSTR groupName);
	BOOL DelCheckGroup(int idx);
private:
	CCheckScheme();
	void Clear();
	BOOL   m_bIsValid;
	CString m_strFileName;
	CArray<CCheckGroup,CCheckGroup&> m_arrGroups;
};

CCheckScheme& GetCheckScheme();

#endif // !defined(AFX_CHECKSCHEME_H__4DF66FBB_2FD7_4B45_9D6D_1ACE9C730CD0__INCLUDED_)
