// SinglePhotoStereoView.h: interface for the CSinglePhotoStereoView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINGLEPHOTOSTEREOVIEW_H__DF254D2D_3836_4A6F_BB61_1DB5444DB791__INCLUDED_)
#define AFX_SINGLEPHOTOSTEREOVIEW_H__DF254D2D_3836_4A6F_BB61_1DB5444DB791__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StereoView.h"


class CStereoParallaxFile
{
public:
	CStereoParallaxFile();
	~CStereoParallaxFile();

	BOOL Load(LPCTSTR leftimage, LPCTSTR rightimage);
	BOOL Save();

public:
	BOOL m_bLoadOK;
	CString m_strPath;
	PT_4D m_curPt;
};

class CSinglePhotoStereoView : public CStereoView  
{
	DECLARE_DYNCREATE(CSinglePhotoStereoView)
public:
	CSinglePhotoStereoView();
	virtual ~CSinglePhotoStereoView();

protected:

	virtual void OnInitialUpdate();
	virtual BOOL ConvertFailed(Coordinate Input, Coordinate &Output);

	//{{AFX_MSG(CSinglePhotoStereoView)
	afx_msg LRESULT OnLoadModel(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SINGLEPHOTOSTEREOVIEW_H__DF254D2D_3836_4A6F_BB61_1DB5444DB791__INCLUDED_)
