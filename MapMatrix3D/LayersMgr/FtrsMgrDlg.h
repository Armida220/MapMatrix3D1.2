// FtrsMgrDlg.h : header file
//

#if !defined(AFX_FTRSMGRDLG_H__D2BC6C36_576F_436F_BCBE_BFEAACFBA6E7__INCLUDED_)
#define AFX_FTRSMGRDLG_H__D2BC6C36_576F_436F_BCBE_BFEAACFBA6E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CFtrsMgrDlg dialog

#include <afxcoll.h>
#include "resource.h"
#include "treectrl.h"
#include "PropList.h"
#include "EditBaseDoc.h"
#include "LayersMgrDlg.h"

class CFtrsMgrDlg : public CManageBaseDlg
{
// Construction
public:
	CFtrsMgrDlg(CWnd* pParent = NULL);	// standard constructor
	virtual void RButtonDown(UINT nFlags, CPoint point);

	virtual int GetManageType() { return ManageFtr; } 

	virtual DWORD_PTR GetCurSelItem() { return m_dCurSelItem; }

	virtual void UpdateStatusString();
// Dialog Data
	//{{AFX_DATA(CFtrsMgrDlg)
	enum { IDD = IDD_FTRSMGR_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFtrsMgrDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CFtrsMgrDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnNewFilter();
	afx_msg void OnUpdateNewLayer(CCmdUI *pCmdUI);
	afx_msg void OnRenameFilter();
	afx_msg void OnDeleteFilter();
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewLayer();
	afx_msg void OnUpdateDelLayer(CCmdUI *pCmdUI);
	afx_msg void OnDelLayer();
	afx_msg void OnAddLayerScheme();
	afx_msg void OnDelLayerScheme();
	afx_msg void OnShowTop();
	afx_msg void OnShowBottom();
	afx_msg void OnShowMove();
	afx_msg void OnShowInsert();
	afx_msg void OnSelectFtrs();
	afx_msg void OnDeSelectFtrs();
	//}}AFX_MSG
	afx_msg void OnUpdateSelectAllLayers(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeSelectLayers(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDelLayerObj(CCmdUI *pCmdUI);
	afx_msg void OnUpdateActiveLayer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDeleteFilter(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRenameFilter(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu); 
	afx_msg void OnDelLayerObj();
	afx_msg void OnActiveLayer();
	afx_msg void OnSelectAllLayers();
	afx_msg void OnDeSelectLayers();

	afx_msg void OnCutSelectLayers();
	afx_msg void OnUpdateCutSelectLayers(CCmdUI *pCmdUI);
	afx_msg void OnPasteLayer();
	afx_msg void OnUpdatePasteLayer(CCmdUI *pCmdUI);
	afx_msg void OnPasteLayerEx();
	afx_msg void OnUpdatePasteLayerEx(CCmdUI *pCmdUI);
	afx_msg void OnExit();
	afx_msg void OnResetDisplayOrder();
	afx_msg void OnResetDefalutLayerColor();
	afx_msg void OnLoadXAttributes();
	afx_msg void OnSelectAllLayersExceptCurSels();	
	afx_msg void OnUpdateSelectAllLayersExceptCurSels(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

private:
	BOOL CreateToolbar(CMFCToolBar& toolbar,UINT ID);
	void AdjustLayout();

	//��ʼ�������ؼ�
	BOOL InitCtrls();

	//�����
	void FillTree();

	//��ȫ��������б�,����Ϊ��ָ��ʱ��Ĭ��Ϊ������Դ
	void FillAllLayersList(CDlgDataSource *pDS = NULL,BOOL bForLoad = FALSE);

	//��ȫ������������б�,����Ϊ��ָ��ʱ��Ĭ��Ϊ������Դ
	void FillAllGroups(CDlgDataSource *pDS = NULL);
	//�ô��������Ĳ�����б�,pDSΪ��ָ��ʱĬ��������Դ
	void FillFilterLayersList(LPCTSTR filter ,CDlgDataSource *pDS = NULL, BOOL bOnlyUsedLayers=FALSE);

	//��ȫ����������б�,����Ϊ��ָ��ʱ��Ĭ��Ϊ������Դ
	void FillAllFeatures(LPCTSTR filter, CDlgDataSource *pDS = NULL);

	//�������в���Ϣ
	void SaveLayers();

	//ɾ��������
	void RemoveFilter(LPCTSTR name);

	//װ�غͱ��������
	void LoadFilterFile(const char *szFileName);
	void SaveFilterFile(const char *szFileName);
	
	//��ʾ���Ĺ���Ϊ�����зִ��ڵĹ��
	BOOL MouseCursorIsSplitter(CPoint pt);

	FilterItem* FindFilterItem(LPCTSTR name);

	int GetCol(CString field);
	void UpdateOrderValue();
	void UpdateColorValue();

	BOOL LoadXAttributes(CDlgDataSource *pDS);
	BOOL LoadUniqueXAttributesDefine();
public:
	//�������������
	virtual void SaveFilter(LPCTSTR name);

	BOOL IsFtrscolorByLayer(LONG_PTR dWord);
	BOOL IsFtrsManage();

	void ModifyAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc ,_variant_t exData);
	virtual void ModifyCheckState(CLVLPropItem **pItems, int num, CLVLPropColumn* pCol, int col, _variant_t value);
	virtual BOOL CanModifyCheckState();

	// �������
	void ModifyFtrsAttri(CLVLPropItem *pItem, CLVLPropColumn* Pc);
	void OnDelFtrs();

	CLVLTreeCtrl	m_wndTreeCtrl;			//����
	CLVLPropList	m_wndPropListCtrl;		//��ʾ�б�	
	DWORD_PTR			m_dCurSelItem;

	CStatic			m_wndStatus;

//	static int CreateLayerID();
private:
	BOOL			m_bIsCuttingObj;
	CArray<CFtrLayer*,CFtrLayer*>		m_arrCutLayers;
	CMFCToolBar	m_wndToolBarFilter;
	CImageList		m_ImageList;

	BOOL			m_bMouseInSplitter;			//�����Splitter��
	BOOL			m_bTrackSplitter;			//����������϶�����		
	
	int				m_iTreeCtrlWidth;			//��ǰ���ؼ��Ŀ��

	int				m_iMinTreeCtrlWidth;		//���ؼ���С���
	int				m_iMinListCtrlWidth;		//�б�ؼ���С���

	CDlgDoc*		m_pDlgDoc;
	CArray<FilterItem*,FilterItem*> m_arrFilters;	//������

	CFtrLayer *m_pMovedLayer;

	//CArray<LayerGroup,LayerGroup&> m_arrLayerGroups;

	CArray<FeatureItem,FeatureItem&> m_arrFtrItems;

	// ��չ���Խ�����ʾ
	CMap<XAttributeItem,XAttributeItem&,int,int> m_arrXAttibutes;

	// ��չ�����ڴ�ֵ
	CMap<CFeature*,CFeature*,CValueTableEx,CValueTableEx&> m_arrMemXAttr;

// public:
// 	static BOOL		m_bWarningAll;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FTRSMGRDLG_H__D2BC6C36_576F_436F_BCBE_BFEAACFBA6E7__INCLUDED_)
