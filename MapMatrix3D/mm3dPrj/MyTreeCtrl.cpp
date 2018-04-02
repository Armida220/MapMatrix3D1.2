// G:\testMm3D\FeatureOne\FeatureCollector2009\mm3dPrj\MyTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "MyTreeCtrl.h"


// CMyTreeCtrl

IMPLEMENT_DYNAMIC(CMyTreeCtrl, CTreeCtrl)

CMyTreeCtrl::CMyTreeCtrl()
{

}

CMyTreeCtrl::~CMyTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CMyTreeCtrl message handlers




int CMyTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetBkColor(RGB(50, 50, 50));
	SetTextColor(RGB(255, 255, 255));

	return 0;
}
