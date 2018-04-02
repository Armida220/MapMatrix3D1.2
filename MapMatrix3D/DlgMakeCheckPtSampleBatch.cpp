// DlgMakeCheckPtSampleBatch.cpp : implementation file
//

#include "stdafx.h"
#include "Editbase.h"
#include "DlgMakeCheckPtSampleBatch.h"
#include "CoordCenter.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static CDlgMakeCheckPtSampleBatch::TempSaveData sData;

/////////////////////////////////////////////////////////////////////////////
// CDlgMakeCheckPtSampleBatch dialog

#define REGPATH_MAKESAMPLE			"MakeSample"

CDlgMakeCheckPtSampleBatch::CDlgMakeCheckPtSampleBatch(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMakeCheckPtSampleBatch::IDD, pParent)
	, m_nRotationAngle(0)
{
	//{{AFX_DATA_INIT(CDlgMakeCheckPtSampleBatch)
	m_nCrossWidth = 0;
	m_strImageID = _T("");
	m_nImageWidth = 1023;
	m_nImageHigh = 1023;
	m_nFontSize = 25;
	m_nRotationAngle = 0;
	m_strNoteText = _T("");
	m_strImagePath = _T("C:\\");
	m_strPtID = _T("50000");
	m_clrFont = RGB(255,0,0);
	m_clrCross = RGB(255,0,0);
	m_nCrossWidth = 11;
	m_strFontName = "黑体";
	m_bPointNumFromFile = TRUE;
	//}}AFX_DATA_INIT

	if(sData.bInited)
	{
		m_nCrossWidth = sData.m_nCrossWidth;
		m_strImageID = sData.m_strImageID;
		m_nImageWidth = sData.m_nImageWidth;
		m_nImageHigh = sData.m_nImageHigh;
		m_nFontSize = sData.m_nFontSize;
		m_nRotationAngle = sData.m_nRotationAngle;
		m_strImagePath = sData.m_strImagePath;
		m_strPtID = sData.m_strPtID;
		m_clrFont = sData.m_clrFont;
		m_clrCross = sData.m_clrCross;
		m_bPointNumFromFile = sData.m_bPointNumFromFile;
		m_strPointFilePath = sData.m_strPointFilePath;
		m_strProjectFilePath = sData.m_strProjectFilePath;
	}
	else
	{
		sData.m_nCrossWidth = m_nCrossWidth;
		sData.m_strImageID = m_strImageID;
		sData.m_nImageWidth = m_nImageWidth;
		sData.m_nImageHigh = m_nImageHigh;
		sData.m_nFontSize = m_nFontSize;
		sData.m_nRotationAngle = m_nRotationAngle;
		sData.m_strImagePath = m_strImagePath;
		sData.m_strPtID = m_strPtID;
		sData.m_clrFont = m_clrFont;
		sData.m_clrCross = m_clrCross;

		sData.m_bPointNumFromFile = m_bPointNumFromFile;
		sData.m_strPointFilePath = m_strPointFilePath;
		sData.m_strProjectFilePath = m_strProjectFilePath;

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
	m_strPtID = AfxGetApp()->GetProfileString(REGPATH_MAKESAMPLE,"PointID",m_strPtID);
	m_nRotationAngle = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE, "RotationAngle", m_nRotationAngle);

	m_strPointFilePath = AfxGetApp()->GetProfileString(REGPATH_MAKESAMPLE,"PointFilePath",m_strPointFilePath);
	m_bPointNumFromFile = AfxGetApp()->GetProfileInt(REGPATH_MAKESAMPLE,"bPointNumFromFile",m_bPointNumFromFile);
	m_strProjectFilePath = AfxGetApp()->GetProfileString(REGPATH_MAKESAMPLE,"ProjectFilePath",m_strProjectFilePath);

}


void IncreasePtID(CString& strPtID)
{
	if(sData.bInited)
	{
		int nLen = strPtID.GetLength();
		if(nLen<=0)
		{
			strPtID = "0001";
			return;
		}
		
		char text[1000] = {0};
		strcpy(text,strPtID);
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
			strPtID.Format(strFormat,value+1);
		}
		else if(pos<text + nLen-1 )
		{
			nLen = strlen(pos+1);
			
			CString strFormat;
			strFormat.Format("%%0%dd",nLen);

			long value = atol(pos+1);
			CString t;
			t.Format(strFormat,value+1);
			strPtID = strPtID.Left(pos+1-text) + t;		
		}
		else
		{
			strPtID = strPtID + "1";
		}
	}
}


void CDlgMakeCheckPtSampleBatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMakeCheckPtSampleBatch)
	DDX_Text(pDX, IDC_EDIT_CROSSWIDTH, m_nCrossWidth);
	DDX_Text(pDX, IDC_EDIT_IMAGEWIDTH, m_nImageWidth);
	DDX_Text(pDX, IDC_EDIT_IMAGEHIGHT2, m_nImageHigh);
	DDX_Text(pDX, IDC_EDIT_NOTEFONTSIZE, m_nFontSize);
	DDX_Text(pDX, IDC_EDIT_PATH, m_strImagePath);
	DDX_Text(pDX, IDC_EDIT_PTNUMBER, m_strPtID);
	DDX_Control(pDX, IDC_COMBO_NOTEFONT, m_wndComboFont);
	DDX_Control(pDX, IDC_COMBO_ROTATIONANGLE, m_wndComboRotation);
	
	DDX_Control(pDX, IDC_BUTTON_CROSSCOLOR, m_wndCrossColor);
	DDX_Control(pDX, IDC_BUTTON_NOTECOLOR, m_wndFontColor);
	DDX_Check(pDX, IDC_CHECK_POINTNUM_FROMFILE, m_bPointNumFromFile);
	DDX_Text(pDX, IDC_EDIT_PATH_POINT, m_strPointFilePath);
	DDX_Text(pDX, IDC_EDIT_PATH_PROJECT, m_strProjectFilePath);
	DDX_CBIndex(pDX, IDC_COMBO_ROTATIONANGLE, m_nRotationAngle);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(CDlgMakeCheckPtSampleBatch, CDialog)
	//{{AFX_MSG_MAP(CDlgMakeCheckPtSampleBatch)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnPointFileBrowse)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE3, OnProjectFileBrowse)
	ON_BN_CLICKED(IDC_CHECK_POINTNUM_FROMFILE, OnCheckPointNumFromFile)
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
// CDlgMakeCheckPtSampleBatch message handlers

void CDlgMakeCheckPtSampleBatch::OnButtonBrowse() 
{
	CString retPath;
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),retPath.GetBuffer(256),NULL,GetSafeHwnd()) )
		return;
	
	UpdateData(TRUE);
	m_strImagePath = retPath;
	UpdateData(FALSE);	
}


void CDlgMakeCheckPtSampleBatch::OnPointFileBrowse() 
{
	CString filter(StrFromResID(IDS_ALLFILE_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;

	m_strPointFilePath = dlg.GetPathName();

	UpdateData(FALSE);	
}



void CDlgMakeCheckPtSampleBatch::OnProjectFileBrowse() 
{
	CString filter(StrFromResID(IDS_PROJECT_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE), TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;

	m_strProjectFilePath = dlg.GetPathName();

	UpdateData(FALSE);	
}


BOOL CDlgMakeCheckPtSampleBatch::OnInitDialog() 
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

	UpdatePointFilePathState();
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void MakePointSmapleBatch(CDlgMakeCheckPtSampleBatch *pDlg);

void CDlgMakeCheckPtSampleBatch::OnOK() 
{
	UpdateData(TRUE);

	m_clrCross = m_wndCrossColor.GetColor();
	m_clrFont = m_wndFontColor.GetColor();
	int index = m_wndComboFont.GetCurSel();
	if(index>=0)m_wndComboFont.GetLBText(index,m_strFontName);

	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"FontSize",m_nFontSize);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"FontColor",m_clrFont);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"CrossWidth",m_nCrossWidth);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"ImageWidth",m_nImageWidth);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE, "ImageHigh", m_nImageHigh);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"CrossColor",m_clrCross);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"ImagePath",m_strImagePath);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"FontName",m_strFontName);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"NoteText",m_strNoteText);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"PointFilePath",m_strPointFilePath);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE,"bPointNumFromFile",m_bPointNumFromFile);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"ProjectFilePath",m_strProjectFilePath);
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"PointID",m_strPtID);
	AfxGetApp()->WriteProfileInt(REGPATH_MAKESAMPLE, "RotationAngle", m_nRotationAngle);

	sData.m_nCrossWidth = m_nCrossWidth;
	sData.m_strImageID = m_strImageID;
	sData.m_nImageWidth = m_nImageWidth;
	sData.m_nImageHigh = m_nImageHigh;
	sData.m_nFontSize = m_nFontSize;
	sData.m_nRotationAngle = m_nRotationAngle;
	sData.m_strImagePath = m_strImagePath;
	sData.m_strPtID = m_strPtID;
	sData.m_clrFont = m_clrFont;
	sData.m_clrCross = m_clrCross;
	sData.m_strPointFilePath = m_strPointFilePath;
	sData.m_bPointNumFromFile = m_bPointNumFromFile;
	sData.m_strProjectFilePath = m_strProjectFilePath;

	m_clrCross = m_wndCrossColor.GetColor();
	m_clrFont = m_wndFontColor.GetColor();

	int nsel = m_wndComboFont.GetCurSel();
	if(nsel>=0)
	{
		m_wndComboFont.GetLBText(nsel,m_strFontName);
	}

	nsel = m_wndComboRotation.GetCurSel();
	if (nsel >= 0)
	{
		m_wndComboRotation.GetLBText(nsel, m_strFontName);
		m_lfRotationAngle = atof(m_strFontName);
	}

	MakePointSmapleBatch(this);

	sData.m_strPtID = m_strPtID;
	AfxGetApp()->WriteProfileString(REGPATH_MAKESAMPLE,"PointID",m_strPtID);
	
	CDialog::OnOK();
}


void CDlgMakeCheckPtSampleBatch::UpdatePointFilePathState()
{
	CWnd *pWnd = GetDlgItem(IDC_EDIT_PTNUMBER);
	if(!pWnd)
		return;

	pWnd->EnableWindow(!m_bPointNumFromFile);
}


void CDlgMakeCheckPtSampleBatch::OnCheckPointNumFromFile()
{
	UpdateData(TRUE);
	UpdatePointFilePathState();
}


BOOL MakeCheckPtSample(LPCTSTR tifPath, LPCTSTR tipClipFile, LPCTSTR idtext, int x1, int y1, int imgWid, int imgHig, int crossWid, COLORREF crossColor,
				LPCTSTR fontName, int fontHei, COLORREF fontColor, LPCTSTR text,int imgRotationAngle = 0);



static int TestPoint2(PT_3D *pt, CPtrArray& arr, CArray<QUADRANGLE,QUADRANGLE> &arrModelBounds)
{
	int num = arr.GetSize(), find = -1;

	double dis,min=-1;
	for( int i=0; i<num; i++)
	{
		int index = i;
		CCoordCenter *pCenter = (CCoordCenter*)arr[index];
		if( !pCenter )continue;			

		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));

		QUADRANGLE quad = arrModelBounds[index];
		double x0 = 0, y0 = 0;
		for( int j=0; j<4; j++)
		{
			pt3ds[j].x = quad.lfX[j];
			pt3ds[j].y = quad.lfY[j];
			pt3ds[j].z = 0;

			x0 += quad.lfX[j];
			y0 += quad.lfY[j];
		}

		x0 /= 4;
		y0 /= 4;

		if( GraphAPI::GIsPtInRegion(*pt,pt3ds,4)==2 )
		{
			dis = GraphAPI::GGet2DDisOf2P(pt[0],PT_3D(x0,y0,0));
			if( min<0 || dis<min )
			{
				min = dis;
				find = index;
			}			
		}
	}
	
	return find;
}



void MakePointSmapleBatch(CDlgMakeCheckPtSampleBatch *pDlg)
{
	//读取控制点
	FILE *fp = fopen(pDlg->m_strPointFilePath,"r");
	if (!fp)
	{
		AfxMessageBox(IDS_FILE_OPEN_ERR);
		return;
	}
	
	char line[1024],name[1024],imgID[1024];
	PT_3D pt;
	CArray<PT_3D,PT_3D> arrPts;
	CStringArray pt_names;
	CStringArray imgIDs;
	
	int nSum = 0;
	while (!feof(fp)) 
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line)-1,fp);
		
		pt.z = 0;
		if (sscanf(line,"%s %lf %lf %lf %s",name,&pt.x,&pt.y,&pt.z,imgID) < 5)
			continue;

		arrPts.Add(pt);
		pt_names.Add(CString(name));
		imgIDs.Add(CString(imgID));
		nSum++;
	}
	fclose(fp);

	if(nSum==0)
	{
		AfxMessageBox(IDS_ERR_CONTROLPOINTS_FILEFORMAT);
		return;
	}

	//加载工程文件
	CGeoBuilderPrj prj1;
	if(!prj1.LoadProject(pDlg->m_strProjectFilePath))
		return;

	CoreObject core1 = prj1.GetCoreObject();
	CArray<QUADRANGLE,QUADRANGLE> arrModelBounds;
	CPtrArray arr1;

	for( int i=0; i<core1.iStereoNum; i++)
	{
		CCoordCenter *pCenter = new CCoordCenter();
		if( pCenter )
		{
			pCenter->Init(core1,core1.stereo[i].sp.stereoID);
			arr1.Add(pCenter);

			QUADRANGLE quad;
			pCenter->GetModelBound(&quad);

			arrModelBounds.Add(quad);
		}
	}
	
	//依次遍历控制点，输出样图
	GProgressStart(nSum);

	CString strPtID = pDlg->m_strPtID;

	for(i=0; i<nSum; i++)
	{
		GProgressStep();	
		
		pt = arrPts[i];
		CString strPtName = pt_names[i];
		if(!pDlg->m_bPointNumFromFile || strPtName.IsEmpty())
		{
			IncreasePtID(strPtID);
			strPtName = strPtID;
		}

//		int idx = TestPoint2(&pt,arr1,arrModelBounds);
//		if( idx<0||idx>=core1.iStereoNum )continue;
		int idx = -1;
		CString imgID2 = imgIDs[i];
		for(int j1=0; j1<core1.iStereoNum; j1++)
		{
			if(imgID2.CompareNoCase(core1.stereo[j1].imageID[0])==0 || imgID2.CompareNoCase(core1.stereo[j1].imageID[1])==0)
			{
				idx = j1;
				break;
			}			
		}
		if(idx<0)
			continue;

		CString imgID = core1.stereo[idx].imageID[0];

		CString imgPath = pDlg->m_strImagePath;
		CString imgName;
		imgName.Format("%s_%s.tif",(LPCTSTR)strPtName,(LPCTSTR)imgID);

		Coordinate coord1, coord2;
		coord1.x=pt.x; coord1.y=pt.y;	coord1.z=pt.z;
		coord1.iType=GROUND; 
		coord2.iType=SCANNER;

		CCoordCenter *pCenter = (CCoordCenter*)arr1[idx];
		
		if(!pCenter->Convert(coord1,coord2))
			continue;

		CString srcimgName;

		for(int j=0; j<core1.iStripNum; j++)
		{
			for(int k=0; k<core1.strip[j].iImageNum; k++)
			{
				if(imgID.CompareNoCase(core1.strip[j].image[k].strImageID)==0)
				{
					srcimgName = core1.strip[j].image[k].ie.strFileName;
					j = core1.iStripNum;
					break;
				}
			}			
		}

		if(srcimgName.IsEmpty())
			continue;

		double x = coord2.lx, y = coord2.ly;

		MakeCheckPtSample(srcimgName, imgPath + "\\" + imgName, strPtName, x, y, pDlg->m_nImageWidth, pDlg->m_nImageHigh,
			pDlg->m_nCrossWidth, pDlg->m_clrCross, pDlg->m_strFontName, pDlg->m_nFontSize, pDlg->m_clrFont, pDlg->m_strNoteText, pDlg->m_nRotationAngle);

		CString txtFile = imgPath+"\\"+imgName;
		txtFile = txtFile.Left(txtFile.GetLength()-4) + ".jpg.txt";
		fp = fopen(txtFile,"wt");
		if(fp)
		{
			fprintf(fp,"%s %.4f %.4f %.4f\r\n",(LPCTSTR)strPtName,pt.x,pt.y,pt.z);
			fclose(fp);
		}

	}

	if(!pDlg->m_bPointNumFromFile)
		pDlg->m_strPtID = strPtID;

	GProgressEnd();

	//释放坐标换算器
	for( i=0; i<core1.iStereoNum; i++)
	{
		CCoordCenter *pCenter = (CCoordCenter*)arr1[i];
		if( pCenter )delete pCenter;
	}
}