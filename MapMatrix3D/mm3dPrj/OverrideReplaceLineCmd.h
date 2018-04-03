#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideReplaceLineCmd 重写曲线修测命令
*/
class COverrideReplaceLineCmd : public CReplaceLinesCommand
{
public:
	/**
	* @brief subPtClick 鼠标点击重写，不基于二维视图的缩放比
	*/
	void subPtClick(PT_3D &pt, int flag);

	/**
	* @brief subPtMove 鼠标移动重写，不基于二维视图的缩放比
	*/
	void subPtMove(PT_3D &pt);

	/**
	* @brief DrawPointTip 替换画点tip
	*/
	void subDrawPointTip(CGeometry *pObj, PT_3D pt, GrBuffer *buf);

};

