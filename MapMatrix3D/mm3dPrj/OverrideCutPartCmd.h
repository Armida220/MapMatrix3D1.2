#pragma once
#include "DlgCommand.h"


/**
* @brief COverrideCutPartCmd 重写修剪命令,在完成了修剪后
* @brief 更新osgbview
*/
class COverrideCutPartCmd : public CCutPartCommand
{
public:
	/**
	* @brief subPtClick 用于替换原来的修剪命令的ptClick
	* @brief 完成命令后向osgbview发送更新消息
	*/
	void subPtClick(PT_3D &pt, int flag);
};

