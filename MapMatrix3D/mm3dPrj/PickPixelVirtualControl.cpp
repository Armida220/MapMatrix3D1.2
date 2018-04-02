#include "stdafx.h"
#include "PickPixelVirtualControl.h"
#include "VirtualWindowResourceManager.h"
#include <vector>
class CCheckBoxControlButton :public CVirtualWindow
{
public:
	CCheckBoxControlButton(int px, int py, int w, int h, int type) :
		CVirtualWindow(px, py, w, h, type) { }
	void Paint(CHwndRenderTarget* pRenderTarget)
	{
		pRenderTarget->FillRectangle(m_rect,
			m_manger->GetSolidBrush(CVirtualWindowResourceManager::CHECKBOX_BUTTON));
	}
};

// ¡Ì
CD2DPointF CheckLine[] = {
	CD2DPointF(0.19f,0.5f),
	CD2DPointF(0.24f,0.63f),
	CD2DPointF(0.32f,0.3f)
};

// ¡Á
CD2DPointF NoCheckLine[] = {
	CD2DPointF(0.69f, 0.4f),
	CD2DPointF(0.78f, 0.63f),
	CD2DPointF(0.78f, 0.4f),
	CD2DPointF(0.69f, 0.63f)
};

class CCheckBoxControl :public CVirtualWindow
{
	bool _icheck;
	CCheckBoxControlButton * m_button;
	CD2DRectF m_inerRect;
	CPickPixelVirtualControl * m_parent;
public:

	CCheckBoxControl(CPickPixelVirtualControl * parent, int px, int py, int w, int h, int type, bool check = true) :
		CVirtualWindow(px, py, w, h, type), m_parent(parent)
	{
		m_button = new CCheckBoxControlButton(0,0,0,0,0);
		SetCheck(check);
		m_children.push_back(m_button);
	}
	void Paint(CHwndRenderTarget* pRenderTarget)
	{
		pRenderTarget->FillRectangle(m_rect,
			m_manger->GetSolidBrush(_icheck ? 
			CVirtualWindowResourceManager::CHECKBOX_CHECK: 
			CVirtualWindowResourceManager::CHECKBOX_NOCHECK));
		CD2DBrush * pb = m_manger->GetSolidBrush(CVirtualWindowResourceManager::CHECKBOX_BUTTON);
		if (_icheck)
		{
			CD2DPointF pt0 = ThisToTopWindow(CheckLine[0], 0);
			CD2DPointF pt1 = ThisToTopWindow(CheckLine[1], 0);
			CD2DPointF pt2 = ThisToTopWindow(CheckLine[2], 0);
			pRenderTarget->DrawLine(pt0, pt1, pb, 2);
			pRenderTarget->DrawLine(pt1, pt2, pb, 2);
		}
		else
		{
			CD2DPointF pt0 = ThisToTopWindow(NoCheckLine[0], 0);
			CD2DPointF pt1 = ThisToTopWindow(NoCheckLine[1], 0);
			CD2DPointF pt2 = ThisToTopWindow(NoCheckLine[2], 0);
			CD2DPointF pt3 = ThisToTopWindow(NoCheckLine[3], 0);
			pRenderTarget->DrawLine(pt0, pt1, pb, 2);
			pRenderTarget->DrawLine(pt2, pt3, pb, 2);
		}
		CVirtualWindow::Paint(pRenderTarget);

		if (mouseIn)
			pRenderTarget->DrawRectangle(m_inerRect,
			m_manger->GetSolidBrush(CVirtualWindowResourceManager::CHECKBOX_MOUSEINFRAME), 4);
		pRenderTarget->DrawRectangle(m_rect,
			m_manger->GetSolidBrush(CVirtualWindowResourceManager::CHECKBOX_THINGFRAME));
	}
	bool Click(const CPoint & pt) override
	{
		if (mouseIn)
		{
			SetCheck(!_icheck);
			if (_icheck)
				m_parent->Notify(true);
			else
				m_parent->OnCheckToUncheckByClick();
			
		}
		return mouseIn;
	}
	bool IsCheck() { return _icheck; }
	void SetCheck(bool b) { 
		_icheck = b;
		m_button->ResetWindow(_icheck ? -6 : 6, 10, 37, 80, 0);
		m_button->ResizeByParentRect(m_rect);
	}
	CD2DRectF GetRectByParentRect(const CD2DRectF & prect)
	{
		CD2DRectF res = CVirtualWindow::GetRectByParentRect(prect);
		m_inerRect = res;
		m_inerRect.left += 2;
		m_inerRect.right -= 2;
		m_inerRect.top += 2;
		m_inerRect.bottom -= 2;
		return res;
	}

};



class CImageVirtualControl :public CVirtualWindow
{

	int m_loadingCount;
	int m_cIdx;
	std::vector<double> ImageScales;
	double scale;
	int m_cSIdx;
	const int default_dix = 3;
	CD2DBitmap * pBMP;
	double centerx;
	double centery;
	double windowCenterX;
	double windowCenterY;
	CPickPixelVirtualControl * m_parent;
	bool bLoadReady;
public:
	COri ori;
	bool isLoadReady() { return bLoadReady; }
	CImageVirtualControl(CPickPixelVirtualControl * parent,int px, int py, int w, int h, int type)
		: CVirtualWindow(px, py, w, h, type),
		m_parent(parent),
		m_loadingCount(0),
		bLoadReady(false),
		pBMP(nullptr)
	{
		ImageScales.resize(11);
		ImageScales[0] = 0.25;
		ImageScales[1] = 0.5;
		ImageScales[2] = 0.75;
		ImageScales[3] = 1;
		for (int i = 4; i < ImageScales.size(); i++)
		{
			ImageScales[i] = 1.5 * ImageScales[i - 1];
		}
		m_cSIdx = default_dix;
		scale = ImageScales[m_cSIdx];
	}
	void drawStringInCenter(CHwndRenderTarget* pRenderTarget, bool bStatic = false, CString str = _T("Loading..."))
	{
		
		float cx = (m_rect.left + m_rect.right) / 2;
		float cy = (m_rect.top + m_rect.bottom) / 2;
		float t = cy + 9;
		float b = cy - 9;
		float wh = str.StringLength(str) * 9;
		CD2DRectF rct(cx - wh, t, cx + wh, b);
		CD2DTextFormat * pFormat = m_manger->GetTextFormat();
		pFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		if (bStatic)
		{
			pRenderTarget->DrawText(str, rct,
				m_manger->GetSolidBrush(CVirtualWindowResourceManager::IMG_CROSS),
				pFormat);
		}
		else
		{
			float w = wh * 2;
			m_loadingCount++;
			int n = m_loadingCount / 3;
			n %= 16;

			pRenderTarget->DrawText(str, rct,
				m_manger->GetRadialGradientBrush(CD2DPointF((w)* n / 16.f + rct.left, cy)),
				pFormat);
		}
	}
	void drawStringInLT(CHwndRenderTarget* pRenderTarget, CString str)
	{
		float w = str.StringLength(str) * 14;
		CD2DRectF rct = m_rect;
		rct.left += 10;
		rct.right = w + m_rect.left;
		rct.top = rct.bottom - 18;

		CD2DTextFormat * pFormat = m_manger->GetTextFormat();
		pFormat->Get()->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pRenderTarget->DrawText(str, rct,
			m_manger->GetSolidBrush(CVirtualWindowResourceManager::IMG_TEXT),
			pFormat);

	}
	CD2DRectF GetRectByParentRect(const CD2DRectF & prect)
	{
		CD2DRectF res = CVirtualWindow::GetRectByParentRect(prect);
		windowCenterX = (res.right + res.left) / 2;
		windowCenterY = (res.bottom + res.top) / 2;
		return res;
	}

	double ImageToWidowX(double x)
	{
		return (x - centerx) / scale + windowCenterX;
	}
	double ImageToWidowY(double y)
	{
		return (y - centery) / scale + windowCenterY;
	}
	CD2DPointF ImageToWidow(const CD2DPointF & pt)
	{
		return CD2DPointF(ImageToWidowX(pt.x), ImageToWidowY(pt.y));
	}

	double WidowToImageX(double x)
	{
		return (x - windowCenterX) * scale + centerx;
	}
	double WidowToImageY(double y)
	{
		return (y - windowCenterY) * scale + centery;
	}
	CD2DPointF WidowToImage(const CD2DPointF & pt)
	{
		return CD2DPointF(WidowToImageX(pt.x), WidowToImageY(pt.y));
	}
	void drawCross(CHwndRenderTarget* pRenderTarget)
	{
		CD2DPointF pt = ImageToWidow(CD2DPointF(ori.imgX, ori.imgY));
		//if (!m_rect.operator CRect().PtInRect(pt)) return;
		CD2DBrush * pbrush = m_manger->GetSolidBrush(CVirtualWindowResourceManager::IMG_CROSS);
		CD2DPointF pts[4] = {
			CD2DPointF(pt.x, pt.y - 10),
			CD2DPointF(pt.x, pt.y + 10),
			CD2DPointF(pt.x - 10, pt.y),
			CD2DPointF(pt.x + 10, pt.y)
		};
		
		pRenderTarget->DrawLine(pts[0], pts[1], pbrush,1.5f);
		pRenderTarget->DrawLine(pts[2], pts[3], pbrush,1.5f);
	}
	void Paint(CHwndRenderTarget* pRenderTarget)
	{
		
		if (!pBMP)
		{
			pBMP = m_manger->getImageByIndex(m_cIdx, ori);
			centerx = ori.imgX;
			centery = ori.imgY;
		}
		if (pBMP)
		{
			bLoadReady = true;
			CD2DSizeF sz = pBMP->GetSize();
			double L,T,R,B;
			L = WidowToImageX(m_rect.left);
			R = WidowToImageX(m_rect.right);
			T = WidowToImageY(m_rect.top);
			B = WidowToImageY(m_rect.bottom);
			CD2DRectF dPic = m_rect;
			bool bFillAll = true;
			if (L < 0) { bFillAll = false; L = 0; dPic.left = ImageToWidowX(L); }
			if (T < 0) { bFillAll = false; T = 0; dPic.top = ImageToWidowY(T); }
			if (R > (sz.width - 1)) { bFillAll = false; R = sz.width - 1; dPic.right = ImageToWidowX(R); }
			if (B > (sz.height - 1)) { bFillAll = false; B = sz.height - 1; dPic.bottom =  ImageToWidowY(B); }
			CD2DRectF rc(L, T, R, B);
			if (!bFillAll) 
				pRenderTarget->FillRectangle(m_rect, m_manger->GetSolidBrush(CVirtualWindowResourceManager::IMG_BACKGROUND));
			pRenderTarget->DrawBitmap(pBMP, dPic, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, &rc);
			drawCross(pRenderTarget);
			CString str;
			str = ori.getFileName(ori.imgName).c_str();
			drawStringInLT( pRenderTarget,str);
		}
		else
		{
			pRenderTarget->FillRectangle(m_rect, m_manger->GetSolidBrush(CVirtualWindowResourceManager::IMG_BACKGROUND));
			CString str;
			str = ori.imgName.c_str();
			if (ori.imgName == "Null" || (ori.imgName[0] == 'C' && ori.imgName[1] == 'a'))
			{
				
				drawStringInCenter(pRenderTarget, true, str);
			}
			else
			{
				drawStringInCenter(pRenderTarget, false, str);
			}
		}
		
	}
	bool Click(const CPoint & pt)
	{
		if (mouseIn && !m_parent->m_cc->IsCheck())
		{
			CD2DPointF _pt = WidowToImage(pt);
			ori.imgX = _pt.x;
			ori.imgY = _pt.y;
			m_parent->Notify(false);

		}
		return mouseIn;
	}
	int Idx() const { return m_cIdx; }
	void SetIdx(int idx)
	{
		m_cSIdx = default_dix;
		pBMP = nullptr;
		bLoadReady = false;
		m_cIdx = idx;
	}
	void Zoom(bool in, const CPoint & pt)
	{
		int sIdx = m_cSIdx;
		if (in)
		{
			if (m_cSIdx>=1)
				--m_cSIdx;
		}
		else
		{
			if (m_cSIdx < ImageScales.size() - 1)
				m_cSIdx++;
		}
		if (sIdx != m_cSIdx)
		{
			CD2DPointF ptI = WidowToImage(pt);
			scale = ImageScales[m_cSIdx];
			CD2DPointF nPt = ImageToWidow(ptI);
			Move(nPt.x - pt.x, nPt.y - pt.y);
		}
	}
	void Move(double dx, double dy)
	{
		double scale = ImageScales[m_cSIdx];
		centerx += dx * scale;
		centery += dy * scale;
	}
};

CPickPixelVirtualControl::CPickPixelVirtualControl(int px, int py, int w, int h, int type)
	: CVirtualWindow(px, py, w, h, type), m_notifyFun(nullptr)
{
	m_cc = new CCheckBoxControl(this, -10, 10, 68, 30, XABS | YABS | WABS | HABS);
	m_ic = new CImageVirtualControl(this , 0, 0, 100, 100, 0);
	m_children.push_back(m_cc);
	m_children.push_back(m_ic);
}


CPickPixelVirtualControl::~CPickPixelVirtualControl()
{
}

void CPickPixelVirtualControl::Paint(CHwndRenderTarget* pRenderTarget)
{
	CVirtualWindow::Paint(pRenderTarget);
	

	//pRenderTarget->DrawRectangle(m_rect, m_manger->GetSolidBrush(CVirtualWindowResourceManager::IMG_CROSS));
}

void CPickPixelVirtualControl::SetNotifyFun(notifyFun fun)
{
	m_notifyFun = fun;
}


void CPickPixelVirtualControl::SetIdx(int idx)
{
	m_ic->SetIdx(idx);
}

int CPickPixelVirtualControl::GetIdx() const
{
	return m_ic->Idx();
}

void CPickPixelVirtualControl::Zoom(bool in, const CPoint & pt)
{
	if (mouseIn)
		m_ic->Zoom(in, pt);
}

void CPickPixelVirtualControl::Move(double dx, double dy)
{
	if (mouseIn)
		m_ic->Move(dx, dy);
}

void CPickPixelVirtualControl::OnCheckToUncheckByClick()
{
	if (!m_cbfun._Empty())
	{
		m_cbfun();
	}
}

void CPickPixelVirtualControl::SetState(int checkState)
{
	m_cc->SetCheck(checkState != 0);
}

bool CPickPixelVirtualControl::IsSetNotifyFun() const
{
	return !m_notifyFun._Empty();
}

void CPickPixelVirtualControl::Notify(bool b)
{
	if (!m_notifyFun._Empty())
	{
		m_notifyFun(b, m_ic->ori.imgName, m_ic->ori.imgX, m_ic->ori.imgY);
	}
}

void CPickPixelVirtualControl::setCheckToUncheckByClickFun(callbackFun fun)
{
	m_cbfun = fun;
}
