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
	CButton	m_UpDifs; //���������ͬ�
	CButton	m_DownDifs;//���������ͬ�
	CButton	m_HandleMerge; //�ֶ��ϲ���ť��
	CComboBox	m_DesComBoScale;
	CMergeTree	m_SrcTree;
	CMergeTree	m_DesTree;
	CComboBox	m_SrcComBoScale;
	CString	m_DesSchemePath;
	CString	m_SrcSchemePath;
	CString	m_MergeSchemePath;
	CToolTipCtrl m_ToolTip; //��ʾ�ؼ���
	//}}AFX_DATA
	
	//{hcw,2012.4.18
	BOOL FillMergeDlgTree(CMergeTree& treeCtrl, USERIDX& UserIdx, ConfigLibItem config, multimap<int,HTREEITEM>&hTreeItemMap);
	BOOL FillMergeDlgScaleCombo();
	BOOL FillMergeDlgScaleCombo(CComboBox& comBoScale,CConfigLibManager*& pCfgLibMan,ConfigLibItem& config,
							ConfigLibItem& backConfig,CString& backupSchemeXML,int& scale,UINT nSchemePos);//hcw,2012.7.25,�������½���������ʱ�ָ��˳�ǰ��״̬��
	BOOL FillMergeDlgScaleCombo(CComboBox& comBoScale, CString strConfigPath, CConfigLibManager* pCfgLibMan);//hcw,2012.7.20,���أ����ڸ���Դ\Ŀ������
	void MergeGroupandLayer(ConfigLibItem& DesConfig,ConfigLibItem SrcConfig);//�ϲ���Ͳ�
	BOOL MergeConfig(ConfigLibItem& DesConfig, ConfigLibItem SrcConfig); //hcw,2012.5.14,�ϲ�������
	int  IsExistIn(CArray<CSchemeLayerDefine*, CSchemeLayerDefine*>& SchemeInfo, 
					CSchemeLayerDefine* layerInfo);//���з������MSchemeInfo���Ƿ������ͬ�ĽY������,
	BOOL IsCorrespondItem(CMergeTree&treeCtrl,HTREEITEM hItem, 
						CSchemeLayerDefine* schemeInfo, CStringArray& restGroupNameArray);//�ҳ�
												//����SchemeInfo����ͬ���ݵ�Index��
	BOOL IsCorrespondItem(CMergeTree&treeCtrl,HTREEITEM hItem, 
						LayerInfo layerInfo, CStringArray& restGroupNameArray);//���أ�����LayerInfo��Ӧ�����ڵ㡣
	void UpdateDrawTree();
	void HighLightDiff(); //hcw,2012.4.26,for test Loop paint in TreeCtrl
	void ExpandAll( CMergeTree& treeCtrl,HTREEITEM hItem );//hItem==NULL,չ����������
	void ExpandExt(CMergeTree& treeCtrl, map<CString, UINT>& GroupStateMap, HTREEITEM hItem); //��չ���ض��ڵ�hItem�Ժ�,�ڵ���ΪGroupNameArray�����нڵ㡣
	
	void TravelandSetItemState(CMergeTree& treeCtrl, UINT nState, UINT nStateMask, HTREEITEM hItem, CSchemeLayerDefine* schemeInfo, CStringArray& StrGroupArray);
	void TravelandSetItemState(CMergeTree& treeCtrl, UINT nState, UINT nStateMask, HTREEITEM hItem, LayerInfo layerInfo, CStringArray& StrGroupArray);
	int FindinArray(CString str, CStringArray& strArray, BOOL bCase=TRUE);//������ͬ����,bCase==TRUE,���ִ�Сд��bCase==FALSE,�����ִ�Сд��
	
	//void LocateSpecialTreeItem(CMergeTree& treeCtrl, BOOL flag, int iLayerCode,  HTREEITEM hCurItem); //��λ�ӵ�ǰ�ڵ㿪ʼ��ĵ�һ��(δ)����ѡ�Ľڵ㡣
	HTREEITEM LocateSpecificTreeItem(CMergeTree& treeCtrl, UINT nState,UINT nStateMask, __int64 iLayerCode, CString strNodeText, HTREEITEM hCurItem, BOOL byCode=TRUE);//���أ����ݽڵ��״̬���жϡ�
	BOOL FillandHighLight(CMergeTree& treeCtrl, USERIDX& UserIdx,  ConfigLibItem config,
							UINT nState, UINT nStateMask, CArray<LayerInfo,LayerInfo>& restLayerInfo,
							multimap<int,HTREEITEM>& hTreeItemMap,UINT nSchemePosID);//�ϲ����ڸ������Ĺ��������ò�ͬ�ڵ��״̬��
	void SetUpDownButtonState(int curLayerIndex); //����curLayerIndex��restLayerInfo�е�λ�����趨UpDown��ť��״̬��
	int hasFoundSameLayerin(CArray<LayerInfo,LayerInfo>& restLayerInfo , CString strLayerNodeTxt); //����LayInfo�в�ͬ���������
	int GetFirstLayerTxtofGroup(CArray<LayerInfo,LayerInfo>& restLayerInfo,CString strGroupName,CString& strLayerNodeTxt);//��ȡ���µĵ�һ������ı���

	int FindCorrespondItem(CMergeTree& treeCtrl,HTREEITEM hItem, ConfigLibItem config); //�ڷ������ҵ���ڵ��Ӧ�Ĳ�ڵ㣬�����ط����ļ��еĲ�������
	
	void FillEdit(); //��ʼ������༭��
	void ReLoad(ConfigLibItem& DesConfig, ConfigLibItem& SrcConfig);//���¼��ر����߷�����
	BOOL hasDetailedDiffs(CSchemeLayerDefine *pDesLayerDefine,CSchemeLayerDefine *pSrcLayerDefine); //for Reserved,ͬ����ͬ����Ĳ㷽�������еľ�������֮��Ĳ��졣
	int  GetHtreeItemFromMap(int idx,multimap<int,HTREEITEM> hTreeItemMap,CTreeItemList& treeItemList); //���ݲ����HTREEITEM֮���ӳ������λָ�������HTREEITEM,����ָ�������HTREEITEM;
	BOOL DeleteItemfromTree(CMergeTree& treeCtrl, CTreeItemList& treeItemList); //ɾ��ָ�������HTREEITEM.
	BOOL DeleteItemfromMap(int iCode, multimap<int,HTREEITEM>& hTreeItemMap);
	
	void DeleteRestLayerInfo(CArray<LayerInfo,LayerInfo>& restLayerInfo, __int64 iLayerCode, CString strLayerName, BOOL bByCode=TRUE);//ɾ��restLayerInfo��ָ������������Ϣ��
	//��ϲ�
	BOOL GetLayerSchemeofGroup(CScheme* pScheme,CString selectedGroupName, int &nfirstLayerIndex,
						CArray<CSchemeLayerDefine*,CSchemeLayerDefine*>&pLayerDefineSpecificGroup);//��ȡ������ָ����Ĳ��롣
	void MergeGrouptoDesScheme(CScheme* pScheme, int nFirstLayerIndex,CArray<CSchemeLayerDefine*,CSchemeLayerDefine*>& pLayerDefineArray);//
	void SaveCellDefandLinetypeLib(ConfigLibItem& Config);
	
	//��ȡ���ڵ��״̬
	void GetGroupStateMap(CMergeTree& treeCtrl, map<CString, UINT>& GroupStateMap, UINT nStateMask);
	void AddGroupStateMap(map<CString,UINT>& GroupStateMap,CStringArray& SelectedGroups, UINT nState);
	UINT GetNodeState(map<CString, UINT>& NodeStateMap, CString strNodeText);
	//��ȡ���еĵ�ǰѡ�нڵ��ı���
	void GetSelectedItemsText(CMergeTree& treeCtrl,CTreeItemList&curSelectedTreeNodeList,CStringArray& selectedItemsText);
	//��ȡ������
	int GetSrcScale(){return m_SrcScale;};
	int GetDesScale(){return m_DesScale;};
	//��ȡ���������·����
	CConfigLibManager *GetSrcCfgLibMan(){return m_pSrcCfgLibMan;};
	CConfigLibManager *GetDesCfgLibMan(){return m_pDesCfgLibMan;};
	void SortLayerInfo(CArray<LayerInfo,LayerInfo>& layersInfo,USERIDX& userIdx);//hcw,2012.7.19,���Զ�������鰴�շ����е��Ⱥ�˳����������
	BOOL IsSrcTreeLoaded(){return m_bIsSrcTreeLoaded; }; //hcw,2012.9.20,Ŀ�귽�����Ƿ񱻼��ء�
	CConfigLibManager *m_pDesCfgLibMan; //hcw,2012.7.20,Ŀ�귽���¶Բ�ͬ�����������ÿ�Ĺ���
	CConfigLibManager *m_pSrcCfgLibMan; //hcw,2012.7.20,Դ�����¶Բ�ͬ�����������ÿ�Ĺ���
	CString m_DesCfgPath;//hcw,2012.7.24,Ŀ�귽��·��
	CString m_SrcCfgPath;//hcw,2012.7.24,Դ����·��
	ConfigLibItem m_DesConfig; //Ŀ�귽������
	ConfigLibItem m_SrcConfig; //Դ��������
	ConfigLibItem m_BackupDesConfig;//Ŀ�귽�����ñ���
	ConfigLibItem m_BackupSrcConfig;//Դ�������ñ��ݡ�
	USERIDX m_DesUserIdx;
	USERIDX m_DesBackupUserIdx;//Ŀ�걸��

	USERIDX m_SrcUserIdx;
	USERIDX m_SrcBackUpUserIdx;//Դ����
     
	CString m_BackupCurDesSchemeXML;//���ݵ�ǰĿ����������
	CString m_BackupCurSrcSchemeXML;//���ݵ�ǰԴ���������

	CStringArray m_JustAppendedCellNames;//һ���ֶ����Զ��ϲ����Ѿ���ӹ���ͼԪ����
	CStringArray m_JustAppendedLineTypeNames;//һ���ֶ����Զ��ϲ���ո��Ѿ���ӹ�����������
	CStringArray m_SameCellNames;//�ظ���ͼԪ��
	CStringArray m_SameLineTypeNames;//�ظ������͡�
	//
	BOOL m_bModified; //�����Ƿ���Ĺ���
	BOOL m_bSpecialRtn; //ɾ��������ķ���ֵ��
	int m_DesScale; //Ŀ�귽��������
	int m_SrcScale;//Դ����������
	int m_iSrcLayerCode ;//Դ��������
	int m_iDesLayerCode; //Ŀ�귽������
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_DesSchemeInfo; //Ŀ�귽����Ϣ��
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_SrcSchemeInfo;	//Դ������Ϣ��
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_RestSchemeInfo; //�������еĲ�ͬ�Ĳ���Ϣ(CSchemeLayerDefine)��
	CArray<LayerInfo,LayerInfo> m_RestLayerInfo; //�������еĲ�ͬ�Ĳ���Ϣ(�����������Ų���Ϣ),��ֻ�洢����Ϣ��
	CArray<LayerInfo,LayerInfo> m_RestSpecialLayerInfo;//���������в�ͬ�Ķ������Ų���Ϣ�����ڻع�m_RestLayerInfo.
	int m_CurLayerIndex;//LayerInfo�����еĵ�ǰ�Ĳ���š�
	CStringArray m_RestStrGroupArray;//�������в�ͬ������Ϣ����ֻ�洢����Ϣ��
	//HTREEITEM m_hCurTreeItem;//��ǰ�����ڵ㡣
	CTreeItemList m_CurSrcSelectedTreeNodeList;//��ǰ��Դ������ѡ�е����нڵ㡣
	CStringArray m_SrcSelectedItemsText;//��ȡ��ǰѡ�нڵ���ı��顣
	HTREEITEM m_hSelectedItem;//���ѡ�еĽڵ㡣
	HTREEITEM m_hCurItem;//for OnMoveLayer;
	int m_nMovedLayer;//Ҫ�ƶ��Ĳ��롣
	HTREEITEM m_hMovedItem;//Ҫ�ƶ���Դ���ڵ㡣
    HTREEITEMEX m_hSelectedItemEx;//ѡ�нڵ����չ��
	HTREEITEM m_hSelectedTreeItemArray[2];//��¼Դ��Ŀ�������ϵĵ�ǰѡ�нڵ㡣0��Դ������1��Ŀ�귽����
	int m_SchemePosID;//��ע��Դ��Ŀ��������
	int m_bSpecialLayer;//�����㡣
	int m_bUnMerged;//��ͬ����Ĳ��Ƿ񸲸ǣ���m_bUnMerged == FALSE,���и��ǣ���m_bUnMerged == TRUE,�����и��ǡ�
					//��m_bUnMerged == -1��ȡ�������˳�����
	BOOL m_bHasSameLayerCode;//Ŀ�귽�����Ƿ���ں�Դ��������ͬ�Ĳ��롣��ΪTRUE,���ڣ���ΪFALSE,�����ڡ�
	int m_nDesRemovedLayer; //��Ŀ�귽���б��ƶ��Ľڵ㡣
	int m_nMovedGroup; //Դ�����б��ϲ����顣
	CString m_SrcSelectedGroupName;//Դ�����б�ѡ�е�������
	int m_nDesRemovedGroup;//Ŀ�귽����Ӧ�ظ��򸲸Ǳ�ɾ���Ĳ㡣
	multimap<int,HTREEITEM> m_hDesTreeItemMap;//����Ŀ�귽����������ڵ�ָ��HTREEITEM֮���ӳ���ϵ��
	multimap<int,HTREEITEM> m_hSrcTreeItemMap;//����Դ������������ڵ�ָ��HTREEITEM֮���ӳ���ϵ��
	
	map<CString, UINT> m_DesGroupStateMap; //��¼Ŀ�귽��������ڵ��������Ӧ״̬(eg:TVIS_EXPANDED)
	map<CString, UINT> m_SrcGroupStateMap; //��¼Ŀ�귽��������ڵ��������Ӧ״̬(eg:TVIS_EXPANDED)

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
	BOOL InitialSchemeMergeDialog(); //����������ʹ�ã�2012.8.31
	void DelGroup(CString strGroupName,USERIDX& UserIdx, ConfigLibItem& Config, BOOL bSpecial= FALSE);//ɾ����
	void DelLayer(__int64 iLayerCode,CString strLayerName, USERIDX& UserIdx, ConfigLibItem& Config,BOOL bSpecial=FALSE);//���ݲ���ɾ����
	void DelLayer(CString strNodeText, USERIDX& UserIdx, ConfigLibItem& Config);//���ݽڵ��ı�ɾ���㡣
	BOOL isValidDelItem(CMergeTree &treeCtrl, HTREEITEM hItem);

	void OnMoveLayer(); //ѡ��Ҫ�ƶ��Ĳ�
	void OnCopyToLayer();//��ѡ�еĲ��ƶ����ض�����
	void OnMoveGroup(); //ѡ��Ҫ�ƶ�����
	void OnCopyToGroup();//��ѡ�е����ƶ����ض������
	//��ѡģʽ��
	void OnMoveSrcLayer(HTREEITEM hSrcItem);
	void OnCopyToDesLayer(HTREEITEM& hDesItem);
	void OnMoveSrcGroup(HTREEITEM hSrcItem);
	void OnCopyToDesGroup(HTREEITEM hDesItem);	
	void AddDifLayerInfo(CArray<LayerInfo,LayerInfo>& restLayerInfo,CSchemeLayerDefine*pSchemeLayerDefine);
	
	void MergeActionandSymbols(CPlaceConfigLib* pPlaceCfgLib,CCellDefLib *pCellDefLib,
							CBaseLineTypeLib *pLineTypeLib, CSchemeLayerDefine*pSchemeLayerDefine);//hcw,2012.6.7,especaily for HandMerged.
	void SaveScheme();//hcw,2012.7.24.
	void ReSetDlgCtrl();//�ͷſؼ�������2012.7.26
	
	CSchemeLayerDefine* m_pMovedLayerDefine;
	CString m_strSrcGroupName;//Դ�����е���������
	CArray<CSchemeLayerDefine*,CSchemeLayerDefine*> m_pMovedLayerDefineArray; //���ڴ�����µ����в㡣
	int m_firstDesSameGroupIndex; //�״κϲ�ʱ������ͬ���顣
	BOOL m_bFirstSelectedIndex;//��ѡ�������״�ѡ������λ�á�
	BOOL m_bFirstPrompt; //ͬ������ͬʱ�״ε���
	BOOL m_bFirstPromptInSymbolLib;//ͼԪ��ͬʱ�������Ƿ񸲸ǵ���ʾ��
	BOOL m_bCoveredInSymbolLib;//��ͬͼԪ�Ƿ��滻��
	int m_nDesIndex;//��ڵ�Ҫ�����λ�á�
	int m_nDesSelectedGroupIndex;//��ڵ�Ҫ�����λ��
	CStringArray m_SelectedGroupsNonExistInDes;//Դ�����б�ѡ�е�������ڵ�����
	int m_nSameCellCount  ;
	int m_nSameLineType;
	HTREEITEM m_hSrcParentItem;//Դ�����б�ѡ�нڵ�ĸ��ڵ㡣
	BOOL m_bChangedSrcComboScale; //Դ�����������Ƿ��Ѿ����ġ�
	BOOL m_bChangedDesComboScale; //Ŀ�귽���������Ƿ��Ѿ����ġ�
	BOOL m_bChangedSrcSchemePath; //Դ����·���Ѹ��ġ�2012.9.13
	BOOL m_bChangedDesSchemePath; //Ŀ�귽��·���Ѹ��ġ�2012.9.13
	BOOL m_bSaved;				   //�Ƿ��Ѿ����档2012.9.13
	BOOL m_bIsSrcTreeLoaded;			//�ϲ�������Ŀ�����Ƿ��Ѿ����ع�.2012.9.20
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
