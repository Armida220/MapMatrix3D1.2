// UserCustomToolbar.cpp: implementation of the CUserCustomToolbar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "UserCustomToolbar.h"
#include "PlugBase.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TOOLBARID_USERCUSTOM     60000
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT gToolBarId = TOOLBARID_USERCUSTOM;


#define CTRLBTN_WID		18
#define CTRLBTN_HEI		18
#define CTRLOFF			30

#define SEPERATOR		"*"

#define IsEndChar(x)		((x)==_T('\0'))
#define IsSpaceChar(x)		((x)==_T(' ')||(x)==_T('\t')||(x)==_T('\r')||(x)==_T('\n'))

CString FormatKeyin(LPCTSTR keyin)
{
	CString str;
	LPCTSTR p = keyin;
	int space = 0;
	while( IsSpaceChar(*p) )p++;
	if( IsEndChar(*p) )return str;
	
	while( !IsEndChar(*p) )
	{
		if( space==0 )
		{
			if( !IsSpaceChar(*p) )
				str += *p;
			else
			{
				space = 1-space;
			}
		}
		else if( space==1 )
		{
			if( !IsSpaceChar(*p) )
			{
				str += ' ';
				str += *p;
				space = 1-space;
			}
		}
		p++;
	}
	
	return str;
}

CUserCustomToolbar::CUserCustomToolbar()
{
	m_hToolbarBmp = NULL;
	m_hAccel = NULL;
}

CUserCustomToolbar::~CUserCustomToolbar()
{
	if( m_hToolbarBmp )
		::DeleteObject(m_hToolbarBmp);
	if( m_hAccel )
		::DestroyAcceleratorTable(m_hAccel);
}


BEGIN_MESSAGE_MAP(CUserCustomToolbar, CMFCToolBar)
	//{{AFX_MSG_MAP(CUserToolbar)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CUserCustomToolbar::LoadFromPulgins(CWnd *pParentWnd, const CArray<PlugItem,PlugItem> &arrItem, CPtrArray &arr)
{
	CUserCustomToolbar *pTool = NULL;
	BtnParam item;
	CArray<BtnParam,BtnParam> btns;
	for (int i=0;i<arrItem.GetSize();i++)
	{
		if (!arrItem[i].bUsed) continue;
		if (arrItem[i].pObj->GetItemCount()<=0)
			continue;
		pTool = new CUserCustomToolbar;
		if(!pTool) continue;
		pTool->CreateToolbar(pParentWnd);
		pTool->SetWindowText(arrItem[i].pObj->GetToolBarTitle());
		arr.Add(pTool);
		btns.RemoveAll();
		
		pTool->LoadBitmap(arrItem[i].pObj->GetToolBarBmpResID(),arrItem[i].hIns);
		const UINT * Names = arrItem[i].pObj->GetItemNamesResIDs();
		const char ** ShortCuts = arrItem[i].pObj->GetItemShortCuts();
		const char ** CmdLines = arrItem[i].pObj->GetItemCmdLines();
		int nsz = arrItem[i].pObj->GetItemCount();
		for (int j=0;j<nsz;j++)
		{
			if (Names[j]==NULL_ID)
			{
				continue;
			}
			CString str;					
			::LoadString(arrItem[i].hIns,Names[j],str.GetBuffer(256),256);
			str.ReleaseBuffer();
			_tcscpy(item.name,str);
			item.id = arrItem[i].itemID[j];
			if (strlen(ShortCuts[j])>0)
			{
				strcpy(item.key, ShortCuts[j]);
			}
			if (strlen(CmdLines[j])>0)
			{
				strcpy(item.cmd, CmdLines[j]);
			}
			btns.Add(item);
		}
		pTool->CreateButtons(btns.GetSize(),btns.GetData());
	}
}

// void CUserCustomToolbar::LoadFromConfig(CWnd *pParentWnd, LPCTSTR cfgPath, CPtrArray &arr)
// {
// 	CString strCfgPath;
// 
// 	if( cfgPath==NULL )
// 	{
// 		GetModuleFileName(NULL,strCfgPath.GetBuffer(_MAX_PATH),_MAX_PATH);
// 		strCfgPath.ReleaseBuffer();
// 		int pos = strCfgPath.ReverseFind(_T('\\'));
// 		if( pos>=0 )
// 		{
// 			strCfgPath = strCfgPath.Left(pos);
// 			pos = strCfgPath.ReverseFind(_T('\\'));
// 			if( pos>=0 )
// 			{
// 				strCfgPath = strCfgPath.Left(pos+1) + _T("config\\MSUserToolbar.txt");
// 			}
// 		}
// 	}
// 	else
// 		strCfgPath = cfgPath;
// 
// 	CUserCustomToolbar *pTool = NULL;
// 	FILE *fp = fopen(strCfgPath,"r");
// 	if( !fp )return;
// 
// 	CString dir = strCfgPath;
// 	int pos = dir.ReverseFind(_T('\\'));
// 	if( pos>=0 )dir = dir.Left(pos+1);
// 
// 	char line[1024];
// 	CString strLine, strTest;
// 
// 	int step = 0;
// 
// 	BtnParam item;
// 	CArray<BtnParam,BtnParam> btns;
// 
// 	while( !feof(fp) )
// 	{
// 		memset(line,0,sizeof(line));
// 		fgets(line,sizeof(line),fp);
// 		strLine = FormatKeyin(line);
// 		if( strLine.GetLength()<=0 )continue;
// 
// 		strTest = strLine;
// 		strTest.MakeUpper();
// 
// 		if( strTest.Find("#",0)==0 )
// 		{
// 		}
// 		else if( strTest.Find("BEGIN",0)==0 )
// 		{
// 			step++;
// 			pTool = new CUserCustomToolbar;
// 			if( pTool )
// 			{
// 				pTool->CreateToolbar(pParentWnd);
// 				arr.Add(pTool);
// 			}
// 			btns.RemoveAll();
// 		}
// 		else if( strTest.Find("END",0)==0 )
// 		{
// 			if( pTool )
// 				pTool->CreateButtons(btns.GetSize(),btns.GetData());
// 			step = 0;
// 		}
// 		// 标题
// 		else if( step==1 )
// 		{
// 			if( pTool )
// 				pTool->SetWindowText(strLine);
// 			step++;
// 		}
// 		// 位图
// 		else if( step==2 )
// 		{
// 			if( pTool )
// 				pTool->LoadBitmap(dir+strLine);
// 			step++;
// 		}
// 		else if( step==3 )
// 		{
// 			memset(&item,0,sizeof(item));
// 
// 			int pos1=0, pos2=0;
// 			strTest = line;
// 
// 			//按钮名称
// 			pos1 = strTest.Find(_T('\"'),0);
// 			pos2 = strTest.Find(_T('\"'),pos1+1);
// 			if( pos2>pos1+1 && pos1>=0 )
// 			{
// 				strcpy(item.name, strTest.Mid(pos1+1,pos2-pos1-1));
// 			}
// 			if( pos2>0 )
// 				strTest = strTest.Right(strTest.GetLength()-pos2-1);
// 
// 			//快捷键
// 			pos1 = strTest.Find(_T('\"'),0);
// 			pos2 = strTest.Find(_T('\"'),pos1+1);
// 			if( pos2>pos1+1 && pos1>=0 )
// 			{
// 				strcpy(item.key, strTest.Mid(pos1+1,pos2-pos1-1));
// 			}
// 			if( pos2>0 )
// 				strTest = strTest.Right(strTest.GetLength()-pos2-1);
// 
// 			//命令行的内容
// 			pos1 = strTest.Find(_T('\"'),0);
// 			pos2 = strTest.Find(_T('\"'),pos1+1);
// 			if( pos2>pos1+1 && pos1>=0 )
// 			{
// 				strcpy(item.cmd, strTest.Mid(pos1+1,pos2-pos1-1));
// 			}
// 
// 			if( strcmp(item.name,SEPERATOR)!=0 )
// 				item.id = gToolBtnId++;
// 			btns.Add(item);
// 		}
// 	}
// }


CString CUserCustomToolbar::GetCmdString(int nId)
{
	for( int i=0; i<m_arrBtnParams.GetSize(); i++)
	{
		if( nId==m_arrBtnParams.GetAt(i).id )
		{
			return m_arrBtnParams.GetAt(i).cmd;
		}
	}
	return CString();
}


BOOL ParseKey(const char *text, BYTE& fVirt, WORD& key)
{
	CString str = text;
	str.MakeUpper();
	str.Remove(' ');

	fVirt = 0;
	key = 0;
	if( str.Find("ALT")>=0 )
	{
		fVirt |= FALT;
		str.Replace("ALT",NULL);
	}
	if( str.Find("CTRL")>=0 )
	{
		fVirt |= FCONTROL;
		str.Replace("CTRL",NULL);
	}
	if( str.Find("SHIFT")>=0 )
	{
		fVirt |= FSHIFT;
		str.Replace("SHIFT",NULL);
	}
	str.Remove('+');

	if( str.GetLength()>0 )
	{
		key = (VkKeyScan(str.GetAt(0))&0xff);
		fVirt |= FVIRTKEY;
		return TRUE;
	}
	return FALSE;
}


HACCEL CUserCustomToolbar::GetAccelerator()
{
	if( m_hAccel )
		return m_hAccel;

	BtnParam param;
	ACCEL item;
	CArray<ACCEL,ACCEL> arrAccel;
	for( int i=0; i<m_arrBtnParams.GetSize(); i++)
	{
		param = m_arrBtnParams.GetAt(i);
		if( param.id!=0 && strlen(param.key)>0 )
		{
			memset(&item,0,sizeof(item));
			item.cmd = param.id;
			if( ParseKey(param.key,item.fVirt,item.key) )
			{
				arrAccel.Add(item);
			}
		}
	}

	if( arrAccel.GetSize()>0 )
		m_hAccel = ::CreateAcceleratorTable(arrAccel.GetData(),arrAccel.GetSize());

	return m_hAccel;
}


BOOL CUserCustomToolbar::LoadState (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}

BOOL CUserCustomToolbar::SaveState (LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	return TRUE;
}


void CUserCustomToolbar::CreateToolbar(CWnd *pParentWnd)
{
	CRect rcBoders(1, 2, 1, 2);
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	CreateEx(pParentWnd, TBSTYLE_FLAT, dwStyle, rcBoders, gToolBarId++);
	//Create(pParentWnd, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_HIDE_INPLACE, gToolBarId++);
	//m_uiOriginalResID = gToolBarId-1;
}

int CUserCustomToolbar::GetButtonsCount()
{
	return m_arrBtnParams.GetSize();
}
UINT CUserCustomToolbar::GetButtonsID(int idx)
{
	return m_arrBtnParams[idx].id;
}

void CUserCustomToolbar::CreateButtons(int num, const CUserCustomToolbar::BtnParam *params)
{
	RemoveAllButtons();
	m_arrBtnParams.RemoveAll();

	for( int i=0, j=0; i<num; i++)
	{
		if( strcmp(params[i].name,SEPERATOR)==0 )
			InsertSeparator();
		else
		{
			CMFCToolBarButton btn(params[i].id, m_iImagesOffset + j,
				params[i].name);
			
			btn.m_bText = FALSE;
			btn.m_bImage = TRUE;
			InsertButton(btn);
			m_DefaultImages.SetAt(params[i].id,m_iImagesOffset+j);
			
			m_arrBtnParams.Add(params[i]);
			j++;
		}
	}

	if( m_hAccel )
		::DestroyAcceleratorTable(m_hAccel);
	m_hAccel = NULL;
}

BOOL CUserCustomToolbar::LoadBitmap(LPCTSTR bmpFile)
{
	m_iImagesOffset = m_Images.GetCount();
	m_Images.Load(bmpFile);
	
	return TRUE;
}

BOOL CUserCustomToolbar::LoadBitmap(UINT uiResID,HINSTANCE hinstRes)
{
	m_iImagesOffset = m_Images.GetCount();
	m_Images.Load(uiResID,hinstRes,TRUE);
	
	return TRUE;
}

BOOL CUserCustomToolbar::OnUserToolTip(CMFCToolBarButton* pButton, CString& strTTText) const
{
	for( int i=0; i<m_arrBtnParams.GetSize(); i++)
	{
		if( pButton->m_nID==m_arrBtnParams.GetAt(i).id )
		{
			strTTText = m_arrBtnParams.GetAt(i).name;
			return TRUE;
		}
	}
	return FALSE;
}