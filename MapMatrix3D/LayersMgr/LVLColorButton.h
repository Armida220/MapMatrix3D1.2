// LVLColorButton.h: interface for the CLVLColorButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LVLCOLORBUTTON_H__194459E5_7E11_4817_9B08_51DEFB013A21__INCLUDED_)
#define AFX_LVLCOLORBUTTON_H__194459E5_7E11_4817_9B08_51DEFB013A21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CLVLColorButton : public CMFCColorButton
{
	DECLARE_DYNAMIC(CLVLColorButton)

public:
	CLVLColorButton();
	virtual ~CLVLColorButton();

	void ShowColorPopup();

	void CloseColorPop();

};

#endif // !defined(AFX_LVLCOLORBUTTON_H__194459E5_7E11_4817_9B08_51DEFB013A21__INCLUDED_)
