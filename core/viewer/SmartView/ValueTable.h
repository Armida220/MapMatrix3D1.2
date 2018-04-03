// EBValueTable.h: interface for the CValueTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EBVALUETABLE_H__6C1C9DDF_4DDE_4E6F_906D_A884DDC601FF__INCLUDED_)
#define AFX_EBVALUETABLE_H__6C1C9DDF_4DDE_4E6F_906D_A884DDC601FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

MyNameSpaceBegin

#define VT_EX_NULL				0
#define VT_EX_SHAPE				0x10000
#define VT_EX_BLOB				0x20000


class EXPORT_SMARTVIEW CVariantEx
{
public:	
	struct Blob
	{
		int nLen;
		BYTE *buf;
		Blob()
		{
			nLen = 0;
			buf = NULL;
		}
		Blob(int n, const BYTE *p)
		{
			nLen = n;
			buf = (BYTE*)p;
		}
		BOOL Copy(const Blob *other)
		{
			if( other->nLen>0 && other->buf!=NULL )
			{
				BYTE *pNewBuf = new BYTE[other->nLen];
				if( pNewBuf!=NULL )
				{
					memcpy(pNewBuf,other->buf,other->nLen);

					if( buf!=NULL )delete[] buf;
					buf = pNewBuf;
					nLen = other->nLen;
				}

				return FALSE;
			}
			return TRUE;
		}
		BOOL Clear()
		{
			if( buf!=NULL )delete[] buf;
			buf = NULL;
			nLen = 0;
			return TRUE;
		}
	};

	CVariantEx();
	virtual ~CVariantEx();

	// 兼容性/格式转换的函数
	CVariantEx( const _variant_t& varSrc );	
	CVariantEx& operator=( const _variant_t& varSrc );	

	operator _variant_t()const;

	// 扩展数据类型的支持函数；如果需要支持新的数据类型，就需要在派生类中重载这几个函数
	virtual BOOL SetExValue(int type, void *data);
	virtual BOOL GetExValue(void *data)const;
	virtual BOOL ClearExValue();
	virtual CVariantEx *CVariantEx::Clone()const;
	virtual BOOL CopyFrom(const CVariantEx *pObj);

	// 当前类的实现中，支持 CArray<PT_3DEX,PT_3DEX> 类型的数据
	BOOL SetAsShape(CArray<PT_3DEX,PT_3DEX>& arrPts);
	BOOL GetShape(CArray<PT_3DEX,PT_3DEX>& arrPts)const;
	int  GetType()const;

	BOOL SetAsBlob(const BYTE *buf, int nLen);
	const BYTE* GetBlob(int& nLen)const;

	BOOL CreateFromString(LPCTSTR text, int type);
	BOOL operator==(const CVariantEx &v) const;
	BOOL operator>(const CVariantEx &v) const;
	BOOL operator<(const CVariantEx &v) const;
	BOOL operator!=(const CVariantEx &v) const;

	void Clear();

	// base variant
	_variant_t m_variant;

	// extension value
	int m_nExType;
	void *m_pExData;
};

struct ValueItem
{
	ValueItem();
	~ValueItem();
	void AddValue(const CVariantEx *var);
	void SetValue(int i, const CVariantEx *var);
	void DelValue(int i);
	void CopyFrom(const ValueItem& item);
	void DeleteAll();
	void SetValueCount(int ncount);
	CArray<CVariantEx*, CVariantEx*> arrPValues;
};

class EXPORT_SMARTVIEW CValueTable  
{
public:
	CValueTable();
	virtual ~CValueTable();
	void CopyFrom(CValueTable& tab);
	void CopyFieldsFrom(CValueTable& tab);
	//将tab中的字段和第一项的值追加到本地数据，要求本地也只有一项数据
	void AddValuesFromTab(CValueTable& tab);
	void AddItemFromTab(CValueTable& tab, int idx=-1);
	void SetItemFromTab(int idx0, CValueTable& tab, int idx);
	// add a value item, the first item define fields and their value types.
	int	 BeginAddValueItem();
	// add in batches, used only if all value items have the same fields, the same value types, 
	// and the same adding order.
	int  BeginBatchAddValueItem();
	// these two Addxxx functions must be used between a pair of 
	// BeginAddValueItem or BeginBatchAddValueItem and EndAddValueItem.
	void AddValue(LPCTSTR field, CVariantEx* value, LPCTSTR name=NULL);
	void AddValue(LPCTSTR field, CVariantEx* value, UINT nStrID);
	void EndAddValueItem();

	// these two Getxxx functions must be used between a pair of 
	// BeginBatchGetValueItem and EndBatchGetValueItem only if
	// you are getting values in batches. 
	BOOL GetValue(int item, LPCTSTR field, const CVariantEx*& value);
	BOOL GetValue(int item, int ifield, const CVariantEx*& value)const;
	BOOL GetValueByName(int item, LPCTSTR name, const CVariantEx*& value)const;
	void BeginBatchGetValueItem(BOOL bBatchFirst);
	void EndBatchGetValueItem();

	// these two Setxxx functions must be used between a pair of 
	// BeginBatchSetValueItem and EndBatchSetValueItem only if
	// you are setting values in batches.	
	BOOL SetValue(int item, LPCTSTR field, const CVariantEx* value);
	void BeginBatchSetValueItem(BOOL bBatchFirst);
	void EndBatchSetValueItem();
	BOOL SetFieldDesc(int item, LPCTSTR field, LPCTSTR desc);
	BOOL GetFieldDesc(int item, LPCTSTR field, CString& desc)const;
	BOOL SetFieldLength(int item, LPCTSTR field, int length);
	BOOL GetFieldLength(int item, LPCTSTR field, int& length)const;

	//manage fileds
	int  GetFieldCount()const;
	BOOL SetFieldName(LPCTSTR field,LPCTSTR name);
	BOOL SetFieldName(int i,LPCTSTR name );
	BOOL GetField(int i, CString &field, int& type, CString& name)const;
	void DelField(LPCTSTR field);
	int	 FindField(LPCTSTR field)const;
	
	//manage value items
	int  GetItemCount()const;
	void DelValueItem(int i);
	void DelAll();

protected:
	void DelField(int i);

protected:
	CStringArray m_arrFields;
	CStringArray m_arrNames;
	CStringArray m_arrDescs;
	CArray<int,int> m_arrFieldLength;
	CArray<int,int> m_arrFieldType;
	CArray<ValueItem*,ValueItem*> m_arrPValueItems;

	BOOL m_bBeginAddValue;
	BOOL m_bBeginBAddValue;
	BOOL m_bBeginBGetValue;
	BOOL m_bBeginBSetValue;
	BOOL m_bBatchFirst;

	int m_nCurAddIdx, m_nCurGetIdx, m_nCurSetIdx;
	CArray<int,int> m_arrFldIdxForBatGet;
	CArray<int,int> m_arrFldIdxForBatSet;
};

MyNameSpaceEnd

#endif // !defined(AFX_EBVALUETABLE_H__6C1C9DDF_4DDE_4E6F_906D_A884DDC601FF__INCLUDED_)
