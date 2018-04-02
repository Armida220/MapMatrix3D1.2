
// extractAerialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "extractAerial.h"
#include "extractAerialDlg.h"
#include "afxdialogex.h"
#include "Smart3DImportor.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "EasySize.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
	DECLARE_EASYSIZE
public:
	CAboutDlg(const StringArray & sa);

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	const StringArray & m_sa;
	virtual BOOL OnInitDialog();

	CListCtrl m_listControl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

CAboutDlg::CAboutDlg(const StringArray & sa) : CDialogEx(CAboutDlg::IDD)
, m_sa(sa)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listControl);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CAboutDlg)
	EASYSIZE(IDC_STATIC_ICON, ES_BORDER, ES_BORDER, ES_KEEPSIZE, ES_KEEPSIZE, 0)
	EASYSIZE(IDC_LIST1, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
	EASYSIZE(IDOK, ES_KEEPSIZE, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP


BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	INIT_EASYSIZE;
	m_listControl.InsertColumn(0, _T(""), LVCFMT_LEFT,500);
	for (int i = m_sa.GetSize() - 1; i >= 0; --i)
	{
		if (i < 0) break;
		m_listControl.InsertItem(0, m_sa.GetAt(i));
	}
	return TRUE;  
}


void CAboutDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
}



// CextractAerialDlg dialog



CextractAerialDlg::CextractAerialDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CextractAerialDlg::IDD, pParent)
	, m_inXML(_T(""))
	, m_outXML(_T(""))
	
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	StringArray msgA;

// 	CAboutDlg ab(msgA);
// 	ab.DoModal();
}

void CextractAerialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_wndList);
	DDX_Text(pDX, IDC_STATIC_INXML, m_inXML);
	DDX_Text(pDX, IDC_STATIC_OUTXML, m_outXML);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_EXIT);

	//  DDX_Check(pDX, IDC_CHECK1, m_showWarning);
	DDX_Control(pDX, IDC_CHECK1, m_cShowWarning);
}

BEGIN_MESSAGE_MAP(CextractAerialDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DRAWITEM()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CextractAerialDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_IMGPATH, &CextractAerialDlg::OnBnClickedButtonImgpath)
//	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CextractAerialDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_INXML, &CextractAerialDlg::OnBnClickedButtonInxml)
	ON_BN_CLICKED(IDC_BUTTON_OUTXML, &CextractAerialDlg::OnBnClickedButtonOutxml)
	ON_BN_CLICKED(IDOK, &CextractAerialDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CextractAerialDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CextractAerialDlg message handlers

BOOL CextractAerialDlg::OnInitDialog()
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
	SetBackgroundColor(RGB(80, 80, 80));
	//SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE | WS_EX_LAYERED) ^ 0x80000);
	//SetLayeredWindowAttributes(RGB(80, 80, 80), 100, LWA_COLORKEY);
	m_wndList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_wndList.InsertColumn(0, _T(""), LVCFMT_LEFT, 500);
	m_wndList.setColor(RGB(60, 60, 60));
	m_wndList.SetTextBkColor(RGB(60, 60, 60));
	m_wndList.SetBkColor(RGB(60, 60, 60));
	m_wndList.SetTextColor(RGB(255, 255, 255));

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
	s1->SetFont(&m_font);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CextractAerialDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
// 	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
// 	{
// 		CAboutDlg dlgAbout;
// 		dlgAbout.DoModal();
// 	}
// 	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CextractAerialDlg::OnPaint()
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
HCURSOR CextractAerialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CextractAerialDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl == IDC_BUTTON_INXML || nIDCtl == IDC_BUTTON_OUTXML || nIDCtl == IDC_BUTTON_IMGPATH 
		//|| nIDCtl == IDC_BUTTON_DELETE 
		|| nIDCtl == IDOK || nIDCtl == IDCANCEL)         //checking for the button 
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


HBRUSH CextractAerialDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	// TODO:  Return a different brush if the default is not desired
	return hbr;
}


void CextractAerialDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}


void CextractAerialDlg::OnBnClickedButtonImgpath()
{
	// TODO: Add your control notification handler code here
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, (LPCTSTR)_TEXT("Image Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"), NULL);

	if (dialog.DoModal() == IDOK)
	{
		POSITION pos = dialog.GetStartPosition();
		do{
			CString mImageFileName = dialog.GetNextPathName(pos);
			//CString mImageFileName = dialog.GetPathName();
			m_wndList.InsertItem(0, mImageFileName);
			//m_wndList.SetItemText(0, 0, mImageFileName);
		} while (pos != NULL);
	}
}


// void CextractAerialDlg::OnBnClickedButtonDelete()
// {
// 	// TODO: Add your control notification handler code here
// 	int n = m_wndList.GetSelectionMark();
// 	if (n == -1)
// 	{
// 		return;
// 	}
// 	m_wndList.DeleteItem(n);
// 	m_wndList.SetSelectionMark(-1);
// }


void CextractAerialDlg::OnBnClickedButtonInxml()
{
	// TODO: Add your control notification handler code here
	CFileDialog dialog(TRUE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
	m_inXML = _T("");
	if (dialog.DoModal() == IDOK)
	{
		m_inXML = dialog.GetPathName();
	}

	UpdateData(FALSE);
}


void CextractAerialDlg::OnBnClickedButtonOutxml()
{
	// TODO: Add your control notification handler code here
	CFileDialog dialog(FALSE, NULL, NULL, OFN_HIDEREADONLY, (LPCTSTR)_TEXT("xml Files (*.xml)|*.xml|All Files (*.*)|*.*||"), NULL);
	m_outXML = _T("");
	if (dialog.DoModal() == IDOK)
	{
		m_outXML = dialog.GetPathName();

		CString fileExt = dialog.GetFileExt();

		if (fileExt != _T("xml"))
		{
			m_outXML += _T(".xml");
		}
	}
	UpdateData(FALSE);
}


void CextractAerialDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CSmart3DImportor import;
	//string strImgFileName = mImageFileName;
	std::string strSmart3dFileName = m_inXML;
	TCHAR originXml[_MAX_PATH] = { 0 };
	GetModuleFileName(NULL, originXml, _MAX_FNAME);
	TCHAR *pos = _tcsrchr(originXml, _T('\\'));
	if (pos)pos[1] = _T('\0');
	_tcscat_s(originXml, _T("1.xml"));
	std::string strOriXmlPrjFileName = originXml;
	std::string outXmlPrjFileName = m_outXML;

	std::vector<std::string> vecImageFileName;
	for (int i = 0; i < m_wndList.GetItemCount(); ++i)
	{
		std::string s = m_wndList.GetItemText(i, 0);
		vecImageFileName.push_back(s);
	}

	try{
		import.start(vecImageFileName, strSmart3dFileName, strOriXmlPrjFileName, outXmlPrjFileName);
		const StringArray& msgA = import.getMessage();
		if (!msgA.IsEmpty() && m_cShowWarning.GetCheck()){
			if (msgA.GetSize() > 10){
				CAboutDlg ab(msgA);
				//ab.m_str = (msg);
				ab.DoModal();
			}
			else{
				CString msg;
				for (int i = 0; i < 0; i ++)
				{
					msg += msgA.GetAt(i) + "\n";
				}
				MessageBox(msg, _T("Warning"), MB_ICONWARNING);
			}
		}
	}
	
	catch (std::exception & e){
		MessageBox(e.what(), _T("Error"), MB_ICONERROR);
	}
}


void CextractAerialDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}


void CextractAerialDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int n = m_wndList.GetSelectionMark();
	if (n == -1)
	{
		return;
	}
	m_wndList.DeleteItem(n);
	m_wndList.SetSelectionMark(-1);
	*pResult = 0;
}


