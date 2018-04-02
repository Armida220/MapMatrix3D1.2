// OptionsDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "FeatureCollector.h"
//#include "CTView.h"
#include "OptionsDlg.h"
#include "SmartViewFunctions.h"
#include "ExMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int idTree = 101;
/////////////////////////////////////////////////////////////////////////////
// COptionsDlg

IMPLEMENT_DYNAMIC(COptionsDlg, CMFCPropertySheet)

//##ModelId=41466B7C030D
COptionsDlg::COptionsDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
:CMFCPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	SetLook(CMFCPropertySheet::PropSheetLook_Tree, 150 /* Tree control width */);
	SetIconsList (IDB_OPTIONSIMAGES, 16 /* Image width */);

	CMFCPropertySheetCategoryInfo* pCat1 = AddTreeCategory(StrFromResID(IDS_OPTION_UI), 0, 1);

	AddPageToTree (pCat1, &m_Ui_Commonly, -1, 2);
	//AddPageToTree (pCat1, &m_Ui_SaveOptions, -1, 2);

	CMFCPropertySheetCategoryInfo* pCat2 = AddTreeCategory(StrFromResID(IDS_OPTION_VIEW), 0, 1);

	//AddPageToTree (pCat2, &m_View_Commonly, -1, 2);
	//AddPageToTree (pCat2, &m_View_Measure, -1, 2);
	AddPageToTree (pCat2, &m_View_VectView, -1, 2);
	AddPageToTree (pCat2, &m_View_ImageView, -1, 2);

	CMFCPropertySheetCategoryInfo* pCat3 = AddTreeCategory(StrFromResID(IDS_OPTION_DOC), 0, 1);

	AddPageToTree (pCat3, &m_Precision, -1, 2);
	AddPageToTree (pCat3, &m_Doc_Snap, -1, 2);
	//AddPageToTree (pCat3, &m_Doc_Alert, -1, 2);
	//AddPageToTree (pCat3, &m_Doc_Paint, -1, 2);
	AddPageToTree (pCat3, &m_Doc_Select, -1, 2);
	AddPageToTree (pCat3, &m_Doc_Path, -1, 2);
	AddPageToTree (pCat3, &m_Doc_Save, -1, 2);
	AddPageToTree (pCat3, &m_Doc_User, -1, 2);
	AddPageToTree(pCat3, &m_Page_NetWork, -1, 2);
// 	CUIFPropSheetCategory* pCat4 = AddTreeCategory (StrFromResID(IDS_QT_GRAPHIC_CHECK),0,1);
// 	AddPageToTree (pCat4, &m_QT_Param1, -1, 2);	
// 	AddPageToTree (pCat4, &m_QT_Param2, -1, 2);
// 	AddPageToTree (pCat4, &m_QT_Param3, -1, 2);
	
/*
	// 创建注册登记表项
	CFeatureCollectorApp *pApp = (CFeatureCollectorApp*)AfxGetApp();
	CString strRegSect = pApp->GetRegSectionPath();

	CUIFRegistrySP regSP;
	CUIFRegistry& reg = regSP.Create (FALSE, FALSE);

	if (!reg.Open (strRegSect + strOptsRegEntryName))
	{
		if (!reg.CreateKey (strRegSect + strOptsRegEntryName))
		{
			AfxMessageBox("创建注册登记表失败!");
		}
	}*/
}

//##ModelId=41466B7C0369
COptionsDlg::~COptionsDlg()
{
}


BEGIN_MESSAGE_MAP(COptionsDlg, CMFCPropertySheet)
	//{{AFX_MSG_MAP(COptionsDlg)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGEDW, idTree, OnSelectTree)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

INT_PTR COptionsDlg::DoModal()
{
	INT_PTR ret = CMFCPropertySheet::DoModal();

	AfxGetMainWnd()->SendMessage(FCCM_OPTIONUPDATE);
	return ret;
}

BOOL COptionsDlg::OnInitDialog() 
{
	BOOL bResult = CMFCPropertySheet::OnInitDialog();
	m_wndTree.Expand(m_wndTree.GetRootItem(),TVE_COLLAPSE);
	POSITION pos = m_lstTreeCategories.GetHeadPosition();
	CMFCPropertySheetCategoryInfo* pCate = NULL;
	for (int i=0;i < m_lstTreeCategories.GetCount();i++)
	{
		pCate = m_lstTreeCategories.GetNext(pos);
		m_wndTree.Expand (pCate->m_hTreeItem, TVE_EXPAND);
	}
	return bResult;
}

void COptionsDlg::OnSelectTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CMFCPropertySheet::OnSelectTree(NULL, pResult);

	POSITION pos = m_lstTreeCategories.GetHeadPosition();
	CMFCPropertySheetCategoryInfo* pCate = NULL;
	for (int i = 0; i < m_lstTreeCategories.GetCount(); i++)
	{
		pCate = m_lstTreeCategories.GetNext(pos);
		m_wndTree.Expand(pCate->m_hTreeItem, TVE_EXPAND);
	}
}