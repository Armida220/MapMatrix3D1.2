#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideInsertVertCmd 重写插入节点命令，使得插入节点命令
* @brief 切换不依赖于矢量视图分辨率
*/
class COverrideInsertVertCmd : public CDlgInsertVertexCommand
{
public:
	/**
	* @brief subHitTest 重写点击切换事件方法，不依赖于
	* @brief 矢量视图分辨率
	*/
	BOOL subHitTest(PT_3D pt, int state)const;
};

