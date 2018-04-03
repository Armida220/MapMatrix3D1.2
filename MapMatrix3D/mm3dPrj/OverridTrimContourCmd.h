#pragma once
#include "DlgCommand.h"

/**
* @brief COverrideTrimContourCmd 重写的等高线编辑命令
*/
class COverridTrimContourCmd : public CTrimContourAcrossBankCommand
{
public:
	/**
	* @brief subPtClick 重写的等高线编辑命令, 通知osgbview刷新
	*/
	void subPtClick(PT_3D &pt, int flag);
};

