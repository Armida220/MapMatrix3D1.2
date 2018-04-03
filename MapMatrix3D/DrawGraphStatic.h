#if !defined(AFX_DRAWGRAPHSTATIC_H__B85BD878_7BEA_4029_BD72_D5310631EBED__INCLUDED_)
#define AFX_DRAWGRAPHSTATIC_H__B85BD878_7BEA_4029_BD72_D5310631EBED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DrawGraphStatic.h : header file
//
#include "GeoBuilderPrj2.h "
#include "MruFile.h"
/////////////////////////////////////////////////////////////////////////////
// CDrawGraphStatic window

class CDrawGraphStatic : public CStatic
{
	struct ModelScope 
	{
		ModelScope(){ }
		ModelScope(PT_2D pp1,PT_2D pp2,PT_2D pp3,PT_2D pp4,int idx):p1(pp1),p2(pp2),p3(pp3),p4(pp4),index(idx){}
		void GetRect(double *minX,double *minY,double *maxX,double *maxY)
		{
			double x1,y1,x2,y2;
			
			//�õ�һ�������������Ի�����
			x1=p1.x;
			x2=p1.x;
			y1=p1.y;
			y2=p1.y;
			x1=min(x1,p2.x);    
			x2=max(x2,p2.x);    
			y1=min(y1,p2.y);    
			y2=max(y2,p2.y);
			x1=min(x1,p3.x);    
			x2=max(x2,p3.x);    
			y1=min(y1,p3.y);    
			y2=max(y2,p3.y); 
			x1=min(x1,p4.x);    
			x2=max(x2,p4.x);    
			y1=min(y1,p4.y);    
			y2=max(y2,p4.y); 
			
			*minX=x1;*maxX=x2;*minY=y1;*maxY=y2; //��ֵ������ַ����
		}
		 PT_2D p1,p2,p3,p4;
		 int index;
	};
	enum
	{
		modeZoomOut,
		modeZoomIn,
		modeDrag,
		modeDragDisplay,
		modeWholeScreen,
		modeSwitch,
		modeAutoSwitch
	};
// Construction
public:
	CDrawGraphStatic();

// Attributes
public:
	BOOL Create(LPCTSTR lpszText, DWORD dwStyle,const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawGraphStatic)
	protected:
		virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO *pTI) const;
		
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTolerForStereoMove(double toler);
	void SetNearstMSFlag(bool bIsExist);
	void SwitchModel();
	void SetCurrentMS(int index);
	int FindMS(const CString & str);
	void ClearMSArray();
	bool GetNameForNearestMS(CString *str);
	void RefreshView();
	void SetMsArrayFlag(bool bIsExist);
	void SetVecWinFlag(bool bIsExist);
	void SetCurStereoWinFlag(bool bIsExist);
	void SetCurMSFlag(bool bExist);
 	void SetCoreObj( CoreObject *pcoreObj);
	void SetWorldOrg(double x,double y);
	void SetLogicOrg(double x,double y);

	void ShowWholeMap();

	void SetCurrentVecWin(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);
	void SetCurrentStereoWin(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);
	void SetCurrentMS(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);
	void SetNearestMS(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4);	
	
	void ZoomIn(/*ModelScope *ms*/);
	void ZoomOut(/*ModelScope *ms*/);
	void SetButtonRect(const CRect & rect);
	void SetDrawArea(const CRect &rect);
	void AddModelScope(PT_2D p1,PT_2D p2,PT_2D p3,PT_2D p4,int index);
	virtual ~CDrawGraphStatic();
	void OnOpenDocimage(int index);
	// Generated message map functions
	void CalcCoreModelScope();

	/**
	* @brief getCoreObj ��ô洢���ڹ�����Ϣ�Ķ���
	*/
	CoreObject getCoreObj();

protected:
	double m_lfTolerForStereoMove;
	void SwitchToNearest();
	void GeoToDev(PT_2D *pt);

	bool IsEqualForMS(const ModelScope &ms,const ModelScope &ms1 );
	bool GetNearestMsToPt(ModelScope *ms,CPoint point);   //�õ�����point�����ģ�ͷ�Χ��������������ҵ�����ڴ�ģ�ͷ�Χ֮�ڣ�ʧ���򷵻�0���ɹ�����1.
														  //ע����ģ�ͷ�Χ�Ѿ�������ת��
	void DrawScope(CDC *pDC,const ModelScope &ms);
	void GeoToDev( ModelScope *ms);
	
	bool GetRect(double* m_Xmin, double* m_Ymin, double * m_Xmax, double * m_Ymax);
	int PtInButton(CPoint pt) const;
	CRect GetButtonRect(int mode) const;
	

	//{{AFX_MSG(CDrawGraphStatic)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPopupOpenPhotoMs();
	afx_msg void OnPopupOpenEpipMs();
	afx_msg void OnPopupSwitchEpip();
	afx_msg void OnPopupRealtimeEpip();
	afx_msg void OnOpenDocimage1();
	afx_msg void OnOpenDocimage2();
	afx_msg void OnOpenDocimage3();
	afx_msg void OnOpenDocimage4();
	afx_msg void OnOpenDocimage5();
	afx_msg void OnOpenDocimage6();
	afx_msg void OnOpenDocimage7();
	afx_msg void OnOpenDocimage8();
	afx_msg void OnOpenDocimage9();
	afx_msg void OnOpenDocimage10();
	afx_msg void OnOpenDocimage11();
	afx_msg void OnOpenDocimage12();
	afx_msg void OnOpenDocimage13();
	afx_msg void OnOpenDocimage14();
	afx_msg void OnOpenDocimage15();
	afx_msg void OnOpenDocimage16();
	afx_msg void OnMruClr();
	afx_msg void OnReCalcModelBound();
	afx_msg void OnUpdateOpenEpip(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpenPhoto(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:

	
	ModelScope m_msNearestMSForRightClick;     //��¼���Ҽ����ʱ������ڵ�ģ�ͷ�Χ
	CoreObject m_coreObj;
	bool m_bIsOldPtInMS;
	ModelScope m_msOldMSForHighlight;
	double m_lfMsOrgWorldY;
	double m_lfMsOrgWorldX;
	double m_lfMsOrgLogicY;
	double m_lfMsOrgLogicX;
	double m_lfRatioForCoorTrans;
	CPoint m_ptEnd;										//����������ʾͼ
	bool m_bIsDragDisplay;								//��־�Ƿ�������ʾ
	bool m_bIsFirstClickInDrawArea;						//�����ƶ�ͼ��������ʾͼ
	CPoint m_ptStartForDrag;
	
	bool m_bIsAutoSwitch;								 //�Ƿ��Զ��л�ģ��
	bool m_bIsDragForMouse;                              //����Ƿ��϶�������������ʾ�е���ʱ��Ļ滭
	bool m_bIsDrag;                                      //��־�Ƿ��϶�ͼ
	int m_nRatioForZoom;                                 //�Ŵ���С�ı���

	bool m_bIsExistForMSArr;                             //��־ģ�ͷ�Χ�����Ƿ����
	bool m_bIsExistForCurMS;							 //��־�Ƿ���ڵ�ǰ����ģ�ͷ�Χ
	bool m_bIsExistForMsNearest;                         //��־���ٽ���ģ���Ƿ����
	bool m_bIsExistForStereoWin;
	bool m_bIsExistForVecWin;                            //��־ʸ�����ڷ�Χ�Ƿ����
	CRect m_rectDrawArea;                                //��ͼ����
	CRect m_rectButton;									 //���߰�ťλͼ�ķ�Χ
	CArray<ModelScope,ModelScope> m_arrModelScopes;		 //ģ�ͷ�Χ��
	CArray<ModelScope,ModelScope> m_arrModelScopes_core;		 //ģ�ͷ�Χ�ߵĺ������򣬱��罫ԭģ�ͷ�Χ��С1/4�õ�������
	ModelScope m_msNearestModel;								 //���ڽ�ģ�ͷ�Χ
	ModelScope m_msCurrentModel;						 //��ǰ����ģ�ͷ�Χ
	ModelScope m_msCurrentStereoWindow;						 //��ǰ���崰�ڷ�Χ
	ModelScope m_msCurrentVectorScope;					 //��ǰʸ����Χ

	CMruFile	m_CMruFile;		//����ļ��˵�����
	CString		m_exePath;		//ģ�鵱ǰ·��

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAWGRAPHSTATIC_H__B85BD878_7BEA_4029_BD72_D5310631EBED__INCLUDED_)
