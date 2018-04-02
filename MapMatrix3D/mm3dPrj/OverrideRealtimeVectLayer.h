#pragma once
#include "RealtimeVectLayer.h"
#include <vector>

/**
* @brief COverrideRealtimeVectLayer 继承CRealtimeVectLayer,
* @brief 作为指针访问里面的tin数据
*/
class COverrideRealtimeVectLayer : public CRealtimeVectLayer
{
public:

	/**
	* @brief getRenderBufs 获得渲染前后的bufs
	* @brief 作为指针访问里面的tin数据
	*/
	std::vector<GrBuffer*> getRenderBufs();

};

