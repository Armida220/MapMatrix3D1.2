// SchemeCopy.cpp: implementation of the CSchemeCopy class.
//
//////////////////////////////////////////////////////////////////////
//hcw,2012.8.30, for Copying Property

//#include "featurecollector2009.h"
#include "stdafx.h"
#include "EditBase.h" //for StrFromResID
#include "SchemePropertyCopy.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//#define TIMERID_PREVIEW 2
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSchemePropertyCopy::CSchemePropertyCopy( CWnd* pParent /*= NULL*/,CConfigLibManager* pSrcCfgLibMan/*=NULL*/, CConfigLibManager* pDesCfgLibMan/*=NULL*/,int iDesScale/*=0*/,int iSrcScale/*=0*/ ,BOOL bIsDesTreeLoaded/*=FASE*/)
:CSchemeMerge(pParent ,pSrcCfgLibMan, pDesCfgLibMan, iDesScale, iSrcScale,bIsDesTreeLoaded)
{
	m_pWndEditCopyStatus = NULL;
	m_pWndEditSrcXAttStatus = NULL;
	m_pWndEditDesXAttStatus = NULL;
	m_bMatchedbyLayerName = FALSE;
	m_bHasXDefines = FALSE;
	m_CurLayerIndex = 0; //浏览标记重新初始化为0;
	m_strInvalidChars = "_、()（）";
	m_pwndButtonDel = NULL;//2012.9.20
}
CSchemePropertyCopy::~CSchemePropertyCopy()
{
	
}


BEGIN_MESSAGE_MAP(CSchemePropertyCopy,CSchemeMerge)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
 	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_CODE, CSchemePropertyCopy::OnBnClickedRadioByCode)
	ON_BN_CLICKED(IDC_RADIO_NAME, CSchemePropertyCopy::OnBnClickedRadioByName)
	ON_BN_CLICKED(IDC_BUTTON_COPY, CSchemePropertyCopy::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_SKIP, CSchemePropertyCopy::OnBnClickedButtonSkip)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SRCSCHEME2, OnSelchangedTreeSrc)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DESCHEME2, OnSelchangedTreeDes)
	ON_CBN_SELCHANGE(IDC_COMBO_SRCSCHEME2, OnSelChangeSrcScaleCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_DESSCHEME2, OnSelChangeDesScaleCombo)
	ON_BN_CLICKED(IDC_BUTTON_SRCPATH, OnButtonSrcpath)
	ON_BN_CLICKED(IDC_BUTTON_DESPATH, OnButtonDespath)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	//ON_BN_CLICKED()
END_MESSAGE_MAP()

BOOL CSchemePropertyCopy::OnInitDialog()
{
	CSchemeMerge::OnInitDialog();
	m_wstrInvalidChars = CstrtoWstr(m_strInvalidChars);
	//Modified the title of Dialog
	SetWindowText(StrFromResID(IDS_COPY_ATTRIBUTES));
	int x = 0;
	int y = 0;
	MoveCtrl(IDC_STATIC_DESSCALE,x,y);
	MoveCtrl(IDC_STATIC_SRCSCALE,x,y);
	
	MoveCtrl(IDC_STATIC_DES,x,y);
	MoveCtrl(IDC_STATIC_RES,x,y);
	
	//Move the Ctrl to Another Pos;
	MoveCtrl(IDC_TREE_DESCHEME2,x,y);
	MoveCtrl(IDC_TREE_SRCSCHEME2,x,y);
	MoveCtrl(IDC_COMBO_DESSCHEME2,x,y);
	MoveCtrl(IDC_COMBO_SRCSCHEME2,x,y);
	
	MoveCtrl(IDCANCEL,x,y);
	
	//hide unnecessary Ctrl
	this->GetDlgItem(IDC_STATIC_MERGEPATH)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_EDIT_MERGEPATH)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_BUTTON_DEL)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_BUTTON_MERGE)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_BUTTON_HANDMERGE)->ShowWindow(FALSE);

	//IDC_BUTTON_UP IDC_BUTTON_DOWN
	this->GetDlgItem(IDC_BUTTON_UP)->ShowWindow(FALSE);
	this->GetDlgItem(IDC_BUTTON_DOWN)->ShowWindow(FALSE);

	//Add necessary ctrl for copying property of scheme;
	CreateCtrlsforPropCpy();
	InitializeAppendedCtrls();
	
	//Initially Locate the node of TreeCtrl(SRC,DES)
	if(!m_SrcConfig.pScheme) return FALSE;
	int nSrcLayerCount = m_SrcConfig.pScheme->GetLayerDefineCount(FALSE);
	int nSrcSpecialLayerCount = m_SrcConfig.pScheme->GetLayerDefineCount(TRUE);
	if((!nSrcLayerCount)&&(!nSrcSpecialLayerCount))
	{
		return FALSE;
	}
	if (nSrcLayerCount>0)
	{
		m_bSpecialLayer = FALSE;
	}
	else if(nSrcSpecialLayerCount>0)
	{
		m_bSpecialLayer = TRUE;	 
	}
	CSchemeLayerDefine *pFirstSrcLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(0,m_bSpecialLayer);
	__int64 nFirstSrcLayerCode = -1;
	if (pFirstSrcLayerDefine)
	{
		nFirstSrcLayerCode = pFirstSrcLayerDefine->GetLayerCode();
	}
	
	
	if (nFirstSrcLayerCode>=0)
	{
		m_wndRadiobyCode.SetCheck(1);
		//{2012.9.20,cancel off
		if (m_bIsSrcTreeLoaded)
		{
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_BOLD|TVIS_SELECTED,nFirstSrcLayerCode,"",NULL);
			__int64 nFirstDesLayerCode = GetMatchedCode(nFirstSrcLayerCode,m_DesUserIdx); //m_DesUserIdx→m_DesConfig
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_BOLD|TVIS_SELECTED,nFirstDesLayerCode,"",NULL);
		}
		//}
	}

	
	//显示源方案中属性值的存在状态。Display the Value State of XDefines;
	if(m_pWndEditSrcXAttStatus=this->GetDlgItem(IDC_EDIT_SRCVALUESTATUS))
	{	
		if(m_bHasXDefines = HasXDefines(m_hSelectedTreeItemArray[SRC-1], m_SrcTree ,m_SrcConfig))
		{
			m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCHASVALUE);
		}
		else
		{
			m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCNOVALUE);
		}
		m_pWndEditSrcXAttStatus->SetWindowText(m_strSrcXAttStatusPrompt);
	}

	
	UpdateData(FALSE);
	return TRUE;
}

BOOL CSchemePropertyCopy::ExchangeCtrlPos( UINT nCtrlID1,UINT nCtrlID2 )
{
	UpdateData(TRUE);
	RECT rectCtrlID1;
	RECT rectCtrlID2;
	CWnd* pCtrlID1= this->GetDlgItem(nCtrlID1);
	CWnd* pCtrlID2 = this->GetDlgItem(nCtrlID2);
	if ((!pCtrlID1)&&(!pCtrlID2))
	{
		return FALSE;
	}
	pCtrlID1->GetWindowRect(&rectCtrlID1);
	pCtrlID2->GetWindowRect(&rectCtrlID2);
	ScreenToClient(&rectCtrlID1);
	ScreenToClient(&rectCtrlID2);
	BOOL flag1 = FALSE;
	BOOL flag2 = FALSE;
	flag1 = pCtrlID1->SetWindowPos(NULL,rectCtrlID2.left,rectCtrlID2.top,rectCtrlID2.right-rectCtrlID2.left,rectCtrlID2.bottom-rectCtrlID2.top,SWP_NOSIZE|SWP_NOZORDER);
	flag2 = pCtrlID2->SetWindowPos(NULL,rectCtrlID1.left,rectCtrlID1.top,rectCtrlID1.right-rectCtrlID1.left,rectCtrlID1.bottom-rectCtrlID1.top,SWP_NOSIZE|SWP_NOZORDER);
	//UpdateData(FALSE);
	if ((!flag1)||(!flag2))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CSchemePropertyCopy::MoveCtrl( UINT nCtrlID,int x, int y )
{

	RECT rectCtrlID;
	CWnd* pCtrlID = this->GetDlgItem(nCtrlID);
	if (!pCtrlID)
	{
		return FALSE;
	}
	pCtrlID->GetWindowRect(&rectCtrlID);
	ScreenToClient(&rectCtrlID);
	BOOL flag = FALSE;
	flag = pCtrlID->SetWindowPos(NULL,rectCtrlID.left+x,rectCtrlID.top+y,
					rectCtrlID.right-rectCtrlID.left+x,rectCtrlID.bottom-rectCtrlID.top+y,SWP_NOSIZE|SWP_NOZORDER);
	if (!flag)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CSchemePropertyCopy::CreateCtrlsforPropCpy()
{
	CRect rt;
	rt.SetRectEmpty();
	const DWORD dwPushBtnStyle = WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON;
	if(!m_wndButtonCopy.Create(StrFromResID(IDS_COPY),dwPushBtnStyle,rt,this,IDC_BUTTON_COPY))
	{	
		TRACE0("Fail to Copy Button\n");
		return FALSE;
	}
	if (!m_wndButtonSkip.Create(StrFromResID(IDS_SKIP2),dwPushBtnStyle,rt,this,IDC_BUTTON_SKIP))
	{
		TRACE0("Fail to Skip Button\n");
		return FALSE;
	}
	
	const DWORD dwRadioBtnStyle = WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON;
	if(!m_wndRadiobyCode.Create(StrFromResID(IDS_MATCH_LAYERCODE),dwRadioBtnStyle,rt,this,IDC_RADIO_CODE))
	{
		TRACE0("Fail to Matched Button by Code\n");
		return FALSE;
	}
	if(!m_wndRadiobyName.Create(StrFromResID(IDS_MATCH_LAYERNAME),dwRadioBtnStyle,rt,this,IDC_RADIO_NAME))
	{
		TRACE0("Fail to Matched Button by Name");
		return FALSE;
	}
	
	const DWORD dwEditStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_READONLY;

	if(!m_wndEditCopyedStatus.CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"), NULL,dwEditStyle,rt, this,IDC_EDIT_CPYSTATUS))
	{	
		TRACE0("Fail to Copyed Edit\n");
		return FALSE;
	}
	if (!m_wndEditSrcValStatus.CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"), NULL,dwEditStyle,rt, this,IDC_EDIT_SRCVALUESTATUS))
	{
		TRACE0("Fail to Value State Edit of Source Scheme\n");
		return FALSE;
	}
    if (!m_wndEditDesValStatus.CreateEx(WS_EX_CLIENTEDGE,_T("EDIT"),NULL,dwEditStyle,rt,this,IDC_EDIT_DESVALUESTATUS))
    {
		TRACE0("Fail to Value State Edit of Destination Scheme\n");
		return FALSE;
    }
	m_wndEditCopyedStatus.ShowWindow(SW_SHOW);
	m_wndEditSrcValStatus.ShowWindow(SW_SHOW);
	return TRUE;
}

BOOL CSchemePropertyCopy::InitializeAppendedCtrls()
{
	//Push Button
	InitializePushButton();

	
	//Radio button
	InitializeRadio();


	//Edit Ctrl
	InitializeEditCtrl();

	return TRUE;
}

void CSchemePropertyCopy::OnBnClickedRadioByCode()
{
	m_bMatchedbyLayerName = FALSE;
	return ;
}

void CSchemePropertyCopy::OnBnClickedRadioByName()
{
  	m_bMatchedbyLayerName = TRUE;
}

BOOL CSchemePropertyCopy::InitializePushButton()
{
	RECT rectBtnCpy;
	RECT rectBtnCancel;
	CWnd* pCtrlHandleMerge = this->GetDlgItem(IDC_BUTTON_HANDMERGE);
	CWnd* pCtrlCancel = this->GetDlgItem(IDCANCEL); 
	if ((!pCtrlHandleMerge)||(!pCtrlCancel))
	{
		return FALSE;
	}
	
	pCtrlHandleMerge->GetWindowRect(&rectBtnCpy);
	pCtrlCancel->GetWindowRect(&rectBtnCancel);
	ScreenToClient(&rectBtnCpy);
	ScreenToClient(&rectBtnCancel);
	if(!m_wndButtonCopy.GetSafeHwnd())
	{
		return FALSE;
	}
	m_wndButtonCopy.SetWindowPos(NULL,rectBtnCpy.left-5,rectBtnCpy.top,rectBtnCpy.right-rectBtnCpy.left+10,rectBtnCancel.bottom-rectBtnCancel.top,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndButtonCopy.SetFont(this->GetDlgItem(IDC_BUTTON_HANDMERGE)->GetFont(),TRUE); 

	RECT rectBtnUp;
	CWnd* pCtrlBtnUp = this->GetDlgItem(IDC_BUTTON_UP);
	if (!pCtrlBtnUp)
	{
		return FALSE;
	}
	pCtrlBtnUp->GetWindowRect(&rectBtnUp);
	ScreenToClient(&rectBtnUp);
	if (!m_wndButtonSkip.GetSafeHwnd())
	{
		return FALSE;
	}
	m_wndButtonSkip.SetWindowPos(NULL,rectBtnCpy.left-5,rectBtnUp.top,rectBtnCpy.right-rectBtnCpy.left+10,rectBtnCancel.bottom-rectBtnCancel.top,SWP_NOACTIVATE|SWP_NOZORDER);
	m_wndButtonSkip.SetFont(this->GetDlgItem(IDC_BUTTON_UP)->GetFont(),FALSE);

	//2012.9.18.弹出"删除"按钮，改成删除属性。
	RECT rectBtnDel;
	RECT rectEditMerge;
	m_pwndButtonDel = this->GetDlgItem(IDC_BUTTON_DEL);
	CWnd* pCtrlEditMerge = this->GetDlgItem(IDC_EDIT_MERGEPATH);
	if(m_pwndButtonDel)
	{	
		m_pwndButtonDel->GetWindowRect(&rectBtnDel);
		ScreenToClient(&rectBtnDel);
	}
	if (pCtrlEditMerge)
	{
		pCtrlEditMerge->GetWindowRect(&rectEditMerge);
		ScreenToClient(&rectEditMerge);
	}
	RECT rectBtnDelProp;
	memset(&rectBtnDelProp,0,sizeof(RECT));
	rectBtnDelProp.left = rectEditMerge.right - (rectBtnDel.right-rectBtnDel.left);
	rectBtnDelProp.right = rectBtnDelProp.left + (rectBtnDel.right-rectBtnDel.left)+30;
	rectBtnDelProp.top = rectEditMerge.top ;
	rectBtnDelProp.bottom = rectBtnDelProp.top + (rectBtnDel.bottom-rectBtnDel.top);
	m_pwndButtonDel->MoveWindow(&rectBtnDelProp);
	m_pwndButtonDel->SetWindowText(StrFromResID(IDS_DEL_ATTRIBUTES));
	m_pwndButtonDel->ShowWindow(TRUE);
	
	//{2012.9.21
	this->GetDlgItem(IDC_BUTTON_SAVE)->MoveWindow(rectBtnCancel.left-140, rectBtnCancel.top + 10, rectBtnCancel.right - rectBtnCancel.left, rectBtnCancel.bottom - rectBtnCancel.top);
	this->GetDlgItem(IDCANCEL)->MoveWindow(rectBtnCancel.left,rectBtnCancel.top+10,rectBtnCancel.right-rectBtnCancel.left,rectBtnCancel.bottom-rectBtnCancel.top);
	//}
	//{2012.9.20
	if((m_DesTree.GetCount()<=0)
		||(m_SrcTree.GetCount()<=0))
	{

		EnableBtnCtrl(FALSE);
	}
	//}
	return TRUE;
}

BOOL CSchemePropertyCopy::InitializeRadio()
{
	RECT rectStaticMergePath;
	
	RECT rectRadiobyName;
	
	CWnd* pCtrlStaticMergePath = this->GetDlgItem(IDC_STATIC_MERGEPATH);
	CWnd* pCtrlEditMergePath = this->GetDlgItem(IDC_EDIT_MERGEPATH);
	if ((!pCtrlStaticMergePath)||(!pCtrlEditMergePath))
	{
		return FALSE;
	}
	pCtrlStaticMergePath->GetWindowRect(&rectStaticMergePath);
	pCtrlEditMergePath->GetWindowRect(&m_RectEditMergePath);
	ScreenToClient(&rectStaticMergePath);
	ScreenToClient(&m_RectEditMergePath);
	
	rectRadiobyName.left = m_RectEditMergePath.left;
	rectRadiobyName.top	 = m_RectEditMergePath.top;
	rectRadiobyName.right = rectRadiobyName.left + (rectStaticMergePath.right-rectStaticMergePath.left);
	rectRadiobyName.bottom = m_RectEditMergePath.bottom;
	
	if (m_wndRadiobyCode.GetSafeHwnd())
	{
		m_wndRadiobyCode.SetWindowPos(NULL,rectStaticMergePath.left,rectStaticMergePath.top,
			rectStaticMergePath.right - rectStaticMergePath.left,
		rectStaticMergePath.bottom - rectStaticMergePath.top,SWP_NOACTIVATE|SWP_NOZORDER);
		 m_wndRadiobyCode.SetFont(this->GetDlgItem(IDC_STATIC_MERGEPATH)->GetFont(),FALSE);
	}
	if(m_wndRadiobyName.GetSafeHwnd())
	{
		m_wndRadiobyName.SetWindowPos(NULL,rectRadiobyName.left,rectRadiobyName.top,
			rectRadiobyName.right - rectRadiobyName.left,
		rectRadiobyName.bottom - rectRadiobyName.top,SWP_NOACTIVATE|SWP_NOZORDER);
		m_wndRadiobyName.SetFont(this->GetDlgItem(IDC_EDIT_MERGEPATH)->GetFont(),FALSE);
	}
	return TRUE;	
}

BOOL CSchemePropertyCopy::InitializeEditCtrl()
{
	RECT rectEditCpyStatus;
	RECT rectStaticDes;
	RECT rectStaticSrc;
	RECT rectSrcEditValueStatus;
	RECT rectDesEditValueStatus;

	CWnd *pStaticDesWnd;
	CWnd *pStaticSrcWnd;
	if (!(pStaticDesWnd=this->GetDlgItem(IDC_STATIC_DES)))
	{
		TRACE0("Fail to get the rect of IDC_STATIC_DES\n");
		return FALSE;
	}
	
	if (!(pStaticSrcWnd=this->GetDlgItem(IDC_STATIC_RES)))
	{
		TRACE0("Fail to get the rect of IDC_STATIC_RES\n");
		return FALSE;
	}
	pStaticDesWnd->GetWindowRect(&rectStaticDes);
	pStaticSrcWnd->GetWindowRect(&rectStaticSrc);
	ScreenToClient(&rectStaticDes);
	ScreenToClient(&rectStaticSrc);

	int widthEditCpyStatus = m_RectEditMergePath.right - m_RectEditMergePath.left;
	int HeightEditCpyStatus = m_RectEditMergePath.bottom - m_RectEditMergePath.top;
	rectEditCpyStatus.left = m_RectEditMergePath.left+widthEditCpyStatus/4+20;
	rectEditCpyStatus.right = m_RectEditMergePath.right-widthEditCpyStatus/4+20;
	rectEditCpyStatus.top = m_RectEditMergePath.top;
	rectEditCpyStatus.bottom = m_RectEditMergePath.bottom;
	
	rectSrcEditValueStatus.left = rectStaticDes.left;
	rectSrcEditValueStatus.right = rectSrcEditValueStatus.left + (rectEditCpyStatus.right-rectEditCpyStatus.left);
	rectSrcEditValueStatus.top = rectStaticDes.bottom + 10;
	rectSrcEditValueStatus.bottom = rectSrcEditValueStatus.top + (rectEditCpyStatus.bottom-rectEditCpyStatus.top);
	
	rectDesEditValueStatus.left = rectStaticSrc.left;
	rectDesEditValueStatus.right = rectDesEditValueStatus.left + (rectEditCpyStatus.right-rectEditCpyStatus.left);
	rectDesEditValueStatus.top = rectStaticSrc.bottom + 10;
	rectDesEditValueStatus.bottom = rectDesEditValueStatus.top + (rectEditCpyStatus.bottom-rectEditCpyStatus.top);

	if (m_wndEditCopyedStatus.GetSafeHwnd())
	{
		m_wndEditCopyedStatus.SetFont(this->GetDlgItem(IDC_STATIC_DES)->GetFont(),FALSE);
		m_wndEditCopyedStatus.MoveWindow(&rectEditCpyStatus,TRUE);
	}
	if(m_wndEditSrcValStatus.GetSafeHwnd())
	{
		m_wndEditSrcValStatus.SetFont(this->GetDlgItem(IDC_STATIC_DES)->GetFont(),FALSE);
		m_wndEditSrcValStatus.MoveWindow(&rectSrcEditValueStatus,TRUE);
	}
	if (m_wndEditDesValStatus.GetSafeHwnd())
	{
		m_wndEditDesValStatus.SetFont(this->GetDlgItem(IDC_STATIC_DES)->GetFont(),FALSE);
		m_wndEditDesValStatus.MoveWindow(&rectDesEditValueStatus,TRUE);
	}
	ExchangeCtrlPos(IDC_EDIT_SRCVALUESTATUS,IDC_EDIT_DESVALUESTATUS);
	return TRUE;
}

void CSchemePropertyCopy::OnBnClickedButtonCopy()
{
	if(!m_pWndEditCopyStatus)
	{
		if(!(m_pWndEditCopyStatus=this->GetDlgItem(IDC_EDIT_CPYSTATUS)))
		{
			return;		
		}	
	}
	((CEdit*)m_pWndEditCopyStatus)->Clear();
    	
	
	if(CopyPropstoDes(m_hSelectedTreeItemArray[DES-1], m_hSelectedTreeItemArray[SRC-1], m_bSpecialLayer))
	{	
		m_bModified = TRUE;//2012.9.13
		m_pWndEditCopyStatus->SetWindowText(StrFromResID(IDS_COPY_OK));
	}
	else
	{
		m_pWndEditCopyStatus->SetWindowText(StrFromResID(IDS_COPY_FAILED));
	}
	//按层码或层名定位到下一个节点。
	GotoNextNode(m_SrcTree,m_DesTree,m_bMatchedbyLayerName);

	//显示源，目方案节点属性值的状态。
    RefreshSrcValStatusCtrl();
	RefreshDesValStatusCtrl();
	return;
}

void CSchemePropertyCopy::OnBnClickedButtonSkip()
{	
	if (!m_pWndEditCopyStatus)
	{
		if (!(m_pWndEditCopyStatus=this->GetDlgItem(IDC_EDIT_CPYSTATUS)))
		{
			return;
		}
	}

	((CEdit*)m_pWndEditCopyStatus)->Clear();
	m_pWndEditCopyStatus->SetWindowText(StrFromResID(IDS_SKIP));
	
	GotoNextNode(m_SrcTree,m_DesTree,m_bMatchedbyLayerName);
	return;
}

BOOL CSchemePropertyCopy::PreTranslateMessage( MSG* pMsg )
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN)
		{
			if (m_SrcTree.GetParentItem(m_hSelectedTreeItemArray[SRC-1])
				&&m_DesTree.GetParentItem(m_hSelectedTreeItemArray[DES-1]))
			{

				OnBnClickedButtonCopy();
			}
			else
			{
				if(::GetFocus()==m_SrcTree.GetSafeHwnd())
				{
					m_SrcTree.Expand(m_hSelectedTreeItemArray[SRC-1],TVE_EXPAND);
				}
				else if(::GetFocus()==m_DesTree.GetSafeHwnd())
				{
					m_DesTree.Expand(m_hSelectedTreeItemArray[DES-1],TVE_EXPAND);
				}
				
			}
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE)
		{ 
			if (m_SrcTree.GetParentItem(m_hSelectedTreeItemArray[SRC-1])
				&&m_DesTree.GetParentItem(m_hSelectedTreeItemArray[DES-1]))
			{
				OnBnClickedButtonSkip();
			}
			else
			{
				if(::GetFocus()==m_SrcTree.GetSafeHwnd())
				{
					m_SrcTree.Expand(m_hSelectedTreeItemArray[SRC-1],TVE_EXPAND);
				}
				else if(::GetFocus()==m_DesTree.GetSafeHwnd())
				{
					m_DesTree.Expand(m_hSelectedTreeItemArray[DES-1],TVE_EXPAND);
				}
				
			}
			return TRUE;

		}
		else if (pMsg->wParam == VK_UP)
		{
			if ((::GetFocus()!=m_SrcTree.GetSafeHwnd())
				&&(::GetFocus()!=m_DesTree.GetSafeHwnd()))
			{
				m_hSelectedTreeItemArray[SRC-1] = m_SrcTree.GetNextItem(m_hSelectedTreeItemArray[SRC-1],TVGN_PREVIOUSVISIBLE);
				m_SrcTree.SetFocus();
				m_hSelectedTreeItemArray[SRC-1] = m_SrcTree.GetNextItem(m_hSelectedTreeItemArray[SRC-1],TVGN_PREVIOUSVISIBLE); 
			}
			if ((::GetFocus()==m_SrcTree.GetSafeHwnd())
				&&m_hSelectedTreeItemArray[SRC-1])
			{
				//{2012.9.24
				if (m_SrcTree.GetParentItem(m_hSelectedTreeItemArray[SRC-1]))
				{
					CString strSrcLayerText = m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]);
					__int64 nLayerCode;
					CString strLayerName;
					SplitCodeandNameofLayer(nLayerCode,strLayerName,strSrcLayerText);

					m_CurLayerIndex = m_SrcConfig.pScheme->GetLayerDefineIndex(strLayerName);
				}
				//}
				m_SrcTree.SelectItem(m_hSelectedTreeItemArray[SRC-1]);
				m_SrcTree.SetFocus();
			
			}
			else if ((::GetFocus()==m_DesTree.GetSafeHwnd())
					&&m_hSelectedTreeItemArray[DES-1])
			{
				m_DesTree.SelectItem(m_hSelectedTreeItemArray[DES-1]);
				m_DesTree.SetFocus();
			}
			

		}
		else if(pMsg->wParam == VK_DOWN)
		{

			if ((::GetFocus()!=m_SrcTree.GetSafeHwnd())
				&&(::GetFocus()!=m_DesTree.GetSafeHwnd()))
			{
				m_hSelectedTreeItemArray[SRC-1] = m_SrcTree.GetNextItem(m_hSelectedTreeItemArray[SRC-1],TVGN_NEXTVISIBLE);
				m_SrcTree.SetFocus();
				m_hSelectedTreeItemArray[SRC-1] = m_SrcTree.GetNextItem(m_hSelectedTreeItemArray[SRC-1],TVGN_NEXTVISIBLE);
			}
			if ((::GetFocus()==m_SrcTree.GetSafeHwnd())&&m_hSelectedTreeItemArray[SRC-1])
			{
				//{2012.9.24
				if (m_SrcTree.GetParentItem(m_hSelectedTreeItemArray[SRC-1]))
				{
					CString strSrcLayerText = m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]);
					__int64 nLayerCode;
					CString strLayerName;
					SplitCodeandNameofLayer(nLayerCode,strLayerName,strSrcLayerText);					
					m_CurLayerIndex = m_SrcConfig.pScheme->GetLayerDefineIndex(strLayerName);
				}
				//}
				m_SrcTree.SelectItem(m_hSelectedTreeItemArray[SRC-1]);
				m_SrcTree.SetFocus();
			}
			else if ((::GetFocus()==m_DesTree.GetSafeHwnd())&&m_hSelectedTreeItemArray[DES-1])
			{
				m_DesTree.SelectItem(m_hSelectedTreeItemArray[DES-1]);
				m_DesTree.SetFocus();
			}

		}
	}
	return CSchemeMerge::PreTranslateMessage(pMsg);
	//return TRUE;
}

__int64 CSchemePropertyCopy::GetMatchedCode( __int64 nSrcLayerCode, USERIDX& desUserIdx)
{
	__int64 nMatchedDesLayerCode;
	vector<__int64> srcLayerCodeArray;
	srcLayerCodeArray.clear();
	BOOL bExistSameCode = FALSE;
	
	for(int i=0; i<desUserIdx.m_aGroup.GetSize();i++)
	{
		
		if (((CString)desUserIdx.m_aGroup[i].GroupName).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			continue;
		}
		for(int j=0; j<desUserIdx.m_aIdx.GetSize(); j++)
		{
			if (desUserIdx.m_aIdx[j].groupidx==i)
			{
				srcLayerCodeArray.push_back(desUserIdx.m_aIdx[j].code);
				if (desUserIdx.m_aIdx[j].code==nSrcLayerCode)
				{
					nMatchedDesLayerCode = nSrcLayerCode;
					bExistSameCode = TRUE;
				}
			}
			
		}
	}
 	long nLayerDefineCount = m_DesConfig.pScheme->GetLayerDefineCount();

	long nVectorSize = srcLayerCodeArray.size();

	if(!bExistSameCode)
	{
		srcLayerCodeArray.push_back(nSrcLayerCode);
		
		sort(srcLayerCodeArray.begin(),srcLayerCodeArray.end(),less<long>());
		//vector<long>::iterator iter;
		int iPos = 0;
		for (int i=0; i<srcLayerCodeArray.size(); i++)
		{
			if (srcLayerCodeArray.at(i)==nSrcLayerCode)
			{
				iPos = i;
				break;
			}
		}
		if (iPos==0)
		{
			if (srcLayerCodeArray.size()>1)
				nMatchedDesLayerCode = srcLayerCodeArray.at(iPos+1);
		}
		else if(iPos==srcLayerCodeArray.size()-1)
		{
			nMatchedDesLayerCode = srcLayerCodeArray.at(iPos-1);
		}
		else
		{
			long iForwardDif = srcLayerCodeArray.at(iPos) - srcLayerCodeArray.at(iPos-1);
			long iBackwardDif = srcLayerCodeArray.at(iPos+1) - srcLayerCodeArray.at(iPos);
			if (iForwardDif <= iBackwardDif)
			{
				nMatchedDesLayerCode = srcLayerCodeArray.at(iPos-1);
			}
			else
			{
				nMatchedDesLayerCode = srcLayerCodeArray.at(iPos+1);
			}
		}

	}
	return nMatchedDesLayerCode;
}

CString CSchemePropertyCopy::GetMatchedName( CString strSrcLayerName, USERIDX& desUserIdx )
{
	CString strMatchedDesLayerName="";
	vector<CString> strLayerNameArray;
	BOOL bExistSameLayerName=FALSE;
	
	for (int i=0; i<desUserIdx.m_aGroup.GetSize(); i++)
	{
		if (((CString)desUserIdx.m_aGroup[i].GroupName).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			continue;
		}
		for(int j=0; j<desUserIdx.m_aIdx.GetSize(); j++)
		{
			if (desUserIdx.m_aIdx[j].groupidx==i)
			{
				strLayerNameArray.push_back(desUserIdx.m_aIdx[j].FeatureName);
				if (strSrcLayerName.CompareNoCase(desUserIdx.m_aIdx[j].FeatureName)==0)
				{
					strMatchedDesLayerName = desUserIdx.m_aIdx[j].FeatureName;
					bExistSameLayerName = TRUE;
					//break;
				}
			}	
		}


	}

	if (!bExistSameLayerName)
	{
		strMatchedDesLayerName = GetLayerNamebyCoincidence(strSrcLayerName,strLayerNameArray);
	}
	return strMatchedDesLayerName;
}

CString CSchemePropertyCopy::GetLayerNamebyCoincidence( CString strLayerName,vector<CString>& strLayerNameArray )
{
	CString strLayerNamewithMaxCoincidence="";
	if(strLayerNameArray.size()<=0)
	{
		return CString();
	}
	strLayerNamewithMaxCoincidence = strLayerNameArray.at(0);

	wstring wstrLayerName = CstrtoWstr(strLayerName);
	//{2012.9.19
	RemoveSameElems(wstrLayerName);
	RemoveSpecificElems(wstrLayerName,m_wstrInvalidChars);
	//}
	vector<CString>::iterator iter;
	long nMaxCoincidence = 0;
	
	for (iter=strLayerNameArray.begin(); iter!=strLayerNameArray.end(); iter++)
	{
		wstring wstrTempDes = CstrtoWstr(*iter);
		//{2012.9.19
		RemoveSameElems(wstrTempDes);
		RemoveSpecificElems(wstrTempDes,m_wstrInvalidChars);
		//}
		long nCoincidence = GetCoincidenceNum(wstrLayerName,wstrTempDes);

		if (nMaxCoincidence<nCoincidence)
		{
			nMaxCoincidence = nCoincidence;
			strLayerNamewithMaxCoincidence = *iter;
		}

	}
	return strLayerNamewithMaxCoincidence;
}

BOOL CSchemePropertyCopy::HasXDefines(HTREEITEM hItem, CMergeTree& treeCtrl, ConfigLibItem config)
{
	if (!hItem)
	{
		return FALSE;
	}
	
	CString strCodeandName;
	long nLayerCode = -2;
	CString strLayerName = "";
	strCodeandName = treeCtrl.GetItemText(hItem);
	
    sscanf(strCodeandName,"%I64 %s",&nLayerCode,strLayerName.GetBuffer(256)); 

	strLayerName.ReleaseBuffer();
	HTREEITEM hParentItem;
	hParentItem = treeCtrl.GetParentItem(hItem);
	if (hParentItem)
	{
		CString str = treeCtrl.GetItemText(hParentItem);
		if(str.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)		
		m_bSpecialLayer = TRUE;
	}
	//根据层名来判断。
	CSchemeLayerDefine *pSchemeLayerDefine = NULL;
	if(!pSchemeLayerDefine) //2012.9.24
	{
		pSchemeLayerDefine = config.pScheme->GetLayerDefine(strLayerName,FALSE,0,m_bSpecialLayer);
	}
	
	if (!pSchemeLayerDefine)
	{
		return FALSE;
	}
	
	int nXDefinesNum = 0;
	BOOL bHasXDefines = FALSE;
	pSchemeLayerDefine->GetXDefines(nXDefinesNum);
	if(nXDefinesNum > 0)
	{
		bHasXDefines = TRUE;
	}
	
	return bHasXDefines;
}

BOOL CSchemePropertyCopy::CopyPropstoDes( HTREEITEM hDesItem, HTREEITEM hSrcItem, BOOL bSpecialLayer)
{
	if ((!hDesItem)||(!hSrcItem))
	{
		return FALSE;
	}
	
	CString strDesItemText = m_DesTree.GetItemText(hDesItem);
	CString strSrcItemText = m_SrcTree.GetItemText(hSrcItem);

	__int64 nSrcLayerCode = -2;
	__int64 nDesLayerCode = -2;
	CString strSrcLayerName = "";
	CString strDesLayerName = "";
	
	if (!bSpecialLayer)
	{
		SplitCodeandNameofLayer(nSrcLayerCode,strSrcLayerName,strSrcItemText);
		SplitCodeandNameofLayer(nDesLayerCode,strDesLayerName,strDesItemText);
	}
	else
	{
		strSrcLayerName = strSrcItemText;
		strDesLayerName = strDesItemText;
	}
	
	if ((strSrcLayerName == "")
		||(strDesLayerName == ""))
	{
		return FALSE;
	}
	CSchemeLayerDefine* pSrcSchemeLayerDefine = NULL;
	CSchemeLayerDefine* pDesSchemeLayerDefine = NULL;
// 	
	pSrcSchemeLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(strSrcLayerName,FALSE,0,bSpecialLayer);
	pDesSchemeLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesLayerName,FALSE,0,bSpecialLayer);
	
	if ((!pSrcSchemeLayerDefine)
		||(!pDesSchemeLayerDefine))
	{
		return FALSE;
	}

	SchemeNode srcSchemeNode;
	SchemeNode desSchemeNode;
	srcSchemeNode.nLayerCode = nSrcLayerCode;
	srcSchemeNode.strSchemePath = m_SrcSchemePath;
	desSchemeNode.nLayerCode = nDesLayerCode;
	desSchemeNode.strSchemePath = m_DesSchemePath;

	m_srcLayersInfoCopyedXDefines.push_back(srcSchemeNode);
	m_desLayersInfoCopyedXDefines.push_back(desSchemeNode);
	pDesSchemeLayerDefine->CopyXDefines(*pSrcSchemeLayerDefine);
	m_bModified = TRUE;
	return TRUE;
}

void CSchemePropertyCopy::SplitCodeandNameofLayer( __int64& nLayerCode, CString& strLayerName, CString strItemText )
{
	if (strItemText=="")
	{
		return;
	}
	sscanf(strItemText,"%I64d %s", &nLayerCode, strLayerName.GetBuffer(256));
	strLayerName.ReleaseBuffer();
	return;	
}

void CSchemePropertyCopy::GotoNextNode(CMergeTree& srcTreeCtrl, CMergeTree& desTreeCtrl, BOOL bMatchedbyName)
{
	//int nCount = m_SrcUserIdx.m_aIdx.GetSize();
	srcTreeCtrl.ClearSelection();
	desTreeCtrl.ClearSelection();
	int nSrcNonSpecialLayerCount = m_SrcConfig.pScheme->GetLayerDefineCount();

	if (nSrcNonSpecialLayerCount<=0)
	{
		return;
	}
	m_CurLayerIndex++;

	if ((m_CurLayerIndex > 0)
		&&(m_CurLayerIndex < nSrcNonSpecialLayerCount))
	{
		CSchemeLayerDefine* pSrcSchemeLayerDefine = NULL;

		while(!pSrcSchemeLayerDefine)
		{
			pSrcSchemeLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(m_CurLayerIndex);
		}
		__int64 nSrcLayerCode = 0;
		CString strSrcLayerName = "";
		nSrcLayerCode = pSrcSchemeLayerDefine->GetLayerCode();
		strSrcLayerName = pSrcSchemeLayerDefine->GetLayerName();
		//定位
		if (!m_bMatchedbyLayerName)
		{
			//按层码

			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(srcTreeCtrl,0, TVIS_BOLD|TVIS_SELECTED, nSrcLayerCode,"",NULL);
			
			__int64 nDesLayerCode = GetMatchedCode(nSrcLayerCode,m_DesUserIdx);//m_DesUserIdx→m_DesConfig

			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(desTreeCtrl,0,TVIS_BOLD|TVIS_SELECTED, nDesLayerCode,"",NULL);
			
			
			//{for test 2012.9.11
			if (m_hSelectedTreeItemArray[SRC-1]==NULL)
			{
				AfxMessageBox(_T("源方案树为空！"));
			}
			if (m_hSelectedTreeItemArray[DES-1]==NULL)
			{
				AfxMessageBox(_T("目标方案树为空！"));
			}
			//}
		}
		else
		{
			//按层名
			//AfxMessageBox(_T("Matched by LayerCode"));
			CString strSrcLayercodeandName;
			CString strSrcLayercode;
			strSrcLayercode.Format("%I64d",nSrcLayerCode);
			strSrcLayercodeandName = strSrcLayercode + " " + strSrcLayerName;
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(srcTreeCtrl,0, TVIS_BOLD|TVIS_SELECTED, 0,strSrcLayercodeandName,NULL,FALSE);
			CString strDesLayerName = GetMatchedName(strSrcLayerName,m_DesUserIdx);
			CSchemeLayerDefine *pSchemeLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesLayerName);
			CString strDesLayerCodeandName;
			if (pSchemeLayerDefine)
			{
				CString strLayerCode;
				strLayerCode.Format("%I64d",pSchemeLayerDefine->GetLayerCode());
				strDesLayerCodeandName = strLayerCode +" " +strDesLayerName;
			}
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(desTreeCtrl,0,TVIS_BOLD|TVIS_SELECTED, 0, strDesLayerCodeandName,NULL,FALSE);
		}
	}
	else if(m_CurLayerIndex >= nSrcNonSpecialLayerCount)
	{
		//修改当前索引值
		m_CurLayerIndex = nSrcNonSpecialLayerCount-1;
	}
	//更新属性值编辑框的显示。
	RefreshSrcValStatusCtrl();
	RefreshDesValStatusCtrl();
	//设置源方案树为焦点。2012.9.19
	//srcTreeCtrl.SetFocus();
	return;
}

void CSchemePropertyCopy::RefreshSrcValStatusCtrl()
{
	if(!m_pWndEditSrcXAttStatus)
	{
		m_pWndEditSrcXAttStatus=this->GetDlgItem(IDC_EDIT_SRCVALUESTATUS);
	}
		
	if(m_bHasXDefines = HasXDefines(m_hSelectedTreeItemArray[SRC-1], m_SrcTree ,m_SrcConfig))
	{
		m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCHASVALUE);
	}
	else
	{
		m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCNOVALUE);
	}
	m_pWndEditSrcXAttStatus->SetWindowText(m_strSrcXAttStatusPrompt);

}

void CSchemePropertyCopy::OnSelchangedTreeSrc( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMTREEVIEW *pTree = (NMTREEVIEW *)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	HTREEITEM hParentItem = m_SrcTree.GetParentItem(hItem);
	m_hSelectedTreeItemArray[SRC-1] = hItem;
	
	if (!m_hSelectedTreeItemArray[SRC-1])  //2012.9.20
	{
		m_DesTree.ClearSelection();
	}

	m_SchemePosID = SRC;

	 CString strSrcItemText;
	strSrcItemText = m_SrcTree.GetItemText(hItem);


	__int64 nSrcLayerCode=0;
	CString strSrcLayerName="";
	SplitCodeandNameofLayer(nSrcLayerCode,strSrcLayerName,strSrcItemText);

	//有无属性值
	RefreshSrcValStatusCtrl();

	//若选中的为独立符号项则状态显示为空
	if (strSrcLayerName=="")
	{
		ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,"");
		m_DesTree.ClearSelection();
		return;
	}

	if (m_DesTree.GetCount()<=0) //2012.9.20，目标方案树未被加载时，到此为止。
	{
		return;
	}
	//是否复制过
	if (GetElemIndex(nSrcLayerCode, m_SrcSchemePath, m_srcLayersInfoCopyedXDefines)>=0)
	{
		ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,StrFromResID(IDS_CURRENT_NOTCOPY));
	}
	else
	{
		ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,StrFromResID(IDS_CURRENT_NOTCOPY));
	}

	//修改当前索引m_CurLayerIndex;
	m_CurLayerIndex = m_SrcConfig.pScheme->GetLayerDefineIndex(strSrcLayerName);

	__int64 nDesLayerCode = 0;
	if (!m_bMatchedbyLayerName)
	{
		//按层码
		//long nDesLayerCode = 0;
		nDesLayerCode = GetMatchedCode(nSrcLayerCode,m_DesUserIdx);
		m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_SELECTED|TVIS_BOLD,nDesLayerCode,"",NULL);
		//更改目标方案属性值的显示状态。
		RefreshDesValStatusCtrl();
	}
	else
	{
		//按层名
        CString strDesMatchedLayerName;
		strDesMatchedLayerName = GetMatchedName(strSrcLayerName,m_DesUserIdx);
		CSchemeLayerDefine *pDesSchemeLayerDefine=NULL;
		pDesSchemeLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesMatchedLayerName);
		if (pDesSchemeLayerDefine)
		{
			nDesLayerCode = pDesSchemeLayerDefine->GetLayerCode();
		}
		m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode, strDesMatchedLayerName, DES, FALSE);

	}
	//以源方案为依据浏览定位时，显示目标方案中相应层的修改信息。
	CString strDesXAttStatusPrompt="";
	if(GetElemIndex(nDesLayerCode, m_DesSchemePath, m_desLayersInfoCopyedXDefines)>=0)
	{
		strDesXAttStatusPrompt = StrFromResID(IDS_CURRENT_MODIFIED);
	}
	else
	{
		strDesXAttStatusPrompt = StrFromResID(IDS_CURRENT_NOT_MODIFIED);
	}
	m_pWndEditDesXAttStatus->SetWindowText(strDesXAttStatusPrompt);
	return;
}
void CSchemePropertyCopy::OnSelchangedTreeDes( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMTREEVIEW *pTree = (NMTREEVIEW *)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	HTREEITEM hParentItem = m_DesTree.GetParentItem(hItem);
	
	m_SchemePosID = DES;
	m_hSelectedTreeItemArray[DES-1] = hItem;
	if (!m_hSelectedTreeItemArray[DES-1]) //2012.9.20
	{
		m_SrcTree.ClearSelection();
	}
	if ((!hParentItem)
		||(m_DesTree.GetItemText(hParentItem).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0))
	{
		ModifiedCtrlValue(IDC_EDIT_DESVALUESTATUS,"");
		return;
	}
	
	
	CString strDesItemText = "";
	strDesItemText = m_SrcTree.GetItemText(hItem);
	
	__int64 nDesLayerCode;
	CString strDesLayerName;
	SplitCodeandNameofLayer(nDesLayerCode,strDesLayerName,strDesItemText);
	//更改目标方案属性值的显示状态。
	RefreshDesValStatusCtrl();
	//定位到源方案中的匹配节点。
	__int64 nSrcLayerCode = 0;
    if (!m_bMatchedbyLayerName)
    {
		//按层码
		
		nSrcLayerCode = GetMatchedCode(nDesLayerCode,m_SrcUserIdx);
		m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_BOLD|TVIS_SELECTED,nSrcLayerCode,"",NULL);
		//修改当前索引m_curLayerIndex.
		if (m_hSelectedTreeItemArray[SRC-1])
		{
			CString strSrcItemText;
			__int64 nSrcLayerCode=0;
			CString strSrcLayerName = "";
			strSrcItemText = m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]);
			SplitCodeandNameofLayer(nSrcLayerCode,strSrcLayerName,strSrcItemText);
			m_CurLayerIndex = m_SrcConfig.pScheme->GetLayerDefineIndex(strSrcLayerName);
		}

    }
	else
	{
		//按层名
		//AfxMessageBox("单击节点时，按层码匹配！");
		CString strMatchedSrcLayerName;		
		strMatchedSrcLayerName = GetMatchedName(strDesLayerName,m_SrcUserIdx);
		CSchemeLayerDefine*pSchemeLayerDefine=NULL;
		pSchemeLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(strMatchedSrcLayerName);
		m_CurLayerIndex = m_SrcConfig.pScheme->GetLayerDefineIndex(strMatchedSrcLayerName);//2012.9.24
		if (pSchemeLayerDefine)
		{
			nSrcLayerCode = pSchemeLayerDefine->GetLayerCode();
		}
		m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nSrcLayerCode,strMatchedSrcLayerName,SRC,FALSE);

	}
	//浏览目标方案时更改源方案状态。
	if (GetElemIndex(nSrcLayerCode, m_SrcSchemePath, m_srcLayersInfoCopyedXDefines)>=0)
	{
		ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,StrFromResID(IDS_CURRENT_COPIED));
	}
	else
	{
		ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,StrFromResID(IDS_CURRENT_NOTCOPY));
	}
	RefreshSrcValStatusCtrl();
	return;
}
long CSchemePropertyCopy::GetElemIndex( __int64 nCode, CString strSchemePath, vector<SchemeNode>& nSchemeNodeArray )
{
	long nLength = nSchemeNodeArray.size();
	if (nLength <= 0)
	{
		return -1;
	}
	long nIndex = -1;

	SchemeNode tmpSchemeNode;
	tmpSchemeNode.nLayerCode = nCode;
	tmpSchemeNode.strSchemePath = strSchemePath;
	for(int i=0; i<nLength; i++)
	{
		if (nSchemeNodeArray.at(i)==tmpSchemeNode)
		{
			nIndex = i;
			break;
		}
	}
	return nIndex;
}

void CSchemePropertyCopy::ModifiedCtrlValue( UINT nCtrlID, LPCTSTR strNoteforPrompt )
{
	UpdateData(TRUE);
	CWnd *pWnd=NULL;
	if(pWnd=GetDlgItem(nCtrlID))
	{
		pWnd->SetWindowText(strNoteforPrompt);
	}
	UpdateData(FALSE);
	return;
}

void CSchemePropertyCopy::OnSelChangeSrcScaleCombo()
{
	CSchemeMerge::OnSelChangeSrcScaleCombo();
	if (!m_bChangedSrcComboScale)
	{
		return;
	}

	//将当前源方案的层索引置为零。
	
	__int64 nLayerCode = 0;
	CString strLayerName = "";
	BOOL bInvalidItem = FALSE;
	
	//看是否存在目标方案
	if (m_DesTree.GetCount()<=0)
	{
		return;
	}
	HTREEITEM hDesParentItem;
	hDesParentItem = m_DesTree.GetParentItem(m_hSelectedTreeItemArray[DES-1]);
	if ((!hDesParentItem)
		||(m_DesTree.GetItemText(hDesParentItem).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0))
	{
		bInvalidItem = TRUE;
		m_CurLayerIndex = 0;
		CSchemeLayerDefine* pSrcSchemeLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(m_CurLayerIndex);
		if(!pSrcSchemeLayerDefine)
		{
			return;
		}
		nLayerCode = pSrcSchemeLayerDefine->GetLayerCode();
		strLayerName = pSrcSchemeLayerDefine->GetLayerName();
	}
	else
	{
		SplitCodeandNameofLayer(nLayerCode,strLayerName,m_DesTree.GetItemText(m_hSelectedTreeItemArray[DES-1]));
	}
	
	if (!m_bMatchedbyLayerName)
	{
		//匹配目标方案的选中节点
		if (!bInvalidItem)
		{
			__int64 nSrcMatchedLayerCode = GetMatchedCode(nLayerCode,m_SrcUserIdx);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_BOLD|TVIS_SELECTED,nSrcMatchedLayerCode,"",NULL);		
		}
		else
		{	
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_BOLD|TVIS_SELECTED,nLayerCode,"",NULL);
			__int64 nDesMatchedLayerCode = GetMatchedCode(nLayerCode,m_DesUserIdx);
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_SELECTED|TVIS_BOLD,nDesMatchedLayerCode,"",NULL);
		}


	}
	else
	{		
		if (!bInvalidItem)
		{
			CString strSrcMatchedLayerName;
			__int64 nSrcLayerCode;
			strSrcMatchedLayerName = GetMatchedName(strLayerName,m_SrcUserIdx);
			CSchemeLayerDefine *pSrcLayerDefine=NULL;
			pSrcLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(strSrcMatchedLayerName);
			if (pSrcLayerDefine)
			{
				nSrcLayerCode = pSrcLayerDefine->GetLayerCode();
			}
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nSrcLayerCode,strSrcMatchedLayerName,SRC,FALSE);

		}
		else{
			//定位到源方案的第一个节点，然后匹配到目标方案。
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nLayerCode,strLayerName,SRC,FALSE);
			CString strDesMatchedLayerName;
			strDesMatchedLayerName = GetMatchedName(strLayerName, m_DesUserIdx);
			CSchemeLayerDefine*pDesLayerDefine = NULL;
			pDesLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesMatchedLayerName);
			__int64 nDesLayerCode;
			if (pDesLayerDefine)
			{
				nDesLayerCode = pDesLayerDefine->GetLayerCode();
			}
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode,strDesMatchedLayerName,DES,FALSE);			
		}

	}
	//将当前复制过的数组清空。
	//更改属性值的显示状态。

	if (HasXDefines(m_hSelectedTreeItemArray[SRC-1],m_SrcTree,m_SrcConfig))
	{
		m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCHASVALUE);
	}
	else
	{
		m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCNOVALUE);
	}
	ModifiedCtrlValue(IDC_EDIT_SRCVALUESTATUS,m_strSrcXAttStatusPrompt);
	//更改拷贝的显示状态。
	ModifiedCtrlValue(IDC_EDIT_CPYSTATUS, "");
	return;
}

void CSchemePropertyCopy::OnSelChangeDesScaleCombo()
{
	CSchemeMerge::OnSelChangeDesScaleCombo();
	if (!m_bChangedDesComboScale)
	{
		return;
	}
	//保存提示时，选“否”，则清理目标方案和源方案下SchemeInfo
	if (m_bSaved==NOSAVED)
	{
		ClearSchemeInfo(m_SrcSchemePath,m_srcLayersInfoCopyedXDefines);
		ClearSchemeInfo(m_DesSchemePath,m_desLayersInfoCopyedXDefines);
		m_bSaved = FALSE;
	}
	
	//根据源方案中的当前选中项，来定位目标方案中匹配节点。
	__int64 nLayerCode = 0;
	CString strLayerName = "";
	BOOL bInvalidItem = FALSE;
	HTREEITEM hSrcParentItem;
	hSrcParentItem = m_SrcTree.GetParentItem(m_hSelectedTreeItemArray[SRC-1]);
	
	if ((!hSrcParentItem)
		||(m_SrcTree.GetItemText(hSrcParentItem).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0))
	{
		//return;
		bInvalidItem = TRUE;
		m_CurLayerIndex=0;
		CSchemeLayerDefine *pSrcLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(m_CurLayerIndex);
		if(pSrcLayerDefine)
		{
			nLayerCode = pSrcLayerDefine->GetLayerCode();
			strLayerName = pSrcLayerDefine->GetLayerName();
		}
		
	}
	else
	{
		SplitCodeandNameofLayer(nLayerCode,strLayerName,m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]));
	}

	
	if (!m_bMatchedbyLayerName)
	{
		__int64 nDesMatchedLayerCode = GetMatchedCode(nLayerCode,m_DesUserIdx);
		if (!bInvalidItem)
		{			
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_SELECTED|TVIS_BOLD,nDesMatchedLayerCode,"",NULL);
		}			
		else
		{	
			//重新找到源方案的第一个节点
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_SELECTED|TVIS_BOLD,nLayerCode,"",NULL);
			//long nDesLayerCode = GetMatchedCode(nLayerCode,m_DesUserIdx);
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_BOLD|TVIS_SELECTED,nDesMatchedLayerCode,"",NULL);
		}
				
		
	}
	else
	{
		//AfxMessageBox(_T("目标方案已更改,按层名匹配！"));
		__int64 nDesLayerCode;
		CString strDesMatchedLayerName = GetMatchedName(strLayerName,m_DesUserIdx);
		CSchemeLayerDefine *pDesSchemeLayerDefine = NULL;
		pDesSchemeLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesMatchedLayerName);
		if(pDesSchemeLayerDefine)
		{
			nDesLayerCode = pDesSchemeLayerDefine->GetLayerCode();
		}
		if (!bInvalidItem)
		{

			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode,strDesMatchedLayerName,DES,FALSE);
			
		}
		else
		{
			//重新定位到源方案的第一个节点，然后在目标方案中选择与之匹配的节点。
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nLayerCode,strLayerName,SRC,FALSE);

			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode,strDesMatchedLayerName,DES,FALSE);

		}

	}
	//更改拷贝的显示状态为空，无需更改目标属性值的显示状态。
	ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,"");
	ModifiedCtrlValue(IDC_EDIT_DESVALUESTATUS,StrFromResID(IDS_CURRENT_MODIFIED));

	return;
}

void CSchemePropertyCopy::RefreshDesValStatusCtrl()
{
	if(!m_pWndEditDesXAttStatus)
	{
		m_pWndEditDesXAttStatus=this->GetDlgItem(IDC_EDIT_DESVALUESTATUS);
	}

	if (!m_hSelectedTreeItemArray[DES-1])
	{
		return;
	}
	
	CString strDesItemText;
	strDesItemText = m_DesTree.GetItemText(m_hSelectedTreeItemArray[DES-1]);
	__int64 nDesLayerCode=0;
	CString strDesLayerName="";
	
	SplitCodeandNameofLayer(nDesLayerCode,strDesLayerName,strDesItemText);

	CString strDesXAttStatusPrompt="";
	if(GetElemIndex(nDesLayerCode, m_DesSchemePath, m_desLayersInfoCopyedXDefines)>=0)
	{
		strDesXAttStatusPrompt =StrFromResID(IDS_CURRENT_MODIFIED);
	}
	else
	{
		strDesXAttStatusPrompt = StrFromResID(IDS_CURRENT_NOT_MODIFIED);
	}
	m_pWndEditDesXAttStatus->SetWindowText(strDesXAttStatusPrompt);
	return;
}

void CSchemePropertyCopy::OnButtonSrcpath()
{
	CSchemeMerge::OnButtonSrcpath();
	if (!m_bChangedSrcSchemePath)
	{
		return;
	}
	if ((m_DesTree.GetCount() > 0)
		&& (m_DesTree.GetCount() > 0))
	{
		EnableBtnCtrl(TRUE);
	}
	if (m_bSaved==NOSAVED)
	{
		ClearSchemeInfo(m_SrcSchemePath,m_srcLayersInfoCopyedXDefines);
		ClearSchemeInfo(m_DesSchemePath,m_desLayersInfoCopyedXDefines);
		m_bSaved = FALSE;
	}
	//重新定位源目树的节点
	ReLocateNodeofSrcTreeView();
	return;
}

void CSchemePropertyCopy::OnButtonDespath()
{
	CSchemeMerge::OnButtonDespath();
	if(!m_bChangedDesSchemePath)
	{	
		return;
	}

	if (m_bSaved==NOSAVED)
	{
		ClearSchemeInfo(m_SrcSchemePath,m_srcLayersInfoCopyedXDefines);
		ClearSchemeInfo(m_DesSchemePath,m_desLayersInfoCopyedXDefines);
		m_bSaved = FALSE;
	}
	//重新定位源目树的节点
	ReLocateNodeofDesTreeView();
	return;
}

void CSchemePropertyCopy::ReLocateNodeofSrcTreeView()
{	
	__int64 nLayerCode = 0;
	CString strLayerName = "";
	BOOL bInvalidItem = FALSE;
	//{2012.9.20
	if (m_DesTree.GetCount()<=0)
	{
		return;
	}
	//}
	HTREEITEM hDesParentItem;
	hDesParentItem = m_DesTree.GetParentItem(m_hSelectedTreeItemArray[DES-1]);
	if ((!hDesParentItem)
		||(m_DesTree.GetItemText(hDesParentItem).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0))
	{
		bInvalidItem = TRUE;
		m_CurLayerIndex = 0;
		CSchemeLayerDefine* pSrcSchemeLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(m_CurLayerIndex);
		if(!pSrcSchemeLayerDefine)
		{
			return;
		}
		nLayerCode = pSrcSchemeLayerDefine->GetLayerCode();
		strLayerName = pSrcSchemeLayerDefine->GetLayerName();
	}
	else
	{
		SplitCodeandNameofLayer(nLayerCode,strLayerName,m_DesTree.GetItemText(m_hSelectedTreeItemArray[DES-1]));
	}
	
	if (!m_bMatchedbyLayerName)
	{
		//匹配目标方案的选中节点
		if (!bInvalidItem)
		{
			__int64 nSrcMatchedLayerCode = GetMatchedCode(nLayerCode,m_SrcUserIdx);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_BOLD|TVIS_SELECTED,nSrcMatchedLayerCode,"",NULL);		
		}
		else
		{	
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_BOLD|TVIS_SELECTED,nLayerCode,"",NULL);
			__int64 nDesMatchedLayerCode = GetMatchedCode(nLayerCode,m_DesUserIdx);
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_SELECTED|TVIS_BOLD,nDesMatchedLayerCode,"",NULL);
		}
		
		
	}
	else
	{		
		if (!bInvalidItem)
		{
			CString strSrcMatchedLayerName;
			__int64 nSrcLayerCode;
			strSrcMatchedLayerName = GetMatchedName(strLayerName,m_SrcUserIdx);
			CSchemeLayerDefine *pSrcLayerDefine=NULL;
			pSrcLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(strSrcMatchedLayerName);
			if (pSrcLayerDefine)
			{
				nSrcLayerCode = pSrcLayerDefine->GetLayerCode();
			}
			//AfxMessageBox(_T("更改源方案比例尺，按层名搜索匹配"));
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nSrcLayerCode,strSrcMatchedLayerName,SRC,FALSE);
			
		}
		else{
			//定位到源方案的第一个节点，然后匹配到目标方案。
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nLayerCode,strLayerName,SRC,FALSE);
			CString strDesMatchedLayerName;
			strDesMatchedLayerName = GetMatchedName(strLayerName, m_DesUserIdx);
			CSchemeLayerDefine*pDesLayerDefine = NULL;
			pDesLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesMatchedLayerName);
			__int64 nDesLayerCode;
			if (pDesLayerDefine)
			{
				nDesLayerCode = pDesLayerDefine->GetLayerCode();
			}
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode,strDesMatchedLayerName,DES,FALSE);			
		}
		
	}
	//更改属性值的显示状态。
	if (HasXDefines(m_hSelectedTreeItemArray[SRC-1],m_SrcTree,m_SrcConfig))
	{
		m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCHASVALUE);
	}
	else
	{
		m_strSrcXAttStatusPrompt.LoadString(IDS_EDIT_SRCNOVALUE);
	}
	ModifiedCtrlValue(IDC_EDIT_SRCVALUESTATUS,m_strSrcXAttStatusPrompt);
	//更改拷贝的显示状态。
	ModifiedCtrlValue(IDC_EDIT_CPYSTATUS, "");
	return;
}

void CSchemePropertyCopy::ReLocateNodeofDesTreeView()
{
	//根据源方案中的当前选中项，来定位目标方案中匹配节点。

	__int64 nLayerCode = 0;
	CString strLayerName = "";
	BOOL bInvalidItem = FALSE;
	HTREEITEM hSrcParentItem=NULL;
	if (m_hSelectedTreeItemArray[SRC - 1])
	{
		hSrcParentItem = m_SrcTree.GetParentItem(m_hSelectedTreeItemArray[SRC - 1]);
	}
	
	if ( hSrcParentItem || (m_SrcTree.GetItemText(hSrcParentItem).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0))
	{
		//return;
		bInvalidItem = TRUE;
		m_CurLayerIndex=0;
		CSchemeLayerDefine *pSrcLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(m_CurLayerIndex);
		if(pSrcLayerDefine)
		{
			nLayerCode = pSrcLayerDefine->GetLayerCode();
			strLayerName = pSrcLayerDefine->GetLayerName();
		}
		
	}
	else
	{
		SplitCodeandNameofLayer(nLayerCode,strLayerName,m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]));
	}
	
	
	if (!m_bMatchedbyLayerName)
	{
		__int64 nDesMatchedLayerCode = GetMatchedCode(nLayerCode,m_DesUserIdx);
		if (!bInvalidItem)
		{			
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_SELECTED|TVIS_BOLD,nDesMatchedLayerCode,"",NULL);
		}			
		else
		{	
			//重新找到源方案的第一个节点
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,0,TVIS_SELECTED|TVIS_BOLD,nLayerCode,"",NULL);
			
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,0,TVIS_BOLD|TVIS_SELECTED,nDesMatchedLayerCode,"",NULL);
		}
		
		
	}
	else
	{
		//AfxMessageBox(_T("目标方案已更改,按层名匹配！"));
		__int64 nDesLayerCode;
		CString strDesMatchedLayerName = GetMatchedName(strLayerName,m_DesUserIdx);
		CSchemeLayerDefine *pDesSchemeLayerDefine = NULL;
		pDesSchemeLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesMatchedLayerName);
		if(pDesSchemeLayerDefine)
		{
			nDesLayerCode = pDesSchemeLayerDefine->GetLayerCode();
		}
		if (!bInvalidItem)
		{
			
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode,strDesMatchedLayerName,DES,FALSE);
			
		}
		else
		{
			//重新定位到源方案的第一个节点，然后在目标方案中选择与之匹配的节点。
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItemofCopiedScheme(nLayerCode,strLayerName,SRC,FALSE);
			
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItemofCopiedScheme(nDesLayerCode,strDesMatchedLayerName,DES,FALSE);
			
		}
		
	}
	//更改拷贝的显示状态为空，无需更改目标属性值的显示状态。
	ModifiedCtrlValue(IDC_EDIT_CPYSTATUS,"");
	ModifiedCtrlValue(IDC_EDIT_DESVALUESTATUS,StrFromResID(IDS_CURRENT_MODIFIED));
	return;

}

BOOL CSchemePropertyCopy::ClearSchemeInfo( CString strSchemePath, vector<SchemeNode>& schemeNodeArray )
{
	if ((!strSchemePath)
		||(!schemeNodeArray.size()))
	{
		return FALSE;
	}
	vector<SchemeNode>::iterator iter;
	for (iter = schemeNodeArray.begin();iter!=schemeNodeArray.end(); iter++)
	{
		if(*iter->strSchemePath==strSchemePath)
		{
			schemeNodeArray.erase(iter);
			iter--;
		}
	}
	return TRUE;
}

std::wstring CSchemePropertyCopy::CstrtoWstr( CString str )
{
	std::wstring wstr;
	ConvertCharToWstring(str, wstr, CP_ACP);
	return wstr;
}

long CSchemePropertyCopy::GetCoincidenceNum( wstring wstrSrc, wstring wstrDes )
{
	 int nCoincidence = 0;
	//for test
	int nSrc = wstrSrc.size();
	int nDes = wstrDes.size();
	wchar_t wcharSrc;
	wchar_t wcharDes;
	
	for(int iSrcIndex = 0; iSrcIndex < wstrSrc.size(); iSrcIndex ++)
	{
		for (int iDesIndex = 0; iDesIndex < wstrDes.size(); iDesIndex ++)
		{
			wcharSrc = wstrSrc.at(iSrcIndex);
			wcharDes = wstrDes.at(iDesIndex);
			if (wcharSrc==wcharDes)
			{
				nCoincidence++;
			}
		}
	}

	return nCoincidence;
}

void CSchemePropertyCopy::RemoveSameElems(wstring& wstr )
{
	if (wstr.size()<=0)
	{
		return;
	}
	//{2012.9.14
	wstring wstrTemp;
	int nSize = wstrTemp.size();
	//}
	wchar_t wcharTemp;
	wstrTemp += (wchar_t)wstr.at(0);
	BOOL bSame=FALSE;
	for(int i=0; i<wstr.size(); i++)
	{
		bSame = FALSE;
		for(int j=0; j<wstrTemp.size(); j++)
		{
			if ((wchar_t)wstr.at(i)==(wchar_t)wstrTemp.at(j))
			{
				bSame = TRUE; 	
				continue;
			}
		
		}
		if (!bSame)
		{
			wstrTemp += (wchar_t)wstr.at(i);
		}
	}

	wstr = wstrTemp;
	return;

}

HTREEITEM CSchemePropertyCopy::LocateSpecificTreeItemofCopiedScheme( __int64 nLayerCode, CString strLayerName, UINT nTreeCtrlID, BOOL byCode /*= TRUE*/ )
{
	CMergeTree *pTree = NULL;
	HTREEITEM hItem;
	if(nTreeCtrlID==DES)
	{
		pTree = &m_DesTree;
	}
	else if(nTreeCtrlID==SRC)
	{
		pTree = &m_SrcTree;	
	}

	if(!pTree)
	{
		return NULL;
	}
		
	if (!byCode)
	{
		CString strSrcLayercodeandName;
		CString strSrcLayercode;
		strSrcLayercode.Format("%I64d",nLayerCode);
		strSrcLayercodeandName = strSrcLayercode + " " + strLayerName;		
		hItem = LocateSpecificTreeItem(*pTree, 0, TVIS_BOLD|TVIS_SELECTED,0,strSrcLayercodeandName,NULL,FALSE);	
	}
	else
	{
		hItem = LocateSpecificTreeItem(*pTree, 0, TVIS_BOLD|TVIS_SELECTED,nLayerCode,"",NULL);
	}
	return hItem;
}









void CSchemePropertyCopy::OnButtonDel()
{
	//AfxMessageBox("删除属性");
	if (m_SchemePosID == DES)
	{
		m_bModified = FALSE;
		if (IDYES==MessageBox(StrFromResID(IDS_DEL_SCHEME_ATTRS),StrFromResID(IDS_TIPS),MB_YESNO|MB_ICONQUESTION))
		{
			CTreeItemList curSelectedDesTreeItem;
			m_DesTree.GetSelectedList(curSelectedDesTreeItem);
			CStringArray desSelectedItemsTextArray;
			CSchemeMerge::GetSelectedItemsText(m_DesTree,curSelectedDesTreeItem,desSelectedItemsTextArray);
			
			if (desSelectedItemsTextArray.GetSize()<0)
			{
				m_bModified = FALSE;
				return;
			}
			for(int i=0; i<desSelectedItemsTextArray.GetSize(); i++)
			{
				if(DelProps(m_DesConfig,desSelectedItemsTextArray)&&!m_bModified)
				{
					m_bModified = TRUE;
				}
			}
			
		}
	}
	else
	{
		MessageBox(StrFromResID(IDS_SELECT_LAYORGRP),StrFromResID(IDS_TIPS),MB_OK);
	}
}

BOOL CSchemePropertyCopy::DelProps( ConfigLibItem& config, CStringArray& treeItemTextArr)
{
	BOOL bModified = FALSE;
	int nLayerCount = config.pScheme->GetLayerDefineCount();
	int nTextCount = treeItemTextArr.GetSize();
	if((nLayerCount<=0)||(nTextCount<=0))
	{
		return bModified;
	}
	CStringArray strArrTempGroupName;
	ClearGroupsofLayers(treeItemTextArr,config);
    
	for (int i=0; i<nLayerCount; i++)
	{
		CSchemeLayerDefine *pSchemeLayerDef = config.pScheme->GetLayerDefine(i);
		if (!pSchemeLayerDef)
		{
			continue;
		}
		CString strGroupName = pSchemeLayerDef->GetGroupName();
		CString strLayerName = pSchemeLayerDef->GetLayerName();
		__int64 nLayerCode = pSchemeLayerDef->GetLayerCode();
		CString strLayerItemText;
		CString strLayerCode;
		strLayerCode.Format("%I64d",nLayerCode);
		strLayerItemText = strLayerCode + " " + strLayerName; 
		
		if ((FindinArray(strGroupName, treeItemTextArr)>=0)
			||(FindinArray(strLayerItemText, treeItemTextArr)>=0))
		{
			if (pSchemeLayerDef->DelXDefines()&&(!bModified))
			{
				bModified = TRUE;
			}	
		}

	}
	return bModified;
}

void CSchemePropertyCopy::RemoveSpecificElems( wstring& wstr, wstring wstrInvalidChars )
{


	int nInvalidCharsNum = wstrInvalidChars.size();
	int nSize = wstr.size();
	if (nInvalidCharsNum<=0)
	{
		return;
	}
	wstring::iterator iter;
	for(iter = wstr.begin(); iter != wstr.end();iter++ )
	{
		if (FindinWstr(wstrInvalidChars,*iter)>=0)
		{
			wstr.erase(iter);
			iter--;
		}
			
	}
	nSize = wstr.size();
	return;

}

int CSchemePropertyCopy::FindinWstr( wstring wstr, wchar_t desWchar )
{
	int nIndex = -1;
	for (int i=0; i<wstr.size(); i++)
	{
	    if ((wchar_t)wstr[i]==desWchar)
	    {
			nIndex = i;
			break;
	    }
	}
	return nIndex;
}

wchar_t* CSchemePropertyCopy::AnsiToUnicode( const char* szStr )
{
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
	return pResult;
}

void CSchemePropertyCopy::EnableBtnCtrl( BOOL bEnable )
{
	m_wndButtonCopy.EnableWindow(bEnable);
	m_wndButtonSkip.EnableWindow(bEnable);
	m_pwndButtonDel->EnableWindow(bEnable);
	return;
}

void CSchemePropertyCopy::ClearGroupsofLayers(CStringArray& strGroupsandLayers, ConfigLibItem config)
{
	CString strLayerName;
	CString strGroupName;
	__int64 nLayerCode;
	CString strLayerCode;
	CString strLayerItemTxt;
    for(int i=0; i<config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine* pSchemeLayer = config.pScheme->GetLayerDefine(i);
		if (!pSchemeLayer)
		{
			continue;
		}
		strLayerName = pSchemeLayer->GetLayerName();
		strGroupName = pSchemeLayer->GetGroupName();
		nLayerCode = pSchemeLayer->GetLayerCode();
		strLayerCode.Format("%I64d",nLayerCode);
		strLayerItemTxt = strLayerCode + " " + strLayerName;

		long nLayerId = -1;
		long nGroupId = -1;
		if(((nLayerId=FindinArray(strLayerItemTxt,strGroupsandLayers))>=0)
			&&((nGroupId=FindinArray(strGroupName,strGroupsandLayers))>=0))
		{
			strGroupsandLayers.RemoveAt(nGroupId);
		}

	}
	return;
}
