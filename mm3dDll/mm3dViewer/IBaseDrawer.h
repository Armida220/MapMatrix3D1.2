#pragma once
#include <memory>
#include <osg/Matrix>
#include <osg/group>


/**
 * @brief IBaseDrawer 绘制模块接口
 */
namespace osgDraw
{
	class IBaseDrawer
	{
	public:
		/**
		 * @brief drawPt 绘制点
		 * @param worldPt 点坐标
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawPt(const osg::Vec3d &worldPt, const osg::Matrix &worldMatrix) = 0;

		/**
		* @brief drawHightPt 绘制高亮点
		* @param worldPt 点坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawHightPt(const osg::Vec3d &worldPt, const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawLine 绘制线
		 * @param worldPt1 起点坐标
		 * @param worldPt2 终点坐标
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawLine(const osg::Vec3d &worldPt1, const osg::Vec3d &worldPt2, 
			const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawSolid 绘制体
		 * @param worldCoord 体底部多边形坐标
		 * @param height 多边形高度
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawSolid(const std::vector<osg::Vec3d> &worldCoord, const double height, 
			const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawFlatPlane 绘制平面
		 * @param center 平面中心
		 * @param height 平面所在高度
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawFlatPlane(const osg::Vec3d &center, const double radius, 
			const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawLineLoop 绘制闭合线
		 * @param vecWorldCoord 闭合线坐标
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawLineLoop(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix) = 0;

		/**
		* @brief drawHighLineLoop 绘制高亮闭合线
		* @param vecWorldCoord 闭合线坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawHighLineLoop(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawCurLine 绘制曲线
		 * @param vecWorldCoord 闭合线坐标
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawCurLine(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix) = 0;

		/**
		* @brief drawHightCurLine 绘制高亮线
		* @param vecWorldCoord 曲线坐标
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawHightCurLine(std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawCircleLoop 绘制圆圈
		 * @param center 圆圈中心
		 * @param radius 圆圈半径
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawCircleLoop(osg::Vec3d &center, const double radius, const osg::Matrix &worldMatrix) = 0;

		/**
		 * @brief drawPolygon 绘制多边形
		 * @param vecWorldCoord 多边形坐标集合
		 * @param worldMatrix 模型到世界的矩阵
		 */
		virtual osg::Node* drawPolygon(const std::vector<osg::Vec3d> &vecWorldCoord, const osg::Matrix &worldMatrix) = 0;

		/**
		* @brief drawText 绘制文本
		* @param text 文本字段
		* @param position 位置
		* @param fontH 字体高度
		*/
		virtual osg::Node* drawText(const std::string &text, const osg::Vec3 &position, double fontH, int nAlign,
			const osg::Matrix &mat) = 0;

		/**
		* @brief drawArrow 绘制箭头
		* @param xyz 箭头的起始点
		* @param worldMatrix 模型到世界的矩阵
		*/
		virtual osg::Node* drawArrow(const osg::Vec3 &xyz, double tangent, const osg::Matrix &mat) = 0;

		/**
		 * @brief deleteVecChild 删除保存坐标
		 * @param childIndex 删除序号
		 * @param vecGroup 顶点集合
		 */
		virtual void deleteVecChild(const int childIndex, std::vector<osg::Vec3d> &vecGroup) = 0;

		/**
		 * @brief clearGroupChild 清除组
		 * @param group 清除的组
		 */
		virtual void clearGroupChild(osg::Group* group) = 0;

		///**
		// * @brief addDraggerToScene 向场景中增加拖拽器
		// * @param mRoot 场景节点
		// * @param tempGroup 临时组
		// * @param scopeGroup 范围组
		// */
		//virtual void addDraggerToScene(osg::Group* mRoot, osg::Group* tempGroup, osg::Group* scopeGroup) = 0;

		/**
		* @brief resetColor 重新设置颜色
		*/
		virtual void resetColor(osg::Vec4d clr) = 0;

		/**
		* @brief resetColor 重新设置颜色
		*/
		virtual void resetSize(double size) = 0;

		/**
		* @brief getColor 获取颜色
		*/
		virtual osg::Vec4d getColor() = 0;
	};

	/**
	 * @brief IBaseDrawerFactory 生产绘制模块接口的接口
	 */

	class IBaseDrawerFactory
	{
	public:
		/**
		 * @brief create 生产
		 * @param color 颜色
		 * @param size 制定绘制线或者点大小
		 * @param drawGroup 绘制组
		 * @param root 根节点
		 */
		static std::shared_ptr<IBaseDrawer> create(osg::Vec4d color, double size, osg::Group *drawGroup, osg::Group* root);
	};
}
