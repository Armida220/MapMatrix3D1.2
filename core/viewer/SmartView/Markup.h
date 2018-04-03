// Markup.h: interface for the CMarkup class.
//
// Markup Release 7.3
// Copyright (C) 1999-2004 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#if !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
#define AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#define _DS(i) (i?&((LPCTSTR)m_strDoc)[m_aPos[i].nStart]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_iPos); m_pChildDS=_DS(m_iPosChild)
#else
#define MARKUP_SETDEBUGSTATE
#endif

MyNameSpaceBegin


class EXPORT_SMARTVIEW CMarkup  
{
public:
	CMarkup() { SetDoc( NULL ); m_nFlags=0; };
	CMarkup( LPCTSTR szDoc ) { SetDoc( szDoc ); m_nFlags=0; };
	CMarkup( const CMarkup& markup ) { *this = markup; };
	void operator=( const CMarkup& markup );
	~CMarkup() {};

	// Navigate
	bool Load( LPCTSTR szFileName );
	bool SetDoc( LPCTSTR szDoc );
	bool IsWellFormed();
	bool FindElem( LPCTSTR szName=NULL, BOOL bFromTop=FALSE );
	bool FindChildElem( LPCTSTR szName=NULL );
	bool IntoElem();
	bool OutOfElem();
	void ResetChildPos() { x_SetPos(m_iPosParent,m_iPos,0); };
	void ResetMainPos() { x_SetPos(m_iPosParent,0,0); };
	void ResetPos() { x_SetPos(0,0,0); };
	CString GetTagName() const;
	CString GetChildTagName() const { return x_GetTagName(m_iPosChild); };
	CString GetData() const { return x_GetData(m_iPos); };
	CString GetChildData() const { return x_GetData(m_iPosChild); };
	CString GetAttrib( LPCTSTR szAttrib ) const { return x_GetAttrib(m_iPos,szAttrib); };
	CString GetChildAttrib( LPCTSTR szAttrib ) const { return x_GetAttrib(m_iPosChild,szAttrib); };
	CString GetAttribName( int n ) const;
	bool SavePos( LPCTSTR szPosName=_T("") );
	bool RestorePos( LPCTSTR szPosName=_T("") );
	const CString& GetError() const { return m_strError; };

	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01
		MNT_TEXT					= 2,  // 0x02
		MNT_WHITESPACE				= 4,  // 0x04
		MNT_CDATA_SECTION			= 8,  // 0x08
		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10
		MNT_COMMENT					= 32, // 0x20
		MNT_DOCUMENT_TYPE			= 64, // 0x40
		MNT_EXCLUDE_WHITESPACE		= 123,// 0x7b
	};

	// Create
	bool Save( LPCTSTR szFileName );
	const CString& GetDoc() const { return m_strDoc; };
	bool AddElem( LPCTSTR szName, LPCTSTR szData=NULL ) { return x_AddElem(szName,szData,false,false); };
	bool InsertElem( LPCTSTR szName, LPCTSTR szData=NULL ) { return x_AddElem(szName,szData,true,false); };
	bool AddChildElem( LPCTSTR szName, LPCTSTR szData=NULL ) { return x_AddElem(szName,szData,false,true); };
	bool InsertChildElem( LPCTSTR szName, LPCTSTR szData=NULL ) { return x_AddElem(szName,szData,true,true); };
	bool AddElem( LPCTSTR szName, int nValue ) { return x_AddElem(szName,nValue,false,false); };
	bool InsertElem( LPCTSTR szName, int nValue ) { return x_AddElem(szName,nValue,true,false); };
	bool AddChildElem( LPCTSTR szName, int nValue ) { return x_AddElem(szName,nValue,false,true); };
	bool InsertChildElem( LPCTSTR szName, int nValue ) { return x_AddElem(szName,nValue,true,true); };
	bool AddAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool AddChildAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
	bool AddAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
	bool AddChildAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
	bool AddSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,false,false); };
	bool InsertSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,true,false); };
	CString GetSubDoc() const { return x_GetSubDoc(m_iPos); };
	bool AddChildSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,false,true); };
	bool InsertChildSubDoc( LPCTSTR szSubDoc ) { return x_AddSubDoc(szSubDoc,true,true); };
	CString GetChildSubDoc() const { return x_GetSubDoc(m_iPosChild); };

	// Modify
	bool RemoveElem();
	bool RemoveChildElem();
	bool SetAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPos,szAttrib,szValue); };
	bool SetChildAttrib( LPCTSTR szAttrib, LPCTSTR szValue ) { return x_SetAttrib(m_iPosChild,szAttrib,szValue); };
	bool SetAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPos,szAttrib,nValue); };
	bool SetChildAttrib( LPCTSTR szAttrib, int nValue ) { return x_SetAttrib(m_iPosChild,szAttrib,nValue); };
	bool SetData( LPCTSTR szData, int nCDATA=0 ) { return x_SetData(m_iPos,szData,nCDATA); };
	bool SetChildData( LPCTSTR szData, int nCDATA=0 ) { return x_SetData(m_iPosChild,szData,nCDATA); };
	bool SetData( int nValue ) { return x_SetData(m_iPos,nValue); };
	bool SetChildData( int nValue ) { return x_SetData(m_iPosChild,nValue); };

	// Utility
	static bool ReadTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError=NULL, int* pnFlags=NULL );
	static bool WriteTextFile( LPCTSTR szFileName, CString& strDoc, CString* pstrError=NULL, int* pnFlags=NULL );

protected:

#ifdef _DEBUG
	LPCTSTR m_pMainDS;
	LPCTSTR m_pChildDS;
#endif

	CString m_strDoc;
	CString m_strError;

	int m_iPosParent;
	int m_iPos;
	int m_iPosChild;
	int m_iPosFree;
	int m_iPosDeleted;
	int m_nNodeType;
	int m_nNodeOffset;
	int m_nNodeLength;
	int m_nFlags;

	struct ElemPos
	{
		ElemPos() {};
		ElemPos( const ElemPos& pos ) { *this = pos; };
		enum { EP_STBITS=22, EP_STMASK=0x2fffff, EP_LEVMASK=0xffff };
		int StartTagLen() const { return (nTagLengths & EP_STMASK); };
		void SetStartTagLen( int n ) { nTagLengths = (nTagLengths & ~EP_STMASK) + n; };
		void AdjustStartTagLen( int n ) { nTagLengths += n; };
		int EndTagLen() const { return (nTagLengths >> EP_STBITS); };
		void SetEndTagLen( int n ) { nTagLengths = (nTagLengths & EP_STMASK) + (n << EP_STBITS); };
		bool IsEmptyElement() { return EndTagLen()?false:true; };
		int StartContent() const { return nStart + StartTagLen(); };
		int ContentLen() const { return nLength - StartTagLen() - EndTagLen(); };
		int StartAfter() const { return nStart + nLength; };
		int Level() const { return nFlags & EP_LEVMASK; };
		void SetLevel( int nLev ) { nFlags = (nFlags & ~EP_LEVMASK) | nLev; };
		void ClearVirtualParent() { memset(this,0,sizeof(ElemPos)); SetEndTagLen(1); };

		// Memory size: 8 32-bit integers == 32 bytes
		int nStart;
		int nLength;
		int nTagLengths; // 22 bits 4MB limit for start tag, 10 bits 1K limit for end tag
		int nFlags; // 16 bits flags, 16 bits level 65536 depth limit
		int iElemParent;
		int iElemChild; // first child
		int iElemNext;
		int iElemPrev; // if this is first child, iElemPrev points to last
	};

	enum MarkupNodeFlags
	{
		MNF_EMPTY   = 0x010000,
		MNF_DELETED = 0x020000,
		MNF_ERROR   = 0x040000,
		MNF_FIRST   = 0x080000,
	};

	struct NodePos
	{
		int nNodeType;
		int nStart;
		int nLength;
		int nFlags;
		CString strName;
	};

	struct TokenPos
	{
		TokenPos( LPCTSTR sz ) { Clear(); szDoc = sz; };
		void Clear() { nL=0; nR=-1; nNext=0; bIsString=false; };
		int Length() const { return nR - nL + 1; };
		bool Match( LPCTSTR szName )
		{
			int nLen = nR - nL + 1;
		// To ignore case, define MARKUP_IGNORECASE
		#ifdef MARKUP_IGNORECASE
			return ( (_tcsncicmp( &szDoc[nL], szName, nLen ) == 0)
		#else
			return ( (_tcsnccmp( &szDoc[nL], szName, nLen ) == 0)
		#endif
				&& ( szName[nLen] == _T('\0') || _tcschr(_T(" =/["),szName[nLen]) ) );
		};
		int nL;
		int nR;
		int nNext;
		LPCTSTR szDoc;
		bool bIsString;
	};

	struct SavedPos
	{
		SavedPos() { nSavedPosFlags=0; iPos=0; };
		CString strName;
		int iPos;
		int nSavedPosFlags;
	};

	struct SavedPosMap
	{
		SavedPosMap() { pTable = NULL; };
		~SavedPosMap() { RemoveAll(); };
		void RemoveAll() { if (pTable) Release(); pTable=NULL; };
		enum { SPM_SIZE = 7, SPM_MAIN = 1, SPM_CHILD = 2, SPM_USED = 4, SPM_LAST = 8 };
		void Release() { for (int n=0;n<SPM_SIZE;++n) if (pTable[n]) delete[] pTable[n]; delete[] pTable; };
		void AllocMapTable() { pTable = new SavedPos*[SPM_SIZE]; for (int n=0; n<SPM_SIZE; ++n) pTable[n]=NULL; };
		int Hash( LPCTSTR szName ) { int n=0; while (*szName) n += *szName++; return n % SPM_SIZE; };
		SavedPos** pTable;
	};
	SavedPosMap m_mapSavedPos;

	struct PosArray
	{
		PosArray() { Clear(); };
		~PosArray() { Release(); };
		enum { PA_SEGBITS = 16, PA_SEGMASK = 0xffff };
		void RemoveAll() { Release(); Clear(); };
		void Release() { for (int n=0;n<SegsUsed();++n) delete[] (char*)pSegs[n]; if (pSegs) delete[] (char*)pSegs; };
		void Clear() { nSegs=0; nSize=0; pSegs=NULL; };
		int GetSize() const { return nSize; };
		int SegsUsed() const { return ((nSize-1)>>PA_SEGBITS) + 1; };
		ElemPos& operator[](int n) const { return pSegs[n>>PA_SEGBITS][n&PA_SEGMASK]; };
		ElemPos** pSegs;
		int nSize;
		int nSegs;
	};
	PosArray m_aPos;

	struct NodeStack
	{
		NodeStack() { nTop=-1; nSize=0; pN=NULL; };
		~NodeStack() { if (pN) delete [] pN; };
		NodePos& Top() { return pN[nTop]; };
		void Add() { ++nTop; if (nTop==nSize) Alloc(nSize*2+6); };
		void Remove() { --nTop; };
		int TopIndex() { return nTop; };
	protected:
		void Alloc( int nNewSize ) { NodePos* pNNew = new NodePos[nNewSize]; Copy(pNNew); nSize=nNewSize; };
		void Copy( NodePos* pNNew ) { for(int n=0;n<nSize;++n) pNNew[n]=pN[n]; if (pN) delete [] pN; pN=pNNew; };
		NodePos* pN;
		int nSize;
		int nTop;
	};

	void x_SetPos( int iPosParent, int iPos, int iPosChild )
	{
		m_iPosParent = iPosParent;
		m_iPos = iPos;
		m_iPosChild = iPosChild;
		m_nNodeOffset = 0;
		m_nNodeLength = 0;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};
	int x_GetFreePos()
	{
		if ( m_iPosFree == m_aPos.GetSize() )
			x_AllocPosArray();
		return m_iPosFree++;
	};
	bool x_AllocPosArray( int nNewSize = 0 );
	bool x_ParseDoc();
	int x_ParseElem( int iPos, TokenPos& token );
	static bool x_FindChar( LPCTSTR szDoc, int& nChar, _TCHAR c );
	static bool x_FindAny( LPCTSTR szDoc, int& nChar );
	static bool x_FindToken( TokenPos& token );
	static CString x_GetToken( const TokenPos& token );
	int x_FindElem( int iPosParent, int iPos, LPCTSTR szPath );
	CString x_GetTagName( int iPos ) const;
	CString x_GetData( int iPos ) const;
	CString x_GetAttrib( int iPos, LPCTSTR szAttrib ) const;
	bool x_AddElem( LPCTSTR szName, LPCTSTR szValue, bool bInsert, bool bAddChild );
	bool x_AddElem( LPCTSTR szName, int nValue, bool bInsert, bool bAddChild );
	CString x_GetSubDoc( int iPos ) const;
	bool x_AddSubDoc( LPCTSTR szSubDoc, bool bInsert, bool bAddChild );
	static bool x_FindAttrib( TokenPos& token, LPCTSTR szAttrib=NULL );
	bool x_SetAttrib( int iPos, LPCTSTR szAttrib, LPCTSTR szValue );
	bool x_SetAttrib( int iPos, LPCTSTR szAttrib, int nValue );
	static bool x_CreateNode( CString& strNode, int nNodeType, LPCTSTR szText );
	void x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags );
	void x_LinkElem( int iPosParent, int iPosBefore, int iPos );
	int x_UnlinkElem( int iPos );
	static int x_ParseNode( TokenPos& token, NodePos& node );
	bool x_SetData( int iPos, LPCTSTR szData, int nCDATA );
	bool x_SetData( int iPos, int nValue );
	int x_RemoveElem( int iPos );
	void x_DocChange( int nLeft, int nReplace, const CString& strInsert );
	void x_Adjust( int iPos, int nShift, bool bAfterPos = false );
	static CString x_TextToDoc( LPCTSTR szText, bool bAttrib = false );
	CString x_TextFromDoc( int nLeft, int nCopy ) const;
};


MyNameSpaceEnd

#endif // !defined(AFX_MARKUP_H__948A2705_9E68_11D2_A0BF_00105A27C570__INCLUDED_)
