#pragma once
#include "VectorView_new.h"


namespace EditBase
{
	class GrBuffer;
}

// COsgbVecView view

/**
* @brief COsgbVecView 继承vecview，但是只能作为一个指针指向vecview
* @brief 主要目的是改写vecview的功能中不适合osgb测图的地方，不能有成员变量
* @brief 这样可以不改动需要new cosgbvecview的地方，满足陈震提的需求
*/
class COsgbVecView : public CVectorView_new
{
	friend class CVectorViewCenterPoint;
	friend class CDlgDoc;
	friend class COsgbView;

protected:
	/**
	* @brief mouseMv 修改vecview的鼠标移动事件, 去掉导致捕捉不准确的地方, 解决动线移动不及时的问题
	*/
	void mouseMv(UINT nFlags, CPoint point);

	/**
	* @brief baseViewMv 去除baseview中捕捉依赖于vecview分辨率的问题
	*/
	void baseViewMv(UINT nFlags, CPoint point);

	/**
	* @brief interMouseMove 去除interMouseMove中捕捉依赖于vecview分辨率的问题
	*/
	void interMouseMove(UINT nFlags, CPoint point);

	/**
	* @brief UpdateSnapDrawing1 重写，增加黄框的问题， 通知三维视图，捕捉到点，设置捕捉标志位
	*/
	void UpdateSnapDrawing1();
	
	/**
	* @brief ClearSnapDrawing1 重写，清除黄框的问题，通知三维视图, 没有捕捉到点，设置捕捉标志位
	*/
	void ClearSnapDrawing1();

	/**
	* @brief getOsgbView 获取osgbview的指针的单例, 删除后单例不会释放，所有不能用单例
	*/
	COsgbView* getInstanceOfOsgbView();

	/**
	* @brief getOsgbView 获取osgbview的指针
	*/
	COsgbView* getOsgbView();

	/**
	* @brief getGroundPt 获取矢量视图存储的地面点坐标
	*/
	PT_3D getGroundPt();

public:
	/**
	* @brief leftClk 重写vecview的鼠标左击
	* @param pt 三维点坐标，从osgbview中获取
	* @param screen2world 屏幕到世界的矩阵
	*/
	void leftClk(PT_3D *pt, double* screen2world);


	void leftDBClk(PT_3D *pt);

	/**
	* @brief rightClk 重写vecview的鼠标右击
	* @param pt 三维点坐标，从osgbview中获取
	*/
	void rightClk(PT_3D *pt);

	/**
	* @brief mouseMove 重写vecview的鼠标移动
	* @param pt 三维点坐标，从osgbview中获取
	*/
	void mouseMove(PT_3D *pt);

	/**
	* @brief changeCurPt 改变当前点坐标
	* @param pt 三维点坐标，从osgbview中获取
	*/
	void changeCurPt(PT_3D *pt);

	/**
	* @brief isHasOsgView 是否在显示osg倾斜的模式
	*/
	bool isHasOsgView();

	/**
	* @brief getLinePick  获得竖直碰撞检测的结果
	* @param x y 平面二维点
	*/
	std::vector<double> getLinePick(const double x, const double y);

	/**
	* @brief AddObj  增加物体
	* @param pFtr 特征
	*/
	GrBuffer* AddObj(CFeature* pFtr);

	/**
	* @brief getDragLineClr  获取动线颜色
	*/
	COLORREF getDragLineClr();

	/**
	* @brief getIsSymbolize  是否需要符号化
	*/
	bool getIsSymbolize();

	/**
	* @brief getRenderBufs 获得vector View中的
	* @brief 额外渲染数据
	*/
	std::vector<GrBuffer*> getRenderBufs();

	/**
	* @brief DriveToXyz 把矢量视图平移到pt位置
	* @brief pt 按pt位置进行平移显示
	*/
	void DriveToXyz(PT_3D *pt, BOOL bMoveImg = FALSE);

	/**
	* @brief adjustScale 根据当前视图的尺度进行对比，调整二维视图，
	* @brief 使得和三维视图比例一致
	* @param x 缩放中心x
	* @param y 缩放中心y
	*/
	void adjustScale(double x, double y);

	/**
	* @brief zoomToScale 把矢量视图放大到一定比例
	* @param x 缩放中心x
	* @param y 缩放中心y
	* @param scale 放大的倍率
	*/
	void zoomToScale(double x, double y, double scale);

	/**
	* @brief changeShowImg 改变显示影像
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
	* @brief getScale 获得二维视图尺度
	*/
	double getScale();

private:

};


