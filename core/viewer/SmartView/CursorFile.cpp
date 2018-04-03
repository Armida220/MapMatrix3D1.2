// CursorFile.cpp: implementation of the CCursorFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "CursorFile.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


MyNameSpaceBegin

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCursorFile::CCursorFile()
{
	m_nSum = 0;
	m_nCol = 0;
	m_nRow = 0;
	m_nCurType = 0;
	
	m_pCursorData = NULL;
	m_bValid = false;
	m_nBitWid = 0;
}

CCursorFile::~CCursorFile()
{
	if( m_pCursorData )delete[] m_pCursorData;
}

bool CCursorFile::LoadCursor(const char* name)
{
	m_bValid = false;

	char lines[1024]={0}, *curbit;
	int  bitcount=0, bytesum, currow=0, curcursor=0 ;

	FILE *fp = fopen(name,"r");
	if( !fp )return false;

	if( fscanf(fp,"%d %d %d",&m_nSum, &m_nCol, &m_nRow)!=3 )goto EXIT;
	if( m_nSum<=0 || m_nCol<=0 || m_nRow<=0 )goto EXIT;
	if( m_nCol>100 || m_nRow>100 )goto EXIT;
	
	m_nBitWid = ceil(m_nCol/8.0);
	bytesum = m_nBitWid * m_nRow;

	if( m_pCursorData )delete[] m_pCursorData;
	m_pCursorData = NULL;
	
	m_pCursorData = new char[bytesum*m_nSum];
	if( !m_pCursorData )goto EXIT;
	curbit = m_pCursorData+curcursor*bytesum;

	while( fgets(lines,sizeof(lines)-1,fp) )
	{
		if( strlen(lines)>=2 && lines[0]=='/'&&lines[1]=='/' )continue;
		char b = 0;
		char *c = lines;
		bitcount = 0;
		while( *c )
		{
			if( c-lines>=m_nCol )break;

			if( bitcount>7 )
			{
				*curbit = b;
				curbit++;
				bitcount = 0;
				b = 0;
			}
			
			if( *c=='1' ) b|=(1<<bitcount);
			else if( *c==' ' );
			else break;
			
			c++;
			bitcount++;
		}

		if( bitcount>0 )
		{
			*curbit = b;
			curbit++;
		}
		
		if( c-lines>=m_nCol )currow++;
		if( currow>=m_nRow )
		{
			currow = 0;
			curcursor++;
			curbit = m_pCursorData+curcursor*bytesum;
		}

		if( curcursor>=m_nSum )break;
	}

	m_bValid = true;

EXIT:
	fclose(fp);
	SetType(0);
	return m_bValid;
}

bool CCursorFile::IsValid()
{
	return m_bValid;
}

int CCursorFile::GetTypeSum()
{
	if( !m_bValid )return 0;
	return m_nSum;
}

CSize CCursorFile::GetSize()
{
	if( !m_bValid )return CSize(0,0);
	return CSize(m_nCol,m_nRow);
}

int CCursorFile::GetCursorBitWid()
{
	if( !m_bValid )return 0;
	return m_nBitWid;
}

int CCursorFile::GetType()
{
	if( !m_bValid )return -1;
	return m_nCurType;
}

void CCursorFile::SetType(int type)
{
	if( !m_bValid )return;
	if( type>=GetTypeSum() )return;

	m_nCurType = type;

	int bytesum  = m_nBitWid * m_nRow;
	m_pCurData = m_pCursorData + bytesum*type;
}

char *CCursorFile::GetTypeData()
{
	if( !m_bValid )return NULL;
	return m_pCurData;
}


MyNameSpaceEnd