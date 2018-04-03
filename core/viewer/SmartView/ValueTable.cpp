// EBValueTable.cpp: implementation of the CValueTable class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "EditBase.h"
#include "ValueTable.h"
#include "SmartViewFunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define _ttof _tstof

void EXPORT_SMARTVIEW VariantToText(const CVariantEx& v, CString &text)
{
	switch(v.m_variant.vt) 
	{
	case VT_UI1:
		text.Format(_T("%i"),v.m_variant.bVal);
		break;
	case VT_UI2:
		text.Format(_T("%i"),v.m_variant.uiVal);
		break;
	case VT_UI4:
		text.Format(_T("%i"),v.m_variant.ulVal);
		break;
	case VT_UINT:
		text.Format(_T("%i"),v.m_variant.uintVal);
		break;
	case VT_I1:
		text.Format(_T("%c"),v.m_variant.cVal);
		break;
	case VT_I2:
		text.Format(_T("%i"),v.m_variant.iVal);
		break;	
	case VT_I4:
		text.Format(_T("%i"),(long)(_variant_t)v);
		break;
	case VT_INT:
		text.Format(_T("%i"),(long)(_variant_t)v);
		break;
	case VT_BOOL:
		text.Format(_T("%i"),(bool)(_variant_t)v);
		break;
	case VT_R4:
		text.Format(_T("%f"),(float)(_variant_t)v);
		break;
	case VT_R8:
		text.Format(_T("%f"),(double)(_variant_t)v);
		break;
	case VT_BSTR:
		text.Format(_T("%s"),(LPCTSTR)(_bstr_t)(_variant_t)v);
		break;
	default:
		text.Empty();
		break;
	}
}

MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// CVariantEx
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CVariantEx::CVariantEx()
{
	m_nExType = VT_EX_NULL;
	m_pExData = NULL;
}


CVariantEx::~CVariantEx()
{
	Clear();
}


CVariantEx::CVariantEx( const _variant_t& varSrc )
{
	m_variant = varSrc;

	m_nExType = VT_EX_NULL;
	m_pExData = NULL;
}


CVariantEx& CVariantEx::operator=( const _variant_t& varSrc )
{
	Clear();
	m_variant = varSrc;
	return *this;
}


CVariantEx::operator _variant_t()const
{
	return m_variant;
}

CVariantEx *CVariantEx::Clone()const
{
	CVariantEx *pNew = new CVariantEx;
	if( pNew )
	{
		pNew->m_variant = m_variant;
		pNew->SetExValue(m_nExType,m_pExData);
	}

	return pNew;
}


BOOL CVariantEx::CopyFrom(const CVariantEx *pObj)
{
	if( pObj )
	{
		m_variant = pObj->m_variant;
		SetExValue(pObj->m_nExType,pObj->m_pExData);
	}
	return TRUE;
}

BOOL CVariantEx::SetAsShape(CArray<PT_3DEX,PT_3DEX>& arrPts)
{
	return SetExValue(VT_EX_SHAPE, &arrPts);
}

BOOL CVariantEx::GetShape(CArray<PT_3DEX,PT_3DEX>& arrPts)const
{
	return GetExValue(&arrPts);
}


BOOL CVariantEx::SetAsBlob(const BYTE *buf, int nLen)
{
	Blob b(nLen, buf);
	return SetExValue(VT_EX_BLOB, &b);
}


const BYTE* CVariantEx::GetBlob(int& nLen)const
{
	if( m_nExType==VT_EX_BLOB && m_pExData!=NULL )
	{
		const Blob *p = (Blob*)m_pExData;
		nLen = p->nLen;
		return p->buf;
	}

	return NULL;
}

BOOL CVariantEx::CreateFromString(LPCTSTR text, int type)
{
	if (type!=VT_EX_BLOB && type!=VT_EX_SHAPE && type!=VT_EX_NULL)
	{
		if (type != VT_BSTR)
		{
			m_variant.vt = type;
		}
		
		switch(type) 
		{
		case VT_UI1:
			m_variant.bVal = (BYTE)_ttoi(text);
			break;
		case VT_UI2:
			m_variant.uiVal = (unsigned short)_ttoi(text);
			break;
		case VT_UI4:
			m_variant.ulVal = (unsigned long)_ttoi(text);
			break;
		case VT_UINT:
			m_variant.uintVal = (UINT)_ttoi(text);
			break;
		case VT_I1:
			m_variant.cVal = (CHAR)_ttoi(text);
			break;
		case VT_I2:
			m_variant.iVal = (short)_ttoi(text);
			break;	
		case VT_I4:
			m_variant.lVal = _ttoi(text);
			break;
		case VT_INT:
			m_variant.intVal = _ttoi(text);
			break;
		case VT_BOOL:
			m_variant.boolVal = (VARIANT_BOOL)_ttoi(text);
			break;
		case VT_R4:
			m_variant.fltVal = (float)_ttof(text);
			break;
		case VT_R8:
			m_variant.dblVal = _ttof(text);
			break;
		case VT_BSTR:
//			m_variant.bstrVal = (BSTR)text;
			m_variant.SetString(ConvertTCharToChar(text));
			break;
		default:
			return FALSE;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CVariantEx::operator==(const CVariantEx &v) const
{
	int thistype = GetType(), type = v.GetType();

	if (thistype==VT_EX_BLOB || thistype==VT_EX_SHAPE || type==VT_EX_BLOB || type==VT_EX_SHAPE)
		return FALSE;

	if ((thistype==VT_BSTR&&type!=VT_BSTR) || (thistype!=VT_BSTR&&type==VT_BSTR))
		return FALSE;
	
	CString str1,str2;
	VariantToText(*this,str1);
	VariantToText(v,str2);
	
	//字符串
	if (thistype == VT_BSTR)
	{
		if (str1.CompareNoCase(str2) == 0)
			return TRUE;
		else
			return FALSE;
	}
	
	//浮点
	if (thistype==VT_R4 || thistype==VT_R8)
	{		
		double d1,d2;
		d1 = _ttof(str1);
		d2 = _ttof(str2);
		if (fabs(d1-d2) < 0.0001)
			return TRUE;
		else
			return FALSE;
	}
	
	//整型
	long l1,l2;
	l1 = _ttof(str1);
	l2 = _ttof(str2);
	
	if (l1 == l2)
		return TRUE;
	else
		return FALSE;
}

BOOL CVariantEx::operator>(const CVariantEx &v) const
{
	int thistype = GetType(), type = v.GetType();
	if (thistype==VT_EX_BLOB || thistype==VT_EX_SHAPE || type==VT_EX_BLOB || type==VT_EX_SHAPE)
	{
		return FALSE;
	}

	if ((thistype==VT_BSTR&&type!=VT_BSTR) || (thistype!=VT_BSTR&&type==VT_BSTR))
		return FALSE;
	
	CString str1,str2;
	VariantToText(*this,str1);
	VariantToText(v,str2);
	
	//字符串
	if (thistype == VT_BSTR)
	{
		if (str1.CompareNoCase(str2) > 0)
			return TRUE;
		else
			return FALSE;
	}
	
	//浮点
	if (thistype==VT_R4 || thistype==VT_R8)
	{		
		double d1,d2;
		d1 = _ttof(str1);
		d2 = _ttof(str2);
		if (d1 > d2)
			return TRUE;
		else
			return FALSE;
	}
	
	//整型
	long l1,l2;
	l1 = _ttof(str1);
	l2 = _ttof(str2);
	
	if (l1 > l2)
		return TRUE;
	else
		return FALSE;
}

BOOL CVariantEx::operator<(const CVariantEx &v) const
{
	int thistype = GetType(), type = v.GetType();
	if (thistype==VT_EX_BLOB || thistype==VT_EX_SHAPE || type==VT_EX_BLOB || type==VT_EX_SHAPE)
	{
		return FALSE;
	}

	if ((thistype==VT_BSTR&&type!=VT_BSTR) || (thistype!=VT_BSTR&&type==VT_BSTR))
		return FALSE;
	
	CString str1,str2;
	VariantToText(*this,str1);
	VariantToText(v,str2);
	
	//字符串
	if (thistype == VT_BSTR)
	{
		if (str1.CompareNoCase(str2) < 0)
			return TRUE;
		else
			return FALSE;
	}
	
	//浮点
	if (thistype==VT_R4 || thistype==VT_R8)
	{		
		double d1,d2;
		d1 = _ttof(str1);
		d2 = _ttof(str2);
		if (d1 < d2)
			return TRUE;
		else
			return FALSE;
	}
	
	//整型
	long l1,l2;
	l1 = _ttof(str1);
	l2 = _ttof(str2);
	
	if (l1 < l2)
		return TRUE;
	else
		return FALSE;
}

BOOL CVariantEx::operator!=(const CVariantEx &v) const
{
	int thistype = GetType(), type = v.GetType();

	if (thistype==VT_EX_BLOB || thistype==VT_EX_SHAPE || type==VT_EX_BLOB || type==VT_EX_SHAPE)
		return FALSE;

	if ((thistype==VT_BSTR&&type!=VT_BSTR) || (thistype!=VT_BSTR&&type==VT_BSTR))
		return FALSE;

	CString str1,str2;
	VariantToText(*this,str1);
	VariantToText(v,str2);
	
	//字符串
	if (thistype == VT_BSTR)
	{
		if (str1.CompareNoCase(str2) != 0)
			return TRUE;
		else
			return FALSE;
	}
	
	//浮点
	if (thistype==VT_R4 || thistype==VT_R8)
	{		
		double d1,d2;
		d1 = _ttof(str1);
		d2 = _ttof(str2);
		if (d1-d2 > 0.0001)
			return TRUE;
		else
			return FALSE;
	}

	//整型
	long l1,l2;
	l1 = _ttof(str1);
	l2 = _ttof(str2);
	
	if (l1 != l2)
		return TRUE;
	else
		return FALSE;

}

int  CVariantEx::GetType()const
{
	if( m_nExType!=VT_EX_NULL )
		return m_nExType;

	return m_variant.vt;
}

// 扩展数据类型的支持函数
BOOL CVariantEx::SetExValue(int type, void *data)
{
	ClearExValue();

	if( type==VT_EX_SHAPE && data!=NULL )
	{
		CArray<PT_3DEX,PT_3DEX> *p = (CArray<PT_3DEX,PT_3DEX>*)data;
		CArray<PT_3DEX,PT_3DEX> *pNew = new CArray<PT_3DEX,PT_3DEX>;
		
		if( !pNew )return FALSE;

		pNew->Copy(*p);

		m_nExType = type;
		m_pExData = pNew;

		return TRUE;
	}
	else if( type==VT_EX_BLOB && data!=NULL )
	{
		Blob *p = (Blob*)data;
		Blob *pNew = new Blob;
		if( !pNew )return FALSE;

		pNew->Copy(p);
		
		m_nExType = type;
		m_pExData = pNew;
		
		return TRUE;
	}

	return FALSE;
}


BOOL CVariantEx::GetExValue(void *data)const
{
	if( m_nExType==VT_EX_SHAPE && m_pExData!=NULL && data!=NULL )
	{
		const CArray<PT_3DEX,PT_3DEX> *p = (CArray<PT_3DEX,PT_3DEX>*)m_pExData;
		CArray<PT_3DEX,PT_3DEX> *p2 = (CArray<PT_3DEX,PT_3DEX>*)data;

		p2->Copy(*p);
		return TRUE;
	}
	else if( m_nExType==VT_EX_BLOB && m_pExData!=NULL && data!=NULL )
	{
		const Blob *p = (Blob*)m_pExData;
		Blob *p2 = (Blob*)data;
		
		p2->Copy(p);
		return TRUE;
	}
	
	return FALSE;
}


BOOL CVariantEx::ClearExValue()
{
	if( m_nExType==VT_EX_SHAPE && m_pExData!=NULL )
	{
		CArray<PT_3DEX,PT_3DEX> *p = (CArray<PT_3DEX,PT_3DEX>*)m_pExData;
		delete p;

		m_nExType = VT_EX_NULL;
		m_pExData = NULL;

		return TRUE;
	}
	else if( m_nExType==VT_EX_BLOB && m_pExData!=NULL )
	{
		Blob *p = (Blob*)m_pExData;
		p->Clear();

		delete[] p;
		
		m_nExType = VT_EX_NULL;
		m_pExData = NULL;
		return TRUE;
	}

	return FALSE;
}

void CVariantEx::Clear()
{
	m_variant.Clear();
	ClearExValue();
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ValueItem::ValueItem()
{

}


ValueItem::~ValueItem()
{
	DeleteAll();
}

void ValueItem::DeleteAll()
{
	int nsz = arrPValues.GetSize();
	CVariantEx *p;
	for( int i=0; i<nsz; i++)
	{
		p = arrPValues.GetAt(i);
		if( p )delete p;
	}
	arrPValues.RemoveAll();
}


void ValueItem::SetValueCount(int ncount)
{
	int nsz = arrPValues.GetSize();
	if( nsz>ncount )
	{
		for( int i=nsz-1; i>=ncount; i--)
		{
			DelValue(i);
		}
	}
	else if( nsz<ncount )
	{
		for( int i=nsz; i<ncount; i++)
		{
			arrPValues.Add(NULL);
		}
	}
}


void ValueItem::AddValue(const CVariantEx* var)
{
	CVariantEx *p = var->Clone();
	if( !p )return;

	arrPValues.Add(p);
}


void ValueItem::SetValue(int i, const CVariantEx *var)
{
	int nsz = arrPValues.GetSize();
	if( i>=0 && i<=nsz )
	{
		CVariantEx *p = arrPValues.GetAt(i);
		if( p!=NULL )delete p;

		p = var->Clone();
		if( !p )return;
		arrPValues.SetAt(i,p);
	}
}


void ValueItem::DelValue(int i)
{
	int nsz = arrPValues.GetSize();
	if( i>=0 && i<nsz )
	{
		CVariantEx *p = arrPValues.GetAt(i);
		if( p )delete p;
		arrPValues.RemoveAt(i);
	}
}


void ValueItem::CopyFrom(const ValueItem& item)
{
	DeleteAll();
	int nsz = item.arrPValues.GetSize();
	arrPValues.SetSize(nsz);

	CVariantEx *p0, *p1;
	for( int i=0; i<nsz; i++)
	{
		p0 = item.arrPValues.GetAt(i);
		if (!p0) continue;

		p1 = p0->Clone();

		if( !p1 )arrPValues.SetAt(i,NULL);
		else arrPValues.SetAt(i,p1);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CValueTable::CValueTable()
{
	m_arrPValueItems.SetSize(0,256);

	m_bBeginAddValue = m_bBatchFirst = FALSE;
	m_bBeginBAddValue = FALSE;
	m_bBeginBGetValue = FALSE;
	m_bBeginBSetValue = FALSE;
}

CValueTable::~CValueTable()
{
	DelAll();
}

void CValueTable::CopyFrom(CValueTable& tab)
{
	DelAll();
	m_arrNames.Copy(tab.m_arrNames);
	m_arrFields.Copy(tab.m_arrFields);
	m_arrFieldType.Copy(tab.m_arrFieldType);
	m_arrDescs.Copy(tab.m_arrDescs);
	m_arrFieldLength.Copy(tab.m_arrFieldLength);

	int nsz = tab.m_arrPValueItems.GetSize();
	m_arrPValueItems.SetSize(nsz,256);

	ValueItem *p0, *p1;
	for( int i=0; i<nsz; i++)
	{
		p0 = tab.m_arrPValueItems.GetAt(i);
		p1 = new ValueItem;
		if( !p1 )m_arrPValueItems.SetAt(i,NULL);
		else
		{
			p1->CopyFrom(*p0);
			m_arrPValueItems.SetAt(i, p1);
		}
	}
}


void CValueTable::CopyFieldsFrom(CValueTable& tab)
{
	DelAll();
	m_arrNames.Copy(tab.m_arrNames);
	m_arrFields.Copy(tab.m_arrFields);
	m_arrFieldType.Copy(tab.m_arrFieldType);
	m_arrDescs.Copy(tab.m_arrDescs);
	m_arrFieldLength.Copy(tab.m_arrFieldLength);
}

void CValueTable::AddItemFromTab(CValueTable& tab, int idx)
{
	if( idx<0 )
	{
		if (m_arrFields.GetSize()<=0)
		{
			CopyFrom(tab);
			return;
		}
		const CVariantEx* pVar = NULL;
		ValueItem *p;
		for (int i=0;i<tab.GetItemCount();i++)
		{
			p = new ValueItem;
			if(!p) 
			{
				continue;
			}
			p->SetValueCount(m_arrFields.GetSize());	
			
			for (int j=0;j<m_arrFields.GetSize();j++)
			{			
				if(tab.GetValue(i,m_arrFields[j],pVar))
				{
					p->SetValue(j,pVar);	
				}
			}		
			m_arrPValueItems.Add(p);
		}	
	}
	else
	{
		if (m_arrFields.GetSize()<=0)
		{
			CopyFieldsFrom(tab);
		}

		const CVariantEx* pVar = NULL;
		ValueItem *p = new ValueItem;
		if( !p )return;

		p->SetValueCount(m_arrFields.GetSize());	
		
		for (int j=0;j<m_arrFields.GetSize();j++)
		{			
			if(tab.GetValue(idx,m_arrFields[j],pVar))
			{
				p->SetValue(j,pVar);	
			}
		}		
		m_arrPValueItems.Add(p);
	}
}


void CValueTable::SetItemFromTab(int idx0, CValueTable& tab, int idx)
{
	int nsz = m_arrPValueItems.GetSize();
	if( nsz<=0 || idx0<0 || idx0>=nsz )
	{
		AddItemFromTab(tab,idx);
		return;
	}
	
	const CVariantEx* pVar = NULL;
	ValueItem *p = m_arrPValueItems[idx0];	
	if( !p )return;	
	
	for (int j=0;j<m_arrFields.GetSize();j++)
	{			
		if(tab.GetValue(idx,m_arrFields[j],pVar))
		{
			p->SetValue(j,pVar);	
		}
	}
}

int	CValueTable::BeginAddValueItem()
{
	if( m_bBeginAddValue || m_bBeginBAddValue )
	{
		EndAddValueItem();
	}
	ValueItem *p = new ValueItem;
	if( !p )return -1;
	m_bBeginAddValue = TRUE;
	m_nCurAddIdx = 0;
	
	return m_arrPValueItems.Add(p);
}


int	CValueTable::BeginBatchAddValueItem()
{
	if( m_bBeginAddValue || m_bBeginBAddValue )
	{
		EndAddValueItem();
	}
	ValueItem *p = new ValueItem;
	if( !p )return -1;
	m_bBeginBAddValue = TRUE;
	m_nCurAddIdx = 0;

	return m_arrPValueItems.Add(p);
}

void CValueTable::AddValue(LPCTSTR field, CVariantEx* value, LPCTSTR name)
{
	if( !m_bBeginAddValue && !m_bBeginBAddValue )
		return;

	if( field==NULL || _tcslen(field)<=0 )
		return;

	ValueItem *p = m_arrPValueItems.GetAt(m_arrPValueItems.GetSize()-1);
	if( !p )return;

	// the first value define the value type of its field
	if( m_arrPValueItems.GetSize()==1 )
	{
		m_arrFields.Add(field);
		m_arrNames.Add(name/*==NULL?"":name*/);
		p->AddValue(value);
		m_arrFieldType.Add(value->GetType());
		m_arrDescs.Add(CString());
		m_arrFieldLength.Add(0);
	}
	else
	{
		if( m_bBeginBAddValue )
		{
			// use incorrectly BeginBatchAddValueItem
			ASSERT( value->GetType()==m_arrFieldType.GetAt(m_nCurAddIdx) );

			p->AddValue(value);
			m_nCurAddIdx++;
		}
		else
		{
			p->SetValueCount(m_arrFields.GetSize());
			int idx = FindField(field);
			if( idx>=0 && value->GetType()==m_arrFieldType.GetAt(idx) )
				p->SetValue(idx,value);
		}
	}
}


void CValueTable::AddValue(LPCTSTR field, CVariantEx* value, UINT nStrID)
{
	CString str = StrFromResID(nStrID);
	if( str.IsEmpty() )
		str = StrFromResID(nStrID);
	AddValue(field,value,str);
}


void CValueTable::EndAddValueItem()
{
	if( !m_bBeginAddValue && !m_bBeginBAddValue )
		return;

	int nsz = m_arrPValueItems.GetSize();
	ValueItem *p;
	if( nsz>0 )
	{
		// delete the last item if it is invalid
		p = m_arrPValueItems.GetAt(nsz-1);
		if( p && p->arrPValues.GetSize()==0 )
		{
			delete p;
			m_arrPValueItems.RemoveAt(nsz-1);
		}
		else if( p==NULL )
			m_arrPValueItems.RemoveAt(nsz-1);

		// delete fields that exist in part of value items
		else if( nsz>1 )
		{
			int nsz1 = p->arrPValues.GetSize();
			for( int i=nsz1-1; i>=0; i--)
			{
				if( p->arrPValues.GetAt(i)==NULL )
				{
					DelField(i);
				}
			}
		}
	}

	m_bBeginAddValue = FALSE;
	m_bBeginBAddValue = FALSE;
}


int	CValueTable::FindField(LPCTSTR field)const
{
	int nsz = m_arrFields.GetSize();
	for( int i=0; i<nsz; i++)
	{
		if( m_arrFields.GetAt(i).CompareNoCase(field)==0 )
			return i;
	}
	return -1;
}


void CValueTable::BeginBatchGetValueItem(BOOL bBatchFirst)
{
	m_bBeginBGetValue = TRUE;
	m_bBatchFirst = bBatchFirst;
	m_arrFldIdxForBatGet.RemoveAll();
	m_nCurGetIdx = 0;
}


void CValueTable::EndBatchGetValueItem()
{
	m_bBeginBGetValue = FALSE;
	m_bBatchFirst = FALSE;
}

BOOL CValueTable::GetValue(int item, LPCTSTR field, const CVariantEx*& value)
{
	if( item<0 || item>=m_arrPValueItems.GetSize() )
		return FALSE;

	ValueItem *p = m_arrPValueItems.GetAt(item);
	if( !p )return FALSE;

	if( m_bBeginBGetValue )
	{
		int idx;
		if( m_bBatchFirst )
		{
			idx = FindField(field);
			m_arrFldIdxForBatGet.Add(idx);
		}
		else
		{
			ASSERT(m_nCurGetIdx<m_arrFldIdxForBatGet.GetSize());
			
			idx = m_arrFldIdxForBatGet.GetAt(m_nCurGetIdx++);
		}

		if( idx<0 )return FALSE;
		CVariantEx *pv = p->arrPValues.GetAt(idx);
		if( !pv )return FALSE;

		value = pv;
	}
	else
	{
		int idx = FindField(field);
		
		if( idx<0 )return FALSE;
		CVariantEx *pv = p->arrPValues.GetAt(idx);
		if( !pv )return FALSE;
		
		value = pv;
	}
	
	return TRUE;
}


BOOL CValueTable::GetValueByName(int item, LPCTSTR name, const CVariantEx*& value)const
{
	if( item<0 || item>=m_arrPValueItems.GetSize() )
		return FALSE;
	
	ValueItem *p = m_arrPValueItems.GetAt(item);
	if( !p )return FALSE;

	for( int i=0; i<m_arrNames.GetSize(); i++)
	{
		if( m_arrNames[i].CompareNoCase(name)==0 )
		{
			CVariantEx *pv = p->arrPValues.GetAt(i);

			if( !pv )return FALSE;
			
			value = pv;
			
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CValueTable::GetValue(int item, int ifield, const CVariantEx*& value)const
{
	if( item<0 || item>=m_arrPValueItems.GetSize() )
		return FALSE;

	if( ifield<0 || ifield>=m_arrFields.GetSize() )
		return FALSE;
	
	ValueItem *p = m_arrPValueItems.GetAt(item);
	if( !p )return FALSE;

	CVariantEx *pv = p->arrPValues.GetAt(ifield);
	if( !pv )return FALSE;

	value = pv;
	
	return TRUE;
}


void CValueTable::BeginBatchSetValueItem(BOOL bBatchFirst)
{
	m_bBeginBSetValue = TRUE;
	m_bBatchFirst = bBatchFirst;
	m_arrFldIdxForBatSet.RemoveAll();
	m_nCurSetIdx = 0;
}


void CValueTable::EndBatchSetValueItem()
{
	m_bBeginBSetValue = FALSE;
	m_bBatchFirst = FALSE;
}



BOOL CValueTable::SetValue(int item, LPCTSTR field, const CVariantEx* value)
{
	if( item<0 || item>=m_arrPValueItems.GetSize() )
		return FALSE;
	
	ValueItem *p = m_arrPValueItems.GetAt(item);
	if( !p )return FALSE;
	
	if( m_bBeginBSetValue )
	{
		int idx;
		if( m_bBatchFirst )
		{
			idx = FindField(field);
			m_arrFldIdxForBatSet.Add(idx);
		}
		else
		{
			ASSERT(m_nCurSetIdx<m_arrFldIdxForBatGet.GetSize());
			idx = m_arrFldIdxForBatSet.GetAt(m_nCurSetIdx++);
		}
		
		if( idx<0 )return FALSE;
		p->SetValue(idx,value);
	}
	else
	{
		int idx = FindField(field);
		
		if( idx<0 )return FALSE;
		p->SetValue(idx,value);
	}
	
	return TRUE;
}



int CValueTable::GetFieldCount()const
{
	return m_arrFields.GetSize();
}

BOOL CValueTable::SetFieldName(LPCTSTR field,LPCTSTR name )
{
	if( m_arrFields.GetSize()<=0 )return FALSE;
	if( m_arrPValueItems.GetSize()<=0 )return FALSE;	
	
	for(int i=0;i<m_arrFields.GetSize();i++)
	{
		if (_tcsicmp(field, m_arrFields[i]) == 0)
		{
			m_arrNames.SetAt(i,name);
		}
	}	
	return TRUE;
}

BOOL CValueTable::SetFieldName(int i,LPCTSTR name )
{
	if( m_arrFields.GetSize()<=0 )return FALSE;
	if( m_arrPValueItems.GetSize()<=0 )return FALSE;	
	if( i<0 || i>=m_arrFields.GetSize() )return FALSE;
	m_arrNames.SetAt(i,name);
	return TRUE;
}


BOOL CValueTable::GetField(int i, CString &field, int& type, CString& name)const
{
	if( m_arrFields.GetSize()<=0 )return FALSE;
	if( m_arrPValueItems.GetSize()<=0 )return FALSE;

	if( i<0 || i>=m_arrFields.GetSize() )return FALSE;
	
	ValueItem *p = m_arrPValueItems.GetAt(0);
	if( !p )return FALSE;
	
	CVariantEx *pv = p->arrPValues.GetAt(i);
	if( !pv )return FALSE;
	type = pv->GetType();
	field = m_arrFields.GetAt(i);
	name  = m_arrNames.GetAt(i);
	return TRUE;
}


BOOL CValueTable::SetFieldDesc(int item, LPCTSTR field, LPCTSTR desc)
{
	if (m_arrFields.GetSize() <= 0)return FALSE;
	if (item<0 || item >= m_arrFields.GetSize())return FALSE;

	for (int i = 0; i<m_arrFields.GetSize(); i++)
	{
		if (_tcsicmp(field, m_arrFields[i]) == 0)
		{
			m_arrDescs.SetAt(i, CString(desc));
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CValueTable::GetFieldDesc(int item, LPCTSTR field, CString& desc)const
{
	if (m_arrFields.GetSize() <= 0)return FALSE;
	if (item<0 || item >= m_arrFields.GetSize())return FALSE;

	for (int i = 0; i<m_arrFields.GetSize(); i++)
	{
		if (_tcsicmp(field, m_arrFields[i]) == 0)
		{
			desc = m_arrDescs[i];
			return TRUE;
		}
	}

	return FALSE;
}


BOOL CValueTable::SetFieldLength(int item, LPCTSTR field, int length)
{
	if (m_arrFields.GetSize() <= 0)return FALSE;
	if (item<0 || item >= m_arrFields.GetSize())return FALSE;

	for (int i = 0; i<m_arrFields.GetSize(); i++)
	{
		if (_tcsicmp(field, m_arrFields[i]) == 0)
		{
			m_arrFieldLength.SetAt(i, length);
			return TRUE;
		}
	}
	return FALSE;
}


BOOL CValueTable::GetFieldLength(int item, LPCTSTR field, int& length)const
{
	if (m_arrFields.GetSize() <= 0)return FALSE;
	if (item<0 || item >= m_arrFields.GetSize())return FALSE;

	for (int i = 0; i<m_arrFields.GetSize(); i++)
	{
		if (_tcsicmp(field, m_arrFields[i]) == 0)
		{
			length = m_arrFieldLength[i];
			return TRUE;
		}
	}

	return FALSE;
}




void CValueTable::DelField(int i)
{
	int nsz = m_arrPValueItems.GetSize();
	ValueItem *p;
	for( int j=0; j<nsz; j++)
	{
		p = m_arrPValueItems.GetAt(j);
		if( !p )continue;
		
		p->DelValue(i);
	}
	m_arrFieldType.RemoveAt(i);
	m_arrNames.RemoveAt(i);
	m_arrFields.RemoveAt(i);
}

void CValueTable::DelField(LPCTSTR field)
{
	int idx = FindField(field);
	if( idx>=0 )DelField(idx);
}


void CValueTable::DelValueItem(int i)
{
	int nsz = m_arrPValueItems.GetSize();
	if( nsz<=0 || i<0 || i>=nsz )return;
	
	ValueItem *p = m_arrPValueItems.GetAt(i);
	if( !p )return;
		
	delete p;
	m_arrPValueItems.RemoveAt(i);
}


int CValueTable::GetItemCount()const
{
	return m_arrPValueItems.GetSize();
}

void CValueTable::DelAll()
{
	int nsz = m_arrPValueItems.GetSize();
	if( nsz<=0 )return;

	ValueItem *p;
	for( int j=0; j<nsz; j++)
	{
		p = m_arrPValueItems.GetAt(j);
		if( !p )continue;
		
		delete p;
	}
	m_arrPValueItems.RemoveAll();

	nsz = m_arrFields.GetSize();
	for( int i=nsz-1; i>=0; i--)
		DelField(i);
}


void CValueTable::AddValuesFromTab(CValueTable& tab)
{
	if (m_arrFields.GetSize()<=0||m_arrPValueItems.GetSize()<=0)
	{
		CopyFrom(tab);
		return;
	}
	
	if (tab.GetItemCount()<=0)
	{
		return;
	}
	if (tab.GetItemCount()!=1)
	{
		return;
	}

	ValueItem *p = m_arrPValueItems.GetAt(0);
	if( !p )return;

	CString field, name, data;
	int type, idx = -1;
	const CVariantEx *var;
	for (int i=0;i<tab.GetFieldCount();i++)
	{		
		tab.GetField(i,field,type,name);

		if (!tab.GetValue(0,i,var))
		{
			continue;
		}
		if( (idx=FindField(field))>=0 )
		{
			p->SetValue(idx,var);
			continue;
		}	
		
		m_arrFields.Add(field);
		m_arrNames.Add(name/*==NULL?"":name*/);
		p->AddValue(var);
		m_arrFieldType.Add(var->GetType());
	}
}


MyNameSpaceEnd