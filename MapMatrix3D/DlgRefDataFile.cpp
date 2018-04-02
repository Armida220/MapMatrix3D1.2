// DlgRefDataFile.cpp : implementation file
//

#include "stdafx.h"
#include "editbase.h"
#include "DlgRefDataFile.h"
#include "SmartViewFunctions.h"
#include "UIFFileDialogEx.h "
#include "editbasedoc.h "
#include "DlgDataSource.h "
#include "DxfAccess.h"
#include "DlgChooseMP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_LM_PROPLIST			1001
#define FIELD_DATASOURCENAME      _T("DATASOURCENAME")
#define FIELD_DSISACTIVE	      _T("DSISACTIVE")
#define FIELD_DSISSHOWBOUND		  _T("DSISSHOWBOUND")
#define FIELD_DSISSINGLECOL		  _T("DSISSINGLECOL")
#define FIELD_DSSINGLECOL		  _T("DSSINGLECOL")
#define FIELD_DSNEWORDEL		  _T("DSNEWORDEL")
/////////////////////////////////////////////////////////////////////////////
// CDlgRefDataFile dialog


CDlgRefDataFile::CDlgRefDataFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRefDataFile::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRefDataFile)
	//}}AFX_DATA_INIT
	m_pDlgDoc = NULL;
}


void CDlgRefDataFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRefDataFile)
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRefDataFile, CDialog)
	//{{AFX_MSG_MAP(CDlgRefDataFile)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_NEW2, OnButtonNewDB)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST, OnEndlabeleditList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
	
/////////////////////////////////////////////////////////////////////////////
// CDlgRefDataFile message handlers

BOOL DlgSelectMultiFiles(HWND hOwner, LPCTSTR filter, LPCTSTR defExt, CStringArray& fileName, CString *all_name);

	
void CDlgRefDataFile::OnButtonNew() 
{
	CString filter(StrFromResID(IDS_LOADMDB_FILTER));
	filter.Replace('|','\0');

	CStringArray arrFileNames;
	CString filePaths;
	
	if( !DlgSelectMultiFiles(GetSafeHwnd(),
		filter,
		NULL,arrFileNames,&filePaths) )
		return;

	//先挑出非 fdb 格式；
	CStringArray convert_filenames;
	for(int i=0; i<arrFileNames.GetSize();)
	{
		CString ext = arrFileNames[i].Right(4);
		if(ext.CompareNoCase(".dwg")==0 || ext.CompareNoCase(".dxf")==0 || ext.CompareNoCase(".DXF")==0)
		{
            convert_filenames.Add(arrFileNames[i]);
			arrFileNames.RemoveAt(i,1);
			continue;
		}
		//
		++i;
	}
    //将所有非 fdb 格式数据转换成 fdb 格式;
	if(convert_filenames.GetSize()>0)
	{
		BOOL need_convert = FALSE;
		if(MessageBox(StrFromResID(IDS_FORMAT_CONVERT_TO_FDB),"",MB_OKCANCEL)==IDOK)
			need_convert = TRUE;
		else
			need_convert = FALSE;
		//
		for(int t=0; need_convert,t<convert_filenames.GetSize(); ++t)
		{
            TCHAR driver[_MAX_DRIVE], path[_MAX_PATH], fname[_MAX_FNAME], ext[_MAX_EXT];
		    _tsplitpath(convert_filenames[t], driver, path, fname, ext);
			//
            CString convert_folder = CString(driver)+path+"FDB\\";
			if(!PathIsDirectory(convert_folder) && !CreateDirectory(convert_folder,NULL))
			{
				continue;
			}
			//
			CDxfRead dxf_read;
			CStringArray temp;
			temp.Add(convert_filenames[t]);
			if(dxf_read.ConvertDxf2FDB(gpCfgLibMan->GetScheme(GetActiveDlgDoc()->GetDlgDataSource()->GetScale()),temp,convert_folder))
			{
                 arrFileNames.Add(convert_folder+fname+".fdb");
			}
		} 
	}
    //
	for( int j=0; j<arrFileNames.GetSize(); j++)
	{
		CString fileName = arrFileNames[j];

		CLVLPropItem0 *pItem0 = NULL;
		CString temp;
		int nTtem = m_wndPropListCtrl.GetItemCount();
		for (int i=0;i<nTtem;i++)
		{
			pItem0 = m_wndPropListCtrl.GetPropItem(i);	
			temp = (LPCTSTR)(_bstr_t)pItem0->GetValue(0);
			if (temp.CompareNoCase((LPCTSTR)fileName)==0)
			{
				AfxMessageBox(StrFromResID(IDS_SELECT_EXIST));
				break;
			}
		}

		if( i<nTtem )
			continue;

		CLVLPropItem0 *pItem = new CLVLPropItem0;
		if( !pItem )continue;
		
		m_wndPropListCtrl.AddItem(pItem);
		pItem->SetValue((_variant_t)(LPCTSTR)fileName,0);
		
		pItem->SetValue((_variant_t)(bool)false,1);
		pItem->SetValue((_variant_t)(bool)true,2);
		pItem->SetValue((_variant_t)(bool)true,3);
		COLORREF clr = RGB(125,125,125);	
		pItem->SetValue((_variant_t)(long)(clr),4);
		pItem->SetValue((_variant_t)(LPCTSTR)("Add"),5);
		pItem->SetData(1);
	}

	m_wndPropListCtrl.FilterPropItems();
}

void CDlgRefDataFile::OnButtonNewDB() 
{
	CDlgChooseMP dlg;
	if( dlg.DoModal()!=IDOK )
		return;

	for (int j = 0; j < dlg.m_arrSelectedMaps.GetSize(); j++)
	{
		CString fileName = dlg.m_arrSelectedMaps[j];

		CLVLPropItem0 *pItem0 = NULL;
		CString temp;
		int nTtem = m_wndPropListCtrl.GetItemCount();
		for (int i = 0; i < nTtem; i++)
		{
			pItem0 = m_wndPropListCtrl.GetPropItem(i);
			temp = (LPCTSTR)(_bstr_t)pItem0->GetValue(0);
			if (temp.CompareNoCase((LPCTSTR)fileName) == 0)
			{
				AfxMessageBox(StrFromResID(IDS_SELECT_EXIST));
				break;
			}
		}

		if (i < nTtem) return;

		CLVLPropItem0 *pItem = new CLVLPropItem0;
		if (!pItem) return;

		m_wndPropListCtrl.AddItem(pItem);
		pItem->SetValue((_variant_t)(LPCTSTR)fileName, 0);

		pItem->SetValue((_variant_t)(bool)false, 1);
		pItem->SetValue((_variant_t)(bool)true, 2);
		pItem->SetValue((_variant_t)(bool)true, 3);
		COLORREF clr = RGB(125, 125, 125);
		pItem->SetValue((_variant_t)(long)(clr), 4);
		pItem->SetValue((_variant_t)(LPCTSTR)("Add"), 5);
		pItem->SetData(0);
	}
	
	m_wndPropListCtrl.FilterPropItems();
}

void CDlgRefDataFile::OnButtonDel() 
{
 	int nCount = m_wndPropListCtrl.GetSelectedCount();

	CArray<CLVLPropItem0*,CLVLPropItem0*> arrPSels;
	for( int i=0; i<nCount; i++)
	{
		arrPSels.Add(m_wndPropListCtrl.GetSelectedItem(i));
	}

 	for( i=0; i<nCount; i++)
	{
		CLVLPropItem0* pSelItem = arrPSels[i];
		if( !pSelItem )continue;

		CString temp = (LPCTSTR)(_bstr_t)pSelItem->GetValue(5);
		if(temp.CompareNoCase(_T("Add"))==0)
		{
			m_wndPropListCtrl.RemovePropItem(pSelItem);		
		}
		else
		{
			if (temp.CompareNoCase(_T("Del"))==0)
			{
				pSelItem->SetValue((_variant_t)(_bstr_t)_T(""),5);
			}
			else
			{
				if (m_wndPropListCtrl.GetPropItem(0)==pSelItem)
				{
					AfxMessageBox(StrFromResID(IDS_MAINDS_INVALID_DEL));
					continue;
				}
				pSelItem->SetValue((_variant_t)(_bstr_t)_T("Del"),5);
			}
		}
	}
	m_wndPropListCtrl.FilterPropItems();
	OnSelChange();
}

void CDlgRefDataFile::OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 1;
}

BOOL CDlgRefDataFile::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_pDlgDoc = GetActiveDlgDoc();
	CRect rect;
	GetDlgItem(IDC_POS)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (!m_wndPropListCtrl.Create(WS_VISIBLE|WS_CHILD|WS_BORDER,rect,this,IDC_LM_PROPLIST))
		return FALSE;
	CLVLPropColumn0 col;
	strcpy(col.FieldName,FIELD_DATASOURCENAME);
	strcpy(col.ColumnName,StrFromResID(IDS_DATASOURCENAME));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn0::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	
	strcpy(col.FieldName,FIELD_DSISACTIVE);
	strcpy(col.ColumnName,StrFromResID(IDS_DSISACTIVE));
	col.ValueType = VT_BOOL/*VT_I4*/;
	col.BitmapOffset = 2;
	col.Type = CLVLPropColumn0::RADIO/*COLOR*/;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);
	strcpy(col.FieldName,FIELD_DSISSHOWBOUND);
	strcpy(col.ColumnName,StrFromResID(IDS_DSISSHOWBOUND));
	col.ValueType = VT_BOOL;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn0::CHECK;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);
	strcpy(col.FieldName,FIELD_DSISSINGLECOL);
	strcpy(col.ColumnName,StrFromResID(IDS_DSISSINGLECOL));
	col.ValueType = VT_BOOL;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn0::CHECK;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);
	strcpy(col.FieldName,FIELD_DSSINGLECOL);
	strcpy(col.ColumnName,StrFromResID(IDS_SINGLECOL));
	col.ValueType = VT_I4;
	col.BitmapOffset = 1;
	col.Type = CLVLPropColumn0::COLOR;
	col.bReadOnly = FALSE;
	m_wndPropListCtrl.AddColumn(col);
	strcpy(col.FieldName,FIELD_DSNEWORDEL);
	strcpy(col.ColumnName,StrFromResID(IDS_DSNEWORDEL));
	col.ValueType = VT_BSTR;
	col.Type = CLVLPropColumn0::EDIT;
	col.bReadOnly = TRUE;
	m_wndPropListCtrl.AddColumn(col);
	m_wndPropListCtrl.InitHeaderCtrl();
	CHeaderCtrl& header = m_wndPropListCtrl.GetHeaderCtrl();
	for(int i=0; i<header.GetItemCount(); i++)
	{
		HDITEM hitem;
		hitem.mask = HDI_WIDTH;
		hitem.cxy = 75;
		header.SetItem(i,&hitem);
	}
	
	CLVLPropItem0 *pItem;
	int nData = m_pDlgDoc->GetDlgDataSourceCount();
	int nActiveData = m_pDlgDoc->GetActiveDataSourceIdx();
	BOOL bEnable;
	COLORREF clr;
	for(  i=0; i<nData; i++)
	{
		pItem = new CLVLPropItem0;
		if( !pItem )continue;
		m_wndPropListCtrl.AddItem(pItem);
		CString dsname = m_pDlgDoc->GetDlgDataSource(i)->GetName();
		pItem->SetValue((_variant_t)(LPCTSTR)dsname, 0);
		if(nActiveData==i)pItem->SetValue((_variant_t)(bool)true,1);
		else pItem->SetValue((_variant_t)(bool)false,1);
		if(m_pDlgDoc->GetDlgDataSource(i)->GetShowBound())
			pItem->SetValue((_variant_t)(bool)true,2);
		else
			pItem->SetValue((_variant_t)(bool)false,2);
		m_pDlgDoc->GetDlgDataSource(i)->GetMonoColor(&bEnable,&clr);
		if(bEnable)
			pItem->SetValue((_variant_t)(bool)true,3);
		else
			pItem->SetValue((_variant_t)(bool)false,3);
		pItem->SetValue((_variant_t)(long)(clr),4);
		pItem->SetValue((_variant_t)(LPCTSTR)(""),5);

		//标记该数据源是否可以激活
		if (i==0)
		{
			pItem->SetData(2);//主数据
		}
		else if (dsname.GetLength() < 5)
		{
			pItem->SetData(0);
		}
		else if (0 == dsname.Right(4).CompareNoCase(".fdb"))
		{
			pItem->SetData(1);
		}
		else
		{
			pItem->SetData(0);
		}
	}
	m_wndPropListCtrl.FilterPropItems();

	if (!CheckLicense(86))
	{
		GetDlgItem(IDC_BUTTON_NEW2)->ShowWindow(SW_HIDE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRefDataFile::ModifyDataSourceAttri(CLVLPropItem0 *pItem, CLVLPropColumn0* Pc ,_variant_t exData)
{
	m_pDlgDoc->UpdateAllViews(NULL,hc_Refresh);
}
void CDlgRefDataFile::OnSelChange()
{
	int nSel = m_wndPropListCtrl.GetSelectedCount();
	if (nSel>0)
	{
		m_btnDel.EnableWindow(TRUE);
		CLVLPropItem0* pItem = m_wndPropListCtrl.GetSelectedItem(0);
		CString temp = (LPCTSTR)(_bstr_t)pItem->GetValue(5);
		if (temp.CompareNoCase(_T("Add"))==0)
		{
			m_btnDel.SetWindowText(StrFromResID(IDS_DELDATA));
		}
		else
		{
			if (temp.CompareNoCase(_T("Del"))==0)
			{
				m_btnDel.SetWindowText(StrFromResID(IDS_RESTOREDATA));
			}
			else
				m_btnDel.SetWindowText(StrFromResID(IDS_DELDATA));
		}
	}
	else
		m_btnDel.EnableWindow(FALSE);
}
void CDlgRefDataFile::OnCancel() 
{
// 	UpdateData(TRUE);
// 	m_arrFileNames.RemoveAll();
// 	int nCount = m_wndList.GetItemCount();
// 	for( int i=0; i<nCount; i++)
	CDialog::OnCancel();
}
void CDlgRefDataFile::OnOK()
{
	UpdateData();
	CLVLPropItem0 *pItem = NULL;
	dataParam item;
	int nTtem = m_wndPropListCtrl.GetItemCount();
	int nCol = m_wndPropListCtrl.GetColumnCount();
	for (int i=0;i<nTtem;i++)
	{
		pItem = m_wndPropListCtrl.GetPropItem(i);
		{
			item.strDataName = (LPCTSTR)(_bstr_t)pItem->GetValue(0);
			item.bIsActive = (bool)pItem->GetValue(1)?TRUE:FALSE;
			item.bShowBound = (bool)pItem->GetValue(2)?TRUE:FALSE;
			item.bEnableMono = (bool)pItem->GetValue(3)?TRUE:FALSE;
			item.clrMono = (long)pItem->GetValue(4);
			item.strDataState = (LPCTSTR)(_bstr_t)pItem->GetValue(5);
		}
		m_arrDataParams.Add(item);		
	}
	CDialog::OnOK();
}

