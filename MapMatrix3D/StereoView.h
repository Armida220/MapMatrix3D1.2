// StereoView.h: interface for the CStereoView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STEREOVIEW_H__83CBCC6D_5739_411D_8D71_47C2C76F2F9C__INCLUDED_)
#define AFX_STEREOVIEW_H__83CBCC6D_5739_411D_8D71_47C2C76F2F9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseView.h"
#include "DlgStretchImg.h "
#include "GeoBuilderPrj2.h "
#include "CoordCenter.h "
#include "RasterLayer.h"

// class CStereoView : public CBaseView  
// {
// public:
// 	CStereoView();
// 	virtual ~CStereoView();
// 
// };

#define TIMERID_AUTOMOVEIMAGE		2000

typedef struct
{
	PT_3D start;
	PT_3D end;
	double ang;
} LockLine; 

#define STEREOVIEW_INPUT_SYSMOUSE		1
#define STEREOVIEW_INPUT_3DMOUSE		2

struct ImgSaveData
{
public:
	ImgSaveData():rcSaveRectL(0,0,0,0),rcSaveRectR(0,0,0,0),
		nSaveRectType(0),pRectLData(0),pRectRData(0)
	{
	}
	CRect rcSaveRectL;
	CRect rcSaveRectR;
	int	  nSaveRectType;
	BYTE *pRectLData;
	BYTE *pRectRData;
	
	CDC dcMemRect;
	CBitmap bmpRect;
};

/*
 *	2008.01.17 �������·�������������ʽʵ���������Σ���ӽ���
 *  Ϊ�˼��ٶ��������ֵ�Ӱ�죬�����·�����ԭ���ķ����Ǻϲ������ģ�
 *  ͨ�� m_bTextMode �����ֵ�ǰ���õķ�ʽ��
 *  ��ԭ������ʾ������ȣ�����ģʽ��������ͼ�����²�ͬ��
 *  1��Ӱ�����ʾ����������ɣ�
 *  2������ʸ������ʾ������ DrawCustomLayer ���ƣ�
 *  2������ʸ����ʾ�õ� CXVVectorLayer ���� CXVElementBank ���������
 *  
 *  ������ʾ�������ַ�ʽ�����ֽ����˶���ʱ��ͨ��DrawCustomLayer��ʽ��
 *  ����˶���ʱ����Ȼ������ǰ�ķ�ʽ�������ַ�ʽͨ�� m_bMovableCursorInTextMode ���֣�
 *  �û�ʹ������ʱ�򣬴�ֵΪTRUE���û�ʹ�����ֽ��̵�ʱ�򣬴�ֵΪFALSE��
 */

class CStereoView : public CBaseView
{
	DECLARE_DYNCREATE(CStereoView)
protected:

	friend class CDlgDoc;
	//��ǰ���������ļ�·��
	CString m_strMdlImgFile[2];

	//��ǰ��������ID
	CString	m_strStereID;
 
	CRasterLayer m_ImgLayLeft;
	CRasterLayer m_ImgLayRight;

	CVariantDragLayer m_snapTipLayL;
	CVariantDragLayer m_snapTipLayR;

	//��ǰ����ά������Ĵ�����꣬
	//ֻ��������ʹ����ά��������ڴ�����������ʱ����
	PT_3D m_gCur3DMousePoint;

	//��ǰ��ϵͳ������Ĵ�����꣬�������������Ҫ����ϵͳ�����㣬
	//��Ϊsnap���ܻ�Ӱ����ҵ�Ĳ���(m_gCurPoint)��
	PT_3D m_gCurMousePoint;

	//��ǰ����ά������Ŀͻ������ϵͳ������Ŀͻ����꣬
	//�����������������Ϊ����ϵͳ������ά�����룬�ֲ�������ţ�
	PT_4D m_cCur3DMousePoint;
	PT_4D m_cCurMousePoint;

	//���Ĳ��ģʽ�µĲ�궨λ��
	CPoint m_ptCenterPoint;
	//�Ƿ�ʹ��������λ����Ϊ��궨λ�㣬�������ʹ�õ�ǰ�ͻ���������Ϊ��λ��
	BOOL m_bUseCenterPoint;

	//��ǰʹ���еĲ����������; 
	//STEREOVIEW_INPUT_SYSMOUSE, ϵͳ���; 
	//STEREOVIEW_INPUT_3DMOUSE, ��ά���
	int  m_nInputType;

	//��ǰ�Ĳ��ģʽ
	DWORD m_dwModeCoord;

	//�Ƿ�����ȷ������ת��
	BOOL m_bAutoMatch;
	BOOL m_bLoadConvert;
	int  m_nImageType;

	BOOL m_bLoadModel;
	BOOL m_bRealTimeEpipolar;

	//�Ƿ���Դ�ֱ�����Ӳ�
	BOOL m_bIgnoreVParallax;

	BOOL m_bStereoMidButton;

	CoreObject	m_coreObj;
	//����ת����
	CCoordCenter *m_pConvert;

	//Ӱ�񵽴������ķ���任ϵ��
	double m_trA[3],m_trB[3];
	double m_trZ;

	//�̲߳���
	double m_lfHeiStep;

	//�Ƿ���ʾʸ�����Ӳ�
	BOOL m_bViewVector;

	//Խ�羯��
	BOOL m_bOutAlert;

	//�Ҽ������˵��Ĵ��ھ��
	HWND m_hPopupWnd;
	
	//�Ƿ��ڲ���ƶ���ʱ������ʸ��
	BOOL m_bMoveDriveVector;
	
	//�Ƿ�������ƶ���ʱ������ʸ��
	BOOL m_bMouseDriveVector;

	//�Ƿ���������ϵͳ���ƶ���Ϣ
	BOOL m_bMoveFromSys;
	
	//�Ƿ���3D����ͼ��ʱ�����������Ϣ
	BOOL m_bDisableMouseIn3DDraw;
	BOOL m_b3DDraw;
	CPoint m_ptLastMouse;

	//�Ƿ��һ�μ�������ģ�ͣ��л�����ģ�͵�ʱ�򣬴�ֵΪ FALSE
	BOOL m_bFirstLoadModel;

	//Ӱ����ת���öԻ���
	CDlgStretchImg m_wndStretchDlg;

	BOOL m_bDriveOnlyCursor;

	BOOL m_bLockXY;

	// ֧������ά����ͼʱ�����Ƕ�
	BOOL m_bLockAngle;
	LockLine m_LockLine;

	int m_nDragLineWid;

	//TURE -> ������������
	BOOL m_bMouseWheelZoom;

	BOOL m_bFirstCheckStere;
protected:
	// �Ƿ���ʾ�ڽڵ�
	GrBuffer *m_pBufVariantDrag;
	GrBuffer *m_pBufConstDrag;
//	BOOL m_bShowKeyPoint;

	BOOL m_bMouseNoHeiMode;

protected:
	//ֱ����ͼ��Ӱ�����ر��ݣ�������ҧ�ϵ���ʾ������ҧ����ʾ����ʾ��

	BOOL	 m_bUseStereoEvlp;
	CArray<PT_3D,PT_3D>    m_arrStereoBoundPts_forDisplayTrim;

	//Ӱ��任���µı任����
	double m_lfStretchLMatrix[9], m_lfStretchRMatrix[9];

	//�任�����
	double m_lfStretchRLMatrix[9], m_lfStretchRRMatrix[9];

	BOOL    m_bAutoAdjustCoordMode;

	// �߳�����ģʽ���Ƿ�����ʹ�ý��̵����߳�
	BOOL m_bDeviceModifyZProcessing;
	BOOL m_bModifyheightBy3DMouse;

	// �Ƿ��Ե�ǰ���Ϊ��������
	BOOL m_bZoomWithCurrentMouse;

	// ɨ��ֱ���
	double m_lfScanSize;

	float m_fWheelSpeed;

public:	
	CCoordWnd GetCoordWnd();
	void SwitchModelForRefresh();

	CStereoView();
	virtual ~CStereoView();
	void DriveToXyz(PT_3D *pt, int coord_mode, BOOL bMoveImg = TRUE);
	virtual void StereoMove();

	void GetStereoBound(PT_3D* pts);
	
	PT_3D GetCrossPoint();
	CString	GetStereID();
	int GetImageType();
	BOOL GetRealTimeEpipolar();
protected:
	
	void ScrollForDrawBitmap(double dx,double dy,double dz = 0);
	virtual BOOL InitBmpLayer();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	
	virtual float ZoomSelect(float wantZoom);
	virtual void OnPreInnerCmd(UINT nInnerCmd);
	virtual void OnPostInnerCmd();
	
	virtual void ClientToImage(PT_4D *pt0, PT_4D *pt1);
	virtual void ImageToClient(PT_4D *pt0, PT_4D *pt1);
	virtual void GroundToImage(PT_3D *pt0, PT_4D *pt1);
	virtual void ImageToGround(PT_4D *pt0, PT_3D *pt1);

	virtual void GroundToVectorLay(PT_3D *pt0, PT_4D *pt1);

	void RotateCoordinate(double &x, double &y, const double *m);
//	BOOL Convert(Coordinate Input, Coordinate &Output);
	

	virtual void DrawAccuBox();
	virtual void EraseAccuBox();
	virtual void DrawAccuDrg();
	virtual void EraseAccuDrg();

	virtual void AddVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void AddConstDragLine(const GrBuffer *pBuf);

	virtual void UpdateConstDragLine();
	virtual void UpdateAccuDrawing();

	virtual void ClearAccuDrawing();
	virtual void ClearDragLine();

	virtual void InitUpdateLayersDepth();

	virtual void UpdateGridDrawing();

	virtual void AddObjtoVectLay(LONG_PTR handle, const GrBuffer *pBuf, BOOL bGround = TRUE, CVectorLayer *pLL = NULL, CVectorLayer *pLR = NULL);
	virtual void AddObjtoMarkVectLay(CFeature *pFtr);

	float GetActualImageParallax(float x, float y, int dir=0);
	BOOL LoadModel();
	
	
	void CheckCoordMode();
	BOOL IsDrawStreamline();
	void ModifyHeightWithUpdate(double dz);
	BOOL CheckStereoIsOpen(CString strStereID);
	void DisplayTrim(GrBuffer *pBuf);
	void UpdateVisible();
	
protected:
	BOOL Convert(Coordinate Input, Coordinate &Output);
	virtual BOOL ConvertFailed(Coordinate Input, Coordinate &Output);
	//���غ���
	virtual BOOL LoadDocData();
	virtual void ClearSnapDrawing();
	virtual void SetVariantDragLine(const GrBuffer *pBuf, BOOL bGround=TRUE);
	virtual void SetConstDragLine(const GrBuffer *pBuf);

	
	virtual void UpdateSelDrawing(BOOL bAlways=FALSE);
	virtual void UpdateVariantDragLine();
	virtual void UpdateSnapDrawing();

	virtual void HiliteObject(CFeature* pFtr);
	virtual void UnHiliteObject(CFeature* pFtr);
	
	virtual void DelObjfromVectLay(LONG_PTR handle, CVectorLayer *pLL = NULL, CVectorLayer *pLR = NULL);

	virtual void PushViewPos();
	virtual void ActiveViewPos(ViewPos* pos);
	
	CSize GetDimension(BOOL bLeft=TRUE);

	void AdjustCellScale();

	void UpdateButtonStates();

	//�ж������Ƿ���ܻ���˸��ԭ��ͨ���Ǳ���ס������GDI��ͼ������
	//�ú����������ж�ʸ�������Ƿ����崰����ס
	BOOL StereoMaybeFlicker();

	// �ƶ���ǰ���λ��Ӱ����Ļ����
	void AdjustViewWithCurrentMouse();

	void HideOutsideVertexes(GrBuffer2d *pBufL, GrBuffer2d *pBufR);

	//��������ģ�ͷŴ����
	void SetStereoMagnity(double change,UINT Mode,BOOL bToCenter = TRUE);
	//{{AFX_MSG(CStereoView)
	virtual void OnInitialUpdate();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	afx_msg void OnViewOverlay();
	afx_msg void OnHeightMode();
	afx_msg void OnHeightLock();
	afx_msg void OnAutoMode();
	afx_msg void OnManualMode();
	afx_msg void OnParaMode();
	afx_msg void OnShareMode();
	afx_msg void OnIndepMode();
	afx_msg void OnHParallaxLeft();
	afx_msg void OnHParallaxRight();
	afx_msg void OnVParallaxUp();
	afx_msg void OnVParallaxDown();
	afx_msg void OnHeightUpByStep();
	afx_msg void OnHeightDownByStep();
	afx_msg void OnHeightUp();
	afx_msg void OnHeightDown();
	afx_msg void OnSetXYZ();
	afx_msg void OnMoveCursorToCenter();
	afx_msg void OnMouseDrive();
	afx_msg void OnUpdateViewOverlay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHeightMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHeightLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAutoMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateParaMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateManualMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShareMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndepMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMouseDrive(CCmdUI* pCmdUI);
	afx_msg LRESULT OnDeviceLFootDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceMove2(WPARAM wParam, LPARAM lParam); //�ƶ���������Ӱ��
	afx_msg LRESULT OnDeviceRFootDown(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadModel(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	 
	afx_msg void OnHscrollLeft();
	afx_msg void OnHscrollRight();
	afx_msg void OnVscrollDown();
	afx_msg void OnVscrollUp();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnExportRaster();
	afx_msg void OnReset3DMouseParam();
	afx_msg void OnUpdateNoHatch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayOrder(CCmdUI* pCmdUI);
	afx_msg void OnShareSysMouse();
	afx_msg void OnUpdateShareSysMouse(CCmdUI* pCmdUI);
	afx_msg void OnReverseCrossColor();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void On3DMouseDriveCross();
//	afx_msg void OnUpdate3DMouseDriveCross(CCmdUI* pCmdUI);
	afx_msg void OnWsAsstereo();
	afx_msg void OnUpdateStereoFullscreen(CCmdUI* pCmdUI);
	afx_msg void OnStereoNohatch();
	afx_msg void OnStereoReverseDisplayorder();
	afx_msg void OnUpdateStereoViewoverlay(CCmdUI* pCmdUI);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseNoHeiMode();
	afx_msg void OnUpdateMouseNoHeiMode(CCmdUI* pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnCtrlPtsBound();
	afx_msg void OnMagnifyHalf();
	afx_msg void OnMagnifyOne();
	afx_msg void OnMagnifyOneAndHalf();
	afx_msg void OnMagnifyTwo();
	afx_msg void OnMagnifyTwoAndHalf();
	afx_msg void OnMagnifyThree();
	afx_msg void OnMakeCheckPtSample();

	//}}AFX_MSG
	afx_msg LRESULT OnClientToGround(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGroundToClient(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnModifyMousepoint(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMouseLeave();
	afx_msg void OnStereoFullScreen();
	afx_msg LRESULT UpdateViewParams(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT LockAngle(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	void ReadStereoBound_forDisplayTrim();
	void DefineAffineMatrix();

	void ManualLoadVect(Envelope evlp);

	//ʵʱ���ߵ����껻��
	BOOL RealtimeConvertCoordLeft(double x0, double y0, double &x1, double &y1, BOOL bOrder);
	BOOL RealtimeConvertCoordRight(double x0, double y0, double &x1, double &y1, BOOL bOrder);
};

#endif // !defined(AFX_STEREOVIEW_H__83CBCC6D_5739_411D_8D71_47C2C76F2F9C__INCLUDED_)
