// DlgReorientation.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgReorientation.h"
#include "CoordCenter.h"
#include "SmartViewFunctions.h"
#include "Functions_temp.h"
#include "DlgDataSource.h"
#include "ExMessage.h"
#include "SQLiteAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static int gnCvtBaseType = PHOTO;


void ConvertDlg(CoreObject& core1, int nDlg1, CoreObject& core2);

/////////////////////////////////////////////////////////////////////////////
// CDlgReorientation dialog


CDlgReorientation::CDlgReorientation(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReorientation::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgReorientation)
	m_strPrjFile1 = _T("");
	m_strPrjFile2 = _T("");
	m_nCvtWay = 0;
	//}}AFX_DATA_INIT
}


void CDlgReorientation::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgReorientation)
	DDX_Control(pDX, IDC_LIST_DATAFILE, m_wndDataFile);
	DDX_Text(pDX, IDC_EDIT_PROJECTFILE1, m_strPrjFile1);
	DDX_Text(pDX, IDC_EDIT_PROJECTFILE2, m_strPrjFile2);
	DDX_CBIndex(pDX, IDC_COMBO_TYPE, m_nCvtWay);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgReorientation, CDialog)
	//{{AFX_MSG_MAP(CDlgReorientation)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnBrowse2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgReorientation message handlers

void CDlgReorientation::OnBrowse1() 
{
	CString filter(StrFromResID(IDS_LOADXML_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	m_strPrjFile1 = dlg.GetPathName();
	UpdateDataFileList();
	UpdateData(FALSE);
}

void CDlgReorientation::OnBrowse2() 
{
	CString filter(StrFromResID(IDS_LOADXML_FILTER));
	CFileDialogEx dlg(StrFromResID(IDS_SELECT_FILE),FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter);
	
	if( dlg.DoModal()!=IDOK )return;
	
	m_strPrjFile2 = dlg.GetPathName();
	UpdateDataFileList();
	UpdateData(FALSE);
}

BOOL CDlgReorientation::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgReorientation::UpdateDataFileList()
{
	if( m_strPrjFile1.GetLength()<=0 || m_strPrjFile2.GetLength()<=0 )
		return;

	CGeoBuilderPrj prj1, prj2;
	if( !prj1.LoadProject(m_strPrjFile1) || !prj2.LoadProject(m_strPrjFile2) )
		return;

	CoreObject core1 = prj1.GetCoreObject(), core2 = prj2.GetCoreObject();
	CStringArray arrDlgFile;
	CDWordArray arrDataItem;
	for( int i=0; i<core1.tmp.dlgs.iDlgNum; i++)
	{
		//是否含有相同的立体像对
		int m,n;
		for( m=0; m<core1.tmp.dlgs.dlg[i].iStereoNum; m++)
		{
			for( n=0; n<core2.iStereoNum; n++)
			{
				if( core1.tmp.dlgs.dlg[i].stereoID[m].CompareNoCase(core2.stereo[n].sp.stereoID)==0 )
					goto UpdateDataFileList_FindOver;
			}
		}
		if( m>=core1.tmp.dlgs.dlg[i].iStereoNum )continue;

UpdateDataFileList_FindOver:
		arrDlgFile.Add(core1.tmp.dlgs.dlg[i].strDlgFile);
		arrDataItem.Add(i);
	}

	CString str;
	m_wndDataFile.GetWindowText(str);
	m_wndDataFile.ResetContent();
	if( arrDlgFile.GetSize()<=0 )return;
	
	for( i=0; i<arrDlgFile.GetSize(); i++)
	{
		int idx = m_wndDataFile.AddString(arrDlgFile[i]);
		if( idx>=0 )m_wndDataFile.SetItemData(idx,arrDataItem[i]);
	}
	m_wndDataFile.SetSel(-1,TRUE);
}

void CDlgReorientation::OnOK() 
{
	UpdateData(TRUE);

	if( m_nCvtWay==0 )
		gnCvtBaseType = PHOTO;
	else
		gnCvtBaseType = SCANNER;

	if( m_wndDataFile.GetSelCount()>0 )
	{
		CGeoBuilderPrj prj1, prj2;
		if( !prj1.LoadProject(m_strPrjFile1) || !prj2.LoadProject(m_strPrjFile2) )
			return;

		CoreObject core1 = prj1.GetCoreObject(), core2 = prj2.GetCoreObject();
		int num = m_wndDataFile.GetCount();
		for( int i=0; i<num; i++)
		{
			if( m_wndDataFile.GetSel(i)>0 )
			{
				DWORD_PTR data = m_wndDataFile.GetItemData(i);
				int m = data;
				if( m>=0 && m<core1.tmp.dlgs.iDlgNum )
				{
					ConvertDlg(core1,m,core2);
				}
			}
		}
	}
	
	CDialog::OnOK();
}

extern int TestPoint(PT_3DEX *pt, Envelope e, CPtrArray& arr, CArray<QUADRANGLE,QUADRANGLE> &arrModelBounds, CUIntArray *arrsStereoIndexs);


static int TestPoint2(PT_3DEX *pt, Envelope e, CPtrArray& arr, CArray<QUADRANGLE,QUADRANGLE> &arrModelBounds, CUIntArray *arrsStereoIndexs)
{
	double minArea = -1;
	int num = arr.GetSize(), find = -1;
	CUIntArray arrSameAreaIndex;
	for( int i=0; i<num; i++)
	{
		CCoordCenter *pCenter = (CCoordCenter*)arr[i];
		if( !pCenter )continue;

		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));
		
		QUADRANGLE quad = arrModelBounds[i];

		/*RECT rect = pCenter->GetModelBound(&quad);*/
		
		for( int j=0; j<4; j++)
		{
			pt3ds[j].x = quad.lfX[j];
			pt3ds[j].y = quad.lfY[j];
			pt3ds[j].z = 0;
		}

		Envelope e0;
		e0.CreateFromPts(pt3ds,4,sizeof(PT_3D),3);
		
		//Envelope e0 = arrModelBounds[i];
		e0.Intersect(&e,2);

		double area = e0.Width()*e0.Width() + e0.Height()*e0.Height();
		if (minArea < 0 || area > minArea || fabs(area-minArea) < 1e-6)
		{
			minArea = area;
			find = i;

			if (!(fabs(area-minArea) < 1e-6))
			{
				arrSameAreaIndex.RemoveAll();
			}

			arrSameAreaIndex.Add(i);
		}
	}

	int areaSize = arrSameAreaIndex.GetSize();
	if (areaSize < 2)
	{
		return find;
	}

	double dis,min=-1;
	num = areaSize, find = -1;
	Coordinate coord1, coord2;
	COPY_3DPT(coord1,(*pt));
	coord1.iType = GROUND, coord2.iType = gnCvtBaseType;
	for( i=0; i<num; i++)
	{
		int index = arrSameAreaIndex[i];
		CCoordCenter *pCenter = (CCoordCenter*)arr[index];
		if( !pCenter )continue;
			
		if( !pCenter->Convert(coord1,coord2) )continue;

		PT_3D pt3ds[4];
		memset(pt3ds,0,sizeof(pt3ds));

		QUADRANGLE quad = arrModelBounds[index];		
		for( int j=0; j<4; j++)
		{
			pt3ds[j].x = quad.lfX[j];
			pt3ds[j].y = quad.lfY[j];
			pt3ds[j].z = 0;
		}

		double fWidth = GraphAPI::GGet2DDisOf2P(pt3ds[0],pt3ds[1]), fHeight = GraphAPI::GGet2DDisOf2P(pt3ds[1],pt3ds[2]);


		dis = coord2.lx*coord2.lx/(fWidth*fWidth)+coord2.ly*coord2.ly/(fHeight*fHeight);
		if( min<0 || dis<min )
		{
			min = dis;
			find = index;
		}
	}
	
	return find;
}


void ConvertDlg(CoreObject& core1, int nDlg1, CoreObject& core2)
{
	//备份Dlg文件
	CString bakFile = core1.tmp.dlgs.dlg[nDlg1].strDlgFile;
	int pos = bakFile.ReverseFind(_T('.'));
	if( pos>=0 )bakFile.Insert(pos,_T(".bak"));
	else bakFile += _T(".bak");
	::CopyFile(core1.tmp.dlgs.dlg[nDlg1].strDlgFile,bakFile,FALSE);
	CString curFile = core1.tmp.dlgs.dlg[nDlg1].strDlgFile;

	char database[256]={0};

	_CheckLoadPath((void*)(LPCTSTR)curFile,database,sizeof(database));

	CAccessModify *pAccess = new CSQLiteAccess;
	if( !pAccess )
	{
		return;
	}
	
	if( !pAccess->Attach(database) )
	{
		CString str = database;
		str = str+_T("\n")+StrFromResID(IDS_FILE_OPEN_ERR);
		AfxMessageBox(str);
		::DeleteFile(bakFile);
		delete pAccess;
		return;
	}
	
	CDlgDataSource *pDataSource = new CDlgDataSource(NULL);	
	if( !pDataSource )
	{
		
		CString str = database;
		str = str+_T("\n")+StrFromResID(IDS_INVALID_FILE);
		AfxMessageBox(str);
		::DeleteFile(bakFile);
		delete pAccess;
		return;
	}
	
	pDataSource->SetAccessObject(pAccess);
	pAccess->ReadDataSourceInfo(pDataSource);
	pAccess->BatchUpdateBegin();

	pDataSource->LoadAll(NULL);


	CString str = StrFromResID(IDS_PROCESS_FILE) + _T(": ") + database + _T("\n");
	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)str);

	//准备两套坐标转换器，它们之间相互对应
	int i,j;
	CPtrArray arr1, arr2;
	CCoordCenter *pCenter;
	arr1.SetSize(core1.tmp.dlgs.dlg[nDlg1].iStereoNum);
	arr2.SetSize(core1.tmp.dlgs.dlg[nDlg1].iStereoNum);

	CArray<QUADRANGLE,QUADRANGLE> arrModelBounds;

	for( i=0; i<core1.tmp.dlgs.dlg[nDlg1].iStereoNum; i++)
	{
		for( j=0; j<core2.iStereoNum; j++)
		{
			if( core1.tmp.dlgs.dlg[nDlg1].stereoID[i].CompareNoCase(core2.stereo[j].sp.stereoID)==0 )
			{
				pCenter = new CCoordCenter;
				if( pCenter )
				{
					pCenter->Init(core1,core1.tmp.dlgs.dlg[nDlg1].stereoID[i]);
					arr1.SetAt(i,pCenter);

					QUADRANGLE quad;
					pCenter->GetModelBound(&quad);
					
					/*PT_3D pt3ds[4];
					memset(pt3ds,0,sizeof(pt3ds));
					for( int j=0; j<4; j++)
					{
						pt3ds[j].x = quad.lfX[j];
						pt3ds[j].y = quad.lfY[j];
						pt3ds[j].z = 0;
					}
					
					Envelope e;
					e.CreateFromPts(pt3ds,4,sizeof(PT_3D),3);*/
					arrModelBounds.Add(quad);
				}
				else
				{
					arr1.SetAt(i,0);
					arrModelBounds.Add(QUADRANGLE());
				}

				pCenter = new CCoordCenter;
				if( pCenter )
				{
					pCenter->Init(core2,core2.stereo[j].sp.stereoID);
					arr2.SetAt(i,pCenter);
				}
				else
					arr2.SetAt(i,0);
				break;
			}
		}
	}

	//计数
	long lSum = 0;
	if( pDataSource )
	{
		int nlaynum = pDataSource->GetFtrLayerCount();
		for( int i=0; i<nlaynum; i++ )
		{
			CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(i);
			if( pLayer )
			{
				lSum += pLayer->GetObjectCount();
			}
		}
	}

	GProgressStart(lSum);
	
	//读取每个对象
	int laynum = pDataSource->GetFtrLayerCount();
	if( laynum<=0 )
	{
		delete pDataSource;
		delete pAccess;
		return;
	}
	
	for( i=0; i<laynum; i++)
	{
		CFtrLayer *pLayer = pDataSource->GetFtrLayerByIndex(i);
		if( !pLayer )continue;

		int objnum = pLayer->GetObjectCount();
		for( j=0; j<objnum; j++ )
		{
			//增长进度条
			GProgressStep();
			
			
			CFeature *pFtr = pLayer->GetObject(j);
			if (!pFtr) continue;
			CGeometry *pObj = pFtr->GetGeometry();
//			CGeometry *pObj = pLayer->GetObject(j)->GetGeometry();
			if( !pObj )continue;

			CArray<PT_3D,PT_3D> pt3ds;
			PT_3D pt3d;

			int ptnum = pObj->GetCtrlPointSum();
			pt3ds.SetSize(ptnum);
			for( int k=0; k<ptnum; k++)
			{
				pt3d = pObj->GetCtrlPoint(k);
				pt3ds.SetAt(k,pt3d);
			}

			//获得每个点
			CArray<PT_3DEX,PT_3DEX> arr;
			pObj->GetShape(arr);
			Envelope e = pObj->GetEnvelope();
			ptnum = arr.GetSize();
			PT_3DEX expt;
			for( k=0; k<ptnum; k++)
			{
				expt = arr[k];

				//判断该点位于哪个模型中
				int idx = TestPoint2(&expt,e,arr1,arrModelBounds,NULL);
				if( idx<0||idx>=core1.tmp.dlgs.dlg[nDlg1].iStereoNum )continue;

				//装入第二个工程中的这个模型
				if( arr2[idx]==NULL )continue;

				//转换这个点
				Coordinate coord1, coord2;
				COPY_3DPT(coord1,expt);
				coord1.iType = GROUND, coord2.iType = gnCvtBaseType;

				pCenter = (CCoordCenter*)arr1[idx];
				pCenter->Convert(coord1,coord2);

				pCenter = (CCoordCenter*)arr2[idx];
				pCenter->Convert(coord2,coord1);
				COPY_3DPT(expt,coord1);

				//修改点
				arr[k] = expt;
			}

			pObj->CreateShape(arr.GetData(),arr.GetSize());

			e = pObj->GetEnvelope();

			ptnum = pt3ds.GetSize();
			for( k=0; k<ptnum; k++)
			{
				pt3d = pt3ds.GetAt(k);
				COPY_3DPT(expt,pt3d);

				//判断该点位于哪个模型中
				int idx = TestPoint2(&expt,e,arr1,arrModelBounds,NULL);
				if( idx<0||idx>=core1.tmp.dlgs.dlg[nDlg1].iStereoNum )continue;
				
				//装入第二个工程中的这个模型
				if( arr2[idx]==NULL )continue;
				
				//转换这个点
				Coordinate coord1, coord2;
				COPY_3DPT(coord1,pt3d);
				coord1.iType = GROUND, coord2.iType = gnCvtBaseType;
				
				pCenter = (CCoordCenter*)arr1[idx];
				pCenter->Convert(coord1,coord2);
				
				pCenter = (CCoordCenter*)arr2[idx];
				pCenter->Convert(coord2,coord1);
				COPY_3DPT(pt3d,coord1);
				
				//修改点
				pObj->SetCtrlPoint(k,pt3d);
			}

//			pObj->UpdateData(TRUE);
		}
	}

	//进度条复位
	GProgressEnd();

	//释放坐标换算器
	for( i=0; i<core1.tmp.dlgs.dlg[nDlg1].iStereoNum; i++)
	{
		pCenter = (CCoordCenter*)arr1[i];
		if( pCenter )delete pCenter;

		pCenter = (CCoordCenter*)arr2[i];
		if( pCenter )delete pCenter;
	}

	//保存Dlg文件
	pDataSource->SaveAll(NULL);

	pAccess->BatchUpdateEnd();

	if (pDataSource) delete pDataSource;
	

	AfxGetMainWnd()->SendMessage(FCCM_PRINTSTR,0,(LPARAM)(LPCTSTR)StrFromResID(IDS_PROCESS_END) );
}
