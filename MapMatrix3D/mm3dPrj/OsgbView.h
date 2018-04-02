#pragma once
#include <deque>
#include <string>
#include <memory>
#include <set>
#include "BaseView.h"
#include "CursorFile.h"

// COsgbView view
#define HINT_SETCROSS 1
#define HINT_SETOSGCROSS 2
namespace mm3dView
{
	class IOsgMfcViewer;
}

namespace osgCall
{
	class ICallBack;
	class osgFtr;
}

namespace EditBase
{
	class GrBuffer;
}

class CMsgSender;
class CGeomParser;
class CFtrParser;
class COsgbVecView;
class COri;
class CShowImgDlg;
class CMyAbsOriDlg;
/*
* brief stGrBuffer  ����buf��������Ҫ��Ⱦ����Ϣ�Ľṹ��
*/
struct stGrBuffer
{
	stGrBuffer(std::string idStr, GrBuffer* buf,
		COLORREF rgb, bool bIsLocal) : mIdStr(idStr),
		mBuf(buf), mRgb(rgb), mbIsLocal(bIsLocal)
	{
	}

	/*
	* brief idStr id�ַ���
	*/
	std::string mIdStr;

	/*
	* brief buf ��Ⱦ������
	*/
	GrBuffer* mBuf;

	/*
	* brief rgb ��ɫ
	*/
	COLORREF mRgb;

	/*
	* brief bIsLocal �Ƿ��Ǳ�ͼ
	*/
	bool mbIsLocal;
};

typedef std::vector<stGrBuffer> VEC_BUFFER;
struct wmID_h;
typedef std::map<std::string, wmID_h> sid_wmid;
/*
*	2017.05.4 ����б��Ӱ��ͼ����featureone, ����
*	osgb������ά����
*/
class COsgbView : public CBaseView
{
	DECLARE_DYNCREATE(COsgbView)
protected:
	friend class CDlgDoc;
	friend class CFtrParser;
public:
	/**
	* @brief showAbsOriDlg ��ʾ���Զ���Ի���
	*/
	virtual void showAbsOriDlg();

	/**
	* @brief hideAbsOriDlg ���ؾ��Զ���Ի���
	*/
	virtual void hideAbsOriDlg();

	/**
	* @brief getAbsOriDlg ��ȡ���Զ���Ի���
	*/
	std::shared_ptr<CMyAbsOriDlg> getAbsOriDlg() { return mAbsOriDlg; }

	/**
	* @brief getScale �����ά��ͼ�ĳ߶�
	*/
	virtual double getScale();

	/**
	* @brief getScaleFromDist ���ݾ�������ά��ͼ�ĳ߶�
	*/
	virtual double getScaleFromDist();
	/**
	* @brief initOsgScene ��ʼ��osgb����
	* @brief fileName ��ʼ��osgb�ļ�
	*/
	virtual void initOsgScene(std::string fileName);

	/**
	* @brief isLinePick ͨ����ֱ��ײ��⣬�Ƿ��ܹ���õ�
	* @brief x y ƽ���ά����д�ֱ��ײ���
	*/
	virtual bool isLinePick(const double x, const double y) const;

	/**
	* @brief getLinePick ��ô�ֱ��ײ������ά��
	* @brief x y ƽ���ά����д�ֱ��ײ���
	*/
	std::vector<double> getLinePick(const double x, const double y) const;

	/**
	* @brief GetDocument ��ȡ�ĵ�ָ��
	*/
	CDlgDoc* GetDocument(); // non-debug version is inline

	/**
	* @brief isSelect vecView�У��Ƿ���feature��ѡ��ѡ��
	*/
	bool isSelect();

	/**
	* @brief setSelect ���ñ�־λ
	*/
	void setSelect(const bool bisSelect);

	/**
	* @brief getSelDist ����ѡ��ķ�Χ
	*/
	double getSelDist();

	/**
	* @brief getOsgbExtent ��ȡosgb��Χ
	*/
	Envelope getOsgbExtent();

	/**
	* @brief OnUpdate ������ͼ�¼�
	* @param pSender  �¼�������
	* @param lHint    �¼�����
	* @param pHint    �¼�����
	*/
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	/**
	* @brief changeImgOri �ı�ο���λ��
	* @param imgName  ��Ҫ�޸�Ӱ����
	* @param x		  ��Ҫ�޸�Ӱ������x
	* @param y		  ��Ҫ�޸�Ӱ������y
	*/
	virtual void changeFakePt(std::string imgName, double x, double y);

	/**
	* @brief reCalFront ���¼���ǰ������
	* @param imgName  ��Ҫ�޸�Ӱ����
	* @param x		  ��Ҫ�޸�Ӱ������x
	* @param y		  ��Ҫ�޸�Ӱ������y
	*/
	virtual void reCalFront(std::string imgName, double x, double y);

	/**
	* @brief bkSpcaeLastOps ������һ������
	*/
	virtual void bkSpcaeLastOps(/*int*/);

	bool MouseIn() const{ return mouseIn; }

	void UpdateImgVisible();
protected:
	COsgbView();           // protected constructor used by dynamic creation
	virtual ~COsgbView();

	bool bBoundSet;
	bool bIsFit;
	Envelope m_scenceBound;

	void getScenceBound();
	bool isFit();
	
	bool bCatchCorner;
	bool bLockH;
	bool bNeedSetLockH;
	bool bShowImg;
	bool bSkipOnceLeftClick;

	bool m_bViewVector;

protected:
	/**
	* @brief OnDraw ��д��Ⱦ�����������������ά��Ⱦ
	*/
	virtual void OnDraw(CDC* pDC);

	/**
	* @brief LoadDocData ����ȫ�����ݵ���ͼ������ͼ��ʼ��ʱ����ͼ�Լ�����
	*/
	virtual BOOL LoadDocData();

	/**
	* @brief OnInitialUpdate ���³�ʼ������ʼ�������
	*/
	virtual void OnInitialUpdate();

	/**
	* @brief createCursor �������
	*/
	//virtual HCURSOR createCursor(int type);

	/**
	* @brief CreateCursorFromBitmap ͨ��bitmap�������
	*/
// 	HCURSOR CreateCursorFromBitmap(HBITMAP hSourceBitmap,
// 		COLORREF clrTransparent, DWORD xHotspot, DWORD yHotspot);

	/**
	* @brief GetMaskBitmaps ��ȡbitmaps
	*/
// 	void GetMaskBitmaps(HBITMAP hSourceBitmap, COLORREF clrTransparent,
// 		HBITMAP &hAndMaskBitmap, HBITMAP &hXorMaskBitmap);


#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

protected:
	/**
	* @brief addScene  ���س���
	* @param strModel  �򳡾��м���ģ�͵�·����
	*/
	virtual void addScene(const std::string &strModel) const;

	/**
	* @brief removeAllScene  ��������е���������
	*/
	virtual void removeAllScene() const;

	/**
	* @brief setNoDragCur  �������϶����
	*/
	virtual void setNoDragCur();

	/**
	* @brief setDragCur  �����϶����
	*/
	virtual void setDragCur();

	/**
	* @brief SetCursorType  ���ù�������
	*/
	virtual void SetCursorType(long type);

	/**
	* @brief addObj  ����ά��ͼ��������
	* @param pFtr  �����������ӵ�����
	*/
	virtual void addObj(CFeature* pFtr);

	/**
	* @brief removeObj  ����ά��ͼɾ������
	* @param pFtr  �����������ӵ�����
	*/
	virtual void removeObj(CFeature* pFtr);

	/**
	* @brief synchronize  ��vecView�����ݺ���ά�����ݽ���ͬ��
	*/
	virtual int synchronize();

	/**
	* @brief synHighLigh  ��vecView�����ݺ���ά�����ݽ��и���ͬ����ֻ�Ը������ݲ��ֽ���ͬ��
	*/
	virtual void synHighLigh();

	/**
	* @brief SetConstDragLine  ��vecView�����ݺ���ά���¼�������ʶ����ͬ��
	* @brief pBuf ������ʶ������
	*/
	virtual void SetConstDragLine(const GrBuffer *pBuf);

	/**
	* @brief AddConstDragLine  ���Ӹ�����ʶ����ά����
	* @brief pBuf ������ʶ������
	*/
	virtual void AddConstDragLine(const GrBuffer *pBuf);

	/**
	* @brief SetConstDragLine  ��vecView�����ݺ���ά���¼��Ķ�̬�����ݽ���ͬ��
	* @brief pBuf ��̬�����ݵ�����
	*/
	virtual void SetVariantDragLine(const GrBuffer *pBuf);

	/**
	* @brief AddVariantDragLine  ���Ӷ�̬�����ݵ���ά����
	* @brief pBuf ��̬�����ݵ�����
	*/
	virtual void AddVariantDragLine(const GrBuffer *pBuf);

	/**
	* @brief UpdatesnapDraw ���²�׽�ı�ʶ����
	* @brief pBuf ��׽�ı�ʶ���ӵ�����
	*/
	virtual void UpdatesnapDraw(const GrBuffer * pBuf);

	/**
	* @brief addGraph ���tin����
	* @brief pBuf ����������
	*/
	virtual void addGraph(const GrBuffer* pBuf);

	/**
	* @brief delGraph ɾ��tin����
	*/
	virtual void delGraph();

	/**
	* @brief ClearDragLine  ��������ߺͶ�̬��
	*/
	virtual void ClearDragLine();

	/**
	* @brief driveTo ģ��ƽ�Ƶ���x, y)λ��
	* @param cx x�����λ��
	* @param cy y�����λ��
	*/
	virtual void driveTo(double cx, double cy);

	/**
	* @brief panTo ģ��ƽ�Ƶ���x, y)λ��, ����ǶȲ���
	* @param cx x�����λ��
	* @param cy y�����λ��
	*/
	virtual void panTo(double cx, double cy);

	/**
	* @brief zoomTo ģ������, ����ǶȲ���
	* @param scale ĳ���߶�
	*/
	virtual void zoomTo(double scale);

	/**
	* @brief parseVecFtr ����datasource�е����ݼ���Ϊosg֧�ֵ���ʽ
	*/
	std::vector<osgCall::osgFtr> parseVecFtr();

	/**
	* @brief parseSymbolBuf ����datasource�е����ݼ���������Ż�
	*/
//	VEC_BUFFER parseSymbolBuf();

	/**
	* @brief makeCellwithCoord���ݷ��ſ��ͼԪ����ƫ�����ĵ�������ã�
	* @brief ���ɾ�����ά��ʵ�����ͼԪ
	* @param trans ͼԪ����ʵλ�õ�ƫ��
	* @param scale ͼԪ���ű���
	*/
	GrBuffer makeCellwithCoord(const CellDef &cell, const PT_3D &trans, const double scale);

	/*
	* @brief getCurLayerColor ��ȡ��ǰ�����ɫ
	*/
	PT_3D getCurLayerColor();

	/*
	* @brief getDragLineClr ��ȡ���ߵ���ɫ
	*/
	PT_3D getDragLineClr();

	/*
	* @brief updateSnap ���²�׽��
	*/
	void updateSnap(const SNAPITEM* t0);

	/*
	* @brief selectImgByPt ���ݵ���Ϣѡ��Ӱ��
	* @param pt ��ά��������Ϣ
	* @return ����ѡ��Ӱ���id�ŵ�����
	*/
	virtual std::vector<COri> selectImgByPt(PT_3D *pt);

	/*
	* @brief getInitRect ��ȡ��ʼ������Χ
	*/
	virtual void getInitRect();

	/*
	* @brief updateVisibal ������ʾ���
	*/
	void updateLayerVisibal(const UpdateDispyParam * udp);

	void updateObjectVisibal(const UpdateDispyParam * udp);
private:
	/**
	* @brief ary_filename ��string���д���ļ���·�� 
	*/
	std::deque<std::string> ary_filename;

	CString str_Tips;
	CString str_On;
	CString str_Off;
	std::vector<CString> vec_tips;
	/**
	* @brief openFileFlag �ж��Ƿ��Ѿ��򿪹��ļ�
	*/
	BOOL openFileFlag;
	
	std::set<std::string> m_hideIDS;

	sid_wmid m_map_featerID_WhiteModelID;

	bool isHide(const std::string & id);

	void hideAll(const std::vector<osgCall::osgFtr> & vecFtr);
	void viewAll(const std::vector<osgCall::osgFtr> & vecFtr);
	void synData(std::vector<osgCall::osgFtr> & vecFtr);
	/**
	* @brief mThreadHandle ��Ⱦ�߳̾��
	*/
	HANDLE mThreadHandle;

	/**
	* @brief IMfcViewer ��ά������ӿ�
	*/
	std::shared_ptr<mm3dView::IOsgMfcViewer> IViewer;

	/**
	* @brief bisSel ����document�е�onselectstate�������ص��������ֵ,�����ж�vecview�Ƿ��ж�����ѡ����
	*/
	bool bisSel;

	/**
	* @brief mCur �����״�ļ� 
	*/
	//CCursorFile mCur;

	//ѡ����
	//HCURSOR m_hSelCursor;

	//��ͼ���
	//HCURSOR m_hMapCursor;

	//�༭���
	//HCURSOR m_hEditCursor;

	//�϶����
	HCURSOR m_hHandCursor;

	//��ǰ���
//	HCURSOR m_hCurCursor;

	/**
	* @brief ��Ϣ������, ��Ҫ���ڽ����ά�Ͷ�ά��Ⱦ���̵߳ĳ�ͻ
	*/
	std::shared_ptr<CMsgSender> pMsgSender;

	/**
	* @brief ����������, ���԰�feature one�е��������ݽ�����osg֧�ֵ�
	*/
	std::shared_ptr<CFtrParser> pFtrParser;

	/**
	* @brief ��ʾ�Ի���
	*/
	//std::shared_ptr<CShowImgDlg> showImgDlg;

	/**
	* @brief osgview�Ŀ����windows�ϵķ�Χ
	*/
	CRect osgFrameRect;

	/**
	* @brief vecview�Ŀ����windows�ϵķ�Χ
	*/
	CRect vecFrameRect;

	/**
	* @brief Ӱ��Ĵ̵�ɹ�
	*/
	std::vector<COri> vecImgOri;

	/**
	* @brief ����Ƿ��ڴ��ڱ�־λ
	*/
	bool mouseIn;

	int hintState;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	/**
	* @brief OnLoadScene ���س���
	*/
	afx_msg LRESULT OnLoadScene(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnRmAllScene �������
	*/
	afx_msg LRESULT OnRmAllScene(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnVecViewLfClk ֪ͨvecView ������������¼�
	*/
	afx_msg LRESULT OnVecViewLfClk(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnVecViewMouseMv ֪ͨvecView ������������ƶ��¼�
	*/
	afx_msg LRESULT OnVecViewMouseMv(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnVecViewRightClk ֪ͨvecView ������������Ҽ�����¼�
	*/
	afx_msg LRESULT OnVecViewRightClk(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnPromptLockMsg ��ʾ�Ѿ�������
	*/
	afx_msg LRESULT OnPromptLockMsg(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnLButtonDown ��д��������Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/**
	* @brief OnMouseMove ��д����ƶ���Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/**
	* @brief OnMouseMove ��д��������Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	/**
	* @brief OnMouseMove ��д��������Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown ��д����һ���Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown ��д����һ���Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown ��д����м���Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown ��д����м���Ӧ��Ϊ�����������ó�ָ����״
	*/
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	/**
	* @brief OnKeyDown ��д�����¼����Ѱ�����Ϣ���͵�ʸ����ͼ
	*/
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	/**
	* @brief OnPaint ��д�����¼���sceneviewԭ���Ļ�����osgb��ʾ�г�ͻ
	*/
	afx_msg void OnPaint();

	/**
	* @brief OnTimer ��д��ʱ�¼���sceneviewԭ���Ļ���ʹ���˸��¼�
	*/
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	/**
	* @brief OnSize ��дonsize�¼���sceneviewԭ���Ļ���ʹ���˸��¼�
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);

	/**
	* @brief OnSize ����ͼ�л�
	*/
	afx_msg LRESULT OnTopView(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnLeftView ����ͼ�л�
	*/
	afx_msg LRESULT OnLeftView(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnRightView ����ͼ�л�
	*/
	afx_msg LRESULT OnRightView(WPARAM wParam, LPARAM lParam);


	afx_msg void OnTopView  ();
	afx_msg void OnLeftView ();
	afx_msg void OnRightView();
	afx_msg void OnChangeShowImage();
	afx_msg void OnHome();
	afx_msg void OnShiftOperation();
	afx_msg void OnLockHeight(); 
	afx_msg void OnCatchCorner(); 
protected:

	void lockHeight(int x, int y);

	/**
	* @brief PreTranslateMessage ����home���ؼҹ��ܣ����ո�ı���ֵ
	*/
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/**
	* @brief ViewAll ��ʾ���У�osgb����������������ʾ����֪ͨvecview��ʾ
	*/
	virtual void ViewAll();

	/**
	* @brief ViewHideAll �������У�osgb�����������������أ���֪ͨvecview����
	*/
	virtual void ViewHideAll();

	/**
	* @brief ViewHideAll ��ʾ�ⲿ��osgb�����������ⲿ������ʾ����֪ͨvecview
	*/
	virtual void ViewExternal();

	/**
	* @brief ViewLocal ��ʾ���أ�osgb���������б���������ʾ����֪ͨvecview
	*/
	virtual void ViewLocal();

	COsgbVecView* posgVectorView;

	/**
	* @brief mAbsOriDlg ���Զ���Ի���
	*/
	std::shared_ptr<CMyAbsOriDlg> mAbsOriDlg;
public:
	/**
	* @brief getOsgbVecView ��ȡosgvecview��ָ�룬���þ�̬ת����ǿ�а�vecviewת��Ϊosgbvecview
	*/
	COsgbVecView* getOsgbVecView();

	/**
	* @brief hideImgDlg ����ͼƬ�Ի���
	*/
	virtual void hideImgDlg();
protected:

	/**
	* @brief isOsgbViewActive ��ǰ�����Ƿ��Ǽ���״̬
	*/
	bool isOsgbViewActive();

	/**
	* @brief reFrontIntersect ���¼���ǰ������ɹ�
	* @param type ����
	*/
	virtual void reFrontIntersect(int type);

	/**
	* @brief initSwImgDlg ��ʾͼƬ�Ի���
	*/
	virtual void initSwImgDlg();

	/**
	* @brief swImgDlg ��ʾͼƬ�Ի���
	*/
	virtual void swImgDlg();



	/**
	* @brief adjustViewToOrigin �ָ���ͼ��ԭʼ,������ͼ����
	*/
	virtual void adjustViewToOrigin();

	/**
	* @brief adjustViewToOrigin ������ͼ����������ʾӰ�����ͼ
	*/
	virtual void adjustViewToShowImg();

	/**
	* @brief OnPromptCatch �����������ʾ��׽�ڽǵ�Ŀ���״̬
	*/
	afx_msg LRESULT OnPromptCatch(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnBack ������һ����
	*/
	afx_msg LRESULT OnBack(WPARAM wParam, LPARAM lParam);
	
	/**
	* @brief OnPromptOpera ��ʾ�����л�
	*/
	afx_msg LRESULT OnPromptOpera(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnPanXy �ƶ�����
	*/
	afx_msg LRESULT OnPanXy(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnZoomTo ���Ų���
	*/
	afx_msg LRESULT OnZoomTo(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnShowImg ��ʾͼƬ
	*/
	afx_msg LRESULT OnShowImg(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnChangeCurpt �ı䵱ǰ������
	*/
	afx_msg LRESULT OnChangeCurpt(WPARAM wParam, LPARAM lParam);


	afx_msg LRESULT OnLeftBtDoubleClick(WPARAM wParam, LPARAM lParam);

	
public:
	afx_msg void OnMouseLeave();

	virtual void GroundToClient(PT_3D *pt0, PT_4D *pt1);;

	void whiteModel(const FTR_HANDLE * pftr, int num, double h, bool badd);

	afx_msg void OnWhiteModel();

	afx_msg void OnViewoverlay();

private:
	void OutputMessage(LPCTSTR str);
};


