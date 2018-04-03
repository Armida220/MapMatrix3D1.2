
/***************************************************************************************************
 This is a part of the Visiontek MapMatrix family.
 Copyright (C) 2005-2006 Visiontek Inc.
 All rights reserved.

 Module name: Public-CvsEncrypt
 Author: ¶¡¶ûÄÐ
 Description: Enhenced Sense-Lock

***************************************************************************************************/

#include "StdAfx.h"
#include "vsEncrypt.h"
#include <AFXTEMPL.H>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


/**************************************************************************************************



HRESULT WINAPI VS4Execute( SENSE4_CONTEXT *pS4Ctx, LPCSTR lpszFileID,
						   VOID *pInBuffer, DWORD dwInbufferSize,
						   VOID *pOutBuffer, DWORD dwOutBufferSize, 
						   DWORD *pdwBytesReturned )
{
	HRESULT hr;
	static CArray< BYTE, BYTE& > arrBuf;

	short shKey;
	if ( arrBuf.GetSize() >= 6 )
	{
		shKey = *(short*)( arrBuf.GetData() + 4 );
	}

	arrBuf.SetSize( 0 );
	arrBuf.SetSize( __max( dwInbufferSize, dwOutBufferSize ) + 8  );

	memcpy( arrBuf.GetData() + 6, pInBuffer, dwInbufferSize );

	*(short*)( arrBuf.GetData() + 4 ) = shKey;

	hr = S4Execute( pS4Ctx, lpszFileID,
		arrBuf.GetData() + 4, dwInbufferSize + 2,
		arrBuf.GetData(), dwOutBufferSize + 6, pdwBytesReturned );
	*pdwBytesReturned -= 6;
	memcpy( pOutBuffer, arrBuf.GetData() + 6, dwOutBufferSize );

	if ( S4_SUCCESS == hr )
	{
		hr = *(HRESULT*)arrBuf.GetData();
	}

	arrBuf.SetSize( 6 );

	return hr;
}

/**************************************************************************************************/

