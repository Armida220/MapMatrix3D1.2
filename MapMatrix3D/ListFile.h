// ListFile.h: interface for the CListFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTFILE_H__C61782FA_9629_4696_9A6B_15F579CFE23E__INCLUDED_)
#define AFX_LISTFILE_H__C61782FA_9629_4696_9A6B_15F579CFE23E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CListFile  
{
public:
	CListFile();
	virtual ~CListFile();

	BOOL Open(const char *name);
	void Close();
	const char* FindMatchItem(const char* item, BOOL bNoCase=TRUE, BOOL bMatchOne=TRUE );

protected:
	CStringArray m_lstOne;
	CStringArray m_lstTwo;

	FILE *m_fp;
};

typedef struct tag_LISTFILEEX 
{
	CString m_strGroup;//组名
	CString m_strColour;//颜色
	BOOL m_bHasLayer;//是否有具体的层
	int m_nSize;
	CStringArray m_lstOne;
	CStringArray m_lstTwo;
}LISTFILEEX,*PLISTFILEEX ;
class CListFileEx : public CListFile
{
public:
	CListFileEx();
	virtual ~CListFileEx();
	BOOL Open(const char *name);
	void Close();
	int GetLayerSizeByIndex(int nindex);
	BOOL Split(CString source, LPCTSTR divKey);
	CArray<LISTFILEEX* , LISTFILEEX*> m_ListFile;
protected:
private:
};
class CMultiListFile
{
public:
	CMultiListFile();
	virtual ~CMultiListFile();
	
	BOOL Open(const char *name);
	BOOL Save(const char *name);
	BOOL Read(FILE *fp);
	BOOL Write(FILE *fp);
	void Clear();
	CStringArray* FindRowItem(const char* instr, int inidx, BOOL bNoCase=TRUE);
	int FindIntItem(const char* instr, int inidx, int outidx, int value, BOOL bNoCase=TRUE);
	double FindFloatItem(const char* instr, int inidx, int outidx, double value, BOOL bNoCase=TRUE);
	CString FindTextItem(const char* instr, int inidx, int outidx, const char* value, BOOL bNoCase=TRUE);
	CString FindMatchItem(const char* item, int srcIdx=0, int destIdx=1, BOOL bNoCase=TRUE );
	CString FindMatchItem2(const char* item1, int srcIdx1, const char* item2, int srcIdx2, int destIdx=1, BOOL bNoCase=TRUE );
	CStringArray * FindMatchRow(const char* item1, int srcIdx1, const char* item2, int srcIdx2, BOOL bNoCase=TRUE );

	CStringArray *GetRow(int nRow);
	int GetRowCount();
	int GetIntItem(int nRow, int nCol, int value);
	double GetFloatItem(int nRow, int nCol, double value);
	CString GetTextItem(int nRow, int nCol, const char* value);
	void AddRow(const CStringArray& a){
		CStringArray *p = new CStringArray;
		if( p )
		{
			p->Copy(a);
			m_lstItems.Add(p);
		}
	}

	//平均列数
	int GetAvgColCount();
	
	void SetReadIgnoredChar(BOOL bUse, int ch){
		m_bUseReadIgnoredChar = bUse;
		m_nIgnoredChar = ch;
	}

	void SetSplitChar(int ch){
		m_chSplit = ch;
	}
	
	void CopyFrom(const CMultiListFile& a);
	void CopyRowsFrom(const CMultiListFile& a, int nStartRow, int nRow);
	
protected:
	CStringArray *FindRow(const char* instr, int inidx, BOOL bNoCase);
	
	
protected:
	CArray<CStringArray*,CStringArray*> m_lstItems;
	
	BOOL m_bUseReadIgnoredChar;
	int  m_nIgnoredChar;
	int  m_chSplit;
};

#endif // !defined(AFX_LISTFILE_H__C61782FA_9629_4696_9A6B_15F579CFE23E__INCLUDED_)
