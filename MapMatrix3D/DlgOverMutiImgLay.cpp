// DlgOverMutiImgLay.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgOverMutiImgLay.h"
#include "ImageRead.h "
#include "GeoBuilderPrj2.h"
#include "ExMessage.h"
#include "gdal_priv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void DDX_Text_Silence(CDataExchange* pDX, int nIDC, double& value);
extern void DDX_Text_Silence(CDataExchange* pDX, int nIDC, UINT& value);
extern void DDX_Text_Silence(CDataExchange* pDX, int nIDC, int& value);
extern void DDX_Text_Silence(CDataExchange* pDX, int nIDC, float& value);
/////////////////////////////////////////////////////////////////////////////
// CDlgMutiList dialog


CDlgMutiList::CDlgMutiList(CWnd* pParent /*=NULL*/)
: CDialog(CDlgMutiList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMutiList)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgMutiList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMutiList)
	DDX_Control(pDX, IDC_LIST, m_wndList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMutiList, CDialog)
//{{AFX_MSG_MAP(CDlgMutiList)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMutiList message handlers

BOOL CDlgMutiList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int num = m_strList.GetSize();
	int maxLength = 0;
	for( int i=0; i<num; i++)
	{
		m_wndList.AddString(m_strList[i]);
		if( maxLength<m_strList[i].GetLength() )
			maxLength = m_strList[i].GetLength();
	}
	
	CDC *pDC = m_wndList.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);
		m_wndList.ReleaseDC(pDC);
		
		maxLength = maxLength*tm.tmAveCharWidth;
		CRect rect;
		m_wndList.GetClientRect(&rect);
		if( maxLength>rect.Width() )m_wndList.SetHorizontalExtent(maxLength);
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgMutiList::OnOK() 
{
	int nCount = m_wndList.GetSelCount();
	CArray<int,int> aryListBoxSel;
	
	aryListBoxSel.SetSize(nCount);
	m_wndList.GetSelItems(nCount, aryListBoxSel.GetData());
	CString temp;
	for (int i=0;i<nCount;i++)
	{
		m_wndList.GetText(aryListBoxSel[i],temp);
		m_strSelItems.Add(temp);
	}
	
	CDialog::OnOK();
}


BEGIN_MESSAGE_MAP(CCustomizeListBox, CListBox)

END_MESSAGE_MAP()
CCustomizeListBox::CCustomizeListBox()
{
	m_nMainIdx = -1;
}

CCustomizeListBox:: ~CCustomizeListBox()
{
	
}
//=============================================================================	
void CCustomizeListBox::MeasureItem(LPMEASUREITEMSTRUCT)
//=============================================================================	
{
}

//=============================================================================	
int CCustomizeListBox::CompareItem(LPCOMPAREITEMSTRUCT)
//=============================================================================	
{
	return 0;
}
void CCustomizeListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);
	LPCTSTR lpszText = (LPCTSTR) lpDrawItemStruct->itemData;
	ASSERT(lpszText != NULL);
	if (lpszText==NULL)
	{
		return ;
	}
	CDC dc;
	
	dc.Attach(lpDrawItemStruct->hDC);
	
	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();
	
	if (lpDrawItemStruct->itemID==m_nMainIdx)
	{
		dc.SetTextColor(RGB(255,0,0));
	}
	// If this item is selected, set the background color 
	// and the text color to appropriate values. Also, erase
	// rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
		dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, 
			::GetSysColor(COLOR_HIGHLIGHT));
	}
	else
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, crOldBkColor);
	
	// If this item has the focus, draw a red frame around the
	// item's rect.
	if ((lpDrawItemStruct->itemAction | ODA_FOCUS) &&
		(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		CBrush br(RGB(255, 0, 0));
		dc.FrameRect(&lpDrawItemStruct->rcItem, &br);
	}
	
	// Draw the text.
	dc.DrawText(
		lpszText,
		strlen(lpszText),
		&lpDrawItemStruct->rcItem,
		DT_LEFT|DT_SINGLELINE|DT_VCENTER);
	//	CListBox::DrawItem(lpDrawItemStruct);
	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	
	dc.Detach();
}

void CCustomizeListBox::SetMainItem(int index)
{
	m_nMainIdx = index;
	Invalidate(TRUE);
}

int CCustomizeListBox::GetMainItem()
{
	return m_nMainIdx;
}

int CCustomizeListBox::DeleteString(UINT nIndex)
{
	int ret = CListBox::DeleteString(nIndex);
	if (nIndex==m_nMainIdx)
	{
		if (GetCount()>0)
		{
			m_nMainIdx = 0;
		}
		else
			m_nMainIdx = -1;
	}
	else if (nIndex<m_nMainIdx)
	{
		m_nMainIdx--;
	}
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgOverMutiImgLay dialog


CDlgOverMutiImgLay::CDlgOverMutiImgLay(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgOverMutiImgLay::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOverMutiImgLay)
	m_bVisible = TRUE;
	m_lfMatrix1 = 1.0;
	m_lfMatrix2 = 0.0;
	m_lfMatrix3 = 0.0;
	m_lfMatrix4 = 1.0;
	m_lfXOff = 0.0;
	m_lfYOff = 0.0;
	m_nPixelBase = 0;
	//}}AFX_DATA_INIT
	memset(m_ptsBound,0,sizeof(m_ptsBound));
	m_lfPixelSizeX = m_lfPixelSizeY = 0;
	m_nMainImageIdx = -1;
}


void CDlgOverMutiImgLay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOverMutiImgLay)
	DDX_Control(pDX, IDC_LIST_MUTIIMG_FILENAME, m_listMutiImg);
	DDX_Check(pDX, IDC_CHECK_VISIBLE, m_bVisible);
	DDX_Text_Silence(pDX, IDC_EDIT_MATRIX1, m_lfMatrix1);
	DDX_Text_Silence(pDX, IDC_EDIT_MATRIX2, m_lfMatrix2);
	DDX_Text_Silence(pDX, IDC_EDIT_MATRIX3, m_lfMatrix3);
	DDX_Text_Silence(pDX, IDC_EDIT_MATRIX4, m_lfMatrix4);
	DDX_Text_Silence(pDX, IDC_EDIT_XOFF, m_lfXOff);
	DDX_Text_Silence(pDX, IDC_EDIT_YOFF, m_lfYOff);
	DDX_Radio(pDX, IDC_RADIO_PIXELLBCORNER, m_nPixelBase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgOverMutiImgLay, CDialog)
	//{{AFX_MSG_MAP(CDlgOverMutiImgLay)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_WORKSPACE, OnButtonBrowseWorkspace)
	ON_BN_CLICKED(IDC_BUTTON_ID_BROWSE_ANY, OnButtonIdBrowseAny)
	ON_BN_CLICKED(IDC_BUTTON_UNLOAD, OnButtonUnload)
	ON_BN_CLICKED(IDC_BUTTON_MAPBOUND, OnButtonMapbound)
	ON_BN_CLICKED(IDC_CHECK_VISIBLE, OnButtonVisible)
	ON_LBN_SELCHANGE(IDC_LIST_MUTIIMG_FILENAME, OnSelchangeListMutiimgFilename)
	ON_BN_CLICKED(IDC_BUTTON_SET_MAINIMG, OnButtonSetMainimg)
	ON_BN_CLICKED(IDC_BUTTON_RECOVERORIGINALPARAMETERS, OnButtonRecoverOriginalParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOverMutiImgLay message handlers
static BOOL IsExistInStrArray(const CString &str,const CArray<ViewImgPosition,ViewImgPosition> &arrStr)
{
	for (int i=arrStr.GetSize()-1;i>=0;i--)
	{
		if (str.CompareNoCase(arrStr[i].fileName)==0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CString CDlgOverMutiImgLay::m_curSelFileName = _T("");

void CDlgOverMutiImgLay::RefreshUIWithOption(LPCTSTR fileName)
{
	//if (m_nMainImageIdx!=-1)
	if( m_arrImgPos.GetSize()>0 )
	{	
		m_listMutiImg.ResetContent();	
		
		for (int i=0;i<m_arrImgPos.GetSize();i++)
		{
			m_arrImgPos[i].ConvertToImageType();

			m_listMutiImg.AddString(m_arrImgPos[i].fileName);
		}
		CString Name;
		if (fileName==NULL)
		{
			m_listMutiImg.SetSel(0,TRUE);
			Name = m_arrImgPos[0].fileName;
		}
		else
		{
			int nindex = 0;
			nindex = m_listMutiImg.FindStringExact(nindex,fileName);
			if (nindex!=LB_ERR)
			{
				m_listMutiImg.SetSel(nindex,TRUE);
				Name = (LPCTSTR)m_listMutiImg.GetItemDataPtr(nindex);

				m_nMainImageIdx = nindex;
			}
			else
			{
				m_listMutiImg.SetSel(0,TRUE);
				Name = m_arrImgPos[0].fileName;

				m_nMainImageIdx = 0;
			}
			
		}		
		{
			for (int i=m_arrImgPos.GetSize()-1;i>=0;i--)
			{
				if (Name.CompareNoCase(m_arrImgPos[i].fileName)==0)
				{
					break;
				}
			}
			// 			if (i<0)
			// 			{
			// 				return ;
			// 			}
			m_curSelFileName = Name;
			m_lfXOff = m_arrImgPos[i].lfImgLayOX;
			m_lfYOff = m_arrImgPos[i].lfImgLayOY;
			m_lfMatrix1 = m_arrImgPos[i].lfImgMatrix[0];
			m_lfMatrix2 = m_arrImgPos[i].lfImgMatrix[1];
			m_lfMatrix3 = m_arrImgPos[i].lfImgMatrix[2];
			m_lfMatrix4 = m_arrImgPos[i].lfImgMatrix[3];
			m_lfPixelSizeX = m_arrImgPos[i].lfPixelSizeX;
			m_lfPixelSizeY = m_arrImgPos[i].lfPixelSizeY;
			m_nPixelBase = m_arrImgPos[i].nPixelBase;
		}
		m_listMutiImg.SetMainItem(m_nMainImageIdx);
		UpdateData(FALSE);
	}
}

void CDlgOverMutiImgLay::RefreshUIForSelChange(LPCTSTR fileName)
{
	CString Name(fileName);

	{
		for (int i=m_arrImgPos.GetSize()-1;i>=0;i--)
		{
			if (Name.CompareNoCase(m_arrImgPos[i].fileName)==0)
			{
				break;
			}
		}
		if (i<0)
		{
			return ;
		}
		m_curSelFileName = Name;
		m_lfXOff = m_arrImgPos[i].lfImgLayOX;
		m_lfYOff = m_arrImgPos[i].lfImgLayOY;
		m_lfMatrix1 = m_arrImgPos[i].lfImgMatrix[0];
		m_lfMatrix2 = m_arrImgPos[i].lfImgMatrix[1];
		m_lfMatrix3 = m_arrImgPos[i].lfImgMatrix[2];
		m_lfMatrix4 = m_arrImgPos[i].lfImgMatrix[3];
		m_lfPixelSizeX = m_arrImgPos[i].lfPixelSizeX;
		m_lfPixelSizeY = m_arrImgPos[i].lfPixelSizeY;
		m_nPixelBase = m_arrImgPos[i].nPixelBase;
	}
	UpdateData(FALSE);

}

void CDlgOverMutiImgLay::OnButtonBrowseWorkspace() 
{
	CGeoBuilderPrj *prj = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETPROJECT,0,(LPARAM)&prj);
	if( !prj )return;
	
	CDlgMutiList dlg;
	
	CoreObject core = prj->GetCoreObject();
	for( int i=0; i<core.tmp.doms.iDomNum; i++)
	{
		dlg.m_strList.Add(core.tmp.doms.dom[i].strDomFile);
	}
	
	if( dlg.DoModal()!=IDOK )return;

	if(dlg.m_strSelItems.GetSize()<=0)return;
	GeoHeader head;
	CStringArray failedFile;
	CString temp;

	{
		CStringArray arr;
		arr.Copy(dlg.m_strSelItems);		
		for (int i=arr.GetSize()-1;i>=0;i--)
		{
			if(IsExistInStrArray(arr.GetAt(i),m_arrImgPos))
				arr.RemoveAt(i);
		}
	
		for (i=0;i<arr.GetSize();i++)
		{
			ViewImgPosition imgPos;
			strcpy(imgPos.fileName,(LPCTSTR)arr[i]);
		
			if(!prj->IsGeoImage(arr[i],head))
			{

				failedFile.Add(arr[i]);
				CString strTip(arr[i]);
				strTip+=StrFromResID(IDS_ERR_READIMGGEOINFO);
				GOutPut(strTip);
			
			
			}		
			else if( prj->IsGeoImage(arr[i],head) )//带参考信息的tif 先读取tifref 不存在时再读tfw 
			{
				imgPos.lfImgLayOX= head.lfStartX; imgPos.lfImgLayOY = head.lfStartY;
				imgPos.lfImgMatrix[0] = head.lfDx*head.lfScale*0.001*cos(head.lfKapa); 
				imgPos.lfImgMatrix[1] = head.lfDy*head.lfScale*0.001*sin(head.lfKapa); 
				imgPos.lfImgMatrix[2] =-head.lfDx*head.lfScale*0.001*sin(head.lfKapa); 
				imgPos.lfImgMatrix[3] = head.lfDy*head.lfScale*0.001*cos(head.lfKapa); 
				imgPos.lfPixelSizeX = head.lfDx*head.lfScale*0.001;
				imgPos.lfPixelSizeY = head.lfDy*head.lfScale*0.001;
				imgPos.nPixelBase = 0;
				if (fabs(imgPos.lfImgMatrix[2])<=1e-9)//win7 -0 
				{
					imgPos.lfImgMatrix[2] = 0;
				}
				
				CString name = arr[i];
				name = name.Left(name.GetLength()-3) + _T("tifref");
				FILE *fp = fopen(name,"r");
				if( fp )
				{
					double v[6];
					if( fscanf(fp,"%lf%lf%lf%lf%lf%lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5])==6 )
					{
						imgPos.lfImgMatrix[0] = v[0]; 
						imgPos.lfImgMatrix[1] = v[1]; 
						imgPos.lfImgMatrix[2] = v[2]; 
						imgPos.lfImgMatrix[3] = v[3]; 
						imgPos.lfImgLayOX = v[4];
						imgPos.lfImgLayOY = v[5];
						imgPos.lfPixelSizeX = sqrt(v[0]*v[0] + v[1]*v[1]);
						imgPos.lfPixelSizeY = sqrt(v[2]*v[2] + v[3]*v[3]);	
					}
					fclose(fp);
				}
				else
				{
					CString name_tfw;	
					name_tfw = arr[i];
					name_tfw = name_tfw.Left(name_tfw.GetLength()-3) + _T("tfw");
					FILE *fp = fopen(name_tfw,"r");
					if (fp)
					{
						double v[6];
						if( fscanf(fp,"%lf%lf%lf%lf%lf%lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5])==6 )
						{
							imgPos.lfImgMatrix[0] = v[0]; 
							imgPos.lfImgMatrix[1] = v[1]; 
							imgPos.lfImgMatrix[2] = v[2]; 
							imgPos.lfImgMatrix[3] = v[3]; 
							imgPos.lfImgLayOX = v[4];
							imgPos.lfImgLayOY = v[5];
							imgPos.lfPixelSizeX = sqrt(v[0]*v[0] + v[1]*v[1]);
							imgPos.lfPixelSizeY = sqrt(v[2]*v[2] + v[3]*v[3]);
							
						}
						fclose(fp);
					}
					
				}
				
			}
				
			
			m_arrImgPos.Add(imgPos);

		}
		if (arr.GetSize()>=0)
		{
			temp = arr[0];
			if (m_nMainImageIdx==-1)
			{
				m_nMainImageIdx = 0;
			}
		}
		else
			temp = _T("");
		
	}

	if (!temp.IsEmpty())
	{
		m_bVisible = TRUE;		
		RefreshUIWithOption(temp);
	}	
	if(failedFile.GetSize()>0)
	{
		CString strMessage;
		for (int i=0;i<failedFile.GetSize();i++)
		{
			strMessage+=failedFile[i];
			strMessage+=_T("\n");
		}
		strMessage+=StrFromResID(IDS_ERR_READIMGGEOINFO);
		AfxMessageBox(strMessage);
	}
}

void CDlgOverMutiImgLay::OnButtonIdBrowseAny() 
{
	CString filter(StrFromResID(IDS_LOADTIF_FILTER));
	CString aaa = StrFromResID(IDS_SELECT_FILE);
	CFileDialogEx dlg(aaa,TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		filter);

	dlg.m_ofn.nMaxFile = 1024;
	CHAR strFile[1024*MAX_PATH] = { 0 };
	dlg.m_ofn.lpstrFile = strFile;
	if( dlg.DoModal()!=IDOK )return;

	CStringArray arr;

//	if( dlg.GetPage()==CFileDialogEx::UIFFileRecent )
//	{
//		arr.Add(dlg.GetPathName());
//	}
//	else
	{
		POSITION   pos   =   dlg.GetStartPosition();  
		while   (pos   !=   NULL)  
		{  
			arr.Add(dlg.GetNextPathName(pos));  
		}
	}
	
//	UpdateData(TRUE);

	if(arr.GetSize()<=0)return;

	CStringArray failedFile;
	CString temp;

	{
		CStringArray arr0;
		arr0.Copy(arr);		
		for (int i=arr0.GetSize()-1;i>=0;i--)
		{
			if(IsExistInStrArray(arr0.GetAt(i),m_arrImgPos))
				arr0.RemoveAt(i);
		}
		arr0.FreeExtra();
		ViewImgPosition imgPos;

		for (i=0;i<arr0.GetSize();i++)
		{
			strcpy(imgPos.fileName, arr0[i]);

			BOOL bOK = FALSE;
			CString tifrefname = arr0[i];
			if (tifrefname.Right(3).CompareNoCase(_T("tif")) == 0)
				tifrefname = tifrefname.Left(tifrefname.GetLength() - 3) + _T("tifref");
			else
				tifrefname += _T(".tifref");

			FILE *fp = fopen(tifrefname, "r");
			if( fp )
			{
				double v[6];
				if( fscanf(fp,"%lf%lf%lf%lf%lf%lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5])==6 )
				{
					imgPos.lfImgMatrix[0] = v[0]; 
					imgPos.lfImgMatrix[1] = v[1]; 
					imgPos.lfImgMatrix[2] = v[2]; 
					imgPos.lfImgMatrix[3] = v[3]; 
					imgPos.lfImgLayOX = v[4];
					imgPos.lfImgLayOY = v[5];
					imgPos.lfPixelSizeX = sqrt(v[0]*v[0] + v[1]*v[1]);
					imgPos.lfPixelSizeY = sqrt(v[2]*v[2] + v[3]*v[3]);
					bOK = TRUE;
				}
				fclose(fp);
			}

			if (!bOK)
			{
				CString name_tfw = arr0[i];
				if (name_tfw.Right(3).CompareNoCase(_T("tif")) == 0)//tiff
				{
					name_tfw = name_tfw.Left(name_tfw.GetLength() - 3) + _T("tfw");
					FILE *fp1 = fopen(name_tfw, "r");
					if (fp1)
					{
						double v[6];
						if (fscanf(fp1, "%lf%lf%lf%lf%lf%lf", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) == 6)
						{
							imgPos.lfImgMatrix[0] = v[0];
							imgPos.lfImgMatrix[1] = v[1];
							imgPos.lfImgMatrix[2] = v[2];
							imgPos.lfImgMatrix[3] = v[3];
							imgPos.lfImgLayOX = v[4];
							imgPos.lfImgLayOY = v[5];
							imgPos.lfPixelSizeX = sqrt(v[0] * v[0] + v[1] * v[1]);
							imgPos.lfPixelSizeY = sqrt(v[2] * v[2] + v[3] * v[3]);
							bOK = TRUE;
						}
						fclose(fp1);
					}
				}
			}

			if (!bOK)
			{
				GDALDataset *pdataset = (GDALDataset *)GDALOpen(arr0[i], GA_ReadOnly);
				if (pdataset)
				{
					double	v[6] = { 0 };
					GDALGetGeoTransform((GDALDatasetH)pdataset, v);

					int nSizeY = GDALGetRasterYSize((GDALDatasetH)pdataset);

					imgPos.lfImgMatrix[0] = v[1];
					imgPos.lfImgMatrix[1] = v[2];
					imgPos.lfImgMatrix[2] = v[4];
					imgPos.lfImgMatrix[3] = v[5];
					imgPos.lfPixelSizeX = sqrt(v[1] * v[1] + v[2] * v[2]);
					imgPos.lfPixelSizeY = sqrt(v[4] * v[4] + v[5] * v[5]);
					imgPos.lfImgLayOX = v[0];
					imgPos.lfImgLayOY = v[3] + (nSizeY - 1)*v[5];
					GDALClose((GDALDatasetH)pdataset);
					bOK = TRUE;
				}
			}
				
			if (!bOK)
			{
				failedFile.Add(arr0[i]);
			}

			m_arrImgPos.Add(imgPos);
		}

		if (arr0.GetSize()>0)
		{
			temp = arr0[0];
			if (m_nMainImageIdx==-1)
			{
				m_nMainImageIdx = 0;
			}
		}
		else
		    temp = _T("");
	}	
	
	if (!temp.IsEmpty())
	{
		m_bVisible = TRUE;
		RefreshUIWithOption(temp);
	}	
	if(failedFile.GetSize()>0)
	{
		CString strMessage;
		for (int i=0;i<failedFile.GetSize();i++)
		{
			strMessage+=failedFile[i];
			strMessage+=_T("\n");
		}
		strMessage+=StrFromResID(IDS_ERR_READIMGGEOINFO);
		AfxMessageBox(strMessage);
	}
}

void CDlgOverMutiImgLay::OnButtonUnload()
{
	int nCount = m_listMutiImg.GetSelCount();
	if(nCount==0)return;
	CArray<int, int> aryListBoxSel;
	
	aryListBoxSel.SetSize(nCount);
	m_listMutiImg.GetSelItems(nCount, aryListBoxSel.GetData());
	CStringArray arr;
	CString temp;
	for (int i=0;i<nCount;i++)
	{		
		temp = (LPCTSTR)m_listMutiImg.GetItemDataPtr (aryListBoxSel[i]);
		arr.Add(temp);
	}
	for (i=nCount-1;i>=0;i--)
	{
		m_listMutiImg.DeleteString(aryListBoxSel[i]);
	}
	m_nMainImageIdx = m_listMutiImg.GetMainItem();
	if(m_nMainImageIdx==-1)
	{
		m_arrImgPos.RemoveAll();
	}
	else
	{
		for (i = arr.GetSize()-1;i>=0;i-- )
		{
			CString strTemp = arr[i];	
			{
				int j;
				for (j=m_arrImgPos.GetSize()-1;j>=0;j--)
				{
					if (strTemp.CompareNoCase(m_arrImgPos[j].fileName)==0)
					{					
						break;
					}
				}
				m_arrImgPos.RemoveAt(j);
			}
		}
	}
	if (m_arrImgPos.GetSize()<=0)
	{
		m_lfMatrix1 = 1.0;
		m_lfMatrix2 = 0.0;
		m_lfMatrix3 = 0.0;
		m_lfMatrix4 = 1.0;
		m_lfXOff = 0.0;
		m_lfYOff = 0.0;	
		m_bVisible = FALSE;		
		UpdateData(FALSE);
	}
	else
	{		
		RefreshUIWithOption();
	}	
	
}

void CDlgOverMutiImgLay::OnButtonMapbound() 
{
	UpdateData(TRUE);
	int nCount = m_listMutiImg.GetSelCount();
	if(nCount==0)return;
	CArray<int, int> aryListBoxSel;
	
	aryListBoxSel.SetSize(nCount);
	m_listMutiImg.GetSelItems(nCount, aryListBoxSel.GetData());
	CString str;
	str = (LPCTSTR)m_listMutiImg.GetItemDataPtr (aryListBoxSel[0]);
	CSize szImage;
	CImageRead image/*,image0*/;
	if( image.Load(str) )
	{
		szImage = image.GetImageSize();
	}
	else
	{
		szImage.cx = szImage.cy = 0;
	}
// 	image0.Load(str);
// 	image.SetCurReadZoom(1.0);
// 	image.DeAttach();
// 	image.SetCurReadZoom(0.98);
// 	image.SetCurReadZoom(0.045);
// 	image.GetCurReadZoom();
// 	image.GetCurReadZoom();
// 	image.Attach();
	
	Envelope evlp;
	evlp.CreateFromPts(m_ptsBound,4);
	
	m_lfXOff = evlp.m_xl; m_lfYOff = evlp.m_yl;
	UpdateData(FALSE);
	if( evlp.IsEmpty() || szImage.cx==0 || szImage.cy==0 )
	{
		m_lfMatrix1 = 1.0; m_lfMatrix4 = 1.0;
		m_lfMatrix2 = 0.0; m_lfMatrix3 = 0.0;
	}
	else
	{
		m_lfMatrix1 = (evlp.m_xh-evlp.m_xl)/szImage.cx;
		m_lfMatrix4 = (evlp.m_yh-evlp.m_yl)/szImage.cy;
		m_lfMatrix2 = 0;
		m_lfMatrix3 = 0;
	}
	UpdateData(FALSE);
	SaveImgPos(str);
	UpdateData(FALSE);
}

void CDlgOverMutiImgLay::SaveImgPos(LPCTSTR fileName)
{
	CString str;
	if (fileName==NULL)
	{
		str = m_curSelFileName;
	}
	else
	    str = fileName;
	UpdateData(TRUE);
	for (int i=0;i<m_arrImgPos.GetSize();i++)
	{
		if (str.CompareNoCase(m_arrImgPos[i].fileName)==0)
		{
			m_arrImgPos[i].lfImgLayOX = m_lfXOff;
			m_arrImgPos[i].lfImgLayOY = m_lfYOff;
			m_arrImgPos[i].lfImgMatrix[0] = m_lfMatrix1;
			m_arrImgPos[i].lfImgMatrix[1] = m_lfMatrix2;
			m_arrImgPos[i].lfImgMatrix[2] = m_lfMatrix3;
			m_arrImgPos[i].lfImgMatrix[3] = m_lfMatrix4;
			m_arrImgPos[i].lfPixelSizeX = m_lfPixelSizeX;
			m_arrImgPos[i].lfPixelSizeY = m_lfPixelSizeY;
			m_arrImgPos[i].nPixelBase = m_nPixelBase;
			m_arrImgPos[i].nType = 0;
			return;
		}
	}
	
}

void CDlgOverMutiImgLay::OnButtonRecoverOriginalParameters()
{
	UpdateData(TRUE);
	int nCount = m_listMutiImg.GetSelCount();
	if(nCount==0)return;
	CArray<int, int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount);
	m_listMutiImg.GetSelItems(nCount, aryListBoxSel.GetData());
	CString str;
	str = (LPCTSTR)m_listMutiImg.GetItemDataPtr (aryListBoxSel[0]);
	m_nMainImageIdx = m_listMutiImg.GetMainItem();
	CGeoBuilderPrj prj;
	GeoHeader head;
	if (prj.IsGeoImage(str,head) )
	{

		ViewImgPosition imgPos;
		FILE *fp;
		CString name_tfw(str);
		name_tfw = name_tfw.Left(name_tfw.GetLength()-3) + _T("tfw");
		fp = fopen(name_tfw,"r");
		if (!fp)
		{
			return;
		}
		double v[6];
		if( fscanf(fp,"%lf%lf%lf%lf%lf%lf",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5])==6 )
		{
			imgPos.lfImgMatrix[0] = v[0]; 
			imgPos.lfImgMatrix[1] = v[1]; 
			imgPos.lfImgMatrix[2] = v[2]; 
			imgPos.lfImgMatrix[3] = v[3]; 
			imgPos.lfImgLayOX = v[4];
			imgPos.lfImgLayOY = v[5];
			imgPos.lfPixelSizeX = sqrt(v[0]*v[0] + v[1]*v[1]);
			imgPos.lfPixelSizeY = sqrt(v[2]*v[2] + v[3]*v[3]);	
			m_lfMatrix1 = imgPos.lfImgMatrix[0];
			m_lfMatrix2 = imgPos.lfImgMatrix[1];
			m_lfMatrix3 = imgPos.lfImgMatrix[2];
			m_lfMatrix4 = imgPos.lfImgMatrix[3];
			m_lfXOff =imgPos.lfImgLayOX;
			m_lfYOff = imgPos.lfImgLayOY;	
			m_bVisible = TRUE;
			UpdateData(FALSE);	
			SaveImgPos(str);
		}
		fclose(fp);
	}
	else//找不到参考文件
	{
		CString strTip(str);
		strTip+=StrFromResID(IDS_ERR_READIMGGEOINFO);
		GOutPut(strTip);
	}

	

}
void CDlgOverMutiImgLay::OnSelchangeListMutiimgFilename() 
{
	int nCount = m_listMutiImg.GetSelCount();
	if(nCount<=0)return;
	CArray<int,int> aryListBoxSel;
	
	aryListBoxSel.SetSize(nCount);
	m_listMutiImg.GetSelItems(nCount, aryListBoxSel.GetData());
    
	//
	((CButton*)GetDlgItem(IDC_CHECK_VISIBLE))->SetCheck(m_arrImgPos[aryListBoxSel[0]].is_visible);
	m_bVisible = m_arrImgPos[aryListBoxSel[0]].is_visible;
	//

	CString temp;
	temp = (LPCTSTR)m_listMutiImg.GetItemDataPtr (aryListBoxSel[0]);
	if (temp.CompareNoCase(m_curSelFileName)==0) return;
	UpdateData(TRUE);
	SaveImgPos(m_curSelFileName);
	m_curSelFileName = temp;

	RefreshUIForSelChange(m_curSelFileName);	
}


void CDlgOverMutiImgLay::OnButtonVisible()
{
	int nCount = m_listMutiImg.GetSelCount();
	if(nCount<=0)return;

	CArray<int,int> aryListBoxSel;
	aryListBoxSel.SetSize(nCount);
	m_listMutiImg.GetSelItems(nCount, aryListBoxSel.GetData());
    //
	m_bVisible = !m_arrImgPos[aryListBoxSel[0]].is_visible;
	((CButton*)GetDlgItem(IDC_CHECK_VISIBLE))->SetCheck(m_bVisible);
	//
	for(int i = 0; i<nCount; ++i)
	{
		m_arrImgPos[aryListBoxSel[i]].is_visible = m_bVisible;
	}
}

void CDlgOverMutiImgLay::OnButtonSetMainimg() 
{
	int nCount = m_listMutiImg.GetSelCount();
	if(nCount<=0)return;
	CArray<int,int> aryListBoxSel;
	
	aryListBoxSel.SetSize(nCount);
	m_listMutiImg.GetSelItems(nCount, aryListBoxSel.GetData());
	CString temp;
	temp = (LPCTSTR)m_listMutiImg.GetItemDataPtr (aryListBoxSel[0]);
	m_listMutiImg.SetMainItem(aryListBoxSel[0]);
	m_nMainImageIdx = m_listMutiImg.GetMainItem();

}

BOOL CDlgOverMutiImgLay::OnInitDialog() 
{	
	CDialog::OnInitDialog();
	CGeoBuilderPrj *prj = NULL;
	AfxGetMainWnd()->SendMessage(FCCM_GETPROJECT,0,(LPARAM)&prj);
	if( !prj || prj->strCurXML.GetLength()<=0 )
	{
		CWnd *pWnd = GetDlgItem(IDC_BUTTON_BROWSE_WORKSPACE);
		if( pWnd )pWnd->EnableWindow(FALSE);
	}
	if(m_curSelFileName.IsEmpty())
		RefreshUIWithOption();
	else
		RefreshUIWithOption(m_curSelFileName);
	
	return TRUE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgOverMutiImgLay::OnOK() 
{	
	CDialog::OnOK();
	if(m_nMainImageIdx==-1)
	{
		m_curSelFileName.Empty();
	}
	else
		SaveImgPos(m_curSelFileName);
}
