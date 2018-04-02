// VectorView_EX.h: interface for the CVectorView_EX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTORVIEW_EX_H__16CF36C7_BCCD_40A4_8FD2_532D364D17B2__INCLUDED_)
#define AFX_VECTORVIEW_EX_H__16CF36C7_BCCD_40A4_8FD2_532D364D17B2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "VectorView_new.h"


//≤‡ ”Õº
class CDlgDoc;
class CVectorView_EX : public CVectorView_new
{
protected: // create from serialization only
	CVectorView_EX();
	DECLARE_DYNCREATE(CVectorView_EX)

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVectorView_new)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVectorView_EX();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:	
	virtual void SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);

	void ConvertGrBufferToVectLayer(const GrBuffer *in, GrBuffer *out);

// Generated message map functions
protected:
	//{{AFX_MSG(CVectorView_new)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg	void OnViewReset();
	afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateManulLoadVect(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:

	double m_lfInitMatrix[16];		
};


#endif // !defined(AFX_VECTORVIEW_EX_H__16CF36C7_BCCD_40A4_8FD2_532D364D17B2__INCLUDED_)
