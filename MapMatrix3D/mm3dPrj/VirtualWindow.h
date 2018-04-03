#pragma once
#include <list>
class CVirtualWindowResourceManager;
class CVirtualWindow
{
 	int _ipx;
 	int _ipy;
 	int _iw;
 	int _ih;
 	int _itype;
 protected:
	 CD2DRectF m_prect;
	 CD2DRectF m_rect;
	bool mouseIn;
	std::list<CVirtualWindow *> m_children;
	CVirtualWindowResourceManager * m_manger;
 public:
 	enum
 	{
 		XABS    = 0x01,
 		YABS    = 0x02,
 		WABS    = 0x04,
 		HABS    = 0x08
 	};
	/*******************************************/
	/*  -------------------------------------  */
	/*  |         py   <- w  ->             |  */
	/*  |         --   --------             |  */
	/*  |              |   |  |             |  */
	/*  |              |   h  |             |  */
	/*  | < -  px  - > |   |  | < - -px - > |  */
	/*  |         ---  --------             |  */
	/*  |         -py                       |  */
	/*  -------------------------------------  */
	/*******************************************/
	// 默认为相对值,单位为0.01,为绝对时单位为像素
 	CVirtualWindow(int px, int py, int w, int h, int type = 0);
	~CVirtualWindow();
 	virtual void Paint(CHwndRenderTarget* pRenderTarget);
	virtual void ResetWindow(int px, int py, int w, int h, int type);
	virtual CD2DRectF GetRectByParentRect(const CD2DRectF & prect);
	virtual CD2DPointF ThisToTopWindow(const CD2DPointF & ipt, int type);
	virtual void ResizeByParentRect(const CD2DRectF & prect);
	virtual bool MouseMove(const CPoint & pt);
	virtual bool Click(const CPoint & pt);
	virtual void SetManager(CVirtualWindowResourceManager * manger);

};
