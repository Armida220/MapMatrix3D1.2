#if !defined(AFX_SCHEMEMERGE_H__BC97B184_FF9F_4B14_BD90_D2D657ECCF35__INCLUDED_)
#define AFX_SCHEMEMERGE_H__BC97B184_FF9F_4B14_BD90_D2D657ECCF35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SchemeMerge.h : header file
//Author: hcw
#include "SymbolLib.h"
#include "CollectionTreeCtrl.h"
#include "MergeTree.h"
#include <set>
#include <map>
#include "resource.h" //hcw,2012.8.31
using namespace std;
/////////////////////////////////////////////////////////////////////////////
// CSchemeMerge dialog
#define DES 1
#define SRC 2
#define NOSAVED 3
#define SAVED 4
typedef struct LayerInfo
{
	CString strGroup;
	CString strLayer;
	__int64 iLayerCode;
	//hcw,2012.7.19,Value Copy.
	void Copy(const LayerInfo& layerInfo)
	{
		this->strGroup = layerInfo.strGroup;
		this->strLayer = layerInfo.strLayer;
		this->iLayerCode = layerInfo.iLayerCode;
		return;
	}
}LayerInfo,*pSchemeInfo;

typedef struct HTREEITEMEX
{
	HTREEITEM m_hItem;
	int iTreeID;	
}HTREEITEMEX,*PHTREEITEMEX;

class CSchemeMerge : public CDialog
{
// Construction
public:
	CSchemeMerge(CWnd* pParent = NULL,CConfigLibManager* pSrcCfgLibMan=NULL, CConfigLibManager* pDesCfgLibMan=NULL,int iDesScale=0,int iSrcScale=0,BOOL bLoadedSrcTree=FALSE);   // standard constructor

	virtual ~CSchemeMerge();
// Dialog Data
	//{{AFX_DATA(CSchemeMerge)
	enum { IDD = IDD_MERGE };
	CButton	m_BtnDesPath;
	CButton	m_BtnSrcPath;
	CButton	m_AutoMerge;
	CButton m_BtnSave;
	CButton	m_Del;
	CButton	m_UpDifs; //向上浏览不同项。
	CButton	m_DownDifs;//向下浏览不同项。
	CButton	m_HandleMerge; //手动合并按钮。
	CComboBox	m_DesComBoScale;
	CMergeTree	m_SrcTree;
	CMergeTree	m_DesTree;
	CComboBox	m_SrcComBoScale;
	CString	m_DesSchemePath;
	CString	m_SrcSchemePath;
	CString	m_MergeSchemePath;
	CToolTipCtrl m_ToolTip; //提示控件。
	//}}AFX_DATA
	
	//{hcw,2012.4.18
	BOOL FillMergeDlgTree(CMergeTree& treeCtrl, USERIDX& UserIdx, ConfigLibItem config, multimap<int,HTREEITEM>&hTreeItemMap);
	BOOL FillMergeDlgScaleCombo();
	BOOL FillMergeDlgScaleCombo(CComboBox& comBoScale,CConfigLibManager*& pCfgLibMan,ConfigLibItem& config,
							ConfigLibItem& backConfig,CString& backupSchemeXML,int& scale,UINT nSchemePos);//hcw,2012.7.25,便于重新进入此类对象时恢复退出前的状态。
	BOOL FillMergeDlgScaleCombo(CComboBox& comBoScale, CString strConfigPath, CConfigLibManager* pCfgLibMan);//hcw,2012.7.20,重载，便于更新源\目方案。
	void MergeGroupandLayer(ConfigLibItem& DesConfig,ConfigLibItem SrcConfig);//合并组和层
	BOOL MergeConfig(ConfigLibItem& DesConfig, ConfigLibItem SrcConfig); //hcw,2012.5.14,合并方案。
	int  IsExistIn(CArray<CSchemeLayerDefine*, CSchemeLayerDefine*>& SchemeInfo, 
					CSchemeLayerDefine* layerInfo);//已有方案數組SchemeInfo中是否存在相同的結構内容,
	BOOL IsCorrespondItem(CMergeTree&treeCtrl,HTREEITEM hItem, 
						CSchemeLayerDefine* schemeInfo, CStringArray& restGroupNameArray);//找出
												//返回SchemeInfo中相同内容的Index。
	BOOL IsCorrespondItem(CMergeTree&treeCtrl,HTREEITEM hItem, 
						LayerInfo layerInfo, CStringArray& restGroupNameArray);//重载，返回LayerInfo对应的树节点。
	void UpdateDrawTree();
	void HighLightDiff(); //hcw,2012.4.26,for test Loop paint in TreeCtrl
	void ExpandAll( CMergeTree& treeCtrl,HTREEITEM hItem );//hItem==NULL,展开整个树。
	void ExpandExt(CMergeTree& treeCtrl, map<CString, UINT>& GroupStateMap, HTREEITEM hItem); //不展开特定节点hItem以后,节点名为GroupNameArray的所有节点。
	
	void TravelandSetItemState(CMergeTree& treeCtrl, UINT nState, UINT nStateMask, HTREEITEM hItem, CSchemeLayerDefine* schemeInfo, CStringArray& StrGroupArray);
	void TravelandSetItemState(CMergeTree& treeCtrl, UINT nState, UINT nStateMask, HTREEITEM hItem, LayerInfo layerInfo, CStringArray& StrGroupArray);
	int FindinArray(CString str, CStringArray& strArray, BOOL bCase=TRUE);//存在相同的组,bCase==TRUE,区分大小写；bCase==FALSE,不区分大小写。
	
	//void LocateSpecialTreeItem(CMergeTree& treeCtrl, BOOL flag, int iLayerCode,  HTREEITEM hCurItem); //定位从当前节点开始后的第一个(未)被勾选的节点。
	HTREEITEM LocateSpecificTreeItem(CMergeTree& treeCtrl, UINT nState,UINT nStateMask, __int64 iLayerCode, CString strNodeText, HTREEITEM hCurItem, BOOL byCode=TRUE);//重载，根据节点的状态来判断。
	BOOL FillandHighLight(CMergeTree& treeCtrl, USERIDX& UserIdx,  ConfigLibItem config,
							UINT nState, UINT nStateMask, CArray<LayerInfo,LayerInfo>& restLayerInfo,
							multimap<int,HTREEITEM>& hTreeItemMap,UINT nSchemePosID);//合并后，在更新树的过程中设置不同节点的状态。
	void SetUpDownButtonState(int curLayerIndex); //根据curLayerIndex在restLayerInfo中的位置来设定UpDown按钮的状态。
	int hasFoundSameLayerin(CArray<LayerInfo,LayerInfo>& restLayerInfo , CString strLayerNodeTxt); //返回LayInfo中不同项的索引。
	int GetFirstLayerTxtofGroup(CArray<LayerInfo,LayerInfo>& restLayerInfo,CString strGroupName,CString& strLayerNodeTxt);//获取组下的第一个层的文本。

	int FindCorrespondItem(CMergeTree& treeCtrl,HTREEITEM hItem, ConfigLibItem config); //在方案中找到书节点对应的层节点，并返回方案文件中的层索引。
	
	void FillEdit(); //初始化界面编辑框。
	void ReLoad(ConfigLibItem& DesConfig, ConfigLibItem& SrcConfig);//重新加载比例尺方案。
	BOOL hasDetailedDiffs(CSchemeLayerDefine *pDesLayerDefine,CSchemeLayerDefine *pSrcLayerDefine); //for Reserved,同名和同层码的层方案定义中的具体内容之间的差异。
	int  GetHtreeItemFromMap(int idx,multimap<int,HTREEITEM> hTreeItemMap,CTreeItemList& treeItemList); //根据层码和HTREEITEM之间的映射来定位指定层码的HTREEITEM,返回指定层码的HTREEITEM;
	BOOL DeleteItemfromTree(CMergeTree& treeCtrl, CTreeItemList& treeItemList); //删除指定层码的HTREEITEM.
	BOOL DeleteItemfromMap(int iCode, multimap<int,HTREEITEM>& hTreeItemMap);
	
	void DeleteRestLayerInfo(CArray<LayerInfo,LayerInfo>& restLayerInfo, __int64 iLayerCode, CString strLayerName, BOOL bByCode=TRUE);//删除restLayerInfo中指定层码的相关信息。
	//组合并
	BOOL GetLayerSchemeofGroup(CScheme* pScheme,CString selectedGroupName, int &nfirstLayerIndex,
						CArray<CSchemeLayerDefine*,CSchemeLayerDefine*>&pLayerDefineSpecificGroup);//获取方案中指定组的层码。
	void MergeGrouptoDesScheme(CScheme* pScheme, int nFirstLayerIndex,CArray<CSchemeLayerDefine*,CSchemeLayerDefine*>& pLayerDefineArray);//
	void SaveCellDefandLinetypeLib(ConfigLibItem& Config);
	
	//获取树节点的状态
	void GetGroupStateMap(CMergeTree& treeCtrl, map<CString, UINT>& GroupStateMap, UINT nStateMask);
	void AddGroupStateMap(map<CString,UINT>& GroupStateMap,CStringArray& SelectedGroups, UINT nState);
	UINT GetNodeState(map<CString, UINT>& NodeStateMap, CString strNodeText);
	//获取树中的当前选中节点文本。
	void GetSelectedItemsText(CMergeTree& treeCtrl,CTreeItemList&curSelectedTreeNodeList,CStringArray& selectedItemsText);
	//获取比例尺
	int GetSrcScale(){return m_SrcScale;};
	int GetDesScale(){return m_DesScale;};
	//获取方案管理库路径。
	CConfigLibManager *GetSrcCfgLibMan(){return m_pSrcCfgLibMan;};
	CConfigLibManager *GetDesCfgLibMan(){return m_pDesCfgLibMan;};
	void SortLayerInfo(CArray<LayerInfo,LayerInfo>& layersInfo,USERIDX& userIdx);//hcw,2012.7.19,对自定义层数组按照方案中的先后顺序重新排序。
	BOOL IsSrcTreeLoaded(){return m_bIsSrcTreeLoaded; }; //hcw,2012.9.20,目标方案树是否被加载。
	CConfigLibManager *m_pDesCfgLibMan; //hcw,2012.7.20,目标方案下对不同比例尺下配置库的管理
	CConfigLibManager *m_pSrcCfgLibMan; //hcw,2012.7.20,源方案下对不同比例尺下配置库的管理
	CString m_DesCfgPath;//hcw,2012.7.24,目标方案路径
	CString m_SrcCfgPath;//hcw,2012.7.24,源方案路径
	ConfigLibItem m_DesConfig; //目标方案配置
	ConfigLibItem m_SrcConfig; //源方案配置
	ConfigLibItem m_BackupDesConfig;//目标方案配置备份
	ConfigLibItem m_BackupSrcConfig;//源方案配置备份。
	USERIDX m_DesUserIdx;
	USERIDX m_DesBackupUserIdx;//目标备份

	USERIDX m_SrcUserIdx;
	USERIDX m_SrcBackUpUserIdx;//源备份
     
	CString m_BackupCurDesSchemeXML;//备份当前目标解决方案。
	CString m_BackupCurSrcSchemeXML;//备份当前源解决方案。

	CStringArray m_JustAppendedCellNames;//一次手动或自动合并后已经添加过的图元名。
	CStringArray m_JustAppendedLineTypeNames;//一次手动或自动合并后刚刚已经添加过得线型名。
	CStringArray m_SameCellNames;//重复的图元。
	CStringArray m_SameLineTypeNames;//重复的线型。
	//
	BOOL m_bModified; //方案是否更改过。
	BOOL m_bSpecialRtn; //删除独立项的返回值。
	int m_DesScale; //目标方案比例尺
	int m_SrcScale;//源方案比例尺
	int m_iSrcLayerCode ;//源方案层码
	int m_iDesLayerCode; //目标方案层码
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_DesSchemeInfo; //目标方案信息。
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_SrcSchemeInfo;	//源方案信息。
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_RestSchemeInfo; //两方案中的不同的层信息(CSchemeLayerDefine)。
	CArray<LayerInfo,LayerInfo> m_RestLayerInfo; //两方案中的不同的层信息(包括独立符号层信息),且只存储层信息。
	CArray<LayerInfo,LayerInfo> m_RestSpecialLayerInfo;//两个方案中不同的独立符号层信息，用于回滚m_RestLayerInfo.
	int m_CurLayerIndex;//LayerInfo数组中的当前的层序号。
	CStringArray m_RestStrGroupArray;//两方案中不同的组信息，且只存储组信息。
	//HTREEITEM m_hCurTreeItem;//当前的树节点。
	CTreeItemList m_CurSrcSelectedTreeNodeList;//当前在源方案中选中的所有节点。
	CStringArray m_SrcSelectedItemsText;//获取当前选中节点的文本组。
	HTREEITEM m_hSelectedItem;//鼠标选中的节点。
	HTREEITEM m_hCurItem;//for OnMoveLayer;
	int m_nMovedLayer;//要移动的层码。
	HTREEITEM m_hMovedItem;//要移动的源树节点。
    HTREEITEMEX m_hSelectedItemEx;//选中节点的拓展。
	HTREEITEM m_hSelectedTreeItemArray[2];//记录源，目两棵树上的当前选中节点。0：源方案，1：目标方案。
	int m_SchemePosID;//标注（源、目）方案。
	int m_bSpecialLayer;//独立层。
	int m_bUnMerged;//相同层码的层是否覆盖，若m_bUnMerged == FALSE,进行覆盖；若m_bUnMerged == TRUE,不进行覆盖。
					//若m_bUnMerged == -1，取消操作退出程序。
	BOOL m_bHasSameLayerCode;//目标方案中是否存在和源方案中相同的层码。若为TRUE,存在，若为FALSE,不存在。
	int m_nDesRemovedLayer; //从目标方案中被移动的节点。
	int m_nMovedGroup; //源方案中被合并的组。
	CString m_SrcSelectedGroupName;//源方案中被选中的组名。
	int m_nDesRemovedGroup;//目标方案中应重复或覆盖被删除的层。
	multimap<int,HTREEITEM> m_hDesTreeItemMap;//建立目标方案层码和树节点指针HTREEITEM之间的映射关系。
	multimap<int,HTREEITEM> m_hSrcTreeItemMap;//建立源方案层码和树节点指针HTREEITEM之间的映射关系。
	
	map<CString, UINT> m_DesGroupStateMap; //记录目标方案树中组节点名和其对应状态(eg:TVIS_EXPANDED)
	map<CString, UINT> m_SrcGroupStateMap; //记录目标方案树中组节点名和其对应状态(eg:TVIS_EXPANDED)

	//}
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchemeMerge)
	public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL InitialSchemeMergeDialog(); //在派生类中使用，2012.8.31
	void DelGroup(CString strGroupName,USERIDX& UserIdx, ConfigLibItem& Config, BOOL bSpecial= FALSE);//删除组
	void DelLayer(__int64 iLayerCode,CString strLayerName, USERIDX& UserIdx, ConfigLibItem& Config,BOOL bSpecial=FALSE);//根据层码删除层
	void DelLayer(CString strNodeText, USERIDX& UserIdx, ConfigLibItem& Config);//根据节点文本删除层。
	BOOL isValidDelItem(CMergeTree &treeCtrl, HTREEITEM hItem);

	void OnMoveLayer(); //选择要移动的层
	void OnCopyToLayer();//将选中的层移动的特定层上
	void OnMoveGroup(); //选择要移动的组
	void OnCopyToGroup();//将选中的组移动到特定的组后。
	//多选模式下
	void OnMoveSrcLayer(HTREEITEM hSrcItem);
	void OnCopyToDesLayer(HTREEITEM& hDesItem);
	void OnMoveSrcGroup(HTREEITEM hSrcItem);
	void OnCopyToDesGroup(HTREEITEM hDesItem);	
	void AddDifLayerInfo(CArray<LayerInfo,LayerInfo>& restLayerInfo,CSchemeLayerDefine*pSchemeLayerDefine);
	
	void MergeActionandSymbols(CPlaceConfigLib* pPlaceCfgLib,CCellDefLib *pCellDefLib,
							CBaseLineTypeLib *pLineTypeLib, CSchemeLayerDefine*pSchemeLayerDefine);//hcw,2012.6.7,especaily for HandMerged.
	void SaveScheme();//hcw,2012.7.24.
	void ReSetDlgCtrl();//释放控件变量，2012.7.26
	
	CSchemeLayerDefine* m_pMovedLayerDefine;
	CString m_strSrcGroupName;//源方案中的数组名。
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_pMovedLayerDefineArray; //用于存放组下的所有层。
	int m_firstDesSameGroupIndex; //首次合并时遇到相同的组。
	BOOL m_bFirstSelectedIndex;//多选层项中首次选择插入的位置。
	BOOL m_bFirstPrompt; //同层码相同时首次弹框。
	BOOL m_bFirstPromptInSymbolLib;//图元相同时，给出是否覆盖的提示。
	BOOL m_bCoveredInSymbolLib;//相同图元是否替换。
	int m_nDesIndex;//层节点要插入的位置。
	int m_nDesSelectedGroupIndex;//组节点要插入的位置
	CStringArray m_SelectedGroupsNonExistInDes;//源方案中被选中的树的组节点名。
	int m_nSameCellCount  ;
	int m_nSameLineType;
	HTREEITEM m_hSrcParentItem;//源方案中被选中节点的父节点。
	BOOL m_bChangedSrcComboScale; //源方案比例尺是否已经更改。
	BOOL m_bChangedDesComboScale; //目标方案比例尺是否已经更改。
	BOOL m_bChangedSrcSchemePath; //源方案路径已更改。2012.9.13
	BOOL m_bChangedDesSchemePath; //目标方案路径已更改。2012.9.13
	BOOL m_bSaved;				   //是否已经保存。2012.9.13
	BOOL m_bIsSrcTreeLoaded;			//合并方案中目标树是否已经加载过.2012.9.20
	// Generated message map functions
	//{{AFX_MSG(CSchemeMerge)
	afx_msg void OnSelChangeDesScaleCombo();
	afx_msg void OnSelChangeSrcScaleCombo();
	afx_msg void OnButtonMerge();
	afx_msg void OnCancel(); 
	afx_msg void OnDeltaposMergeSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonDel();
	
	afx_msg void OnSelchangedTreeDes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeSrc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonHandmerge();
	afx_msg void OnKillfocusTreeSrcscheme2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTreeSrcscheme2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetfocusTreeDescheme2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusTreeDescheme2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonSrcpath();
	afx_msg void OnButtonDespath();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCHEMEMERGE_H__BC97B184_FF9F_4B14_BD90_D2D657ECCF35__INCLUDED_)
