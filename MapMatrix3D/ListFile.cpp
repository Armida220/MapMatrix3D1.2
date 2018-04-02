// ListFile.cpp: implementation of the CListFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "ListFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CListFile::CListFile()
{
	m_fp = NULL;
}

CListFile::~CListFile()
{
	Close();
}


BOOL CListFile::Open(const char *name)
{
	if( m_fp )
	{
		fclose( m_fp );
		m_lstOne.RemoveAll();
		m_lstTwo.RemoveAll();
	}

	m_fp = fopen(name,"r");
	if( !m_fp )return FALSE;

	char line[1024]={0},str1[1024]={0},str2[1024]={0};
	while(!feof(m_fp))
	{
		memset(line,0,sizeof(line)-1);
		fgets(line,sizeof(line)-1,m_fp);
		if( sscanf(line,"%s %s",str1,str2)==2 )
		{
			m_lstOne.Add(CString(str1));
			m_lstTwo.Add(CString(str2));
		}
	}
	
	fclose(m_fp);
	m_fp = NULL;
	
	return TRUE;
}


void CListFile::Close()
{
	if( m_fp )
	{
		fclose( m_fp );
		m_lstOne.RemoveAll();
		m_lstTwo.RemoveAll();
	}
}


const char* CListFile::FindMatchItem(const char* item, BOOL bNoCase, BOOL bMatchOne )
{
	if( item==NULL || strlen(item)<=0 )return NULL;
	int num = m_lstOne.GetSize();

	CStringArray *m = &m_lstOne, *r = &m_lstTwo;
	if( !bMatchOne )
	{
		m = &m_lstTwo;
		r = &m_lstOne;
	}

	if( bNoCase )
	{
		for( int i=0; i<num; i++)
		{
			if( (*m)[i].CompareNoCase(item)==0 )
				return (LPCTSTR)(*r)[i];
		}

	}
	else
	{
		for( int i=0; i<num; i++)
		{
			if( (*m)[i].Compare(item)==0 )
				return (LPCTSTR)(*r)[i];
		}
	}
	return NULL;
}

CListFileEx::CListFileEx()
{
}
CListFileEx::~CListFileEx()
{
	Close();
}
BOOL CListFileEx::Open(const char *name)
{
	if( m_fp )
	{
		fclose( m_fp );
		m_lstOne.RemoveAll();
		m_lstTwo.RemoveAll();
		m_ListFile.RemoveAll();
	}
	m_fp = fopen(name,"r");
	if( !m_fp )return FALSE;
	char line[1024]={0},str1[1024]={0},str2[1024]={0};
	while(!feof(m_fp))
	{
		memset(line,0,sizeof(line)-1);
		fgets(line,sizeof(line)-1,m_fp);
		Split(line,_T(" "));	
	}
	fclose(m_fp);
	m_fp = NULL;
	return TRUE;
}
BOOL CListFileEx::Split(CString source, LPCTSTR divKey)
{
	if (strlen(divKey)<=0)
	{
		return FALSE;
	}
	if (source.IsEmpty())
	{
		return FALSE;
	}
	int size = strlen(divKey);
//判断结尾是不是分隔符 不是加上一个
	if (strcmp((LPCTSTR)source.Right(size),divKey)!=0)
	{
		source +=divKey;
	}
	int pos = 0;
	int nCount = 0;
	int pre_pos = 0;
	LISTFILEEX *pListTile = new LISTFILEEX ;//临时结构
	pListTile->m_bHasLayer  =FALSE;
	pListTile->m_nSize =0;
	pListTile->m_lstOne.RemoveAll();
	pListTile->m_lstTwo.RemoveAll();
	while( -1 != pos ){
		pre_pos = pos;
		pos = source.Find(divKey,pos);
		if (-1!= pos)
		{
			if (0 == nCount)
			{
				pListTile->m_strGroup = source.Mid(pre_pos,(pos-pre_pos));	
			}
			else if (1 == nCount)
			{
				CString strcolor  = source.Mid(pre_pos,(pos-pre_pos));
				strcolor.Delete(0,2);
				pListTile->m_strColour = strcolor;	
			}
			else
			{
				pListTile->m_bHasLayer = TRUE;
				if (1 == nCount%2)//??êy???a??é?
				{
					CString strcolor  = source.Mid(pre_pos,(pos-pre_pos));
					strcolor.Delete(0,2);
					pListTile->m_lstOne.Add(strcolor);
					pListTile->m_nSize=pListTile->m_lstOne.GetSize();
				}
				else//?ùêy???a2???
				{
					CString strcode  = source.Mid(pre_pos,(pos-pre_pos));
					pListTile->m_lstTwo.Add(strcode);			
				}
			}
			pos= pos+1;
			nCount++;
		}		
	}	
	if (pListTile)
	{
		m_ListFile.Add(pListTile);
	}
	return TRUE;
}
void CListFileEx::Close()
{
	if( m_fp )
	{
		fclose( m_fp );
	}
	m_lstOne.RemoveAll();
	m_lstTwo.RemoveAll();
	for( int i=0; i<m_ListFile.GetSize(); i++)
	{
		delete m_ListFile.GetAt(i);
	}
	m_ListFile.RemoveAll();
}
int CListFileEx::GetLayerSizeByIndex(int nindex)
{
	int nsize = 0;
	if (nindex<m_ListFile.GetSize())
	{		
		nsize = m_ListFile.GetAt(nindex)->m_lstOne.GetSize();
		return nsize;
	}
	return -1;
}
CMultiListFile::CMultiListFile()
{
	m_bUseReadIgnoredChar = FALSE;
	m_nIgnoredChar = 0;
	m_chSplit = ' ';
}


CMultiListFile::~CMultiListFile()
{
	Clear();
}

void CMultiListFile::Clear()
{
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		delete m_lstItems.GetAt(i);
	}
	m_lstItems.RemoveAll();
}

static BOOL IsSplitChar(int ch, int chSplit)
{
	if( chSplit==' ' )
	{
		if( ch==' ' || ch=='\n' || ch=='\r' || ch=='\t' )
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if( ch==chSplit || ch=='\n' || ch=='\r' )
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}

int ReadStringFromString(char *line, int *pos, int chSplit, char *ret)
{
	if( *pos<0 )
		return 1;
	
	if( line[0]==0 )
	{
		*pos = -1;
		return 1;
	}
	else if( *pos>=strlen(line) )
	{
		*pos = -1;
		return 1;
	}
	else
	{
		char *p = line + *pos;
		while( *p!=0 && IsSplitChar(*p,chSplit) )
		{
			p++;
		}
		int readchar = 0;
		while( *p!=0 && !IsSplitChar(*p,chSplit) )
		{
			*ret = *p;
			ret++; p++;
			readchar++;
		}
		*ret = 0;
		*pos = p - line;
		if( readchar==0 )
		{
			*pos = -1;
			return 1;
		}

		return 0;
	}
}

int ReadStringFromString(char *line, int *pos, char *ret)
{
	return ReadStringFromString(line,pos,' ',ret);
}


BOOL CMultiListFile::Read(FILE *fp)
{
	char line[1024]={0},str1[1024]={0};

	while(!feof(fp))
	{
		memset(line,0,sizeof(line)-1);
		fgets(line,sizeof(line)-1,fp);

		if( m_bUseReadIgnoredChar && line[0]==m_nIgnoredChar )
			continue;

		int pos = 0;
		CStringArray *p = new CStringArray;
		if( p!=NULL )
		{
			while( ReadStringFromString(line,&pos,m_chSplit,str1)==0 )
			{
				p->Add(str1);
			}
			if( p->GetSize()>0 )
			{
				m_lstItems.Add(p);
			}
			else
				delete p;
		}
	}

	return TRUE;
}


BOOL CMultiListFile::Write(FILE *fp)
{
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		CStringArray *p = m_lstItems.GetAt(i);
		for( int j=0; j<p->GetSize(); j++)
		{
			fprintf(fp,"%s ",(LPCTSTR)p->GetAt(j));
		}
		fprintf(fp,"\n");
	}
	return TRUE;
}

BOOL CMultiListFile::Open(const char *name)
{	
	Clear();
	
	FILE *fp = fopen(name,"rt");
	if( !fp )return FALSE;

	Read(fp);

	fclose(fp);
	fp = NULL;
	
	return TRUE;
}


BOOL CMultiListFile::Save(const char *name)
{
	FILE *fp = fopen(name,"wt");
	if( !fp )return FALSE;
	
	Write(fp);
	
	fclose(fp);
	fp = NULL;
	
	return TRUE;
}

CStringArray* CMultiListFile::FindRowItem(const char* instr, int inidx, BOOL bNoCase)
{
	return FindRow(instr, inidx, bNoCase);
}

CStringArray *CMultiListFile::FindRow(const char* instr, int inidx, BOOL bNoCase)
{
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		CStringArray *p = m_lstItems.GetAt(i);
		if( p->GetSize()<=inidx )continue;

		if( bNoCase )
		{
			if( p->GetAt(inidx).CompareNoCase(instr)==0 )
			{
				return p;
			}
		}
		else
		{
			if( p->GetAt(inidx).Compare(instr)==0 )
			{
				return p;
			}
		}			
	}
	return NULL;
}


int CMultiListFile::FindIntItem(const char* instr, int inidx, int outidx, int value, BOOL bNoCase)
{
	CStringArray *p = FindRow(instr,inidx,bNoCase);
	if( p==NULL )return value;
	if( p->GetSize()<=outidx )return value;
	return atol(p->GetAt(outidx));
}


double CMultiListFile::FindFloatItem(const char* instr, int inidx, int outidx, double value, BOOL bNoCase)
{
	CStringArray *p = FindRow(instr,inidx,bNoCase);
	if( p==NULL )return value;
	if( p->GetSize()<=outidx )return value;
	return atof(p->GetAt(outidx));
}


CString CMultiListFile::FindTextItem(const char* instr, int inidx, int outidx, const char* value, BOOL bNoCase)
{
	CStringArray *p = FindRow(instr,inidx,bNoCase);
	if( p==NULL )return value;
	if( p->GetSize()<=outidx )return value;
	return (p->GetAt(outidx));
}


CString CMultiListFile::FindMatchItem(const char* item, int srcIdx, int destIdx, BOOL bNoCase )
{
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		CStringArray *p = m_lstItems.GetAt(i);
		if( p->GetSize()<=srcIdx || p->GetSize()<=destIdx )continue;
		
		if( bNoCase )
		{
			if( p->GetAt(srcIdx).CompareNoCase(item)==0 )
			{
				return p->GetAt(destIdx);
			}
		}
		else
		{
			if( p->GetAt(srcIdx).Compare(item)==0 )
			{
				return p->GetAt(destIdx);
			}
		}			
	}
	return CString();
}

CString CMultiListFile::FindMatchItem2(const char* item1, int srcIdx1, const char* item2, int srcIdx2, int destIdx, BOOL bNoCase )
{
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		CStringArray *p = m_lstItems.GetAt(i);
		if( p->GetSize()<=srcIdx1 || p->GetSize()<=srcIdx2 || p->GetSize()<=destIdx )continue;
		
		if( bNoCase )
		{
			if( (item1==NULL || p->GetAt(srcIdx1).CompareNoCase(item1)==0) && 
				(item2==NULL || p->GetAt(srcIdx2).CompareNoCase(item2)==0) )
			{
				return p->GetAt(destIdx);
			}
		}
		else
		{
			if( (item1==NULL || p->GetAt(srcIdx1).Compare(item1)==0) && 
				(item2==NULL || p->GetAt(srcIdx2).Compare(item2)==0) )
			{
				return p->GetAt(destIdx);
			}
		}			
	}
	return CString();
}


CStringArray * CMultiListFile::FindMatchRow(const char* item1, int srcIdx1, const char* item2, int srcIdx2, BOOL bNoCase )
{
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		CStringArray *p = m_lstItems.GetAt(i);
		if( p->GetSize()<=srcIdx1 || p->GetSize()<=srcIdx2 )continue;
		
		if( bNoCase )
		{
			if( (item1==NULL || p->GetAt(srcIdx1).CompareNoCase(item1)==0) && 
				(item2==NULL || p->GetAt(srcIdx2).CompareNoCase(item2)==0) )
			{
				return p;
			}
		}
		else
		{
			if( (item1==NULL || p->GetAt(srcIdx1).Compare(item1)==0) && 
				(item2==NULL || p->GetAt(srcIdx2).Compare(item2)==0) )
			{
				return p;
			}
		}			
	}
	return NULL;
}


CStringArray *CMultiListFile::GetRow(int nRow)
{
	if( nRow<0 || nRow>=m_lstItems.GetSize() )
		return NULL;
	return m_lstItems.GetAt(nRow);
}


int CMultiListFile::GetRowCount()
{
	return m_lstItems.GetSize();
}

int CMultiListFile::GetIntItem(int nRow, int nCol, int value)
{
	CStringArray *p = GetRow(nRow);
	if( p==NULL )return value;
	if( p->GetSize()<=nCol )return value;
	return atol(p->GetAt(nCol));
}

double CMultiListFile::GetFloatItem(int nRow, int nCol, double value)
{
	CStringArray *p = GetRow(nRow);
	if( p==NULL )return value;
	if( p->GetSize()<=nCol )return value;
	return atof(p->GetAt(nCol));
}

CString CMultiListFile::GetTextItem(int nRow, int nCol, const char* value)
{
	CStringArray *p = GetRow(nRow);
	if( p==NULL )return value;
	if( p->GetSize()<=nCol )return value;
	return (p->GetAt(nCol));
}



void CMultiListFile::CopyFrom(const CMultiListFile& a)
{
	Clear();
	
	m_bUseReadIgnoredChar = a.m_bUseReadIgnoredChar;
	m_nIgnoredChar = a.m_nIgnoredChar;

	CopyRowsFrom(a,0,-1);
}


void CMultiListFile::CopyRowsFrom(const CMultiListFile& a, int nStartRow, int nRow)
{
	if( nRow==-1 )nRow = a.m_lstItems.GetSize();
	for( int i=nStartRow; i<a.m_lstItems.GetSize() && i<(nStartRow+nRow); i++)
	{
		CStringArray *p = new CStringArray;
		p->Copy(*(a.m_lstItems.GetAt(i)));
		
		m_lstItems.Add(p);
	}
}

int CMultiListFile::GetAvgColCount()
{
	if( m_lstItems.GetSize()<=0 )
		return 0;

	int ncol = 0;
	for( int i=0; i<m_lstItems.GetSize(); i++)
	{
		CStringArray *p = m_lstItems.GetAt(i);
		ncol += p->GetSize();
	}

	ncol = floor(ncol*0.1/m_lstItems.GetSize() + 0.5);
	return ncol;
}