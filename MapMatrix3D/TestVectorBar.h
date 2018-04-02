// TestVectorBar.h: interface for the CTestVectorBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTVECTORBAR_H__10C58218_6A00_49FE_9EF2_AED6324CAF3A__INCLUDED_)
#define AFX_TESTVECTORBAR_H__10C58218_6A00_49FE_9EF2_AED6324CAF3A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MyPropList.h  "
#include "DlgSearchCheckPt.h" //hcw,2013.1.14
extern HWND g_hwnd;//hcw,2013.1.14
//##ModelId=41466B7F009B
class CMyToolBar : public CMFCToolBar
{
	//##ModelId=41466B7F009D
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
	{		
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*)GetOwner(), bDisableIfNoHndler);
	}
	virtual BOOL AllowShowOnList () const		{	return FALSE;	}

};

class CDlgDoc;


class CTestVectorBar : public CDockablePane  
{	
	struct RefPtToTestPtItem
	{
		RefPtToTestPtItem()
		{
			memset(Id,0,sizeof(Id));
			lfRefX = lfRefY = lfRefZ = 0;
			lfTestX = lfTestY = lfTestZ = 0;
			bFinsh = FALSE;
		}
		char Id[16];
		double lfRefX,lfRefY,lfRefZ;
		double lfTestX,lfTestY,lfTestZ;
		BOOL bFinsh;
	};
public:
	CTestVectorBar();
	virtual ~CTestVectorBar();
	void ChangeDlgDoc(CDlgDoc *pDoc);
	BOOL GetFeatureId(CString &strId);
	BOOL SetCurActiveItem(double x,double y,double z);
	void AdjustLayout ();
	BOOL IsFinishCurActiveItem();
	void RefreshActiveItem();
	void Popup();
	virtual BOOL PreTranslateMessage(MSG* pMsg);					//hcw,2013.1.8
	void GetListColumnArr(MyList::CLVLPropList0 &wndPropListCtrl, CStringArray &strArr, int iCol);//hcw,2013.1.14
	int GetSelectedItem(MyList::CLVLPropList0 &wndPropListCtrl);	//hcw,2013.1.14,??¦Ì?¨¢D¡À¨ª¦Ì?¦Ì¡À?¡ã???D???¡ê
	void SelectNextSameItem(BOOL bIncrement, int nListIndex, int nCol);  //hcw,2013.1.15,???¡Â2¡é???D??¨°???¨®?nListIndex?¨²¦Ì¨²iCol¨¦??¨¤¨ª?¦Ì????¡ê
	int FindPropItem (MyList::CLVLPropList0 &wndPropListCtrl, CString str, int nCol, int nPos, 
						 BOOL bIncrement=TRUE, BOOL bIncludednPosItem=TRUE); //hcw,2013.1.14.¡ä¨®ID=nPos?????a¨º????¡Â¡ê?
protected:
	CDlgDoc *m_pDoc;
	RefPtToTestPtItem* m_pActiveItem;
	MyList::CLVLPropItem0* m_pCurItem;
//	CStatic     m_wndStatic;
	CMyToolBar   m_wndToolBar;
	MyList::CLVLPropList0  m_wndPropListCtrl;
    CStringArray m_strArrColumninCheckList; //hcw,2013.1.8,?¨¬2¨¦¦Ì????¨¨¡¤????D?3¨°?¨¢D¨ºy?Y?¡ê
	CTypedPtrArray<CPtrArray,RefPtToTestPtItem*> m_arrDataItems;
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler)	
	{		
		CDockablePane::OnUpdateCmdUI (pTarget, bDisableIfNoHndler);
	}
	BOOL  m_bClosedBeforePopup, m_bAutoHideBeforePopup;
	void ClearData();
private:
//	BOOL AddItem(RefPtToTestPtItem* pItem);
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDrawTestPt();
	afx_msg void OnDrawTestPtUpdateUI(CCmdUI* pCmdUI);
	afx_msg void OnImportRefTestPt();
	afx_msg void OnTestEvalue();
	afx_msg void OnUpdateData();
	afx_msg void OnPaint();
	afx_msg LRESULT OnSelChange(WPARAM   wParam,LPARAM   lParam);
	afx_msg void OnDelAllTestPt();
	afx_msg void OnSearchTestPt();//hcw,2013.1.8,???¡Â?¨¬2¨¦¦Ì??¡ê
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_TESTVECTORBAR_H__10C58218_6A00_49FE_9EF2_AED6324CAF3A__INCLUDED_)
