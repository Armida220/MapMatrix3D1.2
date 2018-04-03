// SinglePhotoStereoView.cpp: implementation of the CSinglePhotoStereoView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditBase.h"
#include "SinglePhotoStereoView.h"

#include "DlgSelectPairPhoto.h"
#include "ExMessage.h"
#include "display.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CStereoParallaxFile::CStereoParallaxFile()
{
	m_bLoadOK = FALSE;
}


CStereoParallaxFile::~CStereoParallaxFile()
{

}

BOOL CStereoParallaxFile::Load(LPCTSTR leftimage, LPCTSTR rightimage)
{
	CString strLeft = leftimage, strRight = rightimage;
	CString title;
	int pos1 = strLeft.ReverseFind('\\');
	int pos2 = strLeft.ReverseFind('.');
	if( pos1<pos2 )title = strLeft.Mid(pos1+1,pos2-pos1-1) + "-";
	else title = strLeft.Mid(pos1+1) + "-";

	if( pos1>0 )m_strPath = strLeft.Left(pos1);
	else m_strPath.Empty();
	
	pos1 = strRight.ReverseFind('\\');
	pos2 = strRight.ReverseFind('.');
	if( pos1<pos2 )title += strRight.Mid(pos1+1,pos2-pos1-1);
	else title += strRight.Mid(pos1+1);

	m_strPath += title + "par";

	FILE *fp = fopen(m_strPath,"r");
	if( !fp )return FALSE;

	if( fscanf(fp,"%lf%lf%lf%lf",&m_curPt.x,&m_curPt.y,&m_curPt.z,&m_curPt.yr)==4 )
	{
		m_bLoadOK = TRUE;
	}
	else
		m_bLoadOK = FALSE;

	fclose(fp);

	return m_bLoadOK;
}


BOOL CStereoParallaxFile::Save()
{
	FILE *fp = fopen(m_strPath,"w");
	if( !fp )return FALSE;
	
	fprintf(fp,"%lf %lf %lf %lf",m_curPt.x,m_curPt.y,m_curPt.z,m_curPt.yr);
	
	fclose(fp);
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNCREATE(CSinglePhotoStereoView, CStereoView)

BEGIN_MESSAGE_MAP(CSinglePhotoStereoView, CStereoView)
	//{{AFX_MSG_MAP(CSinglePhotoStereoView)
	ON_MESSAGE(WM_STEREOVIEW_LOADMDL, OnLoadModel)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CSinglePhotoStereoView::CSinglePhotoStereoView()
{
	m_bAutoAdjustCoordMode = FALSE;
}

CSinglePhotoStereoView::~CSinglePhotoStereoView()
{

}


void CSinglePhotoStereoView::OnInitialUpdate()
{
	CStereoView::OnInitialUpdate();
}

LRESULT CSinglePhotoStereoView::OnLoadModel(WPARAM wParam, LPARAM lParam)
{
	CDlgSelectPairPhoto dlg;
	if( dlg.DoModal()!=IDOK )
		return 0;

	m_strMdlImgFile[0] = dlg.m_strLeftImage;
	m_strMdlImgFile[1] = dlg.m_strRightImage;

	m_nImageType = SCANNER;

	CString title;
	int pos1 = dlg.m_strLeftImage.ReverseFind('\\');
	int pos2 = dlg.m_strLeftImage.ReverseFind('.');
	if( pos1<pos2 )title = dlg.m_strLeftImage.Mid(pos1+1,pos2-pos1-1) + "-";
	else title = dlg.m_strLeftImage.Mid(pos1+1) + "-";

	pos1 = dlg.m_strRightImage.ReverseFind('\\');
	pos2 = dlg.m_strRightImage.ReverseFind('.');
	if( pos1<pos2 )title += dlg.m_strRightImage.Mid(pos1+1,pos2-pos1-1);
	else title += dlg.m_strRightImage.Mid(pos1+1);

	GetParentFrame()->SetTitle(title);
	GetParentFrame()->OnUpdateFrameTitle(TRUE);

	//获得当前窗口中点的地理坐标
	PT_4D pt0;
	PT_3D pt1;
	CRect rcClient;
	GetClientRect(&rcClient);
    pt0.x = rcClient.CenterPoint().x; pt0.y = rcClient.CenterPoint().y; pt0.z = pt0.x; pt0.yr = pt0.y;
	ClientToGround(&pt0,&pt1);
	pt1.z=m_gCurPoint.z;
		
	if( !LoadModel() )
	{
		//如果是新开一个立体窗口并且又无法加载该模型，就可以退出视图
		if( m_bFirstLoadModel )
			GetParentFrame()->PostMessage(WM_CLOSE);
		return 0;
	}

	m_bLoadConvert = FALSE;
	
	if( m_pConvert!=NULL )
		delete m_pConvert;
	m_pConvert = NULL;

	m_laymgrLeft.InsertDrawingLayer(&m_ImgLayLeft,0);
	m_laymgrRight.InsertDrawingLayer(&m_ImgLayRight,0);
	
	m_bUseStereoEvlp = FALSE;

	DefineAffineMatrix();

	m_pContext->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->m_fDrawCellKY = m_gsa.lfGKY;

	m_pContext->GetLeftContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetLeftContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetLeftContext()->m_fDrawCellKY = m_gsa.lfGKY;

	m_pContext->GetRightContext()->m_fDrawCellAngle = -m_gsa.lfAngle;
	m_pContext->GetRightContext()->m_fDrawCellKX = m_gsa.lfGKX;
	m_pContext->GetRightContext()->m_fDrawCellKY = m_gsa.lfGKY;

	//如果是在切换立体模型（不是第一次打开立体模型），我们需要将立体模型驱动到当前坐标上
	if( !m_bFirstLoadModel )
	{
		
		//将矢量重新叠加显示
		InitBmpLayer();

		LoadDocData();

		RecalcScrollBar(TRUE);

		SetFocus();

		//定位到当前位置（保持当前位置不变化）
		PT_3D pt3d = m_gCurPoint;
		DriveToXyz(&pt3d,CROSS_MODE_HEIGHT);

		GrBuffer buf;
		if( m_pBufConstDrag!=NULL )buf.CopyFrom(m_pBufConstDrag);
		SetConstDragLine(&buf);

		buf.DeleteAll();
		if(m_pBufVariantDrag!=NULL)buf.CopyFrom(m_pBufVariantDrag);
		SetVariantDragLine(&buf);

	//	UpdateVariantDragLine();
 		UpdateConstDragLine();
 		FinishUpdateDrawing();
	}
	
	m_bFirstLoadModel = FALSE;
	Invalidate(TRUE);

	return 0;
}

//将 x,y 从[-499999.99,500000.00]映射到[0,999999.99]，并编码到z的高8位和低8位(共16位有效数)
#define PLOW    -499999.00f
#define PHIGH	500000.00f

double EncodeParallax(double x, double y)
{
	if(x<PLOW)x = PLOW;
	if(x>PHIGH)x = PHIGH;
	x -= PLOW;
	long nx = (long)(x*100);

	if(y<PLOW)y = PLOW;
	if(y>PHIGH)y = PHIGH;
	y -= PLOW;

	//字符串化，然后交叉合并，这样即使运算的过程中出现了精度损失，高位数仍然比较安全
	long ny = (long)(y*100);

	char buf[128] = {0}, buf2[128]={0};
	sprintf(buf,"%08d",nx);
	sprintf(buf+20,"%08d",ny);

	for( int i=0; i<16; i++)
	{
		if( (i%2)==0 )
			buf2[i] = buf[i/2];
		else
			buf2[i] = buf[(i-1)/2+20];
	}

	for( i=0; i<16; i++)
	{
		if( buf2[i]!=0 )break;
	}

	if( i>=16 )return 0;

	double z = 0;
	sscanf(buf2+i,"%lf",&z);
	//return (x*1e+8) + y;
	return z;
}

void DecodeParallax(double z, double &x, double &y)
{
	char buf[128] = {0}, buf2[128]={0};
	sprintf(buf,"%16.0f",z);

	for( int i=0; i<16; i++)
	{
		if( (i%2)==0 )
			buf2[i/2] = buf[i];
		else
			buf2[(i-1)/2+20] = buf[i];
	}

	int nx = 0, ny = 0;

	for( i=0; i<8; i++)
	{
		if( buf2[i]!=0 )break;
	}
	if( i<8 )
		sscanf(buf2,"%d",&nx);

	for( i=0; i<8; i++)
	{
		if( buf2[i+20]!=0 )break;
	}
	if( i<8 )
		sscanf(buf2+20,"%d",&ny);

	//x = (int)(z*1e-8);
	//y = z-x*1e+8;

	x = nx*0.01 + PLOW; 
	y = ny*0.01 + PLOW;
}



BOOL CSinglePhotoStereoView::ConvertFailed(Coordinate Input, Coordinate &Output)
{
	if( Input.iType==GROUND )
	{
		Output.lx = Input.x;
		Output.ly = Input.y;

		DecodeParallax(Input.z,Output.rx,Output.ry);

		Output.rx += Output.lx;
		Output.ry += Output.ly;
	}
	else if( Output.iType==GROUND )
	{
		Output.x = Input.lx;
		Output.y = Input.ly;
		Output.z = EncodeParallax(Input.rx-Input.lx,Input.ry-Input.ly);
	}
	else
		return FALSE;

	return TRUE;
}



void CSinglePhotoStereoView::OnMouseMove(UINT nFlags, CPoint point) 
{
	PT_4D pt4d;
	pt4d.x = pt4d.z = point.x;
	pt4d.y = pt4d.yr = point.y;

	PT_3D pt3d;

	ClientToGround(&pt4d,&pt3d);
	GroundToClient(&pt3d,&pt4d);

	CStereoView::OnMouseMove(nFlags,point);
	return;
}