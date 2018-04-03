#include "stdafx.h"
#include "VirtualWindow.h"


CVirtualWindow::CVirtualWindow(int px, int py, int w, int h, int type /*= 0*/) :
_ipx(px),
_ipy(py),
_iw(w),
_ih(h),
_itype(type),
mouseIn(false),
m_manger(nullptr)
{

}

CVirtualWindow::~CVirtualWindow()
{
	for (CVirtualWindow * p : m_children)
	{
		delete p;
	}
	m_children.clear();
}

void CVirtualWindow::Paint(CHwndRenderTarget* pRenderTarget)
{
	// Ö»»­´°¿Ú·¶Î§ÄÚµÄÄÚÈİ
	CD2DLayer layer(pRenderTarget, false);
	pRenderTarget->PushLayer(
		D2D1::LayerParameters(m_rect),
		layer
		);
	// ÏÈ»­µ×²ã
	for (auto i = m_children.rbegin(); i != m_children.rend(); i ++)
	{
		(*i)->Paint(pRenderTarget);
	}
	pRenderTarget->PopLayer();
}

void CVirtualWindow::ResetWindow(int px, int py, int w, int h, int type)
{
	_ipx = px;
	_ipy = py;
	_iw = w;
	_ih = h;
	_itype = type;
}

CD2DRectF CVirtualWindow::GetRectByParentRect(const CD2DRectF & prect)
{
	m_prect = prect;
	float rcw = prect.right - prect.left;
	float rch = prect.bottom - prect.top;
	float wid = (_itype & WABS) ? _iw : (rcw * _iw / 100.f);
	float hei = (_itype & HABS) ? _ih : (rch * _ih / 100.f);
	float tmx = (_itype & XABS) ? _ipx : (rcw * _ipx / 100.f);
	float tmy = (_itype & YABS) ? _ipy : (rch * _ipy / 100.f);
	float left = 0;
	float right = 0;
	if (tmx >= 0)
	{
		left = prect.left + tmx;
		right = left + wid;
	}
	else
	{
		right = prect.right + tmx;
		left = right - wid;
	}
	float top = 0;
	float botton = 0;
	if (tmy >= 0)
	{
		top = prect.top + tmy;
		botton = top + hei;
	}
	else
	{
		botton = prect.bottom + tmy;
		top = botton - hei;
	}
	return CD2DRectF(left, top, right, botton);
}

CD2DPointF CVirtualWindow::ThisToTopWindow(const CD2DPointF & ipt, int type)
{

	float tmx = (type & XABS) ? ipt.x : ((m_rect.right - m_rect.left) * ipt.x);
	float tmy = (type & YABS) ? ipt.y : ((m_rect.bottom - m_rect.top) * ipt.y);
	return CD2DPointF(
		(tmx >= 0 ? m_rect.left : m_rect.right) + tmx,
		(tmy >= 0 ? m_rect.top : m_rect.bottom) + tmy);

}

void CVirtualWindow::ResizeByParentRect(const CD2DRectF & prect)
{
	m_rect = GetRectByParentRect(prect);
	for (CVirtualWindow * p : m_children)
	{
		p->ResizeByParentRect(m_rect);
	}
}

bool CVirtualWindow::MouseMove(const CPoint & pt)
{
	bool change = false;
	for (CVirtualWindow * p : m_children)
	{
		change |= p->MouseMove(pt);
		
	}
	bool srcB = mouseIn;
	mouseIn = m_rect.operator CRect().PtInRect(pt);
	return change || (srcB != mouseIn);
}

bool CVirtualWindow::Click(const CPoint & pt)
{
	if (!mouseIn) return false;
	for (CVirtualWindow * p : m_children)
	{
		if (p->Click(pt)) return true;
	}
	return true;
}

void CVirtualWindow::SetManager(CVirtualWindowResourceManager * manger)
{
	m_manger = manger;
	for (CVirtualWindow * p : m_children)
	{
		p->SetManager(manger);
	}

}
