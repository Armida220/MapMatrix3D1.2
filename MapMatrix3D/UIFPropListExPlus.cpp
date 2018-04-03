// UIFPropListExPlus.cpp: implementation of the CUIFPropListExPlus class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "EditBase.h"
#include "UIFPropListExPlus.h"
#include "EditBaseDoc.h"
#include "UIFLayerProp.h"
#include "UIFBoolProp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIFPropListExPlus::CUIFPropListExPlus()
{
	m_pLoadParam = NULL;
}

CUIFPropListExPlus::~CUIFPropListExPlus()
{
	if( m_pLoadParam )delete m_pLoadParam;
}

void CUIFPropListExPlus::LoadParams(const CUIParam *param)
{
	ClearParams();
	
	if( !m_pLoadParam )m_pLoadParam = new CUIParam;
	if( !m_pLoadParam )return;
	m_pLoadParam->CopyFrom(param);
	CString strTitle(param->m_strTitle);
	strTitle+=StrFromResID(IDS_OPSETTING_TITLE);
	SetWindowText(strTitle);	
	int nsz = m_pLoadParam->m_arrItems.GetSize();
	int j;
	for( j=0; j<nsz; j++)
	{
		CUIParam::ParamItem item = m_pLoadParam->m_arrItems.GetAt(j);
// 		if (strcmp(item.field,PF_PENCODE)==0)
// 		{
// 			continue;
// 		}
		CUIFProp *pProp = CreateProp(item);
		if( pProp )AddCanHideProperty(pProp,TRUE);
	}
}

void CUIFPropListExPlus::ShowParams(const CUIParam *param)
{
	if( !param )return;
	if( !m_pLoadParam || m_pLoadParam->m_strID!=param->m_strID )
		return;
	
	int nsz = param->m_arrItems.GetSize(), ncount;
	int i;
	CArray<int,int> arrExistFlags;
	
	arrExistFlags.SetSize(nsz);
	memset(arrExistFlags.GetData(),0,sizeof(int)*nsz);
	
	CUIParam::ParamItem item, it;
	
	//隐藏参数表中不存在的属性项，并为存在的搭上标记
	while( 1 )
	{
		ncount = GetPropertyCount();
		for( i=0; i<ncount; i++)
		{
			CUIFProp *pProp = GetProperty(i);
			
			int idx = param->GetParamByTitle(pProp->GetName(),it);			
			if( idx>=0 )
			{
				arrExistFlags.SetAt(idx,1);
				SetPropValue((CUIFPropEx*)pProp,it);
			}
			else
			{
				HidePropertyByName(pProp->GetName());
				break;
			}
		}
		
		if( i>=ncount )break;
	}
	
	//将参数表中还没有显示的参数显示为属性项
	for( i=0; i<nsz; i++)
	{
		item = param->m_arrItems.GetAt(i);
		// 		if (strcmp(item.field,PF_PENCODE)==0)
		// 		{
		// 			continue;
		// 		}
		//	if(item.type==CUIParam::HotKeyType) continue;
		//已经存在了
		if( arrExistFlags.GetAt(i)==1 )continue;
		
		ShowPropByNameToIndex(item.title,i);
	}
}

void CUIFPropListExPlus::ClearParams()
{
	SetWindowText(StrFromResID(IDS_OPSETTING_TITLE));
	if( m_pLoadParam )delete m_pLoadParam;
	m_pLoadParam = NULL;
	
	SetCurSel(NULL);
	RemoveAll();
	RemoveAllHideProps();
	
	RedrawWindow();
}

void CUIFPropListExPlus::GetCmdParams(CValueTable &tab)
{
	if( !m_pLoadParam )return ;
	int nProp = GetPropertyCount();
	CUIParam::ParamItem item;
	for (int i=0;i<nProp;i++)
	{
		CUIFProp* pProp = GetProperty(i);
		if(pProp)
		{
			CString name = pProp->GetName();			
			if( m_pLoadParam->GetParamByTitle(name,item)<0 )
				continue;
			if( item.type==CUIParam::NoneType )
				continue;
			_variant_t var;
			
			switch( item.type )
			{
			case CUIParam::BoolType:
				//var = (bool)(((CUIFPropEx*)pProp)->GetCurSelOfCombo()==0?true:false);
				var = pProp->GetValue();
				break;
			case CUIParam::IntType:	
			case CUIParam::FloatType:
			case CUIParam::DoubleType:
			case CUIParam::StringType:
			case CUIParam::MultiEditType:
			case CUIParam::LayerNameType:
			case CUIParam::UsedLayerNameType:
			case CUIParam::MutiLayerNameType:
			case CUIParam::FontNameType:
			case CUIParam::CheckListType:
			case CUIParam::FileNameType:
			case CUIParam::PathNameType:
				var = pProp->GetValue();
				break;
			case CUIParam::OptionType:
				var = (long)item.data.pOptions->GetAt((long)((CUIFPropEx*)pProp)->GetCurSelOfCombo()).value;
				break;		
			}			
			tab.AddValue(item.field,&CVariantEx(var));		
		}
	}
}

extern void SetLayerPropFlags(CUIFLayerProp *pProp, DWORD flag);

CUIFProp * CUIFPropListExPlus::CreateProp(const CUIParam::ParamItem& item)
{
	CUIFProp *pProp = NULL;
	switch(item.type) 
	{
	case CUIParam::NoneType:
		break;
	case CUIParam::IntType:
		pProp = new CUIFPropEx(item.title,(long)item.data.nValue,item.desc);
		break;
	case CUIParam::BoolType:
		pProp = new CUIFBoolProp(item.title,item.data.bValue,item.desc);
		/*
		pProp = new CUIFPropEx(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);
		if( pProp )
		{
			pProp->AddOption(StrFromResID(IDS_YES));
			pProp->AddOption(StrFromResID(IDS_NO));
			pProp->AllowEdit(FALSE);
			pProp->SetCurSelOfCombo(item.data.bValue?0:1);
		}*/
		break;
	case CUIParam::FloatType:
		pProp = new CUIFPropEx(item.title,(float)item.data.fValue,item.desc);
		break;
	case CUIParam::DoubleType:
		pProp = new CUIFPropEx(item.title,(double)item.data.lfValue,item.desc);
		break;
	case CUIParam::StringType:
		pProp = new CUIFPropEx(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		break;
	case CUIParam::MultiEditType:
		pProp = new CUIFMultiEditProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		break;
	case CUIParam::LayerNameType:
	case CUIParam::UsedLayerNameType:
		pProp = new CUIFLayerProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		{
			CDlgDoc *pDoc = GetActiveDlgDoc();
			if( pDoc )
			{
				((CUIFLayerProp*)pProp)->SetLayersOption(pDoc->GetDlgDataSource());
			}
		}
		if( item.type==CUIParam::UsedLayerNameType )
		{
			((CUIFLayerProp*)pProp)->EnableNotEmpty(TRUE);
			((CUIFLayerProp*)pProp)->EnableNotLocalLayers(TRUE);
		}
		SetLayerPropFlags((CUIFLayerProp*)pProp,item.exParam);
		break;
	case CUIParam::MutiLayerNameType:
		pProp = new CUIFLayerProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);
		((CUIFLayerProp*)pProp)->EnableMutiSelMode(TRUE);
		{
			CDlgDoc *pDoc = GetActiveDlgDoc();
			if( pDoc )
			{
				((CUIFLayerProp*)pProp)->SetLayersOption(pDoc->GetDlgDataSource());
			}
		}
		SetLayerPropFlags((CUIFLayerProp*)pProp,item.exParam);
		break;
	case CUIParam::FontNameType:
		pProp = new CUIFFontNameProp(item.title,(LPCTSTR)*item.data.pStrValue,item.desc);

		((CUIFFontNameProp*)pProp)->LoadFontNames();
		break;
	case CUIParam::ButtonType:
		pProp = new CUIFButtonProp(item.title,item.desc);
		break;
	case CUIParam::OptionType:
		pProp = new CUIFPropEx(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);
		if( pProp )
		{
			int nsz = item.data.pOptions->GetSize();
			int nsel = 0;
			for( int i=0; i<nsz; i++)
			{
				pProp->AddOption(item.data.pOptions->GetAt(i).desc);
				if( item.data.pOptions->GetAt(i).flag==1 )nsel = i;
			}
			pProp->AllowEdit(FALSE);
			((CUIFPropEx*)pProp)->SetCurSelOfCombo(nsel);
		}
		break;
	case CUIParam::CheckListType:
		pProp = new CUIFCheckListProp(item.title,(LPCTSTR)StrFromResID(IDS_NO),item.desc);
		if( pProp )
		{
			CUIFCheckListProp *p = (CUIFCheckListProp*)pProp;
			
			CStringArray names;
			CArray<int,int> flags;
			int nsz = item.data.pOptions->GetSize();
			
			names.SetSize(nsz);
			flags.SetSize(nsz);
			
			for( int i=0; i<nsz; i++)
			{
				names[i] = item.data.pOptions->GetAt(i).desc;
				flags[i] = item.data.pOptions->GetAt(i).value;
			}
			p->SetList(names,&flags);
		}
		break;
	case CUIParam::FileNameType:
		pProp = new CUIFFilePropEx(item.title,TRUE,*item.data.FileName.pFileName,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,*item.data.FileName.pFileFilter,item.desc);
		break;
	case CUIParam::PathNameType:
		pProp = new CUIFFilePropEx(item.title,(LPCTSTR)*item.data.pStrValue,0,item.desc);
		break;
	case CUIParam::ColorType:
		pProp = new CMFCPropertyGridColorProperty(item.title, item.data.lValue, 0, item.desc);
		((CMFCPropertyGridColorProperty*)pProp)->EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);
		break;
	default:;
	}
	return pProp;
}

void CUIFPropListExPlus::SetPropValue(CUIFProp *pProp, const CUIParam::ParamItem& item)
{
	switch(item.type) 
	{
	case CUIParam::NoneType:
		break;
	case CUIParam::IntType:
		if( item.data.nValue!=(long)(_variant_t)pProp->GetValue() )
			pProp->SetValue((long)item.data.nValue);
		break;
	case CUIParam::BoolType:
		//if( item.data.bValue!=((long)pProp->GetCurSelOfCombo()==0) )
		//	pProp->SetCurSelOfCombo(item.data.bValue?0:1);
		pProp->SetValue(item.data.bValue ? VARIANT_TRUE : VARIANT_FALSE);
		break;
	case CUIParam::ColorType:
		pProp->SetValue(item.data.lValue);
		break;
	case CUIParam::FloatType:
		if( item.data.fValue!=(float)(_variant_t)pProp->GetValue() )
			pProp->SetValue((float)item.data.fValue);
		break;
	case CUIParam::DoubleType:
		if( item.data.lfValue!=(double)(_variant_t)pProp->GetValue() )
			pProp->SetValue((double)item.data.lfValue);
		break;
	case CUIParam::MultiEditType:
	case CUIParam::StringType:
		if( item.data.pStrValue->CompareNoCase((LPCTSTR)(_bstr_t)pProp->GetValue())!=0 )
			pProp->SetValue((LPCTSTR)*item.data.pStrValue);
		break;
	case CUIParam::OptionType:
		{
			int nsz = item.data.pOptions->GetSize();
			int nsel = 0;
			for( int i=0; i<nsz; i++)
			{
				if( item.data.pOptions->GetAt(i).flag==1 )nsel = i;
			}
			if( nsel!=((CUIFPropEx*)pProp)->GetCurSelOfCombo() )
				((CUIFPropEx*)pProp)->SetCurSelOfCombo(nsel);
		}
		break;
	case CUIParam::CheckListType:
		{
			CUIFCheckListProp *p = DYNAMIC_DOWNCAST(CUIFCheckListProp,pProp);
			
			if( p )
			{
				CArray<int,int> flags;
				int nsz = item.data.pOptions->GetSize();
				
				flags.SetSize(nsz);
				
				for( int i=0; i<nsz; i++)
				{
					flags[i] = item.data.pOptions->GetAt(i).value;
				}
				p->SetValue(flags);
			}
		}
		break;
	default:;
	}
	return;
}


const CUIParam *CUIFPropListExPlus::GetOriginalParams()
{
	return m_pLoadParam;
}