#pragma once
#include "RealtimeVectLayer.h"
#include <vector>

/**
* @brief COverrideRealtimeVectLayer �̳�CRealtimeVectLayer,
* @brief ��Ϊָ����������tin����
*/
class COverrideRealtimeVectLayer : public CRealtimeVectLayer
{
public:

	/**
	* @brief getRenderBufs �����Ⱦǰ���bufs
	* @brief ��Ϊָ����������tin����
	*/
	std::vector<GrBuffer*> getRenderBufs();

};

