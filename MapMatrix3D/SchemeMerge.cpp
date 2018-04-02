// SchemeMerge.cpp : implementation file
//

#include "stdafx.h"
#include "EditBase.h"
#include "SchemeMerge.h"
#include "ExMessage.h"
 
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
/*#include "Resource.h"*/
static char THIS_FILE[] = __FILE__;
#endif

#define TIMERID_PREVIEW 2
/////////////////////////////////////////////////////////////////////////////
// CSchemeMerge dialog

extern BOOL BrowseFolderEx( LPCTSTR lpszTitle,// [in] ���ڱ���
						   LPTSTR lpszPath,		// [out] ���ص��ļ�·��
						   LPCTSTR lpszInitDir,	// [in] ��ʼ�ļ�·��
						   HWND hWnd,				// [in] ��ϵ����
						   BOOL bNetwork=FALSE,	// [in] ����������·����Χ��
					UINT ulFlags=BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS );// [in] ���÷��

CSchemeMerge::CSchemeMerge(CWnd* pParent /*=NULL*/,CConfigLibManager* pSrcCfgLibMan, CConfigLibManager* pDesCfgLibMan,int iDesScale,int iSrcScale, BOOL bLoadedSrcTree)
	: CDialog(CSchemeMerge::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSchemeMerge)
	m_DesSchemePath = _T("");
	m_SrcSchemePath = _T("");
	m_MergeSchemePath = _T("");
	m_bModified = FALSE ;
	m_CurLayerIndex = -1 ;
	m_SchemePosID = 0;
	m_bSpecialLayer = FALSE;
	m_bUnMerged = TRUE; //FALSE->TRUE;  //�Ƿ����ͬ�Ĳ�����и���,FALSE���ǣ�TRUE�����ǡ�
	m_bHasSameLayerCode = FALSE; //�Ƿ������ͬ�Ĳ��롣
	m_iSrcLayerCode = -1;
	m_iDesLayerCode = -1;
	m_nDesRemovedLayer = -1; //Ŀ�귽���б��Ƴ��Ĳ㡣
	m_nMovedLayer = -1;
	m_nMovedGroup = -1;//Դ�����б��ϲ����顣
	m_nDesRemovedGroup = -1;//Ŀ�귽���б�ɾ�����顣
	m_SrcSelectedGroupName = "";
	m_firstDesSameGroupIndex = -2;//
	m_bFirstPrompt = TRUE;//�״ε�����ʾ��
	m_nDesIndex = -1;//��ѡʱ��Ҫ�����λ�á�
    m_nDesSelectedGroupIndex = -1;//��ѡʱ��Ҫ�����λ�á�
	m_hSelectedTreeItemArray[DES-1] = NULL;
	m_hSelectedTreeItemArray[SRC-1] = NULL;
	m_bFirstSelectedIndex = TRUE; 
	m_bCoveredInSymbolLib = FALSE;
	m_DesScale = iDesScale; 
	m_SrcScale = iSrcScale; 
	m_nSameCellCount = 0;
	m_nSameLineType = 0;
	m_bSpecialRtn = 0;//ɾ������������ʱ��ʾ�Ի���ķ���ֵ��
	m_hSrcParentItem = NULL;
	m_pSrcCfgLibMan = pSrcCfgLibMan;//hcw,2012.7.20,pSrcCfgLibMan
	m_pDesCfgLibMan = pDesCfgLibMan;//hcw,2012.7.20,pDesCfgLibMan
	
	m_SrcCfgPath = "";//hcw,2012.7.24
	m_DesCfgPath = "";
	m_bChangedDesComboScale = FALSE;
	m_bChangedSrcComboScale = FALSE;

	m_bChangedSrcSchemePath = FALSE;
	m_bChangedDesSchemePath = FALSE;
	m_bIsSrcTreeLoaded = bLoadedSrcTree; //hcw,2012.9.20
	m_bSaved = FALSE; //hcw,2012.9.13
	//}}AFX_DATA_INIT
}


void CSchemeMerge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchemeMerge)
	DDX_Control(pDX, IDC_BUTTON_DESPATH, m_BtnDesPath);
	DDX_Control(pDX, IDC_BUTTON_SRCPATH, m_BtnSrcPath);
	DDX_Control(pDX, IDC_BUTTON_MERGE, m_AutoMerge);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_Del);
	DDX_Control(pDX, IDC_BUTTON_UP, m_UpDifs);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_DownDifs);
	DDX_Control(pDX, IDC_BUTTON_HANDMERGE, m_HandleMerge);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_BtnSave);
	DDX_Control(pDX, IDC_COMBO_DESSCHEME2, m_DesComBoScale);
	DDX_Control(pDX, IDC_TREE_SRCSCHEME2, m_SrcTree);
	DDX_Control(pDX, IDC_TREE_DESCHEME2, m_DesTree);
	DDX_Control(pDX, IDC_COMBO_SRCSCHEME2, m_SrcComBoScale);
	DDX_Text(pDX, IDC_EDIT_DESPATH, m_DesSchemePath);
	DDX_Text(pDX, IDC_EDIT_SRCPATH, m_SrcSchemePath);
	DDX_Text(pDX, IDC_EDIT_MERGEPATH, m_MergeSchemePath);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CSchemeMerge, CDialog)
	//{{AFX_MSG_MAP(CSchemeMerge)
	ON_CBN_SELCHANGE(IDC_COMBO_DESSCHEME2, OnSelChangeDesScaleCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SRCSCHEME2, OnSelChangeSrcScaleCombo)
	ON_BN_CLICKED(IDC_BUTTON_MERGE, OnButtonMerge)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposMergeSpin)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DESCHEME2, OnSelchangedTreeDes)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SRCSCHEME2, OnSelchangedTreeSrc)
    ON_WM_VSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_HANDMERGE, OnButtonHandmerge)
	ON_NOTIFY(NM_KILLFOCUS, IDC_TREE_SRCSCHEME2, OnKillfocusTreeSrcscheme2)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE_SRCSCHEME2, OnSetfocusTreeSrcscheme2)
	ON_NOTIFY(NM_SETFOCUS, IDC_TREE_DESCHEME2, OnSetfocusTreeDescheme2)
	ON_NOTIFY(NM_KILLFOCUS, IDC_TREE_DESCHEME2, OnKillfocusTreeDescheme2)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_SRCPATH, OnButtonSrcpath)
	ON_BN_CLICKED(IDC_BUTTON_DESPATH, OnButtonDespath)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSchemeMerge::FillMergeDlgTree(CMergeTree& treeCtrl, USERIDX& UserIdx,  ConfigLibItem config,multimap<int,HTREEITEM>&hTreeItemMap)
{
	UpdateData(TRUE);
	if ((config.pScheme == NULL))
		return FALSE;
	if (hTreeItemMap.size()>0)
	{
		hTreeItemMap.clear();
	}
	// ������Ϣ�浽USERIDX��
	UserIdx.Clear();
	
	treeCtrl.DeleteAllItems();
	
	for (int i=0; i<config.pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		LAYGROUP group;
		strcpy(group.GroupName,config.pScheme->m_strSortedGroupName.GetAt(i));

		UserIdx.m_aGroup.Add(group);
	}
	int nCount = config.pScheme->GetLayerDefineCount();
	for(i=0; i<config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *layer = config.pScheme->GetLayerDefine(i);

		if(layer->GetLayerName()=="")	
			continue;
		for (int j=0; j<UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
				idx.code = layer->GetLayerCode();
				strcpy(idx.FeatureName,layer->GetLayerName());
				strcpy(idx.strAccel,layer->GetAccel());
				idx.groupidx = j;				
				UserIdx.m_aIdx.Add(idx);
				break;
			}
		}
		
	}
	//hcw,2012.7.27
	int nSpecialCount = config.pScheme->GetLayerDefineCount(TRUE);

	if (nSpecialCount>0)
	{
		for(i=0; i<config.pScheme->GetLayerDefineCount(TRUE); i++)
		{
			CSchemeLayerDefine *layer = config.pScheme->GetLayerDefine(i,TRUE);
			
			for (int j=0; j<UserIdx.m_aGroup.GetSize(); j++)
			{
				if (layer->GetGroupName().CompareNoCase(UserIdx.m_aGroup[j].GroupName) == 0)
				{
					IDX idx;
					idx.code = layer->GetLayerCode();
					strcpy(idx.FeatureName,layer->GetLayerName());
					strcpy(idx.strAccel,layer->GetAccel());
					idx.groupidx = j;
					
					UserIdx.m_aIdx.Add(idx);
					break;
				}
			}
			
		}

	}


	//�����
	char strR[_MAX_FNAME];
	int  recentidx = -1, specialidx = -1;
	CArray<HTREEITEM,HTREEITEM> m_aRootItem;
	for (i=0; i<UserIdx.m_aGroup.GetSize(); i++)
	{
		LAYGROUP gp;
		gp = UserIdx.m_aGroup.GetAt(i);
		
		if (stricmp(gp.GroupName,"Recent") == 0x00)
			recentidx = i;
		
		if (stricmp(gp.GroupName,StrFromResID(IDS_SPECIALGROUP)) == 0x00)
			specialidx = i;
		
		if (recentidx != i)
		{
			HTREEITEM hRoot = treeCtrl.InsertRoot(_T(gp.GroupName),0,GROUPID_FROMIDX(i));
			treeCtrl.SetItemState(hRoot,0,TVIS_BOLD);
			m_aRootItem.Add(hRoot);
		}
		
	}
	
	for (i=0; i<UserIdx.m_aIdx.GetSize(); i++)
	{
		IDX idx;
		idx = UserIdx.m_aIdx.GetAt(i);
		
		if (idx.groupidx != recentidx)
		{
			if (idx.groupidx == specialidx)
			{

				if (strlen(idx.strAccel) <= 0)
				{
					sprintf(strR,"%s",idx.FeatureName);
				}
				else
				{
					sprintf(strR,"%s^%s",idx.FeatureName,idx.strAccel);
				}
			}
			else
			{

				if (strlen(idx.strAccel) <= 0)
				{
					sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
				}
				else
				{
					sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
				}
			}
			
			
			// �Դ洢��recent���µ��飬Ӧ�������ʱ������ӵ�recent�����棬�����������(�ϰ汾��ʽ)
			if (recentidx != -1 && idx.groupidx > recentidx)
			{
				treeCtrl.InsertSubItem(_T(strR),m_aRootItem.GetAt(idx.groupidx-1),0,CODEID_FROMIDX(i));
			}
			else
			{
				HTREEITEM item = treeCtrl.InsertSubItem(_T(strR),m_aRootItem.GetAt(idx.groupidx),0,CODEID_FROMIDX(i));
			    //Establish MultiMap;
				hTreeItemMap.insert(make_pair(idx.code, item)); //hcw,2012.5.22
 			}
		}
		
	}
	
	treeCtrl.RedrawWindow();
	UpdateData(FALSE);
	
	return TRUE;

}



BOOL CSchemeMerge::FillMergeDlgScaleCombo()
{
	UpdateData(TRUE);
	m_DesComBoScale.ResetContent();
	m_SrcComBoScale.ResetContent();
	if (gpCfgLibMan->GetConfigLibCount() < 1)
		return FALSE;
	DWORD minScale = MAXLONG;//hcw,2012.4.5
	DWORD maxScale = 0;//2012.6.21
	for (int i=0; i<gpCfgLibMan->GetConfigLibCount(); i++)
	{
		CString data;
		for (int j=0; j<m_DesComBoScale.GetCount();j++)
		{
			
			m_DesComBoScale.GetLBText(j,data);
			DWORD dScale = atoi(data);
			//{hcw,2012.4.5,ѡ����С�����ߡ�
			if (minScale>dScale)
			{
				minScale = dScale;
			}
			//}
			//{2012.6.21,ѡ����������
			if (maxScale<dScale)
			{
				maxScale = dScale;
			}
			//}
			if (gpCfgLibMan->GetConfigLibItem(i).GetScale() < dScale)
				break;
		}
		data.Format("%d",gpCfgLibMan->GetConfigLibItem(i).GetScale());
		int index = m_DesComBoScale.InsertString(j,data);
		m_SrcComBoScale.InsertString(j,data);
		m_DesComBoScale.SetItemData(index,i);
		//m_ResComBoScale.SetItemData(index,i);
	}
	
	//ѡ��ǰ��С������
	CString strScale;
	//{hcw,2012.4.5
	strScale.Format("%d",minScale);
	//}	
	if (m_DesScale<=0)
	{
		m_DesScale = atoi(strScale);
	}
	CString strDesScale = "";
	strDesScale.Format("%d",m_DesScale);
	m_DesComBoScale.SelectString(-1,strDesScale);

	CString strScaleMax="";
	strScaleMax.Format("%d",maxScale);	
	
	if (m_SrcScale<=0)
	{
		m_SrcScale = atoi(strScaleMax);
	}
	CString strSrcScale;
	strSrcScale.Format("%d",m_SrcScale);
	m_SrcComBoScale.SelectString(-1,strSrcScale);
	//{hcw,2012.5.3,reallocate new place for SchemeLayerDefine of m_DesConfig.
	
	ConfigLibItem tmpDesConfig = gpCfgLibMan->GetConfigLibItemByScale(m_DesScale); //atoi(strScale)->m_DesScale
	m_DesConfig.SetScalePath(tmpDesConfig.GetPath());
	m_DesConfig.Load();
	

	ConfigLibItem tmpSrcConfig = gpCfgLibMan->GetConfigLibItemByScale(m_SrcScale);//atoi(strScaleMax)->m_SrcScale
	CString strPath = tmpSrcConfig.GetPath();
	m_SrcConfig.SetScalePath(tmpSrcConfig.GetPath());
	m_SrcConfig.Load();

	m_BackupDesConfig = m_DesConfig;
	m_BackupSrcConfig = m_SrcConfig;
	
	UpdateData(FALSE);
	
	
	// �л�ĵ�ʱ������ɾ����ز���
	//ForBidDel();
	
	//����Ŀ�귽����
	
	m_BackupCurDesSchemeXML.Empty();
	m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
	//����Դ������
	m_BackupCurSrcSchemeXML.Empty();
	m_SrcConfig.pScheme->WriteTo(m_BackupCurSrcSchemeXML);
	return TRUE;
}

BOOL CSchemeMerge::FillMergeDlgScaleCombo( CComboBox& comBoScale, CString strConfigPath, CConfigLibManager* pCfgLibMan)
{
	UpdateData(TRUE);
	comBoScale.ResetContent();
    if (!IsValidConfigPath(strConfigPath))
    {
		return FALSE;
    }
	if (!pCfgLibMan)
	{
		return FALSE;
	}
	pCfgLibMan->LoadConfig(strConfigPath);
	if (pCfgLibMan->GetConfigLibCount()<1)
	{
		return FALSE;
	}
	//���comboScale;
    for (int i=0; i<pCfgLibMan->GetConfigLibCount();i++)
    {
		
		CString strData;
		
		for (int j=0; j<comBoScale.GetCount();j++)
		{
			comBoScale.GetLBText(j,strData);
			DWORD iComBoScale = atoi(strData);

			if (pCfgLibMan->GetConfigLibItem(i).GetScale() < iComBoScale)
			{
				break;
			}
		}
		strData.Format("%d",pCfgLibMan->GetConfigLibItem(i).GetScale());		
		int nIndex = comBoScale.InsertString(j,strData);
		comBoScale.SetItemData(nIndex,i);
    }
	

	UpdateData(FALSE);

	return TRUE;
}

BOOL CSchemeMerge::FillMergeDlgScaleCombo( CComboBox& comBoScale,CConfigLibManager*& pCfgLibMan, ConfigLibItem& Config, 
										  ConfigLibItem& backConfig,CString& backupSchemeXML,int& scale,UINT nSchemePos)
{
	UpdateData(TRUE);
	comBoScale.ResetContent();
	if (!pCfgLibMan)
	{	
		//{2012.7.30
		pCfgLibMan = new CConfigLibManager(); 
		pCfgLibMan->Copy(gpCfgLibMan);
	     //}
	}

	DWORD minScale = MAXLONG ;
	DWORD maxScale = 0;
	//for test
	long nCount = pCfgLibMan->GetConfigLibCount();
	for (int i=0; i<pCfgLibMan->GetConfigLibCount();i++)
	{
		CString strData;
		int j = comBoScale.GetCount();
		int nScale = pCfgLibMan->GetConfigLibItem(i).GetScale();
		if (minScale>nScale)
		{
			minScale = nScale;
		}
		if (maxScale<nScale)
		{
			maxScale = nScale;
		}
		strData.Format("%d",nScale);
		int nIndex = comBoScale.InsertString(j,strData);
		comBoScale.SetItemData(nIndex,i);
	}

	if (nSchemePos==DES)
	{
		//ѡ����С�����ߡ�
		CString strMinScale = "";
		
		if (scale<=0)
		{
			scale = minScale;
			strMinScale.Format("%d",minScale); 
		}
		else
		{
			strMinScale.Format("%d",scale);
		}
		

		comBoScale.SelectString(-1,strMinScale);

	}
	if (nSchemePos==SRC)
	{
		//ѡ���������ߡ�
		CString strMaxScale = "";
		if (scale<=0)
		{
			scale = maxScale;
			strMaxScale.Format("%d",maxScale);
		}
		else
			strMaxScale.Format("%d",scale);
		
		comBoScale.SelectString(-1,strMaxScale);
	}	
	//��ʼ���������á�
	//{hcw,2012.7.30.
    ConfigLibItem tmpConfig = pCfgLibMan->GetConfigLibItemByScale(scale);
	CString strPath = tmpConfig.GetPath();
    Config.SetScalePath(strPath); 
	Config.Load();
	//}
	backConfig = Config;
	backupSchemeXML.Empty();
	Config.pScheme->WriteTo(backupSchemeXML);
	UpdateData(FALSE);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CSchemeMerge message handlers


BOOL CSchemeMerge::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_DesConfig.Create();
	m_HandleMerge.EnableWindow(FALSE);
	m_UpDifs.EnableWindow(FALSE);
	m_DownDifs.EnableWindow(FALSE);
	m_Del.EnableWindow(FALSE);
	//{hcw,2012.5.30,��ť��ʾ
	if(!m_ToolTip.Create(this,TTS_ALWAYSTIP))
	{
		TRACE(_T("Unable to create ToolTip :-)"));
		return TRUE;
	}
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_HANDMERGE),"�ֶ��ϲ�ѡ�еĲ����");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_DEL),"ɾ��Ŀ�귽���еĲ����");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_UP),"�����һ����ͬ��");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_DOWN),"�����һ����ͬ��");
	m_ToolTip.SetDelayTime(50);
	//}
	//FillMergeDlgScaleCombo();
	
	//{hcw,2012.7.25,���Զ���Ĺ����ָ��m_pDesConfigLibMan,m_pSrcConfigLibMan�����.

	if (m_bIsSrcTreeLoaded)//hcw,2012.9.20
	{
		FillMergeDlgScaleCombo(m_SrcComBoScale,m_pSrcCfgLibMan,m_SrcConfig,m_BackupSrcConfig,m_BackupCurSrcSchemeXML,m_SrcScale,SRC);
	}

	FillMergeDlgScaleCombo(m_DesComBoScale,m_pDesCfgLibMan,m_DesConfig,m_BackupDesConfig,m_BackupCurDesSchemeXML,m_DesScale,DES);
	//}
	FillEdit();
	
	if (m_bIsSrcTreeLoaded)//hcw,2012.9.20
	{
		FillMergeDlgTree(m_SrcTree, m_SrcUserIdx, m_SrcConfig, m_hSrcTreeItemMap);
		m_SrcBackUpUserIdx = m_SrcUserIdx;
	}

	FillMergeDlgTree(m_DesTree, m_DesUserIdx, m_DesConfig, m_hDesTreeItemMap); //2012.9.20,cancel off
	m_DesBackupUserIdx = m_DesUserIdx;
	
	//{�����ؼ�Ϊ��ʱ,����
	if ((m_SrcTree.GetCount()<=0)
		&&(m_DesTree.GetCount()<=0)) //2012.9.20,remove (m_DesTree.GetCount()<=0)	
	{
		ReSetDlgCtrl();
	}
	//}
	UpdateData(FALSE);

	return TRUE;
}

void CSchemeMerge::MergeGroupandLayer( ConfigLibItem& DesConfig,ConfigLibItem SrcConfig )
{
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> restSchemeInfo;
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> tmpSchemeInfo;
	restSchemeInfo.RemoveAll();
	for (int id = 0 ; id < DesConfig.pScheme->GetLayerDefineCount();id++)
	{
		tmpSchemeInfo.Add(DesConfig.pScheme->GetLayerDefine(id));
	}
	for(int i=0; i<SrcConfig.pScheme->GetLayerDefineCount();i++)
	{
		restSchemeInfo.Add(SrcConfig.pScheme->GetLayerDefine(i));
	}
	
	//�ϲ�SrcSchemeInfo��DesSchemeInfo
	
	m_RestStrGroupArray.RemoveAll();			
    
	for (int iSrc = 0; iSrc < SrcConfig.pScheme->GetLayerDefineCount(); iSrc++)   
    {
		//�ϲ���
		//�����������Ŀ�귽�����У��Ҹ������²���Ϊ�ǿ�,�򽫸������m_RestStrGroupArray��
		CString strGroupName = SrcConfig.pScheme->GetLayerDefine(iSrc)->GetGroupName();
		if ((FindinArray(strGroupName,DesConfig.pScheme->m_strSortedGroupName)<0)
			&&(SrcConfig.pScheme->GetLayerDefine(iSrc)->GetLayerName()!=""))
		{
			int idx = FindinArray(strGroupName,SrcConfig.pScheme->m_strSortedGroupName);
			DesConfig.pScheme->m_strSortedGroupName.InsertAt(idx,strGroupName);
			m_RestStrGroupArray.Add(strGroupName);
		}
		else //�����������ѭ����//�ϲ���
		{
			for (int iDes = 0; iDes < DesConfig.pScheme->GetLayerDefineCount(); iDes++)	
			{
				if ((SrcConfig.pScheme->GetLayerDefine(iSrc)->GetGroupName()==DesConfig.pScheme->GetLayerDefine(iDes)->GetGroupName())
					&&(SrcConfig.pScheme->GetLayerDefine(iSrc)->GetLayerName()==DesConfig.pScheme->GetLayerDefine(iDes)->GetLayerName()))
				{
					int sameIndex = IsExistIn(restSchemeInfo,SrcConfig.pScheme->GetLayerDefine(iSrc));
					while (sameIndex >= 0)
					{
						restSchemeInfo.RemoveAt(sameIndex);
						sameIndex = IsExistIn(restSchemeInfo,SrcConfig.pScheme->GetLayerDefine(iSrc));
					}
					
				}

			}
		}
    }	
	
	m_RestSchemeInfo.Copy(restSchemeInfo);
	//����ͬ�Ĳ����ṹ�����顣
	m_RestLayerInfo.RemoveAll();
	for(int iRest1 = 0; iRest1 < restSchemeInfo.GetSize(); iRest1++ )
	{
		LayerInfo layerInfo;
		layerInfo.strGroup = restSchemeInfo.GetAt(iRest1)->GetGroupName();
		layerInfo.strLayer = restSchemeInfo.GetAt(iRest1)->GetLayerName();
		layerInfo.iLayerCode = restSchemeInfo.GetAt(iRest1)->GetLayerCode();
		m_RestLayerInfo.Add(layerInfo);
	}
	//��ʾ�仯��Ϣ��
	CString str="";
	for (int j=0; j<restSchemeInfo.GetSize(); j++)
	{
		CString strLayerCode;
		strLayerCode.Format("%I64d",restSchemeInfo[j]->GetLayerCode());
		str = str + restSchemeInfo[j]->GetGroupName() + "," +strLayerCode +restSchemeInfo[j]->GetLayerName() + ";";
	}
	
	tmpSchemeInfo.Append(restSchemeInfo);
	BOOL bSpecial = FALSE;
    int siz = tmpSchemeInfo.GetSize();
	for (int k = 0; k < restSchemeInfo.GetSize(); k++)
	{
		bSpecial = FALSE;
		if (restSchemeInfo[k]->GetGroupName().CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			bSpecial = TRUE;
		}
		
		DesConfig.pScheme->AddLayerDefine(restSchemeInfo[k],bSpecial);
		
	}
	
	// �ͷ�ָ��
	m_DesSchemeInfo.RemoveAll();
	m_DesSchemeInfo.Copy(tmpSchemeInfo);
	m_bModified = TRUE;
	return;
}


int CSchemeMerge::IsExistIn( CArray<CSchemeLayerDefine*, CSchemeLayerDefine*>& SchemeInfo, CSchemeLayerDefine* layerInfo )
{
	int nCount = SchemeInfo.GetSize();
	for (int i=0; i<SchemeInfo.GetSize(); i++)
	{
		if ((SchemeInfo[i]->GetGroupName()==layerInfo->GetGroupName())
			&&(SchemeInfo[i]->GetLayerCode()==layerInfo->GetLayerCode()))
		{
			return i;
		}

	}
	return -1;
}


void CSchemeMerge::OnSelChangeDesScaleCombo()
{
	UpdateData(TRUE);
	
	int nComboIndex = m_DesComBoScale.GetCurSel();
	if (nComboIndex == CB_ERR) return;
	CString strScale;
	m_DesComBoScale.GetLBText(nComboIndex,strScale);
	int iDesScale = atoi(strScale);
	
	if (iDesScale==m_DesScale)
	{
		m_bChangedDesComboScale = FALSE;
		return ; //Ŀ�������δ�����ı䣬ֱ���˳���
	}

	
	//Դ������Ŀ�귽��·����ȫ��ͬʱֱ���˳���
	CString strTmpDesSchemePath = m_DesCfgPath + "\\" + strScale;
	if (strTmpDesSchemePath.CompareNoCase(m_SrcSchemePath)==0)
	{
		AfxMessageBox(IDS_ERR_SAMESHEME);
		//�ָ�ԭ����Ŀ�귽�������ߡ�
		CString strCurDesScale = "";
		strCurDesScale.Format("%d",m_DesScale);
		m_DesComBoScale.SelectString(-1,strCurDesScale);
		m_bChangedDesComboScale = FALSE;
		return;
	}
	m_bChangedDesComboScale = TRUE;
	if(m_bModified)
	{

		//�ָ��ڵ�����״̬��
		m_UpDifs.EnableWindow(FALSE);
		m_DownDifs.EnableWindow(FALSE);
		CString szText,szCaption;
		szText = StrFromResID(IDS_SAVE_SCHEME);
		szCaption.LoadString(IDS_ATTENTION);
		if(MessageBox(szText,szCaption,MB_YESNO|MB_ICONASTERISK) == IDYES)
		{
			// �������˳��
			m_DesConfig.pScheme->m_strSortedGroupName.RemoveAll();
			for (int i=0; i<m_DesUserIdx.m_aGroup.GetSize(); i++)
			{
				m_DesConfig.pScheme->m_strSortedGroupName.Add(m_DesUserIdx.m_aGroup.GetAt(i).GroupName);
			}
			
			m_DesConfig.pScheme->Save();
			m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
			m_DesConfig.pPlaceCfgLib->Save();
			m_DesConfig.SaveCellLine();
			m_bSaved = SAVED; //2012.9.13
			//{2012.8.1,for Test Enter Impossible
			if (m_DesCfgPath.CompareNoCase(m_SrcCfgPath)==0)
			{
				//
				m_pSrcCfgLibMan->ResetConfig();
			    m_pSrcCfgLibMan->SetPath("");
				m_pSrcCfgLibMan->LoadConfig(m_SrcCfgPath,FALSE);
				
			}
			//}
		}
		else
		{
			m_bSaved = NOSAVED; //2012.9.13
			AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,0,0);
			m_DesConfig.pScheme->ReadFrom(m_BackupCurDesSchemeXML);
			
		}
		m_bModified = FALSE;
		
	}
	
	if ((m_DesScale<=0)
		||(m_DesScale!=iDesScale))
	{
		m_DesScale = iDesScale;
	}
	if (!m_pDesCfgLibMan)
	{
		m_DesConfig = gpCfgLibMan->GetConfigLibItemByScale(m_DesScale);
	}
	else
	{
		//{2012.8.1
		m_pDesCfgLibMan->ResetConfig();
		m_pDesCfgLibMan->SetPath("");
		m_pDesCfgLibMan->LoadConfig(m_DesCfgPath,FALSE);
		m_DesConfig = m_pDesCfgLibMan->GetConfigLibItemByScale(m_DesScale);
		//}
	}
	//�޸�Ŀ�귽��·��
	m_DesSchemePath = m_DesConfig.GetPath();
	m_MergeSchemePath = m_DesSchemePath;
	//	int nConfigIndex = m_ResComBoScale.GetItemData(nComboIndex);
	UpdateData(FALSE);//2012.8.1
	
	//����Ŀ�귽��
	m_BackupCurDesSchemeXML.Empty();
	m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
	
	//m_hDesTreeItemMap.clear();
	FillMergeDlgTree(m_DesTree,m_DesUserIdx,m_DesConfig,m_hDesTreeItemMap);
	
	//ForBidDel();
	UpdateData(FALSE);
	return;
}

void CSchemeMerge::OnSelChangeSrcScaleCombo()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	int nComboIndex = m_SrcComBoScale.GetCurSel();
	if (nComboIndex == CB_ERR) return;
	//	int nConfigIndex = m_ResComBoScale.GetItemData(nComboIndex);
	CString strScale;
	m_SrcComBoScale.GetLBText(nComboIndex,strScale);
	int iSrcScale = atoi(strScale);
	if (iSrcScale==m_SrcScale)
	{
		m_bChangedSrcComboScale = FALSE;
		return;
	}
	
	//Դ������Ŀ�귽��·����ȫ��ͬʱֱ���˳���
	CString strTmpSrcSchemePath = m_SrcCfgPath+"\\"+strScale;
	if (strTmpSrcSchemePath.CompareNoCase(m_DesSchemePath)==0)
	{
		AfxMessageBox(IDS_ERR_SAMESHEME);
		//�ָ�ԭ���ı����ߡ�
		CString strCurSrcScale = "";
		strCurSrcScale.Format("%d",m_SrcScale);
		m_SrcComBoScale.SelectString(-1,strCurSrcScale);
		m_bChangedSrcComboScale = FALSE;
		return;
	}
	m_bChangedSrcComboScale = TRUE;
	if ((m_SrcScale<=0)
		||(m_SrcScale!=iSrcScale))
	{
		m_SrcScale = iSrcScale;
	}
	if (!m_pSrcCfgLibMan)
	{
		m_SrcConfig = gpCfgLibMan->GetConfigLibItemByScale(m_SrcScale);
	}
	else
	{
		//{hcw,2012.9.17
		m_pSrcCfgLibMan->ResetConfig();
		m_pSrcCfgLibMan->SetPath("");
		m_pSrcCfgLibMan->LoadConfig(m_SrcCfgPath,FALSE); //m_DesCfgPath->m_SrcCfgPath
		//}
		m_SrcConfig = m_pSrcCfgLibMan->GetConfigLibItemByScale(m_SrcScale);
	}
	//�޸�Դ����·��
	m_SrcSchemePath = m_SrcConfig.GetPath();
	UpdateData(FALSE);
	//����
	m_BackupCurSrcSchemeXML.Empty();
	m_SrcConfig.pScheme->WriteTo(m_BackupCurSrcSchemeXML);
	
	//m_hSrcTreeItemMap.clear();
	FillMergeDlgTree(m_SrcTree,m_SrcUserIdx,m_SrcConfig,m_hSrcTreeItemMap);
	
	UpdateData(FALSE);
	//ForBidDel();
	return;
}

void CSchemeMerge::ReLoad(ConfigLibItem &DesConfig, ConfigLibItem &SrcConfig)
{
	if (DesConfig.bLoaded==TRUE)
	{
		DesConfig = m_BackupDesConfig;
	}
	if (SrcConfig.bLoaded==TRUE)
	{
 		SrcConfig = m_BackupSrcConfig;
	}

	
	return;
	
}
void CSchemeMerge::OnButtonMerge() 
{
	// TODO: Add your control notification handler code here

	if(MergeConfig(m_DesConfig,m_SrcConfig)==FALSE)//hcw,2012.5.14
	{
		return;
	};
	//�������ϡ��������ͼ�ꡣ
	if (m_bModified==FALSE)
	{
		m_CurLayerIndex=-1 ;
		return;
	}
	else if(m_RestLayerInfo.GetSize()>0)
	{
		m_UpDifs.EnableWindow(FALSE);
		m_DownDifs.EnableWindow(TRUE);
	}
	else if(m_RestLayerInfo.GetSize()<=0)
	{
		m_UpDifs.EnableWindow(FALSE);
		m_DownDifs.EnableWindow(FALSE);
	}
	//����������ʾ��ͬʱ������֮ͬ����
	UpdateDrawTree();

	//{2012.6.25,for Test;
	int nCellsCount = m_JustAppendedCellNames.GetSize();
	//
	m_CurLayerIndex=-1 ;
	
	
}
//hcw,2012.5.16,�������ڵ��״̬����λ��
HTREEITEM CSchemeMerge::LocateSpecificTreeItem(CMergeTree& treeCtrl, UINT nState, UINT nStateMask,__int64 iLayerCode, CString strNodeText,HTREEITEM hCurItem,BOOL byCode)
{
	if (hCurItem == NULL)
	{
		hCurItem = treeCtrl.GetRootItem();
	}
	
	HTREEITEM hRoot = treeCtrl.GetParentItem(hCurItem);
	if (!hRoot)
	{
		hRoot = hCurItem;
	}
	
	//Ѱ��hCurItem���һ����ѡ�Ľڵ㡣
	HTREEITEM hChild = NULL;
	BOOL bParentNode = FALSE;
	while(hRoot)
	{
		
		hChild = treeCtrl.GetChildItem(hRoot);
		//{���ڵ��ı�����
		if (!byCode)
		{

			if ((treeCtrl.GetItemState(hRoot,nStateMask)&nState)
				||(treeCtrl.GetItemState(hRoot,nStateMask)==nState)) //hcw,2012.7.5				
			{
				CString strRootText = treeCtrl.GetItemText(hRoot);				
				if (strRootText==strNodeText)					
				{
					bParentNode = TRUE;
					goto FINDITEM;
				}
				
			}

		}
		//}
		while(hChild)
		{
			if (!byCode)
			{
				if ((treeCtrl.GetItemState(hChild,nStateMask)&nState)
					||treeCtrl.GetItemState(hChild,nStateMask)==nState)
				{
					CString strChildText = treeCtrl.GetItemText(hChild);
					if(strChildText==strNodeText)
					{
						bParentNode = FALSE;
						goto FINDITEM;
					}
				}
			}	

			if ((treeCtrl.GetItemState(hChild,nStateMask)&nState)
				||(treeCtrl.GetItemState(hChild,nStateMask)==nState)) //GetItemData->GetItemState.
			{
				//judge and exit
				CString strTreeNode = treeCtrl.GetItemText(hChild);
				if (byCode)
				{
					__int64  iTreeNodeLayerCode=-2;
					int idx = strTreeNode.Find(' ');
					CString strLayerCode = strTreeNode.Left(idx);
					iTreeNodeLayerCode = _atoi64(strLayerCode); 
					
					if (((iTreeNodeLayerCode==iLayerCode)
						||(iLayerCode==-2))&&(iTreeNodeLayerCode!=0))
					{
						goto FINDITEM;
					}
				}
				
			}
			hChild = treeCtrl.GetNextItem(hChild, TVGN_NEXT);
		}
		hRoot = treeCtrl.GetNextItem(hRoot, TVGN_NEXT);
	}

FINDITEM:
	if((!hRoot)||(!hChild)) 
	{
		return NULL;	
	}
	
	//�ö���ʾ
	if (byCode)
	{
		HTREEITEM hRoot2 = treeCtrl.GetRootItem();
		while(hRoot2)
		{
			if(hRoot2 !=  hRoot)
			{
				treeCtrl.Expand(hRoot, TVE_COLLAPSE);
			}
			hRoot2 = treeCtrl.GetNextItem(hRoot2,TVGN_NEXT);
		}
		treeCtrl.Expand(hRoot, TVE_EXPAND);
		hRoot = treeCtrl.GetNextSiblingItem(hRoot);
		treeCtrl.Expand(hRoot, TVE_EXPAND);
				
		treeCtrl.Select(hChild, TVGN_FIRSTVISIBLE);
		treeCtrl.ClearSelection();
		treeCtrl.SetItem(hChild, TVIF_STATE,NULL,0,0,TVIS_SELECTED,TVIS_SELECTED,0);
		return hChild;
	}
	else
	{
		if (bParentNode)
		{
			treeCtrl.Select(hRoot,TVGN_FIRSTVISIBLE);
			treeCtrl.ClearSelection();
			treeCtrl.SetItem(hRoot,TVIF_STATE,NULL,0,0,TVIS_SELECTED,TVIS_SELECTED,0); //hcw,2012.7.5
			return hRoot;
		}
		else
		{
			HTREEITEM hRoot2 = treeCtrl.GetRootItem();
			while (hRoot2)
			{
				if (hRoot2!=hRoot)
				{
					treeCtrl.Expand(hRoot, TVE_COLLAPSE);
				}
				hRoot2 = treeCtrl.GetNextItem(hRoot2,TVGN_NEXT);
			}
			treeCtrl.Expand(hRoot,TVE_EXPAND);
			hRoot = treeCtrl.GetNextSiblingItem(hRoot);
			treeCtrl.Expand(hRoot,TVE_EXPAND);

			treeCtrl.Select(hChild,TVGN_FIRSTVISIBLE);
			treeCtrl.ClearSelection();
			treeCtrl.SetItem(hChild,TVIF_STATE,NULL,0,0,TVIS_SELECTED,TVIS_SELECTED,0);
			return hChild;
		}
	}
	
}

void CSchemeMerge::UpdateDrawTree()
{
	
	FillandHighLight(m_SrcTree,m_SrcUserIdx,m_SrcConfig,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo,m_hSrcTreeItemMap,SRC);
	FillandHighLight(m_DesTree,m_DesUserIdx,m_DesConfig,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo,m_hDesTreeItemMap,DES);
	//��m_RestLayerInfo�������ڵ������������
	SortLayerInfo(m_RestLayerInfo,m_DesUserIdx);
	
	//չ��Ŀ�귽����
	if (m_DesGroupStateMap.size()>0)
	{
		ExpandExt(m_DesTree,m_DesGroupStateMap,NULL);
	}
	else
		ExpandAll(m_DesTree,NULL);

	//չ��Դ��
	if (m_SrcGroupStateMap.size()>0)
	{
		ExpandExt(m_SrcTree,m_SrcGroupStateMap,NULL);
	}
	else
		ExpandAll(m_SrcTree,NULL);
	return;
}

BOOL CSchemeMerge::FillandHighLight(CMergeTree& treeCtrl, USERIDX& UserIdx,  ConfigLibItem config,
									UINT nState, UINT nStateMask, CArray<LayerInfo,LayerInfo>& restLayerInfo,
									multimap<int,HTREEITEM>& hTreeItemMap,UINT nSchemePosID)
{
	if ((config.pScheme == NULL))
		return FALSE;
	// ������Ϣ�浽USERIDX��
	UserIdx.Clear();
	hTreeItemMap.clear();
	treeCtrl.DeleteAllItems();
	
	for (int i=0; i<config.pScheme->m_strSortedGroupName.GetSize(); i++)
	{
		LAYGROUP group;

		strcpy(group.GroupName,config.pScheme->m_strSortedGroupName.GetAt(i));

		UserIdx.m_aGroup.Add(group);
	}
	
	for(i=0; i<config.pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine *layer = config.pScheme->GetLayerDefine(i);
		if (!layer)//hcw,2012.7.10
		{
			continue;
		}
		for (int j=0; j<UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
				//				idx.index = i;
				idx.code = layer->GetLayerCode();
				strcpy(idx.FeatureName,layer->GetLayerName());
				strcpy(idx.strAccel,layer->GetAccel());
				idx.groupidx = j;
				
				UserIdx.m_aIdx.Add(idx);
				break;
			}
		}
		
	}
	
	for(i=0; i<config.pScheme->GetLayerDefineCount(TRUE); i++)
	{
		CSchemeLayerDefine *layer = config.pScheme->GetLayerDefine(i,TRUE);
		if (!layer)
		{
			continue;
		}
		for (int j=0; j<UserIdx.m_aGroup.GetSize(); j++)
		{
			if (layer->GetGroupName().CompareNoCase(UserIdx.m_aGroup[j].GroupName) == 0)
			{
				IDX idx;
				//				idx.index = i;
				idx.code = layer->GetLayerCode();

				strcpy(idx.FeatureName,layer->GetLayerName());
				strcpy(idx.strAccel,layer->GetAccel());
				idx.groupidx = j;
				
				UserIdx.m_aIdx.Add(idx);
				break;
			}
		}
		
	}
	
	//�����
	char strR[_MAX_FNAME];
	int  recentidx = -1, specialidx = -1;
	CArray<HTREEITEM,HTREEITEM> m_aRootItem;
	for (i=0; i<UserIdx.m_aGroup.GetSize(); i++)
	{
		LAYGROUP gp;
		gp = UserIdx.m_aGroup.GetAt(i);
		
		if (stricmp(gp.GroupName,"Recent") == 0x00)
			recentidx = i;
		
		if (stricmp(gp.GroupName,StrFromResID(IDS_SPECIALGROUP)) == 0x00)
			specialidx = i;
		
		if (recentidx != i)
		{
			HTREEITEM hRoot = treeCtrl.InsertRoot(_T(gp.GroupName),0,GROUPID_FROMIDX(i));
			treeCtrl.SetItemState(hRoot,0,TVIS_BOLD);//ȥ���������ֺŵļӴ�״̬��
			//{��Դ�����еı��ϲ�����ڵ㴦��ѡ��״̬
			if (FindinArray(gp.GroupName,m_SrcSelectedItemsText)>=0)
			{
				treeCtrl.SetItemState(hRoot,TVIS_SELECTED,TVIS_SELECTED);
				if(nSchemePosID==DES)
				{
					m_hSelectedTreeItemArray[DES-1] = hRoot;
				}
			}
			//}
			m_aRootItem.Add(hRoot);
		}
		
	}
	
	for (i=0; i<UserIdx.m_aIdx.GetSize(); i++)
	{
		IDX idx;
		idx = UserIdx.m_aIdx.GetAt(i);
		
		if (idx.groupidx != recentidx)
		{
			if (idx.groupidx == specialidx)
			{
				if (strlen(idx.strAccel) <= 0)
				{
					sprintf(strR,"%s",idx.FeatureName);
				}
				else
				{
					sprintf(strR,"%s^%s",idx.FeatureName,idx.strAccel);
				}
			}
			else
			{
				if (strlen(idx.strAccel) <= 0)
				{
					sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
				}
				else
				{
					sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
				}
			}
			
			
			// �Դ洢��recent���µ��飬Ӧ�������ʱ������ӵ�recent�����棬�����������(�ϰ汾��ʽ)
			if (recentidx != -1 && idx.groupidx > recentidx)
			{

				treeCtrl.InsertSubItem(_T(strR),m_aRootItem.GetAt(idx.groupidx-1),0,CODEID_FROMIDX(i));

			}
			else
			{

				HTREEITEM item = treeCtrl.InsertSubItem(_T(strR),m_aRootItem.GetAt(idx.groupidx),0,CODEID_FROMIDX(i));

				hTreeItemMap.insert(make_pair(idx.code,item));
				// ����Ԥ��ѡ���Ĳ���ѡ��
				// hcw,2012.4.26,���ò�ڵ��״̬��
				CString strLayerNodeTxt = "";
				strLayerNodeTxt.Format("%s",strR);
				if (hasFoundSameLayerin(restLayerInfo,strLayerNodeTxt)>=0) //m_RestLayerInfo->restLayerInfo
				{
					//treeCtrl.SetCheck(item, TRUE);
					treeCtrl.SetItemData(item,nState);
					treeCtrl.SetItemState(item,nState,nStateMask); //hcw,2012.5.16,TVIS_SELECTED->TVIS_BOLD
					//{��Դ�����б��ϲ��Ĳ�ڵ㴦��ѡ��״̬
					if (FindinArray(strLayerNodeTxt,m_SrcSelectedItemsText)>=0)
					{
						treeCtrl.SetItemState(item,TVIS_SELECTED,TVIS_SELECTED);
						if (nSchemePosID==DES)
						{
							m_hSelectedTreeItemArray[DES-1] = item;
						}
					}
					//}
				}
 			}
		}
		
	}
	
	m_DesBackupUserIdx = UserIdx;
	return TRUE;
}
int CSchemeMerge::hasFoundSameLayerin(CArray<LayerInfo,LayerInfo>& restLayerInfo, CString strLayerNodeTxt)
{
	if ((restLayerInfo.GetSize()<=0)
		||(strLayerNodeTxt==""))
	{
		return -1;
	}
	for (int i=0; i<restLayerInfo.GetSize();i++)
	{
		
		CString strGroupName = restLayerInfo.GetAt(i).strGroup;
		CString strTmpLayerNodeTxt = "";
		if (strGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0) //hcw,2012.7.4
		{
			strTmpLayerNodeTxt = restLayerInfo.GetAt(i).strLayer;
		}
		else
		{
			
			strTmpLayerNodeTxt.Format("%I64d",restLayerInfo.GetAt(i).iLayerCode);
			strTmpLayerNodeTxt = strTmpLayerNodeTxt+" "+restLayerInfo.GetAt(i).strLayer;
		}
		if (strTmpLayerNodeTxt == strLayerNodeTxt)
		{
			return i;
		}
		
	}
	return -1;
}
//hcw,2012.4.26,for test Loop paint in TreeCtrl
void CSchemeMerge::HighLightDiff()
{
	//highLight the Destination Tree(Left) and  resource Tree(right)

	for (int i=0; i < m_RestLayerInfo.GetSize(); i++)
	{	
		TravelandSetItemState(m_SrcTree,TVIS_SELECTED, TVIS_SELECTED,NULL, m_RestLayerInfo[i],m_RestStrGroupArray);
		TravelandSetItemState(m_DesTree, TVIS_SELECTED, TVIS_SELECTED, NULL ,m_RestLayerInfo[i],m_RestStrGroupArray);
	}
	//judge and cancel off the selected state of root ;
	HTREEITEM hRoot = m_DesTree.GetRootItem();
	CString strGroupName = m_DesTree.GetItemText(hRoot);

	if (FindinArray(strGroupName,m_RestStrGroupArray)>0)
	{
		m_DesTree.SetItemState(hRoot, 0, TVIS_SELECTED);
	}
	return;
}
void CSchemeMerge::TravelandSetItemState(CMergeTree& treeCtrl, UINT nState, UINT nStateMask, HTREEITEM hItem, CSchemeLayerDefine* schemeInfo, CStringArray& StrGroupArray)
{
	if (NULL == hItem)
	{
		hItem = treeCtrl.GetRootItem();
		while (hItem!=NULL)
		{
			TravelandSetItemState(treeCtrl,nState,nStateMask,hItem,schemeInfo, StrGroupArray);
			hItem = treeCtrl.GetNextSiblingItem(hItem);
		}
		return;
	}
	HTREEITEM hChild = NULL;
	
	if (IsCorrespondItem(treeCtrl,hItem,schemeInfo,StrGroupArray))
	{
		CString str = treeCtrl.GetItemText(hItem);
		treeCtrl.SetItemState(hItem, nState, nStateMask);
		if (nState==TVIS_SELECTED)
		{
			treeCtrl.SetCheck(hItem, TRUE);
		}
		treeCtrl.SetFocus();
	}
	
	hChild = treeCtrl.GetChildItem(hItem);

	while (hChild!=NULL)
	{
		TravelandSetItemState(treeCtrl, nState, nStateMask, hChild, schemeInfo, StrGroupArray);
		hChild = treeCtrl.GetNextSiblingItem(hChild);
	}
	return;
}

//���أ�����LayerInfo��Ӧ�����ڵ��״̬��
void CSchemeMerge::TravelandSetItemState( CMergeTree& treeCtrl, UINT nState, UINT nStateMask, HTREEITEM hItem, LayerInfo layerInfo, CStringArray& StrGroupArray )
{
	if (NULL == hItem)
	{
		hItem = treeCtrl.GetRootItem();
		while (hItem!=NULL)
		{
			TravelandSetItemState(treeCtrl,nState,nStateMask,hItem,layerInfo,StrGroupArray);
			hItem = treeCtrl.GetNextSiblingItem(hItem);
		}
		return;
	}
	HTREEITEM hChild = NULL;
	
	if (IsCorrespondItem(treeCtrl,hItem,layerInfo,StrGroupArray))
	{
		CString str = treeCtrl.GetItemText(hItem);
		treeCtrl.SetItemState(hItem, nState, nStateMask);
		if (nState==TVIS_SELECTED)
		{
			treeCtrl.SetCheck(hItem, TRUE);
		}
		//���ӽڵ��ϵĸ��ڵ㱻ѡ�У���ȡ�����ڵ�ѡ�С�
		
		HTREEITEM hParent = treeCtrl.GetParentItem(hItem);
		HTREEITEM hChild = treeCtrl.GetChildItem(hItem);
		if ((hParent)&&treeCtrl.GetCheck(hParent))
		{
			treeCtrl.SetItemState(hParent, 0, TVIS_SELECTED);
			treeCtrl.SetCheck(hParent,FALSE);
		}

		treeCtrl.SetFocus();
	}
	
	hChild = treeCtrl.GetChildItem(hItem);
	
	while (hChild!=NULL)
	{
		TravelandSetItemState(treeCtrl, nState, nStateMask, hChild, layerInfo, StrGroupArray);
		hChild = treeCtrl.GetNextSiblingItem(hChild);
	}
	return;	
}



void CSchemeMerge::ExpandAll( CMergeTree& treeCtrl,HTREEITEM hItem )
{
	if (NULL==hItem)
	{
		hItem = treeCtrl.GetRootItem();
		while (NULL != hItem)
		{
			ExpandAll(treeCtrl,hItem);
			hItem = treeCtrl.GetNextItem(hItem,TVGN_NEXT);
		}
		return;
	}
	
	HTREEITEM hChild = NULL;
	treeCtrl.Expand(hItem,TVE_EXPAND);
	hChild = treeCtrl.GetNextItem(hItem,TVGN_CHILD);
	
	while (NULL!=hChild)
	{
		ExpandAll(treeCtrl,hChild);
		hChild = treeCtrl.GetNextItem(hChild,TVGN_NEXT);
	}
}

void CSchemeMerge::ExpandExt(CMergeTree& treeCtrl, map<CString, UINT>& GroupStateMap,HTREEITEM hItem)
{
	if (NULL==hItem)
	{
		hItem = treeCtrl.GetRootItem();
		while (NULL!=hItem)
		{
			ExpandExt(treeCtrl,GroupStateMap,hItem);
			hItem = treeCtrl.GetNextItem(hItem,TVGN_NEXT);
		}
		return;
	}
	HTREEITEM hChild = NULL;
	HTREEITEM hParent = treeCtrl.GetParentItem(hItem);
    CString  strItemText = treeCtrl.GetItemText(hItem);
	int nSelectedState = GetNodeState(GroupStateMap,strItemText);

	if(!hParent)
	{
		if (nSelectedState&TVIS_EXPANDED)
		{
			treeCtrl.Expand(hItem,TVE_EXPAND);
		}
		
	};
	hChild = treeCtrl.GetNextItem(hItem,TVGN_CHILD);
	while (NULL!=hChild)
	{
		ExpandExt(treeCtrl,GroupStateMap,hChild);
		hChild = treeCtrl.GetNextItem(hChild,TVGN_NEXT);
	}
	return;	
}

void CSchemeMerge::OnCancel() 
{
	// TODO: Add extra cleanup here
	CString szText,szCaption;

	int nResult;
	if (m_bModified)
	{
		szText.LoadString(IDS_SAVE_SCHEME);
		nResult = MessageBox(szText,szCaption,MB_YESNOCANCEL|MB_ICONASTERISK);
	}
	else
	{
		szText.LoadString(IDS_CONFIRM_CLOSE);
		nResult = MessageBox(szText,szCaption,MB_OKCANCEL|MB_ICONASTERISK);
	}

	if (nResult==IDCANCEL)
    {
		return;
    }
	
	//��ʾ����
	if (m_bModified)
	{
		if (nResult == IDYES)
		{
			// �������˳��
			m_DesConfig.pScheme->m_strSortedGroupName.RemoveAll();
			for (int i=0; i<m_DesUserIdx.m_aGroup.GetSize(); i++)
			{
				m_DesConfig.pScheme->m_strSortedGroupName.Add(m_DesUserIdx.m_aGroup.GetAt(i).GroupName);
			}

			m_DesConfig.pScheme->Save();			
			m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
			
			//pPlaceCfgLib�ı��档
			m_DesConfig.pPlaceCfgLib->Save();

			//pCellDefLib��pLinetypeLib�ı���
			m_DesConfig.SaveCellLine();
			m_bModified = FALSE;
		}
		else if(nResult==IDNO)
		{
			// ֹͣCCollectViewBar���£���ֹ�������¼��س��ֵ�bug
			AfxGetMainWnd()->SendMessage (FCCM_COLLECTIONVIEW,0,0);
			m_DesConfig.pScheme->ReadFrom(m_BackupCurDesSchemeXML);	//hcw,2012.6.8,recover from cancel off.
			m_bModified = FALSE;
		}
	}

	//{hcw,2012.7.30
	m_pDesCfgLibMan->ResetConfig();
	CString strDesConfigPath = m_pDesCfgLibMan->GetPath();
	m_pDesCfgLibMan->SetPath("");
	m_pDesCfgLibMan->LoadConfig(strDesConfigPath,FALSE);
	//}
	//{2012.8.2
	if (m_SrcCfgPath.CompareNoCase(m_DesCfgPath)==0)
	{
		m_pSrcCfgLibMan->SetPath("");
		m_pSrcCfgLibMan->LoadConfig(m_SrcCfgPath);
	}
	//}

	KillTimer(TIMERID_CREATEIMAGE);
	KillTimer(TIMERID_PREVIEW);

	CDialog::OnCancel();
}

int CSchemeMerge::FindinArray(CString str, CStringArray& strArray, BOOL bCase)
{
	if (strArray.GetSize()<=0)
	{
		return -1;
	}
	for (int i = 0; i < strArray.GetSize(); i++ )
	{
		switch (bCase)
		{
			case FALSE:
				if (str.CompareNoCase(strArray[i])==0) //hcw,2012.6.22,==��CompareNoCase
				{
					return i;
				}
				
			case TRUE:
			default:
				if (str==strArray[i])
				{
					return i;
				}				
			
		}

	}
	return -1;
}

BOOL CSchemeMerge::IsCorrespondItem( CMergeTree&treeCtrl,HTREEITEM hItem, CSchemeLayerDefine* schemeInfo, CStringArray& restGroupNameArray)
{
	CString strNode = treeCtrl.GetItemText(hItem);
	__int64 iLayerCode = 0;
	CString strLayerName = ""; 
	sscanf(strNode,"%I64d %s",&iLayerCode,strLayerName);


	HTREEITEM hParentItem = treeCtrl.GetParentItem(hItem);
	HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);
	if ((!hParentItem)
		&&(!hChildItem))//���ӽڵ�ĸ��ڵ�,����ڵ�Ƚϡ�
	{
		CString strGroupName = treeCtrl.GetItemText(hItem);
		if (FindinArray(strGroupName,restGroupNameArray)>0)
		{
			return TRUE;
		}
		else
		   return FALSE;
		
	}
	else 
	{
		CString str = "";
		str	= treeCtrl.GetItemText(hParentItem);
		if ((str==schemeInfo->GetGroupName())
			&&(iLayerCode==schemeInfo->GetLayerCode()))
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CSchemeMerge::IsCorrespondItem( CMergeTree&treeCtrl,HTREEITEM hItem, LayerInfo layerInfo, CStringArray& restGroupNameArray )
{
	if ((!treeCtrl.GetParentItem(hItem))
		&&(FindinArray(treeCtrl.GetItemText(hItem),restGroupNameArray)>0))
	{

		return TRUE;
	}
	else 
	{
		HTREEITEM hParentItem = treeCtrl.GetParentItem(hItem);
		CString strTreeNode;

		strTreeNode.Format("%I64d",layerInfo.iLayerCode);

		strTreeNode = strTreeNode+" "+layerInfo.strLayer ;
		if ((treeCtrl.GetItemText(hParentItem)==layerInfo.strGroup)
			&&(treeCtrl.GetItemText(hItem)==strTreeNode))
		{
			return TRUE;
		}
	}
	return FALSE;
	
}

CSchemeMerge::~CSchemeMerge()
{
	
}
void CSchemeMerge::ReSetDlgCtrl()
{
	UpdateData(TRUE);
	//��ѡ��:������
	m_DesComBoScale.ResetContent();
	m_SrcComBoScale.ResetContent();
	//�༭��:·��
	m_DesSchemePath = "";
	m_SrcSchemePath = "";
	m_MergeSchemePath = "";
	//��
	m_SrcTree.DeleteAllItems();
	m_DesTree.DeleteAllItems();
	//button
	m_UpDifs.EnableWindow(FALSE); //���������ͬ�
	m_DownDifs.EnableWindow(FALSE);//���������ͬ�
	m_HandleMerge.EnableWindow(FALSE); //�ֶ��ϲ���ť��
	m_AutoMerge.EnableWindow(FALSE);
	m_Del.EnableWindow(FALSE);
	UpdateData(FALSE);
	return;
}
void CSchemeMerge::OnDeltaposMergeSpin( NMHDR* pNMHDR, LRESULT* pResult )
{

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	int nCount = m_RestLayerInfo.GetSize();
	if (pNMUpDown->iDelta == 1)
	{
		//����
		if (nCount<=0)
		{
			return;
		}
		m_CurLayerIndex++;

		if (m_CurLayerIndex < m_RestLayerInfo.GetSize())
		{
			if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			}
			else
			{
				m_hSelectedTreeItemArray[DES-1]=LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
				m_hSelectedTreeItemArray[SRC-1]=LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			}

		
		}
		else
		{
			m_CurLayerIndex = m_RestLayerInfo.GetSize()-1;
			if (m_CurLayerIndex<0)
			{
				m_CurLayerIndex=0;
			}
			if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			}
			else
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			}
			
		}
		
	}
	
	if (pNMUpDown->iDelta == -1)
	{
		//����
		
		if (nCount<=0)
		{
			return;
		}		
		m_CurLayerIndex--;
		if (m_CurLayerIndex >= 0)
		{
			if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			}
			else
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			}

		}
		else
		{
			m_CurLayerIndex = 0;
			if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			}
			else
			{
				m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
				m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			}
		}
		
	}
}

void CSchemeMerge::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	if (m_SchemePosID == DES)
	{
		if (IDYES==AfxMessageBox(IDS_DEL_CONFIRM,MB_YESNO|MB_ICONQUESTION))
		{
			//hcw,2012.5.10,��Ӷ�ѡ֧�֡�
			CTreeItemList curSelectedTreeNodeList;
			BOOL bHasDeledCurItem = FALSE;
			m_DesTree.GetSelectedList(curSelectedTreeNodeList);
			int nCount = curSelectedTreeNodeList.GetCount();
			HTREEITEM hPreItem = NULL;
			while (!curSelectedTreeNodeList.IsEmpty())
			{
				m_hSelectedItem = (HTREEITEM)curSelectedTreeNodeList.GetHead();

				CString strLayerNodeTxt = m_DesTree.GetItemText(m_hSelectedItem);
				CString strGroupName = "";//hcw,2012.7.3,for SpecialItem
				CString strLayerName = "";
				if (m_DesTree.GetParentItem(m_hSelectedItem)) //hcw,2012.7.3,for SpecialItem
				{
					HTREEITEM hItem = m_DesTree.GetParentItem(m_hSelectedItem);
					strGroupName = m_DesTree.GetItemText(hItem);
				}
				else
					strGroupName = strLayerNodeTxt;
				BOOL bSpecial = FALSE;

				__int64 num = -2;
				sscanf(strLayerNodeTxt,"%I64d %s",&num,strLayerName.GetBuffer(256));
				if (strGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
				{
					bSpecial = TRUE;
				}
				
				if (!m_DesTree.GetParentItem(m_hSelectedItem))
				{
					DelGroup(strLayerNodeTxt,m_DesUserIdx,m_DesConfig,bSpecial);
					//hcw,2012.7.10,ɾ��m_RestLayerInfo����Ӧ����Ϣ,���ڶԱ�ʱ���.
					int nDes = hasFoundSameLayerin(m_RestLayerInfo,strLayerNodeTxt);
					if (nDes>=0)
					{
						m_RestLayerInfo.RemoveAt(nDes);
					}
					m_nDesSelectedGroupIndex = -1;
					//ͬʱɾ����������Ӧ�ĵ㡣
				}
				else
				{	
					DelLayer(num,strLayerNodeTxt,m_DesUserIdx,m_DesConfig,bSpecial);
					//hcw,2012.7.10,ɾ��m_RestLayerInfo����Ӧ����Ϣ�����ڶԱ�ʱ���.
					int nDes = hasFoundSameLayerin(m_RestLayerInfo,strLayerNodeTxt);
					if (nDes>=0)
					{
						m_RestLayerInfo.RemoveAt(nDes);
					}
				}
				strLayerName.ReleaseBuffer(); //hcw,2012.7.2,transfer to here
				if (isValidDelItem(m_DesTree,m_hSelectedItem))
				{
					hPreItem = m_DesTree.GetPrevSiblingItem(m_hSelectedItem);
					m_DesTree.DeleteItem(m_hSelectedItem); //hcw,2012.4.28,2012.7.3,modified
					bHasDeledCurItem = TRUE;
				}
				else if(bSpecial&&(m_bSpecialRtn==IDYES))
				{
					hPreItem = m_DesTree.GetPrevSiblingItem(m_hSelectedItem);
					HTREEITEM hItem = m_DesTree.GetChildItem(m_hSelectedItem);
					HTREEITEM hTempItem = NULL;
					while(hItem)
					{
						hTempItem = hItem;
						hItem = m_DesTree.GetNextItem(hItem,TVGN_NEXT);
						m_DesTree.DeleteItem(hTempItem);						
					}
				}

				//hcw,2012.5.25
				curSelectedTreeNodeList.RemoveHead();
			}

			//��ʾ	
			if (bHasDeledCurItem)
			{
				HTREEITEM hSiblingofSelectedItem = m_DesTree.GetNextItem(m_hSelectedItem, TVGN_NEXTVISIBLE);
				if ((m_DesTree.GetParentItem(m_hSelectedItem)==NULL)
					&&hPreItem&&hSiblingofSelectedItem) //TVGN_NEXT:
				{
					m_hSelectedItem = hPreItem;
				}
				CString strCurItem = m_DesTree.GetItemText(m_hSelectedItem);
				m_DesTree.ClearSelection();
				m_DesTree.Select(m_hSelectedItem,TVGN_FIRSTVISIBLE);
				m_DesTree.SetItemState(m_hSelectedItem,TVIS_SELECTED,TVIS_SELECTED);
				

				m_hSelectedTreeItemArray[DES-1] = m_hSelectedItem; //hcw,2012.7.9
			}
			//hcw,2012.7.10,���û��������ͻҵ����ϡ����µ������ť��
			int nRestCount = m_RestLayerInfo.GetSize(); //for monitor;
			
			if (m_RestLayerInfo.GetSize()<=0)
			{
				m_UpDifs.EnableWindow(FALSE);
				m_DownDifs.EnableWindow(FALSE);
			}

			m_bModified = TRUE;
			m_bSpecialRtn = FALSE;
		}
		return ;
	
	}	
	
	return;
}

int CSchemeMerge::FindCorrespondItem(CMergeTree& treeCtrl,HTREEITEM hItem, ConfigLibItem Config)
{
	CString strLayerNodeTxt="";	
	for (int idx=0; idx < Config.pScheme->GetLayerDefineCount(); idx++)
	{
		CSchemeLayerDefine *pdef = Config.pScheme->GetLayerDefine(idx);
		strLayerNodeTxt = "";
		strLayerNodeTxt.Format("%I64d",pdef->GetLayerCode());
		strLayerNodeTxt = strLayerNodeTxt+" "+pdef->GetLayerName();
		
		if (strLayerNodeTxt==treeCtrl.GetItemText(hItem))
		{
			return idx;
		}
	}
	return -1;
}


void CSchemeMerge::OnSelchangedTreeDes( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMTREEVIEW *pTree = (NMTREEVIEW *)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	m_hSelectedItem = hItem;
    CString strTreeNodeText = m_DesTree.GetItemText(hItem);
	if (!m_DesTree.GetParentItem(hItem))
	{
		CString strLayerTxt;
		m_CurLayerIndex = GetFirstLayerTxtofGroup(m_RestLayerInfo,strTreeNodeText,strLayerTxt);
	}
	else
		m_CurLayerIndex = hasFoundSameLayerin(m_RestLayerInfo,strTreeNodeText);

	SetUpDownButtonState(m_CurLayerIndex);
	m_DesTree.SelectItem(hItem);	
	m_SchemePosID = DES;
	UpdateData(TRUE);
	UpdateData(FALSE);
	m_hSelectedTreeItemArray[DES-1] = hItem;
	
}

void CSchemeMerge::OnSelchangedTreeSrc( NMHDR* pNMHDR, LRESULT* pResult )
{
	NMTREEVIEW *pTree = (NMTREEVIEW *)pNMHDR;
	HTREEITEM hItem = pTree->itemNew.hItem;
	m_hSelectedItem = hItem;
	m_SrcTree.SelectItem(hItem);
	m_SchemePosID = SRC;
	UpdateData(TRUE);
	m_HandleMerge.EnableWindow(TRUE);
	UpdateData(FALSE);
	m_hSelectedTreeItemArray[SRC-1] = hItem;

}

void CSchemeMerge::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
}

void CSchemeMerge::FillEdit()
{
	UpdateData(TRUE);

	if(m_bIsSrcTreeLoaded) //2012.9.20
	{
		m_SrcSchemePath = m_SrcConfig.GetPath();
	}
	m_SrcCfgPath = m_pSrcCfgLibMan->GetPath();

	m_DesSchemePath = m_DesConfig.GetPath();
	m_MergeSchemePath = m_DesConfig.GetPath();
	m_DesCfgPath = m_pDesCfgLibMan->GetPath();

	//}
	UpdateData(FALSE);
}

void CSchemeMerge::DelGroup(CString strGroupName,USERIDX& UserIdx, ConfigLibItem& Config, BOOL bSpecial)
{
	int idx = -2;
 	idx=FindinArray(strGroupName,Config.pScheme->m_strSortedGroupName);
	if ( idx<0 )
 	{
		return;
	}

	if (strGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))!=0) //hcw,2012.7.3,��Ӷ�����������жϡ�
	{
		int iDes = FindinArray(strGroupName,Config.pScheme->m_strSortedGroupName);
		if (iDes>=0)
		{
			Config.pScheme->m_strSortedGroupName.RemoveAt(iDes);
		}
	}

	//��Ϊ����������ʱ������ʾ��
	if (bSpecial)
	{
		if (Config.pScheme->GetLayerDefineCount(bSpecial)<=0)
		{
			return;
		}
		int iRtn=0;
		iRtn=AfxMessageBox(IDS_DEL_CONFIRM,MB_YESNO);
		m_bSpecialRtn = iRtn;
		if (iRtn==IDNO)
		{
			return;
		}

	}
	for(int i=0;i<Config.pScheme->GetLayerDefineCount(bSpecial);i++)
	{
		CSchemeLayerDefine *pdef = Config.pScheme->GetLayerDefine(i,bSpecial);
		CString strTmpGroupName = pdef->GetGroupName();

		if(strGroupName.CompareNoCase(pdef->GetGroupName()) == 0)
		{

			Config.pScheme->DelLayerDefine(i,TRUE,bSpecial);
			i--;
		}
	}
	int nCount = Config.pScheme->GetLayerDefineCount(bSpecial);
	//����USERIDX;
	int nGroup;

	// Ѱ��ѡ�������
	for(i=0;i<UserIdx.m_aGroup.GetSize();i++)
	{
		if(UserIdx.m_aGroup[i].GroupName == strGroupName)
		{
			nGroup = i;	
			break;
		}
	}
	// ɾ��С������
	for(i=0;i<UserIdx.m_aIdx.GetSize();i++)
	{
		//�޸�m_UserIdx
		if(UserIdx.m_aIdx[i].groupidx == nGroup)
		{
			UserIdx.m_aIdx.RemoveAt(i);
			i--;
		}
		// ���Ĵ�������������groupidx
		else if(UserIdx.m_aIdx[i].groupidx > nGroup)
		{
			UserIdx.m_aIdx[i].groupidx--;
		}
		
	}
	UserIdx.m_aGroup.RemoveAt(nGroup);
		
}

void CSchemeMerge::DelLayer( __int64 iLayerCode, CString strLayerName,USERIDX& UserIdx, ConfigLibItem& Config,BOOL bSpecial)
{

	for (int idx1=0; idx1 < Config.pScheme->GetLayerDefineCount(bSpecial); idx1++)
	{
		CSchemeLayerDefine* pDef = Config.pScheme->GetLayerDefine(idx1,bSpecial);
		//ͨ�������жϡ�
		if (pDef==NULL)
		{
			continue;
		}
		if (!bSpecial)
		{
			if (pDef->GetLayerCode()==iLayerCode)
			{
				
				Config.pScheme->DelLayerDefine(idx1,TRUE,bSpecial);
				idx1--;
				//break;
			}
		}
		else
		{
			if (pDef->GetLayerName()==strLayerName)
			{
				Config.pScheme->DelLayerDefine(idx1,TRUE,bSpecial);
				//idx1--;
			}
		}

	}

	//ɾ��m_UserIdx�е�����
	
	for (int i=0; i < UserIdx.m_aIdx.GetSize(); i++)
	{
		if (!bSpecial)
		{
			if (UserIdx.m_aIdx[i].code == iLayerCode)
			{
				UserIdx.m_aIdx.RemoveAt(i);	
				i--;
				//break;
			}
		}
		else
		{
			if (UserIdx.m_aIdx[i].FeatureName == strLayerName)
			{
				UserIdx.m_aIdx.RemoveAt(i);
			//	i--;
			}
		}

	}
	return;
}
//����DelLayer;
void CSchemeMerge::DelLayer( CString strNodeText, USERIDX& UserIdx, ConfigLibItem& Config )
{
	int nCountBefore = Config.pScheme->GetLayerDefineCount(FALSE);
	for(int idx=0; idx < Config.pScheme->GetLayerDefineCount(FALSE); idx++)
	{
		CSchemeLayerDefine *pDef = Config.pScheme->GetLayerDefine(idx, FALSE);	    
		CString strCombinedNodeTxt;
		
		strCombinedNodeTxt.Format("%I64d", pDef->GetLayerCode());
		strCombinedNodeTxt = strCombinedNodeTxt + " " + pDef->GetLayerName();
		if (strNodeText==strCombinedNodeTxt)
		{
			Config.pScheme->DelLayerDefine(idx);
			idx--;
		}
		
	}
    int nCountAfter = Config.pScheme->GetLayerDefineCount(FALSE);
	//ɾ��UserIdx�е�����
	for(int j=0; j < UserIdx.m_aIdx.GetSize(); j++)
	{
		CString strNodeTextbyUserIdx;
		strNodeTextbyUserIdx.Format("%d",UserIdx.m_aIdx[j].code);
		strNodeTextbyUserIdx = strNodeTextbyUserIdx + " " + CString(UserIdx.m_aIdx[j].FeatureName);
		
		if (strNodeTextbyUserIdx == strNodeText)
		{
			UserIdx.m_aIdx.RemoveAt(j);	
			j--;
			
		}
	}
	//ɾ��ӳ��m_hDesTreeItemMap�е����ݡ�
	int iCode;
	char strLayerName[128];
	sscanf(strNodeText,"%i %s",&iCode,strLayerName);
	DeleteItemfromMap(iCode,m_hDesTreeItemMap);
	
}

void CSchemeMerge::OnButtonHandmerge() 
{
	// TODO: Add your control notification handler code here
		//�ж�ԴĿ�������Ƿ���ѡ�нڵ㡣
		if (m_hSelectedTreeItemArray[SRC-1]==NULL)
		{
			AfxMessageBox(IDS_ERR_SCHEME_NOSELECTITEMS,MB_OK);
			m_bUnMerged = -1;
			return;
		}
		if (m_hSelectedTreeItemArray[DES-1]==NULL)
		{
			AfxMessageBox(IDS_ERR_SCHEME_NOSELECTITEMS,MB_OK);
			m_bUnMerged = -1;
			return;
		}
		//��ȡԴ��Ŀ��������ڵ��չ��״̬��
	    m_JustAppendedLineTypeNames.RemoveAll();
		m_JustAppendedCellNames.RemoveAll();
		m_SrcGroupStateMap.clear();
		m_DesGroupStateMap.clear();
		GetGroupStateMap(m_SrcTree,m_SrcGroupStateMap,TVIS_EXPANDED);
		GetGroupStateMap(m_DesTree,m_DesGroupStateMap,TVIS_EXPANDED);
		
		m_nDesIndex = -1;
		m_bFirstSelectedIndex = TRUE;
		m_RestLayerInfo.RemoveAll(); //�����ͬ���
		CTreeItemList curSrcSelectedTreeNodeList;
		m_SrcTree.GetSelectedList(curSrcSelectedTreeNodeList);
		m_SrcSelectedItemsText.RemoveAll();
		GetSelectedItemsText(m_SrcTree,curSrcSelectedTreeNodeList,m_SrcSelectedItemsText);
		int n = curSrcSelectedTreeNodeList.GetCount();
		//��Դ�����е�ѡ�з�����ѭ������Ŀ�귽����
		POSITION pos;
		set<CString> tmpSetParentItemTxt;
		CStringArray tmpSelectedItemTxt;

		//��ϲ�
		for(pos=curSrcSelectedTreeNodeList.GetHeadPosition(); 
			pos!=NULL;)
		{
			HTREEITEM hItem = (HTREEITEM)curSrcSelectedTreeNodeList.GetNext(pos);
			
			HTREEITEM hParentItem = m_SrcTree.GetParentItem(hItem);

			if (hParentItem)
			{				
				//{��ѡʱ�ĺϲ�
				OnMoveSrcLayer(hItem);
				OnCopyToDesLayer(m_hSelectedTreeItemArray[DES-1]);

				if (m_bUnMerged==-1) //�����Ƿ����룬�磺һ�����Ͷ���������֮����໥�������ʱ
				{
					m_bUnMerged = TRUE;
					m_bFirstPrompt = TRUE;//�ָ�����ʱ��ʾ�ĳ�ֵ�������Զ��ϲ����´��ֶ��ϲ�ʱʹ�á�
					m_bFirstPromptInSymbolLib = TRUE;//�ָ�ͼԪ���ǵ���ĳ�ֵ�������Զ��ϲ����´��ֶ��ϲ���
					m_firstDesSameGroupIndex = -2;
					m_bModified = TRUE;
					return;
				}
				tmpSetParentItemTxt.insert(m_SrcTree.GetItemText(hParentItem));
			}
			else
			{
				continue;
			}
						
		}
		
		//��ϲ�
		m_bSpecialLayer = FALSE;
		m_bFirstSelectedIndex = TRUE; //�ָ���ʼֵ���Ա���ϲ�֮�á�
		m_bFirstPrompt = TRUE;
		m_nDesIndex = -1;//�ָ������ĳ�ֵ��
		//int nCount = curSelectedTreeNodeList.GetCount();
		int nCountSet = tmpSetParentItemTxt.size();
		m_firstDesSameGroupIndex = -2;
		CStringArray tmpParentItemTxtArray;
		
		set<CString>::const_iterator iter = tmpSetParentItemTxt.begin();
		while (iter!=tmpSetParentItemTxt.end())
		{
			tmpParentItemTxtArray.Add(*iter);
			iter++;
		}
		int nCountStrArray = tmpParentItemTxtArray.GetSize();
		m_SelectedGroupsNonExistInDes.RemoveAll(); //��������顣
		for (pos = curSrcSelectedTreeNodeList.GetHeadPosition();
			pos!=NULL;)
		{
			HTREEITEM hItem = (HTREEITEM)curSrcSelectedTreeNodeList.GetNext(pos);
			HTREEITEM hParentItem = m_SrcTree.GetParentItem(hItem);
			
			if (!hParentItem)
			{	
				 int i = 0;
				 if ((i = FindinArray(m_SrcTree.GetItemText(hItem),tmpParentItemTxtArray))>=0)
				 {
					continue;
				 }
				 else
				 {
					OnMoveSrcGroup(hItem);
					OnCopyToDesGroup(m_hSelectedTreeItemArray[DES-1]);
					//���úϲ���Ŀ�귽���е����״̬��
					AddGroupStateMap(m_DesGroupStateMap,m_SelectedGroupsNonExistInDes,0);

				 }
				 if (m_bUnMerged==-1) //ȡ��֮ǰ�ĺϲ�������
				 {
					 m_bUnMerged = TRUE;
					 m_bFirstPrompt = TRUE;//�ָ�������ʾ�ĳ�ֵ,�����Զ��ϲ����´��ֶ��ϲ�ʱʹ�á�
					 m_bFirstPromptInSymbolLib = TRUE; //�ָ�ͼԪ���ǵ���ʾ�ĳ�ֵ,�����Զ��ϲ����´��ֶ��ϲ���
					 m_firstDesSameGroupIndex = -2;
					 m_bModified = TRUE;
					 return;
				 }
			}

		}
		//������ʾ
		//FillMergeDlgTree(m_DesTree,m_DesUserIdx,m_DesConfig,m_hDesTreeItemMap);	

		UpdateDrawTree();

		//�������ϡ������������ͼ�ꡣ
		if (m_RestLayerInfo.GetSize()>0)
		{
			m_UpDifs.EnableWindow(FALSE);
			m_DownDifs.EnableWindow(TRUE);
		}
		else if(m_RestLayerInfo.GetSize()<=0)
		{
			m_UpDifs.EnableWindow(FALSE);
			m_DownDifs.EnableWindow(FALSE);
		}
		//��λ�����һ����ͬ�ĵط���

		int nCount = m_SrcSelectedItemsText.GetSize();
		CString strDifLastNodeText = "";
		if (nCount<=0)
		{
			AfxMessageBox(IDS_ERR_SCHEME_NOSELECTITEMS,MB_OK);
			return;
		}
		else
		{
			strDifLastNodeText = m_SrcSelectedItemsText.GetAt(0);
		}

		m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_SELECTED,TVIS_SELECTED,-2,
											strDifLastNodeText,NULL,FALSE);
		m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_SELECTED,TVIS_SELECTED,-2,
											strDifLastNodeText,NULL,FALSE);
	    
		m_CurLayerIndex = 0;//�����״ΰ����������ʱ��Ч��״̬��

		//��λ���պϲ��Ľڵ�
		m_bUnMerged = TRUE;
		m_bFirstPrompt = TRUE;//�ָ�������ʾ�ĳ�ֵ,�����Զ��ϲ����´��ֶ��ϲ�ʱʹ�á�
		m_bFirstPromptInSymbolLib = TRUE; //�ָ�ͼԪ���ǵ���ʾ�ĳ�ֵ,�����Զ��ϲ����´��ֶ��ϲ���
		m_firstDesSameGroupIndex = -2;
		m_bModified = TRUE;
		UpdateData(TRUE);
		m_HandleMerge.EnableWindow(FALSE);
		UpdateData(FALSE);

	//}

	
}

void CSchemeMerge::OnKillfocusTreeSrcscheme2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CSchemeMerge::OnSetfocusTreeSrcscheme2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_HandleMerge.EnableWindow(m_bIsSrcTreeLoaded);
	m_Del.EnableWindow(FALSE);
	UpdateData(FALSE);
	*pResult = 0;
}
void CSchemeMerge::DeleteRestLayerInfo(CArray<LayerInfo,LayerInfo>& restLayerInfo, __int64 iLayerCode,CString strLayerName,BOOL bByCode)
{
	if (restLayerInfo.GetSize()<=0)
	{
		return;
	}
	for (int i=0; i<restLayerInfo.GetSize();i++)
	{
		if (bByCode==FALSE)
		{
			if (strLayerName==restLayerInfo.GetAt(i).strLayer)
			{
				restLayerInfo.RemoveAt(i);
			}
		}
		else
		{
			if (iLayerCode==restLayerInfo.GetAt(i).iLayerCode)
			{
				restLayerInfo.RemoveAt(i);
			}
		}

	}
	return;
}
BOOL CSchemeMerge::MergeConfig( ConfigLibItem& DesConfig, ConfigLibItem SrcConfig )
{
	//��������·����ȫ��ͬʱ,���ϲ�
	if (DesConfig.GetPath()==SrcConfig.GetPath())
	{
		AfxMessageBox(IDS_ERR_SAMESHEME,MB_OK);
		return FALSE;
	}

	int iRtn = 0;
	//�ϲ��㡣

	m_JustAppendedLineTypeNames.RemoveAll();
	m_JustAppendedCellNames.RemoveAll();
	m_SameCellNames.RemoveAll();
	m_SameLineTypeNames.RemoveAll();
	
	//{hcw,2012.7.5	
	m_SelectedGroupsNonExistInDes.RemoveAll();
	m_DesGroupStateMap.clear();
	m_SrcGroupStateMap.clear(); 
	GetGroupStateMap(m_DesTree,m_DesGroupStateMap,TVIS_EXPANDED|TVIS_BOLD);
	GetGroupStateMap(m_SrcTree,m_SrcGroupStateMap,TVIS_EXPANDED|TVIS_BOLD);
	//}

	m_SrcSelectedItemsText.RemoveAll();

	m_RestLayerInfo.RemoveAll(); //Note��ÿ�κϲ�֮ǰ����ղ�ͬ�
	//�Ƕ�����ĺϲ���
	for(int iSrc=0; iSrc<SrcConfig.pScheme->GetLayerDefineCount();iSrc++)
	{
		//memset(pChrLayerName,NULL,256);
		CSchemeLayerDefine*pSrcLayerDefine = SrcConfig.pScheme->GetLayerDefine(iSrc);
		if(!pSrcLayerDefine)
		{
			continue;
		}
		CString strSrcGroupName = pSrcLayerDefine->GetGroupName();
		__int64 iSrcLayerCode = pSrcLayerDefine->GetLayerCode();
		CString strSrcLayerName = pSrcLayerDefine->GetLayerName();
		
		LayerInfo layerInfo;
		layerInfo.iLayerCode = iSrcLayerCode;
		layerInfo.strGroup = strSrcGroupName;
		layerInfo.strLayer = strSrcLayerName;

		//{hcw,2012.5.23
		CSchemeLayerDefine*pTempSrcLayerDefine = new CSchemeLayerDefine();
		pTempSrcLayerDefine->Copy(*pSrcLayerDefine);
		//}
		int iRsltFindinArray =  FindinArray(strSrcGroupName,DesConfig.pScheme->m_strSortedGroupName);
		int k=FindinArray(strSrcGroupName,SrcConfig.pScheme->m_strSortedGroupName);
		int iDes = DesConfig.pScheme->GetLayerDefineIndex(strSrcLayerName);
		
		//{����Դ�����еĲ����ȡĿ�귽���е���Ӧ�Ĳ�,�Ͻ����ǣ�����ѭ����
		CSchemeLayerDefine *pDesLayerDefine = DesConfig.pScheme->GetLayerDefine(NULL,TRUE,iSrcLayerCode,FALSE);
		if(pDesLayerDefine==NULL)
		{
			//˵��δ�ҵ���ͬ�Ĳ���,ֱ����ӡ��������£�

			//���������ڣ��ͽ�����Ӧ�����,����ӵ�Ŀ�귽������Ӧ��λ�á�
			if ((iRsltFindinArray==-1)&&(k>=0))
			{
				if (k>DesConfig.pScheme->m_strSortedGroupName.GetSize()-1)
				{
					DesConfig.pScheme->m_strSortedGroupName.Add(strSrcGroupName);
					m_SelectedGroupsNonExistInDes.Add(strSrcGroupName);//hcw,2012.7.5,keep original state of tree(KOST)
				}
				else
				{
					DesConfig.pScheme->m_strSortedGroupName.InsertAt(k, strSrcGroupName);
					m_SelectedGroupsNonExistInDes.Add(strSrcGroupName);//hcw,2012.7.5,KOST.
				}
			}
			//���m_RestLayerInfo��
			m_RestLayerInfo.Add(layerInfo);
			DesConfig.pScheme->AddLayerDefine(pTempSrcLayerDefine); //pSrcLayerDefine->pTempSrcLayerDefine
			//action.xml&symbol.bin�ĺϲ�
			MergeActionandSymbols(DesConfig.pPlaceCfgLib,DesConfig.pCellDefLib,DesConfig.pLinetypeLib,pTempSrcLayerDefine);		
		}
		else
		{
			//˵��������ͬ,�������Ƿ񸲸ǡ�
			if (m_bFirstPrompt)
			{
				iRtn = AfxMessageBox(IDS_TIP_COVER_SAMELAYERNAME,MB_YESNOCANCEL);
				m_bFirstPrompt = FALSE;
			}
			
			if (iRtn==IDYES)
			{
				CString strGroupName=""; 
				while(pDesLayerDefine)
				{
				   __int64 iLayerCode = pDesLayerDefine->GetLayerCode();
				   strGroupName = pDesLayerDefine->GetGroupName();
				   iDes = DesConfig.pScheme->GetLayerDefineIndex(pDesLayerDefine->GetLayerName(),FALSE);
				   DesConfig.pScheme->DelLayerDefine(iDes,TRUE,FALSE);//TRUE
				   //ɾ��m_RestLayerInfo�������Ӧ��Ϣ��
				   DeleteRestLayerInfo(m_RestLayerInfo,iLayerCode,""); //pDesLayerDefine->GetLayerCode()->iLayerCode;
					
				   pDesLayerDefine = DesConfig.pScheme->GetLayerDefine("",TRUE,iLayerCode,FALSE); //pDesLayerDefine->GetLayerCode()��iLayerCode;
				}
				//hcw,2012.7.19,����ӵ�Ŀ�귽���е�Դ�㷽���е�������ΪĿ�귽���е�������
				if (strGroupName!="")
				{
					pTempSrcLayerDefine->SetGroupName(strGroupName);
					layerInfo.strGroup = strGroupName;
				}
				
				//���Դ�����е���Ӧ�㡣pTempSrcLayerDefine->pSrcLayerDefine
				DesConfig.pScheme->InsertLayerDefineAt(iDes,pTempSrcLayerDefine,FALSE);
				m_RestLayerInfo.Add(layerInfo);
				//action.xml&symbol.bin�ĺϲ�
				MergeActionandSymbols(DesConfig.pPlaceCfgLib,DesConfig.pCellDefLib,DesConfig.pLinetypeLib,pTempSrcLayerDefine);
			}
			else if(iRtn == IDCANCEL)
			{
				//m_RestLayerInfo.RemoveAll();
				DesConfig.pScheme->ReadFrom(m_BackupCurDesSchemeXML);
				m_bFirstPrompt = TRUE;
				m_bModified = FALSE;
				return FALSE;
			}
		
		}
		
		//}
	}
	//m_bFirstPrompt = TRUE;
	m_RestSpecialLayerInfo.RemoveAll();
	//������(Special)�ĺϲ�,����һ��forѭ��
	for (iSrc = 0; iSrc < SrcConfig.pScheme->GetLayerDefineCount(TRUE); iSrc++)
	{
		CSchemeLayerDefine *pSrcSpecialLayerDefine = SrcConfig.pScheme->GetLayerDefine(iSrc,TRUE);
		if(!pSrcSpecialLayerDefine)
		{
			continue;
		}
		
		__int64 iSrcSpecialLayerCode = pSrcSpecialLayerDefine->GetLayerCode(); //invalid value
		CString strSrcSpecialLayerName = pSrcSpecialLayerDefine->GetLayerName();
		CString strSrcSpecialGroupName = pSrcSpecialLayerDefine->GetGroupName();
		
		LayerInfo specialLayerInfo;
		specialLayerInfo.iLayerCode = iSrcSpecialLayerCode;
		specialLayerInfo.strLayer = strSrcSpecialLayerName;
		specialLayerInfo.strGroup = strSrcSpecialGroupName;
		
		int iPosFindinSrcSpecialArray = FindinArray(strSrcSpecialGroupName,SrcConfig.pScheme->m_strSortedGroupName);
		int iPosFindinDesSpecialArray = FindinArray(strSrcSpecialGroupName,DesConfig.pScheme->m_strSortedGroupName);
		int iIndexinDesScheme = 0;
		
		CSchemeLayerDefine *pTempSpecialSrcLayerDefine = new CSchemeLayerDefine;
		pTempSpecialSrcLayerDefine->Copy(*pSrcSpecialLayerDefine);
		
		CSchemeLayerDefine *pDesSpecialLayerDefine = DesConfig.pScheme->GetLayerDefine(strSrcSpecialLayerName,FALSE,0,TRUE);
		if(!pDesSpecialLayerDefine)
		{
			//δ�ҵ���ͬ�Ĳ㣬ֱ����ӡ�
			//�������������ھͽ�����Ӧ�����,����ӵ���������ĩβ��
			if ((iPosFindinSrcSpecialArray>=0)
				&&(iPosFindinDesSpecialArray==-1))
			{
				DesConfig.pScheme->m_strSortedGroupName.Add(strSrcSpecialGroupName);
				m_SelectedGroupsNonExistInDes.Add(strSrcSpecialGroupName);//hcw,2012.7.5,KOST
			}
			//���m_RestLayerInfo��
			m_RestLayerInfo.Add(specialLayerInfo);
			m_RestSpecialLayerInfo.Add(specialLayerInfo);//hcw,2012.7.4,�洢��ʱ��ӵĶ��������
			DesConfig.pScheme->AddLayerDefine(pTempSpecialSrcLayerDefine,TRUE);
			//action.xml&symbol.bin�ĺϲ�
			MergeActionandSymbols(DesConfig.pPlaceCfgLib,DesConfig.pCellDefLib,DesConfig.pLinetypeLib,pTempSpecialSrcLayerDefine);
		}
		else
		{
			//˵��������Ĳ�����ͬ��
			if (m_bFirstPrompt)
			{
				iRtn = MessageBox("�в���������ظ�(����),�Ƿ���и���?","��ʾ",MB_YESNOCANCEL);
				m_bFirstPrompt = FALSE;
			}
			if (iRtn==IDYES)
			{
				int iIndexFlag = 0;
				CString strSpecialGroupName="";
				while(iIndexFlag>=0)
				{
					CString strLayerName = pDesSpecialLayerDefine->GetLayerName();  //for test.
					strSpecialGroupName = pDesSpecialLayerDefine->GetGroupName();
					iIndexFlag = DesConfig.pScheme->GetLayerDefineIndex(pDesSpecialLayerDefine->GetLayerName(),TRUE);
					int ibRtn = FALSE;
					if (iIndexFlag>=0)
					{
						ibRtn =	DesConfig.pScheme->DelLayerDefine(iIndexFlag,TRUE,TRUE);//TRUE
						//ɾ��m_RestLayerInfo�������Ӧ��Ϣ��
						DeleteRestLayerInfo(m_RestLayerInfo,0,strLayerName,FALSE);
						iIndexinDesScheme = iIndexFlag;
					}
					
					iIndexFlag = DesConfig.pScheme->GetLayerDefineIndex(strLayerName,TRUE);
					//pDesSpecialLayerDefine = DesConfig.pScheme->GetLayerDefine(strSrcSpecialLayerName,FALSE,0,TRUE);
				}
				//hcw,2012.7.19,����ӵ�Ŀ�귽���еĲ���������и��ġ�
				if (strSpecialGroupName!="")
				{
					pTempSpecialSrcLayerDefine->SetGroupName(strSpecialGroupName); 
					specialLayerInfo.strGroup = strSpecialGroupName; //hcw,2012.7.19
				}
				//���Դ�����е���Ӧ�㡣pTempSrcLayerDefine->pSrcLayerDefine
				DesConfig.pScheme->InsertLayerDefineAt(iIndexinDesScheme,pTempSpecialSrcLayerDefine,TRUE);
				m_RestLayerInfo.Add(specialLayerInfo);
				//action.xml&symbol.bin�ĺϲ�
				MergeActionandSymbols(DesConfig.pPlaceCfgLib,DesConfig.pCellDefLib,DesConfig.pLinetypeLib,pTempSpecialSrcLayerDefine);
			}
			else if(iRtn==IDCANCEL)
			{
				DesConfig.pScheme->ReadFrom(m_BackupCurDesSchemeXML);
				m_bFirstPrompt = FALSE;
				m_bModified = FALSE;
				return FALSE;
			}
		}

	}
	
	AddGroupStateMap(m_DesGroupStateMap,m_SelectedGroupsNonExistInDes,0);	
	m_bModified = TRUE;
	m_bFirstPrompt = TRUE; //hcw,2012.6.7,�����ֶ��ϲ�ʱ��������ʾ��
	//int nCount = m_RestLayerInfo.GetSize();
	int nCount = DesConfig.pCellDefLib->GetCellDefCount();
	return TRUE;
}
//hcw,for Reserved. 
BOOL CSchemeMerge::hasDetailedDiffs( CSchemeLayerDefine *pDesLayerDefine,CSchemeLayerDefine *pSrcLayerDefine )
{
	//�ڲ���Ͳ�����ͬ��ǰ���¡�
	if ((pDesLayerDefine->GetLayerName()==pSrcLayerDefine->GetLayerName())
		&&(pDesLayerDefine->GetLayerCode()==pSrcLayerDefine->GetLayerCode()))
	{
		CStringArray desSupportClsNameArray,srcSupportClsNameArray;
		pDesLayerDefine->GetSupportClsName(desSupportClsNameArray);
		pSrcLayerDefine->GetSupportClsName(srcSupportClsNameArray);
		if ((pDesLayerDefine->GetAccel()==pSrcLayerDefine->GetAccel())
			&&(pDesLayerDefine->GetGeoClass()==pSrcLayerDefine->GetGeoClass())
			&&(pDesLayerDefine->GetDbGeoClass()==pSrcLayerDefine->GetDbGeoClass())
			&&(pDesLayerDefine->GetColor()==pSrcLayerDefine->GetColor())
			//&&(StrArrayCmp(desSupportClsNameArray,srcSupportClsNameArray)==0)
			)
		{
			//for reserved
		}
	}
	return FALSE;
}

void CSchemeMerge::OnMoveLayer()
{
	if (m_bSpecialLayer)  return;
	
	//m_hCurItem = m_hSelectedItem; 
	CString strNameAndCode = m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]);
	char layName[_MAX_FNAME];
	int num;
	
	sscanf(strNameAndCode,"%i %s",&num,layName);
	m_iSrcLayerCode  = num;
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}

	//��Ŀ�귽����Ѱ����Ӧ�Ĳ���,������ʾ�������Ƿ�ɾ����
	int nUserIdx = m_DesUserIdx.m_aIdx.GetSize();
	m_bHasSameLayerCode = FALSE;
	for (int j=0; j<m_DesUserIdx.m_aIdx.GetSize();j++)
	{
		if (m_DesUserIdx.m_aIdx[j].code==num)
		{
			m_bHasSameLayerCode = TRUE;
			
			if (IDYES==MessageBox("Ŀ�귽���д�����ͬ�Ĳ��룬�Ƿ���Ӳ�ɾ����ͬ�Ĳ���?","��ʾ",MB_YESNO))
			{
				//ɾ����ͬ�Ĳ���,����ӵ�Ŀ�귽����ȥ��
				m_bUnMerged = FALSE;//ɾ������ı�ǡ�
				break;
			}
			else
			{
				//����ԭ���������κ��޸ķ����Ĳ�����
				m_bUnMerged = TRUE;				
			}
		}
	}

	//���ݲ������жϡ�
	for(int i=0;i<m_SrcUserIdx.m_aIdx.GetSize();i++)
	{
		if (m_SrcUserIdx.m_aIdx[i].code==num)
		{
			m_nMovedLayer = i;
			break;
		}
	}	
	if (m_bUnMerged)
	{
		return;
	}
	m_hMovedItem = m_hSelectedTreeItemArray[DES-1];
	
}

void CSchemeMerge::OnCopyToLayer()
{
	if (m_nMovedLayer < 0 || m_bSpecialLayer||(m_bHasSameLayerCode&&m_bUnMerged))  
	{	
		m_bHasSameLayerCode = FALSE;
		
		return; 
	}
	//hcw,2012.5.25,���m_bUnMerged���ж�
	CString strNameAndCode = m_DesTree.GetItemText(m_hSelectedTreeItemArray[DES-1]);
	char layName[_MAX_FNAME];
	int num;
	sscanf(strNameAndCode,"%i %s",&num,layName);
	m_iDesLayerCode = num;
	CString strAccel;		
	char *pdest = strstr(layName,"^");
	if (pdest != NULL)
	{
		strAccel = pdest+1;
		pdest[0] = 0;
	}
	//copy to another Scheme
	int nAfterLayer = -1;
	long groupidx   = -1;

	//{for Test,2012.5.30
 	int nCount= m_DesUserIdx.m_aIdx.GetSize();

	nCount = m_DesUserIdx.m_aIdx.GetSize();

	//����Դ������ѡ�нڵ������Ѱ��Ŀ�귽��UserIdx����Ӧ�Ĳ�,������Ӧ�ļ�¼��
	for(int i=0;i<m_DesUserIdx.m_aIdx.GetSize();i++)
	{

		int tmpLayerCode = m_DesUserIdx.m_aIdx[i].code;
		if (m_DesUserIdx.m_aIdx[i].code==m_iSrcLayerCode) //num->m_iSrcLayerCode;
		{
			//����OnMoveLayer�еı������Ӧ��ɾ�������m_bUnMerged = FALSE,��ɾ����Ӧ�Ĳ�
			if (!m_bUnMerged) //May be Unnecessary ? 
			{
				m_nDesRemovedLayer = i;				
			}
		}
	}
	nCount= m_DesUserIdx.m_aIdx.GetSize();

	//����Ŀ�귽����ѡ�нڵ������Ѱ��Ŀ�귽��UserIdx����Ӧ�Ĳ�,����ȷ������ڵ��λ�á�
	for(i=0;i<m_DesUserIdx.m_aIdx.GetSize();i++) //���Ժ���һ��ѭ������һ��
	{
		int tmpLayerCode = m_DesUserIdx.m_aIdx[i].code;
		if (m_DesUserIdx.m_aIdx[i].code==m_iDesLayerCode) //num->m_iSrcLayerCode;
		{
			nAfterLayer = i;
			groupidx = m_DesUserIdx.m_aIdx[i].groupidx;
		}
	}
	int nCountAfter = m_DesConfig.pScheme->GetLayerDefineCount();

	// �ƶ�������
	int nNewGroup = -1;
	
	if (nAfterLayer >= 0)
	{
		// ��˳�����
		IDX idx = m_SrcUserIdx.m_aIdx.GetAt(m_nMovedLayer);
		CSchemeLayerDefine *pLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(idx.FeatureName);//��������������ô��
		CSchemeLayerDefine *pTmpLayerDefine = new CSchemeLayerDefine();
		
		BOOL bChangeGrouped = FALSE;
		if (idx.groupidx != groupidx)
		{
			idx.groupidx = groupidx;
			pLayerDefine->SetGroupName(m_SrcUserIdx.m_aGroup[groupidx].GroupName);
		}
		pTmpLayerDefine->Copy(*pLayerDefine); //hcw,2012.5.25,memcpy->copy

		
		m_DesUserIdx.m_aIdx.InsertAt(nAfterLayer+1,idx); 
		m_DesConfig.pScheme->InsertLayerDefineAt(nAfterLayer+1,pTmpLayerDefine);
		
		int nCountAfter2 = m_DesConfig.pScheme->GetLayerDefineCount(FALSE);
		//ɾ��Ŀ�귽��m_DesConfig�б����ǵĲ���Ϣ,ֱ�ӽ�Ŀ����о�����ͬ����Ĳ�ɾ����
	  		
		int index = nAfterLayer+1;
		if (!m_bUnMerged)
		{
			if (nAfterLayer >= m_nDesRemovedLayer)
			{			
				m_DesUserIdx.m_aIdx.RemoveAt(m_nDesRemovedLayer);
				m_DesConfig.pScheme->DelLayerDefine(m_nDesRemovedLayer,FALSE);
				--index;
			}
			else
			{
				m_DesUserIdx.m_aIdx.RemoveAt(m_nDesRemovedLayer+1);
				m_DesConfig.pScheme->DelLayerDefine(m_nDesRemovedLayer+1,FALSE);
			}
		}
		
		// ������ʾ
		char strR[_MAX_FNAME];
		if (strlen(idx.strAccel) <= 0)
		{
			sprintf(strR,"%I64d %s",idx.code,idx.FeatureName);
		}
		else
		{
			sprintf(strR,"%I64d %s^%s",idx.code,idx.FeatureName,idx.strAccel);
		}
		

		HTREEITEM hParent = m_DesTree.GetParentItem(m_hSelectedTreeItemArray[DES-1]);
		HTREEITEM item = m_DesTree.InsertSubItem(_T(strR),hParent,0,CODEID_FROMIDX(index),m_hSelectedTreeItemArray[DES-1]);
		
		//ɾ����ͬ��������нڵ㡣
		if ((!m_bUnMerged)&&m_bHasSameLayerCode)
		{
			CTreeItemList treeItemList;
			GetHtreeItemFromMap(idx.code,m_hDesTreeItemMap,treeItemList);//���ݲ����HTREEITEM֮���ӳ�����ҵ���֮��Ӧ��HTREEITEM��
			int nCountTreeItem = treeItemList.GetCount();
			if (treeItemList.GetCount()>0)
			{
				DeleteItemfromTree(m_DesTree,treeItemList); //ɾ����ʾ
				DeleteItemfromMap(idx.code,m_hDesTreeItemMap);//ɾ��ӳ��
			}

			//m_DesTree.DeleteItem(m_hMovedItem);
			m_bUnMerged = TRUE;
		}
		

		m_hDesTreeItemMap.insert(make_pair(idx.code,item));
		
		m_DesTree.SelectItem(item);
		m_nMovedLayer = -1;
		m_bModified = TRUE;
		m_bUnMerged = TRUE;//hcw,2012.5.28
	}
	
}

void CSchemeMerge::OnMoveGroup()
{
	CString groupName = m_SrcTree.GetItemText(m_hSelectedTreeItemArray[SRC-1]);
	m_SrcSelectedGroupName = groupName ;
	for (int i=0; i<m_DesUserIdx.m_aGroup.GetSize(); i++)
	{
		if (stricmp(m_DesUserIdx.m_aGroup[i].GroupName,groupName)==0)
		{
		    
			m_nDesRemovedGroup = i;
			if (IDYES==AfxMessageBox(IDS_TIP_COVER_SAMELAYERNAME,MB_YESNO))
			{
				m_bUnMerged = FALSE;
				break;
			}
			else
			{
				m_bUnMerged = TRUE;
			}
					
		}
	}
	
	//Ѱ��Դ�����е�ѡ�в�
	for (i=0; i<m_SrcUserIdx.m_aGroup.GetSize();i++)
	{
		if (stricmp(m_DesUserIdx.m_aGroup[i].GroupName,groupName)==0)
		{
			m_nMovedGroup = i;
		}

	}
	
	if (m_bUnMerged = TRUE)
	{
		return;
	}
	
		
}

void CSchemeMerge::OnCopyToGroup()
{
	if ((m_nMovedGroup < 0)||m_bUnMerged) return;

	CString DesGroupName = m_DesTree.GetItemText(m_hSelectedTreeItemArray[DES-1]);
	//��Ŀ�귽����Ѱ��Ҫ�����λ�ú͸��Ǻ�Ҫɾ����λ�á�
	int nDesAfterGroup = -1;
	for (int i=0; i<m_DesUserIdx.m_aGroup.GetSize(); i++)
	{
		if (m_DesUserIdx.m_aGroup[i].GroupName == DesGroupName)
		{
			nDesAfterGroup = i;
			
		}
		if (m_DesUserIdx.m_aGroup[i].GroupName == m_SrcSelectedGroupName)
		{
			m_nDesRemovedGroup = i;
		}
	}
	
	//
	//�ϲ�������
	int nMovedGroup = -1;
	int nGroupInsertPos = -1;
	if(nDesAfterGroup>=0)
	{
		//�޸����еĲ���š�����m_nDesRemovedGroup(Ҫȥ���Ĳ�)��
		//m_nMovedGroup(��Դ�������ƶ���)����nDesAfterGroup(Ҫ�����λ��)�����λ�á�
		for (i=0;i<m_DesUserIdx.m_aIdx.GetSize();i++)
		{
			IDX &idx = m_DesUserIdx.m_aIdx.ElementAt(i);
			
			if(!m_bUnMerged)//������ͬ����,��Ҫɾ����
			{
				if ((idx.groupidx>m_nDesRemovedGroup)&&(idx.groupidx<=nDesAfterGroup))
				{
					idx.groupidx--;
				}
				
			}
			else//��������ͬ����,����ɾ����
			{
			   if (idx.groupidx>nDesAfterGroup)
			   {
				   idx.groupidx++;
			   }
				
			}
			
		}
		if (!m_bUnMerged)
		{
			nGroupInsertPos = nDesAfterGroup;
		}
		else
		{
			nGroupInsertPos= nDesAfterGroup+1;
		}
	}
	
	//����CSchemeLayerDefine;
	CArray<CSchemeLayerDefine*, CSchemeLayerDefine*> layerDefineatSpecificGroup;
	int nfirstLayerIndex = 0;
	if(GetLayerSchemeofGroup(m_SrcConfig.pScheme, m_SrcSelectedGroupName, nfirstLayerIndex, layerDefineatSpecificGroup))
	{
		MergeGrouptoDesScheme(m_DesConfig.pScheme, nfirstLayerIndex,layerDefineatSpecificGroup);
	}
	
	//����
	
	

}

BOOL CSchemeMerge::GetLayerSchemeofGroup(CScheme* pScheme,CString selectedGroupName, int &nfirstLayerIndex,
						   CArray<CSchemeLayerDefine*,CSchemeLayerDefine*>& pLayerDefineSpecificGroup)
{
	nfirstLayerIndex = -1;
	
	for (int i=0; i<pScheme->GetLayerDefineCount(); i++)
	{
		CSchemeLayerDefine* pLayerDefine = pScheme->GetLayerDefine(i,FALSE);
		if (pLayerDefine)
		{
			if (pLayerDefine->GetGroupName()==selectedGroupName)
			{
				if (nfirstLayerIndex==-1)
				{
					nfirstLayerIndex = pScheme->GetLayerDefineIndex(pLayerDefine->GetLayerName());
				}
				CSchemeLayerDefine*pTmpLayerDefine = new CSchemeLayerDefine();
				pTmpLayerDefine->Copy(*pLayerDefine);
				pLayerDefineSpecificGroup.Add(pTmpLayerDefine);
			}
			
		}
	}
	if (nfirstLayerIndex==-1)
	{
		return FALSE;
	}
	else
		return TRUE;
}

void CSchemeMerge::MergeGrouptoDesScheme(CScheme* pScheme, int nFirstLayerIndex,CArray<CSchemeLayerDefine*,CSchemeLayerDefine*>& pLayerDefineArray)
{
	if(pLayerDefineArray.GetSize()<=0)
	{
		return;
	}
	int nInsertPos=0;
	nInsertPos = nFirstLayerIndex;
	for (int i=0; i<pLayerDefineArray.GetSize();i++)
	{
		pScheme->InsertLayerDefineAt(nInsertPos,pLayerDefineArray.GetAt(i));
		nInsertPos++;
	}
	return;
}


int CSchemeMerge::GetHtreeItemFromMap(int idx,multimap<int,HTREEITEM> hTreeItemMap,CTreeItemList& treeItemList )
{
    multimap<int, HTREEITEM>::const_iterator mapIter;
	 
	pair<multimap<int, HTREEITEM>::const_iterator,multimap<int, HTREEITEM>::const_iterator> range;
	range=hTreeItemMap.equal_range(idx);

	for (mapIter=range.first; mapIter!=range.second; mapIter++)
	{
		treeItemList.AddTail(mapIter->second);
	}
	int nCount=treeItemList.GetCount();
	
	return nCount ;
}

BOOL CSchemeMerge::DeleteItemfromTree( CMergeTree& treeCtrl, CTreeItemList& treeItemList )
{
	if (treeItemList.GetCount()<=0)
	{
		return FALSE;
	}
	POSITION pos;
	for (pos=treeItemList.GetHeadPosition(); pos!=NULL;)
	{
		HTREEITEM hItem = (HTREEITEM)treeItemList.GetNext(pos);
		treeCtrl.DeleteItem(hItem);
	}
	return TRUE;
}

BOOL CSchemeMerge::DeleteItemfromMap( int iCode, multimap<int,HTREEITEM>& hTreeItemMap )
{
	multimap<int,HTREEITEM>::iterator iter = hTreeItemMap.find(iCode) ;
	while(iter!=hTreeItemMap.end())
	{
		hTreeItemMap.erase(iter++);
		return TRUE;
	}
	return FALSE;
}

BOOL CSchemeMerge::PreTranslateMessage( MSG* pMsg )
{
	if (m_ToolTip.m_hWnd!=NULL)
	{
		m_ToolTip.RelayEvent(pMsg);
	}

	return CDialog::PreTranslateMessage(pMsg);	
}

void CSchemeMerge::OnMoveSrcLayer(HTREEITEM hSrcItem)
{
	int  nSrcLayerCode;
	char srcStrLayerName[128];
	sscanf(m_SrcTree.GetItemText(hSrcItem),"%i %s",&nSrcLayerCode,srcStrLayerName);
	m_pMovedLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(NULL,TRUE,nSrcLayerCode,FALSE);
	//��������
	if (!m_pMovedLayerDefine)
	{
		CString strItemTxt = "";
		strItemTxt = m_SrcTree.GetItemText(hSrcItem);
		m_pMovedLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(strItemTxt,FALSE,0,TRUE);	
	}

	CString str = m_pMovedLayerDefine->GetLayerName(); //for monitor
	return;
}

void CSchemeMerge::OnCopyToDesLayer( HTREEITEM& hDesItem )
{
	//����Դ�����еĲ�ϲ���Ŀ��Ķ����������£�����ʾ������
	
	CString strDesGroup;
	HTREEITEM hDesParentItem = m_DesTree.GetParentItem(hDesItem);
	if (hDesParentItem)
	{
		 strDesGroup = m_DesTree.GetItemText(hDesParentItem);
	}
	else
		strDesGroup = m_DesTree.GetItemText(hDesItem);
	
	if(!m_pMovedLayerDefine)
		return;

	CString strMovedGroup = m_pMovedLayerDefine->GetGroupName();
	if ((strDesGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&(strMovedGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))!=0))
	{
		//m_bSpecialLayer;
		MessageBox(strDesGroup+"���治�������һ��Ĳ���!������ѡ��!","��ʾ",MB_OK);
		m_bUnMerged = -1;
		//m_bSpecialLayer = TRUE;
		return;
	}
	else if ((strMovedGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&(strDesGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))!=0))
	{
		MessageBox("һ��Ĳ������治�������"+strMovedGroup+"!������ѡ��!","��ʾ",MB_OK);
		m_bUnMerged = -1;
		return;
	}
	else if ((strMovedGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&(strDesGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0))
	{
		m_bSpecialLayer = TRUE;
		//return;
	}
	else
	{
		m_bSpecialLayer = FALSE;
		//return;
	}
	int  nDesLayerCode;
	char strDesLayerName[128];
	sscanf(m_DesTree.GetItemText(hDesItem),"%i %s",&nDesLayerCode,strDesLayerName);

	int iDesIndex = -1;
	int iSameIndex= -1;

    //���ò����ж�
	CString DesSameGroupName = "";
	CSchemeLayerDefine* pDesSameLayerDefine = NULL;
	if (m_bSpecialLayer)
	{
		pDesSameLayerDefine = m_DesConfig.pScheme->GetLayerDefine(m_pMovedLayerDefine->GetLayerName(),TRUE,0,m_bSpecialLayer);
	}
	else
		pDesSameLayerDefine = m_DesConfig.pScheme->GetLayerDefine(NULL,TRUE,m_pMovedLayerDefine->GetLayerCode(),FALSE);
	
		
	if (pDesSameLayerDefine)
	{
		iSameIndex = m_DesConfig.pScheme->GetLayerDefineIndex(pDesSameLayerDefine->GetLayerName(),m_bSpecialLayer); //FALSE->m_bSpecialLayer
		DesSameGroupName = pDesSameLayerDefine->GetGroupName();
	}

	CSchemeLayerDefine* pTmpSchemeLayerDefine = new CSchemeLayerDefine();
	pTmpSchemeLayerDefine->Copy(*m_pMovedLayerDefine);
	
	//��ȡĿ�귽����ѡ�нڵ��������
	HTREEITEM hParentItem = m_DesTree.GetParentItem(hDesItem);
	CString DesGroupName = "";
	CString SrcGroupName = "";
	SrcGroupName = m_pMovedLayerDefine->GetGroupName();
	//�ж�Ŀ�����Ƿ�����ͬ������,��û�оͽ�����Ӧ����ӡ�

	CString SelGroupName = "";
	if (hParentItem)
	{
		SelGroupName = m_DesTree.GetItemText(hParentItem);
	}
	else
	{
		SelGroupName = m_DesTree.GetItemText(hDesItem);
	}
	int iDesSameGroupIndex = -1;
	int iDesSelectedGroupIndex = -1;
	
	iDesSelectedGroupIndex = FindinArray(SelGroupName,m_DesConfig.pScheme->m_strSortedGroupName);
	iDesSameGroupIndex = FindinArray(m_pMovedLayerDefine->GetGroupName(),m_DesConfig.pScheme->m_strSortedGroupName);

    if (!hParentItem)
    {
		DesGroupName = m_DesTree.GetItemText(hDesItem);
    }
	else
	{
		DesGroupName = m_DesTree.GetItemText(hParentItem);
		//��Ŀ��ѡ���ǲ㣬���ȡ��������
		int nSelectedLayerCode;
		char strSelectedLayerName[128];
		CString strDesItemTxt = m_DesTree.GetItemText(hDesItem);
		sscanf(strDesItemTxt,"%i %s",&nSelectedLayerCode,strSelectedLayerName);
		CSchemeLayerDefine *pDesSelectedLayerDefine = NULL;
		if (!m_bSpecialLayer)
		{
			pDesSelectedLayerDefine = m_DesConfig.pScheme->GetLayerDefine(NULL,TRUE,nSelectedLayerCode,m_bSpecialLayer);
		}
		else
		{
			pDesSelectedLayerDefine = m_DesConfig.pScheme->GetLayerDefine(strDesItemTxt,FALSE,0,m_bSpecialLayer);
		}

		if (pDesSelectedLayerDefine)
		{
			iDesIndex = m_DesConfig.pScheme->GetLayerDefineIndex(pDesSelectedLayerDefine->GetLayerName(),m_bSpecialLayer);
			//�жϺ��ʵ�λ��
			if ((iDesIndex!=m_nDesIndex)&&m_bFirstSelectedIndex)
			{
				m_nDesIndex = iDesIndex;
				m_bFirstSelectedIndex = FALSE;
			}
			else
				m_nDesIndex++;
		}
	}

	if((iSameIndex>=0)&&m_bFirstPrompt)
	{
		int iRtn = AfxMessageBox(IDS_TIP_COVER_SAMELAYERNAME,MB_YESNOCANCEL);
		if (IDYES==iRtn)
		{
			m_bUnMerged = FALSE;
		}
		else if(IDNO==iRtn)
		{
			m_bUnMerged = TRUE;
		}
		else
			m_bUnMerged = -1;
		m_bFirstPrompt = FALSE;
	}
	
	//����m_bUnMerged���ж��Ƿ���и��ǡ�
	if ((iSameIndex>=0)&&!m_bUnMerged)
	{
		m_DesConfig.pScheme->DelLayerDefine(iSameIndex,TRUE,m_bSpecialLayer); 
		pTmpSchemeLayerDefine->SetGroupName(DesSameGroupName);
		m_DesConfig.pScheme->InsertLayerDefineAt(iSameIndex,pTmpSchemeLayerDefine,m_bSpecialLayer);//hcw,�հס�m_bSpecialLayer
		AddDifLayerInfo(m_RestLayerInfo,pTmpSchemeLayerDefine);
		//action.xml��symbol.lib
		MergeActionandSymbols(m_DesConfig.pPlaceCfgLib,m_DesConfig.pCellDefLib,m_DesConfig.pLinetypeLib,pTmpSchemeLayerDefine);
		//����Ŀ�귽����ѡ�нڵ㡣
		return;
	}
	else if((iSameIndex>=0)&&m_bUnMerged)
	{
		return;
	}

	if((iSameIndex>=0)&&(m_nDesIndex>iSameIndex)&&(!m_bUnMerged))
	{
		m_nDesIndex--;
	}
	//�������ظ��Ĳ���ʱ
	pTmpSchemeLayerDefine->SetGroupName(DesGroupName);
	if (!hParentItem)
	{		
		//{����ֱ�Ӳ嵽����ײ�
	    int nDesFirstLayer = m_DesConfig.pScheme->GetFirstLayerDefineIndex(DesGroupName,m_bSpecialLayer);
		if ((m_nDesIndex!=nDesFirstLayer)
			&&m_bFirstSelectedIndex)
		{
			m_nDesIndex = nDesFirstLayer;
			m_bFirstSelectedIndex = FALSE;
		}
		else
			m_nDesIndex++;

		if(nDesFirstLayer>=0)
			m_DesConfig.pScheme->InsertLayerDefineAt(m_nDesIndex,pTmpSchemeLayerDefine,m_bSpecialLayer);
		else
			m_DesConfig.pScheme->AddLayerDefine(pTmpSchemeLayerDefine,m_bSpecialLayer);
		//}
		AddDifLayerInfo(m_RestLayerInfo,pTmpSchemeLayerDefine);
		//action.xml��symbol.lib
		MergeActionandSymbols(m_DesConfig.pPlaceCfgLib,m_DesConfig.pCellDefLib,m_DesConfig.pLinetypeLib,pTmpSchemeLayerDefine);
		//����Ŀ�귽����ѡ�нڵ㡣
	}
	else
	{
		
		m_DesConfig.pScheme->InsertLayerDefineAt(m_nDesIndex+1,pTmpSchemeLayerDefine,m_bSpecialLayer); //hcw,2012.7.9,FALSE��m_bSepcialLayer
		//m_nDesIndex++;
		AddDifLayerInfo(m_RestLayerInfo,pTmpSchemeLayerDefine);
		//action.xml��symbol.lib
		MergeActionandSymbols(m_DesConfig.pPlaceCfgLib,m_DesConfig.pCellDefLib,m_DesConfig.pLinetypeLib,pTmpSchemeLayerDefine);
		//����Ŀ�귽����ѡ�нڵ㡣
	}
	
	return;
}
void CSchemeMerge::MergeActionandSymbols(CPlaceConfigLib* pPlaceCfgLib,CCellDefLib *pCellDefLib,
										 CBaseLineTypeLib *pLinetypeLib, CSchemeLayerDefine*pSchemeLayerDefine)
{
	if (!pSchemeLayerDefine)
	{
		return;
	}

	CString strLayerName = pSchemeLayerDefine->GetLayerName();
	CArray<CSymbol*,CSymbol*> srcLayerArrPCfgs ;
	pSchemeLayerDefine->GetSymbols(srcLayerArrPCfgs);
	//pPlaceCfgLib
	CPlaceConfig* pSrcPlaceConfig = m_SrcConfig.pPlaceCfgLib->GetConfig(strLayerName);
	CPlaceConfig* pDesPlaceConfig = pPlaceCfgLib->GetConfig(strLayerName);
	CPlaceConfig* pTmpSrcPlaceConfig = new CPlaceConfig();
	pTmpSrcPlaceConfig = pSrcPlaceConfig; //2012.6.11,added
	if (pSrcPlaceConfig)
	{
		if (pDesPlaceConfig)
		{
			pDesPlaceConfig = pTmpSrcPlaceConfig;//2012.6.11,pSrcPlaceConfig->pTmpSrcPlaceConfig
		}
		else
		{
			pPlaceCfgLib->AddConfig(*pTmpSrcPlaceConfig);//2012.6.11,pSrcPlaceConfig->pTmpSrcPlaceConfig
		}
	}
	 
	if (srcLayerArrPCfgs.GetSize()<=0)
	{
		return;
	}
	int nCount = srcLayerArrPCfgs.GetSize();
	
	
	for (int iSrc=0; iSrc<srcLayerArrPCfgs.GetSize();iSrc++)
	{
		BOOL bCellDef = FALSE; //��ͼԪ
		BOOL bSameWithCellName = FALSE;
		
		CString name = srcLayerArrPCfgs.GetAt(iSrc)->GetName();
		
		
		CellDef srcCellDef;
		CellDef tmpSrcCellDef;
		BaseLineType srcBaseLineTypeDef;
		srcCellDef.Create();
		tmpSrcCellDef.Create();
		
		if (m_SrcConfig.pCellDefLib->GetCellDefIndex(name)!=-1)
		{
			//pCellDefLib
			bSameWithCellName = TRUE;
			srcCellDef = m_SrcConfig.pCellDefLib->GetCellDef(name);
			//hcw,2012.7.5
			if (tmpSrcCellDef.m_pgr&&srcCellDef.m_pgr)
			{
				tmpSrcCellDef.m_pgr->AddBuffer(srcCellDef.m_pgr);
				Envelope elop = tmpSrcCellDef.m_pgr->GetEnvelope();
				tmpSrcCellDef.m_pgr->SetAllColor(0);
				tmpSrcCellDef.RefreshEnvelope();
				strcpy(tmpSrcCellDef.m_name,srcCellDef.m_name);
			}
		}
		else if (m_SrcConfig.pLinetypeLib->GetBaseLineTypeIndex(name)!=-1)
		{
			//pLinetypeLib
			bSameWithCellName = FALSE;
			srcBaseLineTypeDef = m_SrcConfig.pLinetypeLib->GetBaseLineType(name);
		}
		else
		{			
			continue;
		}
		//pCellDefLib;
		if (bSameWithCellName)
		{
			//����ͼԪ
			if (pCellDefLib->GetCellDefIndex(name)==-1)
			{
				//ֱ�����ͼԪ
				if (FindinArray(name, m_JustAppendedCellNames,FALSE)==-1)
				{
					m_JustAppendedCellNames.Add(name);
					//pCellDefLib->AddCellDef(srcCellDef);
					pCellDefLib->AddCellDef(tmpSrcCellDef); //hcw,2012.7.5,modified
				}

			}
			else if((pLinetypeLib->GetBaseLineTypeIndex(name)==-1)
					&&(FindinArray(name,m_JustAppendedCellNames,FALSE)==-1)
					&&(FindinArray(name,m_JustAppendedLineTypeNames,FALSE)==-1))
			{	
				//{for test 2012.6.28
				int ninLib = pCellDefLib->GetCellDefIndex(name);
				int nIndexCells = FindinArray(name,m_JustAppendedCellNames,FALSE);
				int nIndexLineType = FindinArray(name,m_JustAppendedLineTypeNames,FALSE);
				int nCountCellName = m_JustAppendedCellNames.GetSize();
				m_nSameCellCount++;
				m_SameCellNames.Add(name);
				//}
				//ͼԪ��������
				if(m_bFirstPromptInSymbolLib)
				{
					int iRtn = 0;
					iRtn = AfxMessageBox(IDS_TIP_COVER_SAMECELLDEF,MB_YESNO);
					if (iRtn==IDYES)
					{
						m_bCoveredInSymbolLib = TRUE;
					}
					else
					{
						m_bCoveredInSymbolLib = FALSE;
					}
					m_bFirstPromptInSymbolLib = FALSE;
				}
				if (m_bCoveredInSymbolLib)
				{
					//ֱ�Ӹ���
					pCellDefLib->DelCellDef(name);
					//pCellDefLib->AddCellDef(srcCellDef); 
					pCellDefLib->AddCellDef(tmpSrcCellDef);
				}
				else
				{
					continue;
				}
				
			}
		}
		else
		{
			//��������
			if (pLinetypeLib->GetBaseLineTypeIndex(name)==-1)
			{
				//ֱ���������
				if (FindinArray(name,m_JustAppendedLineTypeNames,FALSE)==-1)
				{
					m_JustAppendedLineTypeNames.Add(name);
					pLinetypeLib->AddBaseLineType(srcBaseLineTypeDef);
				}
				
			}
			else if((pCellDefLib->GetCellDefIndex(name)==-1)
				&&(FindinArray(name,m_JustAppendedLineTypeNames,FALSE)==-1)
				&&(FindinArray(name,m_JustAppendedCellNames,FALSE)==-1))
			{
				int nLineTypeCount = m_JustAppendedLineTypeNames.GetSize();
				m_nSameLineType++;
				m_SameLineTypeNames.Add(name);
				//���͸�������
			    if (m_bFirstPromptInSymbolLib)
			    {
					int iRtn = 0;
					iRtn = AfxMessageBox(IDS_TIP_COVER_SAMECELLDEF,MB_YESNO); //hcw,2012.6.28,���͡�ͼԪ
					if (iRtn==IDYES)
					{
						m_bCoveredInSymbolLib = TRUE;
					}
					else
					{
						m_bCoveredInSymbolLib = FALSE;
					}
					m_bFirstPromptInSymbolLib = FALSE;
			    }
				if (m_bCoveredInSymbolLib)
				{
					//ֱ�Ӹ���
					pLinetypeLib->DelBaseLineType(name);
					pLinetypeLib->AddBaseLineType(srcBaseLineTypeDef);
				}
				else
				{
					continue;
				}
			}
		}

	}
	return;
}
void CSchemeMerge::OnMoveSrcGroup( HTREEITEM hSrcItem )
{
	
	CString strSrcGroupName = m_SrcTree.GetItemText(hSrcItem);
	m_hSrcParentItem = m_SrcTree.GetParentItem(hSrcItem);

	if (strSrcGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
	{
		m_bSpecialLayer = TRUE;
	}
	m_strSrcGroupName = strSrcGroupName;
	m_pMovedLayerDefineArray.RemoveAll();
	for(int i=0; i<m_SrcConfig.pScheme->GetLayerDefineCount(m_bSpecialLayer);i++)
	{
		CSchemeLayerDefine* pSrcLayerDefine = m_SrcConfig.pScheme->GetLayerDefine(i,m_bSpecialLayer);
		if(pSrcLayerDefine)
		{
			if(pSrcLayerDefine->GetGroupName()==strSrcGroupName)
			{
				CSchemeLayerDefine* pTmpSrcLayerDefine = new CSchemeLayerDefine();
				
				pTmpSrcLayerDefine->Copy(*pSrcLayerDefine);
				CString str = pTmpSrcLayerDefine->GetLayerName();
				m_pMovedLayerDefineArray.Add(pTmpSrcLayerDefine);
			}
		}
	}
	int nCount = m_pMovedLayerDefineArray.GetSize();
	return;
}

void CSchemeMerge::OnCopyToDesGroup( HTREEITEM hDesItem )
{
	//ѡ�����λ�á�
	CString strSelectedDesGroupName ;
   
	HTREEITEM hDesParentItem = m_DesTree.GetParentItem(hDesItem);
	int nSelectedItemIndex=-1;
	int nDesSelectedGroupIndex = -1;
	int nDesLayerIndex = -1;
	
	if (hDesParentItem)
	{
		strSelectedDesGroupName = m_DesTree.GetItemText(hDesParentItem);
		nSelectedItemIndex = FindinArray(m_DesTree.GetItemText(hDesParentItem),m_DesConfig.pScheme->m_strSortedGroupName);
		char DesLayerName[128];
		__int64 iLayerCode;
		sscanf(m_DesTree.GetItemText(hDesItem),"%I64d %s",&iLayerCode,DesLayerName);
		nDesLayerIndex = m_DesConfig.pScheme->GetLayerDefineIndex(DesLayerName);
		nDesSelectedGroupIndex = FindinArray(m_DesTree.GetItemText(hDesParentItem),m_DesConfig.pScheme->m_strSortedGroupName);
	}
	else
	{
		strSelectedDesGroupName = m_DesTree.GetItemText(hDesItem);
		nDesSelectedGroupIndex = FindinArray(m_DesTree.GetItemText(hDesItem),m_DesConfig.pScheme->m_strSortedGroupName);
	}
    
	//��������������һ��������໥�ϲ�֮��ĳ�ͻ��
	if ((m_strSrcGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))!=0)
		&&(strSelectedDesGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&m_hSrcParentItem)
	{
		MessageBox(strSelectedDesGroupName+"�����治�������һ�����! ������ѡ��!","��ʾ",MB_OK);
		return;
	}
	else if((m_strSrcGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&(strSelectedDesGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))!=0)
		&&m_hSrcParentItem)
	{
		MessageBox("һ��������治�������"+m_strSrcGroupName+"���µĲ�! ������ѡ��!" ,"��ʾ",MB_OK);
		return;
	}
	else if ((m_strSrcGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&!m_hSrcParentItem)
	{
		m_bSpecialLayer = TRUE;
	}
	else
	{
		m_bSpecialLayer = FALSE;
	}
	//�ж��飬��������Ӧ����ӡ�
	int nSameGroupinDes = FindinArray(m_strSrcGroupName,m_DesConfig.pScheme->m_strSortedGroupName);
	
	if (nSameGroupinDes==-1) //Ŀ�귽����δ������Ӧ���飬ֱ�����ѡ�в�ĺ����ѡ�����ĩβ��
	{
		m_firstDesSameGroupIndex = -1;
		if ((m_nDesSelectedGroupIndex!=nDesSelectedGroupIndex)
			&&m_bFirstSelectedIndex)
		{
			m_nDesSelectedGroupIndex = nDesSelectedGroupIndex;
			m_bFirstSelectedIndex = FALSE;
		}
		else
			m_nDesSelectedGroupIndex++;
		//Ŀ�귽����ѡ�����������ʱ��������ڶ����������ǰ�档
		if (strSelectedDesGroupName.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			m_DesConfig.pScheme->m_strSortedGroupName.InsertAt(m_nDesSelectedGroupIndex,m_strSrcGroupName);
		}
		else
			m_DesConfig.pScheme->m_strSortedGroupName.InsertAt(m_nDesSelectedGroupIndex+1,m_strSrcGroupName);		
		m_SelectedGroupsNonExistInDes.Add(m_strSrcGroupName);
		
	}
	//�жϳ�ͻ��ֱ���жϲ��루�����ж�������
	int nDesSameLayerIndex = -1;
	CString DesGroupNamewithSameLayerCode = "";
	
	for (int i=0; i<m_pMovedLayerDefineArray.GetSize();i++)
	{
		nDesSameLayerIndex = -1;

		CSchemeLayerDefine*pDesSameLayerDefine = NULL;
		if (!m_bSpecialLayer)
		{
			pDesSameLayerDefine = m_DesConfig.pScheme->GetLayerDefine(NULL,TRUE,m_pMovedLayerDefineArray[i]->GetLayerCode(),m_bSpecialLayer);
		}
		else
		{
			pDesSameLayerDefine = m_DesConfig.pScheme->GetLayerDefine(m_pMovedLayerDefineArray[i]->GetLayerName(),FALSE,0,m_bSpecialLayer);
		}

		if (pDesSameLayerDefine)
		{
			nDesSameLayerIndex = m_DesConfig.pScheme->GetLayerDefineIndex(pDesSameLayerDefine->GetLayerName(),m_bSpecialLayer);
			DesGroupNamewithSameLayerCode = pDesSameLayerDefine->GetGroupName();
		}
		//ȷ���Ƿ񸲸�
		if ((nDesSameLayerIndex>=0)
			&&m_bFirstPrompt)
		{
			int iRtn = AfxMessageBox(IDS_TIP_COVER_SAMELAYERNAME,MB_YESNOCANCEL);
			if (IDYES==iRtn)
			{
				m_bUnMerged = FALSE;
			}
			else if(IDNO==iRtn)
			{
				m_bUnMerged = TRUE;
			}
			else //hcw,2012.6.11,added
			{
				m_bUnMerged = -1;
				if (nSameGroupinDes)
				{
					m_DesConfig.pScheme->m_strSortedGroupName.RemoveAt(m_nDesSelectedGroupIndex+1);
				}
				
				return;
			}
			m_bFirstPrompt = FALSE;
		}

		//����ѡ����ȷ���Ƿ񸲸�
		if ((nDesSameLayerIndex>=0)&&(!m_bUnMerged))
		{
			//����
			m_DesConfig.pScheme->DelLayerDefine(nDesSameLayerIndex,TRUE,m_bSpecialLayer);
		    m_pMovedLayerDefineArray[i]->SetGroupName(DesGroupNamewithSameLayerCode);
			m_DesConfig.pScheme->InsertLayerDefineAt(nDesSameLayerIndex,m_pMovedLayerDefineArray[i],m_bSpecialLayer);
			AddDifLayerInfo(m_RestLayerInfo,m_pMovedLayerDefineArray[i]);
			MergeActionandSymbols(m_DesConfig.pPlaceCfgLib,m_DesConfig.pCellDefLib,m_DesConfig.pLinetypeLib,m_pMovedLayerDefineArray[i]);
			//����Ŀ�귽���е�ѡ�нڵ�
			continue;
		}
		else if ((nDesSameLayerIndex>=0)&&m_bUnMerged)
		{
			//������
			
			continue;
		}
		
		//����ѡ�в���顣

		m_pMovedLayerDefineArray[i]->SetGroupName(m_strSrcGroupName);
		//��������ͬ�Ĳ㣬��ֱ����ӡ�
		if (!hDesParentItem)
		{
			m_DesConfig.pScheme->AddLayerDefine(m_pMovedLayerDefineArray[i],m_bSpecialLayer);
			AddDifLayerInfo(m_RestLayerInfo,m_pMovedLayerDefineArray[i]);
			MergeActionandSymbols(m_DesConfig.pPlaceCfgLib,m_DesConfig.pCellDefLib,m_DesConfig.pLinetypeLib,m_pMovedLayerDefineArray[i]);	
			//����Ŀ�귽���е�ѡ�нڵ�
		}
		else
		{
			m_DesConfig.pScheme->InsertLayerDefineAt(nSelectedItemIndex,m_pMovedLayerDefineArray[i],m_bSpecialLayer);
			AddDifLayerInfo(m_RestLayerInfo,m_pMovedLayerDefineArray[i]);
			MergeActionandSymbols(m_DesConfig.pPlaceCfgLib,m_DesConfig.pCellDefLib,m_DesConfig.pLinetypeLib,m_pMovedLayerDefineArray[i]);
			//����Ŀ�귽���е�ѡ�нڵ�
			nSelectedItemIndex ++; 
		}
	}


	//{������ӵ���������CshemeDefine����ɾ��������,��������ӵ�����û�в�ڵ㣬�Ͳ���ӵ�Ŀ�귽���С�	
	CStringArray strArrayNotinSchemeDefine;
	strArrayNotinSchemeDefine.Copy(m_SelectedGroupsNonExistInDes);
	for(int k=0; k<m_DesConfig.pScheme->GetLayerDefineCount(); k++)
	{
		CSchemeLayerDefine* pDesDefine = m_DesConfig.pScheme->GetLayerDefine(k,FALSE);
		if (pDesDefine)
		{
			int iDes = -1;
			if ((iDes=FindinArray(pDesDefine->GetGroupName(),strArrayNotinSchemeDefine))>=0)
			{				
				strArrayNotinSchemeDefine.RemoveAt(iDes);
			}
		
		}
	}
	int nCount=0;
	if ((nCount=strArrayNotinSchemeDefine.GetSize())>0)
	{
		for (int m=0; m<nCount; m++ )
		{
			int nIndex = -1;
			if ((nIndex=FindinArray(strArrayNotinSchemeDefine.GetAt(m),m_DesConfig.pScheme->m_strSortedGroupName))>=0)
			{
				m_DesConfig.pScheme->m_strSortedGroupName.RemoveAt(nIndex);
			}
		}
	}
	//}
	return;
}

void CSchemeMerge::AddDifLayerInfo( CArray<LayerInfo,LayerInfo>& restLayerInfo,CSchemeLayerDefine*pSchemeLayerDefine )
{
	if (!pSchemeLayerDefine)
	{
		return;
	}
	LayerInfo DifLayerInfo;
	DifLayerInfo.iLayerCode = pSchemeLayerDefine->GetLayerCode();
	DifLayerInfo.strGroup = pSchemeLayerDefine->GetGroupName();
	DifLayerInfo.strLayer = pSchemeLayerDefine->GetLayerName();
	m_RestLayerInfo.Add(DifLayerInfo);

	return;

}

void CSchemeMerge::OnSetfocusTreeDescheme2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_HandleMerge.EnableWindow(FALSE);
	m_Del.EnableWindow(TRUE);
	UpdateData(FALSE);
	*pResult = 0;
}

void CSchemeMerge::OnKillfocusTreeDescheme2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CSchemeMerge::SaveCellDefandLinetypeLib(ConfigLibItem& Config)
{
	CString configPath = Config.pCellDefLib->GetPath();
	Config.SaveCellLine(configPath);
	return;
}

void CSchemeMerge::GetGroupStateMap( CMergeTree& treeCtrl, map<CString, UINT>& GroupStateMap, UINT nStateMask)
{
	HTREEITEM hItem = treeCtrl.GetRootItem();
	HTREEITEM hParentItem = treeCtrl.GetParentItem(hItem);
	while(hItem&&!hParentItem)
	{
		CString strGroupName = treeCtrl.GetItemText(hItem);
		UINT iGroupState = treeCtrl.GetItemState(hItem,nStateMask);
		GroupStateMap.insert(make_pair(strGroupName,iGroupState));
		hItem = treeCtrl.GetNextItem(hItem,TVGN_NEXT);
		hParentItem = treeCtrl.GetParentItem(hItem);
	}
	return;
}

void CSchemeMerge::AddGroupStateMap(map<CString,UINT>& GroupStateMap,CStringArray& SelectedGroups, UINT nState )
{
	if (SelectedGroups.GetSize()<=0)
	{
		return;
	}
	int nCount = SelectedGroups.GetSize();
	for(int i=0; i<SelectedGroups.GetSize();i++)
	{
		GroupStateMap.insert(make_pair(SelectedGroups.GetAt(i),nState));
	}
	return;
}

UINT CSchemeMerge::GetNodeState( map<CString, UINT>& NodeStateMap, CString strNodeText )
{
	int nMapCount = NodeStateMap.size();
	if ((nMapCount<=0)||(strNodeText==""))
	{
		return -1;
	}
	map<CString, UINT>::const_iterator iterMap = NodeStateMap.find(strNodeText) ;
	if (iterMap!=NodeStateMap.end())
	{
		//�ҵ���Ӧ��ӳ��
		return iterMap->second;
	}
	return -1;
}
void CSchemeMerge::SetUpDownButtonState(int curLayerIndex)
{
	if (curLayerIndex<=0)
	{
		m_UpDifs.EnableWindow(FALSE);
	}
	else
	{
		m_UpDifs.EnableWindow(TRUE);
	}
	
	if (curLayerIndex>=m_RestLayerInfo.GetSize()-1)
	{
		m_DownDifs.EnableWindow(FALSE);
	}
	else
	{
		m_DownDifs.EnableWindow(TRUE);
	}
	return;
}
void CSchemeMerge::GetSelectedItemsText(CMergeTree& treeCtrl,CTreeItemList&curSelectedTreeNodeList,CStringArray&selectedItemsText)
{
	if (curSelectedTreeNodeList.GetCount()<=0)
	{
		return ;
	}
	selectedItemsText.RemoveAll();
	POSITION pos;
	int nCount = curSelectedTreeNodeList.GetCount();
	CString strLayerText;
	for (pos = curSelectedTreeNodeList.GetHeadPosition(); pos!=NULL;)
	{
		HTREEITEM hItem = (HTREEITEM)curSelectedTreeNodeList.GetNext(pos);
		strLayerText = treeCtrl.GetItemText(hItem);
		selectedItemsText.Add(treeCtrl.GetItemText(hItem));
	}
	return ;
}



BOOL CSchemeMerge::isValidDelItem( CMergeTree &treeCtrl, HTREEITEM hItem)
{
	if ((treeCtrl.GetItemText(hItem).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		&&(!treeCtrl.GetParentItem(hItem)))
	{
		return FALSE;
	}
	return TRUE;
	
}




void CSchemeMerge::OnButtonUp() 
{
	// TODO: Add your control notification handler code here

	int nCount = m_RestLayerInfo.GetSize();
	//����

	if (nCount<=0)
	{
		return;
	}		
	m_CurLayerIndex--;
	if (m_CurLayerIndex<m_RestLayerInfo.GetSize())
	{
		m_DownDifs.EnableWindow(TRUE);
	}
	if (m_CurLayerIndex > 0)
	{
		if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
		}
		else
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
		}
		
		
	}
	else
	{
		m_CurLayerIndex = 0;
		if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
		}
		else
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
		}
		//�����ϵİ�ť�ҵ���
		m_UpDifs.EnableWindow(FALSE);
	}
}

void CSchemeMerge::OnButtonDown() 
{
	// TODO: Add your control notification handler code here
	int nCount = m_RestLayerInfo.GetSize();
	if (nCount<=0)
	{
		return;
	}
	if (!m_DesTree.GetParentItem(m_hSelectedTreeItemArray[DES-1])
		&&(m_CurLayerIndex>-1)) //�����ǰѡ�е�����ڵ㣬����Ĭ��ѡ����Ǹ����µĵ�һ���㡣
	{
		m_CurLayerIndex--;
	}
	m_CurLayerIndex++;
    
	if (m_CurLayerIndex>0)
	{
		m_UpDifs.EnableWindow(TRUE);
	}
	if (m_CurLayerIndex < m_RestLayerInfo.GetSize()-1)
	{
		if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			CString str = m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer;
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
		}
		else
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
		}
		
		
	}
	else
	{
		m_CurLayerIndex = m_RestLayerInfo.GetSize()-1;
		if (m_CurLayerIndex<0)
		{
			m_CurLayerIndex=0;
		}
		if (m_RestLayerInfo.GetAt(m_CurLayerIndex).strGroup.CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,0,m_RestLayerInfo.GetAt(m_CurLayerIndex).strLayer,NULL,FALSE);
		}
		else
		{
			m_hSelectedTreeItemArray[DES-1] = LocateSpecificTreeItem(m_DesTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
			m_hSelectedTreeItemArray[SRC-1] = LocateSpecificTreeItem(m_SrcTree,TVIS_BOLD,TVIS_BOLD,m_RestLayerInfo.GetAt(m_CurLayerIndex).iLayerCode,"",NULL);
		}
		
		//�����µİ�ť�ҵ���
		m_DownDifs.EnableWindow(FALSE);
		
	}
	
}

int CSchemeMerge::GetFirstLayerTxtofGroup(CArray<LayerInfo,LayerInfo>& restLayerInfo,CString strGroupName,CString& strLayerNodeTxt)
{
  if (strGroupName==""||restLayerInfo.GetSize()<=0)
  {
	  return -1;
  }
  for(int i=0; i<restLayerInfo.GetSize();i++)
  {
	  if (strGroupName==restLayerInfo.GetAt(i).strGroup)
	  {
		  CString strLayerCode;
		  strLayerCode.Format("%I64d",restLayerInfo.GetAt(i).iLayerCode);
		  strLayerNodeTxt = strLayerCode + " " + restLayerInfo.GetAt(i).strLayer;
		  return i;
	  }

  }
  return -1;
}

void CSchemeMerge::SortLayerInfo(CArray<LayerInfo,LayerInfo>& layersInfo,USERIDX& userIdx )
{
	if (layersInfo.GetSize()<=0)
	{
		return;
	}
	CArray<LayerInfo,LayerInfo> tmpLayersInfo;
	tmpLayersInfo.Copy(layersInfo);
	layersInfo.RemoveAll();
	int iSpecialGroupIdx=-1;
	for (int j=0; j<userIdx.m_aGroup.GetSize(); j++)
	{
		if (((CString)userIdx.m_aGroup[j].GroupName).CompareNoCase(StrFromResID(IDS_SPECIALGROUP))==0)
		{
			iSpecialGroupIdx = j;
		}
		for (int i=0; i<userIdx.m_aIdx.GetSize(); i++)
		{
			CString strLayerCode="";
			CString strLayerName="";
			CString strLayerNodeTxt="";
			strLayerCode.Format("%d",userIdx.m_aIdx[i].code);
			strLayerName.Format("%s",userIdx.m_aIdx[i].FeatureName);

			if (userIdx.m_aIdx[i].groupidx==j)
			{
				if (iSpecialGroupIdx==userIdx.m_aIdx[i].groupidx)//Ϊ������ʱ��
				{
					strLayerNodeTxt = strLayerName;
				}
				else
				{
					strLayerNodeTxt = strLayerCode+" "+strLayerName;
				}
			}

			int iRtnIdx = -1;
			if ((iRtnIdx=hasFoundSameLayerin(tmpLayersInfo,strLayerNodeTxt))>=0)
			{
				layersInfo.Add(tmpLayersInfo[iRtnIdx]);
			}
		}
	}

	return;
}

void CSchemeMerge::OnButtonSrcpath() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	char szpath[512];
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),szpath,m_DesSchemePath,m_hWnd) )
	{
		return;
	}
	m_bChangedSrcSchemePath = FALSE; //2012.9.13
	CString strPath(szpath);
	if (IsValidScaleFile(strPath))
	{
		//�ж�Դ��Ŀ������·���Ƿ���ȫ��ͬ
		if(m_DesSchemePath.CompareNoCase(strPath)==0)
		{		
			AfxMessageBox(IDS_ERR_SAMESHEME,MB_OK);
			return;
		}
		if (m_SrcSchemePath.CompareNoCase(strPath)!=0) //����·����ͬʱ�����޸ġ�
		{	
			m_bChangedSrcSchemePath = TRUE; //2012.9.13 
			m_SrcSchemePath = strPath; 
			//��ȡԴ�������ÿ�·��
			int strLen = m_SrcSchemePath.GetLength();
			int pos = m_SrcSchemePath.ReverseFind('\\');
			CString strSrcConfigPath="";
			CString strCurScale="";
			strSrcConfigPath =m_SrcSchemePath.Left(pos);
			m_SrcCfgPath = strSrcConfigPath;
			strCurScale = m_SrcSchemePath.Right(strLen-pos-1);
			UpdateData(FALSE);
			//����Դ���������߶�ѡ��
			
			//{hcw,2012.8.8
			if (m_pSrcCfgLibMan)
			{
				delete m_pSrcCfgLibMan;
				m_pSrcCfgLibMan = NULL;
			}
			//}
			m_pSrcCfgLibMan = new CConfigLibManager(); //hcw,2012.7.27,cancel off.
			//{2012.9.18
			m_pSrcCfgLibMan->ResetConfig();
			m_pSrcCfgLibMan->SetPath("");
			m_pSrcCfgLibMan->LoadConfig(m_SrcCfgPath,FALSE);
			//}
			FillMergeDlgScaleCombo(m_SrcComBoScale,strSrcConfigPath,m_pSrcCfgLibMan); //��ȡm_SrcConfig	
			//���õ�ǰѡ����
			m_SrcComBoScale.SelectString(-1,strCurScale);
			//��ȡm_SrcConfig
			m_SrcScale = atoi(strCurScale);
			m_SrcConfig = m_pSrcCfgLibMan->GetConfigLibItemByScale(m_SrcScale);
			m_SrcConfig.Load();
			//����Դ������
			m_bIsSrcTreeLoaded = TRUE; //2012.9.20
			FillMergeDlgTree(m_SrcTree,m_SrcUserIdx,m_SrcConfig,m_hSrcTreeItemMap);
		}
		
	}
	else
	{
		AfxMessageBox(IDS_INVALID_SYMLIBPATH,MB_OK);
	}
	UpdateData(FALSE);
	return;
}
void CSchemeMerge::SaveScheme()
{
	m_DesConfig.pScheme->m_strSortedGroupName.RemoveAll();
	for (int i=0; i<m_DesUserIdx.m_aGroup.GetSize(); i++)
	{
		m_DesConfig.pScheme->m_strSortedGroupName.Add(m_DesUserIdx.m_aGroup.GetAt(i).GroupName);
	}
	m_DesConfig.pScheme->Save();
	//{2012.8.1
	if (!m_BackupCurDesSchemeXML.IsEmpty())
	{
		m_BackupCurDesSchemeXML.Empty();
	}
	//}
	m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
	m_DesConfig.pPlaceCfgLib->Save();
	m_DesConfig.SaveCellLine();
	return;
}

void CSchemeMerge::OnButtonDespath() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	char szpath[512];
	if( !BrowseFolderEx(StrFromResID(IDS_SELECT_FILE),szpath,m_DesSchemePath,m_hWnd) )
	{
		return;
	}
	m_bChangedDesSchemePath = FALSE; //2012.9.13
	CString strDesPath(szpath);
	if (IsValidScaleFile(strDesPath))
	{
		if (m_SrcSchemePath.CompareNoCase(strDesPath)==0)
		{			
			AfxMessageBox(IDS_ERR_SAMESHEME,MB_OK);
			return;
		}
		if (m_DesSchemePath.CompareNoCase(strDesPath)!=0)
		{
			m_bChangedDesSchemePath = TRUE; //2012.9.13
			//Ŀ�귽��������ʾ���档
			if (m_bModified)
			{
				//�ָ�����ѡ��ť��״̬
				m_UpDifs.EnableWindow(FALSE);
				m_DownDifs.EnableWindow(FALSE);
				//��ʾ����
				CString hintTxt,hintCaption;
				hintTxt = StrFromResID(IDS_SAVE_SCHEME);
				hintCaption.LoadString(IDS_ATTENTION);
				if (MessageBox(hintTxt,hintCaption,MB_YESNO|MB_ICONASTERISK)==IDYES)
				{
					SaveScheme();
					m_bSaved = SAVED; //2012.9.13
				}
				else
				{
					//������
					m_bSaved = NOSAVED; //2012.9.13
					AfxGetMainWnd()->SendMessage(FCCM_COLLECTIONVIEW,0,0);
					m_DesConfig.pScheme->ReadFrom(m_BackupCurDesSchemeXML);
					
				}
				m_bModified = FALSE;
			}
			m_DesSchemePath = strDesPath;
			m_MergeSchemePath = strDesPath;
			//��ȡ���ÿ�·��
			int strLen = m_DesSchemePath.GetLength();
			int pos = m_DesSchemePath.ReverseFind('\\');
			CString strDesConfigPath = m_DesSchemePath.Left(pos);
			m_DesCfgPath = strDesConfigPath;
			CString strDesScale = m_DesSchemePath.Right(strLen-pos-1);
			UpdateData(FALSE);
			//����Ŀ�귽�������߶�ѡ��
			
			//{hcw,2012.8.8
			if(m_pDesCfgLibMan)
			{
				delete m_pDesCfgLibMan;
				m_pDesCfgLibMan = NULL;
			}
			//}
			m_pDesCfgLibMan = new CConfigLibManager(); 
			//{2012.9.18
			m_pDesCfgLibMan->ResetConfig();
			m_pDesCfgLibMan->SetPath("");
			m_pDesCfgLibMan->LoadConfig(m_DesCfgPath,FALSE);
			//}
			FillMergeDlgScaleCombo(m_DesComBoScale,strDesConfigPath,m_pDesCfgLibMan);
			//���õ�ǰѡ���
			m_DesComBoScale.SelectString(-1,strDesScale);
			//��ȡm_DesConfig
			m_DesScale = atoi(strDesScale);
			m_DesConfig = m_pDesCfgLibMan->GetConfigLibItemByScale(m_DesScale);
			m_DesConfig.Load();
			//{2012.8.1
			m_BackupCurDesSchemeXML.Empty();
			m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
			//}
			//����Ŀ�귽����
			FillMergeDlgTree(m_DesTree,m_DesUserIdx,m_DesConfig,m_hDesTreeItemMap);
			
			m_HandleMerge.EnableWindow(TRUE);//2012.9.24
		}
		
	}
	else
	{
		AfxMessageBox(IDS_INVALID_SYMLIBPATH,MB_OK);
	}
	UpdateData(FALSE);
	return;	
}

//����������ʹ�ã�2012.8.31
BOOL CSchemeMerge::InitialSchemeMergeDialog()
{
	UpdateData(TRUE);
	//hcw,2012.8.31
	if ((!m_pDesCfgLibMan)
		||(!m_pSrcCfgLibMan))
	{
		return FALSE;
	}
	//}

	m_HandleMerge.EnableWindow(FALSE);
	m_UpDifs.EnableWindow(FALSE);
	m_DownDifs.EnableWindow(FALSE);
	//{hcw,2012.5.30,��ť��ʾ
	if(!m_ToolTip.Create(this,TTS_ALWAYSTIP))
	{
		TRACE(_T("Unable to create ToolTip :-)"));
		return TRUE;
	}
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_HANDMERGE),"�ֶ��ϲ�ѡ�еĲ����");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_DEL),"ɾ��Ŀ�귽���еĲ����");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_UP),"�����һ����ͬ��");
	m_ToolTip.AddTool(GetDlgItem(IDC_BUTTON_DOWN),"�����һ����ͬ��");
	m_ToolTip.SetDelayTime(50);
	//}

	FillMergeDlgScaleCombo(m_DesComBoScale,m_pDesCfgLibMan,m_DesConfig,m_BackupDesConfig,m_BackupCurDesSchemeXML,m_DesScale,DES);
	FillMergeDlgScaleCombo(m_SrcComBoScale,m_pSrcCfgLibMan,m_SrcConfig,m_BackupSrcConfig,m_BackupCurSrcSchemeXML,m_SrcScale,SRC);
	FillEdit();
	FillMergeDlgTree(m_DesTree, m_DesUserIdx, m_DesConfig, m_hDesTreeItemMap);
	m_DesBackupUserIdx = m_DesUserIdx;
	FillMergeDlgTree(m_SrcTree, m_SrcUserIdx, m_SrcConfig, m_hSrcTreeItemMap);
	m_SrcBackUpUserIdx = m_SrcUserIdx;
	//{�����ؼ�Ϊ��ʱ,����
	if ((m_DesTree.GetCount()<=0)
		&&(m_SrcTree.GetCount()<=0))
	{
		ReSetDlgCtrl();
	}
	//}
	UpdateData(FALSE);
	
	return TRUE;
}

void CSchemeMerge::OnButtonSave() 
{
	// TODO: Add extra cleanup here
	if (m_bModified)
	{
		// �������˳��
		m_DesConfig.pScheme->m_strSortedGroupName.RemoveAll();
		for (int i=0; i<m_DesUserIdx.m_aGroup.GetSize(); i++)
		{
			m_DesConfig.pScheme->m_strSortedGroupName.Add(m_DesUserIdx.m_aGroup.GetAt(i).GroupName);
		}
		
		m_DesConfig.pScheme->Save();			
		m_DesConfig.pScheme->WriteTo(m_BackupCurDesSchemeXML);
		
		//pPlaceCfgLib�ı��档
		m_DesConfig.pPlaceCfgLib->Save();
		
		//pCellDefLib��pLinetypeLib�ı���
		m_DesConfig.SaveCellLine();
		m_bModified = FALSE;
	}
	
	//{hcw,2012.7.30
	m_pDesCfgLibMan->ResetConfig();
	CString strDesConfigPath = m_pDesCfgLibMan->GetPath();
	m_pDesCfgLibMan->SetPath("");
	m_pDesCfgLibMan->LoadConfig(strDesConfigPath,FALSE);
	//}
	//{2012.8.2
	if (m_SrcCfgPath.CompareNoCase(m_DesCfgPath)==0)
	{
		m_pSrcCfgLibMan->SetPath("");
		m_pSrcCfgLibMan->LoadConfig(m_SrcCfgPath);
	}
	//}
	
	KillTimer(TIMERID_CREATEIMAGE);
	KillTimer(TIMERID_PREVIEW);
	CDialog::OnOK();
}


