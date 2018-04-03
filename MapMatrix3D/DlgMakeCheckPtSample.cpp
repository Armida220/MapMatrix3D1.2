// DlgMakeCheckPtSample.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "DlgMakeCheckPtSample.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static CDlgMakeCheckPtSample::TempSaveData sData;

/////////////////////////////////////////////////////////////////////////////
// CDlgMakeCheckPtSample dialog

#define REGPATH_MAKESAMPLE			"MakeSample"

CDlgMakeCheckPtSample::CDlgMakeCheckPtSample(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMakeCheckPtSample::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMakeCheckPtSample)
	m_nCrossWidth = 0;
	m_strImageID = _T("");
	m_nImageWidth = 511;
	m_nImageHigh = 511;
	m_nFontSize = 25;
	m_strNoteText = _T("");
	m_strImagePath = _T("C:\\");
	m_strPtID = _T("0000");
	m_clrFont = RGB(255,0,0);
	m_clrCross = RGB(255,0,0);
	m_nImgSoure = 0;
	m_nCrossWidth = 11;
	m_strFontName = "黑体";
	//}}AFX_DATA_INIT

	if(sData.bInited)
	{
		m_nCrossWidth = sData.m_nCrossWidth;
		m_strImageID = sData.m_strImageID;
		m_nImageWidth = sData.m_nImageWidth;
		m_nImageHigh = sData.m_nImageHigh;
		m_nFontSize = sData.m_nFontSize;
		m_strNoteText = sData.m_strNoteText;
		m_strImagePath = sData.m_strImagePath;
		m_strPtID = sData.m_strPtID;
		m_clrFont = sData.m_clrFont;
		m_clrCross = sData.m_clrCross;
		
	}
	else
	{
		sData.m_nCrossWidth = m_nCrossWidth;
		sData.m_strImageID = m_strImageID;
		sData.m_nImageWidth = m_nImageWidth;
		sData.m_nImageHigh = m_nImageHigh;
		sData.m_nFontSize = m_nFontSize;
		sData.m_strNoteText = m_strNoteText;
		sData.m_strImagePath = m_strImagePath;
		sData.m_strPtID = m_strPtID;
		sData.m_clrFont = m_clrFont;
		sData.m_clrCross = m_clrCross;

		sData.bInited = TRUE;
	}

	m_nFontSize = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE,"FontSize",m_nFontSize);
	m_clrFont = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE,"FontColor",m_clrFont);
	m_nCrossWidth = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE,"CrossWidth",m_nCrossWidth);
	m_nImageWidth = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE,"ImageWidth",m_nImageWidth);
	m_nImageHigh = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE, "ImageHigh", m_nImageHigh);
	m_clrCross = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE,"CrossColor",m_clrCross);
	m_strImagePath = AfxGetApp()->GetProfileString(REGPATH_MAKESAMPLE,"ImagePath",m_strImagePath);
	m_strFontName = AfxGetApp()->GetProfileString(REGPATH_MAKESAMPLE,"FontName",m_strFontName);
	m_strNoteText = AfxGetApp()->GetProfileString(REGPATH_MAKESAMPLE,"NoteText",m_strNoteText);
}


void CDlgMakeCheckPtSample::IncreasePtID()
{
	if(sData.bInited)
	{
		int nLen = m_strPtID.GetLength();
		if(nLen<=0)
		{
			m_strPtID = "0001";
			return;
		}
		
		char text[1000] = {0};
		strcpy(text,m_strPtID);
		char *pos = text + nLen-1;
		while(pos>=text)
		{
			if(*pos>='0' && *pos<='9')
			{	
				pos--;
			}
			else
			{
				break;
			}
		}


		if(pos<text)
		{
			CString strFormat;
			strFormat.Format("%%0%dd",nLen);

			long value = atol(text);
			m_strPtID.Format(strFormat,value+1);
		}
		else if(pos<text + nLen-1 )
		{
			nLen = strlen(pos+1);
			
			CString strFormat;
			strFormat.Format("%%0%dd",nLen);

			long value = atol(pos+1);
			CString t;
			t.Format(strFormat,value+1);
			m_strPtID = m_strPtID.Left(pos+1-text) + t;		
		}
		else
		{
			m_strPtID = m_strPtID + "1";
		}
	}
}


void CDlgMakeCheckPtSample::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMakeCheckPtSample)
	DDX_Text(pDX, IDC_EDIT_CROSSWIDTH, m_nCrossWidth);
	DDX_Text(pDX, IDC_EDIT_IMAGENUMBER, m_strImageID);
	DDX_Text(pDX, IDC_EDIT_IMAGEWIDTH, m_nImageWidth);
	DDX_Text(pDX, IDC_EDIT_IMAGEHIGHT, m_nImageHigh);
	DDX_Text(pDX, IDC_EDIT_NOTEFONTSIZE, m_nFontSize);
	DDX_Text(pDX, IDC_EDIT_NOTETEXT, m_strNoteText);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strImagePath);
	DDX_Text(pDX, IDC_EDIT_PTNUMBER, m_strPtID);
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_nImgSoure);
	DDX_Control(pDX, IDC_COMBO_NOTEFONT, m_wndComboFont);
	DDX_Control(pDX, IDC_BUTTON_CROSSCOLOR, m_wndCrossColor);
	DDX_Control(pDX, IDC_BUTTON_NOTECOLOR, m_wndFontColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMakeCheckPtSample, CDialog)
	//{{AFX_MSG_MAP(CDlgMakeCheckPtSample)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] 窗口标题
						   LPTSTR lpszPath,		// [out] 返回的文件路径
						   LPCTSTR lpszInitDir,	// [in] 初始文件路径
						   HWND hWnd,				// [in] 父系窗口
						   BOOL bNetwork=FALSE,	// [in] 限制在网络路径范围内
						   UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_EDITBOX |BIF_BROWSEINCLUDEFILES );// [in] 设置风格


extern void GetAllFontNames(CStringArray& arr);

/////////////////////////////////////////////////////////////////////////////
// CDlgMakeCheckPtSample message handlers

void CDlgMakeCheckPtSample::OnButtonBrowse() 
{
	CString retPath;
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),retPath.GetBuffer(256),NULL,GetSafeHwnd()) )
		return;
	
	UpdateData(TRUE);
	m_strImagePath = retPath;
	UpdateData(FALSE);	
}


BOOL CDlgMakeCheckPtSample::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CStringArray arrFonts;
	GetAllFontNames(arrFonts);

	for(int i=0; i<arrFonts.GetSize(); i++)
	{
		m_wndComboFont.AddString(arrFonts[i]);
	}

	m_wndComboFont.SelectString(0,m_strFontName);

	m_wndCrossColor.SetColor(m_clrCross);
	m_wndFontColor.SetColor(m_clrFont);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMakeCheckPtSample::OnOK() 
{
	UpdateData(TRUE);

	m_clrCross = m_wndCrossColor.GetColor();
	m_clrFont = m_wndFontColor.GetColor();
	int index = m_wndComboFont.GetCurSel();
	if (index>=0)m_wndComboFont.GetLBText(index,m_strFontName);

	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"FontSize",m_nFontSize);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"FontColor",m_clrFont);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"CrossWidth",m_nCrossWidth);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"ImageWidth",m_nImageWidth);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE, "ImageHigth", m_nImageHigh);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"CrossColor",m_clrCross);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"ImagePath",m_strImagePath);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"FontName",m_strFontName);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"NoteText",m_strNoteText);

	sData.m_nCrossWidth = m_nCrossWidth;
	sData.m_strImageID = m_strImageID;
	sData.m_nImageWidth = m_nImageWidth;
	sData.m_nImageHigh = m_nImageHigh;
	sData.m_nFontSize = m_nFontSize;
	sData.m_strNoteText = m_strNoteText;
	sData.m_strImagePath = m_strImagePath;
	sData.m_strPtID = m_strPtID;
	sData.m_clrFont = m_clrFont;
	sData.m_clrCross = m_clrCross;

	m_clrCross = m_wndCrossColor.GetColor();
	m_clrFont = m_wndFontColor.GetColor();

	int nsel = m_wndComboFont.GetCurSel();
	if(nsel>=0)
	{
		m_wndComboFont.GetLBText(nsel,m_strFontName);
	}
	
	CDialog::OnOK();
}
