#pragma once
#include "CoordCenter.h"


/*
*	重写坐标转换，支持对两张影像进行前方交汇的方式
*	
*/
class COsgCoordCenter : public CCoordCenter
{
public:
	COsgCoordCenter();
	virtual ~COsgCoordCenter();

	/**
	* @brief Init 坐标系初始化	
	* @param core 工程的参数，空三成果等信息
	* @param lImg 进行前方交汇的第一张影像
	* @param rImg 进行前方交汇的第二张影像
	*/
	virtual bool InitByTwoImg(CoreObject& core, CString lImg, CString rImg);

protected:
	/**
	* @brief SwitchToTwoImg 转换到两张影像的模式
	* @param lImg 进行前方交汇的第一张影像
	* @param rImg 进行前方交汇的第二张影像
	*/
	virtual bool SwitchToTwoImg(CString lImg, CString rImg);

};

