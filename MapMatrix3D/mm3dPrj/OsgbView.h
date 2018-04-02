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
* brief stGrBuffer  保存buf和其他需要渲染的信息的结构体
*/
struct stGrBuffer
{
	stGrBuffer(std::string idStr, GrBuffer* buf,
		COLORREF rgb, bool bIsLocal) : mIdStr(idStr),
		mBuf(buf), mRgb(rgb), mbIsLocal(bIsLocal)
	{
	}

	/*
	* brief idStr id字符串
	*/
	std::string mIdStr;

	/*
	* brief buf 渲染缓冲区
	*/
	GrBuffer* mBuf;

	/*
	* brief rgb 颜色
	*/
	COLORREF mRgb;

	/*
	* brief bIsLocal 是否是本图
	*/
	bool mbIsLocal;
};

typedef std::vector<stGrBuffer> VEC_BUFFER;
struct wmID_h;
typedef std::map<std::string, wmID_h> sid_wmid;
/*
*	2017.05.4 将倾斜摄影测图加入featureone, 基于
*	osgb进行三维量测
*/
class COsgbView : public CBaseView
{
	DECLARE_DYNCREATE(COsgbView)
protected:
	friend class CDlgDoc;
	friend class CFtrParser;
public:
	/**
	* @brief showAbsOriDlg 显示绝对定向对话框
	*/
	virtual void showAbsOriDlg();

	/**
	* @brief hideAbsOriDlg 隐藏绝对定向对话框
	*/
	virtual void hideAbsOriDlg();

	/**
	* @brief getAbsOriDlg 获取绝对定向对话框
	*/
	std::shared_ptr<CMyAbsOriDlg> getAbsOriDlg() { return mAbsOriDlg; }

	/**
	* @brief getScale 获得三维视图的尺度
	*/
	virtual double getScale();

	/**
	* @brief getScaleFromDist 根据距离获得三维视图的尺度
	*/
	virtual double getScaleFromDist();
	/**
	* @brief initOsgScene 初始化osgb场景
	* @brief fileName 初始化osgb文件
	*/
	virtual void initOsgScene(std::string fileName);

	/**
	* @brief isLinePick 通过垂直碰撞检测，是否能够获得点
	* @brief x y 平面二维点进行垂直碰撞检测
	*/
	virtual bool isLinePick(const double x, const double y) const;

	/**
	* @brief getLinePick 获得垂直碰撞检测的三维点
	* @brief x y 平面二维点进行垂直碰撞检测
	*/
	std::vector<double> getLinePick(const double x, const double y) const;

	/**
	* @brief GetDocument 获取文档指针
	*/
	CDlgDoc* GetDocument(); // non-debug version is inline

	/**
	* @brief isSelect vecView中，是否有feature被选中选中
	*/
	bool isSelect();

	/**
	* @brief setSelect 设置标志位
	*/
	void setSelect(const bool bisSelect);

	/**
	* @brief getSelDist 返回选择的范围
	*/
	double getSelDist();

	/**
	* @brief getOsgbExtent 获取osgb范围
	*/
	Envelope getOsgbExtent();

	/**
	* @brief OnUpdate 激发视图事件
	* @param pSender  事件发送者
	* @param lHint    事件参数
	* @param pHint    事件代码
	*/
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	/**
	* @brief changeImgOri 改变参考点位置
	* @param imgName  需要修改影像名
	* @param x		  需要修改影像坐标x
	* @param y		  需要修改影像坐标y
	*/
	virtual void changeFakePt(std::string imgName, double x, double y);

	/**
	* @brief reCalFront 重新计算前方交汇
	* @param imgName  需要修改影像名
	* @param x		  需要修改影像坐标x
	* @param y		  需要修改影像坐标y
	*/
	virtual void reCalFront(std::string imgName, double x, double y);

	/**
	* @brief bkSpcaeLastOps 回退上一步操作
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
	* @brief OnDraw 重写渲染函数，在这里进行三维渲染
	*/
	virtual void OnDraw(CDC* pDC);

	/**
	* @brief LoadDocData 加载全部数据到视图，在视图初始化时，视图自己调用
	*/
	virtual BOOL LoadDocData();

	/**
	* @brief OnInitialUpdate 更新初始化，初始化浏览器
	*/
	virtual void OnInitialUpdate();

	/**
	* @brief createCursor 创建光标
	*/
	//virtual HCURSOR createCursor(int type);

	/**
	* @brief CreateCursorFromBitmap 通过bitmap创建光标
	*/
// 	HCURSOR CreateCursorFromBitmap(HBITMAP hSourceBitmap,
// 		COLORREF clrTransparent, DWORD xHotspot, DWORD yHotspot);

	/**
	* @brief GetMaskBitmaps 获取bitmaps
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
	* @brief addScene  加载场景
	* @param strModel  向场景中加入模型的路径名
	*/
	virtual void addScene(const std::string &strModel) const;

	/**
	* @brief removeAllScene  清除场景中的所有数据
	*/
	virtual void removeAllScene() const;

	/**
	* @brief setNoDragCur  设置无拖动光标
	*/
	virtual void setNoDragCur();

	/**
	* @brief setDragCur  设置拖动光标
	*/
	virtual void setDragCur();

	/**
	* @brief SetCursorType  设置光标的类型
	*/
	virtual void SetCursorType(long type);

	/**
	* @brief addObj  往三维视图增加物体
	* @param pFtr  往场景中增加的特征
	*/
	virtual void addObj(CFeature* pFtr);

	/**
	* @brief removeObj  往三维视图删除物体
	* @param pFtr  往场景中增加的特征
	*/
	virtual void removeObj(CFeature* pFtr);

	/**
	* @brief synchronize  让vecView的数据和三维的数据进行同步
	*/
	virtual int synchronize();

	/**
	* @brief synHighLigh  让vecView的数据和三维的数据进行高亮同步，只对高亮数据部分进行同步
	*/
	virtual void synHighLigh();

	/**
	* @brief SetConstDragLine  让vecView的数据和三维的事件辅助标识进行同步
	* @brief pBuf 辅助标识的数据
	*/
	virtual void SetConstDragLine(const GrBuffer *pBuf);

	/**
	* @brief AddConstDragLine  增加辅助标识到三维里面
	* @brief pBuf 辅助标识的数据
	*/
	virtual void AddConstDragLine(const GrBuffer *pBuf);

	/**
	* @brief SetConstDragLine  让vecView的数据和三维的事件的动态线数据进行同步
	* @brief pBuf 动态线数据的数据
	*/
	virtual void SetVariantDragLine(const GrBuffer *pBuf);

	/**
	* @brief AddVariantDragLine  增加动态线数据到三维里面
	* @brief pBuf 动态线数据的数据
	*/
	virtual void AddVariantDragLine(const GrBuffer *pBuf);

	/**
	* @brief UpdatesnapDraw 更新捕捉的标识盒子
	* @brief pBuf 捕捉的标识盒子的数据
	*/
	virtual void UpdatesnapDraw(const GrBuffer * pBuf);

	/**
	* @brief addGraph 添加tin构网
	* @brief pBuf 构网的数据
	*/
	virtual void addGraph(const GrBuffer* pBuf);

	/**
	* @brief delGraph 删除tin构网
	*/
	virtual void delGraph();

	/**
	* @brief ClearDragLine  清除辅助线和动态线
	*/
	virtual void ClearDragLine();

	/**
	* @brief driveTo 模型平移到（x, y)位置
	* @param cx x方向的位置
	* @param cy y方向的位置
	*/
	virtual void driveTo(double cx, double cy);

	/**
	* @brief panTo 模型平移到（x, y)位置, 相机角度不变
	* @param cx x方向的位置
	* @param cy y方向的位置
	*/
	virtual void panTo(double cx, double cy);

	/**
	* @brief zoomTo 模型缩放, 相机角度不变
	* @param scale 某个尺度
	*/
	virtual void zoomTo(double scale);

	/**
	* @brief parseVecFtr 解析datasource中的数据集成为osg支持的形式
	*/
	std::vector<osgCall::osgFtr> parseVecFtr();

	/**
	* @brief parseSymbolBuf 解析datasource中的数据集并将其符号化
	*/
//	VEC_BUFFER parseSymbolBuf();

	/**
	* @brief makeCellwithCoord根据符号库的图元，和偏移量的点进行设置，
	* @brief 生成具有三维真实坐标的图元
	* @param trans 图元与真实位置的偏移
	* @param scale 图元缩放比例
	*/
	GrBuffer makeCellwithCoord(const CellDef &cell, const PT_3D &trans, const double scale);

	/*
	* @brief getCurLayerColor 获取当前层的颜色
	*/
	PT_3D getCurLayerColor();

	/*
	* @brief getDragLineClr 获取动线的颜色
	*/
	PT_3D getDragLineClr();

	/*
	* @brief updateSnap 更新捕捉框
	*/
	void updateSnap(const SNAPITEM* t0);

	/*
	* @brief selectImgByPt 根据点信息选择影像
	* @param pt 三维点坐标信息
	* @return 返回选中影像的id号的容器
	*/
	virtual std::vector<COri> selectImgByPt(PT_3D *pt);

	/*
	* @brief getInitRect 获取初始的区域范围
	*/
	virtual void getInitRect();

	/*
	* @brief updateVisibal 更新显示情况
	*/
	void updateLayerVisibal(const UpdateDispyParam * udp);

	void updateObjectVisibal(const UpdateDispyParam * udp);
private:
	/**
	* @brief ary_filename 用string队列存放文件的路径 
	*/
	std::deque<std::string> ary_filename;

	CString str_Tips;
	CString str_On;
	CString str_Off;
	std::vector<CString> vec_tips;
	/**
	* @brief openFileFlag 判断是否已经打开过文件
	*/
	BOOL openFileFlag;
	
	std::set<std::string> m_hideIDS;

	sid_wmid m_map_featerID_WhiteModelID;

	bool isHide(const std::string & id);

	void hideAll(const std::vector<osgCall::osgFtr> & vecFtr);
	void viewAll(const std::vector<osgCall::osgFtr> & vecFtr);
	void synData(std::vector<osgCall::osgFtr> & vecFtr);
	/**
	* @brief mThreadHandle 渲染线程句柄
	*/
	HANDLE mThreadHandle;

	/**
	* @brief IMfcViewer 三维浏览器接口
	*/
	std::shared_ptr<mm3dView::IOsgMfcViewer> IViewer;

	/**
	* @brief bisSel 根据document中的onselectstate函数，回调设置这个值,用来判断vecview是否有东西被选中了
	*/
	bool bisSel;

	/**
	* @brief mCur 鼠标形状文件 
	*/
	//CCursorFile mCur;

	//选择光标
	//HCURSOR m_hSelCursor;

	//测图光标
	//HCURSOR m_hMapCursor;

	//编辑光标
	//HCURSOR m_hEditCursor;

	//拖动光标
	HCURSOR m_hHandCursor;

	//当前测标
//	HCURSOR m_hCurCursor;

	/**
	* @brief 信息发送器, 主要用于解决三维和二维渲染多线程的冲突
	*/
	std::shared_ptr<CMsgSender> pMsgSender;

	/**
	* @brief 特征解析器, 可以把feature one中的特征数据解析成osg支持的
	*/
	std::shared_ptr<CFtrParser> pFtrParser;

	/**
	* @brief 显示对话框
	*/
	//std::shared_ptr<CShowImgDlg> showImgDlg;

	/**
	* @brief osgview的框架在windows上的范围
	*/
	CRect osgFrameRect;

	/**
	* @brief vecview的框架在windows上的范围
	*/
	CRect vecFrameRect;

	/**
	* @brief 影像的刺点成果
	*/
	std::vector<COri> vecImgOri;

	/**
	* @brief 鼠标是否在窗口标志位
	*/
	bool mouseIn;

	int hintState;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	/**
	* @brief OnLoadScene 加载场景
	*/
	afx_msg LRESULT OnLoadScene(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnRmAllScene 清除场景
	*/
	afx_msg LRESULT OnRmAllScene(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnVecViewLfClk 通知vecView 开启进行左击事件
	*/
	afx_msg LRESULT OnVecViewLfClk(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnVecViewMouseMv 通知vecView 开启进行鼠标移动事件
	*/
	afx_msg LRESULT OnVecViewMouseMv(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnVecViewRightClk 通知vecView 开启进行鼠标右键点击事件
	*/
	afx_msg LRESULT OnVecViewRightClk(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnPromptLockMsg 提示已经上锁了
	*/
	afx_msg LRESULT OnPromptLockMsg(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnLButtonDown 重写鼠标左击响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	/**
	* @brief OnMouseMove 重写鼠标移动响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	/**
	* @brief OnMouseMove 重写鼠标左击响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	/**
	* @brief OnMouseMove 重写鼠标左击响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown 重写鼠标右击响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown 重写鼠标右击响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown 重写鼠标中键响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	/**
	* @brief OnRButtonDown 重写鼠标中键响应，为了鼠标可以设置成指定形状
	*/
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	/**
	* @brief OnKeyDown 重写按键事件，把按键消息发送到矢量视图
	*/
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	/**
	* @brief OnPaint 重写绘制事件，sceneview原来的绘制与osgb显示有冲突
	*/
	afx_msg void OnPaint();

	/**
	* @brief OnTimer 重写计时事件，sceneview原来的绘制使用了该事件
	*/
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	/**
	* @brief OnSize 重写onsize事件，sceneview原来的绘制使用了该事件
	*/
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);

	/**
	* @brief OnSize 顶视图切换
	*/
	afx_msg LRESULT OnTopView(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnLeftView 左视图切换
	*/
	afx_msg LRESULT OnLeftView(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnRightView 右视图切换
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
	* @brief PreTranslateMessage 增加home键回家功能，按空格改变数值
	*/
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	/**
	* @brief ViewAll 显示所有，osgb场景的所有物体显示，并通知vecview显示
	*/
	virtual void ViewAll();

	/**
	* @brief ViewHideAll 隐藏所有，osgb场景的所有物体隐藏，并通知vecview隐藏
	*/
	virtual void ViewHideAll();

	/**
	* @brief ViewHideAll 显示外部，osgb场景的所有外部物体显示，并通知vecview
	*/
	virtual void ViewExternal();

	/**
	* @brief ViewLocal 显示本地，osgb场景的所有本地物体显示，并通知vecview
	*/
	virtual void ViewLocal();

	COsgbVecView* posgVectorView;

	/**
	* @brief mAbsOriDlg 绝对定向对话框
	*/
	std::shared_ptr<CMyAbsOriDlg> mAbsOriDlg;
public:
	/**
	* @brief getOsgbVecView 获取osgvecview的指针，采用静态转换，强行把vecview转换为osgbvecview
	*/
	COsgbVecView* getOsgbVecView();

	/**
	* @brief hideImgDlg 隐藏图片对话框
	*/
	virtual void hideImgDlg();
protected:

	/**
	* @brief isOsgbViewActive 当前窗口是否是激活状态
	*/
	bool isOsgbViewActive();

	/**
	* @brief reFrontIntersect 重新计算前方交汇成果
	* @param type 类型
	*/
	virtual void reFrontIntersect(int type);

	/**
	* @brief initSwImgDlg 显示图片对话框
	*/
	virtual void initSwImgDlg();

	/**
	* @brief swImgDlg 显示图片对话框
	*/
	virtual void swImgDlg();



	/**
	* @brief adjustViewToOrigin 恢复视图到原始,两个视图联动
	*/
	virtual void adjustViewToOrigin();

	/**
	* @brief adjustViewToOrigin 调整视图可以容纳显示影像的视图
	*/
	virtual void adjustViewToShowImg();

	/**
	* @brief OnPromptCatch 让输出窗口提示捕捉内角点的开启状态
	*/
	afx_msg LRESULT OnPromptCatch(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnBack 回退上一个点
	*/
	afx_msg LRESULT OnBack(WPARAM wParam, LPARAM lParam);
	
	/**
	* @brief OnPromptOpera 提示操作切换
	*/
	afx_msg LRESULT OnPromptOpera(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnPanXy 移动操作
	*/
	afx_msg LRESULT OnPanXy(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnZoomTo 缩放操作
	*/
	afx_msg LRESULT OnZoomTo(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnShowImg 显示图片
	*/
	afx_msg LRESULT OnShowImg(WPARAM wParam, LPARAM lParam);

	/**
	* @brief OnChangeCurpt 改变当前点坐标
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


