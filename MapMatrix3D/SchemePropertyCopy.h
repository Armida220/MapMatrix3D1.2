// SchemeCopy.h: interface for the CSchemeCopy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCHEMECOPY_H__0DA5E389_3C5B_4726_852B_51629D797294__INCLUDED_)
#define AFX_SCHEMECOPY_H__0DA5E389_3C5B_4726_852B_51629D797294__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//hcw,2012.8.30, for Copying Property

#include "SchemeMerge.h"
#include "SymbolLib.h"

//2012.9.13,用于记录当前复制过属性的源方案和目标方案的节点。
struct SchemeNode{
	CString strSchemePath;
	__int64 nLayerCode;

	void operator=(const SchemeNode& schemeNode){
		this->strSchemePath = schemeNode.strSchemePath;
		this->nLayerCode = schemeNode.nLayerCode;
	}
	BOOL operator==(const SchemeNode& schemeNode)
	{
		if ((schemeNode.nLayerCode==nLayerCode)
			&&(schemeNode.strSchemePath==strSchemePath))
		{
			return TRUE;
		}
		return FALSE;
	}
};
class CSchemePropertyCopy: public CSchemeMerge  
{
public:
	CSchemePropertyCopy(CWnd* pParent = NULL,CConfigLibManager* pSrcCfgLibMan=NULL, CConfigLibManager* pDesCfgLibMan=NULL,int iDesScale=0,int iSrcScale=0, BOOL bIsDesTreeLoaded=FALSE);   // standard constructor
	virtual ~CSchemePropertyCopy();

public:
	//Overrided
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
public:
	//Common Methods
	BOOL ExchangeCtrlPos(UINT nCtrlID1,UINT nCtrlID2);
	BOOL MoveCtrl(UINT nCtrlID,int x, int y);
	BOOL HasXDefines(HTREEITEM hItem, CMergeTree& treeCtrl, ConfigLibItem config);
	__int64 GetMatchedCode(__int64 nSrcLayerCode, USERIDX& desUserIdx);	//在desUserIdx中搜索与nSrcLayerCode匹配的nDesLayerCode,并以返回值的形式给出。
	CString GetMatchedName(CString nSrcLayerName, USERIDX& desUserIdx);	//在desUserIdex中搜索与nSrcLayerName匹配的nDesLayerName,并以返回值的形式给出。
	CString GetLayerNamebyCoincidence(CString strLayerName,vector<CString>& strLayerNameArray); //在字符数组strLayerNameArray中查找与strLayerName中字符相同数最多的字符串。
	BOOL CopyPropstoDes(HTREEITEM hDesItem, HTREEITEM hSrcItem, BOOL bSpecial);//复制属性
	void SplitCodeandNameofLayer(__int64& nLayerCode, CString& strLayerName, CString strItemText);	
	void GotoNextNode(CMergeTree& srcTreeCtrl, CMergeTree& desTreeCtrl,BOOL bMatchedbyName=FALSE); //		跳到下一个节点。

	long GetElemIndex(__int64 nCode, CString strSchemePath, vector<SchemeNode>& nSchemeNodeArray);
	void ModifiedCtrlValue(UINT nCtrlID, LPCTSTR strNoteforPrompt);
	BOOL ClearSchemeInfo(CString strSchemePath, vector<SchemeNode>& schemeNodeArray); //2012.9.13
	std::wstring CstrtoWstr(CString str); //2012.9.14
	long GetCoincidenceNum(wstring wstrSrc, wstring wstrDes); //2012.9.14,获取两个宽字符串中的相同元素的个数。
	void RemoveSameElems(wstring& wstr); //2012.9.14，消除一个字符串中的重复元素。
	HTREEITEM LocateSpecificTreeItemofCopiedScheme(__int64 nLayerCode, CString strLayerName,UINT nCtrlID, BOOL byCode = TRUE); //2012.9.14,根据层码或层名来定位源、目方案的节点。
	BOOL DelProps(ConfigLibItem& config, CStringArray& treeItemTextArr);
	void RemoveSpecificElems(wstring& wstr, wstring wstrInvalidChars); // 2012.9.19,删除宽字符串中的指定字符。
	int FindinWstr(wstring wstr, wchar_t desWchar); //2012.9.19
	wchar_t* AnsiToUnicode( const char* szStr ); //2012.9.19
	void ClearGroupsofLayers(CStringArray& strGroupsandLayers,ConfigLibItem config);//2012.9.21,删除数组strGroupsandLayers中，其
protected:
	DECLARE_MESSAGE_MAP()
	//{{AFX_MSG(CSchemePropertyCopy)
 	afx_msg void OnBnClickedRadioByCode();
 	afx_msg void OnBnClickedRadioByName();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg void OnBnClickedButtonSkip();
	afx_msg void OnSelchangedTreeSrc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedTreeDes(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg	void OnSelChangeSrcScaleCombo();
	afx_msg void OnSelChangeDesScaleCombo();
	afx_msg void OnButtonSrcpath();
	afx_msg void OnButtonDespath();
	afx_msg void OnButtonDel() ;
	//}}AFX_MSG
	
public:
	//Common Var
	CEdit m_wndEditCopyedStatus; //复制操作的状态.eg:复制成功或复制失败或（其他）。
	CEdit m_wndEditSrcValStatus; //源方案中层扩展属性的状态。eg：无属性值。
	CEdit m_wndEditDesValStatus; //目标方案中层扩展属性的状态。eg：未修改or已修改。

	CString m_StrCopiedStatusPrompt ; //显示在复制状态只读框的文字
	CString m_strSrcXAttStatusPrompt; //显示在属性值只读框的文字
	
	CButton m_wndButtonCopy;	 //复制。
	CButton m_wndButtonSkip;	 //跳过。
	CWnd* m_pwndButtonDel;		 //删除。

	CButton m_wndRadiobyName;  //单选按钮,层名
	CButton m_wndRadiobyCode;	 //单选按钮,层码。
	RECT m_RectEditMergePath;
	CWnd* m_pWndEditCopyStatus;   //状态编辑框。
	CWnd* m_pWndEditSrcXAttStatus;  //源方案中拓展属性存在情况。
	CWnd* m_pWndEditDesXAttStatus;  //目标方案中拓展属性的存在情况。
	BOOL m_bMatchedbyLayerName;   //TRUE: 按层名匹配；FALSE:按层码匹配。
	BOOL m_bHasXDefines;			//层中是否有拓展属性值。
	vector<SchemeNode> m_srcLayersInfoCopyedXDefines; //源方案中已复制过扩展属性的层的层码和其所在方案的路径
	vector<SchemeNode> m_desLayersInfoCopyedXDefines; //目标方案中已被修改过扩展属性的层的层码和其所在方案的路径
	CString m_strInvalidChars ; //层名中的无效字符。
	wstring m_wstrInvalidChars; //转换为Unicode以后的无效字符。
	BOOL m_IsDesTreeLoadedforCopy; //2012.9.20
private:
	BOOL CreateCtrlsforPropCpy();	//添加并创建所需的按钮和编辑框控件。
	BOOL InitializeAppendedCtrls(); //初始化新添控件的位置。
	BOOL InitializePushButton();	//按钮控件的初始化。
	BOOL InitializeRadio();			//单选按钮的初始化。
	BOOL InitializeEditCtrl();		//编辑框的初始化。
	void RefreshSrcValStatusCtrl();	//更新源方案属性值编辑框的显示状态。
	void RefreshDesValStatusCtrl(); //更新目标方案属性值编辑框的显示状态。
	void ReLocateNodeofSrcTreeView(); //重新定位源目树中的节点。
	void ReLocateNodeofDesTreeView();  //重新定位目标书中的节点。
	void EnableBtnCtrl(BOOL bEnable); //点亮或变灰相应的按钮。
	
};

#endif // !defined(AFX_SCHEMECOPY_H__0DA5E389_3C5B_4726_852B_51629D797294__INCLUDED_)
