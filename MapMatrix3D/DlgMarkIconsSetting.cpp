// DlgMarkIconsSetting.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "DlgMarkIconsSetting.h"

#include "DlgCellDefLinetypeView.h"
#include "CheckResultViewBar.h"
#include "dlgDataSource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern void GDI_DrawGrBuffer2d(HDC hdc, const GrBuffer2d *pBuf, BOOL bUseClr, COLORREF clr, double m[9], CSize szDC, CRect rcView, COLORREF backCol=RGB(255,255,255));


/////////////////////////////////////////////////////////////////////////////
// CDlgMarkIconsSetting dialog


CDlgMarkIconsSetting::CDlgMarkIconsSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMarkIconsSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMarkIconsSetting)
	m_fSize = 2.0f;
	//}}AFX_DATA_INIT

	m_nImageWid = m_nImageHei = 32;

	m_nScale = 2000;

	m_pDoc = NULL;
	m_pSettings = NULL;
	m_pSaveSettings = NULL;

	m_nCurSelCmd = m_nCurSelReason = -1;
}


CDlgMarkIconsSetting::~CDlgMarkIconsSetting()
{
	if( m_pSaveSettings )
		delete m_pSaveSettings;
}

void CDlgMarkIconsSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMarkIconsSetting)
	DDX_Control(pDX, IDC_LIST_INCONS, m_wndListIcons);
	DDX_Text(pDX, IDC_EDIT_SIZE, m_fSize);
	DDX_Control(pDX, IDC_LIST_CHKCMD, m_wndChkCmd);
	DDX_Control(pDX, IDC_LIST_RESULTS, m_wndReasons);
	DDX_Control(pDX, IDC_BUTTON2, m_wndColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMarkIconsSetting, CDialog)
	//{{AFX_MSG_MAP(CDlgMarkIconsSetting)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_BN_CLICKED(IDC_SET, OnSet)
	ON_BN_CLICKED(IDC_AUTO_SET, OnAutoSet)
	ON_BN_CLICKED(IDC_AUTO_SET_ALL, OnAutoSetAll)
	ON_BN_CLICKED(IDC_TOP,OnTop)
	ON_LBN_SELCHANGE(IDC_LIST_CHKCMD, OnSelchangeChkCmd)
	ON_LBN_SELCHANGE(IDC_LIST_RESULTS, OnSelchangeReasons)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgMarkIconsSetting message handlers


static void GetChkCmdRegs(CDlgDoc *pDoc, CArray<ChkCmdReg,ChkCmdReg>& regs)
{
	regs.RemoveAll();

	ChkCmdReg item;
	item.checkName = StrFromResID(IDS_SELCOND_SUSPENDPOINT);
	item.reasons.Add(StrFromResID(IDS_SELCOND_SUSPENDPOINT));
	regs.Add(item);

	ChkCmdReg item1;
	item1.checkName = StrFromResID(IDS_PSEUDONODE);
	item1.reasons.Add(StrFromResID(IDS_PSEUDONODE));
	regs.Add(item1);

	int nCmd = 0;
	ChkCmdReg *regs1 = pDoc->GetChkCmdRegs(nCmd);
	//填充检查项
	for( int i=0; i<nCmd; i++)
	{
		regs.Add(regs1[i]);
	}
}


void CDlgMarkIconsSetting::OnAdd() 
{
	CDlgCellDefLinetypeView dlg;
	
	CConfigLibManager *pCfgLibManager = gpCfgLibMan;
	ConfigLibItem config = pCfgLibManager->GetConfigLibItemByScale(m_nScale);
	dlg.SetConfig(config);
	
	dlg.SetShowMode(FALSE,TRUE,0);
	dlg.SetAsSelectMode(0);
	
	if( dlg.DoModal()!=IDOK )
		return;

	//-----------------------------------增加图标
	//创建内存设备
	CClientDC cdc(&m_wndListIcons);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if( !hDC )return;
	
	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC,m_nImageWid,m_nImageHei);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}
	
	CBitmap bm;
	bm.Attach(hBmp);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(255,255,255));
	::FillRect(hDC,CRect(0,0,m_nImageWid,m_nImageHei),(HBRUSH)br);

	m_listImages.Add(&bm,RGB(0, 0, 0));

	m_wndListIcons.SetImageList(&m_listImages,LVSIL_NORMAL);

	bm.Detach();

	::SelectObject(hDC, hOldBmp);
	::DeleteObject(hBmp);
	::DeleteDC(hDC);

	//-----------------------------------增加列表项
	CStringArray &names = *(m_pSettings->GetResultIconNames());

	CString strName = dlg.GetName();
	names.Add(strName);

	CreateImageItem(names.GetSize()-1);

	m_wndListIcons.InsertItem(names.GetSize()-1,strName,names.GetSize()-1);

	//-----------------------------------选中它
	
	int nSel = names.GetSize()-1;
	m_wndListIcons.EnsureVisible(nSel,FALSE);
	m_wndListIcons.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
}

void CDlgMarkIconsSetting::OnDel() 
{
	POSITION pos = m_wndListIcons.GetFirstSelectedItemPosition();
	if( !pos ) 	
		return;

	int nIndex = m_wndListIcons.GetNextSelectedItem(pos);
	CStringArray &names = *(m_pSettings->GetResultIconNames());
	
	m_wndListIcons.DeleteItem(nIndex);
	names.RemoveAt(nIndex);

	if( nIndex<names.GetSize() )
	{
		m_wndListIcons.EnsureVisible(nIndex,FALSE);
		m_wndListIcons.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	}
	else if( names.GetSize()>0 )
	{
		nIndex = names.GetSize()-1;
		m_wndListIcons.EnsureVisible(nIndex,FALSE);
		m_wndListIcons.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	}

}

void CDlgMarkIconsSetting::OnOK() 
{
	UpdateData(TRUE);

	m_pSettings->SetResultIconSize(m_fSize);
	
	CDialog::OnOK();
}

void CDlgMarkIconsSetting::OnCancel() 
{
	if( m_pSaveSettings )
		m_pSettings->Copy(m_pSaveSettings);
	
	CDialog::OnCancel();
}

BOOL CDlgMarkIconsSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( m_pSaveSettings )
		m_fSize = m_pSettings->GetResultIconSize();
	
	FillSymbolList();

	m_wndColor.SetColor(RGB(255,0,0));
	m_wndColor.EnableOtherButton(StrFromResID(IDS_OTHERCOLOR), FALSE);

	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();

	//填充检查项
	for( int i=0; i<nCmd; i++)
	{
		m_wndChkCmd.AddString(regs[i].checkName);
	}
	
	m_nCurSelCmd = -1;
	m_nCurSelReason = -1;
	
	m_wndChkCmd.SetCurSel(0);
	OnSelchangeChkCmd();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgMarkIconsSetting::CreateImageList(int cx, int cy, int nsize)
{
	m_listImages.Create(cx,cy,ILC_COLOR24,0,nsize);
	
	m_listImages.SetImageCount(nsize);
	m_wndListIcons.SetImageList(&m_listImages,LVSIL_NORMAL);
	
	GrBuffer2d buf;
	for( int i=0; i<nsize; i++)
	{
		DrawImageItem(i,cx,cy,&buf);
	}	
}

BOOL CDlgMarkIconsSetting::CreateImageItem(int idx)
{
	CCellDefLib *pCellDefLib = GetCellDefLib();
	if( !pCellDefLib )return FALSE;

	CStringArray &names = *(m_pSettings->GetResultIconNames());
	
	CString strName = names[idx];
	CellDef def = pCellDefLib->GetCellDef(strName);

	if( def.m_pgr==NULL )
		return FALSE;
	
	GrBuffer2d buf;
	DrawSymbol(&def, &buf);
	DrawImageItem(idx,m_nImageWid,m_nImageHei,&buf);
	
	return TRUE;
}



void CDlgMarkIconsSetting::DrawSymbol(CellDef *pDef, GrBuffer2d *buf)
{	
	if( pDef->m_pgr )
	{
		buf->AddBuffer(pDef->m_pgr);
		buf->GetEnvelope();
		buf->SetAllColor(0);
	}
}

void CDlgMarkIconsSetting::DrawImageItem(int idx, int cx, int cy, const GrBuffer2d *pBuf)
{
	// 获取当前选中项的图像索引，防止删除项目时图像位置偏移
	LVITEM item = {0};
	item.iItem = idx;
	item.iSubItem = 0;
	item.iImage = -1;
	item.mask = LVIF_IMAGE|LVIF_TEXT;
	m_wndListIcons.GetItem(&item);

	int iIndexImage = item.iImage>=0?item.iImage:idx;

	//创建内存设备
	CClientDC cdc(&m_wndListIcons);
	HDC hDC = ::CreateCompatibleDC(cdc.m_hDC);
	if( !hDC )return;
	
	HBITMAP hBmp = ::CreateCompatibleBitmap(cdc.m_hDC,cx,cy);
	if( !hBmp )
	{
		::DeleteDC(hDC);
		return;
	}
	
	CBitmap bm;
	bm.Attach(hBmp);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDC, hBmp);
	
	CBrush br(RGB(255,255,255));
	::FillRect(hDC,CRect(0,0,cx,cy),(HBRUSH)br);
	
	CPen pen;
	pen.CreatePen(PS_SOLID,0,(COLORREF)0);
	HPEN hOldPen = (HPEN)::SelectObject(hDC,(HPEN)pen);
	
	//计算变换系数
	CRect rect(2,2,cx-4,cy-4);
	Envelope e = pBuf->GetEnvelope();
	
	float scalex = rect.Width()/(e.m_xh>e.m_xl?(e.m_xh-e.m_xl):1e-10);
	float scaley = rect.Height()/(e.m_yh>e.m_yl?(e.m_yh-e.m_yl):1e-10);
	float xoff=0, yoff=0;
	if( scalex>scaley )
	{
		xoff = cx/2-((e.m_xh-e.m_xl)/2*scaley+rect.left);
		scalex = scaley;
	}
	else
	{
		yoff = cy/2-((e.m_yh-e.m_yl)/2*scalex+rect.top);
	}

	double matrix[9] = {
		scalex,0,-e.m_xl*scalex+rect.left+xoff+0.5,
			0,-scalex,cy+e.m_yl*scalex-rect.top-yoff-0.5,
			0,0,1
	};
	
	GDI_DrawGrBuffer2d(hDC,pBuf,TRUE,0,matrix,CSize(cx,cy),CRect(0,0,cx,cy));
	
	::SelectObject(hDC,hOldBmp);
	::SelectObject(hDC,hOldPen);

	m_listImages.Replace(iIndexImage,&bm,NULL);
	
	::DeleteDC(hDC);
	::DeleteObject(hBmp);
}



void CDlgMarkIconsSetting::FillSymbolList()
{
	m_wndListIcons.DeleteAllItems();
	m_listImages.DeleteImageList();

	CStringArray &names = *(m_pSettings->GetResultIconNames());
	
	int i, nSymbol = names.GetSize();
	
	CreateImageList(m_nImageWid,m_nImageHei,nSymbol);
	
	int nSel = 0;
	for( i=0; i<nSymbol; i++)
	{
		m_wndListIcons.InsertItem(i,names[i],i);
	}

	for( i=0; i<nSymbol; i++)
	{
		CreateImageItem(i);
	}
	
	m_wndListIcons.EnsureVisible(nSel,FALSE);
	m_wndListIcons.SetItemState(nSel, LVIS_SELECTED, LVIS_SELECTED);
}


void CDlgMarkIconsSetting::SetDoc(CDlgDoc *pDoc, CChkResSettings *pSettings)
{
	m_pDoc = pDoc;
	m_pSettings = pSettings;
	m_nScale = pDoc->GetDlgDataSource()->GetScale();

	m_pSaveSettings = new CChkResSettings();
	m_pSaveSettings->Copy(m_pSettings);
}


void CDlgMarkIconsSetting::SaveSomeParameters()
{
	if( m_nCurSelCmd<0 )return;
	if( m_nCurSelReason<0 )return;	

	int nSel1 = m_nCurSelCmd, nSel2 = m_nCurSelReason;
	
	int nSel3 = -1;
	if( m_wndListIcons.GetSelectedCount()<=0 )return;
	for( int i=0; i<m_wndListIcons.GetItemCount(); i++)
	{
		if( m_wndListIcons.GetItemState(i,LVIS_SELECTED)!=0 )
		{
			nSel3 = i;
			break;
		}
	}
	if( nSel3<0 )return;
	
	CString icon = m_wndListIcons.GetItemText(nSel3,0);
	
	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();
	
	m_pSettings->SetColor(regs[nSel1].checkName,regs[nSel1].reasons[nSel2],m_wndColor.GetColor());
}

void CDlgMarkIconsSetting::OnSet()
{
	int nSel1 = m_wndChkCmd.GetCurSel();
	if( nSel1<0 )return;
	
	int nSel2 = m_wndReasons.GetCurSel();
	if( nSel2<0 )return;	

	int nSel3 = -1;
	if( m_wndListIcons.GetSelectedCount()<=0 )return;
	for( int i=0; i<m_wndListIcons.GetItemCount(); i++)
	{
		if( m_wndListIcons.GetItemState(i,LVIS_SELECTED)!=0 )
		{
			nSel3 = i;
			break;
		}
	}
	if( nSel3<0 )return;

	CString icon = m_wndListIcons.GetItemText(nSel3,0);

	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();

	m_pSettings->SetIcon(regs[nSel1].checkName,regs[nSel1].reasons[nSel2],icon);
	m_pSettings->SetColor(regs[nSel1].checkName,regs[nSel1].reasons[nSel2],m_wndColor.GetColor());
	
	m_wndReasons.DeleteString(nSel2);
	m_wndReasons.InsertString(nSel2,regs[nSel1].reasons[nSel2]+" --> "+icon);
	m_wndReasons.SetCurSel(nSel2);
}



void CDlgMarkIconsSetting::OnAutoSet()
{
	int nSel = m_wndChkCmd.GetCurSel();
	if( nSel<0 )return;

	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();
	
	int nIcon = m_pSettings->GetResultIconNames()->GetSize();
	int n = 0;
	
	ChkCmdReg item = regs[nSel];
	for( int j=0; j<item.reasons.GetSize(); j++)
	{
		m_pSettings->SetIcon(item.checkName,item.reasons[j],m_pSettings->GetResultIconNames()->GetAt(n));
		n = (n+1)%nIcon;
	}

	OnSelchangeChkCmd();
}


void CDlgMarkIconsSetting::OnAutoSetAll()
{
	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();
	
	int nIcon = m_pSettings->GetResultIconNames()->GetSize();
	int n = 0;

	COLORREF clrs[] = {
		RGB(255,0,0),RGB(0,255,0),RGB(0,0,255),
		RGB(255,255,0),RGB(255,0,255),RGB(0,255,255),
		RGB(196,196,0),RGB(0,196,196),RGB(196,0,196),
		RGB(128,196,0),RGB(128,0,196),RGB(0,128,196),
		RGB(196,128,0),RGB(0,196,128),RGB(196,0,128),
		RGB(128,196,128),RGB(128,128,196),RGB(196,128,128)
	};

	int nColor = sizeof(clrs)/sizeof(clrs[0]);
	
	for( int i=0; i<nCmd; i++)
	{
		int n = 0;
		ChkCmdReg item = regs[i];
		for( int j=0; j<item.reasons.GetSize(); j++)
		{
			m_pSettings->SetIcon(item.checkName,item.reasons[j],m_pSettings->GetResultIconNames()->GetAt(n));
			m_pSettings->SetColor(item.checkName,item.reasons[j],clrs[i%nColor]);
			n = (n+1)%nIcon;
		}
	}

	OnSelchangeChkCmd();
}


void CDlgMarkIconsSetting::OnTop()
{
	//删除当前列表项
	POSITION pos = m_wndListIcons.GetFirstSelectedItemPosition();
	if( !pos ) 	
		return;
	
	int nIndex = m_wndListIcons.GetNextSelectedItem(pos);
	if( nIndex<=0 )
		return;

	CStringArray &names = *(m_pSettings->GetResultIconNames());
	
	CString icon = names[nIndex];
	names.RemoveAt(nIndex);
	names.InsertAt(0,icon);

	FillSymbolList();

	m_wndListIcons.EnsureVisible(0,FALSE);
	m_wndListIcons.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
}


void CDlgMarkIconsSetting::OnSelchangeChkCmd()
{
	SaveSomeParameters();

	m_nCurSelCmd = m_wndChkCmd.GetCurSel();

	int nSel = m_wndChkCmd.GetCurSel();
	if( nSel<0 )return;	

	m_wndReasons.ResetContent();

	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();

	if( nSel>=nCmd )
		return;

	//填充结果项
	for( int i=0; i<regs[nSel].reasons.GetSize(); i++)
	{
		CString icon = m_pSettings->GetIcon(regs[nSel].checkName,regs[nSel].reasons[i]);
		if( icon.IsEmpty() )
			icon = "NULL";

		m_wndReasons.AddString(regs[nSel].reasons[i]+" --> "+icon);
	}

	// 调整宽度
	CListBox* pmyListBox = &m_wndReasons;
	
	// Find the longest string in the list box.
	CString str;
	CSize   sz;
	int     dx=0;
	CDC*    pDC = pmyListBox->GetDC();
	for (i=0;i < pmyListBox->GetCount();i++)
	{
		pmyListBox->GetText( i, str );
		sz = pDC->GetTextExtent(str);
		
		if (sz.cx > dx)
			dx = sz.cx;
	}
	pmyListBox->ReleaseDC(pDC);	
	pmyListBox->SetHorizontalExtent(dx+30);
	
	m_wndReasons.SetCurSel(0);

	m_nCurSelReason = -1;
	OnSelchangeReasons();
}


void CDlgMarkIconsSetting::OnSelchangeReasons()
{
	SaveSomeParameters();
	m_nCurSelReason = m_wndReasons.GetCurSel();

	int nSel1 = m_wndChkCmd.GetCurSel();
	if( nSel1<0 )return;

	int nSel2 = m_wndReasons.GetCurSel();
	if( nSel2<0 )return;

	CArray<ChkCmdReg,ChkCmdReg> regs;
	GetChkCmdRegs(m_pDoc,regs);
	int nCmd = regs.GetSize();

	if( nSel1>=nCmd )
		return;

	if( nSel2>=regs[nSel1].reasons.GetSize() )
		return;

	CString icon = m_pSettings->GetIcon(regs[nSel1].checkName,regs[nSel1].reasons[nSel2]);
	COLORREF color = m_pSettings->GetColor(regs[nSel1].checkName,regs[nSel1].reasons[nSel2]);

	m_wndColor.SetColor(color);

	int nSelIcon = -1;
	CStringArray &names = *(m_pSettings->GetResultIconNames());
	for( int i=0; i<names.GetSize(); i++)
	{
		if( names[i].CompareNoCase(icon)==0 )
		{
			nSelIcon = i;
			m_wndListIcons.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		}
		else
		{
			m_wndListIcons.SetItemState(i, 0, LVIS_SELECTED);
		}
	}

	if( nSelIcon>=0 )
	{
		m_wndListIcons.EnsureVisible(nSelIcon,FALSE);
	}
}

