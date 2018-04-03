#pragma once

#include "RealtimeVectLayer.h"


class COsgRealtimeVecLayer : public CRealtimeVectLayer
{
public:
	COsgRealtimeVecLayer();
	virtual ~COsgRealtimeVecLayer();

	/**
	* @brief drawSymbol 绘制特征成为符号化的特征，并保存成grbuffer
	* @para  pFtr 绘制的特征
	*/
	GrBuffer* drawSymbol(CFeature *pFtr);
};

