#pragma once
#include "editbasedoc.h"

// COsgbDoc document

/**
* @brief COsgbDoc 基于倾斜摄影测图，重写dlgdoc类
*/
class COsgbDoc : public CDlgDoc
{
public:
	COsgbDoc();
	virtual ~COsgbDoc();

	/**
	* @brief LButtonDown 重写LButtonDown, 主要是为了解决在osgbview 进行点击时候会依赖于vecview这边的分辨率
	*/
	void subLButtonDown(PT_3D &pt, int flag);

	/**
	* @brief workerMouseMv 当在osgbview移动时候，进行不基于分辨率的动线捕捉
	*/
	void subParentMouseMv(PT_3D &pt, int flag);

	/**
	* @brief MouseMove 当在osgbview移动时候，把worker的MouseMove替换成改写的，解决依赖分辨率的问题
	*/
	void subMouseMove(PT_3D &pt, int flag);

	/**
	* @brief TestStateCommand 重写解决编辑顶点和插入顶点，解决依赖分辨率的问题
	*/
	void TestStateCommand(PT_3D pt, int state);

public:

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnOpenOsgb();

	afx_msg void OnRemoveOsgb();


		
};
