#pragma once
#include "DlgCommand2.h"
/**
* @brief COverridMatchVertexsCommand ��д�ڵ�����, ��������ˢ��osgbview
*/
class COverrideMatchVertCmd : public CMatchVertexsCommand
{
public:
	/**
	* @brief subPtClick ��������ˢ��osgbview
	*/
	void subPtClick(PT_3D &pt, int flag);
};

