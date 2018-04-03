// StereoParamFile.cpp: implementation of the CStereoParamFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StereoParamFile.h"
#include "Markup.h"

using namespace EditBase;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStereoParamFile::CStereoParamFile()
{
	//获取历史文件目录
	TCHAR module[_MAX_PATH]={0};	
	GetModuleFileName(NULL,module,_MAX_PATH);
	CString dir = module;
	int pos = dir.ReverseFind(_T('.'));
	if( pos>0 )
	{
		dir = dir.Left(pos+1);
	}
	else
		dir = "";

	dir += "stereoParam.txt";

	m_path = dir;
	Load();

	m_nCurItem = -1;
}

CStereoParamFile::~CStereoParamFile()
{
	for( int i=0; i<m_arrPItems.GetSize(); i++)
	{
		delete m_arrPItems.GetAt(i);
	}
	m_arrPItems.RemoveAll();
}

void CStereoParamFile::Load()
{
	CMarkup file;
	file.Load(m_path);
	
	if( file.FindElem("StereoParamFile",TRUE) )
	{
		file.IntoElem();
		while( file.FindElem() )
		{
			StereoParamItem *pItem = new StereoParamItem();
			if( pItem )
			{
				pItem->id = file.GetTagName();
				file.IntoElem();

				while( file.FindElem() )
				{
					pItem->names.Add(file.GetTagName());
					pItem->values.Add(file.GetData());
				}

				file.OutOfElem();
			}
			m_arrPItems.Add(pItem);
		}
	}
}

int CStereoParamFile::SetStereoID(LPCTSTR id)
{
	if( id==NULL || strlen(id)<=0 )
		return 0;

	//标签名字必须字母开头
	CString strID = CString("A") + id;
	strID.Remove(' ');

	for( int i=0; i<m_arrPItems.GetSize(); i++)
	{
		if( m_arrPItems[i]->id.CompareNoCase(strID)==0 )
		{
			break;
		}
	}

	if( i<m_arrPItems.GetSize() )
	{
		StereoParamItem *pItem = m_arrPItems.GetAt(i);
		m_arrPItems.RemoveAt(i);
		m_arrPItems.InsertAt(0,pItem);
		m_nCurItem = 0;

		return 1;
	}
	else
	{
		StereoParamItem *pItem = new StereoParamItem();
		if( pItem )
		{
			pItem->id = strID;
			m_arrPItems.InsertAt(0,pItem);
			m_nCurItem = 0;
		}
		return 0;
	}
}

CString CStereoParamFile::ReadParam(LPCTSTR name)
{
	if( m_nCurItem<0 || m_nCurItem>=m_arrPItems.GetSize() )
		return CString();

	StereoParamItem *pItem = m_arrPItems.GetAt(m_nCurItem);
	for( int i=0; i<pItem->names.GetSize(); i++)
	{
		if( pItem->names.GetAt(i).CompareNoCase(name)==0 )
		{
			return pItem->values[i];
		}
	}
	return CString();
}

double CStereoParamFile::ReadFloatParam(LPCTSTR name)
{
	return atof(ReadParam(name));
}

int CStereoParamFile::ReadIntParam(LPCTSTR name)
{
	return atol(ReadParam(name));
}

void CStereoParamFile::WriteParam(LPCTSTR name, LPCTSTR value)
{
	if( m_nCurItem<0 || m_nCurItem>=m_arrPItems.GetSize() )
		return;

	StereoParamItem *pItem = m_arrPItems.GetAt(m_nCurItem);
	for( int i=0; i<pItem->names.GetSize(); i++)
	{
		if( pItem->names.GetAt(i).CompareNoCase(name)==0 )
		{
			pItem->values[i] = value;
			return;
		}
	}
	pItem->names.Add(name);
	pItem->values.Add(value);
}

void CStereoParamFile::WriteParam(LPCTSTR name, double value)
{
	CString strValue;
	strValue.Format("%lf",value);

	WriteParam(name,strValue);
}

void CStereoParamFile::WriteParam(LPCTSTR name, int value)
{
	CString strValue;
	strValue.Format("%d",value);
	
	WriteParam(name,strValue);
}

void CStereoParamFile::Save()
{
	CMarkup file;
	file.AddElem("StereoParamFile");
	file.IntoElem();

	int nSave = 0;
	for( int i=0; i<m_arrPItems.GetSize(); i++)
	{
		StereoParamItem *pItem = m_arrPItems.GetAt(i);
		if( pItem->names.GetSize()<=0 )continue;
		if( nSave>=100 )continue;

		file.AddElem(pItem->id);
		file.IntoElem();

		for( int i=0; i<pItem->names.GetSize(); i++)
		{
			file.AddElem(pItem->names[i],pItem->values[i]);
		}

		file.OutOfElem();

		nSave++;
	}

	file.OutOfElem();

	file.Save(m_path);
}
