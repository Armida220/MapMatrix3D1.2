// CollectionTreeCtrl.cpp: implementation of the CCollectionTreeCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "editbase.h"
#include "CollectionTreeCtrl.h"
#include "Resource.h"
#include "ExMessage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



BEGIN_MESSAGE_MAP(CCollectionTreeCtrl, CTreeCtrlEx)
		//{{AFX_MSG_MAP(CCollectionTreeCtrl)		
	ON_WM_LBUTTONDOWN()
	ON_WM_ACTIVATE()
	ON_WM_RBUTTONDOWN()
	ON_WM_ACTIVATEAPP()
		//}}AFX_MSG_MAP
		ON_WM_CREATE()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCollectionTreeCtrl::CCollectionTreeCtrl()
{

}

CCollectionTreeCtrl::~CCollectionTreeCtrl()
{

}



//层类型"数字+空格+FID",如  "1110 hanbing"
BOOL CheckLayerFormat(CString strLayerName)
{
	//去掉两边空格
	strLayerName.TrimLeft();
	strLayerName.TrimRight();
	if( strLayerName.Find(' ') == -1 )
		return FALSE;
	else 
	{
		//如果有多个空格
		if(strLayerName.Find(' ') != strLayerName.ReverseFind(' ') )
			return FALSE;
		else
		{
			char num[256],fid[256];
			sscanf(strLayerName,"%s %s",num,fid);
			if( !IsDigital(num) )
				return FALSE;
			else
				return TRUE;	
		}
	}
	
}

BOOL FindNum(__int64 num, vector<__int64> nums)
{
	if( find(nums.begin(),nums.end(),num) != nums.end() )
		return TRUE;
	return FALSE;
	
}

//判断在一个层中是否重名
CString CheckName(const CString &strCheckedName,vector<CString> &names)
{
	CString strNameNew = strCheckedName;
	CString suffix;
	int number = 0;
	while(1)
	{	
		int size = names.size();
		for(int i=0; i<size; i++)
		{
			if(strNameNew.CompareNoCase(names[i])==0)
			{
				break;
			}
		}	
		
		if(i>=size)
		{
			return strNameNew;
		}
		else
		{
			suffix.Format("%d",number++);
			strNameNew = strCheckedName + suffix;
		}
	}
}

BOOL CCollectionTreeCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS|TVS_TRACKSELECT|TVS_EDITLABELS;
	return CTreeCtrlEx::PreCreateWindow(cs);
}

BOOL CCollectionTreeCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_F2)//hcw,2012.3.31,VK_F3→VF_F2.
			EditLabel(GetSelectedItem());
	}	
	return CTreeCtrlEx::PreTranslateMessage(pMsg);
}

void CCollectionTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call defaule
	
	CTreeCtrlEx::OnLButtonDown(nFlags, point);
	
	GetParent()->PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}


void CCollectionTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CTreeCtrlEx::OnRButtonDown(nFlags, point);
	  
	
}


int CCollectionTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrlEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	SetBkColor(RGB(50, 50, 50));
	SetTextColor(RGB(255, 255, 255));
	return 0;
}
