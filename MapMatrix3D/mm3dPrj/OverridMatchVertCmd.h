#pragma once
#include "DlgCommand2.h"
/**
* @brief COverridMatchVertexsCommand 重写节点吸附, 加入点击后刷新osgbview
*/
class COverrideMatchVertCmd : public CMatchVertexsCommand
{
public:
	/**
	* @brief subPtClick 加入点击后刷新osgbview
	*/
	void subPtClick(PT_3D &pt, int flag);
};

