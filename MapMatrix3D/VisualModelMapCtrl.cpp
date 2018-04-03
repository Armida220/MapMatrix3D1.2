// VisualModelMapCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "VisualModelMapCtrl.h"
#include "CoordCenter.h"
#include "SmartViewFunctions.h"
#include "ProjectViewBar.h "

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_DRAWGRAPH_STATIC 1
#define IDC_TIPINFO_EDIT     2




CStereoBoundFile::CStereoBoundFile()
{
	m_bLoadOK = FALSE;
}


CStereoBoundFile::~CStereoBoundFile()
{
	
}

BOOL CStereoBoundFile::Load(LPCTSTR prj_path)
{
	m_strPath = prj_path;
	m_strPath += ".bound";

	char line[1024];
	FILE *fp = fopen(m_strPath,"rb");
	if( !fp )
		return FALSE;

	m_arrStereos.RemoveAll();
	m_arrBounds.RemoveAll();

	int read_step = 0;

	while( !feof(fp) )
	{
		memset(line,0,sizeof(line));
		fgets(line,sizeof(line),fp);

		int nlen = strlen(line);
		if( nlen<=0 )
			continue;

		//去除行尾的\r\n
		if( line[nlen-1]=='\r' || line[nlen-1]=='\n' )
		{
			line[nlen-1] = '\0';
			nlen--;
		}
		if( line[nlen-1]=='\r' || line[nlen-1]=='\n' )
		{
			line[nlen-1] = '\0';
			nlen--;
		}

		if( read_step==0 )
		{
			m_arrStereos.Add(CString(line));
			read_step = 1;
		}
		else if( read_step==1 )
		{
			PT_3D pts[4];
			if( sscanf(line,"%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",
				&pts[0].x,&pts[0].y,&pts[0].z,
				&pts[1].x,&pts[1].y,&pts[1].z,
				&pts[2].x,&pts[2].y,&pts[2].z,
				&pts[3].x,&pts[3].y,&pts[3].z)==12 )
			{
				m_arrBounds.Add(pts[0]);
				m_arrBounds.Add(pts[1]);
				m_arrBounds.Add(pts[2]);
				m_arrBounds.Add(pts[3]);
				read_step = 0;
			}
		}		
	}

	if( read_step==1 )
	{
		m_arrStereos.RemoveAt(m_arrStereos.GetSize()-1);
	}

	m_bLoadOK = TRUE;
	
	fclose(fp);
	
	return m_bLoadOK;
}


BOOL CStereoBoundFile::Save()
{
	if( m_arrStereos.GetSize()<=0 )
		return FALSE;

	FILE *fp = fopen(m_strPath,"wt");
	if( !fp )return FALSE;

	PT_3D *pts = m_arrBounds.GetData();

	for( int i=0; i<m_arrStereos.GetSize(); i++)
	{
		fprintf(fp,"%s\n",(LPCTSTR)m_arrStereos[i]);
		fprintf(fp,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
			pts[0].x,pts[0].y,pts[0].z,
			pts[1].x,pts[1].y,pts[1].z,
			pts[2].x,pts[2].y,pts[2].z,
			pts[3].x,pts[3].y,pts[3].z);

		pts += 4;
	}
	
	fclose(fp);
	
	return TRUE;
}


BOOL CStereoBoundFile::Find(LPCTSTR stereoID, PT_3D pts_ret[4])
{
	PT_3D *pts = m_arrBounds.GetData();

	for( int i=0; i<m_arrStereos.GetSize(); i++)
	{
		if( m_arrStereos[i].CompareNoCase(stereoID)==0 )
		{
			memcpy(pts_ret,pts,sizeof(PT_3D)*4);			

			return TRUE;
		}		
		pts += 4;
	}

	return FALSE;
}

BOOL CStereoBoundFile::Add(LPCTSTR stereoID, PT_3D pts[4])
{
	PT_3D *pts1 = m_arrBounds.GetData();
	
	for( int i=0; i<m_arrStereos.GetSize(); i++)
	{
		if( m_arrStereos[i].CompareNoCase(stereoID)==0 )
		{
			memcpy(pts1,pts,sizeof(PT_3D)*4);			
			
			return TRUE;
		}		
		pts1 += 4;
	}

	m_arrStereos.Add(CString(stereoID));
	m_arrBounds.Add(pts[0]);
	m_arrBounds.Add(pts[1]);
	m_arrBounds.Add(pts[2]);
	m_arrBounds.Add(pts[3]);
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CVisualModelMapCtrl

#define REFRESHWINDOW 2

#define WM_TEST_SWITCHMODEL		(WM_USER+1)

CVisualModelMapCtrl::CVisualModelMapCtrl()
{
//	m_bIsDisplayForStart=true;
	m_bIsMove=false;
}

CVisualModelMapCtrl::~CVisualModelMapCtrl()
{
}


BEGIN_MESSAGE_MAP(CVisualModelMapCtrl, CWnd)
	//{{AFX_MSG_MAP(CVisualModelMapCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_TEST_SWITCHMODEL,OnTestSwitchModel)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVisualModelMapCtrl message handlers

void CVisualModelMapCtrl::AdjustLayout()
{
	if (GetSafeHwnd () == NULL)
	{
		return;
	}
	int cyTlb = 25;
	CRect rectClient, rect;
	GetClientRect (&rectClient);
	int t=rectClient.Width();
	int r=	rectClient.Height()-cyTlb;
	
	m_staticDrawGraph.SetWindowPos (NULL,
		rectClient.left, 
		rectClient.top,
		rectClient.Width(),
		rectClient.Height()-cyTlb,
		SWP_NOACTIVATE|SWP_NOZORDER);

	CBitmap bm;
	bm.LoadBitmap(IDB_BUTTON_MAPINFO);
	BITMAP bit;
	bm.GetBitmap(&bit);
	m_staticDrawGraph.GetClientRect(&rect);

	//设置按钮区域
	CRect temp,temp1;
	temp.left=rect.left+(rect.Width()-bit.bmWidth)/2;
	temp.top=rect.bottom-bit.bmHeight-10;
	temp.right=rect.left+(rect.Width()-bit.bmWidth)/2+bit.bmWidth;
	temp.bottom=rect.bottom-bit.bmHeight-10+bit.bmHeight;
	m_staticDrawGraph.SetButtonRect(temp);
    

	//确定画图区域
	temp1.left=rect.left+4;
	temp1.right=rect.right-4;
	temp1.top=rect.top+2;
	temp1.bottom=temp.top-4;
	m_staticDrawGraph.SetDrawArea(temp1);

	
	m_editTipInfo.SetWindowPos (NULL,
		rectClient.left, 
		rectClient.top+rectClient.Height()-cyTlb,
		rectClient.Width(),
		cyTlb,
		SWP_NOACTIVATE|SWP_NOZORDER);	

}



int CVisualModelMapCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CFont*pFont = (((CProjectViewBar*)GetParent())->m_wndAttribView).GetFont();
	LOGFONT lofont;
	pFont->GetLogFont(&lofont);
	if (HFONT(m_font)==NULL)
	{
		m_font.CreateFontIndirect(&lofont);
	} 		
	
	// TODO: Add your specialized creation code here
	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	
	if( !m_staticDrawGraph.Create(NULL,WS_CHILD|WS_VISIBLE|SS_BLACKRECT|SS_NOTIFY,
		rectDummy,this,IDC_DRAWGRAPH_STATIC) )
	{
		TRACE0("Failed to create MapGraph\n");
		return -1;      // fail to create
	}
	
	
	const DWORD dwViewStyle =WS_CHILD | WS_VISIBLE|ES_AUTOHSCROLL|ES_CENTER   ; 								
							
	
	if (!m_editTipInfo.Create (dwViewStyle, rectDummy, this, IDC_TIPINFO_EDIT))
	{
		TRACE0("Failed to create imformation edit\n");
		return -1;      // fail to create
	}
	m_editTipInfo.SetFont(&m_font);
	m_editTipInfo.SetReadOnly();
	SetTimer(REFRESHWINDOW,200,NULL);
	return 0;
}

LRESULT CVisualModelMapCtrl::OnRefreshVecWin(WPARAM wParam, LPARAM lParam)
{
	PT_3D *  pPts=(PT_3D*)lParam;

    PT_2D pts[4];
	COPY_2DPT((pts[0]),pPts[0]);
	COPY_2DPT((pts[1]),pPts[1]);
	COPY_2DPT((pts[2]),pPts[2]);
	COPY_2DPT((pts[3]),pPts[3]);
	SetCurrentVecWin(pts[0],pts[1],pts[2],pts[3]);
	m_staticDrawGraph.SetCurMSFlag(false);
	m_staticDrawGraph.SetCurStereoWinFlag(false);
	m_staticDrawGraph.Invalidate();
    return 1;
}

LRESULT CVisualModelMapCtrl::OnUnInstallProj(WPARAM wParam, LPARAM lParam)
{
	
	//by shy
	SetMsArrayFlag(false);
	SetVecWinFlag(false);
	SetCurStereoWinFlag(false);
	SetCurMSFlag(false);
	SetNearstMSFlag(false);
	ClearMSArray();
	m_editTipInfo.SetWindowText("");
	Invalidate();
	return 1;
}



LRESULT CVisualModelMapCtrl::OnRefreshStereoMS(WPARAM wParam, LPARAM lParam)
{

	CString stereoID = *((CString*)lParam);
	int stereoNum=m_staticDrawGraph.FindMS(stereoID);	

	SetCurrentMS(stereoNum);
	m_staticDrawGraph.Invalidate();
	return 1;
}

LRESULT CVisualModelMapCtrl::OnSwitchStereoMS(WPARAM wParam, LPARAM lParam)
{
	m_staticDrawGraph.SwitchModel();
	return 1;
}

LRESULT CVisualModelMapCtrl::OnRefreshStereoWin(WPARAM wParam, LPARAM lParam)
{
	
	PT_3D *pPts = (PT_3D *)lParam;
	PT_2D pts[4];
	COPY_2DPT((pts[0]),pPts[0]);
	COPY_2DPT((pts[1]),pPts[1]);
	COPY_2DPT((pts[2]),pPts[2]);
	COPY_2DPT((pts[3]),pPts[3]);
	SetCurrentStereoWin(pts[0],pts[1],pts[2],pts[3]);

	m_bIsMove = true;

	PostMessage(WM_TEST_SWITCHMODEL,0,0);

	return 1;
	
}

void CVisualModelMapCtrl::AddModelScope(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4,int index)
{
	m_staticDrawGraph.AddModelScope(p1,p2,p3,p4,index);

}

void CVisualModelMapCtrl::SetCurrentVecWin(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4)
{
	m_staticDrawGraph.SetCurrentVecWin(p1,p2,p3,p4);

}
void CVisualModelMapCtrl::SetCurrentStereoWin(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4)
{
	m_staticDrawGraph.SetCurrentStereoWin(p1,p2,p3,p4);
}
void CVisualModelMapCtrl::SetCurrentMS(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4)
{
	m_staticDrawGraph.SetCurrentMS(p1,p2,p3,p4);

}
void CVisualModelMapCtrl::SetNearestMS(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4)
{
	m_staticDrawGraph.SetNearestMS(p1,p2,p3,p4);
}

void CVisualModelMapCtrl::InitialDisplay()
{
 	m_staticDrawGraph.ShowWholeMap();
}



void CVisualModelMapCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

}

void CVisualModelMapCtrl::SetModelScopeArray( CoreObject *pCoreObj, BOOL bReadBoundFile)
{
	CoreObject coreObj = *pCoreObj;
	PT_4D pts[4];
	PT_3D pts2[4];
	CSize	size;
	CString strMdlImgFile[2];

	CStereoBoundFile boundfile;

	boundfile.Load(pCoreObj->pp.strPrjPath + "\\" + pCoreObj->pp.strPrjName);
	
	for (int k=0;k<coreObj.iStereoNum;k++)
	{
		strMdlImgFile[0] = coreObj.stereo[k].se.strImage[0];
		strMdlImgFile[1] = coreObj.stereo[k].se.strImage[1];
	
		BOOL bEpip = TRUE;

		if (strMdlImgFile[0].IsEmpty() && strMdlImgFile[1].IsEmpty())
		{
			bEpip = FALSE;
		}
		else if (strMdlImgFile[0].Left(6).CompareNoCase("uis://") == 0 && strMdlImgFile[1].Left(6).CompareNoCase("uis://") == 0)
		{
			bEpip = FALSE;
		}
		else if(_access(strMdlImgFile[0],0)==-1||_access(strMdlImgFile[1],0)==-1) 
		{
			CoreObject *pCore = &coreObj;
			CString strId1 = pCore->stereo[k].imageID[0];
			CString strId2 = pCore->stereo[k].imageID[1];
			BOOL bFind1=FALSE, bFind2=FALSE;
			for( int i=0; i<pCore->iStripNum; i++)
			{
				for( int j=0; j<pCore->strip[i].iImageNum; j++)
				{
					if( !bFind1 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId1)==0 )
					{
						strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName;
						//如果存在分块核线格式，就用该格式
						if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
							strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
						else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
							strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
						else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
							strMdlImgFile[0] = pCore->strip[i].image[j].ie.strFileName+_T(".ei");
						
						bFind1 = TRUE;
					}
					
					if( !bFind2 && pCore->strip[i].image[j].strImageID.CompareNoCase(strId2)==0 )
					{
						strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName;
						//如果存在分块核线格式，就用该格式
						if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eir"),0)!=-1 )
							strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".eir");
						else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".eil"),0)!=-1 )
							strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".eil");
						else if( _access(pCore->strip[i].image[j].ie.strFileName+_T(".ei"),0)!=-1 )
							strMdlImgFile[1] = pCore->strip[i].image[j].ie.strFileName+_T(".ei");
						
						bFind2 = TRUE;
					}
				}
			}
			
			if (strMdlImgFile[0].Left(6).CompareNoCase("uis://") != 0)
			{
				if (!(bFind1 && _access(strMdlImgFile[0], 0) != -1))
					continue;
			}
			
			if (strMdlImgFile[1].Left(6).CompareNoCase("uis://") != 0)
			{
				if (!(bFind2 && _access(strMdlImgFile[1], 0) != -1))
					continue;
			}

			bEpip = FALSE;
		}

		BOOL bFromBoundFile = TRUE;

		//先从临时存储文件中找范围信息
		if( !bReadBoundFile || !boundfile.Find(coreObj.stereo[k].sp.stereoID,pts2) )
		{
			bFromBoundFile = FALSE;

			CCoordCenter *pConvert=new CCoordCenter;
			if (pConvert==NULL)
			{
				return;
			}
			
			bool bret = pConvert->Init(coreObj, (coreObj.stereo[k].sp).stereoID);
			bool bOK = false;	
			
			if( bEpip )
			{
				size.cy = coreObj.stereo[k].sp.eb.iHeight;
				size.cx = coreObj.stereo[k].sp.eb.iWidth;
				double z0 = pConvert->GetAverageHeight();
				
				//赋予合适的影像坐标
				int sx=0, ex=0, sy=0, ey=0;
				
				sx = 10;
				ex = size.cx-10;
				sy = 10;
				ey = size.cy-10;
				
				pts[0].x = pts[1].x = sx;			
				pts[3].x = pts[2].x = ex;
				pts[0].y = pts[3].y = sy;
				pts[2].y = pts[1].y = ey;
				pts[0].z = pts[1].z = pts[0].x;			
				pts[3].z = pts[2].z = pts[3].x;
				
				Coordinate coord1,coord2; 
				bOK = true;
				for ( int j=0;j<4; j++)
				{
					coord1.lx=pts[j].x; coord1.ly=pts[j].y;
					coord1.rx=pts[j].z; coord1.ry=pts[j].y;
					
					coord1.iType=EPIPOLAR;
					coord2.iType=GROUND;
					
					if( pConvert->Convert(coord1,coord2) )
					{
						pts2[j].x = coord2.x;
						pts2[j].y = coord2.y;
						pts2[j].z = coord2.z;
					}
					else 
					{
						bOK = false;
						break;
					}
					
				}
			}
			
			if (bOK && GraphAPI::GIsClockwise(pts2,4)!=-1 )
			{
				
			}
			else
			{
				QUADRANGLE quad;
				pConvert->GetModelBound(&quad);
				for( int j=0; j<4; j++)
				{
					pts2[j].x = quad.lfX[j];
					pts2[j].y = quad.lfY[j];
				}
			}
			
			delete pConvert;
		}
		
		if (GraphAPI::GIsClockwise(pts2,4)!=-1 )
		{
			AddModelScope(pts2[0],pts2[1],pts2[2],pts2[3],k);
			if( !bFromBoundFile )
			{
				boundfile.Add(coreObj.stereo[k].sp.stereoID,pts2);
			}
		}
		
	}
	m_staticDrawGraph.CalcCoreModelScope();
	
	//将核心对象保存到m_staticDrawGraph中，避免每次都要很麻烦的获得
	m_staticDrawGraph.SetCoreObj(&coreObj);
	m_staticDrawGraph.SetMsArrayFlag(true);

	boundfile.Save();
	
}

void CVisualModelMapCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default	
	CWnd::OnTimer(nIDEvent);
}


void CVisualModelMapCtrl::SetMsArrayFlag(bool bIsExist)
{
	m_staticDrawGraph.SetMsArrayFlag(bIsExist);
}
void CVisualModelMapCtrl::SetVecWinFlag(bool bIsExist)
{
	m_staticDrawGraph.SetVecWinFlag(bIsExist);
}
void CVisualModelMapCtrl::SetCurStereoWinFlag(bool bIsExist)
{
	m_staticDrawGraph.SetCurStereoWinFlag(bIsExist);
}
void CVisualModelMapCtrl::SetCurMSFlag(bool bIsExist)
{
	m_staticDrawGraph.SetCurMSFlag(bIsExist);
}

LRESULT CVisualModelMapCtrl::OnRefreshVecFlag(WPARAM wParam, LPARAM lParam)
{
     SetVecWinFlag(false);
	 return true;
}

LRESULT CVisualModelMapCtrl::OnGetToler(WPARAM wParam, LPARAM lParam)
{
	double width=*((double*)wParam);
	double height=*((double*)lParam);
	double toler = (width + height) / 10;
	if (toler > 100.0) toler = 100.0;
	m_staticDrawGraph.SetTolerForStereoMove(toler);
	return true;
}

void CVisualModelMapCtrl::ClearMSArray()
{
	m_staticDrawGraph.ClearMSArray();

}

void CVisualModelMapCtrl::SetCurrentMS(int index)
{
	m_staticDrawGraph.SetCurrentMS(index);

}

HBRUSH CVisualModelMapCtrl::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// Are we painting the IDC_MYSTATIC control? We can use
	// CWnd::GetDlgCtrlID() to perform the most efficient test.
	if (pWnd->GetDlgCtrlID() == IDC_TIPINFO_EDIT)
	{
	
	}	
	return hbr;
}

void CVisualModelMapCtrl::SetMoveFlag(bool flag)
{
  m_bIsMove=flag;
}

void CVisualModelMapCtrl::SetNearstMSFlag(bool bIsExist)
{
  m_staticDrawGraph.SetNearstMSFlag(bIsExist);
}


LRESULT CVisualModelMapCtrl::OnTestSwitchModel(WPARAM wParam, LPARAM lParam)
{
	MSG	msg;
	while( PeekMessage(&msg,m_hWnd,WM_TEST_SWITCHMODEL,WM_TEST_SWITCHMODEL,PM_REMOVE) );

	if(m_bIsMove)
	{
		m_staticDrawGraph.RefreshView();
		m_staticDrawGraph.Invalidate();
		
		CString str;
		if(m_staticDrawGraph.GetNameForNearestMS(&str))
		{
			CString str1;
			str1.Format(IDS_NEARSTMS,str);
			m_editTipInfo.SetWindowText(str1);
			
		}
		else
		{
			str.Format(IDS_NONEARSTMS);		
			m_editTipInfo.SetWindowText(str);
			
		}
		m_bIsMove=false;
	}

	return 0;
}