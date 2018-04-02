#pragma once
#include "IViewer.h"
#include <osgViewer/Viewer>
#include <osg/Node>
#include <osgGA/TerrainManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <string>
#include <deque>
#include "seldefViewer.h"
#include "SelfDefTerrainManipulator.h"

namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class CBaseUserEvent;
	class IOsgEvent;
}

namespace osgCall
{
	class ICallBack;
}

namespace mm3dView
{
	class COsgModel;
	class CSelDefViewer;
	class CSelfStateManipulator;
	class COsgSyn;

	/**
	* @brief cOSG 在mfc框架中进行三维渲染的浏览器
	*/

	class MFCVIEWERAPI cOSG : public IOsgMfcViewer
	{
	public:
		cOSG(HWND hWnd);
		~cOSG();

		/**
		* @brief getScale 根据高度获得尺度
		*/
		virtual double getScale();

		/**
		* @brief getScaleFromDist 根据距离获得尺度,适用于绘制半径
		*/
		virtual double getScaleFromDist();

		/**
		* @brief getScrren2WorldMat 获得osg从屏幕到世界的矩阵
		*/
		virtual double* getScrren2WorldMat();

		/**
		* @brief InitOSG 初始化osg浏览器
		* @param modelname 初始化的场景名字
		*/
		virtual void InitOSG(std::string modelname);

		/**
		* @brief StartRendering 开始进行渲染，启动渲染线程
		*/
		virtual HANDLE StartRendering();

		/**
		* @brief addScene  添加文件到场景中
		* @param strModel  模型路径名
		*/
		virtual void addScene(const std::string &strModel);

		/**
		* @brief removeAllScene  清除场景中所有数据
		*/
		virtual void removeAllScene();

		/**
		* @brief SetSceneExtent 设置场景范围
		*/
		virtual void SetSceneExtent();

		/**
		* @brief home 回家
		*/
		virtual void home();

		/**
		* @brief light 启动光源
		*/
		virtual void light();

		virtual bool shiftOperation();
		
		virtual bool shiftCatchCorner();

		virtual bool lockHeight(int x, int y);

		virtual bool updataFtrView(bool bShow);

		/**
		* @brief getViewer 获取浏览器指针
		*/
		inline osgViewer::Viewer* getViewer()
		{
			return mViewer;
		}

		/**
		* @brief setCall 设置回调
		* @param ic  回调
		*/
		virtual void setCall(const std::shared_ptr<osgCall::ICallBack> &ic);

		/**
		* @brief driveTo 移动到中心
		* @brief x y 平面二维点移动
		*/
		virtual void driveTo(const double x, const double y);

		/**
		* @brief panTo 平移到点（x,y),过程中相机角度不变
		* @brief x y 平面二维点移动
		*/
		virtual void panTo(const double x, const double y);

		/**
		* @brief zoomTo 缩放到来自二维的同一尺度,过程中相机角度不变
		* @brief scale 来自二维的同一尺度
		*/
		virtual void zoomTo(const double scale);

		/**
		* @brief isLinePick 通过垂直碰撞检测，是否能够获得点
		* @brief x y 平面二维点进行垂直碰撞检测
		*/
		virtual bool isLinePick(const double x, const double y);

		/**
		* @brief addObj 对三维的数据进行增加数据
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addObj(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief removeObj 对三维的数据进行清除数据
		* @param vecFtr 矢量特征的容器
		*/
		virtual void removeObj(const std::vector<osgCall::osgFtr> &vecFtr);
		
		virtual void removeObj(const std::vector<std::string> &vecID);

		/**
		* @brief synData 对三维的数据和vecview进行同步
		* @param vecFtr 矢量特征的容器
		*/
		virtual void synData(const std::vector<osgCall::osgFtr> &vecFtr);


		/**
		* @brief updataDate 对三维的数据和vecview进行更新
		* @param vecFtr 矢量特征的容器
		*/
		virtual void updataDate(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief synHighLightData 对三维的数据和vecview进行高亮同步
		* @param vecHighFtr 矢量特征的容器
		* @param bIsOsgbAcitve osgbView是否是激活状态, 默认是激活的
		*/
		virtual void synHighLightData(const std::vector<osgCall::osgFtr> &vecHighFtr, bool bIsOsgbAcitve = true);

		/**
		* @brief clearHighLightData 清除高亮数据
		*/
		virtual void clearHighLightData();

		/**
		* @brief addConstDragLine 增加辅助标记线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief setConstDragLine 增加辅助标记线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void setConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief setVariantDragLine 设置临时的动态线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void setVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief addVariantDragLine 增加临时的动态线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief UpdatesnapDraw 设置捕捉时候的方框，用于表示该点被捕捉了
		* @param vecFtr 矢量特征的容器
		*/
		virtual void UpdatesnapDraw(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief addGraph 加入tin的dem构网,方面的解析
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addGraph(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief delGraph 删除graph
		*/
		virtual void delGraph();

		/**
		* @brief clearDragLine 清除动线
		*/
		virtual void clearDragLine();

		/**
		* @brief setCursor 二维移动时候设置光标
		* @brief x, y, z 光标的位置
		*/
		virtual void setCursor(double x, double y, double z);

		/**
		* @brief clearGroupResult 清除组结果,
		* @param group 需要清理的group
		*/
		void clearGroupResult(osg::ref_ptr<osg::Group> group) const;

		/**
		* @brief threadSafeClear 清除组结果,经过修改为线程安全
		* @param group 需要清理的group
		*/
		void threadSafeClear(osg::ref_ptr<osg::Group> group) const;

		/**
		* @brief linePickPt 通过平面x,y 向下做垂直碰撞检测
		* @param x y 平面坐标
		*/
		virtual std::vector<double> linePickPt(const double x, const double y) const;

		/**
		* @brief viewAll 显示所有map节点
		*/
		virtual void viewAll(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief ViewHideAll 隐藏所有map节点
		*/
		virtual void ViewHideAll(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief viewLocal 只显示本地
		* @param vecFtr 矢量特征的容器
		*/
		virtual void viewLocal(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief viewExternal 只显示外部的
		* @param vecFtr 矢量特征的容器
		*/
		virtual void viewExternal(const std::vector<osgCall::osgFtr> &vecFtr);

		/**
		* @brief getOsgbExtent 获取osgb范围， 返回的是引用
		* @param left 范围左边，返回引用
		* @param right 范围右边，返回引用
		* @param top 范围上面，返回引用
		* @param bottom 范围下面，返回引用
		*/
		virtual void getOsgbExtent(double &left, double &right, double &top, double &bottom);

		/**
		* @brief topView  三维视图切换到顶视角
		*/
		virtual void topView();

		/**
		* @brief leftView  三维视图切换到左视角
		*/
		virtual void leftView();

		/**
		* @brief rightView  三维视图切换到右视角
		*/
		virtual void rightView();

		/**
		* @brief islineBlock  判断线段是否与模型相交有遮挡
		* @brief x1 y1 z1  线段的起始点
		* @brief x2 y2 z2  线段的终点
		*/
		virtual bool islineBlock(double x1, double y1, double z1,
			double x2, double y2, double z2);

		/**
		* @brief drawVaryPt  绘制参考点,作为和影像互动的点
		* @brief x y z 三维点
		*/
		virtual bool drawVaryPt(double x, double y, double z);

		/**
		* @brief clearVaryGroup 清除前方交汇的辅助测点
		*/
		virtual void clearVaryGroup();

		/**
		* @brief startEvent 激活操作事件
		* @param order  事件的顺序
		*/
		virtual void startEvent(int order);

		/**
		* @brief absOri 绝对定向
		* @param oriInfo 绝对定向信息
		*/
		virtual std::string absOri(std::string oriInfo);

		/**
		* @brief driveToRelaPt 定位到相对点
		* @param relaX  相对点x
		* @param relaY  相对点y
		* @param relaZ  相对点z
		*/
		virtual void driveToRelaPt(double relaX, double relaY, double relaZ);

		/**
		* @brief fetchOriData 获取定向数据
		*/
		virtual void fetchOriData();

		/**
		* @brief showOriPt 显示绝对定向刺点结果
		* @param oriRes 刺点结果
		*/
		virtual void showOriPt(std::string oriRes);

		/**
		* @brief predictOriPt 预测绝对定向刺点
		* @param oriRes 刺点结果
		*/
		virtual std::string predictOriPt(std::string oriRes);

		/**
		* @brief clearOriPt 清除刺点结果
		*/
		virtual void clearOriPt();

		virtual void groundToScreen(double x, double y, double z, double & _x, double &_y);

		virtual bool screenToGround(double x, double y, double & _x, double &_y, double &_z);

	protected:
		/**
		* @brief InitManipulators 初始化操控器
		*/
		virtual void InitManipulators(void);

		/**
		* @brief InitSceneGraph 初始化场景
		*/
		virtual void InitSceneGraph(void);

		/**
		* @brief InitCameraConfig 初始化相机
		*/
		virtual void InitCameraConfig(void);

		/**
		* @brief setHomeCetner 设置回家的点, 把默认的改成45度角度往下看
		*/
		virtual void setHomeCetner();

		inline void Done(bool value) { mDone = value; }

		inline bool Done(void) { return mDone; }

		/**
		* @brief Render 进行渲染
		*/
		static void Render(void* ptr);

		/**
		* @brief initEvent 初始化事件
		*/
		virtual void initEvent();

	
	private:
		bool mDone;

		/**
		* @brief m_ModelName 模型路径名
		*/
		std::string m_ModelName;

		/**
		* @brief m_hWnd mfc传递下来的指针
		*/
		HWND m_hWnd;

		/**
		* @brief mViewer 浏览器指针
		*/
		osg::ref_ptr<CSelDefViewer> mViewer;
	
		/**
		* @brief terrain 模型的操控器，按地形的进行操控，比较适合浏览大型场景
		*/
		osg::ref_ptr<CSelfDefTerrainManipulator> terrain;

		/**
		* @brief keyswitchManipulator 可以切换操控器的控制器
		*/
		osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;

		/**
		* @brief curStateManipulator 当前可以切换状态的控制器
		*/
		CSelfStateManipulator* curStateManipulator;

		/**
		* @brief ic 回调接口
		*/
		std::shared_ptr<osgCall::ICallBack> icall;

		/**
		* @brief osgbModel osg模型
		*/
		COsgModel* osgbModel;

		/**
		* @brief syn 二三维同步器
		*/
		COsgSyn* syn;

		/**
		* @brief pEvent 事件接口
		*/
		std::shared_ptr<osgEvent::IOsgEvent> pEvent;
	};

	class CRenderingThread : public OpenThreads::Thread
	{
	public:
		CRenderingThread(cOSG* ptr);
		virtual ~CRenderingThread();

		virtual void run();

	protected:
		cOSG* _ptr;
		bool _done;
	};
}





