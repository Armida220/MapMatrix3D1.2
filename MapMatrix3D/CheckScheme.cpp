// CheckScheme.cpp: implementation of the CCheckScheme class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CheckScheme.h"
#include "Markup.h "
#include "resource.h"
#include "dlginputstr.h "

CCheckTask::CCheckTask():m_checkCategory(_T("")),
m_checkName(_T("")),m_checkParams(_T(""))
{
}

CCheckTask::~CCheckTask()
{

}

BOOL CCheckTask::ReadFrom(const CString &strData)
{
	CMarkup xmlData;
	xmlData.SetDoc(strData);
	xmlData.FindElem(XMLTAG_CHKTASK);
	xmlData.IntoElem();
	CString tag,data;
	{
		
		if( xmlData.FindElem(XMLTAG_CHKTASKCATEGORY) )
		{
			m_checkCategory = xmlData.GetData();	
		}
		if( xmlData.FindElem(XMLTAG_CHKTASKNAME) )
		{
			m_checkName = xmlData.GetData();		
		}
		if( xmlData.FindElem(NULL/*XMLTAG_CHKTASKPARAM*/) )
		{
		//	xmlData.IntoElem();
			m_checkParams = xmlData.GetSubDoc();			
		//	xmlData.OutOfElem();
		}

	}	
	xmlData.OutOfElem();
	return TRUE;
}

BOOL CCheckTask::WriteTo(CString &strData)
{
	CMarkup xmlFile;
	xmlFile.AddElem(XMLTAG_CHKTASK);

	xmlFile.IntoElem();

	xmlFile.AddElem(XMLTAG_CHKTASKCATEGORY,m_checkCategory);	
	
	xmlFile.AddElem(XMLTAG_CHKTASKNAME,m_checkName);
	
 //	xmlFile.AddElem(XMLTAG_CHKTASKPARAM);//试试不用AddSubDoc
// 	xmlFile.IntoElem();
	xmlFile.AddSubDoc(m_checkParams);
//	xmlFile.OutOfElem();
	xmlFile.OutOfElem();

	strData = xmlFile.GetDoc();
	return TRUE;
}

CCheckItem::CCheckItem():m_itemName(_T(""))
{

}

CCheckItem::~CCheckItem()
{

}

CCheckItem::CCheckItem(const CCheckItem& rhs):m_itemName(rhs.m_itemName)
{
	m_taskList.Copy(rhs.m_taskList);
}

CCheckItem& CCheckItem::operator=(const CCheckItem& rhs)
{
	if(this==&rhs)
		return *this;
	m_itemName = rhs.m_itemName;
	m_taskList.Copy(rhs.m_taskList);
	return *this;
}

BOOL CCheckItem::ReadFrom(const CString &strData)
{
	CMarkup xmlData;
	xmlData.SetDoc(strData);
	
	CString tag,data;
	xmlData.FindElem(XMLTAG_CHKITEM);
	xmlData.IntoElem();
	if(xmlData.FindElem(XMLTAG_CHKITEMNAME))
		m_itemName = xmlData.GetData();
	xmlData.FindElem(XMLTAG_CHKITEMTASKLIST);
	xmlData.IntoElem();
	while(xmlData.FindElem(XMLTAG_CHKTASK))
	{		
		CCheckTask task;
		task.ReadFrom(xmlData.GetSubDoc());
		m_taskList.Add(task);
	}
	xmlData.OutOfElem();
	xmlData.OutOfElem();
	return TRUE;
}

BOOL CCheckItem::WriteTo(CString &strData)
{
	CMarkup xmlFile;
	xmlFile.AddElem(XMLTAG_CHKITEM);
	
	xmlFile.IntoElem();
	
	xmlFile.AddElem(XMLTAG_CHKITEMNAME,m_itemName);	
	
	xmlFile.AddElem(XMLTAG_CHKITEMTASKLIST);
	xmlFile.IntoElem();
	CString data;
	int nsz = m_taskList.GetSize();
	for (int i=0; i<nsz; i++)
	{
		CCheckTask checkTask = m_taskList.GetAt(i);
		checkTask.WriteTo(data);
		xmlFile.AddSubDoc(data);
	}
	xmlFile.OutOfElem();
	xmlFile.OutOfElem();
	
	strData = xmlFile.GetDoc();
	return TRUE;
}

CCheckGroup::CCheckGroup():m_groupName(_T(""))
{
}

CCheckGroup::~CCheckGroup()
{

}

CCheckGroup::CCheckGroup(const CCheckGroup& rhs):m_groupName(rhs.m_groupName)
{
	m_itemList.Copy(rhs.m_itemList);
}

CCheckGroup& CCheckGroup::operator=(const CCheckGroup& rhs)
{
	if (this==&rhs)
		return *this;
	m_groupName = rhs.m_groupName;
	m_itemList.Copy(rhs.m_itemList);
	return *this;
}

BOOL CCheckGroup::ReadFrom(const CString &strData)
{
	CMarkup xmlData;
	xmlData.SetDoc(strData);
	
	CString tag,data;
	xmlData.FindElem(XMLTAG_CHKGROUP);
	xmlData.IntoElem();
	if(xmlData.FindElem(XMLTAG_CHKGROUPNAME))
		m_groupName = xmlData.GetData();
	xmlData.FindElem(XMLTAG_CHKGROUPITEMLIST);
	xmlData.IntoElem();
	while(xmlData.FindElem(XMLTAG_CHKITEM))
	{		
		CCheckItem Item;
		Item.ReadFrom(xmlData.GetSubDoc());
		m_itemList.Add(Item);
	}
	xmlData.OutOfElem();
	xmlData.OutOfElem();
	return TRUE;
}

BOOL CCheckGroup::WriteTo(CString &strData)
{
	CMarkup xmlFile;
	xmlFile.AddElem(XMLTAG_CHKGROUP);
	
	xmlFile.IntoElem();
	
	xmlFile.AddElem(XMLTAG_CHKGROUPNAME,m_groupName);	
	
	xmlFile.AddElem(XMLTAG_CHKGROUPITEMLIST);
	xmlFile.IntoElem();
	CString data;
	int nsz = m_itemList.GetSize();
	for (int i=0; i<nsz; i++)
	{
		CCheckItem checkItem = m_itemList.GetAt(i);
		checkItem.WriteTo(data);
		xmlFile.AddSubDoc(data);
	}
	xmlFile.OutOfElem();
	xmlFile.OutOfElem();
	
	strData = xmlFile.GetDoc();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCheckScheme::CCheckScheme():m_strFileName(_T(""))
{
	m_bIsValid = FALSE;
}

CCheckScheme::~CCheckScheme()
{
	Clear();
}

void CCheckScheme::Clear()
{
	m_arrGroups.RemoveAll();
	m_bIsValid = FALSE;
}

BOOL CCheckScheme::Load(LPCTSTR fileName)
{
	Clear();
	if(fileName==NULL)
		return TRUE;
	m_strFileName = fileName;
	
	CMarkup xmlFile;	
	if(!xmlFile.Load(m_strFileName))
		goto Fail;
	
	if( xmlFile.FindElem(XMLTAG_CHECKSCHEME,TRUE) )
	{		
		if(!xmlFile.IntoElem())
			goto Fail;

		while ( xmlFile.FindElem(XMLTAG_CHKGROUP) )
		{		
			CCheckGroup checkGroup;
			if(!checkGroup.ReadFrom(xmlFile.GetSubDoc()))
				goto Fail;
			m_arrGroups.Add(checkGroup);			
		}	
		
		if(!xmlFile.OutOfElem())
			goto Fail;
	}
	else
	{
		goto Fail;
	}

	m_bIsValid = TRUE;
	return TRUE;
Fail:
	m_bIsValid = FALSE;
	m_arrGroups.RemoveAll();
	return FALSE;
}

BOOL CCheckScheme::Import(LPCTSTR chkFileName)
{
	CArray<CCheckGroup,CCheckGroup&> arrGroups;
	CMarkup xmlFile;	
	if(!xmlFile.Load(chkFileName))
		goto Fail;
	
	if( xmlFile.FindElem(XMLTAG_CHECKSCHEME,TRUE) )
	{		
		if(!xmlFile.IntoElem())
			goto Fail;
		
		while ( xmlFile.FindElem(XMLTAG_CHKGROUP) )
		{		
			CCheckGroup checkGroup;
			if(!checkGroup.ReadFrom(xmlFile.GetSubDoc()))
				goto Fail;
			while(1)//避免重复组名
			{
				for (int i=0;i<m_arrGroups.GetSize();i++)
				{
					if (m_arrGroups[i].m_groupName==checkGroup.m_groupName)
					{
						break;
					}
				}
				if (i<m_arrGroups.GetSize())
				{
					CDlgInputStr dlg;
					CString strTemp;
					strTemp.Format(IDS_EXIST_GROUPNAME,checkGroup.m_groupName);					
					dlg.SetTitle(strTemp);
					if(dlg.DoModal()==IDOK&&!dlg.m_strInput.IsEmpty())
					{
						checkGroup.m_groupName = dlg.m_strInput;
					}
					else
						;
					
				}
				else
					break;
			}
			
			arrGroups.Add(checkGroup);			
		}	
		
		if(!xmlFile.OutOfElem())
			goto Fail;
	}
	else
	{
		goto Fail;
	}	
	m_arrGroups.Append(arrGroups);
	return TRUE;
Fail:	
	arrGroups.RemoveAll();
	return FALSE;

}

BOOL CCheckScheme::Save(LPCTSTR fileName)const
{
	CString path = m_strFileName;
	if (fileName != NULL)
	{
		path = fileName;
	}
	int nsz = m_arrGroups.GetSize();
	if( nsz<=0 )
	{
		CMarkup xmlFile;
		xmlFile.Save(path);
		return TRUE;
	}
	
	CMarkup xmlFile;
	
	xmlFile.AddElem(XMLTAG_CHECKSCHEME);
	xmlFile.IntoElem();
	
	CString data;
	for (int i=0; i<nsz; i++)
	{
		CCheckGroup group = m_arrGroups.GetAt(i);
		group.WriteTo(data);
		xmlFile.AddSubDoc(data);
	}	
	xmlFile.OutOfElem();	
	xmlFile.Save(path);
	return TRUE;
}

CCheckGroup* CCheckScheme::GetCheckGroup(int &nNum)
{
	nNum = m_arrGroups.GetSize();
	return m_arrGroups.GetData();
}

void CCheckScheme::AddCheckGroup( CCheckGroup &group)
{
	m_arrGroups.Add(group);
}

BOOL CCheckScheme::DelCheckGroup(LPCTSTR groupName)
{
	for (int i=0;i<m_arrGroups.GetSize();i++)
	{
		if (m_arrGroups[i].m_groupName==CString(groupName))
		{
			break;
		}
	}
	if (i<m_arrGroups.GetSize())
	{
		m_arrGroups.RemoveAt(i);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CCheckScheme::DelCheckGroup(int idx)
{
	if(idx<0||idx>=m_arrGroups.GetSize())
		return FALSE;
	m_arrGroups.RemoveAt(idx);
	return TRUE;
}

CCheckScheme& GetCheckScheme()
{
	static CCheckScheme st_CheckScheme;
	
	return st_CheckScheme;
}