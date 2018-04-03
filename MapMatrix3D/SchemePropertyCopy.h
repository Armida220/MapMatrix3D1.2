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

//2012.9.13,���ڼ�¼��ǰ���ƹ����Ե�Դ������Ŀ�귽���Ľڵ㡣
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
	__int64 GetMatchedCode(__int64 nSrcLayerCode, USERIDX& desUserIdx);	//��desUserIdx��������nSrcLayerCodeƥ���nDesLayerCode,���Է���ֵ����ʽ������
	CString GetMatchedName(CString nSrcLayerName, USERIDX& desUserIdx);	//��desUserIdex��������nSrcLayerNameƥ���nDesLayerName,���Է���ֵ����ʽ������
	CString GetLayerNamebyCoincidence(CString strLayerName,vector<CString>& strLayerNameArray); //���ַ�����strLayerNameArray�в�����strLayerName���ַ���ͬ�������ַ�����
	BOOL CopyPropstoDes(HTREEITEM hDesItem, HTREEITEM hSrcItem, BOOL bSpecial);//��������
	void SplitCodeandNameofLayer(__int64& nLayerCode, CString& strLayerName, CString strItemText);	
	void GotoNextNode(CMergeTree& srcTreeCtrl, CMergeTree& desTreeCtrl,BOOL bMatchedbyName=FALSE); //		������һ���ڵ㡣

	long GetElemIndex(__int64 nCode, CString strSchemePath, vector<SchemeNode>& nSchemeNodeArray);
	void ModifiedCtrlValue(UINT nCtrlID, LPCTSTR strNoteforPrompt);
	BOOL ClearSchemeInfo(CString strSchemePath, vector<SchemeNode>& schemeNodeArray); //2012.9.13
	std::wstring CstrtoWstr(CString str); //2012.9.14
	long GetCoincidenceNum(wstring wstrSrc, wstring wstrDes); //2012.9.14,��ȡ�������ַ����е���ͬԪ�صĸ�����
	void RemoveSameElems(wstring& wstr); //2012.9.14������һ���ַ����е��ظ�Ԫ�ء�
	HTREEITEM LocateSpecificTreeItemofCopiedScheme(__int64 nLayerCode, CString strLayerName,UINT nCtrlID, BOOL byCode = TRUE); //2012.9.14,���ݲ�����������λԴ��Ŀ�����Ľڵ㡣
	BOOL DelProps(ConfigLibItem& config, CStringArray& treeItemTextArr);
	void RemoveSpecificElems(wstring& wstr, wstring wstrInvalidChars); // 2012.9.19,ɾ�����ַ����е�ָ���ַ���
	int FindinWstr(wstring wstr, wchar_t desWchar); //2012.9.19
	wchar_t* AnsiToUnicode( const char* szStr ); //2012.9.19
	void ClearGroupsofLayers(CStringArray& strGroupsandLayers,ConfigLibItem config);//2012.9.21,ɾ������strGroupsandLayers�У���
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
	CEdit m_wndEditCopyedStatus; //���Ʋ�����״̬.eg:���Ƴɹ�����ʧ�ܻ���������
	CEdit m_wndEditSrcValStatus; //Դ�����в���չ���Ե�״̬��eg��������ֵ��
	CEdit m_wndEditDesValStatus; //Ŀ�귽���в���չ���Ե�״̬��eg��δ�޸�or���޸ġ�

	CString m_StrCopiedStatusPrompt ; //��ʾ�ڸ���״ֻ̬���������
	CString m_strSrcXAttStatusPrompt; //��ʾ������ֵֻ���������
	
	CButton m_wndButtonCopy;	 //���ơ�
	CButton m_wndButtonSkip;	 //������
	CWnd* m_pwndButtonDel;		 //ɾ����

	CButton m_wndRadiobyName;  //��ѡ��ť,����
	CButton m_wndRadiobyCode;	 //��ѡ��ť,���롣
	RECT m_RectEditMergePath;
	CWnd* m_pWndEditCopyStatus;   //״̬�༭��
	CWnd* m_pWndEditSrcXAttStatus;  //Դ��������չ���Դ��������
	CWnd* m_pWndEditDesXAttStatus;  //Ŀ�귽������չ���ԵĴ��������
	BOOL m_bMatchedbyLayerName;   //TRUE: ������ƥ�䣻FALSE:������ƥ�䡣
	BOOL m_bHasXDefines;			//�����Ƿ�����չ����ֵ��
	vector<SchemeNode> m_srcLayersInfoCopyedXDefines; //Դ�������Ѹ��ƹ���չ���ԵĲ�Ĳ���������ڷ�����·��
	vector<SchemeNode> m_desLayersInfoCopyedXDefines; //Ŀ�귽�����ѱ��޸Ĺ���չ���ԵĲ�Ĳ���������ڷ�����·��
	CString m_strInvalidChars ; //�����е���Ч�ַ���
	wstring m_wstrInvalidChars; //ת��ΪUnicode�Ժ����Ч�ַ���
	BOOL m_IsDesTreeLoadedforCopy; //2012.9.20
private:
	BOOL CreateCtrlsforPropCpy();	//��Ӳ���������İ�ť�ͱ༭��ؼ���
	BOOL InitializeAppendedCtrls(); //��ʼ������ؼ���λ�á�
	BOOL InitializePushButton();	//��ť�ؼ��ĳ�ʼ����
	BOOL InitializeRadio();			//��ѡ��ť�ĳ�ʼ����
	BOOL InitializeEditCtrl();		//�༭��ĳ�ʼ����
	void RefreshSrcValStatusCtrl();	//����Դ��������ֵ�༭�����ʾ״̬��
	void RefreshDesValStatusCtrl(); //����Ŀ�귽������ֵ�༭�����ʾ״̬��
	void ReLocateNodeofSrcTreeView(); //���¶�λԴĿ���еĽڵ㡣
	void ReLocateNodeofDesTreeView();  //���¶�λĿ�����еĽڵ㡣
	void EnableBtnCtrl(BOOL bEnable); //����������Ӧ�İ�ť��
	
};

#endif // !defined(AFX_SCHEMECOPY_H__0DA5E389_3C5B_4726_852B_51629D797294__INCLUDED_)
