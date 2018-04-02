
/***************************************************************************************************
 This is a part of the Visiontek MapMatrix family.
 Copyright (C) 2005-2006 Visiontek Inc.
 All rights reserved.

 Module name: Public-CvsEncrypt
 Author: ¶¡¶ûÄÐ
 Description: Enhenced Sense-Lock

***************************************************************************************************/

#if !defined(AFX_CVSENCRYPT_H__4B9456E1_ECC6_44B0_8855_5DB2911B2B1D__INCLUDED_)
#define AFX_CVSENCRYPT_H__4B9456E1_ECC6_44B0_8855_5DB2911B2B1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_LICEDU)
#include "sense4_net.h"
#pragma comment( lib,"sense4.lib" )
#else
#include "sense4.h"
#pragma comment( lib,"sense4st.lib" )
#endif

#define DOG_REINIT			0x49457264
#define DOG_INI_ERROR		0x52456964
#define DOG_KEY_NOT_MATCH	0x4d6e4b44

/**************************************************************************************************

HRESULT WINAPI VS4Execute(
	IN		SENSE4_CONTEXT		*pS4Ctx,
	IN		LPCSTR				lpszFileID,
	IN		VOID				*pInBuffer,
	IN		DWORD				dwInbufferSize,
	OUT		VOID				*pOutBuffer,
	IN		DWORD				dwOutBufferSize,
	OUT		DWORD				*pdwBytesReturned
);

/**************************************************************************************************/

#endif // !defined(AFX_CVSENCRYPT_H__4B9456E1_ECC6_44B0_8855_5DB2911B2B1D__INCLUDED_)
