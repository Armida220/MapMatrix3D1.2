
// linkFetchCoordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "linkFetchCoord.h"
#include "linkFetchCoordDlg.h"
#include "afxdialogex.h"
#include "..\tinyxml2\tinyxml2.h"
#include <regex>
#include <ostream>
#include<iostream>
#include<algorithm>
#include <sstream>  
#include <osg/Group>
#include <osg/Node>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include "OsgPageLodSetter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "..\tinyxml2\tinyxml2.cpp"

using namespace std;
using namespace osg;
using namespace osgDB;


//模板函数：将string类型变量转换为常用的数值类型（此方法具有普遍适用性）  
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
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


// ClinkFetchCoordDlg dialog



ClinkFetchCoordDlg::ClinkFetchCoordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ClinkFetchCoordDlg::IDD, pParent)
	, m_xmlPath(_T(""))
	, m_outputPath(_T(""))
	, m_IndexPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClinkFetchCoordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_EXIT);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Text(pDX, IDC_STATIC_XMLPATH, m_xmlPath);
	DDX_Text(pDX, IDC_STATIC_OUTPUTPATH, m_outputPath);
	DDX_Text(pDX, IDC_STATIC_INDEXPATH, m_IndexPath);
}

BEGIN_MESSAGE_MAP(ClinkFetchCoordDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &ClinkFetchCoordDlg::OnBnClickedButtonExit)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_INDEX, &ClinkFetchCoordDlg::OnBnClickedButtonIndex)
	ON_BN_CLICKED(IDC_BUTTON_XML, &ClinkFetchCoordDlg::OnBnClickedButtonXml)
	ON_BN_CLICKED(IDC_BUTTON_OUTPUT, &ClinkFetchCoordDlg::OnBnClickedButtonOutput)
	ON_BN_CLICKED(IDOK, &ClinkFetchCoordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ClinkFetchCoordDlg message handlers

BOOL ClinkFetchCoordDlg::OnInitDialog()
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
	m_progress.ShowWindow(SW_HIDE);

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
	CStatic* s2 = (CStatic*)GetDlgItem(IDC_STATIC_INDEXPATH);
	CStatic* s3 = (CStatic*)GetDlgItem(IDC_STATIC_OUTPUTPATH);
	CStatic* s4 = (CStatic*)GetDlgItem(IDC_STATIC_XMLPATH);
	s1->SetFont(&m_font);
	s2->SetFont(&m_font1);
	s3->SetFont(&m_font1);
	s4->SetFont(&m_font1);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void ClinkFetchCoordDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void ClinkFetchCoordDlg::OnPaint()
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
HCURSOR ClinkFetchCoordDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



CString ClinkFetchCoordDlg::LoadString(UINT uid)
{
	CString str;
	str.LoadString(uid);
	return str;
}

void ClinkFetchCoordDlg::OnBnClickedButtonExit()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}


void ClinkFetchCoordDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl == IDOK || nIDCtl == IDCANCEL || nIDCtl == IDC_BUTTON_INDEX || nIDCtl == IDC_BUTTON_XML || nIDCtl == IDC_BUTTON_OUTPUT)         //checking for the button 
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


void ClinkFetchCoordDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}


HBRUSH ClinkFetchCoordDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void ClinkFetchCoordDlg::OnBnClickedButtonIndex()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		LoadString(IDS_FILTER_OSGB),
		//(LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"),
		NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();

		if (fileExt != _T("osgb"))
		{
			filePath += _T(".osgb");
		}

		m_IndexPath = filePath;
	}
	else
	{
		m_IndexPath = _T("");
	}
	UpdateData(false);
}


void ClinkFetchCoordDlg::OnBnClickedButtonXml()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, 
		LoadString(IDS_FILTER_XML),
		//(LPCTSTR)_TEXT("xml模型 (*.xml)|*.xml|All Files (*.*)|*.*||"),
		NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();

		if (fileExt != _T("xml"))
		{
			filePath += _T(".xml");
		}

		m_xmlPath = filePath;
	}
	else
	{
		m_xmlPath = _T("");
	}
	UpdateData(false);
}


void ClinkFetchCoordDlg::OnBnClickedButtonOutput()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, 
		LoadString(IDS_FILTER_OSGB),
		//(LPCTSTR)_TEXT("osgb模型 (*.osgb)|*.osgb|All Files (*.*)|*.*||"),
		NULL);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileExt = dlg.GetFileExt();

		if (fileExt != _T("osgb"))
		{
			filePath += _T(".osgb");
		}

		m_outputPath = filePath;
	}
	else
	{
		m_outputPath = _T("");
	}
	UpdateData(false);
}


void ClinkFetchCoordDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if (m_IndexPath.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_INDEX));
		//AfxMessageBox(_T("索引路径不能为空！"));
		return;
	}
	else if (m_xmlPath.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_XML));
		//AfxMessageBox(_T("坐标路径不能为空！"));
		return;
	}
	else if (m_outputPath.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_OUT));
		//AfxMessageBox(_T("输出路径不能为空！"));
		return;
	}
	m_progress.SetRange(0, 1000);
	m_progress.SetPos(1);
	m_progress.ShowWindow(SW_SHOW);
	
	std::string coordPathName = m_xmlPath;
	tinyxml2::XMLDocument doc;
	int res = doc.LoadFile(coordPathName.c_str());

	if (res != 0)
	{
		AfxMessageBox(LoadString(IDS_ERROR_LOADXML));
		//AfxMessageBox(_T("XML文件加载失败！"));
		m_progress.ShowWindow(SW_HIDE);
		return;
	}

	tinyxml2::XMLElement* root = doc.RootElement();

	if (!root)
	{
		AfxMessageBox(LoadString(IDS_ERROR_READXML));
		//AfxMessageBox(_T("XML文件读取遇到错误！"));
		m_progress.ShowWindow(SW_HIDE);
		return;
	}

	tinyxml2::XMLElement *Head = root->FirstChildElement("SRSOrigin");
	std::string text = Head->GetText();

	double x = 0; double y = 0; double z = 0;
	extract(text, x, y, z);

	string linkPathName = m_IndexPath;
	ref_ptr<Node> node = readNodeFile(linkPathName);

	if (node)
	{
		Matrix mat;
		mat.setTrans(Vec3d(x, y, z));
		ref_ptr<MatrixTransform> trans = new MatrixTransform;
		trans->setMatrix(mat);
		trans->addChild(node);

		COsgPageLodSetter ive("");
		trans->accept(ive);

		string outPathName = m_outputPath;
		writeNodeFile(*trans, outPathName);
		m_progress.SetPos(1000);
		AfxMessageBox(LoadString(IDS_SUCCESS_GENERATE));
		//AfxMessageBox(_T("生成索引成功！"));
	}
	else
	{
		AfxMessageBox(LoadString(IDS_ERROR_GENERATE));
		//AfxMessageBox(_T("索引文件读取失败！"));
	}
	
	m_progress.ShowWindow(SW_HIDE);
	
}

void ClinkFetchCoordDlg::extract(std::string str, double &x, double &y, double &z)
{
	regex reg1("(\\d+),(\\d+),(\\d+)");
	smatch r1;
	regex_match(str, r1, reg1);
	x = stringToNum<int>(r1[1]);
	y = stringToNum<int>(r1[2]);
	z = stringToNum<int>(r1[3]);
}