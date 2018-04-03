#pragma once
#include "VectorView_new.h"


namespace EditBase
{
	class GrBuffer;
}

// COsgbVecView view

/**
* @brief COsgbVecView �̳�vecview������ֻ����Ϊһ��ָ��ָ��vecview
* @brief ��ҪĿ���Ǹ�дvecview�Ĺ����в��ʺ�osgb��ͼ�ĵط��������г�Ա����
* @brief �������Բ��Ķ���Ҫnew cosgbvecview�ĵط�����������������
*/
class COsgbVecView : public CVectorView_new
{
	friend class CVectorViewCenterPoint;
	friend class CDlgDoc;
	friend class COsgbView;

protected:
	/**
	* @brief mouseMv �޸�vecview������ƶ��¼�, ȥ�����²�׽��׼ȷ�ĵط�, ��������ƶ�����ʱ������
	*/
	void mouseMv(UINT nFlags, CPoint point);

	/**
	* @brief baseViewMv ȥ��baseview�в�׽������vecview�ֱ��ʵ�����
	*/
	void baseViewMv(UINT nFlags, CPoint point);

	/**
	* @brief interMouseMove ȥ��interMouseMove�в�׽������vecview�ֱ��ʵ�����
	*/
	void interMouseMove(UINT nFlags, CPoint point);

	/**
	* @brief UpdateSnapDrawing1 ��д�����ӻƿ�����⣬ ֪ͨ��ά��ͼ����׽���㣬���ò�׽��־λ
	*/
	void UpdateSnapDrawing1();
	
	/**
	* @brief ClearSnapDrawing1 ��д������ƿ�����⣬֪ͨ��ά��ͼ, û�в�׽���㣬���ò�׽��־λ
	*/
	void ClearSnapDrawing1();

	/**
	* @brief getOsgbView ��ȡosgbview��ָ��ĵ���, ɾ�����������ͷţ����в����õ���
	*/
	COsgbView* getInstanceOfOsgbView();

	/**
	* @brief getOsgbView ��ȡosgbview��ָ��
	*/
	COsgbView* getOsgbView();

	/**
	* @brief getGroundPt ��ȡʸ����ͼ�洢�ĵ��������
	*/
	PT_3D getGroundPt();

public:
	/**
	* @brief leftClk ��дvecview��������
	* @param pt ��ά�����꣬��osgbview�л�ȡ
	* @param screen2world ��Ļ������ľ���
	*/
	void leftClk(PT_3D *pt, double* screen2world);


	void leftDBClk(PT_3D *pt);

	/**
	* @brief rightClk ��дvecview������һ�
	* @param pt ��ά�����꣬��osgbview�л�ȡ
	*/
	void rightClk(PT_3D *pt);

	/**
	* @brief mouseMove ��дvecview������ƶ�
	* @param pt ��ά�����꣬��osgbview�л�ȡ
	*/
	void mouseMove(PT_3D *pt);

	/**
	* @brief changeCurPt �ı䵱ǰ������
	* @param pt ��ά�����꣬��osgbview�л�ȡ
	*/
	void changeCurPt(PT_3D *pt);

	/**
	* @brief isHasOsgView �Ƿ�����ʾosg��б��ģʽ
	*/
	bool isHasOsgView();

	/**
	* @brief getLinePick  �����ֱ��ײ���Ľ��
	* @param x y ƽ���ά��
	*/
	std::vector<double> getLinePick(const double x, const double y);

	/**
	* @brief AddObj  ��������
	* @param pFtr ����
	*/
	GrBuffer* AddObj(CFeature* pFtr);

	/**
	* @brief getDragLineClr  ��ȡ������ɫ
	*/
	COLORREF getDragLineClr();

	/**
	* @brief getIsSymbolize  �Ƿ���Ҫ���Ż�
	*/
	bool getIsSymbolize();

	/**
	* @brief getRenderBufs ���vector View�е�
	* @brief ������Ⱦ����
	*/
	std::vector<GrBuffer*> getRenderBufs();

	/**
	* @brief DriveToXyz ��ʸ����ͼƽ�Ƶ�ptλ��
	* @brief pt ��ptλ�ý���ƽ����ʾ
	*/
	void DriveToXyz(PT_3D *pt, BOOL bMoveImg = FALSE);

	/**
	* @brief adjustScale ���ݵ�ǰ��ͼ�ĳ߶Ƚ��жԱȣ�������ά��ͼ��
	* @brief ʹ�ú���ά��ͼ����һ��
	* @param x ��������x
	* @param y ��������y
	*/
	void adjustScale(double x, double y);

	/**
	* @brief zoomToScale ��ʸ����ͼ�Ŵ�һ������
	* @param x ��������x
	* @param y ��������y
	* @param scale �Ŵ�ı���
	*/
	void zoomToScale(double x, double y, double scale);

	/**
	* @brief changeShowImg �ı���ʾӰ��
	*/
	bool changeShowImg();

	bool isShowImg();

	bool isNoHatch();

	virtual void GroundToClient(PT_3D *pt0, PT_4D *pt1);

	void updataBound(double left, double right, double top, double bottom);
protected:
	DECLARE_MESSAGE_MAP()	

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	/**
	* @brief getScale ��ö�ά��ͼ�߶�
	*/
	double getScale();

private:

};


