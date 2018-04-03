// UIParam.cpp: implementation of the CUIParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIParam.h"
#include "Resource.h"
#include "Functions_temp.h"

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIParam::CUIParam()
{
	m_nRetItem = -1;
}

CUIParam::~CUIParam()
{
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		ParamItem item = m_arrItems.GetAt(i);
		item.Clear();
	}

	m_arrItems.RemoveAll();

	m_optItemInAdding.Clear();
}


void CUIParam::CopyFrom(const CUIParam *p)
{
	m_strID = p->m_strID;
	m_strTitle = p->m_strTitle;
	m_strTip = p->m_strTip;

	m_optItemInAdding.CopyFrom(p->m_optItemInAdding);
	
	int nsz = p->m_arrItems.GetSize();
	m_arrItems.RemoveAll();
	for( int i=0; i<nsz; i++)
	{
		ParamItem item;
		item.CopyFrom(p->m_arrItems.GetAt(i));
		m_arrItems.Add(item);
	}
}


void CUIParam::SetOwnerID(LPCTSTR id, LPCTSTR title)
{
	m_strID = id;
	m_strTitle = title;
}

void CUIParam::AddParam(LPCTSTR field, bool value, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = BoolType;
	item.data.bValue = value?TRUE:FALSE;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;

	m_arrItems.Add(item);
}

void CUIParam::AddParam(LPCTSTR field, int  value, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = IntType;
	item.data.nValue = value;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;

	m_arrItems.Add(item);	
}

void CUIParam::AddParam(LPCTSTR field, long  value, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = IntType;
	item.data.nValue = value;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);	
}

void CUIParam::AddColorParam(LPCTSTR field, long  value, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = ColorType;
	item.data.lValue = value;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);	
}

void CUIParam::AddParam(LPCTSTR field, float value, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = FloatType;
	item.data.fValue = value;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);	
}

void CUIParam::AddParam(LPCTSTR field, double value, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = DoubleType;
	item.data.lfValue = value;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);
}

void CUIParam::AddParam(LPCTSTR field, PT_3D point, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = PointType;
	COPY_3DPT(item.data.point,point);
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);	
}

void CUIParam::AddParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = StringType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);	
}


void CUIParam::AddLayerNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, DWORD checkFlags, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = LayerNameType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;

	if(checkFlags==0)
		checkFlags = 0xfffffffe;
	item.exParam = checkFlags;
	
	m_arrItems.Add(item);	
}

void CUIParam::AddLayerNameParamEx(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, DWORD checkFlags, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = MutiLayerNameType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;

	if(checkFlags==0)
		checkFlags = 0xfffffffe;
	item.exParam = checkFlags;
	
	m_arrItems.Add(item);	
}

void CUIParam::AddUsedLayerNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = UsedLayerNameType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	
	m_arrItems.Add(item);	
}


void CUIParam::AddMultiEditParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = MultiEditType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;

	m_arrItems.Add(item);	
}


void CUIParam::AddFileNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR filter, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = FileNameType;
	item.data.FileName.pFileName = new CString();
	item.data.FileName.pFileFilter = new CString();
	if( !item.data.FileName.pFileName || !item.data.FileName.pFileFilter )
		return;
	*item.data.FileName.pFileName = str;
	*item.data.FileName.pFileFilter = filter;

	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	m_arrItems.Add(item);
}


void CUIParam::AddPathNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = PathNameType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;
	m_arrItems.Add(item);
}


void CUIParam::AddButtonParam(LPCTSTR field, LPCTSTR title, LPCTSTR desc)
{
	ParamItem item;
	item.type = ButtonType;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	
	m_arrItems.Add(item);
}



void CUIParam::AddFontNameParam(LPCTSTR field, LPCTSTR str, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	ParamItem item;
	item.type = FontNameType;
	item.data.pStrValue = new CString();
	if( !item.data.pStrValue )return;
	*item.data.pStrValue = str;
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	item.accelKey = accelKey;
	item.bShow = bShow;

	m_arrItems.Add(item);	
}


void CUIParam::AddParam(LPCTSTR field, char key, LPCTSTR title, LPCTSTR desc)
{
	ParamItem item;
	item.type = HotKeyType;
	item.data.chHotKey = key;	
	_tcsncpy(item.field,field,sizeof(item.field)-1);
	_tcsncpy(item.title,title,sizeof(item.title)-1);
	if( desc )_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
	
	m_arrItems.Add(item);	

}
void CUIParam::BeginOptionParam(LPCTSTR field, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	m_optItemInAdding.type = OptionType;
	_tcsncpy(m_optItemInAdding.field,field,sizeof(m_optItemInAdding.field)-1);
	_tcsncpy(m_optItemInAdding.title,title,sizeof(m_optItemInAdding.title)-1);
	if( desc )_tcsncpy(m_optItemInAdding.desc,desc,sizeof(m_optItemInAdding.desc)-1);
	m_optItemInAdding.accelKey = accelKey;
	m_optItemInAdding.bShow = bShow;

	if( m_optItemInAdding.data.pOptions )
		m_optItemInAdding.data.pOptions->RemoveAll();
	else
		m_optItemInAdding.data.pOptions = new CArray<Option,Option>;
}

void CUIParam::AddOption(LPCTSTR desc, int value, int accelkey, BOOL bDefOpt)
{
	if( m_optItemInAdding.data.pOptions )
	{
		Option item;
		_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
		item.value = value;
		item.accelkey = accelkey;
		item.flag = bDefOpt?1:0;

		m_optItemInAdding.data.pOptions->Add(item);
	}
}

void CUIParam::EndOptionParam()
{
	if( m_optItemInAdding.data.pOptions && 
		m_optItemInAdding.data.pOptions->GetSize()>0 )
	{
		m_arrItems.Add(m_optItemInAdding);
		m_optItemInAdding = ParamItem();
	}
	else
		m_optItemInAdding.Clear();
}


void CUIParam::SetOptionDefault(LPCTSTR field, int value)
{
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CUIParam::ParamItem item1 = m_arrItems.GetAt(i);
		if( _tcsicmp(item1.field,field)==0 && item1.type==OptionType && item1.data.pOptions!=NULL )
		{
			for( int j=0; j<item1.data.pOptions->GetSize(); j++)
			{
				Option opt = item1.data.pOptions->GetAt(j);
				if( opt.value==value )
					opt.flag = 1;
				else
					opt.flag = 0;

				item1.data.pOptions->SetAt(j,opt);
			}
			return;
		}
	}
}

void CUIParam::BeginCheckListParam(LPCTSTR field, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	m_optItemInAdding.type = CheckListType;
	_tcsncpy(m_optItemInAdding.field,field,sizeof(m_optItemInAdding.field)-1);
	_tcsncpy(m_optItemInAdding.title,title,sizeof(m_optItemInAdding.title)-1);
	if( desc )_tcsncpy(m_optItemInAdding.desc,desc,sizeof(m_optItemInAdding.desc)-1);
	m_optItemInAdding.accelKey = accelKey;
	m_optItemInAdding.bShow = bShow;
	
	if( m_optItemInAdding.data.pOptions )
		m_optItemInAdding.data.pOptions->RemoveAll();
	else
		m_optItemInAdding.data.pOptions = new CArray<Option,Option>;
}

void CUIParam::AddCheckItem(LPCTSTR desc, int value)
{
	if( m_optItemInAdding.data.pOptions )
	{
		Option item;
		_tcsncpy(item.desc,desc,sizeof(item.desc)-1);
		item.value = value;
		item.accelkey = 0;
		item.flag = 0;
		
		m_optItemInAdding.data.pOptions->Add(item);
	}
}

void CUIParam::EndCheckListParam()
{
	if( m_optItemInAdding.data.pOptions && 
		m_optItemInAdding.data.pOptions->GetSize()>0 )
	{
		m_arrItems.Add(m_optItemInAdding);
		m_optItemInAdding = ParamItem();
	}
	else
		m_optItemInAdding.Clear();
}




void CUIParam::AddLineTypeParam(LPCTSTR field, int nDefPenCode, LPCTSTR title, LPCTSTR desc, char accelKey, bool bShow)
{
	BeginOptionParam(field,title,desc,accelKey,bShow);

	AddOption(StrFromLocalResID(IDS_CMDTIP_LINE),penLine,'L',nDefPenCode==penLine);
	AddOption(StrFromLocalResID(IDS_CMDTIP_SYNCH),penStream,'S',nDefPenCode==penStream);
	AddOption(StrFromLocalResID(IDS_CMDTIP_CURVE),penSpline,'V',nDefPenCode==penSpline);
	AddOption(StrFromLocalResID(IDS_CMDTIP_ARC),penArc,'A',nDefPenCode==penArc);
	AddOption(StrFromLocalResID(IDS_CMDTIP_3ARC),pen3PArc,'Z',nDefPenCode==pen3PArc);

	EndOptionParam();
}


int CUIParam::GetParam(LPCTSTR field, CUIParam::ParamItem& item)const
{
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		CUIParam::ParamItem item1 = m_arrItems.GetAt(i);
		if( _tcsicmp(item1.field,field)==0 )
		{
			item = item1;
			return i;
		}
	}

	return -1;
}


int CUIParam::GetParamByTitle(LPCTSTR title, CUIParam::ParamItem& item)const
{
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		ParamItem item1 = m_arrItems.GetAt(i);
		if( _tcsicmp(item1.title,title)==0 )
		{
			item = item1;
			return i;
		}
	}
	
	return -1;
}


void CUIParam::DelParam(LPCTSTR field)
{
	int nsz = m_arrItems.GetSize();
	for( int i=0; i<nsz; i++)
	{
		ParamItem item = m_arrItems.GetAt(i);
		if( _tcsicmp(item.field,field)==0 )
		{
			m_arrItems.RemoveAt(i);
			if( item.type==StringType && item.data.pStrValue )
				delete item.data.pStrValue;
			else if( item.type==OptionType && item.data.pOptions )
				delete item.data.pOptions;
			break;
		}
	}
}


MyNameSpaceEnd
