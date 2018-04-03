// GLDynamicTexture.h: interface for the GLDynamicTexture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLDYNAMICTEXTURE_H__5093AB12_F22C_4D5D_AAB1_67D64CA98A6B__INCLUDED_)
#define AFX_GLDYNAMICTEXTURE_H__5093AB12_F22C_4D5D_AAB1_67D64CA98A6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


MyNameSpaceBegin

bool CheckFBOSupported();

class CGLDynamicTexture  
{
public:
	CGLDynamicTexture();
	virtual ~CGLDynamicTexture();

	BOOL CreateFBO(int cx, int cy);
	BOOL CreateTwoDynamicTextures(int cx, int cy);
	BOOL BeginRenderToTexture(int idx);
	void EndRenderToTexture();

	void Clear();

	BOOL SetStencil(CSize szDC);
	void DisplayInterleavedTextures(CSize szDC, CRect rcView, BOOL bStartLeft);
		
	BOOL   m_bCreateFBO;
	CSize  m_szTexture;
	UINT	m_fboID;                     // ID of FBO
	UINT	m_rboID;                     // ID of Renderbuffer object: depth
	UINT	m_rboID2;					// ID of Renderbuffer object: stencil
	UINT	m_texID1, m_texID2; 

	BOOL   m_nInRendingTexture;

	CSize  m_szStencil;
};

MyNameSpaceEnd

#endif // !defined(AFX_GLDYNAMICTEXTURE_H__5093AB12_F22C_4D5D_AAB1_67D64CA98A6B__INCLUDED_)
