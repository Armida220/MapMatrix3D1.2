// D3DClass.cpp: implementation of the D3DClass class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "D3DClass.h"
#include "gl\glu.h"
#include "GlobalFunc.h"
#include "Matrix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



MyNameSpaceBegin




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CD3DWrapper::CD3DWrapper()
{
	m_pD3D9 = NULL;
	m_pD3DDevice9 = NULL;
	m_pBackSurface = NULL;
	m_pOffSurface = NULL;

	m_pPointList_VB = NULL;
	m_pLineStrip_VB = NULL;
	m_pLine = NULL;

	m_pTriangleList_VB = NULL;
	m_pTexture_VB = NULL;

	m_pRenderTexture = NULL;
	m_pTempTexture = NULL;
	m_pRenderTextureSurface = NULL;

	m_szTempTexture.cx = m_szTempTexture.cy = 0;
	m_szRenderTexture.cx = m_szRenderTexture.cy = 0;

	m_nBeginScene = 0;

	m_pDisplayTexture = NULL;

	m_bGDIPresent = FALSE;
	m_hBmpPresent = NULL;
	m_hDCPresent = NULL;
}

CD3DWrapper::~CD3DWrapper()
{
	Destory();
}

#define COM_RELEASE(p)		if( (p)!=NULL ){ (p)->Release(); (p)=NULL; }

void CD3DWrapper::Destory()
{	
	COM_RELEASE(m_pLine);

	COM_RELEASE(m_pTriangleList_VB);
	COM_RELEASE(m_pLineStrip_VB);		
	COM_RELEASE(m_pPointList_VB);
	COM_RELEASE(m_pTexture_VB);

	COM_RELEASE(m_pRenderTexture);
	COM_RELEASE(m_pTempTexture);
	COM_RELEASE(m_pDisplayTexture);

	for( int i=0; i<m_arrPAllocTextures.GetSize(); i++)
	{
		CD3DTexture *pText = m_arrPAllocTextures[i];

		if(pText)
		{
			delete pText;
		}
	}

	m_arrPAllocTextures.RemoveAll();

	COM_RELEASE(m_pOffSurface);
	COM_RELEASE(m_pBackSurface);

	COM_RELEASE(m_pD3DDevice9);
	COM_RELEASE(m_pD3D9);

	if( m_hBmpPresent )
	{
		DeleteObject(m_hBmpPresent);
		m_hBmpPresent = NULL;
	}
	if( m_hDCPresent )
	{
		DeleteDC(m_hDCPresent);
		m_hDCPresent = NULL;
	}
}


extern BOOL GetMaxScreenSize(int& cx, int& cy);

BOOL CD3DWrapper::Create(HWND hWnd)
{
	m_hWnd = hWnd;
	
	CRect rcWnd;
	::GetClientRect(hWnd,&rcWnd);
	if( m_pD3D9==NULL )
	{
		m_szWnd = rcWnd.Size();
	}
	else
	{
		if( m_szWnd==rcWnd.Size() )
			return FALSE;

		m_szWnd = rcWnd.Size();
		Destory();
	}

	if( m_szWnd.cx<1 )m_szWnd.cx = 1;
	if( m_szWnd.cy<1 )m_szWnd.cy = 1;

	m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );//创建D3D对象
	
	//创建Device：
	m_pD3DDevice9 = NULL;

	int cx = m_szWnd.cx, cy = m_szWnd.cy;
	GetMaxScreenSize(cx,cy);

	ZeroMemory( &m_D3DPresentParams, sizeof(m_D3DPresentParams) );
	m_D3DPresentParams.BackBufferWidth = cx;
	m_D3DPresentParams.BackBufferHeight = cy;
	m_D3DPresentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_D3DPresentParams.BackBufferCount = 1;
	m_D3DPresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;  
	m_D3DPresentParams.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	m_D3DPresentParams.SwapEffect = D3DSWAPEFFECT_COPY;
	m_D3DPresentParams.hDeviceWindow = hWnd;
	m_D3DPresentParams.Windowed = TRUE;
	m_D3DPresentParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_D3DPresentParams.EnableAutoDepthStencil = TRUE;
	m_D3DPresentParams.AutoDepthStencilFormat = D3DFMT_D16;
	m_D3DPresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	HRESULT hr = m_pD3D9->CreateDevice(
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_DISABLE_DRIVER_MANAGEMENT|D3DCREATE_SOFTWARE_VERTEXPROCESSING|D3DCREATE_FPU_PRESERVE,
		&m_D3DPresentParams,
		&m_pD3DDevice9 );

	if( !m_pD3DDevice9 )
		return FALSE;

	m_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pD3DDevice9->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);

	m_pBackSurface = NULL;
	m_pD3DDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackSurface);
	if( !m_pBackSurface )
		return FALSE;

	m_pOffSurface = NULL;
    m_pD3DDevice9->CreateOffscreenPlainSurface(cx, cy, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pOffSurface, NULL);
	if( !m_pOffSurface )
		return FALSE;

	m_pD3DDevice9->CreateVertexBuffer( sizeof(D3DVertex), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT, &m_pPointList_VB, NULL );
	if( !m_pPointList_VB )
		return FALSE;

	D3DXCreateLine(m_pD3DDevice9,&m_pLine);
	if( !m_pLine )
		return FALSE;

	return TRUE;
}


BOOL CD3DWrapper::CreateRenderTexture(int cx, int cy)
{
	if( !m_pD3DDevice9 )
		return FALSE;

	COM_RELEASE(m_pRenderTexture);

	m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,&m_pRenderTexture,NULL);

	m_szRenderTexture = CSize(cx,cy);

	return (m_pRenderTexture!=NULL);
}


int CD3DWrapper::AllocTexture(int cx, int cy, int nType)
{
	CD3DTexture *pTexture = new CD3DTexture();
	if( pTexture )
	{
		pTexture->Create(this,cx,cy,nType);
		for(int i=0; i<m_arrPAllocTextures.GetSize(); i++)
		{
			if(m_arrPAllocTextures[i]==NULL)
			{
				m_arrPAllocTextures.SetAt(i,pTexture);
				return i;
			}
		}
		return m_arrPAllocTextures.Add(pTexture);
	}

	return -1;
}

CD3DTexture *CD3DWrapper::GetAllocTexture(int index)
{
	if( index>=0 && index<m_arrPAllocTextures.GetSize() )
	{
		return m_arrPAllocTextures[index];
	}

	return NULL;
}


void CD3DWrapper::ReleaseAllocTexture(int index)
{
	if( index>=0 && index<m_arrPAllocTextures.GetSize() )
	{
		CD3DTexture *pText = m_arrPAllocTextures[index];
		delete pText;

		m_arrPAllocTextures.SetAt(index,NULL);
	}
}


BOOL CD3DWrapper::CreateDisplayTexture(int cx, int cy)
{
	if( !m_pD3DDevice9 )
		return FALSE;

	if( m_pDisplayTexture && m_szDisplayTexture.cx==cx && m_szDisplayTexture.cy==cy )
		return TRUE;
	
	COM_RELEASE(m_pDisplayTexture);
	
	m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,&m_pDisplayTexture,NULL);
	
	m_szDisplayTexture = CSize(cx,cy);
	
	return (m_pDisplayTexture!=NULL);
}

BOOL CD3DWrapper::CreateTempTextureForCopy(int cx, int cy)
{
	if( m_szTempTexture.cx==cx && m_szTempTexture.cy==cy && m_pTempTexture )
		return TRUE;

	COM_RELEASE(m_pTempTexture);

	m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,&m_pTempTexture,NULL);

	if( m_pTempTexture )
	{
		m_szTempTexture.cx = cx;
		m_szTempTexture.cy = cy;
	}

	return (m_pTempTexture!=NULL);
}


void CD3DWrapper::SetSize(CSize *psz)
{
	CSize sz;
	if( psz!=NULL )
	{
		sz = *psz;
	}
	else
	{
		CRect rcClient;
		::GetClientRect(m_hWnd,&rcClient);

		sz = rcClient.Size();
	}

	m_szViewPort = sz;
	
    D3DXMATRIX matView, matWorld;    // the view transform matrix
	
    D3DXMatrixIdentity( &matView );
	
    m_pD3DDevice9->SetTransform(D3DTS_VIEW, &matView);    // set the view transform to matView
	
	D3DXMatrixIdentity( &matWorld );

	//Direct3D光栅化时，需要偏移0.5个像素
	D3DXMatrixTranslation(&matWorld,-0.5-sz.cx*0.5,0.5-sz.cy*0.5,0);	
	
	m_pD3DDevice9->SetTransform(D3DTS_WORLD, &matWorld);    // set the view transform to matView
	
	D3DXMATRIX mProjection;
    D3DXMatrixOrthoLH( &mProjection, sz.cx, sz.cy, 0.0, 1.0f );
	m_pD3DDevice9->SetTransform(D3DTS_PROJECTION,&mProjection);
	
    D3DVIEWPORT9 viewPort;
	
    viewPort.X      = 0;
    viewPort.Y      = 0;
    viewPort.Width  = sz.cx;
    viewPort.Height = sz.cy;
    viewPort.MinZ   = 0.0f;
    viewPort.MaxZ   = 1.0f;
	
    m_pD3DDevice9->SetViewport( &viewPort );
}

void CD3DWrapper::OnSize(CSize sz)
{
	if( !m_pD3DDevice9 )
		return;

	if( sz.cx<=0 || sz.cy<=0 )
		return;

	SetSize(&sz);
}

void CD3DWrapper::BeginDrawing()
{
	if( m_nBeginScene>0 )
	{
	}
	else
	{
		HRESULT hr = m_pD3DDevice9->BeginScene();
	}

	m_nBeginScene++;
}

void CD3DWrapper::SetRenderTexture(LPDIRECT3DTEXTURE9 pText, int cx, int cy)
{
	m_pRenderTexture = pText;
	m_szRenderTexture.cx = cx;
	m_szRenderTexture.cy = cy;
}


void CD3DWrapper::BeginRenderTexture()
{
	COM_RELEASE(m_pRenderTextureSurface);

	HRESULT hr = m_pRenderTexture->GetSurfaceLevel(0,&m_pRenderTextureSurface);
	if( m_pRenderTextureSurface==NULL )
		return;

	hr = m_pD3DDevice9->SetRenderTarget(0,m_pRenderTextureSurface);

	SetSize(&m_szRenderTexture);
}


void CD3DWrapper::EndRenderTexture()
{
	m_pD3DDevice9->SetRenderTarget(0,m_pBackSurface);

	SetSize(NULL);

	COM_RELEASE(m_pRenderTextureSurface);
}


BOOL CD3DWrapper::CreateTextureVB()
{
	if( m_pTexture_VB!=NULL )
		return TRUE;

	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	HRESULT hr = m_pD3DDevice9->CreateVertexBuffer( 4*sizeof(D3DTexVertex), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1,
		D3DPOOL_DEFAULT, &pVB, NULL );	

	if( pVB==NULL )
		return FALSE;

	m_pTexture_VB = pVB;

	return TRUE;
}


void CD3DWrapper::DisplayTexture()
{
	if( !CreateTextureVB() )
		return;

	LPDIRECT3DVERTEXBUFFER9 pVB = m_pTexture_VB;
	if( !pVB )
		return;

	CD3DWrapper::D3DTexVertex *pts = NULL;
	pVB->Lock(0, sizeof(CD3DWrapper::D3DTexVertex)*4, (void**)&pts, D3DLOCK_DISCARD);
	
	if( pts==NULL )
	{
		return;		
	}
	
	pts[0].x = 0; pts[0].y = 0; pts[0].z = 0; pts[0].u = 0; pts[0].v = 0;
	pts[1].x = m_szRenderTexture.cx; pts[1].y = 0; pts[1].z = 0; pts[1].u = 1; pts[1].v = 0;
	pts[2].x = 0; pts[2].y = m_szRenderTexture.cy; pts[2].z = 0; pts[2].u = 0; pts[2].v = 1;
	pts[3].x = m_szRenderTexture.cx; pts[3].y = m_szRenderTexture.cy; pts[3].z = 0; pts[3].u = 1; pts[3].v = 1;

	pts[0].color = 0xffffffff;
	pts[1].color = 0xffffffff;
	pts[2].color = 0xffffffff;
	pts[3].color = 0xffffffff;

	pVB->Unlock();

	m_pD3DDevice9->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_pD3DDevice9->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	m_pD3DDevice9->SetTexture(0,m_pRenderTexture);

	m_pD3DDevice9->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

	m_pD3DDevice9->SetStreamSource(0,pVB,0,sizeof(D3DTexVertex));
	m_pD3DDevice9->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	
    m_pD3DDevice9->SetTexture(0,NULL);

}


void CD3DWrapper::ClearBuffer(COLORREF color, BYTE alpha)
{
	DWORD cur_color = D3DCOLOR_RGBA(GetRValue(color),GetGValue(color),GetBValue(color),alpha);

	m_pD3DDevice9->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,cur_color,1,0);
}


void CD3DWrapper::Scroll(int dx, int dy)
{
}

void CD3DWrapper::BitBltFrom(HBITMAP hBmp, CRect rcBmp)
{
	BITMAP src;	
	if( ::GetObject( hBmp , sizeof(BITMAP), &src )==0 )
		return;
	
	BYTE* pSrc0 =(BYTE*)src.bmBits, *pLine, *p;
	if( pSrc0==NULL )return;
	
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));

	BYTE *pDest = (BYTE*)m_rcLocked.pBits, *pLine2, *p2;

	pLine = pSrc0 + (src.bmHeight-1)*linesize;
	pLine2 = pDest;

	for( int i=0; i<src.bmHeight; i++)
	{
		p = pLine;
		p2 = pLine2;
		for( int j=0; j<src.bmWidth; j++)
		{
			p2[0] = 127;
			p2[1] = 127;
			p2[2] = 127;
			p2[3] = 127;
			p2 += 4;
			p += 3;
		}
		pLine -= linesize;
		pLine2 += m_rcLocked.Pitch;
	}
}


void CD3DWrapper::BitBltTo(HBITMAP hBmp, CRect rcBmp)
{
	BITMAP src;	
	if( ::GetObject( hBmp , sizeof(BITMAP), &src )==0 )
		return;
	
	BYTE* pDest =(BYTE*)src.bmBits, *pLine, *p;
	if( pDest==NULL )return;
	
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));
	
	BYTE *pSrc0 = (BYTE*)m_rcLocked.pBits, *pLine2, *p2;
	
	pLine2 = pDest + (src.bmHeight-1-rcBmp.top)*linesize + rcBmp.left*3;
	pLine = pSrc0;

	int wid = rcBmp.Width(), hei = rcBmp.Height();
	
	for( int i=0; i<hei; i++)
	{
		p = pLine;
		p2 = pLine2;
		for( int j=0; j<wid; j++)
		{
			p2[0] = p[0]; //b
			p2[1] = p[1]; //g
			p2[2] = p[2]; //r , p[3] -> a
			p2 += 3;
			p += 4;
		}
		pLine2 -= linesize;
		pLine += m_rcLocked.Pitch;
	}
}


HBITMAP CD3DWrapper::CreatePresentBmp(int w, int h)
{
	if( w<=0 || h<=0 )
		return NULL;

	if( m_hBmpPresent )
	{
		BITMAP bmp;
		GetObject(m_hBmpPresent,sizeof(bmp),&bmp);
		
		if( w<=bmp.bmWidth && h<=bmp.bmHeight )
		{
			return m_hBmpPresent;
		}

		DeleteObject(m_hBmpPresent);
	}

	m_hBmpPresent = Create24BitDIB(w,h);

	return m_hBmpPresent;
}


HDC CD3DWrapper::CreatePresentDC()
{
	if( m_hDCPresent )
		return m_hDCPresent;

	m_hDCPresent = CreateCompatibleDC(NULL);
	return m_hDCPresent;
}

void CD3DWrapper::SetGDIPresent(BOOL bGDIPresent)
{
	m_bGDIPresent = bGDIPresent;
}

BOOL CD3DWrapper::IsGDIPresent()const
{
	return m_bGDIPresent;
}

void CD3DWrapper::EndDrawing(BOOL bPresent)
{
	m_nBeginScene--;

	if( m_nBeginScene==0 )
	{
		HRESULT hr = m_pD3DDevice9->EndScene();
	}

	if( bPresent && m_nBeginScene==0 )
	{
		CRect rcClient;
		::GetClientRect(m_hWnd,&rcClient);

		if( m_bGDIPresent )
		{			
			HBITMAP hBmp = CreatePresentBmp(rcClient.Width(),rcClient.Height());
			BeginLock(rcClient,TRUE);
			BitBltTo(hBmp,rcClient);
			EndLock();
			
			HDC hdc = GetDC(m_hWnd);
			HDC hdc0 = CreatePresentDC();
			HBITMAP hOld = (HBITMAP)SelectObject(hdc0,hBmp);
			BitBlt(hdc,rcClient.left,rcClient.top,rcClient.Width(),rcClient.Height(),hdc0,rcClient.left,rcClient.top,SRCCOPY);
			SelectObject(hdc0,hOld);
			ReleaseDC(m_hWnd,hdc);
		}
		else
		{
			m_pD3DDevice9->Present(&rcClient,&rcClient,m_hWnd,NULL);
		}
	}
}

void CD3DWrapper::ResetDevice(BOOL bForce)
{	
	if( !m_pD3DDevice9 )
		return;

	if( bForce || m_pD3DDevice9->TestCooperativeLevel()==D3DERR_DEVICENOTRESET )
	{
		BOOL bHaveRenderTexture = (m_pRenderTexture!=NULL);
		
		COM_RELEASE(m_pTriangleList_VB);
		COM_RELEASE(m_pLineStrip_VB);		
		COM_RELEASE(m_pPointList_VB);
		COM_RELEASE(m_pTexture_VB);

		CArray<CD3DTexture,CD3DTexture> arrSave;

		for( int i=0; i<m_arrPAllocTextures.GetSize(); i++)
		{
			CD3DTexture *pText = m_arrPAllocTextures[i];

			if(pText==NULL)
			{
				continue;
			}

			CD3DTexture t;
			t.m_nType = pText->m_nType;
			t.m_size = pText->m_size;
			arrSave.Add(t);

			if( pText->m_nType==CD3DTexture::typeRender || pText->m_nType==CD3DTexture::typeDynamicVideo )
			{
				delete pText;
			}
		}
		
		COM_RELEASE(m_pRenderTexture);
		COM_RELEASE(m_pTempTexture);
		COM_RELEASE(m_pDisplayTexture);
		
		COM_RELEASE(m_pOffSurface);
		COM_RELEASE(m_pBackSurface);		

		m_pLine->OnLostDevice();
		
		CRect rcClient;
		GetClientRect(m_D3DPresentParams.hDeviceWindow,&rcClient);

		m_pD3DDevice9->Reset(&m_D3DPresentParams);
		
		m_pLine->OnResetDevice();
		
		m_pD3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);
		m_pD3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pD3DDevice9->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
		
		m_pD3DDevice9->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackSurface);
		
		m_pD3DDevice9->CreateOffscreenPlainSurface(m_D3DPresentParams.BackBufferWidth, m_D3DPresentParams.BackBufferHeight, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &m_pOffSurface, NULL);
		
		m_pD3DDevice9->CreateVertexBuffer( sizeof(D3DVertex), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE,
			D3DPOOL_DEFAULT, &m_pPointList_VB, NULL );
		
		if( bHaveRenderTexture )
		{
			CreateRenderTexture(m_szRenderTexture.cx,m_szRenderTexture.cy);
		}
		
		m_pLine->SetAntialias(FALSE);

		int k = 0;
		for( i=0; i<m_arrPAllocTextures.GetSize(); i++)
		{
			if( m_arrPAllocTextures[i]==NULL )
				continue;
			
			CD3DTexture t = arrSave[k++];
			if( t.m_nType==CD3DTexture::typeRender || t.m_nType==CD3DTexture::typeDynamicVideo )
			{
				CD3DTexture *pNew = new CD3DTexture();
				if(!pNew->Create(this,t.m_size.cx,t.m_size.cy,t.m_nType))
				{
					int a=1;	
				}

				pNew->m_bfilled = FALSE;
				m_arrPAllocTextures[i] = pNew;
			}
		}

		SetSize(NULL);

		PostMessage(m_D3DPresentParams.hDeviceWindow,WM_PAINT,0,0);
	}
}


void CD3DWrapper::EndLock()
{
	if( m_pOffSurface==NULL )
		return;

	m_pOffSurface->UnlockRect();
	
	//m_pD3DDevice9->StretchRect(m_pOffSurface,NULL,m_pBackSurface,NULL,D3DTEXF_NONE);

	//m_pD3DDevice9->Present(NULL,NULL,m_hWnd,NULL);
	
}



void CD3DWrapper::BeginLock(CRect rect, BOOL bRead)
{
	if( m_pBackSurface==NULL || m_pOffSurface==NULL )
		return;
	
	m_pD3DDevice9->GetRenderTargetData(m_pBackSurface,m_pOffSurface);

	HRESULT hr = D3D_OK;
	if( bRead )
		hr = m_pOffSurface->LockRect(&m_rcLocked,&rect,D3DLOCK_READONLY);
	else
		hr = m_pOffSurface->LockRect(&m_rcLocked,&rect,D3DLOCK_DISCARD);
}

HRESULT CD3DWrapper::SetRenderState(D3DRENDERSTATETYPE state, int value)
{
	if( m_pD3DDevice9 )
	{
		return m_pD3DDevice9->SetRenderState(state,value);
	}

	return 0;
}



//////////////////////////////////////////////////////////////////////////
//static member used to keep track of newly allocated vertices
CPtrList CD3DPolygon::gm_VertexPtrList ; 
CArray<CD3DWrapper::D3DVertex,CD3DWrapper::D3DVertex> CD3DPolygon::m_arrPts ; 
COLORREF CD3DPolygon::m_color = 0; 
int CD3DPolygon::m_nTriType = 0;
CD3DWrapper *CD3DPolygon::m_pD3D = 0;
BOOL CD3DPolygon::m_bSmoothLine = FALSE;


CD3DPolygon::CD3DPolygon()
{
	m_pObj = NULL;
}


CD3DPolygon::~CD3DPolygon()
{
	if( m_pObj!=NULL )gluDeleteTess( m_pObj );
	POSITION pos = gm_VertexPtrList.GetHeadPosition();
	while( pos!=NULL )
	{
		GLdouble *p = (GLdouble*)gm_VertexPtrList.GetNext(pos);
		delete[] p;
	}
	gm_VertexPtrList.RemoveAll();
}


void CD3DPolygon::Init(CD3DWrapper *p)
{
	m_pD3D = p;
}

void CD3DPolygon::BeginPolygon(COLORREF color)
{
	if( m_pObj!=NULL )gluDeleteTess( m_pObj );
	m_pObj = gluNewTess();

	m_color = D3DCOLOR_RGBA(GetRValue(color),GetGValue(color),GetBValue(color),255-(color>>24));

	m_arrPts.RemoveAll();

	gluTessProperty(m_pObj, GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_ODD );

	gluTessCallback(m_pObj, GLU_TESS_BEGIN, (void (CALLBACK *) ())beginCallback);
	gluTessCallback(m_pObj, GLU_TESS_VERTEX, (void (CALLBACK *) ()) vertexCallback);
	gluTessCallback(m_pObj, GLU_TESS_END, (void (CALLBACK *) ())endCallback);
	gluTessCallback(m_pObj, GLU_TESS_ERROR,(void (CALLBACK *) ())errorCallback);
	gluTessCallback(m_pObj, GLU_TESS_COMBINE, (void (CALLBACK *) ())combineCallback);

	gluTessBeginPolygon(m_pObj,NULL);
}


void CD3DPolygon::BeginContour()
{
	if( m_pObj!=NULL )
		gluTessBeginContour(m_pObj);
}


void CD3DPolygon::Vertex(double x, double y, double z)
{
	GLdouble *vertex = new GLdouble[3];
	vertex[0] = x; vertex[1] = y; vertex[2] = z;
	gm_VertexPtrList.AddTail(vertex);

	gluTessVertex( m_pObj, vertex, vertex ) ;
}


void CD3DPolygon::EndPolygon()
{
	gluTessEndPolygon(m_pObj);

	gluTessCallback(m_pObj, GLU_TESS_BEGIN, NULL);
	gluTessCallback(m_pObj, GLU_TESS_VERTEX, NULL);
	gluTessCallback(m_pObj, GLU_TESS_END, NULL);
	gluTessCallback(m_pObj, GLU_TESS_ERROR,NULL);
	gluTessCallback(m_pObj, GLU_TESS_COMBINE, NULL);
}

void CD3DPolygon::EndContour()
{
	if( m_pObj!=NULL )
		gluTessEndContour(m_pObj);
}


void CALLBACK CD3DPolygon::beginCallback(int which)
{
	m_nTriType = which;
	m_arrPts.RemoveAll();
}

void CALLBACK CD3DPolygon::errorCallback(int errorCode)
{
	const GLubyte *estring;
	
	estring = gluErrorString(errorCode);
}

void CALLBACK CD3DPolygon::endCallback(void)
{
	DrawCells();
	m_arrPts.RemoveAll();
}


void CD3DPolygon::DrawCells()
{	
	int npt = m_arrPts.GetSize();
	if( npt<3 )
		return;
	
	D3DPRIMITIVETYPE type = D3DPT_TRIANGLEFAN;
	int obj_count = npt-2;
	
	switch( m_nTriType )
	{
	case GL_TRIANGLES:
		type = D3DPT_TRIANGLELIST;
		obj_count = npt/3;
		break;
	case GL_TRIANGLE_STRIP:
		type = D3DPT_TRIANGLESTRIP;
		obj_count = npt-2;
		break;
	case GL_TRIANGLE_FAN:
		type = D3DPT_TRIANGLEFAN;
		obj_count = npt-2;
		break;
	}
	
	LPDIRECT3DVERTEXBUFFER9 pVB = NULL;
	m_pD3D->m_pD3DDevice9->CreateVertexBuffer( npt*sizeof(CD3DWrapper::D3DVertex), D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_DEFAULT, &pVB, NULL );	

	if( pVB==NULL )
		return;
	
	CD3DWrapper::D3DVertex *pts = NULL;
	pVB->Lock(0, sizeof(CD3DWrapper::D3DVertex)*npt, (void**)&pts, D3DLOCK_DISCARD );
	
	if( pts==NULL )
	{
		pVB->Release();
		return;
	}

	memcpy(pts,m_arrPts.GetData(),sizeof(CD3DWrapper::D3DVertex)*npt);
	
	pVB->Unlock();	
	
	m_pD3D->m_pD3DDevice9->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	
	m_pD3D->m_pD3DDevice9->SetStreamSource(0,pVB,0,sizeof(CD3DWrapper::D3DVertex));
	m_pD3D->m_pD3DDevice9->DrawPrimitive(type,0,obj_count);	

	m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	if( m_bSmoothLine )
	{
		m_pD3D->m_pD3DDevice9->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,TRUE);
		m_pD3D->m_pD3DDevice9->DrawPrimitive(type,0,obj_count);	
		m_pD3D->m_pD3DDevice9->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,FALSE);
	}
	else
	{
		m_pD3D->m_pD3DDevice9->DrawPrimitive(type,0,obj_count);	
	}

	m_pD3D->m_pD3DDevice9->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
	
	pVB->Release();
}

void CALLBACK CD3DPolygon::vertexCallback(void *vertex)
{
	GLdouble *pointer;
	
	pointer = (GLdouble *) vertex;

	CD3DWrapper::D3DVertex pt;
	pt.x = pointer[0]; pt.y = pointer[1]; pt.z = pointer[2];
	pt.color = m_color;

	m_arrPts.Add(pt);
}

/*  combineCallback is used to create a new vertex when edges
*  intersect.  coordinate location is trivial to calculate,
*  but weight[4] may be used to average color, normal, or texture
*  coordinate data.  In this program, color is weighted.
*/
void CALLBACK CD3DPolygon::combineCallback(double coords[3], 
											double *vertex_data[4],
											double weight[4], double **dataOut )
{
	GLdouble *vertex = new GLdouble[6] ;
	gm_VertexPtrList.AddTail( vertex ) ; //keep track for later delete[] at bottom of CD3DPolygon::OnDraw() 
	
	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	vertex[3] = vertex[4] = vertex[5] = 0. ; //01/13/05 bugfix
	
	*dataOut = vertex;
	//   TRACE( "combine: returning (%f,%f,%f,%f,%f,%f)\n",vertex[0],vertex[1],vertex[2],vertex[3],vertex[4],vertex[5] ) ;
}



CD3DTexture::CD3DTexture()
{
	m_pD3D = NULL;
	m_pTexture = NULL;

	m_bfilled = FALSE;
	m_size.cx = m_size.cy = 0;

	m_nType = typeDynamicSysMem;
}


CD3DTexture::~CD3DTexture()
{
	COM_RELEASE(m_pTexture);
}



BOOL CD3DTexture::Create(CD3DWrapper *pD3D, int cx, int cy, int nType)
{
	if( !pD3D )return FALSE;

	m_pD3D = pD3D;

	HRESULT hr = 0;
	
	if( nType==typeRender )
	{
		hr = m_pD3D->m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,&m_pTexture,NULL);

		if(m_pTexture==NULL)
		{
			nType = typeDynamicSysMem;
			hr = m_pD3D->m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM,&m_pTexture,NULL);			
		}
	}
	else if( nType==typeDynamicVideo )
	{
		hr = m_pD3D->m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,&m_pTexture,NULL);

		if(m_pTexture==NULL)
		{
			nType = typeDynamicSysMem;
			hr = m_pD3D->m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,
				D3DPOOL_SYSTEMMEM,&m_pTexture,NULL);			
		}
	}
	else if( nType==typeDynamicSysMem )
		hr = m_pD3D->m_pD3DDevice9->CreateTexture(cx,cy,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,
			D3DPOOL_SYSTEMMEM,&m_pTexture,NULL);

	m_size.cx = cx;
	m_size.cy = cy;

	m_nType = nType;

	return (m_pTexture!=NULL);
}


void CD3DTexture::Delete()
{
	COM_RELEASE(m_pTexture);
}

BOOL CD3DTexture::SetBitmap(HBITMAP hBmp,int x,int y,
				  int xTar,int yTar,int nTarWidth,int nTarHeight, BOOL bSetAlpha)
{
	if( hBmp==NULL || m_pTexture==NULL ) return FALSE;
	if( nTarWidth<=0 || nTarHeight<=0 )return FALSE;
	
	CRect rectTar(xTar,yTar,xTar+nTarWidth,yTar+nTarHeight);
	CRect rectTex(0,0,m_size.cx,m_size.cx);
	CRect rectin;
	
	rectin = rectTar&rectTex;
	if( rectin!=rectTar )
	{
		x += (rectin.left-rectTar.left);
		y += (rectin.top-rectTar.top);
		xTar = rectin.left;
		yTar = rectin.top;
		nTarWidth = rectin.Width();
		nTarHeight = rectin.Height();
	}
	
	if( x<0 || y<0 || nTarWidth<=0 || nTarHeight<=0 )
	{
		return FALSE;
	}
	
	BITMAP src;	
	::GetObject( hBmp , sizeof(BITMAP), &src );	
	BYTE* pSrc0 =(BYTE*)src.bmBits;
	if( pSrc0==NULL )return FALSE;
	
	int bitcount = src.bmBitsPixel/8;
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));

	D3DLOCKED_RECT d3dLocked;
	HRESULT hr = m_pTexture->LockRect(0,&d3dLocked,&CRect(0,0,m_size.cx,m_size.cy),0);

	if( hr!=D3D_OK )
		return FALSE;
	
	BYTE *pDest = (BYTE*)d3dLocked.pBits, *pLine, *pLine2, *p, *p2;

	pLine = pSrc0 + (src.bmHeight-1-y)*linesize + x*bitcount;
	pLine2 = pDest + yTar*d3dLocked.Pitch + xTar*4;
	
	for( int i=0; i<nTarHeight; i++)
	{
		p = pLine;
		p2 = pLine2;
		for( int j=0; j<nTarWidth; j++)
		{
			p2[0] = p[0];
			p2[1] = p[1];
			p2[2] = p[2];

			if( bSetAlpha && bitcount==4 )
				p2[3] = p[3];
			else
				p2[3] = 255;

			p2 += 4;
			p += bitcount;
		}
		pLine -= linesize;
		pLine2 += d3dLocked.Pitch;
	}
	
	m_pTexture->UnlockRect(0);

	return TRUE;
}


BOOL CD3DTexture::SetBitmap(HBITMAP hBmp,BOOL bSetAlpha)
{
	if( hBmp==NULL || m_pTexture==NULL  ) return FALSE;
	
	CRect rectTex(0,0,m_size.cx,m_size.cx);
		
	BITMAP src;	
	::GetObject( hBmp , sizeof(BITMAP), &src );	
	BYTE* pSrc0 =(BYTE*)src.bmBits;
	if( pSrc0==NULL )return FALSE;
	
	int bitcount = src.bmBitsPixel/8;
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));
	
	D3DLOCKED_RECT d3dLocked;
	HRESULT hr = m_pTexture->LockRect(0,&d3dLocked,&CRect(0,0,m_size.cx,m_size.cy),0);
	
	if( hr!=D3D_OK )
		return FALSE;

	BYTE *pDest = (BYTE*)d3dLocked.pBits, *pLine, *pLine2, *p, *p2;
	
	pLine = pSrc0 + (src.bmHeight-1)*linesize;
	pLine2 = pDest;
	
	for( int i=0; i<m_size.cx; i++)
	{
		p = pLine;
		p2 = pLine2;
		for( int j=0; j<m_size.cy; j++)
		{
			p2[0] = p[0];
			p2[1] = p[1];
			p2[2] = p[2];
			
			if( bSetAlpha && bitcount==4 )
				p2[3] = p[3];
			else
				p2[3] = 255;
			
			p2 += 4;
			p += bitcount;
		}
		pLine -= linesize;
		pLine2 += d3dLocked.Pitch;
	}
	
	m_pTexture->UnlockRect(0);
	
	return TRUE;
}


HBITMAP CD3DTexture::GetBitmap()
{	
	if( m_pTexture==NULL )
		return NULL;

	HBITMAP hBmp = Create24BitDIB(m_size.cx,m_size.cy);
	
	if( hBmp==NULL ) return NULL;
	
	BITMAP src;	
	if( ::GetObject( hBmp , sizeof(BITMAP), &src )==0 )
		return NULL;
	
	BYTE* pSrc0 =(BYTE*)src.bmBits, *pSrc;
	if( pSrc0==NULL )return NULL;
	
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));
	pSrc = pSrc0;

	D3DLOCKED_RECT d3dLocked;
	HRESULT hr = m_pTexture->LockRect(0,&d3dLocked,&CRect(0,0,m_size.cx,m_size.cy),0);

	if( hr!=D3D_OK )
		return FALSE;

	BYTE *pDest = (BYTE*)d3dLocked.pBits, *pLine, *pLine2, *p, *p2;

	if( pDest==NULL )
	{
		::DeleteObject(hBmp);
		return NULL;
	}
	
	pLine = pSrc0 + (src.bmHeight-1)*linesize;
	pLine2 = pDest;

	for( int i=0; i<m_size.cy; i++)
	{
		p = pLine;
		p2 = pLine2;
		for( int j=0; j<m_size.cx; j++)
		{
			p[0] = p2[0];
			p[1] = p2[1];
			p[2] = p2[2];			
			
			p2 += 4;
			p += 3;
		}
		pLine -= linesize;
		pLine2 += d3dLocked.Pitch;
	}
	
	m_pTexture->UnlockRect(0);

	return hBmp;
}


BOOL CD3DTexture::CopyToBitmap(HBITMAP hBmp)
{
	if( m_pTexture==NULL )
		return FALSE;

	if( hBmp==NULL ) return FALSE;
	
	BITMAP src;	
	if( ::GetObject( hBmp , sizeof(BITMAP), &src )==0 )
		return FALSE;

	if( src.bmBitsPixel<24 )
		return FALSE;
	
	BYTE* pSrc0 =(BYTE*)src.bmBits, *pSrc;
	if( pSrc0==NULL )return NULL;

	int pixelSize = (src.bmBitsPixel>>3);
	int linesize = (src.bmWidth*src.bmBitsPixel+7)/8;
	linesize =  ((linesize+3)&(~3));
	pSrc = pSrc0;
	
	D3DLOCKED_RECT d3dLocked;
	HRESULT hr = m_pTexture->LockRect(0,&d3dLocked,&CRect(0,0,m_size.cx,m_size.cy),0);

	if( hr!=D3D_OK )
		return FALSE;
	
	BYTE *pDest = (BYTE*)d3dLocked.pBits, *pLine, *pLine2, *p, *p2;
	
	pLine = pSrc0 + (src.bmHeight-1)*linesize;
	pLine2 = pDest;
	
	for( int i=0; i<m_size.cy; i++)
	{
		p = pLine;
		p2 = pLine2;
		for( int j=0; j<m_size.cx; j++)
		{
			p[0] = p2[0];
			p[1] = p2[1];
			p[2] = p2[2];
			
			if( pixelSize==4 )
				p[3] = p2[3];
			
			p2 += 4;
			p += pixelSize;
		}
		pLine -= linesize;
		pLine2 += d3dLocked.Pitch;
	}
	
	m_pTexture->UnlockRect(0);
	
	return TRUE;
}


HBITMAP CD3DTexture::GetBitmapForRenderTexture()
{
	if( m_pTexture==NULL )
		return NULL;

	if( !m_pD3D->CreateTempTextureForCopy(m_size.cx,m_size.cy) )
		return NULL;
	
	LPDIRECT3DSURFACE9 pSurface1=NULL;
	LPDIRECT3DSURFACE9 pSurface2=NULL;
	
	m_pTexture->GetSurfaceLevel(0,&pSurface1);		
	
	m_pD3D->m_pTempTexture->GetSurfaceLevel(0,&pSurface2);
	
	HRESULT hr = m_pD3D->m_pD3DDevice9->GetRenderTargetData(pSurface1,pSurface2);
	
	COM_RELEASE(pSurface1);
	COM_RELEASE(pSurface2);

	LPDIRECT3DTEXTURE9 pSave = m_pTexture;

	m_pTexture = m_pD3D->m_pTempTexture;
	HBITMAP hBmp = GetBitmap();

	m_pTexture = pSave;

	return hBmp;
}



void CD3DTexture::Display(double x[4], double y[4])
{
	if( m_pTexture==NULL )
		return;

	if( !m_pD3D->CreateTextureVB() )
		return;

	LPDIRECT3DVERTEXBUFFER9 pVB = m_pD3D->m_pTexture_VB;

	if( pVB==NULL )
		return;
	
	CD3DWrapper::D3DTexVertex *pts = NULL;
	pVB->Lock(0, sizeof(CD3DWrapper::D3DTexVertex)*4, (void**)&pts, D3DLOCK_DISCARD);

	if( pts==NULL )
	{
		return;		
	}
	
	pts[0].x = x[0]; pts[0].y = y[0]; pts[0].z = 0; pts[0].u = 0; pts[0].v = 0; pts[0].color = 0xffffffff;
	pts[1].x = x[1]; pts[1].y = y[1]; pts[1].z = 0; pts[1].u = 1; pts[1].v = 0; pts[1].color = 0xffffffff;
	pts[2].x = x[3]; pts[2].y = y[3]; pts[2].z = 0; pts[2].u = 0; pts[2].v = 1; pts[2].color = 0xffffffff;
	pts[3].x = x[2]; pts[3].y = y[2]; pts[3].z = 0; pts[3].u = 1; pts[3].v = 1; pts[3].color = 0xffffffff;
	
	pVB->Unlock();

	if( !m_pD3D->CreateDisplayTexture(m_size.cx,m_size.cy) )
	{
		return;
	}

	if( m_nType==typeDynamicSysMem )
	{
		LPDIRECT3DSURFACE9 pSurface1=NULL;
		LPDIRECT3DSURFACE9 pSurface2=NULL;
		
		m_pTexture->GetSurfaceLevel(0,&pSurface1);
		
		m_pD3D->m_pDisplayTexture->GetSurfaceLevel(0,&pSurface2);

		m_pD3D->m_pD3DDevice9->UpdateSurface(pSurface1,NULL,pSurface2,NULL);

		m_pD3D->m_pD3DDevice9->SetTexture(0,m_pD3D->m_pDisplayTexture);

		COM_RELEASE(pSurface2);
		COM_RELEASE(pSurface1);
	}
	else
	{
		m_pD3D->m_pD3DDevice9->SetTexture(0,m_pTexture);
	}

	m_pD3D->m_pD3DDevice9->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	
	m_pD3D->m_pD3DDevice9->SetStreamSource(0,pVB,0,sizeof(CD3DWrapper::D3DTexVertex));
	HRESULT hr = m_pD3D->m_pD3DDevice9->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	
    m_pD3D->m_pD3DDevice9->SetTexture(0,NULL);
	
}

void CD3DTexture::Display(double x0[4], double y0[4], double x[4], double y[4])
{
	if( m_pTexture==NULL )
		return;

	if( !m_pD3D->CreateTextureVB() )
		return;
	
	LPDIRECT3DVERTEXBUFFER9 pVB = m_pD3D->m_pTexture_VB;
	
	if( pVB==NULL )
		return;
	
	CD3DWrapper::D3DTexVertex *pts = NULL;
	pVB->Lock(0, sizeof(CD3DWrapper::D3DTexVertex)*4, (void**)&pts, D3DLOCK_DISCARD);

	if( pts==NULL )
	{
		return;
	}
	
	pts[0].x = x[0]; pts[0].y = y[0]; pts[0].z = 0; pts[0].u = x0[0]; pts[0].v = y0[0]; pts[0].color = 0xffffffff;
	pts[1].x = x[1]; pts[1].y = y[1]; pts[1].z = 0; pts[1].u = x0[1]; pts[1].v = y0[1]; pts[1].color = 0xffffffff;
	pts[2].x = x[3]; pts[2].y = y[3]; pts[2].z = 0; pts[2].u = x0[2]; pts[2].v = y0[2]; pts[2].color = 0xffffffff;
	pts[3].x = x[2]; pts[3].y = y[2]; pts[3].z = 0; pts[3].u = x0[3]; pts[3].v = y0[3]; pts[3].color = 0xffffffff;
	
	pVB->Unlock();
	
	if( !m_pD3D->CreateDisplayTexture(m_size.cx,m_size.cy) )
	{
		return;
	}
	
	if( m_nType==typeDynamicSysMem )
	{
		LPDIRECT3DSURFACE9 pSurface1=NULL;
		LPDIRECT3DSURFACE9 pSurface2=NULL;
		
		m_pTexture->GetSurfaceLevel(0,&pSurface1);
		
		m_pD3D->m_pDisplayTexture->GetSurfaceLevel(0,&pSurface2);
		
		m_pD3D->m_pD3DDevice9->UpdateSurface(pSurface1,NULL,pSurface2,NULL);

		COM_RELEASE(pSurface2);
		COM_RELEASE(pSurface1);
	}
	
	m_pD3D->m_pD3DDevice9->SetTexture(0,m_pD3D->m_pDisplayTexture);
	
	m_pD3D->m_pD3DDevice9->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
	
	m_pD3D->m_pD3DDevice9->SetStreamSource(0,pVB,0,sizeof(CD3DWrapper::D3DTexVertex));
	HRESULT hr = m_pD3D->m_pD3DDevice9->DrawPrimitive(D3DPT_TRIANGLESTRIP,0,2);
	
    m_pD3D->m_pD3DDevice9->SetTexture(0,NULL);
}


void CD3DTexture::Cutto(CD3DTexture *p)
{
	p->m_pD3D = m_pD3D;
	p->m_pTexture = m_pTexture;
	p->m_bfilled = m_bfilled;
	p->m_size = m_size;
	
	m_pD3D = NULL;
	m_pTexture = NULL;
	m_bfilled = FALSE;
	m_size = CSize(0,0);
}


void CD3DTexture::CopyFromRenderTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	if( m_pTexture==NULL )
		return;

	if( m_nType==typeDynamicSysMem )
	{
		LPDIRECT3DSURFACE9 pSurface1=NULL;
		LPDIRECT3DSURFACE9 pSurface2=NULL;

		pTexture->GetSurfaceLevel(0,&pSurface1);
		
		m_pTexture->GetSurfaceLevel(0,&pSurface2);
		
		HRESULT hr = m_pD3D->m_pD3DDevice9->GetRenderTargetData(pSurface1,pSurface2);
		
		COM_RELEASE(pSurface1);
		COM_RELEASE(pSurface2);	

		return;
	}
	else if( m_nType==typeDynamicVideo )
	{
		if( !m_pD3D->CreateTempTextureForCopy(m_size.cx,m_size.cy) )
			return;
		
		LPDIRECT3DSURFACE9 pSurface1=NULL;
		LPDIRECT3DSURFACE9 pSurface2=NULL;
		
		pTexture->GetSurfaceLevel(0,&pSurface1);		
		
		m_pD3D->m_pTempTexture->GetSurfaceLevel(0,&pSurface2);
		
		HRESULT hr = m_pD3D->m_pD3DDevice9->GetRenderTargetData(pSurface1,pSurface2);
		
		COM_RELEASE(pSurface1);
		COM_RELEASE(pSurface2);
		
		m_pTexture->AddDirtyRect(NULL);
		m_pD3D->m_pD3DDevice9->UpdateTexture(m_pD3D->m_pTempTexture,m_pTexture);
	}
}


MyNameSpaceEnd
