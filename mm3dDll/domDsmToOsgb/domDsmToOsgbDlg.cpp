
// domDsmToOsgbDlg.cpp : implementation file
//

#include "stdafx.h"
#include "domDsmToOsgb.h"
#include "domDsmToOsgbDlg.h"
#include "afxdialogex.h"
#include "dataGenerate.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


inline CString  LoadString(UINT uid)
{
	CString str;
	str.LoadString(uid);
	return str;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

int SplitString(const CString str, char split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if (iIndex >= 0)
		{
			CString _str = strTemp.Left(iIndex);
			if (_str.GetLength())
				strArray.Add(_str);
			strTemp = strTemp.Right(strTemp.GetLength() - iIndex - 1);
		}
		else
		{
			break;
		}
	}
	if (!strTemp.IsEmpty())
		strArray.Add(strTemp);

	return strArray.GetSize();
}
void msgfun(const char * t, void * p)
{
	if (p == nullptr)
		return;
	CEdit * pe = (CEdit*)p;
	CStringArray ca;
	SplitString(t, '\n', ca);
	if (ca.IsEmpty()) return;
	CString str = ca.GetAt(ca.GetCount() - 1);
	pe->SetWindowText(str);
}


CdomDsmToOsgbDlg::CdomDsmToOsgbDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdomDsmToOsgbDlg::IDD, pParent)
	, m_domPath(_T(""))
	, m_dsmPath(_T(""))
	, m_Xnum(_T("5"))
	//, m_Ynum(_T("1"))
	, m_osgPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_redirect.set_callback_fun(msgfun, &m_messageControl);
}

void CdomDsmToOsgbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DOMPATH, m_domPath);
	DDX_Text(pDX, IDC_STATIC_DSMPATH, m_dsmPath);
	DDX_Text(pDX, IDC_EDIT1, m_Xnum);
	//DDX_Text(pDX, IDC_EDIT2, m_Ynum);
	//  DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_EXIT);
	DDX_Text(pDX, IDC_STATIC_OSGPATH, m_osgPath);
	DDX_Control(pDX, IDC_EDIT2, m_messageControl);
}

BEGIN_MESSAGE_MAP(CdomDsmToOsgbDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DOM, &CdomDsmToOsgbDlg::OnBnClickedButtonDom)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CdomDsmToOsgbDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_DSM, &CdomDsmToOsgbDlg::OnBnClickedButtonDsm)
	ON_BN_CLICKED(IDOK, &CdomDsmToOsgbDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_OSG, &CdomDsmToOsgbDlg::OnClickedButtonOsg)
END_MESSAGE_MAP()


// CdomDsmToOsgbDlg message handlers

BOOL CdomDsmToOsgbDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetBackgroundColor(RGB(80,80,80));
	
	HBITMAP   hBitmap;
	hBitmap = (HBITMAP)::LoadImage(
		NULL,
		_T("quit.bmp"),                           // 图片全路径  
		IMAGE_BITMAP,                          // 图片格式  
		0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);  // 注意LR_LOADFROMFILE  
	m_EXIT.SetBitmap(hBitmap);

	m_font.CreateFont(
		16,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("宋体"));
	m_font1.CreateFont(
		11,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		_T("黑体"));
	CStatic* s1 = (CStatic*)GetDlgItem(IDC_STATIC_DOMPATH);
	CStatic* s2 = (CStatic*)GetDlgItem(IDC_STATIC_DSMPATH);
	CStatic* s3 = (CStatic*)GetDlgItem(IDC_STATIC_TITLE);
	CStatic* s4 = (CStatic*)GetDlgItem(IDC_STATIC_OSGPATH);
	s1->SetFont(&m_font1);
	s2->SetFont(&m_font1);
	s3->SetFont(&m_font);
	s4->SetFont(&m_font1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CdomDsmToOsgbDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CdomDsmToOsgbDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CdomDsmToOsgbDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CdomDsmToOsgbDlg::OnBnClickedButtonDom()
{
	// TODO: Add your control notification handler code here
	CString filter;
	filter = "dom(*.tif)|*.tif||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{	
		m_domPath = dlg.GetPathName();
	}
	else
	{
		m_domPath = _T("");
	}
	UpdateData(false);
}


void CdomDsmToOsgbDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl == IDOK || nIDCtl == IDCANCEL || nIDCtl == IDC_BUTTON_DOM || nIDCtl == IDC_BUTTON_DSM || nIDCtl == IDC_BUTTON_OSG)         //checking for the button 
	{
		CDC dc;
		RECT rect;
		dc.Attach(lpDrawItemStruct->hDC);   // Get the Button DC to CDC

		rect = lpDrawItemStruct->rcItem;     //Store the Button rect to our local rect.

		dc.Draw3dRect(&rect, RGB(0, 0, 0), RGB(0, 0, 0));

		dc.FillSolidRect(&rect, RGB(50, 50, 50));//Here you can define the required color to appear on the Button.

		UINT state = lpDrawItemStruct->itemState; //This defines the state of the Push button either pressed or not. 

		if ((state & ODS_SELECTED))
		{
			dc.DrawEdge(&rect, EDGE_SUNKEN, BF_RECT);

		}
		else
		{
			dc.DrawEdge(&rect, EDGE_RAISED, BF_RECT);
		}

		dc.SetBkColor(RGB(50, 50, 50));   //Setting the Text Background color
		dc.SetTextColor(RGB(255, 255, 255));     //Setting the Text Color


		TCHAR buffer[MAX_PATH];           //To store the Caption of the button.
		ZeroMemory(buffer, MAX_PATH);     //Intializing the buffer to zero
		::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //Get the Caption of Button Window 

		dc.DrawText(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//Redraw the Caption of Button Window 

		dc.Detach(); // Detach the Button DC
	}
	
	else
		CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


HBRUSH CdomDsmToOsgbDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC )
	{
		
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(80, 80, 80));
		pDC->SetTextColor(RGB(250, 250, 250));
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CdomDsmToOsgbDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}


void CdomDsmToOsgbDlg::OnBnClickedButtonDsm()
{
	// TODO: Add your control notification handler code here
	CString filter;
	filter = "dem(*.tif)|*.tif||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		
		m_dsmPath = dlg.GetPathName();
	}
	else
	{
		m_dsmPath = _T("");
	}
	UpdateData(false);
}
BOOL IsDirExist(const CString & csDir)
{
	DWORD dwAttrib = GetFileAttributes(csDir);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 != (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CdomDsmToOsgbDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_domPath.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_NEEDDOM));
		//AfxMessageBox(_T("请导入DOM!"));
		return;
	}
	if (m_dsmPath.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_NEEDDEM));
		//AfxMessageBox(_T("请导入DEM!"));
		return;
	}
	if (m_osgPath.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_NEEDOUT));
		//AfxMessageBox(_T("请设置输出路径!"));
		return;
	}
	CString str_path = m_osgPath.Left(m_osgPath.ReverseFind('\\'));
	if (IsDirExist(str_path) == TRUE)
	{
		AfxMessageBox(str_path + LoadString(IDS_ERROR_EXIST));
		return;
	}
	m_messageControl.ShowWindow(SW_SHOW);
	m_messageControl.Clear();
	//m_messageControl

//	m_messageControl.DeleteString();


	CString x;
	//CString y;
	GetDlgItemText(IDC_EDIT1, x);
	//GetDlgItemText(IDC_EDIT2, y);

	//记录分块数
	//USES_CONVERSION;
	std::string sx(x.GetBuffer(0));
	//std::string sy(y.GetBuffer(0));
	xPart = x.IsEmpty() ? 5 : atoi(sx.c_str());
	//yPart = y.IsEmpty() ? 1 : atoi(sy.c_str());

	std::string domname(m_domPath.GetBuffer(0));
	std::string demname(m_dsmPath.GetBuffer(0));
	filepath = domname.substr(0, domname.find_last_of("\\") + 1);
	domfile = domname.substr(domname.find_last_of("\\") + 1);
	demfile = demname.substr(demname.find_last_of("\\") + 1);

	CDataConvertor* convert = new CDataConvertor();
	convert->produceDlg = this;
	convert->startProduce();
}


void CdomDsmToOsgbDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}


void CdomDsmToOsgbDlg::OnClickedButtonOsg()
{
	CString filter;
	filter = "osg(*.osg,*.osgb,*.osgt)|*.osg,*.osgb,*.osgt||";
	//CFileDialog dlg(FALSE, "osgb", "out", OFN_HIDEREADONLY, filter);
	CFolderPickerDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		
		m_osgPath = dlg.GetPathName();
		m_osgPath += "\\out\\out.osgb";

	}
	else
	{
		m_osgPath = _T("");
	}
	UpdateData(false);
}
