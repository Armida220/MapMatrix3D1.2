// Markup.cpp: implementation of the CMarkup class.
//
// Markup Release 7.3
// Copyright (C) 1999-2004 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#include "stdafx.h"
#include "Markup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _MBCS
#pragma message( "Note: MBCS build (not UTF-8)" )
// For UTF-8, remove _MBCS from project settings C/C++ preprocessor definitions
#endif

// Defines for Windows CE
#ifdef _WIN32_WCE
#define _tclen(p) 1
#define _tccpy(p1,p2) *(p1)=*(p2)
#endif

// Customization
#define x_EOL _T("\r\n") // can be \r\n or \n or empty
#define x_EOLLEN (sizeof(x_EOL)/sizeof(_TCHAR)-1) // string length of x_EOL
#define x_ATTRIBQUOTE _T("\"") // can be double or single quote


MyNameSpaceBegin


void CMarkup::operator=( const CMarkup& markup )
{
	m_iPosParent = markup.m_iPosParent;
	m_iPos = markup.m_iPos;
	m_iPosChild = markup.m_iPosChild;
	m_iPosFree = markup.m_iPosFree;
	m_iPosDeleted = markup.m_iPosDeleted;
	m_nNodeType = markup.m_nNodeType;
	m_nNodeOffset = markup.m_nNodeOffset;
	m_nNodeLength = markup.m_nNodeLength;
	m_strDoc = markup.m_strDoc;
	m_strError = markup.m_strError;
	m_nFlags = markup.m_nFlags;

	// Copy used part of the index array
	m_aPos.RemoveAll();
	m_aPos.nSize = m_iPosFree;
	if ( m_aPos.nSize < 8 )
		m_aPos.nSize = 8;
	m_aPos.nSegs = m_aPos.SegsUsed();
	if ( m_aPos.nSegs )
	{
		m_aPos.pSegs = (ElemPos**)(new char[m_aPos.nSegs*sizeof(char*)]);
		int nSegSize = 1 << m_aPos.PA_SEGBITS;
		for ( int nSeg=0; nSeg < m_aPos.nSegs; ++nSeg )
		{
			if ( nSeg + 1 == m_aPos.nSegs )
				nSegSize = m_aPos.GetSize() - (nSeg << m_aPos.PA_SEGBITS);
			m_aPos.pSegs[nSeg] = (ElemPos*)(new char[nSegSize*sizeof(ElemPos)]);
			memcpy( m_aPos.pSegs[nSeg], markup.m_aPos.pSegs[nSeg], nSegSize*sizeof(ElemPos) );
		}
	}

	// Copy SavedPos map
	m_mapSavedPos.RemoveAll();
	if ( markup.m_mapSavedPos.pTable )
	{
		m_mapSavedPos.AllocMapTable();
		for ( int nSlot=0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			SavedPos* pCopySavedPos = markup.m_mapSavedPos.pTable[nSlot];
			if ( pCopySavedPos )
			{
				int nCount = 0;
				while ( pCopySavedPos[nCount].nSavedPosFlags & SavedPosMap::SPM_USED )
				{
					++nCount;
					if ( pCopySavedPos[nCount-1].nSavedPosFlags & SavedPosMap::SPM_LAST )
						break;
				}
				SavedPos* pNewSavedPos = new SavedPos[nCount];
				for ( int nCopy=0; nCopy<nCount; ++nCopy )
					pNewSavedPos[nCopy] = pCopySavedPos[nCopy];
				pNewSavedPos[nCount-1].nSavedPosFlags |= SavedPosMap::SPM_LAST;
				m_mapSavedPos.pTable[nSlot] = pNewSavedPos;
			}
		}
	}

	MARKUP_SETDEBUGSTATE;
}

bool CMarkup::SetDoc( LPCTSTR szDoc )
{
	// Set document text
	if ( szDoc )
		m_strDoc = szDoc;
	else
		m_strDoc.Empty();

	m_strError.Empty();
	return x_ParseDoc();
};

bool CMarkup::IsWellFormed()
{
	if ( m_aPos.GetSize() && m_aPos[0].iElemChild )
		return true;
	return false;
}

bool CMarkup::Load( LPCTSTR szFileName )
{
	if ( ! ReadTextFile(szFileName, m_strDoc, &m_strError, &m_nFlags) )
		return false;
	return x_ParseDoc();
}

bool CMarkup::ReadTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError, int* pnFlags )
{
	// Static utility method to load text file into strDoc
	//
	// Open file to read binary
	FILE* fp = _tfopen( szFileName, _T("rb") );
	if ( ! fp )
	{
		if ( pstrError )
			*pstrError = strerror(errno);
		return false;
	}

	// Set flags to 0 unless flags argument provided
	int nFlags = pnFlags?*pnFlags:0;
	_TCHAR szDescBOM[20] = {0};
	strDoc.Empty();

	// Get file length
	fseek( fp, 0, SEEK_END );
	int nFileByteLen = ftell( fp );
	fseek( fp, 0, SEEK_SET );


#if defined(_UNICODE) // convert file to wide char
	int nWideLen = 0;
	if ( nFileByteLen )
	{
		char* pBuffer = new char[nFileByteLen];
		fread( pBuffer, nFileByteLen, 1, fp );
		// For ANSI files, replace CP_UTF8 with CP_ACP in both places
		int nWideLen = MultiByteToWideChar(CP_ACP, 0, pBuffer, nFileByteLen, NULL, 0);
		MultiByteToWideChar(CP_ACP, 0, pBuffer, nFileByteLen, strDoc.GetBuffer(nWideLen), nWideLen);
		strDoc.ReleaseBuffer( nWideLen );
		delete [] pBuffer;
	}
	if ( pstrError )
		(*pstrError).Format(_T("%s%d bytes to %d wide chars"),szDescBOM,nFileByteLen,nWideLen);
#else // read file directly
	if ( nFileByteLen )
	{
		fread( strDoc.GetBuffer(nFileByteLen), nFileByteLen, 1, fp );
		strDoc.ReleaseBuffer( nFileByteLen );
	}
	if ( pstrError )
		(*pstrError).Format( _T("%s%d bytes"), szDescBOM, nFileByteLen );
#endif
	fclose( fp );
	if ( pnFlags )
		*pnFlags = nFlags;
	return true;
}

bool CMarkup::Save( LPCTSTR szFileName )
{
	return WriteTextFile( szFileName, m_strDoc, &m_strError, &m_nFlags );
}

bool CMarkup::WriteTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError, int* pnFlags )
{
	// Static utility method to save strDoc to text file
	//
	// Open file to write binary
	bool bSuccess = true;
	FILE* fp = _tfopen( szFileName, _T("wb") );
	if ( ! fp )
	{
		if ( pstrError )
			*pstrError = strerror(errno);
		return false;
	}

	// Set flags to 0 unless flags argument provided
	int nFlags = pnFlags?*pnFlags:0;
	_TCHAR szDescBOM[20] = {0};

	// Get document length
	int nDocLength = strDoc.GetLength();


#if defined( _UNICODE )
	int nMBLen = 0;
	if ( nDocLength )
	{
		// For ANSI files, replace CP_UTF8 with CP_ACP in both places
		nMBLen = WideCharToMultiByte(CP_ACP, 0, strDoc, nDocLength, NULL, 0, NULL, NULL);
		char* pBuffer = new char[nMBLen+1];
		WideCharToMultiByte(CP_ACP, 0, strDoc, nDocLength, pBuffer, nMBLen + 1, NULL, NULL);
		bSuccess = ( fwrite( pBuffer, nMBLen, 1, fp ) == 1 );
		delete [] pBuffer;
	}
	if ( pstrError )
		(*pstrError).Format( _T("%d wide chars to %s%d bytes"), nDocLength, szDescBOM, nMBLen );
#else // MBCS or UTF-8
	if ( nDocLength )
	{
		CString strDocWrite = strDoc; // reference unless converted
		nDocLength = strDocWrite.GetLength();
		bSuccess = ( fwrite( (LPCTSTR)strDocWrite, nDocLength, 1, fp ) == 1 );
	}
	if ( pstrError )
		(*pstrError).Format( _T("%s%d bytes"), szDescBOM, nDocLength );
#endif
	
	if ( ! bSuccess && pstrError )
		*pstrError = strerror(errno);
	fclose(fp);
	if ( pnFlags )
		*pnFlags = nFlags;
	return bSuccess;
}

bool CMarkup::FindElem( LPCTSTR szName, BOOL bFromTop )
{
	// Change current position only if found
	//
	if ( m_aPos.GetSize() )
	{
		int iPos = x_FindElem( m_iPosParent, bFromTop?0:m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkup::FindChildElem( LPCTSTR szName )
{
	// Change current child position only if found
	//
	// Shorthand: call this with no current main position
	// means find child under root element
	if ( ! m_iPos )
		FindElem();

	int iPosChild = x_FindElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}


CString CMarkup::GetTagName() const
{
	// Return the tag name at the current main position
	CString strTagName;


	if ( m_iPos )
		strTagName = x_GetTagName( m_iPos );
	return strTagName;
}

bool CMarkup::IntoElem()
{
	// If there is no child position and IntoElem is called it will succeed in release 6.3
	// (A subsequent call to FindElem will find the first element)
	// The following short-hand behavior was never part of EDOM and was misleading
	// It would find a child element if there was no current child element position and go into it
	// It is removed in release 6.3, this change is NOT backwards compatible!
	// if ( ! m_iPosChild )
	//	FindChildElem();

	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		x_SetPos( m_iPos, m_iPosChild, 0 );
		return true;
	}
	return false;
}

bool CMarkup::OutOfElem()
{
	// Go to parent element
	if ( m_iPosParent )
	{
		x_SetPos( m_aPos[m_iPosParent].iElemParent, m_iPosParent, m_iPos );
		return true;
	}
	return false;
}

CString CMarkup::GetAttribName( int n ) const
{
	// Return nth attribute name of main position
	TokenPos token( m_strDoc );
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[m_iPos].nStart + 1;
	else if ( m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return _T("");

	for ( int nAttrib=0; nAttrib<=n; ++nAttrib )
		if ( ! x_FindAttrib(token) )
			return _T("");

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkup::SavePos( LPCTSTR szPosName )
{
	// Save current element position in saved position map
	if ( szPosName )
	{
		SavedPos savedpos;
		if ( szPosName )
			savedpos.strName = szPosName;
		if ( m_iPosChild )
		{
			savedpos.iPos = m_iPosChild;
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_CHILD;
		}
		else if ( m_iPos )
		{
			savedpos.iPos = m_iPos;
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_MAIN;
		}
		else
		{
			savedpos.iPos = m_iPosParent;
		}
		savedpos.nSavedPosFlags |= SavedPosMap::SPM_USED;

		if ( ! m_mapSavedPos.pTable )
			m_mapSavedPos.AllocMapTable();
		int nSlot = m_mapSavedPos.Hash( szPosName );
		SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
		int nOffset = 0;
		if ( ! pSavedPos )
		{
			pSavedPos = new SavedPos[2];
			pSavedPos[1].nSavedPosFlags = SavedPosMap::SPM_LAST;
			m_mapSavedPos.pTable[nSlot] = pSavedPos;
		}
		else
		{
			while ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				if ( pSavedPos[nOffset].strName == szPosName )
					break;
				if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
				{
					int nNewSize = (nOffset + 6) * 2;
					SavedPos* pNewSavedPos = new SavedPos[nNewSize];
					for ( int nCopy=0; nCopy<=nOffset; ++nCopy )
						pNewSavedPos[nCopy] = pSavedPos[nCopy];
					pNewSavedPos[nOffset].nSavedPosFlags ^= SavedPosMap::SPM_LAST;
					pNewSavedPos[nNewSize-1].nSavedPosFlags = SavedPosMap::SPM_LAST;
					delete [] pSavedPos;
					pSavedPos = pNewSavedPos;
					m_mapSavedPos.pTable[nSlot] = pSavedPos;
					++nOffset;
					break;
				}
				++nOffset;
			}
		}
		if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
			savedpos.nSavedPosFlags |= SavedPosMap::SPM_LAST;
		pSavedPos[nOffset] = savedpos;

		/*
		// To review hash table balance, uncomment and watch strBalance
		CString strBalance, strSlot;
		for ( nSlot=0; nSlot < SavedPosMap::SPM_SIZE; ++nSlot )
		{
			pSavedPos = m_mapSavedPos.pTable[nSlot];
			int nCount = 0;
			while ( pSavedPos && pSavedPos->nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				++nCount;
				if ( pSavedPos->nSavedPosFlags & SavedPosMap::SPM_LAST )
					break;
				++pSavedPos;
			}
			strSlot.Format( _T("%d "), nCount );
			strBalance += strSlot;
		}
		*/

		return true;
	}
	return false;
}

bool CMarkup::RestorePos( LPCTSTR szPosName )
{
	// Restore element position if found in saved position map
	if ( szPosName )
	{
		int nSlot = m_mapSavedPos.Hash( szPosName );
		SavedPos* pSavedPos = m_mapSavedPos.pTable[nSlot];
		if ( pSavedPos )
		{
			int nOffset = 0;
			while ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_USED )
			{
				if ( pSavedPos[nOffset].strName == szPosName )
				{
					int i = pSavedPos[nOffset].iPos;
					if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_CHILD )
						x_SetPos( m_aPos[m_aPos[i].iElemParent].iElemParent, m_aPos[i].iElemParent, i );
					else if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_MAIN )
						x_SetPos( m_aPos[i].iElemParent, i, 0 );
					else
						x_SetPos( i, 0, 0 );
					return true;
				}
				if ( pSavedPos[nOffset].nSavedPosFlags & SavedPosMap::SPM_LAST )
					break;
				++nOffset;
			}
		}
	}
	return false;
}

bool CMarkup::RemoveElem()
{
	// Remove current main position element
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		int iPos = x_RemoveElem( m_iPos );
		x_SetPos( m_iPosParent, iPos, 0 );
		return true;
	}
	return false;
}

bool CMarkup::RemoveChildElem()
{
	// Remove current child position element
	if ( m_iPosChild )
	{
		int iPosChild = x_RemoveElem( m_iPosChild );
		x_SetPos( m_iPosParent, m_iPos, iPosChild );
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

bool CMarkup::x_AllocPosArray( int nNewSize /*=0*/ )
{
	// Resize m_aPos when the document is created or the array is filled
	// The PosArray class is implemented using segments to reduce contiguous memory requirements
	// It reduces reallocations (copying of memory) since this only occurs within one segment
	// The "Grow By" algorithm ensures there are no reallocations after 2 segments
	//
	if ( ! nNewSize )
		nNewSize = m_iPosFree + (m_iPosFree>>1); // Grow By: multiply size by 1.5
	if ( m_aPos.GetSize() < nNewSize )
	{
		// Grow By: new size can be at most one more complete segment
		int nSeg = (m_aPos.GetSize()?m_aPos.GetSize()-1:0) >> m_aPos.PA_SEGBITS;
		int nNewSeg = (nNewSize-1) >> m_aPos.PA_SEGBITS;
		if ( nNewSeg > nSeg + 1 )
		{
			nNewSeg = nSeg + 1;
			nNewSize = (nNewSeg+1) << m_aPos.PA_SEGBITS;
		}

		// Allocate array of segments
		if ( m_aPos.nSegs <= nNewSeg )
		{
			int nNewSegments = 4 + nNewSeg * 2;
			char* pNewSegments = new char[nNewSegments*sizeof(char*)];
			if ( m_aPos.SegsUsed() )
				memcpy( pNewSegments, m_aPos.pSegs, m_aPos.SegsUsed()*sizeof(char*) );
			if ( m_aPos.pSegs )
				delete[] (char*)m_aPos.pSegs;
			m_aPos.pSegs = (ElemPos**)pNewSegments;
			m_aPos.nSegs = nNewSegments;
		}

		// Calculate segment sizes
		int nSegSize = m_aPos.GetSize() - (nSeg << m_aPos.PA_SEGBITS);
		int nNewSegSize = nNewSize - (nNewSeg << m_aPos.PA_SEGBITS);

		// Complete first segment
		int nFullSegSize = 1 << m_aPos.PA_SEGBITS;
		if ( nSeg < nNewSeg && nSegSize < nFullSegSize )
		{
			char* pNewFirstSeg = new char[ nFullSegSize * sizeof(ElemPos) ];
			if ( nSegSize )
			{
				// Reallocate
				memcpy( pNewFirstSeg, m_aPos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
				delete[] (char*)m_aPos.pSegs[nSeg];
			}
			m_aPos.pSegs[nSeg] = (ElemPos*)pNewFirstSeg;
		}

		// New segment
		char* pNewSeg = new char[ nNewSegSize * sizeof(ElemPos) ];
		if ( nNewSeg == nSeg && nSegSize )
		{
			// Reallocate
			memcpy( pNewSeg, m_aPos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
			delete[] (char*)m_aPos.pSegs[nSeg];
		}
		m_aPos.pSegs[nNewSeg] = (ElemPos*)pNewSeg;
		m_aPos.nSize = nNewSize;
	}
	return true;
}

bool CMarkup::x_ParseDoc()
{
	// Preserve pre-parse result
	CString strResult = m_strError;

	// Reset indexes
	ResetPos();
	m_mapSavedPos.RemoveAll();

	// Starting size of position array: 1 element per 64 bytes of document
	// Tight fit when parsing small doc, only 0 to 2 reallocs when parsing large doc
	// Start at 8 when creating new document
	m_iPosFree = 1;
	x_AllocPosArray( m_strDoc.GetLength() / 64 + 8 );
	m_iPosDeleted = 0;

	// Parse document
	bool bWellFormed = false;
	m_strError.Empty();
	if ( m_strDoc.GetLength() )
	{
		TokenPos token( m_strDoc );
		m_aPos[0].ClearVirtualParent();
		int iPos = x_ParseElem( 0, token );
		if ( iPos > 0 )
		{
			m_aPos[0].iElemChild = iPos;
			m_aPos[0].nLength = m_strDoc.GetLength();
			bWellFormed = true;
		}
	}
	else
		m_strError = _T("Empty document");

	// Clear indexes if parse failed or empty document
	if ( ! bWellFormed )
	{
		m_aPos[0].ClearVirtualParent();
		m_iPosFree = 1;
	}

	ResetPos();

	// Combine preserved result with parse error
	if ( ! strResult.IsEmpty() )
	{
		if ( m_strError.IsEmpty() )
			m_strError = strResult;
		else
			m_strError = strResult + _T(", ") + m_strError;
	}

	return bWellFormed;
};

int CMarkup::x_ParseElem( int iPosParent, TokenPos& token )
{
	// This is either called by x_ParseDoc or x_AddSubDoc
	// This returns the new position if a tag is found, otherwise zero
	// In all cases we need to get a new ElemPos, but release it if unused
	//
	int iElemRoot = 0;
	int iPos = iPosParent;
	int nRootDepth = m_aPos[iPos].Level();
	token.nNext = 0;

	// Loop through the nodes of the document
	NodeStack aNodes;
	aNodes.Add();
	int nDepth = 0;
	int nTypeFound = 0;
	ElemPos* pElem;
	int iElemFirst, iElemLast;
	while ( nTypeFound >= 0 )
	{
		nTypeFound = x_ParseNode( token, aNodes.Top() );
		if ( nTypeFound == MNT_ELEMENT ) // start tag
		{
			iPos = x_GetFreePos();
			if ( ! iElemRoot )
				iElemRoot = iPos;
			else if ( nDepth == 0 )
			{
				m_strError.Format( _T("Element '%s' at offset %d is sibling to root"),
					aNodes.Top().strName, aNodes.Top().nStart );
				return -1;
			}
			pElem = &m_aPos[iPos];
			pElem->iElemParent = iPosParent;
			pElem->iElemNext = 0;
			if ( m_aPos[iPosParent].iElemChild )
			{
				iElemFirst = m_aPos[iPosParent].iElemChild;
				iElemLast = m_aPos[iElemFirst].iElemPrev;
				m_aPos[iElemLast].iElemNext = iPos;
				pElem->iElemPrev = iElemLast;
				m_aPos[iElemFirst].iElemPrev = iPos;
				pElem->nFlags = 0;
			}
			else
			{
				m_aPos[iPosParent].iElemChild = iPos;
				pElem->iElemPrev = iPos;
				pElem->nFlags = MNF_FIRST;
			}
			pElem->SetLevel( nRootDepth + nDepth );
			pElem->iElemChild = 0;
			pElem->nStart = aNodes.Top().nStart;
			pElem->SetStartTagLen( aNodes.Top().nLength );
			if ( aNodes.Top().nFlags & MNF_EMPTY )
			{
				iPos = iPosParent;
				pElem->SetEndTagLen( 0 );
				pElem->nLength = aNodes.Top().nLength;
			}
			else
			{
				iPosParent = iPos;
				++nDepth;
				aNodes.Add();
			}
		}
		else if ( nTypeFound == 0 ) // end tag
		{
			if ( aNodes.TopIndex() == 0 )
			{
				m_strError.Format( _T("No start tag for end tag '%s' at offset %d"),
					x_GetToken(token), aNodes.Top().nStart );
				return -1;
			}
			pElem = &m_aPos[iPos];
			pElem->nLength = aNodes.Top().nStart - pElem->nStart + aNodes.Top().nLength;
			pElem->SetEndTagLen( aNodes.Top().nLength );
			aNodes.Remove();
			if ( ! token.Match(aNodes.Top().strName) )
			{
				m_strError.Format( _T("End tag '%s' at offset %d does not match start tag '%s' at offset %d"),
					x_GetToken(token), token.nL-1, aNodes.Top().strName, pElem->nStart );
				return -1;
			}
			--nDepth;
			iPosParent = pElem->iElemParent;
			iPos = iPosParent;
		}
	}

	if ( nTypeFound == -1 )
	{
		m_strError = aNodes.Top().strName;
		return -1;
	}
	else if ( nDepth > 0 )
	{
		aNodes.Remove();
		m_strError.Format( _T("Element '%s' at offset %d not ended"),
			aNodes.Top().strName, aNodes.Top().nStart );
		return -1;
	}
	else if ( ! iElemRoot )
	{
		m_strError = _T("Root element not found");
		return 0;
	}

	// Successfully parsed element (and contained elements)
	return iElemRoot;
}


bool CMarkup::x_FindChar( LPCTSTR szDoc, int& nChar, _TCHAR c )
{
	// static function
	LPCTSTR pChar = &szDoc[nChar];
	while ( *pChar && *pChar != c )
		pChar += _tclen( pChar );
	nChar = (int)(pChar - szDoc);
	if ( ! *pChar )
		return false;
	return true;
}

bool CMarkup::x_FindAny( LPCTSTR szDoc, int& nChar )
{
	// Starting at nChar, find a non-whitespace char
	// return false if no non-whitespace before end of document, nChar points to end
	// otherwise return true and nChar points to non-whitespace char
	while ( szDoc[nChar] && _tcschr(_T(" \t\n\r"),szDoc[nChar]) )
		++nChar;
	return szDoc[nChar] != _T('\0');
}

bool CMarkup::x_FindToken( CMarkup::TokenPos& token )
{
	// Starting at token.nNext, bypass whitespace and find the next token
	// returns true on success, members of token point to token
	// returns false on end of document, members point to end of document
	LPCTSTR szDoc = token.szDoc;
	int nChar = token.nNext;
	token.bIsString = false;

	// By-pass leading whitespace
	if ( ! x_FindAny(szDoc,nChar) )
	{
		// No token was found before end of document
		token.nL = nChar;
		token.nR = nChar - 1;
		token.nNext = nChar;
		return false;
	}

	// Is it an opening quote?
	_TCHAR cFirstChar = szDoc[nChar];
	if ( cFirstChar == _T('\"') || cFirstChar == _T('\'') )
	{
		token.bIsString = true;

		// Move past opening quote
		++nChar;
		token.nL = nChar;

		// Look for closing quote
		x_FindChar( token.szDoc, nChar, cFirstChar );

		// Set right to before closing quote
		token.nR = nChar - 1;

		// Set nChar past closing quote unless at end of document
		if ( szDoc[nChar] )
			++nChar;
	}
	else
	{
		// Go until special char or whitespace
		token.nL = nChar;
		while ( szDoc[nChar] && ! _tcschr(_T(" \t\n\r<>=\\/?!"),szDoc[nChar]) )
			nChar += (int)_tclen(&szDoc[nChar]);

		// Adjust end position if it is one special char
		if ( nChar == token.nL )
			++nChar; // it is a special char
		token.nR = nChar - 1;
	}

	// nNext points to one past last char of token
	token.nNext = nChar;
	return true;
}

CString CMarkup::x_GetToken( const CMarkup::TokenPos& token )
{
	// The token contains indexes into the document identifying a small substring
	// Build the substring from those indexes and return it
	if ( token.nL > token.nR )
		return _T("");
	CString strToken( &token.szDoc[token.nL], token.Length() );
	return strToken;
}

int CMarkup::x_FindElem( int iPosParent, int iPos, LPCTSTR szPath )
{
	// If szPath is NULL or empty, go to next sibling element
	// Otherwise go to next sibling element with matching path
	//
	if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[0] )
		return iPos;

	// Search
	TokenPos token( m_strDoc );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStart + 1;
		x_FindToken( token ); // Locate tag name
		if ( token.Match(szPath) )
			return iPos;
		iPos = m_aPos[iPos].iElemNext;
	}
	return 0;

}

int CMarkup::x_ParseNode( CMarkup::TokenPos& token, CMarkup::NodePos& node )
{
	// Call this with token.nNext set to the start of the node or tag
	// Upon return token.nNext points to the char after the node or tag
	// 
	// <!--...--> comment
	// <!DOCTYPE ...> dtd
	// <?target ...?> processing instruction
	// <![CDATA[...]]> cdata section
	// <NAME ...> element start tag
	// </NAME ...> element end tag
	//
	// returns the nodetype, or 0 for end tag, -1 for end of document, -2 for error
	//
	enum ParseBits
	{
		PD_OPENTAG = 1,
		PD_BANG = 2,
		PD_DASH = 4,
		PD_BRACKET = 8,
		PD_TEXTORWS = 16,
		PD_DOCTYPE = 32,
		PD_INQUOTE_S = 64,
		PD_INQUOTE_D = 128,
	};
	int nParseFlags = 0;

	LPCTSTR szFindEnd = NULL;
	int nNodeType = -1;
	int nEndLen = 0;
	int nName = 0;
	#define FINDNODETYPE(e,t,n) { szFindEnd=e; nEndLen=(sizeof(e)-1)/sizeof(_TCHAR); nNodeType=t; if(n) nName=(int)(pDoc-token.szDoc)+n-1; }
	#define ISNODEERROR(e) { nNodeType=-1; node.strName.Format(_T("Incorrect %s at offset %d"),e,nR); break; }

	node.nStart = token.nNext;
	node.nFlags = 0;

	int nR = token.nNext;
	LPCTSTR pDoc = &token.szDoc[nR];
	register unsigned int cD = (unsigned int)*pDoc;
	if ( ! cD )
	{
		node.nLength = 0;
		node.nNodeType = 0;
		return -2; // end of document
	}

	while ( 1 )
	{
		cD = (unsigned int)*pDoc;
		if ( ! cD )
		{
			nR = (int)(pDoc - token.szDoc) - 1;
			if ( nNodeType != MNT_WHITESPACE && nNodeType != MNT_TEXT )
			{
				LPCTSTR szType = _T("tag");
				if ( (nParseFlags & PD_DOCTYPE) || nNodeType == MNT_DOCUMENT_TYPE )
					szType = _T("Doctype");
				else if ( nNodeType == MNT_ELEMENT )
					szType = _T("Element tag");
				else if ( nNodeType == 0 )
					szType = _T("Element end tag");
				else if ( nNodeType == MNT_CDATA_SECTION )
					szType = _T("CDATA Section");
				else if ( nNodeType == MNT_PROCESSING_INSTRUCTION )
					szType = _T("Processing instruction");
				else if ( nNodeType == MNT_COMMENT )
					szType = _T("Comment");
				nNodeType = -1;
				node.strName.Format( _T("%s at offset %d unterminated"), szType, node.nStart );
			}
			break;
		}

		if ( nName )
		{
			if ( _tcschr(_T(" \t\n\r/>"),(_TCHAR)cD) )
			{
				int nNameLen = (int)(pDoc - token.szDoc) - nName;
				if ( nNodeType == 0 )
				{
					token.nL = nName;
					token.nR = nName + nNameLen - 1;
				}
				else
				{
					memcpy( node.strName.GetBuffer(nNameLen), &token.szDoc[nName], nNameLen*sizeof(_TCHAR) );
					node.strName.ReleaseBuffer( nNameLen );
				}
				nName = 0;
			}
			else
			{
				++pDoc;
				continue;
			}
		}

		if ( szFindEnd )
		{
			if ( cD == _T('>') && ! (nParseFlags & (PD_INQUOTE_S|PD_INQUOTE_D)) )
			{
				nR = (int)(pDoc - token.szDoc);
				if ( nEndLen == 1 )
				{
					szFindEnd = NULL;
					if ( nNodeType == MNT_ELEMENT && *(pDoc-1) == _T('/') )
						node.nFlags |= MNF_EMPTY;
				}
				else if ( nR > nEndLen )
				{
					// Test for end of PI or comment
					LPCTSTR pEnd = pDoc - nEndLen + 1;
					LPCTSTR pFindEnd = szFindEnd;
					int nLen = nEndLen;
					while ( --nLen && *pEnd++ == *pFindEnd++ );
					if ( nLen == 0 )
						szFindEnd = NULL;
				}
				if ( ! szFindEnd && ! (nParseFlags & PD_DOCTYPE) )
					break;
			}
			else if ( cD == _T('<') && nNodeType == MNT_TEXT )
			{
				nR = (int)(pDoc - token.szDoc) - 1;
				break;
			}
			else if ( nNodeType == MNT_ELEMENT )
			{
				if ( cD == _T('\"') && ! (nParseFlags&PD_INQUOTE_S) )
					nParseFlags ^= PD_INQUOTE_D;
				else if ( cD == _T('\'') && ! (nParseFlags&PD_INQUOTE_D) )
					nParseFlags ^= PD_INQUOTE_S;
			}
		}
		else if ( nParseFlags )
		{
			if ( nParseFlags & PD_TEXTORWS )
			{
				if ( cD == _T('<') )
				{
					nR = (int)(pDoc - token.szDoc) - 1;
					nNodeType = MNT_WHITESPACE;
					break;
				}
				else if ( ! _tcschr(_T(" \t\n\r"),(_TCHAR)cD) )
				{
					nParseFlags ^= PD_TEXTORWS;
					FINDNODETYPE( _T("<"), MNT_TEXT, 0 )
				}
			}
			else if ( nParseFlags & PD_OPENTAG )
			{
				nParseFlags ^= PD_OPENTAG;
				if ( cD > 0x60 || ( cD > 0x40 && cD < 0x5b ) || cD == 0x5f || cD == 0x3a )
					FINDNODETYPE( _T(">"), MNT_ELEMENT, 1 )
				else if ( cD == _T('/') )
					FINDNODETYPE( _T(">"), 0, 2 )
				else if ( cD == _T('!') )
					nParseFlags |= PD_BANG;
				else if ( cD == _T('?') )
					FINDNODETYPE( _T("?>"), MNT_PROCESSING_INSTRUCTION, 2 )
				else
					ISNODEERROR( _T("tag name character") )
			}
			else if ( nParseFlags & PD_BANG )
			{
				nParseFlags ^= PD_BANG;
				if ( cD == _T('-') )
					nParseFlags |= PD_DASH;
				else if ( cD == _T('[') && !(nParseFlags & PD_DOCTYPE) )
					nParseFlags |= PD_BRACKET;
				else if ( cD == _T('D') && !(nParseFlags & PD_DOCTYPE) )
					nParseFlags |= PD_DOCTYPE;
				else if ( _tcschr(_T("EAN"),(_TCHAR)cD) ) // <!ELEMENT ATTLIST ENTITY NOTATION
					FINDNODETYPE( _T(">"), -1, 0 )
				else
					ISNODEERROR( _T("! tag") )
			}
			else if ( nParseFlags & PD_DASH )
			{
				nParseFlags ^= PD_DASH;
				if ( cD == _T('-') )
					FINDNODETYPE( _T("-->"), MNT_COMMENT, 0 )
				else
					ISNODEERROR( _T("comment tag") )
			}
			else if ( nParseFlags & PD_BRACKET )
			{
				nParseFlags ^= PD_BRACKET;
				if ( cD == _T('C') )
					FINDNODETYPE( _T("]]>"), MNT_CDATA_SECTION, 0 )
				else
					ISNODEERROR( _T("tag") )
			}
			else if ( nParseFlags & PD_DOCTYPE )
			{
				if ( cD == _T('<') )
					nParseFlags |= PD_OPENTAG;
				else if ( cD == _T('>') )
				{
					nR = (int)(pDoc - token.szDoc);
					nNodeType = MNT_DOCUMENT_TYPE;
					break;
				}
			}
		}
		else if ( cD == _T('<') )
		{
			nParseFlags |= PD_OPENTAG;
		}
		else
		{
			nNodeType = MNT_WHITESPACE;
			if ( _tcschr(_T(" \t\n\r"),(_TCHAR)cD) )
				nParseFlags |= PD_TEXTORWS;
			else
				FINDNODETYPE( _T("<"), MNT_TEXT, 0 )
		}
		pDoc += _tclen( pDoc );
	}
	token.nNext = nR + 1;
	node.nLength = token.nNext - node.nStart;
	node.nNodeType = nNodeType;
	return nNodeType;
}

CString CMarkup::x_GetTagName( int iPos ) const
{
	// Return the tag name at specified element
	TokenPos token( m_strDoc );
	token.nNext = m_aPos[iPos].nStart + 1;
	if ( ! iPos || ! x_FindToken( token ) )
		return _T("");

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkup::x_FindAttrib( CMarkup::TokenPos& token, LPCTSTR szAttrib )
{
	// If szAttrib is NULL find next attrib, otherwise find named attrib
	// Return true if found
	int nAttrib = 0;
	for ( int nCount = 0; x_FindToken(token); ++nCount )
	{
		if ( ! token.bIsString )
		{
			// Is it the right angle bracket?
			_TCHAR cChar = token.szDoc[token.nL];
			if ( cChar == _T('>') || cChar == _T('/') || cChar == _T('?') )
				break; // attrib not found

			// Equal sign
			if ( cChar == _T('=') )
				continue;

			// Potential attribute
			if ( ! nAttrib && nCount )
			{
				// Attribute name search?
				if ( ! szAttrib || ! szAttrib[0] )
					return true; // return with token at attrib name

				// Compare szAttrib
				if ( token.Match(szAttrib) )
					nAttrib = nCount;
			}
		}
		else if ( nAttrib && nCount == nAttrib + 2 )
		{
			return true;
		}
	}

	// Not found
	return false;
}

CString CMarkup::x_GetAttrib( int iPos, LPCTSTR szAttrib ) const
{
	// Return the value of the attrib
	TokenPos token( m_strDoc );
	if ( iPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_aPos[iPos].nStart + 1;
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
		token.nNext = m_nNodeOffset + 2;
	else
		return _T("");

	if ( szAttrib && x_FindAttrib( token, szAttrib ) )
		return x_TextFromDoc( token.nL, token.Length() );
	return _T("");
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, int nValue )
{
	// Convert integer to string
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_SetAttrib( iPos, szAttrib, szVal );
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, LPCTSTR szValue )
{
	// Set attribute in iPos element
	TokenPos token( m_strDoc );
	int nInsertAt;
	if ( iPos && m_nNodeType == MNT_ELEMENT )
	{
		token.nNext = m_aPos[iPos].nStart + 1;
		nInsertAt = m_aPos[iPos].StartContent() - (m_aPos[iPos].IsEmptyElement()?2:1);
	}
	else if ( iPos == m_iPos && m_nNodeLength && m_nNodeType == MNT_PROCESSING_INSTRUCTION )
	{
		token.nNext = m_nNodeOffset + 2;
		nInsertAt = m_nNodeOffset + m_nNodeLength - 2;
	}
	else
		return false;

	// Create insertion text depending on whether attribute already exists
	int nReplace = 0;
	CString strInsert;
	if ( x_FindAttrib( token, szAttrib ) )
	{
		// Replace value only
		// Decision: for empty value leaving attrib="" instead of removing attrib
		strInsert = x_TextToDoc( szValue, true );
		nInsertAt = token.nL;
		nReplace = token.Length();
	}
	else
	{
		// Insert string name value pair
		CString strFormat;
		strFormat = _T(" ");
		strFormat += szAttrib;
		strFormat += _T("=") x_ATTRIBQUOTE;
		strFormat += x_TextToDoc( szValue, true );
		strFormat += x_ATTRIBQUOTE;
		strInsert = strFormat;
	}

	x_DocChange( nInsertAt, nReplace, strInsert );
	int nAdjust = strInsert.GetLength() - nReplace;
	m_aPos[iPos].AdjustStartTagLen( nAdjust );
	m_aPos[iPos].nLength += nAdjust;
	x_Adjust( iPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}


bool CMarkup::x_CreateNode( CString& strNode, int nNodeType, LPCTSTR szText )
{
	// Set strNode based on nNodeType and szData
	// Return false if szData would jeopardize well-formed document
	//
	switch ( nNodeType )
	{
	case MNT_CDATA_SECTION:
		if ( _tcsstr(szText,_T("]]>")) != NULL )
			return false;
		strNode = "<![CDATA[";
		strNode += szText;
		strNode += "]]>";
		break;
	}
	return true;
}

bool CMarkup::x_SetData( int iPos, int nValue )
{
	// Convert integer to string
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_SetData( iPos, szVal, 0 );
}

bool CMarkup::x_SetData( int iPos, LPCTSTR szData, int nCDATA )
{
	// Set data at specified position
	// if nCDATA==1, set content of element to a CDATA Section
	CString strInsert;


	// Set data in iPos element
	if ( ! iPos || m_aPos[iPos].iElemChild )
		return false;

	// Build strInsert from szData based on nCDATA
	if ( nCDATA != 0 )
	{
		// Split CDATA Sections if there are any end delimiters
		LPCTSTR pszNextStart = szData;
		strInsert += _T("<![CDATA[");
		LPCTSTR pszEnd = _tcsstr( szData, _T("]]>") );
		while ( pszEnd )
		{
			strInsert += CString( pszNextStart, (int)(pszEnd - pszNextStart) );
			strInsert += _T("]]]]><![CDATA[>");
			pszNextStart = pszEnd + 3;
			pszEnd = _tcsstr( pszNextStart, _T("]]>") );
		}
		strInsert += pszNextStart;
		strInsert += _T("]]>");
	}
	else
		strInsert = x_TextToDoc( szData );

	// Decide where to insert
	int nInsertAt, nReplace;
	if ( m_aPos[iPos].IsEmptyElement() )
	{
		CString strTagName = x_GetTagName( iPos );
		CString strFormat;
		strFormat = _T(">");
		strFormat += strInsert;
		strFormat += _T("</");
		strFormat += strTagName;
		strInsert = strFormat;
		nInsertAt = m_aPos[iPos].StartAfter() - 2;
		nReplace = 1;
		m_aPos[iPos].AdjustStartTagLen( -1 );
		m_aPos[iPos].SetEndTagLen( 3 + strTagName.GetLength() );
	}
	else
	{
		nInsertAt = m_aPos[iPos].StartContent();
		nReplace = m_aPos[iPos].ContentLen();
	}
	x_DocChange( nInsertAt, nReplace, strInsert );
	int nAdjust = strInsert.GetLength() - nReplace;
	x_Adjust( iPos, nAdjust );
	m_aPos[iPos].nLength += nAdjust;
	MARKUP_SETDEBUGSTATE;
	return true;
}

CString CMarkup::x_GetData( int iPos ) const
{

	// Return a string representing data between start and end tag
	// Return empty string if there are any children elements
	CString strData;
	if ( ! m_aPos[iPos].iElemChild && ! m_aPos[iPos].IsEmptyElement() )
	{
		// Quick scan for any tags inside content
		int nContentLen = m_aPos[iPos].ContentLen();
		int nStartContent = m_aPos[iPos].StartContent();
		LPCTSTR pszContent = &((LPCTSTR)m_strDoc)[nStartContent];
		LPCTSTR pszTag = _tcschr( pszContent, _T('<') );
		if ( (int)(pszTag-pszContent) < nContentLen )
		{
			// Concatenate all CDATA Sections and text nodes, ignore other nodes
			TokenPos token( m_strDoc );
			token.nNext = nStartContent;
			NodePos node;
			while ( token.nNext < nStartContent + nContentLen )
			{
				x_ParseNode( token, node );
				if ( node.nNodeType == MNT_TEXT )
					strData += x_TextFromDoc( node.nStart, node.nLength );
				else if ( node.nNodeType == MNT_CDATA_SECTION )
					strData += m_strDoc.Mid( node.nStart+9, node.nLength-12 );
			}
		}
		else // no tags
			strData = x_TextFromDoc( nStartContent, nContentLen );
	}
	return strData;
}

CString CMarkup::x_TextToDoc( LPCTSTR szText, bool bAttrib )
{
	// Convert text as seen outside XML document to XML friendly
	// replacing special characters with ampersand escape codes
	// E.g. convert "6>7" to "6&gt;7"
	//
	// &lt;   less than
	// &amp;  ampersand
	// &gt;   greater than
	//
	// and for attributes:
	//
	// &apos; apostrophe or single quote
	// &quot; double quote
	//
	static LPCTSTR szaReplace[] = { _T("&lt;"),_T("&amp;"),_T("&gt;"),_T("&apos;"),_T("&quot;") };
	LPCTSTR pFind = bAttrib?_T("<&>\'\""):_T("<&>");
	CString strText;
	LPCTSTR pSource = szText;
	int nDestSize = (int)_tcslen(pSource);
	nDestSize += nDestSize / 10 + 7;
	_TCHAR* pDest = strText.GetBuffer(nDestSize);
	int nLen = 0;
	_TCHAR cSource = *pSource;
	LPCTSTR pFound;
	while ( cSource )
	{
		if ( nLen > nDestSize - 6 )
		{
			strText.ReleaseBuffer(nLen);
			nDestSize *= 2;
			pDest = strText.GetBuffer(nDestSize);
		}
		if ( (pFound=_tcschr(pFind,cSource)) != NULL )
		{
			pFound = szaReplace[pFound-pFind];
			_tcscpy(&pDest[nLen],pFound);
			nLen += (int)_tcslen(pFound);
		}
		else
		{
			_tccpy( &pDest[nLen], pSource );
			nLen += (int)_tclen( pSource );
		}
		pSource += _tclen( pSource );
		cSource = *pSource;
	}
	strText.ReleaseBuffer(nLen);
	return strText;
}

CString CMarkup::x_TextFromDoc( int nLeft, int nCopy ) const
{
	// Convert XML friendly text to text as seen outside XML document
	// ampersand escape codes replaced with special characters e.g. convert "6&gt;7" to "6>7"
	// ampersand numeric codes replaced with character e.g. convert &#60; to <
	// Conveniently the result is always the same or shorter in byte length
	//
	static LPCTSTR szaCode[] = { _T("lt;"),_T("amp;"),_T("gt;"),_T("apos;"),_T("quot;") };
	static int anCodeLen[] = { 3,4,3,5,5 };
	static LPCTSTR szSymbol = _T("<&>\'\"");
	CString strText;
	LPCTSTR pSource = m_strDoc;
	int nEnd = nLeft + nCopy;
	_TCHAR* pDest = strText.GetBuffer( nCopy );
	int nLen = 0;
	int nCharLen;
	int nChar = nLeft;
	while ( nChar < nEnd )
	{
		if ( pSource[nChar] == _T('&') )
		{
			bool bCodeConverted = false;

			// Is it a numeric character reference?
			if ( pSource[nChar+1] == _T('#') )
			{
				// Is it a hex number?
				int nBase = 10;
				int nNumericChar = nChar + 2;
				_TCHAR cChar = pSource[nNumericChar];
				if ( cChar == _T('x') )
				{
					++nNumericChar;
					cChar = pSource[nNumericChar];
					nBase = 16;
				}

				// Look for terminating semi-colon within 7 characters
				int nCodeLen = 0;
				while ( nCodeLen < 7 && cChar && cChar != _T(';') )
				{
					// only ASCII digits 0-9, A-F, a-f expected
					nCodeLen += (int)_tclen( &pSource[nNumericChar+nCodeLen] );
					cChar = pSource[nNumericChar + nCodeLen];
				}

				// Process unicode
				if ( cChar == _T(';') )
				{
					int nUnicode = _tcstol( &pSource[nNumericChar], NULL, nBase );
#if defined(_UNICODE)
					pDest[nLen++] = (_TCHAR)nUnicode;
#elif defined(_MBCS)
					int nMBLen = wctomb( &pDest[nLen], (wchar_t)nUnicode );
					if ( nMBLen > 0 )
						nLen += nMBLen;
					else
						nUnicode = 0;
#else
					if ( nUnicode < 0x80 )
						pDest[nLen++] = (_TCHAR)nUnicode;
					else if ( nUnicode < 0x800 )
					{
						// Convert to 2-byte UTF-8
						pDest[nLen++] = (_TCHAR)(((nUnicode&0x7c0)>>6) | 0xc0);
						pDest[nLen++] = (_TCHAR)((nUnicode&0x3f) | 0x80);
					}
					else
					{
						// Convert to 3-byte UTF-8
						pDest[nLen++] = (_TCHAR)(((nUnicode&0xf000)>>12) | 0xe0);
						pDest[nLen++] = (_TCHAR)(((nUnicode&0xfc0)>>6) | 0x80);
						pDest[nLen++] = (_TCHAR)((nUnicode&0x3f) | 0x80);
					}
#endif
					if ( nUnicode )
					{
						// Increment index past ampersand semi-colon
						nChar = nNumericChar + nCodeLen + 1;
						bCodeConverted = true;
					}
				}
			}
			else // does not start with #
			{
				// Look for matching &code;
				for ( int nMatch = 0; nMatch < 5; ++nMatch )
				{
					if ( nChar < nEnd - anCodeLen[nMatch]
						&& _tcsncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
					{
						// Insert symbol and increment index past ampersand semi-colon
						pDest[nLen++] = szSymbol[nMatch];
						nChar += anCodeLen[nMatch] + 1;
						bCodeConverted = true;
						break;
					}
				}
			}

			// If the code is not converted, leave it as is
			if ( ! bCodeConverted )
			{
				pDest[nLen++] = _T('&');
				++nChar;
			}
		}
		else // not &
		{
			nCharLen = (int)_tclen(&pSource[nChar]);
			_tccpy( &pDest[nLen], &pSource[nChar] );
			nLen += nCharLen;
			nChar += nCharLen;
		}
	}
	strText.ReleaseBuffer(nLen);
	return strText;
}

void CMarkup::x_DocChange( int nLeft, int nReplace, const CString& strInsert )
{
	// Insert strInsert int m_strDoc at nLeft replacing nReplace chars
	// Do this with only one buffer reallocation if it grows
	//
	int nDocLength = m_strDoc.GetLength();
	int nInsLength = strInsert.GetLength();
	int nNewLength = nInsLength + nDocLength - nReplace;

	// When creating a document, reduce reallocs by reserving string space
	// Allow for 1.5 times the current allocation
	int nBufferLen = nNewLength;
	int nAllocLen = ((CStringData*)((LPCTSTR)m_strDoc)-1)->nAllocLength;
	if ( nNewLength > nAllocLen )
	{
		nBufferLen += nBufferLen/2 + 128;
		if ( nBufferLen < nNewLength )
			nBufferLen = nNewLength;
	}
	_TCHAR* pDoc = m_strDoc.GetBuffer( nBufferLen );

	// Move part of old doc that goes after insert, then copy insert into it
	if ( nLeft+nReplace < nDocLength )
		memmove( &pDoc[nLeft+nInsLength], &pDoc[nLeft+nReplace], (nDocLength-nLeft-nReplace)*sizeof(_TCHAR) );
	memcpy( &pDoc[nLeft], strInsert, nInsLength*sizeof(_TCHAR) );
	m_strDoc.ReleaseBuffer( nNewLength );

}

void CMarkup::x_Adjust( int iPos, int nShift, bool bAfterPos )
{
	// Loop through affected elements and adjust indexes
	// Algorithm:
	// 1. update children unless bAfterPos
	//    (if no children or bAfterPos is true, length of iPos not affected)
	// 2. update starts of next siblings and their children
	// 3. go up until there is a next sibling of a parent and update starts
	// 4. step 2
	int iPosTop = m_aPos[iPos].iElemParent;
	bool bPosFirst = bAfterPos; // mark as first to skip its children
	while ( iPos )
	{
		// Were we at containing parent of affected position?
		bool bPosTop = false;
		if ( iPos == iPosTop )
		{
			// Move iPosTop up one towards root
			iPosTop = m_aPos[iPos].iElemParent;
			bPosTop = true;
		}

		// Traverse to the next update position
		if ( ! bPosTop && ! bPosFirst && m_aPos[iPos].iElemChild )
		{
			// Depth first
			iPos = m_aPos[iPos].iElemChild;
		}
		else if ( m_aPos[iPos].iElemNext )
		{
			iPos = m_aPos[iPos].iElemNext;
		}
		else
		{
			// Look for next sibling of a parent of iPos
			// When going back up, parents have already been done except iPosTop
			while ( (iPos=m_aPos[iPos].iElemParent) != 0 && iPos != iPosTop )
				if ( m_aPos[iPos].iElemNext )
				{
					iPos = m_aPos[iPos].iElemNext;
					break;
				}
		}
		bPosFirst = false;

		// Shift indexes at iPos
		if ( iPos != iPosTop )
			m_aPos[iPos].nStart += nShift;
		else
			m_aPos[iPos].nLength += nShift;
	}
}

void CMarkup::x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags )
{
	// Determine where to insert new element or node
	//
	bool bInsert = (nFlags&1)?true:false;
	bool bHonorWhitespace = (nFlags&2)?true:false;

	int nStartAt;
	if ( nLength )
	{
		// Located at a non-element node
		if ( bInsert )
			nStartAt = nOffset;
		else
			nStartAt = nOffset + nLength;
	}
	else if ( iPosRel )
	{
		// Located at an element
		nStartAt = m_aPos[iPosRel].nStart;
		if ( ! bInsert ) // follow iPosRel
			nStartAt += m_aPos[iPosRel].nLength;
	}
	else if ( ! iPosParent )
	{
		// Outside of all elements
		if ( bInsert )
			nStartAt = 0;
		else
			nStartAt = m_strDoc.GetLength();
	}
	else if ( m_aPos[iPosParent].IsEmptyElement() )
	{
		// Parent has no separate end tag, so split empty element
		nStartAt = m_aPos[iPosParent].StartContent() - 1;
	}
	else
	{
		if ( bInsert ) // after start tag
			nStartAt = m_aPos[iPosParent].StartContent();
		else // before end tag
			nStartAt = m_aPos[iPosParent].StartAfter() - m_aPos[iPosParent].EndTagLen();
	}

	// Go up to start of next node, unless its splitting an empty element
	if ( ! bHonorWhitespace && ! m_aPos[iPosParent].IsEmptyElement() )
	{
		LPCTSTR szDoc = (LPCTSTR)m_strDoc;
		int nChar = nStartAt;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
			nStartAt = nChar;
	}

	// Determine iPosBefore
	int iPosBefore = 0;
	if ( iPosRel )
	{
		if ( bInsert )
		{
			if ( ! (m_aPos[iPosRel].nFlags & MNF_FIRST) )
				iPosBefore = m_aPos[iPosRel].iElemPrev;
		}
		else
			iPosBefore = iPosRel;
	}
	else if ( m_aPos[iPosParent].iElemChild )
	{
		if ( bInsert )
			iPosBefore = iPosRel;
		else
			iPosBefore = m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev;
	}

	nOffset = nStartAt;
	iPosRel = iPosBefore;
}

bool CMarkup::x_AddElem( LPCTSTR szName, int nValue, bool bInsert, bool bAddChild )
{
	// Convert integer to string
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_AddElem( szName, szVal, bInsert, bAddChild );
}

bool CMarkup::x_AddElem( LPCTSTR szName, LPCTSTR szValue, bool bInsert, bool bAddChild )
{
	if ( bAddChild )
	{
		// Adding a child element under main position
		if ( ! m_iPos )
			return false;
	}
	else if ( m_iPosParent == 0 )
	{
		// Adding root element
		if ( IsWellFormed() )
			return false;


		// Locate after any version and DTD
		m_aPos[0].nLength = m_strDoc.GetLength();
	}

	// Locate where to add element relative to current node
	int iPosParent, iPosBefore, nOffset = 0, nLength = 0;
	if ( bAddChild )
	{
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
		nOffset = m_nNodeOffset;
		nLength = m_nNodeLength;
	}
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, nLength, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	bool bNoContentParent = (iPosParent && m_aPos[iPosParent].ContentLen() == 0)?true:false;
	if ( bEmptyParent || bNoContentParent )
		nOffset += x_EOLLEN;

	// Create element and modify positions of affected elements
	// If no szValue is specified, an empty element is created
	// i.e. either <NAME>value</NAME> or <NAME/>
	//
	int iPos = x_GetFreePos();
	ElemPos* pElem = &m_aPos[iPos];
	pElem->nStart = nOffset;
	pElem->iElemChild = 0;
	x_LinkElem( iPosParent, iPosBefore, iPos );

	// Create string for insert
	CString strInsert;
	int nLenName = (int)_tcslen(szName);
	int nLenValue = szValue? (int)_tcslen(szValue) : 0;
	if ( ! nLenValue )
	{
		// <NAME/> empty element
		strInsert = _T("<");
		strInsert += szName;
		strInsert += _T("/>") x_EOL;
		pElem->SetStartTagLen( nLenName + 3 );
		pElem->SetEndTagLen( 0 );
		pElem->nLength = nLenName + 3;
	}
	else
	{
		// <NAME>value</NAME>
		CString strValue = x_TextToDoc( szValue );
		nLenValue = strValue.GetLength();
		strInsert = _T("<");
		strInsert += szName;
		strInsert += _T(">");
		strInsert += strValue;
		strInsert += _T("</");
		strInsert += szName;
		strInsert += _T(">") x_EOL;
		pElem->SetStartTagLen( nLenName + 2 );
		pElem->SetEndTagLen( nLenName + 3 );
		pElem->nLength = nLenName * 2 + nLenValue + 5;
	}

	// Insert
	int nReplace = 0, nInsertAt = pElem->nStart;
	if ( bEmptyParent )
	{
		CString strParentTagName = x_GetTagName(iPosParent);
		CString strFormat;
		strFormat = _T(">") x_EOL;
		strFormat += strInsert;
		strFormat += _T("</");
		strFormat += strParentTagName;
		strInsert = strFormat;
		nInsertAt = m_aPos[iPosParent].StartContent() - 2;
		nReplace = 1;
		m_aPos[iPosParent].AdjustStartTagLen( -1 );
		m_aPos[iPosParent].SetEndTagLen( 3 + strParentTagName.GetLength() );
	}
	else if ( bNoContentParent )
	{
		strInsert = x_EOL + strInsert;
		nInsertAt = m_aPos[iPosParent].StartContent();
	}
	x_DocChange( nInsertAt, nReplace, strInsert );
	x_Adjust( iPos, strInsert.GetLength() - nReplace );

	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else
		x_SetPos( iPosParent, iPos, 0 );
	return true;
}

CString CMarkup::x_GetSubDoc( int iPos ) const
{
	if ( iPos )
	{
		int nStart = m_aPos[iPos].nStart;
		int nNext = nStart + m_aPos[iPos].nLength;
		LPCTSTR szDoc = (LPCTSTR)m_strDoc;
		int nChar = nNext;
		if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
			nNext = nChar;
		return m_strDoc.Mid( nStart, nNext - nStart );
	}
	return _T("");
}

bool CMarkup::x_AddSubDoc( LPCTSTR szSubDoc, bool bInsert, bool bAddChild )
{
	// Add subdocument, parse, and modify positions of affected elements
	//
	int nOffset = 0, iPosParent, iPosBefore;
	if ( bAddChild )
	{
		// Add a subdocument under main position, before or after child
		if ( ! m_iPos )
			return false;
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		// Add a subdocument under parent position, before or after main
		if ( ! m_iPosParent )
			return false;
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
	}

	// Determine insert location
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, 0, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	if ( bEmptyParent )
		nOffset += x_EOLLEN;

	// Parse subdocument
	TokenPos token( szSubDoc );
	int iPosTempParent = x_GetFreePos();
	int iPosFreeBeforeAdd = m_iPosFree;
	m_aPos[iPosTempParent].ClearVirtualParent();
	m_aPos[iPosTempParent].SetLevel( m_aPos[iPosParent].Level() + 1 );
	int iPos = x_ParseElem( iPosTempParent, token );
	m_aPos[iPosTempParent].nFlags = MNF_DELETED;
	if ( iPos <= 0 )
	{
		// Abort because not well-formed
		m_iPosFree = iPosFreeBeforeAdd;
		m_aPos[iPosTempParent].iElemNext = m_iPosDeleted;
		m_iPosDeleted = iPosTempParent;
		return false;
	}

	// Extract subdocument without leading/trailing nodes
	CString strInsert;
	int nExtractStart = m_aPos[iPos].nStart;
	int nExtractLength = m_aPos[iPos].nLength;
	memcpy( strInsert.GetBuffer(nExtractLength+x_EOLLEN), &szSubDoc[nExtractStart], nExtractLength*sizeof(_TCHAR) );
	strInsert.ReleaseBuffer( nExtractLength );
	if ( x_EOLLEN )
		strInsert += x_EOL;

	// Do an adjust here, using iPosTempParent so that it does not affect rest of document
	int nAdjust = nOffset - nExtractStart;
	m_aPos[iPos].nStart += nAdjust;
	x_Adjust( iPos, nAdjust );

	// Link in parent and siblings
	x_LinkElem( iPosParent, iPosBefore, iPos );
	m_aPos[iPosTempParent].iElemNext = m_iPosDeleted;
	m_iPosDeleted = iPosTempParent;

	// Insert subdocument
	int nReplace = 0, nInsertAt = nOffset;
	if ( bEmptyParent )
	{
		CString strParentTagName = x_GetTagName(iPosParent);
		ElemPos* pParent = &m_aPos[iPosParent];
		CString strFormat;
		strFormat = _T(">") x_EOL;
		strFormat += strInsert;
		strFormat += _T("</");
		strFormat += strParentTagName;
		strInsert = strFormat;
		nInsertAt = pParent->StartContent() - 2;
		nReplace = 1;
		pParent->AdjustStartTagLen( -1 );
		pParent->SetEndTagLen( 3 + strParentTagName.GetLength() );
	}
	x_DocChange( nInsertAt, nReplace, strInsert );
	x_Adjust( iPos, strInsert.GetLength() - nReplace, true );

	// Set position to top element of subdocument
	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else // Main
		x_SetPos( m_iPosParent, iPos, 0 );
	return true;
}

int CMarkup::x_RemoveElem( int iPos )
{
	// Remove element and all contained elements
	// Return new position
	//
	if ( ! iPos )
		return 0;

	// Determine whether any whitespace up to next tag
	int nAfterEnd = m_aPos[iPos].StartAfter();
	LPCTSTR szDoc = (LPCTSTR)m_strDoc;
	int nChar = nAfterEnd;
	if ( ! x_FindAny(szDoc,nChar) || szDoc[nChar] == _T('<') )
		nAfterEnd = nChar;

	// Remove from document, adjust affected indexes, and unlink
	int nLen = nAfterEnd - m_aPos[iPos].nStart;
	x_DocChange( m_aPos[iPos].nStart, nLen, CString() );
	x_Adjust( iPos, - nLen, true );
	return x_UnlinkElem( iPos );
}

void CMarkup::x_LinkElem( int iPosParent, int iPosBefore, int iPos )
{
	// Link in element, and initialize nFlags, and iElem indexes
	ElemPos* pElem = &m_aPos[iPos];
	pElem->iElemParent = iPosParent;
	if ( iPosBefore )
	{
		// Link in after iPosBefore
		pElem->nFlags = 0;
		pElem->iElemNext = m_aPos[iPosBefore].iElemNext;
		if ( pElem->iElemNext )
			m_aPos[pElem->iElemNext].iElemPrev = iPos;
		else
			m_aPos[m_aPos[iPosParent].iElemChild].iElemPrev = iPos;
		m_aPos[iPosBefore].iElemNext = iPos;
		pElem->iElemPrev = iPosBefore;
	}
	else
	{
		// Link in as first child
		pElem->nFlags = MNF_FIRST;
		if ( m_aPos[iPosParent].iElemChild )
		{
			pElem->iElemNext = m_aPos[iPosParent].iElemChild;
			pElem->iElemPrev = m_aPos[pElem->iElemNext].iElemPrev;
			m_aPos[pElem->iElemNext].iElemPrev = iPos;
			m_aPos[pElem->iElemNext].nFlags ^= MNF_FIRST;
		}
		else
		{
			pElem->iElemNext = 0;
			pElem->iElemPrev = iPos;
		}
		m_aPos[iPosParent].iElemChild = iPos;
	}
	if ( iPosParent )
		pElem->SetLevel( m_aPos[iPosParent].Level() + 1 );
}

int CMarkup::x_UnlinkElem( int iPos )
{
	// Fix links to remove element and mark as deleted
	// return previous position or zero if none
	ElemPos* pElem = &m_aPos[iPos];

	// Find previous sibling and bypass removed element
	int iPosPrev = 0;
	if ( pElem->nFlags & MNF_FIRST )
	{
		if ( pElem->iElemNext ) // set next as first child
		{
			m_aPos[pElem->iElemParent].iElemChild = pElem->iElemNext;
			m_aPos[pElem->iElemNext].iElemPrev = pElem->iElemPrev;
			m_aPos[pElem->iElemNext].nFlags |= MNF_FIRST;
		}
		else // no children remaining
			m_aPos[pElem->iElemParent].iElemChild = 0;
	}
	else
	{
		iPosPrev = pElem->iElemPrev;
		m_aPos[iPosPrev].iElemNext = pElem->iElemNext;
		if ( pElem->iElemNext )
			m_aPos[pElem->iElemNext].iElemPrev = iPosPrev;
		else
			m_aPos[m_aPos[pElem->iElemParent].iElemChild].iElemPrev = iPosPrev;
	}
	pElem->nFlags = MNF_DELETED;
	pElem->iElemNext = m_iPosDeleted;
	m_iPosDeleted = iPos;
	return iPosPrev;
}


MyNameSpaceEnd