// LVLColorButton.cpp: implementation of the CLVLColorButton class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LVLColorButton.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CLVLColorButton, CMFCColorButton)

CLVLColorButton::CLVLColorButton()
{

}

CLVLColorButton::~CLVLColorButton()
{
	CloseColorPop();
}

void CLVLColorButton::ShowColorPopup()
{
	OnShowColorPopup();
}

void CLVLColorButton::CloseColorPop()
{
	if (m_pPopup != NULL)
	{
		m_pPopup->SendMessage (WM_CLOSE);
		m_pPopup = NULL;
		return;
	}

}
