// LVLColorButton.cpp: implementation of the CLVLColorButton0 class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LVLColorButton0.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CLVLColorButton0, CMFCColorButton)

CLVLColorButton0::CLVLColorButton0()
{

}

CLVLColorButton0::~CLVLColorButton0()
{
	CloseColorPop();
}

void CLVLColorButton0::ShowColorPopup()
{
	OnShowColorPopup();
}

void CLVLColorButton0::CloseColorPop()
{
	if (m_pPopup != NULL)
	{
		m_pPopup->SendMessage (WM_CLOSE);
		m_pPopup = NULL;
		return;
	}

}
