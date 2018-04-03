
// mm3dConvertSmart3DDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mm3dConvertSmart3D.h"
#include "mm3dConvertSmart3DDlg.h"
#include "afxdialogex.h"
#include "ConvertTool.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// Cmm3dConvertSmart3DDlg dialog



Cmm3dConvertSmart3DDlg::Cmm3dConvertSmart3DDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmm3dConvertSmart3DDlg::IDD, pParent)
	, m_path(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmm3dConvertSmart3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_PATHINFO, m_path);
	DDX_Control(pDX, IDC_BUTTON2, m_EXIT);
	DDX_Control(pDX, IDC_PROGRESS_MM3D2SMART3D, m_progress);
}

BEGIN_MESSAGE_MAP(Cmm3dConvertSmart3DDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &Cmm3dConvertSmart3DDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_OSGB, &Cmm3dConvertSmart3DDlg::OnBnClickedButtonSelectOsgb)
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &Cmm3dConvertSmart3DDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// Cmm3dConvertSmart3DDlg message handlers

BOOL Cmm3dConvertSmart3DDlg::OnInitDialog()
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
	m_progress.ShowWindow(SW_HIDE);
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
	CStatic* s1 = (CStatic*)GetDlgItem(IDC_STATIC_TITLE);
	CStatic* s2 = (CStatic*)GetDlgItem(IDC_STATIC_PATHINFO);
	s1->SetFont(&m_font);
	s2->SetFont(&m_font1);

	m_progress.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cmm3dConvertSmart3DDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cmm3dConvertSmart3DDlg::OnPaint()
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
HCURSOR Cmm3dConvertSmart3DDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cmm3dConvertSmart3DDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}


void Cmm3dConvertSmart3DDlg::OnBnClickedButtonSelectOsgb()
{
	// TODO: Add your control notification handler code here
	USES_CONVERSION;
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, 
		LoadString(IDS_FILE_FILTER_3D),
		//(LPCTSTR)_TEXT("3D model Files (*.osgb, *.osg, *.obj)|*.osgb;*.osg;*.obj;*.osgt|All Files (*.*)|*.*||"), 
		NULL);
	m_path = _T("");
	if (dlg.DoModal() == IDOK)
	{
		m_path = dlg.GetPathName();
	}
	UpdateData(FALSE);
}


void Cmm3dConvertSmart3DDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl == IDOK || nIDCtl == IDCANCEL || nIDCtl == IDC_BUTTON_SELECT_OSGB)         //checking for the button 
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
	CDialogEx::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


HBRUSH Cmm3dConvertSmart3DDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(250, 250, 250));
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void Cmm3dConvertSmart3DDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_path.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_SETFILE));
		//AfxMessageBox(_T("请选择文件！"));
		return;
	}
	//USES_CONVERSION;
	CString extSelect_ = LoadString(IDS_FILE_FILTER_OSG);
	
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, extSelect_, NULL);
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();
		CString _ext = filePath.Left(filePath.GetLength() - filePath.ReverseFind(_T('.')));
		if (_ext.IsEmpty())		//防止忘了输出后缀名
		{
			filePath += fileExt;
		}
		CStringA filePathA;
		filePathA = filePath;
		outputFilePath = std::string(filePathA);
		CConvertTool* cTool = new CConvertTool();
		cTool->convertDlg = this;
		cTool->startConvert();
	}
}


void Cmm3dConvertSmart3DDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}
