#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideEditCmd 重写编辑节点命令，使得编辑命令
* @brief 切换不依赖于分辨率
*/
class COverrideEditVertCmd : public CDlgEditVertexCommand
{
public:
	/**
	* @brief subPtClick 把原来的ptClick代码抠出来, 并替换
	*/
	void subPtClick(PT_3D &pt, int flag);

	/**
	* @brief subHitTest 把原来的hitTest代码抠出来, 并替换
	*/
	BOOL subHitTest(PT_3D pt, int state)const;

	/**
	* @brief subPtMove 把原来的ptMove代码抠出来, 并替换
	*/
	void subPtMove(PT_3D &pt);

protected:
	/**
	* @brief subParentPtClick 替换基类的PtClick代码，不基于矢量视图的尺度
	*/
	void subParentPtClick(PT_3D &pt, int flag);
	
	/**
	* @brief subParentHitTest 替换基类的HitTest代码，不基于矢量视图的尺度
	*/
	BOOL subParentHitTest(PT_3D pt, int state)const;

	/**
	* @brief subParentPtMove 替换基类的PtMove代码，不基于矢量视图的尺度
	*/
	void subParentPtMove(PT_3D &pt);
};

