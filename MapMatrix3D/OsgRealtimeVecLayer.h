#pragma once

#include "RealtimeVectLayer.h"


class COsgRealtimeVecLayer : public CRealtimeVectLayer
{
public:
	COsgRealtimeVecLayer();
	virtual ~COsgRealtimeVecLayer();

	/**
	* @brief drawSymbol ����������Ϊ���Ż����������������grbuffer
	* @para  pFtr ���Ƶ�����
	*/
	GrBuffer* drawSymbol(CFeature *pFtr);
};

