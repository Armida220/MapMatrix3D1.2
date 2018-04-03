
// osgMultiFileLinkCreatorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "osgMultiFileLinkCreator.h"
#include "osgMultiFileLinkCreatorDlg.h"
#include "afxdialogex.h"
#include <process.h>


#include "osg\Geode"
#include "osg\PagedLod"
#include <osg/Group>
#include <osgDB/WriteFile>
#include <osgDB/ReadFile>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
inline CString  LoadString(UINT uid)
{
	CString str;
	str.LoadString(uid);
	return str;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CosgMultiFileLinkCreatorDlg �Ի���



CosgMultiFileLinkCreatorDlg::CosgMultiFileLinkCreatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CosgMultiFileLinkCreatorDlg::IDD, pParent)
	, m_path(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CosgMultiFileLinkCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DIRPATH, m_path);
	DDX_Control(pDX, IDC_PROGRESS_FILELINKER, m_progress);
	DDX_Control(pDX, IDC_BUTTON_EXIT1, m_EXIT);
}

BEGIN_MESSAGE_MAP(CosgMultiFileLinkCreatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_DIR, &CosgMultiFileLinkCreatorDlg::OnBnClickedButtonSelectDir)
	ON_BN_CLICKED(IDOK, &CosgMultiFileLinkCreatorDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_EXIT1, &CosgMultiFileLinkCreatorDlg::OnBnClickedButtonExit1)
	ON_WM_CTLCOLOR()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CosgMultiFileLinkCreatorDlg ��Ϣ�������

BOOL CosgMultiFileLinkCreatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	SetBackgroundColor(RGB(80,80,80));
	m_progress.ShowWindow(SW_HIDE);

	HBITMAP   hBitmap;
	hBitmap = (HBITMAP)::LoadImage(
		NULL,
		_T("quit.bmp"),                           // ͼƬȫ·��  
		IMAGE_BITMAP,                          // ͼƬ��ʽ  
		0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION);  // ע��LR_LOADFROMFILE  
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
		_T("����"));
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
		_T("����"));
	CStatic* s1 = (CStatic*)GetDlgItem(IDC_STATIC_TILTE1);
	CStatic* s2 = (CStatic*)GetDlgItem(IDC_STATIC_DIRPATH);
	s1->SetFont(&m_font);
	s2->SetFont(&m_font1);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CosgMultiFileLinkCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CosgMultiFileLinkCreatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CosgMultiFileLinkCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CosgMultiFileLinkCreatorDlg::SelectDir(std::string &filePath)
{
	CFolderPickerDialog cpd;
	if (cpd.DoModal() == IDOK)
	{
		CStringA fpA;
		fpA = cpd.GetFolderPath();
		filePath = fpA.operator LPCSTR();
	}
}

void CosgMultiFileLinkCreatorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CDialogEx::OnLButtonDown(nFlags, point);
	PostMessage(WM_NCLBUTTONDOWN,
		HTCAPTION,
		MAKELPARAM(point.x, point.y));
}

void CosgMultiFileLinkCreatorDlg::OnBnClickedButtonSelectDir()
{
	// TODO: Add your control notification handler code here
	std::string filePath;
	SelectDir(filePath);
	if (filePath.empty())
	{
		m_path = _T("");
		UpdateData(false);
		CRect rc;
		GetDlgItem(IDC_STATIC_DIRPATH)->GetWindowRect(&rc);
		ScreenToClient(&rc);
		InvalidateRect(rc);
		UpdateWindow();
		return;
	}
	else
	{
		m_path = CString(filePath.c_str());
		UpdateData(false);
	}
}


void CosgMultiFileLinkCreatorDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if (m_path.IsEmpty())
	{
		AfxMessageBox(LoadString(IDS_ERROR_SETFOLDER));
		return;
	}
	COsgMultiFileLinkCreator* Creator = new COsgMultiFileLinkCreator();
	Creator->linkDlg = this;
	Creator->createLinkForMultiFile();
}

void CosgMultiFileLinkCreatorDlg::OnBnClickedButtonExit1()
{
	// TODO: Add your control notification handler code here
	DestroyWindow();
}

HBRUSH CosgMultiFileLinkCreatorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void CosgMultiFileLinkCreatorDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDCtl == IDOK || nIDCtl == IDCANCEL || nIDCtl == IDC_BUTTON_SELECT_DIR)         //checking for the button 
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

//////////////////////////////////////////////////////////
void COsgMultiFileLinkCreator::createLinkForMultiFile()
{
	StartcreateLinkForMultiFile();
}

void COsgMultiFileLinkCreator::createMultiFileLink(void* ptr)
{
	COsgMultiFileLinkCreator* Creator = (COsgMultiFileLinkCreator*)ptr;
	Creator->createLink();
}

void COsgMultiFileLinkCreator::StartcreateLinkForMultiFile()
{
	_beginthread(&createMultiFileLink, 0, this);
}


void COsgMultiFileLinkCreator::createLink()
{
	USES_CONVERSION;
	std::string filePath(linkDlg->m_path.GetBuffer(0));
	
	linkDlg->m_path = LoadString(IDS_MESSAGE_FINDINDEX);
	linkDlg->UpdateData(false);
	CRect rc;
	linkDlg->GetDlgItem(IDC_STATIC_DIRPATH)->GetWindowRect(&rc);
	linkDlg->ScreenToClient(&rc);
	linkDlg->InvalidateRect(rc);
	linkDlg->UpdateWindow();

	linkDlg->m_progress.SetRange(0,1000);
	linkDlg->m_progress.SetPos(0);
	linkDlg->m_progress.ShowWindow(SW_SHOW);
	std::string outputLodFile = filePath + "\\MultiFoderReader.osgb";
	int length = filePath.length() - 1;

	if (filePath.substr(length - 1, 1) != "\\")
	{
		filePath += "\\";
		filePath += "*.*";
	}

	int percent = 0;
	

	std::vector<std::string> vecStrFileName;
	SearchFile(filePath, &vecStrFileName);											//�ļ�����

	if (vecStrFileName.empty())
	{
		AfxMessageBox(_T("�Ҳ���Tile�ļ�"));
		linkDlg->m_path = _T("");
		linkDlg->UpdateData(false);
		CRect rc;
		linkDlg->GetDlgItem(IDC_STATIC_DIRPATH)->GetWindowRect(&rc);
		linkDlg->ScreenToClient(&rc);
		linkDlg->InvalidateRect(rc);
		linkDlg->UpdateWindow();
		linkDlg->m_progress.ShowWindow(SW_HIDE);
		return;
	}

	linkDlg->m_progress.SetPos(100);

	
	CString fmtA;
	fmtA = LoadString(IDS_MESSAGE_CREATEINDEX);
	//��Ŀ¼����
	std::string str = filePath.substr(0, filePath.length() - 3);
	char strMsg[99];
	sprintf(strMsg, fmtA, str.c_str());
	linkDlg->m_path = CString(strMsg);
	linkDlg->UpdateData(false);
	
	linkDlg->GetDlgItem(IDC_STATIC_DIRPATH)->GetWindowRect(&rc);
	linkDlg->ScreenToClient(&rc);
	linkDlg->InvalidateRect(rc);
	linkDlg->UpdateWindow();

	osg::ref_ptr<osg::Group> group = new osg::Group();

	for (int i = 0; i < vecStrFileName.size(); i++)
	{
		if (i % 10 == 0 || i < 10)
		{
			percent = (int)((double)(i + 1) / (double)vecStrFileName.size() * 800) + 100;
			linkDlg->m_progress.SetPos(percent);
		}

		osg::ref_ptr<osg::PagedLOD> lod = dynamic_cast<osg::PagedLOD*>(osgDB::readNodeFile(vecStrFileName[i]));

		if (!lod)
		{
			continue;
		}

		int pos = vecStrFileName[i].rfind('\\');
		std::string subSTR = vecStrFileName[i].substr(0, pos);
		int posL = subSTR.rfind('\\');
		std::string relativeFilePath = ".\\" + vecStrFileName[i].substr(posL + 1, vecStrFileName[i].length() - posL);;  //���·��
		lod->setFileName(1, relativeFilePath);

		lod->setRange(0, 0, 1.0);																							//��һ�㲻�ɼ�
		lod->setRange(1, 1.0, 1e+030);

		lod->setDatabasePath("");
		group->addChild(lod);
	}

	osgDB::writeNodeFile(*group, outputLodFile);

	//����������
	linkDlg->m_progress.SetPos(1000);
	
	linkDlg->m_path = _T("");
	linkDlg->UpdateData(false);
	linkDlg->GetDlgItem(IDC_STATIC_DIRPATH)->GetWindowRect(&rc);
	linkDlg->ScreenToClient(&rc);
	linkDlg->InvalidateRect(rc);
	linkDlg->UpdateWindow();
	
	AfxMessageBox(LoadString(IDS_SUCCESS));
	linkDlg->m_progress.ShowWindow(SW_HIDE);
}

void COsgMultiFileLinkCreator::SearchFile(std::string dir, std::vector<std::string>* vecStrFileName)
{
	CString strDir = dir.c_str();
	CFileFind Finder;
	bool bWorking = Finder.FindFile(strDir);

	while (bWorking)
	{
		bWorking = Finder.FindNextFile();
		CString DataFileName = Finder.GetFilePath();		//GetFilePath���õ�ȫ·���� 

		if (Finder.IsDirectory() && !Finder.IsDots())			//�����Ŀ¼          
		{
			DataFileName += _T("\\*.osgb");
			std::string temp = DataFileName;
			SearchFile(temp, vecStrFileName);
		}
		else if (!Finder.IsDirectory() && !Finder.IsDots())		//��ΪĿ¼ 
		{
			CString DataFileTitle = Finder.GetFileName();
			int pos = DataFileTitle.ReverseFind('.');
			CString DataFileTitleName = DataFileTitle.Left(pos);
			CString DataFileTitleExt = DataFileTitle.Right(DataFileTitle.GetLength() - 1 - pos);

			if (DataFileTitleName.Find(_T('L')) == -1 && DataFileTitleExt == _T("osgb") && DataFileTitleName.Find(_T("Tile")) != -1)
			{
				std::string temp = DataFileName;
				vecStrFileName->push_back(temp);
			}
		}
	}

	Finder.Close();
}






