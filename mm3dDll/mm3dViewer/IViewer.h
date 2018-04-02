#pragma once
#include <string>
#include <memory>
#include <vector>
#include "ICallBack.h"
#include <deque>
#include <windows.h>

#ifdef MFCVIEWERIBDLL  
#define MFCVIEWERAPI _declspec(dllexport)  
#else  
#define MFCVIEWERAPI  _declspec(dllimport)  
#endif  


namespace osgDraw
{
	class IBaseDrawer;
}

namespace osgEvent
{
	class CBaseUserEvent;
}

namespace osgCall
{
	class ICallBack;
}

namespace mm3dView
{

	/**
	* @brief IOsgMfcViewer 在mfc框架中进行三维渲染的浏览器接口
	*/

	class MFCVIEWERAPI IOsgMfcViewer
	{
	public:
		/**
		* @brief getScale 根据距离获得尺度
		*/
		virtual double getScale() = 0;

		/**
		* @brief getScaleFromDist 根据距离获得尺度,适用于绘制半径
		*/
		virtual double getScaleFromDist() = 0;

		/**
		* @brief getScrren2WorldMat 获得osg从屏幕到世界的矩阵
		*/
		virtual double* getScrren2WorldMat() = 0;

		/**
		* @brief InitOSG 初始化osg浏览器
		* @param modelname 初始化的场景名字
		*/
		virtual void InitOSG(std::string modelname) = 0;

		/**
		* @brief StartRendering 开始进行渲染，启动渲染线程
		*/
		virtual HANDLE StartRendering() = 0;

		/**
		* @brief addScene  添加文件到场景中
		* @param strModel  模型路径名
		*/
		virtual void addScene(const std::string &strModel) = 0;

		/**
		* @brief removeAllScene  清除场景中所有数据
		*/
		virtual void removeAllScene() = 0;

		/**
		* @brief SetSceneExtent 设置场景范围
		*/
		virtual void SetSceneExtent() = 0;

		/**
		* @brief home 回家
		*/
		virtual void home() = 0;

		/**
		* @brief light 启动光源
		*/
		virtual void light() = 0;

		/**
		* @brief setCall 设置回调
		* @param ic  回调
		*/
		virtual void setCall(const std::shared_ptr<osgCall::ICallBack> &ic) = 0;

		/**
		* @brief isLinePick 通过垂直碰撞检测，是否能够获得点
		* @brief x y 平面二维点进行垂直碰撞检测
		*/
		virtual bool isLinePick(const double x, const double y) = 0;

		/**
		* @brief addObj 对三维的数据进行增加数据
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addObj(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief removeObj 对三维的数据进行清除数据
		* @param vecFtr 矢量特征的容器
		*/
		virtual void removeObj(const std::vector<osgCall::osgFtr> &vecFtr) = 0;
		virtual void removeObj(const std::vector<std::string> &vecID) = 0;

		/**
		* @brief driveTo 移动到中心
		* @brief x y 平面二维点移动
		*/
		virtual void driveTo(const double x, const double y) = 0;

		/**
		* @brief panTo 平移到点（x,y),过程中相机角度不变
		* @brief x y 平面二维点移动
		*/
		virtual void panTo(const double x, const double y) = 0;

		/**
		* @brief zoomTo 缩放到来自二维的同一尺度,过程中相机角度不变
		* @brief scale 来自二维的同一尺度
		*/
		virtual void zoomTo(const double scale) = 0;

		/**
		* @brief synData 对三维的数据和vecview进行同步
		* @param vecFtr 矢量特征的容器
		*/
		virtual void synData(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief updataDate 对三维的数据和vecview进行更新
		* @param vecFtr 矢量特征的容器
		*/
		virtual void updataDate(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief synHighLightData 对三维的数据和vecview进行高亮同步
		* @param vecHighFtr 矢量特征的容器
		*/
		virtual void synHighLightData(const std::vector<osgCall::osgFtr> &vecHighFtr, bool bIsOsgbAcitve = true) = 0;

		/**
		* @brief clearHighLightData 清除高亮数据
		*/
		virtual void clearHighLightData() = 0;

		/**
		* @brief addConstDragLine 增加辅助标记线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief setConstDragLine 设置辅助标记线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void setConstDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief setVariantDragLine 设置临时的动态线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void setVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief addVariantDragLine 增加临时的动态线
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addVariantDragLine(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief UpdatesnapDraw 设置捕捉时候的方框，用于表示该点被捕捉了
		* @param vecFtr 矢量特征的容器
		*/
		virtual void UpdatesnapDraw(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief addGraph 加入tin的dem构网,方面的解析
		* @param vecFtr 矢量特征的容器
		*/
		virtual void addGraph(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief delGraph 删除graph
		*/
		virtual void delGraph() = 0;

		/**
		* @brief clearDragLine 清除动线
		*/
		virtual void clearDragLine() = 0;

		/**
		* @brief linePickPt 清除组结果,经过修改为线程安全
		* @param 平面xy 坐标
		*/
		virtual std::vector<double> linePickPt(const double x, const double y) const = 0;

		/**
		* @brief viewAll 显示所有map节点
		*/
		virtual void viewAll(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief ViewHideAll 隐藏所有map节点
		*/
		virtual void ViewHideAll(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief viewLocal 只显示本地
		* @param vecFtr 矢量特征的容器
		*/
		virtual void viewLocal(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief viewExternal 只显示外部的
		* @param vecFtr 矢量特征的容器
		*/
		virtual void viewExternal(const std::vector<osgCall::osgFtr> &vecFtr) = 0;

		/**
		* @brief getOsgbExtent 获取osgb范围， 返回的是引用
		* @param left 范围左边，返回引用
		* @param right 范围右边，返回引用
		* @param top 范围上面，返回引用
		* @param bottom 范围下面，返回引用
		*/
		virtual void getOsgbExtent(double &left, double &right, double &top, double &bottom) = 0;

		/**
		* @brief topView  三维视图切换到顶视角
		*/
		virtual void topView() = 0;

		/**
		* @brief leftView  三维视图切换到左视角
		*/
		virtual void leftView() = 0;

		/**
		* @brief leftView  三维视图切换到右视角
		*/
		virtual void rightView() = 0;

		/**
		* @brief islineBlock  判断线段是否与模型相交有遮挡
		* @brief x1 y1 z1  线段的起始点
		* @brief x2 y2 z2  线段的终点
		*/
		virtual bool islineBlock(double x1, double y1, double z1,
			double x2, double y2, double z2) = 0;

		/**
		* @brief drawVaryPt  绘制参考点,作为和影像互动的点
		* @brief x y z 三维点
		*/
		virtual bool drawVaryPt(double x, double y, double z) = 0;

		/**
		* @brief clearVaryGroup 清除前方交汇的辅助测点
		*/
		virtual void clearVaryGroup() = 0;

		/**
		* @brief startEvent 激活操作事件
		* @param order  事件的顺序
		*/
		virtual void startEvent(int order) = 0;

		/**
		* @brief absOri 绝对定向
		* @param oriInfo 绝对定向信息
		*/
		virtual std::string absOri(std::string oriInfo) = 0;

		/**
		* @brief driveToRelaPt 定位到相对点
		* @param relaX  相对点x
		* @param relaY  相对点y
		* @param relaZ  相对点z
		*/
		virtual void driveToRelaPt(double relaX, double relaY, double relaZ) = 0;

		/**
		* @brief showOriPt 显示绝对定向刺点结果
		* @param oriRes 刺点结果 
		*/
		virtual void showOriPt(std::string oriRes) = 0;

		/**
		* @brief predictOriPt 预测绝对定向刺点
		* @param oriRes 刺点结果
		*/
		virtual std::string predictOriPt(std::string oriRes) = 0;

		/**
		* @brief fetchOriData 获取定向数据
		*/
		virtual void fetchOriData() = 0;

		/**
		* @brief clearOriPt 清除刺点结果
		*/
		virtual void clearOriPt() = 0;
		
		virtual void groundToScreen(double x, double y, double z, double & _x, double &_y) = 0;
		
		virtual bool screenToGround(double x, double y, double & _x, double &_y, double &_z) = 0;

		virtual bool shiftOperation() = 0;

		virtual bool shiftCatchCorner() = 0;

		virtual bool lockHeight(int x, int y) = 0;

		virtual bool updataFtrView(bool bShow) = 0;

	};

	/**
	* @brief IOsgMfcViewerFactory 生产三维渲染的浏览器接口的工厂
	*/

	class MFCVIEWERAPI IOsgMfcViewerFactory
	{
	public:
		/**
		* @brief create 生产接口
		* @param hWnd   句柄
		*/
		static std::shared_ptr<IOsgMfcViewer> create(HWND hWnd);
	};

}

