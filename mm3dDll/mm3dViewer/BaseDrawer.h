#pragma once
#include "StdAfx.h"
#include <osgManipulator/Command>
#include <osgManipulator/CommandManager>  
#include <osgManipulator/Translate2DDragger>
#include "IBaseDrawer.h"

/**
 * @brief CSceneDrawer 绘制模块
 */
namespace osgDraw
{
	class CSceneDrawer : public IBaseDrawer
	{
	public:
		CSceneDrawer(osg::Vec4d color, double size, osg::Group *drawGroup, osg::Group* root);

		/**
		* @brief drawPt 绘制点
		* @param worldPt 点坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawPt(const osg::Vec3d &worldPt, const osg::Matrix &worldMatrix);

		/**
		* @brief drawHightPt 绘制高亮点
		* @param worldPt 点坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawHightPt(const osg::Vec3d &worldPt, const osg::Matrix &worldMatrix);

		/**
		* @brief drawLine 绘制线
		* @param worldPt1 起点坐标
		* @param worldPt2 终点坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawLine(const osg::Vec3d &worldPt1, const osg::Vec3d &worldPt2, const osg::Matrix &worldMatrix);

		/**
		* @brief drawSolid 绘制体
		* @param worldCoord 体底部多边形坐标
		* @param height 多边形高度
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawSolid(const std::vector<osg::Vec3d> &worldCoord, const double height, const osg::Matrix &worldMatrix);

		/**
		* @brief drawFlatPlane 绘制平面
		* @param center 平面中心
		* @param height 平面所在高度
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawFlatPlane(const osg::Vec3d &center, const double radius, const osg::Matrix &worldMatrix);

		/**
		* @brief drawLineLoop 绘制闭合线
		* @param vecWorldCoord 闭合线坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawLineLoop(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix);

		/**
		* @brief drawHighLineLoop 绘制高亮闭合线
		* @param vecWorldCoord 闭合线坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawHighLineLoop(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix);

		/**
		* @brief drawCurLine 绘制曲线
		* @param vecWorldCoord 曲线坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawCurLine(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix);

		/**
		* @brief drawHightCurLine 绘制高亮线
		* @param vecWorldCoord 曲线坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawHightCurLine(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix);

		/**
		* @brief drawCircleLoop 绘制圆圈
		* @param center 圆圈中心
		* @param radius 圆圈半径
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawCircleLoop(osg::Vec3d &center, const double radius, const osg::Matrix &worldMatrix);

		/**
		* @brief drawPolygon 绘制多边形
		* @param vecWorldCoord 多边形坐标集合
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawPolygon(const std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix);

		/**
		* @brief drawArrow 绘制箭头
		* @param xyz 箭头的起始点
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawArrow(const osg::Vec3 &xyz, double tangent, const osg::Matrix &mat);

		/**
		* @brief drawText 绘制文本
		* @param text 文本字段
		* @param position 位置
		* @param fontH 字体高度
		* @param mat 矩阵
		*/
		virtual osg::Node* drawText(const std::string &text, const osg::Vec3 &position, double fontH, int nAlign,const osg::Matrix &mat);

		/**
		* @brief deleteVecChild 删除保存坐标
		* @param childIndex 删除序号
		* @param vecGroup 顶点集合
		*/
		virtual void deleteVecChild(const int childIndex, std::vector<osg::Vec3d> &vecGroup);
		/**
		* @brief clearGroupChild 清除组
		* @param group 清除的组
		*/
		virtual void clearGroupChild(osg::Group* group);

		/**
		* @brief addDraggerToScene 向场景中增加拖拽器
		* @param mRoot 场景节点
		* @param tempGroup 临时组
		* @param scopeGroup 范围组
		*/
		virtual void addDraggerToScene(osg::Group* mRoot, osg::Group* tempGroup, osg::Group* scopeGroup);

		/**
		* @brief resetColor 重新设置颜色
		*/
		virtual void resetColor(osg::Vec4d clr);

		/**
		* @brief resetColor 重新设置颜色
		*/
		virtual void resetSize(double size);

		/**
		* @brief getColor 获取颜色
		*/
		virtual osg::Vec4d getColor();

	protected:
		/**
		* @brief drawBasePoint 绘制点
		* @param pt 点
		* @return 点
		*/
		osg::Geode* drawBasePoint(osg::Vec3d pt);

		/**
		* @brief drawBaseLine 绘制线
		* @param pt1 点1
		* @param pt2 点2
		* @return 线
		*/
		osg::Geode* drawBaseLine(osg::Vec3d pt1, osg::Vec3d pt2);

		/**
		* @brief drawBaseLineLoop 绘制闭合线
		* @param coord 闭合线坐标
		* @return 闭合线
		*/
		osg::Geode* drawBaseLineLoop(std::vector<osg::Vec3d> coord);

		/**
		* @brief drawBaseCurLine 绘制曲线
		* @param coord 曲线坐标
		* @return 曲线
		*/
		osg::Geode* drawBaseCurLine(std::vector<osg::Vec3d> coord);

		/**
		* @brief drawBasePolygon 绘制多边形
		* @param coord 曲线坐标
		* @return 多边形
		*/
		osg::Geode* drawBasePolygon(std::vector<osg::Vec3d> coord);

		osg::Geode* drawBasePolygonNT(std::vector<osg::Vec3d> coord);

		/**
		* @brief drawBaseSolid 绘制体
		* @param coord 底多边形坐标
		* @param height 高度
		* @return 体
		*/
		osg::Group* drawBaseSolid(std::vector<osg::Vec3d> coord, double height);

		/**
		* @brief drawBaseFlatPlane 绘制平面
		* @param center 中心
		* @param radius 半径
		* @return 平面
		*/
		osg::Geode* drawBaseFlatPlane(osg::Vec3d center, double radius);

		/**
		* @brief drawBaseCircleLoop 绘制圆形
		* @param center 中心
		* @param radius 半径
		* @return 圆形
		*/
		osg::Geode* drawBaseCircleLoop(osg::Vec3d center, double radius);

		/**
		* @brief drawBaseArrow 绘制箭头
		* @param xyz 起始点坐标
		* @param tangent 切线方向
		* @return 箭头几何形状
		*/
		osg::Group* drawBaseArrow(const osg::Vec3 &xyz, double tangent, const osg::Matrix &mat);

		/**
		* @brief addNodeChildToGroup 增加节点到组中，使得绘制图形可以被渲染出来
		* @param nodeChild 子节点
		* @param parent 父节点
		* @param matrix 局部到世界的矩阵
		*/
		osg::Node* addNodeChildToGroup(osg::Node* nodeChild, osg::Group* parent, osg::Matrix matrix);

		/**
		* @brief threadSafeAddNode 线程安全增加节点到组中，使得绘制图形可以被渲染出来
		* @param nodeChild 子节点
		* @param parent 父节点
		* @param matrix 局部到世界的矩阵
		*/
		osg::Node* threadSafeAddNode(osg::Node* nodeChild, osg::Group* parent, osg::Matrix matrix);

		/**
		* @brief makePtState 制作点的状态集，作为渲染的依据，可以设置绘制点的大小
		* @param size 大小
		* @return 点状态集
		*/
		osg::StateSet* makePtState(int size);

		/**
		* @brief makeLineState 制作线的状态集，作为渲染的依据
		* @return 线状态集
		*/
		osg::StateSet* makeLineState();

		osg::StateSet* makeLineStateNT();

		///**
		//* @brief createDragger 创建拖拽器
		//* @param group 增加拖拽器的组
		//* @return 拖拽器
		//*/
		//osgManipulator::Dragger* createDragger(osg::Group* group);

		/**
		* @brief highLightGeode 高亮几何节点
		* @param geode 几何节点
		*/
		void highLightGeode(osg::Geode *geode);

		/**
		* @brief unicodeToUTF8 unicode转换成utf8
		* @param src 输入字符串
		* @param result 输出字符串
		*/
		void unicodeToUTF8(const std::wstring &src, std::string& result);

		/**
		* @brief gb2312ToUnicode gb2312转换成unicode
		* @param src 输入字符串
		* @param result 输出字符串
		*/
		void gb2312ToUnicode(const std::string& src, std::wstring& result);

		/**
		* @brief gb2312ToUnicode gb2312转换成utf8
		* @param src 输入字符串
		* @param result 输出字符串
		*/
		void gb2312ToUtf8(const std::string& src, std::string& result);

		/**
		* @brief AnsiToUnicode Ansi转换成unicode
		* @param szStr 输入字符串
		*/
		wchar_t* AnsiToUnicode(const char* szStr);

	private:
		/**
		* @brief mClr 绘制的颜色
		*/
		osg::Vec4d mClr;

		/**
		* @brief mSize 绘制的物体的大小，可以表示线和点的粗线
		*/
		double mSize;

		/**
		* @brief mLineSize 绘制的线型物体的大小，可以表示线和点的粗线
		*/
		double mLineSize;

		/**
		* @brief mDrawGroup 绘制的租，把节点加入才可以被渲染
		*/
		osg::ref_ptr<osg::Group> mDrawGroup;

		/**
		* @brief mRoot 根节点
		*/
		osg::ref_ptr<osg::Group> mRoot;
	};

}
