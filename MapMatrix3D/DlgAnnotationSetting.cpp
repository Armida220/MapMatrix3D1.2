// DlgAnnotationSetting.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgAnnotationSetting.h"
#include "ExMessage.h"
#include "SmartViewFunctions.h"
#include "SilenceDDX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgAnnotationSetting dialog


CDlgAnnotationSetting::CDlgAnnotationSetting(CWnd* pParent /*=NULL*/)
	: CSonDialog(CDlgAnnotationSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgAnnotationSetting)
	m_nAnnoType = 0;
	m_lfCharHei = 0.0;
	m_lfCharWidScale = 1.0;
	m_lfCharIntvScale = 0.0;
	m_nDigit = 0;
	m_lfInclineAngle = 0.0;
	m_lfLineSpaceScale = 1.0;
	m_fXOff = 0.0f;
	m_fYOff = 0.0f;
	m_nPlaceType = 0;
	m_fExtendDis = 0.0f;
	//}}AFX_DATA_INIT
	m_pAnno = NULL;
}


void CDlgAnnotationSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAnnotationSetting)
	DDX_Control(pDX, IDC_FIELD_COMBO, m_cFieldCombo);
	DDX_Control(pDX, IDC_FONTNAME_COMBO, m_cFontName);
	DDX_Control(pDX, IDC_INCLINETYPE_COMBO, m_cInclineType);
	DDX_Control(pDX, IDC_TEXTALIGNTYPE_COMBO, m_cTextAligntype);
	DDX_Control(pDX, IDC_ANNOTYPE_COMBO, m_cAnnoType);
	DDX_Control(pDX, IDC_PLACETYPE_COMBO, m_cPlaceType);
	DDX_Text_Silence(pDX, IDC_CHARHEI_EDIT, m_lfCharHei);
	DDX_Text_Silence(pDX, IDC_CHARWID_EDIT, m_lfCharWidScale);
	DDX_Text_Silence(pDX, IDC_CHARINTV_EDIT, m_lfCharIntvScale);
	DDX_Text_Silence(pDX, IDC_DIGIT_EDIT, m_nDigit);
	DDX_Text_Silence(pDX, IDC_INCLINEANGLE_EDIT, m_lfInclineAngle);
	DDX_Text_Silence(pDX, IDC_LINEINTV_EDIT, m_lfLineSpaceScale);
	DDX_Text_Silence(pDX, IDC_XOFF_EDIT, m_fXOff);
	DDX_Text_Silence(pDX, IDC_YOFF_EDIT, m_fYOff);
	DDX_Text(pDX, IDC_TEXT_EDIT, m_strText);
	DDX_Control(pDX, IDC_COVERTYPE_COMBO, m_cCoverType);
	DDX_Text_Silence(pDX, IDC_EXTENDDIS_EDIT, m_fExtendDis);
	//}}AFX_DATA_MAP
}

int CALLBACK EnumFontFamProc(LPENUMLOGFONT lpelf, LPNEWTEXTMETRIC lpntm, DWORD nFontType, LONG_PTR lparam);


BOOL CDlgAnnotationSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_cAnnoType.AddString(StrFromResID(IDS_ANNO_TEXT));
	m_cAnnoType.AddString(StrFromResID(IDS_ANNO_HEIGHT));
	m_cAnnoType.AddString(StrFromResID(IDS_ANNO_DHEIGHT));
	m_cAnnoType.AddString(StrFromResID(IDS_ANNO_ATTRIBUTION));
	m_cAnnoType.AddString(StrFromResID(IDS_ANNO_LAYERCONFIG));
	m_cAnnoType.AddString(StrFromResID(IDS_SELCOND_LEN));
	m_cAnnoType.AddString(StrFromResID(IDS_SELCOND_AREA));

	for (int i=0; i<7; i++)
	{
		m_cAnnoType.SetItemData(i,i);
	}

	for (i=0; i<m_cAnnoType.GetCount(); i++)
	{
		if (m_cAnnoType.GetItemData(i) == m_nAnnoType)
		{
			m_cAnnoType.SetCurSel(i);
			break;
		}
	}

	// 如果是属性注记
	UpdataFieldComobo();

	for (i=0; i<9; i++)
	{
		m_cPlaceType.AddString(StrFromResID(IDS_FIRST_POINT+i));
		m_cPlaceType.SetItemData(i,i);
	}
	m_cPlaceType.AddString(StrFromResID(IDS_CLOSEFTR_CENTER_POINT));
	m_cPlaceType.SetItemData(9,9);
	for (i=0; i<m_cPlaceType.GetCount(); i++)
	{
		if (m_cPlaceType.GetItemData(i) == m_nPlaceType)
		{
			m_cPlaceType.SetCurSel(i);
			break;
		}
	}
	m_cInclineType.AddString(StrFromResID(IDS_CMDPLANE_SHRUGN));
	m_cInclineType.AddString(StrFromResID(IDS_CMDPLANE_SHRUGL));
	m_cInclineType.AddString(StrFromResID(IDS_CMDPLANE_SHRUGR));
	m_cInclineType.AddString(StrFromResID(IDS_CMDPLANE_SHRUGU));
	m_cInclineType.AddString(StrFromResID(IDS_CMDPLANE_SHRUGD));
	m_cInclineType.SetCurSel(m_pAnno->m_textSettings.nInclineType);


	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNLT));
	m_cTextAligntype.SetItemData(0,TAH_LEFT|TAV_TOP);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNMT));
	m_cTextAligntype.SetItemData(1,TAH_MID|TAV_TOP);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNRT));
	m_cTextAligntype.SetItemData(2,TAH_RIGHT|TAV_TOP);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNLM));
	m_cTextAligntype.SetItemData(3,TAH_LEFT|TAV_MID);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNMM));
	m_cTextAligntype.SetItemData(4,TAH_MID|TAV_MID);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNRM));
	m_cTextAligntype.SetItemData(5,TAH_RIGHT|TAV_MID);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNLB));
	m_cTextAligntype.SetItemData(6,TAH_LEFT|TAV_BOTTOM);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNMB));
	m_cTextAligntype.SetItemData(7,TAH_MID|TAV_BOTTOM);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGNRB));
	m_cTextAligntype.SetItemData(8,TAH_RIGHT|TAV_BOTTOM);
	m_cTextAligntype.AddString(StrFromResID(IDS_TEXTALIGN_DOT));
	m_cTextAligntype.SetItemData(9,TAH_DOT);
	switch(m_pAnno->m_textSettings.nAlignment)
	{
	case TAH_LEFT:
	case TAH_LEFT|TAV_TOP:
		m_cTextAligntype.SetCurSel(0);					
		break;
	case TAH_MID:
	case TAH_MID|TAV_TOP:
		m_cTextAligntype.SetCurSel(1);
		break;
	case TAH_RIGHT:
	case TAH_RIGHT|TAV_TOP:
		m_cTextAligntype.SetCurSel(2);
		break;
	case TAH_LEFT|TAV_MID:
	case TAV_MID:
		m_cTextAligntype.SetCurSel(3);
		break;
	case TAH_MID|TAV_MID:
		m_cTextAligntype.SetCurSel(4);
		break;
	case TAH_RIGHT|TAV_MID:	
		m_cTextAligntype.SetCurSel(5);
		break;
	case TAH_LEFT|TAV_BOTTOM:
	case TAV_BOTTOM:
		m_cTextAligntype.SetCurSel(6);
		break;
	case TAH_MID|TAV_BOTTOM:
		m_cTextAligntype.SetCurSel(7);
		break;
	case TAH_RIGHT|TAV_BOTTOM:
		m_cTextAligntype.SetCurSel(8);
		break;
	case TAH_DOT:
		m_cTextAligntype.SetCurSel(9);
		break;
	default:
		m_cTextAligntype.SetCurSel(0);
		break;
	}

	LOGFONT logfont;
    logfont.lfCharSet = DEFAULT_CHARSET;
    strcpy(logfont.lfFaceName,"");
	HDC hDC = ::CreateCompatibleDC(NULL);
		
	::EnumFontFamiliesEx(hDC,&logfont,
		(FONTENUMPROC)EnumFontFamProc,(LPARAM)&m_arrFont,0);
	::DeleteDC(hDC);


	for (i=0;i<m_arrFont.GetSize();i++)
	{		
		m_cFontName.AddString(m_arrFont[i]);		
	}
	m_cFontName.SelectString(-1,m_pAnno->m_textSettings.tcFaceName);
	
	m_cCoverType.AddString(StrFromResID(IDS_COVERTYPE_NONE));
	m_cCoverType.AddString(StrFromResID(IDS_COVERTYPE_RECT));
	m_cCoverType.AddString(StrFromResID(IDS_COVERTYPE_CIRCLE));
	m_cCoverType.SetCurSel(m_pAnno->m_nCoverType);

	return TRUE;
}
BEGIN_MESSAGE_MAP(CDlgAnnotationSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgAnnotationSetting)
	ON_EN_CHANGE(IDC_CHARHEI_EDIT, OnChangeCharheiEdit)
	ON_EN_CHANGE(IDC_CHARINTV_EDIT, OnChangeCharintvEdit)
	ON_EN_CHANGE(IDC_CHARWID_EDIT, OnChangeCharwidEdit)
	ON_EN_CHANGE(IDC_DIGIT_EDIT, OnChangeDigitEdit)
	ON_EN_CHANGE(IDC_INCLINEANGLE_EDIT, OnChangeInclineangleEdit)
	ON_EN_CHANGE(IDC_LINEINTV_EDIT, OnChangeLineintvEdit)
	ON_EN_CHANGE(IDC_TEXT_EDIT, OnChangeTextEdit)
	ON_EN_CHANGE(IDC_XOFF_EDIT, OnChangeXoffEdit)
	ON_EN_CHANGE(IDC_YOFF_EDIT, OnChangeYoffEdit)
	ON_CBN_SELCHANGE(IDC_PLACETYPE_COMBO, OnSelchangePlacetypeCombo)
	ON_CBN_SELCHANGE(IDC_ANNOTYPE_COMBO, OnSelchangeAnnotypeCombo)
	ON_CBN_SELCHANGE(IDC_TEXTALIGNTYPE_COMBO, OnSelchangeTextAligntypeCombo)
	ON_CBN_SELCHANGE(IDC_INCLINETYPE_COMBO, OnSelchangeInclinetypeCombo)
	ON_CBN_SELCHANGE(IDC_FONTNAME_COMBO, OnSelchangeFontNameCombo)
	ON_CBN_SELCHANGE(IDC_FIELD_COMBO, OnSelchangeFieldCombo)
	ON_CBN_SELCHANGE(IDC_COVERTYPE_COMBO, OnSelchangeCovertypeCombo)
	ON_EN_CHANGE(IDC_EXTENDDIS_EDIT, OnChangeExtendDisEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgAnnotationSetting message handlers

void CDlgAnnotationSetting::SetMem(CAnnotation *pAnno, CValueTable &tab)
{

	m_pAnno = pAnno;
	
	if(m_pAnno == NULL)
		return;
	
	m_fXOff = m_pAnno->m_fXOff;
	m_fYOff = m_pAnno->m_fYOff;
	m_lfCharHei = m_pAnno->m_textSettings.fHeight;
	m_lfCharIntvScale = m_pAnno->m_textSettings.fCharIntervalScale;
	m_lfCharWidScale = m_pAnno->m_textSettings.fWidScale;
	m_lfInclineAngle = m_pAnno->m_textSettings.fInclineAngle;
	m_lfLineSpaceScale = m_pAnno->m_textSettings.fLineSpacingScale;
	m_nAnnoType = m_pAnno->m_nAnnoType;
	m_nDigit = m_pAnno->m_nDigit;
	m_nPlaceType = m_pAnno->m_nPlaceType;
	m_strText = m_pAnno->m_strText;
	m_strField = m_pAnno->m_strField;
	m_fExtendDis = m_pAnno->m_fExtendDis;

	m_annTab.CopyFrom(tab);
	
}

void CDlgAnnotationSetting::OnChangeCharheiEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.fHeight = m_lfCharHei;
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgAnnotationSetting::OnChangeCharintvEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.fCharIntervalScale = m_lfCharIntvScale;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgAnnotationSetting::OnChangeCharwidEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.fWidScale = m_lfCharWidScale;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgAnnotationSetting::OnChangeDigitEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_nDigit = m_nDigit;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}



void CDlgAnnotationSetting::OnChangeInclineangleEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.fInclineAngle = m_lfInclineAngle;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}



void CDlgAnnotationSetting::OnChangeLineintvEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.fLineSpacingScale = m_lfLineSpaceScale;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgAnnotationSetting::OnChangeTextEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	strcpy(m_pAnno->m_strText,m_strText);
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgAnnotationSetting::OnChangeXoffEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_fXOff = m_fXOff;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgAnnotationSetting::OnChangeYoffEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_fYOff = m_fYOff;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}


void CDlgAnnotationSetting::OnSelchangePlacetypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_nPlaceType = m_cPlaceType.GetItemData(m_cPlaceType.GetCurSel());
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAnnotationSetting::UpdataFieldComobo()
{
	// 如果是属性注记
	if (m_nAnnoType == CAnnotation::AnnoType::Attribute)
	{
		CWnd *pWnd = GetDlgItem(IDC_TEXT_EDIT);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_HIDE);
		}
		m_cFieldCombo.ShowWindow(SW_SHOW);
		
		m_cFieldCombo.ResetContent();
		
		CString field, name, data;
		int type;
		const CVariantEx *var;
		CString strSelFiled = "";
		for (int i=0;i<m_annTab.GetFieldCount();i++)
		{		
			m_annTab.GetField(i,field,type,name);
			if (!m_annTab.GetValue(0,i,var))
			{
				continue;
			}					
			m_cFieldCombo.AddString(name);
			m_cFieldCombo.SetItemData(i,i);
			if (m_strField.CompareNoCase(field) == 0)
			{
				strSelFiled = name;
			}
		}
		if (strSelFiled.IsEmpty())
		{
			m_annTab.GetField(0,field,type,name);
			strSelFiled = name;
		}
		m_cFieldCombo.SelectString(-1,strSelFiled);
	}
	else
	{
		m_cFieldCombo.ShowWindow(SW_HIDE);
		
		CWnd *pWnd = GetDlgItem(IDC_TEXT_EDIT);
		if (pWnd)
		{
			pWnd->ShowWindow(SW_SHOW);
			
			if (m_nAnnoType == CAnnotation::AnnoType::Text)
			{
				pWnd->EnableWindow(TRUE);
			}
			else
			{
				m_strText = "";
				pWnd->EnableWindow(FALSE);
			}
			
		}
		
		
	}
}

void CDlgAnnotationSetting::OnSelchangeAnnotypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_nAnnoType = m_nAnnoType = m_cAnnoType.GetItemData(m_cAnnoType.GetCurSel());

	UpdataFieldComobo();	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAnnotationSetting::OnSelchangeTextAligntypeCombo()
{
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.nAlignment = m_cTextAligntype.GetItemData(m_cTextAligntype.GetCurSel());
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAnnotationSetting::OnSelchangeInclinetypeCombo()
{
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_textSettings.nInclineType = m_cInclineType.GetCurSel();
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAnnotationSetting::OnSelchangeFontNameCombo()
{
	if(m_pAnno == NULL)
		return;
	
	int iSel =  m_cFontName.GetCurSel();
	if (iSel==-1) return;
	m_cFontName.GetLBText(iSel,m_strFontName);
	
	_tcscpy(m_pAnno->m_textSettings.tcFaceName,m_strFontName);

	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAnnotationSetting::OnSelchangeFieldCombo()
{
	if(m_pAnno == NULL)
		return;
	
	int iSel =  m_cFieldCombo.GetCurSel();
	int nField = m_cFieldCombo.GetItemData(iSel);

	CString field, name;
	int type;
	CVariantEx *var;
	m_annTab.GetField(nField,field,type,name);
	m_strField = field;	
	strcpy(m_pAnno->m_strField,m_strField);
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}

void CDlgAnnotationSetting::OnChangeExtendDisEdit() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_fExtendDis = m_fExtendDis;
	
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
	
}

void CDlgAnnotationSetting::OnSelchangeCovertypeCombo() 
{
	// TODO: Add your control notification handler code here
	if(m_pAnno == NULL)
		return;
	
	m_pAnno->m_nCoverType = m_cCoverType.GetCurSel();
	
	CWnd *pWnd = GetParentOwner();
	pWnd->SendMessage(WM_SYMBOL_REPAINT);
}